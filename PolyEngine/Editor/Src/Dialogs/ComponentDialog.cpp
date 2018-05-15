#include "PolyEditorPCH.hpp"

ComponentDialog::ComponentDialog(::Entity* entity, bool removeComponents)
{
	setModal(true);
	setMaximumHeight(800);

	MainLayout = new QGridLayout(this);
	MainLayout->setColumnStretch(0, 1);
	MainLayout->setColumnStretch(1, 1);
	MainLayout->setColumnStretch(2, 1);

	GroupBox = new QGroupBox(this);
	GroupBoxLayout = new QGridLayout(this);
	ScrollArea = new QScrollArea(this);
	ScrollArea->setLayout(GroupBoxLayout);

	for (int i = 0; i < MAX_COMPONENTS_COUNT; ++i)
		if ((removeComponents && entity->HasComponent(i))
			|| (!removeComponents && !entity->HasComponent(i)))
		{
			// create component from ID, get its name and delete it

			QCheckBox* box = new QCheckBox(tr(QString::number(i).toLatin1()), this);
			GroupBoxLayout->addWidget(box, (int)CheckBoxes.GetSize(), 0);
			CheckBoxes.PushBack(box);
		}

	GroupBox->setLayout(GroupBoxLayout);
	MainLayout->addWidget(GroupBox, 0, 0, 1, 3);
}
