/* 将与用户交互的数据结构作为通用的数据结构提取出来 */
#ifndef _COMMON_H
#define _COMMON_H

//#ifndef LBDEBUG						/* 调试开关 */
//#define LBDEBUG
//#endif 

#define KEY_UP	    0x4800
#define KEY_DOWN    0x5000
#define KEY_LEFT		0x4b00
#define KEY_RIGHT		0x4d00
#define KEY_ENTER   0x1c0d
#define KEY_TAB			0x0f09
#define KEY_ALTR		0x1300
#define KEY_F1      0x3b00
#define KEY_F2      0x3c00
#define KEY_F3      0x3d00
#define KEY_F4      0x3e00
#define KEY_F5      0x3f00
#define KEY_SPACE   0x3920

#define KEY_ALTF9   0x7000
#define KEY_1				0x0231
#define KEY_2				0x0332
#define KEY_3				0x0433
#define KEY_4				0x0534
#define KEY_5				0x0635
#define KEY_6				0x0736
#define KEY_7				0x0837
#define KEY_8				0x0938
#define KEY_9				0x0A39
#define KEY_0				0x0B30
#define KEY_NUM1				0x4F31
#define KEY_NUM2				0x5032
#define KEY_NUM3				0x5133
#define KEY_NUM4				0x4B34
#define KEY_NUM5				0x4C35
#define KEY_NUM6				0x4D36
#define KEY_NUM7				0x4737
#define KEY_NUM8				0x4838
#define KEY_NUM9				0x4939
#define KEY_NUM0				0x5230

#define KEY_ESC			0x011b
#define KEY_PGUP		0x4900
#define KEY_PGDN		0x5100
#define KEY_CTRLS		0x1F13
/* 为了实现可变参数的存取,仿照C语言的实现定义了一批宏 */
/*__i_size(x) 中的写法是为了内存对齐的*/

typedef void far *i_va_list;
#define __i_size(x) ((sizeof(x)+sizeof(int)-1) & ~(sizeof(int)-1))
#define i_va_start(ap, parmN) ((void)((ap) = (i_va_list)((char far *)(&parmN)+__i_size(parmN))))
#define i_va_arg(ap, type) (*(type far *)(((*(char far *far *)&(ap))+=__i_size(type))-(__i_size(type))))
#define i_va_end(ap)          ((void)0)


typedef struct navBar
{
	unsigned int os_index;			/* 导航条对应的相应的资源序号(OS) */
	unsigned int x,y;						/* 导航条的位置 */
	unsigned char msg[13];			/* 导航条显示的字符串 */
	unsigned long	msg_color;		/* 字符串的颜色 */
	unsigned long font;					/* 字体 */
	unsigned long	chg_color;		/* 选定后导航的颜色,注意,初始化的时候为低8位背景色,次8位选定后的颜色 */
}BAR;
typedef	struct menuitem
{
	//unsigned char index;				/* 菜单项所属序号 */
	unsigned int x,y;						  /* 菜单项位置 */
	unsigned char valid;				  /* 菜单项是否有效 */
	unsigned char *msg;					  /* 菜单显示字符串 */
	//unsigned long font;					/* 字体固定为16x16汉字 */
	unsigned long msg_color;		  /* 菜单字符串的颜色 高8位为无效时候的颜色,低8位为有效时候的颜色 */
	unsigned long chg_color;		  /* 当菜单选中时候的颜色 注意,初始化的时候为低8位背景色,次8位选定后的颜色 */
}MenuItem;

typedef struct _WallPaper{
	unsigned long lba_addr;						/* 存放在硬盘中的地址 */
	/*unsigned long size;						  /* 壁纸缺省为1024*768*/
	unsigned char valid;
}iWallPaper;

typedef struct FoxdiskPara{
	signed int cntdown_time;						/* 倒计时时间,-1为无效 */
	unsigned int cntdown_x;							/* 显示倒计时的坐标 */
	unsigned int cntdown_y;
	signed int def_os;									/* 默认进入的系统,0～4 */
	unsigned int wpaper_num;						/* 壁纸的数目,如果为0则wallpaper必须为-1 */
	signed int wallpaper;								/* 壁纸的选定 -1表示无效,即隐藏壁纸 */
	unsigned long lba_ipara;						/* 参数表存放的lba地址 由安装程序初始化 */
	unsigned long lba_ios;							/* OS信息存放的lba地址 由安装程序初始化 */
	unsigned long lba_ifox;							/* foxdisk主程序存放的位置 */	
	unsigned int ver;									  /* 版本变迁描述,从0x0100开始 */
	iWallPaper iwapaper[7];
	
	/*signed int modified;								/* 用户是否更新了系统参数 0为未更改 */
}FOXPARA;


typedef struct OperationSystem{
	unsigned char bar_index;				/* 导航条的显示顺序 0~3 */
	unsigned char os_valid;					/* 此结构是否有实际硬件资源对应 */
	unsigned char os_name[13];			/* 系统名称,最大为12个字符串 */
	unsigned char pwd_valid;				/* 密码是否有效 */
	unsigned char pwd[13];					/* 最大可设置12个字符的密码 */
	unsigned long os_mbr;						/* 当前OS对应的MBR的起始地址 */	
}OS;   

#endif