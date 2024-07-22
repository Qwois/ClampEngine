#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include "raylib.h"
#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <windows.h>  // Include for UUID generation on Windows
#include <rpc.h>      // Include for CoCreateGuid and UuidToString

struct Project {
    std::string name;
    std::string id;
    std::string path;
};

class ProjectManager {
public:
    ProjectManager() {}

    Project CreateProject(const std::string& name, const std::string& path) {
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
        UUID uuid;
        UuidCreate(&uuid);

        RPC_CSTR uuidStr;
        UuidToStringA(&uuid, &uuidStr);

        std::string result(reinterpret_cast<char*>(uuidStr));

        RpcStringFreeA(&uuidStr);
        return result;
    }
};

#endif