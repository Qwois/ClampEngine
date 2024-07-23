#include "ResourceManager.h"
#include <iostream>

ResourceManager::ResourceManager() {
    textures.clear();
    models.clear();

    std::cout << "INFO: ResourceManager initialized." << std::endl;
}

ResourceManager::~ResourceManager() {

    for (auto& pair : textures) {
        UnloadTexture(pair.second);
    }
    for (auto& pair : models) {
        UnloadModel(pair.second);
    }
}

Texture2D ResourceManager::LoadTexture(const std::string& filePath) {
    if (textures.find(filePath) != textures.end()) {
        return textures[filePath];
    }

    Texture2D texture = LoadTexture(filePath.c_str());
    textures[filePath] = texture;
    return texture;
}

Model ResourceManager::LoadModel(const std::string& filePath) {
    if (models.find(filePath) != models.end()) {
        return models[filePath];
    }

    Model model = LoadModel(filePath.c_str());
    models[filePath] = model;
    return model;
}

void ResourceManager::UnloadTextureByPath(const std::string& filePath) {
    auto it = textures.find(filePath);
    if (it != textures.end()) {
        UnloadTexture(it->second);
        textures.erase(it);
    }
}

void ResourceManager::UnloadModelByPath(const std::string& filePath) {
    auto it = models.find(filePath);
    if (it != models.end()) {
        UnloadModel(it->second);
        models.erase(it);
    }
}
