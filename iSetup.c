#include <stdio.h>
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
#include "iSetup.h"
#include "Setup.h"
#include "Global.h"
#include "iMem.h"

unsigned int isFirSetup(FOXPARA *para);
void updateSetup(FOXPARA *para);
int upgradeFox(FOXPARA *para);
void updateMBR(FOXPARA *para,unsigned int x,unsigned int y);
void updateFox(FOXPARA *para,unsigned int x,unsigned int y);
void addWallPaper(FOXPARA *para,unsigned int winx,unsigned int winy,unsigned int flag);
int savFoxKeyData(FOXPARA *para,OS *my_os,unsigned int flag);
void loadNewOS(FOXPARA *para,unsigned int x,unsigned int y);

/* 判断是否第一次安装 */
/*通过三个部分判断是否第一安装: */
/*    1 MBR区sig位置的标志'LUOB',使用Setup.h中的结构 */
/*    2 para扇区处也有同样的标志 */
/*    3 Data起始扇区有 'iXLZ' 标志*/
/* 返回:  0:是第一次安装 如果不是第一次安装则返回当前版本号 0xffff表示出现意外错误 */
unsigned int isFirSetup(FOXPARA *para)
{
	BootPk mbr;
	unsigned char buff[512];
	unsigned int err=0;
	unsigned int ver=0;
	
	if(linear_ReadWDK(0, 1, (unsigned char *)&mbr))
		err=1;
	
	if( (mbr.sig[0]=='L') && (mbr.sig[1]=='U') && (mbr.sig[2]=='O') && (mbr.sig[3]=='B') )
		err=1;
	
	imemset(buff,0,512);
	if(linear_ReadWDK(para->lba_ipara, 1, buff))
		err=1;
	
	if((buff[0]=='L') && (buff[1]=='U') && (buff[2]=='O') && (buff[3]=='B'))
		err=1;
	
	imemset(buff,0,512);
	if(linear_ReadWDK(para->lba_ifox, 1, buff))
		err=1;
	
	if((buff[0]=='i') && (buff[1]=='X') && (buff[2]=='L') && (buff[3]=='Z'))
		err=1;
	
	ver=(unsigned int)buff[5] + (((unsigned int)buff[6])<<8);
	if(err)
	{ 
		if( (ver<0x9999) &&(ver>=0x0102))			/*从2008年开始的第一个版本号为0102，依次增长 */
			err = ver;
		else
			err=0xffff;
		}
	
	return err;
}

