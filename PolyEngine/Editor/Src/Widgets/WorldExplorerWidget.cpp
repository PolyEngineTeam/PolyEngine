#include "PolyEditorPCH.hpp"

WorldExplorerWidget::WorldExplorerWidget(const QString& title, QWidget* parent)
	: PolyWidget(title, parent)
{
	Tree = std::make_unique<QTreeWidget>(this);
	Tree->move(12, 120);
}

void WorldExplorerWidget::SetWorld(::World* world)
{
	if (Tree)
		Tree.release();
	
	World = world;
	
	Tree = std::make_unique<QTreeWidget>(this);
	Tree->move(12, 12);
	Tree->resize(400, 600);
	//Tree->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	
	for (auto child : world->GetRoot().GetChildren())
	{
		QTreeWidgetItem* entity = new QTreeWidgetItem(Tree.get());
		//entity->setText(0, String::From((int)child->GetID().GetHash()).GetCStr());
		entity->setText(0, "dupa");

		for (int i = 0; i < 10; ++i)
		{
			QTreeWidgetItem* component = new QTreeWidgetItem(entity);
			component->setText(0, String::From((int)child->GetID().GetHash()).GetCStr());
			entity->addChild(component);
		}
	}
}
