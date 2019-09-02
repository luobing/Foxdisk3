/*luobing 2008-10-3 11:41 diskapp.c*/
/*为Foxdisk2所写的通用磁盘访问程序: 应用层*/
#include "DISKDRV.h"
#include "DISKAPP.h"
#include "Global.h"

/* 调试用的全局变量 */
/*
unsigned int LB_FLAG=0;
unsigned int LB_FLAG1=0;*/



signed int CreateDiskParaTable (void);
signed int Get_Disk_Info (DiskDT *dk, PARTN *partnArr, unsigned long mbr_addr,signed int find_flag);
void Init_Fdisk (void);
static signed int Partn_Info (PARTN *partnArr, unsigned char *buff);
static void Get_One_Partn (unsigned long begin, PARTN *ptr, unsigned char *buff, signed int field, DiskDT *dk);
signed int Get_Logic_Partn (PARTN *partnArr, PARTN *extPartn);
signed int Find_Problem (PARTN *partnArr);
static signed int Find_Overlap (PARTN *ptr, PARTN *end);
signed int IsHiddenPartn (PARTN *ptr);
signed int IsValid (DiskDT *dk);
signed int Is_Extended (unsigned char id);
signed int IsHiddenWinID (signed int id);
signed int IsHiddenID (signed int id);
signed int ChkSysID (unsigned char id, signed int cond);
signed int linear_WriteWDK (unsigned long linearSect, unsigned long nSects, void *buff);
signed int linear_ReadWDK (unsigned long linearSect, unsigned long nSects, void *buff);
signed int chs_WriteWDK (unsigned long head, unsigned long cyl, unsigned long sect, unsigned long nSects, void *buff);
signed int chs_ReadWDK (unsigned long head, unsigned long cyl, unsigned long sect, unsigned long nSects, void *buff);
signed int chs_CheckWDK (unsigned long head, unsigned long cyl, unsigned long sect, unsigned long nSects, void *buff);
unsigned long GetStart (PARTN *ptr);
unsigned long GetEnd (PARTN *ptr);
unsigned long Get_nSector (PARTN *ptr);
static signed int ForStartCyl (const void *elem1, const void *elem2);
static signed int ForNO (const void *elem1, const void *elem2);
void Sort_Partn (PARTN *partnArr, signed int kind);
void Select_Sort (void *base, signed int elem_num, signed int elem_size,
                  signed int (*fcmp)(const void *elem1, const void *elem2));
static void Swap_Element (void *elem1, void *elem2, signed int elem_size);
void Find_space (PARTN *partnArr);
signed int Out_of_ext (PARTN *ptr);
void Ins_elem (PARTN *partnArr, signed int ndx, unsigned long begin, unsigned long end);
void CopyMem (void *target, const void *source, unsigned int size);

