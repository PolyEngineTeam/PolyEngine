#pragma once

#include <QTextEdit>
#include <QBoxLayout>
#include "PolyWidget.hpp"

class LoggerWidget : public PolyWidget
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