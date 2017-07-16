#include"CorePCH.hpp"

#include"String.hpp"

using namespace Poly;

size_t Poly::StrLen(const char* str) {
	size_t len = 0;
	while (str[len] != 0)
		++len;
	return len;
}


String::String(const char* data) {
	size_t length = StrLen(data);
	Data.Resize(length + 1);
	std::memcpy(Data.GetData(), data, sizeof(char) * length);
	Data[length] = 0;
}

String::String(const String& rhs) {
	*this = rhs;
}

String::String(String&& rhs) {
	*this = String(std::move(rhs));
}


String* String::From(int var) {
	return new String(std::to_string(var).c_str());
}

String* String::From(float var) {
	return new String(std::to_string(var).c_str());
}

String* String::From(double var) {
	return new String(std::to_string(var).c_str());
}

String* String::From(char var) {
	return From(std::string(1, var));
}

String* String::From(const char* var) {
	return new String(var);
}

String* String::From(const std::string& var) {
	return new String(var.c_str());
}


String& String::operator=(const String& rhs) {
	Data = rhs.Data;
	return *this;
}

String& String::operator=(String&& rhs) {
	Data = std::move(rhs.Data);
	return *this;
}

bool String::operator==(const char* str) {
	if (GetLength() != StrLen(str))
		return false;
	for (size_t k = 0; k < GetLength(); ++k)
		if (Data[k] != str[k])
			return false;
	return true;
}

bool String::operator==(const String& str) {
	return Data == str.Data;
}

bool String::operator<(const String& rhs) const {
	if (GetLength() < rhs.GetLength())
		return true;
	else if (GetLength() > rhs.GetLength())
		return false;

	for (size_t i = 0; i < GetLength(); ++i)
	{
		if (Data[i] < rhs.Data[i])
			return true;
		else if (Data[i] > rhs.Data[i])
			return false;
	}
	return false;
}

String String::operator+(const String& rhs) const {
	String ret;
	size_t totalLength = GetLength() + rhs.GetLength();
	ret.Data.Resize(totalLength + 1);
	memcpy(ret.Data.GetData(), Data.GetData(), sizeof(char) * GetLength());
	memcpy(ret.Data.GetData() + GetLength(), rhs.Data.GetData(), sizeof(char) * rhs.GetLength());
	ret.Data[totalLength] = 0;
	return ret;
}

const char& String::operator[](int idx) const { 
	HEAVY_ASSERTE(idx <= GetLength(), "Index out of bounds!"); 
	return Data[idx];
}
