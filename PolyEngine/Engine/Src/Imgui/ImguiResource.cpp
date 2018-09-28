#include <EnginePCH.hpp>

#include <Imgui/ImguiResource.hpp>
#include <Resources/ResourceManager.hpp>
// #include <Rendering/IRenderingDevice.hpp>
// #include <ECS/Scene.hpp>
#include <Input/KeyBindings.hpp>
#include <Imgui/imgui.h>

using namespace Poly;

ImguiResource::ImguiResource()
{
	gConsole.LogInfo("ImguiResource::ImguiResource");

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); // (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
	// ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);


// region Init
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;       // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;        // We can honor io.WantSetMousePos requests (optional, rarely used)
																// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = (int)eKey::TAB; // SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = (int)eKey::LEFT; // SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = (int)eKey::RIGHT; // SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = (int)eKey::UP; // SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = (int)eKey::DOWN; // SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = (int)eKey::PAGEUP; // SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = (int)eKey::PAGEDOWN; // SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = (int)eKey::HOME; // SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = (int)eKey::END; // SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = (int)eKey::INSERT; // SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = (int)eKey::DEL; // SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = (int)eKey::BACKSPACE; // SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = (int)eKey::SPACE; // SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = (int)eKey::RETURN; // SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = (int)eKey::ESCAPE; // SDL_SCANCODE_ESCAPE;
	io.KeyMap[ImGuiKey_A] = (int)eKey::KEY_A; // SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = (int)eKey::KEY_C; // SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = (int)eKey::KEY_V; // SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = (int)eKey::KEY_X; // SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = (int)eKey::KEY_Y; // SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = (int)eKey::KEY_Z; // SDL_SCANCODE_Z;

	// io.SetClipboardTextFn = ImGui_ImplSDL2_SetClipboardText;
	// io.GetClipboardTextFn = ImGui_ImplSDL2_GetClipboardText;
	// io.ClipboardUserData = NULL;

	// g_MouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	// g_MouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	// g_MouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	// g_MouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	// g_MouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	// g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	// g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
	// g_MouseCursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
// endregion
}

ImguiResource::~ImguiResource()
{
	gConsole.LogInfo("ImguiResource::~ImguiResource");

	// ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplSDL2_Shutdown();
	

	// SDL_GL_DeleteContext(gl_context);
	// SDL_DestroyWindow(window);
	// SDL_Quit();
}
