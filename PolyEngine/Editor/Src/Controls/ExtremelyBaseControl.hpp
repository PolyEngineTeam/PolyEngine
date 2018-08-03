#pragma once

#include <QtWidgets/qwidget.h>

#include <Core.hpp>

class ExtremelyBaseControl;
class Command;

namespace Impl
{
	// @TODO(squares): use using
	typedef ExtremelyBaseControl* (*ControlCreatorPtr)(QWidget* parent);

	// ControlCreator stores pointer to function that creates and returns pointer to 
	// an object of particular control. This method is called with operator().
	// This class is needed because we needed to override operator new to enable adding new controls within
	// global scope (using ASSIGN_CONTROL macro somewhere in that control *.cpp file).
	// Otherwise when designing new control class we would need to add control to map in some 
	// control manager at initialization. We can't use templates because in RTTIViewer we don't know the type 
	// of particular object during the compilation time.
	class ControlCreator
	{
	public:
		// 
		ControlCreator() = default;

		// Initializes pointer to function with given value.
		ControlCreator(ControlCreatorPtr ptr) : Ptr(ptr) {}

		// @param parent - parent for newly created control.
		ExtremelyBaseControl* operator()(QWidget* parent) { return Ptr(parent); };

		// We need to override placement new because we want to add this object to array so we can easily 
		// map from type enum to control creators but firstly this array must be initialized and all
		// insertions to this array happens in global scope using placement new in different *.cpp files
		// so we don't know which code is called first so the first creator that is created also initializes 
		// our array.
		void* operator new(size_t, void* where);

	private:
		ControlCreatorPtr Ptr;
	};

	extern ControlCreator* CoreTypeToControlMap;
}

class ExtremelyBaseControl : public QWidget
{
	Q_OBJECT

public:
	ExtremelyBaseControl(QWidget* parent) : QWidget(parent) {}
	
	// These two functions are here only because wee need to call these in RTTIViewer using ExtremelyBaseControl pointers.
	virtual void SetObject(void* ptr, ::Poly::String name, ::Poly::String type) = 0;
	virtual void UpdateControl() = 0;

	// Returns ptr to newly created proper control for given core type.
	static ExtremelyBaseControl* CreateControl(QWidget* parent, ::Poly::RTTI::eCorePropertyType type)
	{
		return ::Impl::CoreTypeToControlMap[static_cast<int>(type)](parent);
	}

signals:
	// After object is updated this signal is emitted.
	// @param cmd - pointer to command object to enable undo/redo actions.
	void ObjectUpdated(Command* cmd);
};

// Use this to add Your control to map from core type to control creator function.
// @param CONTROL - control type to register
// @param CORE_TYPE - core type that will be paired with given control type so 
//		we can easily map from RTTI::eCorePropertyType to control.
// @param NAME - we don't need it much but all pointers have to have different names.
//		It is a problem when we try to assign more than one core types to single control.
//
// 1. Here we create new global pointer to ControlCreator object which only purpose is to 
//		let us call placement new on ControlCreator.
// 2. We pass address of one cell from Impl::CoreTypeToControlMap that corresponds with given 
//		type (RTTI::eCorePropertyType) as an argument to placement new.
// 3. As an argument to ControlCreator constructor we pass a function that creates new instance of CONTROL.
#define ASSIGN_CONTROL(CONTROL, CORE_TYPE, NAME) \
	namespace Impl \
	{ \
		ControlCreator* CONTROL##Creator##NAME = \
			new(&::Impl::CoreTypeToControlMap[static_cast<int>(CORE_TYPE)]) \
			ControlCreator([](QWidget* parent) -> ExtremelyBaseControl* { return new CONTROL(parent); }); \
	}

#define ASSIGN_PARAMETRIZED_CONTROL(CONTROL, PARAM, CORE_TYPE, NAME) \
	namespace Impl \
	{ \
		ControlCreator* CONTROL##Creator##NAME = \
			new(&::Impl::CoreTypeToControlMap[static_cast<int>(CORE_TYPE)]) \
			ControlCreator([](QWidget* parent) -> ExtremelyBaseControl* { return new CONTROL<PARAM>(parent); }); \
	}
