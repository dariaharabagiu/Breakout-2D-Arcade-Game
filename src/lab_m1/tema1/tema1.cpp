#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/tema1/transform2D.h"
#include "lab_m1/tema1/object2D.h"
#include "lab_m1/tema1/shipEditor.h"


using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);


    float size = editor.GetCellSize() + editor.GetCellSpacing();
    float frameHeight = resolution.y / 4.0f;

    AddMeshToList(object2D::CreateSquare("metal", glm::vec3(0), size, glm::vec3(0.6f, 0.6f, 0.65f), true));
    AddMeshToList(object2D::CreateSquare("palette_frame", glm::vec3(0), frameHeight, glm::vec3(1, 0, 0), false));
    AddMeshToList(object2D::CreateSquare("grid_frame", glm::vec3(0), 1.0f, glm::vec3(0.2f, 0.4f, 1.0f), false));
    AddMeshToList(object2D::CreateSquare("cell", glm::vec3(0), 50, glm::vec3(0.2f, 0.4f, 1.0f), false));
    AddMeshToList(object2D::CreateSquare("cell_filled", glm::vec3(0), 50, glm::vec3(0.2f, 0.4f, 1.0f), true));
    AddMeshToList(object2D::CreateSquare("parts", glm::vec3(0), 120, glm::vec3(0.0f, 1.0f, 0.0f), true));
    AddMeshToList(object2D::CreateStartButton("start_button_green", glm::vec3(0, 1, 0)));
    AddMeshToList(object2D::CreateStartButton("start_button_red", glm::vec3(1, 0, 0)));

    // Mesh-uri pentru joc
    AddMeshToList(object2D::CreateBrick("brick_red", glm::vec3(1.0f, 0.0f, 0.0f)));       // Rosu
    AddMeshToList(object2D::CreateBrick("brick_orange", glm::vec3(1.0f, 0.5f, 0.0f)));    // Portocaliu
    AddMeshToList(object2D::CreateBrick("brick_yellow", glm::vec3(1.0f, 1.0f, 0.0f)));    // Galben
    AddMeshToList(object2D::CreateBrick("brick_pink", glm::vec3(1.0f, 0.4f, 0.7f)));      // Roz
    AddMeshToList(object2D::CreateBrick("brick_purple", glm::vec3(0.5f, 0.0f, 1.0f)));    // Mov
    AddMeshToList(object2D::CreateStar("star"));
    AddMeshToList(object2D::CreateHeart("heart"));
    AddMeshToList(object2D::CreateBall("ball"));

    editor.Init(resolution); // Initializeaza ShipEditor cu rezolutia initiala
}


void Tema1::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    editor.Update(deltaTimeSeconds, [&](const std::string& mesh, const glm::mat3& model)
        {
            if (meshes.count(mesh))
                RenderMesh2D(meshes.at(mesh), shaders["VertexColor"], model);
        });
}


void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    // Input continuu pentru miscarea paletei
    if (editor.IsGameStarted()) {
        if (window->KeyHold(GLFW_KEY_LEFT)) {
            editor.HandleInput(deltaTime, GLFW_KEY_LEFT);
        }
        if (window->KeyHold(GLFW_KEY_RIGHT)) {
            editor.HandleInput(deltaTime, GLFW_KEY_RIGHT);
        }
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
    // Add key press event
    // Lansare bila cu SPACE
    if (key == GLFW_KEY_SPACE && editor.IsGameStarted()) {
        editor.LaunchBall();
    }

    // ESC pentru a reveni la editor
    if (key == GLFW_KEY_ESCAPE && editor.IsGameStarted()) {
        cout << "Reset la editor" << endl;
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    glm::vec2 p(mouseX, window->GetResolution().y - mouseY);
    editor.MouseMove(p);
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    glm::vec2 p(mouseX, window->GetResolution().y - mouseY);
    editor.MousePress(p, button);
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
    glm::vec2 p(mouseX, window->GetResolution().y - mouseY);
    editor.MouseRelease(p, button);
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
    // actualizeaza rezolutia editorului
    editor.UpdateResolution(glm::ivec2(width, height));
}
