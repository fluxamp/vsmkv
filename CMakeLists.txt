cmake_minimum_required(VERSION 3.7)
project(vsmkv)

option(test "build tests" OFF)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
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
add_executable(vsmkv ${SOURCE_FILES} src/main.cpp)
target_include_directories(vsmkv PRIVATE ./)

target_link_libraries(vsmkv fuse)
target_link_libraries(vsmkv vapoursynth)
target_link_libraries(vsmkv vapoursynth-script)
target_link_libraries(vsmkv pthread)
add_definitions(-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26)

#if(test)
    enable_testing()

    file(GLOB TEST_SRC_FILES tests/*.cpp)

    add_executable(vsmkv_test ${TEST_SRC_FILES} ${SOURCE_FILES})
    target_include_directories(vsmkv_test PRIVATE ./)

    target_link_libraries(vsmkv_test criterion)
    target_link_libraries(vsmkv_test pthread)

    add_test(runtest vsmkv_test)
#endif()