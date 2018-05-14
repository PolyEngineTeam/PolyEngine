#pragma once

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>


//TODO(squares): create PolyControl
class SectionContainer : public QWidget 
{
public:
	SectionContainer(const QString& title, QWidget* parent = nullptr, int animationDuration = 100);

	void SetLayout(QLayout* layout);

private:
	QScrollArea* Content;

	QGridLayout* Layout;
	QToolButton* Button;
	QFrame* HLine;
	QParallelAnimationGroup* Animation;
	int AnimationDuration;
};