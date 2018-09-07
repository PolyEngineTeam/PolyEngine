#pragma once

#include <QtWidgets/qlineedit.h>

#include <Defines.hpp>

#include "Controls/ControlBase.hpp"

template <typename T>
class Vector2Control : public ControlBase<T>
{
	using Base = ControlBase<T>;

public:
	Vector2Control(QWidget* parent)
		: Base(parent)
	{
		Base::Layout->setColumnStretch(0, 1);
		Base::Layout->setColumnStretch(1, 2);
		Base::Layout->setColumnStretch(2, 2);

		for (int x = 0; x < 2; ++x)
		{
			Field[x] = new QLineEdit(this);
			Base::Layout->addWidget(Field[x], 0, x + 1);
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
		if (Field[0]->hasFocus() || Field[1]->hasFocus() || Base::DisableUpdateControl)
			return;

		auto* vector = Base::Object;
		Field[0]->setText(QString::number(vector->X));
		Field[1]->setText(QString::number(vector->Y));
	}

private:
	void UpdateObject()
	{
		auto val = T(Vector2f(Field[0]->text().toFloat(), Field[1]->text().toFloat()));

		if (Base::DisableEdit || *Base::Object == val)
			return; 
			
		ControlCommand<T>* cmd = new ControlCommand<T>(); 
		cmd->Object = Base::Object;
		cmd->Control = this; 
			
		cmd->UndoValue = new T(*Base::Object);
		cmd->RedoValue = new T(val); 
			
		*Base::Object = val;
			
		emit ObjectUpdated(cmd);
	}

	QLineEdit* Field[2];
};