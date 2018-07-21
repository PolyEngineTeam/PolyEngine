#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

ASSIGN_CONTROL(Vector3Control, RTTI::eCorePropertyType::VECTOR, Vector)
ASSIGN_CONTROL(Vector3Control, RTTI::eCorePropertyType::COLOR, Color)
ASSIGN_CONTROL(Vector3Control, RTTI::eCorePropertyType::QUATERNION, Quaternion)

#define UPDATE_OBJECT(T, V)\
	ControlCommand<T>* cmd = new ControlCommand<T>(); \
	cmd->Object = Object; \
	cmd->Control = this; \
\
	cmd->UndoValue = new T(*reinterpret_cast<T*>(Object));\
	cmd->RedoValue = new T(V);\
\
	*reinterpret_cast<T*>(Object) = V; \
\
	emit ObjectUpdated(cmd);

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
		connect(Field[x], &QLineEdit::editingFinished, this, &Vector3Control::Confirm);
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

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR:
	{
		UPDATE_OBJECT(Vector, Vector((float)Field[0]->text().toDouble()
			, (float)Field[1]->text().toDouble()
			, (float)Field[2]->text().toDouble()));
		break;
	}

	case RTTI::eCorePropertyType::COLOR:
	{
		UPDATE_OBJECT(Color, Color((float)Field[0]->text().toDouble()
			, (float)Field[1]->text().toDouble()
			, (float)Field[2]->text().toDouble()));
		break;
	}

	case RTTI::eCorePropertyType::QUATERNION:
	{
		UPDATE_OBJECT(Quaternion, Quaternion(EulerAngles(Angle::FromDegrees((float)Field[0]->text().toDouble())
			, Angle::FromDegrees((float)Field[1]->text().toDouble())
			, Angle::FromDegrees((float)Field[2]->text().toDouble()))));
		break;
	}

	default:
		ASSERTE(false, "Unknown type.");
	}
}