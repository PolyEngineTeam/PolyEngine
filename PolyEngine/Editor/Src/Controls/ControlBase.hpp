#pragma once

#include <QWidget>
#include <qtimer.h>

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

	bool ASAPUpdate = true;

protected:
	void* Object = nullptr;
	const RTTI::Property* Property;

public slots:
	void Confirm()
	{
		if (ASAPUpdate)
			QTimer::singleShot(1, this, [object = this]() { object->UpdateObject(); });
	}
};

#define ASSIGN_CONTROL(CONTROL, CORE_TYPE) \
	namespace Impl \
	{ \
		ControlCreator* CONTROL##Creator_##CORE_TYPE = \
			new(&::Impl::CoreTypeToControlMap[static_cast<int>(RTTI::eCorePropertyType::##CORE_TYPE)]) \
				ControlCreator([]() -> ControlBase* { return new CONTROL(); }); \
	}