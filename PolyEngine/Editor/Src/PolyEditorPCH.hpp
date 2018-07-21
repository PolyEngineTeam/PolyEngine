#include <Defines.hpp>

// Qt includes
SILENCE_MSVC_WARNING(4244, "")
SILENCE_MSVC_WARNING(4800, "")
#include <QKeyEvent>
#include <QtCore/qcoreevent.h>
#include <QApplication>
#include <QtCore/qtimer.h>
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
#include "Managers/CmdManager.hpp"
#include "Managers/InspectorManager.hpp"
#include "Managers/UndoRedoManager.hpp"

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
#include "Controls/CoreControls/VectorControl.hpp"
#include "Controls/CoreControls/Vector2Control.hpp"

#include "Controls/EngineControls/MeshResourceControl.hpp"
#include "Controls/EngineControls/TransformControl.hpp"

// dialogs
#include "Dialogs/CreateProjectDialog.hpp"

#include "Dialogs/Inspectors/Entities/EntityDialog.hpp"

#include "Dialogs/Inspectors/Components/ComponentDialog.hpp"

// systems
#include "Systems/Gizmo/GizmoSystem.hpp"
#include "Systems/Gizmo/GizmoWorldComponent.hpp"

#include "Systems/Camera/EditorCameraMovementSystem.hpp"
#include "Systems/Camera/EditorCameraMovementComponent.hpp"

// engine
#include <Engine.hpp>