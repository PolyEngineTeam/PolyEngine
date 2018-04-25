#pragma once

#include <RTTI/RTTI.hpp>
#include <QFormLayout>

#include "PolyWidget.hpp"
#include "Controls/RTTIViewControl.hpp"

using namespace Poly;

class RTTIViewerWidget : public PolyWidget
{
public:
	RTTIViewerWidget(const QString& title, QWidget* parent);

	void SetObject(RTTIBase* obj, bool debug = false);

	void UpdateViewer();
	void UpdateObject();

private:
	std::unique_ptr<RTTIViewControl> Control;
};