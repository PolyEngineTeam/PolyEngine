#include "PolyEditorPCH.hpp"

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
		connect(Field[x], &QLineEdit::editingFinished, this, &Vector2Control::Confirm);
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
		reinterpret_cast<Vector2f*>(Object)->X = Field[0]->text().toFloat();
		reinterpret_cast<Vector2f*>(Object)->Y = Field[1]->text().toFloat();
		break;

	case RTTI::eCorePropertyType::VECTOR_2I:
		reinterpret_cast<Vector2i*>(Object)->X = Field[0]->text().toInt();
		reinterpret_cast<Vector2i*>(Object)->Y = Field[1]->text().toInt();
		break;

	default:
		ASSERTE(false, "Not supported type");
	}

}

void Vector2Control::UpdateControl() {

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR_2F:
		Field[0]->setText(QString::number(reinterpret_cast<Vector2f*>(Object)->X));
		Field[1]->setText(QString::number(reinterpret_cast<Vector2f*>(Object)->Y));
		break;

	case RTTI::eCorePropertyType::VECTOR_2I:
		Field[0]->setText(QString::number(reinterpret_cast<Vector2i*>(Object)->X));
		Field[1]->setText(QString::number(reinterpret_cast<Vector2i*>(Object)->Y));
		break;

	default:
		ASSERTE(false, "Not supported type");
	}
}