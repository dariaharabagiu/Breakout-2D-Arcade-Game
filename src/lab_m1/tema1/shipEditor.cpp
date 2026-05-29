#include "ShipEditor.h"
#include "lab_m1/tema1/transform2D.h"
#include <queue>
#include <iostream>


ShipEditor::ShipEditor() {
    gameStarted = false;
    game = nullptr;
}

ShipEditor::~ShipEditor() {
    if (game) delete game;
}

void ShipEditor::Init(const glm::ivec2& res)
{
    resolution = res;
    virtualResolution = res; // Salveaza rezolutia initiala (pentru mouse scaling)
    actualResolution = res; // Rezolutia curenta

    // Initializare dimensiuni grid
    gridSizeX = 17;
    gridSizeY = 10;
    cellSize = 45;
    cellSpacing = 8;

    maxBlocks = 10;
    remaining = 10;

    float totalGridWidth = gridSizeX * (cellSize + cellSpacing) - cellSpacing;
    float totalGridHeight = gridSizeY * (cellSize + cellSpacing) - cellSpacing;

    float frameHeight = resolution.y / 4.0f;
    float paletteWidth = (frameHeight - cellSize) / 2.0f + cellSize;

    float usableWidth = resolution.x - paletteWidth;

    gridOrigin = glm::vec2(
        paletteWidth + (usableWidth - totalGridWidth) / 2.0f,
        resolution.y * 0.05f
    );

    float commonY = resolution.y - 80.0f;

    startBtnSize = glm::vec2(50, 50);
    startBtnPos = glm::vec2(0.0f, commonY);

    // Palette de blocuri (doar blocul Solid)
    palette = {
        Block(BlockType::Solid, glm::vec2(0), "metal")
    };

    gameStarted = false;
    game = new BreakoutGame();
}


void ShipEditor::UpdateResolution(const glm::ivec2& res) {
    // Actualizeaza rezolutia reala (curenta) a ferestrei
    actualResolution = res;
}

glm::vec2 ShipEditor::ScaleMouseCoords(const glm::vec2& mousePos) {
    // Calculeaza factorul de scalare pentru a corecta pozitia mouse-ului
    float scaleX = (float)virtualResolution.x / (float)actualResolution.x;
    float scaleY = (float)virtualResolution.y / (float)actualResolution.y;

    return glm::vec2(mousePos.x * scaleX, mousePos.y * scaleY);
}

void ShipEditor::Update(
    float dt,
    std::function<void(const std::string&, const glm::mat3&)> render)
{
    if (gameStarted)
    {
        // MOD JOC
        game->Update(dt);
        game->Draw(render);

        if (game->IsGameOver()) {
            std::cout << "Press ESC to return to editor" << std::endl;
        }
        if (game->IsGameWon()) {
            std::cout << "Press ESC to return to editor" << std::endl;
        }
    }
    else
    {
        // MOD EDITOR
        DrawDraggingBlock(render);
        DrawPalette(render);
        DrawRemaining(render);
        DrawStartButton(render);

        for (auto& b : placedBlocks)
        {
            glm::vec2 pos = gridOrigin + glm::vec2(
                b.gridPos.x * (cellSize + cellSpacing),
                b.gridPos.y * (cellSize + cellSpacing)
            );

            glm::mat3 m = transform2D::Translate(
                pos.x - cellSpacing / 2.0f,
                pos.y - cellSpacing / 2.0f
            );
            render(b.meshName, m);
        }
        DrawGrid(render);
    }
}

void ShipEditor::MouseMove(glm::vec2 pos)
{
    mousePos = ScaleMouseCoords(pos);
}

void ShipEditor::HandleInput(float deltaTime, int key) {
    if (!gameStarted) return;

    if (key == GLFW_KEY_LEFT) {
        game->MoveLeft(deltaTime);
    }
    if (key == GLFW_KEY_RIGHT) {
        game->MoveRight(deltaTime);
    }
}

void ShipEditor::LaunchBall() {
    if (gameStarted) {
        game->LaunchBall();
    }
}

