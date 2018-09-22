#include <Defines.hpp>

// Qt includes
SILENCE_MSVC_WARNING(4244, "")
SILENCE_MSVC_WARNING(4800, "")
#include <QKeyEvent>
#include <QEvent>
#include <QApplication>
#include <QTimer>
#include <QMenu>
#include <QTextEdit>
#include <QBoxLayout>
#include <QDockWidget>
#include <QMainWindow>
#include <QWidget>
UNSILENCE_MSVC_WARNING()
UNSILENCE_MSVC_WARNING()

// application
#include <EditorApp.hpp>
#include <EditorUi.hpp>
#include <GlobalEventFilter.hpp>
#include <DockManager.hpp>

// windows
#include <PolyWindow.hpp>
#include <PolyMainWindow.hpp>

// widgets base
#include <PolyWidget.hpp>

// widgets
#include <ViewportWidget.hpp>
#include <LoggerWidget.hpp>

// engine
#include <Engine.hpp>