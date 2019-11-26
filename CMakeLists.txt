cmake_minimum_required(VERSION 3.7)
project(vsmkv)

option(test "build tests" OFF)

set(CMAKE_CXX_STANDARD 17)

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

add_library(vsmkv_lib STATIC ${SOURCE_FILES})
target_include_directories(vsmkv_lib PUBLIC ./)
target_link_libraries(vsmkv_lib vapoursynth)
target_link_libraries(vsmkv_lib vapoursynth-script)
target_link_libraries(vsmkv_lib pthread)

if(NOT test)
    add_definitions(-O2)
else()
    add_definitions(-g -Og)
endif()

add_executable(vsmkv src/main.cpp)

target_link_libraries(vsmkv vsmkv_lib)
target_link_libraries(vsmkv fuse)
add_definitions(-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26)

if(test)
    enable_testing()

    set(TEST_SRC_FILES
        tests/element_test.cpp
        tests/node_test.cpp
        tests/vint_test.cpp
    )
    add_executable(vsmkv_test ${TEST_SRC_FILES})

    target_link_libraries(vsmkv_test vsmkv_lib)
    target_link_libraries(vsmkv_test criterion)

    add_test(runtest vsmkv_test)
endif()
