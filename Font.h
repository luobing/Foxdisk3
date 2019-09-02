#ifndef _FONT_H
#define _FONT_H

#define		ASCIIMAX		256
#define   CHARWIDTH  	8
#define   CHARHEIGHT  16
#define 	HZWIDTH			16
#define 	HZHEIGHT		16
#define 	HZ24WIDTH		24
#define 	HZ24HEIGHT	24

//#define 	HZK24S_ACTIVE
#define 	HZK24K_ACTIVE
//#define 	HZK24F_ACTIVE
//#define 	HZK24H_ACTIVE


#ifndef HZK24TYPE_DEFINE
#define HZK24TYPE_DEFINE
struct	hzk24_typ{         /*  汉字字模结构体  */
  unsigned int code;
  unsigned char array[3*24];
};
#endif

#ifndef HZK16TYPE_DEFINE
#define HZK16TYPE_DEFINE
struct	hzk16_typ{         /*  汉字字模结构体  */
  unsigned int code;
  unsigned int array[16];
};
#endif
struct	ascii_typ{				/* ASCII码字模 */
	unsigned char code;
	unsigned array[16];
};
	
//extern int hz24_count;
//extern struct hzk24_typ	*hzk24_table;
//extern unsigned long selfont;									/* 字体的选择全局变量 */
/* 以下为此变量的含义 */
/* [][][][]  [][][][]    [][][][] [][][][] */
/* 英文字体  英文大小    中文字体 中文大小 */
#define ASC0 			0x01000000					/* 缺省的ASCII字符，也是一直常用的 */
#define _ASC0			0x00010000					/* 8x16 大小的英文字符 */	
#define HZKK			0x00000100					/* 汉字库 楷体 */
#define HZKS			0x00000200					/* 汉字库 宋体 */
#define HZKF			0x00000400					/* 汉字库 仿宋 */
#define HZKH			0x00000800					/* 汉字库 黑体 */
/*#define _HZ12			0x00000001					/* 汉字大小 12x16 */		
#define _HZ16			0x00000002					/* 汉字大小 16x16 */	
#define _HZ24			0x00000004					/* 汉字大小 24x24 */	
/*#define _HZ40			0x00000008					/* 汉字大小 40x40 */	
/*#define _HZ48			0x00000010					/* 汉字大小 48x48 */	

extern void	putStr(unsigned int x,unsigned int y,unsigned char *s,unsigned long color,unsigned char mag);
extern void selFONT(unsigned long font);
extern unsigned long getFONT(void);

extern void	(* putHZ)(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
extern void	putASCII(unsigned int x,unsigned int y,unsigned char code,unsigned long color,unsigned char mag);
extern void	putHZ16(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
extern void	putHZ24(unsigned int x,unsigned int y,unsigned int code,unsigned long color,unsigned char mag);
extern void	selHZK24(char mode);
#endif