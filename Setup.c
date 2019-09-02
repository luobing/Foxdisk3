#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <string.h>
#include <dir.h>

#include "BMP.h"				

#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"

#include "Common.h"
#include "Painter.h"
#include "MainMenu.h"
#include "INT1ch.h"
#include "BootMenu.h"
#include "DISKDRV.h"
#include "DISKAPP.h"

#include "Foxdisk.h"
#include "Setup.h"
#include "iSetup.h"
#include "Global.h"
#include "iMem.h"

void	MoveData(unsigned int dstSeg,unsigned int dstOff,unsigned int srcSeg,unsigned int srcOff,unsigned int n);
int savWallPaper(iWallPaper *wp,int nwp,char *file);
int saviFoxPic(iWallPaper *wp,int nwp,char *file);
int Check_Bmp(bmp_picture_ptr bmp_ptr);
int	Setup(unsigned long	addr,unsigned int flag);
void firstSetup(void);
void initKeyData(FOXPARA *para,OS *os);
int dirWallPaper(unsigned char *file_array);
unsigned char mircosoft[] = 
{
  0X33,0XC0,0X8E,0XD0,0XBC,0X00,0X7C,0XFB, 
  0X50,0X07,0X50,0X1F,0XFC,0XBE,0X1B,0X7C,
  0XBF,0X1B,0X06,0X50,0X57,0XB9,0XE5,0X01, 
  0XF3,0XA4,0XCB,0XBE,0XBE,0X07,0XB1,0X04, 
  0X38,0X2C,0X7C,0X09,0X75,0X15,0X83,0XC6, 
  0X10,0XE2,0XF5,0XCD,0X18,0X8B,0X14,0X8B, 
  0XEE,0X83,0XC6,0X10,0X49,0X74,0X16,0X38, 
  0X2C,0X74,0XF6,0XBE,0X10,0X07,0X4E,0XAC, 
  0X3C,0X00,0X74,0XFA,0XBB,0X07,0X00,0XB4, 
  0X0E,0XCD,0X10,0XEB,0XF2,0X89,0X46,0X25, 
  0X96,0X8A,0X46,0X04,0XB4,0X06,0X3C,0X0E, 
  0X74,0X11,0XB4,0X0B,0X3C,0X0C,0X74,0X05, 
  0X3A,0XC4,0X75,0X2B,0X40,0XC6,0X46,0X25, 
  0X06,0X75,0X24,0XBB,0XAA,0X55,0X50,0XB4, 
  0X41,0XCD,0X13,0X58,0X72,0X16,0X81,0XFB, 
  0X55,0XAA,0X75,0X10,0XF6,0XC1,0X01,0X74, 
  0X0B,0X8A,0XE0,0X88,0X56,0X24,0XC7,0X06, 
  0XA1,0X06,0XEB,0X1E,0X88,0X66,0X04,0XBF, 
  0X0A,0X00,0XB8,0X01,0X02,0X8B,0XDC,0X33, 
  0XC9,0X83,0XFF,0X05,0X7F,0X03,0X8B,0X4E, 
  0X25,0X03,0X4E,0X02,0XCD,0X13,0X72,0X29, 
  0XBE,0X46,0X07,0X81,0X3E,0XFE,0X7D,0X55, 
  0XAA,0X74,0X5A,0X83,0XEF,0X05,0X7F,0XDA, 
  0X85,0XF6,0X75,0X83,0XBE,0X27,0X07,0XEB, 
  0X8A,0X98,0X91,0X52,0X99,0X03,0X46,0X08, 
  0X13,0X56,0X0A,0XE8,0X12,0X00,0X5A,0XEB, 
  0XD5,0X4F,0X74,0XE4,0X33,0XC0,0XCD,0X13, 
  0XEB,0XB8,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X56,0X33,0XF6,0X56,0X56,0X52,0X50,0X06, 
  0X53,0X51,0XBE,0X10,0X00,0X56,0X8B,0XF4, 
  0X50,0X52,0XB8,0X00,0X42,0X8A,0X56,0X24, 
  0XCD,0X13,0X5A,0X58,0X8D,0X64,0X10,0X72, 
  0X0A,0X40,0X75,0X01,0X42,0X80,0XC7,0X02, 
  0XE2,0XF7,0XF8,0X5E,0XC3,0XEB,0X74,0X49, 
  0X6E,0X76,0X61,0X6C,0X69,0X64,0X20,0X70, 
  0X61,0X72,0X74,0X69,0X74,0X69,0X6F,0X6E, 
  0X20,0X74,0X61,0X62,0X6C,0X65,0X00,0X45, 
  0X72,0X72,0X6F,0X72,0X20,0X6C,0X6F,0X61, 
  0X64,0X69,0X6E,0X67,0X20,0X6F,0X70,0X65,
  0X72,0X61,0X74,0X69,0X6E,0X67,0X20,0X73, 
  0X79,0X73,0X74,0X65,0X6D,0X00,0X4D,0X69, 
  0X73,0X73,0X69,0X6E,0X67,0X20,0X6F,0X70, 
  0X65,0X72,0X61,0X74,0X69,0X6E,0X67,0X20, 
  0X73,0X79,0X73,0X74,0X65,0X6D,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X8B,0XFC,0X1E,0X57,0X8B, 
  0XF5,0XCB,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00, 
  0X00,0X00,0X00,0X00,0X00,0X00,0X55,0XAA 
};	
void main(int argc,char * argv[])
{
	#define S_FOXMBR  0x01			/* 安装Fox MBR  */
	#define S_NEWOS		0x02			/* 新的系统载入 */
	#define S_WALLP		0x04			/* 安装壁纸     */
	#define S_PARTOOL 0x08			/* 仅作分区工具 */
	#define S_ALL			0x00			/* 重新全部安装 */
	
	#define SYNTAX_ERR	0x01		/* 语法错误 */
	#define INPARA_ERR	0x02		/* 输入的参数越界 */
	/*
	unsigned int setup_flag=0,err=0;
	char filename[8+3+1+1];
	int nos=0,nwp=0;*/
	FOXPARA para;
	unsigned long lba_para;
	unsigned char buff[512];
	unsigned int firflag,hitkey;
	
	
	CreateDiskParaTable();							/* 初始化全局表,预备硬盘访问 */
	lba_para=CHS_to_Sect(CUR_DK->lgeo.maxCyl+1,0,1,CUR_DK);
	if(linear_ReadWDK(lba_para, 1, buff))
	{
		printf("Access HD Error!");
		exit(0);
	}
	imemcpy(&para,buff+0x10,sizeof(FOXPARA));
	
	if(argc==2)
		if((argv[1][0] == '/')||(argv[1][0] == '-'))
		{
			switch(argv[1][1])
			{
				case 'c':
				case 'C':
					printf("This operation is very dangerous! Press hotkey to continue...\n");
					asm xor ax,ax
					asm int 0x16
					asm mov hitkey,ax
					if(hitkey==KEY_ALTF9)
					{
						/* 清除iXLZ标志和 para的LUOB标志 */
						int err=0;
						if(para.lba_ipara>0x3f)
						{
							if(linear_ReadWDK(para.lba_ipara, 1, buff))
								err=1;
							imemset(buff,0,5);
							if(linear_WriteWDK(para.lba_ipara, 1, buff))
								err=1;
						}
						if(para.lba_ifox>0x3f)
						{
							if(linear_ReadWDK(para.lba_ifox, 1, buff))
								err=1;
							imemset(buff,0,5);
							if(linear_WriteWDK(para.lba_ifox, 1, buff))
								err=1;
						}
						if(err)
							printf("Uninstall Failue!");
						else
							printf("Uninstall OK!");
							
					}		
					break;
				default:
					break;
			}
		}
	if(argc>1)
	{
		if(hitkey!=KEY_ALTF9)
			printf("Syntax Error!");
		exit(0);
	}
	
			
	
	
	firflag=isFirSetup(&para);
	if(firflag==0)
		firstSetup();
	else if(firflag!=0xffff)
		updateSetup(&para);
	else
	{
		printf("Unknown Error,Can\'t run foxdisk\n");
		exit(0);
	}
		
	asm mov ax,3
	asm int 0x10
	#if 0
	if(argc>1)
	{
		if((argv[1][0] == '/')||(argv[1][0] == '-'))
		{
			switch(argv[1][1])
			{
				case 'B':
				case 'b':
					setup_flag|=S_WALLP;
					if(argc!=3)
						err|=SYNTAX_ERR;
					else if((argv[1][2]>0x30) && (argv[1][2]<0x36))			/* 1-6 bmp*/
					{
						nwp=(int)(argv[1][2]-0x30)-1;
						strcpy(filename,argv[2]);
					}
					else
						err|=INPARA_ERR;
					break;
				case 'P':
				case 'p':	
					setup_flag|=S_WALLP;
					if(argc!=3)
						err|=SYNTAX_ERR;
					else if((argv[1][2]>0x30) && (argv[1][2]<0x37))			/* 1-7 bmp*/
					{
						nwp=(int)(argv[1][2]-0x30)-1;
						strcpy(filename,argv[2]);
					}
					else
						err|=INPARA_ERR;
					break;
				case	'f':
				case  'F':
					if(argc!=2)
						err|=SYNTAX_ERR;
					else
						setup_flag|=S_WALLP;
						break;
				case 'r':
				case 'R':
					if(argc!=2)
						err|=	SYNTAX_ERR;
					else
						setup_flag|=S_FOXMBR;
					break;
				case '?':
				case 'h':
				case 'H':
					break;
				default:
					err|=	SYNTAX_ERR;
					break;
			}
		}
	}
	else setup_flag=S_ALL;
	if(err)
	{
		switch(err)
		{
			case SYNTAX_ERR:
				printf("Syntax error! Use \'-?\' or \'-h\' to get help.");
				break;
			case INPARA_ERR:
				printf("Parameter error! Use \'-?\' or \'-h\' to get help.");
				break;
			default:
				printf("Unknown error!");
				break;
		}
		exit(0);
	}
	#endif
}

