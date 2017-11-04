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

#include "vsmkv/fourcc.h"
#include "vapoursynth/VapourSynth.h"

std::pair<int, uint32_t> codes[] =
{
    // RGB formats not supported yet (due to non-planar format)
//    std::make_pair(pfRGB24, 0x00000000),

    // https://github.com/FFmpeg/FFmpeg/blob/master/libavcodec/raw.c
    std::make_pair(pfYUV420P8, FOURCC('I', '4', '2', '0')),
    std::make_pair(pfYUV410P8, FOURCC('I', '4', '1', '0')),
    std::make_pair(pfYUV411P8, FOURCC('I', '4', '1', '1')),
    std::make_pair(pfYUV422P8, FOURCC('I', '4', '2', '2')),
    std::make_pair(pfYUV444P8, FOURCC('I', '4', '4', '4')),

    std::make_pair(pfYUV420P9, FOURCC('Y', '3', 11, 9)),
    std::make_pair(pfYUV422P9, FOURCC('Y', '3', 10, 9)),
    std::make_pair(pfYUV444P9, FOURCC('Y', '3', 0, 9)),
    std::make_pair(pfYUV420P10, FOURCC('Y', '3', 11, 10)),
    std::make_pair(pfYUV420P12, FOURCC('Y', '3', 11, 12)),
    std::make_pair(pfYUV420P14, FOURCC('Y', '3', 11, 14)),
    std::make_pair(pfYUV420P16, FOURCC('Y', '3', 11, 16)),
    std::make_pair(pfYUV422P10, FOURCC('Y', '3', 10, 10)),
    std::make_pair(pfYUV422P12, FOURCC('Y', '3', 10, 12)),
    std::make_pair(pfYUV422P14, FOURCC('Y', '3', 10, 14)),
    std::make_pair(pfYUV422P16, FOURCC('Y', '3', 10, 16)),
    std::make_pair(pfYUV444P10, FOURCC('Y', '3', 0, 10)),
    std::make_pair(pfYUV444P12, FOURCC('Y', '3', 0, 12)),
    std::make_pair(pfYUV444P14, FOURCC('Y', '3', 0, 14)),
    std::make_pair(pfYUV444P16, FOURCC('Y', '3', 0, 16)),
};

// http://forums.codeguru.com/showthread.php?478012-how-to-implement-c-table-lookup&s=6bc1d8bcb3da39a4b87600d792eb5790&p=1846716#post1846716
const int N = sizeof(codes) / sizeof(codes[0]);
const std::map<int, uint32_t> fourcc::cc_map(codes, codes+N);

const uint32_t fourcc::lookup(int key, uint32_t def)
{
    uint32_t ret = def;

    auto search = cc_map.find(key);
    if(search != cc_map.end()) {
        ret = search->second;
    }

    return ret;
}