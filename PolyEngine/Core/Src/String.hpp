#pragma once

#include "Dynarray.hpp"
#include "Defines.hpp"

namespace Poly {

	size_t StrLen(const char* str);

	class CORE_DLLEXPORT String : BaseObject<> {
	public:

		/// <summary>Basic String costructor that creates empty String</summary>
		String() : String("") {};

		/// <summary>String constructor that creates String based on provided Cstring</summary>
		/// <param name="data"></param>
		String(const char* data);

		/// <summary>String copy constructor</summary>
		/// <param name="rhs">Reference to String instance which state should be copied</param>
		String(const String& rhs);

		/// <summary>String move constructor</summary>
		/// <param name="rhs">Reference to String instance which state should be moved</param>
		String(String&& rhs);


		/// <summary>Casts int to String</summary>
		/// <param name="var">Integer value which should be used to make String instance</param>
		/// <returns>String containing integer value</returns>
		static String From(int var);

		/// <summary>Casts float to String</summary>
		/// <param name="var">Float value which should be used to make String instance</param>
		/// <param name="precision">Number of digits after dot which String instance should contain</param>
		/// <returns>String containing float value</returns>
		static String From(float var);

		/// <summary>Casts float to String with given precision</summary>
		/// <param name="var">Float value which should be used to make String instance</param>
		/// <param name="precision">Number of digits after dot which String instance should contain</param>
		/// <returns>String containing float value</returns>
		static String From(float var, size_t precision);

		/// <summary>Casts double to String</summary>
		/// <param name="var">Double value which should be used to make String instance</param>
		/// <param name="precision">Number of digits after dot which String instance should contain</param>
		/// <returns>String containing double value</returns>
		static String From(double var);

		/// <summary>Casts double to String with given precision</summary>
		/// <param name="var">Double value which should be used to make String instance</param>
		/// <param name="precision">Number of digits after dot which String instance should contain</param>
		/// <returns>String containing double value</returns>
		static String From(double var, size_t precision);

		/// <summary>Casts single char to String</summary>
		/// <param name="var">Char value which should be used to make String instace</param>
		/// <returns>String conaining only one char</returns>
		static String From(char var);

		/// <summary>Casts Cstring to String</summary>
		/// <param name="var">Cstring value which should be used to make String instance</param>
		/// <returns>String containing given Cstring</returns>
		static String From(const char* var);

		/// <summary>Casts std::string to String</summary>
		/// <param name="var">std::string reference which should be used to make String instance</param>
		/// <returns>String containing given std::string</returns>
		static String From(const std::string& var);


		/// <summary>Checks if String instance contains another String instance</summary>
		/// <param name="var">String reference which should be contained in another String instance</param>
		bool Contains(const String& var) const;

		/// <summary>Checks if String instance contains given char</summary>
		/// <param name="var">Char which should be contained in String instance</param>
		bool Contains(char var) const;

		/// <summary>Returns the same String value, except all upper-case characters are made lower</summary>
		/// <returns>Lower-case String instance</returns>
		String ToLower() const;

		/// <summary>Returns the same Strin value, except all lower-case characters ae made upper</summary>
		/// <returns>Upper-case String instance</returns>
		String ToUpper() const;

		/// <summary>Checks if String is empty</summary>
		bool IsEmpty() const;

		/// <summary>Replaces all occurences of one char with another char</summary>
		/// <param name="a">Char which should be replaced</param>
		/// <param name="b">Char which should overwrite another char</param>
		/// <returns>String instance witch replaced chars</returns>
		String Replace(char a, char b) const;

		/// <summary>Replaces all occurences of given String with another String</summary>
		/// <param name="a">String reference which should be replaced</param>
		/// <param name="b">String reference which should overwrite another String</param>
		/// <returns>String instance witch replaced substrings</returns>
		String Replace(const String& a, const String& b) const;

		/// <summary>Splits String by given delimite to array of Strings</summary>
		/// <param name="delimiter">Char used as token to split String with</param>
		/// <returns>String array</returns>
		Dynarray<String> Split(char delimiter) const;

		/// <summary>Splits String by given delimiter to array of Strings</summary>
		/// <param name="delimiter">String used as token to split String with</param>
		/// <returns>String array</returns>
		Dynarray<String> Split(const String& delimiter) const;

		/// <summary>Merges String array to single String instance, separating them with given separator</summary>
		/// <param name="vars">String array to be merged</param>
		/// <param name="size">Size of String array</param>
		/// <param name="separator">String instance used to separate substrings in merged string</param>
		/// <returns>Merged String instance</returns>
		static String Join(const String* vars, size_t size, const String& separator);

		/// <summary>Merges String array to single String instance, separating them with given separator</summary>
		/// <param name="vars">String array to be merged</param>
		/// <param name="size">Size of String array</param>
		/// <param name="separator">Char used to separate substrings in merged string</param>
		/// <returns>Merged String instance</returns>
		static String Join(const String* vars, size_t size, char separator);

		/// <summary>Checks if String instance starts with given char</summary>
		/// <param name="var">Char which is supposed to be at the beginning of String</param>
		bool StartsWith(char var) const;

		/// <summary>Checks if String instance ends with given char</summary>
		/// <param name="var">Char which is supposed to be at the end of String</param>
		bool EndsWith(char var) const;

		/// <summary>Cuts String to the given index</summary>
		/// <param name="end">Index at which new String should end</param>
		/// <returns>String instance containing part of original String</returns>
		String Substring(size_t end) const;

		/// <summary>Cuts String from start index to end index</summary>
		/// <param name="start">Index at which new String should start</param>
		/// <param name="end">Index at which new String should end</param>
		/// <returns>String instance containing part of original String</returns>
		String Substring(size_t start, size_t end) const;

		/// <summary>Removes whitespaces from the beginning and the end of String</summary>
		/// <returns>Trimmed String instance</returns>
		String GetTrimmed() const;


		/// <summary>Copies String state to another String instance</summary>
		/// <param name="rhs">String reference which should be used as source</param>
		/// <returns>Copied String reference</returns>
		String& operator=(const String& rhs);

		/// <summary>Moves String state from another String instance</summary>
		/// <param name="rhs">String reference which sould be used as source</param>
		/// <returns>Moved String reference</returns>
		String& operator=(String&& rhs);

		/// <summary>Compares String with Cstring</summary>
		/// <param name="str">Cstring to be compared with</param>
		bool operator==(const char* str) const;

		/// <summary>Compares two String references</summary>
		/// <param name="str">String to be compared with</param>
		bool operator==(const String& str) const;

		bool operator<(const String& rhs) const;

		/// <summary>Appends one String to another</summary>
		/// <param name="rhs">String instance to be appended to source String</param>
		/// <returns>Appended String instance</returns>
		String operator+(const String& rhs) const;

		/// <summary>Appends char to String</summary>
		/// <param name="rhs">Char which should be appended to source String</param>
		/// <returns>Appended String instance</returns>
		String operator+(char rhs) const;

		/// <summary>Char access operator</summary>
		/// <param name="idx">Index of the char to access</param>
		/// <returns>Char from given index</returns>
		char operator[](int idx) const;

		size_t GetLength() const { return Data.GetSize() - 1; }
		const char* GetCStr() const { return Data.GetData(); }

		/*CORE_DLLEXPORT*/ friend std::ostream& operator<< (std::ostream& stream, const String& rhs) { return stream << rhs.GetCStr(); }

	private:
		Dynarray<char> Data;

		size_t FindSubstrFromPoint(size_t startPoint, const String& str) const;
	};
}
