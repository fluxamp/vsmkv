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

#ifndef VSMKV_BLOCK_H
#define VSMKV_BLOCK_H

#include <vapoursynth/VapourSynth.h>
#include "element.h"

typedef struct _header {
    _header(uint8_t track, int16_t time, uint8_t f) : track_number((uint8_t)0x80|track), timecode(time), flags(f) {};

    uint8_t track_number = 0x81;
    int16_t timecode;
    uint8_t flags;
} block_header;

class block : public element{
public:
    virtual ~block();
    block(const std::string name, const vint id, const VSAPI* api, VSNodeRef* node, const VSFrameRef* frame,
          const uint64_t frame_size, const int16_t timecode);

    size_t getSize() const override;
    size_t output(char *buffer, size_t _size, size_t offset) const override;

    virtual node* addChild(node_ptr child);
    virtual void report(size_t offset, uint8_t indent) const;

private:
    const VSFrameRef* frame;
    const block_header head;

    const uint64_t total_size;
    const uint64_t frame_size;

    const VSAPI* vsapi;
    const VSVideoInfo* vi;
    VSNodeRef* node;
};

inline node_ptr Block(const VSAPI* api, VSNodeRef* node, const VSFrameRef* frame, const uint64_t size, const int16_t timecode=0) {
    return node_ptr(new block("Block", vint(0x21), api, node, frame, size, timecode));
}
inline node_ptr SimpleBlock(const VSAPI* api, VSNodeRef* node, const VSFrameRef* frame, const uint64_t size, const int16_t timecode=0) {
    return node_ptr(new block("SimpleBlock", vint(0x23), api, node, frame, size, timecode));
}

#endif //VSMKV_BLOCK_H
