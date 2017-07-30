cmake_minimum_required(VERSION 3.7)
project(vsmkv)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp stream.cpp stream.h vint.cpp vint.h utils.h element.cpp element.h node.cpp node.h elements.h string.cpp string.h integer.cpp integer.h clusterlist.cpp clusterlist.h block.cpp block.h floating.cpp floating.h fourcc.cpp fourcc.h)
add_executable(vsmkv ${SOURCE_FILES})

target_link_libraries(vsmkv fuse)
target_link_libraries(vsmkv vapoursynth)
target_link_libraries(vsmkv vapoursynth-script)
target_link_libraries(vsmkv pthread)

add_definitions(-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26)
