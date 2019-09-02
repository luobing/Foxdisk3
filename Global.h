#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "Font.h"
#include "Vesa.h"
#include "DISKDRV.h"
#include "DISKAPP.h"
/*===data seg flag==== */
extern char  gFoxdisk[];
extern unsigned char	gEndOfData[];
extern unsigned int iFoxVer;

extern unsigned char debug1[];
extern unsigned char debug2[];
/* ====================BootMenu.c==================== */
extern unsigned char *bootStr[];
extern unsigned long bmsg_color;
extern unsigned long bbg_color;
/* ====================MainMenu.c==================== */
#define HELP_LINES	24
extern unsigned char helpTitle[];
extern unsigned char *helpStr[];
extern unsigned char aboutTitle[];
#define SYSFUN_NUM 5							/* 系统菜单中的功能菜单数目 */
extern unsigned char *sysFun_str[];
extern unsigned char *sysFun_Name[];
extern unsigned char *sysFun_Sort[];
extern unsigned char *sysFun_Pwd[];
extern unsigned char hideFun_str0[];
extern unsigned char hideFun_str1[];
extern unsigned char hideFun_str2[];
extern unsigned char hideFun_str3[];
extern unsigned char hideFun_str4[];
extern unsigned char hideFun_str5[];
extern unsigned char *about_str[];
extern unsigned char *exitSys_str[];
/* ====================SetPara.c==================== */
extern unsigned char *paramenu_str[];
extern unsigned char *cntdown_str[];
extern unsigned char *setdefos_str[];
extern unsigned char *setwallp_str[];
extern unsigned char *exitPara_str[];
/* ====================Fdisk.c==================== */
extern unsigned char *fdisk_str[];
#define PARTFUN_NUM 9
extern unsigned char *part_fun[];
extern unsigned char *creatpart_str[];
extern unsigned char *delPartn_str[];
extern unsigned char *chkpart_str[];
extern unsigned char *savepart_str[];
extern unsigned char setid_array[];
extern unsigned char *setid_str[];
extern char *g_SysName[];

/* ====================INT1ch==================== */
extern unsigned int seg_oldint;							/* 保存以前的中断向量 */
extern unsigned int ofs_oldint;
/* bit0=1打开显示时钟的开关 bit1=1打开光标显示 bit2=1打开倒计时的显示  */
extern unsigned int _sw_INT_;									/* 中断函数使用的开关 */
extern unsigned int i_int_count;									/* 我的中断钟使用的计数器 */
extern unsigned char i_number[];			/* 当前显示使用的字符表 */
/* 时钟显示用到的字符串数组 */
extern unsigned char i_when[];
extern unsigned int oldhour_min;
extern unsigned char old_sec;
extern unsigned int _hms_x,_hms_y;			/* 时钟显示的坐标 */
/* 光标显示用到的变量 */
/*x_cur,y_cur作为全局变量存在 */
extern unsigned char curcolor;
extern unsigned char bg_curcolor;
extern unsigned char cur_flag;
/* 倒计时用到的变量 */
#define CNTD_SWID 16*2
#define CNTD_SHEI 16*2
extern unsigned char cntd_str[];
extern unsigned char cov_pci[(CNTD_SWID+1)*(CNTD_SHEI+1)];
extern unsigned int cntdown_times;					/* 倒计时的总时间 */
extern unsigned int time_rad;							/* 全局变量倍数计算器*/
extern unsigned int cntdown_over;					/* 倒计时完成的标志 1表示完成*/
extern unsigned int _cntd_x,_cntd_y;
extern unsigned long cntd_color,cntd_bcolor;

/* ====================Painter.c==================== */  
extern unsigned int x_cur;    
extern unsigned int y_cur;    
extern unsigned char digits[];
extern unsigned char iFaceStr[];
extern unsigned char iFaceStr1[];
extern unsigned char iFaceStr2[];
extern unsigned char iFaceStr3[];
extern unsigned char iFaceStr4[];
extern unsigned char iFaceStr5[];
extern unsigned char ifoxdisk[];
extern unsigned char iflag[];
extern unsigned char wing[17][3];
/* ====================Font.c==================== */
extern unsigned long selfont;
extern struct	ascii_typ	ascii_table[ASCIIMAX];
extern int hz24_count;
extern struct hzk24_typ	*hzk24_table;	
extern struct	hzk24_typ	HZK24Ktable[];		/* define in HZK24K.h */
extern struct	hzk16_typ	hzk16_table[];		/* define in HZTABLE.h*/
extern int hzcount;
extern int hzk24kcount;

/* ====================Graphic.c==================== */
extern SvgaDAC sys_pal[256];

/* ====================Vesa.c==================== */ 
extern void (far *bankSwitch)(void);   			/* Direct bank switching function */
extern void (*setbank)(unsigned int bank);
extern signed int (*setpalette)(unsigned int start,unsigned int num,SvgaDAC far *palette);
extern signed int (*getpalette)(unsigned int start,unsigned int num,SvgaDAC far *palette);
extern void (*putpixel)(unsigned int x,unsigned int y,unsigned long color);
extern unsigned long	(*getpixel)(unsigned int x,unsigned int y);
extern VBEInfo vbeinfo;         							/* VESA 环境信息表 00h功能调用 */
extern ModeInfoBlock modeinfo;   						/* VESA 模式详细信息 01h功能调用 */
extern unsigned int xres,yres;         			/* Resolution of video mode used */
extern unsigned int bytesperline;      			/* Logical CRT scanline length */
extern unsigned char bytesperpixel;    			/* 每象素所占字节数 */
extern unsigned int curBank;           			/* Current read/write bank */
extern unsigned int bankShift;         			/* Bank granularity adjust factor */
extern unsigned int oldMode;           			/* Old video mode number */
extern unsigned long	maxpage;								/* 最大显示页面 */
extern char far *screenPtr;            			/* Pointer to start of video memory */
extern unsigned int VRAMStartX,VRAMStartY; 	/* 逻辑显示起始 */
extern unsigned int WriteMode;								/* 画图写模式 */	

/* ====================DISKAPP.c==================== */
extern DiskDT    g_DTable[1];
extern INFO      g_Info;
extern DiskDRV  g_diskDrv;
/* ====================EndCode.c==================== */
extern void EndofCode(void);

/* ====================Loader.asm==================== */
extern	void beg_load(void);
extern	void end_load(void);

/* bootOS.c */
extern unsigned char bootos_str[];
extern unsigned char bootos_str1[];
extern unsigned char bootos_str2[];                            

#endif