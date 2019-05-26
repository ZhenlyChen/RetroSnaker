#include "SpriteSnake.h"

const int MoveX[4] = { 0 , 0, -1, 1 };
const int MoveY[4] = { -1, 1, 0, 0 };
const float Rotate[4] = { 1.57, 4.71, 3.14, 0 };

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

SpriteSnake::SpriteSnake(SpriteRenderer* renderer, int mX, int mY) {
  this->renderer = renderer;
  this->mapMaxX = mX;
  this->mapMaxY = mY;
  this->snakeLength = 0;
  this->snakePos[0] = glm::vec3(0, 0, 0);
  this->snakePos[1] = glm::vec3(0, 0, 0);
  this->snakePosOld[0] = glm::vec3(0, 0, 0);
  this->snakePosOld[1] = glm::vec3(0, 0, 0);
  this->foodPos = glm::vec2(0, 0);
  this->foodPosOld = glm::vec2(0, 0);
  this->smartSnake = new SmartSnake(mX * 2 + 1, mY * 2 + 1);

  // 食物
  ResourceManager::LoadModel("resources/objects/food/Hamburger.obj", "food");
  ResourceManager::LoadModel("resources/objects/food/ball.obj", "ball");
  ResourceManager::LoadModel("resources/objects/food/lemon.obj", "lemon");
  // 蛇头
  ResourceManager::LoadModel("resources/objects/snake/Snake_Head.obj", "snake");
  ResourceManager::LoadModel("resources/objects/snake/duck.obj", "duck");
  ResourceManager::LoadModel("resources/objects/snake/chicken.obj", "chicken");
  // 蛇身
  ResourceManager::LoadModel("resources/objects/snake/body.obj", "carBody");
  ResourceManager::LoadModel("resources/objects/snake/snake_body.obj", "body");
}


