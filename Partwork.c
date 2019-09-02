#include "DISKDRV.h"
#include "DISKAPP.h"
#include "Partwork.h"
#include "iMath.h"
#include "iMem.h"

void Remove_unused_partn (PARTN *partnArr);
void Create_Partn (PARTN *partnArr, int ndx, unsigned long begin, unsigned long end,
                   int kind, int find_sw);
void Del_Partn (PARTN *partnArr, int ndx);
int Find_EXT (PARTN *partnArr);
void Set_Active_Partn (PARTN *partnArr, PARTN *ptr, int bToggle);  
int IsLikeExtPartn (void *buffer);                
int ChkValidLogicPartn (PARTN *ptr);
void ReloadLogicPartn (PARTN *partnArr);
int Partn_Save_Process (void *pArg,unsigned long mbr);
int Write_Partn (PARTN *partnArr, int doom, int adjust,unsigned long mbr);
int Save_Extended (PARTN *partnArr, unsigned long extStart, int doom, int adjust);
void Set_Partn_Field (unsigned char *table, PARTN *ptr, unsigned long extStart, int adjust);
void Asm_Sect_Cyl (unsigned char *buff, unsigned long sector, unsigned long cyl);
int Fill_Sector (DiskDT *dk, unsigned long NowSect, unsigned long nSect, unsigned char ch);

