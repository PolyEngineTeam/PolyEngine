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
};