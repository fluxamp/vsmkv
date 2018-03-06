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
#include "vsmkv/node.h"
#include "vsmkv/vint.h"

node_ptr parent;
node_ptr child1 = std::make_shared<vint>(0x8F);  // size 2
node_ptr child2 = std::make_shared<vint>(0x04);  // size 1
node_ptr child3 = std::make_shared<vint>(0x15);  // size 1

void setup(void)
{
    parent = std::make_shared<node>();
    parent->addChild(child1);
    parent->addChild(child2);
}

void finish(void)
{
    parent = NULL;
}

Test(node, getSize, .init=setup, .fini=finish)
{
    node_ptr parent2 = std::make_shared<node>();
    cr_assert(parent2->getSize() == 0, "empty node has wrong size (expected 0, received %d)", parent2->getSize());
    cr_assert(parent2->addChild(child1)->getSize() == 2, "node has invalid size after adding a child (expected 2, received %d)", parent2->getSize());
    cr_assert(parent2->addChild(child2)->getSize() == 3, "node has invalid size after adding a child (expected 3, received %d)", parent2->getSize());
}

Test(node, getOffset, .init=setup, .fini=finish)
{
    size_t offset = 0;
    offset = parent->getOffset(child1);
    cr_assert(offset == 0, "child has wrong offset (expected 0, received %d)", offset);
    offset = parent->getOffset(child2);
    cr_assert(offset == 2, "child has wrong offset (expected 2, received %d)", offset);
    offset = parent->getOffset(child3);
    cr_assert(offset == 0, "non-child has offset (expected 0, received %d)", offset);
    parent->addChild(child3);
    offset = parent->getOffset(child3);
    cr_assert(offset == 3, "child has wrong offset (expected 3, received %d)", offset);
}

Test(node, output, .init=setup, .fini=finish)
{
    unsigned char buffer1[32];
    unsigned char buffer2[32];

    memset(buffer1, 0, 32);
    memset(buffer2, 0, 32);

    parent->output((char*)buffer1, 0, 32);
    child1->output((char*)buffer2, 0, 32);
    child2->output((char*)buffer2+2, 0, 30);

    for(int i=0; i<32; i++) {
        cr_assert(buffer1[i] == buffer2[i], "error in node.output function at offset %d", i);
    }
}
