#pragma once

#include <Core.hpp>
#include "ShaderProgram.hpp"

namespace Poly {

	struct ENGINE_DLLEXPORT IRenderingContextParams : public BaseObject<> {};

	struct ScreenSize
	{
		int Width = 0, Height = 0;
	};

	enum class eShaderProgramType {
		TEST,
		DEBUG_NORMALS,
		_COUNT
	};

	class IRenderingContext : public BaseObject<> {
	public:
		virtual bool Init(const IRenderingContextParams* context) = 0;
		virtual void Deinit() = 0;

		virtual void EndFrame() = 0;

		virtual void Resize(const ScreenSize& size) = 0;
		virtual const ScreenSize& GetScreenSize() const = 0;

		virtual ShaderProgram& GetProgram(eShaderProgramType type) { return *ShaderPrograms[type]; }
	protected:
		EnumArray<ShaderProgram*, eShaderProgramType> ShaderPrograms;
	};

	IRenderingContext* CreateRenderingContext();
}