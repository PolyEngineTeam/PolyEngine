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

class EngineManager : public QObject, public IEditorProxy
{
public:
	EngineManager();
	~EngineManager() = default;
	
	void Init(std::unique_ptr<IGame> game, std::unique_ptr<IRenderingDevice> device
		, const String& assetsPathConfigPath);
	const String& GetAssetsPathConfigPath() override { return AssetsPathConfigPath; }

private:
	std::unique_ptr<QTimer> Updater = nullptr;
	std::unique_ptr<Engine> Engine = nullptr;
	String AssetsPathConfigPath = String::EMPTY;

private slots:
	void UpdatePhase();
};