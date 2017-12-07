#pragma once

class PolyEditorUi;

class GlobalEventFilter : public QObject
{
public:
	GlobalEventFilter(PolyEditorUi* ui);
	~GlobalEventFilter();

	PolyEditorUi* Ui;

	bool eventFilter(QObject* watched, QEvent* event) override;
};