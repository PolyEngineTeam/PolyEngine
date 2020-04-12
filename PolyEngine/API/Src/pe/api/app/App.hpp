#pragma once

#include <pe/Defines.hpp>
#include <pe/api/app/IAppBackend.hpp>
#include <pe/api/deps/DependencyBase.hpp>

#include <pe/api/input/InputQueue.hpp>
#include <pe/api/input/OutputQueue.hpp>

namespace pe::api {
class IEngine;
class IGame;
namespace rendering { class IRenderingDevice; }
namespace editor { class IEditor; }
namespace app {

class API_DLLEXPORT App : public deps::DependencyBase<App>
{
    using Base = deps::DependencyBase<App>;
public:
    App(std::unique_ptr<IAppBackend>&& backend);
    virtual ~App();

    IEngine* getEngine() { return m_engine.get(); }
    editor::IEditor* getEditor() { return m_editor.get(); }
    IGame* getGame() { return m_game.get(); }
    rendering::IRenderingDevice* getRenderingDevice() { return m_renderingDevice.get(); }

	// Input
	// @todo Move this to specific i/o interface.
	void keyDown(input::eKey key) { m_inputEventsQueue.pushBack({input::eInputEventType::KEYDOWN, key}); }
	void keyUp(input::eKey key) { m_inputEventsQueue.pushBack({input::eInputEventType::KEYUP, key}); }
	void addCharacterUTF8(const char* charUTF8) { m_inputEventsQueue.pushBack({input::eInputEventType::TEXTCHAR, charUTF8}); }
	void mouseButtonDown(input::eMouseButton button) { m_inputEventsQueue.pushBack({input::eInputEventType::MOUSEBUTTONDOWN, button}); }
	void mouseButtonUp(input::eMouseButton button) { m_inputEventsQueue.pushBack({input::eInputEventType::MOUSEBUTTONUP, button}); }
	void updateMouseMove(const ::pe::core::math::Vector2i& delta) { m_inputEventsQueue.pushBack({input::eInputEventType::MOUSEMOVE, delta }); }
	void updateMousePos(const ::pe::core::math::Vector2i& pos) { m_inputEventsQueue.pushBack({ input::eInputEventType::MOUSEPOS, pos }); }
	void updateWheelPos(const ::pe::core::math::Vector2i& deltaPos) { m_inputEventsQueue.pushBack({input::eInputEventType::WHEELMOVE, deltaPos}); }
	void addController(size_t id) { m_inputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_ADDED, id}); };
	void removeController(size_t id) { m_inputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_REMOVED, id}); };
	void controllerButtonDown(size_t id, input::eControllerButton button) { m_inputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_BUTTON_DOWN, id, button}); };
	void controllerButtonUp(size_t id, input::eControllerButton button) { m_inputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_BUTTON_UP, id, button}); };
	void controllerAxisMotion(size_t id, input::eControllerAxis axis, i16 value) { m_inputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_AXIS_MOTION, id, axis, value/35768.0f}); };

    input::InputQueue& getInputQueue() { return m_inputEventsQueue; }
    input::OutputQueue& getOutputQueue() { return m_outputEventsQueue; }


	void registerEngine(std::unique_ptr<IEngine>&& engine) { m_engine = std::move(engine); }
	void registerEditor(std::unique_ptr<editor::IEditor>&& editor) { m_editor = std::move(editor); }
	void registerRenderingDevice(std::unique_ptr<rendering::IRenderingDevice>&& dev) { m_renderingDevice = std::move(dev); }
	void registerGame(std::unique_ptr<IGame>&& game) { m_game = std::move(game); }
private:
    std::unique_ptr<IAppBackend> m_backend;

    std::unique_ptr<IEngine> m_engine;
    std::unique_ptr<editor::IEditor> m_editor;
    std::unique_ptr<rendering::IRenderingDevice> m_renderingDevice;
	std::unique_ptr<IGame> m_game;
    
    input::InputQueue m_inputEventsQueue;
	input::OutputQueue m_outputEventsQueue;
};

}
}