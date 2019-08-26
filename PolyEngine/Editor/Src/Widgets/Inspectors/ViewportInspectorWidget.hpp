#pragma once

#include <pe/Defines.hpp>
#include <ECS/Scene.hpp>

#include "Windows/CustomSDLWindow.hpp"
#include "Rendering/IRenderingDevice.hpp"
#include "Widgets/Inspectors/InspectorWidgetBase.hpp"
#include "IEditor.hpp"

namespace Poly
{
	class CameraComponent;
}

using namespace Poly;

class InspectorManager;

class ViewportInspectorWidget : public InspectorWidgetBase, public PolyEditor::IEditor
{
	Q_OBJECT

public:
	ViewportInspectorWidget(QWidget* parent, InspectorManager* mgr);

	void InitializeConnections() override;
	void Reset() override {}

	std::unique_ptr<IRenderingDevice> GetRenderingDevice();

	//		IEditor functions
	// When engine checks if there is iEditor object registered it will obtain assets path from here.
	// It is important because when we run a game it is executed from engine folder not game folder.
	::pe::core::storage::String GetAssetsPathConfigPath() override;

	// This function is similar to iGame::Init function; it is called after game initialization.
	void OnGameInit() override;

	// Same as above; it is called before game deinitialization.
	void OnGameDeinit() override;

	// If any system needs currently selected entities list it should use this function.
	std::vector<Entity*> GetSelectedEntities() override;

	// If system wants to set new list of selected entities it hould call this one.
	void SetSelectedEntities(std::vector<Entity*> entities) override;

	// Can be called from systems to update inspectors content 
	void UpdateInspectors() override;

	void SetEngineState(PolyEditor::eEngineState state) override;
	PolyEditor::eEngineState GetEngineState() override;

public:
	// handling output events from Engine to SDL
	void HandleEngineOutputEvents(Poly::OutputQueue& OutputEventsQueue);
	
	Qt::CursorShape GetCursorType(Poly::eMouseCursorType cursorType);

public slots:
	// engine
	void StateChanged();

private:
	Entity* EditorCameraEnt = nullptr;
	CameraComponent* GameCamera = nullptr;

	QWidget* SDLWidget;
	CustomSDLWindow WindowInSDL;

	void enterEvent(QEvent *e) override;
	void leaveEvent(QEvent *e) override;
	void resizeEvent(QResizeEvent* resizeEvent) override;
	void wheelEvent(QWheelEvent* wheelEvent) override;
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;
	void mousePressEvent(QMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QMouseEvent* mouseREvent) override;
	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;
};