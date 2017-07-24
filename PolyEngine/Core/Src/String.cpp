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

/// <summary>String constructor that creates String based on provided Cstring</summary>
/// <param name="data"></param>
String::String(const char* data) {
	size_t length = StrLen(data);
	Data.Resize(length + 1);
	std::memcpy(Data.GetData(), data, sizeof(char) * length);
	Data[length] = 0;
}

/// <summary>String copy constructor</summary>
/// <param name="rhs">Reference to String instance which state should be copied</param>
String::String(const String& rhs) {
	*this = rhs;
}

/// <summary>String move constructor</summary>
/// <param name="rhs">Reference to String instance which state should be moved</param>
String::String(String&& rhs) {
	Data = std::move(rhs.Data);
}

/// <summary>Casts int to String</summary>
/// <param name="var">Integer value which should be used to make String instance</param>
/// <returns>String containing integer value</returns>
String String::From(int var) {
	return String(std::to_string(var).c_str());
}

/// <summary>Casts float to String with given precision</summary>
/// <param name="var">Float value which should be used to make String instance</param>
/// <param name="precision">Number of digits after dot which String instance should contain</param>
/// <returns>String containing float value</returns>
String String::From(float var, int precision) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << var;
	return String(stream.str().c_str());
}

/// <summary>Casts double to String with given precision</summary>
/// <param name="var">Double value which should be used to make String instance</param>
/// <param name="precision">Number of digits after dot which String instance should contain</param>
/// <returns>String containing double value</returns>
String String::From(double var, int precision) {
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << var;
	return String(stream.str().c_str());
}

/// <summary>Casts single char to String</summary>
/// <param name="var">Char value which should be used to make String instace</param>
/// <returns>String conaining only one char</returns>
String String::From(char var) {
	return From(std::string(1, var));
}

/// <summary>Casts Cstring to String</summary>
/// <param name="var">Cstring value which should be used to make String instance</param>
/// <returns>String containing given Cstring</returns>
String String::From(const char* var) {
	return String(var);
}

/// <summary>Casts std::string to String</summary>
/// <param name="var">std::string reference which should be used to make String instance</param>
/// <returns>String containing given std::string</returns>
String String::From(const std::string& var) {
	return String(var.c_str());
}

/// <summary>Checks if String instance contains another String instance</summary>
/// <param name="var">String reference which should be contained in another String instance</param>
bool String::Contains(const String& var) const {
	//TODO Dynarray.FindAllIdx()
	//for all this.Data.FindAllIdx()
	//	if var == this->Substring(idx, idx+var.Data.GetSize()
	//		return true
	return false;
}

/// <summary>Checks if String instance contains given char</summary>
/// <param name="var">Char which should be contained in String instance</param>
bool String::Contains(char var) const {
	return this->Data.FindIdx(var) != this->Data.GetSize();
}

/// <summary>Returns the same String value, except all upper-case characters are made lower</summary>
/// <returns>Lower-case String instance</returns>
String String::ToLower() const {
	String s = String();
	s.Data.Resize(this->Data.GetSize());

	for (int i = 0; i < this->Data.GetSize(); i++) {
		char c = this->Data[i];
		if (c > 64 && c < 91) {
			c += 32;
		}
		s.Data[i] = c;
	}
	return s;
}

/// <summary>Returns the same Strin value, except all lower-case characters ae made upper</summary>
/// <returns>Upper-case String instance</returns>
String String::ToUpper() const {
	String s = String();
	s.Data.Resize(this->Data.GetSize());

	for (int i = 0; i < this->Data.GetSize(); i++) {
		char c = this->Data[i];
		if (c > 96 && c < 123) {
			c -= 32;
		}
		s.Data[i] = c;
	}
	return s;
}

/// <summary>Checks if String is empty</summary>
bool String::IsEmpty() const {
	return GetLength() == 0;
}

/// <summary>Replaces all occurences of one char with another char</summary>
/// <param name="a">Char which should be replaced</param>
/// <param name="b">Char which should overwrite another char</param>
/// <returns>String instance witch replaced chars</returns>
String String::Replace(char a, char b) const {
	String s = String(*this);
	s.Data.Resize(this->Data.GetSize());
	for (int i = 0; i < this->Data.GetSize(); i++) {
		if (this->Data[i] == a) {
			s.Data[i] = b;
		}
	}
	return s;
}

/// <summary>Replaces all occurences of given String with another String</summary>
/// <param name="a">String reference which should be replaced</param>
/// <param name="b">String reference which should overwrite another String</param>
/// <returns>String instance witch replaced substrings</returns>
String String::Replace(const String& a, const String& b) const {
	//split source by delimiter a
	//return string joined with separator b
	return NULL;
}

/// <summary>Splits String by given delimite to array of Strings</summary>
/// <param name="delimiter">Char used as token to split String with</param>
/// <returns>String array</returns>
String String::Split(char delimiter) const {
	//TODO Dynarray.FindAllIdx(T rhs)
	//for all indexes get substring
	//and return an array of these substrings
	return NULL;
}

/// <summary>Splits String by given delimiter to array of Strings</summary>
/// <param name="delimiter">String used as token to split String with</param>
/// <returns>String array</returns>
String String::Split(const String& delimiter) const {
	//TODO Dynarray.FindAllIdx(T rhs)
	//for all indexes remove delimiter strings from source
	//return an array of whats left (using Substring)
	return NULL;
}

