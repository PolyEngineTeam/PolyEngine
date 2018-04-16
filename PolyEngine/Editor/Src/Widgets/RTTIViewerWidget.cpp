#include "PolyEditorPCH.hpp"

RTTIViewerWidget::RTTIViewerWidget(const QString& title, QWidget* parent)
	: PolyWidget(title, parent)
{
}

void RTTIViewerWidget::SetObject(RTTIBase* obj, bool debug)
{
	if (Layout)
		Layout.release();

	Layout = std::make_unique<QFormLayout>(this);
	setLayout(Layout.get());

	Object = obj;

	AddChild(Layout.get(), Object, debug);
}

void RTTIViewerWidget::AddChild(QFormLayout* parent, const RTTIBase* obj, bool debug)
{	
	for (auto& child : obj->GetPropertyManager()->GetPropertyList())
	{
		ASSERTE(child.CoreType != RTTI::eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (!debug && child.Flags.IsSet(RTTI::ePropertyFlag::DONT_SERIALIZE))
			continue;

		const void* ptr = ((const char*)obj) + child.Offset;

		if (child.CoreType == RTTI::eCorePropertyType::NONE)
		{
			QFormLayout* layout = new QFormLayout();
			parent->addItem(layout);
			AddChild(layout, reinterpret_cast<const RTTIBase*>(ptr), debug);
		}
		else
			AddItem(parent, ptr, child);
	}
}

void RTTIViewerWidget::AddItem(QFormLayout* parent, const void* value, const RTTI::Property& prop)
{
	QString v;

	switch (prop.CoreType)
	{
	case RTTI::eCorePropertyType::BOOL:
		v = QString(*reinterpret_cast<const bool*>(value));
		break;
	case RTTI::eCorePropertyType::INT8:
		v = QString(*reinterpret_cast<const i8*>(value));
		break;
	case RTTI::eCorePropertyType::INT16:
		v = QString(*reinterpret_cast<const i16*>(value));
		break;
	case RTTI::eCorePropertyType::INT32:
		v = QString(*reinterpret_cast<const i32*>(value));
		break;
	case RTTI::eCorePropertyType::INT64:
		v = QString("%1").arg(*reinterpret_cast<const i64*>(value));
		break;
	case RTTI::eCorePropertyType::UINT8:
		v = QString(*reinterpret_cast<const u8*>(value));
		break;
	case RTTI::eCorePropertyType::UINT16:
		v = QString(*reinterpret_cast<const u16*>(value));
		break;
	case RTTI::eCorePropertyType::UINT32:
		v = QString(*reinterpret_cast<const u32*>(value));
		break;
	case RTTI::eCorePropertyType::UINT64:
		v = QString("%1").arg(*reinterpret_cast<const u64*>(value));
		break;
	case RTTI::eCorePropertyType::FLOAT:
		v = QString("%1").arg(*reinterpret_cast<const float*>(value));
		break;
	case RTTI::eCorePropertyType::DOUBLE:
		v = QString("%1").arg(*reinterpret_cast<const double*>(value));
		break;
	case RTTI::eCorePropertyType::STRING:
	{
		const String* str = reinterpret_cast<const String*>(value);
		v = str->GetCStr();
		break;
	}
	case RTTI::eCorePropertyType::ENUM:
	{
		HEAVY_ASSERTE(prop.ImplData.get() != nullptr, "Invalid enum impl data!");
		const RTTI::EnumPropertyImplData* implData = static_cast<const RTTI::EnumPropertyImplData*>(prop.ImplData.get());
		i64 val;
		if (implData->EnumInfo->GetUnderlyingValueSize() == sizeof(i32))
			val = *reinterpret_cast<const i32*>(value);
		else if (implData->EnumInfo->GetUnderlyingValueSize() == sizeof(i64))
			val = *reinterpret_cast<const i64*>(value);
		else
			ASSERTE(false, "Unhadled value size!");

		v = implData->EnumInfo->GetEnumName(val);
		break;
	}
	case RTTI::eCorePropertyType::NONE:
		ASSERTE(false, "Invalid property type!");
		break;
	default:
		ASSERTE(false, "Unknown property type!");
	}

	QLineEdit* field = new QLineEdit();
	Fields.PushBack(field);
	parent->addRow(prop.Name.GetCStr(), field);
	field->setText(v);
}
