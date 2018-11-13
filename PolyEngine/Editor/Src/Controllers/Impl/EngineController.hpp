#pragma once

// qt
#include <QtCore/qtimer.h>

// interface
#include <Controllers/IEngineController.hpp>

#include <Configs/ProjectConfig.hpp>

//------------------------------------------------------------------------------
namespace PolyEditor
{
	class IEditor;
}

//------------------------------------------------------------------------------
namespace editor::controllers::impl
{
	class EngineController final : public IEngineController
	{
	public:
		static std::unique_ptr<EngineController> Create(const ProjectConfig& projectConfig);

		EngineController();

		void RegisterView() final {}
		void UnregisterView() final {}

		void SetEngine(std::unique_ptr<Poly::Engine> engineToControl) final;
		void Edit() final;
		void Play() final;
		void Pause() final;

		// TODO(squeras): remove this
		Poly::Engine* GetEngine() { return ControlledEngine.get(); }

	private:
		void OnEngineUpdateTimerTick();

		std::unique_ptr<Poly::Engine> ControlledEngine;
		PolyEditor::IEditor* Editor = nullptr;
		QTimer EngineUpdateTimer;

	}; // class EngineController

} // namespace editor::controllers::impl