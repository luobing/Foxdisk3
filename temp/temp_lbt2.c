//lbt.c
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>

#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"

void draw_gear(unsigned int cenx,unsigned int ceny,unsigned int radius);
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
	draw_gear(400,400,128);
	/*
	selFONT(ASC0|_ASC0|HZKK|_HZ16);
	putStr(202,102,"WinXP  Fedora 3 ",LIGHTGRAY,2);
	putStr(200,100,"WinXP  Fedora 3 ",WHITE,2);
	
	selFONT(ASC0|_ASC0|HZKK|_HZ24);
	putStr(202,152,"ＷｉｎｘｐＦｅｄｏｒａ３",LIGHTGRAY,2);
	putStr(200,150,"ＷｉｎｘｐＦｅｄｏｒａ３",WHITE,2);
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

void draw_gear(unsigned int cenx,unsigned int ceny,unsigned int radius)
{
	unsigned int x0,y0,rad1,rad2,rad3,rev1,rev2;
	
	rad1=(radius-(radius>>2));
	rad2=(rad1>>1);
	rad3=rad2>>2;
	rev1=(radius+(radius>>2));
	rev2=((rev1*45)>>6);						/*sin45~45/64*/
	
	cirblock(cenx,ceny,radius,WHITE);
	cirblock(cenx,ceny,rad1,BLACK);
	cirblock(cenx,ceny,rad2,WHITE);
	
	/*对于(2^(1/2))/2,用 45/64来替代  误差约为0.003981781186547524400844362104*/
	circle(cenx,ceny-rev1,rad2,WHITE);
	cirblock(cenx,ceny-rev1,rad2-2,DEEPBLUE);
	circle(cenx,ceny-rev1,rad2,DEEPBLUE);
	circle(cenx,ceny-rev1,rad2+1,DEEPBLUE);
	delay(200);
	
	circle(cenx+rev2,ceny-rev2,rad2,WHITE);
	cirblock(cenx+rev2,ceny-rev2,rad2-2,DEEPBLUE);
	circle(cenx+rev2,ceny-rev2,rad2,DEEPBLUE);
	circle(cenx+rev2,ceny-rev2,rad2+1,DEEPBLUE);
	delay(200);
	
	circle(cenx+rev1,ceny,rad2,WHITE);
	cirblock(cenx+rev1,ceny,rad2-2,DEEPBLUE);
	circle(cenx+rev1,ceny,rad2,DEEPBLUE);
	circle(cenx+rev1,ceny,rad2+1,DEEPBLUE);
	delay(200);
	
	circle(cenx+rev2,ceny+rev2,rad2,WHITE);
	cirblock(cenx+rev2,ceny+rev2,rad2-2,DEEPBLUE);
	circle(cenx+rev2,ceny+rev2,rad2,DEEPBLUE);
	circle(cenx+rev2,ceny+rev2,rad2+1,DEEPBLUE);
	delay(200);
	
	circle(cenx,ceny+rev1,rad2,WHITE);
	cirblock(cenx,ceny+rev1,rad2-2,DEEPBLUE);
	circle(cenx,ceny+rev1,rad2,DEEPBLUE);
	circle(cenx,ceny+rev1,rad2+1,DEEPBLUE);
	delay(200);
	
	circle(cenx-rev2,ceny+rev2,rad2,WHITE);
	cirblock(cenx-rev2,ceny+rev2,rad2-2,DEEPBLUE);
	circle(cenx-rev2,ceny+rev2,rad2,DEEPBLUE);
	circle(cenx-rev2,ceny+rev2,rad2+1,DEEPBLUE);
	delay(200);
	
	circle(cenx-rev1,ceny,rad2,WHITE);
	cirblock(cenx-rev1,ceny,rad2-2,DEEPBLUE);
	circle(cenx-rev1,ceny,rad2,DEEPBLUE);
	circle(cenx-rev1,ceny,rad2+1,DEEPBLUE);
	delay(200);
	
	circle(cenx-rev2,ceny-rev2,rad2,WHITE);
	cirblock(cenx-rev2,ceny-rev2,rad2-2,DEEPBLUE);
	circle(cenx-rev2,ceny-rev2,rad2,DEEPBLUE);
	circle(cenx-rev2,ceny-rev2,rad2+1,DEEPBLUE);
	delay(200);
}