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
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "GameplayViewportWidget.hpp"

QT_BEGIN_NAMESPACE

class Ui_EditorMainWindowClass
{
public:
    QAction *actionQuit;
    QWidget *centralWidget;
    QGridLayout *gridLayout_4;
    GameplayViewportWidget *viewportWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *consoleDockWidget;
    QWidget *dockWidgetContents;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *consoleOutput;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents_2;
    QGridLayout *gridLayout_2;
    QTreeView *assetsTreeView;
    QDockWidget *dockWidget_2;
    QWidget *dockWidgetContents_3;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QProgressBar *progressBar_2;
    QLabel *label;
    QProgressBar *progressBar;

    void setupUi(QMainWindow *EditorMainWindowClass)
    {
        if (EditorMainWindowClass->objectName().isEmpty())
            EditorMainWindowClass->setObjectName(QStringLiteral("EditorMainWindowClass"));
        EditorMainWindowClass->resize(1280, 720);
        actionQuit = new QAction(EditorMainWindowClass);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        centralWidget = new QWidget(EditorMainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_4 = new QGridLayout(centralWidget);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        viewportWidget = new GameplayViewportWidget(centralWidget);
        viewportWidget->setObjectName(QStringLiteral("viewportWidget"));

        gridLayout_4->addWidget(viewportWidget, 0, 0, 1, 1);

        EditorMainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(EditorMainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1280, 31));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        EditorMainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(EditorMainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        EditorMainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(EditorMainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        EditorMainWindowClass->setStatusBar(statusBar);
        consoleDockWidget = new QDockWidget(EditorMainWindowClass);
        consoleDockWidget->setObjectName(QStringLiteral("consoleDockWidget"));
        consoleDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(dockWidgetContents->sizePolicy().hasHeightForWidth());
        dockWidgetContents->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(dockWidgetContents);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetMaximumSize);
        consoleOutput = new QTextEdit(dockWidgetContents);
        consoleOutput->setObjectName(QStringLiteral("consoleOutput"));
        consoleOutput->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(consoleOutput->sizePolicy().hasHeightForWidth());
        consoleOutput->setSizePolicy(sizePolicy1);
        consoleOutput->setMinimumSize(QSize(0, 0));
        consoleOutput->setSizeIncrement(QSize(0, 0));
        consoleOutput->setReadOnly(true);

        verticalLayout_2->addWidget(consoleOutput);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);

        consoleDockWidget->setWidget(dockWidgetContents);
        EditorMainWindowClass->addDockWidget(static_cast<Qt::DockWidgetArea>(8), consoleDockWidget);
        dockWidget = new QDockWidget(EditorMainWindowClass);
        dockWidget->setObjectName(QStringLiteral("dockWidget"));
        dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea|Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QStringLiteral("dockWidgetContents_2"));
        gridLayout_2 = new QGridLayout(dockWidgetContents_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        assetsTreeView = new QTreeView(dockWidgetContents_2);
        assetsTreeView->setObjectName(QStringLiteral("assetsTreeView"));

        gridLayout_2->addWidget(assetsTreeView, 0, 0, 1, 1);

        dockWidget->setWidget(dockWidgetContents_2);
        EditorMainWindowClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dockWidget);
        dockWidget_2 = new QDockWidget(EditorMainWindowClass);
        dockWidget_2->setObjectName(QStringLiteral("dockWidget_2"));
        dockWidget_2->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents_3 = new QWidget();
        dockWidgetContents_3->setObjectName(QStringLiteral("dockWidgetContents_3"));
        gridLayout_3 = new QGridLayout(dockWidgetContents_3);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_2 = new QLabel(dockWidgetContents_3);
        label_2->setObjectName(QStringLiteral("label_2"));

        verticalLayout->addWidget(label_2);

        progressBar_2 = new QProgressBar(dockWidgetContents_3);
        progressBar_2->setObjectName(QStringLiteral("progressBar_2"));
        progressBar_2->setValue(24);

        verticalLayout->addWidget(progressBar_2);

        label = new QLabel(dockWidgetContents_3);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        progressBar = new QProgressBar(dockWidgetContents_3);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setValue(24);

        verticalLayout->addWidget(progressBar);


        gridLayout_3->addLayout(verticalLayout, 0, 0, 1, 1);

        dockWidget_2->setWidget(dockWidgetContents_3);
        EditorMainWindowClass->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget_2);

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
        consoleDockWidget->setWindowTitle(QApplication::translate("EditorMainWindowClass", "Console", Q_NULLPTR));
        dockWidget->setWindowTitle(QApplication::translate("EditorMainWindowClass", "Assets explorer", Q_NULLPTR));
        dockWidget_2->setWindowTitle(QApplication::translate("EditorMainWindowClass", "Profiler", Q_NULLPTR));
        label_2->setText(QApplication::translate("EditorMainWindowClass", "Performance 1", Q_NULLPTR));
        label->setText(QApplication::translate("EditorMainWindowClass", "Performance 2", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class EditorMainWindowClass: public Ui_EditorMainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITORMAINWINDOW_H
