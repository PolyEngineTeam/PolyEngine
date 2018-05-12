#include "EnginePCH.hpp"
#include <cmath>

#include "Debugging/DebugDrawComponents.hpp"
#include "Debugging/DebugDrawSystem.hpp"

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
				const auto & meshVerticesPositions = subMesh->GetMeshData().GetPositions();

				// find mins and maxs of each mesh coordinates
				// in order to create AABB
				auto minMeshVector = Util::FindExtremum(meshVerticesPositions, std::less<float>());
				auto maxMeshVector = Util::FindExtremum(meshVerticesPositions, std::greater<float>());

				Vector minVector(minMeshVector.X, minMeshVector.Y, minMeshVector.Z);
				Vector maxVector(maxMeshVector.X, maxMeshVector.Y, maxMeshVector.Z);

				minVector = objTransform * minVector;
				maxVector = objTransform * maxVector;

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
	constexpr float arrowheadScale = 0.5f;
	
	// body line
	auto arrowTip = position + directionVector*arrowLengthScale;
	DrawLine(world, position, arrowTip, color);
	directionVector.Normalize();

	// arrowhead
	const auto rotationStep = Angle::FromDegrees(24.0f);
	Quaternion rotAroundDirectionVector;
	rotAroundDirectionVector.SetRotation(directionVector, rotationStep);

	if(position.X == 0.0f && position.Y == 0.0f && position.Z == 0.0f) {
		position.X += 0.01f; position.Y += 0.01f; position.Z += 0.01f;
	}

	// calculate point which sets edge points around the arrowhead
	auto arrowTipEdgePoint = position.Cross(directionVector);
	arrowTipEdgePoint.Normalize();
	arrowTipEdgePoint *= arrowheadScale; // scale the arrowhead (cone base diameter)

	for (float degrees = 0.0f; degrees < 360.0f; degrees += rotationStep.AsDegrees())
	{
		DrawLine(world, arrowTip, arrowTip + arrowTipEdgePoint, color);
		DrawLine(world, arrowTip + arrowTipEdgePoint, arrowTip + directionVector*arrowheadScale, color);
		arrowTipEdgePoint = rotAroundDirectionVector*arrowTipEdgePoint;
	}
}

void Poly::DebugDrawSystem::DrawText2D(Scene* world, const Vector2i& screenPosition, String text, size_t fontSize, const Color& color)
{
	if (!gDebugConfig.DebugRender)
		return;

	auto debugLinesComponent = world->GetWorldComponent<DebugDrawStateWorldComponent>();
	debugLinesComponent->DebugTexts2D.PushBack({ std::move(text), screenPosition , fontSize, color });
}
