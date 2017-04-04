#include "DX11Renderer.hpp"

#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

using namespace Poly;

namespace Poly {
	IRenderer* CreateRenderer() { return new DX11Renderer; }
}

void DX11Renderer::Init(const IRenderingContext * context)
{
	const DX11RenderingContext* dx11context = static_cast<const DX11RenderingContext*>(context);

	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = dx11context->Hwnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

															// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&Swapchain,
		&Dev,
		NULL,
		&Devcon);

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	Dev->CreateRenderTargetView(pBackBuffer, NULL, &Backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	Devcon->OMSetRenderTargets(1, &Backbuffer, NULL);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = dx11context->Rect.top;
	viewport.TopLeftY = dx11context->Rect.left;
	viewport.Width = dx11context->Rect.right - dx11context->Rect.left;
	viewport.Height = dx11context->Rect.bottom - dx11context->Rect.top;

	Devcon->RSSetViewports(1, &viewport);
}

void DX11Renderer::Deinit()
{
	Swapchain->Release();
	Dev->Release();
	Devcon->Release();
}

void DX11Renderer::Update(float dt)
{
	// clear the back buffer to a deep blue
	Devcon->ClearRenderTargetView(Backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

	// do 3D rendering on the back buffer here

	// switch the back buffer and the front buffer
	Swapchain->Present(0, 0);
}

//TODO implement imediate drawing
void Poly::DX11Renderer::DrawPoint(const Vector & pos, const Color & color)
{
}

void Poly::DX11Renderer::DrawLine(const Vector & pos1, const Vector & pos2, const Color & color1, const Color & color2)
{
}

void Poly::DX11Renderer::DrawTraingle(const Vector & pos1, const Vector & pos2, const Vector & pos3, const Color & color1, const Color & color2, const Color & color3)
{
}

DX11RenderingContext::DX11RenderingContext(HWND hwnd, RECT rect)
	: Hwnd(hwnd), Rect(rect)
{
}
