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
	template<typename Function> LibraryFunctionHandle<Function> LoadFunctionFromSharedLibrary(const String& libraryName, const char* functionSymbol);

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

		inline bool FunctionValid() const { return static_cast<bool>(FunctionPtr); }

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
		friend LibraryFunctionHandle<Function> LoadFunctionFromSharedLibrary<Function>(const String& libraryName, const char* functionSymbol);
	};


	template<typename Function>
	LibraryFunctionHandle<Function> LoadFunctionFromSharedLibrary(const String& libraryName, const char* functionSymbol)
	{
		STATIC_ASSERTE(std::is_function<Function>::value, "Not a valid function type!");
#if defined(_WIN32)
		String fullLibName = libraryName + ".dll";
#elif defined(__linux__)
		String fullLibName = libraryName + ".so";
#elif defined(__APPLE__)
		String fullLibName = libraryName + ".dylib";
#else
#error "Unsupported platform :("
#endif

#if defined(_WIN32)
		HMODULE libHandle = LoadLibrary(fullLibName.GetCStr());
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
		void* libHandle = dlopen(fullLibName.GetCStr(), RTLD_NOW /*| RTLD_GLOBAL*/); //don't be lazy in resolving symbols /*and allow subsequently loaded libs to use them*/
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