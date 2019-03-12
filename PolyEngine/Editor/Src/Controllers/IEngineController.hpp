#pragma once

namespace Poly
{
	class Engine;
}

namespace editor::controllers
{
	class IEngineController
	{
	public:
		virtual ~IEngineController() = default;

		virtual void SetEngine(std::unique_ptr<Poly::Engine> engineToControl) = 0;
		virtual void Play() = 0;
		virtual void Pause() = 0;
		virtual void Edit() = 0;
	};
}