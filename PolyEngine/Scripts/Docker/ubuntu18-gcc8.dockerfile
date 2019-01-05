FROM ubuntu18-base

LABEL Description="This image is used to build PolyEngine for Linux with GCC 8" Vendor="PolyEngine team" Version="1.0"

ENV CC gcc-8
ENV CXX g++-8