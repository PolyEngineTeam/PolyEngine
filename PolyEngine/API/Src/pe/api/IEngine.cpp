#include <pe/api/APIPCH.hpp>

#include <pe/api/IEngine.hpp>

namespace pe::api {

bool IEngine::IsQuitRequested() const { return QuitRequested; }
void IEngine::RequestGameQuit() { QuitRequested = true; }

bool IEngine::ShouldCaptureMouse() const { return MouseCaptureEnabled; }
void IEngine::SetMouseCapture(bool enabled) { MouseCaptureEnabled = enabled; }

bool IEngine::ShouldMouseBeVisible() const { return MouseVisible; }
void IEngine::SetMouseVisible(bool visible) { MouseVisible = visible; }

}