#pragma once

#include <Core.hpp>
#include "ComponentBase.hpp"
#include "Optional.hpp"
#include <Vector2f.hpp>

#include "PathfindingSystem.hpp"

namespace Poly
{
	class ENGINE_DLLEXPORT NavGrid : public BaseObject<>
	{
	public:
		struct Cell
		{
			bool Occupied = false;
		};

		NavGrid(const Vector2i& gridSize, float cellSize = 1.0f);
		NavGrid(const Vector2i& gridSize, const Dynarray<bool>& occpanceMap, float cellSize = 1.0f);

		bool IsPositionValid(const Vector2f& pos) const;
		bool IsPositionValid(const Vector2f& pos, float radius) const;
		bool IsLineValid(const Vector2f& start, const Vector2f& end, float radius) const;

		Optional<Vector2i> GetCellAtPosition(const Vector2f& pos) const;
		Dynarray<Vector2i> GetCellAtPosition(const Vector2f& pos, float radius) const;
		Dynarray<Vector2i> GetCellsInLine(const Vector2f& start, const Vector2f& end, float radius) const;

		Vector2f GetCellMiddlePos(const Vector2i& cell) const;

		Dynarray<Vector2i> GetNeighbours(const Vector2i& cell) const;

		const Cell& GetCell(size_t x, size_t y) const { return Cells[GetArrayIdx(x, y)]; }
		Cell& GetCell(size_t x, size_t y) { return Cells[GetArrayIdx(x, y)]; }

		bool IsPointInsideGrid(const Vector2f& point) const;
	private:
		Vector2f GetCellOrigin(const Vector2i& cell) const;
		Vector2f ClosestPointOnCell(const Vector2i& cell, const Vector2f& pos) const;
		Vector2f ClosestPointOnCell(const Vector2i& cell, const Vector2f& start, const Vector2f& end) const;
		Vector2f TrimPointToGrid(const Vector2f& pos) const;
		Optional<Vector2i> GetIdxFromPos(const Vector2f& pos) const;
		inline size_t GetArrayIdx(const Vector2i& v) const { return GetArrayIdx(v.X, v.Y); }
		inline size_t GetArrayIdx(size_t x, size_t y) const { return y * GridSize.X + x; }

		void GetCellAtPositionInternal(const Vector2f& pos, float radius, Dynarray<Vector2i>& cells) const;
		void GetCellsInLineInternal(const Vector2f& start, const Vector2f& end, float radius, Dynarray<Vector2i>& cells) const;

		Vector2f GridOrigin;
		float CellSize = 1.0f;
		Vector2i GridSize;
		Dynarray<Cell> Cells;
	};

	class ENGINE_DLLEXPORT PathfindingComponent : public ComponentBase
	{
	public:
		PathfindingComponent(const NavGrid* grid) : NavigationGrid(grid) {}

		void SetDestination(const Vector2f& pos);
		void ResetDestination();

		inline const Dynarray<Vector2f>& GetPath() const { return CalculatedPath; }
	private:
		const NavGrid* NavigationGrid = nullptr;
		float AgentRadius = 0.5f;
		Dynarray<Vector2f> CalculatedPath;
		Optional<Vector2f> CurentDestination;
		bool RecalculateRequested = false;
		bool LastPathSearchFailed = false;

		friend void PathfindingSystem::UpdatePhase(World* world);
	};

	REGISTER_COMPONENT(ComponentsIDGroup, PathfindingComponent)
}
