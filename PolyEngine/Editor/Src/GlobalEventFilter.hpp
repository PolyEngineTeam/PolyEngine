#pragma once

class GlobalEventFilter : public QObject
{
public:
	GlobalEventFilter() = default;

	bool eventFilter(QObject* watched, QEvent* event) override;
};