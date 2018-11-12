#pragma once

namespace Poly
{
	class Engine;
}

namespace editor::views
{
	class IEngineView
	{
	public:
		virtual ~IEngineView() = default;

		virtual void OnModelChanged(const Poly::Engine* model) = 0;
		virtual void OnPlay() = 0;
		virtual void OnPause() = 0;
		virtual void OnEdit() = 0;
	};
}

namespace editor::controllers
{
	class IEngineController
	{
	public:
		virtual ~IEngineController() = default;

		virtual void RegisterView() = 0;
		virtual void UnregisterView() = 0;

		virtual void SetEngine(std::unique_ptr<Poly::Engine> engineToControl) = 0;
		virtual void Play() = 0;
		virtual void Pause() = 0;
		virtual void Edit() = 0;
	};
}