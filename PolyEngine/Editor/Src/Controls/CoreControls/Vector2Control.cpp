#include "PolyEditorPCH.hpp"

#include "Managers/CommandsImpl.hpp"

#include "pe/core/math/Vector2f.hpp"
#include "pe/core/math/Vector2i.hpp"

ASSIGN_PARAMETRIZED_CONTROL(Vector2Control, ::pe::core::math::Vector2f, RTTI::eCorePropertyType::VECTOR_2F, Vector2f)
ASSIGN_PARAMETRIZED_CONTROL(Vector2Control, ::pe::core::math::Vector2i, RTTI::eCorePropertyType::VECTOR_2I, Vector2i)