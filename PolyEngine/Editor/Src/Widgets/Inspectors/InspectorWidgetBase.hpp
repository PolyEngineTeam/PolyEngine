#pragma once

#include "Widgets/PolyWidget.hpp"

class InspectorManager;

class InspectorWidgetBase : public PolyWidget
{
public:
	InspectorWidgetBase(QWidget* parent, InspectorManager* mgr);

	virtual void InitializeConnections() = 0;
	virtual void Reset() = 0;

protected:
	InspectorManager* Manager;
};