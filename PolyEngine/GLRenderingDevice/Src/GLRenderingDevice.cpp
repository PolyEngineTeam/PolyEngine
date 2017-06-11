#include "GLRenderingDevice.hpp"

#include <Logger.hpp>

#include "GLUtils.hpp"

#ifdef _WIN32
	#include <GL/wglew.h>
#elif defined(__linux__)
	#include <GL/glxew.h>
#endif

#include "GLTextureDeviceProxy.hpp"


using namespace Poly;

#if defined(_WIN32)

	IRenderingDevice* Poly::CreateRenderingDevice(HWND hwnd, RECT rect) { return new GLRenderingDevice(hwnd, rect); }

	Poly::GLRenderingDevice::GLRenderingDevice(HWND hwnd, RECT rect)
		: hWnd(hwnd)
	{
		hDC = GetDC(hWnd);

		ScreenDim.Width = rect.right - rect.left;
		ScreenDim.Height = rect.bottom - rect.top;

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
			throw RenderingDeviceSetupFailedException();
		}

		// Try and set the pixel format based on our PFD
		bool bResult = SetPixelFormat(hDC, nPixelFormat, &pfd) != 0;
		if (!bResult)
		{
			gConsole.LogError("Setting PixelFormat failed!");
			throw RenderingDeviceSetupFailedException();
		}

		// Create temporary context with OpenGL 1.1
		HGLRC tempContext = wglCreateContext(hDC);
		wglMakeCurrent(hDC, tempContext);

		// Initialize GLEW
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			gConsole.LogError("GLEW init failed, code: {}, status: {}", err, glewGetErrorString(err));
			throw RenderingDeviceSetupFailedException();
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
				throw RenderingDeviceSetupFailedException();
			}
			wglMakeCurrent(hDC, hRC);
		}
		else
		{
			gConsole.LogError("This platform does not support OpenGL 3.3+");
			throw RenderingDeviceSetupFailedException();
		}

		gConsole.LogInfo("OpenGL context succesfully setup. [{}]", glGetString(GL_VERSION));

		// We have successfully created a context, return true
		
		//Init programs
		ShaderPrograms[eShaderProgramType::TEST] = new ShaderProgram("test.vsh", "test.fsh");
		//ShaderPrograms[eShaderProgramType::TEST].RegisterUniform("uMVP");
		ShaderPrograms[eShaderProgramType::TEST]->RegisterUniform("uTransform");

		ShaderPrograms[eShaderProgramType::DEBUG_NORMALS] = new ShaderProgram("debugVertSh.shader", "debugGeomSh.shader", "debugFragSh.shader");
		ShaderPrograms[eShaderProgramType::DEBUG_NORMALS]->RegisterUniform("u_projection");
		ShaderPrograms[eShaderProgramType::DEBUG_NORMALS]->RegisterUniform("u_MVP");
		ShaderPrograms[eShaderProgramType::DEBUG_NORMALS]->RegisterUniform("u_normalMatrix4x4");

		ShaderPrograms[eShaderProgramType::TEXT_2D] = new ShaderProgram("Shaders/text2DVert.shader", "Shaders/text2DFrag.shader");
		ShaderPrograms[eShaderProgramType::TEXT_2D]->RegisterUniform("u_projection");
		ShaderPrograms[eShaderProgramType::TEXT_2D]->RegisterUniform("u_textColor");
		ShaderPrograms[eShaderProgramType::TEXT_2D]->RegisterUniform("u_position");
	}

	Poly::GLRenderingDevice::~GLRenderingDevice()
	{
		wglMakeCurrent(nullptr, nullptr);
		if (hRC)
		{
			wglDeleteContext(hRC);
			hRC = nullptr;
		}
	}

	void Poly::GLRenderingDevice::EndFrame()
	{
		SwapBuffers(hDC);
	}

#elif defined(__linux__)
#error "Unsupported platform :("
#else
#error "Unsupported platform :("
#endif

void Poly::GLRenderingDevice::Resize(const ScreenSize & size)
{
	ScreenDim = size;
}

std::unique_ptr<ITextureDeviceProxy> Poly::GLRenderingDevice::CreateTexture(size_t width, size_t height, eTextureUsageType usage)
{
	return std::make_unique<GLTextureDeviceProxy>(width, height, usage);
}