void ShipEditor::MousePress(glm::vec2 pos, int button)
{
    if (gameStarted) return;  // Nu procesam mouse in timpul jocului

    pos = ScaleMouseCoords(pos);
    mousePos = pos;

    if (button == 1) // Click stanga (Selectie si Start)
    {
        // Logica de selectie a blocului din paleta
        float paletteX = 0.0f;
        float frameHeight = resolution.y / 4.0f;

        for (int i = palette.size() - 1; i >= 0; i--)
        {
            float remainingHeight = resolution.y - frameHeight;
            float y = remainingHeight / 2.0f;
            float bw = cellSize;
            float bh = cellSize;

            float offsetX = (frameHeight - bw) / 2.0f;
            float offsetY = (frameHeight - bh) / 2.0f;

            float bx = paletteX + offsetX;
            float by = y + offsetY;

            // Daca mouse-ul este in interiorul blocului
            if (pos.x >= bx && pos.x <= bx + bw &&
                pos.y >= by && pos.y <= by + bh)
            {
                selected = new Block(palette[i].type, glm::vec2(0), palette[i].meshName);
                dragging = true;
                return;
            }
        }

        // Verifica butonul START
        if (pos.x >= startBtnPos.x &&
            pos.x <= startBtnPos.x + startBtnSize.x &&
            pos.y >= startBtnPos.y &&
            pos.y <= startBtnPos.y + startBtnSize.y)
        {
            if (ValidateShip())
            {
                std::cout << "Ship OK - Starting game!\n";
                // Initializam jocul
                game->Init(resolution, placedBlocks, gridSizeX, gridSizeY, cellSize);
                gameStarted = true;
            }
            else
            {
                std::cout << "Ship INVALID.\n";
            }
            return;
        }
    }

    // Click dreapta (Stergere bloc)
    if (button == 2)
    {
        glm::vec2 local = pos - gridOrigin;
        int gx = local.x / (cellSize + cellSpacing);
        int gy = local.y / (cellSize + cellSpacing);

        // Cauta blocul in vectorul de blocuri plasate (placedBlocks)
        for (int i = 0; i < placedBlocks.size(); i++)
        {
            if ((int)placedBlocks[i].gridPos.x == gx &&
                (int)placedBlocks[i].gridPos.y == gy)
            {
                placedBlocks.erase(placedBlocks.begin() + i); // Sterge blocul
                remaining++; // Creste contorul de blocuri disponibile
                std::cout << "Bloc sters. Raman: " << remaining << "\n";
                return;
            }
        }
    }
}


void ShipEditor::MouseRelease(glm::vec2 pos, int button)
{
    if (button != 1 || !dragging || !selected)
        return;

    dragging = false;

    pos = ScaleMouseCoords(pos);

    glm::vec2 local = pos - gridOrigin;

    int gx = local.x / (cellSize + cellSpacing);
    int gy = local.y / (cellSize + cellSpacing);

    bool valid = (gx >= 0 && gx < gridSizeX && gy >= 0 && gy < gridSizeY);

    if (valid)
    {
        // Verifica daca celula este deja ocupata
        for (auto& b : placedBlocks)
            if ((int)b.gridPos.x == gx && (int)b.gridPos.y == gy)
                valid = false;
        // Daca celula este valida si libera:
        if (valid)
        {
            // Finalizeaza plasarea blocului
            selected->gridPos = glm::vec2(gx, gy);
            placedBlocks.push_back(*selected);
            remaining--; // Scade numarul de blocuri disponibile
            std::cout << "Bloc plasat la (" << gx << ", " << gy << "). Raman: " << remaining << "\n";
        }
    }

    // Elibereaza memoria alocata pentru blocul tarat
    delete selected;
    selected = nullptr;
}

bool ShipEditor::CheckConnected() const
{
    // Verifica daca nava este conexa folosind BFS
    if (placedBlocks.empty()) return false;

    std::queue<int> q;
    std::vector<bool> visited(placedBlocks.size(), false);

    q.push(0);
    visited[0] = true;
    int count = 1;

    while (!q.empty())
    {
        int i = q.front(); q.pop();

        for (int j = 0; j < placedBlocks.size(); j++)
        {
            if (!visited[j])
            {
                glm::vec2 a = placedBlocks[i].gridPos;
                glm::vec2 b = placedBlocks[j].gridPos;

                if (glm::distance(a, b) < 1.1f)
                {
                    visited[j] = true;
                    q.push(j);
                    count++;
                }
            }
        }
    }

    return count == placedBlocks.size();
}

bool ShipEditor::ValidateShip() const
{
    if (placedBlocks.empty()) {
        return false;
    }

    // Inaltimea maxima (Max 3 blocuri pe Y)
    if (!placedBlocks.empty())
    {
        int minY = INT_MAX, maxY = INT_MIN;

        for (const auto& block : placedBlocks)
        {
            int gy = (int)block.gridPos.y;
            minY = std::min(minY, gy);
            maxY = std::max(maxY, gy);
        }

        // Daca inaltimea totala depaseste 3 celule, este nevalid
        if ((maxY - minY + 1) > 3)
        {
            return false;
        }
    }

    if (placedBlocks.size() > 10) {
        return false;
    }

    if (!CheckConnected()) {
        return false;
    }

    // Daca trece de toate constrangerile, nava este valida
    return true;
}


