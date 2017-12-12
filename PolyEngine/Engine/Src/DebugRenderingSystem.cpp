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
	const auto & inputComponent = world->GetWorldComponent<InputWorldComponent>();
	if(inputComponent->IsPressed(eKey::LCTRL) && inputComponent->IsPressed(eKey::KEY_D))
		gDebugConfig.DebugRender = !gDebugConfig.DebugRender;

	if(!gDebugConfig.DebugRender)
		return;

	enum class RenderMode : int { /*POINT = 1,*/ LINE = 1, /*STRING,*/ _COUNT };

	// iterate RenderMode::_COUNT times to create shapes defining debug primitives
	for(int renderMode = static_cast<int>(RenderMode::LINE); renderMode < static_cast<int>(RenderMode::_COUNT); ++renderMode)
	{
		for(auto componentsTuple : world->IterateComponents<MeshRenderingComponent, TransformComponent>())
		{
			const auto meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
			const auto transformCmp = std::get<TransformComponent*>(componentsTuple);

			Vector objPos = transformCmp->GetGlobalTranslation();

			const Matrix& objTransform = transformCmp->GetGlobalTransformationMatrix();

			// spawn a box for every mesh, in correct size
			for(const auto subMesh : meshCmp->GetMesh()->GetSubMeshes())
			{
				const auto & meshVerticesPositions = subMesh->GetMeshData().GetPositions();

				// find mins and maxs of each mesh coordinates
				// in order to create AABB
				auto minVector = Util::findExtremum(meshVerticesPositions, std::less<float>());
				auto maxVector = Util::findExtremum(meshVerticesPositions, std::greater<float>());

				Vector _minVector(minVector.X, minVector.Y, minVector.Z);
				Vector _maxVector(maxVector.X, maxVector.Y, maxVector.Z);

				_minVector = objTransform * _minVector;
				_maxVector = objTransform * _maxVector;

				const float boundingOffset = 0.04f;
				// convert from Vector to Mesh::Vector3D and move avay a little bit from a mesh
				minVector.X = _minVector.X - boundingOffset;
				minVector.Y = _minVector.Y - boundingOffset;
				minVector.Z = _minVector.Z - boundingOffset;
				maxVector.X = _maxVector.X + boundingOffset;
				maxVector.Y = _maxVector.Y + boundingOffset;
				maxVector.Z = _maxVector.Z + boundingOffset;

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
	Mesh::Vector3D colorBegin, colorEnd;
	colorBegin.X = 0.0f; colorBegin.Y = 0.3f; colorBegin.Z = 0.0f;
	colorEnd.X = 0.0f; colorEnd.Y = 0.2f; colorEnd.Z = 0.0f;
	debugLinesComponent->DebugLinesColors.PushBack(DebugRenderingLinesComponent::DebugLine{ colorBegin, colorEnd });
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