/*
void lbdebug(PARTN *partnArr)
{
	if(LB_FLAG==1)
 		{
 			 int i;
 			 LB_FLAG=0;
 			 asm xor ax,ax
			 asm mov ax,3
			 asm int 0x10
			 for(i=0;i<4;i++)
				{
				printf("partnArr[%d]: ",i);
				printf(" active=%02x,no=%04x,id=%04x,old_id=%04x,",partnArr[i].active,partnArr[i].no,partnArr[i].id,partnArr[i].old_id);
				printf("remake=%04x,adjust=%04x\n",partnArr[i].remake,partnArr[i].adjust);
				printf("              stCyl=%.10ld,endCyl=%.10ld\n",partnArr[i].stCyl,partnArr[i].endCyl);
				}
			 	printf("g_info:\n");
				printf(" ext_head:%ld,ext_end:%ld,logic_exist:%04x,maxParNO:%04x,used:%04x\n",g_Info.fd.ext_head,
			           g_Info.fd.ext_end,g_Info.fd.logic_exist,g_Info.fd.maxParNO,g_Info.fd.used);
			printf(" pri_use:%x,primary:%x,modified:%x,use_fat32:%x,canSaveFlag%x,clr_screen:%x\n",
					g_Info.fd.pri_use,g_Info.fd.primary,g_Info.fd.modified,g_Info.fd.use_fat32,g_Info.fd.canSaveFlag,g_Info.fd.clr_screen);
			printf("CUR_DK:lgeo,bytePerS:%d,sectPerCyl:%ld,maxCyl:%ld,maxHead:%ld,maxSect:%ld,tnSector:%ld\n",CUR_DK->lgeo.bytePerSect,\
			          CUR_DK->lgeo.sectPerCyl,CUR_DK->lgeo.maxCyl,CUR_DK->lgeo.maxHead,CUR_DK->lgeo.maxSect,(unsigned long)(CUR_DK->lgeo.tnSector));
			printf("CUR_DK:pgeo,bytePerS:%d,sectPerCyl:%ld,maxCyl:%ld,maxHead:%ld,maxSect:%ld,tnSector:%ld\n",CUR_DK->pgeo.bytePerSect,\
			          CUR_DK->pgeo.sectPerCyl,CUR_DK->pgeo.maxCyl,CUR_DK->pgeo.maxHead,CUR_DK->pgeo.maxSect,(unsigned long)(CUR_DK->pgeo.tnSector));
			 asm xor ax,ax
			 asm int 0x16
			 //exit(0);
 		}
}*/
/*----------------------- Create Disk Parameter table ------------------------+*/  
signed int CreateDiskParaTable (void)
{
    signed int   no;
    DiskDT  *dk;
   /*  本程序直接在硬盘上运行，肯定有硬盘存在直接挂载驱动程序即可 */
    no = 0x80;
    dk = g_DTable;																/*指向全局磁盘参数表*/
    dk->flag = HARD_DISK;
    dk->diskNO = no;
		dk->drv    = &g_diskDrv;                      /** 决定使用的 driver **/
  
		dk->drv->open(dk);                             /** 开启这个磁碟设备 **/  
		dk->drv->getAttr(dk);                    /** 取得磁碟几何及其它参数 **/  
		/* foxdisk 此处为Foxdisk预留10个磁道 foxdisk */
		dk->pgeo.maxCyl-=10;
		dk->lgeo.maxCyl-=10;
		
		dk->drv->close(dk);                            /** 关闭这个磁碟设备 **/  
 
    return(0);
} /* end CreateDiskParaTable */