/* 更新程序 */
void updateSetup(FOXPARA *para)
{
	unsigned char *updateSetup_str[]={
		"安装引导--更新程序",				/* 0 */
		"发现已安装的版本较低(已安装版本:%s,当前版本:%s),",/* 1 */
		"您希望升级吗(Y/N)?: %c", /* 2 */
		"升级成功,您可以体验新版本的Foxdisk了^^", /* 3 */
		"升级失败,硬盘某些区域无法写入!", /* 4 */
		"检测到Foxdisk已经安装过了,您希望:", /* 5 */
		"1 加载新系统", /* 6 */
		"2 添加壁纸", /* 7 */
		"3 更改壁纸", /* 8 */		
		"4 还原Foxdisk的引导程序(用在没有出现界面的情况下,慎用!!!!)",  /* 9 */
		"5 重新安装代码",/* 10 */
		"请选择(0-5,选0或直接回车则退出当前程序): %d",  /* 11 */
		"感谢您的使用,再见!", /* 12 */
		"您选择了%d,将进入相应的处理程序,按任意键继续...", /* 13 */
		"程序:%s  内部版本码:%s" /* 14 */
	};
	unsigned int i,j,cur_ver=iFoxVer,winx=320,winy=220,sel=0xffff;
	
	char VerStr0[13],VerStr1[13],ack=0;
	SvgaDAC pal[256];
	
	imemset(pal,0,sizeof(SvgaDAC)*256);
	initDisplay(0x105,pal);				/* 初始化图形显示 */

	imemcpy(pal,sys_pal,sizeof(SvgaDAC)*256);
	imemset(VerStr1,0,13);
	sprintf(VerStr1,"%.2x.%.2x",(cur_ver>>8)&0xff,cur_ver&0xff);
	iFacePic(BOTTOM_FACE,RIGHT_FACE);
	iMessage(WHITE);			
	SvgaSmoothOn(pal);
	
	setCur(winx+1,winy-100+1);
	c_printf(DEEPBLUE,updateSetup_str[14],ifoxdisk,VerStr1);
	setCur(winx,winy-100);
	c_printf(WHITE,updateSetup_str[14],ifoxdisk,VerStr1);
	iWindowOut(winx,winy,updateSetup_str[0],10);

	InitIntRes(CLOCK);
	enable1CH();
		
	if(para->ver<cur_ver)
	{
		imemset(VerStr0,0,13);
		sprintf(VerStr0,"%.2x.%.2x",((para->ver)>>8)&0xff,(para->ver)&0xff);
		
		setCur(winx+8+16,winy+40+24);
		c_printf(WHITE,updateSetup_str[1],VerStr0,VerStr1);
		ack=0;
		while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
		{
			setCur(winx+8,winy+40+24*2);
			c_scanf(DEEPBLUE,WHITE,updateSetup_str[2],&ack);
		}
		if((ack=='Y') || (ack=='y'))
		{
			disable1CH();							/* 关闭时间中断 */
			RelIntRes(CLOCK);
			setCur(winx+8,winy+40+24*3);
			if(upgradeFox(para))
				c_printf(WHITE,updateSetup_str[4]);
			else
				c_printf(WHITE,updateSetup_str[3]);
			InitIntRes(CLOCK);				/* 重新打开时间中断 */
			enable1CH();
			asm xor ax,ax
			asm int 0x16	
		}
	}
	winx=320;winy=220;
	rectblock(winx+8,winy+40,winx+8+(66<<3),winy+40+9*24+16,GROUND1);
	setCur(winx+8,winy+40);
	c_printf(SURFACE1,updateSetup_str[5]);
	
	for(i=0;i<5;i++)
	{
		setCur(winx+8+8*4,winy+40+24*(1+i));
		c_printf(SURFACE1,updateSetup_str[6+i]);
	}
	
	while(sel>5)
	{
		setCur(winx+8,winy+40+24*6);
		c_scanf(DEEPBLUE,WHITE,updateSetup_str[11],&sel);
	}
	setCur(winx+8,winy+40+24*8);
	if(sel==0)
	{
		c_printf(WHITE,updateSetup_str[12]);
		SvgaSmoothOff(pal);
		disable1CH();							/* 关闭时间中断 */
		RelIntRes(CLOCK);				/* 关闭时间中断,使用权利转交给各子程序 */
		return;
	}
	else
		c_printf(WHITE,updateSetup_str[13],sel);
	asm xor ax,ax
	asm int 0x16
	rectblock(winx+8,winy+40,winx+8+(66<<3),winy+40+9*24+16,GROUND1);
	disable1CH();							/* 关闭时间中断 */
	RelIntRes(CLOCK);				/* 关闭时间中断,使用权利转交给各子程序 */
	switch(sel)
	{
		case 4:				/* 更新为Foxdisk MBR*/
			updateMBR(para,winx,winy);
			break;
		case 5:				/* 更新代码 */
			updateFox(para,winx,winy);
			break;			
		case 2:				/* 添加壁纸 */
			addWallPaper(para,winx,winy,0);
			break;
		case 3:				/* 更改壁纸 */
			addWallPaper(para,winx,winy,1);
			break;
		case 1:				/* 加载新系统 */
			loadNewOS(para,winx,winy);
			break;
		default:
			break;
	}
	SvgaSmoothOff(pal);
}

