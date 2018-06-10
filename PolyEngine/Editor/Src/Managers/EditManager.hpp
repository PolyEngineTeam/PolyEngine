#pragma once

#include <qobject.h>
#include <ECS/Entity.hpp>

using namespace Poly;

class EditManager : public QObject
{
public:
	EditManager();

private:

private slots:
	void SelectedEntityChanged(Entity* entity);
};