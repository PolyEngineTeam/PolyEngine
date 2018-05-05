#include "NumberControl.hpp"

#include <QIntValidator>
#include <QDoubleValidator>

DEFINE_CONTROL(NumberControl, INT8)
DEFINE_CONTROL(NumberControl, INT16)
DEFINE_CONTROL(NumberControl, INT32)
DEFINE_CONTROL(NumberControl, INT64)
DEFINE_CONTROL(NumberControl, UINT8)
DEFINE_CONTROL(NumberControl, UINT16)
DEFINE_CONTROL(NumberControl, UINT32)
DEFINE_CONTROL(NumberControl, UINT64)
DEFINE_CONTROL(NumberControl, FLOAT)
DEFINE_CONTROL(NumberControl, DOUBLE)

NumberControl::NumberControl(QWidget* parent)
	: ControlBase(parent)
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
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::INT64:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::UINT8:
		*reinterpret_cast<u8*>(Object) = (u8)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::UINT16:
		*reinterpret_cast<u16*>(Object) = (u16)Field->text().toInt();
		break;
	case RTTI::eCorePropertyType::UINT32:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::UINT64:
		ASSERTE(false, "Not supported type");
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
		break;
	case RTTI::eCorePropertyType::INT16:
		Field->setText(QString::number(*reinterpret_cast<i16*>(Object)));
		break;
	case RTTI::eCorePropertyType::INT32:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::INT64:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::UINT8:
		Field->setText(QString::number(*reinterpret_cast<u8*>(Object)));
		break;
	case RTTI::eCorePropertyType::UINT16:
		Field->setText(QString::number(*reinterpret_cast<u16*>(Object)));
		break;
	case RTTI::eCorePropertyType::UINT32:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::UINT64:
		ASSERTE(false, "Not supported type");
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

void NumberControl::InitializeControl()
{
	// TODO(squares): write validator for INT32 and INT64

	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setVerticalStretch(2);
	sizePolicy.setHorizontalStretch(2);

	Field = std::make_unique<QLineEdit>(this);
	Field->setSizePolicy(sizePolicy);

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::INT8:
		Validator = std::make_unique<QIntValidator>(-128, 127);
		Field->setText(QString::number(*reinterpret_cast<i8*>(Object)));
		break;
	case RTTI::eCorePropertyType::INT16:
		Validator = std::make_unique<QIntValidator>(-32768, 32767);
		Field->setText(QString::number(*reinterpret_cast<i16*>(Object)));
		break;
	case RTTI::eCorePropertyType::INT32:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::INT64:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::UINT8:
		Validator = std::make_unique<QIntValidator>(0, 255);
		Field->setText(QString::number(*reinterpret_cast<u8*>(Object)));
		break;
	case RTTI::eCorePropertyType::UINT16:
		Validator = std::make_unique<QIntValidator>(0, 65536);
		Field->setText(QString::number(*reinterpret_cast<u16*>(Object)));
		break;
	case RTTI::eCorePropertyType::UINT32:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::UINT64:
		ASSERTE(false, "Not supported type");
		break;
	case RTTI::eCorePropertyType::FLOAT:
		Validator = std::make_unique<QDoubleValidator>(-3.4E+38, +3.4E+38, 7);
		Field->setText(QString::number(*reinterpret_cast<f32*>(Object)));
		break;
	case RTTI::eCorePropertyType::DOUBLE:
		Validator = std::make_unique<QDoubleValidator>(-1.7E+308, +1.7E+308, 16);
		Field->setText(QString::number(*reinterpret_cast<f64*>(Object)));
		break;
	default:
		ASSERTE(false, "Not supported type");
	}

	Field->setValidator(Validator.get());
}
