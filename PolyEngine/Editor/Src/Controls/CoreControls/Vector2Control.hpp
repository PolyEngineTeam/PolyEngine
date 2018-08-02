#pragma once

#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qgridlayout.h>

#include <Core.hpp>

#include "Controls/ControlBase.hpp"

template <typename T>
class Vector2Control : public ControlBase<T>
{
public:
	Vector2Control(QWidget* parent)
		: ControlBase(parent)
	{
		Layout = new QGridLayout(this);

		Layout->setColumnStretch(0, 1);
		Layout->setColumnStretch(1, 1);
		Layout->setContentsMargins(0, 0, 0, 0);

		for (int x = 0; x < 2; ++x)
		{
			Field[x] = new QLineEdit(this);
			Layout->addWidget(Field[x], 0, x);
			connect(Field[x], &QLineEdit::editingFinished, this, &Vector2Control::UpdateObject);
		}
	}

	void Reset() override
	{
		ControlBase<T>::Reset();

		Field[0]->setText("");
		Field[1]->setText("");
	}
	void UpdateControl() override
	{
		if (Field[0]->hasFocus() || Field[1]->hasFocus())
			return;

		auto* vector = Object;
		Field[0]->setText(QString::number(vector->X));
		Field[1]->setText(QString::number(vector->Y));
	}

private:
	void UpdateObject()
	{
		auto val = T(Vector2f(Field[0]->text().toFloat(), Field[1]->text().toFloat()));

		if (DisableEdit || *Object == val) 
			return; 
			
		ControlCommand<T>* cmd = new ControlCommand<T>(); 
		cmd->Object = Object; 
		cmd->Control = this; 
			
		cmd->UndoValue = new T(*Object); 
		cmd->RedoValue = new T(val); 
			
		*Object = val; 
			
		emit ObjectUpdated(cmd);
	}

	QGridLayout* Layout;
	QLineEdit* Field[2];
};