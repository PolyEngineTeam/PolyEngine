#include "OpenGLRenderer.hpp"

#include <gl/glew.h>

using namespace Poly;

namespace Poly {
	IRenderer* CreateRenderer() { return new OpenGLRenderer; }
}

OpenGLRenderingContext::OpenGLRenderingContext(HINSTANCE hInstance, HWND hwnd, RECT rect)
	: HInstance(hInstance), HWnd(hwnd), Rect(rect)
{
}

bool OpenGLRenderer::Init(const IRenderingContext * context)
{
	const OpenGLRenderingContext* openGLContext = static_cast<const OpenGLRenderingContext*>(context);
	hWnd = openGLContext->HWnd; // Set the HWND for our window  

	hDC = GetDC(hWnd); // Get the device context for our window  

	PIXELFORMATDESCRIPTOR pfd; // Create a new PIXELFORMATDESCRIPTOR (PFD)  
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD  
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class  
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window  
	pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels  
	pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)  
	pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)  
	pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD  

	int nPixelFormat = ChoosePixelFormat(hDC, &pfd); // Check if our PFD is valid and get a pixel format back  
	if (nPixelFormat == 0) // If it fails  
		return false;

	bool bResult = SetPixelFormat(hDC, nPixelFormat, &pfd); // Try and set the pixel format based on our PFD  
	if (!bResult) // If it fails  
		return false;

	if (GLEW_OK != glewInit())
	{
		// GLEW failed!
		exit(1);
	}

	return true; // We have successfully created a context, return true  
}
void OpenGLRenderer::Deinit()
{
}
void OpenGLRenderer::Update(float dt)
{
}
void OpenGLRenderer::DrawPoint(const Vector & pos, const Color & color)
{
}
void OpenGLRenderer::DrawLine(const Vector & pos1, const Vector & pos2, const Color & color1, const Color & color2)
{
}
void OpenGLRenderer::DrawTraingle(const Vector & pos1, const Vector & pos2, const Vector & pos3, const Color & color1, const Color & color2, const Color & color3)
{
}

bool OpenGLRenderer::InitGlew(HINSTANCE hInstance)
{
	//if (bGlewInitialized)return true;

	//RegisterSimpleOpenGLClass(hInstance);

	/*HWND hWndFake = CreateWindow(SIMPLE_OPENGL_CLASS_NAME, "FAKE", WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
		0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL,
		NULL, hInstance, NULL);

	hDC = GetDC(hWndFake);

	// First, choose false pixel format

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int iPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (iPixelFormat == 0)return false;

	if (!SetPixelFormat(hDC, iPixelFormat, &pfd))return false;

	// Create the false, old style context (OpenGL 2.1 and before)

	HGLRC hRCFake = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRCFake);

	bool bResult = true;

	if (!bGlewInitialized)
	{
		if (GlewInit() != GLEW_OK)
		{
			MessageBox(*hWnd, "Couldn't initialize GLEW!", "Fatal Error", MB_ICONERROR);
			bResult = false;
		}
		bGlewInitialized = true;
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRCFake);
	DestroyWindow(hWndFake);

	return bResult;*/
	return false;
}
