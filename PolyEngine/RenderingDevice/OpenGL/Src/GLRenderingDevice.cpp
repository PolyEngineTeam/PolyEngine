#include "GLRenderingDevice.hpp"

#include <memory>

#include <Logger.hpp>

#include "GLUtils.hpp"
#include "GLTextureDeviceProxy.hpp"
#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLMeshDeviceProxy.hpp"

#include "UnlitRenderingPass.hpp"
#include "BlinnPhongRenderingPass.hpp"
#include "Text2DRenderingPass.hpp"
#include "DebugNormalsRenderingPass.hpp"
#include "PostprocessRenderingPass.hpp"
#include "TransparentRenderingPass.hpp"
#include "PostprocessQuad.hpp"

using namespace Poly;

GLRenderingDevice* Poly::gRenderingDevice = nullptr;

#if defined(_WIN32)

//------------------------------------------------------------------------------
IRenderingDevice* __stdcall PolyCreateRenderingDevice(HWND hwnd, RECT rect) { return new GLRenderingDevice(hwnd, rect); }

//------------------------------------------------------------------------------
GLRenderingDevice::GLRenderingDevice(HWND hwnd, RECT rect)
	: hWnd(hwnd)
{
	ASSERTE(gRenderingDevice == nullptr, "Creating device twice?");
	gRenderingDevice = this;

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

	// Setup OpenGL 3.3 context attribs
	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	// Create OpenGL 3.3 context and destroy the temporary one.
	if (epoxy_has_wgl_extension(hDC, "WGL_ARB_create_context"))
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

	glClearColor(0.2f, 0.2f, 0.2f, 1.f);

	// We have successfully created a context, return true

	// InitPrograms();
}

//------------------------------------------------------------------------------
GLRenderingDevice::~GLRenderingDevice()
{
	CleanUpResources();
	wglMakeCurrent(nullptr, nullptr);
	if (hRC)
	{
		wglDeleteContext(hRC);
		hRC = nullptr;
	}
	gRenderingDevice = nullptr;
}

//------------------------------------------------------------------------------
void GLRenderingDevice::EndFrame()
{
	SwapBuffers(hDC);
}

#elif defined(__linux__)

//------------------------------------------------------------------------------
IRenderingDevice* PolyCreateRenderingDevice(Display* display, Window window, GLXFBConfig fbConfig, const ScreenSize& size) { return new GLRenderingDevice(display, window, fbConfig, size); }

//------------------------------------------------------------------------------
GLRenderingDevice::GLRenderingDevice(Display* display, Window window, GLXFBConfig fbConfig, const ScreenSize& size)
	: display(display), window(window)
{
	ASSERTE(gRenderingDevice == nullptr, "Creating device twice?");
	gRenderingDevice = this;

	ScreenDim = size;

	//create GLX OpenGL context
	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		None
	};
	if (epoxy_has_glx_extension(display, DefaultScreen(display), "GLX_ARB_create_context")) {
		this->context = glXCreateContextAttribsARB(this->display, fbConfig, /*share context*/ nullptr, /*direct*/ True, context_attribs);
	} else {
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
	} else {
		Poly::gConsole.LogDebug("Indirect GLX rendering context obtained");
	}
	glXMakeCurrent(this->display, this->window, this->context);

	gConsole.LogInfo("OpenGL context set up successfully");
	gConsole.LogInfo("GL Renderer: {}", glGetString(GL_RENDERER));
	gConsole.LogInfo("GL Version: {}", glGetString(GL_VERSION));
	gConsole.LogInfo("GLSL Version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// InitPrograms();
}

//------------------------------------------------------------------------------
GLRenderingDevice::~GLRenderingDevice()
{
	CleanUpResources();
	if (this->display && this->context) {
		glXMakeCurrent(this->display, None, nullptr);
		glXDestroyContext(this->display, this->context);
		this->context = nullptr;
	}
	gRenderingDevice = nullptr;
}

//------------------------------------------------------------------------------
void GLRenderingDevice::EndFrame()
{
	glXSwapBuffers(this->display, this->window);
}

#elif defined(__APPLE__)

#include "ObjCInterface.hpp"

//------------------------------------------------------------------------------
IRenderingDevice* PolyCreateRenderingDevice(void* window, const ScreenSize& size) { return new GLRenderingDevice(window, size); }

//------------------------------------------------------------------------------
GLRenderingDevice::GLRenderingDevice(void* window, const ScreenSize& size)
	: window(window)
{
    ASSERTE(gRenderingDevice == nullptr, "Creating device twice?");
    gRenderingDevice = this;

    ScreenDim = size;
    view = CreateDeviceImpl(window, size.Width, size.Height);

    gConsole.LogInfo("OpenGL context succesfully setup. [{}]", glGetString(GL_VERSION));
}

//------------------------------------------------------------------------------
GLRenderingDevice::~GLRenderingDevice()
{
	CleanUpResources();
	DestroyDeviceImpl(window);
}

//------------------------------------------------------------------------------
void GLRenderingDevice::EndFrame()
{
    EndFrameImpl(view);
}

#else
#error "Unsupported platform :("
#endif

//------------------------------------------------------------------------------
void GLRenderingDevice::Resize(const ScreenSize& size)
{
	ScreenDim = size;
	for (auto& target : RenderingTargets)
		target->Resize(size);
}

