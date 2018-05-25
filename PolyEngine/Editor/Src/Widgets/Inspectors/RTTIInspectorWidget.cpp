#include "PolyEditorPCH.hpp"

#include <Resources/ResourceBase.hpp>

RTTIInspectorWidget::RTTIInspectorWidget(QWidget* parent)
	: PolyWidget(parent)
{
	Layout = new QGridLayout(this);
	Layout->setColumnStretch(0, 1);
	Layout->setColumnStretch(1, 3);

	setLayout(Layout);
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::SetObject(RTTIBase* obj, bool debug)
{
	QLayoutItem* child;

	while ((child = Layout->takeAt(0)) != 0)
		delete child;

	Object = obj;
	UpdateInspector(debug);
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::UpdateInspector(bool debug)
{
	int row = 0;

	for (auto& child : Object->GetPropertyManager()->GetPropertyList())
	{
		if (!debug && child.Flags.IsSet(RTTI::ePropertyFlag::DONT_SERIALIZE))
			continue;

		void* ptr = ((char*)Object) + child.Offset;

		if (child.CoreType == RTTI::eCorePropertyType::CUSTOM)
			//&& !IsOfType<EntityTransform>(&child)
			//&& !child.Type.isTypeDerivedFrom<ResourceBase>())
		{
			Layout->addWidget(AddChild((RTTIBase*)ptr, child, debug), row, 0, 1, 2);
		}
		else
			AddItem(Layout, row, ptr, child);

		++row;
	}
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::UpdateObject()
{
	for (auto field : Fields)
		field->UpdateObject();
}

//------------------------------------------------------------------------------
SectionContainer* RTTIInspectorWidget::AddChild(RTTIBase* obj, const RTTI::Property& prop, bool debug)
{
	SectionContainer* section = new SectionContainer(prop.Name.GetCStr(), this);
	Sections.PushBack(section);

	int row = 0;

	for (auto& child : Object->GetPropertyManager()->GetPropertyList())
	{
		if (!debug && child.Flags.IsSet(RTTI::ePropertyFlag::DONT_SERIALIZE))
			continue;

		void* ptr = ((char*)Object) + child.Offset;

		if (child.CoreType == RTTI::eCorePropertyType::CUSTOM)
			//&& !IsOfType<EntityTransform>(&child)
			//&& !child.Type.isTypeDerivedFrom<ResourceBase>())
		{
			Layout->addWidget(AddChild((RTTIBase*)ptr, child, debug), row, 0, 1, 2);
		}
		else
			AddItem(Layout, row, ptr, child);

		++row;
	}

	return section;
}

//------------------------------------------------------------------------------
void RTTIInspectorWidget::AddItem(QGridLayout* parent, int row, void* ptr, const RTTI::Property& prop)
{
	ControlBase* field;

	if (prop.CoreType == RTTI::eCorePropertyType::CUSTOM)
	{
		// TODO(squares): uncomment this when ptr serilization will be available
		//if (IsOfType<EntityTransform>(&prop))
		//	field = new TransformControl(this);
		//else if (prop.Type.isTypeDerivedFrom<ResourceBase>())
		//	field = new ResourceControl(this);
		//else
			field = new PlaceHolderControl(this);
	}
	else
		field = ControlBase::CreateControl(this, prop.CoreType);

	field->SetObject(ptr, &prop);

	if (field->ContainsLabel())
	{
		parent->addWidget(field, row, 0, 1, 2);
	}
	else
	{
		QLabel* label = new QLabel();
		label->setText(prop.Name.GetCStr());
		parent->addWidget(label, row, 0);
		parent->addWidget(field, row, 1);
	}

	Fields.PushBack(field);
}