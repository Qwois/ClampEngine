#include "ProjectMenuTop.h"
#include "../../../Project/Manager/ProjectManager.h"

ProjectManager projectManager;
Project currentProject;

bool showCreateProjectPopup = false;

char projectName[128] = "New Project";
char projectPath[256] = "./projects";

namespace fs = std::experimental::filesystem;


void ToolBarMain()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Create Project")) {
                std::cout << "INFO: Open Create New Project Popup" << std::endl; // Debug statement
                showCreateProjectPopup = true;
            }
            if (ImGui::MenuItem("Save Project")) {
                if (!currentProject.name.empty()) {
                    projectManager.SaveProjectConfig(currentProject);
                    std::cout << "INFO: Project saved successfully." << std::endl;
                }
                else {
                    std::cerr << "ERROR: No project loaded to save.\n";
                }
            }
            if (ImGui::MenuItem("Load Project")) {
                std::string path = "./projects";
                currentProject = projectManager.LoadProject(path);
                std::cout << "INFO: Project loaded successfully." << std::endl;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void PopCreateProjectMenu()
{
    if (showCreateProjectPopup) {
        if (ImGui::BeginPopupModal("Create New Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("Project Name", projectName, IM_ARRAYSIZE(projectName));
            ImGui::InputText("Project Path", projectPath, IM_ARRAYSIZE(projectPath));

            if (ImGui::Button("Create")) {
                std::cout << "INFO: Attempting to create project." << std::endl;
                try {
                    fs::create_directories(projectPath);
                    if (!fs::exists(projectPath)) {
                        std::cerr << "ERROR: Failed to create directory." << std::endl;
                    }
                    else {
                        std::cout << "INFO: Directory created successfully." << std::endl;
                    }
                    currentProject = projectManager.CreateProject(projectName, projectPath);
                    std::cout << "INFO: Project created successfully." << std::endl;
                }
                catch (const std::exception& e) {
                    std::cerr << "ERROR: Exception during directory creation: " << e.what() << std::endl;
                }

                if (!currentProject.name.empty()) {
                    std::cout << "INFO: Closing Create New Project Popup." << std::endl;
                    showCreateProjectPopup = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                showCreateProjectPopup = false;
            }
            ImGui::EndPopup();
        }
    }
}