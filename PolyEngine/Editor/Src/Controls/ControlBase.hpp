#pragma once

#include <QtWidgets/qwidget.h>
#include <QtCore/qtimer.h>

#include <RTTI/RTTI.hpp>

#include <Utils/Logger.hpp>

using namespace Poly;

class ControlBase;
class Command;

namespace Impl
{
	// @TODO(squares): use using
	typedef ControlBase* (*ControlCreatorPtr)(QWidget* parent);

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
		ControlBase* operator()(QWidget* parent) { return Ptr(parent); };

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

// This is base class for all controls for core types such as int, string or vector.
// @see Poly::RTTI::eCorePropertyType
class ControlBase : public QWidget
{
	Q_OBJECT

public:
	ControlBase(QWidget* parent) : QWidget(parent) {}

	// Sets object assigned to control and updates this control.
	// @see ControlBase::UpdateControl;
	void SetObject(void* ptr, const RTTI::Property* prop);

	// Reset control to initial state;
	virtual void Reset() { Object = nullptr; DisableEdit = false; Property = nullptr; ASAPUpdate = true; }

	// Call this to update assigned object from current control state.
	// If ControlBase::ASAPUpdate is set to true You probably won't need to use this fnction
	// unless you change control content programmatically.
	virtual void UpdateObject() = 0;

	// Call this to update control state from assigned object.
	// Unlike ControlBase::UpdateObject You have to call this function by yourself.
	// @see ControlBase::SetObject;
	virtual void UpdateControl() = 0;

	// If label is set by control this function returns false.
	// For example TransformControl looks better when label is set by control.
	// Label is set automatically by control.
	virtual bool ContainsLabel() { return false; }

	// With disabled edit control changes its color and becomes inactive.
	void SetDisableEdit(bool disable);

	bool GetDisableEdit() { return DisableEdit; }

	// Use this as slot to connect to Your custom controls' signals like editingFinished in QLineEdit.
	// @see StringControl::StringControl
	virtual void Confirm() {};

	// Returns ptr to newly created proper control for given core type.
	static ControlBase* CreateControl(QWidget* parent, RTTI::eCorePropertyType type)
	{ 
		return ::Impl::CoreTypeToControlMap[static_cast<int>(type)](parent);
	}

	// if You want this control to update object as soon as state is changed, enter pressed or 
	// focus lost set this to true. Otherwise You will have to update it calling UpdateObject.
	// This applies only for OBJECT opdate; you still have to update CONTROL "manually"
	// @see ControlBase::UpdateObject
	bool ASAPUpdate = true;

signals:
	// After object is updated this signal is emitted.
	// @param cmd - pointer to command object to enable undo/redo actions.
	void ObjectUpdated(Command* cmd);

protected:
	// Pointer to an object which is assigned to this control.
	void* Object = nullptr;

	// Property of assigned object. Useful when one control is assigned to multiple core types
	// like NumberControl.
	const RTTI::Property* Property;

	bool DisableEdit = false;
	bool DisableUpdateControl = false;
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
			ControlCreator([](QWidget* parent) -> ControlBase* { return new CONTROL(parent); }); \
	}
