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

        inline StringBuilder& AppendFormat(const String& fmt)
        {
            Append(fmt);
            Data[Data.GetSize()-1] = 0;
            return *this;
        }

        template <typename T, typename... Args>
        StringBuilder& AppendFormat(const char* fmt, const T& head, Args&&... tail)
        {
            return AppendFormat(String(fmt), head, tail...);
        }

        template <typename T, typename... Args>
        StringBuilder& AppendFormat(const String& fmt, const T& head, Args&&... tail)
        {
            constexpr char marker[] = "{}";
            std::string::size_type format_marker_pos = fmt.FindSubstrFromPoint(0, marker);
            Append(fmt.Substring(0, format_marker_pos));
            if (format_marker_pos != fmt.GetLength())
            {
                Append(head);
                AppendFormat(fmt.Substring(format_marker_pos + sizeof(marker) -1, fmt.GetLength()), tail...);
            }
            else
            {
                Data[Data.GetSize() - 1] = 0;
            }
            return *this;
        }

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