void SpriteSnake::Render( float currentP) {
  glm::vec3 newPos;
  // 渲染蛇头
  if (this->snakeLength > 0) {
    newPos = getMiddlePos(snakePosOld[0], snakePos[0], currentP);
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
    newPos = getMiddlePos(snakePosOld[i], snakePos[i], currentP);
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
  // 渲染食物
  float foodHeight = 0.0f;
  float foodSize = 1.0f;
  if (foodPos != foodPosOld) {
    foodHeight = 20.0f * (1 - UtilTool::scaleValue(currentP));
    foodSize = UtilTool::scaleValue(currentP);
    this->renderFood(currentP, this->styleFoodOld, foodPosOld, 0.0f, 1 - foodSize);
  }
  // 随机动画效果
  if (this->styleFood == 2) {
    this->renderFood(currentP, this->styleFood, foodPos, foodHeight, 1.0f);
  }
  else {
    this->renderFood(currentP, this->styleFood, foodPos, 0.0f, foodSize);
  }
}

bool SpriteSnake::Over() {
  return this->isOver;
}

void SpriteSnake::Reset() {
  this->isOver = false;

  this->foodPosOld = this->foodPos;
  this->snakeLength = 2;
  int dir = rand() % 4;
  int x = rand() % 7 - 3;
  int y = rand() % 7 - 3;
  this->snakePos[1] = glm::vec3(
    x,
    y,
    dir);
  this->snakePos[0] = glm::vec3(
    x + MoveX[dir],
    y + MoveY[dir],
    dir);
  this->styleBody = rand() % 3;
  this->styleHead = rand() % 3;
  this->newFood(); 
  this->nextDir = (SnakeDir)(int)snakePos[0].z;
}

void SpriteSnake::Update(bool run, bool ai) {
  // 更新过渡数据
  for (int i = 0; i < this->snakeLength; i++) {
    snakePosOld[i] = snakePos[i];
  }
  this->foodPosOld = this->foodPos;
  this->styleFoodOld = this->styleFood;
  int currentLen = this->snakeLength;

  // 计算游戏数据
  if (run) {
    if (this->isOver) return;
    this->updateDir(ai);

    // 更新蛇体
    for (int i = this->snakeLength - 1; i > 0; i--) {
      this->snakePos[i] = this->snakePos[i - 1];
    }
    // 更新头
    this->snakePos[0].x = this->snakePos[0].x + MoveX[(int)this->snakePos[0].z];
    this->snakePos[0].y = this->snakePos[0].y + MoveY[(int)this->snakePos[0].z];
    // 碰撞检测
    for (int i = 1; i < this->snakeLength; i++) {
      if (snakePos[0].x == snakePos[i].x && snakePos[0].y == snakePos[i].y) {
        this->isOver = true;
        break;
      }
    }
    // 判断吃到食物
    if (snakePos[0].x == foodPos.x && snakePos[0].y == foodPos.y) {
      this->snakePos[this->snakeLength] = this->snakePos[this->snakeLength - 1];
      this->snakeLength++;
      this->newFood();
    }

    // 判断结束
    if (this->snakePos[0].x > mapMaxX || this->snakePos[0].x < -mapMaxX) {
      this->isOver = true;
    }
    else if (this->snakePos[0].y > mapMaxY || this->snakePos[0].y < -mapMaxY) {
      this->isOver = true;
    }
  }

  // 更新蛇尾
  if (this->snakeLength != currentLen) {
    snakePosOld[currentLen] = snakePos[currentLen];
  }
}


int SpriteSnake::GetLenght() {
  return this->snakeLength;
}

void SpriteSnake::renderFood(float p, int style, glm::vec2 pos, float height, float size) {
  switch (style) {
  case 1:
    this->renderer->DrawSprite(ResourceManager::GetModel("food"),
      glm::vec3(pos.x, 0.3f + height, pos.y),
      glm::vec3(0.17f * size));
    break;
  case 2:
    this->renderer->DrawSprite(ResourceManager::GetModel("ball"),
      glm::vec3(pos.x, 0.0f + height, pos.y),
      glm::vec3(1 * size));
    break;
  default:
    this->renderer->DrawSprite(ResourceManager::GetModel("lemon"),
      glm::vec3(pos.x, 0.0f + height, pos.y),
      glm::vec3(1.8f * size));
    break;
  }
}


// 生成新食物
void SpriteSnake::newFood() {
  int xLen = (mapMaxX * 2 + 1);
  int yLen = (mapMaxY * 2 + 1);
  int foodPos = xLen * yLen - this->snakeLength;
  if (foodPos == 0) {
    this->isOver = true;
    return;
  }
  foodPos = rand() % foodPos;
  int currentPos = 0;
  bool makeFood = false;
  for (int x = -mapMaxX; x <= mapMaxX; x++) {
    for (int y = -mapMaxY; y <= mapMaxY; y++) {
      bool has = false;
      for (int i = 0; i < snakeLength; i++) {
        if (snakePos[i].x == x && snakePos[i].y == y) {
          has = true;
          break;
        }
      }
      if (!has) {
        if (currentPos == foodPos) {
          this->foodPos.x = x;
          this->foodPos.y = y;
          this->styleFood = rand() % 3;
          makeFood = true;
          break;
        }
        else {
          currentPos++;
        }
      }
    }
    if (makeFood) {
      break;
    }
  }
}

void SpriteSnake::SetHeadStyle(int style) {
  this->styleHead = style;
}

void SpriteSnake::SetBodyStyle(int style) {
  this->styleBody = style;
}


void SpriteSnake::SetNextDir(SnakeDir dir) {
  this->nextDir = dir;
}

void SpriteSnake::updateDir(bool ai) {
  if (ai) {
    // 标准化坐标
    glm::vec2* snakeCoord = new glm::vec2[snakeLength];
    for (int i = 0; i < snakeLength; i++) {
      snakeCoord[snakeLength - i - 1].x = snakePos[i].x + mapMaxX;
      snakeCoord[snakeLength - i - 1].y = snakePos[i].y + mapMaxY;
    }
    glm::vec2 foodCoord = foodPos + glm::vec2(mapMaxX, mapMaxY);
    // 获取下一步的坐标
    glm::vec2 nextStep = smartSnake->GetNextStep(snakeCoord, snakeLength, foodCoord);
    nextStep.x = nextStep.x - mapMaxX;
    nextStep.y = nextStep.y - mapMaxY;
    // 更改蛇的方向
    if (nextStep.y < snakePos[0].y) {
      this->snakePos[0].z = DIR_UP;
    }
    else if (nextStep.y > snakePos[0].y) {
      this->snakePos[0].z = DIR_DOWN;
    }
    else if (nextStep.x < snakePos[0].x) {
      this->snakePos[0].z = DIR_LEFT;
    }
    else if (nextStep.x > snakePos[0].x) {
      this->snakePos[0].z = DIR_RIGHT;
    }
  } else if (
    (this->nextDir == DIR_UP && this->snakePos[0].z != DIR_DOWN) ||
    (this->nextDir == DIR_DOWN && this->snakePos[0].z != DIR_UP) ||
    (this->nextDir == DIR_LEFT && this->snakePos[0].z != DIR_RIGHT) ||
    (this->nextDir == DIR_RIGHT && this->snakePos[0].z != DIR_LEFT)) {
    this->snakePos[0].z = this->nextDir;
  }
}