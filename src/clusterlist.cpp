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

#include <vapoursynth/VSScript.h>
#include <iostream>
#include <assert.h>
#include <mutex>
#include <shared_mutex>

#include "../vsmkv/clusterlist.h"
#include "../vsmkv/elements.h"
#include "../vsmkv/block.h"

std::shared_timed_mutex cluster_lock;

clusterlist::clusterlist(const VSAPI* api, VSNodeRef* node) : clusterlist(api, node, 10) {}

clusterlist::clusterlist(const VSAPI* api, VSNodeRef *node, int blocks_per_cluster) :
    blocks_per_cluster(blocks_per_cluster), node(node), vsapi(api)
{
    char error_message[1024];

    const VSVideoInfo* vi = vsapi->getVideoInfo(node);
    const VSFrameRef* null_frame = vsapi->getFrame(0, node, error_message, sizeof(error_message));

    uint64_t frame_size = 0;

    for(int p=0; p < vi->format->numPlanes; p++) {
        frame_size += vsapi->getFrameWidth(null_frame, p) *
                      vsapi->getFrameHeight(null_frame, p);
    }
    frame_size *= vsapi->getFrameFormat(null_frame)->bytesPerSample;

    vsapi->freeFrame(null_frame);

    num_frames = vi->numFrames;

    int remaining_blocks = num_frames % blocks_per_cluster;
    num_clusters = (num_frames - remaining_blocks) / blocks_per_cluster;

    // EBML ID + size + (BlockHeader + BlockData)
    block_size = 1 + vint(4 + frame_size).getSize() + (4 + frame_size);
    // EBML ID + size + ClusterTimeCode + Blocks
    cluster_size = 4 + vint(6 + block_size * blocks_per_cluster).getSize() + (6 + block_size * blocks_per_cluster);
    // left-over frames at the end
    tail_cluster_size = 4 + vint(6 + block_size * remaining_blocks).getSize() + (6 + block_size * remaining_blocks);

    total_size = cluster_size * num_clusters;
    total_size += tail_cluster_size;

    auto fps = ((double) vi->fpsNum)/vi->fpsDen;
    auto frame_duration = 1000.0 / fps;

    cluster_duration = (size_t)((1000.0 / fps) * blocks_per_cluster);

    cached_cluster = std::make_shared<caching_cluster>(vsapi, node, frame_duration, frame_size, blocks_per_cluster,
                                                       num_clusters, remaining_blocks);
}

size_t clusterlist::getSize() const {
    return total_size;
}

size_t clusterlist::output(char *buffer, size_t _size, size_t offset) const {
    size_t cluster_offset = offset % cluster_size;
    uint64_t cluster_number = (offset - cluster_offset) / cluster_size;

    cached_cluster->cache_cluster(cluster_number);
    size_t written = cached_cluster->output(buffer, _size, cluster_offset);

    if(written < _size) {
        assert(cluster_size - written - cluster_offset == 0);
        cached_cluster->cache_cluster(cluster_number+1);
        written += cached_cluster->output(buffer + written, _size - written, 0);
    }

    return written;
}

node* clusterlist::addChild(const node_ptr& child) {
    (void) child;
    return this;
}

void clusterlist::report(size_t offset, uint8_t indent) const {
    size_t cluster_offset = offset % cluster_size;
    uint64_t cluster_number = (offset - cluster_offset) / cluster_size;

    cached_cluster->cache_cluster(cluster_number);
    return cached_cluster->report(offset, indent);
}

uint64_t clusterlist::getClustersize() const {
    return cluster_size;
}

uint64_t clusterlist::getClusterduration() const {
    return cluster_duration;
}
int clusterlist::getNumClusters() const {
    return num_clusters;
}

caching_cluster::caching_cluster(const VSAPI* api, VSNodeRef* node, const double frame_dur, const uint64_t frame_size,
                                 const int blocks_per_cluster, const int num_cluster, const int tail_blocks) :
        frame_duration(frame_dur), frame_size(frame_size), blocks_per_cluster(blocks_per_cluster),
        num_clusters(num_cluster), tail_blocks(tail_blocks), node(node), vsapi(api)
{
}

node* caching_cluster::addChild(const node_ptr& child) {
    (void) child;
    return this;
}

void caching_cluster::report(size_t offset, uint8_t indent) const {
    if(cached_cluster_number >= 0) {
        cached_cluster->report(offset, indent);
    }
}

void caching_cluster::cache_cluster(const uint64_t cluster_number) {
    // implement caching of the last cluster and block(s) used
    if(cluster_number != cached_cluster_number) {
        auto cluster = Cluster();

        cluster->addChild(ClusterTimecode(static_cast<uint64_t>(cluster_number * frame_duration * blocks_per_cluster)));

        char error_message[128];

        int num_blocks = blocks_per_cluster;

        if(cluster_number == num_clusters) {
            // tail cluster
            num_blocks = tail_blocks;
        } else if (cluster_number > num_clusters) {
            // TODO: error handling
            throw std::string("encountered unexpected cluster_number") + std::to_string(cluster_number);
        }

        for(int i=0; i<num_blocks; i++) {
            const int frame_number = (int)(cluster_number * blocks_per_cluster + i);
            const VSFrameRef* frame = vsapi->getFrame(frame_number, node,
                                                      error_message, sizeof(error_message));

            if(!frame) {
                // TODO: error handling
                throw std::string("failed to retrieve frame number ") + std::to_string(frame_number) +
                        std::string(". received error message: \"") + std::string(error_message) + std::string("\".");
            }

            auto block = SimpleBlock(vsapi, node, frame, frame_size, (int16_t)(i * frame_duration) /*timecode*/);
            cluster->addChild(block);
        }

        if(cluster_lock.try_lock_for(std::chrono::seconds(60))) {
            cached_cluster_number = (int) cluster_number;
            cached_cluster = cluster;

            cluster_lock.unlock();
        } else {
            // TODO: error handling
            throw std::string("failed to lock caching_cluster for 60s!");
        }
    }
}

size_t caching_cluster::output(char *buffer, size_t _size, size_t offset) const {
    if(cached_cluster_number < 0) {
        // TODO: error handling
        throw std::string("cached cluster wasn't initialized");
    }

    cluster_lock.lock_shared();
    size_t written = cached_cluster->output(buffer, _size, offset);
    cluster_lock.unlock_shared();

    return written;
}
