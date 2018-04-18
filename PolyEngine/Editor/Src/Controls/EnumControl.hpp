#pragma once

#include "RTTIBaseControl.hpp"

using namespace Poly;

class EnumControl : public RTTIBaseControl
{
public:
	EnumControl(void*);

	void UpdateObject() override;
	void UpdateControl() override;
};

class EnumControl1 : public RTTIBaseControl
{
public:
	EnumControl1(void*);

	void UpdateObject() override;
	void UpdateControl() override;
};

class EnumControl2 : public RTTIBaseControl
{
public:
	EnumControl2(void*);

	void UpdateObject() override;
	void UpdateControl() override;
};

class EnumControl3 : public RTTIBaseControl
{
public:
	EnumControl3(void*);

	void UpdateObject() override;
	void UpdateControl() override;
}; 

ASSIGN_CONTROL(EnumControl, RTTI::eCorePropertyType::BOOL)
ASSIGN_CONTROL(EnumControl1, RTTI::eCorePropertyType::INT8)
ASSIGN_CONTROL(EnumControl2, RTTI::eCorePropertyType::INT16)
ASSIGN_CONTROL(EnumControl3, RTTI::eCorePropertyType::INT32)

int dupa()
{
	void* v = new int;
	TypeToControl<RTTI::eCorePropertyType::BOOL>::CreateControl(v);
}