/*luobing 2008-10-2 23:05*/
/*为Foxdisk2所写的通用磁盘访问程序*/
#include "DISKDRV.h"
#include "iMath.h"

#define RETRY_TIMES 3                      /*访问重复次数*/

/*------ 磁盘驱动访问通用程序(硬件抽象) -------*/
signed int LBA_Access (unsigned int cmd, DiskDT *dk,
                  unsigned long linearSect, unsigned long nSects, void *buff);
signed int CHS_Access (unsigned int cmd, DiskDT *dk, unsigned long head,
                  unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff);
signed int int13 (unsigned int cmd, DiskDT *dk, unsigned long head,
             unsigned long cyl,  unsigned long sector, unsigned long nSects, void FAR *buffer);
int INT_13H (unsigned char cmd, unsigned char disk, unsigned int head,
        unsigned int cyl, unsigned int sect, unsigned char nSects, void FAR *buff);
signed int int13ext (unsigned int cmd, DiskDT *dk,  unsigned long head, 
                       unsigned long cyl, unsigned long sector, unsigned long nSects, void FAR *buffer);
void Sect_to_CHS (unsigned long linearSect, unsigned long *c, unsigned long *h, unsigned long *s, DiskDT *dk);
unsigned long CHS_to_Sect (unsigned long c, unsigned long h, unsigned long s, DiskDT *dk);
signed int Reset_Disk (signed int disk);
/*------ 磁盘驱动访问程序 -------*/
signed int drv_DkOpen (DiskDT *dk);
signed int drv_DkClose (DiskDT *dk);
signed int drv_DkReset (DiskDT *dk);
signed int drv_DkGetAttr (DiskDT *dk);

signed int drv_LBA_DkRead (DiskDT *dk,
                      unsigned long linearSect, unsigned long nSects, void *buff); 
signed int drv_LBA_DkWrite (DiskDT *dk,
                       unsigned long linearSect, unsigned long nSects, void *buff); 
signed int drv_LBA_DkCheck (DiskDT *dk,
                       unsigned long linearSect, unsigned long nSects, void *buff);
signed int drv_CHS_DkRead (DiskDT *dk, unsigned long head,
                      unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff);
signed int drv_CHS_DkWrite (DiskDT *dk, unsigned long head,
                       unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff);
signed int drv_CHS_DkCheck (DiskDT *dk, unsigned long head,\
                              unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff);


