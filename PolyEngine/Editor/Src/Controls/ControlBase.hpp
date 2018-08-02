#pragma once

#include "Controls/IControlBase.hpp"

class ControlBase : public IControlBase
{
public:
	ControlBase(QWidget* parent) : IControlBase(parent) { Reset(); }

	virtual void SetObject(void* ptr, const RTTI::Property* prop) override;
	virtual void Reset() override;
	virtual bool ContainsLabel() override;
	virtual void SetDisableEdit(bool disable) override;
	virtual bool GetDisableEdit() override;

private:
	// Pointer to an object which is assigned to this control.
	void* Object = nullptr;

	// Property of assigned object. Useful when one control is assigned to multiple core types
	// like NumberControl.
	const RTTI::Property* Property;

	bool DisableEdit = false;
	bool DisableUpdateControl = false;
};