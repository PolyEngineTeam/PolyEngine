#include <Defines.hpp>

// Qt includes
SILENCE_MSVC_WARNING(4244, "")
SILENCE_MSVC_WARNING(4800, "")
#include <QKeyEvent>
#include <QEvent>
#include <QApplication>
#include <QTimer>
#include <QMenu>
#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qboxlayout.h>
#include <QDockWidget>
#include <QMainWindow>
#include <QtWidgets/qwidget.h>
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
#include "Managers/InspectorManager.hpp"

// configs
#include "Configs/ProjectConfig.hpp"

// windows
#include "Windows/PolyWindow.hpp"
#include "Windows/PolyMainWindow.hpp"
#include "Windows/PolyDockWindow.hpp"

// widgets
#include "Widgets/PolyWidget.hpp"

#include "Widgets/LoggerWidget.hpp"

#include "Widgets/Inspectors/WorldInspectorWidget.hpp"
#include "Widgets/Inspectors/WorldComponentsInspectorWidget.hpp"
#include "Widgets/Inspectors/ResourceInspectorWidget.hpp"
#include "Widgets/Inspectors/ViewportInspectorWidget.hpp"
#include "Widgets/Inspectors/EntityInspectorWidget.hpp"
#include "Widgets/Inspectors/RTTIInspectorWidget.hpp"

// containers
#include "Containers/SectionContainer.hpp"

// controls
#include "Controls/ControlBase.hpp"

#include "Controls/CoreControls/PlaceHolderControl.hpp"
#include "Controls/CoreControls/BoolControl.hpp"
#include "Controls/CoreControls/NumberControl.hpp"
#include "Controls/CoreControls/EnumControl.hpp"
#include "Controls/CoreControls/StringControl.hpp"

#include "Controls/EngineControls/ResourceControl.hpp"
#include "Controls/EngineControls/TransformControl.hpp"

// dialogs
#include "Dialogs/CreateProjectDialog.hpp"

#include "Dialogs/Inspectors/Entities/AddEntityDialog.hpp"
#include "Dialogs/Inspectors/Entities/ChangeParentDialog.hpp"

#include "Dialogs/Inspectors/Components/AddComponentDialog.hpp"
#include "Dialogs/Inspectors/Components/RemoveComponentDialog.hpp"

// systems
#include "Systems/Gizmo/GizmoSystem.hpp"
#include "Systems/Gizmo/GizmoWorldComponent.hpp"

#include "Systems/Camera/EditorCameraMovementSystem.hpp"
#include "Systems/Camera/EditorCameraMovementComponent.hpp"

// engine
#include <Engine.hpp>