#include "EnginePCH.hpp"

#include "Debugging/DebugDrawComponents.hpp"
#include "Debugging/DebugDrawSystem.hpp"
#include "Rendering/ViewportWorldComponent.hpp"
#include "Rendering/Camera/CameraComponent.hpp"
#include "Rendering/MeshRenderingComponent.hpp"
#include "Rendering/Lighting/LightSourceComponent.hpp"
#include "ECS/Scene.hpp"
#include "ECS/DeferredTaskSystem.hpp"
#include "Movement/MovementSystem.hpp"
#include "UI/ScreenSpaceTextComponent.hpp"
#include "Physics2D/Rigidbody2DComponent.hpp"

using namespace Poly;

namespace Util
{
	/// <summary>Get vector with minimum/maximum/else (depends on comparison function) values
	/// on each axis in given set of vectors.</summary>
	Vector3f FindExtremum(const Dynarray<Vector3f>& dynarray, std::function< bool(const float, const float) > comp)
	{
		ASSERTE(dynarray.GetSize() > 0, "SubMesh vertex data is empty.");

		Vector3f result = dynarray[0];
		for (auto const & vec : dynarray)
		{
			if (comp(vec.X, result.X))
				result.X = vec.X;
			if (comp(vec.Y, result.Y))
				result.Y = vec.Y;
			if (comp(vec.Z, result.Z))
				result.Z = vec.Z;
		}

		return result;
	}

	/// <summary>Produces two orthonormal vectors from a single normal vector.</summary>
	/// Building an Orthonormal Basis, Revisited; http://jcgt.org/published/0006/01/01/
	void BranchlessONB(const Vector & n, Vector & b1, Vector & b2)
	{
		float sign = std::copysignf(1.0f, n.Z);
		const float a = -1.0f / (sign + n.Z);
		const float b = n.X * n.Y * a;
		b1 = Vector(1.0f + sign * n.X * n.X * a, sign * b, -sign * n.X);
		b2 = Vector(b, sign + n.Y * n.Y * a, -n.Y);
	}
}

