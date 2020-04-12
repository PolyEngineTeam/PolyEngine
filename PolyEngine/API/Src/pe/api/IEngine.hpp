#pragma once

#include <pe/Defines.hpp>

namespace pe::api {
namespace app { class App; }
namespace ecs { class ISystem; class Scene; }

enum class eEngineUpdatePhase
{
    PREUPDATE,
    EDITOR,
    GAME,
    POSTUPDATE,
    _COUNT
};

class API_DLLEXPORT IEngine : public ::pe::core::BaseObject<>
{   
public:
    virtual void startGame() = 0;
    virtual void endGame() = 0;

    virtual void registerSystem(std::unique_ptr<ecs::ISystem>&& system, eEngineUpdatePhase order) = 0;
    virtual void update(std::chrono::duration<double> dt) = 0;
    virtual void selectiveUpdate(std::chrono::duration<double> dt, const std::vector<eEngineUpdatePhase>& phases) = 0;

    /// <summary>Returns current base world refference.</summary>
    /// <returns>Pointer to current world.</returns>
    virtual ecs::Scene* getActiveScene() = 0;
    virtual void loadDefaultScene() = 0;

    //#################### LEGACY API #######################
	// @todo Move this functionality to App.
    bool isQuitRequested() const;
	void requestGameQuit();
    
	bool shouldCaptureMouse() const;
	void setMouseCapture(bool enabled);

	bool shouldMouseBeVisible() const;
	void setMouseVisible(bool visible);

private:
    bool m_quitRequested = false; //stop the game
    bool m_mouseCaptureEnabled = false;
    bool m_mouseVisible = true;
};

}