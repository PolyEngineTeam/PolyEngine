#include "PolyDockManager.hpp"
#include "PolyEditorPCH.hpp"

void PolyDockManager::WidgetCatchEvent(PolyWidget* catched)
{
	DraggedWidget = catched;
}

void PolyDockManager::ProcessEvent(QEvent* event)
{
	if (DraggedWidget != nullptr)
		if (event->type() == QEvent::MouseMove)
			WidgetMoveEvent(event);
		else if (event->type() == QEvent::MouseButtonRelease)
			WidgetDropEvent(event);
}

void PolyDockManager::WidgetMoveEvent(QEvent * event)
{
	QPoint mousePos = ((QMouseEvent*)event)->pos() + DraggedWidget->GetDockWidget()->pos();
	MouseOver = nullptr;

	for (int i = 0; i < gApp->Ui.Windows.GetSize(); i++)
	{
		QPoint diff = mousePos - gApp->Ui.Windows[i]->pos();

		if (diff.x() >= 0 && diff.x() < gApp->Ui.Windows[i]->size().width() &&
			diff.y() >= 0 && diff.y() < gApp->Ui.Windows[i]->size().height())
		{
			MouseOver = gApp->Ui.Windows[i];
			break;
		}
	}
}

void PolyDockManager::WidgetDropEvent(QEvent* event)
{
	if (MouseOver)
	{
		if (MouseOver == ((PolyWindow*)DraggedWidget->GetDockWidget()->parent()))
			DraggedWidget = nullptr;
		else
		{
			PolyWindow* oldOwner = DraggedWidget->GetOwner();
			MouseOver->AddWidget(Qt::DockWidgetArea::TopDockWidgetArea, DraggedWidget);

			if (oldOwner != gApp->Ui.MainWindow
				&& oldOwner->WidgetsCount() == 1)
			{
				oldOwner->close();
			}
		}
	}
	else
	{
		PolyWindow* wnd = new PolyWindow;
		wnd->resize(DraggedWidget->GetDockWidget()->size());
		wnd->move(DraggedWidget->GetDockWidget()->pos());
		wnd->AddWidget(Qt::DockWidgetArea::TopDockWidgetArea, DraggedWidget);
		wnd->show();
		gApp->Ui.Windows.PushBack(std::move(wnd));
	}

	DraggedWidget = nullptr;
}
