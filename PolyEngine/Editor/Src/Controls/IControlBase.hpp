#pragma once

#include <QtWidgets/qwidget.h>
#include <QtCore/qtimer.h>

#include <RTTI/RTTI.hpp>
#include <Utils/Logger.hpp>

using namespace Poly;

// This is base class for all controls for core types such as int, string or vector.
// @see Poly::RTTI::eCorePropertyType
class IControlBase
{
public:
	// Assigns given object to control and updates this control.
	virtual void SetObject(void* ptr) = 0;

	// Name of assigned property.
	virtual void SetName(String name) = 0;
	// Type of assigned property.
	virtual void SetType(String type) = 0;
	// If set to true then control will not permit changing its content.
	virtual void SetDisableEdit(bool disable) = 0;

	// Resets this control to initial state;
	virtual void Reset() = 0;

	// Call this to update control state from assigned object.
	virtual void UpdateControl() = 0;
};
