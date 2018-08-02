#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

#include "Math/Vector2f.hpp"
#include "Math/Vector2i.hpp"

ASSIGN_CONTROL(Vector2Control, RTTI::eCorePropertyType::VECTOR_2F, Vector2f)
ASSIGN_CONTROL(Vector2Control, RTTI::eCorePropertyType::VECTOR_2I, Vector2i)

#define UPDATE_OBJECT(T, V)\
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
	emit ObjectUpdated(cmd);

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
void Vector2Control::Reset() 
{
	Object = nullptr;
	Field[0]->setText("");
	Field[1]->setText("");
}

//------------------------------------------------------------------------------
void Vector2Control::UpdateControl() 
{
	if (Field[0]->hasFocus() || Field[1]->hasFocus())
		return;

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
		ASSERTE(false, "Unknown type.");
	}
}

//------------------------------------------------------------------------------
void Vector2Control::Confirm()
{
	if (DisableEdit)
		return;

	switch (Property->CoreType)
	{
	case RTTI::eCorePropertyType::VECTOR_2F:
	{
		UPDATE_OBJECT(Vector2f, Vector2f((float)Field[0]->text().toDouble(), (float)Field[1]->text().toDouble()))
		break;
	}

	case RTTI::eCorePropertyType::VECTOR_2I:
	{
		UPDATE_OBJECT(Vector2i, Vector2i((VectorIntType)Field[0]->text().toDouble(), (VectorIntType)Field[1]->text().toDouble()))
		break;
	}

	default:
		ASSERTE(false, "Unknown type.");
	}
}
