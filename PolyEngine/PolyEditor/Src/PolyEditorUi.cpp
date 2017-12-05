#include "PolyEditorPCH.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <qdockwidget.h>

PolyEditorUi::PolyEditorUi()
{
	InitMainWindow();
}

PolyEditorUi::~PolyEditorUi()
{
	delete MainWindow;
}

void PolyEditorUi::InitMainWindow()
{
	MainWindow = new PolyMainWindow();

	MainWindow->setObjectName(QStringLiteral("EditorMainWindowClass"));

	MainWindow->setWindowTitle(QApplication::translate("EditorMainWindowClass", "PolyEditor", Q_NULLPTR));
	MainWindow->resize(1200, 600);;
	MainWindow->setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);

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
	QDockWidget* consoleDockWidget = new QDockWidget();
	PolyConsoleWidget* consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Assets Explorer");
	consoleDockWidget->setWidget(consoleWidget);
	MainWindow->AddWidget(consoleDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	QDockWidget* viewportDockWidget = new QDockWidget();
	// when you won't pass this argument to the constructor you won't be able to dock this widget
	PolyViewportWidget* viewportWidget = new PolyViewportWidget(viewportDockWidget);
	viewportDockWidget->setWindowTitle("Viewport Widget");
	viewportDockWidget->setWidget(viewportWidget);
	MainWindow->AddWidget(viewportDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	consoleDockWidget = new QDockWidget();
	consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Object Properties");
	consoleDockWidget->setWidget(consoleWidget);
	MainWindow->AddWidget(consoleDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	consoleDockWidget = new QDockWidget();
	consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Console");
	consoleDockWidget->setWidget(consoleWidget);
	MainWindow->AddWidget(consoleDockWidget, Qt::DockWidgetArea::BottomDockWidgetArea, Qt::Orientation::Horizontal);

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

}

void PolyEditorUi::Build()
{
}

void PolyEditorUi::ContactUs()
{
}
