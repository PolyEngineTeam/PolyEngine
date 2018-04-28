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
#include "EditorApp.hpp"
#include "EditorUi.hpp"
#include "GlobalEventFilter.hpp"

#include "Managers/DockManager.hpp"
#include "Managers/ProjectManager.hpp"
#include "Managers/EngineManager.hpp"
#include "Managers/CommandManager.hpp"
#include "Managers/EditManager.hpp"

// configs
#include "Configs/ProjectConfig.hpp"

// windows
#include "Windows/PolyWindow.hpp"
#include "Windows/PolyMainWindow.hpp"
#include "Windows/PolyDockWindow.hpp"

// controls
#include "Controls/RTTIViewControl.hpp"
#include "Controls/SectionControl.hpp"

#include "Controls/ControlBase.hpp"
#include "Controls/BoolControl.hpp"
#include "Controls/NumberControl.hpp"
#include "Controls/EnumControl.hpp"
#include "Controls/StringControl.hpp"

// widgets
#include "Widgets/ViewportWidget.hpp"
#include "Widgets/LoggerWidget.hpp"
#include "Widgets/EntityManagerWidget.hpp"
#include "Widgets/WorldExplorerWidget.hpp"
#include "Widgets/RTTIViewerWidget.hpp"

// dialogs
#include "Dialogs/CreateProjectDialog.hpp"

// engine
#include <Engine.hpp>