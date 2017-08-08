#include "PolyEditorPCH.hpp"
#include "EditorMainWindow.hpp"
#include "ui_EditorMainWindow.h"
#include <QtWidgets/QFileSystemModel>

EditorMainWindow::EditorMainWindow(QWidget *parent)
	: QMainWindow(parent),
	ui(new Ui::EditorMainWindowClass)
{
	ui->setupUi(this);

	for (int i = 0; i<100; ++i)
		ui->consoleOutput->append(QString("dupa dupa\n dupa"));

	QDir dir("C:\\");
	QFileSystemModel* model = new QFileSystemModel();
	model->setRootPath(dir.absolutePath());
	ui->assetsTreeView->setModel(model);
	ui->assetsTreeView->setRootIndex(model->index(dir.absolutePath()));

	QObject::connect(ui->actionQuit, &QAction::triggered, [](bool checked) {QApplication::quit(); });
}

EditorMainWindow::~EditorMainWindow()
{
	delete ui;
}
