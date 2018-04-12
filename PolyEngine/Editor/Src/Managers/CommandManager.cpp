#include <PolyEditorPCH.hpp>

CommandManager::CommandManager()
{
	// setup timer for reading from stream
	Timer = std::make_unique<QTimer>(this);
	connect(Timer.get(), &QTimer::timeout, this, &CommandManager::ReadStdout);
}

void CommandManager::RunCommand(const String& cmd)
{
	Command = cmd;

	// execute command and return output stream
#ifdef _WIN32
	Stream = _popen((Command + String(" 2>&1")).GetCStr(), "r");
#else
	Stream = popen((Command + String(" 2>&1")).GetCStr(), "r");
#endif

	// display some nice info
	*Ostream << "\n\n> ----------     \"" << Command << "\" started...     ----------\n";

	Running = true;
	Timer->start(0);
}

void CommandManager::ReadStdout()
{
	if (Running && !feof(Stream))
	{
		if (fgets(Buffer, MaxBuffer, Stream) != NULL)
			*Ostream << "> " << Buffer;
	}
	else if (Running)
	{
#ifdef _WIN32
		_pclose(Stream);
#else
		pclose(Stream);
#endif
		*Ostream << "\n> ----------     \"" << Command << "\" finished.     ----------\n\n";

		Running = false;
		Timer->stop();
	}
}
