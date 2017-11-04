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

#ifndef VSMKV_INTEGER_H
#define VSMKV_INTEGER_H

#include "element.h"
#include "utils.h"

/*
 * EBML unsigned integer
 */
class integer : public element{
public:
    integer(const std::string name, const vint &id) :
            element(name, id), value(0), size(0) {}
    integer(const std::string name, const vint &id, const uint64_t v) :
            element(name, id), value(v), size(0) { updateSize(); updateLength(size); }
    integer(const std::string name, const vint &id, const uint64_t v, const size_t s) :
            element(name, id), value(v), size(MIN(s, 8)) { updateLength(MIN(s, 8)); }

    virtual size_t output(char *buffer, size_t _size, size_t offset) const;

    virtual node_ptr addChild(node_ptr child);
    virtual void report(size_t offset, uint8_t indent) const;

private:
    void updateSize(void);

    uint64_t value;
    size_t size;
};

#endif //VSMKV_INTEGER_H
