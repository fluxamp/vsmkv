/*

 Copyright (c) 2019 Peter Hegen

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
#include "vsmkv/element.h"

namespace vsmkv::tests
{

Test(element, generic)
{
    cr_assert(element("testelement", 0x00).getSize() == 2, "generic element has size != 2");
}

Test(element, output)
{
    unsigned char buffer[32];
    memset(buffer, 0, 32);

    element("single element", 0x1F).output((char*)buffer, 32, 0);
    cr_expect(buffer[0] == 0x9F, "element failed to output its id (expected 0x9F, received 0x%x)", buffer[0]);
    cr_expect(buffer[1] == 0x80, "element failed to output the length of its children (expected 0x80, received 0x%x)", (unsigned char)buffer[1]);

    element parent("parent", 0x03);
    parent.addChild(std::make_shared<element>("child", 0x42));
    parent.output((char*)(buffer+2), 30, 0);

    cr_expect(buffer[2] == 0x83, "parent element failed to output its id (expected 0x83, received 0x%x)", (unsigned char)buffer[2]);
    cr_expect(buffer[3] == 0x82, "parent element failed to output the length of its children (expected 0x82, received 0x%x)", (unsigned char)buffer[3]);
    cr_expect(buffer[4] == 0xC2, "child element failed to output its id (expected 0xC2, received 0x%x)", (unsigned char)buffer[4]);
    cr_expect(buffer[5] == 0x80, "child element failed to output the length of its children (expected 0x80, received 0x%x)", (unsigned char)buffer[5]);
}

Test(element, addChild)
{
    auto parent = std::make_shared<element>("parent", 0x01);
    auto child = std::make_shared<element>("child", 0x40e1);
    const auto parent_only_size = parent->getSize();

    cr_assert(parent_only_size == 2, "parent element should have default size");

    parent->addChild(child);
    cr_assert(parent->getSize() == parent_only_size + child->getSize(), "element hasn't updated size to encompass its child node");
}

} // namespace vsmkv::test
