<h1> <img src="Media/PolyEngine_logo_black.png" width="128"> PolyEngine </h1>

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
* Linux with both GCC and Clang compilers (editor unsupported)
* macOS with Clang compiler (rendering unsupported until we switch to Vulkan)

PolyEngine currently uses OpenGL 4.3+ only, but might gain support for other graphic  
APIs (DirectX, Vulkan, Metal) in the future.

## Dependencies
* Assimp 3 - model loading
* Box2D 2.3 - 2D physics backend
* Bullet - 3D physics backend
* Catch 2.0.1 - unit tests
* Freetype 2 - font loading and rendering
* OGG Vorbis 1.3 - audio files loading
* OpenAL 1.1 - audio playback
* OpenGL 4.3, Epoxy 1.1 - rendering backend
* RapidJSON 1.0 - JSON parser
* SDL 2 - window management
* stb_image 2.19 - texture loading
* Qt 5.10 - editor frontend

Precompiled Windows libraries are available in the repository to ease the compilation process.

## Documentation
Documentation in Doxygen HTML format is available [here](https://kntgpolygon.github.io/PolyEngine/Docs/html/index.html "PolyEngine docs").

## Branches:
* master - main branch of the project, intended for stable, tested versions of the engine. Releases will be additionally tagged. Guaranteed to be rebase/force-push free.
* dev - branch intended for pull request integration and stability testing. Guaranteed to be rebase/force-push free.

Other branches can be removed, renamed or rebased anytime.
