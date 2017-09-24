#pragma once

#include "Defines.hpp"

#include "RTTICast.hpp"
#include "RTTITypeInfo.hpp"
#include "RTTIProperty.hpp"

// basic types declaration
RTTI_DECLARE_TYPE(bool)
RTTI_DECLARE_TYPE(i8)
RTTI_DECLARE_TYPE(i16)
RTTI_DECLARE_TYPE(i32)
RTTI_DECLARE_TYPE(i64)
RTTI_DECLARE_TYPE(u8)
RTTI_DECLARE_TYPE(u16)
RTTI_DECLARE_TYPE(u32)
RTTI_DECLARE_TYPE(u64)
RTTI_DECLARE_TYPE(f32)
RTTI_DECLARE_TYPE(f64)

// FIXME What to do with those? Under MSVC there are == to int32_t and uint32_t resulting in doubled template specification
//RTTI_DECLARE_TYPE(int)
//RTTI_DECLARE_TYPE(uint)

namespace Poly {
	class RTTIBase {
		RTTI_ENABLE()
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

		virtual Poly::RTTI::IPropertyManager* GetPropertyManager() { return nullptr; }
	};
	RTTI_DECLARE_TYPE(RTTIBase)
}