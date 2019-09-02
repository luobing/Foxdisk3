/* Painter.c:利用已经有的画线、画点函数构造设计图案需要的所有元素 */
#include "Vesa.h"											/* 与硬件相关的图形处理函数 */
#include "Graphic.h"									/* 抽象出来与硬件无关的图形函数 */
#include "Font.h"
#include "Common.h"
#include "Painter.h"
#include "INT1ch.h"
#include "Global.h"
#include "iMath.h"

void gear(unsigned int cenx,unsigned int ceny,unsigned int radius);
unsigned int	c_strlen(unsigned char *s);
unsigned int	c_strlen_ascii(unsigned char *s);
int c_strncmp(unsigned char *str1,unsigned char *str2,unsigned int count);
void	navigationBar(unsigned int x,unsigned int y,unsigned long font,unsigned char *s,unsigned long color,unsigned char mag);
void chgNavbar(unsigned int x,unsigned int y,unsigned long font,unsigned char *s,unsigned long color,unsigned char mag);
void iFacePic(unsigned int ibottom,unsigned int iright);
void	iFasceStr(unsigned char turn,unsigned long color);
void	iFaceClr(unsigned int ibottom);
void iMessage(unsigned long color);
void	iItem(unsigned int x,unsigned int y,unsigned char *s,unsigned long color);
void iItemSel(unsigned int x,unsigned int y,unsigned char *s,unsigned long color);

void	iWindowOut(unsigned int x,unsigned int y,unsigned char *wstr,unsigned int lines);
void Pic_Wing(unsigned int x,unsigned int y,unsigned int color_in,unsigned int color_out,unsigned char direct);
void iScrollBar(unsigned int winx,unsigned int winy,unsigned int num_line,unsigned int lines);
void	iWindowSlide(unsigned int winx,unsigned int winy,unsigned int minline,unsigned int maxline,unsigned char up_down);

void _i2asc(unsigned int num,int base,unsigned char *out);
void _l2asc(unsigned long num,int base,unsigned char *out);
void c_printf(unsigned long color, const char *format,...);
void c_scanf(unsigned long bg_color,unsigned long color,const char *format,...);
void chgLine(unsigned int x,unsigned int y,unsigned int num_line,unsigned long color);
unsigned long getKbin(unsigned long bg_color,unsigned long color,unsigned int num_base,unsigned char *out);
void setCur(unsigned int x,unsigned int y);
unsigned int getXCur(void);
unsigned int getYCur(void);



