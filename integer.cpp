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
#include "integer.h"

size_t integer::output(char *buffer, size_t _size, size_t offset) const {
    size_t written = 0;

    if(_size <= 0) {
        return 0;
    }

    written += outputID(&buffer, _size, offset);
    written += outputLength(&buffer, _size, offset);

    if(offset < size && _size > 0) {
        uint64_t temp_value = value;
        size_t l = MIN(_size, size);

        for(size_t i=l; i > 0; i--) {
            buffer[i-1] = (char)(temp_value & 0xFF);
            temp_value >>= 8;
        }

        written += l;
    }

    return written;
}

node_ptr integer::addChild(node_ptr child) {
    (void) child;
    return (node_ptr)(this);
}

void integer::updateSize(void) {
    if(size == 0) {
        if(value < 0xFF) {
            size = 1;
        } else if(value < 0xFFFF) {
            size = 2;
        } else if(value < 0xFFFFFF) {
            size = 3;
        } else if(value < 0xFFFFFFFF) {
            size = 4;
        } else if(value < 0xFFFFFFFFFF) {
            size = 5;
        } else if(value < 0xFFFFFFFFFFFF) {
            size = 6;
        } else if(value < 0xFFFFFFFFFFFFFF) {
            size = 7;
        } else {
            size = 8;
        }
    }
}

void integer::report(size_t offset, uint8_t indent) const {
    if (offset >= ID.getSize() + length.getSize() + size) {
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