/*=============== 读入磁盘及分区信息到 Partn 数组及 info 数组 ================+
|   传回值：                                                                  |
|                                                                             |
|       CUT_ERR_PARTN       分区表内容发现错误，切除其余分区                  |
|       INV_PARTN_TAB       无效的分区表                                      |
|       PARTN_NUM_TOO_MUCH  超出可接受的分区数量                              |
|       INV_DISK_PARA       中断 13h 工作失败，可能是硬体错误                 |
|       PARTN_ACCESS_ERR    硬盘存取错误                                      |
|       PARTN_OVERLAP       发现分区重迭                                      |
+============================================================================*/
signed int Get_Disk_Info (DiskDT *dk, PARTN *partnArr, unsigned long mbr_addr,signed int find_flag)
{
    unsigned char   buff[512];
    signed int  err;
    unsigned int i;
    unsigned char *ptr;
 
    Init_Fdisk();                                            /** 重置 fdisk info **/
 
    if ( !IsValid(dk) )
        return( INV_DISK_PARA );                             /** 无效参数表 **/
 		
 		ptr=(unsigned char *)(partnArr);
 		for(i=0;i<(sizeof(PARTN) * MAX_PARTN_ELEM);i++)					 /** 分区表阵列初始 **/
 			ptr[i]=0x0;
    
    dk->drv    = &g_diskDrv;                      					 /** 决定使用的 driver **/
 		if(dk->drv->linearRead(g_DTable,mbr_addr,1,buff))
    	*(unsigned int *)&buff[510] = 0;                       /** 主分区表读取失败！**/
    
    
    err = Partn_Info(partnArr, buff);                        /** 析出分区表资讯 **/
 
 
    if ( Find_Problem(partnArr) )                            /** 检查分区表有无重迭或错误 **/
        err |= PARTN_OVERLAP;
 
    if ( find_flag && (err & ~PARTN_OVERLAP_OR_TOO_MUCH) == 0 )
        if ( err & PARTN_OVERLAP_OR_TOO_MUCH )
            g_Info.fd.canSaveFlag = FALSE;
 
    Find_space(partnArr);                              /** 找寻未配置之区间 **/
    return( err ); 
} /* end Get_Disk_Info */
/*============================= Reset FDisk =================================*/
void Init_Fdisk (void)
{                      
    g_Info.fd.ext_head    = -1;
    g_Info.fd.ext_end     = -1;
    g_Info.fd.modified      =  0;
    g_Info.fd.maxParNO    =  0;  
    g_Info.fd.used        =  0;       
    g_Info.fd.pri_use     =  0;    
    g_Info.fd.primary     =  0;
 /* g_Info.fd.use_fat32   =  0; */
    g_Info.fd.logic_exist = FALSE;
    g_Info.fd.canSaveFlag = TRUE;
    g_Info.fd.clr_screen  = TRUE;
} /* end Init_Fdisk */
/*======================== 析 出 分 区 表 信 息 ==============================+
|   传回值：                                                                  |
|                                                                             |
|         CUT_ERR_PARTN       分区表内容发现错误，切除其余分区                |
|         INV_PARTN_TAB       无效的分区表                                    |
|         PARTN_NUM_TOO_MUCH  超出可接受的分区数量                            |
|         PARTN_ACCESS_ERR    硬盘存取错误                                    |
|         PARTN_OVERLAP       发现危险分区存在                                |
+============================================================================*/
static signed int Partn_Info (PARTN *partnArr, unsigned char *buff)
{        
    signed int  adr, err, ndx, ext, i;
    unsigned long  sector;
    PARTN  *ptr;
 
    adr = 0x1be;
    err = 0;
    ndx = 0;
    ptr = partnArr;
    ext = -1;
 		
    if ( *((unsigned int *)(&buff[510])) != 0xaa55 )
        err |= INV_PARTN_TAB;                                /** 无效分区表 **/
    else
    {
        for ( i = 0 ; i < 4 ; i++,  adr += 0x10 )            /** 读取主分区 **/
            if ( buff[adr+4] != 0 )
            {
                if ( (ext == -1) && ( Is_Extended(buff[adr+4]) ))
                {
                    ext = ndx;
                 } /* end if */
    						
    						
                sector = *((unsigned long *)(&(buff[adr+8])));
    						
                Get_One_Partn(sector, ptr, buff, i + 1, CUR_DK);
                
                g_Info.fd.maxParNO = 4;
                g_Info.fd.primary++;
                ptr->no = i + 1;                               /** 分区编号 **/
    
                ndx++;
                ptr++;
            } /* end if */
  
        g_Info.fd.pri_use = g_Info.fd.primary;
        g_Info.fd.used    = g_Info.fd.primary;
  
          
        if ( ext > -1 )
        {
            g_Info.fd.ext_head = partnArr[ext].stCyl;   /** 记录 ext 分区区域 **/
            g_Info.fd.ext_end  = partnArr[ext].endCyl;
            err = Get_Logic_Partn(partnArr, partnArr + ext); /** 读取逻辑分区 **/
        } /* end if */
    } /* end if */
 
    /*----------------------------------------+
    |   将 分 区 内 容 以 起 始 磁 柱 排 序   |
    +----------------------------------------*/
    Sort_Partn(partnArr, FOR_STCYL);               /** 分区阵列改以磁柱排序 **/
    g_Info.fd.modified = 0;
 
    return( err );
} /* end Partn_Info */

