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

#ifndef VSMKV_CLUSTERLIST_H
#define VSMKV_CLUSTERLIST_H

#include <vapoursynth/VapourSynth.h>
#include "node.h"

class caching_cluster : public node {
public:
    caching_cluster(const VSAPI* api, VSNodeRef* node, const int16_t frame_dur, const uint64_t frame_size,
                    const int blocks_per_cluster, const int num_cluster, const int tail_blocks);
    ~caching_cluster();

    void cache_cluster(const uint64_t cluster_number);

    size_t output(char *buffer, size_t _size, size_t offset) const override;

    virtual node_ptr addChild(node_ptr child);
    virtual void report(size_t offset, uint8_t indent) const;

private:
    int cached_cluster_number = -1;
    node_ptr cached_cluster;

    const VSAPI* vsapi;
    VSNodeRef* node;

    const int16_t frame_duration;
    const uint64_t frame_size;
    const int blocks_per_cluster;
    const int num_clusters;
    const int tail_blocks;
};

// TODO: switch to shared_ptr
//typedef std::shared_ptr<caching_cluster> caching_cluster_ptr;
typedef caching_cluster* caching_cluster_ptr;

class clusterlist : public node {
public:
    clusterlist(const VSAPI* api, VSNodeRef* node);
    clusterlist(const VSAPI* api, VSNodeRef* node, int blocks_per_cluster);

    virtual ~clusterlist();

    size_t getSize() const override;
    size_t output(char *buffer, size_t _size, size_t offset) const override;

    virtual node_ptr addChild(node_ptr child);
    virtual void report(size_t offset, uint8_t indent) const;

    uint64_t getClustersize() const;
    uint64_t getClusterduration() const;
    int getNumClusters() const;
private:
    size_t block_size;
    size_t cluster_size;
    size_t total_size;
    size_t tail_cluster_size;
    size_t cluster_duration;

    int blocks_per_cluster;
    int num_frames;
    int num_clusters;

    VSNodeRef* node;
    const VSAPI* vsapi;

    caching_cluster_ptr cached_cluster;
};

inline node_ptr ClusterList(const VSAPI* api, VSNodeRef* node) { return node_ptr(new clusterlist(api, node)); }

#endif //VSMKV_CLUSTERLIST_H