/*============================ 进 行 分 区 建 立 ============================*/
void Create_Partn (PARTN *partnArr, int ndx, unsigned long begin, unsigned long end,
                   int kind, int find_sw)
{
    unsigned long  partnSize;
    int   i, no;
    PARTN   *ptr;
 
    ptr = partnArr + ndx;
    ptr->stCyl  = begin;
    ptr->endCyl = end;
    ptr->adjust = ptr->remake = TRUE;                  /** 设定分区重建旗号 **/
 
    if ( ptr->no == -1 )
    {
        g_Info.fd.maxParNO++;
        no = g_Info.fd.maxParNO;
        g_Info.fd.logic_exist = TRUE;
    }
    else
    {
        no = ( find_sw == 1 ) ? 1 : 4;
        for ( i = 0 ; i < g_Info.fd.pri_use ; i++ ) /** 找出最先未使用的分区编号 **/
            if ( partnArr[i].no == no )
            {
                no += find_sw;
                i = -1;
            } /* end if */
  
        g_Info.fd.primary++;
    } /* end if */
 
    ptr->no = no;
 
    /* 建立扩展分区 */
    if ( kind == EXTENDED_PARTN )
    {
        ptr->old_id = ptr->id = ( ptr->endCyl > 1023 ) ? 0xf : 5;
  
        ptr->side = 0;                                       /** 调整启始面 **/
        if ( ptr->stCyl == 0 )
            ptr->stCyl = 1;                                /** 调整启始磁柱 **/
        g_Info.fd.ext_head = ptr->stCyl;
        g_Info.fd.ext_end  = ptr->endCyl;
        g_Info.fd.maxParNO = 4;
  
        /*  产生第一个空的逻辑分区  */
        ptr     = partnArr + g_Info.fd.used;
        imemcpy(ptr,partnArr+ndx,sizeof(PARTN));
        /**ptr    = partnArr[ndx];  */                   /** 属性由扩展分区取得 **/
        ptr->no = -1;
        ptr->id = ptr->old_id = 0;
        ptr->adjust = ptr->remake = FALSE;
        g_Info.fd.used++;
  
        ptr = partnArr + ndx;
    }
    else
    {      
        if ( kind == LOGIC_PARTN )
        {
            /** 建立逻辑分区 **/
            ptr->side = 1;
            if ( (g_Info.fd.ext_head == ptr->stCyl) && (no != 5) )
            {
                if ( ptr->stCyl == ptr->endCyl )
                    ptr->endCyl++;
                ptr->stCyl++;                      /** 保留扩展分区第一磁柱 **/
            } /* end if */
        }
        else
        {
            /** 建立主分区 **/
            ptr->side = ( ptr->stCyl == 0 ) ? 1 : 0;
        } /* end if */
  			partnSize=lintshr(11,Get_nSector(ptr));
        /*partnSize = (Get_nSector(ptr) >>11 );*/
        ptr->id   = ( partnSize < 16   ) ? 0x01 :
                    ( partnSize < 32   ) ? 0x04 :
                    ( partnSize > 2000 ) ? 0x0b :
                    ( g_Info.fd.use_fat32 && (partnSize > 511) ) ? 0x0b : 6;
  
        /* 主分区范围有部份落在超过 1023 Cyl 以上的区域 */
        if ( /*(kind == PRIMARY_PARTN) &&*/ (ptr->stCyl > 1023) ||  (ptr->endCyl > 1023) )
            ptr->id = ( ptr->id == 0x0b ) ? 0x0c : 0x0e;
  
        ptr->old_id = ptr->id;
    } /* end if */
 
    Remove_unused_partn(partnArr);
    Find_space(partnArr);                               /** 找寻未配置之区间 **/
} /* end Create_Partn */
/*============================= 删 除 分 区 =================================*/
void Del_Partn (PARTN *partnArr, int ndx)
{      
   int  no, i;
   PARTN  *ptr;
	 unsigned char *ptr_tmp;
	 
   ptr = partnArr + ndx;
   no = ( ptr->no < 5 ) ? 0 : -1;
   
   /*   校 正 最 前 端 分 割 之 启 始 磁 柱    */
   if ( (ptr->stCyl == 1) && (ndx < g_Info.fd.pri_use) ||
        (ptr->stCyl == (g_Info.fd.ext_head + 1)) && (ndx >= g_Info.fd.pri_use) )
   {
      ptr->stCyl--;
   } /* end if */

   if ( ptr->id != 0 )
   {
      if ( no == 0 )
      {
         --g_Info.fd.primary;
        /* if ( Is_Extended(ptr->old_id) )               /** 如果删除扩展分区 **/
         if ( Is_Extended(ptr->id) ) 
         {
            g_Info.fd.maxParNO = 4;
            g_Info.fd.used     = g_Info.fd.pri_use;
            g_Info.fd.ext_head = g_Info.fd.ext_end = -1;
         }
      }
      else
      {
          /*---------------- 后面的编号递减 ---------------*/
         for ( i = g_Info.fd.pri_use ; i < g_Info.fd.used ; i++ )
            if ( partnArr[i].no > ptr->no )
               partnArr[i].no--;
         g_Info.fd.maxParNO--;
      }
   } /* end if */

   if ( g_Info.fd.maxParNO < 5 )
      g_Info.fd.logic_exist = FALSE;                 /** 无任何逻辑分区存在 **/

   /*Init_Partn_Elem(ptr);                            /** 被删除之分区内容重置 **/
   ptr_tmp=(unsigned char *)(ptr);
   for(i=0;i<sizeof(PARTN);i++)
   	ptr_tmp[i]=0;
   ptr->sector = 1;

   /*---------------------------------------------------------+
   |  if partition error and exist second extended partition  |
   +---------------------------------------------------------*/
   if ( g_Info.fd.ext_head == -1 && (i = Find_EXT(partnArr)) > -1 )
   {
      g_Info.fd.ext_head = partnArr[i].stCyl;
      g_Info.fd.ext_end  = partnArr[i].endCyl;
   }

   if ( g_Info.fd.modified <= 0 )
      g_Info.fd.modified = -1;
   /*lbdebug*/ 
   /*LB_FLAG=1;  			/* 打开调试开关*/
		 
   Remove_unused_partn(partnArr);

   Find_Problem(partnArr);
   Find_space(partnArr);                               /** 找寻未配置之区间 **/
} /* end Del_Partn */
/*============================ 设 定 活 动 分 区 ============================*/
void Set_Active_Partn (PARTN *partnArr, PARTN *ptr, int bToggle)
{
    int  i;
 
    if ( ptr->active != 0x80 )
    {
        ptr->active = 0x80;
        for ( i = 0 ; i < g_Info.fd.pri_use ; i++ )
            if ( &partnArr[i] != ptr )
                partnArr[i].active = 0;
    }
    else if ( bToggle )
        ptr->active = 0;
    else
        return;

    if ( g_Info.fd.modified == 0 )
        g_Info.fd.modified = -2;
} /* end Set_Active_Partn */

