#include "Globals.h"
#include <raylib.h>
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <iostream>

std::vector<Scene> scenes;
int currentSceneIndex = 0;
int windowWidth = 1600; // Default width
int windowHeight = 900; // Default height
int grid_master = 100;

// Implement functions
void CleanupScene(std::vector<Scene>& scenes) {
    for (Scene& scene : scenes) {
        if (scene.modelLoaded) {
            UnloadModel(scene.model);
        }
        if (scene.textureLoaded) {
            UnloadTexture(scene.texture);
        }
    }
    scenes.clear();
}

void CreateNewScene() {
    scenes.emplace_back(); 
    currentSceneIndex = static_cast<int>(scenes.size()) - 1;
}

void LoadModelFromFile(Scene& scene) {
    const char* filterPatterns[] = { "*.obj", "*.fbx" };
    const char* filePath = tinyfd_openFileDialog("Load Model", "", 2, filterPatterns, "3D files", 1);
    if (filePath) {
        if (scene.modelLoaded) {
            UnloadModel(scene.model);
            if (scene.textureLoaded) {
                UnloadTexture(scene.texture);
                scene.textureLoaded = false;
            }
        }
        scene.model = LoadModel(filePath);
        scene.modelLoaded = scene.model.meshCount > 0;

        if (!scene.modelLoaded) {
            std::cerr << "Failed to load model from: " << filePath << std::endl;
        }
    }
    else {
        std::cerr << "Model load dialog was canceled." << std::endl;
    }
}

void LoadTextureFromFile(Scene& scene) {
    const char* filterPatterns[] = { "*.png", "*.jpg" };
    const char* texturePath = tinyfd_openFileDialog("Load Texture", "", 2, filterPatterns, "Image files", 1);
    if (texturePath) {
        if (scene.textureLoaded) {
            UnloadTexture(scene.texture);
        }
        scene.texture = LoadTexture(texturePath);
        scene.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = scene.texture;
        scene.textureLoaded = true;
    }
}

void UpdateAndDraw(Scene& scene) {
    BeginMode3D(scene.camera);
    if (scene.modelLoaded) {
        DrawModel(scene.model, scene.modelPosition, 1.0f, WHITE);
    }
    DrawGrid(grid_master, 1.0f);
    EndMode3D();

    DrawText("Press L to load a model", 10, 40, 20, DARKGRAY);
    if (scene.modelLoaded) {
        DrawText("Press T to load a texture", 10, 70, 20, DARKGRAY);
    }
}