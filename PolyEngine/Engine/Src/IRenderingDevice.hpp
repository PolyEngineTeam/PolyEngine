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
	class ENGINE_DLLEXPORT RenderingDeviceSetupFailedException : public BaseObject<>, public std::exception
	{
	public:
		RenderingDeviceSetupFailedException() {}
	};

	class ENGINE_DLLEXPORT ITextureDeviceProxy : public BaseObject<>
	{

	};

	class ENGINE_DLLEXPORT IMeshDeviceProxy : public BaseObject<>
	{

	};

	class ENGINE_DLLEXPORT IFontDeviceProxy : public BaseObject<>
	{

	};


	class ENGINE_DLLEXPORT IRenderingDevice : public BaseObject<>
	{
	public:
		virtual void Resize(const ScreenSize& size) = 0;
		virtual const ScreenSize& GetScreenSize() const = 0;

		virtual void RenderWorld(World* world) = 0;

		//virtual ITextureDeviceProxy* CreateTexture() = 0;
		//virtual IMeshDeviceProxy* CreateMesh() = 0;
		//virtual IFontDeviceProxy* CreateFont() = 0;
	protected:
	};
}