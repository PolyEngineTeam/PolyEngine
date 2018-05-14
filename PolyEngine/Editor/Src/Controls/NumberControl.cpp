#include "NumberControl.hpp"

#include <QIntValidator>
#include <QDoubleValidator>

ASSIGN_CONTROL(NumberControl, INT8)
ASSIGN_CONTROL(NumberControl, INT16)
ASSIGN_CONTROL(NumberControl, INT32)
ASSIGN_CONTROL(NumberControl, INT64)
ASSIGN_CONTROL(NumberControl, UINT8)
ASSIGN_CONTROL(NumberControl, UINT16)
ASSIGN_CONTROL(NumberControl, UINT32)
ASSIGN_CONTROL(NumberControl, UINT64)
ASSIGN_CONTROL(NumberControl, FLOAT)
ASSIGN_CONTROL(NumberControl, DOUBLE)

NumberControl::NumberControl()
{
	// TODO(squares): write validator for INT32 and INT64

	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit(this);

	Layout->addWidget(Field);

	setLayout(Layout);
}

NumberControl::~NumberControl()
{
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
}

void NumberControl::UpdateControl()
{
	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::INT8:
		Field->setText(QString::number(*reinterpret_cast<i8*>(Object)));
		setToolTip("INT8");
		break;
	case RTTI::eCorePropertyType::INT16:
		Field->setText(QString::number(*reinterpret_cast<i16*>(Object)));
		setToolTip("INT16");
		break;
	case RTTI::eCorePropertyType::INT32:
		Field->setText(QString::number(*reinterpret_cast<i32*>(Object)));
		setToolTip("INT32");
		break;
	case RTTI::eCorePropertyType::INT64:
		Field->setText(QString::number(*reinterpret_cast<i64*>(Object)));
		setToolTip("INT64");
		break;
	case RTTI::eCorePropertyType::UINT8:
		Field->setText(QString::number(*reinterpret_cast<u8*>(Object)));
		setToolTip("UINT8");
		break;
	case RTTI::eCorePropertyType::UINT16:
		Field->setText(QString::number(*reinterpret_cast<u16*>(Object)));
		setToolTip("UINT16");
		break;
	case RTTI::eCorePropertyType::UINT32:
		Field->setText(QString::number(*reinterpret_cast<u32*>(Object)));
		setToolTip("UINT32");
		break;
	case RTTI::eCorePropertyType::UINT64:
		Field->setText(QString::number(*reinterpret_cast<u64*>(Object)));
		setToolTip("UINT64");
		break;
	case RTTI::eCorePropertyType::FLOAT:
		Field->setText(QString::number(*reinterpret_cast<f32*>(Object)));
		setToolTip("FLOAT");
		break;
	case RTTI::eCorePropertyType::DOUBLE:
		Field->setText(QString::number(*reinterpret_cast<f64*>(Object)));
		setToolTip("DOUBLE");
		break;
	default:
		ASSERTE(false, "Not supported type");
	}
}
