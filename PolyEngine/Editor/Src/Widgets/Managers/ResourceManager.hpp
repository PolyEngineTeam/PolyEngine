#pragma once

#include <QFormLayout>

#include "Widgets/PolyWidget.hpp"

class ResourceManager : public PolyWidget
{
public:
	ResourceManager() = default;

	void UpdateWidget();

private:
	QGridLayout* Layout;
};