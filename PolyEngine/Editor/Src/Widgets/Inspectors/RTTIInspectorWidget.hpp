#pragma once

#include <RTTI/RTTI.hpp>
#include <QFormLayout>

#include "Widgets/PolyWidget.hpp"

class ControlBase;
class SectionContainer;

using namespace Poly;

// Displays all properties of given object. 
// If any property has its own properties then SectionContainer is created.
class RTTIInspectorWidget : public PolyWidget
{
public:
	RTTIInspectorWidget(QWidget* parent);

	// Sets current object, if debug flag is true then all properties are displayed and can be edited by 
	// user even when property is not serialized.
	void SetObject(RTTIBase* obj, bool debug = false);

	// Updates object from inspector.
	void UpdateObject();

	// Update inspector view from object.
	void UpdateInspector();

	// Sestroys everything and creates again
	void ReloadInspector(bool debug = false);

private:
	// Add SectionContainer.
	SectionContainer* AddChild(RTTIBase* obj, const RTTI::Property& prop, bool debug);

	// Add control.
	void AddItem(QGridLayout* parent, int row, void* ptr, const RTTI::Property& prop);

	RTTIBase* Object;

	QGridLayout* Layout;

	Dynarray<ControlBase*> Fields;
	Dynarray<QLabel*> Labels;
	Dynarray<SectionContainer*> Sections;
};