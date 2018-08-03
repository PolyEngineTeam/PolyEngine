#pragma once

#include <QtWidgets/qstyle.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>

#include "Controls/IControlBase.hpp"

template <typename T>
class ControlBase : public IControlBase
{
public:
	ControlBase(QWidget* parent) : IControlBase(parent) 
	{
		// create lauout
		Layout = new QGridLayout(this);
		Layout->setSpacing(0);
		Layout->setContentsMargins(0, 0, 0, 0);
		Layout->setColumnStretch(0, 1);
		Layout->setColumnStretch(1, 3);

		// create label
		Label = new QLabel(this);
		
		// add label
		Layout->addWidget(Label, 0, 0);

		// set defaults
		Reset(); 

		Label->setText("Unassigned");
		setToolTip("Unassigned");
	}

	virtual void SetObject(void* ptr) override
	{
		Object = reinterpret_cast<T*>(ptr);
		UpdateControl();
		SetDisableEdit(false);
	}

	virtual void SetName(String name) override
	{
		Label->setText(name.GetCStr());
	}

	virtual void SetType(String type) override
	{
		setToolTip(type.GetCStr());
	}

	virtual void Reset() override
	{
		Object = nullptr;
		SetDisableEdit(true);
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

protected:
	// Pointer to an object which is assigned to this control.
	T* Object = nullptr;

	bool DisableEdit = false;
	bool DisableUpdateControl = false;

	// ui things
	QGridLayout* Layout;
	QLabel* Label;
};