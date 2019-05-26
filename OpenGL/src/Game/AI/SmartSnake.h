#pragma once
#ifndef SMARKSNAKE_H
#define SMARTSNAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <glm/glm.hpp>

typedef struct {
  int lastX;
  int lastY;
  int valid;
} elem;

class SmartSnake {
public:
  SmartSnake(int width, int height);
  ~SmartSnake();
  glm::vec2 GetNextStep(glm::vec2* snake, int length, glm::vec2 food);
private:
  int DFS(int souX, int souY, int tagX, int tagY, int isFake);
  void findStep(int headX, int headY);
  void makeFakeSnake();
  glm::vec2 getNextStep();
  // 地图
  int mHeight;
  int mWidth;
  // 蛇的数据
  int *locX;
  int *locY;
  int length;
  // 虚拟蛇的数据
  int *locXF;
  int *locYF;
  int lengthF = 3;
  // 食物坐标
  int foodX;
  int foodY;
  // 路径
  int *roadX;
  int *roadY;
  int qHead = 0;
  int qTail = 1;
  elem **dMap;
  // 方向
  int dir[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
  
  int stepX;
  int stepY;
  int nextStepX;
  int nextStepY;
};

#endif // !SMARKSNAKE_H