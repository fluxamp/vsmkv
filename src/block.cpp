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
#include <cstring>
#include <iostream>
#include "../vsmkv/block.h"
#include "../vsmkv/utils.h"

block::block(const std::string name, const vint id, const VSAPI* api, VSNodeRef* node, const VSFrameRef* frame,
             const uint64_t frame_size, const int16_t timecode) :
        element(name, id), frame(frame), head(1, timecode, 0x80),
        total_size(1 + vint(4 + frame_size).getSize() + (4 + frame_size)), frame_size(frame_size),
        vsapi(api), node(node)
{
    vi = vsapi->getVideoInfo(node);

    updateLength(4 + frame_size);
}

block::~block() {
    vsapi->freeFrame(frame);
}

size_t block::getSize() const {
    return total_size;
}

size_t block::output(char *buffer, size_t _size, size_t offset) const {
    size_t written = 0;

    if(_size <= 0) {
        return 0;
    }

    written += outputID(&buffer, _size, offset);
    written += outputLength(&buffer, _size, offset);

    if(offset < 4) {
        size_t m = MIN(4-offset, _size);
        for(size_t i=0; i < m; i++) {
            if(i==0) {
                buffer[i] = head.track_number;
            } else if(i < 3) {
                buffer[i] = (char)((head.timecode >> 8*(2-i))&0xFF);
            } else {
                buffer[i] = head.flags;
            }
        }

        written += m;
        offset = 0;
        _size -= m;
        buffer += m;
    } else {
        offset -= 4;
    }

    if(_size > 0) {
        // copy frame content
        for (int p = 0; p < vi->format->numPlanes && _size > 0; p++) {
            uint64_t stride = (uint64_t)vsapi->getStride(frame, p);
            uint64_t rowSize = (uint64_t)vsapi->getFrameWidth(frame, p) * vi->format->bytesPerSample;
            uint64_t height = (uint64_t)vsapi->getFrameHeight(frame, p);

            if(_size > 0 && offset < rowSize * height) {
                const uint8_t *readPtr = vsapi->getReadPtr(frame, p);

                uint64_t row_offset = offset % rowSize;
                uint64_t start_row = (offset - row_offset) / rowSize;

                if(start_row > 0) {
                    readPtr += stride * start_row;
                }

                if(row_offset > 0) {
                    readPtr += row_offset;
                }

                for (uint64_t y = start_row; y < height && _size > 0; y++) {
                    size_t to_copy = MIN((size_t)(rowSize-row_offset), _size);

                    memcpy(buffer, readPtr, to_copy);

                    _size -= to_copy;
                    written += to_copy;
                    buffer += to_copy;

                    row_offset = 0;
                    offset = 0;

                    readPtr += MIN(stride, to_copy);
                }
            } else {
                offset -= rowSize * height;
            }
        }
    }

    return written;
}

node* block::addChild(const node_ptr& child) {
    (void) child;
    return this;
}

void block::report(size_t offset, uint8_t indent) const {
    if(offset >= total_size) {
        return;
    }

    printIndent(indent);
    std::cout << name << std::endl;

    if(offset < ID.getSize()) {
        ID.report(offset, indent + 1);
        std::cout << " (EBML ID)" << std::endl;
        return;
    }
    offset -= ID.getSize();

    if(offset < length.getSize()) {
        length.report(offset, indent + 1);
        std::cout << " (EBML length)" << std::endl;
        return;
    }
    offset -= length.getSize();

    printIndent(indent + 1);
    std::cout << "offset " << offset;
    if(offset < 4) {
        std::cout << " of block header" << std::endl;
        return;
    }

    std::cout << " of block data" << std::endl;
}
