#include "TransformControlGroup.hpp"

TransformControlGroup::TransformControlGroup(QWidget * parent)
	: ControlBase(parent)
{
	// TODO(squares): write proper validator

	Layout = new QGridLayout(this);


}

void TransformControlGroup::UpdateObject()
{
}

void TransformControlGroup::UpdateControl()
{
}
