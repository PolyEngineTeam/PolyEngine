#pragma once

#include <Core.hpp>
#include <QObject>
#include <QtCore/qtimer.h>

using namespace Poly;

class CommandManagerException : public std::exception
{
public:
	CommandManagerException(const String& msg) : Msg(msg) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	String Msg;
};

class CommandManager : public QObject
{
public:
	CommandManager();
	~CommandManager() = default;

	template <typename S, typename... Args>
	void RegisterStream(Args&&... args)
	{
		constexpr bool isStream = std::is_base_of<Poly::OutputStream, S>::value; // Strange workaround to STATIC_ASSERTE macro on MSVC
		STATIC_ASSERTE(isStream, "Provided value is not stream!");
		if (CurrentStream)
			CurrentStream->OnUnregister();
		CurrentStream = std::make_unique<S>(std::forward<Args>(args)...);
		Ostream = std::make_unique<std::ostream>(CurrentStream.get());
	}

	void RunCommand(const String& cmd);

private:
	bool Running = false;

	String Command;

	std::unique_ptr<QTimer> Timer = nullptr;
	FILE* Stream = nullptr;
	static const size_t MaxBuffer = 256;
	char Buffer[MaxBuffer];
	std::unique_ptr<OutputStream> CurrentStream;
	std::unique_ptr<std::ostream> Ostream;

private slots:
	void ReadStdout();
};