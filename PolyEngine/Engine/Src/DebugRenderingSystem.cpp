#include "EnginePCH.hpp"

#include "DebugRenderingComponent.hpp"
#include "DebugRenderingSystem.hpp"

using namespace Poly;

namespace Util
{
	Mesh::Vector3D findMinMax(const Dynarray<Mesh::Vector3D> & dynarray, std::function< bool(const float, const float) > comp)
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
	if(!gDebugConfig.DebugRender)
		return;

	for(int renderMode = 0; renderMode < 1; ++renderMode)// render modes: LINE (todo POINT, STRING)
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
				auto minVector = Util::findMinMax(meshVerticesPositions, std::less<float>());
				auto maxVector = Util::findMinMax(meshVerticesPositions, std::greater<float>());

				Vector _minVector(minVector.X, minVector.Y, minVector.Z);
				Vector _maxVector(maxVector.X, maxVector.Y, maxVector.Z);

				_minVector = objTransform * _minVector;
				_maxVector = objTransform * _maxVector;

				minVector.X = _minVector.X;
				minVector.Y = _minVector.Y;
				minVector.Z = _minVector.Z;
				maxVector.X = _maxVector.X;
				maxVector.Y = _maxVector.Y;
				maxVector.Z = _maxVector.Z;

				EmitBox(world, minVector, maxVector);
			}
		}
	}
}

void Poly::DebugRenderingSystem::EmitPoint(Vector position, float size)
{
}

void Poly::DebugRenderingSystem::EmitLine(World* world, Mesh::Vector3D begin, Mesh::Vector3D end)
{
	auto debugLinesComponent = world->GetWorldComponent<DebugRenderingLinesComponent>();
	debugLinesComponent->DebugLines.PushBack(DebugRenderingLinesComponent::DebugLine{ begin, end });
	Mesh::Vector3D color;
	color = begin;
	debugLinesComponent->DebugLinesColors.PushBack(color);
}

void Poly::DebugRenderingSystem::EmitQuad(Vector mins, Vector maxs)
{
}

void Poly::DebugRenderingSystem::EmitBox(World* world, Mesh::Vector3D mins, Mesh::Vector3D maxs)
{
	std::array<Mesh::Vector3D, 8> points;
	std::array<Mesh::Vector3D, 2> minmaxVector = { mins, maxs };

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

void Poly::DebugRenderingSystem::EmitSphere(Vector position, float radius)
{
}

void Poly::DebugRenderingSystem::EmitArrow(Vector position, Vector direction)
{
}
