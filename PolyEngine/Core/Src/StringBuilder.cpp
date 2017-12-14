#include "StringBuilder.hpp"
#include "CorePCH.hpp"

using namespace Poly;

StringBuilder::StringBuilder()
{
    //TODO init something?
}

//StringBuilder::StringBuilder(const String &base)
//{
//    this->Append(base);
//}

StringBuilder::StringBuilder(const char *str){
    Append(str);
}

StringBuilder& StringBuilder::Append(bool b)
{
    return Append(std::to_string(b).c_str());
}

StringBuilder& StringBuilder::Append(char c)
{
    return Append(std::string(1, c));
}

StringBuilder& StringBuilder::Append(const std::string& str)
{
    return Append(str.c_str());
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

String StringBuilder::ToString() const
{
    return String(Data);
}