/* 安装foxdisk */
/*入口参数:addr:代码 flag:SETUPMBR 仅安装mbr SETUPFOX 安装代码 */
int	Setup(unsigned long	addr,unsigned int flag)
{
	unsigned int	i,iCodeLen,iDataLen,iLoadLen;			//定义代码、数据、引导代码的长度
	unsigned char gBuff[512];
	unsigned char buff[512];
	BootPk	*pk = (BootPk*)gBuff;					
	
	/*iLoadLen = (unsigned int)((unsigned char*)end_load - (unsigned char *)beg_load);*/ //计算MBR处自检代码的长度
	iLoadLen = FP_OFFS(end_load)-FP_OFFS(beg_load);
	iCodeLen = (FP_OFFS(EndofCode) - FP_OFFS(BootEntry) + 511) /512 ;
	/*iDataLen = ((unsigned int)(gEndOfData- gFoxdisk)+511)/512;*/
	iDataLen= 64*2-2;		/* 在small模式下,最大为64K 即占用128个扇区 
												而在int13 42功能中,最大只能一次读取127个扇区,缺省认为数据段不会超过63k,
												否则堆栈段只有1K使用是不行的 */

	for(i=0;i<512;i++)gBuff[i]=0;
	for(i=0;i<512;i++)buff[i]=0;
	
	/* copy data from cs:beg_load to ds:gBuff (iLoadLen bytes) */
	MoveData(FP_SEGM(pk),FP_OFFS(pk),_CS,(unsigned int)(unsigned char *)beg_load,iLoadLen);

	if(linear_ReadWDK(0, 1, buff))
		return READDISK_ERR;
	imemcpy(gBuff+iLoadLen,buff+iLoadLen,512-iLoadLen-2);			/* 保存分区表 */
	pk->nCodeSect = iCodeLen;						
	pk->nDataSect = iDataLen;						
	pk->iDisk = 0x80;	
	pk->iBegin= addr;
	pk->magic = 0xaa55;
	
	if(flag&SETUPMBR)
		if(linear_WriteWDK(0, 1, gBuff))			/* 安装fox mbr*/
			return WRITEDISK_ERR;
	if(flag&SETUPFOX)
	{
		for(i=0; i<iDataLen; i++ )
		{
			MoveData(FP_SEGM(gBuff),FP_OFFS(gBuff),_DS,FP_OFFS(gFoxdisk)+i*512,512);
			if(linear_WriteWDK(addr+(unsigned long)(i), 1, gBuff))
				return WRITEDISK_ERR;
		}
		for(i = 0;i < iCodeLen;i++)
		{
			MoveData(FP_SEGM(gBuff),FP_OFFS(gBuff),_CS,(unsigned int)(unsigned char *)BootEntry+i*512,512);
			if(linear_WriteWDK(addr+(unsigned long)(i+iDataLen), 1, gBuff))
				return WRITEDISK_ERR;
		}	
	}
	return	0;
}

