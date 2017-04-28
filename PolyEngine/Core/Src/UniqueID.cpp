#include "CorePCH.hpp"

using namespace Poly;

UniqueID::UniqueID() {}
UniqueID UniqueID::Generate() { static size_t val = 0; return UniqueID(++val); }

bool UniqueID::operator==(const UniqueID& rhs) const { return ID == rhs.ID; }
bool UniqueID::operator!=(const UniqueID& rhs) const { return !(*this == rhs); }

UniqueID::operator bool() const { return ID != 0; }
UniqueID::UniqueID(size_t id) : ID(id) {}
