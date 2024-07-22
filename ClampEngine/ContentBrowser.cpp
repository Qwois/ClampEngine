#include "ContentBrowser.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream> 
#include <raylib.h>


std::vector<Asset> assets = {
    {"Model1.obj", "3D Model"},
    {"Texture1.png", "Texture"},
    {"Material1.mat", "Material"},
    {"Script1.lua", "Lua Script"},
    {"Animation1.anim", "Animation"}
};


void EditFile(const std::string& filePath) {
    if (!FileExists(filePath.c_str())) {
        std::cerr << "File does not exist: " << filePath << std::endl;
        return;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    std::string fileContent;
    while (std::getline(file, line)) {
        fileContent += line + "\n";
    }
    file.close();


    static char textBuffer[1024 * 16]; 

    strncpy_s(textBuffer, sizeof(textBuffer), fileContent.c_str(), _TRUNCATE);

    if (ImGui::Begin("Lua Script Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::InputTextMultiline("##script", textBuffer, sizeof(textBuffer), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16))) {
            std::ofstream outFile(filePath);
            if (outFile.is_open()) {
                outFile << textBuffer;
                outFile.close();
            }
            else {
                std::cerr << "Failed to open file for writing: " << filePath << std::endl;
            }
        }
        ImGui::End();
    }
}

void DrawContentBrowser() {
    ImGuiIO& io = ImGui::GetIO();
    float windowHeight = io.DisplaySize.y;
    float windowWidth = io.DisplaySize.x;

    ImGui::SetNextWindowPos(ImVec2(0, windowHeight - 200), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(windowWidth, 200), ImGuiCond_Always);

    ImGui::Begin("Content Browser", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Create New File")) {
        ImGui::OpenPopup("Create File");
    }

    if (ImGui::BeginPopupModal("Create File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char fileName[128] = "";
        ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));

        static int fileType = 0;
        const char* fileTypes[] = { "3D Model", "Texture", "Material", "Lua Script", "Animation" };
        ImGui::Combo("File Type", &fileType, fileTypes, IM_ARRAYSIZE(fileTypes));

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            std::string newFileName = std::string(fileName) + (fileType == 0 ? ".obj" :
                fileType == 1 ? ".png" :
                fileType == 2 ? ".mat" :
                fileType == 3 ? ".lua" :
                ".anim");

            std::ofstream outFile(newFileName);
            if (outFile.is_open()) {
                outFile << ""; 
                outFile.close();
                assets.push_back({ newFileName, fileTypes[fileType] });
            }
            else {
                std::cerr << "Failed to create file: " << newFileName << std::endl;
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::Button("Drop Files Here", ImVec2(-1.0f, 0.0f));

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_ASSET")) {
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Columns(4, nullptr, false);
    ImGui::Separator();

    for (size_t i = 0; i < assets.size(); ++i) {
        const Asset& asset = assets[i];

        bool isSelected = false;

        if (ImGui::Selectable((asset.name + " (" + asset.type + ")").c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, 0))) {
            std::cout << "Selected asset: " << asset.name << " (" << asset.type << ")" << std::endl;

            if (asset.type == "Lua Script") {
                std::string filePath = "./" + asset.name;
                EditFile(filePath);
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("DND_ASSET", &assets[i], sizeof(Asset));
            ImGui::Text("Dragging %s", asset.name.c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::Separator();

    ImGui::End();
}