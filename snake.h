#ifndef SNAKE_H
#define SNAKE_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#define SNAKE_MAX_LENGTH 200
#define BLANK_CELL ' '
#define SNAKE_FOOD '$'
#define  WALL_CELL '*'
#define ECHOFLAGS (ECHO|ECHOE|ECHOK|ECHONL|ICANON)

char map[25][50] =
{
  "******************",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "******************",
  "*  小霸王学习机  *       ",
  "******************",
};

char map_begin[25][50] =
{
 "\n******************\n",
  "*   小蛇冲天炮   *           \n",
  "*                *\n",
  "* 按回车开始游戏 *            \n",
  "*                *\n",
  "*按数字键选择难度*            \n",
  "*                *\n",
  "*  难度：6       *            \n",
  "*                *\n",
  "*按ZXCVBN选择皮肤*            \n",
  "*                *\n",
  "*  皮肤：OOOOX   *            \n",
  "*                *\n",
  "* 按 Q 退出游戏  *            \n",
  "******************\n\n\n",
};

int snakeX[SNAKE_MAX_LENGTH] = {1, 2, 3, 4, 5};
int snakeY[SNAKE_MAX_LENGTH] = {1, 1, 1, 1, 1};
int FsnakeX[SNAKE_MAX_LENGTH] = {1, 2, 3, 4, 5};
int FsnakeY[SNAKE_MAX_LENGTH] = {1, 1, 1, 1, 1};
int is_over = 0, is_begin = 0, speed = 200, snakeLength = 5;//蛇的初始长度
char SNAKE_HEAD  = 'X', SNAKE_BODY = 'O';//蛇的皮肤
int whereX = 0, whereY = 0;
int test1,test2,test3,test4,test5,test6;
void snakeMove(int nextX, int nextY);//蛇的移动
void put_money(void);//投放食物

void output(void);//输出界面

void gameover(void);//游戏结束

void gamewin(void);//游戏胜利

void gameset(char input);//游戏设置

void sleep_ms(unsigned int secs);//毫秒级延时系统


void snakeMove(int nextX, int nextY){
  test1 = nextY;
  test2 = nextX;
  if (map[nextY][nextX] == WALL_CELL) {//如果撞墙了
    //gameover();
    is_over = 1;
    return;
  }
  if (map[nextY][nextX] == SNAKE_FOOD) { // 吃到东西了
    snakeLength ++;
    if (snakeLength >= SNAKE_MAX_LENGTH){// 判断游戏胜利
      gamewin();
      return;
    }
    snakeX[snakeLength - 1] = nextX;
    snakeY[snakeLength - 1] = nextY;
    put_money();
  } else {
    for (int index = 0; index < snakeLength - 1; index ++){//更新身体坐标
    snakeX[index] = snakeX[index + 1];
    snakeY[index] = snakeY[index + 1];
    }
    snakeX[snakeLength - 1] = nextX;
    snakeY[snakeLength - 1] = nextY;
  }

  for (int index = 0; index < snakeLength; index ++){//如果撞到自己的身体了
    for (int indey = 0; indey < snakeLength; indey ++){
      if (snakeX[index] == snakeX[indey] && snakeY[index] == snakeY[indey] && index != indey){
         //gameover();
        is_over = 1;
        return;
      }
    }
  }
  output();
}



void put_money(void){
  srand((unsigned)time(NULL));

  while (1) {
    int flag = 0;
    whereX = 1 + rand() % 16;
    whereY = 1 + rand() % 16;
    for (int index = 0; index < snakeLength; index ++){//如果生成的食物刚好在身体的位置
      if ((snakeX[index] == whereX && snakeY[index] == whereY) || map[whereY][whereX] == '*'){
        flag = 1;
        break;
      }
    }
    if (flag == 1) continue;

    break;
  }
  map[whereY][whereX] = SNAKE_FOOD;//添加食物
  output();
  return;
}

void output(void){//刷新画面
  printf("\033[27A");//移动到起始位置
  printf("******************          \n");
  printf("*  小蛇冲天炮-%d  *                 \n", (10 - (speed / 50)));
  printf("*  WASD控制方向  *                \n");
  printf("*  Q结束游戏     *                \n");
  printf("*  鼠标点击暂停  *                \n");
  printf("*  分数: %06d  *        \n", (snakeLength - 5) * (5 - speed / 100) * 2 );
  for (int index = 0; index < snakeLength - 1; index ++){
    map[snakeY[index]][snakeX[index]] = SNAKE_BODY;
  }//填充蛇体到地图里
  map[snakeY[snakeLength - 1]][snakeX[snakeLength - 1]] = SNAKE_HEAD;
  for (int index = 0; index < 20; index ++) {
    printf("%s\n", map[index]);
  }//输出地图
  for (int index = 0; index < snakeLength - 1; index ++){
    map[snakeY[index]][snakeX[index]] = ' ';
  }//从地图里面清除蛇体
  map[snakeY[snakeLength - 1]][snakeX[snakeLength - 1]] = ' ';
  printf("*%d*%d**%d*%d*%d*%d**********\n", test1, test2,test3,test4,test5,test6);
  return;
}

void gameover(void){
  printf("\033[3A\033[10D\033[1m******************\033[0m\n\033[1m*   Game Over!   *\n\033[1m******************\033[0m\033[3B\n");//输出游戏结束
  is_over = 1;
  return;
}
void gamewin(void){
  printf("\033[3A\033[10D\033[1m******************\033[0m\n\033[1m*   You  Win !   *\n\033[1m******************\033[0m\033[3B\n");//输出胜利
  is_over = 1;
  return;
}

void sleep_ms(unsigned int secs){//毫秒级延时系统
  struct timeval tval;
  tval.tv_sec=secs/1000;
  tval.tv_usec=(secs*1000)%1000000;
  select(0,NULL,NULL,NULL,&tval);
}


void gameset(char input){

      if (input > '0' && input <= '9'){//难度调节
        speed = (10 - (input - '0')) * 50;
        printf("\033[s");
        printf("\033[10A");
        printf("*  难度：%c       *            \n", input);
        printf("\033[u");
      }
      switch(input){//皮肤系统
        case 'z':
        case 'Z':
          SNAKE_BODY = 'O';
          SNAKE_HEAD = 'X';
          break;
        case 'X':
        case 'x':
          SNAKE_BODY = 'O';
          SNAKE_HEAD = 'H';
          break;
        case 'C':
        case 'c':
          SNAKE_BODY = 'X';
          SNAKE_HEAD = 'H';
          break;
        case 'v':
        case 'V':
          SNAKE_BODY = 'o';
          SNAKE_HEAD = 'O';
          break;
        case 'B':
        case 'b':
          SNAKE_BODY = 'O';
          SNAKE_HEAD = 'V';
          break;
        case 'n':
        case 'N':
          SNAKE_BODY = 'X';
          SNAKE_HEAD = 'M';
          break;
      }
      printf("\033[s");
      printf("\033[6A");
      printf("*  皮肤：%c%c%c%c%c   *          \n", SNAKE_BODY, SNAKE_BODY, SNAKE_BODY, SNAKE_BODY, SNAKE_HEAD);
      printf("\033[u");
}
//char temp_where = 0;//消除输入延时**未实现**
#endif