/* 程序界面的主体 */
/*入口参数: 无  */
/*出口参数: 无  */
/*备注: 如果使用双倍8x16 的英文字符 起始 y=250,dy=80 */
/*      如果使用24x24汉字 起始y=250,dy=64*/
void iFacePic(unsigned int ibottom,unsigned int iright)
{
	/* 下部宽度16字符长*/
	unsigned int i,j,m,n;
		
	circle(160,160,72+5,BLACK);	
	circle(160,160,72+4,LIGHTSHADOW1);	
	circle(160,160,72+3,SURFACE1);
	circle(160,160,72+2,SURFACE1);
	circle(160,160,72+1,DARKSHADOW1);	
	circle(160,160,72,DARKPROJECT1);
	
	circle(260,112,54+5,BLACK);	
	circle(260,112,54+4,LIGHTSHADOW1);	
	circle(260,112,54+3,SURFACE1);
	circle(260,112,54+2,SURFACE1);
	circle(260,112,54+1,DARKSHADOW1);	
	circle(260,112,54,DARKPROJECT1);

	
	rectblock(200,101,236,200,GROUND1);			/*13字符*16 =208 208-8*/
	rectblock(233,112,320,174,GROUND1);			/*112+54=166 */	
	putpixel(202,100,DARKPROJECT1);
	putpixel(202,99,SURFACE1);
	line(203,98,203,96,SURFACE1);						/*微调*/
	gear(160,160,64);
	gear(260,112,48);
	rectblock(220,200,225,207,GROUND1);
	line(220,200,220,maxY-ibottom,DARKPROJECT1);
	line(220-1,200+4,220-1,maxY-ibottom-1,DARKSHADOW1);
	line(220,maxY-ibottom,0,maxY-ibottom,DARKPROJECT1);
	line(220-2,maxY-ibottom-1,0,maxY-ibottom-1,DARKSHADOW1);
	line(220-2,200+5,220-2,maxY-ibottom-1-40,SURFACE1);				/* 下面高度为40*/
	line(220-3,200+6,220-3,maxY-ibottom-1-40-1,SURFACE1);
	line(220-4,200+7,220-4,maxY-ibottom-1-40-2,LIGHTSHADOW1);
	line(220-5,200+8+7,220-5,maxY-ibottom-1-40-3,BLACK);
	line(220-5,maxY-ibottom-1-40-3,0,maxY-ibottom-1-40-3,BLACK);
	line(220-4,maxY-ibottom-1-40-2,0,maxY-ibottom-1-40-2,LIGHTSHADOW1);
	rectblock(0,maxY-ibottom-1-40-1,220-2,maxY-ibottom-2,SURFACE1);
	
	line(maxX-iright,0,maxX-iright,112,DARKPROJECT1);
	line(260+54,112,maxX-iright,112,DARKPROJECT1);
	line(260+54+1,112-1,maxX-iright-1,112-1,DARKSHADOW1);
	line(260+54+2,112-2,maxX-iright-2,112-2,SURFACE1);
	line(260+54+3,112-3,maxX-iright-2,112-3,SURFACE1);
	line(260+54+4,112-4,maxX-iright-32-1,112-4,LIGHTSHADOW1);			/* 右边宽度32（两个个汉字字符长）*/
	line(260+54+5,112-5,maxX-iright-32-2,112-5,BLACK);
	line(maxX-iright-32-2,112-5,maxX-iright-32-2,0,BLACK);
	line(maxX-iright-32-1,112-4,maxX-iright-32-1,0,LIGHTSHADOW1);	
	rectblock(maxX-iright-32,112-4,maxX-iright-2,0,SURFACE1);
	line(maxX-iright-1,112-1,maxX-iright-1,0,DARKSHADOW1);
	
	rectblock(0,0,maxX-iright-32-2-1,112-(54+5)-1,MGROUND1);
	rectblock(maxX-iright-32-2-1,112-5-1,260+54+6,112-(54+5)-1,MGROUND1);
	/* 弧形区域填充 */
	for(j=52;j<=106;j++)
	{
		for(i=320;i>112;i--)
			if(getpixel(i,j)==BLACK)break;
			line(i+1,j,320,j,MGROUND1);
	}
	/*miny=52 maxy=237
	maxx=260*/
	for(j=52;j<=237;j++)
	{
		for(i=0;i<260;i++)
			if(getpixel(i,j)==BLACK)break;
			line(0,j,i-1,j,MGROUND1);
	}
	rectblock(0,238,220-6,maxY-ibottom-1-40-4,MGROUND1);
	/*maxx=220-6  maxy=237
	  miny=210*/
	for(j=216;j<=237;j++)
	{
		for(i=220-6;i>160;i--)
			if(getpixel(i,j)==BLACK)break;
			line(i+1,j,220-6,j,MGROUND1);
	}
	/*miny=83  maxy=94
	maxx=210*/
	for(j=83;j<=94;j++)
	{
		i=210;
		while(getpixel(i,j)!=BLACK)
		{
			i--;
		}
		m=i;
		i-=1;
		while(getpixel(i,j)!=BLACK)
		{
			i--;
		}
		line(i+1,j,m-1,j,MGROUND1);
	}
	iFasceStr(0,WHITE);
	
}
/* 显示任务栏的字符 */
/*入口参数: turn:0 显示所有字符 1,2,3,4,5: 显示第x个字符串 */
/*          color字符串的颜色 */
/*出口参数: 无*/
void	iFasceStr(unsigned char turn,unsigned long color)
{
	unsigned int m,n,len,j;
	unsigned int len1,len2,len3,len4;
	m=c_strlen(iFaceStr1);						/* 总数量 */
	n=c_strlen_ascii(iFaceStr1);			/* ascii的数量 */
	len1=(n<<3)+(m-n)*12;
	m=c_strlen(iFaceStr2);						/* 总数量 */
	n=c_strlen_ascii(iFaceStr2);			/* ascii的数量 */
	len2=(n<<3)+(m-n)*12;
	m=c_strlen(iFaceStr3);						/* 总数量 */
	n=c_strlen_ascii(iFaceStr3);			/* ascii的数量 */
	len3=(n<<3)+(m-n)*12;
	m=c_strlen(iFaceStr4);						/* 总数量 */
	n=c_strlen_ascii(iFaceStr4);			/* ascii的数量 */
	len4=(n<<3)+(m-n)*12;
	m=c_strlen(iFaceStr);					/* 总共的字符数目 */
	n=c_strlen_ascii(iFaceStr);		/* ascii码字符数目*/
	len=((n+3)<<3)+((m-n)*12);		/* 右边预留3个ASCII字符 */
	selFONT(ASC0|_ASC0|HZKK|_HZ24);
	switch(turn)
	{
		case 0:
			/* 显示 iFaceStr */
			line(maxX-len,maxY-48,maxX,maxY-48,WHITE);	/* 上面预留半个汉字字符高度,下面预留半个汉字字符高度*/
			cirx(maxX-len,maxY-48+16,16,WHITE,2);
			line(maxX-len-16,maxY-48+16,maxX-len-16,maxY,WHITE);
			putStr(maxX-len,maxY-48+12,iFaceStr,color,1);
			j=8+3;				/* 00.00.00 8个字符,右边预留的三个字符*/
			line(maxX-((j+1)<<3),maxY-48+12,maxX-(2<<3),maxY-48+12,DARKPROJECT1);
			line(maxX-(2<<3),maxY-48+12,maxX-(2<<3),maxY-8,LIGHTSHADOW1);
			line(maxX-(2<<3),maxY-8,maxX-((j+1)<<3),maxY-8,LIGHTSHADOW1);
			line(maxX-((j+1)<<3),maxY-7,maxX-((j+1)<<3),maxY-48+12,DARKPROJECT1);
			rectblock(maxX-((j+1)<<3)+1,maxY-48+12+1,maxX-(2<<3)-1,maxY-8-1,GROUND1);
		break;
		case 1:
			putStr(maxX-len,maxY-48+12,iFaceStr1,color,1);
			break;
		case 2:
			putStr(maxX-len+len1,maxY-48+12,iFaceStr2,color,1);
			break;
		case 3:
			putStr(maxX-len+len1+len2,maxY-48+12,iFaceStr3,color,1);
			break;
		case 4:
			putStr(maxX-len+len1+len2+len3,maxY-48+12,iFaceStr4,color,1);
			break;
		case 5:
			putStr(maxX-len+len1+len2+len3+len4,maxY-48+12,iFaceStr5,color,1);
			break;
		default:
			break;
		
	}
}
/* 清除系统菜单位置的图象，为下一个菜单作准备*/
/*入口参数: ibottom: 离底部的位置,与iFacePic中的相应参数必须相同 */
/*出口参数: 无*/
void	iFaceClr(unsigned int ibottom)
{
	rectblock(0,238,220-6,maxY-ibottom-1-40-4,MGROUND1); 
	line(220-5,238,220-5,maxY-ibottom-1-40-4,BLACK);
	line(220-4,238,220-4,maxY-ibottom-1-40-4,LIGHTSHADOW1);
	line(220-3,238,220-3,maxY-ibottom-1-40-4,SURFACE1);
	line(220-2,238,220-2,maxY-ibottom-1-40-4,SURFACE1);
	line(220-1,238,220-1,maxY-ibottom-1-40-4,DARKSHADOW1);
	line(220,238,220,maxY-ibottom-1-40-4,DARKPROJECT1);
	rectblock(220+1,238,220+1+30,maxY-ibottom-1-40-4,GROUND1);  /*注意,超出部分不得大于30象素*/
}
/* 显示程序信息 */
/*入口参数: color颜色*/
/*出口参数: 无*/
void iMessage(unsigned long color)
{
	if(color!=GROUND1)
	{
		selFONT(ASC0|_ASC0|HZKK|_HZ24);
		putStr(400-1,300,ifoxdisk,LIGHTSHADOW1,5);
		putStr(400+1,300,ifoxdisk,BLACK,5);
		putStr(400,300+1,ifoxdisk,SURFACE1,5);
		putStr(400,300,ifoxdisk,color,5);
		putStr(530-1,424,iflag,LIGHTSHADOW1,1);
		putStr(530+1,424,iflag,BLACK,1);
		putStr(530,424,iflag,color,1);
	}   
	else
	{
		rectblock(400-1,300,400+5*8*12,424+24,GROUND1);
	}
}
/* 齿轮的绘制 */
/*入口参数: cenx,ceny:齿轮的圆心 radius:齿轮最外围半径 */
/*出口参数: 无 */
void gear(unsigned int cenx,unsigned int ceny,unsigned int radius)
{
	/*sin45(o) ~ 45/64    */
	/*sin22.5(o) ~ 49/128 */
	/*cos22.5(o) ~ 59/64  */
	#define out_color	BLACK
	#define clr_color bkGColor
	#define outB_color	LIGHTGRAY				/* 最外围齿轮颜色 */
	#define midB1_color	MLIGSHADOW1			/* 最外圈颜色 */
	#define midB2_color	DARKBLACK				/* 中圈颜色 	*/
	#define inB_color		LIGHTBLACK		  /* 最内圈颜色 */
	#define inBl_color	WHITE
	
	unsigned int r4_1,r8_1,r8_7,r4_1_1,r8_1_1,r8_7_1;
	unsigned int rev1,rev2,minx,maxx,maxy,x,y,lx,rx,i,flag;
	rev1=(radius-(radius>>2));
	r4_1=((radius*45)>>6);			/* rsin45   */
	r8_1=((radius*49)>>7);			/* rsin22.5 */
	r8_7=((radius*59)>>6);			/* rcos22.5 */
	
	r4_1_1=((rev1*45)>>6);			/* rsin45   */
	//r4_1_1=((rev1*91)>>7);
	r8_1_1=((rev1*49)>>7);			/* rsin22.5 */
	r8_7_1=((rev1*59)>>6);			/* rcos22.5 */
	
	/*1 8条线 */
	line(cenx,ceny-radius,cenx,ceny+radius,out_color);
	line(cenx+r8_1,ceny-r8_7,cenx-r8_1,ceny+r8_7,out_color);
	line(cenx+r4_1,ceny-r4_1,cenx-r4_1,ceny+r4_1,out_color);
	line(cenx+r8_7,ceny-r8_1,cenx-r8_7,ceny+r8_1,out_color);
	line(cenx+radius,ceny,cenx-radius,ceny,out_color);	
	line(cenx+r8_7,ceny+r8_1,cenx-r8_7,ceny-r8_1,out_color);
	line(cenx+r4_1,ceny+r4_1,cenx-r4_1,ceny-r4_1,out_color);
	line(cenx+r8_1,ceny+r8_7,cenx-r8_1,ceny-r8_7,out_color);
	/*2 外围圆 */
	circle(cenx,ceny,radius,out_color);
	circle(cenx,ceny,rev1,out_color);
	
	/*3 清除 */
	line(cenx,ceny-rev1,cenx,ceny+rev1,clr_color);
	line(cenx+rev1,ceny,cenx-rev1,ceny,clr_color);	
	/*4 填充第一个区块 */
	minx=cenx-r8_1;
	maxx=cenx;
	maxy=ceny-(r8_7_1);
	y=ceny-radius;
	while(y<maxy)
	{
		if(y<=(ceny-r8_7))
		{
			for(x=maxx;x>minx;x--)
			{
				if(getpixel(x,y)==out_color)
				{
					rx=x;
					break;
				}
			}
			for(x=minx;x<maxx;x++)
			{
				if(getpixel(x,y)==out_color)
				{
					lx=x;
					break;
				}
			}
		}
		else
		{
			for(x=minx;x<maxx;x++)
			{
				if(getpixel(x,y)==out_color)
				{
					lx=x;
					break;
				}
			}
			for(x=maxx;x>minx;x--)
			{
				if(getpixel(x,y)==out_color)
				{
					rx=x;
					break;
				}
			}
		}
		for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel(i,y,outB_color);
		for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel((cenx<<1)-i,(ceny<<1)-y,outB_color);
		y++;
	}
	/* 第二个区块*/
	minx=cenx+r8_1_1;
	maxx=cenx+r4_1;
	maxy=ceny-r4_1_1;
	y=ceny-r8_7;
	while(y<maxy)
	{
		if(y>(ceny-r4_1))
		{
			for(x=maxx;x>minx;x--)
			{
				if(getpixel(x,y)==out_color)
				{
					rx=x;
					break;
				}
			}
			for(x=minx+1;x<maxx;x++)
			{
				if(getpixel(x,y)==out_color)
				{
					lx=x;
					break;
				}
			}
		}
		else
		{
			for(x=minx;x<maxx;x++)
			{
				if(getpixel(x,y)==out_color)
				{
					lx=x;
					break;
				}
			}
			for(x=maxx;x>minx;x--)
			{
				if(getpixel(x,y)==out_color)
				{
					rx=x;
					break;
				}
			}
		}
		for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel(i,y,outB_color);
		for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel((cenx<<1)-i,(ceny<<1)-y,outB_color);
		y++;
	}
	lx=minx;
	rx=maxx;
	y=ceny-r4_1-1;
	for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel(i,y,outB_color);
	for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
		putpixel((cenx<<1)-i,(ceny<<1)-y,outB_color);
	
	line(minx,ceny-r4_1-1,maxx,ceny-r4_1-1,outB_color);
	/* 第三个区块*/
	minx=cenx+r8_7_1;
	maxx=cenx+radius;
	maxy=ceny+1;
	y=ceny-r8_1;
	while(y<maxy)
	{
		for(x=maxx;x>minx;x--)
		{
			if(getpixel(x,y)==out_color)
			{
				rx=x;
				break;
			}
		}
		for(x=minx+1;x<maxx;x++)
		{
			if(getpixel(x,y)==out_color)
			{
				lx=x;
				break;
			}
		}
			
		for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel(i,y,outB_color);
		for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel((cenx<<1)-i,(ceny<<1)-y,outB_color);
		y++;
	}
	
	/* 第四个区块*/
	minx=cenx+r4_1_1;
	maxx=cenx+r8_7;
	maxy=ceny+r4_1;
	y=ceny+r8_1_1;
	while(y<maxy)
	{
		for(x=minx+1;x<maxx;x++)
		{
			if(getpixel(x,y)==out_color)
			{
				lx=x;
				break;
			}
		}
		for(x=maxx;x>minx;x--)
		{
			if(getpixel(x,y)==out_color)
			{
				rx=x;
				break;
			}
		}
		for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel(i,y,outB_color);
		for(i=lx;i<=rx;i++)			/*注意各种模式下的情况*/
			putpixel((cenx<<1)-i,(ceny<<1)-y,outB_color);
		y++;
	}
	/*5 开始画圆 */
	circle(cenx,ceny,radius,clr_color);

	cirblock(cenx,ceny,rev1,midB1_color);
	cirblock(cenx,ceny,rev1-(rev1>>3),midB2_color);
	cirblock(cenx,ceny,(rev1-(rev1>>3))>>1,inB_color);
	circle(cenx,ceny,((rev1-(rev1>>3))>>1)-1,midB2_color);
	rev2=((rev1-(rev1>>3))>>3);
	cirblock(cenx,ceny-(rev2<<1)-(rev2<<2),rev2,inBl_color);
	cirblock(cenx+(rev2<<1)+(rev2<<2),ceny,rev2,inBl_color);
	cirblock(cenx,ceny+(rev2<<1)+(rev2<<2),rev2,inBl_color);
	cirblock(cenx-(rev2<<1)-(rev2<<2),ceny,rev2,inBl_color);

}
/* 导航条绘制 */
/*入口参数: x,y:坐标 font:字体 s:字符串 color:颜色 mag:倍率*/
/*出口参数: 无 */
/*备注: 在iFacePic中留下的导航起始位置为y=250 */
void	navigationBar(unsigned int x,unsigned int y,unsigned long font,unsigned char *s,unsigned long color,unsigned char mag)
{
	/* 提供两种字体选择,为了便于显示对齐,不在此函数中处理*/
	/* 显示时候左右各空余一个字符宽度,上下空域各1/4高度 */
	/* 为了显示效果,与iFacePic配套使用 反射阴影为220-6长度*/
	unsigned int charhei,charwid;				/* 字符的高度和宽度 */
	unsigned int height,length;					/* 显示字符串的框的长和宽*/
	
	switch(font&0x0f)
	{
		case _HZ16:		
			charwid=8*mag;
			charhei=16*mag;
				break;
		case _HZ24:
			charwid=12*mag;
			charhei=24*mag;
				break;
		default:
			break;
	}
	length=((c_strlen(s)+2)*charwid);
	height=(charhei>>1)+charhei;
	line(x,y,(x+4+charwid+length),y,BLACK);
	line(x,y+1,(x+2+charwid+length),y+1,LIGHTSHADOW1);
	rectblock(x,y+2,(x+2+charwid+length),y+2+(charhei>>2),SURFACE1);
	line(x,y+3+(charhei>>2),x+length+1,y+3+(charhei>>2),DARKSHADOW1);
	line(x,y+4+(charhei>>2),x+length+1,y+4+(charhei>>2),BLACK);
	rectblock(x,y+5+(charhei>>2),x+length,y+5+(charhei>>2)+height,MGROUND1);
	line(x,y+5+(charhei>>2)+height+1,x+length+1,y+5+(charhei>>2)+height+1,BLACK);
	line(x,y+5+(charhei>>2)+height+2,x+length+2,y+5+(charhei>>2)+height+2,LIGHTSHADOW1);
	rectblock(x,y+5+(charhei>>2)+height+3,(x+2+charwid+length),y+5+(charhei>>2)+height+3+(charhei>>2),SURFACE1);
	line(x,y+(charhei>>1)+height+9,(x+3+charwid+length),y+(charhei>>1)+height+9,DARKSHADOW1);
	line(x,y+(charhei>>1)+height+10,(x+4+charwid+length),y+(charhei>>1)+height+10,BLACK);
	
	line(x+length+1,y+5+(charhei>>2),x+length+1,y+5+(charhei>>2)+height,BLACK);
	line(x+length+2,y+3+(charhei>>2),x+length+2,y+6+(charhei>>2)+height,LIGHTSHADOW1);
	rectblock(x+length+3,y+3+(charhei>>2),x+length+3+charwid,y+7+(charhei>>2)+height,SURFACE1);
	line(x+length+charwid+3,y+1,x+length+charwid+3,y+(charhei>>1)+height+9,DARKSHADOW1);
	line(x+length+charwid+4,y+1,x+length+charwid+4,y+(charhei>>1)+height+10,BLACK);
	line(0,y+(charhei>>1)+height+11,220-6,y+(charhei>>1)+height+11,DARKPROJECT1);
	line(0,y+(charhei>>1)+height+12,220-6,y+(charhei>>1)+height+12,DARKPROJECT1);  
	
	selFONT(font);
	putStr(x+charwid+1,y+(charhei>>1)+5,s,LIGHTGRAY,mag);
	putStr(x+charwid,y+(charhei>>1)+4,s,color,mag);

}
/* 导航条的选定颜色变换 */
/*入口参数: x,y坐标,font:字体 s:字符串 */
/*          color:高8位为变换后的颜色  低8位为初始颜色 mag:字体倍率*/
/*出口参数: 无 */
/*备注:     颜色由高字 低字共同表示 ,调用的时候注意点*/
void chgNavbar(unsigned int x,unsigned int y,unsigned long font,unsigned char *s,unsigned long color,unsigned char mag)
{
	/* 提供两种字体选择,为了便于显示对其,不在此函数中处理*/
	/* 显示时候左右各空余一个字符宽度,上下空域各1/4高度 */
	unsigned int charhei,charwid;				/* 字符的高度和宽度 */
	unsigned int height,length;					/* 显示字符串的框的长和宽*/
	unsigned int i,j;
	unsigned long src_color,dst_color;
	
	src_color=(color&0xff);
	dst_color=(lintshr(8,color)&0xff);
	/*dst_color=((color>>8)&0xff);*/
	
	switch(font&0x0f)
	{
		case _HZ16:		
			charwid=8*mag;
			charhei=16*mag;
				break;
		case _HZ24:
			charwid=12*mag;
			charhei=24*mag;
				break;
		default:
			break;
	}
	length=((c_strlen(s)+2)*charwid);
	height=(charhei>>1)+charhei;
	for(j=y+5+(charhei>>2);j<=y+5+(charhei>>2)+height;j++)
		for(i=x;i<=x+length;i++)
			if(getpixel(i,j)==src_color)
				putpixel(i,j,dst_color);
}

