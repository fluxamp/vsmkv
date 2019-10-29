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
#include "../vsmkv/cues.h"
#include "../vsmkv/elements.h"

cues::cues(const std::string name, const vint& id, const clusterlist_ptr list) :
    cues(name, id, list, 15) {}

cues::cues(const std::string name, const vint& id, const clusterlist_ptr list, const int cue_every_nth_cluster) :
        element(name, id), list(list), cue_every_nth_cluster(cue_every_nth_cluster) {
    assert(list != NULL);
}

node* cues::addChild(const node_ptr& child) {
    (void) child;
    return this;
}

/*
 * rebuild the cue list when the parent is changed (should only happen once)
 */
void cues::setParent(const node* parent) {
    node::setParent(parent);

    assert(children.size() == 0);   // parent should not change

    const uint64_t cluster_size = list->getClustersize();
    const uint64_t cluster_duration = list->getClusterduration();
    const auto num_clusters = list->getNumClusters();

    const uint64_t num_cues = (num_clusters - (num_clusters % cue_every_nth_cluster)) / cue_every_nth_cluster;

    // calculate size of cues from dummy cue
    auto track_positions = CueTrackPositions();
    track_positions->addChild(CueTrack())
                   ->addChild(CueClusterPosition(0, 8))
    ;

    auto dummy_cue = CuePoint();
    dummy_cue->addChild(CueTime(0, 4))
             ->addChild(track_positions)
    ;
    const uint64_t cue_size = dummy_cue->getSize();

    // get position of cues element relative to segment
    const uint64_t cue_offset = parent->getOffset(this);

    // Cues EBML ID + length + (num_cues * cue_size)
    const uint64_t cluster_offset = cue_offset + 4 + vint(num_cues * cue_size).getSize() + (num_cues * cue_size);

    if(children.size() > 0) {
        // should never execute
        children.clear();
    }

    for(uint64_t i=0; i<num_cues; i++) {
        auto track_positions = CueTrackPositions();
        track_positions->addChild(CueTrack())
                        ->addChild(CueClusterPosition(cluster_offset + i * cluster_size * cue_every_nth_cluster, 8))
        ;

        auto cue = CuePoint();
        cue->addChild(CueTime(i * cluster_duration * cue_every_nth_cluster, 4))
           ->addChild(track_positions)
        ;
        assert(cue->getSize() == cue_size); // cue and dummy cue are not in sync! (forgot to add children?)
        element::addChild(cue);
    }
}
