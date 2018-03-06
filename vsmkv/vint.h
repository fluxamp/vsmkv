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

#ifndef VSMKV_VINT_H
#define VSMKV_VINT_H

#include <cstdint>
#include <cstddef>
#include "node.h"

/*
 * EBML variable integer
 */
class vint : public node {
public:
    vint() : value(0), size(0) { updateSize(); }
    vint(uint64_t v) : value(v), size(0) { updateSize(); }

    vint(uint64_t v, size_t s) : value(v), size(s) { updateSize(); }

    vint(const vint& clone) : value(clone.getValue()), size(clone.getSize()) { updateSize(); }

    operator uint64_t() const { return uint64_t(value); }
    operator uint32_t() const { return uint32_t(value); }
    operator uint16_t() const { return uint16_t(value); }
    operator uint8_t() const { return uint8_t(value); }

    const vint operator =(size_t& o) const { return vint(o); }

    size_t operator +(const size_t& s) const { return value + s; }
    const vint operator +(const vint& o) const { return vint(value + o.getValue()); }
    const vint operator +=(const size_t& o) { value += o; updateSize(true); return *this; }

    size_t operator -(const size_t& s) const { return value - s; }
    const vint operator -(const vint& o) const { return vint(value - o.getValue()); }
    const vint operator -=(const size_t& o) { value -= o; updateSize(true); return *this; }
    inline uint64_t getValue() const { return value; }

    virtual size_t getSize() const;
    virtual size_t output(char* buffer, size_t _size, size_t offset) const;

    virtual node* addChild(node_ptr child);
    virtual void report(size_t offset, uint8_t indent) const;

private:
    void updateSize(bool forceUpdate=false);

    uint64_t value;
    size_t size;
};

#endif //VSMKV_VINT_H
