//lbt.c
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>

#include "Vesa.h"
#include "Graphic.h"



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

	//for(i=0;i<10;i++)
	//	rectblock(100,100+i*40,700,140+i*40,i);
	//rectangle(99,99,701,541,WHITE);
	//circle(600,600,20,WHITE);
	//rectblock(400,400,500,600,WHITE);
	//circle(400,400,20,WHITE);
	//putpixel(170,110,WHITE);
	//cirx(700,100,20,WHITE,1);
	rectblock(140,140,200,200,WHITE);
	rectcirblock(440,440,500,500,WHITE,1);
	cirx(700,100,20,WHITE,1);
	cirx(700,160,20,WHITE,4);
	cirx(600,160,20,WHITE,3);
	cirx(600,100,20,WHITE,2);
	//circle(600,600,50,BLACK);
	//rectcirblock(40,40,240,100,WHITE);
	//rectcirblock(40,40,241,81,WHITE);
	getch();
	SetWriteMode(XOR_PUT);
	
	//rectblock(400,400,500,600,WHITE);
	//circle(600,600,20,WHITE);
	//circle(400,400,20,WHITE);
	rectblock(140,140,200,200,WHITE);
	rectcirblock(440,440,500,500,WHITE,1);
	cirx(700,100,20,WHITE,1);
	cirx(700,160,20,WHITE,4);
	cirx(600,160,20,WHITE,3);
	cirx(600,100,20,WHITE,2);
	getch();
	SetWriteMode(PUT);
	//rectblock(400,400,500,600,WHITE);
	//circle(600,600,20,WHITE);
	//circle(400,400,20,WHITE);
	rectblock(140,140,200,200,WHITE);
	rectcirblock(440,440,500,500,WHITE,1);
	cirx(700,100,20,WHITE,1);
	cirx(700,160,20,WHITE,4);
	cirx(600,160,20,WHITE,3);
	cirx(600,100,20,WHITE,2);
	getch();
	SetWriteMode(XOR_PUT);
	//rectblock(400,400,500,600,WHITE);
	//circle(600,600,20,WHITE);
	//circle(400,400,20,WHITE);
	rectblock(140,140,200,200,WHITE);
	rectcirblock(440,440,500,500,WHITE,1);
	cirx(700,100,20,WHITE,1);
	cirx(700,160,20,WHITE,4);
	cirx(600,160,20,WHITE,3);
	cirx(600,100,20,WHITE,2);
	getch();
	SetWriteMode(PUT);
	//rectblock(400,400,500,600,WHITE);
	//circle(600,600,20,WHITE);
	//circle(400,400,20,WHITE);
	rectblock(140,140,200,200,WHITE);
	rectcirblock(440,440,500,500,WHITE,1);
	cirx(700,100,20,WHITE,1);
	cirx(700,160,20,WHITE,4);
	cirx(600,160,20,WHITE,3);
	cirx(600,100,20,WHITE,2);
	getch();
	SetWriteMode(XOR_PUT);
	//rectblock(400,400,500,600,WHITE);
	//circle(600,600,20,WHITE);
	//circle(400,400,20,WHITE);
	rectblock(140,140,200,200,WHITE);
	rectcirblock(440,440,500,500,WHITE,1);
	cirx(700,100,20,WHITE,1);
	cirx(700,160,20,WHITE,4);
	cirx(600,160,20,WHITE,3);
	cirx(600,100,20,WHITE,2);
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
	printf("x=%d x1=%d x2=%d\n",x,x1,x2);
	printf("NO.  Blue  Green  Red  Alpha\n");
	for(i=0;i<10;i++)
	{
		printf(" %d",i);
		printf("   %03d",pal[i].b);
		printf("   %03d",pal[i].g);
		printf("    %03d",pal[i].r);
		printf("   %03d",pal[i].alpha);
		printf("\n");
	}
	printf("vbeinfo.VBEVersion=%04x\n",vbeinfo.VBEVersion);
	printf("maxX=%d,maxY=%d",maxX,maxY);
	/*
	getVBEInfo(&vbeinfo);
	getModeInfo(0x103,&modeinfo);
	
	ptr=(unsigned char*)(&vbeinfo);
	for(i=0;i<16;i++)
		printf("%02x ",ptr[i]);
	printf("\n");
	printf("vbeinfo.Capabilities=%04x\n",vbeinfo.Capabilities);
	ptr=(unsigned char*)(&modeinfo);
	for(i=0;i<16;i++)
		printf("%02x ",ptr[i]);
	printf("\nXRes=%d,YRes=%d,WinBAttr=%02x,WinGran=%04x",modeinfo.XRes,modeinfo.YRes,modeinfo.WinBAttr,modeinfo.WinGran);
	printf("\nWinASeg=%04x,WinBSeg=%04x",modeinfo.WinASeg,modeinfo.WinBSeg);
*/
	//printf("length VBEInfo: %d,length ModeInfoBlock:%d\n",sizeof(VBEInfo),sizeof(ModeInfoBlock));
}