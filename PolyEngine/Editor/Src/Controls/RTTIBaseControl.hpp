#pragma once

#include <QWidget>

#include <RTTI/RTTI.hpp>

using namespace Poly;

class RTTIBaseControl : public QWidget
{
public:
	virtual void UpdateObject() = 0;
	virtual void UpdateControl() = 0;
};

template<RTTI::eCorePropertyType> struct TypeToControl;

#define ASSIGN_CONTROL(CONTROL, PROPERTY_TYPE) \
	template<> struct TypeToControl<PROPERTY_TYPE> \
	{ \
		static CONTROL* CreateControl(void* ptr, const RTTI::Property& prop) \
		{ \
			return new CONTROL(ptr, prop); \
		} \
	};