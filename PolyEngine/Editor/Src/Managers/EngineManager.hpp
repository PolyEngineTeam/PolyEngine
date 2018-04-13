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

class EngineManager : public QObject, public IEditor
{
public:
	EngineManager();
	~EngineManager() = default;

	eEngineState GetEngineState() { return EngineState; }

	const String& GetAssetsPathConfigPath() const override { return AssetsPathConfigPath; }
	void Init() override;
	void Deinit() override;

	void InitEngine(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device
		, const String& assetsPathConfigPath);
	void DeinitEngine();

	void Edit();
	void Play();

private:
	std::unique_ptr<Engine> Engine = nullptr;
	QTimer Updater;

	String AssetsPathConfigPath = String::EMPTY;
	eEngineState EngineState = eEngineState::NONE;

private slots:
	void UpdatePhase();
};