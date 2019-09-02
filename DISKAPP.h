#ifndef __DISKAPP_H_
#define __DISKAPP_H_

#define  MAX_PARTN_ELEM     24                      		   /** 可接受的分割总数量 **/

#define CUT_ERR_PARTN         0x01                         /** 切除错误分区 **/ 
#define INV_PARTN_TAB         0x02                         /** 无效分区表 **/ 
#define PARTN_NUM_TOO_MUCH    0x04                         /** 分区数超出最大数 **/ 
#define INV_DISK_PARA         0x08                         /** 硬体错误 **/ 
#define PARTN_ACCESS_ERR      0x10                         /** 分区表存取失败 **/ 
#define PARTN_OVERLAP         0x20                         /** 分区发生重迭 **/ 
#define GIVE_UP_CHG_WD        0x40                         /** 放弃切换硬工作碟 **/
#define DISK_ERROR            0x80                         /** 磁碟错误且只有一部硬碟 **/
#define PARTN_OVERLAP_OR_TOO_MUCH \
            (PARTN_OVERLAP | PARTN_NUM_TOO_MUCH)
/* check system ID condition */
#define EXTENDED_ID           0x01
#define FAT12_16              0x02
#define LBA_FAT               0x04
#define NT_ID                 0x08
#define HIDD_ID               0x80
#define FAT12_16_32           (FAT12_16 | LBA_FAT)
#define NT_FAT12_16_32        (FAT12_16_32 | NT_ID)
#define HIDDEN_ID	0x26

#define  FOR_STCYL          0
#define  FOR_NUMBER         1

#define CUR_DK        (g_DTable)                         			/** 目前的工作碟 **/
#define FALSE                 0
#define TRUE                  1
            
/*磁盘应用层抽象：为进行fdisk和format准备相关的API函数*/
typedef struct partition
{
   unsigned char  active,                                                /* 活动分区  */ 
         					problem;                                                               
   signed int    	no,                                                    /* 分区编号  */ 
         					id,                                                    /* 系统 ID   */ 
         					old_id,                                                /* 隐藏前 ID */ 
     
         					remake,                                                /* 重建旗号  */ 
         					adjust;                                                                
   unsigned long	stCyl,                                                 /* 启始磁柱  */ 
         					endCyl,                                                /* 结束磁柱  */ 
         					side,                                                  /* 启始面    */ 
         					sector;                                                /* 启始磁区  */ 
} PARTN;

typedef struct common_information
{               
    unsigned int      nFP,                                         /** 软碟机总数 **/
              nHD;                                         /** 硬碟机总数 **/
    DiskDT    *wdk;                                /** 指向工作碟的参数表 **/
    signed int     reboot_flag;           /** 结束时是否出现提示重新开机之讯息 **/
    signed int     swap_status;                         /** 是否有磁碟置换行为 **/
    signed int     expertMode;                         /** 使用模式(专家,一般) **/
    signed int     usingMenu;                                                     
    signed int     prevMenu;                                                      
                                                             
struct fdisk_info                                  /** FDisk 程式专用 **/
    {          
        unsigned long  ext_head,			/*扩展分区起始,如不存在则为-1*/
               ext_end;								/*扩展分区结束,如不存在则为-1*/
        signed int  logic_exist,			/*逻辑分区个数,0为不存在*/
               maxParNO,							/*最大分区编号,逻辑分区从'5'开始计*/
               used,									/* 分区表的数组用掉了几个 */
               pri_use,								/*(最大为4)mbr中分区可用,存放于分区数组中,如剩余空间是主分区中的,no=0,否则no=-1*/
               primary,								/*主分区个数，包括扩展分区*/
               modified,
               use_fat32,
               canSaveFlag,
               clr_screen;
    } fd;

} INFO;

struct SavePara													/* 保存分区的时候需要用到的数据结构 */
{
    PARTN  *partnArr;
    int  adjust, doom;
};
/*extern unsigned int LB_FLAG;				/* lbdebug */

extern DiskDT    g_DTable[1];        /** 指向执行时配置的磁碟参数表(阵列) **/
  														       /*由于针对的是单硬盘，所以此参数表其实包含了所需要的信息*/
extern INFO      g_Info;             /** 综合系统资讯结构，图形显示及fdisk所用 **/		
extern DiskDRV  g_diskDrv;
/*====================函数声明====================================*/
extern signed int CreateDiskParaTable (void);
extern signed int Get_Disk_Info (DiskDT *dk, PARTN *partnArr, unsigned long mbr_addr,signed int find_flag);
extern void Init_Fdisk (void);
extern unsigned long Get_nSector (PARTN *ptr);
extern signed int Is_Extended (unsigned char id);
extern void Find_space (PARTN *partnArr);
extern signed int Find_Problem (PARTN *partnArr);
extern signed int IsHiddenWinID (signed int id);
extern signed int IsHiddenID (signed int id);
extern void Ins_elem (PARTN *partnArr, signed int ndx, unsigned long begin, unsigned long end);
extern signed int linear_ReadWDK (unsigned long linearSect, unsigned long nSects, void *buff);
extern signed int linear_WriteWDK (unsigned long linearSect, unsigned long nSects, void *buff);
extern signed int Get_Logic_Partn (PARTN *partnArr, PARTN *extPartn);
extern unsigned long GetStart (PARTN *ptr);
extern signed int chs_CheckWDK (unsigned long head, unsigned long cyl, unsigned long sect, unsigned long nSects, void *buff);
extern void Sort_Partn (PARTN *partnArr, signed int kind);
#endif
