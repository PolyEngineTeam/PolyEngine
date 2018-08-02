#include "PolyEditorPCH.hpp"

#include <QIntValidator>
#include <QDoubleValidator>

#include "Managers/CommandsImpl.hpp"

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
ASSIGN_CONTROL(NumberControl, RTTI::eCorePropertyType::ANGLE, ANGLE)

#define UPDATE_OBJECT(T, V) \
{ \
	if (*reinterpret_cast<T*>(Object) == V) \
		break; \
\
	ControlCommand<T>* cmd = new ControlCommand<T>(); \
	cmd->Object = reinterpret_cast<T*>(Object); \
	cmd->Control = this; \
\
	cmd->UndoValue = new T(*reinterpret_cast<T*>(Object));\
	cmd->RedoValue = new T(V);\
\
	*reinterpret_cast<T*>(Object) = V; \
\
	emit ObjectUpdated(cmd);\
} 


//------------------------------------------------------------------------------
NumberControl::NumberControl(QWidget* parent)
	: ControlBase(parent)
{
	Layout = new QGridLayout(this);
	Layout->setSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	Field = new QLineEdit(this);
	connect(Field, &QLineEdit::editingFinished, this, &NumberControl::Confirm);

	Layout->addWidget(Field);

	setLayout(Layout);

	UIntValidator = new QRegExpValidator(QRegExp("[0-9]+"), 0);
	IntValidator = new QRegExpValidator(QRegExp("[-,+]{0,1}[0-9]+"), 0);
	FloatValidator = new QRegExpValidator(QRegExp("[-,+]{0,1}[0-9]+[.][0-9]+"), 0);
}

//------------------------------------------------------------------------------
void NumberControl::Reset()
{
	Object = nullptr;

	Field->setText("");
}

//------------------------------------------------------------------------------
void NumberControl::UpdateControl()
{
	if (Field->hasFocus())
		return;

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
	case RTTI::eCorePropertyType::ANGLE:
		Field->setText(QString::number(reinterpret_cast<Angle*>(Object)->AsDegrees()));
		break;
	default:
		ASSERTE(false, "Not supported type");
	}
}

//------------------------------------------------------------------------------
void NumberControl::Confirm()
{
	if (DisableEdit)
		return;

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::INT8:
		UPDATE_OBJECT(i8, (i8)Field->text().toInt())
			Field->setValidator(IntValidator);
			break;

	case RTTI::eCorePropertyType::INT16:
		UPDATE_OBJECT(i16, (i16)Field->text().toInt())
			Field->setValidator(IntValidator);
			break;

	case RTTI::eCorePropertyType::INT32:
		UPDATE_OBJECT(i32, (i32)Field->text().toInt())
			Field->setValidator(IntValidator);
			break;

	case RTTI::eCorePropertyType::INT64:
		UPDATE_OBJECT(i64, (i64)Field->text().toInt())
			Field->setValidator(IntValidator);
			break;

	case RTTI::eCorePropertyType::UINT8:
		UPDATE_OBJECT(u8, (u8)Field->text().toInt())
			Field->setValidator(UIntValidator);
			break;

	case RTTI::eCorePropertyType::UINT16:
		UPDATE_OBJECT(u16, (u16)Field->text().toInt())
			Field->setValidator(UIntValidator);
			break;

	case RTTI::eCorePropertyType::UINT32:
		UPDATE_OBJECT(u32, (u32)Field->text().toInt())
			Field->setValidator(UIntValidator);
			break;

	case RTTI::eCorePropertyType::UINT64:
		UPDATE_OBJECT(u64, (u64)Field->text().toInt())
			Field->setValidator(UIntValidator);
			break;

	case RTTI::eCorePropertyType::FLOAT:
		UPDATE_OBJECT(f32, (f32)Field->text().toFloat())
			Field->setValidator(FloatValidator);
			break;

	case RTTI::eCorePropertyType::DOUBLE:
		UPDATE_OBJECT(f64, (f64)Field->text().toDouble())
			Field->setValidator(FloatValidator);
			break;

	case RTTI::eCorePropertyType::ANGLE:
		UPDATE_OBJECT(Angle, Angle::FromDegrees((f32)Field->text().toDouble()))
			Field->setValidator(FloatValidator);
			break;

	default:
		ASSERTE(false, "Not supported type");
	}
}
