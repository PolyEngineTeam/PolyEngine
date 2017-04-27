#include <catch.hpp>

#include <TransformComponent.hpp>
#include <Quaternion.hpp>
#include <Angle.hpp>

using namespace Poly;

TEST_CASE("TransformComponent with no parent", "[TransformCompoent]") 
{
	TransformComponent tc;
	Vector translationVector = Vector(1, 2, 3);
	Vector scaleVector = Vector(4, 5, 6);
	SECTION("Empty constructor")
	{
		for (int i = 0; i < 16; ++i)
		{
			REQUIRE(tc.GetLocalTransformationMatrix().Data[i] == (i % 5 == 0 ? 1 : 0));
			REQUIRE(tc.GetGlobalTransformationMatrix().Data[i] == (i % 5 == 0 ? 1 : 0));
		}
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
			REQUIRE(tc2.GetLocalTransformationMatrix().Data[i] - data[i] < 0.000001);
			REQUIRE(tc2.GetGlobalTransformationMatrix().Data[i] - data[i] < 0.000001);
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
		EulerAngles angles1 = { Angle::FromDegrees(10),
			Angle::FromDegrees(20),
			Angle::FromDegrees(30) };
		Quaternion rotation1(angles1);
		parent.SetLocalRotation(rotation1);
		REQUIRE(child.GetGlobalRotation().W - rotation1.W < 0.000001);
		REQUIRE(child.GetGlobalRotation().X - rotation1.X < 0.000001);
		REQUIRE(child.GetGlobalRotation().Y - rotation1.Y < 0.000001);
		REQUIRE(child.GetGlobalRotation().Z - rotation1.Z < 0.000001);
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
	EulerAngles angles = { Angle::FromDegrees(10),
		Angle::FromDegrees(20),
		Angle::FromDegrees(30) };
	Quaternion rotation(angles);
	Vector scale = Vector(4, 5, 6);

	tc2.SetLocalTranslation(translation);
	tc4.SetLocalRotation(rotation);
	tc5.SetLocalScale(scale);

	REQUIRE(tc6.GetGlobalTranslation() == translation);
	REQUIRE(tc6.GetGlobalRotation().W - rotation.W < 0.000001);
	REQUIRE(tc6.GetGlobalRotation().X - rotation.X < 0.000001);
	REQUIRE(tc6.GetGlobalRotation().Y - rotation.Y < 0.000001);
	REQUIRE(tc6.GetGlobalRotation().Z - rotation.Z < 0.000001);
	REQUIRE(tc6.GetGlobalScale() == scale);
}