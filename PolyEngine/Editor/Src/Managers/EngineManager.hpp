#pragma once

#include <QObject>
#include <QTimer>
#include <Engine.hpp>

class EngineManagerException : public std::exception
{
public:
	EngineManagerException(const Poly::String& msg) : Msg(msg) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	Poly::String Msg;
};

class EngineManager : public QObject
{
public:
	EngineManager();
	~EngineManager() = default;
	
	void Init(std::unique_ptr<Poly::IGame> game, std::unique_ptr<Poly::IRenderingDevice> device);

private:
	std::unique_ptr<QTimer> Updater = nullptr;
	std::unique_ptr<Poly::Engine> Engine = nullptr; 

private slots:
	void UpdatePhase();
};