/* 菜单项绘制 */
/*入口参数: x,y:坐标  s:字符串 color:颜色 */
/*出口参数: 无 */
/*备注: 固定为16x16字体显示 */
/*      每个菜单项高度为 32*/
void	iItem(unsigned int x,unsigned int y,unsigned char *s,unsigned long color)
{
	unsigned length,height;
	
	/*圆角形选框架 字体固定为16x16汉字 或者字符*/
	length=((c_strlen(s)+2)<<3);		/* 左右共预留两个字符宽 */
	height=16+16;										/* 上下共预留1个字符高 */
	rectarc(x,y,x+length,y+height,WHITE);
	selFONT(ASC0|_ASC0|HZKK|_HZ16);
	putStr(x+8,y+8,s,color,1);
}

/* 菜单项的选定颜色变换 */
/*入口参数: x,y坐标, s:字符串 */
/*          color:高8位为变换后的颜色  低8位为初始颜色 */
/*出口参数: 无 */
/*备注:     颜色由高字 低字共同表示 ,调用的时候注意点*/
void iItemSel(unsigned int x,unsigned int y,unsigned char *s,unsigned long color)
{
	unsigned long src_color;
	unsigned long dst_color;
	unsigned int startx,maxx,i,j;
	startx=x+8;
	maxx=x+((c_strlen(s)+2)<<3);
	src_color=(color&0x0ff);
	dst_color=(lintshr(8,color)&0xff);
	/*dst_color=((color>>8)&0xff);*/
	for(j=y+1;j<(y+16+16);j++)
		for(i=startx;i<maxx;i++)
			if(getpixel(i,j)==src_color)
				putpixel(i,j,dst_color);
	for(j=y+1;j<(y+16+16);j++)
		for(i=startx;i>x;i--)
			if(getpixel(i,j)==src_color)
				putpixel(i,j,dst_color);
	
}
/* 输出窗口 */
/*入口参数: x,y:坐标 wstr:标题栏字符 lines:准备显示的行数*/
/*出口参数: 无*/
/*备注 编辑区:(x+1,y+33,x+570-25,y+height-1)*/
/*     字符串打印起始:(x+8,y+40)*/
void	iWindowOut(unsigned int x,unsigned int y,unsigned char *wstr,unsigned int lines)
{
	/*33列10行 汉字型窗口 （最大10行）*/
	/*宽度固定为571*/
	unsigned int height,i,j,sx,sy;
	if(lines>10)
		height=(10*24+8+16+16);
	else
		height=(lines*24+8+16+16);
	rectangle(x,y,x+570,y+16+16,WHITE);
	rectblock(x+1,y+1,x+570-1,y+16+16-1,SURFACE1);
	selFONT(ASC0|_ASC0|HZKK|_HZ16);
	Pic_Wing(x+9,y+7,DARKPROJECT1,BLACK,0);
	Pic_Wing(x+8,y+7,DARKPROJECT1,WHITE,0);
	putStr(x+8+24,y+8,wstr,WHITE,1);
	
	rectangle(x,y+16+16,x+570,y+height,WHITE);
	rectangle(x+570-24,y+height-24,x+570,y+height,WHITE);
	rectblock(x+570-24+1,y+height-24+1,x+570-1,y+height-1,SURFACE1);
	sx=x+570-24+4;
	sy=y+height-24+4;
	for(j=16;j>0;j-=2)
	{
			for(i=0;i<j;i++)
				putpixel(sx+i,sy,WHITE);
			sx++;
			sy+=2;
	}
	rectangle(x+570-24,y+16+16,x+570,y+16+16+24,WHITE);
	rectblock(x+570-24+1,y+16+16+1,x+570-1,y+16+16+24-1,SURFACE1);
	sx=x+570-24+4;
	sy=y+16+16+24-4;
	for(j=16;j>0;j-=2)
	{
			for(i=0;i<j;i++)
				putpixel(sx+i,sy,WHITE);
			sx++;
			sy-=2;
	}
	line(x+570-24,y+16+16+24,x+570-24,y+height-24,WHITE);
	rectblock(x+1,y+33,x+570-25,y+height-1,GROUND1);
	rectblock(x+570-24+1,y+16+16+24+1,x+570-1,y+height-24-1,GROUND1);
	
	if(lines<=10)
	{
		rectblock(x+570-24+2,y+58,x+570-2,y+height-26,SURFACE1);
		/* x+570-24+2=x+548*/
		line(x+548,y+58,x+568-1,y+58,WHITE);
		line(x+568,y+58,x+568,y+height-26,DARKPROJECT1);
		line(x+567,y+height-26,x+548,y+height-26,DARKPROJECT1);
		line(x+548,y+height-27,x+548,y+59,WHITE);
	}
	else
	{
		iScrollBar(x,y,0,lines);
	}
	/*Scroll Bar: x+570-24+1+1,x+570-1-1  y+58,y+height-26*/
	//
	//line(x+570-24+2,y+58,x+570-2-1,y+58,WHITE);
	//line(x+570-24+2,y+58,x+570-24+2,y+height-86,WHITE);
	//line(x+570-2,y+58,)
	
}
/* 滚动条的绘制 */
/*入口参数; winx,winy:窗口的起始坐标 num_line:行号(0~lines-1),lines:总行数 */
/*出口参数: 无 */
void iScrollBar(unsigned int winx,unsigned int winy,unsigned int num_line,unsigned int lines)
{
	/* 173=10*24+8+16+16-26-58 -24+1 */
	/* 高度 24 宽度21 */
	unsigned int startx,starty;
	unsigned long dividend,divisor;			/*被除数与除数*/
	if(lines<=10)
		return;
	if(num_line>=lines)
		return;
	startx=winx+548;
	starty=winy+58;
	dividend=lintmul(173,lintshl(10,num_line));
	/*dividend=((unsigned long)(num_line)<<10)*173;*/
	divisor=(lines-1);
	starty+=lintshr(10,lintdiv(divisor,dividend));
	/*starty+=((dividend/divisor)>>10);*/
	rectblock(winx+548,winy+58,winx+568,winy+((lines>10)?10:lines)*24+8+16+16-24-1,GROUND1);
	rectblock(startx,starty,startx+20,starty+23,SURFACE1);
	line(startx,starty,startx+20-1,starty,WHITE);
	line(startx+20,starty,startx+20,starty+23,DARKPROJECT1);
	line(startx+20-1,starty+23,startx,starty+23,DARKPROJECT1);
	line(startx,starty+22,startx,starty+1,WHITE);
	//asm mov ax,3
	//asm int 0x10
	//printf("winy=%d ",winy+58);
	//printf("starty=%d",starty);
	//getch();
}
/* 窗口编辑框中信息显示的滑动 */
/*入口参数: winx,winy:窗口的起始坐标,minline,maxline:滑动的范围(从0开始计行的,0~9)
            up_down:向上滑还是向下滑 0:向上 1:向下 */
