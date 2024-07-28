#ifndef MODEL_IMPORTER_H
#define MODEL_IMPORTER_H

#include "raylib.h"
#include <string>

Model ImportModel(const std::string& filePath);
Model ImportModelWithDialog();

Texture2D ImportTexture(const std::string& filePath);
Texture2D ImportTextureWithDialog();

#endif
