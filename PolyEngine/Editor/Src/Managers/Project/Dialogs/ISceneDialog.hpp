#pragma once

#include <Engine.hpp>

#include "Dialogs/IDialog.h"

class ISceneDialog : public IDialog
{
public:
	// Creates scene according to user preferences and returns pointer to this new scene.
	virtual Poly::Scene* AddScene() = 0;
	// Removes scene selected by user, if given scene can be highlighted at dialog startup for better experience.
	virtual void RemoveScene(Poly::Scene* predefinedScene = nullptr) = 0;
};