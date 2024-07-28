#include "ModelImporter.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>
#include "../../Utils/Vector/VectorUtils.h"
#include "tinyfiledialogs/tinyfiledialogs.h"

Model ImportModel(const std::string& filePath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return { 0 };
    }

    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::vector<unsigned int> indices;

    unsigned int vertexOffset = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D vertex = mesh->mVertices[i];
            vertices.push_back(InitVector3(vertex.x, vertex.y, vertex.z));

            if (mesh->HasNormals()) {
                aiVector3D normal = mesh->mNormals[i];
                normals.push_back(InitVector3(normal.x, normal.y, normal.z));
            }

            if (mesh->mTextureCoords[0]) {
                aiVector3D texcoord = mesh->mTextureCoords[0][i];
                texcoords.push_back(InitVector2(texcoord.x, texcoord.y));
            }
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j] + vertexOffset);
            }
        }

        vertexOffset += mesh->mNumVertices;
    }

    Mesh raylibMesh = { 0 };
    raylibMesh.vertexCount = static_cast<int>(vertices.size());
    raylibMesh.triangleCount = static_cast<int>(indices.size()) / 3;

    raylibMesh.vertices = (float*)RL_MALLOC(raylibMesh.vertexCount * 3 * sizeof(float));
    raylibMesh.normals = (float*)RL_MALLOC(raylibMesh.vertexCount * 3 * sizeof(float));
    raylibMesh.texcoords = (float*)RL_MALLOC(raylibMesh.vertexCount * 2 * sizeof(float));
    raylibMesh.indices = (unsigned short*)RL_MALLOC(raylibMesh.triangleCount * 3 * sizeof(unsigned short));

    for (int i = 0; i < raylibMesh.vertexCount; i++) {
        raylibMesh.vertices[i * 3] = vertices[i].x;
        raylibMesh.vertices[i * 3 + 1] = vertices[i].y;
        raylibMesh.vertices[i * 3 + 2] = vertices[i].z;
        if (!normals.empty()) {
            raylibMesh.normals[i * 3] = normals[i].x;
            raylibMesh.normals[i * 3 + 1] = normals[i].y;
            raylibMesh.normals[i * 3 + 2] = normals[i].z;
        }
        if (!texcoords.empty()) {
            raylibMesh.texcoords[i * 2] = texcoords[i].x;
            raylibMesh.texcoords[i * 2 + 1] = texcoords[i].y;
        }
    }

    for (int i = 0; i < raylibMesh.triangleCount * 3; i++) {
        raylibMesh.indices[i] = indices[i];
    }

    UploadMesh(&raylibMesh, true);
    Model model = LoadModelFromMesh(raylibMesh);

    if (scene->HasMaterials()) {
        aiMaterial* material = scene->mMaterials[scene->mMeshes[0]->mMaterialIndex];
        aiColor4D color;
        if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
            model.materials[0].maps[MATERIAL_MAP_DIFFUSE].color = {
                static_cast<unsigned char>(color.r * 255),
                static_cast<unsigned char>(color.g * 255),
                static_cast<unsigned char>(color.b * 255),
                static_cast<unsigned char>(color.a * 255) };
        }
    }

    return model;
}

Texture2D ImportTexture(const std::string& filePath) {
    return LoadTexture(filePath.c_str());
}

Model ImportModelWithDialog() {
    const char* filters[] = { "*.fbx", "*.obj", "*.dae" };
    const char* filePath = tinyfd_openFileDialog(
        "Выберите модель",
        "",
        3,
        filters,
        "Model files (*.fbx, *.obj, *.dae)",
        0);

    if (filePath) {
        return ImportModel(filePath);
    }
    else {
        std::cerr << "Model import canceled or failed.\n";
        return { 0 };
    }
}

Texture2D ImportTextureWithDialog() {
    const char* filters[] = { "*.png", "*.jpg", "*.bmp" };
    const char* filePath = tinyfd_openFileDialog(
        "Выберите текстуру",
        "",
        3,
        filters,
        "Image files (*.png, *.jpg, *.bmp)",
        0);

    if (filePath) {
        return ImportTexture(filePath);
    }
    else {
        std::cerr << "Texture import canceled or failed.\n";
        return { 0 };
    }
}