#pragma once
#include <vector>
#include <functional>
#include <string>
#include "utils/glm_utils.h"
#include "block.h"
#include "breakoutGame.h"
#include <GLFW/glfw3.h>

class ShipEditor
{
public:
    ShipEditor();
    ~ShipEditor();
    void Init(const glm::ivec2& resolution);

    void Update(float deltaTime,
        std::function<void(const std::string&, const glm::mat3&)> render);

    void UpdateResolution(const glm::ivec2& res);

    glm::ivec2 GetResolution() const { return resolution; }

    void MouseMove(glm::vec2 pos);
    void MousePress(glm::vec2 pos, int button);
    void MouseRelease(glm::vec2 pos, int button);



    // Input pentru joc
    void HandleInput(float deltaTime, int key);
    void LaunchBall();

    bool ValidateShip() const;
    std::vector<Block> GetShipBlocks() const { return placedBlocks; }
    float GetCellSize() const { return cellSize; }
    float GetCellSpacing() const { return cellSpacing; }
    bool IsGameStarted() const { return gameStarted; }

private:
    void DrawPalette(std::function<void(const std::string&, const glm::mat3&)> render);
    void DrawGrid(std::function<void(const std::string&, const glm::mat3&)> render);
    void DrawRemaining(std::function<void(const std::string&, const glm::mat3&)> render);
    void DrawStartButton(std::function<void(const std::string&, const glm::mat3&)> render);
    void DrawDraggingBlock(std::function<void(const std::string&, const glm::mat3&)> render);
    bool CheckConnected() const;
    glm::vec2 ScaleMouseCoords(const glm::vec2& mousePos);

private:
    glm::ivec2 resolution;
    glm::vec2 gridOrigin;

    glm::ivec2 virtualResolution;  // Rezolu?ia ini?ialã (fixã)
    glm::ivec2 actualResolution;   // Rezolu?ia realã (curentã)


    glm::ivec2 gridSize;
    int gridSizeX = 17;
    int gridSizeY = 10;
    float cellSize = 45;
    float cellSpacing = 8;

    std::vector<Block> palette;
    std::vector<Block> placedBlocks;

    int maxBlocks = 10;
    int remaining = 10;

    Block* selected = nullptr;
    bool dragging = false;
    glm::vec2 mousePos;

    glm::vec2 startBtnPos;
    glm::vec2 startBtnSize;

    bool gameStarted;
    BreakoutGame* game;
};