/*========================== 析 出 各 栏 资 讯 ==============================*/
static void Get_One_Partn (unsigned long begin, PARTN *ptr, unsigned char *buff, signed int field, DiskDT *dk)
{
    unsigned char   *bak_id = buff + field + 0x19f;
    unsigned char   *elem   = buff + field * 0x10 + 0x1ae;
    unsigned long  end;

    ptr->adjust =
    ptr->remake = 0;
    ptr->stCyl  =
    ptr->endCyl = 0;
    ptr->active = elem[0];
    ptr->id     = elem[4];

    if ( ptr->id != 0 )
    {
        if ( !IsHiddenPartn(ptr) )
            ptr->old_id = ptr->id;
        else if ( IsHiddenWinID(ptr->id) )
            ptr->old_id = ptr->id & 0x0f;
        else if ( *bak_id != 0 )
            ptr->old_id = *bak_id;
        else
            ptr->old_id = HIDDEN_ID;

        end = begin + *(unsigned long *)&elem[12] - 1;               /** 求结束 CHS **/
        Sect_to_CHS(end, &ptr->endCyl, &ptr->side, &ptr->sector, dk);
                                                            				 /** 求启始 CHS **/
        Sect_to_CHS(begin, &ptr->stCyl, &ptr->side, &ptr->sector, dk);
    }
    else
        ptr->old_id = 0;
} /* end Get_One_Partn */
/*======================== 取 得 各 逻 辑 区 段 ==============================+
|    传回值：                                                                 |
|                                                                             |
|        CUT_ERR_PARTN        分区表内容发现错误，切除其余分区                |
|        PARTN_NUM_TOO_MUCH   超出可接受的分区数量                            |
|        PARTN_ACCESS_ERR     硬盘存取错误                                    |
+============================================================================*/
signed int Get_Logic_Partn (PARTN *partnArr, PARTN *extPartn)
{                         
    unsigned long  extStart, sector;
    signed int  err, ndx, no;
    unsigned char   buff[512];
    PARTN  *ptr;
 
    extStart = GetStart(extPartn);                          /** 取得启始磁区 **/
    sector   = extStart;
 
    /*============ 读入第一个逻辑分区表 ============*/
    if ( linear_ReadWDK(sector, 1, buff) )
        return( PARTN_ACCESS_ERR );
 
    /*============ 第一层逻辑分区不存在 ============*/
    if ( (*(unsigned long *)&buff[0x1be] == (unsigned long)0xf6f6f6f6L) || (buff[0x1c2] == 0) )
        return(0);
 
    no  = 5;
    err = 0;
    ndx = g_Info.fd.pri_use;
    ptr = partnArr + ndx;
    while ( 1 )
    {
        if ( ndx > (MAX_PARTN_ELEM - 1) )
        {
            err |= PARTN_NUM_TOO_MUCH;             /** 超出可接受的分区数量 **/
            break;
        } /* end if */
  
        if ( *(unsigned int *)&buff[510] != 0xaa55 )           /** 无效的逻辑分区表 **/
        {
            err |= CUT_ERR_PARTN;
            break;
        } /* end if */
  
        sector += *(unsigned long *)&buff[0x1c6];                /** 加上隐藏磁区数 **/
        Get_One_Partn(sector, ptr, buff, 1, CUR_DK);
        ptr->no = no;                                          /** 分区编号 **/
        no++;
        ndx++;
        ptr++;
  
        /*---------------------------------------------------+
        |        逻辑分区表第二栏一定是扩充分区或空的        |
        +---------------------------------------------------*/
        if ( (*(unsigned long *)&buff[0x1d2] == 0) || !Is_Extended(buff[0x1d2]) )
            break;
  
        /*---------------------------------------------------+
        |            载 入 下 一 个 逻 辑 分 区 表           |
        +---------------------------------------------------*/
        sector = extStart + *(unsigned long *)&buff[0x1d6];
        if ( linear_ReadWDK(sector, 1, buff) )
        {
            err |= PARTN_ACCESS_ERR;
            break;
        } /* end if */
    } /* end while */
 
    if ( ndx > g_Info.fd.used )
    {
        g_Info.fd.logic_exist = TRUE;
        g_Info.fd.maxParNO    = no - 1;
        g_Info.fd.used        = ndx;
    } /* end if */
     
    return( err );
} /* end Get_Logic_Partn */
/*============================== 判 定 分 区 是 否 重 叠 ==============================*/
signed int Find_Problem (PARTN *partnArr)
{
    PARTN  *cmp, *ext, *end;
    signed int  errFlag = FALSE;
 
    end = partnArr + g_Info.fd.used;
    for ( cmp = partnArr ; cmp < end ; cmp++ )
        cmp->problem = FALSE;
 
    ext = NULL;
    end = partnArr + g_Info.fd.pri_use;
    for ( cmp = partnArr ; cmp < end ; cmp++ )         /** 找寻主分区区重迭 **/
        if ( cmp->id )
        {
            if ( Is_Extended(cmp->old_id) )
            {
                if ( ext )
                {
                    errFlag = TRUE;
                    cmp->problem = TRUE;
                }
                else
                    ext = cmp;
            } /* end if */
  
            if ( (cmp->stCyl > cmp->endCyl) || Find_Overlap(cmp, end) )
            {
                cmp->problem = TRUE;
                errFlag = TRUE;                                /** 发现重迭 **/
            } /* end if */
        } /* end if */
 
    if ( ext )
    {
        end = partnArr + g_Info.fd.used;
        for ( ; cmp < end ; cmp++ )                  /** 找寻逻辑分区区重迭 **/
            if ( cmp->stCyl > cmp->endCyl || Out_of_ext(cmp) ||
                 Find_Overlap(cmp, end) || Is_Extended(cmp->old_id) )
            {
                cmp->problem = TRUE;
                errFlag = TRUE;                                /** 发现重迭 **/
            } /* end if */
    } /* end if */
 
    g_Info.fd.canSaveFlag = ( errFlag ) ? FALSE : TRUE;
 
    return( errFlag );
} /* end Find_Problem */


