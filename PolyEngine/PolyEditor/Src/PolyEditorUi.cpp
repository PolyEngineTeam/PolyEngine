#include "PolyEditorPCH.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <qdockwidget.h>

PolyEditorUi::PolyEditorUi(EditorApp* app)
{
	App = app;
	InitMainWindow();
}

PolyEditorUi::~PolyEditorUi()
{
	delete MainWindow;
}

void PolyEditorUi::InitMainWindow()
{
	MainWindow = new PolyMainWindow();
	Windows.PushBack(std::move(MainWindow));

	MainWindow->setObjectName(QStringLiteral("EditorMainWindowClass"));

	MainWindow->setWindowTitle(QApplication::translate("EditorMainWindowClass", "PolyEditor", Q_NULLPTR));
	MainWindow->resize(1200, 600);;
	MainWindow->setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

	// menu bar
	MenuBar = new QMenuBar(MainWindow);
	MenuBar->setGeometry(QRect(0, 0, 1200, 21));
	MainWindow->setMenuBar(MenuBar);


		// file menu
		FileMenu = new QMenu(MenuBar);
		MenuBar->addAction(FileMenu->menuAction());
		FileMenu->setTitle(QApplication::translate("EditorMainWindowClass", "File", Q_NULLPTR));

			CloseProjectAction = new QAction(MainWindow);
			FileMenu->addAction(CloseProjectAction);
			CloseProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Close Project", Q_NULLPTR));
			QObject::connect(CloseProjectAction, &QAction::triggered, this, &PolyEditorUi::CloseProject);

			QuitAction = new QAction(MainWindow);
			FileMenu->addAction(QuitAction);
			QuitAction->setText(QApplication::translate("EditorMainWindowClass", "Quit", Q_NULLPTR));
			QObject::connect(QuitAction, &QAction::triggered, this, &PolyEditorUi::Quit);


		// edit menu
		EditMenu = new QMenu(MenuBar);
		MenuBar->addAction(EditMenu->menuAction());
		EditMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Edit", Q_NULLPTR));

			UndoAction = new QAction(MainWindow);
			EditMenu->addAction(UndoAction);
			UndoAction->setText(QApplication::translate("EditorMainWindowClass", "Undo", Q_NULLPTR));
			QObject::connect(UndoAction, &QAction::triggered, this, &PolyEditorUi::Undo);

			RedoAction = new QAction(MainWindow);
			EditMenu->addAction(RedoAction);
			RedoAction->setText(QApplication::translate("EditorMainWindowClass", "Redo", Q_NULLPTR));
			QObject::connect(RedoAction, &QAction::triggered, this, &PolyEditorUi::Redo);


		// view menu
		ViewMenu = new QMenu(MenuBar);
		MenuBar->addAction(ViewMenu->menuAction());
		ViewMenu->setTitle(QApplication::translate("EditorMainWindowClass", "View", Q_NULLPTR));

			AddWindowAction = new QAction(MainWindow);
			ViewMenu->addAction(AddWindowAction);
			AddWindowAction->setText(QApplication::translate("EditorMainWindowClass", "Add Window", Q_NULLPTR));
			QObject::connect(AddWindowAction, &QAction::triggered, this, &PolyEditorUi::AddWindow);


		// build menu
		BuildMenu = new QMenu(MenuBar);
		MenuBar->addAction(BuildMenu->menuAction());
		BuildMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Build", Q_NULLPTR));

			BuildAction = new QAction(MainWindow);
			BuildMenu->addAction(BuildAction);
			BuildAction->setText(QApplication::translate("EditorMainWindowClass", "Build", Q_NULLPTR));
			QObject::connect(BuildAction, &QAction::triggered, this, &PolyEditorUi::Build);


		// help menu
		HelpMenu = new QMenu(MenuBar);
		MenuBar->addAction(HelpMenu->menuAction());
		HelpMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Help", Q_NULLPTR));

			ContactUsAction = new QAction(MainWindow);
			HelpMenu->addAction(ContactUsAction);
			ContactUsAction->setText(QApplication::translate("EditorMainWindowClass", "Contact Us", Q_NULLPTR));
			QObject::connect(ContactUsAction, &QAction::triggered, this, &PolyEditorUi::ContactUs);



	// hardcoded initialization of several widgets.
	PolyConsoleWidget* consoleWidget = new PolyConsoleWidget("Assets Explorer");
	consoleWidget->Dock(Qt::DockWidgetArea::TopDockWidgetArea, MainWindow);

	PolyViewportWidget* viewportWidget = new PolyViewportWidget("Viewport");
	viewportWidget->Dock(Qt::DockWidgetArea::TopDockWidgetArea, MainWindow);

	consoleWidget = new PolyConsoleWidget("Object Properties");
	consoleWidget->Dock(Qt::DockWidgetArea::TopDockWidgetArea, MainWindow);

	consoleWidget = new PolyConsoleWidget("Console");
	consoleWidget->Dock(Qt::DockWidgetArea::BottomDockWidgetArea, MainWindow);

	MainWindow->show();
}

void PolyEditorUi::InitDockWidgets()
{
}

void PolyEditorUi::CloseProject()
{
}

void PolyEditorUi::Quit()
{
}

void PolyEditorUi::Undo()
{
}

void PolyEditorUi::Redo()
{
}

void PolyEditorUi::AddWindow()
{
	PolyWindow* window = new PolyWindow();
	//Windows.PushBack(window);
	window->show();
}

void PolyEditorUi::Build()
{
}

void PolyEditorUi::ContactUs()
{
}
