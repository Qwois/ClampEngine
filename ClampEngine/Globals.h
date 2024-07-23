#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "Scene.h"

extern std::vector<Scene> scenes;
extern int currentSceneIndex;
extern int windowWidth;
extern int windowHeight;
extern int grid_master;

void CleanupScene(std::vector<Scene>& scenes);
void CreateNewScene();
void LoadModelFromFile(Scene& scene);
void LoadTextureFromFile(Scene& scene);
void UpdateAndDraw(Scene& scene);

#endif
