/* Font.c:利用已经有的画线、画点函数构造设计字符显示的函数 */

#include "Vesa.h"											/* 与硬件相关的图形处理函数 */
#include "Graphic.h"									/* 抽象出来与硬件无关的图形函数 */
#include "Font.h"
#include "Global.h"
#include "iMath.h"

void selFONT(unsigned long font);
void	putStr(unsigned int x,unsigned int y,unsigned char *s,unsigned long color,unsigned char mag);

void	putASCII(unsigned int x,unsigned int y,unsigned char code,unsigned long color,unsigned char mag);
void	putHZ12(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
void	putHZ16(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
void	putHZ24(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
void	putHZ40(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
void	putHZ48(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
void	(* putHZ)(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
void	selHZK24(char mode);
void	selHZK12(char mode);
void	selHZK16(char mode);
void	selHZK40(char mode);
void	selHZK48(char mode);
void (* selHZK)(char mode);


/*功能：显示ASCII字符*/
/*入口参数：x,y:坐标   code:ASCII码   color：颜色*/
/*          mag:magnification放大倍数*/
/*出口参数：无*/
void	putASCII(unsigned int x,unsigned int y,unsigned char code,unsigned long color,unsigned char mag)
{
	int	i,j,index,m,n;
	
	for(i=0;i<ASCIIMAX;i++)
		if(ascii_table[i].code == code)
			break;
	index = i;
	for(j=0;j<CHARHEIGHT;j++)
		for(i=0;i<CHARWIDTH;i++)
		{
			if(ascii_table[index].array[j] & (1<<(CHARWIDTH-i)))
				for(n=0;n<mag;n++)
					for(m=0;m<mag;m++)
						putpixel(x+mag*i+m,y+mag*j+n,color);	
		}
}	
/* 选定需要显示的24x24汉字库 */
/*入口参数: mode: 's' 'k' 'h' 't' 'f'*/
/*出口参数: 无 */
void	selHZK16(char mode)
{
	/* 16x16 的只有一种字体 在显示程序中使用HZCOUNT和固定的HZHEIGHT HZWIDTH来运行*/
	return;
}
/*功能：显示汉字16X16字库			*/
/*入口参数：x,y:坐标   code:汉字   color：颜色*/
/*          mag:magnification放大倍数*/
/*出口参数：无																*/
void	putHZ16(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag)
{
	int	i,j,index,m,n;
	unsigned int zimo;
		
	for(i=0;i<hzcount;i++)							/* 为加快显示速度,提取的汉字数目已经给出 */
		if(hzk16_table[i].code == code)
			break;
			
	index = i;
	if(index == hzcount) return;
	for(j=0;j<HZHEIGHT;j++)
	{
		zimo=hzk16_table[index].array[j];
		asm mov ax,zimo;
		asm xchg	al,ah
		asm mov zimo,ax
		
		for(i=0;i<HZWIDTH;i++)
			if((zimo>>(HZWIDTH-i))&0x01)
				for(n=0;n<mag;n++)
					for(m=0;m<mag;m++)
						putpixel(x+mag*i+m,y+mag*j+n,color);	
	}
}

/*功能：显示汉字24X24字库			*/
/*入口参数：x,y:坐标   code:汉字   color：颜色*/
/*          mag:magnification放大倍数*/
/*出口参数：无																*/
/*注意,显示汉字前，必须使用selHZK24选定相应的显示字体*/
void	putHZ24(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag)
{
	int	i,j,index,k,m,n;
	unsigned long zimo;

	/*1 寻找对应汉字*/
	for(i=0;i<hz24_count;i++)
		if(hzk24_table[i].code==code)
			break;
	index=i;
	if((index == hz24_count)||(hz24_count==-1) )return;

	/*2 显示*/
	for(j=0;j<HZ24HEIGHT;j++)
	{
		zimo=0;
		
		for(k=0;k<HZ24HEIGHT;k++)
		{
			if(((hzk24_table[index].array[(k*3)+(j>>3)])>>(7-(j%8)))&0x01)
				zimo|=1;
			zimo<<=1;	
		}
	
		for(i=0;i<HZ24WIDTH;i++)
			if(lintshr(HZ24WIDTH-i,zimo)&0x01)	/*if((zimo>>(HZ24WIDTH-i))&0x01)*/
				for(n=0;n<mag;n++)
					for(m=0;m<mag;m++)
						putpixel(x+mag*i+m,y+mag*j+n,color);
	}
}
/* 选定需要显示的24x24汉字库 */
/*入口参数: mode: 's' 'k' 'h' 't' 'f'*/
/*出口参数: 无 */
void	selHZK24(char mode)
{
	switch(mode)
	{
	#ifdef HZK24S_ACTIVE	/* 宋体 */
		case 's':
    case 'S':
          hz24_count=HZK24SCOUNT;
          hzk24_table=HZK24Stable;
          break;
  #endif
  #ifdef HZK24K_ACTIVE
    case 'k':
    case 'K':
          hz24_count=hzk24kcount;
          hzk24_table=HZK24Ktable;
          break;
  #endif
  #ifdef HZK24F_ACTIVE
    case 'f':
    case 'F':
          hz24_count=HZK24FCOUNT;
          hzk24_table=HZK24Ftable;
          break;
  #endif
  #ifdef HZK24T_ACTIVE
    case 't':
    case 'T':
          hz24_count=HZK24TCOUNT;
          hzk24_table=HZK24Ttable;
          break;
  #endif
  #ifdef HZK24H_ACTIVE
    case 'h':
    case 'H':
          hz24_count=HZK24HCOUNT;
          hzk24_table=HZK24Htable;
          break;
	#endif
	default:
		hz24_count=-1;  
		break;	
	}
}
/*    显示字符串	*/
/*入口参数：x,y:坐标   *s:字符串   color：颜色	*/
/*          mag:magnification放大倍数*/
/*出口参数：无	*/
/*注意,显示汉字前，必须使用selFONT选定相应的显示字体*/
void	putStr(unsigned int x,unsigned int y,unsigned char *s,unsigned long color,unsigned char mag)
{
	unsigned int hzwidth,hzheight;
	unsigned int charwidth,charheight;
	unsigned int i=0,hz=0;
	/* 对于英文字符，现在只有一种,如果需要添加的话,要修改此处 */
	switch(selfont&0xff)
	{
		/*case _HZ12:							/* 12x16 *//*
			selHZK=selHZK12;
			putHZ=putHZ12;
			hzwidth=hzheight=12;
			break;*/
		case _HZ16:							/* 16x16 */
			selHZK=selHZK16;
			putHZ=putHZ16;
			hzwidth=hzheight=16;
			break;
		case _HZ24:							/* 24x24 */
			hzwidth=hzheight=24;
			selHZK=selHZK24;
			putHZ=putHZ24;
			break;
		/*case _HZ40:    					/* 40x40 *//*
			hzwidth=40;
			selHZK=selHZK40;
			putHZ=putHZ40;
			break;
	  case _HZ48:							/* 48x48 *//*
	  	hzwidth=48;
	  	selHZK=selHZK48;
			putHZ=putHZ48;
	  	break;*/
	  default:
	  	break;
	}
	switch((selfont)&0x0000ff00)
	{
		case HZKK:
			selHZK('k');
			break;
		case HZKS:
			selHZK('s');
			break;
		case HZKF:
			selHZK('f');
			break;
		case HZKH:
			selHZK('h');
			break;
		default:
			break;
	}
	/* 对英文的处理暂时不在此实现,以后再添加吧 */
	charwidth=8;					/* 使用 8x16字符 */
	charheight=16;
	
	while(*(s+i))
	{
		if(s[i]<128)
		{
			putASCII(x,(y+(hzheight-charheight)*mag),s[i],color,mag);
			i++;
			x+=(charwidth*mag);		
		}
		else
		{
			hz=(unsigned int)(s[i+1]);
			hz<<=8;
			hz+=(unsigned int)(s[i]);
			putHZ(x,y,hz,color,mag);
			i+=2;
			x+=(hzwidth*mag);
		}
	}
}
/* 选定需要显示的字体(汉字和英文) */
/*入口参数: 见 头文件中宏定义说明*/
/*出口参数: 无 */
void selFONT(unsigned long font)
{
	selfont=0;							/* 将以前的显示模式取消 */
	selfont=font;
}
/* 返回当前字体模式 */
unsigned long getFONT(void)
{
	return selfont;
}