/*------------------------------ 取得磁碟参数 -------------------------------*/
signed int drv_DkGetAttr (DiskDT *dk)
{
    unsigned long    maxSect, maxHead, maxCyl;
    DRV_PARA drvPara;
		unsigned char flags;
		unsigned int b_cx,b_bx;
		unsigned char b_ch,b_dh;
		
    dk->flag &= ~(SUPPORT_13EXT | VALID_DISKDT);             /** clear flag **/
    dk->drv->reset(dk);                                      /** reset disk **/

		asm mov ah,8
		b_ch=(unsigned char)(dk->diskNO);
		asm mov dl,b_ch
		asm int 0x13
		asm	push ax
		asm lahf
		asm mov flags,ah
		asm pop ax		
    if(flags&0x01)                                        /** 侦测失败 **/
    {
        dk->drv->reset(dk);
        return(-1);
    } /* end if */

    /*-------------------------+
    |       取得磁盘参数       |
    +-------------------------*/
    asm mov b_dh,dh												 
    asm mov b_cx,cx  
    asm mov b_ch,ch 
    maxHead= (unsigned long)(b_dh);                    
    maxSect = (unsigned long)(b_cx & 0x3f);                    /** get sector_per_track **/
    maxCyl  = (unsigned long)(((b_cx & 0xc0) << 2) | b_ch);  /** get total_cyl - 1 **/
		
#if 1
    if ( (dk->flag & FLOPPY_DISK) == 0 )                   /** if hard_disk **/
    {
        /*---------------------------+
        |  检查是否支持 INT 13h ext  |
        +---------------------------*/
        asm mov ah,0x41
        b_ch=(unsigned char)(dk->diskNO);
		    asm mov dl,b_ch
        asm mov bx,0x55aa
        asm int 0x13
        asm	push ax
				asm lahf
		    asm mov flags,ah
		    asm pop ax		
        asm mov b_bx,bx
        asm mov b_cx,cx

        if ( ((flags&0x01) == 0) &&                    /** 支持 INT 13h ext **/
             (b_bx == 0xaa55) && (b_cx & 1) )
        {
            if ( int13ext(8, dk, 0, 0, 0, 0, &drvPara) == 0 )
            {
                dk->flag |= SUPPORT_13EXT;                     /** 设定支持 **/
                dk->lgeo.bytePerSect=drvPara.bytePerSect;
                dk->pgeo.bytePerSect=drvPara.bytePerSect;
              }
            else
            {
                dk->drv->reset(dk);
                return(-1);
            }
            
        } /* end if */
    } /* end if */
#endif
   /*-------------------------+
    |    将磁盘参数填入结构    |
    +-------------------------*/
    dk->lgeo.maxCyl     = maxCyl;
    dk->lgeo.maxHead    = maxHead;
    dk->lgeo.maxSect    = maxSect;
    /*dk->lgeo.sectPerCyl = (maxHead + 1) * maxSect;*/
    dk->lgeo.sectPerCyl =lintmul(maxHead + 1,maxSect);

    if ( dk->lgeo.sectPerCyl <= 0 )
        return(-1);
        
    /*------------------------------------------+
    |    取得磁盘容量(磁区数)及校正最大磁柱数   |
    +------------------------------------------*/
    if ( (dk->flag & SUPPORT_13EXT) == 0 )
    {
        dk->pgeo.maxCyl     = dk->lgeo.maxCyl;
        dk->pgeo.maxHead    = dk->lgeo.maxHead;
        dk->pgeo.maxSect    = dk->lgeo.maxSect;
        dk->pgeo.sectPerCyl = dk->lgeo.sectPerCyl;

        /*
         *  计算总磁区数 
         */
        /*dk->lgeo.tnSector = (double)(maxCyl + 1) * dk->lgeo.sectPerCyl;*/
        dk-> lgeo.tnSector = lintmul(maxCyl + 1,dk->lgeo.sectPerCyl);
        dk-> lgeo.tnSector1=0x0;			/* 只能支持到2T的硬盘 */
    }
    else
    {
        if ( drvPara.flag.valid4_15 )
        {
            dk->pgeo.maxCyl     = drvPara.maxCyl;
            dk->pgeo.maxHead    = drvPara.maxHead;
            dk->pgeo.maxSect    = drvPara.maxSector;
            dk->pgeo.sectPerCyl = lintmul((drvPara.maxHead + 1), drvPara.maxSector);
            /*dk->pgeo.sectPerCyl = (drvPara.maxHead + 1) * drvPara.maxSector;*/
        } /* end if */

        /*
         *  Total_Sector = Total_Low_unsigned long + Total_High_unsigned long * (2^32);
         *  (2^32) = 2 to the power of 32 = (double)(unsigned long)0xffffffffL + 1;
         */
        dk->lgeo.tnSector = (unsigned long )drvPara.total_Low ;
        dk->lgeo.tnSector1= (unsigned long )drvPara.total_High;
                            
  
        /*
         *  校正 total_cyl - 1
         */
       /* dk->lgeo.maxCyl = (unsigned long)( dk->lgeo.tnSector / dk->lgeo.sectPerCyl ) - 1;*/
        
        dk->lgeo.maxCyl = lintdiv(dk->lgeo.sectPerCyl,(unsigned long)(dk->lgeo.tnSector))-1;

    } /* end if */

    if ( (dk->lgeo.maxCyl <= 0) || (dk->lgeo.tnSector < 1) )
        return(-1);

    dk->pgeo.tnSector = dk->lgeo.tnSector;
    dk->flag |= VALID_DISKDT;
    return(0);
} /* end drv_DkGetAttr */


