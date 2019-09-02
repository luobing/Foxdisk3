#include <stdio.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>

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
	unsigned int ver;									  /* 版本变迁描述,从0x0100开始 */
	iWallPaper iwapaper[6];
	
	/*signed int modified;								/* 用户是否更新了系统参数 0为未更改 */
}FOXPARA;
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
void tt(FOXPARA *para)
{
	FOXPARA old_para={0xf0f0,0x55};
	unsigned char *now,*old,*tx;
	unsigned char *ptr,*ptr1;
	int i;
	
	now=(unsigned char *)para;
	old=(unsigned char *)(&old_para);
	ptr1=(unsigned char *)(&old_para);
	printf("\nluobing old[0]=%d\n",old[0]);
	printf("\nluobing ptr1[0]=%d\n",ptr1[0]);
	

	
	for(i=0;i<sizeof(FOXPARA);i++)
		old[i]=now[i];
	tx=(unsigned char *)(&old_para);	

	printf("old:");
	for(i=0;i<sizeof(FOXPARA)/3;i++)
		printf("%d,",old[i]);
	printf("\ntx:");
	for(i=0;i<sizeof(FOXPARA)/3;i++)
		printf("%d,",tx[i]);
		
	printf("\nnow:");
	for(i=0;i<sizeof(FOXPARA)/3;i++)
		printf("%d,",now[i]);	
}

void ttt(FOXPARA *para)
{
	FOXPARA old_para={0x0f0f,0x55};
	int i;
	unsigned char *cur,*old;
	unsigned char *ptr,*ptr1;
	
	ptr1=(unsigned char *)(&old_para);
	old=(unsigned char *)(&old_para);
	printf("\nluobing ptr1[0]=%d\n",ptr1[0]);
	printf("\nluobing old[0]=%d\n",old[0]);
	
	ptr=(unsigned char *)(para);
	for(i=0;i<sizeof(FOXPARA);i++)
		ptr1[i]=ptr[i];	
		
		ptr1[2]=0x22;
		cur=(unsigned char *)para;
		//old=(unsigned char *)(&old_para);
		printf("cur:");
		for(i=0;i<sizeof(FOXPARA)/2;i++)
			printf("%d,",cur[i]);
		printf("\nold:");
		for(i=0;i<sizeof(FOXPARA)/2;i++)
			printf("%d,",old[i]);
		printf("\nptr-para:");
		for(i=0;i<sizeof(FOXPARA)/2;i++)
			printf("%d,",ptr[i]);
		printf("\nptr1-old_para:");
		for(i=0;i<sizeof(FOXPARA)/2;i++)
			printf("%d,",ptr1[i]);
	
}
void tttt(FOXPARA *para)
{
	unsigned char *old,*ptr1;
	FOXPARA old_para={0xf0f0,0x55};
	
	old=(unsigned char *)(&old_para);
	ptr1=(unsigned char *)(&old_para);
	printf("\nluobing old[0]=%d\n",old[0]);
	printf("\nluobing ptr1[0]=%d\n",ptr1[0]);
}
void	main(void)
{
	FOXPARA para={30,750,660,1,6,5};
	
	tt(&para);
	
	tttt(&para);
	ttt(&para);
}