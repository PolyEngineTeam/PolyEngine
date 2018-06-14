#pragma once

#include "Widgets/PolyWidget.hpp"

class InspectorWidgetBase : public PolyWidget
{
public:
	InspectorWidgetBase(QWidget* parent) : PolyWidget(parent) {}

	virtual void InitializeConnections() = 0;
	virtual void UpdateInspector() = 0;
	virtual void ReloadInspector() = 0;
	virtual void Reset() = 0;;
};