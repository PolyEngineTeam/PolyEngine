#pragma once

#include <QWidget>

#include <RTTI/RTTI.hpp>

using namespace Poly;

class ControlBase;

namespace Impl
{
	typedef ControlBase* (*ControlCreatorPtr)();

	class ControlCreator
	{
	public:
		ControlCreator() = default;
		ControlCreator(ControlCreatorPtr ptr) : Ptr(ptr) {}
		ControlBase* operator()() { return Ptr(); };

		void* operator new(size_t, void* where);
		
	private:
		ControlCreatorPtr Ptr;
	};

	extern ControlCreator* CoreTypeToControlMap;
}

class ControlBase : public QWidget
{
public:
	ControlBase() = default;

	void SetObject(void* ptr, const RTTI::Property* prop);
	virtual void UpdateObject() = 0;
	virtual void UpdateControl() = 0;

	static ControlBase* CreateControl(RTTI::eCorePropertyType type) 
	{ 
		return ::Impl::CoreTypeToControlMap[static_cast<int>(type)]();
	}

	bool UpdateOnFocusOut = true;

protected:
	virtual void InitializeControl() = 0;

	void* Object;
	const RTTI::Property* Property;

private:
	void focusOutEvent(QFocusEvent *event) { if (UpdateOnFocusOut) UpdateControl(); }
};

#define ASSIGN_CONTROL(CONTROL, CORE_TYPE) \
	namespace Impl \
	{ \
		ControlCreator* CONTROL##Creator_##CORE_TYPE = \
			new(&::Impl::CoreTypeToControlMap[static_cast<int>(RTTI::eCorePropertyType::##CORE_TYPE)]) \
				ControlCreator([]() -> ControlBase* { return new CONTROL(); }); \
	}