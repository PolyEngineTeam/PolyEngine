#include "PolyEditorPCH.hpp"
#include "LoggerWidget.hpp" 

LoggerWidget::LoggerWidget(const QString& title, QWidget* parent):
	PolyWidget(title, this), TextEdit(this)
{
	Poly::gConsole.RegisterStream<EditorOutputStream>("console.log", this);
	TextEdit.setReadOnly(true);
	AppendLog("boop");
}

LoggerWidget::~LoggerWidget()
{
	Poly::gConsole.RegisterDefaultStream();
}

void LoggerWidget::AppendLog(const char * data)
{
	QString str(data);
	TextEdit.append(str);
	TextEdit.show();
}