void DebugDrawSystem::DebugRenderingUpdatePhase(Scene* world)
{
	gDebugConfig.DebugRender = false;
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		CameraComponent* cameraCmp = kv.second.GetCamera();
		if (cameraCmp->GetRenderingMode() == eRenderingModeType::IMMEDIATE_DEBUG)
			gDebugConfig.DebugRender = true;
	}

	auto debugDrawWorldCmp = world->GetWorldComponent<DebugDrawStateWorldComponent>();
	if (!gDebugConfig.DebugRender)
	{
		debugDrawWorldCmp->Clear();
		return;
	}

	// Prepare text2D to render
	{
		size_t usedTextEntites = 0;
		for (const auto& text2D : debugDrawWorldCmp->DebugTexts2D)
		{
			Entity* entToUse = nullptr;
			while (usedTextEntites < debugDrawWorldCmp->Text2DEntityPool.GetSize())
			{
				if (debugDrawWorldCmp->Text2DEntityPool[usedTextEntites])
				{
					entToUse = debugDrawWorldCmp->Text2DEntityPool[usedTextEntites].Get();
					++usedTextEntites;
					break;
				}
				else
					debugDrawWorldCmp->Text2DEntityPool.RemoveByIdx(usedTextEntites);
			}

			if (!entToUse)
			{
				entToUse = DeferredTaskSystem::SpawnEntityImmediate(world);
				DeferredTaskSystem::AddComponentImmediate<ScreenSpaceTextComponent>(world, entToUse, Vector2i::ZERO, "Fonts/Raleway/Raleway-Regular.ttf", eResourceSource::ENGINE, 0);
				debugDrawWorldCmp->Text2DEntityPool.PushBack(entToUse);
				++usedTextEntites;
			}

			ScreenSpaceTextComponent* textCmp = entToUse->GetComponent<ScreenSpaceTextComponent>();
			textCmp->SetText(text2D.Text);
			textCmp->GetText().SetFontSize(text2D.FontSize);
			textCmp->GetText().SetFontColor(text2D.FontColor);
			textCmp->SetScreenPosition(text2D.Position);
		}


		// Clear rest of the text entities
		for (size_t i = usedTextEntites; i < debugDrawWorldCmp->Text2DEntityPool.GetSize(); ++i)
		{
			Entity* ent = debugDrawWorldCmp->Text2DEntityPool[i].Get();
			if (ent)
			{
				ScreenSpaceTextComponent* textCmp = ent->GetComponent<ScreenSpaceTextComponent>();
				textCmp->SetText("");
				textCmp->GetText().SetFontSize(0);
				textCmp->GetText().SetFontColor(Color::WHITE);
				textCmp->SetScreenPosition(Vector2i::ZERO);
			}
			else
			{
				debugDrawWorldCmp->Text2DEntityPool.RemoveByIdx(i);
				--i;
			}
		}

		debugDrawWorldCmp->DebugTexts2D.Clear();
	}

	enum class RenderMode : int { /*POINT = 1,*/ LINE = 1, /*STRING,*/ _COUNT };

	// iterate RenderMode::_COUNT times to create shapes defining debug primitives
	for(int renderMode = static_cast<int>(RenderMode::LINE); renderMode < static_cast<int>(RenderMode::_COUNT); ++renderMode)
	{
		for(auto componentsTuple : world->IterateComponents<MeshRenderingComponent, DebugDrawableComponent>())
		{
			const auto ddrawCmp = std::get<DebugDrawableComponent*>(componentsTuple);
			const auto meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
			const auto& transform = meshCmp->GetTransform();

			if(ddrawCmp == nullptr)
				continue;

			if(!gDebugConfig.DebugDrawPresets.IsSet(ddrawCmp->entityPreset))
				continue;

			const Matrix& objTransform = transform.GetWorldFromModel();

			// spawn a box for every mesh, in correct size
			for(const auto subMesh : meshCmp->GetMesh()->GetSubMeshes())
			{
				AABox box = subMesh->GetAABox();
				Vector minVector = objTransform * box.GetMin();
				Vector maxVector = objTransform * box.GetMax();

				const auto boundingOffset = Vector(0.04f, 0.04f, 0.04f);
				// move vector away a little bit from a mesh
				minVector -= boundingOffset;
				maxVector += boundingOffset;
				
				DrawBox(world, minVector, maxVector, Color::GREEN);
			}
		}

		// @fixme
		// move iteration over RigidBody2DComponent to different debug-system (f.ex. PhysicsDebugDrawSystem)
		for(auto componentsTuple : world->IterateComponents<RigidBody2DComponent, DebugDrawableComponent>())
		{
			const auto rigidbodyCmp = std::get<RigidBody2DComponent*>(componentsTuple);
			const auto ddrawCmp = std::get<DebugDrawableComponent*>(componentsTuple);
			const auto& transform = rigidbodyCmp->GetTransform();

			if(ddrawCmp == nullptr)
				continue;

			if(!gDebugConfig.DebugDrawPresets.IsSet(ddrawCmp->entityPreset))
				continue;

			auto localTrans = transform.GetLocalTranslation();
			auto velocity = rigidbodyCmp->GetLinearVelocity();

			if(velocity.LengthSquared() == 0.0f)
				continue;

			DrawArrow(world, localTrans, velocity, Color::RED);
		}


		for(auto componentsTuple : world->IterateComponents<DirectionalLightComponent, DebugDrawableComponent>())
		{
			const auto dirLightCmp = std::get<DirectionalLightComponent*>(componentsTuple);
			const auto ddrawCmp = std::get<DebugDrawableComponent*>(componentsTuple);
			const auto transform = dirLightCmp->GetTransform();

			if(ddrawCmp == nullptr)
				continue;

			if(!gDebugConfig.DebugDrawPresets.IsSet(ddrawCmp->entityPreset))
				continue;

			auto localTrans = transform.GetLocalTranslation();
			//auto lightDirection = Vector(1.0f, 1.0f, 1.0f);
			auto directionRotation = MovementSystem::GetGlobalForward(transform);
			auto lightMagnitude = dirLightCmp->GetIntensity() * 2.0f;

			DrawArrow(world, localTrans, directionRotation*lightMagnitude, Color::WHITE);
		}
	}
}

void Poly::DebugDrawSystem::DrawLine(Scene* world, const Vector& begin, const Vector& end, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	Vector3f meshvecBegin(begin.X, begin.Y, begin.Z), meshvecEnd(end.X, end.Y, end.Z);
	auto debugLinesComponent = world->GetWorldComponent<DebugDrawStateWorldComponent>();
	debugLinesComponent->DebugLines.PushBack(DebugDrawStateWorldComponent::DebugLine{ meshvecBegin, meshvecEnd });
	debugLinesComponent->DebugLinesColors.PushBack(DebugDrawStateWorldComponent::DebugLineColor{ color, color });
}

