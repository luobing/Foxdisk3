//lbt.c
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>

#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"


void	main()
{
	/*
	InitVideo(0x103);
	getch();
	asm mov	ax,3
	asm int 0x10*/
	
	unsigned char *ptr=NULL;
	unsigned int i,j;
	unsigned int x=0;
	int x1,x2;
	SvgaDAC pal[10];
	SvgaDAC temp[256];
	
	memcpy(temp,sys_pal,sizeof(SvgaDAC)*256);
	x=initDisplay(0x103,sys_pal);
	x2=getpalette(0,10,pal);
	for(i=20;i<420;i+=20)
		for(j=20;j<420;j+=20)
		{
			rectangle(i,j,i+18,j+18,BLACK);
			if((i==20)||(j==20)||(i==400)||(j==400))
				rectblock(i+1,j+1,i+17,j+17,LIGHTGRAY);
			else
				rectblock(i+1,j+1,i+17,j+17,WHITE);
		}
	circle(480,100,30,BLACK);
	cirblock(480,100,29,LIGHTGRAY);
	cirblock(480,100,22,LIGHTBLACK);
	cirblock(480,100,20,WHITE);
	cirx(480,100,20,DARKBLACK,1);
	cirx(480,100,19,LIGHTGRAY,1);
	cirx(480,100,20,DARKBLACK,4);
	circle(476,101,3,BLACK);
	circle(476,100,7,BLACK);
	
	circle(480,320,30,BLACK);
	cirblock(480,320,29,LIGHTGRAY);
	cirblock(480,320,22,LIGHTBLACK);
	cirblock(480,320,20,WHITE);
	cirx(480,320,20,DARKBLACK,1);
	cirx(480,320,19,LIGHTGRAY,1);
	cirx(480,320,20,DARKBLACK,4);
	cirblock(479,320,7,BLACK);
	cirblock(479,321,2,WHITE);
	
	selFONT(ASC0|_ASC0|HZKK|_HZ24);
	putStr(602,72,"棋",DARKBLACK,4);
	putStr(600,70,"棋",WHITE,4);
	putStr(602,252,"魂",DARKBLACK,4);
	putStr(600,250,"魂",WHITE,4);
	
		/*
	selFONT(ASC0|_ASC0|HZKK|_HZ16);
	putStr(200,100,"1 如风而来,如雨而至,君如暖风煦煦,伴我夜寐晨醒.",GREEN,1);
	
	selFONT(ASC0|_ASC0|HZKK|_HZ24);
	putStr(200,150,"2 如风而来,如雨而至,君如暖风煦煦,伴我夜寐晨醒.",GREEN,1);
	selFONT(ASC0|_ASC0|HZKS|_HZ24);
	putStr(200,200,"3 如风而来,如雨而至,君如暖风煦煦,伴我夜寐晨醒.",GREEN,1);
	selFONT(ASC0|_ASC0|HZKF|_HZ24);
	putStr(200,250,"4 如风而来,如雨而至,君如暖风煦煦,伴我夜寐晨醒.",GREEN,1);
	selFONT(ASC0|_ASC0|HZKH|_HZ24);
	putStr(200,300,"5 如风而来,如雨而至,君如暖风煦煦,伴我夜寐晨醒.",GREEN,1);
	*/
	getch();
	for(j=0;j<64;j++)
	{
		for(i=0;i<256;i++)
		{
			if(temp[i].b>0)--temp[i].b;
			if(temp[i].g>0)--temp[i].g;
			if(temp[i].r>0)--temp[i].r;
		}
		delay(25);
		setpalette(0,10,temp);
	}
	//getch();
	asm mov	ax,3
	asm int 0x10
	
}