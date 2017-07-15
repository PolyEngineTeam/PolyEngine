#pragma once

#include <Defines.hpp>
#include <String.hpp>
#include <map>
#include "GLUtils.hpp"

namespace Poly
{
	class World;
	class CameraComponent;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class AABox;

	//------------------------------------------------------------------------------
	class RenderingPassBase : public BaseObject<>
	{
		struct IOBind
		{
			RenderingTargetBase* Target = nullptr;
			String IOName;
		};

	public:
		virtual void Run(World* world, const CameraComponent* camera, const AABox& rect) = 0;

		void BindOutput(const String& outputName, RenderingTargetBase* target);
		void BindInput(const String& inputName, RenderingTargetBase* target);

	protected:
		RenderingTargetBase* GetInputTarget(const String& name);
		RenderingTargetBase* GetOutputTarget(const String& name);

	private:
		std::map<String, RenderingTargetBase*> Inputs;
		std::map<String, RenderingTargetBase*> Outputs;
	};

	//------------------------------------------------------------------------------
	enum class eRenderingTargetType
	{
		TEXTURE,
		DEPTH,
		FRAMEBUFFER,
		_COUNT
	};

	//------------------------------------------------------------------------------
	class RenderingTargetBase : public BaseObject<>
	{
		
	public:	
		virtual eRenderingTargetType GetType() const = 0;

		virtual void Resize(size_t width, size_t height) {}
	};

	//------------------------------------------------------------------------------
	class TextureRenderingTarget : public RenderingTargetBase
	{
	public:
		TextureRenderingTarget(GLuint format);

		eRenderingTargetType GetType() const override { return eRenderingTargetType::TEXTURE; }
		void Resize(size_t width, size_t height) override;

		GLuint GetTextureID();
	private:
		GLuint Format;
		std::unique_ptr<GLTextureDeviceProxy> Texture;
	};

	//------------------------------------------------------------------------------
	class DepthRenderingTarget : public TextureRenderingTarget
	{
	public:
		DepthRenderingTarget();

		eRenderingTargetType GetType() const override { return eRenderingTargetType::DEPTH; }
	};

	//------------------------------------------------------------------------------
	class ScreenBufferRenderingTarget : public RenderingTargetBase
	{
	public:
		eRenderingTargetType GetType() const override { return eRenderingTargetType::FRAMEBUFFER; }
	
		void Bind();
	};
}