/*========================== 检 查 是 否 重 迭 ==============================*/
static signed int Find_Overlap (PARTN *ptr, PARTN *end)
{
    PARTN  *cmp = ptr++;

/*  if ( cmp->endCyl > CUR_DK->lgeo.maxCyl )
        return( TRUE );
*/
    for ( ; ptr < end ; ptr++ )
        if ( cmp->endCyl >= ptr->stCyl )
            return( TRUE );                                    /** 发现重迭 **/
    return( FALSE );
} /* end Find_Overlap */

/*======================= 是 否 为 被 隐 藏 的 分 区 ========================*/
signed int IsHiddenPartn (PARTN *ptr)
{
    return( IsHiddenID(ptr->id) );
} /* end IsHiddenPartn */

/*==================== 检 验 磁 碟 参 数 子 是 否 有 效 =====================*/
signed int IsValid (DiskDT *dk)
{
    return( dk && (dk->flag & VALID_DISKDT) );
}  /* end IsValid */
/*--------------------------- Is Extended sysID -----------------------------*/
signed int Is_Extended (unsigned char id)
{
    return( ChkSysID(id, EXTENDED_ID) );
} /* end Is_Extended */
/*---------------------- 是 否 为 DOS 隐 藏 的 系 统 ID ---------------------*/
signed int IsHiddenWinID (signed int id)
{
    return( ChkSysID(id, HIDD_ID | NT_FAT12_16_32) );
} /* end IsHiddenWinID */


/*----------------------- 是 否 为 隐 藏 的 系 统 ID ------------------------*/
/* added for dos hidden ID by SPF, 2002.05.01 */
signed int IsHiddenID (signed int id)
{
    return( ChkSysID(id, HIDD_ID) || IsHiddenWinID(id) );
} /* end IsHiddenID */

/*------------ 是 否 为 DOS、 OS/2、 NT 或 DOS 隐藏的系统ＩＤ ----------------+
| 参数：                                                                      |
|     id      System ID                                                       |
|     cond    FAT12_16 / NT_ID / LBA_FAT / EXTENDED_ID                        |
+-----------------------------------------------------------------------------+
|     如果要检查 hidden id， cond 可与 HIDD_ID 做 or 运算再传入               |
+----------------------------------------------------------------------------*/
signed int ChkSysID (unsigned char id, signed int cond)
{
    signed int  ret = 0;

    if ( cond & HIDD_ID )
    {
        if ( (cond == HIDD_ID) && (id == 0x26) )
            ret = HIDD_ID;
        else if ( (cond & FAT12_16) && ((id == 0x11) || (id == 0x14) || (id == 0x16)) )
            ret = HIDD_ID | FAT12_16;
        else if ( (cond & LBA_FAT) && ((id == 0x1b) || (id == 0x1c) || (id == 0x1e)) )
            ret = HIDD_ID | LBA_FAT;
        else if ( (cond & NT_ID) && (id == 0x17) )
            ret = HIDD_ID | NT_ID;
    }
    else
    {
        if ( (cond & EXTENDED_ID) && ((id == 0x05) || (id == 0x0f) || (id == 0x85)) )
            ret = EXTENDED_ID;
        else if ( (cond & FAT12_16) && ((id == 0x01) || (id == 0x04) || (id == 0x06)) )
            ret = FAT12_16;
        else if ( (cond & NT_ID) && (id == 0x07) )
            ret = NT_ID;
        else if ( (cond & LBA_FAT) && ((id == 0x0b) || (id == 0x0c) || (id == 0x0e)) )
            ret = LBA_FAT;
    } /* end if */

    return( ret );
} /* end ChkSysID */

