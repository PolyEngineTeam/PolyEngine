#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

ASSIGN_CONTROL(Vector3Control, RTTI::eCorePropertyType::VECTOR, Vector)

//------------------------------------------------------------------------------
Vector3Control::Vector3Control(QWidget* parent) : ControlBase(parent)
{
	Layout->setColumnStretch(0, 1);
	Layout->setColumnStretch(1, 1);
	Layout->setColumnStretch(2, 1);
	Layout->setColumnStretch(3, 1);
	Layout->setContentsMargins(0, 0, 0, 0);

	for (int x = 0; x < 3; ++x)
	{
		Field[x] = new QLineEdit(this);
		Layout->addWidget(Field[x], 0, x + 1);
		connect(Field[x], &QLineEdit::editingFinished, this, &Vector3Control::UpdateObject);
	}
}

//------------------------------------------------------------------------------
void Vector3Control::Reset() 
{
	ControlBase<Vector>::Reset();

	Field[0]->setText("");
	Field[1]->setText("");
	Field[2]->setText("");
}

//------------------------------------------------------------------------------
void Vector3Control::UpdateControl() 
{
	if (Field[0]->hasFocus() || Field[1]->hasFocus() || Field[2]->hasFocus())
		return;

	Vector* vector = static_cast<Vector*>(Object);
	Field[0]->setText(QString::number(vector->X));
	Field[1]->setText(QString::number(vector->Y));
	Field[2]->setText(QString::number(vector->Z));
}

//------------------------------------------------------------------------------
void Vector3Control::UpdateObject()
{
	auto val = Vector((float)Field[0]->text().toDouble()
		, (float)Field[1]->text().toDouble()
		, (float)Field[2]->text().toDouble());

	if (DisableEdit || *Object == val)
		return;

	ControlCommand<Vector>* cmd = new ControlCommand<Vector>(); 
	cmd->Object = Object; 
	cmd->Control = this; 
	
	cmd->UndoValue = new Vector(*Object); 
	cmd->RedoValue = new Vector(val); 
	
	*Object = val; 
	
	emit ObjectUpdated(cmd);
}