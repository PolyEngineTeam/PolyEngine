#include "PolyEditorPCH.hpp"

GlobalEventFilter::GlobalEventFilter(PolyEditorUi* ui)
{
	Ui = ui;
}

GlobalEventFilter::~GlobalEventFilter()
{
}

bool GlobalEventFilter::eventFilter(QObject* watched, QEvent* event)
{
	UNUSED(watched);
	if (gApp->draggedWidget != nullptr)
	{
		if (event->type() == QEvent::MouseMove)
		{
			QPoint mousePos = ((QMouseEvent*)event)->pos() + gApp->draggedWidget->pos();
			gApp->MouseOver = nullptr;

			for (size_t i = 0; i < Ui->Windows.GetSize(); i++)
			{
				QPoint diff = mousePos - Ui->Windows[i]->pos();

				if (diff.x() >= 0 && diff.x() < Ui->Windows[i]->size().width() &&
					diff.y() >= 0 && diff.y() < Ui->Windows[i]->size().height())
				{
					gApp->MouseOver = Ui->Windows[i];
				}
			}
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			if (gApp->MouseOver)
			{
				gApp->draggedWidget->setParent(gApp->MouseOver);
				// dock in existing window;
			}
			else 
			{
				PolyWindow* wnd = new PolyWindow;
				wnd->resize(gApp->draggedWidget->size());
				wnd->move(gApp->draggedWidget->pos());
				wnd->AddWidget(gApp->draggedWidget, Qt::DockWidgetArea::TopDockWidgetArea);
				wnd->show();
				Ui->Windows.PushBack(std::move(wnd));
			}

			gApp->draggedWidget = nullptr;
		}
	}

	return false; // return false when you want qt to forward this event to receiver
}
