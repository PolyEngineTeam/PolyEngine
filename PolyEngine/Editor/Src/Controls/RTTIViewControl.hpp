#pragma once

#include <RTTI/RTTI.hpp>
#include <QFormLayout>
#include <qwidget.h>

using namespace Poly;

class RTTIViewControl : public QWidget
{
public:
	RTTIViewControl(QWidget* parent);

	void SetObject(RTTIBase* obj, bool debug = false);

	void UpdateViewer();
	void UpdateObject();

private:
	void AddChild(QFormLayout* parent, RTTIBase* obj, bool debug);
	void AddItem(QFormLayout* parent, void* ptr, const RTTI::Property& prop);

	RTTIBase* Object;
	std::unique_ptr<QFormLayout> Layout;
	Dynarray<QObject*> Fields;
};