<img src="Media/PolyEngine_logo_black.png" height="200">

[![license](https://img.shields.io/github/license/KNTGPolygon/PolyEngine.svg)](LICENSE)

Stable (master):
[![build status](https://travis-ci.org/KNTGPolygon/PolyEngine.svg?branch=master)](https://travis-ci.org/KNTGPolygon/PolyEngine)
[![build status](https://ci.appveyor.com/api/projects/status/wa9par5pe99sow82/branch/master?svg=true)](https://ci.appveyor.com/project/MuniuDev/polyengine)

Testing (dev):
[![build status](https://travis-ci.org/KNTGPolygon/PolyEngine.svg?branch=dev)](https://travis-ci.org/KNTGPolygon/PolyEngine)
[![build status](https://ci.appveyor.com/api/projects/status/wa9par5pe99sow82/branch/dev?svg=true)](https://ci.appveyor.com/project/MuniuDev/polyengine)

Game engine study project developed by [**KNTG Polygon**](http://polygon.pw.edu.pl/), licensed under a permissive MIT license.  
Currently it is in an early stage of development, thus backwards compatibility is not guaranteed.

## Supported platforms
* Windows 7+ with MSVC compiler (Visual Studio 2017, toolset v141 with SDK 8.1)
* Linux with both GCC and/or Clang compilers (editor unsupported)
* macOS with Clang compiler (rendering unsupported until we switch to Vulkan)

PolyEngine currently uses OpenGL 4.3+ only, but might gain support for other graphic  
APIs (DirectX, Vulkan, Metal) in the future.

## Dependencies
Alomost all dependancies are provided by submodules and are placed in `PolyEngine/ThirdParty` folder. Only Qt needs to be installed separately. They are:
* Assimp - model loading
* Box2D - 2D physics backend
* Bullet - 3D physics backend
* Catch - unit tests framework
* Freetype - font loading and rendering
* OGG Vorbis - audio files loading
* OpenAL - audio playback
* OpenGL 4.3 - rendering backend
* RapidJSON - JSON parser
* SDL2 - window management
* stb - texture loading (stb_image)
* Qt 5.10.1 - editor frontend

## Getting started
Our wiki contains guides on how to setup the engine. Each OS is a bit different:
* [Windows quick start guide](https://github.com/KNTGPolygon/PolyEngine/wiki/Windows-quick-start-guide)
* [Linux quick start guide](https://github.com/KNTGPolygon/PolyEngine/wiki/Linux-quick-start-guide)
* [macOS quick start guide](https://github.com/KNTGPolygon/PolyEngine/wiki/macOS-quick-start-guide)

## Example projects
Example projects are available in [the examples repository](https://github.com/KNTGPolygon/PolyEngineExamples).

## Documentation
Documentation in Doxygen HTML format is available [here](https://kntgpolygon.github.io/PolyEngine/Docs/html/index.html "PolyEngine docs").

## Branches:
* master - main branch of the project, intended for stable, tested versions of the engine. Releases will be additionally tagged. Guaranteed to be rebase/force-push free.
* dev - branch intended for pull request integration and stability testing. Guaranteed to be rebase/force-push free.

Other branches can be removed, renamed or rebased anytime.
