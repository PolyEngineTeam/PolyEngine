#pragma once

#include <QtWidgets/qwidget.h>

#include <Defines.hpp>

#include "Controls/IControlBase.hpp"

class RTTIRegisteredControl;
class Command;

using ControlCreatorPtr = RTTIRegisteredControl* (*)(QWidget* parent);



//------------------------------------------------------------------------------
class ControlCreatorsManager
{
public:
	template <typename T>
	static ControlCreatorPtr RegisterControl(RTTI::eCorePropertyType type)
	{
		auto creator = [](QWidget* parent) -> RTTIRegisteredControl* { return new T(parent); };
		ControlCreators[(size_t)type] = creator;
		return creator;
	}

	static RTTIRegisteredControl* CreateControl(QWidget* parent, ::Poly::RTTI::eCorePropertyType type)
	{
		return ControlCreators[(size_t)type](parent);
	}

private:
	inline static std::array<ControlCreatorPtr, (size_t)RTTI::eCorePropertyType::_COUNT> ControlCreators = std::array<ControlCreatorPtr, (size_t)RTTI::eCorePropertyType::_COUNT>();
};



//------------------------------------------------------------------------------
class RTTIRegisteredControl : public QWidget, public IControlBase
{
	Q_OBJECT

public:
	RTTIRegisteredControl(QWidget* parent) : QWidget(parent) {}

signals:
	// After object is updated this signal is emitted.
	// @param cmd - pointer to command object to enable undo/redo actions.
	void ObjectUpdated(Command* cmd);
};

// Use this to add Your control to map from core type to control creator function.
// @param CONTROL - control type to register
// @param CORE_TYPE - core type that will be paired with given control type so 
//		we can easily map from RTTI::eCorePropertyType to control.
// @param NAME - we don't need it much but all pointers have to have different names.
//		It is a problem when we try to assign more than one core types to single control.

#define ASSIGN_CONTROL(CONTROL, CORE_TYPE, NAME) \
	namespace Impl \
	{ \
		ControlCreatorPtr CONTROL##Creator##NAME = \
			ControlCreatorsManager::RegisterControl<CONTROL>(CORE_TYPE);\
	}

#define ASSIGN_PARAMETRIZED_CONTROL(CONTROL, PARAM, CORE_TYPE, NAME) \
	namespace Impl \
	{ \
		ControlCreatorPtr CONTROL##Creator##NAME = \
			ControlCreatorsManager::RegisterControl<CONTROL<PARAM>>(CORE_TYPE);\
	}
