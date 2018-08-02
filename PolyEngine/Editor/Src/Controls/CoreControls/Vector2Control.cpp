#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

#include "Math/Vector2f.hpp"
#include "Math/Vector2i.hpp"

ASSIGN_PARAMETRIZED_CONTROL(Vector2Control, Vector2f, RTTI::eCorePropertyType::VECTOR_2F, Vector2f)
ASSIGN_PARAMETRIZED_CONTROL(Vector2Control, Vector2i, RTTI::eCorePropertyType::VECTOR_2I, Vector2i)