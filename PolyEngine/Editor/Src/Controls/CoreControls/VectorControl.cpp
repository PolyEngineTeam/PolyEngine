#include "PolyEditorPCH.hpp"

ASSIGN_CONTROL(VectorControl, RTTI::eCorePropertyType::VECTOR, Vector)
ASSIGN_CONTROL(VectorControl, RTTI::eCorePropertyType::COLOR, Color)
ASSIGN_CONTROL(VectorControl, RTTI::eCorePropertyType::QUATERNION, Quaternion)

VectorControl::VectorControl(QWidget* parent) : ControlBase(parent)
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
		connect(Field[x], &QLineEdit::returnPressed, this, &VectorControl::Confirm);
	}
}

void VectorControl::Reset() {
	Object = nullptr;
	Field[0]->setText("");
	Field[1]->setText("");
	Field[2]->setText("");
}

void VectorControl::UpdateObject() {

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

void VectorControl::UpdateControl() {

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
	}
}