#include "PolyEditorPCH.hpp"

#include <Engine.hpp>
#include <Utils/LibraryLoader.hpp>
#include <Configs/AssetsPathConfig.hpp>
#include <Movement/FreeFloatMovementComponent.hpp>
#include <Rendering/Camera/CameraComponent.hpp>
#include <Rendering/ViewportWorldComponent.hpp>
#include <Rendering/PostprocessSettingsComponent.hpp>
#include <ECS/DeferredTaskSystem.hpp>
#include <SDL.h>

//------------------------------------------------------------------------------
extern "C"
{
	using CreateRenderingDeviceFunc = Poly::IRenderingDevice* (SDL_Window*, const Poly::ScreenSize&);
}

static Poly::LibraryFunctionHandle<CreateRenderingDeviceFunc> LoadRenderingDevice;

using namespace Poly;

const static float PLAYER_CAMERA_NEAR = 0.1f;
const static float PLAYER_CAMERA_FAR = 300.f;
const static Angle PLAYER_CAMERA_FOV = 60_deg;

//------------------------------------------------------------------------------
ViewportInspectorWidget::ViewportInspectorWidget(QWidget* parent)
	: InspectorWidgetBase(parent)
{
	setAttribute(Qt::WA_NativeWindow);
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	setMinimumSize(320, 200);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		ASSERTE(false, "SDL initialization failed!");

	Poly::gConsole.LogDebug("SDL initialized.");

	new QGridLayout(this);
	SDLWidget = new QWidget(this);
	layout()->addWidget(SDLWidget);

	SDLWidget->setAttribute(Qt::WA_NativeWindow);
	SDLWidget->setMouseTracking(true);
	SDLWidget->setFocusPolicy(Qt::ClickFocus);

	// TODO: catch winId changes (http://doc.qt.io/qt-5/qwidget.html#winId)
	// TODO: something like addviewport to rendering device
	ASSERTE(!WindowInSDL.IsValid(), "Window already initialized!");
	WindowInSDL = CustomSDLWindow::CreateSDLWindowFromArgs((void*)SDLWidget->winId(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	ASSERTE(WindowInSDL.IsValid(), "Window creation failed!");
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::InitializeConnections()
{
	connect(gApp->InspectorMgr, &InspectorManager::EntitiesSelectionChanged, this, &ViewportInspectorWidget::SetSelectedEntities);
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::Reset()
{
	SelectedEntities.Clear();
}

//------------------------------------------------------------------------------
std::unique_ptr<Poly::IRenderingDevice> ViewportInspectorWidget::GetRenderingDevice()
{
	// TODO fix library names differences between platforms
	if (!LoadRenderingDevice.FunctionValid())
	{
		// Load rendering device library
		LoadRenderingDevice = Poly::LoadFunctionFromSharedLibrary<CreateRenderingDeviceFunc>(
			gApp->ProjectMgr->GetProjectConfig().GetRenderingDeviceDllPath().GetCStr(), "PolyCreateRenderingDevice");
		ASSERTE(LoadRenderingDevice.FunctionValid(), "Library libRenderingDevice load failed");
		Poly::gConsole.LogDebug("Library libRenderingDevice loaded.");
	}

	Poly::ScreenSize viewportRect;
	viewportRect.Width = width();
	viewportRect.Height = height();

	return std::unique_ptr<Poly::IRenderingDevice>(LoadRenderingDevice(WindowInSDL.Get(), viewportRect));
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::Init()
{
	if (EngineState == eEngineState::EDIT)
	{
		gEngine->RegisterEditorUpdatePhase(EditorCameraMovementSystem::Update);
		gEngine->RegisterEditorUpdatePhase(GizmoSystem::Update);

		// create camera
		Camera = DeferredTaskSystem::SpawnEntityImmediate(gEngine->GetWorld());
		// add EditorCameraMovementComponent
		DeferredTaskSystem::AddComponentImmediate<EditorCameraMovementComponent>(gEngine->GetWorld(), Camera, 25.f, 0.01f);
		// add camera component
		DeferredTaskSystem::AddComponentImmediate<CameraComponent>(gEngine->GetWorld(), Camera, 60_deg, 1.0f, 1000.f);
		// set camera in world component
		gEngine->GetWorld()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, gEngine->GetWorld()->GetComponent<CameraComponent>(Camera));
		// add postprocess settings component
		DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(gEngine->GetWorld(), Camera);
		gEngine->GetWorld()->GetComponent<PostprocessSettingsComponent>(Camera)->UseFgShader = true;
		gEngine->GetWorld()->GetComponent<PostprocessSettingsComponent>(Camera)->UseBgShader = true;
	}
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::Deinit()
{
	DeferredTaskSystem::DestroyEntityImmediate(gEngine->GetWorld(), Camera);
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::UpdateInspectors()
{
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::resizeEvent(QResizeEvent* resizeEvent)
{
	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		return;

	Poly::ScreenSize screenSize;
	screenSize.Width = resizeEvent->size().width();
	screenSize.Height = resizeEvent->size().height();
	Poly::gEngine->ResizeScreen(screenSize);
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		return;
	Poly::gEngine->UpdateWheelPos(Poly::Vector2i(wheelEvent->delta(), 0));
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		return;
	Poly::gEngine->UpdateMousePos(Poly::Vector2i(mouseEvent->pos().x(), mouseEvent->pos().y()));

	gConsole.LogDebug("{}; {}", mouseEvent->pos().x(), mouseEvent->pos().y());
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		return;

	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Poly::gEngine->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::LEFT));
		break;

	case Qt::RightButton:
		Poly::gEngine->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::RIGHT));
		break;

	case Qt::MiddleButton:
		Poly::gEngine->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::MIDDLE));
		break;

	default:
		// unsupported key
		break;
	}
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		return;
	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Poly::gEngine->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::LEFT));
		break;

	case Qt::RightButton:
		Poly::gEngine->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::RIGHT));
		break;

	case Qt::MiddleButton:
		Poly::gEngine->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::MIDDLE));
		break;

	default:
		// unsupported key
		break;
	}
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		return;

	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
		Poly::gEngine->KeyDown(static_cast<Poly::eKey>(SDL_GetScancodeFromKey(QtKeyEventToSDLKeycode((Qt::Key)keyEvent->key()))));
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (gApp->EngineMgr->GetEngineState() == eEngineState::NONE)
		return;

	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
		Poly::gEngine->KeyUp(static_cast<Poly::eKey>(SDL_GetScancodeFromKey(QtKeyEventToSDLKeycode((Qt::Key)keyEvent->key()))));
}
