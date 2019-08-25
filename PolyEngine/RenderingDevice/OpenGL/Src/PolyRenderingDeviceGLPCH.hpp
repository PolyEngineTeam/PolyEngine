#pragma once

// Core
#include <pe/Defines.hpp>
#include <pe/core/rtti/RTTI.hpp>

// Math
#include <pe/core/math/BasicMath.hpp>
#include <pe/core/math/Angle.hpp>
#include <pe/core/math/Vector.hpp>
#include <pe/core/math/Vector2f.hpp>
#include <pe/core/math/Vector2i.hpp>
#include <pe/core/math/Vector3f.hpp>
#include <pe/core/math/Matrix.hpp>
#include <pe/core/math/Quaternion.hpp>
#include <pe/core/math/Frustum.hpp>
#include <pe/core/math/Random.hpp>

// Geometry
#include <pe/core/math/AABox.hpp>
#include <pe/core/math/AARect.hpp>

// Memory
#include <pe/core/BaseObject.hpp>
#include <pe/core/memory/Allocator.hpp>
#include <pe/core/memory/PoolAllocator.hpp>
#include <pe/core/memory/IterablePoolAllocator.hpp>
#include <pe/core/memory/RefCountedBase.hpp>
#include <pe/core/memory/SafePtr.hpp>
#include <pe/core/memory/SafePtrRoot.hpp>

// Containers
#include <pe/core/storage/String.hpp>
#include <pe/core/storage/StringBuilder.hpp>

#include <pe/core/storage/Queue.hpp>
#include <pe/core/storage/PriorityQueue.hpp>

// Other
#include <pe/core/math/Color.hpp>
#include <pe/core/utils/FileIO.hpp>
#include <pe/core/utils/Logger.hpp>
#include <pe/core/UniqueID.hpp>
#include <pe/core/utils/EnumUtils.hpp>
#include <pe/core/utils/OutputStream.hpp>

// ECS
#include <ECS/Scene.hpp>
#include <ECS/Entity.hpp>
#include <ECS/EntityTransform.hpp>
#include <ECS/DeferredTaskBase.hpp>
#include <ECS/ComponentBase.hpp>

// Rendering
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/Viewport.hpp>
#include <Rendering/ViewportWorldComponent.hpp>
#include <Rendering/Particles/ParticleComponent.hpp>
#include <Rendering/Particles/ParticleEmitter.hpp>
#include <Rendering/Lighting/LightSourceComponent.hpp>
#include <Rendering/MeshRenderingComponent.hpp>
#include <Rendering/SkyboxWorldComponent.hpp>
#include <Rendering/PostprocessSettingsComponent.hpp>
#include <Rendering/SpritesheetComponent.hpp>
#include <Rendering/IRenderingDevice.hpp>

// UI
#include <UI/ScreenSpaceTextComponent.hpp>

// Debugging
#include <Debugging/DebugDrawComponents.hpp>

// Resources 
#include <Resources/TextureResource.hpp>
#include <Resources/ResourceManager.hpp>

// Time
#include <Time/TimeSystem.hpp>
#include <Time/TimeWorldComponent.hpp>

// Configs 
#include <Configs/AssetsPathConfig.hpp>
#include <Configs/DebugConfig.hpp>

// SDL
#include <SDL.h>

// Imgui
#include <imgui.h>

// Temporary namespace usings
using namespace pe;