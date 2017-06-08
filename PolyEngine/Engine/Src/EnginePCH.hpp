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

// Resources
#include "ResourceBase.hpp"
#include "ResourceManager.hpp"
#include "GLTextureResource.hpp"
#include "GLMeshResource.hpp"
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

//Config
#include "CoreConfig.hpp"

// Systems
#include "DeferredTaskSystem.hpp"