#include "raylib.h"
#include "imgui.h"
#include "imgui_impl_raylib.h"
#include "rlImGui.h"
#include "UI.h"
#include "Globals.h"
#include "ContentBrowser.h"
#include <iostream>
#include "ClampEngine.cpp"
#include "ResourceManager.h"
#include "ProjectManager.h"
#include <experimental/filesystem>

#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100
#define SIZE_PIXELS 16.0f

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

namespace fs = std::experimental::filesystem;

int main(void) {
    bool cameraLocked = false;
    float f = 0.0f;

    ResourceManager resourceManager;
    ProjectManager projectManager;
    Project currentProject;

    windowWidth = 1600;
    windowHeight = 900;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "ClampEngine [Core] - 3D Scene");

    SetWindowMinSize(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);

    rlImGuiSetup(true);

    SetTargetFPS(60);

    Color backgroundColor = { 255, 255, 255, 255 }; // Default gray color
    ImVec4 imGuiColor = ImVec4(backgroundColor.r / 255.0f, backgroundColor.g / 255.0f, backgroundColor.b / 255.0f, backgroundColor.a / 255.0f);

    if (scenes.empty()) {
        CreateNewScene();
    }

    LoadAssets();

    char projectName[128] = "New Project";
    char projectPath[256] = "./projects";

    while (!WindowShouldClose()) {
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();

        if (windowWidth < MIN_WINDOW_WIDTH || windowHeight < MIN_WINDOW_HEIGHT) {
            SetWindowSize(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
        }

        // DEPRECATED --------------------------------------------------------------->
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            cameraLocked = true;
            DisableCursor();
        }

        if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
            cameraLocked = false;
            EnableCursor();
        }

        if (cameraLocked && currentSceneIndex != -1) {
            UpdateCamera(&scenes[currentSceneIndex].camera, CAMERA_FREE);
        }

        if (IsKeyPressed(KEY_O) && currentSceneIndex != -1) {
            scenes[currentSceneIndex].camera.position = scenes[currentSceneIndex].modelPosition;
        }

        if (IsKeyPressed(KEY_L) && currentSceneIndex != -1) {
            LoadModelFromFile(scenes[currentSceneIndex]);
        }

        if (IsKeyPressed(KEY_T) && currentSceneIndex != -1 && scenes[currentSceneIndex].modelLoaded) {
            LoadTextureFromFile(scenes[currentSceneIndex]);
        }
        // DEPRECATED --------------------------------------------------------------->

        BeginDrawing();

        ClearBackground(backgroundColor);

        if (scenes.empty()) {
            DrawText("No scenes available. Click the + tab to create a new scene.", 10, 40, 20, DARKGRAY);
        }
        else {
            if (currentSceneIndex >= 0 && currentSceneIndex < scenes.size()) {
                UpdateAndDraw(scenes[currentSceneIndex]);
            }
        }

        rlImGuiBegin();

        ImGui::Begin("Tools-Menu");

        if (ImGui::ColorEdit4("Background Color", (float*)&imGuiColor)) {
            backgroundColor.r = (unsigned char)(imGuiColor.x * 255);
            backgroundColor.g = (unsigned char)(imGuiColor.y * 255);
            backgroundColor.b = (unsigned char)(imGuiColor.z * 255);
            backgroundColor.a = (unsigned char)(imGuiColor.w * 255);
        }

        bool enable_feature = true;

        ImGui::Text("Default position: 0.0, 10.0, 10.0");
        if (ImGui::Button("Zero camera-positon")) {
            scenes[currentSceneIndex].camera.position = scenes[currentSceneIndex].modelPosition;
        }

        if (ImGui::Button("Import Model")) {
            LoadModelFromFile(scenes[currentSceneIndex]);
            printf("Model import: true");
        }

        if (ImGui::Button("Import Texture")) {
            LoadTextureFromFile(scenes[currentSceneIndex]);
        }

        ImGui::SliderInt("Grid", &grid_master, 0, 1000, "%10");

        char text[128] = "Type here...";
        ImGui::InputText("Find method in Clamp Engine", text, IM_ARRAYSIZE(text));

        const char* items[] = { "Item 1", "Item 2", "Item 3" };
        static int item_current = 0;
        ImGui::Combo("Test Case", &item_current, items, IM_ARRAYSIZE(items));

        int selected = 0;
        ImGui::RadioButton("Window Mode", &selected, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Fullscreen Mode", &selected, 1);

        if (ImGui::TreeNode("Settings")) {
            ImGui::Checkbox("Enable Shadows", &enable_feature);
            ImGui::SliderFloat("Shadow Intensity", &f, 0.0f, 1.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Advanced Options")) {
            ImGui::InputFloat("Threshold", &f, 0.0f, 1.0f, "%.3f");
            ImGui::ColorEdit4("Tint Color", (float*)&imGuiColor);
            ImGui::TreePop();
        }

        float progress = 0.67f;  
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Loading");

        ImGui::End();

        const float outlinerWidth = 250.0f;
        const float outlinerHeight = 300.0f;

        ImGui::SetNextWindowPos(ImVec2(windowWidth - outlinerWidth, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(outlinerWidth, windowHeight / 2), ImGuiCond_Always);
        ImGui::Begin("Outliner 1", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Text("Content of Outliner 1");
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(windowWidth - outlinerWidth, windowHeight / 2), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(outlinerWidth, windowHeight / 3.5f), ImGuiCond_Always);
        ImGui::Begin("Outliner 2", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Text("Content of Outliner 2");
        ImGui::End();

        DrawContentBrowser();

        // Add toolbar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Create Project")) {
                    ImGui::OpenPopup("Create New Project");
                }
                if (ImGui::MenuItem("Save Project")) {
                    if (!currentProject.name.empty()) {
                        projectManager.SaveProjectConfig(currentProject);
                    }
                    else {
                        std::cerr << "No project loaded to save.\n";
                    }
                }
                if (ImGui::MenuItem("Load Project")) {
                    std::string path = "./projects";
                    currentProject = projectManager.LoadProject(path);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Create New Project Popup
        if (ImGui::BeginPopupModal("Create New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("Project Name", projectName, IM_ARRAYSIZE(projectName));
            ImGui::InputText("Project Path", projectPath, IM_ARRAYSIZE(projectPath));

            if (ImGui::Button("Create")) {
                fs::create_directories(projectPath);
                currentProject = projectManager.CreateProject(projectName, projectPath);
                printf("INFO: ANUS");
                if (!currentProject.name.empty()) {
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        rlImGuiEnd();

        EndDrawing();
    }
    // Cleanup ImGui context
    rlImGuiShutdown();

    // Сохранить проекты перед выходом
    SaveAssets();

    CloseWindow(); // Close the window and OpenGL context

    return 0;
}