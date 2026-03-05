#include "Editor.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_win32.h>

bool Editor::initialized = false;

void Editor::Init(HWND hwnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_InitForOpenGL(hwnd);
    ImGui_ImplOpenGL3_Init("#version 130");

    initialized = true;
}

void Editor::Begin()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Editor::Draw()
{
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static float f = 0.0f;
    static int counter = 0;

    ImGuiIO& io = ImGui::GetIO();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Demo window
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // Main window
    ImGui::Begin("Hello, world!");

    ImGui::Text("This is some useful text.");
    ImGui::Checkbox("Demo Window", &show_demo_window);
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float*)&clear_color);

    if (ImGui::Button("Button"))
        counter++;

    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text(
        "Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / io.Framerate,
        io.Framerate
    );

    ImGui::End();

    // Another window
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");

        if (ImGui::Button("Close Me"))
            show_another_window = false;

        ImGui::End();
    }
}

void Editor::End()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

bool Editor::IsInitialized()
{
    return initialized;
}