/* 第一次安装(全面初始化安装)*/
void firstSetup(void)
{
	unsigned char *firSetup_str[]={
		"安装引导",
		"请按照程序的指引进行安装(共四步):",
		"步骤一: Foxdisk代码安装",
		"  1 初始化系统参数表...",
		"  2 初始化系统信息...",
		"  3 初始化数据段和代码段...",
		"  4 安装代码...",
		"  安装完成,按任意键进入下一步骤...",
		"  安装失败,指定的安装位置无法访问!",
		"初始化配置文件...",
		"    配置成功! 按任意键进入安装阶段...",
		"抱歉,缺失程序所要求的ifox.bmp或硬盘有坏道,无法正常安装!",
		"程序:%s  内部版本码:%s"
	};
	unsigned char *firSetup_wp[]={
		"步骤二: 壁纸的安装(必须是1024x768 256色BMP图)",
		"  以下为可以安装的壁纸文件:",
		"%s",
		"请稍候,正在从当前目录中获取可安装的壁纸...",
		"    抱歉,当前目录没有发现可安装的壁纸,请确保您要安装的壁纸",
		"与安装程序放在同一目录下! 按任意键进入下一安装步骤...",
		"已安装壁纸总数: %d   当前安装的壁纸号:%d",
		"请输入需要安装的壁纸名:%s",
		"已经安装了六张壁纸,无法再安装了!",
		"安装失败!,文件名输入错误或者文件不符合要求,您还要继续吗(Y/N)?: %c",
		"您已成功安装壁纸,继续安装下一张吗(Y/N)?: %c",
		"您安装的壁纸总数:%d,按任意键进入下一步骤...",
	};
	unsigned char *firSetup_os[]={
			"步骤三: 加载系统信息",
			"  将系统信息录入到Foxdisk的管理系统中.",
			"  请输入系统名称(不能为空):%s",
			"  请输入密码(可以为空):%s",
			"程序出现意外错误,可能是存储Foxdisk数据的硬盘出现坏道了.",
			"Foxdisk已经接受了您的输入,当前系统将作为第一个系统保存.",
			"按任意键进入下一步骤..."
		};
	unsigned char *firSetup_para[]={
			"步骤四: 参数和系统信息录入",
			"    将设定的参数和系统信息保存到Foxdisk的数据库中.",
			"  保存参数表...",
			"  保存系统信息...",
			"  安装Foxdisk MBR引导程序...",
			" 成功!",
			" 失败!",
			" 安装完成,感谢您使用Foxdisk!"
		};
	unsigned char buff[512];
	SvgaDAC pal[256];
	FOXPARA para;
	unsigned char bmparray[12][13],VerStr1[13];			/* 存放当前所有符合要求的BMP名 */
	int bmp_count=0;
	OS os[4];
	unsigned int i,j,winx=320,winy=220;
	char ack=0,bmpname[13],os_str[13];
	
	for(i=0;i<12;i++)
		for(j=0;j<13;j++)
			bmparray[i][j]='\0';
	initKeyData(&para,os);			/* 初始化关键数据 */
	
	imemset(pal,0,sizeof(SvgaDAC)*256);
	initDisplay(0x105,pal);				/* 初始化图形显示 */
	imemcpy(pal,sys_pal,sizeof(SvgaDAC)*256);
	iFacePic(BOTTOM_FACE,RIGHT_FACE);
	iMessage(WHITE);			
	SvgaSmoothOn(pal);
	imemset(VerStr1,0,13);
	sprintf(VerStr1,"%.2x.%.2x",(iFoxVer>>8)&0xff,iFoxVer&0xff);
	setCur(winx+1,winy-100+1);
	c_printf(DEEPBLUE,firSetup_str[12],ifoxdisk,VerStr1);
	setCur(winx,winy-100);
	c_printf(WHITE,firSetup_str[12],ifoxdisk,VerStr1);
	
	iWindowOut(winx,winy,firSetup_str[0],10);
	
	
	setCur(winx+8,winy+40);
	c_printf(SURFACE1,firSetup_str[1]);
	
	setCur(winx+8,winy+40+24*2);
	c_printf(WHITE,firSetup_str[9]);
	setCur(winx+8,winy+40+24*3);
	if(saviFoxPic(para.iwapaper,6,"ifox.bmp"))
	{
		c_printf(WHITE,firSetup_str[11]);
		asm xor ax,ax
		asm int 0x16
		return;
	}
	else
	{
		c_printf(WHITE,firSetup_str[10]);
		iDelay(200);
	}
		
	rectblock(winx+8,winy+40+24,winx+8+(66<<3),winy+40+9*24+16,GROUND1);
	for(i=0;i<5;i++)
	{
		setCur(winx+8,winy+40+24*(i+1));
		c_printf(WHITE,firSetup_str[i+2]);
		iDelay(500);
	}
	setCur(winx+8,winy+40+24*6);
	if(Setup(para.lba_ifox,SETUPFOX))
	{
		c_printf(WHITE,firSetup_str[8]);
		asm xor ax,ax
		asm int 0x16
		return;
	}
	else
		c_printf(WHITE,firSetup_str[7]);
	asm xor ax,ax
	asm int 0x16
	InitIntRes(CLOCK);
	enable1CH();
	/* 进入壁纸的安装阶段 */
	{
		rectblock(winx+8,winy+40+24,winx+8+(66<<3),winy+40+9*24+16,GROUND1);
		setCur(winx+8,winy+40+24);
		c_printf(WHITE,firSetup_wp[0]);
		setCur(winx+8,winy+40+24*2);
		c_printf(WHITE,firSetup_wp[3]);
		bmp_count=dirWallPaper(bmparray);
		rectblock(winx+8,winy+40+24*2,winx+8+(66<<3),winy+40+24*2+16,GROUND1);
		if(bmp_count)
		{
			setCur(winx+8,winy+40+24*2);
			c_printf(WHITE,firSetup_wp[1]);
			for(j=0;j<3;j++)
				for(i=0;i<4;i++)
				{
					setCur(winx+8+4*8+i*13*8,winy+40+24*(3+j));
					c_printf(SURFACE1,firSetup_wp[2],bmparray[j*4+i]);
				}
			i=0;
			ack='y';
			while((ack=='Y') || (ack=='y'))
			{
				rectblock(winx+8,winy+40+24*6,winx+8+(66<<3),winy+40+24*9+16,GROUND1);
				setCur(winx+8,winy+40+24*6);
				c_printf(WHITE,firSetup_wp[6],para.wpaper_num,i+1);
				if(i>=6)
				{
					rectblock(winx+8,winy+40+24*6,winx+8+(66<<3),winy+40+24*6+16,GROUND1);
					setCur(winx+8,winy+40+24*7);
					c_printf(WHITE,firSetup_wp[8]);
					asm xor ax,ax
					asm int 0x16
					break;	
				}
				imemset(bmpname,0,13);
				setCur(winx+8,winy+40+24*7);
				c_scanf(DEEPBLUE,WHITE,firSetup_wp[7],bmpname);
				if(savWallPaper(para.iwapaper,i,bmpname))
				{		/* 安装失败 */
					ack=0;
					while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
					{
						setCur(winx+8,winy+40+24*8);
						c_scanf(DEEPBLUE,WHITE,firSetup_wp[9],&ack);
					}
					if((ack=='n') || (ack=='N'))
						break;
				}
				else
				{			/* 安装成功 */
					ack=0;
					while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
					{
						setCur(winx+8,winy+40+24*8);
						c_scanf(DEEPBLUE,WHITE,firSetup_wp[10],&ack);
					}
					++i;
					++para.wpaper_num;
					if((ack=='n') || (ack=='N'))
						break;
				}
			}
			setCur(winx+8,winy+40+24*9);
			c_printf(WHITE,firSetup_wp[11],para.wpaper_num);
			asm xor ax,ax
			asm int 0x16
		}
		else
		{
			setCur(winx+8,winy+40+24*3);
			c_printf(WHITE,firSetup_wp[4]);
			setCur(winx+8,winy+40+24*4);
			c_printf(WHITE,firSetup_wp[5]);
			asm xor ax,ax
			asm int 0x16
		}
	}
	/* 加载系统信息 */
	{
		rectblock(winx+8,winy+40+24,winx+8+(66<<3),winy+40+9*24+16,GROUND1);
		setCur(winx+8,winy+40+24);
		c_printf(WHITE,firSetup_os[0]);
		setCur(winx+8,winy+40+24*2);
		c_printf(WHITE,firSetup_os[1]);
		
		imemset(os_str,'\0',13);
		while(os_str[0]=='\0')
		{
			setCur(winx+8,winy+40+24*3);
			c_scanf(DEEPBLUE,WHITE,firSetup_os[2],os_str);
		}
		/*为了显示一致,对接收到的字符串进行右对齐的处理 12字节 */
		j=c_strlen(os_str);
		for(i=0;i<j;i++)
			os_str[11-i]=os_str[j-1-i];
		for(i=0;i<(12-j);i++)
			os_str[i]=' ';
		os_str[12]='\0';
		imemcpy(os[0].os_name,os_str,13);
		os[0].os_valid=1;
		imemset(os_str,'\0',13);
		setCur(winx+8,winy+40+24*4);
		c_scanf(DEEPBLUE,WHITE,firSetup_os[3],os_str);
		if(os_str[0]!='\0')
		{
			imemcpy(os[0].pwd,os_str,13);
			os[0].pwd_valid=1;
		}
		/*保存用户的设置,并将os[4]的信息和其MBR区拷贝到相应位置,其余全部设置为'Sun0'*/
		imemset(buff,0,512);
		if(linear_ReadWDK(0, 1, buff))
		{
			setCur(winx+8,winy+40+24*5);
			c_printf(WHITE,firSetup_os[4]);
			asm xor ax,ax
			asm int 0x16
			return;
		}
		else
		{
			if(linear_WriteWDK(os[0].os_mbr, 1, buff))
			{
				setCur(winx+8,winy+40+24*5);
				c_printf(WHITE,firSetup_os[4]);
					asm xor ax,ax
				asm int 0x16
				return;
			}
			imemset(buff,0,512);
			buff[0]='S';
			buff[1]='u';
			buff[2]='n';
			buff[3]='0';
			buff[0x1fe]=0x55;
			buff[0x1ff]=0xaa;
			for(i=1;i<4;i++)
				if(linear_WriteWDK(os[i].os_mbr, 1, buff))
				{
					setCur(winx+8,winy+40+24*5);
					c_printf(WHITE,firSetup_os[4]);
					asm xor ax,ax
					asm int 0x16
					return;
				}
			imemcpy(buff,mircosoft,510);
			if(linear_WriteWDK(os[3].os_mbr+1, 1, buff))
			{
					setCur(winx+8,winy+40+24*5);
					c_printf(WHITE,firSetup_os[4]);
					asm xor ax,ax
					asm int 0x16
					return;
			}
			
		}
		setCur(winx+8,winy+40+24*5);
		c_printf(WHITE,firSetup_os[5]);
		setCur(winx+8,winy+40+24*6);
		c_printf(WHITE,firSetup_os[6]);
		asm xor ax,ax
		asm int 0x16
	}
	/* 保存用户参数和系统信息 */
	{
		rectblock(winx+8,winy+40+24,winx+8+(66<<3),winy+40+9*24+16,GROUND1);
		setCur(winx+8,winy+40+24);
		c_printf(WHITE,firSetup_para[0]);
		setCur(winx+8,winy+40+24*2);
		c_printf(WHITE,firSetup_para[1]);
		setCur(winx+8,winy+40+24*3);
		c_printf(WHITE,firSetup_para[2]);
		imemset(buff,0,512);
		buff[0x1fe]=0x55;
		buff[0x1ff]=0xaa;
		buff[0]='L';
		buff[1]='U';
		buff[2]='O';
		buff[3]='B';
		imemcpy(buff+0x10,&para,sizeof(FOXPARA));
		if(linear_WriteWDK(para.lba_ipara, 1, buff))
			c_printf(RED,firSetup_para[6]);
		else	c_printf(RED,firSetup_para[5]);
		setCur(winx+8,winy+40+24*4);
		c_printf(WHITE,firSetup_para[3]);
		imemset(buff,0,510);
		imemcpy(buff,os,sizeof(OS)*4);
		if(linear_WriteWDK(para.lba_ios, 1, buff))
			c_printf(RED,firSetup_para[6]);
		else	c_printf(RED,firSetup_para[5]);
		setCur(winx+8,winy+40+24*5);
		c_printf(WHITE,firSetup_para[4]);
		if(Setup(para.lba_ifox,SETUPMBR)) c_printf(RED,firSetup_para[6]);
		else	c_printf(RED,firSetup_para[5]);
		setCur(winx+8,winy+40+24*7);
		c_printf(WHITE,firSetup_para[7]);
		asm xor ax,ax
		asm int 0x16
		
	}
	disable1CH();
	RelIntRes(CLOCK);
	
	SvgaSmoothOff(pal);
	
	asm mov ax,3
	asm int 0x10
	
}

