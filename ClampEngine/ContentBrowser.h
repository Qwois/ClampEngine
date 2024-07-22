#ifndef CONTENT_BROWSER_H
#define CONTENT_BROWSER_H

#include "imgui.h"
#include <vector>
#include <string>

struct Asset {
    std::string name;
    std::string type;
    bool isSelected;
};

void DrawContentBrowser();

#endif