#pragma once

// Core
#include <Defines.hpp>

// Math
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
#include <Memory/SafePtr.hpp>
#include <Memory/SafePtrRoot.hpp>

// Containers
#include <Collections/Dynarray.hpp>

// Other
#include <FileIO.hpp>
#include <Utils/Logger.hpp>

// ECS
#include <ECS/Scene.hpp>

// Rendering
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

// Movement
#include <Movement/MovementSystem.hpp>

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