/* 初始化安装用的关键数据 */
void initKeyData(FOXPARA *para,OS *os)
{
	int i;

	para->cntdown_time=15;				/* 倒计时时间,-1为无效 */                   
	para->cntdown_x=750;	        /* 显示倒计时的坐标 */                      
	para->cntdown_y=660;                                                      
	para->def_os=0;			          /* 默认进入的系统,0～4 */                   
	para->wpaper_num=0;           /* 壁纸的数目,如果为0则wallpaper必须为-1 */ 
	para->wallpaper=-1;		        /* 壁纸的选定 -1表示无效,即隐藏壁纸 */      
	para->lba_ipara=CHS_to_Sect(CUR_DK->lgeo.maxCyl+1,0,1,CUR_DK);
																/* 参数表存放的lba地址 由安装程序初始化 */  
	para->lba_ios=para->lba_ipara+1;	      /* OS信息存放的lba地址 由安装程序初始化 */
	para->lba_ifox=	para->lba_ios+1+4+1;
	para->ver=iFoxVer;				      /* 版本变迁描述,从0x0100开始 */  
	for(i=0;i<7;i++)
	{
		para->iwapaper[i].lba_addr=CHS_to_Sect(CUR_DK->lgeo.maxCyl+2,0,1,CUR_DK)+1l*i*0x602;  
		para->iwapaper[i].valid=0; 
	}        
 	for(i=0;i<4;i++)
 	{
 		os[i].bar_index=(unsigned char)(i);	
 		os[i].os_valid=0;	
 		imemset(os[i].os_name,' ',12);
 		os[i].os_name[12]='\0';
 		os[i].pwd_valid=0;	
 		os[i].pwd[0]='\0';		
 	 	os[i].os_mbr=para->lba_ios+1+1l*i;		/* para os osmbr1~4 osmbr_def */
 	}		      	        
}
         
