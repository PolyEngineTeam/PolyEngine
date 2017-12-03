#include "PolyEditorPCH.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>

PolyMainWindowUi::PolyMainWindowUi(PolyMainWindow* window)
{
	window->setObjectName(QStringLiteral("EditorMainWindowClass"));

	window->setWindowTitle(QApplication::translate("EditorMainWindowClass", "PolyEditor", Q_NULLPTR));
	window->resize(1200, 600);;
	window->setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);

	InitMenuBar(window);
}

PolyMainWindowUi::~PolyMainWindowUi()
{
}

void PolyMainWindowUi::InitMenuBar(PolyMainWindow* window)
{
	// menu bar
	MenuBar = new QMenuBar(window);
	MenuBar->setGeometry(QRect(0, 0, 1200, 21));
	window->setMenuBar(MenuBar);


		// file menu
		FileMenu = new QMenu(MenuBar);
		MenuBar->addAction(FileMenu->menuAction());
		FileMenu->setTitle(QApplication::translate("EditorMainWindowClass", "File", Q_NULLPTR));

			CloseProjectAction = new QAction(window);
			FileMenu->addAction(CloseProjectAction);
			CloseProjectAction->setText(QApplication::translate("EditorMainWindowClass", "Close Project", Q_NULLPTR));
			QObject::connect(CloseProjectAction, &QAction::triggered, window, &PolyMainWindow::CloseProject);

			QuitAction = new QAction(window);
			FileMenu->addAction(QuitAction);
			QuitAction->setText(QApplication::translate("EditorMainWindowClass", "Quit", Q_NULLPTR));
			QObject::connect(QuitAction, &QAction::triggered, window, &PolyMainWindow::Quit);


		// edit menu
		EditMenu = new QMenu(MenuBar);
		MenuBar->addAction(EditMenu->menuAction());
		EditMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Edit", Q_NULLPTR));

			UndoAction = new QAction(window);
			EditMenu->addAction(UndoAction);
			UndoAction->setText(QApplication::translate("EditorMainWindowClass", "Undo", Q_NULLPTR));
			QObject::connect(UndoAction, &QAction::triggered, window, &PolyMainWindow::Undo);

			RedoAction = new QAction(window);
			EditMenu->addAction(RedoAction);
			RedoAction->setText(QApplication::translate("EditorMainWindowClass", "Redo", Q_NULLPTR));
			QObject::connect(RedoAction, &QAction::triggered, window, &PolyMainWindow::Redo);


		// view menu
		ViewMenu = new QMenu(MenuBar);
		MenuBar->addAction(ViewMenu->menuAction());
		ViewMenu->setTitle(QApplication::translate("EditorMainWindowClass", "View", Q_NULLPTR));

			AddWindowAction = new QAction(window);
			ViewMenu->addAction(AddWindowAction);
			AddWindowAction->setText(QApplication::translate("EditorMainWindowClass", "Add Window", Q_NULLPTR));
			QObject::connect(AddWindowAction, &QAction::triggered, window, &PolyMainWindow::AddWindow);


		// build menu
		BuildMenu = new QMenu(MenuBar);
		MenuBar->addAction(BuildMenu->menuAction());
		BuildMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Build", Q_NULLPTR));

			BuildAction = new QAction(window);
			BuildMenu->addAction(BuildAction);
			BuildAction->setText(QApplication::translate("EditorMainWindowClass", "Build", Q_NULLPTR));
			QObject::connect(BuildAction, &QAction::triggered, window, &PolyMainWindow::Build);


		// help menu
		HelpMenu = new QMenu(MenuBar);
		MenuBar->addAction(HelpMenu->menuAction());
		HelpMenu->setTitle(QApplication::translate("EditorMainWindowClass", "Help", Q_NULLPTR));

			ContactUsAction = new QAction(window);
			HelpMenu->addAction(ContactUsAction);
			ContactUsAction->setText(QApplication::translate("EditorMainWindowClass", "Contact Us", Q_NULLPTR));
			QObject::connect(ContactUsAction, &QAction::triggered, window, &PolyMainWindow::ContactUs);
}

void PolyMainWindowUi::InitToolbars(PolyMainWindow* window)
{
}

void PolyMainWindowUi::InitDockWidgets(PolyMainWindow* window)
{
}
