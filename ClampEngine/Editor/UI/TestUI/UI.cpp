#include "UI.h"
#include <raylib.h>
#include "../../ClampEngine/ClampEngine.h"
#include "../../Scene/Scene.h"
#include "../../../Editor/Globals/Globals.h"

void DrawTabs() {
    const int tabWidth = 130;
    const int tabHeight = 30;
    const int closeButtonSize = 15;

    for (int i = 0; i < scenes.size(); i++) {
        Rectangle tab = { static_cast<float>(i * tabWidth), 0, static_cast<float>(tabWidth), static_cast<float>(tabHeight) };
        Rectangle closeButton = { tab.x + tabWidth - closeButtonSize - 5, tab.y + (tabHeight - closeButtonSize) / 2, static_cast<float>(closeButtonSize), static_cast<float>(closeButtonSize) };

        if (CheckCollisionPointRec(GetMousePosition(), closeButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (scenes[i].modelLoaded) {
                UnloadModel(scenes[i].model);
            }
            if (scenes[i].textureLoaded) {
                UnloadTexture(scenes[i].texture);
            }
            scenes.erase(scenes.begin() + i);
            if (scenes.empty()) {
                currentSceneIndex = -1;
            }
            else if (currentSceneIndex >= i) {
                currentSceneIndex = std::max(0, currentSceneIndex - 1);
            }
            break;
        }

        if (CheckCollisionPointRec(GetMousePosition(), tab) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(GetMousePosition(), closeButton)) {
            currentSceneIndex = i;
        }

        DrawRectangleRec(tab, (currentSceneIndex == i) ? LIGHTGRAY : GRAY);
        DrawText(TextFormat("Scene %d", i + 1), tab.x + 10, tab.y + 5, 20, BLACK);

        DrawRectangleLinesEx(closeButton, 1, RED);
        DrawLine(closeButton.x, closeButton.y, closeButton.x + closeButton.width, closeButton.y + closeButton.height, RED);
        DrawLine(closeButton.x + closeButton.width, closeButton.y, closeButton.x, closeButton.y + closeButton.height, RED);

        DrawLine(tab.x + tabWidth, tab.y, tab.x + tab.width, tab.y + tab.height, BLACK);
    }

    Rectangle newTab = { static_cast<float>(scenes.size() * tabWidth), 0, static_cast<float>(tabWidth), static_cast<float>(tabHeight) };

    DrawRectangleRec(newTab, BEIGE);
    DrawText("  +", newTab.x + 40, newTab.y + 5, 20, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), newTab) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        CreateNewScene();
    }
}