/*=================== 由分区阵列中移出未配置的分区区域 ======================*/
void Remove_unused_partn (PARTN *partnArr)
{
   PARTN  *ptr, *now;
   int  i;

   ptr = now = partnArr;
   for ( i = 0 ; i < g_Info.fd.pri_use ; i++, ptr++ )
      if ( ptr->id )
      {
      	imemcpy(now,ptr,sizeof(PARTN));
      	++now;
      }
      /*   *now++ = *ptr;*/
   g_Info.fd.pri_use = now - partnArr;

   for ( ; i < g_Info.fd.used ; i++, ptr++ )
      if ( ptr->id )
      {
      	imemcpy(now,ptr,sizeof(PARTN));
      	++now;
      }
      /*   *now++ = *ptr; */
   g_Info.fd.used = now - partnArr;
} /* end Remove_unused_partn */

/*============================= 找 寻 扩 展 分 区 ===========================*/
int Find_EXT (PARTN *partnArr)
{
    int  i;
 
    for ( i = 0 ; i < g_Info.fd.pri_use ; i++ )             /** 找寻扩展分区 **/
        if ( Is_Extended(partnArr[i].old_id) )
            break;
    return( (i < g_Info.fd.pri_use) ? i : -1 );
} /* end Find_EXT */
/*========================= 检查是否存在疑似逻辑分区 ========================*/
int ChkValidLogicPartn (PARTN *ptr)
{
    unsigned char  buff[512];

    if ( linear_ReadWDK(GetStart(ptr), 1, buff) == 0 )
        if ( IsLikeExtPartn(buff) )
            return( TRUE );			/* 如果发现逻辑分区，直接载入 */

    return( FALSE );
} /* end ChkValidLogicPartn */
/*===================== 检查 buffer 内容是否疑似逻辑分区 ====================*/
int IsLikeExtPartn (void *buffer)
{
    unsigned char   *buff = (unsigned char *)buffer;
    int  i;

    if ( *(unsigned int *)&buff[0x1fe] != 0xaa55 )
        return( FALSE );
    if ( buff[0x1be] || buff[0x1ce] )
        return( FALSE );
    for ( i = 0x1de ; i < 0x1fe ; i++ )            /** 检查最后两个栏位 **/
        if ( buff[i] )
            return( FALSE );
    for ( i = 0x1be ; i < 0x1ce ; i += 4 )           /** 检查第一个栏位 **/
        if ( *(unsigned long *)&buff[i] == 0 )
            return( FALSE );
    for ( i = 0x1ce ; i < 0x1de ; i += 4 )           /** 检查第二个栏位 **/
        if ( *(unsigned long *)&buff[i] )
            break;

    /** 如果第二个栏位是空的，或者不是空的但为延伸分区 **/
    return( (i >= 0x1de) || Is_Extended(buff[0x1d2]) );
} /* end IsLikeExtPartn */

/*========================== Reload logic partiiton =========================*/
void ReloadLogicPartn (PARTN *partnArr)
{
    int  ext, err;
 
    if ( g_Info.fd.ext_head != -1 )
    {
        g_Info.fd.maxParNO    = 4;
        g_Info.fd.used        = g_Info.fd.pri_use;
        g_Info.fd.logic_exist = FALSE;
     
        Remove_unused_partn(partnArr);
        ext = Find_EXT(partnArr);                /** 取得扩展分区的阵列索引 **/
        err = Get_Logic_Partn(partnArr, partnArr + ext);
  
       /*if ( g_Info.fd.used == g_Info.fd.pri_use )
            ShowError(29);*/
     

        if ( Find_Problem(partnArr) )          /** 检查分区表有无重迭或错误 **/
            err |= PARTN_OVERLAP;
  
        if ( err & PARTN_OVERLAP_OR_TOO_MUCH )
            g_Info.fd.canSaveFlag = FALSE;
  
        /*if ( err )                                     * 重新读取逻辑分区 *
            Disk_Err_Msg(err);*/
  
        Find_space(partnArr);                          /** 找寻未配置之区间 **/
        g_Info.fd.modified = 1;
    } /* end if */         
} /* end ReloadLogicPartn */

int Partn_Save_Process (void *pArg,unsigned long mbr)
{
	struct SavePara  *para = (struct SavePara *)pArg;	
	return( Write_Partn(para->partnArr, para->doom, para->adjust,mbr) );
} /* end Partn_Save_Process */

