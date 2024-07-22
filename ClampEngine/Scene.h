#ifndef SCENE_H
#define SCENE_H

#include <raylib.h>

class Scene {
public:
    Scene();
    Camera camera;
    Model model;
    Texture2D texture;
    bool modelLoaded;
    bool textureLoaded;
    Vector3 modelPosition;
};

#endif
