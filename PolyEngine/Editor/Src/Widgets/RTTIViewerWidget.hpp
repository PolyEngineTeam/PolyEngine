#pragma once

#include <RTTI/RTTI.hpp>
#include <QFormLayout>

#include "Widgets/PolyWidget.hpp"

class ControlBase;

using namespace Poly;

class RTTIViewerWidget : public PolyWidget
{
public:
	RTTIViewerWidget() = default;

	void SetObject(RTTIBase* obj, bool debug = false);

	void UpdateViewer();
	void UpdateObject();

private:
	void AddChild(QGridLayout* parent, RTTIBase* obj, bool debug);
	void AddItem(QGridLayout* parent, void* ptr, const RTTI::Property& prop);

	RTTIBase* Object;

	QGridLayout* Layout;

	Dynarray<ControlBase*> Fields;
	Dynarray<QLabel*> Labels;
};