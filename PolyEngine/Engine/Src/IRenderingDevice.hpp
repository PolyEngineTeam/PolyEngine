#pragma once

#include <BaseObject.hpp>

namespace Poly
{
	class World;

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
		FONT,
		_COUNT
	};

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

	};

	//------------------------------------------------------------------------------
	class ENGINE_DLLEXPORT IRenderingDevice : public BaseObject<>
	{
	public:
		virtual void Resize(const ScreenSize& size) = 0;
		virtual const ScreenSize& GetScreenSize() const = 0;

		virtual void RenderWorld(World* world) = 0;

		virtual std::unique_ptr<ITextureDeviceProxy> CreateTexture(size_t width, size_t height, eTextureUsageType usage) = 0;
		virtual std::unique_ptr<ITextFieldBufferDeviceProxy> CreateTextFieldBuffer() = 0;
		//virtual IMeshDeviceProxy* CreateMesh() = 0;
	protected:
	};
}