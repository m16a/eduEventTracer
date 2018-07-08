// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "event_collector.h"

#include <stdio.h>
#include <GL/gl3w.h>    // This example is using gl3w to access OpenGL functions. You may freely use any other OpenGL loader such as: glew, glad, glLoadGen, etc.
//#include <glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


void DrawCout(std::stringstream& logBuffer)
{
		static ImGuiTextBuffer buffer;
    ImGui::Separator();

		buffer.appendf(logBuffer.str().c_str());
		ImGui::BeginChild("Log");
		ImGui::TextUnformatted(buffer.begin(), buffer.end());
		ImGui::SetScrollHere(1.0f);
		ImGui::EndChild();
}

void DrawIntervals(CEventCollector& eventCollector)
{
		const std::vector<STimeIntervalArg>& intervals = eventCollector.GetIntervals();

		static float scale = 1.0f;
		ImGui::DragFloat("Scale", &scale, 0.01f, 0.01f, 2.0f, "%.2f");

		ImGui::BeginChild("scrolling", ImVec2(0, ImGui::GetFrameHeightWithSpacing()*7 + 30), true, ImGuiWindowFlags_HorizontalScrollbar);

		if (!intervals.empty())
		{
			float width = 0.0f;
			if (intervals.size() > 1)
				width = (intervals.back().endTime - intervals.front().startTime) * scale;

			ImGui::BeginChild("scrolling2", ImVec2(width, ImGui::GetFrameHeightWithSpacing()*6), false);

			const int epoch = intervals[0].startTime;
			for (const auto& interval : intervals)
			{
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				
        const ImVec2 p = ImGui::GetCursorScreenPos();
				static ImVec4 col = ImVec4(1.0f,1.0f,0.4f,1.0f);
				const ImU32 col32 = ImColor(col);

				const float x1 = p.x + (interval.startTime - epoch) * scale;
				const float y1 = p.y + 30.0f;

				const float x2 = p.x + (interval.endTime - epoch) * scale;
				const float y2 = p.y + 50.0f;

				draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col32);
			}
			ImGui::EndChild();
		}

		
    ImGui::EndChild();
}

void UpdateUI(GLFWwindow* window, CEventCollector& eventCollector, std::stringstream& logBuffer)
{
    static bool show_demo_window = true;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
				ImGui::Begin("Window");
				static bool bIsCapturing = false;

				bool prevIsCapturing = bIsCapturing;
				if (ImGui::Button("Button"))
				{
					bIsCapturing = !bIsCapturing;
				}

				if (prevIsCapturing != bIsCapturing)
				{
					if (bIsCapturing)
						eventCollector.StartCapture();
					else
						eventCollector.StopCapture();
				}


				if (bIsCapturing)
				{
					ImGui::Text("Capturing");
				}
				else
				{
					ImGui::Text("Non capturing");
				}

				DrawIntervals(eventCollector);
				DrawCout(logBuffer);

				ImGui::End();
		}


		// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
		if (show_demo_window)
		{
				ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
				ImGui::ShowDemoWindow(&show_demo_window);
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "eduEventtracer", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

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




    // Main loop
		CEventCollector	ec;
    while (!glfwWindowShouldClose(window))
    {
			std::stringstream logBuffer;
			std::streambuf* old = std::cout.rdbuf(logBuffer.rdbuf());

			ec.Update();
			UpdateUI(window, ec, logBuffer);
			std::cout.rdbuf(old);
    }


    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
