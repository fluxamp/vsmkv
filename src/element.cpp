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
#include "../vsmkv/element.h"
#include "../vsmkv/utils.h"

size_t element::getSize() const {
    return ID.getSize() + length.getSize() + length.getValue();
}

size_t element::output(char *buffer, size_t _size, size_t offset) const {
    size_t written = 0;

    if(_size <= 0) {
        return 0;
    }

    written += outputID(&buffer, _size, offset);
    written += outputLength(&buffer, _size, offset);

    if(offset < length.getValue() && _size > 0) {
        for (unsigned long i = 0; i < children.size(); i++) {
            node_ptr child = children[i];
            size_t child_size = child->getSize();

            if (offset < child_size && _size > 0) {
                size_t child_written = child->output(buffer, MIN(_size, child_size - offset), offset);
                written += child_written;
                _size -= child_written;
                offset = 0;
                buffer += child_written;
            } else {
                offset -= child_size;
            }
        }
    }

    return written;
}
node_ptr element::addChild(node_ptr child) {
    node::addChild(child);
    length += child->getSize();

    return (node_ptr)(this);
}

void element::report(size_t offset, uint8_t indent) const {
    if (offset >= ID.getSize() + length.getSize() + length.getValue()) {
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
    offset -= length.getSize();

    return node::report(offset, indent + 1);
}

size_t element::outputID(char **buffer, size_t &_size, size_t &offset) const {
    size_t written = 0;

    if(offset < ID.getSize()) {
        size_t id_written = ID.output(*buffer, MIN(ID.getSize(), _size - offset), offset);
        written += id_written;
        _size -= id_written;
        offset = 0;
        *buffer += id_written;
    } else {
        offset -= ID.getSize();
    }

    return written;
}

size_t element::outputLength(char **buffer, size_t &_size, size_t &offset) const {
    size_t written = 0;

    if(offset < length.getSize() && _size > 0) {
        size_t length_written = length.output(*buffer, MIN(length.getSize(), _size - offset), offset);
        written += length_written;
        _size -= length_written;
        offset = 0;
        *buffer += length_written;
    } else {
        offset -= length.getSize();
    }

    return written;
}