/*---------------------------------------------------------------------------*/
signed int drv_DkOpen (DiskDT *dk)
{
    return( ( dk == NULL ) ? -1 : 0 );
} /* end drv_DkOpen */


/*---------------------------------------------------------------------------*/
signed int drv_DkClose (DiskDT *dk)
{
    return( ( dk == NULL ) ? -1 : 0 );
} /* end drv_DkClose */


/*---------------------------------------------------------------------------*/
signed int drv_DkReset (DiskDT *dk)
{
    return( Reset_Disk(dk->diskNO) );
} /* end drv_DkReset */
   

/*---------------------------------------------------------------------------*/
signed int drv_LBA_DkRead (DiskDT *dk, unsigned long linearSect, unsigned long nSects, void *buff)
{
    return( LBA_Access(2, dk, linearSect, nSects, buff) );
} /* end drv_LBA_DkRead */


/*---------------------------------------------------------------------------*/
signed int drv_LBA_DkWrite (DiskDT *dk, unsigned long linearSect, unsigned long nSects, void *buff)
{
    unsigned long  sector, head, cyl;
 
    Sect_to_CHS(linearSect, &cyl, &head, &sector, dk);          /** get CHS **/
 
    return( CHS_Access(3, dk, head, cyl, sector, nSects, buff) );

} /* end drv_LBA_DkWrite */


/*---------------------------------------------------------------------------*/
signed int drv_LBA_DkCheck (DiskDT *dk, unsigned long linearSect, unsigned long nSects, void *buff)
{
    return( LBA_Access(4, dk, linearSect, nSects, buff) );
} /* end drv_LBA_DkCheck */


/*---------------------------------------------------------------------------*/
signed int drv_CHS_DkRead (DiskDT *dk, unsigned long head, unsigned long cyl,
                                  unsigned long sector, unsigned long nSects, void *buff)
{
    return( CHS_Access(2, dk, head, cyl, sector, nSects, buff) );
} /* end drv_CHS_DkRead */


/*---------------------------------------------------------------------------*/
signed int drv_CHS_DkWrite (DiskDT *dk, unsigned long head, unsigned long cyl,
                                   unsigned long sector, unsigned long nSects, void *buff)
{                   
        return( CHS_Access(3, dk, head, cyl, sector, nSects, buff) );
 
} /* end drv_CHS_DkWrite */


/*---------------------------------------------------------------------------*/
signed int drv_CHS_DkCheck (DiskDT *dk, unsigned long head, unsigned long cyl,
                                   unsigned long sector, unsigned long nSects, void *buff)
{
    return( CHS_Access(4, dk, head, cyl, sector, nSects, buff) );
} /* end drv_CHS_DkCheck */


/*===========================  以 LBA 方式访问硬盘 ===========================*/
signed int LBA_Access (unsigned int cmd, DiskDT *dk,
                            unsigned long linearSect, unsigned long nSects, void *buff)
{
    unsigned long  sector, head, cyl;
 
    Sect_to_CHS(linearSect, &cyl, &head, &sector, dk);
    return( CHS_Access(cmd, dk, head, cyl, sector, nSects, buff) );
} /* end LBA_Access */


