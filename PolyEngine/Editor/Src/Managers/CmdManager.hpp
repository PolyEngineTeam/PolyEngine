#pragma once

#include <pe/Defines.hpp>
#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>
#include <pe/core/utils/OutputStream.hpp>
#include <pe/core/utils/Logger.hpp>

using namespace Poly;

class CmdManagerException : public std::exception
{
public:
	CmdManagerException(const ::pe::core::storage::String& msg) : Msg((const char*)msg.GetCStr()) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	::pe::core::storage::String Msg;
};

class CmdManager : public QObject
{
public:
	CmdManager();
	~CmdManager() = default;

	template <typename S, typename... Args>
	void RegisterStream(Args&&... args)
	{
		constexpr bool isStream = std::is_base_of<pe::core::utils::OutputStream, S>::value; // Strange workaround to STATIC_ASSERTE macro on MSVC
		STATIC_ASSERTE(isStream, "Provided value is not stream!");
		if (CurrentStream)
			CurrentStream->OnUnregister();
		CurrentStream = std::make_unique<S>(std::forward<Args>(args)...);
		Ostream = std::make_unique<std::ostream>(CurrentStream.get());
	}

	void RunCommand(const ::pe::core::storage::String& cmd);

private:
	bool Running = false;

	::pe::core::storage::String Command;

	std::unique_ptr<QTimer> Timer = nullptr;
	FILE* Stream = nullptr;
	static const size_t MaxBuffer = 256;
	char Buffer[MaxBuffer];
	std::unique_ptr<::pe::core::utils::OutputStream> CurrentStream;
	std::unique_ptr<std::ostream> Ostream;

private slots:
	void ReadStdout();
};