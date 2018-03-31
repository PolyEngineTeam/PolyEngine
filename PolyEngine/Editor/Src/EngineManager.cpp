#include "PolyEditorPCH.hpp"

#include <LibraryLoader.hpp>

EngineManager::EngineManager()
{
	// setup update timer
	Updater = std::make_unique<QTimer>(this);
	connect(Updater.get(), &QTimer::timeout, this, &EngineManager::UpdatePhase);
}

void EngineManager::Init(std::unique_ptr<Poly::IGame> game, std::unique_ptr<Poly::IRenderingDevice> device)
{
	if (Engine)
		throw new EngineManagerException("Creating Engine twice?");

	Engine = std::make_unique<Poly::Engine>();
	Engine->Init(std::move(game), std::move(device));
	Poly::gConsole.LogDebug("Engine created successfully");
	Updater->start(0);
}

void EngineManager::UpdatePhase()
{ 
	Engine->Update();
}