#pragma once

#include "Controls/ControlBase.hpp"

#include <QtWidgets/qlineedit.h>

template <typename T>
class NumberControl : public ControlBase<T>
{
public:
	NumberControl(QWidget* parent)
		: ControlBase(parent)
	{
		Layout = new QGridLayout(this);
		Layout->setSpacing(0);
		Layout->setContentsMargins(0, 0, 0, 0);

		Field = new QLineEdit(this);
		//Field->setValidator(new QRegExpValidator(QRegExp("[-,+]{0,1}[0-9]+[.][0-9]+"), 0));
		connect(Field, &QLineEdit::editingFinished, this, &NumberControl::UpdateObject);

		Layout->addWidget(Field);

		setLayout(Layout);
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
	QGridLayout* Layout;

	QValidator* Validator;
};