#pragma once

#include <pe/Defines.hpp>
#include <pe/api/app/IAppBackend.hpp>
#include <pe/api/deps/DependencyBase.hpp>

#include <pe/api/input/InputQueue.hpp>
#include <pe/api/input/OutputQueue.hpp>

namespace pe::api
{
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

	void KeyDown(input::eKey key) { InputEventsQueue.pushBack({input::eInputEventType::KEYDOWN, key}); }
	void KeyUp(input::eKey key) { InputEventsQueue.pushBack({input::eInputEventType::KEYUP, key}); }
	void AddCharacterUTF8(const char* charUTF8) { InputEventsQueue.pushBack({input::eInputEventType::TEXTCHAR, charUTF8}); }
	void MouseButtonDown(input::eMouseButton button) { InputEventsQueue.pushBack({input::eInputEventType::MOUSEBUTTONDOWN, button}); }
	void MouseButtonUp(input::eMouseButton button) { InputEventsQueue.pushBack({input::eInputEventType::MOUSEBUTTONUP, button}); }
	void UpdateMouseMove(const ::pe::core::math::Vector2i& delta) { InputEventsQueue.pushBack({input::eInputEventType::MOUSEMOVE, delta }); }
	void UpdateMousePos(const ::pe::core::math::Vector2i& pos) { InputEventsQueue.pushBack({ input::eInputEventType::MOUSEPOS, pos }); }
	void UpdateWheelPos(const ::pe::core::math::Vector2i& deltaPos) { InputEventsQueue.pushBack({input::eInputEventType::WHEELMOVE, deltaPos}); }
	void AddController(size_t id) { InputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_ADDED, id}); };
	void RemoveController(size_t id) { InputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_REMOVED, id}); };
	void ControllerButtonDown(size_t id, input::eControllerButton button) { InputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_BUTTON_DOWN, id, button}); };
	void ControllerButtonUp(size_t id, input::eControllerButton button) { InputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_BUTTON_UP, id, button}); };
	void ControllerAxisMotion(size_t id, input::eControllerAxis axis, i16 value) { InputEventsQueue.pushBack({input::eInputEventType::CONTROLLER_AXIS_MOTION, id, axis, value/35768.0f}); };

    input::InputQueue& GetInputQueue() { return InputEventsQueue; }
    input::OutputQueue& GetOutputQueue() { return OutputEventsQueue; }


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
    
    input::InputQueue InputEventsQueue;
	input::OutputQueue OutputEventsQueue;
};

}
}