/* 更新MBR */
void updateMBR(FOXPARA *para,unsigned int x,unsigned int y)
{
	unsigned char *updatembr_str[]={
		"将还原Foxdisk的引导程序,请稍候...",
		"  分析Foxdisk的版本...",
		"  分析Foxdisk的数据库...",
		"  分析Foxdisk的代码...",
		"  还原Foxdisk专用引导程序...",
		"操作成功,感谢您的使用!",
		"操作失败,磁盘出现坏道!"
		"抱歉,已经安装的版本比较高,您不能使用低版本程序进行安装!",
	};
	unsigned int i;
	for(i=0;i<5;i++)
	{
		setCur(x+8,y+40+24*i);
		c_printf(WHITE,updatembr_str[i]);
		iDelay(200);
	}
	setCur(x+8,y+40+24*6);
	if(para->ver<=iFoxVer)
	{
		if(Setup(para->lba_ifox,SETUPMBR))
			c_printf(WHITE,updatembr_str[6]);
		else
			c_printf(WHITE,updatembr_str[5]);
	}
	else c_printf(WHITE,updatembr_str[7]);
	asm xor ax,ax
	asm int 0x16
}
/* 更新代码 */
void updateFox(FOXPARA *para,unsigned int x,unsigned int y)
{
	unsigned char *updatefox_str[]={
		"将重新安装Foxdisk的代码部分,请稍候...",
		"  分析Foxdisk的版本...",
		"  分析Foxdisk的数据库...",
		"  分析Foxdisk的代码...",
		"  还原Foxdisk专用引导程序...",
		"操作成功,感谢您的使用!",
		"操作失败,磁盘出现坏道!",
		"抱歉,已经安装的版本比较高,您不能使用低版本程序进行安装!",
	};
	unsigned int i;
	for(i=0;i<5;i++)
	{
		setCur(x+8,y+40+24*i);
		c_printf(WHITE,updatefox_str[i]);
		iDelay(200);
	}
	setCur(x+8,y+40+24*6);
	if(para->ver<=iFoxVer)
	{
		if(Setup(para->lba_ifox,SETUPFOX))
			c_printf(WHITE,updatefox_str[6]);
		else
			c_printf(WHITE,updatefox_str[5]);
	}
	else
		c_printf(WHITE,updatefox_str[7]);
	asm xor ax,ax
	asm int 0x16
}
/* 添加壁纸 与更新壁纸 */
/* flag=0:添加壁纸 ,flag=1:更新壁纸 */
void addWallPaper(FOXPARA *para,unsigned int winx,unsigned int winy,unsigned int flag)
{
	unsigned char *addwp_str[]={
		"请按照指导修改自己的壁纸库:",															/* 0 */
		"请稍候,正在从当前目录中获取可安装的壁纸...",							/* 1 */
		"    抱歉,当前目录没有发现可安装的壁纸,请确保您要安装的壁纸", /* 2 */
		"与安装程序放在同一目录下! 按任意键退出当前程序...", /* 3 */
		"当前目录下符合要求的壁纸有(1024x768 256色 BMP图):", /* 4 */
		"%s",												/* 5 */
		"已经安装的壁纸数: %d",			/* 6 */
		"将安装第%d张壁纸",					/* 7 */
		"请输入您要安装的壁纸名称: %s",	/* 8 */
		"已经安装了六张壁纸,无法再安装了!",			/* 9 */
		"安装失败!,文件名输入错误或者文件不符合要求,您还要继续吗(Y/N)?: %c", /* 10 */
		"您已成功安装壁纸,继续安装下一张吗(Y/N)?: %c", /* 11 */
		"  已安装壁纸总数: %d.", /* 12 */
		"保存当前设置......",   /* 13 */
		"成功!感谢您的使用^^",/* 14 */
		"很抱歉,保存失败!",   /* 15 */
		"抱歉,您还没有安装过壁纸,无法使用这个功能!",  /* 16 */
		"请输入您要替换的壁纸编号(1--%d):",  /* 17 */
		"%d",   /* 18 */
		"已成功替换,您要继续更新下一张吗(Y/N)?: %c"  /* 19 */
		};
	unsigned char bmparray[12][13];			/* 存放当前所有符合要求的BMP名 */
	int bmp_count=0,i,j;
	char ack=0,bmpname[13];
	imemset(bmparray,0x0,12*13);
	
	
	if((para->wpaper_num==0) && (flag==1))
	{
		setCur(winx+8,winy+40+24*3);
		c_printf(WHITE,addwp_str[16]);
		asm xor ax,ax
		asm int 0x16
		return;
	}
	InitIntRes(CLOCK);
	enable1CH();
	
	setCur(winx+8,winy+40);
	c_printf(SURFACE1,addwp_str[0]);
	setCur(winx+8,winy+40+24);
	c_printf(WHITE,addwp_str[1]);
	bmp_count=dirWallPaper(bmparray);
	rectblock(winx+8,winy+40+24,winx+8+(66<<3),winy+40+24+16,GROUND1);
	if(bmp_count )
	{
		setCur(winx+8,winy+40+24);
		c_printf(SURFACE1,addwp_str[4]);
		for(j=0;j<3;j++)
			for(i=0;i<4;i++)
			{
				setCur(winx+8+4*8+i*13*8,winy+40+24*(2+j));
				c_printf(SURFACE1,addwp_str[5],bmparray[j*4+i]);
			}
		if(flag==0)
		{
			i=para->wpaper_num;
			ack='y';
			while((ack=='Y') || (ack=='y'))
			{
				rectblock(winx+8,winy+40+24*5,winx+8+(66<<3),winy+40+24*9+16,GROUND1);
				setCur(winx+8,winy+40+24*5);
				c_printf(WHITE,addwp_str[6],para->wpaper_num);
				setCur(winx+8,winy+40+24*6);
				if(i<6)
					c_printf(WHITE,addwp_str[7],i+1);
				if(i>=6)
				{
					rectblock(winx+8,winy+40+24*6,winx+8+(66<<3),winy+40+24*6+16,GROUND1);
					setCur(winx+8,winy+40+24*7);
					c_printf(WHITE,addwp_str[9]);
					asm xor ax,ax
					asm int 0x16
					break;	
				}
				imemset(bmpname,0,13);
				setCur(winx+8,winy+40+24*7);
				c_scanf(DEEPBLUE,WHITE,addwp_str[8],bmpname);
				if(savWallPaper(para->iwapaper,i,bmpname))
				{		/* 安装失败 */
					ack=0;
					while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
					{
						setCur(winx+8,winy+40+24*8);
						c_scanf(DEEPBLUE,WHITE,addwp_str[10],&ack);
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
						c_scanf(DEEPBLUE,WHITE,addwp_str[11],&ack);
					}
					++i;
					++para->wpaper_num;
					if((ack=='n') || (ack=='N'))
						break;
				}
			}  
		}
		else
		{
			ack='y';
			while((ack=='Y') || (ack=='y'))
			{
				rectblock(winx+8,winy+40+24*5,winx+8+(66<<3),winy+40+24*9+16,GROUND1);
				setCur(winx+8,winy+40+24*5);
				c_printf(WHITE,addwp_str[6],para->wpaper_num);
				setCur(winx+8,winy+40+24*6);
				c_printf(WHITE,addwp_str[17],para->wpaper_num);
				i=0;
				while((i<1) || (i>para->wpaper_num))
				{
					setCur(winx+8+(c_strlen(addwp_str[17])<<3),winy+40+24*6);
					c_scanf(DEEPBLUE,WHITE,addwp_str[18],&i);
				}
				imemset(bmpname,0,13);
				setCur(winx+8,winy+40+24*7);
				c_scanf(DEEPBLUE,WHITE,addwp_str[8],bmpname);
				if(savWallPaper(para->iwapaper,i-1,bmpname))
				{		/* 安装失败 */
					ack=0;
					while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
					{
						setCur(winx+8,winy+40+24*8);
						c_scanf(DEEPBLUE,WHITE,addwp_str[10],&ack);
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
						c_scanf(DEEPBLUE,WHITE,addwp_str[19],&ack);
					}
					if((ack=='n') || (ack=='N'))
						break;
				}
			}
		}
		rectblock(winx+8,winy+40+24*8,winx+8+(66<<3),winy+40+24*8+16,GROUND1);
		setCur(winx+8,winy+40+24*8);
		c_printf(WHITE,addwp_str[12],para->wpaper_num);
		setCur(winx+8,winy+40+24*9);
		c_printf(WHITE,addwp_str[13],para->wpaper_num);
		iDelay(500);
		if(savFoxKeyData(para,NULL,SAVEKEYPARA))
			c_printf(WHITE,addwp_str[15]);
		else
			c_printf(WHITE,addwp_str[14]);
		asm xor ax,ax
		asm int 0x16
	}
	else
	{
		setCur(winx+8,winy+40+24*2);
		c_printf(WHITE,addwp_str[2]);
		setCur(winx+8,winy+40+24*3);
		c_printf(WHITE,addwp_str[3]);
		asm xor ax,ax
		asm int 0x16
	} 
	 
	
	disable1CH();							/* 关闭时间中断 */
	RelIntRes(CLOCK);				/* 关闭时间中断,使用权利转交给各子程序 */   
}  

