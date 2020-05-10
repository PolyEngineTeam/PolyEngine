FROM ubuntu20-base

LABEL Description="This image is used to build PolyEngine for Linux with GCC 8" Vendor="PolyEngine team" Version="1.0"

ENV CC gcc-10
ENV CXX g++-10