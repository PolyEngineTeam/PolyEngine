#include "OpenGLRenderer.hpp"

#include <gl/glew.h>
#include <gl/wglew.h>
#include <Core.hpp>

using namespace Poly;

namespace Poly {
	IRenderer* CreateRenderer() { return new OpenGLRenderer; }
}

OpenGLRenderingContext::OpenGLRenderingContext(HWND hwnd, RECT rect)
	: HWnd(hwnd), Rect(rect)
{
}

bool OpenGLRenderer::Init(const IRenderingContext * context)
{
	const OpenGLRenderingContext* openGLContext = static_cast<const OpenGLRenderingContext*>(context);
	hWnd = openGLContext->HWnd; // Set the HWND for our window  
	hDC = GetDC(hWnd); // Get the device context for our window  

	// Create a new PIXELFORMATDESCRIPTOR (PFD)  
	PIXELFORMATDESCRIPTOR pfd; 
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR)); // Clear our  PFD  
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); // Set the size of the PFD to the size of the class  
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW; // Enable double buffering, opengl support and drawing to a window  
	pfd.iPixelType = PFD_TYPE_RGBA; // Set our application to use RGBA pixels  
	pfd.cColorBits = 32; // Give us 32 bits of color information (the higher, the more colors)  
	pfd.cDepthBits = 32; // Give us 32 bits of depth information (the higher, the more depth levels)  
	pfd.iLayerType = PFD_MAIN_PLANE; // Set the layer of the PFD  

	// Check if our PFD is valid and get a pixel format back  
	int nPixelFormat = ChoosePixelFormat(hDC, &pfd); 
	if (nPixelFormat == 0)
	{
		gConsole.LogError("Choosing PixelFormat failed! PFD is invalid?");
		return false;
	}

	// Try and set the pixel format based on our PFD
	bool bResult = SetPixelFormat(hDC, nPixelFormat, &pfd);   
	if (!bResult)
	{
		gConsole.LogError("Setting PixelFormat failed!");
		return false;
	}

	// Create temporary context with OpenGL 1.1
	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);

	// Initialize GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		gConsole.LogError("GLEW init failed, code: {}, status: {}", err, glewGetErrorString(err));
		return false;
	}

	// Setup OpenGL 3.3 context attribs
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	// Create OpenGL 3.3 context and destroy the temporary one.
	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hDC, hRC);
	}
	else
	{
		gConsole.LogError("This platform does not support OpenGL 3.3+");
		return false;
	}

	gConsole.LogInfo("OpenGL context succesfully setup. [{}]", glGetString(GL_VERSION));
	return true; // We have successfully created a context, return true  
}

void OpenGLRenderer::Deinit()
{
	wglMakeCurrent(nullptr, nullptr);
	if (hRC)
	{
		wglDeleteContext(hRC);
		hRC = nullptr;
	}
}

void OpenGLRenderer::Update(float dt)
{
	// very simple temporary loop
	glClearColor(0.0f, 0.2f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SwapBuffers(hDC);
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