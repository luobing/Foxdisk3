#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"

#include "Common.h"
#include "Painter.h"
#include "INT1ch.h"
#include "BootMenu.h"
#include "DISKDRV.h"
#include "DISKAPP.h"

#include "Global.h"

void getPicRGB(SvgaDAC *pal,int pal_num,iWallPaper *wp,int nwp);
void cpPIC2GM(iWallPaper *wp,int nwp);
void bootMeun(FOXPARA *para,OS *my_os);

void bootMeun(FOXPARA *para,OS *my_os)
{
	int i,j,nwp=0,hitkey=-1;/* temp[4],*/
	SvgaDAC pal[256];
	
	for(i=0;i<256;i++)
	{
		pal[i].b=0;
		pal[i].g=0;
		pal[i].r=0;
	}
	initDisplay(0x105,pal);				/* 初始化图形显示 */
	
	if(para->cntdown_time==-1)return;					/* 如果没有倒计时则直接进入foxdisk的主处理部分 */
	CreateDiskParaTable();			/* 建立硬盘信息表 */
	/* 按照para中提供的信息显示壁纸,如果没有壁纸,0号为我自己使用的壁纸 */
	if((para->wallpaper!=-1) &&((unsigned int)(para->wallpaper) < para->wpaper_num))
		nwp=para->wallpaper;
	else
		nwp=6;
	getPicRGB(pal,256,para->iwapaper,nwp);
	cpPIC2GM(para->iwapaper,nwp);
	SvgaSmoothOn(pal);
	/*
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if(my_os[j].bar_index==(unsigned char)(i))
				temp[i]=j;	*/
	setCur(para->cntdown_x+1,para->cntdown_y+1);
	c_printf(bbg_color,bootStr[0]);		
	setCur(para->cntdown_x,para->cntdown_y);
	c_printf(bmsg_color,bootStr[0]);
	setCur(para->cntdown_x+1,para->cntdown_y+1+20);
	c_printf(bbg_color,bootStr[1],my_os[para->def_os].bar_index+1,my_os[para->def_os].os_name);		
	setCur(para->cntdown_x,para->cntdown_y+20);
	c_printf(bmsg_color,bootStr[1],my_os[para->def_os].bar_index+1,my_os[para->def_os].os_name);
	setCur(para->cntdown_x+1,para->cntdown_y+1+48);
	c_printf(bbg_color,bootStr[2]);		
	setCur(para->cntdown_x,para->cntdown_y+48);
	c_printf(bmsg_color,bootStr[2]);
	
	InitIntRes(CNTDOWN);			/* 使能倒计时 */
	InitCntDown(para->cntdown_time,bmsg_color,bbg_color,para->cntdown_x+((c_strlen(bootStr[2])+2)<<3),para->cntdown_y+40);
	enable1CH();

	while((hitkey==-1) && (!getCountOver()))
	{
		asm mov ax,0x0100
		asm int 0x16
		asm jz wait_hitdown					/* 没有键按下 */
		asm xor ax,ax
		asm int 0x16
		asm mov hitkey,ax
	wait_hitdown:
	}
	disable1CH();
	RelIntRes(CNTDOWN);
	
	if(hitkey == KEY_F2)
	{
		SvgaSmoothOff(pal);
		return;			/*F2 进入系统菜单 */
	}
	bootOS(my_os,para->def_os,cntd_bcolor,cntd_color,pal);			/* 引导进入当前系统 */
	//...引导进入当前系统 
	//if(getCountOver())				/* 倒计时完毕则进入当前网 */
	//{
	//}
	
}
/* 获取指定数目的颜色寄存器 */
/*入口参数: pal:颜色寄存器首址  pal_num:需要获取的总数(必须为256) */
/*          wp:壁纸的首址  nwp:第几张壁纸(0~5)*/
void getPicRGB(SvgaDAC *pal,int pal_num,iWallPaper *wp,int nwp)
{
	int i,index_g,index_r,index_b;
	unsigned long temp1;
	unsigned int maxc,tempg,tempb,tempr,green=0,blue=0,red=0;
	unsigned long bgcolor=0;
	/* 初始化 */
	for(i=0;i<pal_num;i++)
	{
		pal[i].b=0;
		pal[i].g=0;
		pal[i].r=0;
	}
	if(linear_ReadWDK(wp[nwp].lba_addr,2,pal))return;
	/* 为倒计时显示的颜色作准备 */ 
	for(i=0;i<pal_num;i++)
	{
		temp1=(unsigned long)(pal[i].b)+(unsigned long)(pal[i].g)+(unsigned long)(pal[i].r);
		if(temp1>bgcolor)
		{
			bgcolor=temp1;
			bbg_color=(unsigned long)(i);
		}
		
		tempg=((unsigned int)(pal[i].g)<<2)-(unsigned int)(pal[i].b)-(unsigned int)(pal[i].r);
		tempb=((unsigned int)(pal[i].b)<<2)-(unsigned int)(pal[i].g)-(unsigned int)(pal[i].r);
		tempr=((unsigned int)(pal[i].r)<<2)-(unsigned int)(pal[i].b)-(unsigned int)(pal[i].g);
			
		if(tempg>green)
		{
			green=tempg;
			index_g=i;
		}
		if(tempb>blue)
		{
			blue=tempb;
			index_b=i;
		}
		if(tempr>red)
		{
			red=tempr;
			index_r=i;
		}
	}
	tempg=((unsigned int)(pal[index_g].g)<<2)-(unsigned int)(pal[index_g].b)-(unsigned int)(pal[index_g].r);
	tempb=((unsigned int)(pal[index_b].b)<<2)-(unsigned int)(pal[index_b].g)-(unsigned int)(pal[index_b].r);
	tempr=((unsigned int)(pal[index_r].r)<<2)-(unsigned int)(pal[index_r].b)-(unsigned int)(pal[index_r].g);
	maxc=((tempg>tempb)?tempg:tempb);
	i=((tempg>tempb)?index_g:index_b);
	i=((maxc>tempr)?i:index_r);
	bmsg_color=(unsigned long)(i);
	if(bmsg_color==bbg_color)
	{
		pal[254].b=(255>>2);					
		pal[254].g=(255>>2);
		pal[254].r=(255>>2);
		pal[255].r=(29>>2);
		pal[255].g=(120>>2);
		pal[255].b=(19>>2);
		bmsg_color=255;
		bbg_color=254;
	}
}
/* 将数据拷贝到显存中 */
void cpPIC2GM(iWallPaper *wp,int nwp)
{
	unsigned int i,j;
	unsigned char buffer[1024];
	for(j=0;j<=maxY;j++)
 	{
 		//setbank(j);
 		/* 读取1024字节个数据到缓冲区 */
 		linear_ReadWDK(wp[nwp].lba_addr+1l*(j<<1)+2,2,buffer);
 		for(i=0;i<1024;i++)
 			putpixel(i,maxY-j,buffer[i]);
 	}
}



