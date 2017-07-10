#include "GLRenderingDevice.hpp"

#include <Logger.hpp>

#include "GLUtils.hpp"
#include "GLTextureDeviceProxy.hpp"
#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLMeshDeviceProxy.hpp"

using namespace Poly;

#if defined(_WIN32)

	//------------------------------------------------------------------------------
	IRenderingDevice* PolyCreateRenderingDevice(HWND hwnd, RECT rect) { return new GLRenderingDevice(hwnd, rect); }

	//------------------------------------------------------------------------------
	GLRenderingDevice::GLRenderingDevice(HWND hwnd, RECT rect)
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
		
		InitPrograms();
	}

	//------------------------------------------------------------------------------
	GLRenderingDevice::~GLRenderingDevice()
	{
		wglMakeCurrent(nullptr, nullptr);
		if (hRC)
		{
			wglDeleteContext(hRC);
			hRC = nullptr;
		}
	}

	//------------------------------------------------------------------------------
	void GLRenderingDevice::EndFrame()
	{
		SwapBuffers(hDC);
	}

#elif defined(__linux__)

	//------------------------------------------------------------------------------
	IRenderingDevice* PolyCreateRenderingDevice(Display* display, Window window, GLXFBConfig fbConfig) { return new GLRenderingDevice(display, window, fbConfig); }

	//------------------------------------------------------------------------------
	GLRenderingDevice::GLRenderingDevice(Display* display, Window window, GLXFBConfig fbConfig)
	 : display(display), window(window)
	{
		//create a temporary context to make GLEW happy, then immediately destroy it (it has wrong parameters)
		{
			GLXContext makeGlewHappy = glXCreateNewContext(this->display, fbConfig, GLX_RGBA_TYPE, /*share list*/ nullptr, /*direct*/ True);
			glXMakeCurrent(this->display, this->window, makeGlewHappy);
			gConsole.LogDebug("Temporary GL context for GLEW created.");

			//initialize GLEW
			GLenum err = glewInit();
			if (err != GLEW_OK) {
				gConsole.LogError("GLEW init failed, code: {}, status: {}", err, glewGetErrorString(err));
				throw RenderingDeviceSetupFailedException();
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
			this->context = glXCreateContextAttribsARB(this->display, fbConfig, /*share context*/ nullptr, /*direct*/ True, context_attribs);
		}
		else {
			gConsole.LogError("GLX_ARB_create_context extension not found. This platform does not support OpenGL {}.{}+", context_attribs[1], context_attribs[3]);
			throw RenderingDeviceSetupFailedException();
		}
		XSync(display, /*discard events*/ False); //sync to ensure any errors generated are processed

		if (!this->context) {
			gConsole.LogError("OpenGL context creation failed for version {}.{}. Platform not supported.", context_attribs[1], context_attribs[3]);
			throw RenderingDeviceSetupFailedException();
		}

		if (glXIsDirect(this->display, this->context)) {
			Poly::gConsole.LogDebug("Direct GLX rendering context obtained");
		}
		else {
			Poly::gConsole.LogDebug("Indirect GLX rendering context obtained");
		}
		glXMakeCurrent(this->display, this->window, this->context);

		gConsole.LogInfo("OpenGL context set up successfully");
		gConsole.LogInfo("GL Renderer: {}", glGetString(GL_RENDERER));
		gConsole.LogInfo("GL Version: {}", glGetString(GL_VERSION));
		gConsole.LogInfo("GLSL Version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));

		InitPrograms();
	}

	//------------------------------------------------------------------------------
	GLRenderingDevice::~GLRenderingDevice()
	{
		if (this->display && this->context) {
			glXMakeCurrent(this->display, None, nullptr);
			glXDestroyContext(this->display, this->context);
			this->context = nullptr;
		}
	}

	//------------------------------------------------------------------------------
	void GLRenderingDevice::EndFrame()
	{
		glXSwapBuffers(this->display, this->window);
	}
#else
	#error "Unsupported platform :("
#endif

//------------------------------------------------------------------------------
void GLRenderingDevice::Resize(const ScreenSize & size)
{
	ScreenDim = size;
}

//------------------------------------------------------------------------------
void GLRenderingDevice::InitPrograms()
{
	// Init programs
	ShaderPrograms[eShaderProgramType::TEST] = new GLShaderProgram("test.vsh", "test.fsh");
	ShaderPrograms[eShaderProgramType::DEBUG_NORMALS] = new GLShaderProgram("debugVertSh.shader", "debugGeomSh.shader", "debugFragSh.shader");
	ShaderPrograms[eShaderProgramType::TEXT_2D] = new GLShaderProgram("Shaders/text2DVert.shader", "Shaders/text2DFrag.shader");
}

//------------------------------------------------------------------------------
std::unique_ptr<ITextureDeviceProxy> GLRenderingDevice::CreateTexture(size_t width, size_t height, eTextureUsageType usage)
{
	return std::make_unique<GLTextureDeviceProxy>(width, height, usage);
}

//------------------------------------------------------------------------------
std::unique_ptr<ITextFieldBufferDeviceProxy> GLRenderingDevice::CreateTextFieldBuffer()
{
	return std::make_unique<GLTextFieldBufferDeviceProxy>();
}

//------------------------------------------------------------------------------
std::unique_ptr<IMeshDeviceProxy> GLRenderingDevice::CreateMesh()
{
	return std::make_unique<GLMeshDeviceProxy>();
}