void Poly::DebugDrawSystem::DrawBox(Scene* world, const Vector& mins, const Vector& maxs, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	std::array<Vector, 8> points;
	std::array<Vector, 2> minmaxVector = { {mins, maxs} };

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		points[i].X = minmaxVector[(i ^ (i >> 1)) & 1].X;
		points[i].Y = minmaxVector[(i >> 1) & 1].Y;
		points[i].Z = minmaxVector[(i >> 2) & 1].Z;
	}
	// i: X Y Z
	// 0: 0 0 0
	// 1: 1 0 0
	// 2: 1 1 0
	// 3: 0 1 0
	// 4: 0 0 1
	// 5: 1 0 1
	// 6: 1 1 1
	// 7: 0 1 1

	// bottom
	DrawLine(world, points[0], points[4], color);
	DrawLine(world, points[0], points[1], color);
	DrawLine(world, points[5], points[4], color);
	DrawLine(world, points[5], points[1], color);

	// top
	DrawLine(world, points[3], points[7], color);
	DrawLine(world, points[3], points[2], color);
	DrawLine(world, points[6], points[7], color);
	DrawLine(world, points[6], points[2], color);

	// top-bottom lines
	DrawLine(world, points[0], points[3], color);
	DrawLine(world, points[1], points[2], color);
	DrawLine(world, points[4], points[7], color);
	DrawLine(world, points[5], points[6], color);
}

void DebugDrawSystem::DrawBox(Scene* world, const Vector& mins, const Vector& maxs, const Matrix& worldFromSpace, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	std::array<Vector, 8> points;
	std::array<Vector, 2> minmaxVector = { { mins, maxs } };

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		points[i].X = minmaxVector[(i ^ (i >> 1)) & 1].X;
		points[i].Y = minmaxVector[(i >> 1) & 1].Y;
		points[i].Z = minmaxVector[(i >> 2) & 1].Z;
	}
	// i: X Y Z
	// 0: 0 0 0
	// 1: 1 0 0
	// 2: 1 1 0
	// 3: 0 1 0
	// 4: 0 0 1
	// 5: 1 0 1
	// 6: 1 1 1
	// 7: 0 1 1

	// bottom
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[0], worldFromSpace * points[4], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[0], worldFromSpace * points[1], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[5], worldFromSpace * points[4], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[5], worldFromSpace * points[1], color);

	// top
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[3], worldFromSpace * points[7], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[3], worldFromSpace * points[2], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[6], worldFromSpace * points[7], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[6], worldFromSpace * points[2], color);

	// top-bottom lines
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[0], worldFromSpace * points[3], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[1], worldFromSpace * points[2], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[4], worldFromSpace * points[7], color);
	DebugDrawSystem::DrawLine(world, worldFromSpace * points[5], worldFromSpace * points[6], color);
}


void Poly::DebugDrawSystem::DrawBox(Scene * world, const AABox & box, const Color & color)
{
	DrawBox(world, box.GetMin(), box.GetMax(), color);
}

void Poly::DebugDrawSystem::DrawFrustum(Scene* world, const Frustum& frust, const Vector& pos, const Quaternion& rot, const Color& color, bool withNormals)
{
	const Quaternion rY = Quaternion(Vector::UNIT_Y, (frust.GetFov() / 2) * frust.GetAspect());
	const Quaternion rX = Quaternion(Vector::UNIT_X, (frust.GetFov() / 2));

	const Vector origin = pos;
	const Vector lookDir = rot * -Vector::UNIT_Z;

	std::array<Vector, 4> v;
	v[0] = rY * rX * lookDir;
	v[1] = rY * rX.GetConjugated() * lookDir;
	v[2] = rY.GetConjugated() * rX.GetConjugated() * lookDir;
	v[3] = rY.GetConjugated() * rX * lookDir;

	const float cosHalfFovW = Cos(frust.GetFov() / 2 * frust.GetAspect());
	const float cosHalfFovH = Cos(frust.GetFov() / 2);

	const float nDist = frust.GetZNear() / (cosHalfFovW * cosHalfFovH);
	std::array<Vector, 4> n;
	for (size_t i = 0; i < 4; ++i)
		n[i] = origin + v[i] * nDist;

	const float fDist = frust.GetZFar() / (cosHalfFovW * cosHalfFovH);
	std::array<Vector, 4> f;
	for (size_t i = 0; i < 4; ++i)
		f[i] = origin + v[i] * fDist;

	for (size_t i = 0; i < 4; ++i)
	{
		size_t nextI = (i + 1) % 4;

		DrawLine(world, origin, n[i], color);
		DrawLine(world, n[i], f[i], color);

		DrawLine(world, n[i], n[nextI], color);
		DrawLine(world, f[i], f[nextI], color);
	}

	if (withNormals)
	{
		DrawArrow(world, (n[0] + n[1] + f[0] + f[1]) / 4, rot * frust.GetPlanes()[eFrustumPlane::LEFT].GetNormal(), color);
		DrawArrow(world, (n[1] + n[2] + f[1] + f[2]) / 4, rot * frust.GetPlanes()[eFrustumPlane::UP].GetNormal(), color);
		DrawArrow(world, (n[2] + n[3] + f[2] + f[3]) / 4, rot * frust.GetPlanes()[eFrustumPlane::RIGHT].GetNormal(), color);
		DrawArrow(world, (n[3] + n[0] + f[3] + f[0]) / 4, rot * frust.GetPlanes()[eFrustumPlane::DOWN].GetNormal(), color);
		DrawArrow(world, origin + lookDir * frust.GetZNear(), rot * frust.GetPlanes()[eFrustumPlane::ZNEAR].GetNormal(), color);
		DrawArrow(world, origin + lookDir * frust.GetZFar(), rot * frust.GetPlanes()[eFrustumPlane::ZFAR].GetNormal(), color);
	}
}

