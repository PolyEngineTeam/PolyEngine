#include "PolyEditorPCH.hpp"
#include "LoggerWidget.hpp" 

LoggerWidget::LoggerWidget(const QString& title, QWidget* parent)
	: PolyWidget(title, parent)
{
	Layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);
	TextEdit = new QTextEdit(this);
	TextEdit->setReadOnly(true);
	Layout->addWidget(TextEdit);
	Poly::gConsole.RegisterStream<EditorOutputStream>("console.log", this);
}

LoggerWidget::~LoggerWidget()
{
	Poly::gConsole.RegisterDefaultStream();
}

void LoggerWidget::AppendLog(const char * data)
{
	TextEdit->moveCursor(QTextCursor::End);
	TextEdit->insertPlainText(QString::fromLocal8Bit(data));
	TextEdit->moveCursor(QTextCursor::End);
	TextEdit->show();
}