/*出口参数: 无 */
void	iWindowSlide(unsigned int winx,unsigned int winy,unsigned int minline,unsigned int maxline,unsigned char up_down)
{
	unsigned startx,starty,i,j;
	unsigned char buffer[538];
	if(up_down==1)
	{
		startx=winx+8;			/* 针对当前iWindow的起始位置 */
		starty=winy+40+(minline*24);
		
		for(j=winy+40+(maxline-1)*24+16;j>=starty;j--)
		{
			for(i=0;i<538;i++)
				buffer[i]=getpixel(startx+i,j);
			for(i=0;i<538;i++)
				putpixel(startx+i,j+24,buffer[i]);
		}
		rectblock(startx,starty,startx+537,starty+16,GROUND1);
	}
	else if(up_down==0)
	{
		for(j=winy+40+(minline+1)*24;j<=winy+40+(maxline*24)+16;j++)
		{
			for(i=0;i<538;i++)
				buffer[i]=getpixel(winx+8+i,j);
			for(i=0;i<538;i++)
				putpixel(winx+8+i,j-24,buffer[i]);
		}
		rectblock(winx+8,winy+40+(maxline*24),winx+8+537,winy+40+(maxline*24)+16,GROUND1);
	}
	
}

void Pic_Wing(unsigned int x,unsigned int y,unsigned int color_in,unsigned int color_out,unsigned char direct)
{
	unsigned int i,j;
	unsigned char buffer[18];
	unsigned char flag,firp,endp;
	
	for(i=0;i<17;i++)
	{
		if(direct==0)			//普通方向，翅膀向左展开
		{
			for(j=0;j<8;j++) buffer[j]=(((wing[i][0])>>(7-j))&0x01);						//初始化
			for(j=8;j<16;j++)buffer[j]=(((wing[i][1])>>(8+7-j))&0x01);
			buffer[16]=(((wing[i][2])>>7)&0x01);
			buffer[17]=(((wing[i][2])>>6)&0x01);
		}
		else if(direct==1)//翅膀向右展开
		{
			buffer[0]=(((wing[i][2])>>6)&0x01);
			buffer[1]=(((wing[i][2])>>7)&0x01);
			for(j=2;j<2+8;j++) buffer[j]=(((wing[i][1])>>(j-2))&0x01);
			for(j=2+8;j<2+8+8;j++) buffer[j]=(((wing[i][0])>>(j-2-8))&0x01);
		}
		else if(direct==2)		//翅膀朝下，向左展开
		{
			for(j=0;j<8;j++) buffer[j]=(((wing[16-i][0])>>(7-j))&0x01);						//初始化
			for(j=8;j<16;j++)buffer[j]=(((wing[16-i][1])>>(8+7-j))&0x01);
			buffer[16]=(((wing[16-i][2])>>7)&0x01);
			buffer[17]=(((wing[16-i][2])>>6)&0x01);
		}
		else if(direct==3)//翅膀朝下，向右展开
		{
			buffer[0]=(((wing[16-i][2])>>6)&0x01);
			buffer[1]=(((wing[16-i][2])>>7)&0x01);
			for(j=2;j<2+8;j++) buffer[j]=(((wing[16-i][1])>>(j-2))&0x01);
			for(j=2+8;j<2+8+8;j++) buffer[j]=(((wing[16-i][0])>>(j-2-8))&0x01);
		}
		flag=0;
		for(j=0;j<18;j++)																							//搜寻图象计算
		{
			if((buffer[j]==1)&&(flag==0))
			{
				flag=1;
				firp=j;
			}
			else if((buffer[j]==1)&&(flag>=1))
			{
				flag++;
				endp=j;
			}
		}
		if(endp==(firp+1))
			flag=1;
		if(flag==1)
		{
			for(j=0;j<17;j++)
				if(buffer[j]==0x01)buffer[j]=0x03;										//外围象素点			
		}
		else if(flag>=1)
		{
			
			for(j=0;j<18;j++)
				if(buffer[j]==0x01)buffer[j]=0x03;										//外围象素点	
			for(j=firp+1;j<endp;j++)
				if(buffer[j]==0x00)buffer[j]=0x02;										//内部象素点
		}
		for(j=0;j<18;j++)
		{
			switch(buffer[j])
			{
				case 0x02:
					putpixel(x+j,y+i,color_in);
					break;
				case 0x03:
					putpixel(x+j,y+i,color_out);
					break;
				default:
					break;
			}
		}
	}
	
}
/* 获取字符串中所有字符的个数,汉字算两个字符 */
unsigned int	c_strlen(unsigned char *s)
{
	unsigned int	i = 0;
	while(*s++) i++;
	return	i;
}

