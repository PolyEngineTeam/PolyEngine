#pragma once

// Core
#include <CorePCH.hpp>

// Engine
#include <EnginePCH.hpp>

// ECS
#include <ECS/Scene.hpp>
#include <ECS/Entity.hpp>
#include <ECS/EntityTransform.hpp>
#include <ECS/DeferredTaskBase.hpp>
#include <ECS/ComponentBase.hpp>

// Rendering
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/Camera/CameraSystem.hpp>
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