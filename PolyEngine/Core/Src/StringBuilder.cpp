#include <sstream>
#include <iomanip>
#include <cstdarg>
#include "StringBuilder.hpp"
#include "CorePCH.hpp"

using namespace Poly;

StringBuilder& StringBuilder::Append(char c)
{
    return Append(std::string(1, c));
}

StringBuilder& StringBuilder::Append(int i)
{
    return Append(std::to_string(i).c_str());
}

StringBuilder& StringBuilder::Append(float f)
{
    std::stringstream stream;
    stream << std::fixed << f;
    return Append(stream.str().c_str());
}

StringBuilder& StringBuilder::Append(float f, size_t precision)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << f;
    return Append(stream.str().c_str());
}

StringBuilder& StringBuilder::Append(const std::string& str)
{
    return Append(str.c_str());
}

StringBuilder& StringBuilder::Append(const String& str)
{
    return Append(str.GetCStr());
}

StringBuilder& StringBuilder::Append(const StringBuilder& sb)
{
    return Append(sb.GetCStr());
}

StringBuilder& StringBuilder::Append(const char* data)
{
    size_t length = StrLen(data);
    size_t oldSize = Data.GetSize();
    Data.Resize(Data.GetSize() + length);
    std::memcpy(Data.GetData() + oldSize - 1, data, sizeof(char) * length);
    Data[Data.GetSize()-1] = 0;
    return *this;
}

StringBuilder& StringBuilder::Format(size_t count, ...)
{
    va_list args;
    va_start(args, count);
    String str = ToString().Format(count, args);
    Data = std::move(str.Data);
    return *this;
}

void StringBuilder::Clear()
{
    Data.Clear();
    Data.PushBack('\0');
}

String StringBuilder::ToString() const
{
    return String(Data);
}

String StringBuilder::Steal()
{
    String str = String(std::move(Data));
    Data.PushBack('\0');
    return str;
}