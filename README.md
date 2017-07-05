# PolyEngine
[![license](https://img.shields.io/github/license/KNTGPolygon/PolyEngine.svg)](LICENSE)

Stable (master):
[![build status](https://travis-ci.org/KNTGPolygon/PolyEngine.svg?branch=master)](https://travis-ci.org/KNTGPolygon/PolyEngine)
[![build status](https://ci.appveyor.com/api/projects/status/wa9par5pe99sow82/branch/master?svg=true)](https://ci.appveyor.com/project/MuniuDev/polyengine)

Testing (dev):
[![build status](https://travis-ci.org/KNTGPolygon/PolyEngine.svg?branch=dev)](https://travis-ci.org/KNTGPolygon/PolyEngine)
[![build status](https://ci.appveyor.com/api/projects/status/wa9par5pe99sow82/branch/dev?svg=true)](https://ci.appveyor.com/project/MuniuDev/polyengine)

Game engine study project developed by [**KNTG Polygon**](http://polygon.pw.edu.pl/), licensed under a very permissive MIT license.  
Currently it is in a early stage of development, thus backwards compatibility is not guaranteed.

## Supported platforms
* Windows 7+ with MSVC compiler (Visual Studio 2015, toolset v140 with SDK 8.1)
* Linux with both GCC and Clang compilers (X11 with GLX 1.3 extension additionally required)
* macOS (**planned**)

## Library dependancies
* GLEW, OpenGL 3.3+ - GL rendering device
* Freetype - true type font loading and rendering
* Assimp - mesh loading
* SOIL - image loading
* Catch - unit tests

PolyEngine currently uses OpenGL 3.3+ only, but might gain support for other graphic
APIs (DirectX, Vulkan, Metal) in the future.

## Documentation
Documentation in doxygen html format is available [here](https://kntgpolygon.github.io/PolyEngine/Docs/html/index.html "PolyEngine docs").

## Branch description:
* master - main branch of the project, it's intended for stable, tested versions of the engine. Releases will be additionally tagged. This branch is guaranteed to be rebase/force push free.
* dev - branch intended for pull request integration and stability testing. This branch is guaranteed to be rebase/force push free.

Other branches can be removed, renamed or rebased anytime.
