#include "PolyEditorPCH.hpp"

#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QtCore/qtimer.h>
#include <QtWidgets/QStyleFactory>

#include <Core.hpp>

EditorApp* gApp = nullptr;

// ---------------------------------------------------------------------------------------------------------
EditorApp::EditorApp(int argc, char *argv[])
	: QApplication(argc, argv)
{
	DockMgr = new DockManager();
	CommandMgr = new CommandManager();
	ProjectMgr = new ProjectManager();
	EngineMgr = new EngineManager();
	InspectorMgr = new InspectorManager(this);
	UndoRedoMgr = new UndoRedoManager();

	ASSERTE(!gApp, "Creating application twice?!");
	gApp = this;

	Ui.Init();
	installEventFilter(&EventFilter);

	setStyle(QStyleFactory::create("Fusion"));

	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::WindowText, Qt::lightGray);

	darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));

	darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
	darkPalette.setColor(QPalette::ButtonText, Qt::white);
	darkPalette.setColor(QPalette::BrightText, Qt::red);
	darkPalette.setColor(QPalette::Link, QColor(225, 159, 4));

	darkPalette.setColor(QPalette::Highlight, QColor(225, 159, 4));
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);

	qApp->setPalette(darkPalette);

	qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
	
	Poly::gConsole.LogInfo("PolyEditor succesfully initialized.");
}
