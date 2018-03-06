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

#include <criterion/criterion.h>
#include "vsmkv/vint.h"

Test(vint, empty)
{
    cr_assert(vint().getValue() == 0, "missing initializer");
    cr_assert(vint().getSize() == 1, "empty vint has size != 1");
}

Test(vint, ranges)
{
    cr_assert(vint(0x7F).getSize() == 1, "vint returned wrong size (expected 1 byte for 0x7F)");
    cr_assert(vint(0x80).getSize() == 2, "vint returned wrong size (expected 2 bytes for 0x80)");
    cr_assert(vint(0x3FFF).getSize() == 2, "vint returned wrong size (expected 2 bytes for 0x3FFF)");
    cr_assert(vint(0x4000).getSize() == 3, "vint returned wrong size (expected 3 bytes for 0x4000)");
    cr_assert(vint(0x1FFFFF).getSize() == 3, "vint returned wrong size (expected 3 bytes for 0x1FFFFF)");
    cr_assert(vint(0x200000).getSize() == 4, "vint returned wrong size (expected 4 bytes for 0x200000)");
    cr_assert(vint(0x0FFFFFFF).getSize() == 4, "vint returned wrong size (expected 4 bytes for 0x0FFFFFFF)");
    cr_assert(vint(0x10000000).getSize() == 5, "vint returned wrong size (expected 5 bytes for 0x10000000)");
}

Test(vint, operators)
{
    cr_assert((vint(0x1F) + vint(0x1F)).getValue() - 2*(uint8_t)vint(0x1F) == 0, "error in vint arithmetic");

    vint number;
    number += 0x1F;
    number -= 0x1F;

    cr_assert((uint64_t)number == 0, "error in vint operators -=/+=");
}

Test(vint, output)
{
    unsigned char buffer[32];
    memset(buffer, 0, 32);
    vint(0x1F).output((char*)buffer, 32, 0);
    vint(0x80).output((char*)buffer+1, 31, 1);
    vint(0x81).output((char*)buffer+2, 30, 0);

    cr_assert(buffer[0] == 0x9F, "vint failed to output its number (expected 0x9F, received 0x%x)", (unsigned char)buffer[0]);
    cr_assert(buffer[1] == 0x80, "vint failed to output its number (expected 0x80, received 0x%x)", (unsigned char)buffer[1]);
    cr_assert(buffer[2] == 0x40 && buffer[3] == 0x81, "vint failed to output its number (expected 0x4081, received 0x%x%x)",
              (unsigned char)buffer[2], (unsigned char)buffer[2]);

    // without offset
    cr_assert(vint(0x0).output((char*)buffer, 32, 0) == 1, "vint.output returned wrong number of bytes written");
    cr_assert(vint(0x80).output((char*)buffer, 32, 0) == 2, "vint.output returned wrong number of bytes written");
    cr_assert(vint(0x4000).output((char*)buffer, 32, 0) == 3, "vint.output returned wrong number of bytes written");
    cr_assert(vint(0x200000).output((char*)buffer, 32, 0) == 4, "vint.output returned wrong number of bytes written");

    // with offset
    cr_assert(vint(0x80).output((char*)buffer, 32, 1) == 1, "vint.output returned wrong number of bytes written");
    cr_assert(vint(0x4000).output((char*)buffer, 32, 2) == 1, "vint.output returned wrong number of bytes written");
    cr_assert(vint(0x200000).output((char*)buffer, 32, 2) == 2, "vint.output returned wrong number of bytes written");

    // offset and limited size
    cr_assert(vint(0x210000).output((char*)buffer, 1, 1) == 1, "vint.output returned wrong number of bytes written");
    cr_assert(buffer[0] == 0x21, "vint failed to output its number (expected 0x21, received 0x%x)", (unsigned char)buffer[0]);
}

Test(vint, addChild)
{
    auto parent = std::make_shared<vint>(0x01);
    auto child = std::make_shared<vint>(0x40e1);
    parent->addChild(child);
    cr_assert(parent->getSize() == 1, "vint size changed after added child (vint should not accept any children)");
}