void ShipEditor::DrawPalette(
    std::function<void(const std::string&, const glm::mat3&)> render)
{
    float paletteX = 0;
    float frameHeight = resolution.y / 4.0f;

    for (int i = 0; i < palette.size(); i++)
    {
        
        float remainingHeight = resolution.y - frameHeight;
        float y = remainingHeight / 2.0f;

        glm::vec2 pos(paletteX, y);

        // Offset pentru a centra blocul in interiorul ramei
        float offsetX = (frameHeight - cellSize) / 2.0f;
        float offsetY = (frameHeight - cellSize) / 2.0f;

        glm::mat3 m = transform2D::Translate(pos.x + offsetX, pos.y + offsetY);
        render(palette[i].meshName, m);

        // Desenare Rama Paleta
        glm::mat3 frame = transform2D::Translate(pos.x, pos.y);
        render("palette_frame", frame);
    }
}



void ShipEditor::DrawGrid(
    std::function<void(const std::string&, const glm::mat3&)> render)
{
    float margin = cellSize * 0.5f;
    float gridWidth = gridSizeX * (cellSize + cellSpacing) - cellSpacing + 2 * margin;
    float gridHeight = gridSizeY * (cellSize + cellSpacing) - cellSpacing + 2 * margin;

    // Desenam cadrul grilei
    glm::mat3 frame = transform2D::Translate(gridOrigin.x - margin, gridOrigin.y - margin) *
        transform2D::Scale(gridWidth, gridHeight);
    render("grid_frame", frame);

    for (int x = 0; x < gridSizeX; x++)
    {
        for (int y = 0; y < gridSizeY; y++)
        {
            glm::vec2 pos = gridOrigin + glm::vec2(
                x * (cellSize + cellSpacing),
                y * (cellSize + cellSpacing)
            );

            glm::mat3 m = transform2D::Translate(pos.x, pos.y) *
                transform2D::Scale(cellSize / 50.0f, cellSize / 50.0f);

            // Desenam celulele
            render("cell_filled", m);
        }
    }
}

void ShipEditor::DrawRemaining(
    std::function<void(const std::string&, const glm::mat3&)> render)
{
    float size = 50.0f; // Dimensiunea unui bloc indicator
    float gap = 40.0f;  // Spatiul dintre blocuri

    // Pozitia Y (comuna cu butonul Start)
    float y = resolution.y - 80.0f;

    // Recalculeaza latimea disponibila pentru centrare orizontala
    float paletteWidth = 0;
    float usableWidth = resolution.x - paletteWidth;

    // Calculul dimensiunii totale a grupului
    float indicatorsWidth = maxBlocks * size + (maxBlocks - 1) * gap;
    float groupWidth = indicatorsWidth + gap + startBtnSize.x;

    // Calculeaza punctul de start X pentru a centra intreg grupul
    float startX = (usableWidth - groupWidth) / 2.0f;
    float endX = startX + indicatorsWidth;

    // Desenam blocurile
    for (int i = 0; i < remaining; i++)
    {
        float currentBlockLeftX = startX + i * (size + gap);

        glm::mat3 m = transform2D::Translate(currentBlockLeftX, y) *
            transform2D::Scale(size / 120.0f, size / 120.0f);

        render("parts", m);
    }

    // Actualizeaza pozitia butonului Start (la dreapta grupului de indicatoare)
    startBtnPos.x = endX + gap; 
    startBtnPos.y = y; 
}

void ShipEditor::DrawStartButton(
    std::function<void(const std::string&, const glm::mat3&)> render)
{
    glm::mat3 m = transform2D::Translate(startBtnPos.x, startBtnPos.y) *
        transform2D::Scale(startBtnSize.x / 60.0f, startBtnSize.y / 60.0f);

    // Logica de validare a culorii
    std::string meshName = ValidateShip() ? "start_button_green" : "start_button_red";

    render(meshName, m);
}


void ShipEditor::DrawDraggingBlock(std::function<void(const std::string&, const glm::mat3&)> render)
{
    if (dragging && selected)
    {
        float halfSize = cellSize * 0.5f;

        glm::mat3 m = transform2D::Translate(
            mousePos.x - halfSize,
            mousePos.y - halfSize
        );

        render(selected->meshName, m);
    }
}