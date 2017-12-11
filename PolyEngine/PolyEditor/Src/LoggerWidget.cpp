#include "PolyEditorPCH.hpp"
#include "LoggerWidget.hpp" 

LoggerWidget::LoggerWidget(const QString& title, QWidget* parent):
	PolyWidget(title, this), TextEdit(this)
{
	Poly::gConsole.RegisterStream<EditorOutputStream>("console.log", this);
	TextEdit.setReadOnly(true);
}

LoggerWidget::~LoggerWidget()
{
	Poly::gConsole.RegisterDefaultStream();
}

void LoggerWidget::AppendLog(const char * data)
{
	TextEdit.moveCursor(QTextCursor::End);
	TextEdit.insertPlainText(QString::fromLocal8Bit(data));
	TextEdit.moveCursor(QTextCursor::End);
	TextEdit.show();
}