/*=========================== LBA 地址写入工作硬盘 ============================*/
signed int linear_WriteWDK (unsigned long linearSect, unsigned long nSects, void *buff)
{
    return( CUR_DK->drv->linearWrite(CUR_DK, linearSect, nSects, buff) );
} /* end linear_WriteWDK */


/*=========================== LBA 地址读取工作硬盘 ============================*/
signed int linear_ReadWDK (unsigned long linearSect, unsigned long nSects, void *buff)
{
    return( CUR_DK->drv->linearRead(CUR_DK, linearSect, nSects, buff) );
} /* end linear_ReadWDK */


/*=========================== CHS 地址写入工作硬盘 ============================*/
signed int chs_WriteWDK (unsigned long head, unsigned long cyl, unsigned long sect, unsigned long nSects, void *buff)
                              
{
    return( CUR_DK->drv->chsWrite(CUR_DK, head, cyl, sect, nSects, buff) );
} /* end ChsWriteWDK */


/*=========================== CHS 地址读取工作硬盘 ============================*/
signed int chs_ReadWDK (unsigned long head, unsigned long cyl, unsigned long sect, unsigned long nSects, void *buff)
{      
    return( CUR_DK->drv->chsRead(CUR_DK, head, cyl, sect, nSects, buff) );
} /* end ChsReadeWDK */


/*=========================== CHS 地址检验工作硬盘 ============================*/
signed int chs_CheckWDK (unsigned long head, unsigned long cyl, unsigned long sect, unsigned long nSects, void *buff)
{
    return( CUR_DK->drv->chsCheck(CUR_DK, head, cyl, sect, nSects, buff) );
} /* end chs_CheckWDK */

/*========================== 换 算 起 始 磁 区 ==============================*/
unsigned long GetStart (PARTN *ptr)
{
    return( CHS_to_Sect(ptr->stCyl, ptr->side, ptr->sector, CUR_DK) );
} /* end GetStart */


/*========================== 换 算 结 束 磁 区 ==============================*/
unsigned long GetEnd (PARTN *ptr)
{
    return( CHS_to_Sect(ptr->endCyl, CUR_DK->lgeo.maxHead,
                        CUR_DK->lgeo.maxSect, CUR_DK) );
} /* end GetEnd */


/*=================== 以 起 始 磁 柱 排 序 的 比 较 函 数 ===================*/
static signed int ForStartCyl (const void *elem1, const void *elem2)
{
    const PARTN *e1 = (const PARTN *)elem1;
    const PARTN *e2 = (const PARTN *)elem2;
 
    return( (e1->stCyl < e2->stCyl) ? -1 :
            (e1->stCyl > e2->stCyl) ?  1 : 0 );
} /* end fcmp */


/*=================== 以 分 区 编 号 排 序 的 比 较 函 数 ===================*/
static signed int ForNO (const void *elem1, const void *elem2)
{
    const PARTN *e1 = (const PARTN *)elem1;
    const PARTN *e2 = (const PARTN *)elem2;
 
    return( (e1->id == 0) ? 1 :
            (e2->id == 0) ? 0 : e1->no - e2->no );
} /* end ForNO */


