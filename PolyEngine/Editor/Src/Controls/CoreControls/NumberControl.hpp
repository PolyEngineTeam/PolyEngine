#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>

template <typename T>
class NumberControl : public ControlBase<T>
{
	using Base = ControlBase<T>;

public:
	NumberControl(QWidget* parent)
		: ControlBase<T>(parent)
	{
		Field = new QLineEdit(this);
		connect(Field, &QLineEdit::editingFinished, this, &NumberControl<T>::UpdateObject);

		Base::Layout->addWidget(Field, 0, 1);
	}

	void Reset() override
	{
		Base::Reset();

		Field->setText("");
	}

	void UpdateControl() override
	{
		if (Field->hasFocus() || Base::DisableUpdateControl)
			return;

		Field->setText(QString::number(*Base::Object));
	}

private:
	void UpdateObject()
	{
		auto val = (T)Field->text().toDouble();

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

	QLineEdit* Field;

	QValidator* Validator;
};