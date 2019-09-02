/*luobing 2008-10-13 开始编写*/
/*制作一个在无OS或者DOS下通用的图形库,开始当前的计划.
  并未打算跨平台移植,所有代码均在i386下运行*/
#ifndef _VESA_H
#define _VESA_H

#define DIRECT_BANKING					/*决定是否采用厂商提供的换页机制*/
/*#define MULTI_PAGE							/*多页面显示的编译开关*/

#define INVLIADIN				0x01		/* 初始化图形驱动时无效入口参数 */
#define NOTSUPPORTVBE		0x02		/* 不支持VBE */
#define NOTSUPPORTMODE  0x04		/* 无效显示模式 */
#define LIMITVMEM				0x08		/* 显存不足 */

/* WriteMode 所用的常量*/
#define	PUT			0
#define XOR_PUT	1
#define AND_PUT	2
#define OR_PUT	3 


/*
#ifndef __FP__MACRO
#define __FP__MACRO
#define FP_SEGM(fp)         ((unsigned int)((unsigned long)((void far *)(fp)) >> 16))
#define FP_OFFS(fp)         ((unsigned int)(fp))
#endif
*/
/*VBE信息数据结构定义(VBE功能调用00H 返回)*/
typedef struct{																/*总共512字节*/
	char VBESignature[4];								/*字符串"VESA"*/
	unsigned int VBEVersion;						/*VBE版本号,BCD码*/
	char far *OEMStringPtr;							/*指向OEM厂商的标志串的指针*/
	unsigned long Capabilities;					/*显示卡特性*/
	unsigned int far *VideoModePtr;			/*指向所显示模式列表的指针*/
	unsigned int VRAMMemory;						/*显示内存大小,单位为64KB*/
	
	/*以下为VESA VBE2.0版以上定义*/
	unsigned int OemSoftwareRev;				/*VBE软件的OEM修订版本号,BCD码*/
	char far *OemVendorNamePtr;					/*指向显示卡制造厂商的字符串指针*/
	char far *OemProductNamePtr;				/*指向显示卡产品名称的字符串指针*/
	char far *OemProductRevPtr;				  /*指向显示卡修订版本号或产品等级的字符串指针*/
	char reserved[222];									/*保留*/
	
	char OEMData[256];									/*VESA2.0以上版本定义*/
}VBEInfo;

