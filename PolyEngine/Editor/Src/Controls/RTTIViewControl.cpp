#include "PolyEditorPCH.hpp"

RTTIViewControl::RTTIViewControl(QWidget* parent)
	: QWidget(parent)
{
}

void RTTIViewControl::SetObject(RTTIBase* obj, bool debug)
{
	if (Layout)
		Layout.release();

	Layout = std::make_unique<QFormLayout>(this);
	setLayout(Layout.get());

	Object = obj;

	AddChild(Layout.get(), Object, debug);
}

void RTTIViewControl::UpdateViewer()
{
}

void RTTIViewControl::UpdateObject()
{
}

void RTTIViewControl::AddChild(QFormLayout* parent, RTTIBase* obj, bool debug)
{
	for (auto& child : obj->GetPropertyManager()->GetPropertyList())
	{
		ASSERTE(child.CoreType != RTTI::eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (!debug && child.Flags.IsSet(RTTI::ePropertyFlag::DONT_SERIALIZE))
			continue;

		void* ptr = ((char*)obj) + child.Offset;

		if (child.CoreType == RTTI::eCorePropertyType::NONE)
		{
			QFormLayout* layout = new QFormLayout();
			parent->addItem(layout);
			AddChild(layout, reinterpret_cast<RTTIBase*>(ptr), debug);
		}
		else
			AddItem(parent, ptr, child);
	}
}

void RTTIViewControl::AddItem(QFormLayout* parent, void* ptr, const RTTI::Property& prop)
{
	RTTIBaseControl* field;

	//switch (prop.CoreType)
	//{
	//case RTTI::eCorePropertyType::BOOL:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::BOOL>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::INT8:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::INT8>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::INT16:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::INT16>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::INT32:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::INT32>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::INT64:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::INT64>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::UINT8:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::UINT8>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::UINT16:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::UINT16>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::UINT32:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::UINT32>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::UINT64:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::UINT64>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::FLOAT:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::FLOAT>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::DOUBLE:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::DOUBLE>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::STRING:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::STRING>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::ENUM:
	//	field = RTTIBaseControl::TypeToControl<RTTI::eCorePropertyType::ENUM>::CreateControl(ptr, prop);
	//	break;
	//case RTTI::eCorePropertyType::NONE:
	//	ASSERTE(false, "Invalid property type!");
	//	break;
	//default:
	//	ASSERTE(false, "Unknown property type!");
	//}

	field = new StringControl(ptr, prop, RTTI::eCorePropertyType::STRING);
	Fields.PushBack(field);
	parent->addRow(prop.Name.GetCStr(), field->GetField());
}