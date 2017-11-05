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

#ifndef VSMKV_NODE_H
#define VSMKV_NODE_H

#include <cstddef>
#include <memory>
#include <vector>

class node;

typedef node* node_ptr;

/*
 * master class from which most others are derived from
 */
class node {
public:
    node() {};
    virtual ~node();

    virtual size_t getSize() const;
    virtual size_t output(char* buffer, size_t _size, size_t offset) const;

    virtual node_ptr addChild(node_ptr child);

    virtual void setParent(node_ptr p) { parent = p; }
    size_t getOffset(node_ptr child);

    /* used for debugging: prints the tree that leads to the specified offset */
    virtual void report(size_t offset, uint8_t indent) const;
    inline void report(size_t offset) { return report(offset, 0); }

protected:
    void print_indent(uint8_t indent) const;

    std::vector<node_ptr> children;
    node_ptr parent;
};


#endif //VSMKV_NODE_H