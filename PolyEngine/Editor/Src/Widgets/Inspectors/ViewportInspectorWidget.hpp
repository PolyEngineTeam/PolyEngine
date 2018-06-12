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

	void SetObject(Entity* entity);

	// viewport functions
	std::unique_ptr<IRenderingDevice> GetRenderingDevice();

	// IEditor functions
	const String& GetAssetsPathConfigPath() override { return AssetsPathConfigPath; }
	void SetAssetsPathConfigPath(const String& path) override { AssetsPathConfigPath = path; }
	void Init() override;
	void Deinit() override;
	virtual const Dynarray<Entity*>& GetSelectedEntities() override { return Entities; }
	virtual void SetSelectedEntities(Dynarray<Entity*> entities) override { Entities = std::move(entities); }
	virtual void UpdateInspectors() override;

private:
	Dynarray<Entity*> Entities;
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

private slots:
	void Reset();
};