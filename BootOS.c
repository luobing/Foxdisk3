/* bootOS.c */
#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"

#include "Common.h"
#include "Painter.h"
#include "INT1ch.h"

#include "DISKDRV.h"
#include "DISKAPP.h"

#include "Global.h"
#include "iMem.h"
void bootOS(OS *os,int nos,unsigned long bg,unsigned long fore,SvgaDAC *pal);
void bootAbExit(void);
int canBootOS(OS *os,int nos);

void bootOS(OS *os,int nos,unsigned long bg,unsigned long fore,SvgaDAC *pal)
{
	unsigned int i,iLoadLen,winx=380,winy=320;
	char password[13];
	unsigned char buffer[512];
	unsigned char mbr0[512];
	unsigned char far *ptr=(unsigned char far *)0x00007c00;
	if(os[nos].pwd_valid)			/* 设置了系统密码 */
	{
		enable1CH();
		rectblock(winx,winy,winx+(30<<3),winy+24*4,bg);
		rectangle(winx,winy,winx+(30<<3),winy+32,fore);
		rectangle(winx,winy+32,winx+(30<<3),winy+24*4,fore);
		Pic_Wing(winx+8,winy+7,bg,fore,0);
		setCur(winx+8+8+16,winy+8);
		c_printf(fore,bootos_str,os[nos].os_name);
		setCur(winx+8,winy+40);
		c_printf(fore,bootos_str1);
		while(1)
		{
			setCur(winx+8+8*8,winy+40+24);
			for(i=0;i<13;i++)password[i]=0;
			c_scanf(fore,bg,bootos_str2,password);
			if(!(c_strncmp(os[nos].pwd,password,c_strlen(os[nos].pwd))||(c_strlen(os[nos].pwd)!=c_strlen(password))))
				break;				/* 密码一致 */	
		}
		disable1CH();
	}
	/* 根据系统信息进行引导 */
	/* 1 提取当前系统信息 */
	iLoadLen = FP_OFFS(end_load)-FP_OFFS(beg_load);
	for(i=0;i<512;i++)buffer[i]=0;
	for(i=0;i<512;i++)mbr0[i]=0;
	if(linear_ReadWDK(os[nos].os_mbr, 1, buffer))
		bootAbExit();						/* 意外错误处理机制 */

	if(linear_ReadWDK(0, 1, mbr0))
		bootAbExit();						/* 意外错误处理机制 */
	/* 2 比较要引导的系统和当前mbr的系统分区表是否一致*/
	if(imemcmp(buffer+iLoadLen,mbr0+iLoadLen,512-iLoadLen))			/* 除去引导程序外全部比较 */
	{/* 不相同则将 要引导的系统 的信息写入MBR0*/
		imemcpy(mbr0+iLoadLen,buffer+iLoadLen,512-iLoadLen -2);
		if(linear_WriteWDK(0, 1, mbr0))
			bootAbExit();	
	}
	
	if(linear_ReadWDK(os[nos].os_mbr, 1, buffer))
		bootAbExit();						/* 意外错误处理机制 */
	/*3 lbdebug*/
	/*
	{
		int m,n;
		rectblock(0,0,1024,16*16,GROUND1);
		for(m=0;m<16;m++)
			for(n=0;n<32;n++)
			{
				setCur(n*32,m*16);
				c_printf(WHITE,debug1,buffer[m*32+n]);
			}
		setCur(0,16*16);
		c_printf(YELLOW,debug2,os[nos].os_mbr);
		c_printf(YELLOW,debug2,os[3].os_mbr+1);
		c_printf(YELLOW,debug2,iLoadLen);
		asm xor ax,ax
		asm int 0x16
	}*/
	if((buffer[0]=='S') && (buffer[1]=='u') && (buffer[2]=='n') && (buffer[3]=='0'))
		if(linear_ReadWDK(os[3].os_mbr+1, 1, buffer))		/* 如果没有引导程序,启用备用微软引导程序 */
			bootAbExit();						/* 意外错误处理机制 */
	imemcpy(buffer+iLoadLen,mbr0+iLoadLen,512-iLoadLen-2);
	/*4 lbdebug*/
	/*
	{
		int m,n;
		rectblock(0,0,1024,16*16,GROUND1);
		for(m=0;m<16;m++)
			for(n=0;n<32;n++)
			{
				setCur(n*32,m*16);
				c_printf(WHITE,debug1,buffer[m*32+n]);
			}
		setCur(0,16*16);
		c_printf(YELLOW,debug2,os[nos].os_mbr);
		c_printf(YELLOW,debug2,os[3].os_mbr+1);
		c_printf(YELLOW,debug2,iLoadLen);
		asm xor ax,ax
		asm int 0x16
	}*/
	SvgaSmoothOff(pal);				/* 开始消除显示 */
	/* 3 直接引导,将当前系统引导信息拷贝到0x7c00处 */ 
	for(i=0;i<512;i++)
		ptr[i]=buffer[i];
	asm{ 
  	sti 
  	mov ax,0003h
  	int 10h
  	xor	ax,ax
  	mov	bx,ax
  	mov	cx,ax
  	mov	dx,ax
  	mov	bp,ax
  	mov	si,ax
  	mov	di,ax
  	mov	ds,ax
  	mov	es,ax
  	mov	SP,7c00h
  	db 0eah     				/*jmp 0000:7c00h*/
  	dw 7c00h
  	dw 0000h
	}
	
}
/* 判断系统是否能够引导 */
/*入口参数:os系统信息表 nos(0~4):系统编号 */
/*出口参数: 0:可以启动 非0:不能启动,错误表见DISKAPP.h分区的错误 */
int canBootOS(OS *os,int nos)
{
	/* 系统能够启动,通过几点判断 */
	/* 1 是否有有效分区表 */
	/* 2 是否有活动分区(不检测)*/
	/* 3 是否有启动MBR,如果没有的话,则试着启用Foxdisk自带的微软引导代码 */
	/* 获取当前硬盘的分区 */
	PARTN partnArr[MAX_PARTN_ELEM];				/* 存储当前系统的分区数组 */
	int err=0;
	CreateDiskParaTable();
	err=Get_Disk_Info(CUR_DK,partnArr,os[nos].os_mbr,TRUE);
	if(os[nos].os_valid==0)err|=0x80;
	if(g_Info.fd.used<2)err|=0x80;			/* 占用此标志80为我自己的标志:分区没有划分 */
	
	return err;
}
void bootAbExit(void)
{
}