/*VBE特定模式信息块数据结构定义(VBE功能调用01H返回)*/
typedef struct																			/*总共256字节*/
{
	unsigned int ModeAttr; 										/* 模式的属性 */
	unsigned char WinAAttr;				 						/* 窗口A的属性 */
	unsigned char WinBAttr;										/* 窗口B的属性 */
	unsigned short WinGran; 									/* 窗口粒度,以KB为单位 */
	unsigned short WinSize; 									/* 窗口大小,以KB为单位 */
	unsigned short WinASeg; 									/* 窗口A起始段址 */
	unsigned short WinBSeg; 									/* 窗口B起始段址 */
	char far *BankFunc;												/* 换页调用入口 */
	unsigned int BytesPerScanline; 						/* 每条水平扫描线所占的字节数 */
	unsigned int XRes,YRes; 								  /* 水平和垂直分辨率 */
	unsigned char XCharSize; 									/* 字符的宽度 */
	unsigned char YCharSize; 									/* 字符的高度 */
	unsigned char NumberOfPlanes; 						/* 位平面的个数 */
	unsigned char BitsPerPixel; 							/* 每象素的位数 */
	unsigned char NumberOfBanks;	 						/* CGA逻辑扫描线分组数 */
	unsigned char MemoryModel; 								/* 显示内存模式 */
	unsigned char BankSize; 									/* CGA每组扫描线的大小 */
	unsigned char NumberOfImagePages; 				/* 可同时载入的最大满屏图象数 */
	unsigned char reserve1; 									/* Reserved */
	
	/*对直接写颜色模式的定义区域*/
	unsigned char RedMaskSize; 								/* 红色所占位数 */
	unsigned char RedFieldPosition; 					/* 红色的最低有效位位置 */
	unsigned char GreenMaskSize; 							/* 绿色所占位数 */
	unsigned char GreenFieldPosition; 				/* 绿色的最低有效位位置 */
	unsigned char BlueMaskSize; 							/* 蓝色所占位数 */
	unsigned char BlueFieldPosition; 					/* 蓝色的最低有效位位置 */
	unsigned char RsvdMaskSize; 							/* 保留色所占位数 */
	unsigned char RsvdFieldPosition; 					/* 保留色的最低有效位位置 */
	unsigned char DirectColorModeInfo; 				/* 直接颜色模式属性 */
	
	/*以下位VBE2.0版以上定义*/
	char far *PhyBasePtr;											/* 可使用大的帧缓存时为指向其首址的32位物理地址 */
	unsigned long OffScreenMemOffset;					/* 帧缓存首址的32位偏移量*/
	unsigned int OffScreenMemSize;						/* 可用的、连续的显示缓存区,以KB为单位*/
	
	/*以下为VBE3.0版以上定义*/
	unsigned int LinBytePerScanLine;					/* 线性缓冲区中每条扫描线的长度，以字节为单位*/
	char BnkNumberOfImagePages;								/* 使用窗口功能时的显示页面数*/
	char LinNumberOfImagePages;								/* 使用大的线性缓冲区时显示页面数*/
	char LinRedMaskSize;											/* 使用大的线性缓冲区时红色所占位数*/
	char LinRedFieldPosition;									/* 使用大的线性缓冲区时红色的最低有效位位置*/
	char LinGreenMaskSize;										/* 使用大的线性缓冲区时绿色所占位数*/
	char LinGreenFieldPosition;								/* 使用大的线性缓冲区时绿色的最低有效位位置*/
	char LinBlueMaskSize;											/* 使用大的线性缓冲区时蓝色所占位数*/
	char LinBlueFieldPosition;								/* 使用大的线性缓冲区时蓝色的最低有效位位置*/
	char LinRsvMaskSize;											/* 使用大的线性缓冲区时保留色所占位数*/
	char LinRsvFieldPosition;									/* 使用大的线性缓冲区时保留色的最低有效位位置*/
	unsigned char reserve2[194]; 							/* 保留 */
} ModeInfoBlock;

/* SVGA DAC调色板数据结构 */
typedef struct	
{
	unsigned char b;										/*蓝色*/
	unsigned char g;										/*绿色*/
	unsigned char r;										/*红色*/
	unsigned char alpha;								/*保留*/
}SvgaDAC;

extern VBEInfo vbeinfo;								 /*VESA 环境信息表 00h功能调用*/
extern ModeInfoBlock modeinfo;				 /*VESA 模式详细信息 01h功能调用*/
extern unsigned int xres,yres;         /* Resolution of video mode used */
extern unsigned int bytesperline;      /* Logical CRT scanline length */
extern unsigned char bytesperpixel;		 /* 每象素所占字节数 */
extern unsigned int curBank;           /* Current read/write bank */
extern unsigned int bankShift;         /* Bank granularity adjust factor */
extern unsigned int oldMode;           /* Old video mode number */
extern char far *screenPtr;            /* Pointer to start of video memory */
extern void (far *bankSwitch)(void);   /* Direct bank switching function */

extern signed int getVBEInfo(VBEInfo far *vbeinfo);
extern signed int getModeInfo(unsigned int mode,ModeInfoBlock far *modeinfo);
extern void setmode(unsigned int mode);
extern unsigned int getVBEMode(void);

extern unsigned int initGraphics(unsigned int mode);
extern signed int (*setpalette)(unsigned int start,unsigned int num,SvgaDAC far *palette);
extern signed int (*getpalette)(unsigned int start,unsigned int num,SvgaDAC far *palette);
extern void	SetWriteMode(unsigned int writemode);
extern void (*putpixel)(unsigned int x,unsigned int y,unsigned long color);
extern unsigned long	(*getpixel)(unsigned int x,unsigned int y);
#endif