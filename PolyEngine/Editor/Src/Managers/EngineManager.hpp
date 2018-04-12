#pragma once

#include <QObject>
#include <QTimer>
#include <Engine.hpp>

using namespace Poly;

class EngineManagerException : public std::exception
{
public:
	EngineManagerException(const String& msg) : Msg(msg) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	String Msg;
};

enum class eEngineState
{
	EDIT,
	GAMEPLAY,
	NONE,
	_COUNT
};

class EngineManager : public QObject, public IEditorProxy
{
public:
	EngineManager();
	~EngineManager() = default;

	const String& GetAssetsPathConfigPath() const override { return AssetsPathConfigPath; }
	eEngineState GetEngineState() { return EngineState; }
	
	void Init(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device
		, const String& assetsPathConfigPath);
	void Deinit();

	void Edit();
	void Play();

private:
	std::unique_ptr<Engine> Engine = nullptr;
	QTimer Updater;
	
	eEngineState EngineState = eEngineState::NONE;
	String AssetsPathConfigPath = String::EMPTY;

private slots:
	void UpdatePhase();
};