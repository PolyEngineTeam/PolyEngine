#pragma once

#include <Core.hpp>
#include <QObject>

#include "Configs/ProjectConfig.hpp"

class ProjectManagerException : public std::exception
{
public:
	ProjectManagerException(const Poly::String& msg) : Msg(msg) {}
	const char* what() const noexcept override { return Msg.GetCStr(); }

protected:
	Poly::String Msg;
};

class ProjectManager : public QObject
{
public:
	ProjectManager() {}
	~ProjectManager() = default;

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

	void Create(const Poly::String& projectName, const Poly::String& projectPath, const Poly::String& enginePath);
	void Open(Poly::String projectPath);
	void Update(const Poly::String& enginePath);
	void Build();
	void Close();

	bool IsOpened() { return Opened && !Running; }
	bool IsRunning() { return Running; }

	const Poly::String& GetProjectName() const { return ProjectConfig->ProjectName; }
	const Poly::String& GetProjectPath() const { return ProjectPath; }

private:
	void RunCommand(const Poly::String& cmd);

	bool Opened = false;
	bool Running = false;

	Poly::String ProjectPath = "";

	std::unique_ptr<ProjectConfig> ProjectConfig;

	Poly::String Command = "";
	Poly::String CommandDesc = "";

	// things important for getting output from external programs like ProjectTool.py
	std::unique_ptr<QTimer> Timer = nullptr;
	FILE* Stream = nullptr;
	static const size_t MaxBuffer = 256;
	char Buffer[MaxBuffer];
	std::unique_ptr<Poly::OutputStream> CurrentStream;
	std::unique_ptr<std::ostream> Ostream;

private slots:
	void ReadStdout();
};