/* 安装壁纸*/
/*入口参数: */
/*出口参数: 0:安装成功 0x01:文件未发现 0x02:非标准256色 非压缩1024x768 BMP图 */
/*          0x04:硬盘读写失败 */
/*确保wp中已经包含有效的地址 */
int savWallPaper(iWallPaper *wp,int nwp,char *file)
{
	FILE *fp;
	bmp_picture bmp256;
	SvgaDAC pal[256];				/* SvgaDAC四个字节,共256*4=1024个字节 2个扇区 */
	int i;
	unsigned long j;
	unsigned buffer[2048];
	
	if((fp=fopen(file,"rb")) ==NULL)
		return 0x01;
	fread(&bmp256.file,sizeof(bitmapfile),1,fp);
	fread(&bmp256.info,sizeof(bitmapinfo),1,fp);
	if(Check_Bmp((bmp_picture_ptr)&bmp256))
		return 0x02;
	for (i=0;i<256;i++)
 	{
 		fread(&bmp256.palette[i].blue,sizeof(unsigned char),1,fp);
 		fread(&bmp256.palette[i].green,sizeof(unsigned char),1,fp);
 		fread(&bmp256.palette[i].red,sizeof(unsigned char),1,fp);
 		fread(&bmp256.palette[i].reserved,sizeof(unsigned char),1,fp);
 	 	pal[i].b=(bmp256.palette[i].blue>>2);
 		pal[i].g=(bmp256.palette[i].green>>2);
 		pal[i].r=(bmp256.palette[i].red>>2);
 		pal[i].alpha=0;
	}
	if(linear_WriteWDK(wp[nwp].lba_addr,2,pal))
	{
		fclose(fp);
		return 0x04;
	}
	fseek(fp,1078,0);				/* 移动文件指针到数据区 */
	
	for(j=0;j<((1024l*768)>>11);j++)
	{
		fread(buffer,2048,1,fp);				/* 4个扇区 */
		if(linear_WriteWDK(wp[nwp].lba_addr+2+(j<<2),4,buffer))
		{
			fclose(fp);
			return 0x04;
		}
	}
	fclose(fp);	
	wp[nwp].valid=1;
	return 0;
}

