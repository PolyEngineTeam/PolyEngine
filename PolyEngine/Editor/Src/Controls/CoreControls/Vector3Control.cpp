#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

ASSIGN_CONTROL(Vector3Control, RTTI::eCorePropertyType::VECTOR, Vector)
ASSIGN_CONTROL(Vector3Control, RTTI::eCorePropertyType::COLOR, Color)
ASSIGN_CONTROL(Vector3Control, RTTI::eCorePropertyType::QUATERNION, Quaternion)

//------------------------------------------------------------------------------
Vector3Control::Vector3Control(QWidget* parent) : ControlBase(parent)
{
	Layout = new QGridLayout(this);

	Layout->setColumnStretch(0, 1);
	Layout->setColumnStretch(1, 1);
	Layout->setColumnStretch(2, 1);
	Layout->setContentsMargins(0, 0, 0, 0);

	for (int x = 0; x < 3; ++x)
	{
		Field[x] = new QLineEdit(this);
		Layout->addWidget(Field[x], 0, x);
		connect(Field[x], &QLineEdit::returnPressed, this, &Vector3Control::Confirm);
	}
}

//------------------------------------------------------------------------------
void Vector3Control::Reset() 
{
	Object = nullptr;
	Field[0]->setText("");
	Field[1]->setText("");
	Field[2]->setText("");
}

//------------------------------------------------------------------------------
void Vector3Control::UpdateObject() 
{
	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR:
	{
		Vector* vector = reinterpret_cast<Vector*>(Object);
		vector->X = Field[0]->text().toFloat();
		vector->Y = Field[1]->text().toFloat();
		vector->Z = Field[2]->text().toFloat();
		break;
	}

	case RTTI::eCorePropertyType::COLOR:
	{
		Color* color = reinterpret_cast<Color*>(Object);
		color->R = Field[0]->text().toFloat();
		color->G = Field[1]->text().toFloat();
		color->B = Field[2]->text().toFloat();
		break;
	}

	case RTTI::eCorePropertyType::QUATERNION:
	{
		Angle angleX = Angle::FromDegrees(Field[0]->text().toFloat());
		Angle angleY = Angle::FromDegrees(Field[1]->text().toFloat());
		Angle angleZ = Angle::FromDegrees(Field[2]->text().toFloat());
		EulerAngles angles = EulerAngles(angleX, angleY, angleZ);
		*reinterpret_cast<Quaternion*>(Object) = Quaternion(angles);
		break;
	}

	default:
		ASSERTE(false, "Not supported type");
	}
}

//------------------------------------------------------------------------------
void Vector3Control::UpdateControl() 
{
	if (Field[0]->hasFocus() || Field[1]->hasFocus() || Field[2]->hasFocus())
		return;

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR:
	{
		Vector* vector = reinterpret_cast<Vector*>(Object);
		Field[0]->setText(QString::number(vector->X));
		Field[1]->setText(QString::number(vector->Y));
		Field[2]->setText(QString::number(vector->Z));
		break;
	}

	case RTTI::eCorePropertyType::COLOR:
	{
		Color* color = reinterpret_cast<Color*>(Object);
		Field[0]->setText(QString::number(color->R));
		Field[1]->setText(QString::number(color->G));
		Field[2]->setText(QString::number(color->B));
		break;
	}

	case RTTI::eCorePropertyType::QUATERNION:
	{
		Quaternion* quaternion = reinterpret_cast<Quaternion*>(Object);
		EulerAngles angles = quaternion->ToEulerAngles();
		Field[0]->setText(QString::number(angles.X.AsDegrees()));
		Field[1]->setText(QString::number(angles.Y.AsDegrees()));
		Field[2]->setText(QString::number(angles.Z.AsDegrees()));
		break;
	}

	default:
		ASSERTE(false, "Unknown type.");
	}
}

//------------------------------------------------------------------------------
void Vector3Control::Confirm()
{
	if (DisableEdit)
		return;

	ControlCommand* cmd = new ControlCommand();
	cmd->Object = Object;
	cmd->Control = this;

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR:
	{
		cmd->UndoValue = new Vector(*reinterpret_cast<Vector*>(Object));
		cmd->RedoValue = new Vector((float)Field[0]->text().toDouble()
			, (float)Field[1]->text().toDouble()
			, (float)Field[2]->text().toDouble());

		*reinterpret_cast<Vector*>(Object) = Vector((float)Field[0]->text().toDouble()
			, (float)Field[1]->text().toDouble()
			, (float)Field[2]->text().toDouble());

		cmd->UndoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Vector*>(c->Object) = *reinterpret_cast<Vector*>(c->UndoValue);
			emit c->Control->ObjectUpdated(c);
		};
		cmd->RedoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Vector*>(c->Object) = *reinterpret_cast<Vector*>(c->RedoValue);
			emit c->Control->ObjectUpdated(c);
		};
		break;
	}

	case RTTI::eCorePropertyType::COLOR:
	{
		cmd->UndoValue = new Color(*reinterpret_cast<Color*>(Object));
		cmd->RedoValue = new Color((float)Field[0]->text().toDouble()
			, (float)Field[1]->text().toDouble()
			, (float)Field[2]->text().toDouble());

		*reinterpret_cast<Color*>(Object) = Color((float)Field[0]->text().toDouble()
			, (float)Field[1]->text().toDouble()
			, (float)Field[2]->text().toDouble());

		cmd->UndoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Color*>(c->Object) = *reinterpret_cast<Color*>(c->UndoValue);
			emit c->Control->ObjectUpdated(c);
		};
		cmd->RedoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Color*>(c->Object) = *reinterpret_cast<Color*>(c->RedoValue);
			emit c->Control->ObjectUpdated(c);
		};
		break;
	}

	case RTTI::eCorePropertyType::QUATERNION:
	{
		cmd->UndoValue = new Quaternion(*reinterpret_cast<Quaternion*>(Object));
		cmd->RedoValue = new Quaternion(EulerAngles(Angle::FromDegrees((float)Field[0]->text().toDouble())
			, Angle::FromDegrees((float)Field[1]->text().toDouble())
			, Angle::FromDegrees((float)Field[2]->text().toDouble())));

		*reinterpret_cast<Quaternion*>(Object) = Quaternion(EulerAngles(Angle::FromDegrees((float)Field[0]->text().toDouble())
			, Angle::FromDegrees((float)Field[1]->text().toDouble())
			, Angle::FromDegrees((float)Field[2]->text().toDouble())));

		cmd->UndoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Quaternion*>(c->Object) = *reinterpret_cast<Quaternion*>(c->UndoValue);
			emit c->Control->ObjectUpdated(c);
		};
		cmd->RedoPtr = [](ControlCommand* c)
		{
			*reinterpret_cast<Quaternion*>(c->Object) = *reinterpret_cast<Quaternion*>(c->RedoValue);
			emit c->Control->ObjectUpdated(c);
		};
		break;
	}

	default:
		ASSERTE(false, "Unknown type.");
	}


	emit ObjectUpdated(cmd);
}