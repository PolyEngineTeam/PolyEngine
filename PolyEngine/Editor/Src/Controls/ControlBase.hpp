#pragma once

#include <QWidget>

#include <RTTI/RTTI.hpp>

using namespace Poly;

class ControlBase : public QWidget
{
public:
	template<RTTI::eCorePropertyType> struct TypeToControl;

	ControlBase(QWidget* parent) : QWidget(parent) {}

	void SetObject(void* ptr, RTTI::Property* prop) { Object = ptr; Property = prop; InitializeControl(); };
	virtual void UpdateObject() = 0;
	virtual void UpdateControl() = 0;

protected:
	virtual void InitializeControl() = 0;

	void* Object;
	RTTI::Property* Property;
};

#define ASSIGN_CONTROL(CONTROL, PROPERTY_TYPE) \
	template<> struct ControlBase::TypeToControl<PROPERTY_TYPE> \
	{ \
		static ControlBase* CreateControl(QWidget* parent) \
		{ \
			return new CONTROL(parent); \
		} \
	};