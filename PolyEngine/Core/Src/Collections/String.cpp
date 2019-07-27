#include <CorePCH.hpp>

#include <Collections/String.hpp>
#include <Collections/StringBuilder.hpp>

using namespace Poly;

const String String::EMPTY = String();

static const std::vector<char> WHITESPACES { ' ', '\t', '\r', '\n', '\0' };

size_t Poly::StrLen(const char* str) {
	size_t len = 0;
	while (str[len] != 0)
		++len;
	return len;
}

String::String(const char* data) {
	size_t length = StrLen(data);
	Data.resize(length + 1);
	std::memcpy(Data.data(), data, sizeof(char) * length);
	Data[length] = 0;
}

String::String(const String& rhs) {
	*this = rhs;
}

String::String(String&& rhs) {
	Data = std::move(rhs.Data);
}

String String::From(int var) { return StringBuilder().Append(var).StealString(); }
String String::From(float var) { return StringBuilder().Append(var).StealString(); }
String String::From(float var, size_t precision) { return StringBuilder().Append(var, precision).StealString(); }
String String::From(double var) { return StringBuilder().Append(var).StealString(); }
String String::From(double var, size_t precision) { return StringBuilder().Append(var, precision).StealString(); }
String String::From(char var) { return StringBuilder().Append(var).StealString(); }
String String::From(const char* var) { return StringBuilder().Append(var).StealString(); }
String String::From(const std::string& var) { return StringBuilder().Append(var).StealString(); }

bool String::Contains(const String& var) const
{
	size_t idx2 = 0;
	for (size_t idx1 = 0; idx1 < GetLength(); idx1++)
	{
		if (var[idx2] == Data[idx1])
			idx2++;
		else
			idx2 = 0;

		if (idx2 == var.GetLength())
			return true;
	}
	return false;
}

bool String::Contains(char var) const
{
	return std::find(Data.begin(), Data.end(), var) != Data.end();
}

//note: ASCII only
String String::ToLower() const
{
	String ret;
	ret.Data.clear();
	ret.Data.reserve(Data.size());
	std::transform(Data.begin(), Data.end(), std::back_inserter(ret.Data), tolower);
	return ret;
}

//note: ASCII only
String String::ToUpper() const
{
	String ret;
	ret.Data.clear();
	ret.Data.reserve(Data.size());
	std::transform(Data.begin(), Data.end(), std::back_inserter(ret.Data), toupper);
	return ret;
}

bool String::IsEmpty() const {
	return GetLength() == 0;
}

String String::Replace(char what, char with) const
{
	String ret;
	ret.Data.clear();
	ret.Data.reserve(Data.size());
	std::transform(Data.begin(), Data.end(), std::back_inserter(ret.Data), 
		[what, with](char c) { return what == c ? with : c; });
	return ret;
}

String String::Replace(const String& what, const String& with) const {
	
	std::vector<String> splitted = Split(what);
	return Join(splitted.data(), splitted.size(), with);
}

std::vector<String> String::Split(char delimiter) const {
	return Split(String::From(delimiter));
}

std::vector<String> String::Split(const String& delimiter) const {
	std::vector<String> elements;
	size_t idx = 0;
	while (idx < GetLength())
	{
		size_t delimiterStart = FindSubstrFromPoint(idx, delimiter);
		elements.push_back(Substring(idx, delimiterStart));
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
	return (Data[Data.size()-2] == var);
}

String String::Substring(size_t end) const {
	return Substring(0, end);
}

String String::Substring(size_t start, size_t end) const {
	ASSERTE(start <= end && end <= this->Data.size(), "Invalid start or end parameter");
	String s = String();
	s.Data.resize(end - start + 1);
	for (size_t i = start; i < end; i++) {
		s.Data[i-start] = this->Data[i];
	}
	s.Data[end - start] = 0;
	return s;
}

String String::GetTrimmed() const {
	size_t start = 0;
	size_t end = GetLength();

	while (start < Data.size() && std::find(WHITESPACES.begin(), WHITESPACES.end(), Data[start]) != WHITESPACES.end())
	{
		++start;
	}

	while (end > start && std::find(WHITESPACES.begin(), WHITESPACES.end(), Data[end]) != WHITESPACES.end())
	{
		--end;
	}

	return Substring(start, end + 1);
}

size_t String::Find(char c) const
{
	auto it = std::find(Data.begin(), Data.end(), c);
	return it - Data.begin();
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
	ret.Data.resize(totalLength + 1);
	memcpy(ret.Data.data(), Data.data(), sizeof(char) * GetLength());
	memcpy(ret.Data.data() + GetLength(), rhs.Data.data(), sizeof(char) * rhs.GetLength());
	ret.Data[totalLength] = 0;
	return ret;
}

String String::operator+(char rhs) const {
	String ret;
	size_t totalLength = GetLength() + 1;
	ret.Data.resize(totalLength + 1);
	memcpy(ret.Data.data(), Data.data(), sizeof(char) * GetLength());
	memcpy(ret.Data.data() + GetLength(), &rhs, sizeof(char));
	ret.Data[totalLength] = 0;
	return ret;
}

char String::operator[](size_t idx) const {
	HEAVY_ASSERTE(idx <= GetLength(), "Index out of bounds!");
	return Data[idx];
}

size_t String::GetLength() const
{
	return Data.size() - 1;
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
