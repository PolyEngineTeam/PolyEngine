#include "EnginePCH.hpp"

#include "OpenGLRenderingContext.hpp"

#include <GL/glew.h>
#ifdef _WIN32
	#include <GL/wglew.h>
#elif defined(__linux__)
	#include <GL/glxew.h>
#endif
#include <Core.hpp>

using namespace Poly;

namespace Poly {
	IRenderingContext* CreateRenderingContext() { return new OpenGLRenderingContext; }
}

//------------------------------------------------------------------------------
bool Poly::OpenGLRenderingContext::InitPrograms()
{
	ShaderPrograms[eShaderProgramType::TEST] = new ShaderProgram("test.vsh", "test.fsh");
	//ShaderPrograms[eShaderProgramType::TEST].RegisterUniform("uMVP");
	ShaderPrograms[eShaderProgramType::TEST]->RegisterUniform("uTransform");

	ShaderPrograms[eShaderProgramType::DEBUG_NORMALS] = new ShaderProgram("debugVertSh.shader", "debugGeomSh.shader","debugFragSh.shader");
	ShaderPrograms[eShaderProgramType::DEBUG_NORMALS]->RegisterUniform("u_projection");
	ShaderPrograms[eShaderProgramType::DEBUG_NORMALS]->RegisterUniform("u_MVP");
	ShaderPrograms[eShaderProgramType::DEBUG_NORMALS]->RegisterUniform("u_normalMatrix4x4");

	// TODO error handling
	return true;
}

#if defined(_WIN32)
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
	ScreenDim.Width = openGLContext->Rect.right - openGLContext->Rect.left;
	ScreenDim.Height = openGLContext->Rect.bottom - openGLContext->Rect.top;

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
	return InitPrograms();
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

//------------------------------------------------------------------------------
void Poly::OpenGLRenderingContext::Resize(const ScreenSize & size)
{
	ScreenDim = size;
}

//------------------------------------------------------------------------------
const ScreenSize& Poly::OpenGLRenderingContext::GetScreenSize() const
{
	return ScreenDim;
}


#elif defined(__linux__)

//------------------------------------------------------------------------------
bool OpenGLRenderingContext::Init(const IRenderingContextParams* ictxParams) {
	auto ctxParams = static_cast<const OpenGLRenderingContextParams*>(ictxParams);
	this->display = ctxParams->display;
	this->window = ctxParams->window;

	//create a temporary context to make GLEW happy, then immediately destroy it (it has wrong parameters)
	{
		GLXContext makeGlewHappy = glXCreateNewContext(this->display, ctxParams->fbConfig, GLX_RGBA_TYPE, /*share list*/ nullptr, /*direct*/ True);
		glXMakeCurrent(this->display, this->window, makeGlewHappy);
		gConsole.LogDebug("Temporary GL context for GLEW created.");

		//initialize GLEW
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			gConsole.LogError("GLEW init failed, code: {}, status: {}", err, glewGetErrorString(err));
			return false;
		}
		glXMakeCurrent(this->display, None, nullptr);
		glXDestroyContext(this->display, makeGlewHappy);
		gConsole.LogDebug("GLEW initialized.");
	}

	//create GLX OpenGL context
	this->context = nullptr;
	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		None
	};
	if (GLXEW_ARB_create_context) {
		this->context = glXCreateContextAttribsARB(this->display, ctxParams->fbConfig, /*share context*/ nullptr, /*direct*/ True, context_attribs);
	} else {
		gConsole.LogError("GLX_ARB_create_context extension not found. This platform does not support OpenGL {}.{}+", context_attribs[1], context_attribs[3]);
		return false;
	}
	XSync(display, /*discard events*/ False); //sync to ensure any errors generated are processed

	if (!this->context) {
		gConsole.LogError("OpenGL context creation failed for version {}.{}. Platform not supported.", context_attribs[1], context_attribs[3]);
		return false;
	}

	if (glXIsDirect(this->display, this->context)) {
		Poly::gConsole.LogDebug("Direct GLX rendering context obtained");
	} else {
		Poly::gConsole.LogDebug("Indirect GLX rendering context obtained");
	}
	glXMakeCurrent(this->display, this->window, this->context);

	gConsole.LogInfo("OpenGL context set up successfully");
	gConsole.LogInfo("GL Renderer: {}", glGetString(GL_RENDERER));
	gConsole.LogInfo("GL Version: {}", glGetString(GL_VERSION));
	gConsole.LogInfo("GLSL Version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));

	return true; //success!
}

//------------------------------------------------------------------------------
void OpenGLRenderingContext::Deinit() {
	if (this->display && this->context) {
		glXMakeCurrent(this->display, None, nullptr);
		glXDestroyContext(this->display, this->context);
		this->context = nullptr;
	}
}

//------------------------------------------------------------------------------
void OpenGLRenderingContext::EndFrame() {
	glXSwapBuffers(this->display, this->window);
}

//------------------------------------------------------------------------------
void Poly::OpenGLRenderingContext::Resize(const ScreenSize & size)
{
	ScreenDim = size;
}

//------------------------------------------------------------------------------
const ScreenSize& Poly::OpenGLRenderingContext::GetScreenSize() const
{
	return ScreenDim;
}
#else
	#error
#endif
