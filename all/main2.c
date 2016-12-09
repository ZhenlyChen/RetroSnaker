#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#define ECHOFLAGS (ECHO|ECHOE|ECHOK|ECHONL|ICANON)
#define SNAKE_MAX_LENGTH 200
#define BLANK_CELL ' '
#define SNAKE_FOOD '$'
#define WALL_CELL '*'
void snakeMove(char where);
void put_money(void);
void output(void);
void gameover(void);
void gamewin(void);
static void sleep_ms(unsigned int secs);
char map[25][50] =
{
  "******************",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*    ********    *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "*    *** ***     *",
  "*                *",
  "*                *",
  "*                *",
  "*                *",
  "******************",
  "*  小霸王学习机  *       ",
  "******************",
};
int snakeX[SNAKE_MAX_LENGTH] = {1, 2, 3, 4, 5};
int snakeY[SNAKE_MAX_LENGTH] = {1, 1, 1, 1, 1};
int is_over = 0, is_begin = 0, speed = 200, snakeLength = 5;//蛇的初始长度
char SNAKE_HEAD  = 'X', SNAKE_BODY = 'O';//蛇的皮肤
//char temp_where = 0;//消除输入延时**未实现**
int main(){
  printf("\033[2J\033[1m");//清屏，设置颜色，高亮
  printf("\n******************\n");
  printf("*   小蛇冲天炮   *           \n");
  printf("*                *\n");
  printf("* 按回车开始游戏 *            \n");
  printf("*                *\n");
  printf("*按数字键选择难度*            \n");
  printf("*                *\n");
  printf("*  难度：6       *            \n");
  printf("*                *\n");
  printf("*按ZXCVBN选择皮肤*            \n");
  printf("*                *\n");
  printf("*  皮肤：OOOOX   *            \n");
  printf("*                *\n");
  printf("* 按 Q 退出游戏  *            \n");
  printf("******************\n\n\n");
  struct termios new,old;//实现非阻塞输入
  int flag = 0, input = 0;
  if(tcgetattr(STDIN_FILENO,&old)==-1) exit(1);
  new=old;
  new.c_lflag&=~ECHOFLAGS;
  if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&new)==-1){
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&old);
    exit(1);
  }
  flag=fcntl(STDIN_FILENO,F_GETFL);
  flag|=O_NONBLOCK;
  if(fcntl(STDIN_FILENO,F_SETFL,flag)==-1) exit(1);

  while(1){
    input = getchar();
    if (is_begin == 0 ){//游戏设置系统
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

    if (input == 'Q' || input == 'q' || is_over == 1) break;
    if (input == 10) {
      is_begin = 1;
      put_money();
    }
    if (is_begin == 1){
      is_begin = 2;//开始游戏
      printf("\033[2J");//清屏
      output();
    }
    if (is_begin == 2) {//处于开始状态
      /*
      if (temp_where == input || (temp_where - 32) == input|| (temp_where + 32) == input){//消除输入延时
      //输入与上一次相同
        while (getchar() == temp_where) {
          temp_where = input;
        }
      }
      temp_where = input;
      */
      snakeMove(input);
    } else {

    }

    sleep_ms(speed);


  }
  tcsetattr(STDIN_FILENO,TCSAFLUSH,&old);
  if(is_over == 0) gameover();
  return 0;
}

void snakeMove(char where){
  int nextX = snakeX[snakeLength - 1], nextY = snakeY[snakeLength - 1];
  switch (where) {
    case 'A':
    case 'a':
      nextX --;
      break;
      //temp_where = 'a';
    case 'W':
    case 'w':
      nextY --;
      //temp_where = 'w';
      break;
    case 'D':
    case 'd':
      nextX ++;
      //temp_where = 'd';
      break;
    case 'S':
    case 's':
      nextY ++;
      //temp_where = 's';
      break;
    default://默认前进
      nextX = 2 * nextX - snakeX[snakeLength - 2];
      nextY = 2 * nextY - snakeY[snakeLength - 2];
      break;
  }

  if (nextX == snakeX[snakeLength - 2] && nextY == snakeY[snakeLength - 2]){//如果前进方向是自己的身体
    nextX = snakeX[snakeLength - 1];
    nextY = snakeY[snakeLength - 1];
    nextX = 2 * nextX - snakeX[snakeLength - 2];
    nextY = 2 * nextY - snakeY[snakeLength - 2];
  }

  if (map[nextY][nextX] == WALL_CELL) {//如果撞墙了
    gameover();
    return;
  }

  if (map[nextY][nextX] == SNAKE_FOOD) { // 吃到东西了
    snakeLength ++;
    if (snakeLength >= SNAKE_MAX_LENGTH){
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
         gameover();
          return;
      }
    }
  }
  output();
}

void put_money(void){
  srand((unsigned)time(NULL));
  int whereX = 0, whereY = 0;
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
  printf("\033[26A");//移动到起始位置
  printf("******************          \n");
  printf("*  小蛇冲天炮%d   *                 \n", (10 - (speed / 50)));
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

static void sleep_ms(unsigned int secs){//毫秒级延时系统
  struct timeval tval;
  tval.tv_sec=secs/1000;
  tval.tv_usec=(secs*1000)%1000000;
  select(0,NULL,NULL,NULL,&tval);
}

