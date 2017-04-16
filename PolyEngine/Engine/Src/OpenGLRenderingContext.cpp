#include "OpenGLRenderingContext.hpp"

#include <gl/glew.h>
#include <gl/wglew.h>
#include <Core.hpp>

using namespace Poly;

namespace Poly {
	IRenderingContext* CreateRenderingContext() { return new OpenGLRenderingContext; }
}

//------------------------------------------------------------------------------
OpenGLRenderingContextParams::OpenGLRenderingContextParams(HWND hwnd, RECT rect)
	: HWnd(hwnd), Rect(rect)
{
}

//------------------------------------------------------------------------------
bool OpenGLRenderingContext::Init(const IRenderingContextParams * context)
{
	const OpenGLRenderingContextParams* openGLContext = static_cast<const OpenGLRenderingContextParams*>(context);
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
	bool bResult = SetPixelFormat(hDC, nPixelFormat, &pfd) != 0;   
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
		
		if (!hRC)
		{
			gConsole.LogError("OpenGL context creation failed for version {}.{}. Platform not supported.", attribs[1], attribs[3]);
			return false;
		}
		wglMakeCurrent(hDC, hRC);
	}
	else
	{
		gConsole.LogError("This platform does not support OpenGL 3.3+");
		return false;
	}

	gConsole.LogInfo("OpenGL context succesfully setup. [{}]", glGetString(GL_VERSION));
	
	// We have successfully created a context, return true 
	return true;  
}

//------------------------------------------------------------------------------
void OpenGLRenderingContext::Deinit()
{
	wglMakeCurrent(nullptr, nullptr);
	if (hRC)
	{
		wglDeleteContext(hRC);
		hRC = nullptr;
	}
}

//------------------------------------------------------------------------------
void OpenGLRenderingContext::EndFrame()
{
	SwapBuffers(hDC);
}