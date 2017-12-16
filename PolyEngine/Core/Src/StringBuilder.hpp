#pragma once

#include "Dynarray.hpp"
#include "Defines.hpp"
#include "String.hpp"

namespace Poly {
    class CORE_DLLEXPORT StringBuilder : BaseObject<> {
    public:
        StringBuilder() {};
        StringBuilder(const String& base) { Append(base); }
        StringBuilder(const char *str) { Append(str); }

        StringBuilder& Append(char c);
        StringBuilder& Append(const char* str);
        StringBuilder& Append(int i);
        StringBuilder& Append(float f);
        StringBuilder& Append(float f, size_t precision);
        StringBuilder& Append(const std::string& str);
        StringBuilder& Append(const String& str);
        StringBuilder& Append(const StringBuilder& sb);

        StringBuilder& Format(size_t count, ...);
        void Clear();

        size_t GetLength() const { return Data.GetSize() - 1; }
        const char* GetCStr() const { return Data.GetData(); }

        String ToString() const;
        String Steal();

        char operator[](size_t idx) const { return Data[idx]; }

    private:
        Dynarray<char> Data = {'\0'};
    };
}