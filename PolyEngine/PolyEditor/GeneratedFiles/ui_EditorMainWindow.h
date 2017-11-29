/********************************************************************************
** Form generated from reading UI file 'EditorMainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITORMAINWINDOW_H
#define UI_EDITORMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>

QT_BEGIN_NAMESPACE

class Ui_EditorMainWindowClass
{
public:
    QAction *actionQuit;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;

    void setupUi(QMainWindow *EditorMainWindowClass)
    {
        if (EditorMainWindowClass->objectName().isEmpty())
            EditorMainWindowClass->setObjectName(QStringLiteral("EditorMainWindowClass"));
        EditorMainWindowClass->resize(1200, 700);
        EditorMainWindowClass->setAnimated(false);
        EditorMainWindowClass->setDockOptions(QMainWindow::AllowNestedDocks|QMainWindow::AllowTabbedDocks|QMainWindow::GroupedDragging);
        actionQuit = new QAction(EditorMainWindowClass);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        menuBar = new QMenuBar(EditorMainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1200, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        EditorMainWindowClass->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionQuit);

        retranslateUi(EditorMainWindowClass);

        QMetaObject::connectSlotsByName(EditorMainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *EditorMainWindowClass)
    {
        EditorMainWindowClass->setWindowTitle(QApplication::translate("EditorMainWindowClass", "PolyEditor", Q_NULLPTR));
        actionQuit->setText(QApplication::translate("EditorMainWindowClass", "Quit", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("EditorMainWindowClass", "File", Q_NULLPTR));
        menuEdit->setTitle(QApplication::translate("EditorMainWindowClass", "Edit", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("EditorMainWindowClass", "Help", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EditorMainWindowClass: public Ui_EditorMainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITORMAINWINDOW_H
