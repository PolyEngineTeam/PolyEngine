#pragma once

#include <RTTI/RTTI.hpp>
#include <QFormLayout>

#include "PolyWidget.hpp"

using namespace Poly;

class RTTIViewerWidget : public PolyWidget
{
public:
	RTTIViewerWidget(const QString& title, QWidget* parent);

	void SetObject(RTTIBase* obj, bool debug = false);

	//void UpdateViewer();
	//void UpdateObject();

private:
	void AddChild(QFormLayout* parent, const RTTIBase* obj, bool debug);
	void AddItem(QFormLayout* parent, const void* value, const RTTI::Property& prop);

	RTTIBase* Object;
	std::unique_ptr<QFormLayout> Layout;
	Dynarray<QObject*> Fields;

private slots:
	void OnResize(QResizeEvent *event);
};