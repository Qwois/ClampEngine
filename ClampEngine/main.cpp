#include "raylib.h"
#include "imgui.h"
#include "imgui_impl_raylib.h"
#include "rlImGui.h"
#include "UI.h"
#include "Globals.h"
#include "ContentBrowser.h"
#include <iostream>
#include "ClampEngine.cpp"

#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100

#define SIZE_PIXELS 16.0f


int main(void) {
    bool cameraLocked = false;
    float f = 0.0f;

    // Initialize with a reasonable default size, ensuring the application is windowed
    windowWidth = 1600;
    windowHeight = 900;

    // Create a windowed application with the ability to resize
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "ClampEngine [Core] - 3D Scene");

    // Set the minimum window size
    SetWindowMinSize(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);

    // Initialize the ImGui context with Raylib
    rlImGuiSetup(true);

    SetTargetFPS(60);

    // Initialize background color
    Color backgroundColor = { 255, 255, 255, 255 }; // Default gray color
    ImVec4 imGuiColor = ImVec4(backgroundColor.r / 255.0f, backgroundColor.g / 255.0f, backgroundColor.b / 255.0f, backgroundColor.a / 255.0f);

    // Initialize a new scene if no scenes exist
    if (scenes.empty()) {
        CreateNewScene();
    }

    while (!WindowShouldClose()) {
        // Update the window width and height dynamically
        windowWidth = GetScreenWidth();
        windowHeight = GetScreenHeight();

        // Ensure the window size does not fall below the minimum constraints
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

        if (IsKeyPressed(KEY_SPACE) && currentSceneIndex != -1) {
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

        // Clear background with the selected color
        ClearBackground(backgroundColor);

        if (scenes.empty()) {
            DrawText("No scenes available. Click the + tab to create a new scene.", 10, 40, 20, DARKGRAY);
        }
        else {
            DrawTabs();
            if (currentSceneIndex >= 0 && currentSceneIndex < scenes.size()) {
                UpdateAndDraw(scenes[currentSceneIndex]);
            }
        }

        rlImGuiBegin();

        // Main ImGui Window
        ImGui::Begin("ImGui Window");

        // Color editor
        if (ImGui::ColorEdit3("Background Color", (float*)&imGuiColor)) {
            // Update background color when changed
            backgroundColor.r = (unsigned char)(imGuiColor.x * 255);
            backgroundColor.g = (unsigned char)(imGuiColor.y * 255);
            backgroundColor.b = (unsigned char)(imGuiColor.z * 255);
            backgroundColor.a = (unsigned char)(imGuiColor.w * 255);
        }

        // Checkboxes
        bool show_demo_window = false;
        ImGui::Checkbox("Show Demo Window", &show_demo_window);
        bool enable_feature = true;
        ImGui::Checkbox("Enable Feature", &enable_feature);

        // Buttons
        if (ImGui::Button("Import Model")) {
            LoadModelFromFile(scenes[currentSceneIndex]);
            printf("Model import: true");
        }

        ImGui::SameLine();

        if (ImGui::Button("Import Texture")) {
            LoadTextureFromFile(scenes[currentSceneIndex]);
        }

        // Text input
        char text[128] = "Type here...";
        ImGui::InputText("Find method in Clamp Engine", text, IM_ARRAYSIZE(text));

        // Combo box
        const char* items[] = { "Item 1", "Item 2", "Item 3" };
        static int item_current = 0;
        ImGui::Combo("Test Case", &item_current, items, IM_ARRAYSIZE(items));

        // Radio buttons
        int selected = 0;
        ImGui::RadioButton("Window Mode", &selected, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Fullscrean Mode", &selected, 1);

        // Dropdown (Tree nodes)
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

        // Progress bar
        float progress = 0.67f;  // Completion percentage
        ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "Loading");

        ImGui::End();

        // Outliner windows
        const float outlinerWidth = 250.0f;  // Width of each outliner window
        const float outlinerHeight = 300.0f; // Height of each outliner window

        // First Outliner window
        ImGui::SetNextWindowPos(ImVec2(windowWidth - outlinerWidth, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(outlinerWidth, windowHeight / 2), ImGuiCond_Always);
        ImGui::Begin("Outliner 1", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Text("Content of Outliner 1");
        ImGui::End();

        // Second Outliner window
        ImGui::SetNextWindowPos(ImVec2(windowWidth - outlinerWidth, windowHeight / 2), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(outlinerWidth, windowHeight / 3.5f), ImGuiCond_Always);
        ImGui::Begin("Outliner 2", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        ImGui::Text("Content of Outliner 2");
        ImGui::End();

        DrawContentBrowser();

        rlImGuiEnd();

        EndDrawing();
    }
    // Cleanup ImGui context
    rlImGuiShutdown();

    CloseWindow(); // Close the window and OpenGL context

    return 0;
}
