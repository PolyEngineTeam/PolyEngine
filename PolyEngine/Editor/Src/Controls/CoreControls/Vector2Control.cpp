#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

#include "Math/Vector2f.hpp"
#include "Math/Vector2i.hpp"

ASSIGN_CONTROL(Vector2Control, RTTI::eCorePropertyType::VECTOR_2F, Vector2f)
ASSIGN_CONTROL(Vector2Control, RTTI::eCorePropertyType::VECTOR_2I, Vector2i)

//------------------------------------------------------------------------------
Vector2Control::Vector2Control(QWidget* parent) : ControlBase(parent)
{
	Layout = new QGridLayout(this);

	Layout->setColumnStretch(0, 1);
	Layout->setColumnStretch(1, 1);
	Layout->setContentsMargins(0, 0, 0, 0);

	for (int x = 0; x < 2; ++x)
	{
		Field[x] = new QLineEdit(this);
		Layout->addWidget(Field[x], 0, x);
		connect(Field[x], &QLineEdit::returnPressed, this, &Vector2Control::Confirm);
	}
}

//------------------------------------------------------------------------------
void Vector2Control::Reset() 
{
	Object = nullptr;
	Field[0]->setText("");
	Field[1]->setText("");
}

//------------------------------------------------------------------------------
void Vector2Control::UpdateObject() 
{
	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR_2F:
	{
		Vector2f* vector = reinterpret_cast<Vector2f*>(Object);
		vector->X = Field[0]->text().toFloat();
		vector->Y = Field[1]->text().toFloat();
		break;
	}

	case RTTI::eCorePropertyType::VECTOR_2I:
	{
		Vector2i* vector = reinterpret_cast<Vector2i*>(Object);
		vector->X = Field[0]->text().toInt();
		vector->Y = Field[1]->text().toInt();
		break;
	}

	default:
		ASSERTE(false, "Not supported type");
	}

}

//------------------------------------------------------------------------------
void Vector2Control::UpdateControl() 
{
	if (Field[0]->hasFocus() || Field[1]->hasFocus())
		return;

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR_2F:
	{
		Vector2f* vector = reinterpret_cast<Vector2f*>(Object);
		Field[0]->setText(QString::number(vector->X));
		Field[1]->setText(QString::number(vector->Y));
		break;
	}

	case RTTI::eCorePropertyType::VECTOR_2I:
	{
		Vector2i* vector = reinterpret_cast<Vector2i*>(Object);
		Field[0]->setText(QString::number(vector->X));
		Field[1]->setText(QString::number(vector->Y));
		break;
	}
	}
}

//------------------------------------------------------------------------------
void Vector2Control::Confirm()
{
	if (DisableEdit)
		return;

	ControlCommand* cmd = new ControlCommand();
	cmd->Object = Object;
	cmd->Control = this;

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR_2F:
	{
		cmd->UndoValue = new Vector2f(*reinterpret_cast<Vector2f*>(Object));
		cmd->RedoValue = new Vector2f((float)Field[0]->text().toDouble(), (float)Field[1]->text().toDouble());

		*reinterpret_cast<Vector2f*>(Object) = Vector2f((float)Field[0]->text().toDouble(), (float)Field[1]->text().toDouble());

		cmd->UndoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Vector2f*>(c->Object) = *reinterpret_cast<Vector2f*>(c->UndoValue);
			emit c->Control->ObjectUpdated(c);
		};
		cmd->RedoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Vector2f*>(c->Object) = *reinterpret_cast<Vector2f*>(c->RedoValue);
			emit c->Control->ObjectUpdated(c);
		};
		break;
	}

	case RTTI::eCorePropertyType::VECTOR_2I:
	{
		cmd->UndoValue = new Vector2i(*reinterpret_cast<Vector2i*>(Object));
		cmd->RedoValue = new Vector2i(Field[0]->text().toInt(), Field[1]->text().toInt());
		*reinterpret_cast<Vector2i*>(Object) = Vector2i((VectorIntType)Field[0]->text().toInt(), (VectorIntType)Field[1]->text().toInt());

		cmd->UndoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Vector2i*>(c->Object) = *reinterpret_cast<Vector2i*>(c->UndoValue);
			emit c->Control->ObjectUpdated(c);
		};
		cmd->RedoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Vector2i*>(c->Object) = *reinterpret_cast<Vector2i*>(c->RedoValue);
			emit c->Control->ObjectUpdated(c);
		};
		break;
	}

	default:
		ASSERTE(false, "DDDDDDDDDDDDDDDDDDDDDDDDDDD");
	}


	emit ObjectUpdated(cmd);
}
