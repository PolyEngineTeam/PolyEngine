#include "EnginePCH.hpp"
#include <cmath>

#include "DebugDrawComponents.hpp"
#include "DebugDrawSystem.hpp"

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

void DebugDrawSystem::DebugRenderingUpdatePhase(World* world)
{
	gDebugConfig.DebugRender = false;
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		CameraComponent* cameraCmp = kv.second.GetCamera();
		if (cameraCmp->GetRenderingMode() == eRenderingModeType::IMMEDIATE_DEBUG)
			gDebugConfig.DebugRender = true;
	}

	if (!gDebugConfig.DebugRender)
	{
		auto debugLinesComponent = world->GetWorldComponent<DebugDrawLinesComponent>();
		debugLinesComponent->DebugLines.Clear();
		return;
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

			const Matrix& objTransform = transform.GetGlobalTransformationMatrix();

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

				DrawBox(world, minVector, maxVector);
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

			DrawArrow(world, localTrans, velocity);
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

			DrawArrow(world, localTrans, directionRotation*lightMagnitude);
		}
	}
}

void Poly::DebugDrawSystem::DrawLine(World* world, Vector begin, Vector end)
{
	if (!gDebugConfig.DebugRender)
		return;

	Vector3f meshvecBegin(begin.X, begin.Y, begin.Z), meshvecEnd(end.X, end.Y, end.Z);
	auto debugLinesComponent = world->GetWorldComponent<DebugDrawLinesComponent>();
	debugLinesComponent->DebugLines.PushBack(DebugDrawLinesComponent::DebugLine{ meshvecBegin, meshvecEnd });
	Color colorBegin(0.0f, 0.3f, 0.0f), colorEnd(0.0f, 0.2f, 0.0f);
	debugLinesComponent->DebugLinesColors.PushBack(DebugDrawLinesComponent::DebugLineColor{ colorBegin, colorEnd });
}

void Poly::DebugDrawSystem::DrawBox(World* world, Vector mins, Vector maxs)
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
	DrawLine(world, points[0], points[4]);
	DrawLine(world, points[0], points[1]);
	DrawLine(world, points[5], points[4]);
	DrawLine(world, points[5], points[1]);

	// top
	DrawLine(world, points[3], points[7]);
	DrawLine(world, points[3], points[2]);
	DrawLine(world, points[6], points[7]);
	DrawLine(world, points[6], points[2]);

	// top-bottom lines
	DrawLine(world, points[0], points[3]);
	DrawLine(world, points[1], points[2]);
	DrawLine(world, points[4], points[7]);
	DrawLine(world, points[5], points[6]);
}

void Poly::DebugDrawSystem::DrawCircle(World* world, Vector position, float radius, Vector orientation)
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
		DrawLine(world, position + previousSegmentPoint, position + right);
	}

	DrawArrow(world, position, right);
}

void Poly::DebugDrawSystem::DrawSphere(World* world, Vector position, float radius)
{
	if (!gDebugConfig.DebugRender)
		return;

	DrawCircle(world, position, radius, Vector(1.0f, 0.0f, 0.0f));
	DrawCircle(world, position, radius, Vector(0.0f, 1.0f, 0.0f));
	DrawCircle(world, position, radius, Vector(0.0f, 0.0f, 1.0f));
}

void Poly::DebugDrawSystem::DrawArrow(World* world, Vector position, Vector directionVector)
{
	if (!gDebugConfig.DebugRender)
		return;

	constexpr float arrowLengthScale = 0.5f;
	constexpr float arrowheadScale = 0.5f;
	
	// body line
	auto arrowTip = position + directionVector*arrowLengthScale;
	DrawLine(world, position, arrowTip);
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
		DrawLine(world, arrowTip, arrowTip + arrowTipEdgePoint);
		DrawLine(world, arrowTip + arrowTipEdgePoint, arrowTip + directionVector*arrowheadScale);
		arrowTipEdgePoint = rotAroundDirectionVector*arrowTipEdgePoint;
	}
}
