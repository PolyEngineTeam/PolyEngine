#pragma once

#include "Dynarray.hpp"
#include "Defines.hpp"
#include "String.hpp"

namespace Poly {
    class CORE_DLLEXPORT StringBuilder : BaseObject<> {
    public:
        StringBuilder();
//        StringBuilder(const String& base);
        StringBuilder(const char *str);
        StringBuilder(size_t initial_size);

        StringBuilder& Append(bool b);
        StringBuilder& Append(char c);
        StringBuilder& Append(const char* str);
//        StringBuilder& Append(int i);
//        StringBuilder& Append(float f);
//        StringBuilder& Append(float f, size_t precision);
        StringBuilder& Append(const std::string& str);
//        StringBuilder& Append(String* const &str);
//        StringBuilder& Append(const StringBuilder& sb);

        //character count
        int Length() const;
        char CharAt() const;

        String ToString() const;

    private:
        Dynarray<char> Data = {'\0'};
    };
}