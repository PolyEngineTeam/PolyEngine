#pragma once

#include <QtWidgets/qwidget.h>
#include <QtCore/qtimer.h>

#include <RTTI/RTTI.hpp>

using namespace Poly;

class ControlBase;

namespace Impl
{
	typedef ControlBase* (*ControlCreatorPtr)(QWidget* parent);

	class ControlCreator
	{
	public:
		ControlCreator() = default;
		ControlCreator(ControlCreatorPtr ptr) : Ptr(ptr) {}
		ControlBase* operator()(QWidget* parent) { return Ptr(parent); };

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

	// Sets object assigned to control ans updates this control.
	// @see ControlBase::UpdateControl;
	void SetObject(void* ptr, const RTTI::Property* prop);

	// Reset control to initial state;
	virtual void Reset() = 0;

	// Call this to update assigned object from current control state.
	// If ControlBase::ASAPUpdate is set to true You probably won't need to use this fnction.
	virtual void UpdateObject() = 0;

	// Call this to update control state from assigned object.
	// Unlike ControlBase::UpdateObject You have to call this function by yourself.
	// Besides this function is getting called every time you call ControlBase::SetObject.
	// @see ControlBase::SetObject;
	virtual void UpdateControl() = 0;

	// If label is set by control this function returns false.
	// For example TransformControl looks better when label is set by control.
	// Label is set automatically by control
	virtual bool ContainsLabel() { return false; }

	void SetDisableEdit(bool disable);

	bool GetDisableEdit() { return DisableEdit; }

	// Returns ptr to newly created proper control for given core type.
	static ControlBase* CreateControl(QWidget* parent, RTTI::eCorePropertyType type)
	{ 
		return ::Impl::CoreTypeToControlMap[static_cast<int>(type)](parent);
	}

	// if You want this control to update object as soon as state is changed, enter pressed or 
	// focus lost set this to true. Otherwise You will have to update it calling UpdateObject.
	// This applies only for object opdate; you still have to update control "manually"
	// @see ControlBase::UpdateObject
	bool ASAPUpdate = true;

signals:
	void ObjectUpdated();

protected:
	// Pointer to an object which is assigned to this control.
	void* Object = nullptr;

	// Property of assigned object. Useful when one control is assigned to multiple core types
	// like NumberControl.
	const RTTI::Property* Property;

	bool DisableEdit = false;

public slots:
	// Use this as slot to connect to Your fields' signals like editingFinished in QLineEdit.
	void Confirm()
	{
		if (ASAPUpdate)
			QTimer::singleShot(1, this, [object = this]() { object->UpdateObject(); });
	}
};

// Use this to add Your control to map from core type to control creator function.
#define ASSIGN_CONTROL(CONTROL, CORE_TYPE, NAME) \
	namespace Impl \
	{ \
		ControlCreator* CONTROL##Creator_##NAME = \
			new(&::Impl::CoreTypeToControlMap[static_cast<int>(CORE_TYPE)]) \
				ControlCreator([](QWidget* parent) -> ControlBase* { return new CONTROL(parent); }); \
	}
