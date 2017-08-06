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

#include <assert.h>
#include "cues.h"
#include "elements.h"

cues::cues(const std::string name, const vint& id, const clusterlist* list) :
    cues(name, id, list, 15) {}

cues::cues(const std::string name, const vint& id, const clusterlist* list, const int cue_every_nth_cluster) :
        element(name, id), list(list), cue_every_nth_cluster(cue_every_nth_cluster) {
    assert(list != NULL);
}

node_ptr cues::addChild(node_ptr child) {
    (void) child;
    return (node_ptr)(this);
}

/*
 * rebuild the cue list when the parent is changed (should only happen once)
 */
void cues::setParent(node_ptr parent) {
    node::setParent(parent);

    assert(children.size() == 0);   // parent should not change

    const uint64_t cluster_size = list->getClustersize();
    const uint64_t cluster_duration = list->getClusterduration();
    const int num_clusters = list->getNumClusters();

    const int num_cues = (num_clusters - (num_clusters % cue_every_nth_cluster)) / cue_every_nth_cluster;

    // calculate size of cues from dummy cue
    node_ptr dummy_cue = CuePoint()
            ->addChild(CueTime(0, 4))
            ->addChild(CueTrackPositions()
                               ->addChild(CueTrack())
                               ->addChild(CueClusterPosition(0, 8))
            )
    ;
    const uint64_t cue_size = dummy_cue->getSize();
    delete dummy_cue;

    // get position of cues element relative to segment
    const uint64_t cue_offset = parent->getOffset(this);

    // Cues EBML ID + length + (num_cues * cue_size)
    const uint64_t cluster_offset = cue_offset + 4 + vint(num_cues * cue_size).getSize() + (num_cues * cue_size);

    if(children.size() > 0) {
        // should never execute
        for(node_ptr c : children) {
            delete c;
        }
        children.clear();
    }

    for(int i=0; i<num_cues; i++) {
        node_ptr cue = CuePoint()
                ->addChild(CueTime(i * cluster_duration * cue_every_nth_cluster, 4))
                ->addChild(CueTrackPositions()
                                   ->addChild(CueTrack())
                                   ->addChild(CueClusterPosition(cluster_offset + i * cluster_size * cue_every_nth_cluster, 8))
                )
        ;
        assert(cue->getSize() == cue_size); // cue and dummy cue are not in sync! (forgot to add children?)
        element::addChild(cue);
    }
}
