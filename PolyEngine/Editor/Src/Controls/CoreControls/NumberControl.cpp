#include "NumberControl.hpp"

#include <QIntValidator>
#include <QDoubleValidator>

ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::INT8, INT8)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::INT16, INT16)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::INT32, INT32)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::INT64, INT64)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::UINT8, UINT8)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::UINT16, UINT16)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::UINT32, UINT32)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::UINT64, UINT64)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::FLOAT, FLOAT)
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::DOUBLE, DOUBLE)

NumberControl::NumberControl(QWidget* parent)
	: ControlBase(parent)
{
	// TODO(squares): write proper validators

	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit(this);
	connect(Field, &QLineEdit::editingFinished, this, &ControlBase::Confirm);

	Layout->addWidget(Field);

	setLayout(Layout);
}

void NumberControl::Reset()
{
	Object = nullptr;

	Field->setText("");
}

void NumberControl::UpdateObject()
{
	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::INT8:
		*reinterpret_cast<i8*>(Object) = (i8)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::INT16:
		*reinterpret_cast<i16*>(Object) = (i16)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::INT32:
		*reinterpret_cast<i32*>(Object) = (i32)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::INT64:
		*reinterpret_cast<i64*>(Object) = (i64)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::UINT8:
		*reinterpret_cast<u8*>(Object) = (u8)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::UINT16:
		*reinterpret_cast<u16*>(Object) = (u16)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::UINT32:
		*reinterpret_cast<u32*>(Object) = (u32)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::UINT64:
		*reinterpret_cast<u64*>(Object) = (u64)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::FLOAT:
		*reinterpret_cast<f32*>(Object) = (f32)Field->text().toFloat();
		break;
	case RTTI::eCorePropertyType::DOUBLE:
		*reinterpret_cast<f64*>(Object) = (f64)Field->text().toDouble();
		break;
	default:
		ASSERTE(false, "Not supported type");
	}
	emit ObjectUpdated();
}

void NumberControl::UpdateControl()
{
	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::INT8:
		Field->setText(QString::number(*reinterpret_cast<i8*>(Object)));
		break;
	case RTTI::eCorePropertyType::INT16:
		Field->setText(QString::number(*reinterpret_cast<i16*>(Object)));
		break;
	case RTTI::eCorePropertyType::INT32:
		Field->setText(QString::number(*reinterpret_cast<i32*>(Object)));
		break;
	case RTTI::eCorePropertyType::INT64:
		Field->setText(QString::number(*reinterpret_cast<i64*>(Object)));
		break;
	case RTTI::eCorePropertyType::UINT8:
		Field->setText(QString::number(*reinterpret_cast<u8*>(Object)));
		break;
	case RTTI::eCorePropertyType::UINT16:
		Field->setText(QString::number(*reinterpret_cast<u16*>(Object)));
		break;
	case RTTI::eCorePropertyType::UINT32:
		Field->setText(QString::number(*reinterpret_cast<u32*>(Object)));
		break;
	case RTTI::eCorePropertyType::UINT64:
		Field->setText(QString::number(*reinterpret_cast<u64*>(Object)));
		break;
	case RTTI::eCorePropertyType::FLOAT:
		Field->setText(QString::number(*reinterpret_cast<f32*>(Object)));
		break;
	case RTTI::eCorePropertyType::DOUBLE:
		Field->setText(QString::number(*reinterpret_cast<f64*>(Object)));
		break;
	default:
		ASSERTE(false, "Not supported type");
	}
}
