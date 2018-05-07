#include "PolyEditorPCH.hpp"

RTTIViewerWidget::RTTIViewerWidget()
{
}

void RTTIViewerWidget::SetObject(RTTIBase* obj, bool debug)
{
	if (Layout)
		Layout.release();

	Layout = std::make_unique<QGridLayout>(this);
	setLayout(Layout.get());

	Layout->setColumnStretch(0, 1);
	Layout->setColumnStretch(1, 3);

	Layout->setRowStretch(50, 1);

	Object = obj;

	AddChild(Layout.get(), Object, debug);
}

void RTTIViewerWidget::UpdateViewer()
{
}

void RTTIViewerWidget::UpdateObject()
{
}

void RTTIViewerWidget::AddChild(QGridLayout* parent, RTTIBase* obj, bool debug)
{
	for (auto& child : obj->GetPropertyManager()->GetPropertyList())
	{
		ASSERTE(child.CoreType != RTTI::eCorePropertyType::UNHANDLED, "Invalid type in property declaration!");
		if (!debug && child.Flags.IsSet(RTTI::ePropertyFlag::DONT_SERIALIZE))
			continue;

		void* ptr = ((char*)obj) + child.Offset;

		// use section
		//if (child.CoreType == RTTI::eCorePropertyType::CUSTOM)
		//{
		//	QGridLayout* layout = new QGridLayout();
		//	parent->addItem(layout);
		//	AddChild(layout, reinterpret_cast<RTTIBase*>(ptr), debug);
		//}
		//else
			AddItem(parent, ptr, child);
	}
}

void RTTIViewerWidget::AddItem(QGridLayout* parent, void* ptr, const RTTI::Property& prop)
{
	ControlBase* field = ControlBase::CreateControl(prop.CoreType);
	field->SetObject(ptr, &prop);

	QLabel* label = new QLabel();
	label->setText(prop.Name.GetCStr());
	parent->addWidget(label, (int)Fields.GetSize(), 0);
	parent->addWidget(field, (int)Fields.GetSize(), 1);

	Fields.PushBack(field);
}