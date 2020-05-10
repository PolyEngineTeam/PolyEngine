FROM ubuntu18-base

LABEL Description="This image is used to build PolyEngine for Linux with Clang 5" Vendor="PolyEngine team" Version="1.0"

ENV CC clang-9
ENV CXX clang++-9