/* 安装自己使用的界面 */
/*入口参数: */
/*出口参数: 0:安装成功 0x01:文件未发现 0x02:非标准256色 非压缩1024x768 BMP图 */
/*          0x04:硬盘读写失败 */
/*备注: 当前其实是将第七张壁纸当作缺省的界面来运作的 */
int saviFoxPic(iWallPaper *wp,int nwp,char *file)
{
	FILE *fp;
	bmp_picture bmp256;
	SvgaDAC pal[256];				/* SvgaDAC四个字节,共256*4=1024个字节 2个扇区 */
	int i,j;
	unsigned char buffer[1024],temp[1024];
	int startx,starty,endy,line_width;		/* line_width为当前图的行象素 */
																				/* bmp是行象素4字节对齐的,不足补0 */
	
	for(i=0;i<1024;i++)
		temp[i]=0x0;
	if((fp=fopen(file,"rb")) ==NULL)
		return 0x01;
	fread(&bmp256.file,sizeof(bitmapfile),1,fp);
	fread(&bmp256.info,sizeof(bitmapinfo),1,fp);
	
	if((bmp256.file.bfType!=0x4d42) ||(bmp256.info.biCompression!=0)||(bmp256.info.biBitCount!=8))
		return 0x02;
	
	for (i=0;i<256;i++)
 	{
 		fread(&bmp256.palette[i].blue,sizeof(unsigned char),1,fp);
 		fread(&bmp256.palette[i].green,sizeof(unsigned char),1,fp);
 		fread(&bmp256.palette[i].red,sizeof(unsigned char),1,fp);
 		fread(&bmp256.palette[i].reserved,sizeof(unsigned char),1,fp);
 	 	pal[i].b=(bmp256.palette[i].blue>>2);
 		pal[i].g=(bmp256.palette[i].green>>2);
 		pal[i].r=(bmp256.palette[i].red>>2);
 		pal[i].alpha=0;
	}
	if(linear_WriteWDK(wp[nwp].lba_addr,2,pal))
	{
		fclose(fp);
		return 0x04;
	}
	startx=(int)((1024l-bmp256.info.biWidth)>>1);
	starty=(int)((768l-bmp256.info.biHeight)>>1);
	endy=(int)(starty+(int)bmp256.info.biHeight-1);
	line_width=(int)(((bmp256.info.biWidth+3)>>2)<<2);		/* bmp图行象素为4字节对齐 */
	fseek(fp,1078,0);				/* 移动文件指针到数据区 */
	for(j=0;j<768;j++)
	{
		//if((j!=endy) || (j!=starty))
		if((j<=endy) && (j>=starty))
		{
			for(i=0;i<1024;i++)
					buffer[i]=0x0;
					
			fread(buffer+startx,line_width,1,fp);				
			if(linear_WriteWDK(wp[nwp].lba_addr+2+(j<<1),2,buffer))
			{
				fclose(fp);
				return 0x04;
			}
		}
		else
		{
			if(linear_WriteWDK(wp[nwp].lba_addr+2+(j<<1),2,temp))
			{
				fclose(fp);
				return 0x04;
			}
		}
	}
	fclose(fp);	
	//printf("startx=%d,starty=%d,endy=%d,Width=%d,Height=%d,line_width=%d\n",startx,starty,endy,(int)bmp256.info.biWidth,(int)bmp256.info.biHeight,line_width);
	wp[nwp].valid=1;
	return 0;
}
/*检查是否为256色bmp图*/
/*出口参数: 0x01:非bmp图  0x02:压缩的BMP图 0x04:非256色BMP图 0x08:非1024x768的图*/
int Check_Bmp(bmp_picture_ptr bmp_ptr)
{
	int err=0;
	if(bmp_ptr->file.bfType!=0x4d42)
		err|= 0x01;
	if(bmp_ptr->info.biCompression!=0)
		err|= 0x02;
	if(bmp_ptr->info.biBitCount!=8)
	  err|= 0x04;
	 if((bmp_ptr->info.biWidth!=1024) || (bmp_ptr->info.biHeight!=768))
	 	err|=0x08; 	
	return err;
}

