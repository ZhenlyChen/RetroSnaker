#include <stdio.h>
#include <string.h>
#include "snake.h"

int snakeAi(int fuk, int fck, int *snakeX, int *snakeY, int ok, int free, int whereXss, int whereYss);
int FsnakeMove(int nextX, int nextY, int posX, int posY);
void new(void);

int main(){
  printf("\033[2J\033[1m");//清屏，设置颜色，高亮
  printf("\033[2J");//清屏
  put_money();//投放食物
  output();//输出地图
  while(1){//循环处理系统
      new();
      int flag1 = 0, flag2 = 0;
      if (flag1 = (snakeAi(1, 0, FsnakeX, FsnakeY, 0, 0, whereX, whereY)) == 1 && (flag2 = snakeAi(1, 0, FsnakeX, FsnakeY, 0, 0, FsnakeX[0], FsnakeY[0])) == 1){
        snakeAi(1, 0, snakeX, snakeY, 1, 0, whereX, whereY);
      } else {
        if(flag2 != 1){
          snakeAi(1, 0, snakeX, snakeY, 1, 1, whereX, whereY);
        } else {
          snakeAi(1, 0, snakeX, snakeY, 1, 0, snakeX[0], snakeY[0]);
        }
      }
      if (is_over == 1) break;//游戏结束
      sleep_ms(50);//延时
  }
  //if(is_over == 0) gameover();
  return 0;
}

void new(void){
  for (int index = 0;index < snakeLength; index ++){
        FsnakeX[index] = snakeX[index];
        FsnakeY[index] = snakeY[index];
  }
}

int FsnakeMove(int nextX, int nextY, int posX, int posY){
  if (map[nextY][nextX] == WALL_CELL) {//如果撞墙了
    return 1;
  }
  if (nextY == posY && nextX == posX) { // 吃到东西了
    return 2;
  } else {
    for (int index = 0; index < snakeLength - 1; index ++){//更新身体坐标
    FsnakeX[index] = FsnakeX[index + 1];
    FsnakeY[index] = FsnakeY[index + 1];
    }
    FsnakeX[snakeLength - 1] = nextX;
    FsnakeY[snakeLength - 1] = nextY;
  }
  for (int index = 0; index < snakeLength; index ++){//如果撞到自己的身体了
    for (int indey = 0; indey < snakeLength; indey ++){
      if (FsnakeX[index] == FsnakeX[indey] && FsnakeY[index] == FsnakeY[indey] && index != indey){
        return 1;
      }
    }
  }
  return 0;
}


int snakeAi(int fuk, int fck, int *snakeX, int *snakeY, int ok, int free, int whereXss, int whereYss){
  int flags = 0;
  while (flags != 2){

  int nextX[3], nextY[3], distance[3], maxDis = 0;
  nextX[0] = 2 * snakeX[snakeLength - 1] - snakeX[snakeLength - 2];//前方
  nextY[0] = 2 * snakeY[snakeLength - 1] - snakeY[snakeLength - 2];
  if (snakeX[snakeLength - 1] == snakeX[snakeLength - 2]){
    nextX[1] = snakeX[snakeLength - 1] + 1; //左方
    nextY[1] = snakeY[snakeLength - 1];
    nextX[2] = snakeX[snakeLength - 1] - 1; //右方
    nextY[2] = snakeY[snakeLength - 1];
  } else {
    nextX[1] = snakeX[snakeLength - 1]; //左方
    nextY[1] = snakeY[snakeLength - 1] + 1;
    nextX[2] = snakeX[snakeLength - 1]; //右方
    nextY[2] = snakeY[snakeLength - 1] - 1;
  }

  for (int index = 0; index < 3; index ++) {//计算最优方位
    distance[index] = ((whereXss - nextX[index]) * (whereXss - nextX[index]) + (whereYss - nextY[index]) * (whereYss - nextY[index])) * fuk + ((snakeX[0] - nextX[index]) * (snakeX[0] - nextX[index]) + (snakeY[0] - nextY[index]) * (snakeY[0] - nextY[index])) * fck;
    if (free == 1) {
      distance[index] = -distance[index];
    }
    if (map[nextY[index]][nextX[index]] == '*') distance[index] += 5000;
      for (int myindex = 0; myindex < snakeLength; myindex ++){//如果撞到自己的身体了
          if (snakeX[myindex] == nextX[index] && snakeY[myindex] == nextY[index]){
            distance[index] += 5000;
            break;
          }
      }
  }

  maxDis = distance[0];
  for (int index = 1; index < 3; index ++) {
    maxDis = (maxDis > distance[index]) ? distance[index] : maxDis;
  }
  test3 = distance[0];
  test4 = distance[1];
  test5 = distance[2];
  if (ok == 0){
    if (maxDis == distance[0]) {
      flags = FsnakeMove(nextX[0], nextY[0], whereXss, whereYss);
    } else if (maxDis == distance[1]){
      flags = FsnakeMove(nextX[1], nextY[1], whereXss, whereYss);
    } else {
      flags = FsnakeMove(nextX[2], nextY[2], whereXss, whereYss);
    }
    if(flags == 1) return 0;
  } else {
    if (maxDis == distance[0]) {
      snakeMove(nextX[0], nextY[0]);
      break;
    } else if (maxDis == distance[1]){
      snakeMove(nextX[1], nextY[1]);
      break;
    } else {
      snakeMove(nextX[2], nextY[2]);
      break;
    }
  }
}
  return 1;
}







