#include "breakoutGame.h"
#include "lab_m1/tema1/transform2D.h"
#include <iostream>
#include <cmath>

BreakoutGame::BreakoutGame() {}
BreakoutGame::~BreakoutGame() {}

void BreakoutGame::Init(const glm::ivec2& res, const std::vector<Block>& ship,
	int gx, int gy, float cs) {
	resolution = res;
	screenWidth = (float)res.x;
	screenHeight = (float)res.y;
	gridSizeX = gx;
	gridSizeY = gy;
	cellSize = cs;

	//Initializare paleta
	paddle.shipBlocks = ship; // Preia blocurile construite din editor
	paddle.position = glm::vec2(screenWidth / 2.0f, 50.0f); // Pozitie initiala centrata
	paddle.speed = 400.0f;
	UpdatePaddleSizeFromShip();

	//Initializare bila
	ResetBall();

	//Creare caramizi
	CreateBricks();

	lives = 3;
	score = 0;
	gameOver = false;
	gameWon = false;

	// Initializare Text Renderer
	std::string fontPath = PATH_JOIN(window->props.selfDir, "assets", "fonts", "Hack-Bold.ttf");
	textRenderer = new gfxc::TextRenderer(window->props.selfDir, screenWidth, screenHeight);
	textRenderer->Load(fontPath, 24);

	// Initializare stele (fundal animat)
	stars.clear();
	srand(42); // Seed fix pentru pozi?ii consistente
	for (int i = 0; i < 100; i++) { // 100 de stele
		Star s;
		s.position.x = (rand() % (int)screenWidth);
		s.position.y = (rand() % (int)screenHeight);
		s.speed = 50.0f + (rand() % 50); // Viteza aleatorie
		stars.push_back(s);
	}

}
void BreakoutGame::UpdatePaddleSizeFromShip() {
	if (paddle.shipBlocks.empty()) {
		paddle.size = glm::vec2(100.0f, 20.0f); // Dimensiune implicita
		return;
	}

	float blockScale = 0.6f;

	// Calculeaza bounding box-ul navei pe grid (min/max X, Y)
	int minX = INT_MAX, maxX = INT_MIN;
	int minY = INT_MAX, maxY = INT_MIN;

	for (const auto& block : paddle.shipBlocks) {
		int gx = static_cast<int>(block.gridPos.x);
		int gy = static_cast<int>(block.gridPos.y);

		minX = std::min(minX, gx);
		maxX = std::max(maxX, gx);
		minY = std::min(minY, gy);
		maxY = std::max(maxY, gy);
	}

	// Dimensiunea paletei = dimensiunea navei scalate
	paddle.size.x = (maxX - minX + 1) * cellSize * blockScale;
	paddle.size.y = (maxY - minY + 1) * cellSize * blockScale;
}

void BreakoutGame::CreateBricks() {
	bricks.clear();

	int rows = 5;
	int cols = 12;

	float brickWidth = screenWidth / cols;
	float brickHeight = brickWidth * 2.0f / 3.0f;

	float startX = 0.0f;
	float marginTop = 100.0f;
	float startY = screenHeight - marginTop - brickHeight;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			// Seteaza rezistenta aleatorie intre 1 si 5
			int hits = (rand() % 5) + 1;

			float x = startX + j * brickWidth;
			float y = startY - i * brickHeight;

			Brick brick(x, y, hits);
			brick.size = glm::vec2(brickWidth, brickHeight);
			bricks.push_back(brick);
		}
	}
}


void BreakoutGame::ResetBall() {
	// Pozitioneaza bila pe centrul orizontal al paletei, deasupra
	ball.position = paddle.position + glm::vec2(0, paddle.size.y + ball.radius);
	ball.velocity = glm::vec2(0, 0);
	ball.attached = true;
}


void BreakoutGame::ResetRound() {
	// Reseteaza paleta la pozitia de start (centru jos)
	paddle.position = glm::vec2(screenWidth / 2.0f, 50.0f);
	ResetBall();
}


