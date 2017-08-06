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

#include <cstdio>
#include <iostream>

#include "stream.h"
#include "elements.h"
#include "clusterlist.h"
#include "fourcc.h"
#include "cues.h"

stream::stream(const VSAPI* api, VSNodeRef *node, const int index) : api(api), node(node)
{
    const VSVideoInfo* vi = api->getVideoInfo(node);

    double _f = ((double) vi->fpsNum)/vi->fpsDen;
    float fps = (float)_f;

    uint64_t duration = (uint64_t)(1000000000 / _f);

    const uint32_t cc_code = fourcc::lookup(vi->format->id, 0x000000);
    if(cc_code == 0) {
        throw std::string("unsupported color format ") + std::string(vi->format->name) +
              std::string(" of output node ") + std::to_string(index) +
              std::string(". choose one of the planar YUV formats from http://www.vapoursynth.com/doc/api/vapoursynth.h.html#vspresetformat");
    }

    // TODO: proper logging
//    std::cout << "double fps: " << _f << " from " << vi->fpsNum << "/" << vi->fpsDen << " resulting in " << fps << " as float." << std::endl;
//    std::cout << "frame format: " << vi->format->name << " with " << vi->format->bitsPerSample << " bits/sample and " << vi->format->numPlanes << " planes" << std::endl;

    node_ptr cl = ClusterList(api, node);
    node_ptr cues = Cues(static_cast<clusterlist*>(cl));

    master = MasterNode()
        ->addChild(EBMLHead()
            ->addChild(EBMLVersion())
            ->addChild(EBMLReadVersion())
            ->addChild(EBMLMaxIDLength())
            ->addChild(EBMLMaxSizeLength())
            ->addChild(DocType("matroska"))
            ->addChild(DocTypeVersion())
            ->addChild(DocTypeReadVersion())
        )
        ->addChild(Segment()
            ->addChild(Info()
                ->addChild(TimecodeScale())
                ->addChild(MuxingApp())
                ->addChild(WritingApp())
            )
            ->addChild(Tracks()
                ->addChild(TrackEntry()
                    ->addChild(TrackNumber(1))
                    ->addChild(TrackUID(1))
                    ->addChild(TrackType(video))
/*                    ->addChild(TrackFlagEnabled())
                    ->addChild(TrackFlagDefault())
                    ->addChild(TrackFlagForced())
                    ->addChild(TrackFlagLacing()) */
                    ->addChild(TrackMinCache())
/*                    ->addChild(MaxBlockAdditionID()) */
                    ->addChild(CodecID())
                    ->addChild(TrackDefaultDuration(duration))
                    ->addChild(TrackVideo()
                        ->addChild(VideoFPS(fps))
                        ->addChild(VideoPixelWidth((uint64_t)vi->width))
                        ->addChild(VideoPixelHeight((uint64_t)vi->height))
                        ->addChild(VideoDisplayWidth((uint64_t)vi->width))
                        ->addChild(VideoDisplayHeight((uint64_t)vi->height))
//                        ->addChild(VideoColorSpace(0x49343230)) // I420
                        ->addChild(VideoColorSpace(cc_code))
//                        ->addChild(VideoColor()
//                            ->addChild(ColorBitsPerChannel(vi->format->bitsPerSample))
//                        )
                    )
                )
            )
            ->addChild(cues)
            ->addChild(cl)
        )
    ;

    // TODO: debug
//    master->report(161);
}

stream::~stream() {
    api->freeNode(node);
}

size_t stream::getSize() {
    return master->getSize();
}

size_t stream::read(char *buf, size_t size, off_t offset) {
    return master->output(buf, size, (size_t)offset);
}

