/* my new int 1ch */
/* 在本文件内不能使用高层次的如c_printf c_scanf等函数,会有问题的
   只能使用putStr或者更底层的显示函数 */
#include "DISKDRV.h"													/* 需要借用其中的一些宏定义 */
#include "Vesa.h"														/* 使用最底层的显示函数,加快处理速度 */
#include "Graphic.h"
#include "Font.h"
#include "Painter.h"
#include "INT1ch.h"
#include "Global.h"

void InitIntRes(unsigned int sw);
void RelIntRes(unsigned int sw);
void InitIntRes(unsigned int sw);
void InitCntDown(unsigned int times,unsigned long strcorlor,unsigned long bgcolor,unsigned int x,unsigned int y);


unsigned int getCountOver(void);

/* 初始化各种资源 */
void InitIntRes(unsigned int sw)
{
	ENABLE(sw);
	switch(sw)
	{
		case CLOCK  :
			_hms_x=maxX-95;					/* 设置显示时钟的位置 */
			_hms_y=maxY-35;
			break;
		case CURSOR :
			bg_curcolor=DEEPBLUE;
			curcolor=YELLOW;
			cur_flag=1;
			break;
		case CNTDOWN:
			break;
		default:
			break;
	}	
}
/* 初始化倒计时的各种资源 */
void InitCntDown(unsigned int times,unsigned long strcorlor,unsigned long bgcolor,unsigned int x,unsigned int y)
{
	unsigned int i,j;
	_cntd_x=x;					/* 坐标位置 */
	_cntd_y=y;
	cntd_color=strcorlor;
	cntd_bcolor=bgcolor;
	cntdown_times=times;
	cntdown_over=0;
	time_rad=0;
	for(j=0;j<CNTD_SHEI+1;j++)				/* 0~15 */
		for(i=0;i<CNTD_SWID+1;i++)
			cov_pci[j*(CNTD_SWID+1)+i]=(unsigned char)(getpixel(i+x,j+y));
}
/* 倒计时完成的标志 */
unsigned int getCountOver(void)
{
	return cntdown_over;
}

/* 在关闭各种需要时间中断处理的事件后,需要释放相应资源*/
void RelIntRes(unsigned int sw)
{
	DISABLE(sw);
	switch(sw)
	{
		case CLOCK  :
			break;
		case CURSOR :
			line(x_cur,y_cur,x_cur,y_cur+15,(unsigned long)(bg_curcolor));
			break;
		case CNTDOWN:
			break;
		default:
			break;
	}
}

/* 倒计时的处理 */
void _int_countDown(void)
{
	if((_sw_INT_&CNTDOWN) && (!cntdown_over))
	{
		asm cli
		if(time_rad<18)
			++time_rad;
		else
		{
			unsigned int i,j;
			unsigned long font;
			font=getFONT();
			time_rad=1;
			
			/* 清除上次显示 */
			for(j=0;j<CNTD_SHEI+1;j++)				/* 0~15 */
				for(i=0;i<CNTD_SWID+1;i++)
					putpixel(_cntd_x+i,_cntd_y+j,(unsigned long)(cov_pci[j*(CNTD_SWID+1)+i]));
			/* 显示当前计数时间 */
			i=cntdown_times/10;				/* 十位数 */
			j=(cntdown_times%10);			/* 个位数 */
			cntd_str[0]=i_number[i];
			cntd_str[1]=i_number[j];
			selFONT(ASC0|_ASC0|HZKK|_HZ16);	
			putStr(_cntd_x+1,_cntd_y+1,cntd_str,cntd_bcolor,2);
			putStr(_cntd_x,_cntd_y,cntd_str,cntd_color,2);
			if(cntdown_times==0)cntdown_over=1;
			--cntdown_times;
			selFONT(font);
			asm sti
		}
	}
}
void _int_Clock(void)
{
	if(_sw_INT_&CLOCK)
	{
		unsigned int temp;
		unsigned char temp1;
		unsigned long font;
		font=getFONT();
		asm cli
		asm mov ax,0x0200
		asm int 0x1a
		//asm mov ch,dh				/* 调试用 lbdebug*/
		asm jc clock_pass
		asm mov temp,cx
		asm mov temp1,dh
		if((oldhour_min!=temp) || (old_sec!=temp1))
		{
			oldhour_min=temp;
			old_sec=temp1;
			/* 已经获取了当前时钟,可以显示了 */
			i_when[0]=i_number[(oldhour_min>>12) &0x0f];
			i_when[1]=i_number[(oldhour_min>>8) &0x0f];
			i_when[3]=i_number[(oldhour_min>>4) &0x0f];
			i_when[4]=i_number[oldhour_min &0x0f];
			i_when[6]=i_number[(old_sec>>4) &0x0f];
			i_when[7]=i_number[old_sec&0x0f];
			i_when[8]='\0';
			rectblock(_hms_x,_hms_y,_hms_x+95-17,_hms_y+35-9,GROUND1);
			selFONT(ASC0|_ASC0|HZKK|_HZ16);				/* 作为一种有趣的特性,保留不选定吧^^*/
			putStr(_hms_x+6,_hms_y+5,i_when,WHITE,1);
			selFONT(font);
		}
		clock_pass:
		asm sti
	}
}
void _int_Cursor(void)
{
	#define onesecond 9							/*1/2秒的计数器必须为18的因数*/
	if(i_int_count<onesecond)
		{
			++i_int_count;
			return;
		}
	i_int_count=0;
	if(_sw_INT_&CURSOR)	
	{
		if(cur_flag)			/* 显示 */
		{
			cur_flag=0;
			line(x_cur,y_cur,x_cur,y_cur+15,(unsigned long)(curcolor));
		}
		else							/* 消除显示 */
		{
			cur_flag=1;
			line(x_cur,y_cur,x_cur,y_cur+15,(unsigned long)(bg_curcolor));
		}
	}
}
