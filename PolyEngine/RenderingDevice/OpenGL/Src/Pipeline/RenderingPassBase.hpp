#pragma once

#include <pe/Defines.hpp>
#include <Common/GLUtils.hpp>
#include <Proxy/GLShaderProgram.hpp>

namespace pe::core::math
{
	class AARect;
}

namespace Poly
{
	class Scene;
	class CameraComponent;
	class RenderingTargetBase;
	class GLTextureDeviceProxy;
	class TextureResource;
	struct ScreenSize;
	class GLRenderingDevice;
	enum class eRenderTargetType;

	enum class ePassType
	{
		BY_MATERIAL,
		GLOBAL,
		_COUNT
	};

	//------------------------------------------------------------------------------
	class RenderingPassBase : public ::pe::core::BaseObject<>
	{
		struct IOBind
		{
			RenderingTargetBase* Target = nullptr;
			::pe::core::storage::String IOName;
		};

	public:
		RenderingPassBase(const GLRenderingDevice* RDI, const ::pe::core::storage::String& vertex, const ::pe::core::storage::String& fragment);
		RenderingPassBase(const GLRenderingDevice* RDI, const ::pe::core::storage::String& vertex, const ::pe::core::storage::String& geometry, const ::pe::core::storage::String& fragment);

		virtual ~RenderingPassBase();

		void Run(Scene* world, const CameraComponent* camera, const ::pe::core::math::AARect& rect, ePassType passType = ePassType::BY_MATERIAL);
		void Finalize();

		void BindOutput(const ::pe::core::storage::String& outputName, RenderingTargetBase* target);
		void BindInput(const ::pe::core::storage::String& inputName, RenderingTargetBase* target);

		void DebugDraw();

		void ClearFBO(GLenum flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	protected:

		virtual void OnRun(Scene* world, const CameraComponent* camera, const ::pe::core::math::AARect& rect, ePassType passType) = 0;

		RenderingTargetBase* GetInputTarget(const ::pe::core::storage::String& name);
		RenderingTargetBase* GetOutputTarget(const ::pe::core::storage::String& name);

		const std::map<::pe::core::storage::String, RenderingTargetBase*>& GetInputs() const { return Inputs; }
		const std::map<::pe::core::storage::String, RenderingTargetBase*>& GetOutputs() const { return Outputs; }
		GLShaderProgram& GetProgram() { return Program; }

		const GLRenderingDevice* RDI;

	private:
		std::map<::pe::core::storage::String, RenderingTargetBase*> Inputs;
		std::map<::pe::core::storage::String, RenderingTargetBase*> Outputs;

		GLShaderProgram Program;
		GLuint FBO = 0;
	};

	//------------------------------------------------------------------------------
	enum class eRenderingTargetType
	{
		TEXTURE_2D,
		TEXTURE_2D_INPUT,
		DEPTH,
		_COUNT
	};

	//------------------------------------------------------------------------------
	class RenderingTargetBase : public ::pe::core::BaseObject<>
	{

	public:
		virtual eRenderingTargetType GetType() const = 0;

		virtual void Resize(const ScreenSize& /*size*/) {}
	};

	//------------------------------------------------------------------------------
	class Texture2DRenderingTarget : public RenderingTargetBase
	{
	public:
		Texture2DRenderingTarget(GLuint format);
		Texture2DRenderingTarget(GLuint format, eRenderTargetType internalUsage);

		eRenderingTargetType GetType() const override { return eRenderingTargetType::TEXTURE_2D; }
		void Resize(const ScreenSize& /*size*/) override;

		GLuint GetTextureID();
	private:
		//GLuint Format;
		eRenderTargetType InternalUsage = eRenderTargetType(0);
		std::unique_ptr<GLTextureDeviceProxy> Texture;
	};

	//------------------------------------------------------------------------------
	class DepthRenderingTarget : public Texture2DRenderingTarget
	{
	public:
		DepthRenderingTarget();

		eRenderingTargetType GetType() const override { return eRenderingTargetType::DEPTH; }
	};

	class Texture2DInputTarget : public RenderingTargetBase
	{
	public:
		Texture2DInputTarget(const ::pe::core::storage::String& path);
		~Texture2DInputTarget();

		GLuint GetTextureID() const;
		eRenderingTargetType GetType() const override { return eRenderingTargetType::TEXTURE_2D_INPUT; }
	private:
		TextureResource* Texture;
	};
}
