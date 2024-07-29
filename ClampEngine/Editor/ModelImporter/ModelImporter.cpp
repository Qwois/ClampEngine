#include "ModelImporter.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>
#include "../../Utils/Vector/VectorUtils.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include <experimental/filesystem>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

namespace fs = std::experimental::filesystem;


Color ConvertColor(const aiColor4D& color) {
    return {
        static_cast<unsigned char>(color.r * 255),
        static_cast<unsigned char>(color.g * 255),
        static_cast<unsigned char>(color.b * 255),
        static_cast<unsigned char>(color.a * 255)
    };
}

Mesh LoadMeshFromAssimpMesh(aiMesh* mesh) {
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::vector<unsigned int> indices;

    if (!mesh) {
        std::cerr << "Error: Mesh is null\n";
        return { 0 };
    }

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
        else {
            texcoords.push_back(InitVector2(0.0f, 0.0f));
        }
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
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

    // Free the memory allocated for vertices, normals, texcoords, and indices
    RL_FREE(raylibMesh.vertices);
    RL_FREE(raylibMesh.normals);
    RL_FREE(raylibMesh.texcoords);
    RL_FREE(raylibMesh.indices);

    return raylibMesh;
}

void LoadMaterialData(Model& model, const aiScene* scene) {
    if (scene->HasMaterials()) {
        for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
            aiMesh* mesh = scene->mMeshes[m];
            if (mesh->mMaterialIndex < scene->mNumMaterials) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
                aiColor4D color;
                if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
                    if (m < model.materialCount) {
                        model.materials[m].maps[MATERIAL_MAP_DIFFUSE].color = ConvertColor(color);
                    }
                }
                aiString path;
                if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &path)) {
                    model.materials[m].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture(path.C_Str());
                }
            }
        }
    }
}

void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes) {
    if (!node) {
        std::cerr << "Error: Node is null\n";
        return;
    }

    // Process each mesh in the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        if (mesh) {
            Mesh raylibMesh = LoadMeshFromAssimpMesh(mesh);
            UploadMesh(&raylibMesh, true);
            meshes.push_back(raylibMesh);
        }
        else {
            std::cerr << "Error: Mesh is null at index " << i << "\n";
        }
    }

    // Recursively process each child node
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, meshes);
    }
}

Model ImportModel(const std::string& filePath) {
    if (!std::experimental::filesystem::exists(filePath)) {
        std::cerr << "ERROR: File does not exist: " << filePath << std::endl;
        return LoadModel("");
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return LoadModel("");
    }

    std::vector<Mesh> meshes;
    ProcessNode(scene->mRootNode, scene, meshes);

    Model model = { 0 };
    model.meshCount = static_cast<int>(meshes.size());
    model.meshes = (Mesh*)RL_MALLOC(model.meshCount * sizeof(Mesh));

    for (int i = 0; i < model.meshCount; i++) {
        model.meshes[i] = meshes[i];
    }

    model.materialCount = scene->mNumMaterials;
    model.materials = (Material*)RL_CALLOC(model.materialCount, sizeof(Material));

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        model.materials[i] = LoadMaterialDefault();
    }

    LoadMaterialData(model, scene);

    // Free the memory allocated for meshes and materials


    return model;


}

Texture2D ImportTexture(const std::string& filePath) {
    return LoadTexture(filePath.c_str());
}

Model ImportModelWithDialog() {
    const char* filters[] = { "*.obj", "*.fbx", "*.dae" };
    const char* filePath = tinyfd_openFileDialog(
        "Model",
        "",
        3,
        filters,
        "Model files (*.obj, *.fbx, *.dae)",
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
        "Texture",
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