#include <PolyEditorPCH.hpp>

void CommandManager::RunCommand(const String& cmd, const String& desc)
{
	if (Running)
		throw new CommandManagerException("Another process is currently running within ProjectManager.");

	Command = cmd;
	CommandDesc = desc;

	StringBuilder builder;

	// append some magic chars
	builder.Append(cmd);
	builder.Append(" 2>&1");

	// execute command and return output stream
#ifdef _WIN32
	Stream = _popen(builder.GetString().GetCStr(), "r");
#else
	Stream = popen(builder.GetString().GetCStr(), "r");
#endif

	Running = true;

	// setup timer for reading from stream
	Timer = std::make_unique<QTimer>(this);
	connect(Timer.get(), &QTimer::timeout, this, &ProjectManager::ReadStdout);
	Timer->start(0);

	// display some nice info
	*Ostream << "\n\n> ----------     " << CommandDesc << " started...     ----------\n";
	*Ostream << "> ----------     " << Command << "     ----------\n\n";
}

void CommandManager::ReadStdout()
{
	if (Stream && !feof(Stream))
	{
		if (fgets(Buffer, MaxBuffer, Stream) != NULL)
			*Ostream << "> " << Buffer;
	}
	else
	{
#ifdef _WIN32
		_pclose(Stream);
#else
		pclose(Stream);
#endif
		Running = false;
		*Ostream << "\n> ----------     " << CommandDesc << " finished.     ----------\n\n";
		Command = "";
		CommandDesc = "";
	}
}
