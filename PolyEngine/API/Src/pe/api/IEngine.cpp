#include <pe/api/APIPCH.hpp>

#include <pe/api/IEngine.hpp>

namespace pe::api {

bool IEngine::isQuitRequested() const { return m_quitRequested; }
void IEngine::requestGameQuit() { m_quitRequested = true; }

bool IEngine::shouldCaptureMouse() const { return m_mouseCaptureEnabled; }
void IEngine::setMouseCapture(bool enabled) { m_mouseCaptureEnabled = enabled; }

bool IEngine::shouldMouseBeVisible() const { return m_mouseVisible; }
void IEngine::setMouseVisible(bool visible) { m_mouseVisible = visible; }

}