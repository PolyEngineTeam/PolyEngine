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

#define UPDATE_OBJECT(T, V)\
	cmd->UndoValue = new T(*reinterpret_cast<T*>(Object));\
\
	*reinterpret_cast<T*>(Object) = V;\
\
	cmd->RedoValue = new T(*reinterpret_cast<T*>(Object));\
\
	cmd->UndoPtr = [](ControlCommand* c)\
	{\
		*reinterpret_cast<T*>(c->Object) = *reinterpret_cast<T*>(c->UndoValue);\
		emit c->Control->ObjectUpdated(c);\
	};\
	cmd->RedoPtr = [](ControlCommand* c)\
	{\
		*reinterpret_cast<T*>(c->Object) = *reinterpret_cast<T*>(c->RedoValue);\
		emit c->Control->ObjectUpdated(c);\
	};

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void NumberControl::Reset()
{
	Object = nullptr;

	Field->setText("");
}

//------------------------------------------------------------------------------
void NumberControl::UpdateObject()
{
	ControlCommand* cmd = new ControlCommand();
	cmd->Object = Object;
	cmd->Control = this; 

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::INT8:
		UPDATE_OBJECT(i8, (i8)Field->text().toInt())
		break;

	case RTTI::eCorePropertyType::INT16:
		UPDATE_OBJECT(i16, (i16)Field->text().toInt())
		break;

	case RTTI::eCorePropertyType::INT32:
		UPDATE_OBJECT(i32, (i32)Field->text().toInt())
		break;

	case RTTI::eCorePropertyType::INT64:
		UPDATE_OBJECT(i64, (i64)Field->text().toInt())
		break;

	case RTTI::eCorePropertyType::UINT8:
		UPDATE_OBJECT(u8, (u8)Field->text().toInt())
		break;

	case RTTI::eCorePropertyType::UINT16:
		UPDATE_OBJECT(u16, (u16)Field->text().toInt())
		break;

	case RTTI::eCorePropertyType::UINT32:
		UPDATE_OBJECT(u32, (u32)Field->text().toInt())
		break;

	case RTTI::eCorePropertyType::UINT64:
		UPDATE_OBJECT(u64, (u64)Field->text().toInt())
		break;

	case RTTI::eCorePropertyType::FLOAT:
		UPDATE_OBJECT(f32, (f32)Field->text().toFloat())
		break;

	case RTTI::eCorePropertyType::DOUBLE:
		UPDATE_OBJECT(f64, (f64)Field->text().toDouble())
		break;

	default:
		ASSERTE(false, "Not supported type");
	}
}

//------------------------------------------------------------------------------
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