/*============================== 分区阵列排序 ===============================*/
void Sort_Partn (PARTN *partnArr, signed int kind)
{
    signed int (*fcmp)(const void *elem1, const void *elem2);
 
    fcmp = ( kind == FOR_STCYL ) ? ForStartCyl : ForNO;
 
    /*---------- sort primary partition ----------*/
    Select_Sort(partnArr, g_Info.fd.pri_use, sizeof(PARTN), fcmp);
 
    /*---------- sort logic partition ----------*/
    Select_Sort(partnArr + g_Info.fd.pri_use, 
                g_Info.fd.used - g_Info.fd.pri_use, sizeof(PARTN), fcmp);
} /* end Sort_Partn */

/*=============================== Selection sort ===========================*/
void Select_Sort (void *base, signed int elem_num, signed int elem_size,
                  signed int (*fcmp)(const void *elem1, const void *elem2))
{
    unsigned char   *curr, *walk, *min;
    signed int  i, j;

    curr = (unsigned char *)base;
    for ( i = 0 ; i < elem_num - 1 ; i++ )
    {
        min  = curr;
        walk = curr + elem_size;

        /*----------------- find minmum element -----------------*/
        for ( j = i + 1 ; j < elem_num ; j++ )
        {
            if ( (*fcmp)(min, walk) > 0 )
                min = walk;
            walk += elem_size;
        } /* end for */

        /*---------- swap element ----------*/
        if ( min != curr )
            Swap_Element(min, curr, elem_size);
             
        curr += elem_size;
    } /* end for */
} /* end Select_Sort */
/*============================== Swap two element ===========================*/
static void Swap_Element (void *elem1, void *elem2, signed int elem_size)
{
    unsigned char   *e1 = (unsigned char *)elem1;
    unsigned char   *e2 = (unsigned char *)elem2;
    unsigned char   tmp;

    while ( elem_size-- > 0 )
    {
        tmp   = *e1;
        *e1++ = *e2;
        *e2++ = tmp;
    } /* end while */
} /* end Swap_Element */

/**/
/*============================= 找 寻 空 间 =================================*/
/*       已配置之主分区：  id < 4        未配置之主分区：  id == 0           */
/*       已配置之逻辑分区： id > 4       未配置之逻辑分区： id == -1         */
/*       必须以启始磁柱为键值排序后，才可使用本函式，否则所得结果将不正确    */
/*===========================================================================*/
void Find_space (PARTN *partnArr)
{       
    PARTN  *ptr, *pre;
    signed int  i;
 		
    if ( g_Info.fd.used >= MAX_PARTN_ELEM )			/*分区表数组用完了(一般不可能出现这种情况)*/
        return;
 
    if ( g_Info.fd.primary == 0 )								/*没有分区,全部硬盘都可用*/
        Ins_elem(partnArr, 0, 0, CUR_DK->lgeo.maxCyl);
    else
    {
    		/*1 第一个主分区(不可能为扩展分区)之前是否有剩余空间*/
        ptr = partnArr;
        if ( ptr->stCyl > 0 && (!Is_Extended(ptr->id) || ptr->stCyl > 1) )
            Ins_elem(partnArr, 0, 0, ptr->stCyl - 1);
         
  			/*2 其余分区之间是否有剩余空间*/
        pre = ptr++;
        for ( i = 1 ; i < g_Info.fd.pri_use ; i++,  ptr++,  pre++ )
            if ( ptr->stCyl > (pre->endCyl + 1) )
                Ins_elem(partnArr, i, pre->endCyl + 1, ptr->stCyl - 1);
         
               
  			/*3 最后一个分区之后是否有扩展分区*/
        if ( pre->endCyl < CUR_DK->lgeo.maxCyl )			/* 调试1T硬盘的时候发现问题出在此处 */
        {
        		/*LB_FLAG1=1;*/
            Ins_elem(partnArr, i, pre->endCyl + 1, CUR_DK->lgeo.maxCyl); 
         }
            
  			/*4 对扩展分区进行处理*/
        i = g_Info.fd.pri_use;
        if ( g_Info.fd.ext_head != -1 )
        {
            if ( g_Info.fd.logic_exist == FALSE )
                Ins_elem(partnArr, i, g_Info.fd.ext_head, g_Info.fd.ext_end);
            else
            {
                ptr = partnArr + i;
                if ( !Out_of_ext(ptr) && (ptr->stCyl > (g_Info.fd.ext_head + 1)) )
                    Ins_elem(partnArr, i, g_Info.fd.ext_head, ptr->stCyl - 1);

                pre = ptr++;
                for ( ++i ; i < g_Info.fd.used ; i++,  ptr++,  pre++ )
                    if ( !Out_of_ext(ptr) && (ptr->stCyl > (pre->endCyl + 1)) )
                        Ins_elem(partnArr, i, pre->endCyl + 1, ptr->stCyl - 1);
    
                if ( !Out_of_ext(pre) && pre->endCyl < g_Info.fd.ext_end )
                    Ins_elem(partnArr, i, pre->endCyl + 1, g_Info.fd.ext_end);
            } /* end if */
        } /* end if */
    } /* end if */
    		
} /* end Find_space */
/*==================== 检 查 分 区 是 否 超 出 extended 范 围 ===============*/
signed int Out_of_ext (PARTN *ptr)
{
    return ( g_Info.fd.ext_head == -1 ||
             ptr->stCyl < g_Info.fd.ext_head || ptr->endCyl > g_Info.fd.ext_end ||
             ptr->stCyl > g_Info.fd.ext_end || ptr->endCyl < g_Info.fd.ext_head );
} /* end Out_of_ext */


