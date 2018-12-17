#pragma once

// Core
#include <Defines.hpp>
#include <RTTI/RTTI.hpp>

// Math
#include <Math/BasicMath.hpp>
#include <Math/Angle.hpp>
#include <Math/Vector.hpp>
#include <Math/Vector2f.hpp>
#include <Math/Vector2i.hpp>
#include <Math/Vector3f.hpp>
#include <Math/Matrix.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Frustum.hpp>
#include <Math/Random.hpp>

// Geometry
#include <Math/AABox.hpp>
#include <Math/AARect.hpp>

// Memory
#include <BaseObject.hpp>
#include <Memory/Allocator.hpp>
#include <Memory/PoolAllocator.hpp>
#include <Memory/IterablePoolAllocator.hpp>
#include <Memory/RefCountedBase.hpp>
#include <Memory/SafePtr.hpp>
#include <Memory/SafePtrRoot.hpp>

// Containers
#include <Collections/String.hpp>
#include <Collections/StringBuilder.hpp>
#include <Collections/Dynarray.hpp>
#include <Collections/Queue.hpp>
#include <Collections/PriorityQueue.hpp>
#include <Collections/OrderedMap.hpp>

// Other
#include <Math/Color.hpp>
#include <Utils/FileIO.hpp>
#include <Utils/Logger.hpp>
#include <UniqueID.hpp>
#include <Utils/EnumUtils.hpp>
#include <Utils/OutputStream.hpp> 

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