/* 加载新系统 */
void loadNewOS(FOXPARA *para,unsigned int x,unsigned int y)
{
	unsigned char *loados_str[]={
		"加载系统:将新系统的信息添加到Foxdisk的数据库",			/* 0 */
		"Foxdisk数据库中已有系统信息:",/* 1 */
		"系统%d:%s",	/* 2 */
		"当前系统已经存在数据库中了,您不需要更改!",					/* 3 */
		"当前的系统在数据库中已经存在,您可以:",				/* 4 */
		" 1 使用恢复Foxdisk的功能1 恢复引导程序(强烈建议!)",				/* 5 */
		" 2 仍然继续加载并替换(不建议!)",										/* 6 */
		"请选择(1/2):%d",	/* 7 */
		"谢谢您的使用,请重新运行Foxdisk选择功能1恢复引导程序.",/* 8 */
		"将当前系统录入到Foxdisk的数据库中(替换系统%d:%s):",  /* 9 */
		"请输入新的系统名称:%s",  /* 10 */
		"录入成功,感谢您的使用!",	/* 11 */
		"录入失败,存放Foxdisk的硬盘扇区有坏道!", /* 12 */
		"数据库中只能存储四个系统信息,如果要加载新系统,只能替换其中一个.",		/*13*/
		"当前系统与系统%d:%s的分区表相同,建议您替换这个系统.",/*14*/
		"您要替换哪个系统?(1--4): %d", /*15*/
		"此操作有一定的风险,您确定要替换吗(Y/N)?: %c", /*16*/
		"操作成功,感谢您的使用^^",   /* 17 */
		"操作失败,存放Foxdisk的硬盘扇区有坏道!",/*18*/
		"    感谢您的使用^^",/* 19*/
		
	};
	OS os[4];
	BootPk mbr;
	unsigned char buff[512],*ptr,os_str[13];
	unsigned int i,j,iLoadLen,flag=0,uGet=0,nos;
	int key_os=-1;
	char ack=0;
	
	InitIntRes(CLOCK);
	enable1CH();
	
	setCur(x+8,y+40);
	imemset(buff,0,512);
	if(linear_ReadWDK(para->lba_ios, 1, buff))
	{
		c_printf(WHITE,"Access HD Error!");
		disable1CH();							/* 关闭时间中断 */
				RelIntRes(CLOCK);	
		return;
	}
	imemcpy(os,buff,sizeof(OS)*4);				/* 取当前系统信息 */
	c_printf(SURFACE1,loados_str[0]);
	setCur(x+8,y+40+24);
	c_printf(SURFACE1,loados_str[1]);
	for(i=0;i<4;i++)
	{
		if(i<2)
			setCur(x+8+(23<<3)*i+4*8,y+40+24*2);
		else
			setCur(x+8+(23<<3)*(i-2)+4*8,y+40+24*3);
		if(os[i].os_valid)
			c_printf(SURFACE1,loados_str[2],(unsigned int)(os[i].bar_index+1),os[i].os_name);
	}
	imemset(&mbr,0,512);
	setCur(x+8,y+40+24*4);
	if(linear_ReadWDK(0, 1, &mbr))					/* 读取当前MBR的信息到mbr */
	{
		c_printf(WHITE,"Access HD Error!");
		disable1CH();							/* 关闭时间中断 */
		RelIntRes(CLOCK);	
		return;
	}
	ptr=(unsigned char *)(&mbr);
	iLoadLen = FP_OFFS(end_load)-FP_OFFS(beg_load);
	for(i=0;i<4;i++)												/* 比较当前分区和数据库中的分区是否有相同的 */
	{
		imemset(buff,0,512);
		if(linear_ReadWDK(os[i].os_mbr, 1, buff))					/* 读取os的信息到buff */
		{
			c_printf(WHITE,"Access HD Error!");
			disable1CH();							/* 关闭时间中断 */
			RelIntRes(CLOCK);	
			return;
		}
		//if(!imemcmp(ptr+iLoadLen,buff+iLoadLen,512-iLoadLen))
		if(!imemcmp(ptr+0x1be,buff+0x1be,64))
		{
			flag=1;
			key_os=(int)i;
			break;
		}
	}	
	if(flag)
	{
		if(!imemcmp(mbr.sig,"LUOB",4))			/* 存在匹配,且当前MBR为 foxdisk mbr*/
		{
			c_printf(WHITE,loados_str[3]);
			asm xor ax,ax
			asm int 0x16
			disable1CH();							/* 关闭时间中断 */
			RelIntRes(CLOCK);				
			return;
		}
		else
		{
			for(i=0;i<3;i++)
			{
				setCur(x+8,y+40+24*(4+i));
				c_printf(WHITE,loados_str[4+i]);
			}
			while((uGet!=1) && (uGet!=2))
			{
				setCur(x+8,y+40+24*7);
				c_scanf(DEEPBLUE,WHITE,loados_str[7],&uGet);
			}
			if(uGet==1)
			{
				setCur(x+8,y+40+24*8);
				c_printf(WHITE,loados_str[8]);
				asm xor ax,ax
				asm int 0x16
				disable1CH();							/* 关闭时间中断 */
				RelIntRes(CLOCK);				
				return;
			}
		}	
	}
	/* 寻找可以加入的空位 */
	rectblock(x+8,y+40+24*4,x+8+(66<<3),y+40+24*9+16,GROUND1);
	flag=0;
	for(i=0; i<4; i++)
	{
		imemset(buff,0,512);
		if(linear_ReadWDK(os[i].os_mbr, 1, buff))					/* 读取os的信息到buff */
		{
			c_printf(WHITE,"Access HD Error!");
			disable1CH();							/* 关闭时间中断 */
			RelIntRes(CLOCK);	
			return;
		}
		if(!imemcmp(buff,"Sun0",4))
		{
			unsigned char temp[64];
			imemset(temp,0,64);
			if(!imemcmp(buff+0x1be,temp,64))		/* 分区表是否为0 */
			{
				flag=1;
				break;
			}
		}
		
	}
	/* 如果flag==1,则os[i]为空位或者 uGet==2时 key_os中存放了替换的系统 */
	if((flag)||(uGet==2))
	{
		if(uGet==2)
			nos=key_os;
		else
			nos=i;			/* 保存找到的空位 */
		setCur(x+8,y+40+24*4);
		c_printf(WHITE,loados_str[9],os[nos].bar_index+1,os[nos].os_name);
		imemset(os_str,'\0',13);
		while(os_str[0]=='\0')
		{
			setCur(x+8,y+40+24*5);
			c_scanf(DEEPBLUE,WHITE,loados_str[10],os_str);
		}
		/*为了显示一致,对接收到的字符串进行右对齐的处理 12字节 */
		j=c_strlen(os_str);
		for(i=0;i<j;i++)
			os_str[11-i]=os_str[j-1-i];
		for(i=0;i<(12-j);i++)
			os_str[i]=' ';
		os_str[12]='\0';
		imemcpy(os[nos].os_name,os_str,13);		/* 保存用户设定 */
		os[nos].os_valid=1;
		if(imemcmp(mbr.sig,"LUOB",4))				/* mbr区不是foxdiskmbr*/
			imemcpy(buff,ptr,512);
		else
			imemcpy(buff+iLoadLen,ptr+iLoadLen,512-iLoadLen);		/* 只加载分区表 */
		/* 保存设定 */
		setCur(x+8,y+40+24*6);
		if( linear_WriteWDK(os[nos].os_mbr, 1, buff) || savFoxKeyData(para,os,SAVEKEYOS))
			c_printf(WHITE,loados_str[12]);
		else
			c_printf(WHITE,loados_str[11]);	
	}
	else
	{
		setCur(x+8,y+40+24*4);
		c_printf(WHITE,loados_str[13]);
		setCur(x+8,y+40+24*5);
		if(key_os!=-1)
			c_printf(RED,loados_str[14],(unsigned int)(os[key_os].bar_index+1),os[key_os].os_name);
		i=0;
		while((i<1) || (i>4))
		{
			setCur(x+8,y+40+24*6);
			c_scanf(DEEPBLUE,WHITE,loados_str[15],&i);
		}
		for(j=0;j<4;j++)
			if((unsigned int)(os[j].bar_index+1)==i)
				nos=j;			/* 找到要替换的系统 */
		while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
		{
			setCur(x+8,y+40+24*7);
			c_scanf(DEEPBLUE,WHITE,loados_str[16],&ack);
		}
		if((ack=='Y') || (ack=='y'))
		{
			/* 开始替换 */
			imemset(os_str,'\0',13);
			while(os_str[0]=='\0')
			{
				setCur(x+8,y+40+24*8);
				c_scanf(DEEPBLUE,WHITE,loados_str[10],os_str);
			}
			/*为了显示一致,对接收到的字符串进行右对齐的处理 12字节 */
			j=c_strlen(os_str);
			for(i=0;i<j;i++)
				os_str[11-i]=os_str[j-1-i];
			for(i=0;i<(12-j);i++)
				os_str[i]=' ';
			os_str[12]='\0';
			imemcpy(os[nos].os_name,os_str,13);		/* 保存用户设定 */
			os[nos].os_valid=1;
			imemset(buff,0,512);
			linear_ReadWDK(os[nos].os_mbr, 1, buff);
			if(imemcmp(mbr.sig,"LUOB",4))				/* mbr区不是foxdiskmbr*/
				imemcpy(buff,ptr,512);
			else
				imemcpy(buff+iLoadLen,ptr+iLoadLen,512-iLoadLen);		/* 只加载分区表 */
			/* 保存设定 */
			setCur(x+8,y+40+24*9);
			if( linear_WriteWDK(os[nos].os_mbr, 1, buff) || savFoxKeyData(para,os,SAVEKEYOS))
				c_printf(WHITE,loados_str[18]);
			else
				c_printf(WHITE,loados_str[17]);	
		}
		else
		{
			setCur(x+8,y+40+24*8);
			c_printf(WHITE,loados_str[19]);
		}
	}
	disable1CH();							/* 关闭时间中断 */
	RelIntRes(CLOCK);	
	asm xor ax,ax
	asm int 0x16
}