/// <summary>Merges String array to single String instance, separating them with given separator</summary>
/// <param name="vars">String array to be merged</param>
/// <param name="size">Size of String array</param>
/// <param name="separator">String instance used to separate substrings in merged string</param>
/// <returns>Merged String instance</returns>
String String::Join(const String* vars, size_t size, const String& separator) {
	//TODO replace using stringbuilder
	String s = String("");
	for (int i = 0; i < size; i++) {
		s = s + vars[i];
		if (i != size - 1) {
			s = s + separator;
		}
	}
	return s;
}

/// <summary>Merges String array to single String instance, separating them with given separator</summary>
/// <param name="vars">String array to be merged</param>
/// <param name="size">Size of String array</param>
/// <param name="separator">Char used to separate substrings in merged string</param>
/// <returns>Merged String instance</returns>
String String::Join(const String* vars, size_t size, char separator) {
	//TODO replace using stringbuilder
	String s = String("");
	for (int i = 0; i < size; i++) {
		s = s + vars[i];
		if (i != size - 1) {
			s = s + separator;
		}
	}
	return s;
}

/// <summary>Checks if String instance starts with given char</summary>
/// <param name="var">Char which is supposed to be at the beginning of String</param>
bool String::StartsWith(char var) const {
	return (Data[0] == var);
}

/// <summary>Checks if String instance ends with given char</summary>
/// <param name="var">Char which is supposed to be at the end of String</param>
bool String::EndsWith(char var) const {
	return (Data[Data.GetSize()-2] == var);
}

/// <summary>Cuts String to the given index</summary>
/// <param name="end">Index at which new String should end</param>
/// <returns>String instance containing part of original String</returns>
String String::Substring(int end) const {
	return Substring(0, end);
}

/// <summary>Cuts String from start index to end index</summary>
/// <param name="start">Index at which new String should start</param>
/// <param name="end">Index at which new String should end</param>
/// <returns>String instance containing part of original String</returns>
String String::Substring(int start, int end) const {
	ASSERTE(start >= 0 && end >= 0, "Start and end parameters cannot be negative");
	ASSERTE(start <= end && end <= this->Data.GetSize(), "Invalid start or end parameter");
	String s = String();
	s.Data.Resize(end - start + 1);
	for (int i = start; i < end; i++) {
		s.Data[i-start] = this->Data[i];
	}
	s.Data[end - start] = 0;
	return s;
}

/// <summary>Removes whitespaces from the beginning and the end of String</summary>
/// <returns>Trimmed String instance</returns>
String String::GetTrimmed() const {
	int start = 0;
	int end = this->GetLength();
	String whitespaces = String(" \n\t\r");

	while (whitespaces.Contains(this->Data[start])) {
		++start;
	}	
	while (whitespaces.Contains(this->Data[end])) {
		--end;
	}

	return this->Substring(start, end + 1);
}

/// <summary>Copies String state to another String instance</summary>
/// <param name="rhs">String reference which should be used as source</param>
/// <returns>Copied String reference</returns>
String& String::operator=(const String& rhs) {
	Data = rhs.Data;
	return *this;
}

/// <summary>Moves String state from another String instance</summary>
/// <param name="rhs">String reference which sould be used as source</param>
/// <returns>Moved String reference</returns>
String& String::operator=(String&& rhs) {
	Data = std::move(rhs.Data);
	return *this;
}

/// <summary>Compares String with Cstring</summary>
/// <param name="str">Cstring to be compared with</param>
bool String::operator==(const char* str) const {
	if (GetLength() != StrLen(str))
		return false;
	for (size_t k = 0; k < GetLength(); ++k)
		if (Data[k] != str[k])
			return false;
	return true;
}

/// <summary>Compares two String references</summary>
/// <param name="str">String to be compared with</param>
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

/// <summary>Appends one String to another</summary>
/// <param name="rhs">String instance to be appended to source String</param>
/// <returns>Appended String instance</returns>
String String::operator+(const String& rhs) const {
	String ret;
	size_t totalLength = GetLength() + rhs.GetLength();
	ret.Data.Resize(totalLength + 1);
	memcpy(ret.Data.GetData(), Data.GetData(), sizeof(char) * GetLength());
	memcpy(ret.Data.GetData() + GetLength(), rhs.Data.GetData(), sizeof(char) * rhs.GetLength());
	ret.Data[totalLength] = 0;
	return ret;
}

/// <summary>Appends char to String</summary>
/// <param name="rhs">Char which should be appended to source String</param>
/// <returns>Appended String instance</returns>
String String::operator+(char rhs) const {
	String ret;
	size_t totalLength = GetLength() + 1;
	ret.Data.Resize(totalLength + 1);
	memcpy(ret.Data.GetData(), Data.GetData(), sizeof(char) * GetLength());
	memcpy(ret.Data.GetData() + GetLength(), &rhs, sizeof(char));
	ret.Data[totalLength] = 0;
	return ret;
}

/// <summary>Char access operator</summary>
/// <param name="idx">Index of the char to access</param>
/// <returns>Char from given index</returns>
char String::operator[](int idx) const { 
	HEAVY_ASSERTE(idx <= GetLength(), "Index out of bounds!"); 
	return Data[idx];
}