/*============================ 设 定 分 区 元 素 ============================*/
void Ins_elem (PARTN *partnArr, signed int ndx, unsigned long begin, unsigned long end)
{ 
		unsigned int i;
		unsigned char *ptr_tmp;
    PARTN *ptr = partnArr + ndx;							/*指向需要处理的分区*/
 
    if ( g_Info.fd.used >= MAX_PARTN_ELEM )
       return;  
    CopyMem(&partnArr[ndx + 1], &partnArr[ndx],
            sizeof(PARTN) * (MAX_PARTN_ELEM - 1 - ndx) );	/*将内容后移一个数组元素*/
    /*        
 		if(LB_FLAG1==1)			/*lbdebug*//*
 		{
 			if(LB_FLAG==1)
 			{
 				lbdebug(partnArr);
 				printf("ndx=%d,begin=%ld,end=%ld",ndx,begin,end);
 				exit(0);
 			}
 		}*/
    /*Init_Partn_Elem(ptr);  */                           /** 重置 ptr 所指内容 **/
    ptr_tmp=(unsigned char *)(ptr);
    for(i=0;i<sizeof(PARTN);i++)
    	ptr_tmp[i]=0;
     ptr->sector = 1;
     
    ptr->stCyl  = begin;
    ptr->endCyl = end;
    g_Info.fd.used++;
 
    if ( begin >= g_Info.fd.ext_head && end <= g_Info.fd.ext_end )
       ptr->no = -1;                                       /** 扩充分区区域 **/
    else
    {
        ptr->no = 0;                                         /** 主分区区域 **/
        g_Info.fd.pri_use++;
    } /* end if */
} /* end Ins_elem */
/*======================== 区块拷贝(会自动判断是否重迭) =====================*/
void CopyMem (void *target, const void *source, unsigned int size)
{
    unsigned char  *sptr = (unsigned char *)source;
    unsigned char  *tptr = (unsigned char *)target;
 
    if ( sptr != tptr )
    {
        if ( (tptr > sptr) && (tptr < (sptr + size)) )         /** 空间重迭 **/
        {
            sptr += size;
            tptr += size;
            while ( --sptr >= (unsigned char *)source )                 /** 反向拷贝 **/
                *(--tptr) = *sptr;
        }
        else
        {
            for ( ; size > 0 ; size-- )
               *tptr++ = *sptr++;
        } /* end if */
    } /* end if */
} /* end CopyMem */

unsigned long Get_nSector (PARTN *ptr)
{
   unsigned long begin = GetStart(ptr);
   unsigned long end   = GetEnd(ptr);

   return( ((end > begin) ? end - begin : begin - end) + 1 );
} /* end compute_sects */