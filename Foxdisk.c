#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"

#include "Common.h"
#include "Painter.h"
#include "SetPara.h"
#include "MainMenu.h"
#include "BootMenu.h"

#include "DISKDRV.h"
#include "DISKAPP.h"
#include "Foxdisk.h"

void foxdisk(void);
unsigned int initEntry(FOXPARA *para,OS *my_os,unsigned long lba_para);
void abnormalExit(unsigned int err);
int SavFoxPara(FOXPARA *para);
int SavOSMsg(FOXPARA *para,OS *os);
int i_memcmp(void *cs,void *ct,unsigned int count);
void	BootEntry(void);

/* boot程序进入foxdisk引导阶段的入口 */
void	BootEntry(void)
{
	foxdisk();
}

/* 执行程序 */
/*主要功能: 提供两个阶段的处理: 显示引导阶段和主程序阶段 */
void foxdisk(void)
{
	FOXPARA para;
	OS my_os[4];
	iWallPaper iwapaper[5];					/* 因只有安装程序才能改变此项目值,所以单独列出作为只读的数据项 */
	unsigned long LBA_iPara=0;							/* 参数表的位置,最大磁道数-10 即如519为最大磁道数,则参数起始位置为510*/
	unsigned int err=0;
	/* 第一步,从指定的存储位置获取参数信息 */
	CreateDiskParaTable();									/* 建立参数表,预备进行硬盘访问 */
	LBA_iPara=CHS_to_Sect(CUR_DK->lgeo.maxCyl+1,0,1,CUR_DK);
	err=initEntry(&para,my_os,LBA_iPara);
	if(err)
		abnormalExit(err);
	/*lbdebug_init(&para,my_os);*/
	/* 第二步,引导阶段显示 */
	bootMeun(&para,my_os);				/* 如果没有按'F2'的话,将直接进入当前系统 */

	/* 第三步,主程序阶段 */
	SysRun(my_os,&para);
}
/* 入口变量的初始化 */
/*入口参数: FOXPARA -foxdisk参数的存放地址 */
/*出口参数: 0:获取成功 非0:错误,各种错误见头文件中的定义 */
unsigned int initEntry(FOXPARA *para,OS *my_os,unsigned long lba_para)
{
	unsigned char buffer[512],*ptr;
	int i;

	for(i=0;i<512;i++)buffer[i]=0;
	/* 从硬盘指定位置获取信息 */
	if ( linear_ReadWDK(lba_para, 1, buffer) != 0 )					/* 从指定位置读取信息*/
		return PARALBAERR;
	if(!((buffer[0]=='L') && (buffer[1]=='U') && (buffer[2]=='O') && (buffer[3]=='B') ))
		return PARALBAERR;						/* 判断预放的标志 'LUOB' */
	/* 0x10开始存放的是para的信息 */	
	ptr=(unsigned char *)(para);
	for(i=0; i<sizeof(FOXPARA); i++)
		ptr[i]=buffer[i+0x10];
	if(para->lba_ipara!=lba_para)
		return PARAADRERR;
	/* 获取OS的信息 */
	ptr=(unsigned char *)(my_os);
	if( linear_ReadWDK(para->lba_ios, 1, buffer) != 0 )				/* 读取系统信息 */
		return OSMSGERR;
	for(i=0;i<sizeof(OS)*4;i++)
		ptr[i]=buffer[i];

	return 0;
}


/* 保存参数表 */
/*出口参数: 0:保存成功 0x01:保存失败*/
int SavFoxPara(FOXPARA *para) 
{
	unsigned char buff[512],*ptr;
	int i;
	if( linear_ReadWDK(para->lba_ipara, 1, buff) != 0 )
		return 1;
	if(i_memcmp(buff+0x10,para,sizeof(FOXPARA)))		/* 比较是否相同 */
	{
		ptr=(unsigned char *)(para);
		for(i=0;i<sizeof(FOXPARA);i++)
			buff[0x10+i]=ptr[i];
		if(linear_WriteWDK(para->lba_ipara, 1, buff) != 0)
			return 1;
	}
	return 0;
}
/* 保存四个系统的信息 */
/*出口参数: 0:保存成功 0x01:保存失败*/
int SavOSMsg(FOXPARA *para,OS *os) 
{
	unsigned char buff[512],*ptr;
	int i;
	if( linear_ReadWDK(para->lba_ios, 1, buff) != 0 )	/* 读取信息 */
		return 1;
	if(i_memcmp(buff,os,(sizeof(OS)<<2)))		/* 比较4个系统信息是否相同 */
	{
		ptr=(unsigned char *)(os);
		for(i=0;i<(sizeof(OS)<<2);i++)
			buff[i]=ptr[i];
		if(linear_WriteWDK(para->lba_ios, 1, buff) != 0)
			return 1;
	}
	return 0;
}

/* 内存比较 */
/*出口参数: <0 cs<ct   =0 cs=ct   >1 cs>ct*/
int i_memcmp(void *cs,void *ct,unsigned int count)
{
	unsigned char *su1,*su2;
	int res=0;
	for(su1=cs,su2=ct;count>0;++su1,++su2,count--)
		if((res=*su1-*su2)!=0)
			break;
	return res;
}  

/* 意外错误导致程序非正常退出 */
void abnormalExit(unsigned int err)
{
  asm {
  	jmp	idata_over
			db 'Foxdisk', 0x27,'s parameter files lost!',00h
  		db 'What you can do:',00h
  		db '1 Reinstall foxdisk.',00h
      db '2 Regret.',00h
      db '3 Vituperation.',00h            		
      db '4 Crying.',00h            		
      db '5 Never use foxdisk again.',00h            		
      db 'I choose 1,what about you?^^',00h                
  }
  idata_over:
  
}  
  
  