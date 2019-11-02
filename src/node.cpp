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
#include "../vsmkv/node.h"

size_t node::getSize() const {
    size_t ret = 0;

    for(auto child : children) {
        ret += child->getSize();
    }

    return ret;
}

size_t node::output(char *buffer, size_t _size, size_t offset) const {
    size_t written = 0;

    if(_size <= 0) {
        return 0;
    }

    size_t length = getSize();

    if(offset < length) {
        for (unsigned long i = 0; i < children.size(); i++) {
            node_ptr child = children[i];
            size_t child_size = child->getSize();

            if (offset < child_size && _size > 0) {
                size_t child_written = child->output(buffer, std::min(_size, child_size - offset), offset);
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

node* node::addChild(const node_ptr& child) {
    children.push_back(child);
    child->setParent(this);

    return this;
}

void node::report(size_t offset, uint8_t indent) const {
    size_t length = getSize();

    if(offset < length) {
        for (unsigned long i = 0; i < children.size(); i++) {
            node_ptr child = children[i];
            size_t child_size = child->getSize();

            if (offset < child_size) {
                return child->report(offset, indent);
            } else {
                offset -= child_size;
            }
        }
    } else {
        std::cerr << "offset " << offset << " not found!" << "\n";
    }
}

void node::printIndent(uint8_t indent) const {
    std::cout << "+";
    for(uint8_t i=0; i < indent; i++) {
        std::cout << " ";
    }
}

size_t node::getOffset(const node_ptr& child) const {
    return getOffset(child.get());
}

size_t node::getOffset(const node* child) const {
    size_t ret = 0;
    node* c;

    uint64_t i;
    for(i=0; i<children.size() && (c=children[i].get()) != child; i++) {
        ret += c->getSize();
    }

    return i < children.size() ? ret : 0;
}
