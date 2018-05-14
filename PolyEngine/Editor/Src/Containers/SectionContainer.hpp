#pragma once

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>

class SectionContainer : public QWidget 
{
public:
	SectionContainer(const QString& title, QWidget* parent = nullptr, int animationDuration = 100);

	void SetLayout(QLayout* layout);
	void SetWidget(QWidget* widget);

private:
	void Init();

	QLayout* ContentLayout = nullptr;

	QScrollArea* ScrollArea;
	QGridLayout* MainLayout;

	QToolButton* Button;
	QFrame* HLine;
	QParallelAnimationGroup* Animation;
	int AnimationDuration;
};