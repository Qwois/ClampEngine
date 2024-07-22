#include "ContentBrowser.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <raylib.h>

// Начинаем с пустого вектора assets
std::vector<Asset> assets = {};

// Переменные для отслеживания открытого файла
bool isLuaEditorOpen = false;
std::string currentEditingFilePath = "";
char textBuffer[1024 * 16] = { 0 }; // Буфер для редактирования

void SaveAssets() {
    std::ofstream outFile("project_files.txt");
    if (outFile.is_open()) {
        for (const auto& asset : assets) {
            outFile << asset.name << ";" << asset.type << "\n";
        }
        outFile.close();
        std::cout << "Project saved.\n";
    }
    else {
        std::cerr << "Failed to save project.\n";
    }
}

void LoadAssets() {
    std::ifstream inFile("project_files.txt");
    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            size_t delimiterPos = line.find(';');
            if (delimiterPos != std::string::npos) {
                std::string name = line.substr(0, delimiterPos);
                std::string type = line.substr(delimiterPos + 1);
                assets.push_back({ name, type });
            }
        }
        inFile.close();
        std::cout << "Project loaded.\n";
    }
    else {
        std::cerr << "No saved project found.\n";
    }
}

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

    // Скопировать содержимое файла в textBuffer
    strncpy_s(textBuffer, sizeof(textBuffer), fileContent.c_str(), _TRUNCATE);

    // Установить флаг открытия редактора и сохранить путь к файлу
    isLuaEditorOpen = true;
    currentEditingFilePath = filePath;
}

void DrawLuaEditor() {
    if (!isLuaEditorOpen) return; // Если редактор не открыт, выходим

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver); // Начальный размер окна

    if (ImGui::Begin("Lua Script Editor", &isLuaEditorOpen)) {
        ImGui::InputTextMultiline("##script", textBuffer, sizeof(textBuffer), ImVec2(-1.0f, -1.0f));

        if (ImGui::Button("Save")) {
            std::ofstream outFile(currentEditingFilePath);
            if (outFile.is_open()) {
                outFile << textBuffer;
                outFile.close();
                std::cout << "File saved: " << currentEditingFilePath << std::endl;
            }
            else {
                std::cerr << "Failed to open file for writing: " << currentEditingFilePath << std::endl;
            }
        }
    }
    ImGui::End();
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

    // Отрисовка редактора Lua
    DrawLuaEditor();
}