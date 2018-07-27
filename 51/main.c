#include <reg51.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned int u16;
#include <math.h> 
#include <intrins.h>



void delay(u16 i)
{
	while(i--);	
}
/***************初始化*************/
sbit up=P3^1;
sbit down=P3^0;
sbit left=P3^2;
sbit right=P3^3;

sbit SRCLK=P3^6;
sbit RCLK=P3^5;
sbit SER=P3^4;



u8 addx,addy;
u8 head_x[40],head_y[40];
u8 poit_x,poit_y;
u8 leng,z=0;
u8 y_list[8];
u8 ledwei[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};
static k=0;




/****************延时函数**********/
void delay1s(void)   //?? 0us
{
    unsigned char a,b,c;
    for(c=167;c>0;c--)
        for(b=171;b>0;b--)
            for(a=16;a>0;a--);
}
void delay1ms(u16 num)
{
	u16 x,y;
	for(x=num; x>0; x--)
		for(y=110; y>0; y--)
		{
			;//??1ms
		}
}
void delay500ms(void)   //?? 0us
{
    unsigned char a,b,c;
    for(c=23;c>0;c--)
        for(b=152;b>0;b--)
            for(a=70;a>0;a--);
}
void delay250ms(void)   //误差 0us
{
    unsigned char a,b,c;
    for(c=11;c>0;c--)
        for(b=92;b>0;b--)
            for(a=122;a>0;a--);
}



/****************控制函数***********/
void judge_swit()
{
  if(up==0)
   {
     addx=0;
     if(addy!=-1)
        addy=1;
     else
        addy=-1;
   }
  if(down==0)
   {
     addx=0;
     if(addy!=1)
        addy=-1;
     else
        addy=1;
   }
  if(right==0)
   {
     addy=0;
     if(addx!=-1)
        addx=1;
     else
        addx=-1;
   }
  if(left==0)
   {
     addy=0;
     if(addx!=1)
        addx=-1;
     else
        addx=1;
   }

}


/***************是否相撞****************/
void judg_out()
{  
  u8 i;
  if(head_x[leng-1]>=8)
   {
         P0=0xff;
        
   }
}

/***************显示函数**************/
void display()
{
  u8 i,j;
  u8 x=0;
  u8 y=0;
  u8 w=0x01;
  for(j=0;j<8;j++)
    y_list[j]=0x00;
  for(i=leng-1;i>=1;i--)
    {

     x=head_x[i];
     y=head_y[i];
     switch(x)
          {
       case 0: y_list[0]=w<<y|y_list[0];
               w=0x01;
               break;
       case 1: y_list[1]=w<<y|y_list[1];
               w=0x01;
               break;
       case 2: y_list[2]=w<<y|y_list[2];
               w=0x01;
               break;
       case 3: y_list[3]=w<<y|y_list[3];
               w=0x01;
               break;
       case 4: y_list[4]=w<<y|y_list[4];
               w=0x01;
               break;
       case 5: y_list[5]=w<<y|y_list[5];
               w=0x01;
               break;
       case 6: y_list[6]=w<<y|y_list[6];
               w=0x01;
               break;
       case 7: y_list[7]=w<<y|y_list[7];
               w=0x01;
               break;
}
 
}

}
/****************主函数****************/
void main()
{
  u8 i,x,y;
 /***********中断初始化*********/
  TMOD|=0X01;//
  TL0 = 0x3C;
  TH0 = 0xF6;
  EA = 1;
  ET0 = 1;
  TR0 = 1;


  head_x[1]=0;
  head_y[1]=0;
  head_x[2]=0;
  head_y[2]=0;
  leng=3;
  poit_x=4;
  poit_y=4;
while(1)
{   
    x = rand()%7+1;
    y = rand()%7+1;

    if(!up||!right||!left||!down)
     judge_swit();         
    if(poit_x==head_x[1]+addx&&poit_y==head_y[1]+addy)
      {
        leng++; 
         poit_x=x;
         poit_y=y;  
      }
     else if(poit_x==head_x[1]-addx&&poit_y==head_y[1]-addy)
      {
        leng++;
        poit_x=x;
        poit_y=y;
      }
    // judg_out();
    head_x[1]=head_x[1]+addx;
    head_y[1]=head_y[1]+addy;
    
    for(i=leng-1;i>1;i--)
      {
        head_x[i]=head_x[i-1];
        head_y[i]=head_y[i-1];
      }
    display();
    delay250ms();

}


}
/*******************************************************************************
* 函数名         : Hc595SendByte(u8 dat)
* 函数功能		   : 向74HC595发送一个字节的数据
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void Hc595SendByte(u8 dat)
{
	u8 a;
	SRCLK=0;
	RCLK=0;
	for(a=0;a<8;a++)
	{
		SER=dat>>7;
		dat<<=1;

		SRCLK=1;
		_nop_();
		_nop_();
		SRCLK=0;	
	}

	RCLK=1;
	_nop_();
	_nop_();
	RCLK=0;
}
/****************显示函数**************/
void T0_int() interrupt 1
{
   u8 x,y;
   TL0=0x3C;
   TH0=0xF6;
   x = poit_x;
   y=poit_y;
   y_list[x-1]|=0x01<<y;
   P0=0x7f;
   P0=ledwei[k];   //??
   Hc595SendByte(y_list[z]);	//
			delay(100);		   //
			Hc595SendByte(0x00);  
   z++;
   k++;
	if(z==8)
    {
      z=0;
      k=0;
    }
}