/*=========================== 以 CHS 方式访问硬盘 ===========================*/
signed int CHS_Access (unsigned int cmd, DiskDT *dk, unsigned long head,
                            unsigned long cyl, unsigned long sector, unsigned long nSects, void *buff)
{
    signed int  err, i;
 
    for ( i = 0 ; i < RETRY_TIMES ; i++ )
    {
        if ( dk->flag & SUPPORT_13EXT )
            err = int13ext(cmd, dk, head, cyl, sector, nSects, buff);
        else
            err = int13(cmd, dk, head, cyl, sector, nSects, buff);
  
        if ( err )
            Reset_Disk(dk->diskNO);                                /** 重置 **/
        else
            break;
    } /* end for */
 
    return( err );
} /* end CHS_Access */

 
/*-------------------------- Conventional INT 13h ---------------------------*/
signed int int13 (unsigned int cmd, DiskDT *dk, unsigned long head,
             unsigned long cyl, unsigned long sector, unsigned long nSects, void FAR *buffer)
{
    cmd &= ~0x40;
    return( INT_13H(cmd, dk->diskNO, (unsigned int)head, (unsigned int)cyl, (unsigned int)sector,
                                                 (unsigned char)nSects, buffer) );
} /* end int13 */

/*---------------------------------- INT 13h ---------------------------------+
|                     此 程 序 使 用 INT 13h 存 取 磁 区 资 料                |
+-----------------------------------------------------------------------------+
| 输入值：                                                                    |
|           cmd: 子命令,  _Disk: 磁碟,     _Head: 磁头,    _Cyl: 磁柱,        |
|           _Sect: 磁区,  nSects: 磁区数,  buff: buffer                       |
+-----------------------------------------------------------------------------+
|  * must set _ES before call this                                            |
+----------------------------------------------------------------------------*/
signed int INT_13H (unsigned char cmd, unsigned char disk, unsigned int head,
               unsigned int cyl, unsigned int sect, unsigned char nSects, void FAR *buff)
{
    /*
     * access local variable after popa instruction,
     * the variable must declaration to volatile type
     */
    volatile unsigned int  bakAX, bakCX, bakDX;
    unsigned int rx;
    unsigned char rl;
    
    asm pusha
    asm push ES
    rx=(unsigned int)FP_SEGM(buff);
    asm mov ax,rx
    asm mov ES,ax
    rx=((cyl << 8) + ((cyl >> 2) & 0x00C0)) | (sect & 0x3f);
    asm mov cx,rx
    rx=(unsigned int)buff;
    asm mov bx,rx
    rl=(unsigned char)head;
    asm mov dh,rl
    asm mov dl,disk
    asm mov al,nSects
    asm mov ah,cmd
    asm int 0x13
    asm mov bakAX,ax
    asm mov bakCX,cx
 		asm mov bakDX,dx
    asm pop ES
    asm popa
    
    bakAX = (bakAX >> 8) & 0xff;
    if ( (bakAX == 0) && (cmd == 8) )
    {
        *( unsigned int FAR *)buff    = bakCX;
        *((unsigned int FAR *)buff+1) = bakDX;
    } /* end if */


    return( bakAX );
} /* end INT_13H */

