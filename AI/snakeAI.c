#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define WINDOWS

#ifdef WINDOWS
#include <windows.h>
#endif

void UpdateMap();
void Step();
int DFS();
void PutFood();
void GetNextStep();
int DFS(int souX, int souY, int tagX, int tagY, int isFake);
void FindStep(int headX, int headY);
void MakeFakeSnake();

// 8x8的矩阵
char map[10][10] = {
    "**********", "*        *", "*        *", "*        *", "*        *",
    "*        *", "*        *", "*        *", "*        *", "**********",
};

int locX[64] = {1, 2, 3};
int locY[64] = {2, 2, 2};
int length = 3;
int locXF[64] = {1, 2, 3};
int locYF[64] = {2, 2, 2};
int lengthF = 3;
int foodX = 5, foodY = 5;
int over = 0;
int roadX[64];
int roadY[64];
int qHead = 0;
int qTail = 1;
typedef struct {
  int lastX;
  int lastY;
  int valid;
} elem;
elem dMap[8][8];
int dir[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
int stepX;
int stepY;
int nextStepX;
int nextStepY;

#ifdef WINDOWS
#else
static void sleep_ms(unsigned int secs) {  //毫秒级延时系统
  struct timeval tval;
  tval.tv_sec = secs / 1000;
  tval.tv_usec = (secs * 1000) % 1000000;
  select(0, NULL, NULL, NULL, &tval);
}
#endif

int main() {
  srand(time(0));
  while (1) {
    Step();
    UpdateMap();
    if (over) {
      printf("Game over!\n");
      break;
    }
#ifdef WINDOWS
    Sleep(50);
#else
    sleep_ms(50);
#endif
  }
  return 0;
}

void UpdateMap() {
#ifdef WINDOWS

#else
  printf("\033[2J");   //清屏
  printf("\033[27A");  //移动到起始位置
#endif
  char outputMap[10][10];
  memcpy(outputMap, map, sizeof(map));
  outputMap[foodX + 1][foodY + 1] = 'M';  // 食物
  for (int i = 0; i < length - 1; i++) {
    outputMap[locX[i] + 1][locY[i] + 1] = 'O';  // 蛇身
  }
  outputMap[locX[length - 1] + 1][locY[length - 1] + 1] = 'X';  // 蛇头
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      printf("%c", outputMap[i][j]);
    }
    printf("\n");
  }
}

void PutFood() {
  int x = rand() % 8;
  int y = rand() % 8;
  for (int i = 0; i < length; i++) {
    if (x == locX[i] && y == locY[i]) {
      PutFood();
      return;
    }
  }
  foodX = x;
  foodY = y;
}

// 走一步
void Step() {
  if (length >= 64) return;
  GetNextStep();
  // printf("%d, %d\n", nextStepX, nextStepY);
  if (nextStepX > 7 || nextStepX < 0 || nextStepY > 7 || nextStepY < 0 ||
      over == 1) {
    over = 1;
    return;
  }
  if (nextStepX == foodX && nextStepY == foodY) {
    locX[length] = nextStepX;
    locY[length] = nextStepY;
    length++;
    if (length >= 64) {
      over = 1;
      return;
    }
    PutFood();
  } else {
    for (int i = 0; i < length - 1; i++) {
      locX[i] = locX[i + 1];
      locY[i] = locY[i + 1];
    }
    locX[length - 1] = nextStepX;
    locY[length - 1] = nextStepY;
  }
}

void MakeFakeSnake() {
  for (int i = 0; i < length; i++) {
    locXF[i] = locX[i];
    locYF[i] = locY[i];
  }
  lengthF = length;
}

void GetNextStep() {
  int isEat = DFS(locX[length - 1], locY[length - 1], foodX, foodY, 0);
  FindStep(locX[length - 1], locY[length - 1]);
  int nextStepXF = stepX;
  int nextStepYF = stepY;
  if (isEat) {
    // 虚拟蛇
    MakeFakeSnake();
    while (1) {
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
          return;
        } else {
          break;
        }
      } else {
        for (int i = 0; i < lengthF - 1; i++) {
          locXF[i] = locXF[i + 1];
          locYF[i] = locYF[i + 1];
        }
        locXF[lengthF - 1] = stepX;
        locYF[lengthF - 1] = stepY;
        FindStep(stepX, stepY);
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
    if (x > 7 || x < 0 || y > 7 || y < 0) {
      // printf("out of range.\n");
      dis[i] = -2;
    }
    MakeFakeSnake();
    if (x == foodX && y == foodY) {
      locXF[lengthF] = x;
      locYF[lengthF] = y;
      lengthF++;
    } else {
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
  if (dis[max] == -2) {
    over = 1;
  }
  nextStepX = locX[length - 1] + dir[max][0];
  nextStepY = locY[length - 1] + dir[max][1];
  return;
}

int DFS(int souX, int souY, int tagX, int tagY, int isFake) {
  // 初始化地图
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
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
  } else {
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
      if (nextX > 7 || nextX < 0 || nextY > 7 || nextY < 0) {
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
    if (qHead == qTail || qHead >= 64) {  // 无路可走
      return 0;
    }
  }
  return 0;
}

void FindStep(int headX, int headY) {
  int x = roadX[qHead];
  int y = roadY[qHead];
  // printf("find step:%d, %d\n", headX, headY);
  // printf("begin: %d, %d\n", x, y);
  while (1) {
    int nx = dMap[x][y].lastX;
    int ny = dMap[x][y].lastY;
    if (nx == headX && ny == headY) {
      stepX = x;
      stepY = y;
      return;
    } else if (nx < 0 || ny < 0) {
      return;
    }
    x = nx;
    y = ny;
    // printf("last: %d, %d\n", x, y);
  }
}
