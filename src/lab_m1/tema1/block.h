#pragma once
#include <string>
#include "utils/glm_utils.h"

enum class BlockType {
    Solid // Blocul de baza folosit pentru construirea paletei
};

class Block
{
public:
    Block() {}
    Block(BlockType t, glm::vec2 gridPos, const std::string& mesh)
        : type(t), gridPos(gridPos), meshName(mesh) {
    }

    BlockType type;
    glm::vec2 gridPos;
    std::string meshName;
};