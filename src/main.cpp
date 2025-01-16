#include <imgui.h>
#include <imgui_node_editor.h>
#include <application.h>
#include <windows.h>
#include <iostream>

namespace ed = ax::NodeEditor;

struct Example :
    public Application
{
    using Application::Application;

    void OnStart() override
    {
        ed::Config config;
        config.SettingsFile = "Simple.json";
        m_Context = ed::CreateEditor(&config);
    
        auto& io = ImGui::GetIO();
    
        io.Fonts->Clear();
    
        const char* fontPath1 = "Cuprum-Bold.ttf";
        const char* fontPath2 = "Oswald-Regular.ttf";
    
        ImFont* font1 = io.Fonts->AddFontFromFileTTF(fontPath1, 16.0f);
        ImFont* font2 = io.Fonts->AddFontFromFileTTF(fontPath2, 16.0f);
    
        if (font1 == nullptr) {
            std::cerr << "Could not load font: " << fontPath1 << std::endl;
        } else {
            std::cout << "Font loaded successfully: " << fontPath1 << std::endl;
        }
    
        if (font2 == nullptr) {
            std::cerr << "Could not load font: " << fontPath2 << std::endl;
        } else {
            std::cout << "Font loaded successfully: " << fontPath2 << std::endl;
        }
    
        io.Fonts->Build();
    
        io.FontDefault = font1 ? font1 : font2;
    }


    void OnStop() override
    {
        ed::DestroyEditor(m_Context);
    }

    void OnFrame(float deltaTime) override
    {
        auto& io = ImGui::GetIO();

        ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

        ImGui::Separator();

        ed::SetCurrentEditor(m_Context);
        ed::Begin("My Editor", ImVec2(0.0, 0.0f));
        int uniqueId = 1;
        ed::BeginNode(uniqueId++);
        ImGui::Text("Node A");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("-> In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out ->");
        ed::EndPin();
        ed::EndNode();
        ed::End();
        ed::SetCurrentEditor(nullptr);

        ImGui::ShowMetricsWindow();
    }

    ed::EditorContext* m_Context = nullptr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    Example example("Simple", 0, nullptr);

    if (example.Create())
        return example.Run();

    return 0;
}
