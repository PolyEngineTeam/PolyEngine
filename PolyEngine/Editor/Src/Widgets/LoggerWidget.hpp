#pragma once

#include <QTextEdit>
#include <QBoxLayout>
#include "Windows/PolyDockWindow.hpp"

class LoggerWidget : public PolyDockWindow
{
public:
	explicit LoggerWidget(const QString& title, QWidget* parent = nullptr);
	~LoggerWidget();

	void AppendLog(const char* data);
protected:

private:
	QTextEdit* TextEdit;
	QBoxLayout* Layout;
};