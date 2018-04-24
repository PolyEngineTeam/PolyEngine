#pragma once

#include <RTTI/RTTI.hpp>
#include <QFormLayout>
#include <qwidget.h>

using namespace Poly;

class RTTIViewControl : public QWidget
{
public:
	RTTIViewControl();

	void SetObject(RTTIBase* obj, bool debug = false);

	void UpdateViewer();
	void UpdateObject();

private:
	void AddChild(QFormLayout* parent, const RTTIBase* obj, bool debug);
	void AddItem(QFormLayout* parent, const void* value, const RTTI::Property& prop);

	RTTIBase* Object;
	std::unique_ptr<QFormLayout> Layout;
	Dynarray<QObject*> Fields;

private slots:
	void OnResize(QResizeEvent *event);
};