/* 获取字符串中ASCII码的个数 */
unsigned int	c_strlen_ascii(unsigned char *s)
{
	unsigned int	i = 0;
	/*while((*s++)<0xa0)i++;*/
	while(*s)
	{
		if(*s<0xa0)i++;
		s+=1;
	}		
	return	i;
}
/* 比较两个字符串是否相同 */
/*<0 str1小于str2   =0 str1等于str2  >0 str1大于str2 */
int c_strncmp(unsigned char *str1,unsigned char *str2,unsigned int count)
{
	int res=0;
	while(count){
		if((res=*str1-*str2++)!=0 || !(*str1++))
			break;
		count--;
	}
	return res;
}
/* 图形方式的printf函数 */
/*入口参数: color:颜色 format:可变参数 */
/*出口参数: 无 */
/*备注: 考察了可变参数的C语言实现,通过一组宏(定义在stdarg.h)中来获取依次入栈的参数 
        在转化的汇编代码中能清楚的看出。在我所实现的平台上面不需要那么严格的检测,
        为了简便起见,仿照其实现原理进行了下面的处理
        另外,在程序中隐含了了x_cur,y_cur光标的坐标,所以如果要实现换行的话,通过光标指定来实现*/
void c_printf(unsigned long color, const char *format,...)
{
	/*当前的显示平台中最大显示35个汉字,即字符串不得超过67个*/
	/*仅支持 "%d %ld %x %lx %c %C %s几种格式"*/
	char buffer[100],buffer1[11];								/* 预留缓冲区100字符即可 */
	char ch;
	char *ptr,*ptr1;
	unsigned char flag=0;
	unsigned int len,i;
	i_va_list argptr;
	
	i_va_start(argptr,format);
	for (ptr=buffer ; *format ; ++format) 
	{
		if (*format != '%') 
		{
			*ptr++ = *format;
			continue;
		}
		++format;					/* 滤掉 %*/
		if((*format=='l')||(*format=='L'))
		{
			flag=1;
			++format;
		}
		switch(*format)
		{
			case 'c':
			case 'C':
				*ptr++ = (unsigned char) i_va_arg(argptr, int);
				break;
			case 's':
			case 'S':
				ptr1 = i_va_arg(argptr, char *);
				len=c_strlen(ptr1);
				for (i = 0; i < len; ++i)
					*ptr++ = *ptr1++;
					break;
			case 'x':
			case 'X':
				if(flag)
				{
					flag=0;
					_l2asc(i_va_arg(argptr, unsigned long),16,buffer1);
				}
				else
					_i2asc(i_va_arg(argptr, unsigned int),16,buffer1);
				ptr1=buffer1;
				len=c_strlen(ptr1);
				for (i = 0; i < len; ++i)
					*ptr++ = *ptr1++;
					break;
			case 'd':
			case 'D':
				if(flag)
				{
					flag=0;
					_l2asc(i_va_arg(argptr, unsigned long),10,buffer1);
				}
				else
					_i2asc(i_va_arg(argptr, unsigned int),10,buffer1);
				ptr1=buffer1;
				len=c_strlen(ptr1);
				for (i = 0; i < len; ++i)
					*ptr++ = *ptr1++;
					break;
			default:
				*ptr++=*format;
				break;
				
		}
	}
	*ptr='\0';
	i_va_end(argptr);
	if(c_strlen(buffer)>66)return;
	selFONT(ASC0|_ASC0|HZKK|_HZ16);
	putStr(x_cur,y_cur,buffer,color,1);
	x_cur+=(c_strlen(buffer)*8);
}
/* 图形方式的scanf函数 */
/*入口参数: bg_color:输入的时候的背景颜色 color:字符颜色 format:可变参数 */
/*出口参数: 无 */
/*备注: %d %ld %x %lx %c %C %s %p  %(数字)s  %(数字)p 几种格式 (数字采用十进制，即最大为9)
				如果没有加数字则缺省认为最大可以接受12个字符
        注意参数必须是以地址传送的方式传递.其中%p表示密码输入,显示的时候显示'*'
        而行如%4s %5p之类的参数是我自己特意增加的,为了提供一种确定获取字符的方式 */
