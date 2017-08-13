#include "PolyEditorPCH.hpp"
#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include <QFileSystemModel>
#include <QFileSystemWatcher>

#include <Core.hpp>

EditorMainWindow::EditorMainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::EditorMainWindowClass)
{
	freopen("console.log", "w", stdout);
	setvbuf(stdout, NULL, _IONBF, 0);
	ui->setupUi(this);

	//for (int i = 0; i<100; ++i)
	//	ui->consoleOutput->append(QString("dupa dupa\n dupa"));

	QDir dir("C:\\");
	QFileSystemModel* model = new QFileSystemModel();
	model->setRootPath(dir.absolutePath());
	ui->assetsTreeView->setModel(model);
	ui->assetsTreeView->setRootIndex(model->index(dir.absolutePath()));

	QFileSystemWatcher* watcher = new QFileSystemWatcher(this);
	watcher->addPath("console.log");
	QObject::connect(watcher, &QFileSystemWatcher::fileChanged, [&](const QString& path) 
	{ 
		ui->consoleOutput->setText(path);
	});

	QObject::connect(ui->actionQuit, &QAction::triggered, [](bool checked) { Poly::gConsole.LogError("test3");/*QApplication::quit();*/ });
	

	Poly::gConsole.LogError("test1");
	Poly::gConsole.LogError("test2");
}

EditorMainWindow::~EditorMainWindow()
{
	delete ui;
}
