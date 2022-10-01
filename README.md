## About

vsmkv is a FUSE-based virtual filesystem for exporting VapourSynth scripts as uncompressed videos in the Matroska (MKV) file format.

Note: this is a proof of concept and will likely not work properly (for example might mess up timing)

## Dependencies

FUSE, CMake and a compiler supporting the C++14 shared mutex (e.g., GCC >= 5.2) are required.

Running the unit tests requires the [Criterion](https://github.com/Snaipe/Criterion) framework to be installed.

## Installation

from the commandline:

```commandline
mkdir build
cd build
cmake ../
make
```

## Running

The command requires two arguments. First, the path to your VapourSynth script. Second, an empty directory for mounting the filesystem. Additionally, a number of parameters inherited from FUSE may be specified (e.g., `-s` for running in single threaded mode or `-f` for running in foreground mode instead of daemonizing):

```commandline
cd /tmp
mkdir output
/path/to/vsmkv test.vpy output -f
```

Finally, a player supporting uncompressed video in MKV files can be used to play back the videos (first output node in this example):

```commandline
cd /tmp/output
mpv node_0
```

## Limitations

- Currently, only planar YUV color spaces are supported.
- Concurrent access to the same video from multiple programs (or threads of the same program) results in constant allocation and de-allocation of frames with a huge performance penalty.
- The software lacks proper logging and error handling for now - errors result in uncaught exceptions.
- The filesystem needs to be remounted after changing the VapourSynth script file
- No Windows support (yet) due to the missing native FUSE implementation. [Crossmeta FUSE](https://github.com/crossmeta/cxfuse) may possibly work.
