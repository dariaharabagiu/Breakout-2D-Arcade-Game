#pragma once
#include "components/simple_scene.h"
#include "utils/glm_utils.h"
#include "components/text_renderer.h"
#include "block.h"
#include <vector>
#include <functional>

// Structura pentru bila
struct Ball {
	glm::vec2 position;
	glm::vec2 velocity;
	float radius;
	bool attached; // Daca este pe paleta

	Ball() : position(0, 0), velocity(0, 0), radius(15.0f), attached(true) {}
};

struct BrickFragment {
	glm::vec2 position;
	glm::vec2 velocity;
	float scale;
	float lifetime;
	std::string meshName;
};

// Structura pentru caramida
struct Brick {
	glm::vec2 position;
	glm::vec2 size;
	glm::vec3 color;
	int hits; // Cate lovituri mai trebuie
	int maxHits; // hits initial
	bool destroyed;
	float scale; // Pentru animatia de distrugere

	Brick() : position(0, 0), size(60, 30), hits(1), maxHits(1),
				destroyed(false), scale(1.0f) {}

	Brick(float x, float y, int hitCount)
		: position(x, y), size(60, 30), hits(hitCount), maxHits(hitCount),
		destroyed(false), scale(1.0f) {
		UpdateColor();
	}

	void UpdateColor() {
		// Culori diferite in functie de rezistenta (hits ramase)
		if (hits == 5) {
			color = glm::vec3(0.5f, 0.0f, 1.0f); // Mov
		}
		else if (hits == 4) {
			color = glm::vec3(1.0f, 0.4f, 0.7f); // Roz
		}
		else if (hits == 3) {
			color = glm::vec3(1.0f, 0.0f, 0.0f); // Rosu
		}
		else if (hits == 2) {
			color = glm::vec3(1.0f, 0.5f, 0.0f); // Portocaliu
		}
		else {
			color = glm::vec3(1.0f, 1.0f, 0.0f); // Galben
		}
	}
};

struct Paddle {
	glm::vec2 position;
	glm::vec2 size;
	float speed;
	std::vector<Block> shipBlocks; // Nava

	Paddle() : position(0, 0), size(100, 20), speed(400.0f) {}
};

struct Star {
	glm::vec2 position;
	float speed;
};

class TextRenderer;

class BreakoutGame : public gfxc::SimpleScene {
public:
	BreakoutGame();
	~BreakoutGame();

	void Init(const glm::ivec2& resolution, const std::vector<Block>& ship,
		int gridSizeX, int gridSizeY, float cellSize);
	void Update(float deltaTime);
	void Draw(std::function<void(const std::string&, const glm::mat3&)> render);

	void MoveLeft(float deltaTime);
	void MoveRight(float deltaTime);
	void LaunchBall();

	bool IsGameOver() const { return gameOver;  }
	bool IsGameWon() const { return gameWon; }
	int GetLives() const { return lives; }
	int GetScore() const { return score; }

private:
	void CreateBricks();
	void ResetBall();
	void ResetRound();
	void UpdatePaddleSizeFromShip();
	void CheckCollisions();
	bool CheckBallBrickCollision(const Ball& b, const Brick& brick);
	bool CheckBallPaddleCollision(const Ball& b);
	void CheckWinCondition();

	void DrawPaddle(std::function<void(const std::string&, const glm::mat3&)> render);
	void DrawBall(std::function<void(const std::string&, const glm::mat3&)> render);
	void DrawBricks(std::function<void(const std::string&, const glm::mat3&)> render);
	void DrawLives(std::function<void(const std::string&, const glm::mat3&)> render);
	void DrawStars(std::function<void(const std::string&, const glm::mat3&)> render);

	gfxc::TextRenderer* textRenderer;

	glm::ivec2 resolution;
	Paddle paddle;
	Ball ball;
	std::vector<Brick> bricks;
	std::vector<BrickFragment> fragments;
	std::vector<Star> stars;

	int lives;
	int score;
	bool gameOver;
	bool gameWon;

	// Limite ecran
	float screenWidth;
	float screenHeight;

	int gridSizeX;
	int gridSizeY;
	float cellSize;
};