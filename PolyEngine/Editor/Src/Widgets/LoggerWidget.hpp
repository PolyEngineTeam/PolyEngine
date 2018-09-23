#pragma once

#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qboxlayout.h>
#include "Widgets/PolyWidget.hpp"

enum class eLoggerType
{
	CONSOLE, // gConsole output
	CMD, // command manager output
	_COUNT
};

class LoggerWidget : public PolyWidget
{
public:
	explicit LoggerWidget(eLoggerType type);
	~LoggerWidget();

	void AppendLog(const char* data);
protected:

private:
	QTextEdit* TextEdit;
	QBoxLayout* Layout;
};