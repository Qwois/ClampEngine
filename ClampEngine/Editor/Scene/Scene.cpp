#include "Scene.h"
#include "../../Utils/Vector/VectorUtils.h"

Scene::Scene()
    : modelLoaded(false), textureLoaded(false), modelPosition({ 0.0f, 0.0f, 0.0f }) {
    camera.position = InitVector3( 0.0f, 10.0f, 10.0f );
    camera.target = InitVector3( 0.0f, 0.0f, 0.0f );
    camera.up = InitVector3( 0.0f, 1.0f, 0.0f );
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}