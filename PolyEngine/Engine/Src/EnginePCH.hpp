#pragma once

// Core
#include <CorePCH.hpp>

// Engine
#include "Engine.hpp"

// ECS
#include "ComponentBase.hpp"
#include "Entity.hpp"
#include "World.hpp"

// Rendering
#include "IRenderingDevice.hpp"

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

// Components
#include "CameraComponent.hpp"
#include "FreeFloatMovementComponent.hpp"
#include "MeshRenderingComponent.hpp"
#include "ScreenSpaceTextComponent.hpp"
#include "TransformComponent.hpp"

// World Components
#include "DebugWorldComponent.hpp"
#include "InputWorldComponent.hpp"
#include "TimeWorldComponent.hpp"
#include "ViewportWorldComponent.hpp"
#include "DeferredTaskWorldComponent.hpp"

// Systems
#include "DeferredTaskSystem.hpp"

// Config
#include "CoreConfig.hpp"