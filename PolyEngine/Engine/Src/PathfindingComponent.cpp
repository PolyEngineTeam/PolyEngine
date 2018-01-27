#include "EnginePCH.hpp"

#include "PathfindingComponent.hpp"

using namespace Poly;

void Poly::PathfindingComponent::SetDestination(const Vector& pos)
{
	CurentDestination = pos;
	RecalculateRequested = true;
}

void Poly::PathfindingComponent::ResetDestination()
{
	CurentDestination = Optional<Vector>();
	CalculatedPath.Clear();
	RecalculateRequested = false;
}

Poly::NavGrid::NavGrid(const Vector2i& gridSize, float cellSize)
{
}

Poly::NavGrid::NavGrid(const Vector2i& gridSize, const Dynarray<bool>& occpanceMap, float cellSize)
{
}

Optional<Vector2i> Poly::NavGrid::GetCellAtPosition(const Vector2f& pos) const
{
	return GetIdxFromPos(pos);
}

Dynarray<Vector2i> Poly::NavGrid::GetCellsInLine(const Vector2f& start, const Vector2f& end, float radius) const
{
	ASSERTE(false, "Implement this!");
	/*Vector2f path = end - start;
	Vector2f step;
	step.X = Clamp(path.X, -CellSize, CellSize);
	step.Y = Clamp(path.Y, -CellSize, CellSize);

	const float pathLength = path.Length();
	float factor = pathLength / step.Length();
	float distanceTraveled = 0.f;
	while(distanceTraveled < )*/
	return Dynarray<Vector2i>();
}

Dynarray<Vector2i> Poly::NavGrid::GetCellAtPosition(const Vector2f& pos, float radius) const
{
	Dynarray<Vector2i> cells;
	GetCellAtPositionInternal(pos, radius, cells);
	return cells;
}

void Poly::NavGrid::GetCellAtPositionInternal(const Vector2f & pos, float radius, Dynarray<Vector2i>& cells) const
{
	const Vector2f minVal = TrimPointToGrid(pos - Vector2f(radius, radius));
	const Vector2f maxVal = TrimPointToGrid(pos + Vector2f(radius, radius));
	const Vector2i minIdx = GetIdxFromPos(minVal).Value();
	const Vector2i maxIdx = GetIdxFromPos(maxVal).Value();

	const float radius2 = radius * radius;

	for (int y = minIdx.Y; y <= maxIdx.Y; ++y)
		for (int x = minIdx.X; x <= maxIdx.X; ++x)
		{
			Vector2i cell(x, y);
			Vector2f closestPoint = ClosestPointOnCell(cell, pos);
			if ((closestPoint - pos).LengthSquared() < radius2)
				cells.PushBack(cell);
		}
}

void Poly::NavGrid::GetCellsInLineInternal(const Vector2f& start, const Vector2f& end, float radius, Dynarray<Vector2i>& cells) const
{
	ASSERTE(false, "Implement this!");
	Vector2f radiusVec = Vector2f(radius, radius);
	const Vector2f minVal = TrimPointToGrid(Vector2f::Min(start - radiusVec, end - radiusVec));
	const Vector2f maxVal = TrimPointToGrid(Vector2f::Max(start + radiusVec, end + radiusVec));
	const Vector2i minIdx = GetIdxFromPos(minVal).Value();
	const Vector2i maxIdx = GetIdxFromPos(maxVal).Value();

	const float radius2 = radius * radius;

	for (int y = minIdx.Y; y <= maxIdx.Y; ++y)
		for (int x = minIdx.X; x <= maxIdx.X; ++x)
		{
			Vector2i cell(x, y);
			Vector2f closestPoint = ClosestPointOnCell(cell, start, end);
			//if ((closestPoint - pos).LengthSquared() < radius2)
			//	cells.PushBack(cell);
		}
}

bool Poly::NavGrid::IsPointInsideGrid(const Vector2f& point) const
{
	if (point.X < GridOrigin.X || point.Y < GridOrigin.Y)
		return false;

	Vector2f gridEnd = GridOrigin + GridSize.ToVector2f() * CellSize;

	if (point.X >= gridEnd.X || point.Y >= gridEnd.Y)
		return false;

	return true;
}

Vector2f Poly::NavGrid::GetCellOrigin(const Vector2i& cell) const
{
	return Vector2f(GridOrigin + cell.ToVector2f() * CellSize);
}

Vector2f Poly::NavGrid::ClosestPointOnCell(const Vector2i& cell, const Vector2f& pos) const
{
	Vector2f cellOrigin = GetCellOrigin(GridSize - Vector2i(1, 1)); // get origin of last cell
	return Vector2f::Clamp(pos, cellOrigin, cellOrigin + Vector2f(CellSize - CMPF_EPS, CellSize - CMPF_EPS));
}

Vector2f Poly::NavGrid::ClosestPointOnCell(const Vector2i & cell, const Vector2f & start, const Vector2f & end) const
{
	return Vector2f();
}

Vector2f Poly::NavGrid::TrimPointToGrid(const Vector2f& pos) const
{
	Vector2f gridEnd = GetCellOrigin(GridSize - Vector2i(1, 1)); // get origin of last cell
	return Vector2f::Clamp(pos, GridOrigin, gridEnd - Vector2f(CMPF_EPS, CMPF_EPS));
}

Optional<Vector2i> Poly::NavGrid::GetIdxFromPos(const Vector2f& pos) const
{
	if (!IsPointInsideGrid(pos))
		return Optional<Vector2i>();

	Vector2i idx;
	idx.X = pos.X / GridSize.X;
	idx.Y = pos.Y / GridSize.Y;
	return idx;
}


