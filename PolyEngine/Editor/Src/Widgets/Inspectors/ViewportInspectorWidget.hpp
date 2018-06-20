#pragma once

#include <Core.hpp>
#include <ECS/World.hpp>
#include <Editor/IEditor.hpp>

#include "Windows/CustomSDLWindow.hpp"
#include "Rendering/IRenderingDevice.hpp"
#include "Widgets/Inspectors/InspectorWidgetBase.hpp"

using namespace Poly;

class ViewportInspectorWidget : public InspectorWidgetBase, public IEditor
{
	Q_OBJECT

public:
	ViewportInspectorWidget(QWidget* parent);

	// Initializes object connections with other inspectors and inspector manager.
	void InitializeConnections() override;

	// TODO(squares): decide what this method should do
	void Reset() override;

	void UpdateInspector() {}
	void ReloadInspector() {}

	std::unique_ptr<IRenderingDevice> GetRenderingDevice();

	// IEditor functions
	const String& GetAssetsPathConfigPath() override { return AssetsPathConfigPath; }
	void SetAssetsPathConfigPath(const String& path) override { AssetsPathConfigPath = path; }
	void Init() override;
	void Deinit() override;
	virtual const Dynarray<Entity*>& GetSelectedEntities() override { return SelectedEntities; }
	virtual void SetSelectedEntities(Dynarray<Entity*> entities) override { SelectedEntities = std::move(entities); }
	virtual void UpdateInspectors() override;

signals:
	void EntitiesSelectionChanged(Dynarray<Entity*> entities);

private:
	Dynarray<Entity*> SelectedEntities;
	String AssetsPathConfigPath;

	void resizeEvent(QResizeEvent* resizeEvent) override;
	void wheelEvent(QWheelEvent* wheelEvent) override;
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;
	void mousePressEvent(QMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QMouseEvent* mouseREvent) override;
	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

	Entity* Camera = nullptr;

	QWidget* SDLWidget;
	CustomSDLWindow WindowInSDL;
};