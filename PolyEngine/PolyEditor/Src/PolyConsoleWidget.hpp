#pragma once

#include <QTextEdit>
#include "PolyWidget.hpp"

class PolyConsoleWidget : public QTextEdit, public PolyWidget
{
public:
	explicit PolyConsoleWidget(const QString& title, QWidget* parent = nullptr);


protected:

private:
};