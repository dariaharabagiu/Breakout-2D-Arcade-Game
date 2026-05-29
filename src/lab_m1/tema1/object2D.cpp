#include "lab_m1/tema1/object2D.h"
#include <vector>
#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2D::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreateStartButton(const std::string& name, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));           
    vertices.push_back(VertexFormat(glm::vec3(0, 60, 0), color));         
    vertices.push_back(VertexFormat(glm::vec3(60, 60, 0), color));
    vertices.push_back(VertexFormat(glm::vec3(60, 0, 0), color));

    indices = {
        0, 1, 2,
        3, 0, 1
    };

    Mesh* button = new Mesh(name);
    button->SetDrawMode(GL_TRIANGLES);
    button->InitFromData(vertices, indices);
    return button;
}


Mesh* object2D::CreateBrick(const std::string& name, glm::vec3 color)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float w = 60.0f;
    float h = 25.0f;
    float border = 0.5f;

    glm::vec3 borderColor = glm::vec3(0.0f, 0.0f, 0.0f); // Chenar negru

    // Corp principal colorat
    vertices.push_back(VertexFormat(glm::vec3(border, border, 0), color));
    vertices.push_back(VertexFormat(glm::vec3(w - border, border, 0), color));
    vertices.push_back(VertexFormat(glm::vec3(w - border, h - border, 0), color));
    vertices.push_back(VertexFormat(glm::vec3(border, h - border, 0), color));
    indices.insert(indices.end(), { 0, 1, 2, 0, 2, 3 });

    // Chenar exterior (4 dreptunghiuri)
    unsigned int base = vertices.size();

    // Chenar jos
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(w, 0, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(w, border, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(0, border, 0), borderColor));
    indices.insert(indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });

    // Chenar sus
    base = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(0, h - border, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(w, h - border, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(w, h, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(0, h, 0), borderColor));
    indices.insert(indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });

    // Chenar stanga
    base = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(border, 0, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(border, h, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(0, h, 0), borderColor));
    indices.insert(indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });

    // Chenar dreapta
    base = vertices.size();
    vertices.push_back(VertexFormat(glm::vec3(w - border, 0, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(w, 0, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(w, h, 0), borderColor));
    vertices.push_back(VertexFormat(glm::vec3(w - border, h, 0), borderColor));
    indices.insert(indices.end(), { base, base + 1, base + 2, base, base + 2, base + 3 });

    Mesh* mesh = new Mesh(name);
    mesh->SetDrawMode(GL_TRIANGLES);
    mesh->InitFromData(vertices, indices);
    return mesh;
}


Mesh* object2D::CreateStar(const std::string& name)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    float size = 2.0f;

    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), white));
    vertices.push_back(VertexFormat(glm::vec3(size, 0, 0), white));
    vertices.push_back(VertexFormat(glm::vec3(size, size, 0), white));
    vertices.push_back(VertexFormat(glm::vec3(0, size, 0), white));
    indices.insert(indices.end(), { 0, 1, 2, 0, 2, 3 });

    Mesh* mesh = new Mesh(name);
    mesh->SetDrawMode(GL_TRIANGLES);
    mesh->InitFromData(vertices, indices);
    return mesh;
}

Mesh* object2D::CreateHeart(const std::string& name)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 red = glm::vec3(1.0f, 0.0f, 0.0f);

    float r = 15.0f;               // raza semicercurilor
    int steps = 20;                // precizie semicercuri
    float h = 2.0f * r;            // inaltimea triunghiului

    glm::vec2 leftCenter = glm::vec2(r, r);
    glm::vec2 rightCenter = glm::vec2(3 * r, r);
    glm::vec2 tip = glm::vec2(2 * r, -h);

    for (int i = 0; i <= steps; i++) {
        float angle = i * glm::pi<float>() / steps;
        float x = leftCenter.x + r * cos(angle);
        float y = leftCenter.y + r * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), red));
    }

    for (int i = 0; i <= steps; i++) {
        float angle = i * glm::pi<float>() / steps;
        float x = rightCenter.x + r * cos(angle);
        float y = rightCenter.y + r * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), red));
    }

    // Varful triunghiului
    vertices.push_back(VertexFormat(glm::vec3(tip.x, tip.y, 0), red));
    unsigned int tipIndex = (unsigned int)vertices.size() - 1;

    //Triangulare: fan din varf spre fiecare pereche de puncte
    for (unsigned int i = 0; i < vertices.size() - 1; i++) {
        indices.push_back(tipIndex);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    Mesh* mesh = new Mesh(name);
    mesh->SetDrawMode(GL_TRIANGLES);
    mesh->InitFromData(vertices, indices);
    return mesh;
}

Mesh* object2D::CreateBall(const std::string& name)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);  // alb
    float radius = 20.0f;                           // dimensiune implicita
    int segments = 40;                              
    // Centru
    vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), color));
    unsigned int centerIndex = 0;

    // Puncte pe cerc
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * glm::pi<float>() * i / segments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));

        if (i > 0) {
            indices.push_back(centerIndex);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }

    Mesh* mesh = new Mesh(name);
    mesh->SetDrawMode(GL_TRIANGLES);
    mesh->InitFromData(vertices, indices);
    return mesh;
}
