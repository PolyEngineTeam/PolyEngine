#include <EnginePCH.hpp>

#include <Imgui/ImguiSystem.hpp>
#include <Time/TimeSystem.hpp>
#include <Input/InputWorldComponent.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

eMouseCursorType GetCursorType(ImGuiMouseCursor imguiCursor)
{
	switch (imguiCursor)
	{
		case (int)ImGuiMouseCursor_None:		return eMouseCursorType::ARROW;
		case (int)ImGuiMouseCursor_Arrow:		return eMouseCursorType::ARROW;
		case (int)ImGuiMouseCursor_TextInput:	return eMouseCursorType::TEXTINPUT;
		case (int)ImGuiMouseCursor_ResizeAll:	return eMouseCursorType::RESIZEALL;
		case (int)ImGuiMouseCursor_ResizeNS:	return eMouseCursorType::RESIZENS;
		case (int)ImGuiMouseCursor_ResizeEW:	return eMouseCursorType::RESIZEEW;
		case (int)ImGuiMouseCursor_ResizeNESW:	return eMouseCursorType::RESIZENESW;
		case (int)ImGuiMouseCursor_ResizeNWSE:	return eMouseCursorType::RESIZENWSE;
		case (int)ImGuiMouseCursor_Hand:		return eMouseCursorType::HAND;
		default:
			ASSERTE(false, "Invalid ImGuiMouseCursor_!");
			break;
	}
	return eMouseCursorType::_COUNT;
}

ImguiSystem::ImguiSystem()
{
	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

	ImGui::StyleColorsDark();

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


	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;	// We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;	// We can honor io.WantSetMousePos requests (optional, rarely used)
	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = (int)eKey::TAB;				// SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = (int)eKey::LEFT;		// SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = (int)eKey::RIGHT;		// SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = (int)eKey::UP;			// SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = (int)eKey::DOWN;		// SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = (int)eKey::PAGEUP;			// SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = (int)eKey::PAGEDOWN;		// SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = (int)eKey::HOME;				// SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = (int)eKey::END;				// SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = (int)eKey::INSERT;			// SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = (int)eKey::DEL;			// SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = (int)eKey::BACKSPACE;	// SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = (int)eKey::SPACE;			// SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = (int)eKey::RETURN;			// SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = (int)eKey::ESCAPE;			// SDL_SCANCODE_ESCAPE;
	io.KeyMap[ImGuiKey_A] = (int)eKey::KEY_A;				// SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = (int)eKey::KEY_C;				// SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = (int)eKey::KEY_V;				// SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = (int)eKey::KEY_X;				// SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = (int)eKey::KEY_Y;				// SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = (int)eKey::KEY_Z;				// SDL_SCANCODE_Z;

	io.SetClipboardTextFn = gEngine->SetClipboardTextFunction;
	io.GetClipboardTextFn = gEngine->GetClipboardTextFunction;
	io.ClipboardUserData = NULL;

	gConsole.LogInfo("ImguiSystem::Ctor GetCurrentContext: {}", ImGui::GetCurrentContext() != nullptr);
}

void ImguiSystem::OnUpdate(Scene* scene)
{
	InputWorldComponent* inputCmp = scene->GetWorldComponent<InputWorldComponent>();

	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	
	ImGuiIO& io = ImGui::GetIO();
	if (!io.Fonts->IsBuilt())
	{
		gConsole.LogInfo("ImguiSystem::ImguiUpdatePhase fonts are not build yet, returning");
		return;
	}

	if (inputCmp->GetWheelPosDelta().X > 0) io.MouseWheelH += 1;
	if (inputCmp->GetWheelPosDelta().X < 0) io.MouseWheelH -= 1;
	if (inputCmp->GetWheelPosDelta().Y > 0) io.MouseWheel += 1;
	if (inputCmp->GetWheelPosDelta().Y < 0) io.MouseWheel -= 1;
	
	// dummy loop over key codes
	for (int key = ((int)eKey::_COUNT) - 1; key >= 0; --key)
	{
		if (inputCmp->IsPressed((eKey)key) || inputCmp->IsReleased((eKey)key))
		{
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = inputCmp->IsPressed((eKey)key);
			io.KeyShift = inputCmp->IsPressed(eKey::LSHIFT) || inputCmp->IsPressed(eKey::RSHIFT);
			io.KeyCtrl = inputCmp->IsPressed(eKey::LCTRL) || inputCmp->IsPressed(eKey::RCTRL);
			io.KeyAlt = inputCmp->IsPressed(eKey::LALT) || inputCmp->IsPressed(eKey::RALT);
			io.KeySuper = inputCmp->IsPressed(eKey::LGUI) || inputCmp->IsPressed(eKey::RGUI);
		}
	}

	// used to read keyboard and fill text input Widgets
	const char* charUTF = inputCmp->GetCharUTF8();
	if (charUTF != nullptr)
		io.AddInputCharactersUTF8(charUTF);

	// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[0] = inputCmp->IsPressed(eMouseButton::LEFT);
	io.MouseDown[1] = inputCmp->IsPressed(eMouseButton::RIGHT);
	io.MouseDown[2] = inputCmp->IsPressed(eMouseButton::MIDDLE);

	// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	if (io.WantSetMousePos)
		gEngine->GetOutputQueue().PushBack({eOutputEventType::MOUSEPOS, Vector2i((int)io.MousePos.x, (int)io.MousePos.y)});
	else
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

	Vector2i mousePos = inputCmp->GetMousePos();
	io.MousePos = ImVec2((float)mousePos.X, (float)mousePos.Y);

	if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
	{
		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			gEngine->GetOutputQueue().PushBack({ eOutputEventType::CURSORHIDE });
		}
		else
		{
			// Show OS mouse cursor
			gEngine->GetOutputQueue().PushBack({ eOutputEventType::CURSORSET, GetCursorType(imgui_cursor) });
			gEngine->GetOutputQueue().PushBack({ eOutputEventType::CURSORSHOW });
		}
	}

	IM_ASSERT(io.Fonts->IsBuilt());     // Font atlas needs to be built, call renderer _NewFrame() function e.g. ImGui_ImplOpenGL3_NewFrame()
	
	// Setup display size (every frame to accommodate for window resizing)
	ScreenSize screenSize = gEngine->GetRenderingDevice()->GetScreenSize();
	io.DisplaySize = ImVec2((float)screenSize.Width, (float)screenSize.Height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(scene, Poly::eEngineTimer::GAMEPLAY));
	io.DeltaTime = deltaTime;

	if (io.WantCaptureMouse || io.WantCaptureKeyboard)
		inputCmp->SetConsumed();

	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	// static bool show_demo_window = true;
	// if (show_demo_window)
	// 	ImGui::ShowDemoWindow(&show_demo_window);
}