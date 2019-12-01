#pragma once

#include <pe/Defines.hpp>

#include <pe/engine/input/InputSystem.hpp>
#include <pe/api/ecs/ISystem.hpp>
#include <pe/core/utils/EnumUtils.hpp>
#include <pe/api/ecs/ComponentIDGenerator.hpp>
#include <pe/api/editor/IEditor.hpp>

#include <pe/api/IEngine.hpp>
#include <pe/api/IGame.hpp>
#include <pe/api/rendering/IRenderingDevice.hpp>
#include <pe/api/deps/DependencyPtr.hpp>

namespace pe::engine
{
	/// <summary>Central part of engine. It can be obtained vis gEngine global variable.</summary>
	class ENGINE_DLLEXPORT Engine : public ::pe::api::IEngine
	{
	public:
		void startGame() override;
		void endGame() override;

		void registerSystem(std::unique_ptr<::pe::api::ecs::ISystem>&& system,
			 api::eEngineUpdatePhase order) override;
		void update(std::chrono::duration<double> dt) override;
		void selectiveUpdate(std::chrono::duration<double> dt,
			const std::vector<api::eEngineUpdatePhase>& phases) override;

		::pe::api::ecs::Scene* getActiveScene() override { return m_activeScene.get(); }
		void loadDefaultScene() override;
		
		Engine();
		~Engine();

		api::rendering::IRenderingDevice* GetRenderingDevice() const { return RenderingDevice.get(); }

		void ResizeScreen(const core::math::Vector2i& size);

		const char* (*GetClipboardTextFunction)(void* user_data);
		void(*SetClipboardTextFunction)(void* user_data, const char* text);
	private:
		inline void UpdatePhases(std::chrono::duration<double> dt, api::eEngineUpdatePhase order)
		{
			HEAVY_ASSERTE(order != api::eEngineUpdatePhase::_COUNT, "_COUNT enum value passed to UpdatePhases(), which is an invalid value");
			for (auto&  update : GameUpdatePhases[order])
				update->OnUpdate(getActiveScene());
		}

		
		api::deps::DependencyPtr<api::app::App> m_app;
		std::unique_ptr<::pe::api::ecs::Scene> m_activeScene;
		std::unique_ptr<api::rendering::IRenderingDevice> RenderingDevice;
		::pe::core::utils::EnumArray<std::vector<std::unique_ptr<api::ecs::ISystem>>, api::eEngineUpdatePhase> GameUpdatePhases;
	};
}
