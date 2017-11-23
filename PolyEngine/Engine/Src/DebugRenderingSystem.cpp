#include "EnginePCH.hpp"

#include "DebugRenderingComponent.hpp"
#include "DebugRenderingSystem.hpp"

using namespace Poly;

namespace Util
{
	template<float MinMaxFloat, typename op>
	struct CompareMinMax
	{
		CompareMinMax() : Minmax(MinMaxFloat, MinMaxFloat, MinMaxFloat)
		{}
		void operator()(const Mesh::Vector3D & vec)
		{
			if(op(Minmax.X, vec.X))
				Minmax.X = vec.X;
			if(op(Minmax.Y, vec.Y))
				Minmax.Y = vec.Y;
			if(op(Minmax.Z, vec.Z))
				Minmax.Z = vec.Z;
		}

		Vector Minmax;
	};
}

void DebugRenderingSystem::DebugRenderingUpdatePhase(World* world)
{
	if(!gDebugConfig.debug_rendering)
		return;

	for(int renderMode = 0; renderMode < 3; ++renderMode)
	{
		for(auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
		{
			const auto meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
			auto transformCmp = std::get<TransformComponent*>(componentsTuple);

			Vector objPos = transformCmp->GetGlobalTranslation();

			const Matrix& objTransform = transformCmp->GetGlobalTransformationMatrix();
			//Matrix screenTransform = mvp * objTransform;

			// spawn a box for every mesh, in correct size
			for(const auto subMesh : meshCmp->GetMesh()->GetSubMeshes())
			{
				const auto & meshVerticesPositions = subMesh->GetMeshData().GetPositions();

				// find mins and maxs of each mesh coordinates
				// in order to create AABB
				auto minResult = std::for_each(meshVerticesPositions.Begin(), meshVerticesPositions.End(), Util::CompareMinMax<std::numeric_limits<float>::max(), std::less<float>>());
				auto maxResult = std::for_each(meshVerticesPositions.Begin(), meshVerticesPositions.End(), Util::CompareMinMax<std::numeric_limits<float>::min(), std::greater<float>>());

				EmitBox(minResult.Minmax, maxResult.Minmax);
			}
		}
	}
}

void Poly::DebugRenderingSystem::EmitPoint(Vector position, float size)
{
}

void Poly::DebugRenderingSystem::EmitLine(World* world, Vector begin, Vector end)
{
	auto debugLinesComponent = world->GetWorldComponent<DebugRenderingLinesComponent>();
	debugLinesComponent->DebugLines.PushBack(DebugRenderingLinesComponent::DebugLine{ begin, end, Vector() });
}

void Poly::DebugRenderingSystem::EmitQuad(Vector mins, Vector maxs)
{
}

void Poly::DebugRenderingSystem::EmitBox(Vector mins, Vector maxs)
{
}

void Poly::DebugRenderingSystem::EmitSphere(Vector position, float radius)
{
}

void Poly::DebugRenderingSystem::EmitArrow(Vector position, Vector direction)
{
}