void c_scanf(unsigned long bg_color,unsigned long color,const char *format,...)
{
	/*当前的显示平台中最大显示35个汉字,即字符串不得超过71个*/
	/*仅支持 "%d %ld %x %lx %c %C %s %p  %(数字)s  %(数字)p 几种格式"*/
	char buffer[100];								/* 预留缓冲区100字符即可 */
	unsigned long in_long;
	char *ptr,*ptr1;
	unsigned char flag=0;
	unsigned int code,len,i,lens=0;
	i_va_list argptr;
	
	i_va_start(argptr,format);
	selFONT(ASC0|_ASC0|HZKK|_HZ16);
	for (; *format ; ++format) 
	{
		if (*format != '%') 
		{	
			code=((unsigned int)(format[0])&0xff);	
			if(code<128)			/* 英文字符 */
				putASCII(x_cur,y_cur,*format,color,1);
			else
				{
					code=((unsigned int)(format[1])&0xff);
					code<<=8;
					code+=((unsigned int)(format[0])&0xff);
					putHZ(x_cur,y_cur,code,color,1);
					x_cur+=8;
					++format;
				}
			x_cur+=8;
			continue;
		}
		++format;					/* 滤掉 %*/
		if((*format=='l')||(*format=='L'))
		{
			flag=1;
			++format;
		}
		if((*format>'0')&&(*format<'9'))
		{
			lens=*format-'0';
			++format;
		}
		switch(*format)
		{
			case 'c':
			case 'C':
				ptr = i_va_arg(argptr, char *);
				getKbin(bg_color,color,0x0100,buffer);
				ptr[0]=buffer[0];
				break;
			case 's':
			case 'S':
				ptr = i_va_arg(argptr, char *);
				if(lens!=0)
					getKbin(bg_color,color,(lens<<8),buffer);
				else
					getKbin(bg_color,color,0x0c00,buffer);				/* 最大容纳12个字符 */
				ptr1=buffer;
				len=c_strlen(ptr1);
				for (i = 0; i < len; ++i)
					*ptr++ = *ptr1++;
				*ptr='\0';
				lens=0;
					break;
			case 'x':
			case 'X':
				if(flag)
				{
					flag=0;
					ptr = i_va_arg(argptr, char *);
					in_long=getKbin(bg_color,color,0x0810,buffer);
					ptr[0]=(in_long&0xff);
					ptr[1]=(lintshr(8,in_long)&0xff);
					ptr[2]=(lintshr(16,in_long)&0xff);
					ptr[3]=(lintshr(24,in_long)&0xff);
					/*
					ptr[1]=((in_long>>8)&0xff);
					ptr[2]=((in_long>>16)&0xff);
					ptr[3]=((in_long>>24)&0xff);*/
				}
				else
				{
					ptr = i_va_arg(argptr, char *);
					in_long=getKbin(bg_color,color,0x0410,buffer);
					ptr[0]=(in_long&0xff);
					ptr[1]=(lintshr(8,in_long)&0xff);
					/*ptr[1]=((in_long>>8)&0xff);*/
				}
					break;
			case 'd':
			case 'D':
				if(flag)
				{
					flag=0;
					ptr = i_va_arg(argptr, char *);
					in_long=getKbin(bg_color,color,0x0a0a,buffer);
					ptr[0]=(in_long&0xff);
					ptr[1]=(lintshr(8,in_long)&0xff);
					ptr[2]=(lintshr(16,in_long)&0xff);
					ptr[3]=(lintshr(24,in_long)&0xff);
					/*
					ptr[1]=((in_long>>8)&0xff);
					ptr[2]=((in_long>>16)&0xff);
					ptr[3]=((in_long>>24)&0xff);*/
				}
				else
				{
					ptr = i_va_arg(argptr, char *);
					in_long=getKbin(bg_color,color,0x050a,buffer);
					ptr[0]=(in_long&0xff);
					ptr[1]=(lintshr(8,in_long)&0xff);
					/*ptr[1]=((in_long>>8)&0xff);*/
				}
					break;
			case 'p':
			case 'P':
				ptr = i_va_arg(argptr, char *);
				if(lens!=0)
					getKbin(bg_color,color,((lens<<8)|0x01),buffer);
				else
					getKbin(bg_color,color,0x0C01,buffer);				/* 最大容纳12个字符 */
				ptr1=buffer;
				len=c_strlen(ptr1);
				for (i = 0; i < len; ++i)
					*ptr++ = *ptr1++;
				*ptr='\0';
				lens=0;
					break;
			default:
				putASCII(x_cur,y_cur,*format,color,1);
				x_cur+=8;
				break;
				
		}
	}
}
/* 编辑框中行的选定颜色变换 */
/*入口参数: x,y坐标,           */
/*          color:高8位为变换后的颜色  低8位为初始颜色 mag:字体倍率*/
/*出口参数: 无 */
/*备注:     颜色由高字 低字共同表示 ,调用的时候注意点*/
void chgLine(unsigned int x,unsigned int y,unsigned int num_line,unsigned long color)
{

	unsigned int height,length;					/* 显示字符串的框的长和宽*/
	unsigned int i,j;
	unsigned long src_color,dst_color;
	
	if(num_line>9)return;										/* 最大只显示10行 */
	src_color=(color&0xff);
	dst_color=(lintshr(8,color)&0xff);
	/*dst_color=((color>>8)&0xff);*/
	

	length=(33<<4);						/* 一行的最大长度 */
	height=4+16+4;						/* 选定后显示宽度 */
	
	for(j=y+40+num_line*24-4;j<y+40+num_line*24-4+height;j++)
		for(i=x+8;i<x+8+length;i++)
			if(getpixel(i,j)==src_color)
				putpixel(i,j,dst_color);
}
/* 将整型数转换为ASCII字符串 */
/*入口参数: num:需要转换的数 base:进制,2~16为有效参数 out:转换后的字符串 */
/*出口参数: 无 */
void _i2asc(unsigned int num,int base,unsigned char *out)
{
	/*const unsigned char *digits="0123456789ABCDEF";*/
	unsigned char outstr[17];
	unsigned int i,j,shang;
	
	if(num==0)
	{
		out[0]='0';
		out[1]='\0';
		return;
	}
	shang=num;
	i=0;
	outstr[0]='\0';
	if(base!=16)
	{
		while(shang!=0)
		{
			asm xor dx,dx
			asm mov ax,shang
			asm div base
			asm mov shang,ax											/* 商 */
			outstr[(++i)]=digits[_DX];

		}
	}
	else
	{
		while(shang!=0)
		{
			outstr[(++i)]=digits[(shang&0x0f)];
			shang=lintshr(4,shang);
			/*shang>>=4;*/
		}
	}
	for(j=0;j<=i;j++)
		out[j]=outstr[i-j];
}
/* 将长整型数转换为ASCII字符串 */
/*入口参数: num:需要转换的数 base:进制,2~16为有效参数 out:转换后的字符串 */
/*出口参数: 无 */
void _l2asc(unsigned long num,int base,unsigned char *out)
{
	/*const unsigned char *digits="0123456789ABCDEF";*/
	unsigned char outstr[33];
	unsigned int i,j;
	unsigned long shang;
	
	if(num==0)
	{
		out[0]='0';
		out[1]='\0';
		return;
	}
	shang=num;
	i=0;
	outstr[0]='\0';
	if(base!=16)
	{
		while(shang!=0)
		{
			outstr[(++i)]=digits[lintmod(base,shang)];
			shang=lintdiv(base,shang);
			/*outstr[(++i)]=digits[shang%base];
			shang=(shang/base);									/* 商 */	
		}
	}
	else
	{
		while(shang!=0)
		{
			outstr[(++i)]=digits[(shang&0x0f)];
			shang=lintshr(4,shang);
			/*shang>>=4;*/
		}
	}
	for(j=0;j<=i;j++)
		out[j]=outstr[i-j];	
}

