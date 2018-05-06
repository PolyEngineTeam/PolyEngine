#include "PolyEditorPCH.hpp"

RTTIViewerWidget::RTTIViewerWidget()
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

void RTTIViewerWidget::UpdateViewer()
{
}

void RTTIViewerWidget::UpdateObject()
{
}

void RTTIViewerWidget::AddChild(QFormLayout* parent, RTTIBase* obj, bool debug)
{
	for (auto& child : obj->GetPropertyManager()->GetPropertyList())
	{
		ASSERTE(child.CoreType != RTTI::eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (!debug && child.Flags.IsSet(RTTI::ePropertyFlag::DONT_SERIALIZE))
			continue;

		void* ptr = ((char*)obj) + child.Offset;

		if (child.CoreType == RTTI::eCorePropertyType::CUSTOM)
		{
			QFormLayout* layout = new QFormLayout();
			parent->addItem(layout);
			AddChild(layout, reinterpret_cast<RTTIBase*>(ptr), debug);
		}
		else
			AddItem(parent, ptr, child);
	}
}

void RTTIViewerWidget::AddItem(QFormLayout* parent, void* ptr, const RTTI::Property& prop)
{
	ControlBase* field = ControlBase::CreateControl(prop.CoreType, this);

	Fields.PushBack(field);
	parent->addRow(prop.Name.GetCStr(), field);
}