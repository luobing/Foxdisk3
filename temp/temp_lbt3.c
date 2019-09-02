//lbt.c
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>

#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"
#include "Painter.c"
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
	SvgaDAC pal[10];
	SvgaDAC temp[256];
	memcpy(temp,sys_pal,sizeof(SvgaDAC)*256);
	x=initDisplay(0x105,sys_pal);
	x2=getpalette(0,10,pal);

	iFacePic(72,64);
	iMessage(DARKPROJECT1);
	//putStr(400,300,"FoxDisk 2.01",WHITE,5);
	/*
	i=250;j=64;
	{
		navigationBar(00,i+j*5,(ASC0|_ASC0|HZKK|_HZ24),"    返回系统菜单",WHITE,1);
		navigationBar(00,i+j*4,(ASC0|_ASC0|HZKK|_HZ24),"     设 置 音 效",WHITE,1);
		navigationBar(00,i+j*3,(ASC0|_ASC0|HZKK|_HZ24),"     设置倒计时 ",WHITE,1);
		navigationBar(00,i+j*2,(ASC0|_ASC0|HZKK|_HZ24),"     取消倒计时 ",WHITE,1);
		navigationBar(00,i+j,(ASC0|_ASC0|HZKK|_HZ24),  "     设置缺省网 ",WHITE,1);
		navigationBar(00,i,(ASC0|_ASC0|HZKK|_HZ24),    "    设置时间显示",WHITE,1);  
	}
	getch();
	iFasceStr(1,GREEN);
	iFasceStr(2,RED);
	iFasceStr(3,BLACK);
	iFasceStr(4,YELLOW);
	iFasceStr(5,DEEPBLUE);
	iFaceClr(72);
	iMessage(GROUND1);*/
	i=250;
	j=80;
	{
		navigationBar(00,i+j*3,(ASC0|_ASC0|HZKK|_HZ16)," MacOSX 10  ",WHITE,2);
		navigationBar(00,i+j*2,(ASC0|_ASC0|HZKK|_HZ16),"Windows2000 ",WHITE,2);
		navigationBar(00,i+j,(ASC0|_ASC0|HZKK|_HZ16)," Windows XP ",WHITE,2);
		navigationBar(00,i,(ASC0|_ASC0|HZKK|_HZ16),"Fedora Core3",WHITE,2);  
	}
	rectangle(260,240,300,500,WHITE);
	getch();
	
	//rectblock(500-16*2,100-8,500-16*2+16*16,100+32+8,SURFACE1);
	//selFONT(ASC0|_ASC0|HZKK|_HZ16);
	//putStr(0,101,"Fedora Core3 ",LIGHTGRAY,2);
	//putStr(0,100,"Fedora Core3 ",WHITE,2);
	//putStr(0,142,"Fedora Core3 ",WHITE,1);
	/*
	chin=0;
	colorx=(DEEPBLUE<<8)|MGROUND1;
	while(chin!='1')
	{
		unsigned long temp;
		asm xor ax,ax
		asm int 0x16
		asm mov chin,al
		
		//for(j=(100-8);j<=(100+32+8);j++)
		//	for(i=(500-16*2);i<=(500-16*2+16*16);i++)
		//		if(getpixel(i,j)==SURFACE1)
		//			putpixel(i,j,RED);
		//		else if(getpixel(i,j)==RED)
		//			putpixel(i,j,SURFACE1);
		chgNavbar(00,i,(ASC0|_ASC0|HZKK|_HZ16),"Fedora Core3",colorx,2);
		temp=colorx;
		temp=(temp>>8)&0xff;
		colorx=((colorx<<8)&0xff00);
		colorx|=temp;
					
	}*/
	
				
	
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

void gearx(unsigned int cenx,unsigned int ceny,unsigned int radius)
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
