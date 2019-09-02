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
	
	//unsigned char *ptr=NULL;
	unsigned int i,j;
	unsigned int x=0;
	int x1,x2;
	SvgaDAC pal[10];
	SvgaDAC temp[256];
	
	memcpy(temp,sys_pal,sizeof(SvgaDAC)*256);
	x=initDisplay(0x103,sys_pal);
	//x2=getpalette(0,10,pal);
	
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