#include "PolyEditorPCH.hpp"
#include "EditorApp.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QSurfaceFormat format;
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(32);
	format.setStencilBufferSize(8);
	format.setAlphaBufferSize(8);
	format.setRedBufferSize(8);
	format.setBlueBufferSize(8);
	format.setGreenBufferSize(8);
	format.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(format);

	QApplication a(argc, argv);
	
	EditorApp w;
	w.show();
	return a.exec();
}