void BreakoutGame::Update(float deltaTime) {
	if (gameOver || gameWon) return;

	//Bila atasata se misca cu paleta
	if (ball.attached) {
		ball.position.x = paddle.position.x;
		ball.position.y = paddle.position.y + paddle.size.y + ball.radius;
	}
	else {
		//Miscare bila
		ball.position += ball.velocity * deltaTime;

		// Coliziuni cu peretii laterali
		if (ball.position.x - ball.radius <= 0) {
			ball.velocity.x = abs(ball.velocity.x);
			ball.position.x = ball.radius;
		}
		if (ball.position.x + ball.radius >= screenWidth) {
			ball.velocity.x = -abs(ball.velocity.x);
			ball.position.x = screenWidth - ball.radius;
		}

		// Coliziuni cu tavanul
		if (ball.position.y + ball.radius >= screenHeight) {
			ball.velocity.y = -abs(ball.velocity.y);
			ball.position.y = screenHeight - ball.radius;
		}

		//Bila cade jos + pierdeere viata
		if (ball.position.y - ball.radius <= 0) {
			lives--;
			
			if (lives <= 0) {
				gameOver = true;
				return;
			}
			ResetRound();
			return;
		}

		CheckCollisions(); // Verifica coliziunile cu paleta si caramizile
	}

	// Animatia fragmentelor de caramida
	for (auto& frag : fragments) {
		frag.position += frag.velocity * deltaTime;
		frag.scale -= 1.5f * deltaTime; // Micsoreaza fragmentul 
		frag.lifetime -= deltaTime;
	}

	// Sterge fragmentele care au disparut
	fragments.erase(std::remove_if(fragments.begin(), fragments.end(),
		[](const BrickFragment& f) { return f.lifetime <= 0.0f || f.scale <= 0.0f; }),
		fragments.end());

	CheckWinCondition();

	// Animatia stelelor (miscare in jos)
	for (auto& star : stars) {
		star.position.y -= star.speed * deltaTime;

		// Resetare daca iese de jos
		if (star.position.y < 0) {
			star.position.y = screenHeight; // Muta inapoi la tavan
			star.position.x = (rand() % (int)screenWidth); // Pozitie X noua
		}
	}
}


void BreakoutGame::CheckCollisions() {
	// COLIZIUNE CU PALETA
	if (CheckBallPaddleCollision(ball)) {
		// Logica coliziunii (Paddle AABB)
		float left = paddle.position.x - paddle.size.x / 2.0f;
		float right = paddle.position.x + paddle.size.x / 2.0f;
		float bottom = paddle.position.y;
		float top = paddle.position.y + paddle.size.y;

		// Gaseste punctul cel mai apropiat din AABB la centrul bilei
		float closestX = glm::clamp(ball.position.x, left, right);
		float closestY = glm::clamp(ball.position.y, bottom, top);

		// Vector de la punctul cel mai apropiat la centrul bilei
		float deltaX = ball.position.x - closestX;
		float deltaY = ball.position.y - closestY;

		// determina tipul coliziunii
		bool isVerticalEdge = (abs(deltaX) > abs(deltaY));

		if (isVerticalEdge) {
			// MARGINE VERTICALA (laterala)
			ball.velocity.x = -ball.velocity.x;

			// Împinge bila afarã lateral
			if (ball.position.x < paddle.position.x) {
				ball.position.x = left - ball.radius;
			}
			else {
				ball.position.x = right + ball.radius;
			}
		}
		else {
			// MARGINE ORIZONTALA (sus)

			// Calculeaza punctul de impact normalizat (-1 la 1) pe X
			float paddleCenter = paddle.position.x;
			float hitPosition = ball.position.x - paddleCenter;
			float normalizedHit = hitPosition / (paddle.size.x / 2.0f);
			normalizedHit = glm::clamp(normalizedHit, -1.0f, 1.0f);

			// Converteste la unghi real (in radiani)
			float maxDeviation = glm::radians(45.0f); 
			float angle = glm::radians(90.0f) - normalizedHit * maxDeviation;

			const float FIXED_SPEED = 300.0f;

			ball.velocity.x = FIXED_SPEED * cos(angle);
			ball.velocity.y = FIXED_SPEED * sin(angle);

			// Impinge bila afara
			ball.position.y = top + ball.radius;
		}
	}

	// COLIZIUNI CU CARAMIZILE
	Brick* closestBrick = nullptr;
	float minDistance = FLT_MAX;

	for (auto& brick : bricks) {
		if (!brick.destroyed && CheckBallBrickCollision(ball, brick)) {
			glm::vec2 brickCenter = glm::vec2(
				brick.position.x + brick.size.x / 2.0f,
				brick.position.y + brick.size.y / 2.0f
			);
			float dist = glm::distance(ball.position, brickCenter);

			if (dist < minDistance) {
				minDistance = dist;
				closestBrick = &brick;
			}
		}
	}

	if (closestBrick != nullptr) {
		float left = closestBrick->position.x;
		float right = closestBrick->position.x + closestBrick->size.x;
		float bottom = closestBrick->position.y;
		float top = closestBrick->position.y + closestBrick->size.y;

		float centerX = (left + right) / 2.0f;
		float centerY = (bottom + top) / 2.0f;

		float deltaX = ball.position.x - centerX;
		float deltaY = ball.position.y - centerY;

		float absDeltaX = abs(deltaX);
		float absDeltaY = abs(deltaY);

		if (absDeltaX / closestBrick->size.x > absDeltaY / closestBrick->size.y) {
			ball.velocity.x = -ball.velocity.x;

			if (deltaX > 0) {
				ball.position.x = right + ball.radius;
			}
			else {
				ball.position.x = left - ball.radius;
			}
		}
		else {
			ball.velocity.y = -ball.velocity.y;

			if (deltaY > 0) {
				ball.position.y = top + ball.radius;
			}
			else {
				ball.position.y = bottom - ball.radius;
			}
		}

		score++;

		std::string oldMeshName;
		if (closestBrick->hits == 5) {
			oldMeshName = "brick_purple";
		}
		else if (closestBrick->hits == 4) {
			oldMeshName = "brick_pink";
		}
		else if (closestBrick->hits == 3) {
			oldMeshName = "brick_red";
		}
		else if (closestBrick->hits == 2) {
			oldMeshName = "brick_orange";
		}
		else {
			oldMeshName = "brick_yellow";
		}

		closestBrick->hits--;  // Scade rezistenta

		if (closestBrick->hits <= 0) {
			closestBrick->destroyed = true;
		}
		else {
			closestBrick->UpdateColor();  // Actualizeaza culoarea
		}

		// Creare fragmente
		for (int i = 0; i < 4; i++) {
			BrickFragment frag;
			frag.position = closestBrick->position;
			frag.velocity = glm::vec2((rand() % 20 - 10) * 10.0f,
				(rand() % 20 - 10) * 10.0f);
			frag.scale = 1.0f;
			frag.lifetime = 0.5f;
			frag.meshName = oldMeshName;
			fragments.push_back(frag);
		}
	}
}


