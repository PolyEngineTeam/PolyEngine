#include <PolyEditorPCH.hpp>

int main(int argc, char *argv[])
{
	/*
	QSurfaceFormat format;
	format.setDepthBufferSize(32);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(format);
	*/

	EditorApp app(argc, argv);
	auto exitCode = app.exec();

	// Clean managers, otherwise their constructors crash due to messed deinitialization order across shared libraries
	Poly::ComponentManager::Get().Clear();
	Poly::RTTI::Impl::TypeManager::Get().Clear();

	return exitCode;
}
