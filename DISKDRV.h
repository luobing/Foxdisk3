/*磁盘访问驱动*/
#ifndef __DISKDRV_H_
#define __DISKDRV_H_


#define FAR                  far

#define VALID_DISKDT        0x01
#define SUPPORT_13EXT       0x02
#define FLOPPY_DISK         0x04
#define HARD_DISK           0x08
#define SCSI_DISK           0x10

#define MAKE_FP(seg,ofs)    ((void far *)(((unsigned long)(seg) << 16) | (unsigned int)(ofs)))
#define FP_SEGM(fp)         ((unsigned int)((unsigned long)((void FAR *)(fp)) >> 16))
#define FP_OFFS(fp)         ((unsigned int)(fp))

#ifndef NULL
#define NULL 0
#endif

typedef struct Disk_Descriptor  DiskDT;
/*    Disk Driver Structure Declaration    */
typedef struct Disk_Driver
{
   signed int   (*open)(DiskDT *dk);
   signed int   (*close)(DiskDT *dk);
   signed int   (*reset)(DiskDT *dk);
   signed int   (*getAttr)(DiskDT *dk);
   /*------------------------------------------------------------*/
   signed int   (*linearRead)(DiskDT *dk,
                      unsigned long linearSect, unsigned long nSects, void *buff); 
   signed int   (*linearWrite)(DiskDT *dk,
                       unsigned long linearSect, unsigned long nSects, void *buff); 
   signed int   (*linearCheck)(DiskDT *dk,                         
                       unsigned long linearSect, unsigned long nSects, void *buff);
   /*------------------------------------------------------------*/
   signed int   (*chsRead)(DiskDT *dk,
                      unsigned long head, unsigned long cyl, unsigned long sector,
                      unsigned long nSects, void *buff);
   signed int   (*chsWrite)(DiskDT *dk,
                       unsigned long head, unsigned long cyl, unsigned long sector,
                       unsigned long nSects, void *buff);
   signed int   (*chsCheck)(DiskDT *dk,
                       unsigned long head, unsigned long cyl, unsigned long sector, 
                       unsigned long nSects, void *buff);
}DiskDRV;

typedef struct Disk_Geometry
{
   unsigned int    bytePerSect;
   unsigned long   sectPerCyl;
   unsigned long   maxCyl;
   unsigned long   maxHead;
   unsigned long   maxSect;
   /*double  tnSector;                      /** (tnSects_H << 32) + tnSects_L **/
   unsigned long tnSector;
   unsigned long tnSector1;
} DiskGEO;


/*    磁盘参数表里的每个元素皆为下面的结构  */ 
struct Disk_Descriptor
{        
   unsigned int     flag;                /** 记录内容是否有效、是否支援 INT13 ext ...**/
   DiskGEO  lgeo;
   DiskGEO  pgeo;
   DiskDRV  *drv;                  			 /** Disk Driver **/                        

   unsigned int     diskNO;             /**  0x80, 0x81... **/
};

typedef struct DRV_ADR_Packet		/* 使 用 INT 13h extension 存 取 硬 碟 专 用 结 构  */
{ 
   unsigned char    size,
          					reserved1,
         						transfer,
          					reserved2;
   unsigned int   buff_OFF,
          buff_SEG;
   unsigned long  startLow,
          startHigh;
} DRV_PKT;


typedef struct FLD							/* 使 用 INT 13h extension 存 放 硬 碟 参 数 用 结 构  */
{
    unsigned int  dma_boundary_err:1,
          valid4_15:1,
          isRemovable:1,
          SupportsW_V:1,
          notification:1,
          lockable:1,
          dvl_Maximum:1,
          reserved:9;
} WORD_FLD;

typedef struct Result_Buffer		/* 使 用 INT 13h extension 传 回 硬 碟 参 数 用 结 构  */
{
   unsigned int       size;
   WORD_FLD   flag;
   unsigned long      maxCyl,
              maxHead,
              maxSector,
              total_Low,              /** (High << 32) + Low = Total Sector **/
              total_High;
   unsigned int       bytePerSect;
   unsigned long      dpt_ptr;
} DRV_PARA;



/*------ 磁盘驱动访问通用程序(硬件抽象) -------*/
extern  signed int LBA_Access (unsigned int cmd, DiskDT *dk,
                          unsigned long linearSect, unsigned long nSects, void *buff);
extern  signed int CHS_Access (unsigned int cmd, DiskDT *dk, unsigned long head,
                          unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff);
extern  signed int int13 (unsigned int cmd, DiskDT *dk, unsigned long head,
                     unsigned long cyl,  unsigned long sector, unsigned long nSects, void FAR *buffer);
extern signed int INT_13H (unsigned char cmd, unsigned char disk, unsigned int head,
                unsigned int cyl, unsigned int sect, unsigned char nSects, void FAR *buff);
extern  signed int int13ext (unsigned int cmd, DiskDT *dk,  unsigned long head, 
                        unsigned long cyl, unsigned long sector, unsigned long nSects, void FAR *buffer);
extern void Sect_to_CHS (unsigned long linearSect, unsigned long *c, unsigned long *h, unsigned long *s, DiskDT *dk);
extern unsigned long CHS_to_Sect (unsigned long c, unsigned long h, unsigned long s, DiskDT *dk);
extern signed int Reset_Disk (signed int disk);
 /*------ 磁盘驱动访问程序 -------*/
extern  signed int drv_DkOpen (DiskDT *dk);
extern  signed int drv_DkClose (DiskDT *dk);
extern  signed int drv_DkReset (DiskDT *dk);
extern  signed int drv_DkGetAttr (DiskDT *dk);
 
extern  signed int drv_LBA_DkRead (DiskDT *dk,
                              unsigned long linearSect, unsigned long nSects, void *buff); 
extern  signed int drv_LBA_DkWrite (DiskDT *dk,
                              unsigned long linearSect, unsigned long nSects, void *buff); 
extern  signed int drv_LBA_DkCheck (DiskDT *dk,
                               unsigned long linearSect, unsigned long nSects, void *buff);
extern  signed int drv_CHS_DkRead (DiskDT *dk, unsigned long head,
                              unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff);
extern  signed int drv_CHS_DkWrite (DiskDT *dk, unsigned long head,
                               unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff);
extern  signed int drv_CHS_DkCheck (DiskDT *dk, unsigned long head,\
                              unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff);

#endif

