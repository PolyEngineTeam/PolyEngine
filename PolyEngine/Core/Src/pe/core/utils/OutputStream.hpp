#pragma once

#include <pe/Defines.hpp>

namespace pe::core::utils {

	class CORE_DLLEXPORT OutputStream : public BaseObject<>, public std::streambuf
	{
	public:
		virtual void OnUnregister() {}
		virtual void Append(const char*) = 0;
	protected:
		std::streamsize xsputn(const char_type* s, std::streamsize n) override final;
		int_type overflow(int_type c) override final;
	};

	class CORE_DLLEXPORT FileOutputStream : public OutputStream
	{
	public:
		FileOutputStream(const char* name);
		FileOutputStream& operator=(FileOutputStream&& rhs);
		FileOutputStream(FileOutputStream&& rhs);
		FileOutputStream() = default;
		~FileOutputStream() { EnsureFileClosed(); }
		void OnUnregister() override { EnsureFileClosed(); }
		void EnsureFileClosed();
		bool IsFileOpen() const { return FileHandle != nullptr; }
		void Append(const char*) override;
	private:
		FILE* FileHandle = nullptr;
	};
}