﻿#pragma once

#include "Defines.hpp"
#include <ostream>

namespace Poly {

	class OutputStream : public BaseObject<>, public std::streambuf
	{
	public:
		virtual void OnUnregister() {}
		virtual void Append(String&& str) = 0;
	protected:
		std::streamsize xsputn(const char_type* s, std::streamsize n) override final
		{
			String str(s);
			size_t len = str.GetLength();
			Append(std::move(str));
			return len;
		}

		int_type overflow(int_type c) override final
		{
			char tab[2];
			tab[0] = c;
			tab[1] = '\0';
			Append(String(tab));
			return c;
		}
	};
/*
	class OutputStreamBase : public std::streambuf {
	public:
		virtual void OnUnregister() = 0;
		virtual OutputStreamBase& operator<<(const String& data) = 0;
	};
*/
	/*
	class StandardOutputStream : public OutputStream {
	public:
		StandardOutputStream(std::ostream& ostream) : stream(ostream) {};
		void OnUnregister() { stream.flush(); }
		void Append(const String&& data) {
			const char* cdata = data.GetCStr();
			stream.write(cdata, sizeof(cdata));
		}
	private:
		std::ostream& stream;
	};
	*/
	class FileOutputStream : public OutputStream {
	public:
		FileOutputStream(const char* name) { 
			freopen_s(&file, name, "w", stdout); 
			setvbuf(stdout, NULL, _IONBF, 0);
		}
		FileOutputStream& operator=(FileOutputStream&& rhs) {
			EnsureFileClosed();
			file = rhs.file;
			rhs.file = nullptr;
		}
		FileOutputStream(FileOutputStream&& rhs) {
			*this = std::move(rhs);
		}
		FileOutputStream() = default;
		~FileOutputStream() { EnsureFileClosed(); }
		void OnUnregister() override { EnsureFileClosed(); }
		void EnsureFileClosed() {
			if (file) { fclose(file); file = nullptr; }
		}
		bool IsFileOpen() const { return file != nullptr; }
		void Append(String&& data) override {
			fwrite(data.GetCStr(), sizeof(char), data.GetLength(), file);
		}
	private:
		FILE* file = nullptr;
	};
}