/* 获取用户的各种输入 */
/*入口参数: 依次为:背景色 字符颜色 可接受的字符个数及进制 输出字符串*/            
/*出口参数: 如果获取的是数字则返回其值，如果是字符则返回0 */
unsigned long getKbin(unsigned long bg_color,unsigned long color,unsigned int num_base,unsigned char *out)
{
	/*num:获取的字符个数,base:0获取字符串 1获取密码字符串  10获取10进制数 16获取16进制数*/	
	unsigned char buffer[32];
	unsigned int number,base,cur_num,i,num_chars;
	unsigned char chin=0;
	unsigned long retval=0;
	number=((num_base>>8)&0xff);
	base=(num_base&0xff);
	selFONT(ASC0|_ASC0|HZKK|_HZ16);
	rectblock(x_cur,y_cur,x_cur+number*8,y_cur+16-1,bg_color);
	
	InitIntRes(CURSOR);			/* 打开光标的显示 */
	if(base==16)base=22;
	if(base>1)
	{
		cur_num=0;
		while(chin!='\r')					/*enter 回车*/
		{
			asm xor ax,ax
			asm int 0x16
			asm mov chin,al
			if((chin==0x08)&&cur_num>0)				/*1 回退键的处理 */
			{
				--cur_num;
				buffer[cur_num]=0;
				x_cur-=8;						/* 光标回移 */
				rectblock(x_cur,y_cur,x_cur+8,y_cur+16-1,bg_color);		/* 消除当前显示 */	
			}
			/*2 判断是否是符合要求的数,如果不是则继续获取下一字符 */
			for(i=0;i<=base;i++)
				if(digits[i]==chin)			/* 符合要求 */
				{
					/*2 判断是否已经是数据满 */
					if(cur_num==(number))
					{
						buffer[cur_num-1]=((i>0xf)?(i-6):i);
						rectblock(x_cur-8,y_cur,x_cur-1,y_cur+16-1,bg_color);			/*清除当前显示*/
						putASCII(x_cur-8,y_cur,chin,color,1);						/* 显示得到的新字符 */
					}
					else
					{
						buffer[cur_num]=((i>0xf)?(i-6):i);
						++cur_num;
						x_cur+=8;
						rectblock(x_cur-8,y_cur,x_cur,y_cur+16-1,bg_color);
						putASCII(x_cur-8,y_cur,chin,color,1);					/*显示当前字符*/
					}
				}
			
		}
		/* 至此为止,buffer中包含了得到的数据,cur_num为得到的个数*/
		if(base==22)base=16;
		for(i=0;i<cur_num;i++)
			retval=lintmul(retval,base)+(unsigned long)(buffer[i]);
		/*	retval=retval*base+buffer[i];	*/
	}
	else
	{
		cur_num=0;
		num_chars=c_strlen(digits);
		while(chin!='\r')
		{
			asm xor ax,ax
			asm int 0x16
			asm mov chin,al
			if((chin==0x08)&&(cur_num>0))				/*1 回退键的处理 */
			{
				--cur_num;
				out[cur_num]='\0';
				/* 消除当前显示	*/
				x_cur-=8;								/* 光标回移 */
				rectblock(x_cur,y_cur,x_cur+8,y_cur+16-1,bg_color);
									
			}
			for(i=0;i<num_chars;i++)
				if(chin==digits[i])
				{
					/*2 判断是否已经是数据满 */
					if(cur_num==(number))
					{
						out[cur_num-1]=chin;
						/*清除当前显示*/
						rectblock(x_cur-8,y_cur,x_cur-1,y_cur+16-1,bg_color);
						
						if(base==0)
							putASCII(x_cur-8,y_cur,chin,color,1);	/*显示得到的新字符*/
						else
							putASCII(x_cur-8,y_cur,'*',color,1);
						
					}
					else
					{
						out[cur_num]=chin;
						++cur_num;
						x_cur+=8;
						rectblock(x_cur-8,y_cur,x_cur,y_cur+16-1,bg_color);
						if(base==0)
							putASCII(x_cur-8,y_cur,chin,color,1);	/*显示当前字符*/
						else
							putASCII(x_cur-8,y_cur,'*',color,1);
						
					}
				}
		}
		out[cur_num]='\0';
	}
	RelIntRes(CURSOR);
	return retval;
}
/* 光标处理的系列函数 */
void setCur(unsigned int x,unsigned int y)
{
	x_cur=x;
	y_cur=y;
}
unsigned int getXCur(void)
{
	return x_cur;
}
unsigned int getYCur(void)
{
	return y_cur;
}

/*      ASCII码转换为十六进制数                 */
/*入口参数：  需要转换的ascii                   */      
/*出口参数：  AL=转换字符                       */
/*     
unsigned char asc2hex (unsigned char ch)
{
	asm{
		mov	al,ch
		or  al,20h            /*大写转小写*//*
 		sub al,'0'
 		cmp al,9
 		jbe  athover          
 		sub al,'a'-'0'-10
	}
	athover: 
	return(_AL);
}*/

/* 十六进制数转换为对应的ASCII码         */
/*入口参数：  ch:要转换的数字(0~f)       */
/*出口参数：  对应的ASCII码              */
/*unsigned char hex2asc(unsigned char ch) 
{
	asm{
		mov al,ch
		and al,0fh
    add al,30h
    cmp al,39h
    jbe htoasc1
    add al,7
	}
	htoasc1:
	return(_AL);
}  
*/