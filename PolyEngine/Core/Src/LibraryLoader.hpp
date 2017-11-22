#pragma once

#include "Defines.hpp"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>
#else
#error "Unsupported platform :("
#endif

namespace Poly
{
	template<typename Function> class LibraryFunctionHandle;
	template<typename Function> LibraryFunctionHandle<Function> LoadFunctionFromSharedLibrary(const char* libraryName, const char* functionSymbol);

	template<typename Function>
	class LibraryFunctionHandle
	{
	public:
		STATIC_ASSERTE(std::is_function<Function>::value, "Not a valid function type!");

		LibraryFunctionHandle() = default;
		LibraryFunctionHandle(LibraryFunctionHandle&& other) 
			: LibraryFunctionHandle{ other.LibHandle, other.FunctionPtr }
		{ 
			other.LibHandle = nullptr;
			other.FunctionPtr = nullptr;
		}
		LibraryFunctionHandle& operator=(LibraryFunctionHandle&& other)
		{
			LibHandle = other.LibHandle;
			FunctionPtr = other.FunctionPtr;
			other.LibHandle = nullptr;
			other.FunctionPtr = nullptr;
			return *this;
		}
		
		template<typename... Args> inline typename std::result_of<Function*(Args...)>::type operator()(Args... args) 
		{ 
			return FunctionPtr(std::forward<Args>(args)...); 
		}

		inline bool FunctionValid() const { return FunctionPtr != nullptr; }

#if defined(_WIN32)
		~LibraryFunctionHandle() { if (LibHandle) FreeLibrary(LibHandle); }
	private:
		LibraryFunctionHandle(HMODULE libHandle, Function* funcHandle) : LibHandle(libHandle), FunctionPtr(funcHandle) {}
		HMODULE LibHandle = nullptr;
#elif defined(__linux__) || defined(__APPLE__)
		~LibraryFunctionHandle() { if (LibHandle) dlclose(LibHandle); }
	private:
		LibraryFunctionHandle(void* libHandle, Function* funcHandle) : LibHandle(libHandle), FunctionPtr(funcHandle) {}
		void* LibHandle = nullptr;
#else
#error "Unsupported platform :("
#endif

		
		Function* FunctionPtr = nullptr;
		friend LibraryFunctionHandle<Function> LoadFunctionFromSharedLibrary<Function>(const char* libraryName, const char* functionSymbol);
	};


	template<typename Function>
	LibraryFunctionHandle<Function> LoadFunctionFromSharedLibrary(const char* libraryName, const char* functionSymbol)
	{
		STATIC_ASSERTE(std::is_function<Function>::value, "Not a valid function type!");
#if defined(_WIN32)
		static const char* LIB_EXTENSION = ".dll";
#elif defined(__linux__)
		static const char* LIB_EXTENSION = ".so";
#elif defined(__APPLE__)
		static const char* LIB_EXTENSION = ".dylib";
#else
#error "Unsupported platform :("
#endif

		const size_t fullLibNameLength = strlen(libraryName) + strlen(LIB_EXTENSION);
		std::unique_ptr<char> fullLibName = std::unique_ptr<char>(new char[fullLibNameLength + 1]);
		strcpy_s(fullLibName.get(), fullLibNameLength + 1, libraryName);
		strcat_s(fullLibName.get(), fullLibNameLength + 1, LIB_EXTENSION);

#if defined(_WIN32)
		// If UNICODE is defined windows is using wchar_t. Then we need to convert const char* array to wchar_t array.
#if defined(UNICODE)
		std::unique_ptr<wchar_t> windowsFullLibName = std::unique_ptr<wchar_t>(new wchar_t[fullLibNameLength + 1]);
		MultiByteToWideChar(CP_ACP, 0, fullLibName.get(), -1, windowsFullLibName.get(), fullLibNameLength + 1);
#else
		std::unique_ptr<char> windowsFullLibName = std::move(fullLibName);
#endif

		HMODULE libHandle = LoadLibrary(windowsFullLibName.get());
		if (!libHandle)
		{
			DWORD err = GetLastError();
			Poly::gConsole.LogError("Shared library [{}] load failed. Error code: {}", libraryName, err);
			return LibraryFunctionHandle<Function>{ nullptr, nullptr };
		}

		void* funcHandle = GetProcAddress(libHandle, functionSymbol);
		if (!funcHandle)
		{
			DWORD err = GetLastError();
			Poly::gConsole.LogError("Function [{}] from shared library [{}] load failed. Error code: {}", functionSymbol, libraryName, err);
			return LibraryFunctionHandle<Function>{ nullptr, nullptr };
		}
#elif defined(__linux__) || defined(__APPLE__)
		void* libHandle = dlopen(fullLibName, RTLD_NOW /*| RTLD_GLOBAL*/); //don't be lazy in resolving symbols /*and allow subsequently loaded libs to use them*/
		if (const char* err = dlerror()) 
		{ //we could simply check if the handle is null, but using dlerror() doubles as clearing error flags
			Poly::gConsole.LogError("Shared library [{}] load failed. Error: {}", libraryName, err);
			return LibraryFunctionHandle<Function>{ nullptr, nullptr };
		}

		void* funcHandle = dlsym(libHandle, functionSymbol);
		if (const char* err = dlerror())
		{ //symbols can be legally null, so we need to check for errors instead
			Poly::gConsole.LogError("Function [{}] from shared library [{}] load failed. Error: {}", functionSymbol, libraryName, err);
			dlclose(libHandle);
			return LibraryFunctionHandle<Function>{ nullptr, nullptr };
		}
#else
#error "Unsupported platform :("
#endif

		Poly::gConsole.LogError("Function [{}] from shared library [{}] loaded succesfully!", functionSymbol, libraryName);
		return LibraryFunctionHandle<Function>{ libHandle, reinterpret_cast<Function*>(funcHandle) };
	}
}