bool BreakoutGame::CheckBallPaddleCollision(const Ball& b) {
	// Calculeaza limitele AABB ale paletei
	float left = paddle.position.x - paddle.size.x / 2.0f;
	float right = paddle.position.x + paddle.size.x / 2.0f;
	float bottom = paddle.position.y;
	float top = paddle.position.y + paddle.size.y;

	// Gaseste cel mai apropiat punct de pe AABB la centrul bilei (Clamping)
	float closestX = glm::clamp(b.position.x, left, right);
	float closestY = glm::clamp(b.position.y, bottom, top);

	// Calculeaza distanta patrata de la centrul bilei la cel mai apropiat punct
	float distX = b.position.x - closestX;
	float distY = b.position.y - closestY;
	float distSquared = distX * distX + distY * distY;

	// Returneaza TRUE daca distanta este mai mica decat raza la patrat (coliziune)
	return distSquared < (b.radius * b.radius);
}

bool BreakoutGame::CheckBallBrickCollision(const Ball& b, const Brick& brick) {
	float left = brick.position.x;
	float right = brick.position.x + brick.size.x;
	float bottom = brick.position.y;
	float top = brick.position.y + brick.size.y;

	float closestX = glm::clamp(b.position.x, left, right);
	float closestY = glm::clamp(b.position.y, bottom, top);

	float distX = b.position.x - closestX;
	float distY = b.position.y - closestY;
	float distSquared = distX * distX + distY * distY;

	return distSquared < (b.radius * b.radius);
}

void BreakoutGame::CheckWinCondition() {
	// Verifica daca toate caramizile au fost distruse
	bool allDestroyed = true;
	for (const auto& brick : bricks) {
		if (!brick.destroyed) {
			allDestroyed = false;
			break;
		}
	}

	// Seteaza starea de joc castigat
	if (allDestroyed) {
		gameWon = true;
	}
}

void BreakoutGame::MoveLeft(float deltaTime) {
	paddle.position.x -= paddle.speed * deltaTime;
	// Limiteaza miscarea la marginea stanga a ecranului
	if (paddle.position.x - paddle.size.x / 2.0f < 0) {
		paddle.position.x = paddle.size.x / 2.0f;
	}
}

void BreakoutGame::MoveRight(float deltaTime) {
	paddle.position.x += paddle.speed * deltaTime;
	if (paddle.position.x + paddle.size.x / 2.0f > screenWidth) {
		paddle.position.x = screenWidth - paddle.size.x / 2.0f;
	}
}

void BreakoutGame::LaunchBall() {
	if (ball.attached) {
		ball.attached = false;
		float speed = 300.0f;

		// Lansarea bilei la un unghi fix de 45 de grade
		float angle = glm::radians(45.0f);
		ball.velocity = glm::vec2(
			speed * cos(angle), 
			speed * sin(angle)
		);
	}
}


