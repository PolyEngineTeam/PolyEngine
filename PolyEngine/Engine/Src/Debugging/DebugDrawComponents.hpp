#pragma once

#include <Defines.hpp>
#include <ECS/ComponentBase.hpp>
#include <Configs/DebugConfig.hpp>

namespace Poly
{
	/// Component used for labeling entities for which the debug primitives 
	/// will be drawn.
	class ENGINE_DLLEXPORT DebugDrawableComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::DebugDrawableComponent) { NO_RTTI_PROPERTY(); }
		DebugDrawableComponent(eDebugDrawPreset preset) : entityPreset(preset) { };

		const eDebugDrawPreset entityPreset;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DebugDrawableComponent)

	class ENGINE_DLLEXPORT DebugDrawStateWorldComponent : public ComponentBase
	{
	public:
		RTTI_DECLARE_COMPONENT(::Poly::DebugDrawStateWorldComponent) { NO_RTTI_PROPERTY(); }
		DebugDrawStateWorldComponent()
		{ }

		void Clear()
		{
			DebugLines.clear();
			DebugLinesColors.clear();
			DebugTexts2D.clear();
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

		std::vector<DebugLine> DebugLines;
		std::vector<DebugLineColor> DebugLinesColors;
		std::vector<DebugText2D> DebugTexts2D;

		std::vector<SafePtr<Entity>> Text2DEntityPool;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DebugDrawStateWorldComponent)
}