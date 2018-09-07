#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>

template <typename T>
class NumberControl : public ControlBase<T>
{
public:
	NumberControl(QWidget* parent)
		: ControlBase<T>(parent)
	{
		Field = new QLineEdit(this);
		connect(Field, &QLineEdit::editingFinished, this, &NumberControl<T>::UpdateObject);

		ControlBase<T>::Layout->addWidget(Field, 0, 1);
	}

	void Reset() override
	{
		ControlBase<T>::Reset();

		Field->setText("");
	}

	void UpdateControl() override
	{
		if (Field->hasFocus())
			return;

		Field->setText(QString::number(*Object));
	}

private:
	void UpdateObject()
	{
		auto val = (T)Field->text().toDouble();

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

	QLineEdit* Field;

	QValidator* Validator;
};