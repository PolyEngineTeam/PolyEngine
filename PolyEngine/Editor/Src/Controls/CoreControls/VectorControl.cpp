#include "PolyEditorPCH.hpp"

ASSIGN_CONTROL(VectorControl, RTTI::eCorePropertyType::VECTOR, Vector)

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
		connect(Field[x], &QLineEdit::editingFinished, this, &VectorControl::Confirm);
	}
}

void VectorControl::Reset() {
	Object = nullptr;
	Field[0]->setText("");
	Field[1]->setText("");
	Field[2]->setText("");
}

void VectorControl::UpdateObject() {
	Vector* vector = reinterpret_cast<Vector*>(Object);
	vector->X = Field[0]->text().toFloat();
	vector->Y = Field[1]->text().toFloat();
	vector->Z = Field[2]->text().toFloat();
}

void VectorControl::UpdateControl() {
	Vector* vector = reinterpret_cast<Vector*>(Object);
	Field[0]->setText(QString::number(vector->X));
	Field[1]->setText(QString::number(vector->Y));
	Field[2]->setText(QString::number(vector->Z));
}