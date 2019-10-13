/*

 Copyright (c) 2017 Peter Hegen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

 */

#include <fuse/fuse.h>
#include <fuse/fuse_lowlevel.h>
#include <vector>
#include <sstream>
#include <iostream>

#include "vapoursynth/VSScript.h"
#include "vapoursynth/VSHelper.h"

#include "vsmkv/stream.h"

struct options {
    char* script;
    std::vector<std::unique_ptr<stream>> streams;
};

static struct options options;
static struct fuse_operations oper;

static int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
{
    (void) offset;
    (void) fi;

    if(strncmp(path, "/", 1) != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for(int i=0; i<options.streams.size(); i++) {
        std::stringstream fmt;
        fmt << "node_" << i;
        filler(buf, fmt.str().c_str(), NULL, 0);
    }

    return 0;
}

static int getattr(const char* path, struct stat* stbuf)
{
    int ret = 0;

    memset(stbuf, 0, sizeof(struct stat));

    std::string p = std::string(path);
    int node_number = -1;

    try {
        if(strcmp(path, "/") == 0) {
            stbuf->st_mode = S_IFDIR | 0755;
        } else if(p.compare(0, 6, std::string("/node_")) == 0 && (node_number = std::stoi(p.substr(6))) < options.streams.size() && node_number >= 0) {
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = options.streams[node_number]->getSize();
        } else {
            ret = -ENOENT;
        }
    } catch (const std::invalid_argument& ia) {
        std::cout << "received invalid argument while trying to parse \"" << path << "\"" << std::endl;
    }

    return ret;
}

static int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;

    std::string p = std::string(path);
    int node_number = -1;

    if(p.compare(0, 6, std::string("/node_")) != 0 || (node_number = std::stoi(p.substr(6))) >= options.streams.size() || node_number < 0) {
        return -ENOENT;
    }

    return (int)options.streams[node_number]->read(buf, size, offset);
}

static int opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs) {
    (void) data;
    (void) outargs;

    switch(key) {
        case FUSE_OPT_KEY_NONOPT:
            if (!options.script) {
                options.script = strdup(arg);
                return 0;
            }
            return 1;
        default:
            break;
    }

    return 1;
}

// workaround for older fuse versions
#ifndef fuse_cmdline_opts
struct fuse_cmdline_opts {
    int singlethread;
	int foreground;
	int debug;
	int nodefault_subtype;
	char *mountpoint;
	int show_version;
	int show_help;
    int clone_fd;
};

int fuse_parse_cmdline(struct fuse_args *args, struct fuse_cmdline_opts *opts) {
    int ret = 0;
    int multithreaded = 0;

    ret = fuse_parse_cmdline(args, &opts->mountpoint, &multithreaded, &opts->foreground);
    opts->singlethread = !multithreaded;

    return ret;
}

int fuse_loop_mt(struct fuse* f, int clone_fd) {
    (void) clone_fd;
    return fuse_loop_mt(f);
}
#endif

int main(int argc, char **argv) {
    const VSAPI *vsapi = NULL;
    VSScript *se = NULL;

    struct fuse_args f_args = FUSE_ARGS_INIT(argc, argv);
    struct fuse_cmdline_opts f_opts;
    struct fuse* fuse;

    int ret;

    {
        oper.getattr = getattr;
        oper.read = read;
        oper.readdir = readdir;
    }

    // init vapoursynth
    if (!vsscript_init()) {
        std::cerr << "failed to init vapoursynth script" << std::endl;
        return 1;
    }

    vsapi = vsscript_getVSApi();
    assert(vsapi);

    if (vsscript_evaluateFile(&se, argv[1], 0)) {
        std::cerr << "failed to load vapoursynth script file \"" << argv[1] << "\": " << vsscript_getError(se) << std::endl;

        vsscript_freeScript(se);
        vsscript_finalize();
        return 1;
    }

    int index = 0;
    while(auto node = vsscript_getOutput(se, index++)){
        if(node == NULL) {
            break;
        }

        try {
            options.streams.push_back(std::make_unique<stream>(vsapi, node, index-1));
        } catch(std::string error) {
            std::cerr << error << std::endl;
            return 1;
        }
    }

    // mount filesystem and deal with the weird old FUSE API
    if(fuse_opt_parse(&f_args, &options, NULL, &opt_proc ) == -1) {
        std::cerr << "failed to parse options" << std::endl;
#ifdef fuse_cmdline_opts
        fuse_cmdline_help();
#endif
        return 1;
    }

    if(fuse_parse_cmdline(&f_args, &f_opts) == -1) {
        std::cerr << "failed to parse commandline arguments" << std::endl;
#ifdef fuse_cmdline_opts
        fuse_cmdline_help();
#endif
        return 1;
    }

#ifndef fuse_cmdline_opts
    struct fuse_chan* ch = fuse_mount(f_opts.mountpoint, &f_args);
    if(!ch) {
        std::cerr << "failed to mount filesystem" << std::endl;
        return 1;
    }

    fuse = fuse_new(ch, &f_args, &oper, sizeof(struct fuse_operations), NULL);

    if(fuse == NULL) {
        std::cerr << "failed to create fuse_struct" << std::endl;
        fuse_unmount(f_opts.mountpoint, ch);
        return 1;
    }

#else
    fuse = fuse_new (&f_args, &oper, sizeof(struct fuse_operations), NULL);

    if(fuse == NULL){
        // fuse_new already printed error
        return 1;
    }

    if(fuse_mount (fuse, f_opts.mountpoint) == -1) {
        std::cerr << "failed to mount filesystem" << std::endl;
        return 1;
    }

#endif

    if(fuse_daemonize(f_opts.foreground) == -1) {
        std::cerr << "failed to daemonize" << std::endl;
#ifndef fuse_cmdline_opts
        fuse_unmount(f_opts.mountpoint, ch);
#else
        fuse_unmount(fuse);
#endif
        return 1;
    }

    if(f_opts.singlethread) {
        ret = fuse_loop (fuse);
    } else {
        ret = fuse_loop_mt (fuse, f_opts.clone_fd);
    }

#ifndef fuse_cmdline_opts
    fuse_unmount(f_opts.mountpoint, ch);
#else
    fuse_unmount(fuse);
#endif
    fuse_destroy(fuse);

    free(f_opts.mountpoint);
    fuse_opt_free_args(&f_args);

    // cleanup vapoursynth
    vsscript_freeScript(se);
    vsscript_finalize();

    return ret == -1 ? 1 : 0;
}
