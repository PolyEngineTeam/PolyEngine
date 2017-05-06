# PolyEngine
[![license](https://img.shields.io/github/license/KNTGPolygon/PolyEngine.svg)](LICENSE)
[![build status](https://travis-ci.org/KNTGPolygon/PolyEngine.svg?branch=dev)](https://travis-ci.org/KNTGPolygon/PolyEngine)
[![build status](https://ci.appveyor.com/api/projects/status/wa9par5pe99sow82?svg=true)](https://ci.appveyor.com/project/MuniuDev/polyengine)

Game engine study project developed by [**KNTG Polygon**](http://polygon.pw.edu.pl/), licensed under a very permissive MIT license.  
Currently it is in a very early stage of development, thus backwards compatibility is not guaranteed.

## Supported platforms
* Windows 7+ with MSVC compiler (Visual Studio 2015, toolset v140 with SDK 8.1)
* Linux with both GCC and Clang compilers (X11 with GLX 1.3 extension additionally required)
* macOS (**planned**)

PolyEngine currently uses OpenGL 3.3+ only, but might gain support for other graphic
APIs (DirectX, Vulkan, Metal) in the future.

## Branch description:
* master - main branch of the project, it's intended for nightly versions of the engine. Stable releases will be additionally tagged. This branch is guaranteed to be rebase/force push free.
* dev - branch intended for pull request integration and stability testing. Rebases are allowed here, but will be kept to minimum.

Other branches can be removed, renamed or rebased anytime.
