#include "PolyEditorPCH.hpp"
#include "EditorApp.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	EditorApp w;
	w.show();
	return a.exec();
}
