#ifndef SNAKE_H
#define SNAKE_H

#include <glm/glm.hpp>

#include "../ResourceManager/ResourceManager.h"
#include "../SpriteRenderer/SpriteRenderer.h"
#include "../Util/Util.h"
#include "../AI/SmartSnake.h"

enum SnakeDir { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

const int MoveX[4] = { 0 , 0, -1, 1 };
const int MoveY[4] = { -1, 1, 0, 0 };
const float Rotate[4] = { 1.57, 4.71, 3.14, 0 };


class SpriteSnake  {
public:
  SpriteSnake(SpriteRenderer* renderer, int maxX, int maxY) {

    this->renderer = renderer;
    this->snakeLength = 0;
    this->snakePos[0] = glm::vec3(0, 0, 0);
    this->snakePos[1] = glm::vec3(0, 0, 0);
    this->snakePosOld[0] = glm::vec3(0, 0, 0);
    this->snakePosOld[1] = glm::vec3(0, 0, 0);

    // 蛇头
    ResourceManager::LoadModel("resources/objects/snake/Snake_Head.obj", "snake");
    ResourceManager::LoadModel("resources/objects/snake/duck.obj", "duck");
    ResourceManager::LoadModel("resources/objects/snake/chicken.obj", "chicken");
    // 蛇身
    ResourceManager::LoadModel("resources/objects/snake/body.obj", "carBody");
    ResourceManager::LoadModel("resources/objects/snake/snake_body.obj", "body");
  }


  void Render(float p, bool isMe) {
    glm::vec3 newPos;
    // 渲染蛇头
    if (isMe) {
      this->styleHead = 1;
      this->styleBody = 1;
    }
    else {
      this->styleHead = 2;
      this->styleBody = 2;
    }

    if (this->snakeLength > 0) {
      newPos = getMiddlePos(snakePosOld[0], snakePos[0], p);
      switch (this->styleHead) {
      case 1:
        this->renderer->DrawSprite(ResourceManager::GetModel("snake"),
          glm::vec3(newPos.x, 0, newPos.y),
          glm::vec3(5.5f, 5.5f, 5.5f),
          newPos.z);
        break;
      case 2:
        this->renderer->DrawSprite(ResourceManager::GetModel("duck"),
          glm::vec3(newPos.x, 0, newPos.y),
          glm::vec3(0.28f, 0.28f, 0.28f),
          newPos.z);
        break;
      default:
        this->renderer->DrawSprite(ResourceManager::GetModel("chicken"),
          glm::vec3(newPos.x, 0, newPos.y),
          glm::vec3(0.041f, 0.041f, 0.041f),
          newPos.z);
        break;
      }
    }
    // 渲染蛇身
    for (int i = 1; i < snakeLength; i++) {
      newPos = getMiddlePos(snakePosOld[i], snakePos[i], p);
      switch (this->styleBody) {
      case 1:
        this->renderer->DrawSprite(ResourceManager::GetModel("body"),
          glm::vec3(newPos.x, 0, newPos.y),
          glm::vec3(2.0f, 2.0f, 2.0f),
          newPos.z);
        break;
      case 2:
        this->renderer->DrawSprite(ResourceManager::GetModel("ball"),
          glm::vec3(newPos.x, 0, newPos.y),
          glm::vec3(1, 1, 1),
          newPos.z);
        break;
      default:
        this->renderer->DrawSprite(ResourceManager::GetModel("carBody"),
          glm::vec3(newPos.x, 0, newPos.y),
          glm::vec3(0.27f, 0.27f, 0.27f),
          newPos.z);
        break;
      }
    }
  }
  void Update(bool tail = false) {
    if (tail) {
      snakePosOld[snakeLength - 1] = snakePos[snakeLength - 1];
      return;
    }
    for (int i = 0; i < this->snakeLength; i++) {
      snakePosOld[i] = snakePos[i];
    }
  }

  void SetData(vector<glm::vec3> pos, int length, int styleHead, int styleBody) {
    this->snakeLength = length;
    this->styleBody = styleBody;
    this->styleHead = styleHead;
    int i = 0;
    for (auto p : pos) {
      this->snakePos[i++] = p;
    }
  }

  int GetLenght() {
    return this->snakeLength;
  }

  SnakeDir Dir;
  bool First = true;

private:

  // 计算蛇的中间插值位置
  glm::vec3 getMiddlePos(glm::vec3 oldValue, glm::vec3 newValue, float p) {
    float x = oldValue.x;
    float y = oldValue.y;
    float z = Rotate[(int)oldValue.z];
    if (newValue.x != x || newValue.y != y) {
      x += (newValue.x - x) * p;
      y += (newValue.y - y) * p;
      float stepZ = Rotate[(int)newValue.z] - z;
      if (stepZ > 3.5) {
        z += (Rotate[(int)newValue.z] - 6.28) * p;
      }
      else if (stepZ < -3.5) {
        z += (6.28 - z) * p;
      }
      else {
        z += (Rotate[(int)newValue.z] - z) * p;
      }
    }
    return glm::vec3(x, y, z);
  }

  SpriteRenderer* renderer;

  glm::vec3 snakePos[300];
  glm::vec3 snakePosOld[300];
  int snakeLength;
  int styleHead = 0;
  int styleBody = 0;


};

#endif // !SNAKE_H
