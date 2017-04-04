#pragma once

#include <Windows.h>
#include <d3d11.h>

#include <Core.hpp>
#include "Renderer.hpp"

namespace Poly {

	struct DLLEXPORT DX11RenderingContext : public IRenderingContext {
		DX11RenderingContext(HWND hwnd, RECT rect);
		HWND Hwnd;
		RECT Rect;
	};

	class DX11Renderer : public IRenderer {
	public:
		virtual void Init(const IRenderingContext* context) override;
		virtual void Deinit() override;
		virtual void Update(float dt) override;

		virtual void DrawPoint(const Vector& pos, const Color& color) override;
		virtual void DrawLine(const Vector& pos1, const Vector& pos2, const Color& color1, const Color& color2) override;
		virtual void DrawTraingle(const Vector& pos1, const Vector& pos2, const Vector& pos3, const Color& color1, const Color& color2, const Color& color3) override;

	private:
		IDXGISwapChain *Swapchain;             // the pointer to the swap chain interface
		ID3D11Device *Dev;                     // the pointer to our Direct3D device interface
		ID3D11DeviceContext *Devcon;           // the pointer to our Direct3D device context
		ID3D11RenderTargetView *Backbuffer;    // global declaration
	};

}