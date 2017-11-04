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

#include <string>
#include <iostream>
#include "vsmkv/vint.h"
#include "vsmkv/utils.h"

size_t vint::output(char* buffer, size_t _size, size_t offset) const
{
    size_t ret = MIN(size, 8);

    if(ret == 0) {
        if(value < 0x80) {
            ret = 1;
        } else if(value < 0x4000) {
            ret = 2;
        } else if(value < 0x200000) {
            ret = 3;
        } else if(value < 0x10000000) {
            ret = 4;
        } else if(value < 0x800000000) {
            ret = 5;
        } else if(value < 0x40000000000) {
            ret = 6;
        } else if(value < 0x2000000000000) {
            ret = 7;
        } else if(value < 0x100000000000000) {
            ret = 8;
        } else {
            char error_buffer[64];
            sprintf(error_buffer, "encountered misformed value for value: 0x%lx", value);

            throw std::string(error_buffer);
        }
    }

    if(ret > _size) {
        throw std::string("integer value is greater than buffer size (but still within spec)");
    }

    buffer[0] = (char)(0x80 >> (ret-1));
    char left_mask = (char)(0x7F >> (ret-1));

    uint64_t temp_value = value;
    for(size_t i=ret; i > offset + 1; i--) {
        buffer[i-1] = (char)(temp_value & 0xFF);
        temp_value >>= 8;
    }

    if(offset <= 0) {
        buffer[0] |= (char) (temp_value & left_mask);
    }

    return ret;
}

size_t vint::getSize() const {
    return size;
}

void vint::updateSize(bool forceUpdate) {
    if(size > 0 && !forceUpdate) {
        return;
    }

    if(value < 0x80) {
        size = 1;
    } else if(value < 0x4000) {
        size = 2;
    } else if(value < 0x200000) {
        size = 3;
    } else if(value < 0x10000000) {
        size = 4;
    } else if(value < 0x800000000) {
        size = 5;
    } else if(value < 0x40000000000) {
        size = 6;
    } else if(value < 0x2000000000000) {
        size = 7;
    } else if(value < 0x100000000000000) {
        size = 8;
    } else {
        char error_buffer[64];
        sprintf(error_buffer, "encountered misformed value for value: 0x%lx", value);

        throw std::string(error_buffer);
    }
}

node_ptr vint::addChild(node_ptr child) {
    (void) child;
    return (node_ptr)(this);
}

void vint::report(size_t offset, uint8_t indent) const {
    if(offset < size) {
        print_indent(indent);
        std::cout << "vint " << value;
    }
}
