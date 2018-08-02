#pragma once

#include <QtWidgets/qwidget.h>
#include <QtCore/qtimer.h>

#include <RTTI/RTTI.hpp>
#include <Utils/Logger.hpp>

#include "Controls/ExtremelyBaseControl.hpp"

using namespace Poly;

// This is base class for all controls for core types such as int, string or vector.
// @see Poly::RTTI::eCorePropertyType
class IControlBase : public ExtremelyBaseControl
{
public:
	IControlBase(QWidget* parent) : ExtremelyBaseControl(parent) {}

	// Assigns given object to control and updates this control.
	virtual void SetObject(void* ptr, const RTTI::Property* prop) = 0;

	// Resets this control to initial state;
	virtual void Reset() = 0;

	// Call this to update control state from assigned object.
	virtual void UpdateControl() = 0;

	// If label is set by control this function returns false.
	// For example TransformControl looks better when label is set by control.
	virtual bool ContainsLabel() = 0;

	// With disabled edit control changes its color and becomes inactive.
	virtual void SetDisableEdit(bool disable) = 0;
	virtual bool GetDisableEdit() = 0;
};
