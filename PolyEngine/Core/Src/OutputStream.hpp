#pragma once

#include "Defines.hpp"
#include <ostream>

namespace Poly {

	class CORE_DLLEXPORT OutputStream : public BaseObject<>, public std::streambuf
	{
	public:
		virtual void OnUnregister() {}
		virtual void Append(const char*) = 0;
	protected:
		std::streamsize OutputStream::xsputn(const char_type* s, std::streamsize n) override final
		{
			UNUSED(n);
			std::string str(s);
			size_t len = str.length();
			Append(s);
			return len;
		}

		int_type OutputStream::overflow(int_type c) override final
		{
			char tab[2];
			tab[0] = c;
			tab[1] = '\0';
			Append(tab);
			return c;
		}
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