#include "GLRenderingDevice.hpp"

#include <memory>

#include <Logger.hpp>

#include "GLUtils.hpp"
#include "GLTextureDeviceProxy.hpp"
#include "GLCubemapDeviceProxy.hpp"
#include "GLTextFieldBufferDeviceProxy.hpp"
#include "GLMeshDeviceProxy.hpp"

#include "UnlitRenderingPass.hpp"
#include "BlinnPhongRenderingPass.hpp"
#include "Text2DRenderingPass.hpp"
#include "DebugNormalsRenderingPass.hpp"
#include "DebugNormalsWireframeRenderingPass.hpp"
#include "DebugRenderingPass.hpp"
#include "PostprocessRenderingPass.hpp"
#include "TransparentRenderingPass.hpp"
#include "SkyboxRenderingPass.hpp"
#include "PostprocessQuad.hpp"
#include "PrimitiveCube.hpp"

using namespace Poly;

GLRenderingDevice* Poly::gRenderingDevice = nullptr;

IRenderingDevice* POLY_STDCALL PolyCreateRenderingDevice(SDL_Window* window, const Poly::ScreenSize& size) { return new GLRenderingDevice(window, size); }

GLRenderingDevice::GLRenderingDevice(SDL_Window* window, const Poly::ScreenSize& size)
: Window(window), ScreenDim(size)
{
	ASSERTE(window, "Invalid window passed to rendering device.");
	ASSERTE(gRenderingDevice == nullptr, "Creating device twice?");
	gRenderingDevice = this;

	// Setup SDL OpenLG attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	Context = SDL_GL_CreateContext(Window);

    if(!Context)
    {
        gConsole.LogError("Context setup failed, err: {}", SDL_GetError());
        ASSERTE(Context, "OpenGL context creation failed!");
    }

	gConsole.LogInfo("OpenGL context set up successfully");
	gConsole.LogInfo("GL Renderer: {}", glGetString(GL_RENDERER));
	gConsole.LogInfo("GL Version: {}", glGetString(GL_VERSION));
	gConsole.LogInfo("GLSL Version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Setup V-Sync
	SDL_GL_SetSwapInterval(1);
}

//------------------------------------------------------------------------------
GLRenderingDevice::~GLRenderingDevice()
{
	CleanUpResources();

	if(Context)
		SDL_GL_DeleteContext(Context);

	gRenderingDevice = nullptr;
}

//------------------------------------------------------------------------------
void GLRenderingDevice::EndFrame()
{
	if(Window && Context)
		SDL_GL_SwapWindow(Window);
}

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
	PrimitiveRenderingCube = std::make_unique<PrimitiveCube>();
	
	// Init input textures
	//Texture2DInputTarget* RGBANoise256 = CreateRenderingTarget<Texture2DInputTarget>("Textures/RGBANoise256x256.png");

	// Init programs
	Texture2DRenderingTarget* texture = CreateRenderingTarget<Texture2DRenderingTarget>(GL_R11F_G11F_B10F);
	DepthRenderingTarget* depth = CreateRenderingTarget<DepthRenderingTarget>();
	// Texture2DRenderingTarget* depth2 = CreateRenderingTarget<Texture2DRenderingTarget>(GL_R16F);

	RegisterGeometryPass<UnlitRenderingPass>(eGeometryRenderPassType::UNLIT, {}, { { "color", texture },{ "depth", depth } });
	RegisterGeometryPass<BlinnPhongRenderingPass>(eGeometryRenderPassType::BLINN_PHONG, {}, { { "color", texture }, { "depth", depth } });
	RegisterGeometryPass<DebugNormalsRenderingPass>(eGeometryRenderPassType::DEBUG_NORMALS, {}, { { "color", texture },{ "depth", depth } });
	RegisterGeometryPass<DebugRenderingPass>(eGeometryRenderPassType::DEBUG, {}, { { "color", texture },{ "depth", depth } });
	RegisterGeometryPass<DebugNormalsWireframeRenderingPass>(eGeometryRenderPassType::DEBUG_NORMALS_WIREFRAME, {}, { { "color", texture },{ "depth", depth } });
	RegisterGeometryPass<Text2DRenderingPass>(eGeometryRenderPassType::TEXT_2D, {}, { { "color", texture },{ "depth", depth } });
	RegisterGeometryPassWithArgs<SkyboxRenderingPass>(eGeometryRenderPassType::SKYBOX, {}, { { "color", texture },{ "depth", depth } }, PrimitiveRenderingCube.get());
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
	PrimitiveRenderingCube.reset();
}

//------------------------------------------------------------------------------
std::unique_ptr<ITextureDeviceProxy> GLRenderingDevice::CreateTexture(size_t width, size_t height, eTextureUsageType usage)
{
	return std::make_unique<GLTextureDeviceProxy>(width, height, usage);
}

std::unique_ptr<ICubemapDeviceProxy> Poly::GLRenderingDevice::CreateCubemap(size_t width, size_t height)
{
	return std::make_unique<GLCubemapDeviceProxy>(width, height);
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