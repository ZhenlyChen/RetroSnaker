#include <reg51.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned int u16;
#include <intrins.h>
#include <math.h>

void Step();
int DFS();
void GetNextStep();
int DFS(int souX, int souY, int tagX, int tagY, int isFake);
void FindStep(int headX, int headY);
void MakeFakeSnake();
void PutFood();
char locX[64] = {1, 2, 3};
char locY[64] = {2, 2, 2};
char length = 3;
char locXF[64] = {1, 2, 3};
char locYF[64] = {2, 2, 2};
char lengthF = 3;
char foodX = 5, foodY = 5;
char over = 0;
char roadX[64];
char roadY[64];
char qHead = 0;
char qTail = 1;
typedef struct {
  char lastX;
  char lastY;
  char valid;
} elem;
elem dMap[8][8];
char dir[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
char stepX;
char stepY;
char nextStepX;
char nextStepY;

void delay(u16 i) {
  while (i--)
    ;
}
/***************初始化*************/
sbit up = P3 ^ 1;
sbit down = P3 ^ 0;
sbit left = P3 ^ 2;
sbit right = P3 ^ 3;

sbit SRCLK = P3 ^ 6;
sbit RCLK = P3 ^ 5;
sbit SER = P3 ^ 4;

u8 addx, addy;
u8 head_x[40], head_y[40];
u8 poit_x, poit_y;
u8 leng, z = 0;
u8 y_list[8];
u8 ledwei[] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
static k = 0;

/****************延时函数**********/
void delay1s(void)  //?? 0us
{
  unsigned char a, b, c;
  for (c = 167; c > 0; c--)
    for (b = 171; b > 0; b--)
      for (a = 16; a > 0; a--)
        ;
}
void delay1ms(u16 num) {
  u16 x, y;
  for (x = num; x > 0; x--)
    for (y = 110; y > 0; y--) {
      ;  //??1ms
    }
}
void delay500ms(void)  //?? 0us
{
  unsigned char a, b, c;
  for (c = 23; c > 0; c--)
    for (b = 152; b > 0; b--)
      for (a = 70; a > 0; a--)
        ;
}
void delay250ms(void)  //误差 0us
{
  unsigned char a, b, c;
  for (c = 11; c > 0; c--)
    for (b = 92; b > 0; b--)
      for (a = 122; a > 0; a--)
        ;
}

/***************显示函数**************/
void display() {
  u8 i, j;
  u8 x = 0;
  u8 y = 0;
  u8 w = 0x01;
  for (j = 0; j < 8; j++) y_list[j] = 0x00;
  for (i = 0; i < length; i++) {
    x = locX[i];
    y = locY[i];
    switch (x) {
      case 0:
        y_list[0] = w << y | y_list[0];
        w = 0x01;
        break;
      case 1:
        y_list[1] = w << y | y_list[1];
        w = 0x01;
        break;
      case 2:
        y_list[2] = w << y | y_list[2];
        w = 0x01;
        break;
      case 3:
        y_list[3] = w << y | y_list[3];
        w = 0x01;
        break;
      case 4:
        y_list[4] = w << y | y_list[4];
        w = 0x01;
        break;
      case 5:
        y_list[5] = w << y | y_list[5];
        w = 0x01;
        break;
      case 6:
        y_list[6] = w << y | y_list[6];
        w = 0x01;
        break;
      case 7:
        y_list[7] = w << y | y_list[7];
        w = 0x01;
        break;
    }
  }
}
/****************主函数****************/
void main() {
  u8 i, x, y;
  /***********中断初始化*********/
  TMOD |= 0X01;  //
  TL0 = 0x3C;
  TH0 = 0xF6;
  EA = 1;
  ET0 = 1;
  TR0 = 1;
  srand(time(0));

  while (1) {
    Step();
    display();
    if (over) {
      printf("Game over!\n");
      break;
    }
    delay250ms();
  }
}
/*******************************************************************************
 * 函数名         : Hc595SendByte(u8 dat)
 * 函数功能		   : 向74HC595发送一个字节的数据
 * 输入           : 无
 * 输出         	 : 无
 *******************************************************************************/
void Hc595SendByte(u8 dat) {
  u8 a;
  SRCLK = 0;
  RCLK = 0;
  for (a = 0; a < 8; a++) {
    SER = dat >> 7;
    dat <<= 1;

    SRCLK = 1;
    _nop_();
    _nop_();
    SRCLK = 0;
  }

  RCLK = 1;
  _nop_();
  _nop_();
  RCLK = 0;
}
/****************显示函数**************/
void T0_int() interrupt 1 {
  u8 x, y;
  TL0 = 0x3C;
  TH0 = 0xF6;
  x = foodX;
  y = foodY;
  y_list[x - 1] |= 0x01 << y;
  P0 = 0x7f;
  P0 = ledwei[k];            //??
  Hc595SendByte(y_list[z]);  //
  delay(100);                //
  Hc595SendByte(0x00);
  z++;
  k++;
  if (z == 8) {
    z = 0;
    k = 0;
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
