#include "PolyEditorPCH.hpp"

TransformControl::TransformControl(QWidget* parent)
	: ControlBase(parent)
{
	// TODO(squares): write proper validator

	Layout = new QGridLayout(this);

	Layout->setColumnStretch(0, 1);
	Layout->setColumnStretch(1, 1);
	Layout->setColumnStretch(2, 1);
	Layout->setColumnStretch(3, 1);

	TranslationLabel = new QLabel(this);
	TranslationLabel->setText("Translation");
	Layout->addWidget(TranslationLabel, 1, 0);

	RotationLabel = new QLabel(this);
	RotationLabel->setText("Rotation");
	Layout->addWidget(RotationLabel, 2, 0);

	ScaleLabel = new QLabel(this);
	ScaleLabel->setText("Scale");
	Layout->addWidget(ScaleLabel, 3, 0);

	XYZLabels[0] = new QLabel(this);
	XYZLabels[0]->setText("X");
	XYZLabels[0]->setAlignment(Qt::AlignCenter);
	Layout->addWidget(XYZLabels[0], 0, 1);

	XYZLabels[1] = new QLabel(this);
	XYZLabels[1]->setText("Y");
	XYZLabels[1]->setAlignment(Qt::AlignCenter);
	Layout->addWidget(XYZLabels[1], 0, 2);

	XYZLabels[2] = new QLabel(this);
	XYZLabels[2]->setText("Z");
	XYZLabels[2]->setAlignment(Qt::AlignCenter);
	Layout->addWidget(XYZLabels[2], 0, 3);

	for (int x = 0; x < 3; ++x)
	{
		TranslationField[x] = new QLineEdit(this);
		Layout->addWidget(TranslationField[x], 1, 1 + x);
		connect(TranslationField[x], &QLineEdit::editingFinished, this, &TransformControl::ConfirmTranslation);

		RotationField[x] = new QLineEdit(this);
		Layout->addWidget(RotationField[x], 2, 1 + x);
		connect(RotationField[x], &QLineEdit::editingFinished, this, &TransformControl::ConfirmRotation);

		ScaleField[x] = new QLineEdit(this);
		Layout->addWidget(ScaleField[x], 3, 1 + x);
		connect(ScaleField[x], &QLineEdit::editingFinished, this, &TransformControl::ConfirmScale);
	}
}

void TransformControl::Reset()
{
	Object = nullptr;

	TranslationField[0]->setText("");
	TranslationField[1]->setText("");
	TranslationField[2]->setText("");

	RotationField[0]->setText("");
	RotationField[1]->setText("");
	RotationField[2]->setText("");

	ScaleField[0]->setText("");
	ScaleField[1]->setText("");
	ScaleField[2]->setText("");
}

void TransformControl::UpdateControl()
{
	EntityTransform* transform = static_cast<EntityTransform*>(Object);

	if (!(TranslationField[0]->hasFocus() || TranslationField[1]->hasFocus() || TranslationField[2]->hasFocus()))
	{
		core::math::Vector translation = transform->GetLocalTranslation();
		TranslationField[0]->setText(QString(::pe::core::storage::String::From(translation.X).GetCStr()));
		TranslationField[1]->setText(QString(::pe::core::storage::String::From(translation.Y).GetCStr()));
		TranslationField[2]->setText(QString(::pe::core::storage::String::From(translation.Z).GetCStr()));
	}

	if (!(RotationField[0]->hasFocus() || RotationField[1]->hasFocus() || RotationField[2]->hasFocus()))
	{
		core::math::EulerAngles rotation = transform->GetLocalRotation().ToEulerAngles();
		RotationField[0]->setText(QString(::pe::core::storage::String::From(rotation.X.AsDegrees()).GetCStr()));
		RotationField[1]->setText(QString(::pe::core::storage::String::From(rotation.Y.AsDegrees()).GetCStr()));
		RotationField[2]->setText(QString(::pe::core::storage::String::From(rotation.Z.AsDegrees()).GetCStr()));
	}

	if (!(ScaleField[0]->hasFocus() || ScaleField[1]->hasFocus() || ScaleField[2]->hasFocus()))
	{
		core::math::Vector scale = transform->GetLocalScale();
		ScaleField[0]->setText(QString(::pe::core::storage::String::From(scale.X).GetCStr()));
		ScaleField[1]->setText(QString(::pe::core::storage::String::From(scale.Y).GetCStr()));
		ScaleField[2]->setText(QString(::pe::core::storage::String::From(scale.Z).GetCStr()));
	}
}

void TransformControl::UodateTranslation()
{
	EntityTransform* transform = static_cast<EntityTransform*>(Object);

	core::math::Vector translation;
	translation.X = TranslationField[0]->text().toFloat();
	translation.Y = TranslationField[1]->text().toFloat();
	translation.Z = TranslationField[2]->text().toFloat();
	transform->SetLocalTranslation(translation);
}

void TransformControl::UodateRotation()
{
	EntityTransform* transform = static_cast<EntityTransform*>(Object);

	core::math::EulerAngles rotation;
	rotation.X = core::math::Angle::FromDegrees(RotationField[0]->text().toFloat());
	rotation.Y = core::math::Angle::FromDegrees(RotationField[1]->text().toFloat());
	rotation.Z = core::math::Angle::FromDegrees(RotationField[2]->text().toFloat());
	transform->SetLocalRotation(::pe::core::math::Quaternion(rotation));
}

void TransformControl::UodateScale()
{
	EntityTransform* transform = static_cast<EntityTransform*>(Object);

	core::math::Vector scale;
	scale.X = ScaleField[0]->text().toFloat();
	scale.Y = ScaleField[1]->text().toFloat();
	scale.Z = ScaleField[2]->text().toFloat();
	transform->SetLocalScale(scale);
}
