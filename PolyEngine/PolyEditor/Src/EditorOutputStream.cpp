#include "PolyEditorPCH.hpp"
#include "EditorOutputStream.hpp"
#include "LoggerWidget.hpp"

EditorOutputStream::EditorOutputStream(const char* name, LoggerWidget* widget) : FileOutputStream(name)
{ 
	Logger = widget; 
}

void EditorOutputStream::Append(const char* data) 
{
	Logger->AppendLog(data);
	//Poly::FileOutputStream::Append(data);
}