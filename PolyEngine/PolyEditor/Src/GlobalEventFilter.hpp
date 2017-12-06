#pragma once

class GlobalEventFilter : public QObject
{
public:
	GlobalEventFilter();
	~GlobalEventFilter();

	bool eventFilter(QObject* watched, QEvent* event) override;
};