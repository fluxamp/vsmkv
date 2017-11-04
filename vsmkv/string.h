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

#ifndef VSMKV_STRING_H
#define VSMKV_STRING_H

#include <string>
#include "element.h"

/*
 * EBML string
 */
class string : public element {
public:
    string(const std::string name, const vint &id) : element(name, id) {}
    string(const std::string name, const vint& id, const std::string& s) :
            element(name, id), _s(s) { updateLength(s.length()); }

    virtual size_t output(char *buffer, size_t _size, size_t offset) const;

    virtual node_ptr addChild(node_ptr child);
    virtual void report(size_t offset, uint8_t indent) const;

private:
    std::string _s;
};

#endif //VSMKV_STRING_H
