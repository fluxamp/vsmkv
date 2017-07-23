## About

vsmkv is a FUSE-based virtual filesystem for exporting VapourSynth scripts as uncompressed videos in the Matroska (MKV) file format.

## Dependencies

FUSE, CMake and a compiler supporting the C++14 shared mutex (e.g., GCC >= 5.2) are required.

## Installation

from the commandline:

```commandline
mkdir build
cd build
cmake ../
make
```

## Running

The command requires two arguments. First, the path to your VapourSynth script. Second, an empty directory for mounting the filesystem. Additionally, a number of parameters inherited from FUSE may be specified (e.g., `-s` for running in single threaded mode or `-f` for running in foreground mode instead of daemonizing):

```commandline
cd /tmp
mkdir output
/path/to/vsmkv test.vpy output -f
```

Finally, a player supporting uncompressed video in MKV files can be used to play back the videos (first output node in this example):

```commandline
cd /tmp/output
mpv node_0
```

## Limitations

- Currently, the color space is hardcoded to YUV420p, though other color spaces like RGB24 should work after changing the FourCC value in `stream.cpp`.
- Concurrent access to the same video from multiple programs results in constant allocation and de-allocation of frames, resulting in a huge performance penalty.
- The software lacks proper logging and error handling for now - errors result in uncaught exceptions.
- Cues are not supported, yet. Skipping will require reading the whole file, i.e., will be really slow.