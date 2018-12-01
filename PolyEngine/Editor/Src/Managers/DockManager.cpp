#include <PolyEditorPCH.hpp>

void DockManager::WidgetCatchEvent(PolyDockWindow* catched)
{
	DraggedWidget = catched;
}

void DockManager::ProcessEvent(QEvent* event)
{
	if (DraggedWidget != nullptr)
	{
		if (event->type() == QEvent::MouseMove)
			WidgetMoveEvent(event);
		else if (event->type() == QEvent::MouseButtonRelease)
			WidgetDropEvent();
	}
}

void DockManager::WidgetMoveEvent(QEvent* event)
{
	QPoint mousePos = ((QMouseEvent*)event)->pos() + DraggedWidget->pos();
	MouseOver = nullptr;

	for (size_t i = 0; i < gApp->Ui.Windows.GetSize(); i++)
	{
		QPoint diff = mousePos - gApp->Ui.Windows[i]->pos();

		if (diff.x() >= 0 && diff.x() < gApp->Ui.Windows[i]->size().width() &&
			diff.y() >= 0 && diff.y() < gApp->Ui.Windows[i]->size().height())
		{
			MouseOver = gApp->Ui.Windows[i];

			// @fixme: it doesn't work because when any widget that already is in window is docked 
			// in LeftDockWidgetArea and we want to dock this widget there it will be docked beside previous widget,
			// and any other DockWidgetArea behaviours so.

			//if (diff.x() < MouseOver->size().height() / 3)
			//	DraggedWidgetDockArea = Qt::DockWidgetArea::TopDockWidgetArea;
			//else if (diff.x() > MouseOver->size().height() / 3 * 2)
			//	DraggedWidgetDockArea = Qt::DockWidgetArea::BottomDockWidgetArea;
			//else if (diff.y() < MouseOver->size().width() / 5)
			//	DraggedWidgetDockArea = Qt::DockWidgetArea::LeftDockWidgetArea;
			//else if (diff.y() > MouseOver->size().width() / 5 * 4)
			//	DraggedWidgetDockArea = Qt::DockWidgetArea::RightDockWidgetArea;
			//else
			//	DraggedWidgetDockArea = Qt::DockWidgetArea::LeftDockWidgetArea;

			break;
		}
	}
}

void DockManager::WidgetDropEvent()
{
	if (MouseOver)
	{
		if (MouseOver == ((PolyWindow*)DraggedWidget->parent()))
			DraggedWidget = nullptr;
		else
		{
			PolyWindow* oldOwner = DraggedWidget->GetOwner();
			MouseOver->AddDockWindow(DraggedWidgetDockArea, DraggedWidget);

			if (oldOwner != gApp->Ui.MainWindow
				&& oldOwner->DockWindowsCount() == 1)
			{
				oldOwner->close();
			}
		}
	}
	else
	{
		PolyWindow* wnd = new PolyWindow;
		wnd->resize(DraggedWidget->size());
		wnd->move(DraggedWidget->pos());
		wnd->AddDockWindow(Qt::DockWidgetArea::TopDockWidgetArea, DraggedWidget);
		wnd->show();
		gApp->Ui.Windows.PushBack(std::move(wnd));
	}

	DraggedWidget = nullptr;
}