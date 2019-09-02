//lbt.c
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>

#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"
#include "Painter.h"
/*sin45(o) ~ 45/64    */
/*sin22.5(o) ~ 49/128 */
/*cos22.5(o) ~ 59/64  */
void gearx(unsigned int cenx,unsigned int ceny,unsigned int radius);

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
	char chin;
	unsigned long colorx;
	//unsigned char itemx[4][]={"分 区 管 理 ","系统菜单命令","系统菜单排序","   退  出   "};
	SvgaDAC pal[10];
	SvgaDAC temp[256];
		
	MenuItem item[4];
	for(i=0;i<4;i++)
	{
		item[i].x=900;
		item[i].y=240+i*34;
		item[i].valid=1;
		item[i].msg_color=((DARKSHADOW1<<8)|WHITE);
		item[i].chg_color=((DEEPBLUE<<8)|GROUND1);
	}
	item[0].msg="分 区 管 理 ";
	item[1].msg="系统菜单命名";
	item[1].valid=0;
	item[2].msg="系统菜单排序";
	item[3].msg="   退  出   ";
	
	memcpy(temp,sys_pal,sizeof(SvgaDAC)*256);
	x=initDisplay(0x105,sys_pal);
	x2=getpalette(0,10,pal);

	//iFacePic(72,64);
	//iMessage(DARKPROJECT1);
	
	i=250;
	j=80;
	{
		navigationBar(00,i+j*3,(ASC0|_ASC0|HZKK|_HZ16)," MacOSX 10  ",WHITE,2);
		navigationBar(00,i+j*2,(ASC0|_ASC0|HZKK|_HZ16),"Windows2000 ",WHITE,2);
		navigationBar(00,i+j,(ASC0|_ASC0|HZKK|_HZ16)," Windows XP ",WHITE,2);
		navigationBar(00,i,(ASC0|_ASC0|HZKK|_HZ16),"Fedora Core3",WHITE,2);  
	}
	
	iWindowOut(320,120,"系统菜单-分区管理",14);
	iWindowOut(320,120+10*24+32+16,"系统菜单-菜单排序",6);
	{
		
		for(i=0;i<10;i++)
		{
			setCur(328,160+i*24);
			c_printf(WHITE,"当前所属编辑框的行数: %d",i);
		}
	}
	
	{
		unsigned long x1;
		unsigned int x2;
		unsigned char buf[50];
		setCur(328,120+10*24+32+16+40);
		c_scanf(WHITE,BLACK,"inPut number: %x",&x2);
			setCur(328,120+10*24+32+16+40+24);
		c_printf(WHITE,"number=%x",x2);
		//putASCII(328,450,0x31,RED,1);
		//getKbin(WHITE,BLACK,0x0e00,buf); 
		
	}
	//getch();
	
	/*rectangle(320,120,890,700,WHITE);
	selFONT(ASC0|_ASC0|HZKK|_HZ16);
	for(i=0;i<24;i++)
		putStr(328,128+i*24,"分分分分分分分分分分分分分分分分分分分分分分分分分分分分分分分分分分分分",WHITE,1);
	*/
	{
		unsigned int	xx=1;
		for(i=0;i<4;i++)
			if(item[i].valid==1)
				iItem(item[i].x,item[i].y,item[i].msg,((item[i].msg_color)&0xff));
			else
				iItem(item[i].x,item[i].y,item[i].msg,(((item[i].msg_color)>>8)&0xff));
		chin=0;
		i=0;
		
		while(chin!=0x31)
		{
			unsigned long colorx;
			asm xor ax,ax
			asm int 0x16
			asm mov chin,al
			if(i==0)j=3;
			else j=i-1;
			colorx=(((item[j].chg_color)>>8)&0xff)+(((item[j].chg_color)<<8)&0xff00);
			iItemSel(item[i].x,item[i].y,item[i].msg,item[i].chg_color);
			iItemSel(item[j].x,item[j].y,item[j].msg,colorx);
			i++;
			if(i==4)i=0;
			iScrollBar(320,120,xx,14);
			xx++;
			if(xx==14)xx=0;
			iWindowSlide(320,120,0,9,1);
		}		
	}
	//getch();
				
	
	for(j=0;j<64;j++)
	{
		for(i=0;i<256;i++)
		{
			if(temp[i].b>0)--temp[i].b;
			if(temp[i].g>0)--temp[i].g;
			if(temp[i].r>0)--temp[i].r;
		}
		delay(25);
		setpalette(0,20,temp);
	}
	//getch();
	asm mov	ax,3
	asm int 0x10
	
}