int Write_Partn (PARTN *partnArr, int doom, int adjust,unsigned long mbr)
{
	PARTN *ptr;
	unsigned char   buff[512];
	unsigned long  extStart;
	int  err, adr, i, j;
	
	if ( linear_ReadWDK(mbr, 1, buff) )											/* 读取指定分区表 */
  	return(-1);                                           /** 存取错误！**/
  	
  Sort_Partn(partnArr, FOR_NUMBER);               /** partnArr 以编号排序 **/
  err = 0;
	adr = 0x1be;  
	ptr = partnArr;
	if ( g_Info.fd.modified <= -2 )             /** 只是设定 active partition **/
  {
      for ( i = 1 ; i < 5 ; i++,  adr += 0x10 )      /** 设定 ACTIVE 分区 **/
          if ( ptr->no == i )
          {
              buff[adr] = ptr->active;
              ptr++;
          }
          else
              buff[adr] = 0;
  }
  else                                     /** 如果修改程度必须重建分区表 **/
  {
		g_Info.reboot_flag = TRUE;       /** 显示重开机的提示讯息之旗号 **/
    if ( *(unsigned int *)&buff[510] != 0xaa55 )       /** 无效分区表才重建 MBR **/
    {
        *(unsigned int *)&buff[510] = 0xaa55;
    } /* end if */
    
    /*  建 立 主 分 割 表  */
    extStart = 0;
    for ( i = 1 ; i < 5 ; i++,  adr += 0x10 )        /** 造一份主分区表 **/
        if ( ptr->no != i )                                /** 空的分区 **/
        {
            /*buff[i + 0x19f] = 0;               /** 备份隐藏分区 id 位置 **/
            for ( j = 0 ; j < 16 ; j += 4 )
                *(unsigned long *)&buff[adr + j] = 0;
        }
        else
        {                                            /** 填入主分区内容 **/
            Set_Partn_Field(&buff[adr], ptr, 0, adjust);
            /*buff[i + 0x19f] = ptr->old_id;      /** 填入隐藏前的 SYS_ID **/
  
            if ( Is_Extended(ptr->old_id) )          /** 如果是扩展分区 **/
                extStart = *(unsigned long *)&buff[adr + 8];
            /*
            else
                if ( ptr->remake && doom )
                    DestroyBootSect(ptr);
               if ( ptr->remake && doom && DestroyBootSect(ptr) )
                   err = -1;*/
   
            /*if ( GET_WRITE_SW() )*/
                ptr->remake = FALSE;
  
            ptr++;
 
        } /* end if */
  
      /*  建 立 扩 展 分 区  */
      if ( extStart != 0 )
      {
          if ( g_Info.fd.logic_exist )
          {
              if ( Save_Extended(partnArr, extStart, doom, adjust) )
                  err = -1;
          }
          else                                       /** 如果没有逻辑分区 **/
          {
              if ( Fill_Sector(CUR_DK, GetStart(ptr), 1, 0xf6) )
                 err = -1;
          } /* end if */
      } /* end if */
  } /* end if */
 	/*     写入主分区表   */
	if ( linear_WriteWDK(mbr, 1, buff) )
     err = -1;
 
	Sort_Partn(partnArr, FOR_STCYL);                /** partnArr 以磁柱排序 **/
	if ( (err == 0)  )
     g_Info.fd.modified = 0;

	return( err );
} /* end Write_Partn */
/*============================= 储存逻辑分区 ================================*/
int Save_Extended (PARTN *partnArr, unsigned long extStart, int doom, int adjust)
{
    unsigned char   buff[512];
    unsigned long  saveSect;
    PARTN  tmp, *ptr;
    int  i,j;
 
   /* memset(buff, 0, 512);                               /** initial buffers **/
    for(i=0;i<512;i++)
    	buff[i]=0;
    *(unsigned int *)&buff[510] = 0xaa55;
 
    saveSect = extStart;                               /** 扩充分区启始磁区 **/
    ptr = partnArr + g_Info.fd.pri_use;          /** 指向编号最小的逻辑分区 **/
 
    for ( i = 4 ; i < g_Info.fd.maxParNO ; i++,  ptr++ )
    {
        Set_Partn_Field(&buff[0x1be], ptr, saveSect, adjust);   /** 分区资讯 **/
        buff[0x1a0] = ptr->old_id;                     /** 填入备份的 SYS_ID **/
  
    /** if ( doom && ptr->remake && DestroyBootSect(ptr) )
            return(-1);
    **/
       /* if ( doom && ptr->remake )
            DestroyBootSect(ptr);*/
  
        if ( i < (g_Info.fd.maxParNO - 1) )          /** 下个逻辑分区表存在 **/
        {
        		imemcpy(&tmp,ptr+1,sizeof(PARTN));
            /*tmp = *(ptr + 1);*/
            if( tmp.side == 0 )
            {
            	tmp.side = (int)CUR_DK->lgeo.maxHead;
              tmp.stCyl--;
            }
            else
            	tmp.side--;
            /*
            if ( tmp.side < 0 )
            {
                tmp.side = (int)CUR_DK->lgeo.maxHead;
                tmp.stCyl--;
            } /* end if */
   
            tmp.id = tmp.old_id = 5;
            Set_Partn_Field(&buff[0x1ce], &tmp, extStart, 0);  /** 分区资讯 **/
        }
        else
        {
            /*memset(&buff[0x1ce], 0, 0x30);         */        /** 清除之后三栏 **/
            for(j=0;j<0x30;j++)
            	buff[0x1ce+j]=0;
        }
  
        if ( linear_WriteWDK(saveSect, 1, buff) )
            return(-1);
  
        saveSect = GetStart(&tmp);                       /** 下个分区表位置 **/
  
        /*if ( GET_WRITE_SW() )*/
            ptr->remake = FALSE;
    } /* end for */
 
    return(0);
} /* end Save_Extended */
/*========================== 将参数转成分区表资料 ===========================*/
void Set_Partn_Field (unsigned char *table, PARTN *ptr, unsigned long extStart, int adjust)
{
    unsigned long  startCyl, endCyl;
    int i;
 		
    if ( ptr->id == 0 )
    {
    	for(i=0;i<0x10;i++)
    		table[i]=0;
    } /* end if */
 
    table[0] = ptr->active;                                     /* ACTIVE    */
    table[1] = ptr->side;                                       /* 启始面    */
    table[4] = ptr->id;                                         /* System ID */
    table[5] = (unsigned char)CUR_DK->lgeo.maxHead;                      /* 结束面    */
 
    *(unsigned long *)&table[8]  = GetStart(ptr) - extStart;            /* 隐藏磁区  */
    *(unsigned long *)&table[12] = Get_nSector(ptr);             /* 分区容量(磁区数) */
 
    startCyl = ptr->stCyl;
    endCyl   = ptr->endCyl;
 
    if ( ptr->no < 5 )
    {
        if ( startCyl > 1023 )
           startCyl = 1023;
 
        if ( endCyl > 1023 )
           endCyl = 1023;
    } /* end if */
                           
    Asm_Sect_Cyl(&table[2], ptr->sector, startCyl);                /** 启始 **/
    Asm_Sect_Cyl(&table[6], CUR_DK->lgeo.maxSect, endCyl);         /** 结束 **/
 
 		/*
    if ( adjust && ptr->adjust )                  /** 调整 DOS 启动磁区内容 **//*
    {
        Adjust_DOS_BootSector(ptr /*, extStart*//*);     /** 位于 partnadj.c **//*
        ptr->adjust = FALSE;
    } /* end if */
} /* end Set_Partn_Field */