/* 保存关键数据 */
/*返回0: 保存成功 返回1:保存失败 */
int savFoxKeyData(FOXPARA *para,OS *my_os,unsigned int flag)
{
	FOXPARA old_para;
	OS old_os[4];
	unsigned char buff[512];

	if(flag&SAVEKEYPARA)	
	{
		imemset(buff,0,512);
		if(linear_ReadWDK(para->lba_ipara, 1, buff))
			return 0x01;
		imemcpy(&old_para,buff+0x10,sizeof(FOXPARA));
		if(imemcmp(para,&old_para,sizeof(FOXPARA)))		/* 判断是否相同 */
		{
			imemcpy(buff+0x10,para,sizeof(FOXPARA));			/* 不同则准备保存 */
			if(linear_WriteWDK(para->lba_ipara, 1, buff))
				return 0x01;
		}	
	}
	if(flag&SAVEKEYOS)	
	{
		imemset(buff,0,512);
		if(linear_ReadWDK(para->lba_ios, 1, buff))
			return 0x01;
		imemcpy(old_os,buff,sizeof(OS)*4);
		if(imemcmp(my_os,old_os,sizeof(OS)*4))		/* 判断是否相同 */
		{
			imemcpy(buff,my_os,sizeof(OS)*4);			/* 不同则准备保存 */
			if(linear_WriteWDK(para->lba_ios, 1, buff))
				return 0x01;
		}	
	}
	
	return 0;
	
}   
/* 版本变迁的升级 */
/*返回:0表示版本升级成功,其余表示失败 */
/* 为将来版本间不同差异提供一个迁移平台 */
int upgradeFox(FOXPARA *para)
{     
	/* 现在因为数据库是相同的,不需要转换。将来如果发生换代的时候再做这件事 */
	if(iFoxVer>=para->ver)
	{   
		return Setup(para->lba_ifox,SETUPFOX|SETUPMBR);
	}  
	return 0; 
}     
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
			
      
      
      
      
      
      
      
      
      
      
			
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      