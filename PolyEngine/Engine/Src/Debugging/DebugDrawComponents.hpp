#pragma once
#include "ECS/ComponentBase.hpp"
#include "Configs/DebugConfig.hpp"
#include <Collections/Dynarray.hpp>
#include <Math/Color.hpp>
#include <Math/Vector3f.hpp>

namespace Poly
{
	/// Component used for labeling entities for which the debug primitives 
	/// will be drawn.
	class ENGINE_DLLEXPORT DebugDrawableComponent : public ComponentBase
	{
	public:
		DebugDrawableComponent(eDebugDrawPreset preset) : entityPreset(preset) { };

		const eDebugDrawPreset entityPreset;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DebugDrawableComponent)

	class ENGINE_DLLEXPORT DebugDrawStateWorldComponent : public ComponentBase
	{
	public:
		DebugDrawStateWorldComponent()
		{ }

		void Clear()
		{
			DebugLines.Clear();
			DebugLinesColors.Clear();
			DebugTexts2D.Clear();
		}

		struct DebugLine final : public BaseObjectLiteralType<>
		{
			DebugLine(const Vector3f& begin, const Vector3f& end) : Begin(begin), End(end) {}
			Vector3f Begin;
			Vector3f End;
		};

		struct DebugLineColor final : public BaseObjectLiteralType<>
		{
			DebugLineColor(const Color& begin, const Color& end) : Begin(begin), End(end) {}
			Color Begin;
			Color End;
		};

		struct DebugText2D final : public BaseObjectLiteralType<>
		{
			DebugText2D(String text, const Vector2i& pos, size_t fontSize, Color fontColor) 
				: Text(std::move(text)), Position(pos), FontSize(fontSize), FontColor(fontColor) {}
			String Text;
			Vector2i Position;
			size_t FontSize;
			Color FontColor;
		};

		Dynarray<DebugLine> DebugLines;
		Dynarray<DebugLineColor> DebugLinesColors;
		Dynarray<DebugText2D> DebugTexts2D;

		Dynarray<SafePtr<Entity>> Text2DEntityPool;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DebugDrawStateWorldComponent)
}