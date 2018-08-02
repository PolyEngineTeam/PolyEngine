#pragma once

#include <RTTI/RTTI.hpp>
#include <QFormLayout>

#include "Widgets/PolyWidget.hpp"

class IControlBase;
class SectionContainer;
class Command;

using namespace Poly;

// Displays all properties of given object. 
// If any property has its own properties then SectionContainer is created.
class RTTIInspectorWidget : public PolyWidget
{
	Q_OBJECT

public:
	RTTIInspectorWidget(QWidget* parent);

	// Sets current object, if debug flag is true then all properties are displayed and can be edited by 
	// user even when property is not serialized.
	void SetObject(RTTIBase* obj, bool debug);

	// Updates object from inspector.
	void UpdateObject();

	// Update inspector view from object.
	void UpdateInspector();

	// Destroys everything and creates again
	void ReloadInspector(bool debug = false);

signals:
	void ControlObjectUpdated(Command* c);

private:
	void ControlObjectUpdatedSlot(Command* c);
	
	// Add SectionContainer.
	SectionContainer* AddChild(RTTIBase* obj, const RTTI::Property& prop, bool debug);

	// Add control.
	void AddItem(QGridLayout* parent, int row, void* ptr, const RTTI::Property& prop);

	RTTIBase* Object;

	QGridLayout* Layout;

	Dynarray<IControlBase*> Fields;
	Dynarray<QLabel*> Labels;
	Dynarray<SectionContainer*> Sections;
};