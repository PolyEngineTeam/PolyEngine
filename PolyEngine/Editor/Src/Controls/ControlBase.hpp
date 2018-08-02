#pragma once

#include <QtWidgets/qstyle.h>

#include "Controls/IControlBase.hpp"

template <typename T>
class ControlBase : public IControlBase
{
public:
	ControlBase(QWidget* parent) : IControlBase(parent) { Reset(); }

	virtual void SetObject(void* ptr, const RTTI::Property* prop) override
	{
		Object = reinterpret_cast<T*>(ptr);
		Property = prop;
		UpdateControl();
		setToolTip(Property->Name.GetCStr());
	}

	virtual void Reset() override
	{
		Object = nullptr;
		DisableEdit = false;
		Property = nullptr;
	}

	virtual bool ContainsLabel() override
	{
		return false;
	}

	virtual void SetDisableEdit(bool disable) override
	{
		DisableEdit = disable;

		if (disable)
		{
			setDisabled(true);
		}
		else
		{
			setPalette(style()->standardPalette());

			setDisabled(false);
		}
	}

	virtual bool GetDisableEdit() override
	{
		return DisableEdit;
	}

protected:
	// Pointer to an object which is assigned to this control.
	T* Object = nullptr;

	// Property of assigned object. Useful when one control is assigned to multiple core types
	// like NumberControl.
	const RTTI::Property* Property;

	bool DisableEdit = false;
	bool DisableUpdateControl = false;
};