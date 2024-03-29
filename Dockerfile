FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

# dependencies for zimg and vapoursynth
RUN apt update && apt install -y \
  gdb cmake fuse libfuse-dev build-essential nasm git libavcodec-dev libavformat-dev libavutil-dev libass-dev python3-pip python3-dev autoconf libtool libtesseract-dev

RUN pip3 install cython

RUN cd /opt/ && git clone -b release-3.0.4 https://github.com/sekrit-twc/zimg && cd zimg && ./autogen.sh && ./configure && make -j4 && make install
RUN cd /opt/ && git clone -b R60 https://github.com/vapoursynth/vapoursynth && cd vapoursynth && ./autogen.sh && ./configure && make -j4 && make install

# dependencies for ffms2
RUN apt install -y libswscale-dev
RUN cd /opt/ && git clone --depth 1 https://github.com/FFMS/ffms2 && cd ffms2 && ./autogen.sh && ./configure && make -j4 && make install

# tools for benchmarking, testing and debugging
RUN apt install -y ffmpeg x264 mkvtoolnix libcriterion-dev

# clean installation files
RUN apt -y autoclean

RUN useradd builder
USER builder

VOLUME /workspace
WORKDIR /workspace

# run vsmkv as:
# PYTHONPATH=/usr/local/lib/python3.x/site-packages build/vsmkv
