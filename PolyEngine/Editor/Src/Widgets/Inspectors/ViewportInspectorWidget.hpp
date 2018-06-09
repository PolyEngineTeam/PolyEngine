#pragma once

#include <Core.hpp>
#include <ECS/World.hpp>
#include <Editor/IEditor.hpp>

#include "Windows/CustomSDLWindow.hpp"
#include "Rendering/IRenderingDevice.hpp"
#include "Widgets/PolyWidget.hpp"

using namespace Poly;

class ViewportInspectorWidget : public PolyWidget, public IEditor
{
public:
	ViewportInspectorWidget(QWidget* parent);

	// inspector functions
	void SetObject(Entity* entity);

	// viewport functions
	std::unique_ptr<IRenderingDevice> GetRenderingDevice();

	// IEditor functions
	const String& GetAssetsPathConfigPath() override { return AssetsPathConfigPath; }
	void SetAssetsPathConfigPath(const String& path) override { AssetsPathConfigPath = path; }
	void Init() override;
	void Deinit() override;
	virtual const Dynarray<UniqueID>& GetSelectedEntitiesIds() override;
	virtual void SetSelectedEntitiesIds(Dynarray<UniqueID>) override;
	virtual void UpdateInspectors() override;

private:
	Entity* Entities;
	String AssetsPathConfigPath;
	eEngineState EngineState = eEngineState::NONE;

	void resizeEvent(QResizeEvent* resizeEvent) override;
	void wheelEvent(QWheelEvent* wheelEvent) override;
	void mouseMoveEvent(QMouseEvent* mouseEvent) override;
	void mousePressEvent(QMouseEvent* mouseEvent) override;
	void mouseReleaseEvent(QMouseEvent* mouseREvent) override;
	void keyPressEvent(QKeyEvent* keyEvent) override;
	void keyReleaseEvent(QKeyEvent* keyEvent) override;

	CustomSDLWindow WindowInSDL;
};