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

#ifndef VSMKV_FLOATING_H
#define VSMKV_FLOATING_H

#include "element.h"

/*
 * EBML float
 */
class floating : public element{
public:
    floating(const std::string name, const vint &id) :
            element(name, id), value(0.0f) {}
    floating(const std::string name, const vint &id, const float v) :
            element(name, id), value(v) { updateLength(4); }
    floating(const std::string name, const vint &id, const float v, const size_t s) :
            element(name, id), value(v) { updateLength(4); }

    virtual size_t output(char* buffer, size_t _size, size_t offset) const override;

    virtual node* addChild(const node_ptr& child) override;
    virtual void report(size_t offset, uint8_t indent) const override;

private:
    union {
        float value;
        char cvalue[4];
    };
};

#endif //VSMKV_FLOATING_H
