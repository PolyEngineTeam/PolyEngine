#pragma once

#include "PolyWindow.hpp"
#include "PolyEditorUi.hpp"

class PolyMainWindow : public PolyWindow
{
	friend class PolyEditorUi;

	Q_OBJECT

public:
	PolyMainWindow(QWidget* parent = nullptr);
	~PolyMainWindow();

private slots:
	void CloseProject();
	void Quit();
	void Undo();
	void Redo();
	void AddWindow();
	void Build();
	void ContactUs();
};