#ifndef _GRAPHIC_H
#define _GRAPHIC_H

#define ANOTHER_AR					1										/*编译的时候将其关闭,我自己调试的一些算法*/

//#define bkGColor DEEPBLUE												/*背景颜色*/
#define bkGColor GROUND1
//#define BOOTColor 	255				
//#define BOOTColor1  254				/* 外围颜色 */


#define 		GROUND1			0
#define			MGROUND1			(GROUND1+1)
#define 		LIGHTSHADOW1	(MGROUND1+1)
#define 		MLIGSHADOW1		(LIGHTSHADOW1+1)
#define 		SURFACE1			(MLIGSHADOW1+1)
#define 		DARKSHADOW1		(SURFACE1+1)
#define 		DARKPROJECT1	(DARKSHADOW1+1)

#define			BLACK					(DARKPROJECT1+1)
#define			WHITE					(BLACK 		+ 	1)
#define			LIGHTGRAY			(WHITE 		+	1)
#define			DEEPBLUE			(LIGHTGRAY 	+	1)
#define			DARKBLACK			(DEEPBLUE	+	1)
#define			LIGHTBLACK		(DARKBLACK	+	1)
#define			YELLOW				(LIGHTBLACK	+	1)
#define			GREEN					(YELLOW		+	1)
#define			RED						(GREEN		+	1)
#define			BLUE					(RED		+	1)

#define maxX	(xres-1)													/* 横坐标最大值 */
#define maxY	(yres-1)													/* 纵坐标最大值 */

extern SvgaDAC sys_pal[256];
extern void setBackGround(unsigned long color);
extern unsigned int initDisplay(unsigned int mode,SvgaDAC far *palette);
extern void	iDelay(unsigned long millsec);
extern void SvgaSmoothOn(SvgaDAC *pal);
extern void SvgaSmoothOff(SvgaDAC *pal);
extern void	line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long color);
extern void	rectangle(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long color);
extern void	rectblock(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long color);
extern void	circle(unsigned int centerx,unsigned int centery,unsigned int radius,unsigned long color);
extern void cirblock(unsigned int cenx,unsigned int ceny,unsigned int radius,unsigned long color);
#if ANOTHER_AR
extern void	cirx(unsigned int centerx,unsigned int centery,unsigned int radius,unsigned long color,unsigned char quadrant);
extern void	rectarc(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2,unsigned long color);

#endif
#endif