//------------------------------------------------------------------------------
template<typename T>
inline void GLRenderingDevice::RegisterGeometryPass(eGeometryRenderPassType type, const std::initializer_list<InputOutputBind>& inputs, const std::initializer_list<InputOutputBind>& outputs)
{
	RegisterGeometryPassWithArgs<T>(type, inputs, outputs);
}

//------------------------------------------------------------------------------
template<typename T, class... Args_t>
inline void GLRenderingDevice::RegisterGeometryPassWithArgs(eGeometryRenderPassType type, const std::initializer_list<InputOutputBind>& inputs, const std::initializer_list<InputOutputBind>& outputs, Args_t&&... args)
{
	GeometryRenderingPasses[type] = std::make_unique<T>(std::forward<Args_t>(args)...);

	for (const InputOutputBind& bind : outputs)
		GeometryRenderingPasses[type]->BindOutput(bind.Name, bind.Target);

	for (const InputOutputBind& bind : inputs)
		GeometryRenderingPasses[type]->BindInput(bind.Name, bind.Target);

	GeometryRenderingPasses[type]->Finalize();
}

//------------------------------------------------------------------------------
template<typename T, typename... Args>
T* Poly::GLRenderingDevice::CreateRenderingTarget(Args&&... args)
{
	T* target = new T(std::forward<Args>(args)...);
	RenderingTargets.PushBack(std::unique_ptr<RenderingTargetBase>(target));
	return target;
}

//------------------------------------------------------------------------------
void Poly::GLRenderingDevice::RegisterPostprocessPass(ePostprocessRenderPassType type, const String& fragShaderName, const std::initializer_list<InputOutputBind>& inputs, const std::initializer_list<InputOutputBind>& outputs)
{
	PostprocessRenderingPasses[type] = std::make_unique<PostprocessRenderingPass>(PostprocessRenderingQuad.get(), fragShaderName);

	for (const InputOutputBind& bind : outputs)
		PostprocessRenderingPasses[type]->BindOutput(bind.Name, bind.Target);

	for (const InputOutputBind& bind : inputs)
		PostprocessRenderingPasses[type]->BindInput(bind.Name, bind.Target);

	PostprocessRenderingPasses[type]->Finalize();
}

//------------------------------------------------------------------------------
void GLRenderingDevice::InitPrograms()
{
	PostprocessRenderingQuad = std::make_unique<PostprocessQuad>();
	
	// Init input textures
	//Texture2DInputTarget* RGBANoise256 = CreateRenderingTarget<Texture2DInputTarget>("Textures/RGBANoise256x256.png");

	// Init programs
	Texture2DRenderingTarget* texture = CreateRenderingTarget<Texture2DRenderingTarget>(GL_R11F_G11F_B10F);
	DepthRenderingTarget* depth = CreateRenderingTarget<DepthRenderingTarget>();
	// Texture2DRenderingTarget* depth2 = CreateRenderingTarget<Texture2DRenderingTarget>(GL_R16F);

	RegisterGeometryPass<UnlitRenderingPass>(eGeometryRenderPassType::UNLIT, {}, { { "color", texture },{ "depth", depth } });
	RegisterGeometryPass<BlinnPhongRenderingPass>(eGeometryRenderPassType::BLINN_PHONG, {}, { { "color", texture }, { "depth", depth } });
	RegisterGeometryPass<DebugNormalsRenderingPass>(eGeometryRenderPassType::DEBUG_NORMALS, {}, { { "color", texture },{ "depth", depth } });
	RegisterGeometryPass<Text2DRenderingPass>(eGeometryRenderPassType::TEXT_2D, {}, { { "color", texture },{ "depth", depth } });
	RegisterGeometryPassWithArgs<TransparentRenderingPass>(eGeometryRenderPassType::TRANSPARENT_GEOMETRY, {}, { { "color", texture },{ "depth", depth } }, PostprocessRenderingQuad.get());


	RegisterPostprocessPass(ePostprocessRenderPassType::BACKGROUND,			"Shaders/bgFrag.shader",		{}, { { "o_color", texture },	{ "depth", depth } });
	RegisterPostprocessPass(ePostprocessRenderPassType::BACKGROUND_LIGHT,	"Shaders/bgLightFrag.shader",	{}, { { "o_color", texture },	{ "depth", depth } });
	RegisterPostprocessPass(ePostprocessRenderPassType::FOREGROUND,			"Shaders/fgFrag.shader",		{ { "i_color", texture } },		{} );
	RegisterPostprocessPass(ePostprocessRenderPassType::FOREGROUND_LIGHT,	"Shaders/fgLightFrag.shader",	{ { "i_color", texture } },		{} );
	RegisterPostprocessPass(ePostprocessRenderPassType::VINETTE,			"Shaders/vinetteFrag.shader",	{ { "i_color", texture } } );
}

//------------------------------------------------------------------------------
void Poly::GLRenderingDevice::CleanUpResources()
{
	RenderingTargets.Clear();
	for (eGeometryRenderPassType passType : IterateEnum<eGeometryRenderPassType>())
		GeometryRenderingPasses[passType].reset();
	for (ePostprocessRenderPassType passType : IterateEnum<ePostprocessRenderPassType>())
		PostprocessRenderingPasses[passType].reset();
	PostprocessRenderingQuad.reset();
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
