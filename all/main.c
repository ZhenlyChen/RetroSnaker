#include "snake.h"
int main(){
  printf("\033[2J\033[1m");//清屏，设置颜色，高亮
  for (int index = 0; index < 15; index ++) printf("%s", map_begin[index]);//输出开始界面

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
    input = getchar();//得到键盘实时输入
    if (is_begin == 0 ){//游戏设置系统
      gameset(input);//游戏初始界面刷新
    }
    if (input == 'Q' || input == 'q' || is_over == 1) break;//退出游戏
    if (input == 10) {//回车开始游戏
      is_begin = 1;
      put_money();//投放食物
      printf("\033[2J");//清屏
      output();
    }
    if (is_begin == 1) {//处于开始状态
      snakeMove(input);//移动蛇
    }
    sleep_ms(speed);//延时
  }
  tcsetattr(STDIN_FILENO,TCSAFLUSH,&old);
  if(is_over == 0) gameover();
  return 0;
}

