#pragma once

#include <QWidget>

#include <RTTI/RTTI.hpp>

using namespace Poly;

class ControlBase;

namespace Impl
{
	typedef ControlBase* (*ControlCreatorPtr)(QWidget*);

	extern ControlCreatorPtr* CoreTypeToControlMap;
}

class ControlBase : public QWidget
{
public:
	ControlBase(QWidget* parent) : QWidget(parent) {}

	void SetObject(void* ptr, RTTI::Property* prop) { Object = ptr; Property = prop; InitializeControl(); };
	virtual void UpdateObject() = 0;
	virtual void UpdateControl() = 0;

	static ControlBase* CreateControl(RTTI::eCorePropertyType type, QWidget* parent) 
	{ 
		return ::Impl::CoreTypeToControlMap[static_cast<int>(type)](parent);
	}

protected:
	virtual void InitializeControl() = 0;


	void* Object;
	RTTI::Property* Property;
};

#define DEFINE_CONTROL(CONTROL, CORE_TYPE) \
	namespace Impl \
	{ \
		ControlCreatorPtr* CONTROL##Creator_##CORE_TYPE = \
			new(&::Impl::CoreTypeToControlMap[static_cast<int>(RTTI::eCorePropertyType::##CORE_TYPE)]) \
				ControlCreatorPtr([](QWidget* parent) -> ControlBase* { return new CONTROL(parent); }); \
	}