void BreakoutGame::Draw(std::function<void(const std::string&, const glm::mat3&)> render) {
	DrawBricks(render);   // Caramizi
	DrawPaddle(render);   // Paleta
	DrawBall(render);     // Bila
	DrawLives(render);    // Inimi (vieti)
	DrawStars(render);    // Fundal cu stele

	// Desenarea fragmentelor (Animatia de spargere)
	for (const auto& frag : fragments) {
		glm::mat3 m = transform2D::Translate(frag.position.x, frag.position.y) *
			transform2D::Scale(frag.scale, frag.scale) *
			transform2D::Translate(-30.0f, -15.0f); // recentrare pe mesh 60x30

		render(frag.meshName, m);
	}

	// Desenare scor
	textRenderer->RenderText("Score: " + std::to_string(score), 20.0f, 30.0f, 1.0f, glm::vec3(1, 1, 1));
}

void BreakoutGame::DrawStars(std::function<void(const std::string&, const glm::mat3&)> render) {
	for (const auto& star : stars) {
		glm::mat3 m = transform2D::Translate(star.position.x, star.position.y);
		render("star", m);
	}
}

void BreakoutGame::DrawPaddle(std::function<void(const std::string&, const glm::mat3&)> render) {
	float blockScale = 0.6f;

	// Calculeaza dimensiunea navei pe grid
	int minX = INT_MAX, maxX = INT_MIN;
	int minY = INT_MAX, maxY = INT_MIN;

	for (const auto& block : paddle.shipBlocks) {
		int gx = static_cast<int>(block.gridPos.x);
		int gy = static_cast<int>(block.gridPos.y);

		minX = std::min(minX, gx);
		maxX = std::max(maxX, gx);
		minY = std::min(minY, gy);
		maxY = std::max(maxY, gy);
	}

	float shipWidth = (maxX - minX + 1) * cellSize * blockScale;
	float shipHeight = (maxY - minY + 1) * cellSize * blockScale;

	// Deseneaza fiecare bloc in parte
	for (const auto& block : paddle.shipBlocks) {
		float offsetX = (block.gridPos.x - minX + 0.5f) * cellSize * blockScale - shipWidth / 2.0f;
		float offsetY = (block.gridPos.y - minY + 0.5f) * cellSize * blockScale;

		float blockX = paddle.position.x + offsetX;
		float blockY = paddle.position.y + offsetY;

		float sx = cellSize * blockScale / 50.0f;
		float sy = cellSize * blockScale / 50.0f;

		glm::mat3 bm = transform2D::Translate(blockX, blockY) *
			transform2D::Scale(sx, sy) *
			transform2D::Translate(-25.0f, -25.0f); // Centrare mesh 50x50
		render(block.meshName, bm);
	}
}

void BreakoutGame::DrawBall(std::function<void(const std::string&, const glm::mat3&)> render) {
	float scale = ball.radius / 20.0f;  // 20 raza mesh-ului original

	glm::mat3 m = transform2D::Translate(ball.position.x, ball.position.y) *
		transform2D::Scale(scale, scale);
	render("ball", m);
}

void BreakoutGame::DrawBricks(std::function<void(const std::string&, const glm::mat3&)> render) {
	for (const auto& brick : bricks) {
		if (!brick.destroyed) {
			// Selecteaza mesh-ul (culoarea) in functie de hits ramase
			std::string meshName;

			if (brick.hits == 5) {
				meshName = "brick_purple";
			}
			else if (brick.hits == 4) {
				meshName = "brick_pink";
			}
			else if (brick.hits == 3) {
				meshName = "brick_red";
			}
			else if (brick.hits == 2) {
				meshName = "brick_orange";
			}
			else { // Hits == 1
				meshName = "brick_yellow";
			}

			// Calculeaza factorul de scalare
			float sx = brick.size.x * brick.scale / 60.0f;  
			float sy = brick.size.y * brick.scale / 25.0f;  

			// Centrul cãrãmizii
			float centerX = brick.position.x + brick.size.x / 2.0f;
			float centerY = brick.position.y + brick.size.y / 2.0f;

			glm::mat3 m = transform2D::Translate(centerX, centerY) *
				transform2D::Scale(sx, sy) *
				transform2D::Translate(-30.0f, -15.0f);  // Recentrare pe mesh 60x30

			render(meshName, m);
		}
	}
}

void BreakoutGame::DrawLives(std::function<void(const std::string&, const glm::mat3&)> render) {
	float heartSize = 25.0f;
	float spacing = 60.0f;
	float startX = screenWidth - 40.0f;  // Pozitie initiala (dreapta-sus)
	float y = screenHeight - 40.0f;

	for (int i = 0; i < lives; i++) {
		glm::mat3 m = transform2D::Translate(startX - i * spacing, y) *
			transform2D::Scale(heartSize / 30.0f, heartSize / 30.0f) *
			transform2D::Translate(-15.0f, -15.0f);
		render("heart", m);
	}
}