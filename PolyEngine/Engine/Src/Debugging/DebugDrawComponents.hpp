#pragma once

#include <pe/Defines.hpp>
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

		struct DebugLine final : public ::pe::core::BaseObjectLiteralType<>
		{
			DebugLine(const ::pe::core::math::Vector3f& begin, const ::pe::core::math::Vector3f& end) : Begin(begin), End(end) {}
			::pe::core::math::Vector3f Begin;
			::pe::core::math::Vector3f End;
		};

		struct DebugLineColor final : public ::pe::core::BaseObjectLiteralType<>
		{
			DebugLineColor(const ::pe::core::math::Color& begin, const ::pe::core::math::Color& end) : Begin(begin), End(end) {}
			::pe::core::math::Color Begin;
			::pe::core::math::Color End;
		};

		struct DebugText2D final : public ::pe::core::BaseObjectLiteralType<>
		{
			DebugText2D(::pe::core::storage::String text, const ::pe::core::math::Vector2i& pos, size_t fontSize, ::pe::core::math::Color fontColor) 
				: Text(std::move(text)), Position(pos), FontSize(fontSize), FontColor(fontColor) {}
			::pe::core::storage::String Text;
			::pe::core::math::Vector2i Position;
			size_t FontSize;
			::pe::core::math::Color FontColor;
		};

		std::vector<DebugLine> DebugLines;
		std::vector<DebugLineColor> DebugLinesColors;
		std::vector<DebugText2D> DebugTexts2D;

		std::vector<::pe::core::memory::SafePtr<Entity>> Text2DEntityPool;
	};

	REGISTER_COMPONENT(ComponentsIDGroup, DebugDrawStateWorldComponent)
}