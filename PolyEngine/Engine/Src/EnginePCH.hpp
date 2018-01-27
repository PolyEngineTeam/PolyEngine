#pragma once

// Core
#include <CorePCH.hpp>

// Engine
#include "Engine.hpp"

// ECS
#include "ComponentBase.hpp"
#include "Entity.hpp"
#include "World.hpp"
#include "EntityTransform.hpp"

// Rendering
#include "IRenderingDevice.hpp"

// Audio
#include "OpenALDevice.hpp"

// Utils
#include "InputQueue.hpp"
#include "KeyBindings.hpp"
#include "Mesh.hpp"

// Resources
#include "ResourceBase.hpp"
#include "ResourceManager.hpp"
#include "TextureResource.hpp"
#include "MeshResource.hpp"
#include "FontResource.hpp"
#include "SoundResource.hpp"

// Components
#include "CameraComponent.hpp"
#include "FreeFloatMovementComponent.hpp"
#include "MeshRenderingComponent.hpp"
#include "ScreenSpaceTextComponent.hpp"
#include "SoundEmitterComponent.hpp"
#include "SoundListenerComponent.hpp"
#include "Physics2DColliders.hpp"
#include "Rigidbody2DComponent.hpp"
#include "LightSourceComponent.hpp"
#include "PostprocessSettingsComponent.hpp"
#include "DebugDrawComponents.hpp"

// World Components
#include "DebugWorldComponent.hpp"
#include "InputWorldComponent.hpp"
#include "SoundWorldComponent.hpp"
#include "TimeWorldComponent.hpp"
#include "ViewportWorldComponent.hpp"
#include "DeferredTaskWorldComponent.hpp"
#include "Physics2DWorldComponent.hpp"

// Systems
#include "DeferredTaskSystem.hpp"
#include "SoundSystem.hpp"
#include "ParticleUpdateSystem.hpp"
#include "DeferredTaskSystem.hpp"
#include "Physics2DSystem.hpp"
#include "DebugDrawSystem.hpp"

// Config
#include "AssetsPathConfig.hpp"
#include "DebugConfig.hpp"