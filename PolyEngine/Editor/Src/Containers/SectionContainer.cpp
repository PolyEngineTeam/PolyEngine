#include "PolyEditorPCH.hpp"

#include <QPropertyAnimation>

SectionContainer::SectionContainer(const QString& title, QWidget* parent, int animationDuration)
	: QWidget(parent), AnimationDuration(animationDuration)
{
	// create main container area
	Content = new QScrollArea(this);
	Content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	// start out collapsed
	Content->setMaximumHeight(0);
	Content->setMinimumHeight(0);

	// create layout for all items like arrow or separator
	Layout = new QGridLayout(this);
	Layout->setVerticalSpacing(0);
	Layout->setContentsMargins(0, 0, 0, 0);

	// create nice arrow button
	Button = new QToolButton(this);
	Button->setStyleSheet("QToolButton {border: none;}");
	Button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	Button->setArrowType(Qt::ArrowType::RightArrow);
	Button->setText(title);
	Button->setCheckable(true);
	Button->setChecked(false);

	// create separator
	HLine = new QFrame(this);
	HLine->setFrameShape(QFrame::HLine);
	HLine->setFrameShadow(QFrame::Sunken);
	HLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	// create nice animation
	Animation = new QParallelAnimationGroup(this);
	Animation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
	Animation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
	Animation->addAnimation(new QPropertyAnimation(Content, "maximumHeight"));

	// add everything to main layout
	Layout->addWidget(Button, 0, 1, 1, 1, Qt::AlignLeft);
	Layout->addWidget(HLine, 0, 3, 1, 1);
	Layout->addWidget(Content, 1, 1, 1, 3);
	setLayout(Layout);

	// connect animatiom to button
	QObject::connect(Button, &QToolButton::clicked, [this](const bool checked)
	{
		Button->setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
		Animation->setDirection(checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
		Animation->start();
	});
}

void SectionContainer::SetLayout(QLayout* layout)
{
	delete Content->layout();
	Content->setLayout(layout);
	const auto collapsedHeight = sizeHint().height() - Content->maximumHeight();
	auto contentHeight = layout->sizeHint().height();

	for (int i = 0; i < Animation->animationCount() - 1; ++i)
	{
		QPropertyAnimation* SectionAnimation = static_cast<QPropertyAnimation *>(Animation->animationAt(i));
		SectionAnimation->setDuration(AnimationDuration);
		SectionAnimation->setStartValue(collapsedHeight);
		SectionAnimation->setEndValue(collapsedHeight + contentHeight);
	}

	QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation *>(Animation->animationAt(Animation->animationCount() - 1));
	contentAnimation->setDuration(AnimationDuration);
	contentAnimation->setStartValue(0);
	contentAnimation->setEndValue(contentHeight);
}
