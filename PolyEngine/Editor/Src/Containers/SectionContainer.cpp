#include "PolyEditorPCH.hpp"

#include <QPropertyAnimation>

SectionContainer::SectionContainer(const QString& title, QWidget* parent, int animationDuration)
	: QWidget(parent), AnimationDuration(animationDuration)
{
	// create main container area
	ScrollArea = new QScrollArea(this);
	ScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// start out collapsed
	ScrollArea->setMaximumHeight(0);
	ScrollArea->setMinimumHeight(0);

	// create layout for all items like arrow or separator
	MainLayout = new QGridLayout(this);
	MainLayout->setVerticalSpacing(0);
	MainLayout->setContentsMargins(0, 0, 0, 0);

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
	Animation->addAnimation(new QPropertyAnimation(ScrollArea, "maximumHeight"));

	// add everything to main layout
	MainLayout->addWidget(Button, 0, 1, 1, 1, Qt::AlignLeft);
	MainLayout->addWidget(HLine, 0, 3, 1, 1);
	MainLayout->addWidget(ScrollArea, 1, 1, 1, 3);
	setLayout(MainLayout);

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
	if (ContentLayout)
	{
		QLayoutItem* child;

		while ((child = ContentLayout->takeAt(0)) != 0)
			delete child;

		delete ContentLayout;
	}

	ContentLayout = layout;
	Init();
}

void SectionContainer::SetWidget(QWidget* widget)
{
	if (ContentLayout)
	{
		QLayoutItem* child;

		while ((child = ContentLayout->takeAt(0)) != 0)
			delete child;

		delete ContentLayout;
	}

	ContentLayout = new QGridLayout();
	ContentLayout->addWidget(widget);
	static_cast<QGridLayout*>(ContentLayout)->setVerticalSpacing(0);
	static_cast<QGridLayout*>(ContentLayout)->setContentsMargins(0, 0, 0, 0);

	Init();
}

void SectionContainer::Init()
{
	ScrollArea->setLayout(ContentLayout);
	const auto collapsedHeight = sizeHint().height() - ScrollArea->maximumHeight();
	auto contentHeight = ContentLayout->sizeHint().height();

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

