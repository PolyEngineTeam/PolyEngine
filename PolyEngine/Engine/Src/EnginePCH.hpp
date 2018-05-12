#pragma once

// Core
#include <CorePCH.hpp>

// Engine
#include "Engine.hpp"

// ECS
#include "ECS/ComponentBase.hpp"
#include "ECS/Entity.hpp"
#include "ECS/World.hpp"
#include "ECS/EntityTransform.hpp"

// Rendering
#include "Rendering/IRenderingDevice.hpp"

// Audio
#include "Audio/OpenALDevice.hpp"

// Utils
#include "Input/InputQueue.hpp"
#include "Input/KeyBindings.hpp"
#include "Resources/Mesh.hpp"

// Resources
#include "Resources/ResourceBase.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/TextureResource.hpp"
#include "Resources/MeshResource.hpp"
#include "Resources/FontResource.hpp"
#include "Resources/SoundResource.hpp"

// Components
#include "Rendering/Camera/CameraComponent.hpp"
#include "Movement/FreeFloatMovementComponent.hpp"
#include "Rendering/MeshRenderingComponent.hpp"
#include "UI/ScreenSpaceTextComponent.hpp"
#include "Audio/SoundEmitterComponent.hpp"
#include "Audio/SoundListenerComponent.hpp"
#include "Physics2D/Physics2DColliders.hpp"
#include "Physics2D/Rigidbody2DComponent.hpp"
#include "Physics3D/Rigidbody3DComponent.hpp"
#include "Physics3D/Collider3DComponent.hpp"
#include "Rendering/Lighting/LightSourceComponent.hpp"
#include "Rendering/PostprocessSettingsComponent.hpp"
#include "Debugging/DebugDrawComponents.hpp"

// Scene Components
#include "Debugging/DebugWorldComponent.hpp"
#include "Input/InputWorldComponent.hpp"
#include "Audio/SoundWorldComponent.hpp"
#include "Time/TimeWorldComponent.hpp"
#include "Rendering/ViewportWorldComponent.hpp"
#include "ECS/DeferredTaskWorldComponent.hpp"
#include "Physics2D/Physics2DWorldComponent.hpp"
#include "Physics3D/Physics3DWorldComponent.hpp"

// Systems
#include "ECS/DeferredTaskSystem.hpp"
#include "Audio/SoundSystem.hpp"
#include "Rendering/Particles/ParticleUpdateSystem.hpp"
#include "Physics2D/Physics2DSystem.hpp"
#include "Physics3D/Physics3DSystem.hpp"
#include "Debugging/DebugDrawSystem.hpp"
#include "AI/PathfindingSystem.hpp"

// Config
#include "Configs/AssetsPathConfig.hpp"
#include "Configs/DebugConfig.hpp"
