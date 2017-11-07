#include "CorePCH.hpp"

#include "String.hpp"
#include <iomanip>
#include <sstream>

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
	Data = std::move(rhs.Data);
}

String String::From(int var) {
	return String(std::to_string(var).c_str());
}

String String::From(float var) {
	std::stringstream stream;
	stream << std::fixed << var;
	return String(stream.str().c_str());
}

String String::From(float var, size_t precision) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << var;
	return String(stream.str().c_str());
}

String String::From(double var) {
	std::stringstream stream;
	stream << std::fixed << var;
	return String(stream.str().c_str());
}

String String::From(double var, size_t precision) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << var;
	return String(stream.str().c_str());
}

String String::From(char var) {
	return From(std::string(1, var));
}

String String::From(const char* var) {
	return String(var);
}

String String::From(const std::string& var) {
	return String(var.c_str());
}

bool String::Contains(const String& var) const {
	Dynarray<int> idx = this->Data.FindAllIdx(*var.GetCStr());
	// -1 because of null terminator
	size_t dataSize = var.Data.GetSize() - 1;
	for (size_t i = 0; i < idx.GetSize(); i++) {
		String substr = this->Substring(idx[i], idx[i] + dataSize);
		if (var == this->Substring(idx[i], idx[i] + dataSize)) {
			return true;
		}
	}
	return false;
}

bool String::Contains(char var) const {
	return this->Data.FindIdx(var) != this->Data.GetSize();
}

//note: ASCII only
String String::ToLower() const {
	String s = String();
	s.Data.Resize(this->Data.GetSize());

	for (size_t i = 0; i < this->Data.GetSize(); i++) {
		char c = this->Data[i];
		if (c >= 'A' && c <= 'Z') {
			c += ('a' - 'A');
		}
		s.Data[i] = c;
	}
	return s;
}

//note: ASCII only
String String::ToUpper() const {
	String s = String();
	s.Data.Resize(this->Data.GetSize());

	for (size_t i = 0; i < this->Data.GetSize(); i++) {
		char c = this->Data[i];
		if (c >= 'a' && c <= 'z') {
			c -= ('a' - 'A');
		}
		s.Data[i] = c;
	}
	return s;
}

bool String::IsEmpty() const {
	return GetLength() == 0;
}

String String::Replace(char what, char with) const {
	String s = String(*this);
	s.Data.Resize(this->Data.GetSize());
	for (size_t i = 0; i < this->Data.GetSize(); i++) {
		if (this->Data[i] == what) {
			s.Data[i] = with;
		}
	}
	return s;
}

String String::Replace(const String& what, const String& with) const {
	String s = "";
	Dynarray<String> splitted = this->Split(what);
	return s.Join(splitted.GetData(), splitted.GetSize(), with);
}

Dynarray<String> String::Split(char delimiter) const {
	return Split(String::From(delimiter));
}

Dynarray<String> String::Split(const String& delimiter) const {
	Dynarray<String> elements;
	size_t idx = 0;
	while (idx < GetLength())
	{
		size_t delimiterStart = FindSubstrFromPoint(idx, delimiter);
		elements.PushBack(Substring(idx, delimiterStart));
		idx = delimiterStart + delimiter.GetLength();
	}
	return elements;
}

String String::Join(const String* vars, size_t size, const String& separator) {
	//TODO replace using stringbuilder
	String s = String("");
	for (size_t i = 0; i < size; i++) {
		s = s + vars[i];
		if (i != size - 1) {
			s = s + separator;
		}
	}
	return s;
}

String String::Join(const String* vars, size_t size, char separator) {
	//TODO replace using stringbuilder
	String s = String("");
	for (size_t i = 0; i < size; i++) {
		s = s + vars[i];
		if (i != size - 1) {
			s = s + separator;
		}
	}
	return s;
}

bool String::StartsWith(char var) const {
	return (Data[0] == var);
}

bool String::EndsWith(char var) const {
	return (Data[Data.GetSize()-2] == var);
}

String String::Substring(size_t end) const {
	return Substring(0, end);
}

String String::Substring(size_t start, size_t end) const {
	ASSERTE(start <= end && end <= this->Data.GetSize(), "Invalid start or end parameter");
	String s = String();
	s.Data.Resize(end - start + 1);
	for (size_t i = start; i < end; i++) {
		s.Data[i-start] = this->Data[i];
	}
	s.Data[end - start] = 0;
	return s;
}

String String::GetTrimmed() const {
	size_t start = 0;
	size_t end = this->GetLength();
	String whitespaces = String(" \n\t\r");

	while (whitespaces.Contains(this->Data[start])) {
		++start;
	}
	while (whitespaces.Contains(this->Data[end])) {
		--end;
	}

	return this->Substring(start, end + 1);
}

String& String::operator=(const String& rhs) {
	Data = rhs.Data;
	return *this;
}

String& String::operator=(String&& rhs) {
	Data = std::move(rhs.Data);
	return *this;
}

bool String::operator==(const char* str) const {
	if (GetLength() != StrLen(str))
		return false;
	for (size_t k = 0; k < GetLength(); ++k)
		if (Data[k] != str[k])
			return false;
	return true;
}

bool String::operator==(const String& str) const {
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

String String::operator+(char rhs) const {
	String ret;
	size_t totalLength = GetLength() + 1;
	ret.Data.Resize(totalLength + 1);
	memcpy(ret.Data.GetData(), Data.GetData(), sizeof(char) * GetLength());
	memcpy(ret.Data.GetData() + GetLength(), &rhs, sizeof(char));
	ret.Data[totalLength] = 0;
	return ret;
}

char String::operator[](size_t idx) const {
	HEAVY_ASSERTE(idx <= GetLength(), "Index out of bounds!");
	return Data[idx];
}

size_t String::GetLength() const
{
	return Data.GetSize() - 1;
}

size_t String::FindSubstrFromPoint(size_t startPoint, const String& str) const
{
	for (size_t idx = startPoint; idx < GetLength(); ++idx)
	{
		size_t matchCount = 0;
		while (matchCount < str.GetLength())
		{
			if (Data[idx + matchCount] == str[matchCount])
				++matchCount;
			else
				break;
		}

		if (matchCount == str.GetLength())
			return idx;
	}
	return GetLength();
}
