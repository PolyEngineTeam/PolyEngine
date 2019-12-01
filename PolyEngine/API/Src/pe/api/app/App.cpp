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

}