/*======================== 组合磁区、磁柱为 2 unsigned char ==========================*/
void Asm_Sect_Cyl (unsigned char *buff, unsigned long sector, unsigned long cyl)
{
    /** 结合磁轨的第 8,9 bit **/
    buff[0] = (unsigned char)(sector & 0x3f) | (lintshr(2,cyl) & 0xc0);
/*    buff[0] = (unsigned char)(sector & 0x3f) | ((cyl >> 2) & 0xc0);*/
 
    /** 磁轨剩下的 0 - 7 bit **/   
    buff[1] = (unsigned char)cyl;
} /* end Asm_Sect_Cyl */

/*=========================== 将某字元填入连续磁区 ==========================*/
int Fill_Sector (DiskDT *dk, unsigned long NowSect, unsigned long nSect, unsigned char ch)
{
    unsigned char buff[512] ;
    unsigned long  i;
    int j;
 
    /*if ( buff[0] != ch )    */                       /** CommBuff must static **/
    	/*memset(buff, ch, 512);*/
    	for(j=0;j<512;j++)
    		buff[j]=ch;
        
        
  
    for ( i = 0 ; i < nSect ; i++,  NowSect++ )
        if ( dk->drv->linearWrite(dk, NowSect, 1, buff) )
            return(-1);
 
    return(0);
} /* end Fill_Sector */