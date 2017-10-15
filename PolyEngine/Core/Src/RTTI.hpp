#pragma once

#include "Defines.hpp"
#include "BaseObject.hpp"

#include "RTTICast.hpp"
#include "RTTITypeInfo.hpp"
#include "RTTIProperty.hpp"

// basic types declaration
RTTI_DECLARE_PRIMITIVE_TYPE(bool)
RTTI_DECLARE_PRIMITIVE_TYPE(i8)
RTTI_DECLARE_PRIMITIVE_TYPE(i16)
RTTI_DECLARE_PRIMITIVE_TYPE(i32)
RTTI_DECLARE_PRIMITIVE_TYPE(i64)
RTTI_DECLARE_PRIMITIVE_TYPE(u8)
RTTI_DECLARE_PRIMITIVE_TYPE(u16)
RTTI_DECLARE_PRIMITIVE_TYPE(u32)
RTTI_DECLARE_PRIMITIVE_TYPE(u64)
RTTI_DECLARE_PRIMITIVE_TYPE(f32)
RTTI_DECLARE_PRIMITIVE_TYPE(f64)

// FIXME What to do with those? Under MSVC there are == to int32_t and uint32_t resulting in doubled template specification
//RTTI_DECLARE_PRIMITIVE_TYPE(int)
//RTTI_DECLARE_PRIMITIVE_TYPE(uint)

namespace Poly {
	class RTTIBase : public BaseObject<> {
		RTTI_DECLARE_TYPE(Poly::RTTIBase) { NO_RTTI_PROPERTY(); }
	public:
		// TODO rework this API
		virtual void Init() {
			if (Poly::RTTI::IPropertyManager* mgr = GetPropertyManager()) {
				mgr->Deserialize();
			}
		}

		virtual void Destroy() {
			if (Poly::RTTI::IPropertyManager* mgr = GetPropertyManager()) {
				mgr->Serialize();
			}
		}
	};
}
// FIXME Clang really don't like template specialization inside of namespaces. For now this needs to be in global scope.
