#include "snake.h"

int main(){
  printf("\033[2J\033[1m");//清屏，设置颜色，高亮
  put_money();//投放食物
  output();//输出地图
  while(1){//循环处理系统
      new();
      int flag1 = 0, flag2 = 0, flag3 = 0;
      if ((snakeAi(11, 1, FsnakeX, FsnakeY, 0, 0, whereX, whereY) == 1) && (flag2 = snakeAi(1, 0, FsnakeX, FsnakeY, 0, 0, FsnakeX[0], FsnakeY[0]) == 1)){
        snakeAi(11, 1, snakeX, snakeY, 1, 0, whereX, whereY);test6 = 1;
      } else {
        int ind_x = 1, ind_y = 1;
        if (flag2 == 1){
          snakeAi(1, 0, snakeX, snakeY, 0, 0, snakeX[0], snakeY[0]);test6 = 2;
        } else {
          for (ind_x = 1; ind_x < 17; ind_x ++) {
          for (ind_y = 1; ind_y < 17; ind_y ++){
            new();
            test1 = ind_x;
            test2 = ind_y;
            if ((snakeAi(1, 0, FsnakeX, FsnakeY, 0, 0, ind_x, ind_y) == 1) && (snakeAi(1, 0, FsnakeX, FsnakeY, 0, 0, FsnakeX[0], FsnakeY[0]) == 1)){
              flag3 = 1;
              break;
            }
          }
          if (flag3 == 1) break;
        }
        if (flag3 == 1){
          snakeAi(1, 0, snakeX, snakeY, 1, 0, ind_x, ind_y);test6 = 3;
        } else {
          snakeAi(1, 0, snakeX, snakeY, 1, 0, whereX, whereY);test6 = 4;
        }
      }
        }

      if (is_over == 1) break;//游戏结束
      sleep_ms(speed);//延时
  }
  //if(is_over == 0) gameover();
  return 0;
}
