#pragma once
#include "utils/glm_utils.h"
#include "core/gpu/mesh.h"
#include <string>
#include <vector>

namespace object2D
{
    Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateStartButton(const std::string& name, glm::vec3 color);

    // Mesh-uri pentru joc
    Mesh* CreateBrick(const std::string& name, glm::vec3 color);
    Mesh* CreateStar(const std::string& name);
    Mesh* CreateHeart(const std::string& name);
    Mesh* CreateBall(const std::string& name);
}