#pragma once

#include <Core.hpp>
#include <ECS/World.hpp>
#include <Editor/IEditor.hpp>

#include "Windows/CustomSDLWindow.hpp"
#include "Rendering/IRenderingDevice.hpp"
#include "Widgets/Inspectors/InspectorWidgetBase.hpp"
#include "Managers/InspectorManager.hpp"

namespace Poly
{
	class CameraComponent;
}

using namespace Poly;

class ViewportInspectorWidget : public InspectorWidgetBase, public IEditor
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
	String GetAssetsPathConfigPath() override;

	// This function is similar to iGame::Init function; it is called after game initialization.
	void Init() override;

	// Same as above; it is called before game deinitialization.
	void Deinit() override;

	// If any system needs currently selected entities list it should use this function.
	const Dynarray<Entity*>& GetSelectedEntities() override { return SelectedEntities; }

	// If system wants to set new list of selected entities it hould call this one.
	void SetSelectedEntities(Dynarray<Entity*> entities) override;

	// Can be called from systems to update inspectors content 
	void UpdateInspectors() override;

	void SetEngineState(eEngineState state) override;
	eEngineState GetEngineState() override { return EngineState; }

public slots:
	// engine
	void StateChanged();

private:
	Entity* EditorCameraEnt = nullptr;
	CameraComponent* GameCamera = nullptr;

	QWidget* SDLWidget;
	CustomSDLWindow WindowInSDL;

	void resizeEvent(QResizeEvent* resizeEvent) override;
	void wheelEvent(QWheelEvent* wheelEvent) override;
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;
	void mousePressEvent(QMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QMouseEvent* mouseREvent) override;
	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;
};