/*---------------------------- INT 13h extension -----------------------------+
|                                                                             |
|   The function just support Fn 42h, 43h, 44h, 48h, can't use other fn !!!   |
+----------------------------------------------------------------------------*/
signed int int13ext (unsigned int cmd, DiskDT *dk, unsigned long head, 
                unsigned long cyl, unsigned long sector, unsigned long nSects, void FAR *buffer)
{
    DRV_PKT para;
 		unsigned char retval;
 		unsigned int rx1,rx2;
 		unsigned char rl,rl1;
 		
 		cmd |= 0x40;
 		
    if ( cmd == 0x48 )
    {
    		
        signed int  i;
  
        *(unsigned char FAR *)buffer = 30;            /** 为48h准备的驱动器参数的缓冲区30字节 **/
        for ( i = 1 ; i < 30 ; i++ )         					/** added for some BIOS **/
            *((unsigned char FAR *)buffer + i) = 0;        
        rx1=FP_OFFS(buffer);
        rx2=FP_SEGM(buffer);
        rl=(unsigned char)(dk->diskNO) ;
        asm pusha
        asm push ds
        asm mov ax,rx2
        asm mov ds,ax																		/*ds fill*/
        asm mov ax,0
        asm mov ah,0x48
        asm mov dl,rl																		/*dl fill*/
        asm mov si,rx1																	/*si fill*/
        asm int 0x13
        asm mov retval,ah
        asm pop ds
        asm popa       
    }
    else                                                   /** 取得磁盘参数 **/
    {
        para.size      = 16;
        para.reserved1 =  0;
        para.reserved2 =  0;
        /*
         * (StartHigh) high unsigned int no use, FIX ME
         */
        para.startLow  = CHS_to_Sect(cyl, head, sector, dk);
        para.startHigh = 0;
  

        para.transfer = (unsigned char)nSects;
        para.buff_OFF = (unsigned int)FP_OFFS(buffer);
        para.buff_SEG = (unsigned int)FP_SEGM(buffer);

        /** DS:SI -> &para **/
        rx1=(unsigned int)(FP_OFFS(&para));
        rx2=(unsigned int)FP_SEGM(&para);
        rl=(unsigned char)(dk->diskNO) ;
        rl1=(unsigned char)(cmd);
        asm push ds
        asm pusha
        asm mov ax,rx2
        asm mov ds,ax 									/*ds fill*/
        asm mov si,rx1									/*si fill*/
        asm mov dl,rl										/*dl fill*/
        asm mov ax,0
        asm mov ah,rl1									/*ah fill*/
        asm int 0x13
        asm mov retval,ah
        asm popa
        asm pop ds                 
    } /* end if */
 
    return( (signed int)(retval) );    
} /* end int13ext */
/*==================== 将LBA线性磁区转成 Ｃ／Ｈ／Ｓ 定址 =======================*/
void Sect_to_CHS (unsigned long linearSect, unsigned long *c, unsigned long *h, unsigned long *s, DiskDT *dk)
{
	
		#if 0
    if(linearSect==0xc8d00)
    {
    	if( c )printf("c ok! ");
    	if ( h )printf("h ok! ");
    	if ( s )printf("s ok! ");
    	printf("\n");
    	printf("lgeo->spc=%08lx,dk->lgeo.mS=%08lx\n",dk->lgeo.sectPerCyl,dk->lgeo.maxSect);
    }
    #endif
    
    if ( c )
    		*c  = lintdiv(dk->lgeo.sectPerCyl,linearSect);
        /**c = linearSect / dk->lgeo.sectPerCyl;*/
    if ( h )
    		*h = lintdiv(dk->lgeo.maxSect,(lintmod(dk->lgeo.sectPerCyl,linearSect)));
        /**h = (linearSect % dk->lgeo.sectPerCyl) / dk->lgeo.maxSect;*/
    if ( s )
    		*s = lintmod(dk->lgeo.maxSect,linearSect)+1;
       /* *s = (linearSect % dk->lgeo.maxSect) + 1;*/
} /* end Sect_to_CHS */


/*==================== 将 Ｃ／Ｈ／Ｓ 定址转成线性磁区 =======================*/
unsigned long CHS_to_Sect (unsigned long c, unsigned long h, unsigned long s, DiskDT *dk)
{
		 
		return (lintmul(lintmul(c,dk->lgeo.maxHead+1)+h,dk->lgeo.maxSect) +s -1 );
    /*return( (c * (dk->lgeo.maxHead+1) + h) * dk->lgeo.maxSect + s - 1 );*/
} /* end CHS_to_Sect */

/*----------------------------- Reset a disk --------------------------------*/
signed int Reset_Disk (signed int disk)
{
		unsigned char retval;
		unsigned int rx;
		asm mov ax,0
		rx=(unsigned int)disk & 0x00ff;
		asm mov dx,rx
		asm int 0x13
    asm mov retval,ah
    return( (signed int)(unsigned int)retval );
} /* end Reset_Disk */