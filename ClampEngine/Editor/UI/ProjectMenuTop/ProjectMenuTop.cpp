#include "ProjectMenuTop.h"
#include "../../../Project/Manager/ProjectManager.h"
#include "imgui.h"
#include <iostream>

ProjectManager projectManager;
Project currentProject;

char projectName[128] = "New Project";
char projectPath[256] = "./projects/NewProject";

bool showCreateProjectPopup = false;
bool themeWindowOpen = false;


void ShowMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Create Project")) {
                std::cout << "Create Project menu item selected\n";
                showCreateProjectPopup = true;
                ImGui::OpenPopup("Create New Project");
            }
            if (ImGui::MenuItem("Save Project")) {
                if (!currentProject.name.empty()) {
                    projectManager.SaveProjectConfig(currentProject);
                    std::cout << "INFO: Project saved successfully.\n";
                }
                else {
                    std::cerr << "ERROR: No project loaded to save.\n";
                }
            }
            if (ImGui::MenuItem("Load Project")) {
                currentProject = projectManager.LoadProject("./projects/NewProject"); 
                if (!currentProject.name.empty()) {
                    std::cout << "INFO: Project loaded successfully.\n";
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::MenuItem("Theme")) {
                themeWindowOpen = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (themeWindowOpen) {
        if (ImGui::Begin("ThemeWindow", &themeWindowOpen)) {
            ImGui::Text("Pick theme:");
            if (ImGui::Button("Light Theme")) {
                ImGui::StyleColorsLight();
            }

            if (ImGui::Button("Dark Theme")) {
                ImGui::StyleColorsDark();
            }

            if (ImGui::Button("Classic Theme")) {
                ImGui::StyleColorsClassic();
            }
        }
        ImGui::End();
    }
}

void ShowCreateProjectPopup() {
    if (ImGui::BeginPopupModal("Create New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Project Name", projectName, IM_ARRAYSIZE(projectName));
        ImGui::InputText("Project Path", projectPath, IM_ARRAYSIZE(projectPath));

        if (ImGui::Button("Create")) {
            std::string path(projectPath);
            if (path.back() != '/') {
                path += '/';
            }
            path += projectName;

            currentProject = projectManager.CreateProject(projectName, path);
            if (!currentProject.name.empty()) {
                std::cout << "INFO: Project created successfully.\n";
                showCreateProjectPopup = false;
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            showCreateProjectPopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}