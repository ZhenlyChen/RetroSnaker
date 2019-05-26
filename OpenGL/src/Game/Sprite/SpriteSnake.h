#ifndef SNAKE_H
#define SNAKE_H

#include <glm/glm.hpp>

#include "../ResourceManager/ResourceManager.h"
#include "../SpriteRenderer/SpriteRenderer.h"
#include "../Util/Util.h"
#include "../AI/SmartSnake.h"

enum SnakeDir { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

class SpriteSnake  {
public:
  SpriteSnake(SpriteRenderer* renderer, int maxX, int maxY);
  void Render(float p);
  void Reset();
  void Update(bool run, bool ai = false);
  bool Over();
  int GetLenght();

  void SetHeadStyle(int style);
  void SetBodyStyle(int style);

  void SetNextDir(SnakeDir dir);


private:
  void renderFood(float p, int style, glm::vec2 pos, float height, float size);
  void newFood();
  void updateDir(bool ai = false);

  int mapMaxX;
  int mapMaxY;

  SpriteRenderer* renderer;
  SnakeDir nextDir;

  bool isOver = false;
  glm::vec3 snakePos[300];
  glm::vec3 snakePosOld[300];
  int snakeLength;
  int styleHead = 0;
  int styleBody = 0;

  glm::vec2 foodPos;
  glm::vec2 foodPosOld;
  int styleFood = 0;
  int styleFoodOld = 0;

  // 智能蛇
  SmartSnake* smartSnake;
};

#endif // !SNAKE_H
