#pragma once

#include <QWidget>

#include <RTTI/RTTI.hpp>

using namespace Poly;

namespace Impl
{
	typedef ControlBase* (*ControlCreatorPtr)(QWidget*);
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
		return Map[static_cast<int>(type)](parent);	
	}

	static ::Impl::ControlCreatorPtr Map[static_cast<int>(RTTI::eCorePropertyType::_COUNT)];

protected:
	virtual void InitializeControl() = 0;


	void* Object;
	RTTI::Property* Property;
};

#define DEFINE_CONTROL(CONTROL, CORE_TYPE) \
	namespace Impl \
	{ \
		ControlCreatorPtr* CONTROLCreator = new(ControlBase::Map[static_cast<int>(CORE_TYPE)] ControlCreatorPtr ([](QWidget* parent) -> ControlBase* { return new CONTROL(parent); }); \
	}