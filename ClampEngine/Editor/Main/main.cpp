#include "raylib.h"
#include "imgui.h"
#include "../../RlImGui/rlImGui.h"
#include "../../RlImGui/imgui_impl_raylib.h"
#include "../UI/TestUI/UI.h"
#include "../Globals/Globals.h"
#include "../ContentBrowser/ContentBrowser.h"
#include <iostream>
#include "../ClampEngine/ClampEngine.cpp"
#include "../../ResourceManager/ResourceManager.h"
#include "../../Project/Manager/ProjectManager.h"
#include <experimental/filesystem>
#include "../UI/ProjectMenuTop/ProjectMenuTop.h"
#include "../../Editor/ModelImporter/ModelImporter.h"
#include "../../Editor/ModelImporter/ModelImporter.h"

#define MIN_WINDOW_WIDTH 100
#define MIN_WINDOW_HEIGHT 100
#define SIZE_PIXELS 16.0f

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

namespace fs = std::experimental::filesystem;

int main(void) {
    bool cameraLocked = false;
    float shadowIntensity = 0.0f;

    ResourceManager resourceManager;

    windowWidth = 1600;
    windowHeight = 900;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "ClampEngine [Core] - 3D Scene");

    SetWindowMinSize(MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);

    rlImGuiSetup(true);

    SetTargetFPS(60);

    Color backgroundColor = { 255, 255, 255, 255 }; // Default gray color
    ImVec4 imGuiColor = ImVec4(backgroundColor.r / 255.0f, backgroundColor.g / 255.0f, backgroundColor.b / 255.0f, backgroundColor.a / 255.0f);

    bool showCreateProjectPopup = false;

    if (scenes.empty()) {
        CreateNewScene();
    }

    LoadAssets();

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
            if (currentSceneIndex >= 0 && currentSceneIndex < static_cast<int>(scenes.size())) {
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

        bool enableShadows = true;

        ImGui::Text("Default position: 0.0, 10.0, 10.0");
        if (ImGui::Button("Zero camera-position")) {
            scenes[currentSceneIndex].camera.position = scenes[currentSceneIndex].modelPosition;
        }

        if (ImGui::Button("Import Model")) {
            scenes[currentSceneIndex].model = ImportModelWithDialog();
            scenes[currentSceneIndex].modelLoaded = true; // Обновите статус загрузки модели
        }

        if (ImGui::Button("Import Texture")) {
            scenes[currentSceneIndex].texture = ImportTextureWithDialog();
            if (scenes[currentSceneIndex].modelLoaded) {
                SetMaterialTexture(
                    &scenes[currentSceneIndex].model.materials[0],
                    MATERIAL_MAP_DIFFUSE,
                    scenes[currentSceneIndex].texture
                );
            }
        }


        ImGui::SliderInt("Grid", &grid_master, 0, 1000, "%d");

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

        ShowMainMenuBar();  // Main menu bar
        ShowCreateProjectPopup();

        rlImGuiEnd();

        EndDrawing();
    }
    rlImGuiShutdown();

    SaveAssets();

    CloseWindow();

    return 0;
}