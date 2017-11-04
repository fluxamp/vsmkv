cmake_minimum_required(VERSION 3.7)
project(vsmkv)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/main.cpp
        src/stream.cpp
        src/vint.cpp
        src/element.cpp
        src/string.cpp
        src/integer.cpp
        src/node.cpp
        src/clusterlist.cpp
        src/block.cpp
        src/floating.cpp
        src/fourcc.cpp
        src/cues.cpp
        )
add_executable(vsmkv ${SOURCE_FILES})
target_include_directories(vsmkv PRIVATE ./)

target_link_libraries(vsmkv fuse)
target_link_libraries(vsmkv vapoursynth)
target_link_libraries(vsmkv vapoursynth-script)
target_link_libraries(vsmkv pthread)
target_link_libraries(vsmkv criterion)

add_definitions(-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26)
