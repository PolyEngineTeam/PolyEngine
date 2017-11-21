#include "PolyEditorPCH.hpp"
#include "EditorMainWindow.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	EditorApplication w;
	w.show();
	return a.exec();
}
