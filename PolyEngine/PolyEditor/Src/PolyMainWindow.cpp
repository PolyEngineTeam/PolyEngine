#include "PolyEditorPCH.hpp"

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QHeaderView>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QDockWidget>

PolyMainWindow::PolyMainWindow(QWidget* parent) :
	PolyWindow(parent)
{
	SetupUi();

	QDockWidget* consoleDockWidget = new QDockWidget();
	PolyConsoleWidget* consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Assets Explorer");
	consoleDockWidget->setWidget(consoleWidget);
	AddWidget(consoleDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	QDockWidget* viewportDockWidget = new QDockWidget();
	// when you won't pass this argument to the constructor you won't be able to dock this widget
	PolyViewportWidget* viewportWidget = new PolyViewportWidget(viewportDockWidget); 
	viewportWidget->InitializeViewport();
	viewportWidget->setFocusPolicy(Qt::ClickFocus);
	viewportDockWidget->setWindowTitle("Viewport Widget");
	viewportDockWidget->setWidget(viewportWidget);
	AddWidget(viewportDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	consoleDockWidget = new QDockWidget();
	consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Object Properties");
	consoleDockWidget->setWidget(consoleWidget);
	AddWidget(consoleDockWidget, Qt::DockWidgetArea::TopDockWidgetArea, Qt::Orientation::Horizontal);

	consoleDockWidget = new QDockWidget();
	consoleWidget = new PolyConsoleWidget();
	consoleWidget->setFocusPolicy(Qt::ClickFocus);
	consoleWidget->setEnabled(true);
	consoleWidget->setReadOnly(true);
	consoleDockWidget->setWindowTitle("Console");
	consoleDockWidget->setWidget(consoleWidget);
	AddWidget(consoleDockWidget, Qt::DockWidgetArea::BottomDockWidgetArea, Qt::Orientation::Horizontal);
}

PolyMainWindow::~PolyMainWindow()
{
}

void PolyMainWindow::SetupUi()
{
	if (objectName().isEmpty())
		setObjectName(QStringLiteral("EditorMainWindowClass"));
	resize(1200, 700);
	setAnimated(false);
	setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);
	actionQuit = new QAction(this);
	actionQuit->setObjectName(QStringLiteral("actionQuit"));
	menuBar = new QMenuBar(this);
	menuBar->setObjectName(QStringLiteral("menuBar"));
	menuBar->setGeometry(QRect(0, 0, 1200, 21));
	menuFile = new QMenu(menuBar);
	menuFile->setObjectName(QStringLiteral("menuFile"));
	menuEdit = new QMenu(menuBar);
	menuEdit->setObjectName(QStringLiteral("menuEdit"));
	menuHelp = new QMenu(menuBar);
	menuHelp->setObjectName(QStringLiteral("menuHelp"));
	setMenuBar(menuBar);

	menuBar->addAction(menuFile->menuAction());
	menuBar->addAction(menuEdit->menuAction());
	menuBar->addAction(menuHelp->menuAction());
	menuFile->addAction(actionQuit);

	RetranslateUi(this);

	QMetaObject::connectSlotsByName(this);
}

void PolyMainWindow::RetranslateUi(QMainWindow *EditorMainWindowClass)
{
	EditorMainWindowClass->setWindowTitle(QApplication::translate("EditorMainWindowClass", "PolyEditor", Q_NULLPTR));
	actionQuit->setText(QApplication::translate("EditorMainWindowClass", "Quit", Q_NULLPTR));
	menuFile->setTitle(QApplication::translate("EditorMainWindowClass", "File", Q_NULLPTR));
	menuEdit->setTitle(QApplication::translate("EditorMainWindowClass", "Edit", Q_NULLPTR));
	menuHelp->setTitle(QApplication::translate("EditorMainWindowClass", "Help", Q_NULLPTR));
}
