#include "PolyEditorPCH.hpp"

#include <QIntValidator>
#include <QDoubleValidator>

#include "Managers/CommandsImpl.hpp"

ASSIGN_PARAMETRIZED_CONTROL(NumberControl, i8, RTTI::eCorePropertyType::INT8, INT8)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, i16, RTTI::eCorePropertyType::INT16, INT16)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, i32, RTTI::eCorePropertyType::INT32, INT32)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, i64, RTTI::eCorePropertyType::INT64, INT64)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, u8, RTTI::eCorePropertyType::UINT8, UINT8)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, u16, RTTI::eCorePropertyType::UINT16, UINT16)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, u32, RTTI::eCorePropertyType::UINT32, UINT32)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, u64, RTTI::eCorePropertyType::UINT64, UINT64)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, f32, RTTI::eCorePropertyType::FLOAT, FLOAT)
ASSIGN_PARAMETRIZED_CONTROL(NumberControl, f64, RTTI::eCorePropertyType::DOUBLE, DOUBLE)
