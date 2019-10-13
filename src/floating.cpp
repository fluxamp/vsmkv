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

#include <iostream>
#include <cstring>
#include "vsmkv/floating.h"
#include "vsmkv/utils.h"

size_t floating::output(char *buffer, size_t _size, size_t offset) const {
    size_t written = 0;

    if(_size <= 0) {
        return 0;
    }

    written += outputID(&buffer, _size, offset);
    written += outputLength(&buffer, _size, offset);

    if(offset < 4 && _size > 0) {
        size_t l = MIN(_size, 4);

        for(int i=0; i<l; i++) {
            buffer[i] = cvalue[3-i];
        }

        written += l;
    }

    return written;
}

node* floating::addChild(const node_ptr& child) {
    (void) child;
    return this;
}

void floating::report(size_t offset, uint8_t indent) const {
    if (offset >= ID.getSize() + length.getSize() + 4) {
        return;
    }

    print_indent(indent);
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

    print_indent(indent + 1);
    std::cout << "value " << value << std::endl;
}
