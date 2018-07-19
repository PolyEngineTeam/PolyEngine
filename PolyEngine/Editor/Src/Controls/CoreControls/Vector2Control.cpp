#include "PolyEditorPCH.hpp"

#include "Math/Vector2f.hpp"

ASSIGN_CONTROL(Vector2Control, RTTI::eCorePropertyType::VECTOR_2F, Vector2f)
ASSIGN_CONTROL(Vector2Control, RTTI::eCorePropertyType::VECTOR_2I, Vector2i)

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

void Vector2Control::Reset() {
	Object = nullptr;
	Field[0]->setText("");
	Field[1]->setText("");
}

void Vector2Control::UpdateObject() {

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

void Vector2Control::UpdateControl() {

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

	default:
		ASSERTE(false, "Not supported type");
	}
}