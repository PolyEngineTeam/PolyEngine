#pragma once

#include "PolyWindow.hpp"
#include "PolyMainWindowUi.hpp"

class PolyMainWindow : public PolyWindow
{
	friend class PolyMainWindowUi;

	Q_OBJECT

public:
	PolyMainWindow(QWidget* parent = nullptr);
	~PolyMainWindow();

private:
	PolyMainWindowUi Ui;

private slots:
	void CloseProject();
	void Quit();
	void Undo();
	void Redo();
	void AddWindow();
	void Build();
	void ContactUs();
};