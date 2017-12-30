#include "EnginePCH.hpp"

#include "DebugRenderingComponent.hpp"
#include "DebugRenderingSystem.hpp"

using namespace Poly;

namespace Util
{
	/// <summary>Get vector with minimum/maximum/else (depends on comparison function) values
	/// on each axis in given set of vectors.</summary>
	Mesh::Vector3D findExtremum(const Dynarray<Mesh::Vector3D> & dynarray, std::function< bool(const float, const float) > comp)
	{
		if(dynarray.GetSize() <= 0)
			Mesh::Vector3D(); // or assert?

		Mesh::Vector3D result = dynarray[0];
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
}

void DebugRenderingSystem::DebugRenderingUpdatePhase(World* world)
{
	gDebugConfig.DebugRender = false;
	for (auto& kv : world->GetWorldComponent<ViewportWorldComponent>()->GetViewports())
	{
		CameraComponent* cameraCmp = kv.second.GetCamera();
		if (cameraCmp->GetRenderingMode() == eRenderingModeType::DEBUG)
			gDebugConfig.DebugRender = true;
	}

	if(!gDebugConfig.DebugRender)
		return;

	enum class RenderMode : int { /*POINT = 1,*/ LINE = 1, /*STRING,*/ _COUNT };

	// iterate RenderMode::_COUNT times to create shapes defining debug primitives
	for(int renderMode = static_cast<int>(RenderMode::LINE); renderMode < static_cast<int>(RenderMode::_COUNT); ++renderMode)
	{
		for(auto componentsTuple : world->IterateComponents<DebugDrawableComponent, MeshRenderingComponent, TransformComponent>())
		{
			const auto ddrawCmp = std::get<DebugDrawableComponent*>(componentsTuple);
			const auto meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
			const auto transformCmp = std::get<TransformComponent*>(componentsTuple);

			if (meshCmp == nullptr)
				continue;

			if (!gDebugConfig.DebugDrawPresets.IsSet(ddrawCmp->entityPreset))
				continue;

			Vector objPos = transformCmp->GetGlobalTranslation();

			const Matrix& objTransform = transformCmp->GetGlobalTransformationMatrix();

			// spawn a box for every mesh, in correct size
			for(const auto subMesh : meshCmp->GetMesh()->GetSubMeshes())
			{
				const auto & meshVerticesPositions = subMesh->GetMeshData().GetPositions();

				// find mins and maxs of each mesh coordinates
				// in order to create AABB
				auto minMeshVector = Util::findExtremum(meshVerticesPositions, std::less<float>());
				auto maxMeshVector = Util::findExtremum(meshVerticesPositions, std::greater<float>());

				Vector minVector(minMeshVector.X, minMeshVector.Y, minMeshVector.Z);
				Vector maxVector(maxMeshVector.X, maxMeshVector.Y, maxMeshVector.Z);

				minVector = objTransform * minVector;
				maxVector = objTransform * maxVector;

				const auto boundingOffset = Vector(0.04f, 0.04f, 0.04f);
				// move vector away a little bit from a mesh
				minVector -= boundingOffset;
				maxVector += boundingOffset;

				EmitBox(world, minVector, maxVector);
			}
		}

		for (auto componentsTuple : world->IterateComponents<RigidBody2DComponent, DebugDrawableComponent, TransformComponent>())
		{
			const auto rigidbodyCmp = std::get<RigidBody2DComponent*>(componentsTuple);
			const auto ddrawCmp = std::get<DebugDrawableComponent*>(componentsTuple);
			const auto transformCmp = std::get<TransformComponent*>(componentsTuple);

			if (ddrawCmp == nullptr)
				continue;

			if (!gDebugConfig.DebugDrawPresets.IsSet(ddrawCmp->entityPreset))
				continue;

			auto localTrans = transformCmp->GetLocalTranslation();
			auto velocity = rigidbodyCmp->GetLinearVelocity();

			if (velocity.LengthSquared() == 0.0f)
				continue;

			EmitArrow(world, localTrans, velocity);
		}
	}
}

void Poly::DebugRenderingSystem::EmitPoint(World* world, Mesh::Vector3D position, float size)
{
}

void Poly::DebugRenderingSystem::EmitLine(World* world, Vector begin, Vector end)
{
	Mesh::Vector3D meshvecBegin, meshvecEnd;
	meshvecBegin.X = begin.X; meshvecBegin.Y = begin.Y; meshvecBegin.Z = begin.Z;
	meshvecEnd.X = end.X; meshvecEnd.Y = end.Y; meshvecEnd.Z = end.Z;
	auto debugLinesComponent = world->GetWorldComponent<DebugRenderingLinesComponent>();
	debugLinesComponent->DebugLines.PushBack(DebugRenderingLinesComponent::DebugLine{ meshvecBegin, meshvecEnd });
	Mesh::Vector3D colorBegin, colorEnd;
	colorBegin.X = 0.0f; colorBegin.Y = 0.3f; colorBegin.Z = 0.0f;
	colorEnd.X = 0.0f; colorEnd.Y = 0.2f; colorEnd.Z = 0.0f;
	debugLinesComponent->DebugLinesColors.PushBack(DebugRenderingLinesComponent::DebugLine{ colorBegin, colorEnd });
}

void Poly::DebugRenderingSystem::EmitQuad(Mesh::Vector3D mins, Mesh::Vector3D maxs)
{
}

void Poly::DebugRenderingSystem::EmitBox(World* world, Vector mins, Vector maxs)
{
	std::array<Vector, 8> points;
	std::array<Vector, 2> minmaxVector = { mins, maxs };

	for (int i = 0; i < points.size(); ++i)
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
	EmitLine(world, points[0], points[4]);
	EmitLine(world, points[0], points[1]);
	EmitLine(world, points[5], points[4]);
	EmitLine(world, points[5], points[1]);

	// top
	EmitLine(world, points[3], points[7]);
	EmitLine(world, points[3], points[2]);
	EmitLine(world, points[6], points[7]);
	EmitLine(world, points[6], points[2]);

	// top-bottom lines
	EmitLine(world, points[0], points[3]);
	EmitLine(world, points[1], points[2]);
	EmitLine(world, points[4], points[7]);
	EmitLine(world, points[5], points[6]);
}

void Poly::DebugRenderingSystem::EmitSphere(World* world, Mesh::Vector3D position, float radius)
{
}

void Poly::DebugRenderingSystem::EmitArrow(World* world, Vector position, Vector directionVector)
{
	constexpr float arrowLengthScale = 0.5f;
	constexpr float arrowheadScale = 0.5f;
	
	// body line
	auto arrowTip = position + directionVector*arrowLengthScale;
	EmitLine(world, position, arrowTip);
	directionVector.Normalize();

	// arrowhead
	const auto rotationStep = Angle::FromDegrees(24.0f);
	Quaternion rotAroundDirectionVector;
	rotAroundDirectionVector.SetRotation(directionVector, rotationStep);

	// calculate point which sets edge points around the arrowhead
	auto arrowTipEdgePoint = position.Cross(directionVector);
	arrowTipEdgePoint.Normalize();
	arrowTipEdgePoint *= arrowheadScale; // scale the arrowhead (cone base diameter)

	for (float degrees = 0.0f; degrees < 360.0f; degrees += rotationStep.AsDegrees())
	{
		EmitLine(world, arrowTip, arrowTip + arrowTipEdgePoint);
		EmitLine(world, arrowTip + arrowTipEdgePoint, arrowTip + directionVector*arrowheadScale);
		arrowTipEdgePoint = rotAroundDirectionVector*arrowTipEdgePoint;
	}
}