void Poly::DebugDrawSystem::DrawCircle(Scene* world, const Vector& position, float radius, Vector orientation, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	const auto circleSegmentStep = Angle::FromDegrees(15.0f);
	orientation.Normalize();

	Quaternion rotAroundCircle;
	rotAroundCircle.SetRotation(orientation, circleSegmentStep);

	// get a vector perpendicular to orientation
	Vector right, forward; // forward is unused
	Util::BranchlessONB(orientation, right, forward);

	right *= radius;

	for(float degrees = 0.0f; degrees < 360.0f; degrees += circleSegmentStep.AsDegrees())
	{
		Vector previousSegmentPoint = right;
		right = rotAroundCircle*right;
		DrawLine(world, position + previousSegmentPoint, position + right, color);
	}

	DrawArrow(world, position, right, color);
}

void Poly::DebugDrawSystem::DrawSphere(Scene* world, const Vector& position, float radius, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	DrawCircle(world, position, radius, Vector(1.0f, 0.0f, 0.0f), color);
	DrawCircle(world, position, radius, Vector(0.0f, 1.0f, 0.0f), color);
	DrawCircle(world, position, radius, Vector(0.0f, 0.0f, 1.0f), color);
}

void Poly::DebugDrawSystem::DrawArrow(Scene* world, Vector position, Vector directionVector, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	constexpr float arrowLengthScale = 0.5f;
	constexpr float arrowheadScale = 0.2f;
	
	// body line
	auto arrowTip = position + directionVector*arrowLengthScale;
	DrawLine(world, position, arrowTip, color);
	directionVector.Normalize();

	// arrowhead
	const auto rotationStep = Angle::FromDegrees(60.0f);
	Quaternion rotAroundDirectionVector;
	rotAroundDirectionVector.SetRotation(directionVector, rotationStep);

	if(position.X == 0.0f && position.Y == 0.0f && position.Z == 0.0f) {
		position.X += 0.01f; position.Y += 0.01f; position.Z += 0.01f;
	}

	// calculate point which sets edge points around the arrowhead
	Vector perp[2];
	perp[0] = Vector(directionVector.Z, directionVector.Z, -directionVector.X - directionVector.Y);
	perp[1] = Vector(-directionVector.Y - directionVector.Z, directionVector.X, directionVector.X);
	int selectIndex = ((directionVector.Z != 0) && (-directionVector.X != directionVector.Y)); // this is not a branch
	Vector perpendicularVector = perp[selectIndex];

	perpendicularVector.Normalize();
	perpendicularVector *= arrowheadScale; // scale the arrowhead (cone base diameter)

	for (float degrees = 0.0f; degrees < 360.0f; degrees += rotationStep.AsDegrees())
	{
		DrawLine(world, arrowTip, arrowTip + perpendicularVector, color);
		DrawLine(world, arrowTip + perpendicularVector, arrowTip + directionVector*arrowheadScale, color);
		perpendicularVector = rotAroundDirectionVector* perpendicularVector;
	}
}

void Poly::DebugDrawSystem::DrawText2D(Scene* world, const Vector2i& screenPosition, String text, size_t fontSize, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	auto debugLinesComponent = world->GetWorldComponent<DebugDrawStateWorldComponent>();
	debugLinesComponent->DebugTexts2D.PushBack({ std::move(text), screenPosition , fontSize, color });
}
