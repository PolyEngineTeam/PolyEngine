#pragma once

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QWidget>


//TODO(squares): create PolyControl
class SectionControl : public QWidget 
{
public:
	SectionControl(const QString& title, QWidget* parent, int animationDuration = 100);

	void setContentLayout(QLayout& contentLayout);

private:
	std::unique_ptr<QScrollArea> Content;

	std::unique_ptr<QGridLayout> Layout;
	std::unique_ptr<QToolButton> Button;
	std::unique_ptr<QFrame> Line;
	std::unique_ptr<QParallelAnimationGroup> Animation;
	int AnimationDuration;
};