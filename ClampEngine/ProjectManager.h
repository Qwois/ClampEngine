#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include "raylib.h"
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <random>

// Structure to represent a project
struct Project {
    std::string name;
    std::string id;
    std::string path;
};

class ProjectManager {
public:
    ProjectManager() {}

    Project CreateProject(const std::string& name, const std::string& path) {
        // Check if the project already exists
        if (ProjectExists(path)) {
            std::cerr << "Project already exists at this path.\n";
            return {}; // Return an empty project
        }

        Project project;
        project.name = name;
        project.id = GenerateUUID();
        project.path = path;

        SaveProjectConfig(project);

        return project;
    }

    void SaveProjectConfig(const Project& project) {
        nlohmann::json j;
        j["name"] = project.name;
        j["id"] = project.id;
        j["path"] = project.path;

        std::ofstream file(project.path + "/project.clampfile");
        if (file.is_open()) {
            file << j.dump(4);
            file.close();
        }
        else {
            std::cerr << "Could not open file to save project configuration.\n";
        }
    }

    Project LoadProject(const std::string& path) {
        std::ifstream file(path + "/project.clampfile");
        Project project;

        if (file.is_open()) {
            nlohmann::json j;
            file >> j;

            project.name = j["name"].get<std::string>();
            project.id = j["id"].get<std::string>();
            project.path = j["path"].get<std::string>();

            file.close();
        }
        else {
            std::cerr << "Could not open project configuration file.\n";
        }

        return project;
    }

private:
    std::string GenerateUUID() {
        static const char characters[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<int> distribution(0, sizeof(characters) - 2);

        std::string uuid;
        for (int i = 0; i < 16; ++i) {
            uuid += characters[distribution(generator)];
        }

        return uuid;
    }

    bool ProjectExists(const std::string& path) {
        std::ifstream file(path + "/project.clampfile");
        return file.good();
    }
};

#endif
