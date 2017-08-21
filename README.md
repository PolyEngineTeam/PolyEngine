# PolyEngine
[![license](https://img.shields.io/github/license/KNTGPolygon/PolyEngine.svg)](LICENSE)

Stable (master):
[![build status](https://travis-ci.org/KNTGPolygon/PolyEngine.svg?branch=master)](https://travis-ci.org/KNTGPolygon/PolyEngine)
[![build status](https://ci.appveyor.com/api/projects/status/wa9par5pe99sow82/branch/master?svg=true)](https://ci.appveyor.com/project/MuniuDev/polyengine)

Testing (dev):
[![build status](https://travis-ci.org/KNTGPolygon/PolyEngine.svg?branch=dev)](https://travis-ci.org/KNTGPolygon/PolyEngine)
[![build status](https://ci.appveyor.com/api/projects/status/wa9par5pe99sow82/branch/dev?svg=true)](https://ci.appveyor.com/project/MuniuDev/polyengine)

Game engine study project developed by [**KNTG Polygon**](http://polygon.pw.edu.pl/), licensed under a permissive MIT license.  
Currently it is in a early stage of development, thus backwards compatibility is not guaranteed.

## Supported platforms
* Windows 7+ with MSVC compiler (Visual Studio 2015, toolset v140 with SDK 8.1)
* Linux with both GCC and Clang compilers (X11 with GLX 1.4 extension additionally required)
* macOS (**planned**)

PolyEngine currently uses OpenGL 3.3+ only, but might gain support for other graphic  
APIs (DirectX, Vulkan, Metal) in the future.

## Dependencies
* OpenGL 3.3, libepoxy 1.1 - rendering
* Freetype 2 - font loading and rendering
* Assimp 3 - model loading
* SOIL - texture loading
* Catch 1.8.2 - unit tests
* OpenAL 1.1 - audio playback
* OGG Vorbis 1.3 - audio files loading
* Box2D 2.3 - physics engine
* RapidJSON 1.0 - JSON parsing

Precompiled Windows libraries are available in the repository to ease the compilation process.

## Documentation
Documentation in Doxygen HTML format is available [here](https://kntgpolygon.github.io/PolyEngine/Docs/html/index.html "PolyEngine docs").

## Branches:
* master - main branch of the project, intended for stable, tested versions of the engine. Releases will be additionally tagged. Guaranteed to be rebase/force-push free.
* dev - branch intended for pull request integration and stability testing. Guaranteed to be rebase/force-push free.

Other branches can be removed, renamed or rebased anytime.
