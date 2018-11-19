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

// unused variables
//const static float PLAYER_CAMERA_NEAR = 0.1f;
//const static float PLAYER_CAMERA_FAR = 300.f;
//const static Angle PLAYER_CAMERA_FOV = 60_deg;



//		general
//------------------------------------------------------------------------------
ViewportInspectorWidget::ViewportInspectorWidget(QWidget* parent, InspectorManager* mgr)
	: InspectorWidgetBase(parent, mgr)
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
	connect(Manager, &InspectorManager::StateChanged, this, &ViewportInspectorWidget::StateChanged);
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



//		iEditor
//------------------------------------------------------------------------------
String ViewportInspectorWidget::GetAssetsPathConfigPath()
{
	StringBuilder builder;
	builder.Append(Manager->GetConfig()->GetProjectPath());
	builder.Append("/Dist/Debug/AssetsPathConfig.json");

	return builder.StealString();
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::OnGameInit()
{
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::OnGameDeinit()
{
}

//------------------------------------------------------------------------------
Dynarray<Entity*> ViewportInspectorWidget::GetSelectedEntities()
{
	return Manager->GetSelectedEntities();
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::SetSelectedEntities(Dynarray<Entity*> entities)
{
	Manager->EntitiesSelectionChangedSlot(std::move(entities));
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::UpdateInspectors()
{
	//Manager->EntitiesModifiedSlot();
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::SetEngineState(PolyEditor::eEngineState state)
{
	Manager->StateChangedSlot(state);
}

//------------------------------------------------------------------------------
PolyEditor::eEngineState ViewportInspectorWidget::GetEngineState()
{
	return Manager->GetEngineState();
}

void ViewportInspectorWidget::HandleEngineOutputEvents(Poly::OutputQueue& OutputEventsQueue)
{
	while (!OutputEventsQueue.IsEmpty())
	{
		Poly::OutputEvent& ev = OutputEventsQueue.Front();
		switch (ev.Type)
		{
			case Poly::eOutputEventType::MOUSEPOS:
				SDLWidget->cursor().setPos(ev.Pos.X, ev.Pos.Y);
				break;
			case Poly::eOutputEventType::CURSORSHOW:
				SDLWidget->setCursor(Qt::ArrowCursor);
				break;
			case Poly::eOutputEventType::CURSORSET:
				SDLWidget->setCursor(QCursor(GetCursorType(ev.CursorType)));
				break;
			case Poly::eOutputEventType::CURSORHIDE:
				SDLWidget->setCursor(Qt::BlankCursor);
				break;
			case Poly::eOutputEventType::_COUNT:
				HEAVY_ASSERTE(false, "_COUNT enum value passed to InputEventQueue::Push(), which is an invalid value");
				break;
		}
		OutputEventsQueue.PopFront();
	}
}

Qt::CursorShape ViewportInspectorWidget::GetCursorType(Poly::eMouseCursorType cursorType)
{
	switch (cursorType)
	{
		case Poly::eMouseCursorType::ARROW:			return Qt::ArrowCursor;
		case Poly::eMouseCursorType::TEXTINPUT:		return Qt::IBeamCursor;
		case Poly::eMouseCursorType::RESIZEALL:		return Qt::SizeAllCursor;
		case Poly::eMouseCursorType::RESIZENS:		return Qt::SizeVerCursor;
		case Poly::eMouseCursorType::RESIZEEW:		return Qt::SizeHorCursor;
		case Poly::eMouseCursorType::RESIZENESW:	return Qt::SizeFDiagCursor;
		case Poly::eMouseCursorType::RESIZENWSE:	return Qt::SizeBDiagCursor;
		case Poly::eMouseCursorType::HAND:			return Qt::OpenHandCursor;
		default: return Qt::CursorShape::ArrowCursor;
	}
}

//		slots
//------------------------------------------------------------------------------
void ViewportInspectorWidget::StateChanged()
{
	switch (Manager->GetEngineState())
	{
	case PolyEditor::eEngineState::EDIT:
	{
		if (EditorCameraEnt)
			break;

		Scene* w = Manager->GetEngine()->GetActiveScene();

		//		register editor phases
		Manager->GetEngine()->RegisterEditorUpdatePhase(EditorCameraMovementSystem::Update);
		Manager->GetEngine()->RegisterEditorUpdatePhase(GizmoSystem::Update);

		//		create camera
		EditorCameraEnt = DeferredTaskSystem::SpawnEntityImmediate(w);
		EditorCameraEnt->SetName("Editor Camera");
		// add EditorCameraMovementComponent
		DeferredTaskSystem::AddComponentImmediate<EditorCameraMovementComponent>(w, EditorCameraEnt, 25.f, 0.01f);
		// add camera component
		DeferredTaskSystem::AddComponentImmediate<CameraComponent>(w, EditorCameraEnt, 60_deg, 1.0f, 1000.f);
		// add postprocess settings component
		DeferredTaskSystem::AddComponentImmediate<PostprocessSettingsComponent>(w, EditorCameraEnt);

		//		obtain game camera
		//GameCamera = w->GetWorldComponent<ViewportWorldComponent>()->GetCamera(0);

		// set editor camera
		Manager->GetScene()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(
			0, Manager->GetScene()->GetComponent<CameraComponent>(EditorCameraEnt));

		break;
	}
	case PolyEditor::eEngineState::GAMEPLAY:
	{
		EditorCameraEnt = nullptr;
		// set gameplay camera (probably don't need to because this camera is set when game is restarted)
		//Manager->GetScene()->GetWorldComponent<ViewportWorldComponent>()->SetCamera(0, GameCamera);

		break;
	}
	case PolyEditor::eEngineState::NONE:
	{
		break;
	}
	default:
		throw new std::exception(); // unknown engine state
	}
}



//		viewport functions
//------------------------------------------------------------------------------
void ViewportInspectorWidget::enterEvent(QEvent* e)
{
	setFocus();
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::leaveEvent(QEvent* e)
{
	clearFocus();
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::resizeEvent(QResizeEvent* resizeEvent)
{
	if (!Manager->GetEngine())
		return;

	Poly::ScreenSize screenSize;
	screenSize.Width = resizeEvent->size().width();
	screenSize.Height = resizeEvent->size().height();
	Manager->GetEngine()->ResizeScreen(screenSize);
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::wheelEvent(QWheelEvent* wheelEvent)
{
	if (!Manager->GetEngine())
		return;

	Manager->GetEngine()->UpdateWheelPos(Poly::Vector2i(wheelEvent->delta(), 0));
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::mouseMoveEvent(QMouseEvent* mouseEvent)
{
	if (!Manager->GetEngine())
		return;

	Manager->GetEngine()->UpdateMousePos(Poly::Vector2i(mouseEvent->pos().x(), mouseEvent->pos().y()));
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::mousePressEvent(QMouseEvent* mouseEvent)
{
	if (!Manager->GetEngine())
		return;

	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Manager->GetEngine()->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::LEFT));
		break;

	case Qt::RightButton:
		Manager->GetEngine()->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::RIGHT));
		break;

	case Qt::MiddleButton:
		Manager->GetEngine()->MouseButtonDown(static_cast<Poly::eMouseButton>(Poly::eMouseButton::MIDDLE));
		break;

	default:
		// unsupported key
		break;
	}
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if (!Manager->GetEngine())
		return;

	switch (mouseEvent->button())
	{
	case Qt::LeftButton:
		Manager->GetEngine()->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::LEFT));
		break;

	case Qt::RightButton:
		Manager->GetEngine()->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::RIGHT));
		break;

	case Qt::MiddleButton:
		Manager->GetEngine()->MouseButtonUp(static_cast<Poly::eMouseButton>(Poly::eMouseButton::MIDDLE));
		break;

	default:
		// unsupported key
		break;
	}
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::keyPressEvent(QKeyEvent* keyEvent)
{
	if (!Manager->GetEngine())
		return;

	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
		Manager->GetEngine()->KeyDown(static_cast<Poly::eKey>(SDL_GetScancodeFromKey(QtKeyEventToSDLKeycode((Qt::Key)keyEvent->key()))));
}

//------------------------------------------------------------------------------
void ViewportInspectorWidget::keyReleaseEvent(QKeyEvent* keyEvent)
{
	if (!Manager->GetEngine())
		return;

	if (keyEvent->isAutoRepeat())
		keyEvent->ignore();
	else
		Manager->GetEngine()->KeyUp(static_cast<Poly::eKey>(SDL_GetScancodeFromKey(QtKeyEventToSDLKeycode((Qt::Key)keyEvent->key()))));
}
