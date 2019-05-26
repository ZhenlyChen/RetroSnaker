#include "SmartSnake.h"

SmartSnake::SmartSnake(int width, int height) {
  this->mWidth = width;
  this->mHeight = height;

  int size = width * height;
  size = size > 10000 ? 10000 : size;
  this->roadX = new int[size];
  this->roadY = new int[size];
  this->locX = new int[size];
  this->locY = new int[size];
  this->locXF = new int[size];
  this->locYF = new int[size];

  this->dMap = new elem *[mWidth];
  for (int i = 0; i < mWidth; i++) {
    this->dMap[i] = new elem[mHeight];
  }
}

SmartSnake::~SmartSnake() {
  delete[] this->roadX;
  delete[] this->roadY;
  delete[] this->locX;
  delete[] this->locY;
  delete[] this->locXF;
  delete[] this->locYF;

  for (int i = 0; i < mWidth; i++) {
    delete[] dMap[i];
  }
  delete this->dMap;
}

glm::vec2 SmartSnake::GetNextStep(glm::vec2* snake, int length, glm::vec2 food) {
  this->length = length;

  for (int i = 0; i < length; i++) {
    locX[i] = snake[i].x;
    locY[i] = snake[i].y;
  }

  this->foodX = food.x;
  this->foodY = food.y;
  return this->getNextStep();
}

void SmartSnake::makeFakeSnake() {
  for (int i = 0; i < length; i++) {
    locXF[i] = locX[i];
    locYF[i] = locY[i];
  }
  lengthF = length;
}

glm::vec2 SmartSnake::getNextStep() {
  int isEat = DFS(locX[length - 1], locY[length - 1], foodX, foodY, 0);
  // printf("%d, %d eat %d %d %d ", locX[length - 1], locY[length - 1], foodX, foodY, isEat);
  findStep(locX[length - 1], locY[length - 1]);
  int nextStepXF = stepX;
  int nextStepYF = stepY;
  if (isEat) {
    // 虚拟蛇
    makeFakeSnake();
    int maxCount = 0;
    while (1) {
      maxCount++;
      if (maxCount > 100000) break;
      if (stepX == foodX && stepY == foodY) {
        locXF[lengthF] = stepX;
        locYF[lengthF] = stepY;
        lengthF++;
        int isSafe =
          DFS(locXF[lengthF - 1], locYF[lengthF - 1], locXF[0], locYF[0], 1);
        // printf("%d,%d to %d,%d %d\n",locXF[lengthF - 1], locYF[lengthF - 1],
        // locXF[0], locYF[0], isSafe);
        if (isSafe) {
          nextStepX = nextStepXF;
          nextStepY = nextStepYF;
          // printf("safe to %d, %d\n", nextStepX, nextStepY);
          return glm::vec2(nextStepX, nextStepY);
        }
        else {
          break;
        }
      } else {
        for (int i = 0; i < lengthF - 1; i++) {
          locXF[i] = locXF[i + 1];
          locYF[i] = locYF[i + 1];
        }
        locXF[lengthF - 1] = stepX;
        locYF[lengthF - 1] = stepY;
        findStep(stepX, stepY);
      }
    }
  }
  // 吃不到 或者 吃完找不到安全路径
  int dis[4];
  for (int i = 0; i < 4; i++) {
    dis[i] = 0;
    int x = locX[length - 1] + dir[i][0];
    int y = locY[length - 1] + dir[i][1];
    // printf("%d, %d\n", x, y);
    for (int j = 1; j < length; j++) {
      if (x == locX[j] && y == locY[j]) {
        // printf("in body.\n");
        dis[i] = -2;
        break;
      }
    }
    if (x >= mWidth || x < 0 || y >= mHeight || y < 0) {
      // printf("out of range.\n");
      dis[i] = -2;
    }
    makeFakeSnake();
    if (x == foodX && y == foodY) {
      locXF[lengthF] = x;
      locYF[lengthF] = y;
      lengthF++;
    }
    else {
      for (int j = 0; j < lengthF - 1; j++) {
        locXF[j] = locXF[j + 1];
        locYF[j] = locYF[j + 1];
      }
      locXF[lengthF - 1] = x;
      locYF[lengthF - 1] = y;
    }
    if (dis[i] != -2 && !DFS(x, y, locXF[0], locYF[0], 1)) {
      // printf("no path.\n");
      dis[i] = -1;
    }
    if (dis[i] != -1 && dis[i] != -2) {
      dis[i] = abs(x - foodX) + abs(y - foodY);
    }
  }
  // printf("%d,%d,%d,%d\n", dis[0], dis[1], dis[2], dis[3]);
  int max = 0;
  for (int i = 1; i < 4; i++) {
    if (dis[i] > dis[max]) {
      max = i;
    }
  }
  nextStepX = locX[length - 1] + dir[max][0];
  nextStepY = locY[length - 1] + dir[max][1];
  return glm::vec2(nextStepX, nextStepY);
}


int SmartSnake::DFS(int souX, int souY, int tagX, int tagY, int isFake) {
  // 初始化地图
  for (int i = 0; i < mWidth; i++) {
    for (int j = 0; j < mHeight; j++) {
      dMap[i][j].lastX = -1;
      dMap[i][j].lastY = -1;
      dMap[i][j].valid = 1;
    }
  }
  // 填充蛇身
  if (isFake) {
    for (int i = 1; i < lengthF; i++) {
      dMap[locXF[i]][locYF[i]].valid = 0;
    }
  }
  else {
    for (int i = 1; i < length; i++) {
      dMap[locX[i]][locY[i]].valid = 0;
    }
  }
  qHead = 0;
  qTail = 1;
  roadX[qHead] = souX;
  roadY[qHead] = souY;
  // printf("dfs %d, %d to %d, %d:\n", souX, souY, tagX, tagY);
  while (1) {
    int x = roadX[qHead];
    int y = roadY[qHead];
    // printf(" %d, %d:\n", x, y);
    if (x == tagX && y == tagY) {
      return 1;
    }
    for (int i = 0; i < 4; i++) {
      int nextX = x + dir[i][0];
      int nextY = y + dir[i][1];
      if (nextX >= mWidth || nextX < 0 || nextY >= mHeight || nextY < 0) {
        continue;
      }
      if (dMap[nextX][nextY].valid == 1 || (nextX == tagX && nextY == tagY)) {
        dMap[nextX][nextY].lastX = x;
        dMap[nextX][nextY].lastY = y;
        dMap[nextX][nextY].valid = 0;
        roadX[qTail] = nextX;
        roadY[qTail] = nextY;
        qTail++;
      }
    }
    qHead++;
    if (qHead == qTail || qHead > mHeight *  mWidth) {  // 无路可走
      return 0;
    }
  }
  return 0;
}

void SmartSnake::findStep(int headX, int headY) {
  if (qHead > mHeight * mWidth) {
    return;
  }
  int x = roadX[qHead];
  int y = roadY[qHead];
  // printf("find step:%d, %d\n", headX, headY);
  // printf("begin: %d, %d\n", x, y);
  int count = 0;
  while (1) {
    count++;
    if (count > mHeight * mWidth) break;
    int nx = dMap[x][y].lastX;
    int ny = dMap[x][y].lastY;
    if (nx == headX && ny == headY) {
      stepX = x;
      stepY = y;
      return;
    }
    else if (nx < 0 || ny < 0) {
      return;
    }
    x = nx;
    y = ny;
    // printf("last: %d, %d\n", x, y);
  }
}