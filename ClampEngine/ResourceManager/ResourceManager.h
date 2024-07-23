#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <string>
#include <map>
#include <raylib.h>

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();


    Texture2D LoadTexture(const std::string& filePath);
    Model LoadModel(const std::string& filePath);
    void UnloadTextureByPath(const std::string& filePath);
    void UnloadModelByPath(const std::string& filePath);

private:
    std::map<std::string, Texture2D> textures;
    std::map<std::string, Model> models;
};

#endif