/* Move data from srcSeg:srcOff to dstSeg:dstOff n bytes */
void	MoveData(unsigned int dstSeg,unsigned int dstOff,unsigned int srcSeg,unsigned int srcOff,unsigned int n)
{
  asm		push	ds			/* save ds     */
  asm		push	es      /* save es     */
  asm		pusha					/* save all    */
  asm		pushf					/* save flags  */
  asm		cld						/* clear direction*/
  asm		mov		es,dstSeg /* es:[di] <- dstSeg:dstOff */
  asm		mov		di,dstOff 
  asm		mov		ds,srcSeg /* ds:[si] <- srcSeg:srcOff */
  asm		mov		si,srcOff
  asm		mov		cx,n
  asm		rep		movsb				/* repeat cx times do es:[di] <-- ds:[si]  */

  asm		popf					/* restore flags */
  asm		popa					/* restore all   */
  asm		pop		es			/* restore es    */
  asm		pop		ds			/* restore ds    */

}

/* 获取符合要求的*.bmp文件 */
/*入口函数: 存放文件名的数组(保证至多存放12个文件名) */
/*出口函数: 找到的符合要求的数组个数 */
int dirWallPaper(unsigned char *file_array)
{
	FILE *fp;
	bmp_picture bmp256;
	struct ffblk file;			
	unsigned int done;
	int count=0;
	
	done=findfirst("*.bmp",&file,0);			/* 寻找通配符*.bmp */
	if(done)return 0;
	else
	{
		if((fp=fopen(file.ff_name,"rb")) !=NULL)
		{
			fread(&bmp256.file,sizeof(bitmapfile),1,fp);
			fread(&bmp256.info,sizeof(bitmapinfo),1,fp);
			if(!Check_Bmp((bmp_picture_ptr)&bmp256))			/* 检测是否符合要求的文件 */
			{
				imemcpy(file_array+count*13,file.ff_name,13);
				++count;
			}
			fclose(fp);	
		}
	}
	
	while((!done) && (count<12))
	{
		done=findnext(&file);
		if(((fp=fopen(file.ff_name,"rb")) !=NULL) && (!done))
		{
			fread(&bmp256.file,sizeof(bitmapfile),1,fp);
			fread(&bmp256.info,sizeof(bitmapinfo),1,fp);
			if(!Check_Bmp((bmp_picture_ptr)&bmp256))			
			{
				imemcpy(file_array+count*13,file.ff_name,13);
				++count;
			}
			fclose(fp);	
		}
	}
	return count;
}