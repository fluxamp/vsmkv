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

#ifndef VSMKV_ELEMENTS_H
#define VSMKV_ELEMENTS_H

#include "element.h"
#include "string.h"
#include "integer.h"
#include "floating.h"

// TODO: add cue elements

inline node_ptr MasterNode(void) { return node_ptr (new node()); }

inline node_ptr EBMLHead(void) { return node_ptr(new element("EBMLHead", vint(0xa45dfa3))); }
inline node_ptr EBMLVersion(const uint64_t v=1) { return node_ptr(new integer("EBMLVersion", vint(0x286), v)); }
inline node_ptr EBMLReadVersion(const uint64_t v=1) { return node_ptr(new integer("EBMLReadVersion", vint(0x2f7), v)); }
inline node_ptr EBMLMaxIDLength(const uint64_t v=4) { return node_ptr(new integer("EBMLMaxIDLength", vint(0x2f2), v)); }
inline node_ptr EBMLMaxSizeLength(const uint64_t v=8) { return node_ptr(new integer("EBMLMaxSizeLength", vint(0x2f3), v)); }

inline node_ptr DocType(const char* type = "matroska") { return node_ptr(new string("DocType", vint(0x282), std::string(type))); }
inline node_ptr DocTypeVersion(const uint64_t v=4) { return node_ptr(new integer("DocTypeVersion", vint(0x287), v)); }
inline node_ptr DocTypeReadVersion(const uint64_t v=2) { return node_ptr(new integer("DocTypeReadVersion", vint(0x285), v)); }

inline node_ptr Segment(void) { return node_ptr(new element("Segment", vint(0x8538067))); }
inline node_ptr Info(void) { return node_ptr(new element("Info", vint(0x549a966))); }
inline node_ptr TimecodeScale(const uint64_t v=1000000) { return node_ptr(new integer("TimecodeScale", vint(0x285), v)); }
inline node_ptr MuxingApp(const char* type = "vsmkv") { return node_ptr(new string("MuxingAp", vint(0xd80), std::string(type))); }
inline node_ptr WritingApp(const char* type = "vsmkv") { return node_ptr(new string("WritingApp", vint(0x1741), std::string(type))); }

inline node_ptr Tracks(void) { return node_ptr(new element("Tracks", vint(0x654ae6b))); }
inline node_ptr TrackEntry(void) { return node_ptr(new element("TrackEntry", vint(0x2e))); }
inline node_ptr TrackNumber(const uint64_t v=1) { return node_ptr(new integer("TrackNumber", vint(0x57), v)); }
inline node_ptr TrackUID(const uint64_t v=1) { return node_ptr(new integer("TrackUID", vint(0x33c5), v)); }

enum TrackTypes {
    video = 1,
    audio = 2,
    complex = 3,
    logo = 16,
    subtitle = 17,
    buttons = 18,
    control = 32,
};

inline node_ptr TrackType(const TrackTypes t=video) { return node_ptr(new integer("TrackType", vint(0x03), vint(t))); }
inline node_ptr TrackFlagEnabled(const bool b=true) { return node_ptr(new integer("TrackFlagEnabled", vint(0x39), vint(b == true ? 1 : 0))); }
inline node_ptr TrackFlagDefault(const bool b=true) { return node_ptr(new integer("TrackFlagDefault", vint(0x08), vint(b == true ? 1 : 0))); }
inline node_ptr TrackFlagForced(const bool b=false) { return node_ptr(new integer("TrackFlagForced", vint(0x15aa), vint(b == true ? 1 : 0))); }
inline node_ptr TrackFlagLacing(const bool b=true) { return node_ptr(new integer("TrackFlagLacing", vint(0x1c), vint(b == true ? 1 : 0))); }
inline node_ptr TrackMinCache(const uint64_t v=0) { return node_ptr(new integer("TrackMinCache", vint(0x2de7), v)); }
inline node_ptr MaxBlockAdditionID(const uint64_t v=0) { return node_ptr(new integer("MaxBlockAdditionID", vint(0x15ee), v)); }
inline node_ptr CodecID(const char* type = "V_UNCOMPRESSED") { return node_ptr(new string("CodecID", vint(0x06), std::string(type))); }
inline node_ptr TrackDefaultDuration(const uint64_t v) { return node_ptr(new integer("TrackDefaultDuration", vint(0x3E383), v)); }

inline node_ptr TrackVideo(void) { return node_ptr(new element("TrackVideo", vint(0x60))); }
inline node_ptr VideoPixelWidth(const uint64_t v) { return node_ptr(new integer("VideoPixelWidth", vint(0x30), v)); }
inline node_ptr VideoPixelHeight(const uint64_t v) { return node_ptr(new integer("VideoPixelHeight", vint(0x3A), v)); }
inline node_ptr VideoDisplayWidth(const uint64_t v) { return node_ptr(new integer("VideoDisplayWidth", vint(0x14b0), v)); }
inline node_ptr VideoDisplayHeight(const uint64_t v) { return node_ptr(new integer("VideoDisplayHeight", vint(0x14ba), v)); }
inline node_ptr VideoColorSpace(uint32_t cc=0x00000000) { return node_ptr(new integer("VideoColorSpace", vint(0xeb524), cc, 4)); }
inline node_ptr VideoFPS(float f) { return node_ptr(new floating("VideoFPS", vint(0x383e3), f)); }


inline node_ptr VideoColor(void) { return node_ptr(new element("VideoColor", vint(0x15b0))); }
inline node_ptr ColorBitsPerChannel(const uint64_t v) { return node_ptr(new integer("ColorBitsPerChannel", vint(0x15b2), v)); }
// TODO: Chroma subsampling etc.

inline node_ptr Cluster(void) { return node_ptr(new element("Cluster", vint(0xF43B675))); }
inline node_ptr ClusterTimecode(uint64_t tc=0, size_t size=4) { return node_ptr(new integer("ClusterTimecode", vint(0x67), tc, size)); }
inline node_ptr BlockGroup(void) { return node_ptr(new element("BlockGroup", vint(0x20))); }

inline node_ptr CuePoint(void) { return node_ptr(new element("CuePoint", vint(0x3B))); }
inline node_ptr CueTime(uint64_t tc=0, size_t size=4) { return node_ptr(new integer("CueTime", vint(0x33), tc, size)); }
inline node_ptr CueTrackPositions(void) { return node_ptr(new element("CuePoint", vint(0x37))); }
inline node_ptr CueTrack(uint8_t track=1) { return node_ptr(new integer("CueTrack", vint(0x77), track, 1)); }
inline node_ptr CueClusterPosition(uint64_t pos, size_t size=4) { return node_ptr(new integer("CueClusterPosition", vint(0x71), pos, size)); }
inline node_ptr CueDuration(uint64_t dur, size_t size=4) { return node_ptr(new integer("CueDuration", vint(0x32), dur, size)); }

#endif //VSMKV_ELEMENTS_H
