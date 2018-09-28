#include <EnginePCH.hpp>

#include <Imgui/imgui.h>
// #include <Imgui/imgui_impl_opengl3.h>
#include <Imgui/ImguiSystem.hpp>
#include <Time/TimeSystem.hpp>
#include <Imgui/ImguiWorldComponent.hpp>
#include <Input/InputWorldComponent.hpp>
#include <ECS/Scene.hpp>

using namespace Poly;

void ImguiSystem::ImguiUpdatePhase(Scene* scene)
{
	gConsole.LogInfo("ImguiSystem::ImguiUpdatePhase");
	
	float deltaTime = (float)(TimeSystem::GetTimerDeltaTime(scene, Poly::eEngineTimer::GAMEPLAY));
	// float time = (float)(TimeSystem::GetTimerElapsedTime(scene, Poly::eEngineTimer::GAMEPLAY));

	// ImguiWorldComponent* imguiCmp = scene->GetWorldComponent<ImguiWorldComponent>();
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

//  region Imgui ProcessEvent
	if (inputCmp->GetWheelPosDelta().X > 0) io.MouseWheelH += 1;
	if (inputCmp->GetWheelPosDelta().X < 0) io.MouseWheelH -= 1;
	if (inputCmp->GetWheelPosDelta().Y > 0) io.MouseWheel += 1;
	if (inputCmp->GetWheelPosDelta().Y < 0) io.MouseWheel -= 1;
	
	// dummy loop over key codes
	for (int key = 0; key < (int)eKey::_COUNT; ++key)
	{
		if (inputCmp->IsPressed((eKey)key)
			|| inputCmp->IsReleased((eKey)key))
		{
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = inputCmp->IsPressed((eKey)key);
			io.KeyShift = inputCmp->IsPressed(eKey::LSHIFT) || inputCmp->IsPressed(eKey::RSHIFT);
			io.KeyCtrl = inputCmp->IsPressed(eKey::LCTRL) || inputCmp->IsPressed(eKey::RCTRL);
			io.KeyAlt = inputCmp->IsPressed(eKey::LALT) || inputCmp->IsPressed(eKey::RALT);
			io.KeySuper = inputCmp->IsPressed(eKey::LGUI) || inputCmp->IsPressed(eKey::RGUI);
		}
	}
//  endregion
		
// 	region UpdateMousePosAndButtons

	// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	// if (io.WantSetMousePos)
	// 	SDL_WarpMouseInWindow(g_Window, (int)io.MousePos.x, (int)io.MousePos.y);
	// else
	//	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

	// int mx, my;
	// Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
	io.MouseDown[0] = inputCmp->IsPressed(eMouseButton::LEFT);  // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
	io.MouseDown[1] = inputCmp->IsPressed(eMouseButton::RIGHT);
	io.MouseDown[2] = inputCmp->IsPressed(eMouseButton::MIDDLE);

// #if SDL_HAS_CAPTURE_MOUSE && !defined(__EMSCRIPTEN__)
// 	SDL_Window* focused_window = SDL_GetKeyboardFocus();
// 	if (g_Window == focused_window)
// 	{
// 		// SDL_GetMouseState() gives mouse position seemingly based on the last window entered/focused(?)
// 		// The creation of a new windows at runtime and SDL_CaptureMouse both seems to severely mess up with that, so we retrieve that position globally.
// 		int wx, wy;
// 		SDL_GetWindowPosition(focused_window, &wx, &wy);
// 		SDL_GetGlobalMouseState(&mx, &my);
// 		mx -= wx;
// 		my -= wy;
// 		io.MousePos = ImVec2((float)mx, (float)my);
// 	}
// 
// 	// SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger the OS window resize cursor. 
// 	// The function is only supported from SDL 2.0.4 (released Jan 2016)
// 	bool any_mouse_button_down = ImGui::IsAnyMouseDown();
// 	SDL_CaptureMouse(any_mouse_button_down ? SDL_TRUE : SDL_FALSE);
// #else
	// if (SDL_GetWindowFlags(g_Window) & SDL_WINDOW_INPUT_FOCUS)
	Vector2i mousePos = inputCmp->GetMousePos();
	io.MousePos = ImVec2((float)mousePos.X, (float)mousePos.Y);
// #endif
// }

//  endregion

//  region UpdateMouseCursor
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return;

	// ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	// if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
	// {
	// 	Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
	// 	SDL_ShowCursor(SDL_FALSE);
	// 	inputCmp->SetCursorHidden(); // TODO: input cursor visibility handling
	// }
	// else
	// {
	// 	Show OS mouse cursor
	// 	SDL_SetCursor(g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
	// 	SDL_ShowCursor(SDL_TRUE);
	// }
//  endregion

//  region Imgui NewFrame
	IM_ASSERT(io.Fonts->IsBuilt());     // Font atlas needs to be built, call renderer _NewFrame() function e.g. ImGui_ImplOpenGL3_NewFrame()
										// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	// SDL_GetWindowSize(window, &w, &h);
	ScreenSize screenSize = gEngine->GetRenderingDevice()->GetScreenSize();
	display_w = w = screenSize.Width;
	display_h = h = screenSize.Height;

	// SDL_GL_GetDrawableSize(window, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(
		w > 0 ? ((float)display_w / w) : 0,
		h > 0 ? ((float)display_h / h) : 0
	);

	// Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
	// static Uint64 frequency = SDL_GetPerformanceFrequency();
	// Uint64 current_time = SDL_GetPerformanceCounter();
	// io.DeltaTime = g_Time > 0 ? (float)((double)(current_time - g_Time) / frequency) : (float)(1.0f / 60.0f);
	io.DeltaTime = deltaTime;
	// g_Time = time;
//  endregion

	static bool show_demo_window = true;
	// static bool show_another_window = false;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::NewFrame();
	gConsole.LogInfo("ImguiSystem::ImguiUpdatePhase ImGui::NewFrame()");

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		// ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;

		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	// if (show_another_window)
	// {
	// 	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	// 	ImGui::Text("Hello from another window!");
	// 	if (ImGui::Button("Close Me"))
	// 		show_another_window = false;
	// 	ImGui::End();
	// }
}