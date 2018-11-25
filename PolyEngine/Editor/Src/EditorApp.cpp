#include <PolyEditorPCH.hpp>

#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTimer>
#include <Utils/Logger.hpp>
#include <QtWidgets/QStyleFactory>

EditorApp* gApp = nullptr;

std::unique_ptr<ProjectConfig> GetProjectConfigFromArgs(int argc, char *argv[])
{
	constexpr unsigned PROJ_JSON_FILE_PATH_INDEX = 1;
	constexpr unsigned CONFIGURATION_INDEX = 2;

	const auto modeName = String(argv[CONFIGURATION_INDEX]);

	ProjectConfig::eConfiguration configuration;
	if (modeName == "Debug")
		configuration = ProjectConfig::eConfiguration::DEBUG;
	else
		ASSERTE(false, "Wrong argument passed.");

	return std::make_unique<ProjectConfig>(argv[PROJ_JSON_FILE_PATH_INDEX], configuration);
}

// C:/Users/pmoscicki/Documents/PolyEngine/PolyEngineExamples/SGJ2018Game\SGJ18.proj.json 
// Debug

// ---------------------------------------------------------------------------------------------------------
EditorApp::EditorApp(int argc, char *argv[])
	: QApplication(argc, argv)
{
	ASSERTE(!gApp, "Creating application twice?!");
	gApp = this;

	DockMgr = new DockManager();
	CmdMgr = new CmdManager();
	ProjectMgr = new ProjectManager(GetProjectConfigFromArgs(argc, argv));
	InspectorMgr = new InspectorManager(this);
	CommandMgr = new CommandManager();


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

// ---------------------------------------------------------------------------------------------------------
EditorApp::~EditorApp()
{
}
