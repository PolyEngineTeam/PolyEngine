#include <catch.hpp>

#include <TransformComponent.hpp>
#include <Quaternion.hpp>
#include <Angle.hpp>

using namespace Poly;

TEST_CASE("TransformComponent with no parent", "[TransformComponent]")
{
	TransformComponent tc;
	Vector translationVector = Vector(1, 2, 3);
	Vector scaleVector = Vector(4, 5, 6);
	Matrix identity;
	SECTION("Empty constructor")
	{
		REQUIRE(tc.GetLocalTransformationMatrix() == identity);
		REQUIRE(tc.GetGlobalTransformationMatrix() == identity);
	}

	// basic transform setting
	tc.SetLocalTranslation(translationVector);
	tc.SetLocalScale(scaleVector);

	SECTION("Local transformation")
	{
		Matrix tcl = tc.GetLocalTransformationMatrix();
		REQUIRE(tcl.Data[0] == scaleVector.X);
		REQUIRE(tcl.Data[3] == translationVector.X);
		REQUIRE(tcl.Data[5] == scaleVector.Y);
		REQUIRE(tcl.Data[7] == translationVector.Y);
		REQUIRE(tcl.Data[10] == scaleVector.Z);
		REQUIRE(tcl.Data[11] == translationVector.Z);
		REQUIRE(tc.GetLocalScale() == scaleVector);
		REQUIRE(tc.GetLocalTranslation() == translationVector);
	}

	SECTION("Global transformation")
	{
		Matrix tcg = tc.GetGlobalTransformationMatrix();
		REQUIRE(tcg.Data[0] == scaleVector.X);
		REQUIRE(tcg.Data[3] == translationVector.X);
		REQUIRE(tcg.Data[5] == scaleVector.Y);
		REQUIRE(tcg.Data[7] == translationVector.Y);
		REQUIRE(tcg.Data[10] == scaleVector.Z);
		REQUIRE(tcg.Data[11] == translationVector.Z);
		REQUIRE(tc.GetGlobalScale() == scaleVector);
		REQUIRE(tc.GetGlobalTranslation() == translationVector);
	}

	SECTION("Rotation")
	{
		TransformComponent tc2;
		EulerAngles angles = { Angle::FromDegrees(0),
			Angle::FromDegrees(0),
			Angle::FromDegrees(90) };
		Quaternion rotation(angles);
		tc2.SetLocalRotation(rotation);
		float data[] = {0,-1,0,0,
			  1,0,0,0,
			  0,0,1,0,
			  0,0,0,1};
		for (int i = 0; i < 16; i++)
		{
			REQUIRE(Cmpf(tc2.GetLocalTransformationMatrix().Data[i], data[i]));
			REQUIRE(Cmpf(tc2.GetGlobalTransformationMatrix().Data[i], data[i]));
		}
	}
}


TEST_CASE("TransformComponent with parent", "[TransformComponent]")
{
	TransformComponent parent;
	TransformComponent child;
	child.SetParent(&parent);

	SECTION("Translation")
	{
		Vector translation1 = Vector(1, 1, 1);
		Vector translation2 = Vector(1, 1, 1);
		parent.SetLocalTranslation(translation1);
		child.SetLocalTranslation(translation2);
		REQUIRE(child.GetGlobalTranslation() == Vector(2, 2, 2));
	}

	SECTION("Scale")
	{
		float scale1 = 2;
		float scale2 = 3;
		parent.SetLocalScale(scale1);
		child.SetLocalScale(scale2);
		REQUIRE(child.GetGlobalScale() == Vector(6, 6, 6));
	}

	SECTION("Rotation")
	{
		EulerAngles angles1 = { 10_deg, 20_deg, 30_deg };
		Quaternion rotation1(angles1);
		parent.SetLocalRotation(rotation1);
		REQUIRE(Cmpf(child.GetGlobalRotation().X, rotation1.X));
		REQUIRE(Cmpf(child.GetGlobalRotation().Y, rotation1.Y));
		REQUIRE(Cmpf(child.GetGlobalRotation().Z, rotation1.Z));
		REQUIRE(Cmpf(child.GetGlobalRotation().W, rotation1.W));
	}
}

TEST_CASE("Multi-layer hierarchy", "[TransformComponent]")
{
	TransformComponent tc1, tc2, tc3, tc4, tc5, tc6;
	tc2.SetParent(&tc1);
	tc3.SetParent(&tc2);
	tc4.SetParent(&tc3);
	tc5.SetParent(&tc4);
	tc6.SetParent(&tc5);

	Vector translation = Vector(1, 2, 3);
	EulerAngles angles = { 10_deg, 20_deg, 30_deg };
	Quaternion rotation(angles);
	Vector scale = Vector(4, 5, 6);

	tc2.SetLocalTranslation(translation);
	tc4.SetLocalRotation(rotation);
	tc5.SetLocalScale(scale);

	REQUIRE(tc6.GetGlobalTranslation() == translation);
	REQUIRE(Cmpf(tc6.GetGlobalRotation().X, rotation.X));
	REQUIRE(Cmpf(tc6.GetGlobalRotation().Y, rotation.Y));
	REQUIRE(Cmpf(tc6.GetGlobalRotation().Z, rotation.Z));
	REQUIRE(Cmpf(tc6.GetGlobalRotation().W, rotation.W));
	REQUIRE(tc6.GetGlobalScale() == scale);
}

TEST_CASE("Multiple transformation changes", "[TransformComponent]")
{
	TransformComponent tc1, tc2;
	Vector v1 = Vector(1, 1, 1);
	Vector v2 = Vector(2, 2, 2);
	Vector v3 = Vector(3, 3, 3);

	tc2.SetParent(&tc1);

	tc1.SetLocalTranslation(v1);
	tc1.GetGlobalTranslation();
	REQUIRE(tc2.GetGlobalTranslation() == v1);
	REQUIRE(tc2.GetGlobalTransformationMatrix().Data[3] == v1.X);
	tc1.SetLocalTranslation(v2);
	tc1.GetGlobalTransformationMatrix();
	REQUIRE(tc2.GetGlobalTranslation() == v2);
	REQUIRE(tc2.GetGlobalTransformationMatrix().Data[3] == v2.X);
	tc1.SetLocalTranslation(v3);
	tc1.GetGlobalTransformationMatrix();
	REQUIRE(tc2.GetGlobalTranslation() == v3);
	REQUIRE(tc2.GetGlobalTransformationMatrix().Data[3] == v3.X);
}
