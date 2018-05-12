#pragma once

#include <BaseObject.hpp>

#include "Resources/Mesh.hpp"

namespace Poly
{
	class Scene;
	class ParticleEmitter;

	struct ScreenSize
	{
		int Width = 0, Height = 0;
	};

	//------------------------------------------------------------------------------
	enum class eTextureDataFormat
	{
		RED,
		RGB,
		RGBA,
		_COUNT
	};

	//------------------------------------------------------------------------------
	enum class eTextureUsageType
	{
		DIFFUSE,
		SPECULAR,
		NORMAL,
		FONT,
		_COUNT
	};

	enum class eCubemapSide
	{
		RIGHT,
		LEFT,
		TOP,
		DOWN,
		BACK,
		FRONT,
		_COUNT
	};

	// REGISTER_ENUM_NAMES(eCubemapSide, "RIGHT", "LEFT", "TOP", "BOTTOM", "BACK", "FRONT");

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT RenderingDeviceSetupFailedException : public BaseObject<>, public std::exception
	{
	public:
		RenderingDeviceSetupFailedException() {}
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT RenderingDeviceProxyCreationFailedException : public BaseObject<>, public std::exception
	{
	public:
		RenderingDeviceProxyCreationFailedException() {}
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ITextureDeviceProxy : public BaseObject<>
	{
	public:
		virtual void SetContent(eTextureDataFormat format, const unsigned char* data) = 0;
		virtual void SetSubContent(size_t width, size_t height, size_t offsetX, size_t offsetY, eTextureDataFormat format, const unsigned char* data) = 0;
	};

	class ENGINE_DLLEXPORT ICubemapDeviceProxy : public BaseObject<>
	{
	public:
		virtual void SetContent(const eCubemapSide side, const unsigned char* data) = 0;
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT ITextFieldBufferDeviceProxy : public BaseObject<>
	{
	public:
		struct ENGINE_DLLEXPORT TextFieldLetter
		{
			float PosX, SizeX;
			float PosY, SizeY;
			float MinU, MaxU;
			float MinV, MaxV;
		};

		virtual void SetContent(size_t count, const TextFieldLetter* letters) = 0;
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT IMeshDeviceProxy : public BaseObject<>
	{
	public:
		virtual void SetContent(const Mesh& mesh) = 0;
	};

	class ENGINE_DLLEXPORT IParticleDeviceProxy : public BaseObject<>
	{
	public:
		virtual void SetContent(const ParticleEmitter& particles) = 0;
	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT IRenderingDevice : public BaseObject<>
	{
	public:
		virtual void Resize(const ScreenSize& size) = 0;
		virtual const ScreenSize& GetScreenSize() const = 0;

		virtual void RenderWorld(Scene* world) = 0;
		virtual void Init() = 0;

		virtual std::unique_ptr<ITextureDeviceProxy> CreateTexture(size_t width, size_t height, eTextureUsageType usage) = 0;
		virtual std::unique_ptr<ICubemapDeviceProxy> CreateCubemap(size_t width, size_t height) = 0;
		virtual std::unique_ptr<ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() = 0;
		virtual std::unique_ptr<IMeshDeviceProxy> CreateMesh() = 0;
		virtual std::unique_ptr<IParticleDeviceProxy> CreateParticle() = 0;
	protected:
	};
}