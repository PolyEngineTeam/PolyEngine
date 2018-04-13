#include "PolyEditorPCH.hpp"
#include "LoggerWidget.hpp" 

class LoggerWidget;

class EditorOutputStream : public Poly::FileOutputStream
{
public:
	EditorOutputStream(const char* name, LoggerWidget* widget)
		: FileOutputStream(name)
	{
		Logger = widget;
	}

	void Append(const char* data) override
	{
		Logger->AppendLog(data);
		Poly::FileOutputStream::Append(data);
	}
private:
	LoggerWidget* Logger = nullptr;
};


LoggerWidget::LoggerWidget(const QString& title, QWidget* parent)
	: PolyWidget(title, parent)
{
	Layout = new QBoxLayout(QBoxLayout::Direction::LeftToRight, this);
	TextEdit = new QTextEdit(this);
	TextEdit->setReadOnly(true);
	Layout->addWidget(TextEdit);

	if (title == "Console")
		Poly::gConsole.RegisterStream<EditorOutputStream>("console.log", this);
	else if (title == "Cmd")
		gApp->CommandMgr.RegisterStream<EditorOutputStream>("cmd.log", this);
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
