#pragma once

#include "Widgets/PolyWidget.hpp"

class InspectorWidgetBase : public PolyWidget
{
public:
	InspectorWidgetBase(QWidget* parent) : PolyWidget(parent) {}

	virtual void InitializeConnections() = 0;
	virtual void Reset() = 0;
};