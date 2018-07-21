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
	CommandMgr = new CmdManager();
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
	darkPalette.setColor(QPalette::Window, QColor(45, 45, 45));
	darkPalette.setColor(QPalette::WindowText, Qt::lightGray);
	darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
	darkPalette.setColor(QPalette::AlternateBase, QColor(255, 0, 0)); // no idea what's this
	darkPalette.setColor(QPalette::ToolTipBase, QColor(40, 40, 40));
	darkPalette.setColor(QPalette::ToolTipText, Qt::lightGray);
	darkPalette.setColor(QPalette::Text, Qt::lightGray);
	darkPalette.setColor(QPalette::Button, QColor(55, 55, 55));
	darkPalette.setColor(QPalette::ButtonText, Qt::lightGray);
	darkPalette.setColor(QPalette::BrightText, QColor(255, 0, 0)); // no idea what's this

	darkPalette.setColor(QPalette::Highlight, QColor(225, 159, 4));
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);

	darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
	darkPalette.setColor(QPalette::LinkVisited, QColor(150, 0, 150));

	qApp->setPalette(darkPalette);

	qApp->setStyleSheet("QToolTip { color: #000000; background-color: #e19f04; border: 1px solid black; } \
						 QDockWidget { font-family: \"Bennet\" } \
						 QWidget { font-family: \"Consolas\" } \
						 QComboBox:disabled { color: #c0c0c0; background-color: #242424; } \
						 QComboBox { color: #c0c0c0; background-color: #323232; } \
						 QLineEdit:disabled { color: #c0c0c0; background-color: #323232; } \
						 QLineEdit { color: #c0c0c0; background-color: #404040; }");
	
	Poly::gConsole.LogInfo("PolyEditor successfully initialized.");
}
