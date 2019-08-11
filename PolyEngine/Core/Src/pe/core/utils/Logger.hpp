#pragma once

#include <pe/Defines.hpp>
#include <pe/core/utils/EnumUtils.hpp>
#include <pe/core/utils/OutputStream.hpp>
#include <pe/core/storage/StringBuilder.hpp>

namespace Poly 
{
	/**
	*  Enum describing possible levels of logging
	*/
	enum class eLogLevel { LVL_DEBUG, LVL_INFO, LVL_WARNING, LVL_ERROR, _COUNT };
	REGISTER_ENUM_NAMES(eLogLevel, "DEBUG", "INFO", "WARNING", "ERROR");

	constexpr eLogLevel LOG_LEVEL_FILTER = eLogLevel::LVL_DEBUG;

	class CORE_DLLEXPORT Console : public BaseObject<> 
	{
	public:
		Console() : Ostream(new std::ostream(std::cout.rdbuf())) {}

		template <typename S, typename... Args>
		void RegisterStream(Args&&... args)
		{
			constexpr bool isStream = std::is_base_of<OutputStream, S>::value; // Strange workaround to STATIC_ASSERTE macro on MSVC
			STATIC_ASSERTE(isStream, "Provided value is not stream!");
			if (CurrentStream)
				CurrentStream->OnUnregister();
			CurrentStream = std::make_unique<S>(std::forward<Args>(args)...);
			Ostream = std::make_unique<std::ostream>(CurrentStream.get());
		}
		
		void RegisterDefaultStream()
		{
			if (CurrentStream)
				CurrentStream->OnUnregister();
			CurrentStream = nullptr;
			Ostream = std::make_unique<std::ostream>(std::cout.rdbuf());
		}
		
		/**
		*  Set of methods for easy logging. Only those should be used in engine code.
		*  Future compatibility is guaranteed
		*
		*  Usage:
		*  - First argument contains main message to log.
		*  - Other arguments are optional, each provided optional argument must
		* override stream << operator.
		*  - For each optional argument {} marker should be placed in main message
		* string similarly to C-style printf.
		*  - Markers that do not have coresponding arguments will be treated as normal
		* string.
		*  - Arguments that do not have coresponding markers will be ignored.
		*/
		template <typename... Args>
		void Log(eLogLevel lvl, const String& fmt, Args&&... args) { LogImpl(lvl, GetEnumName(lvl), fmt, std::forward<Args>(args)...); }

		template <typename... Args>
		void LogDebug(const String& fmt, Args&&... args) { Log(eLogLevel::LVL_DEBUG, fmt, std::forward<Args>(args)...); }
		template <typename... Args>
		void LogInfo(const String& fmt, Args&&... args) { Log(eLogLevel::LVL_INFO, fmt, std::forward<Args>(args)...); }
		template <typename... Args>
		void LogWarning(const String& fmt, Args&&... args) { Log(eLogLevel::LVL_WARNING, fmt, std::forward<Args>(args)...); }
		template <typename... Args>
		void LogError(const String& fmt, Args&&... args) { Log(eLogLevel::LVL_ERROR, fmt, std::forward<Args>(args)...); }

	private:

		/**
		*  Main logging function. Future compatibility is not guaranteed. DO NOT USE
		* IT!
		*
		*  @param[in] level Level of logging.
		*  @param[in] levelStr String with logging level name.
		*  @param[in] fmt Format string.
		*  @param[in] args Variadic template argument list.
		*/
		template <typename... Args>
		void LogImpl(eLogLevel level, const String& levelStr, const String& fmt, Args&&... args) 
		{
			if (level >= LOG_LEVEL_FILTER)
			{
				static StringBuilder sb;
				String fullFmt = StringBuilder().AppendFormat("[{}] {}", levelStr, fmt).StealString();
				sb.AppendFormat(fullFmt.GetCStr(), std::forward<Args>(args)...);
				*Ostream << sb.StealString() << std::endl;
				sb.Clear();
			}
		}

		std::unique_ptr<OutputStream> CurrentStream;
		std::unique_ptr<std::ostream> Ostream;
	};

	CORE_DLLEXPORT extern Console gConsole;
} //namespace Poly
