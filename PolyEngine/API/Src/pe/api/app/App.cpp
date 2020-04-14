#include <pe/api/app/App.hpp>

#include <pe/api/IEngine.hpp>
#include <pe/api/IGame.hpp>
#include <pe/api/editor/IEditor.hpp>
#include <pe/api/rendering/IRenderingDevice.hpp>

namespace pe::api::app {

App::App(std::unique_ptr<IAppBackend>&& backend)
: Base("pe::api::app::App"), m_backend(std::move(backend))
{

}

App::~App()
{  
	m_engine->endGame();
	m_game.reset();
	m_editor.reset();
	m_renderingDevice.reset();
	m_engine.reset();
}

void App::registerEngine(std::unique_ptr<IEngine>&& engine)
{
	m_engine = std::move(engine);
}

void App::registerEditor(std::unique_ptr<editor::IEditor>&& editor)
{
	m_editor = std::move(editor);

}
void App::registerRenderingDevice(std::unique_ptr<rendering::IRenderingDevice>&& dev)
{ 
	m_renderingDevice = std::move(dev);
}

void App::registerGame(std::unique_ptr<IGame>&& game)
{
	m_game = std::move(game);
}

}