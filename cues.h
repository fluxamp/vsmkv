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

#ifndef VSMKV_CUES_H
#define VSMKV_CUES_H

#include "element.h"
#include "clusterlist.h"

class cues : public element {
public:
    cues(const std::string name, const vint& id, const clusterlist* list);
    cues(const std::string name, const vint& id, const clusterlist* list, const int cue_every_nth_cluster);

    virtual node_ptr addChild(node_ptr child);
    virtual void setParent(node_ptr parent);

private:
    const clusterlist* list;
    const int cue_every_nth_cluster;

};

inline node_ptr Cues(clusterlist* cl) { return node_ptr(new cues("Cues", vint(0xC53BB6B), cl)); }

#endif //VSMKV_CUES_H
