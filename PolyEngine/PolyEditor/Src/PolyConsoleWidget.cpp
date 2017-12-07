#include "PolyEditorPCH.hpp"

PolyConsoleWidget::PolyConsoleWidget(const QString& title, QWidget* parent):
	QTextEdit(parent),
	PolyWidget(title, this)
{
}
