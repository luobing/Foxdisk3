#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"
#include "Common.h"
#include "Painter.h"
#include "DISKDRV.h"
#include "DISKAPP.h"
#include "Fdisk.h"
#include "MainMenu.h"
#include "Partwork.h"
#include "Global.h"
#include "iMath.h"

extern unsigned char *sysFun_str[];

void Fdisk(unsigned int x,unsigned int y,unsigned long mbr_addr);
int partnFun(unsigned int x,unsigned int y,PARTN *partnArr,int ndx);
void CreateNewPartn (unsigned int x,unsigned y,PARTN *partnArr, int ndx,int kind);
void DeletePartn(unsigned int x,unsigned y,PARTN *partnArr, int ndx);
void SetPartID(unsigned int x,unsigned y,PARTN *partnArr, int ndx);
void showERROR(unsigned int x,unsigned y,unsigned char *s);
void ChkPartSurface(unsigned int x,unsigned y,PARTN *partnArr, int ndx);
int SavePartn(unsigned int x,unsigned y,PARTN *partnArr, unsigned long mbr_addr);

void Fdisk(unsigned int x,unsigned int y,unsigned long mbr_addr)
{
	PARTN partnArr[MAX_PARTN_ELEM];				/* 存储当前系统的分区数组 */
	PARTN *ptr;
	int old_sel=1,cur_sel=1,err=0,lines=0,minline=1,maxline=9,logic_line=-1,hitkey;
	int ndx=0,i;
	unsigned char sys_file[12];		
	unsigned long cap,color;
	int refresh_flag=-1;			/* 刷新标志 */
	int exit_flag=-1;
	
	iMessage(GROUND1);										/* 清除显示 */
	
	/* 获取当前硬盘的分区 */
	CreateDiskParaTable();
	err=Get_Disk_Info(CUR_DK,partnArr,mbr_addr,TRUE);
		/* lbdebug 为调试1000G硬盘时候添加*/
		/*{
			
			partnArr[0].active=0x80;
			partnArr[0].no=1;
			partnArr[0].old_id=0xc;
			partnArr[0].id=0xc;
			partnArr[0].stCyl=0;
			partnArr[0].endCyl=2047;
			partnArr[1].active=0x0;
			partnArr[1].no=1;
			partnArr[1].old_id=0x7;
			partnArr[1].id=0x7;
			partnArr[1].stCyl=2048;
			partnArr[1].endCyl=121600;
			
			g_Info.fd.ext_head=-1;
			g_Info.fd.ext_end=-1;
			g_Info.fd.logic_exist=0;
			g_Info.fd.maxParNO		=	4;			
  	  g_Info.fd.used			  = 2; 
  	  g_Info.fd.pri_use		  = 2;
  	  g_Info.fd.primary			=	2;				
  	  g_Info.fd.modified    = 0;
  	  g_Info.fd.use_fat32   = 0;
  	  g_Info.fd.canSaveFlag = 1;
  	  g_Info.fd.clr_screen  = 1;
  	  
  	  CUR_DK->lgeo.bytePerSect=512;
  	  CUR_DK->lgeo.sectPerCyl=16065;
  	  CUR_DK->lgeo.maxCyl=121600;
  	  CUR_DK->lgeo.maxHead=254;
  	  CUR_DK->lgeo.maxSect=63;
  	  CUR_DK->lgeo.tnSector=1953525168;   
  	  
  	  CUR_DK->pgeo.bytePerSect=512;
  	  CUR_DK->pgeo.sectPerCyl=0;
  	  CUR_DK->pgeo.maxCyl=0;
  	  CUR_DK->pgeo.maxHead=0;
  	  CUR_DK->pgeo.maxSect=0;
  	  CUR_DK->pgeo.tnSector=1953525168;    
		} */
		
	if(err)
	{	
		iWindowOut(x,y,sysFun_str[0],4);
		
		setCur(x+8+8*4,y+40);
		if(err&PARTN_OVERLAP)
			c_printf(WHITE,fdisk_str[0]);
		else if((err&CUT_ERR_PARTN)||(err&INV_PARTN_TAB)||(err&PARTN_NUM_TOO_MUCH))
			c_printf(WHITE,fdisk_str[1]);
		else
			c_printf(WHITE,fdisk_str[2]);
			
		setCur(x+8+8*20,y+40+24*2);
		c_printf(WHITE,fdisk_str[3]);
		/* lbdebug */
		/*setCur(0,20);
		c_printf(WHITE,"err=%x",err);*/
		
		asm xor ax,ax
		asm int 0x16
	}
	else
	{
		do{
				/* 清除前一行的选中显示 */
				if(refresh_flag==3)		/* 删除了分区，必须重新初始化*/
				{
					cur_sel=1;
					old_sel=1;
					minline=1;
					maxline=9;
					ndx=0;
				}
				if(refresh_flag)
				{
					if(g_Info.fd.ext_head==-1)	
					{		
						lines=g_Info.fd.used+1;									/* 总共需要显示的行数 */
						logic_line=-1;
					}
					else lines=g_Info.fd.used+2;
					if(lines<10)
						iWindowOut(x,y,sysFun_str[0],10);
					else
						iWindowOut(x,y,sysFun_str[0],(unsigned int)(lines));	
					setCur(x+8,y+40);
					c_printf(SURFACE1,fdisk_str[5]);
        	
					rectblock(x+8,y+40+24,x+8+(66<<3),y+40+24*9+16,GROUND1);
					if(logic_line==-1)
						ndx=minline-1;
					else if(minline>=logic_line)
						ndx=minline-1-1;
					else
						ndx=minline-1;
							
					ptr=partnArr+ndx;
					if(g_Info.fd.ext_head!= -1)
    			  	logic_line=g_Info.fd.pri_use+1;
					for(i=minline; ((i<=maxline)&&(ndx<g_Info.fd.used)); i++,ndx++,ptr++)
					{
					
						/*lbdebug*/
						#if 0
						{
							asm xor ax,ax
							asm int 0x16
							/*lbdebug 	*/
      				setCur(0,0);
      				rectblock(0,0,80*8,16,RED);
      				c_printf(WHITE,"old_sel=%d,cur_sel=%d,minline=%d,maxline=%d,ndx=%d,",old_sel,cur_sel,minline,maxline,ndx);
							c_printf(WHITE,"logic_line=%d,i=%d",logic_line,i);
						}
						#endif
						setCur(x+8,y+40+24*(i-minline+1));
    		  	if(i==logic_line)
    		  		++i;
    		  	if(i<=maxline)
    		  	{	
    		  		/*cap=(Get_nSector(ptr)>>11);				          /*以M bytes为单位*/
    		  		cap=lintshr(11,Get_nSector(ptr));
    		  		if((partnArr[ndx].no==-1) || (partnArr[ndx].no==0))
    		  	{
    		  		setCur(x+8,y+40+24*(i-minline+1));				          /* 空两个字符对齐 */
    		  		color=LIGHTGRAY;
    		  		c_printf(color,fdisk_str[10],fdisk_str[11]);
    		  	}
    		  	else
    		  	{
    		  		setCur(x+8+8*2,y+40+24*(i-minline+1));				          /* 空两个字符对齐 */
    		  		color=WHITE;
    		  		c_printf(color,fdisk_str[6],partnArr[ndx].no);
    		  	}
    		  	setCur(x+8+8*8,y+40+24*(i-minline+1));				          /* "分区号 " */
    		  	if(partnArr[ndx].active==0x80)
    		  		c_printf(RED,fdisk_str[7],0x03);	        /* 显示活动分区 */
    		  	setCur(x+8+8*14,y+40+24*(i-minline+1));						      /*"分区号 活动  "*/
    		  	c_printf(color,fdisk_str[8],partnArr[ndx].stCyl);	/*显示起始分区*/
    		  	setCur(x+8+8*24,y+40+24*(i-minline+1));									/*"分区号 活动  起始磁道  "*/
    		  	c_printf(color,fdisk_str[8],partnArr[ndx].endCyl);	/*显示结束分区*/
    		  	setCur(x+8+8*34,y+40+24*(i-minline+1));		              /*"分区号 活动  起始磁道  结束磁道  "*/
    		  	c_printf(color,fdisk_str[8],cap);						/* 分区大小 */
    		  	setCur(x+8+8*46,y+40+24*(i-minline+1));									/*分区号 活动  起始磁道  结束磁道  容量(M)  */
    		  	c_printf(color,fdisk_str[9],partnArr[ndx].id);	/* 分区类型 */
    		  	setCur(x+8+8*51,y+40+24*(i-minline+1));	                /*"分区号 活动  起始磁道  结束磁道  容量(M)  分区类型 "*/
    		  	c_printf(color,fdisk_str[10],g_SysName[partnArr[ndx].id]);	/*描述*/
    		  }
				}
				/* 显示完毕 ,logic_line为显示逻辑分区分界字符串的行,不用显示*/
				refresh_flag=0;
			}
			chgLine(x,y,old_sel-minline+1,((GROUND1<<8)+DEEPBLUE));
			while(cur_sel<minline)
			{ 
				iWindowSlide(x,y,1,9,1);			/* 掉下去了 */
				--minline;
				--maxline;
			}
			while(cur_sel>maxline)
			{
				iWindowSlide(x,y,1,9,0);		  /* 升起来了 */
				++maxline;
				++minline;
			}
			/* 显示当前行选中 */
			{
				if(logic_line!= -1)
					ndx=(cur_sel>logic_line)?(cur_sel-1-1):(cur_sel-1);
				else
					ndx=cur_sel-1;
				ptr=	partnArr + ndx;					/* 指向当前选定的分区内容 */
				/*cap=(Get_nSector(ptr)>>11);				          /*以M bytes为单位*/
				cap=lintshr(11,Get_nSector(ptr));
				if((partnArr[ndx].no==-1) || (partnArr[ndx].no==0))
      	{
      		setCur(x+8,y+40+24*(cur_sel-minline+1));				          /* 空两个字符对齐 */
      		color=LIGHTGRAY;
      		//color=YELLOW;
      		c_printf(color,fdisk_str[10],fdisk_str[11]);
      	}
      	else
      	{
      		setCur(x+8+8*2,y+40+24*(cur_sel-minline+1));				          /* 空两个字符对齐 */
      		color=WHITE;
      		//color=YELLOW;
      		c_printf(color,fdisk_str[6],partnArr[ndx].no);
      	}
      	setCur(x+8+8*8,y+40+24*(cur_sel-minline+1));				          /* "分区号 " */
      	if(partnArr[ndx].active==0x80)
      		c_printf(RED,fdisk_str[7],0x03);	        									/* 显示活动分区 */
      	setCur(x+8+8*14,y+40+24*(cur_sel-minline+1));						      /*"分区号 活动  "*/
      	c_printf(color,fdisk_str[8],partnArr[ndx].stCyl);							/*显示起始分区*/
      	setCur(x+8+8*24,y+40+24*(cur_sel-minline+1));									/*"分区号 活动  起始磁道  "*/
      	c_printf(color,fdisk_str[8],partnArr[ndx].endCyl);						/*显示结束分区*/
      	setCur(x+8+8*34,y+40+24*(cur_sel-minline+1));		              /*"分区号 活动  起始磁道  结束磁道  "*/
      	c_printf(color,fdisk_str[8],cap);															/* 显示分区大小 */
      	setCur(x+8+8*46,y+40+24*(cur_sel-minline+1));									/*分区号 活动  起始磁道  结束磁道  容量(M)  */
      	c_printf(color,fdisk_str[9],partnArr[ndx].id);								/* 显示分区类型 */
      	setCur(x+8+8*51,y+40+24*(cur_sel-minline+1));	                /*"分区号 活动  起始磁道  结束磁道  容量(M)  分区类型 "*/
      	c_printf(color,fdisk_str[10],g_SysName[partnArr[ndx].id]);		/*描述*/	
      	
      	if((logic_line>=minline)&&(logic_line<=maxline))
      	{
      		setCur(x+8,y+40+24*(logic_line-minline+1));
      		c_printf(SURFACE1,fdisk_str[4]);
      	}
      	#if 0
      	/*lbdebug */	
      	setCur(0,0);
      	rectblock(0,0,80*8,16,RED);
      	c_printf(WHITE,"old_sel=%d, cur_sel=%d, minline=%d, maxline=%d, ndx=%d,",old_sel,cur_sel,minline,maxline,ndx);
				c_printf(WHITE,"logic_line=%d,i=%d",logic_line,i);
				setCur(0,24);
      	rectblock(0,24,0+60*8,24+16,GROUND1);
      	c_printf(WHITE,"maxParNO=%d,used:=%d,pri_use=%d,primary=%d,modified=%d",g_Info.fd.maxParNO,g_Info.fd.used,g_Info.fd.pri_use,g_Info.fd.primary,g_Info.fd.modified);
				#endif
			}
			chgLine(x,y,cur_sel-minline+1,((DEEPBLUE<<8)+GROUND1));
			iScrollBar(x,y,cur_sel,lines);
			asm xor ax,ax
			asm int 0x16
			asm mov hitkey,ax
			switch(hitkey)
			{
				case KEY_UP:
					old_sel=cur_sel;
					if(cur_sel==1)cur_sel=2;
					--cur_sel;
					if(cur_sel==logic_line)		/* 在其上面必定有信息显示，所以可以直接上移*/
						--cur_sel;
					break;
				case KEY_DOWN:
					old_sel=cur_sel;
					if(cur_sel==(lines -1))cur_sel=(lines -2);/* 在其下面必定有信息显示，所以可以直接上移*/
					++cur_sel;
					if(cur_sel==logic_line)
						++cur_sel;	
					break;
				case KEY_RIGHT:
				case KEY_ENTER:				/* 分区管理的功能键 */
					refresh_flag=
					partnFun(900,y+40+24*(cur_sel-minline+1),partnArr,ndx);
					break;
				case KEY_PGUP:			/* 翻页功能还是不实现了 增加无谓的程序量*/
					break;
				case KEY_PGDN:
					break;
				case KEY_ESC:
				case KEY_LEFT:
					exit_flag=SavePartn(x,y,partnArr,mbr_addr);
					break;
				default:
					break;
			}
		}while((hitkey!=KEY_ESC) && (hitkey!=KEY_LEFT) && (exit_flag!=0));
	}
	rectblock(x,y,x+570,y+240+40,GROUND1);
}
/* 分区的各种操作功能 */
/*入口参数: x,y:项目菜单的起始位置 partnArr:分区数组起始地址 ndx:数组号(0~最大) */
/*出口参数: 0:没有操作 -1:创建扩展分区 1:创建主分区 2创建逻辑分区 3:删除扩展分区*/
int partnFun(unsigned int x,unsigned int y,PARTN *partnArr,int ndx)
{
	
	MenuItem item[PARTFUN_NUM];
	int i,old_sel=0,cur_sel=0,hitkey=0,retval=0;
	unsigned long or_color,m,n;
	
	/* 初始化项目菜单变量 */
	for(i=0;i<PARTFUN_NUM;i++)
	{
		item[i].x=x;				/*900;*/
		item[i].y=y+i*34;   /*240+i*34;*/
		item[i].valid=0;
		item[i].msg=part_fun[i];
		item[i].msg_color=((SURFACE1<<8)|WHITE);
		item[i].chg_color=((DEEPBLUE<<8)|GROUND1);
	}
	if((partnArr[ndx].no==0) && (g_Info.fd.primary<4))
		item[0].valid=1;																	/* 可以创建主分区 */
	if((partnArr[ndx].no==0) && (g_Info.fd.primary<4) && (g_Info.fd.ext_head==-1))
		item[1].valid=1;																	/* 可以创建扩展分区 */
	if((partnArr[ndx].no==-1) && (g_Info.fd.used< MAX_PARTN_ELEM))
		item[2].valid=1;																	/* 可以创建逻辑分区 */	
	if((!Is_Extended(partnArr[ndx].id)) && (partnArr[ndx].no!=-1) && (partnArr[ndx].no!=0) &&(partnArr[ndx].no<=g_Info.fd.primary))
		item[3].valid=1;																  /* 可以设置活动分区 */
	if(partnArr[ndx].active==0x80)
	{
		item[4].valid=1;																  /* 可以清除活动分区 */
		item[3].valid=0;																	/* 不能设置活动分区 */	
	}
	if((partnArr[ndx].no!=-1) && (partnArr[ndx].no!=0))
	{
		item[5].valid=1;																	/* 改变分区类型 */
		item[6].valid=1;																	/* 删除分区 */
	}
	item[7].valid=1;																		/* 分区表面检测 */
	item[8].valid=1;																		/* 退出 */

	/* 将菜单显示出来 */
	for(i=0;i<PARTFUN_NUM;i++)
	{
		if(item[i].valid)
			iItem(item[i].x,item[i].y,item[i].msg,(item[i].msg_color&0xff));
		else
			iItem(item[i].x,item[i].y,item[i].msg,lintshr(8,item[i].msg_color)&0xff);
			/*iItem(item[i].x,item[i].y,item[i].msg,((item[i].msg_color>>8)&0xff));*/
	}
	do{
			/*lbdebug*/
		 #if 0	
		 {     					
			int m,n;
			rectblock(320,0,900,7*16+4,RED);
			for(m=0; m<2; m++)
			{
				setCur(320,(m*2)*16);
				c_printf(WHITE,"part%d:active=%x,no=%x,id=%x,old_id=%x,stCyl=%ld,endCyl=%ld",m+1,partnArr[m].active,partnArr[m].no,partnArr[m].id,partnArr[m].old_id,partnArr[m].stCyl,partnArr[m].endCyl);
				setCur(320,(m*2+1)*16);
				c_printf(WHITE,"  prolem=%x,remake=%d,adjust=%d,side=%ld,sector=%ld",partnArr[m].problem,partnArr[m].remake,partnArr[m].adjust,partnArr[m].side,partnArr[m].sector);
			}
			setCur(320,4*16);
			c_printf(WHITE,"pri_use=%x,primary=%x,maxParNO=%x,used=%x",g_Info.fd.pri_use,g_Info.fd.primary,g_Info.fd.maxParNO,g_Info.fd.used);
			setCur(320,5*16);
			c_printf(WHITE,"modified=%x,logic_exist=%x,ext_head=%ld",g_Info.fd.modified,g_Info.fd.logic_exist,g_Info.fd.ext_head);
			
    }
     #endif 
		
    /*
		or_color=((item[old_sel].chg_color<<8)&0xff00) + ((item[old_sel].chg_color>>8) &0xff);*/
		or_color=(lintshl(8,item[old_sel].chg_color)&0xff00)+(lintshr(8,item[old_sel].chg_color)&0xff);
		/* 清除前一个显示 */
		iItemSel(item[old_sel].x,item[old_sel].y,item[old_sel].msg,or_color);
		/* 显示当前选单 */
		iItemSel(item[cur_sel].x,item[cur_sel].y,item[cur_sel].msg,item[cur_sel].chg_color);
		asm xor ax,ax
		asm int 0x16
		asm mov hitkey,ax
		switch(hitkey)
		{
			case KEY_UP:
				old_sel=cur_sel;
				if(cur_sel==0)cur_sel=PARTFUN_NUM;
				--cur_sel;
				break;
			case KEY_DOWN:
			case KEY_TAB:
				old_sel=cur_sel;
				if(cur_sel==(PARTFUN_NUM-1))cur_sel=-1;
				++cur_sel;
				break;
			case KEY_1: case KEY_2:  case KEY_3: case KEY_4: case KEY_5:
			case KEY_6: case KEY_7:  case KEY_8: case KEY_9: case KEY_0:
			case KEY_NUM1:  case KEY_NUM2:  case KEY_NUM3:  case KEY_NUM4:
			case KEY_NUM5:  case KEY_NUM6:  case KEY_NUM7:  case KEY_NUM8:
			case KEY_NUM9:  case KEY_NUM0:
			
			  old_sel=cur_sel;
			  cur_sel=(hitkey&0x0F);
			  if((cur_sel==9)||(cur_sel==0))cur_sel=8;			/*当前共有9个菜单 0~8*/
			  else  --cur_sel;
			  hitkey=KEY_ENTER;
				break;
			default:
				break;
		}/*只有在以下情况下才能退出:左方向键、ESC、ENTER键并且所选菜单项有效 */	
	}while((hitkey!=KEY_LEFT) && (hitkey!=KEY_ESC) &&  (!((hitkey==KEY_ENTER)&& (item[cur_sel].valid))));
	/* 清除当前菜单的显示 */
	rectblock(item[0].x,item[0].y,item[PARTFUN_NUM-1].x+((c_strlen(item[PARTFUN_NUM-1].msg)+2)<<3),item[PARTFUN_NUM-1].y+32,(item[0].chg_color&0xff));
	if(hitkey==KEY_ENTER)
	{
		switch(cur_sel)
		{
			case 0:								/*  创建主分区  */
				CreateNewPartn(320,120,partnArr,ndx,PRIMARY_PARTN);
				retval=1;
				break;
			case 1:       				/* 创建扩展分区 */
				CreateNewPartn(320,120,partnArr,ndx,EXTENDED_PARTN);
				retval=-1;
				break;
			case 2:       				/* 创建逻辑分区 */
				CreateNewPartn(320,120,partnArr,ndx,LOGIC_PARTN);
				retval=2;
				break;
			case 3:       				/* 设置活动标志 */
				Set_Active_Partn(partnArr, partnArr+ndx, TRUE);
				retval=4;
				break;
			case 4:       				/* 清除活动标志 */
				Set_Active_Partn(partnArr, partnArr+ndx, TRUE);
				retval=4;
				break;
			case 5:       				/* 设置分区类型 */
				SetPartID(320,120,partnArr,ndx);
				retval=5;
				break;
			case 6:       				/* 删 除 分 区  */
				DeletePartn(320,120,partnArr,ndx);
				retval=3;
				break;
			case 7:       				/* 分区表面检测 */
				ChkPartSurface(320,120,partnArr,ndx);
				break;
			case 8:       				/*   退    出   */
				break;
			default:	break;	                     
		}
	}
	return retval;
}
/* 创建新的分区 */
/*入口参数: x,y:分区管理的坐标 kind,分区的种类*/
/*出口参数: 0:没有选定功能 1:进行了功能处理 */
void CreateNewPartn (unsigned int x,unsigned y,PARTN *partnArr, int ndx,int kind)
{
	/* 分区管理部分 必定使用了十行，必须空出这部分 */
	char title[17],ack;
	int i,j;
	unsigned int winx=x,winy=y+10*24+32+16;
	PARTN  tmp,*ptr;
	unsigned long disk_size,begin,end;
		
	ptr= partnArr + ndx;
	for(i=0;i<c_strlen(part_fun[0]);i++)
	{
		switch(kind)
		{
			case PRIMARY_PARTN :
				title[i]=part_fun[0][i];
				break;
			case EXTENDED_PARTN:
				title[i]=part_fun[1][i];
				break;
			case LOGIC_PARTN:
				title[i]=part_fun[2][i];
				break;
			default:
				title[i]='\0';
				break;
		}
	}
	title[c_strlen(part_fun[0])]='\0';
	
	iWindowOut(winx,winy,title,10);
	tmp.sector = 1;
  tmp.stCyl  = tmp.side = 0;
  tmp.endCyl = CUR_DK->lgeo.maxCyl;
  disk_size=lintshr(11,Get_nSector(&tmp));
  /*disk_size  = (Get_nSector(&tmp)>>11);*/
	setCur(winx+8,winy+40);
	c_printf(SURFACE1,creatpart_str[0],disk_size,CUR_DK->lgeo.maxCyl+1,CUR_DK->lgeo.maxHead+1,CUR_DK->lgeo.maxSect);
	setCur(winx+8,winy+40+24);
	c_printf(SURFACE1,creatpart_str[1],partnArr[ndx].stCyl,partnArr[ndx].endCyl);
	if(ptr->stCyl == ptr->endCyl)
	{
		setCur(winx+8,winy+40+24*2);
		c_printf(WHITE,creatpart_str[2]);
		asm xor ax,ax
		asm int 0x16
	}
	else
	{
		begin=0xffffffff;
		end=0xffffffff;
		/*setCur(winx+8,winy+40+24*3);
		c_printf(WHITE,creatpart_str[3],ptr->stCyl,ptr->endCyl);*/
		while((begin<ptr->stCyl) || (begin >ptr->endCyl))
		{
			setCur(winx+8,winy+40+24*3);
			c_printf(WHITE,creatpart_str[3],ptr->stCyl,ptr->endCyl);
			/*setCur(winx+8+(c_strlen(creatpart_str[3])<<3),winy+40+24*3);*/			/* 输入起始磁道 */
			c_scanf(DEEPBLUE,YELLOW,creatpart_str[5],&begin);
		}
		/*setCur(winx+8,winy+40+24*4);
		c_printf(WHITE,creatpart_str[4],begin,ptr->endCyl);*/
		while((end<ptr->stCyl) || (end >ptr->endCyl)|| (end<begin))
		{
			setCur(winx+8,winy+40+24*4);
			c_printf(WHITE,creatpart_str[4],begin,ptr->endCyl);
			/*setCur(winx+8+(c_strlen(creatpart_str[4])<<3),winy+40+24*4);*/
			c_scanf(DEEPBLUE,YELLOW,creatpart_str[5],&end);
		}
		/*setCur(winx+8,winy+40+24*5);
		c_printf(WHITE,creatpart_str[6],begin,end);*/
		ack=0;
		while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n')) 
		{
			setCur(winx+8,winy+40+24*5);
			c_printf(WHITE,creatpart_str[6],begin,end);
			/*setCur(winx+8+(c_strlen(creatpart_str[6])<<3),winy+40+24*5);*/
			c_scanf(DEEPBLUE,WHITE,creatpart_str[7],&ack);
		}
		if((ack=='Y') || (ack=='y'))
		{
			 Create_Partn(partnArr, ndx, begin, end, kind, 1);          /** 建立分区 **/
			 g_Info.fd.modified = 1;
			}
	}
	rectblock(winx,winy,winx+570,winy+240+40,GROUND1);
}
/* 删除分区 */
void DeletePartn(unsigned int x,unsigned y,PARTN *partnArr, int ndx)
{
	unsigned int winx=x,winy=y+10*24+32+16;
	char ack=0;
	iWindowOut(winx,winy,part_fun[6],6);
	setCur(winx+8,winy+40);
	c_printf(SURFACE1,delPartn_str[0],partnArr[ndx].no,partnArr[ndx].stCyl,partnArr[ndx].endCyl);
	while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n')) 
	{
		setCur(winx+8,winy+40+24*2);
		c_scanf(DEEPBLUE,WHITE,delPartn_str[1],&ack);
	}
	if((ack=='Y') || (ack=='y'))
		Del_Partn(partnArr,ndx);
	rectblock(winx,winy,winx+570,winy+240+40,GROUND1);
}
/* 设定分区类型 */
void SetPartID(unsigned int x,unsigned y,PARTN *partnArr, int ndx)
{
	unsigned int winx=x,winy=y+10*24+32+16;
	int i, j, update, newID, oldID;
  PARTN  *ptr;
 
	ptr = &partnArr[ndx];
	iWindowOut(winx,winy,part_fun[5],10);
	/* 每个分区类型最大占用19个字节 */
	for(j=0;j<9;j++)
		for(i=0;i<3;i++)
		{
			setCur(winx+8+(3<<3)+((i*20)<<3),winy+40+24*j);
			c_printf(YELLOW,setid_str[1],setid_array[j*3+i]);
			c_printf(WHITE,setid_str[2],g_SysName[setid_array[j*3+i]]);
		}
	newID=-1;
	while((newID<0) || (newID>0xff))
	{
		setCur(winx+8,winy+40+24*9);
		c_scanf(DEEPBLUE,WHITE,setid_str[0],&newID);
	}
	
  if ( ptr->id != newID )
  {
      update = 1;
      if ( Is_Extended(newID) )
      {
          if ( !Is_Extended(ptr->old_id) )           /** 原本不是扩充分割 **/
          {
              if ( g_Info.fd.ext_head != -1 )   /** 检查扩充分割是否已存在**/
              {
                  update = 0;
                  showERROR(winx+(12<<3),winy+40+4*24,setid_str[3]);
              }
              else
              {
                  if ( ptr->stCyl == 0 )                 /** 调整启始磁柱 **/
                      ptr->stCyl = 1;
                  g_Info.fd.ext_head = ptr->stCyl;       /** 建立扩充分割 **/
                  g_Info.fd.ext_end  = ptr->endCyl;
                  g_Info.fd.maxParNO = 4;
                  Ins_elem(partnArr, g_Info.fd.used, ptr->stCyl, ptr->endCyl);
              } /* end if */
          } /* end if */
      }
      else
      {
          if ( newID == 0 )                     /** 如果 ID 是 0 等于删除 **/
          {
              Del_Partn(partnArr, ndx);
          }
          else if ( Is_Extended(ptr->old_id) )           /** Ext 改为其它 **/
          {
              if ( ptr->stCyl == 1 )                     /** 调整启始磁柱 **/
                  ptr->stCyl = 0;
              g_Info.fd.used     =  g_Info.fd.pri_use;
              g_Info.fd.ext_head = -1;
              g_Info.fd.ext_end  = -1;
              g_Info.fd.maxParNO =  4;
          } /* end if */
      } /* end if */
  
      if ( update )
      {
          if ( newID != 0 )
          {
              ptr->id = newID;
              oldID   = ptr->old_id;

              if ( !IsHiddenID(newID) )
                  ptr->old_id = newID;
              else if ( IsHiddenWinID(newID) )
                  ptr->old_id = newID & 0x0f;

              if ( !Is_Extended(oldID) && Is_Extended(newID) )
                  if ( ChkValidLogicPartn(ptr) )
                      ReloadLogicPartn(partnArr);
          } /* end if */

          if ( g_Info.fd.modified <= 0 )
              g_Info.fd.modified = -1;             /** 如果没有建立新的分割 **/
      } /* end if */
  } /* end if */
    
	rectblock(winx,winy,winx+570,winy+240+40,GROUND1);				/* 消除自身 */
}
/* 分区表面检测 */
void ChkPartSurface(unsigned int x,unsigned y,PARTN *partnArr, int ndx)
{
	#define MAXBADCOUNT 1024
	
	unsigned int winx=x,winy=y+10*24+32+16;
	unsigned int editx=x+8,edity=winy+40;
	unsigned long badblock[MAXBADCOUNT];				/* 只保留发现的前512个磁道,如果这个分区有这么多的坏道,估计也差不多了*/
	unsigned long badcount=0,totalCyl=0,curCyl=0,gradx;
	unsigned int  blocks,b_lines,res_block;
	int i,j;
	char ack;
	
	for(i=0;i<MAXBADCOUNT;i++)badblock[i]=0xffffffff;		/* 初始化空间 */
	totalCyl=partnArr[ndx].endCyl-partnArr[ndx].stCyl+1;
	/*7、8、9行为信息显示专用 */
	/* 检测专用的范围(winx+8,winy+40),(winx+8+66*8,winy+40+24*6+16)*/
	iWindowOut(winx,winy,part_fun[7],10);
	/*gradx=(totalCyl+7*66-1)/(7*66);*/
	gradx=lintdiv(462,totalCyl+461);
	/*blocks=(unsigned int)((totalCyl+gradx-1)/gradx);*/
	blocks=(unsigned int)lintdiv(gradx,totalCyl+gradx-1);
	b_lines=(blocks/66);
	res_block=(blocks%66);
	if(b_lines)
		rectblock(editx,edity,editx+66*8-2,edity+24*b_lines-2,DEEPBLUE);
	if(res_block)	
		rectblock(editx,edity+24*b_lines-1,editx+((res_block-1)<<3)+6,edity+24*b_lines-1+23,DEEPBLUE);	
	//rectblock(editx,edity,editx+66*8-2,edity+24*6+16+6,DEEPBLUE);			/* 初始化显示需要检测的坏道数 */
	
	/*lbdebug*/
	/*
	setCur(0,20);
	c_printf(WHITE,"blocks=%d,b_lines=%d,res_block=%d",blocks,b_lines,res_block);*/
	rectblock(editx,edity+7*24+1,editx+8,edity+24*7+16,DEEPBLUE);
	setCur(editx+8*2-4,edity+24*7);
	c_printf(SURFACE1,chkpart_str[0]);
	i=c_strlen(chkpart_str[0])+2;
	rectblock(editx+((i+2)<<3),edity+7*24+1,editx+((i+3)<<3),edity+7*24+16,WHITE);
	setCur(editx+((i+4)<<3)-4,winy+40+24*7);
	c_printf(SURFACE1,chkpart_str[1]);
	i=c_strlen(chkpart_str[0])+c_strlen(chkpart_str[1])+1+5;
	rectblock(editx+((i+2)<<3),edity+7*24+1,editx+((i+3)<<3),edity+7*24+16,BLACK);
	setCur(editx+((i+4)<<3)-4,winy+40+24*7);
	c_printf(SURFACE1,chkpart_str[2]);
	j=((i+4+4)<<3)+(c_strlen(chkpart_str[2])<<3);
	setCur(editx+j,winy+40+24*7);
	c_printf(SURFACE1,chkpart_str[3],partnArr[ndx].stCyl,partnArr[ndx].endCyl);
	setCur(editx,edity+8*24);
	c_printf(SURFACE1,chkpart_str[4],totalCyl,badcount,curCyl,totalCyl);
	ack=0;
	while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n')) 
	{
		setCur(editx,edity+9*24);
		rectblock(editx,edity+9*24,editx+66*8,edity+9*24+16,GROUND1);
		c_scanf(DEEPBLUE,WHITE,chkpart_str[5],&ack);
	}
	if((ack=='y') || (ack=='Y'))
	{
		unsigned long grad,  side, temp;
		unsigned long ctrl;
		unsigned char badflag=0;
		unsigned int hitkey=0;
		
		/*lbdebug*/
		/*
		partnArr[ndx].stCyl=400;
		partnArr[ndx].endCyl=700;
		totalCyl=700-400+1;*/
		/* 显示提示 */
		setCur(editx,edity+9*24);
		rectblock(editx,edity+9*24,editx+66*8,edity+9*24+16,GROUND1);
		c_printf(WHITE,chkpart_str[8]);
		/*grad=(totalCyl+7*66-1)/(7*66);			/* 共有 7行66列的显示块 */
		grad=lintdiv(462,totalCyl+461);
		i=0;j=0;
		for(curCyl=partnArr[ndx].stCyl,ctrl=0;curCyl<=partnArr[ndx].endCyl;++curCyl,++ctrl)
		{
			/* lbdebug */
			/*asm xor ax,ax
			//asm int 0x16
      setCur(0,0);
      rectblock(0,0,80*8,16,RED);
      c_printf(WHITE,"grad=%ld,i=%d,j=%d,curCyl=%ld,ctrl=%ld",grad,i,j,curCyl,ctrl);*/
			/* 更新显示信息 */
			setCur(editx,edity+8*24);
			rectblock(editx,edity+8*24,editx+66*8,edity+8*24+16,GROUND1);
			c_printf(SURFACE1,chkpart_str[4],totalCyl,badcount,curCyl,totalCyl-curCyl+partnArr[ndx].stCyl-1);
			for ( side = 0 ; side <= CUR_DK->lgeo.maxHead ; side++ )
				if( chs_CheckWDK(side, curCyl, 1, CUR_DK->lgeo.maxSect, NULL) )
				{
					badflag=1;							/* 发现坏扇区，本磁道停止检测 */
					break;
				}
			if((badcount<MAXBADCOUNT) && (badflag==1))
			{
				badblock[badcount]=curCyl;						/* 保存坏道的信息 */
				++badcount;
			}
			/*if(!(ctrl%grad))*/		/* 根据粒度显示 */
			if(!(lintmod(grad,ctrl)))
			{
				/*i=(unsigned int)((ctrl/grad)%66);					/* 显示块显示的位置 行*/
				/*j=(unsigned int)((ctrl/grad)/66);					/* 显示块显示的位置 列*/
				i=(unsigned int)(lintmod(66,lintdiv(grad,ctrl)));
				j=(unsigned int)(lintdiv(66,lintdiv(grad,ctrl)));
				
				if(badflag==1)
					rectblock(editx+8*i,edity+j*24,editx+8*i+6,edity+j*24+22,BLACK);
				else
					rectblock(editx+8*i,edity+j*24,editx+8*i+6,edity+j*24+22,WHITE);
				badflag=0;					/* 重置坏道标志 */
			}
			/*检测是否有 ESC 键按下 如果有则退出*/
			asm mov ax,0x0100
			asm int 0x16
			asm jz NextCyc					/* 没有键按下 */
			asm xor ax,ax
			asm int 0x16
			asm mov hitkey,ax
			if(hitkey==KEY_ESC)
				break;
			if(hitkey==KEY_SPACE)
			{
				/*暂停*/
				asm xor ax,ax
				asm int 0x16
				asm mov hitkey,ax			
			}
			if(hitkey==KEY_ESC)
				break;
			NextCyc:
		}
		if(badcount)				/* 有坏道 */
		{
			ack=0;
			while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n')) 
			{
				setCur(editx,edity+9*24);
				rectblock(editx,edity+9*24,editx+66*8,edity+9*24+16,GROUND1);
				c_scanf(DEEPBLUE,WHITE,chkpart_str[7],&ack);
			}
			if((ack=='y') ||(ack=='Y'))
			{
				int old_sel=0,cur_sel=0,minline=0,maxline=7,lines;
				
				/* 显示坏道信息 */
				rectblock(winx,winy,winx+570,winy+240+40,GROUND1);
				/*lines=(int)((((badcount<MAXBADCOUNT)?badcount:MAXBADCOUNT)+7)/8);   	/* 一行显示8个坏道信息 */
				lines=(int)( lintshr(3,((badcount<MAXBADCOUNT)?badcount:MAXBADCOUNT) +7) );
				if(lines+2<=10)
					iWindowOut(winx,winy,part_fun[7],10);
				else
					iWindowOut(winx,winy,part_fun[7],lines+2);
				setCur(editx,edity+8*24);
				c_printf(SURFACE1,chkpart_str[9],partnArr[ndx].stCyl,partnArr[ndx].endCyl,totalCyl);
				setCur(editx,edity+9*24);
				temp=MAXBADCOUNT;
				c_printf(SURFACE1,chkpart_str[10],temp);
					
				for(j=0;j<((lines<8)?lines:8);j++)
					for(i=0;i<8;i++)
					{
						setCur(editx+(3<<3)+((i*8)<<3),edity+j*24);
						if(badblock[j*8+i]!=0xffffffff)
							c_printf(WHITE,chkpart_str[11],badblock[j*8+i]);
					}
				do{
						/*lbdebug 	*/
						/*
      			setCur(0,0);
      			rectblock(0,0,80*8,16,RED);
      			c_printf(WHITE,"old_sel=%d,cur_sel=%d,minline=%d,maxline=%d,lines=%d,",old_sel,cur_sel,minline,maxline,lines);
						*/
						/* 清除前一行的选中显示 */
						chgLine(winx,winy,old_sel-minline,((GROUND1<<8)+DEEPBLUE));
						if(cur_sel<minline)
						{ 
							iWindowSlide(winx,winy,0,7,1);			/* 掉下去了 */
							--minline;
							--maxline;
						}
						if(cur_sel>maxline)
						{
							iWindowSlide(winx,winy,0,7,0);		  /* 升起来了 */
							++maxline;
							++minline;
						}
						/* 显示当前行选中 */
						for(i=0;i<8;i++)
						{
							setCur(editx+(3<<3)+((i*8)<<3),edity+(cur_sel-minline)*24);
							if(badblock[cur_sel*8+i]!=0xffffffff)
								c_printf(WHITE,chkpart_str[11],badblock[cur_sel*8+i]);
						}
						chgLine(winx,winy,cur_sel-minline,((DEEPBLUE<<8)+GROUND1));
						iScrollBar(winx,winy,cur_sel,lines+2);
						asm xor ax,ax
						asm int 0x16
						asm mov hitkey,ax
						switch(hitkey)
						{
							case KEY_UP:
								old_sel=cur_sel;
								if(cur_sel==0)cur_sel=1;
								--cur_sel;
								break;
							case KEY_DOWN:
								old_sel=cur_sel;
								if(cur_sel==(lines-1))cur_sel=(lines-2);
								++cur_sel;
								break;
							case KEY_PGUP:			/* 翻页功能还是不实现了 增加无谓的程序量*/
								break;
							case KEY_PGDN:
								break;
							default:
								break;
						}
					}while(hitkey!=KEY_ESC);			
			}
		}
		else
		{
			setCur(editx,edity+9*24);
			rectblock(editx,edity+9*24,editx+66*8,edity+9*24+16,GROUND1);
			c_printf(WHITE,chkpart_str[6]);
			asm xor ax,ax
			asm int 0x16
		}
	}
	rectblock(winx,winy,winx+570,winy+240+40,GROUND1);				/* 消除自身 */
}
/* 错误显示,一般会直接退出当前的功能 */
void showERROR(unsigned int x,unsigned y,unsigned char *s)
{
	unsigned int length,height;
	
	length=((c_strlen(s)+2)<<3);
	height=32;
	rectangle(x,y,x+length,y+height,WHITE);
	line(x+length,y,x+length,y+height,DARKPROJECT1);
	line(x+length,y+height,x,y+height,DARKPROJECT1);
	rectblock(x+1,y+1,x+length-1,y+height-1,SURFACE1);
	setCur(x+8,y+8);
	c_printf(RED,s);
	asm xor ax,ax
	asm int 0x16
}


/* 存储用户对分区的更改 */
/*出口参数:0:存储完毕或者无法存储,允许用户退出fdisk, 非0:用户还要继续进行更改*/
int SavePartn(unsigned int x,unsigned y,PARTN *partnArr, unsigned long mbr_addr)
{
	unsigned int winx=x,winy=y+10*24+32+16;
	int  active, noChg, i;
  struct SavePara para;
  char ack;
		
	if(!g_Info.fd.modified)return;						/* 没有修改则直接退出 */
	iWindowOut(winx,winy,sysFun_str[0],5);		/* 准备显示 */
	ack=0;
	while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n')) 
	{
		setCur(winx+8,winy+40);;
		c_scanf(DEEPBLUE,WHITE,savepart_str[1],&ack);
	}
	if((ack=='n') || (ack=='N'))
	{
		rectblock(winx,winy,winx+570,winy+240+40,GROUND1);				/* 消除自身 */
		return 1;																/* 不退出,用户继续操作*/
	}
	else
	{
		if ( !g_Info.fd.canSaveFlag )							/* 如果因分区错误不能存储,则提示,但是允许退出fdisk*/
		{
			setCur(winx+8,winy+40);
			c_printf(WHITE,savepart_str[0]);
			asm xor ax,ax
			asm int 0x16
		}
		/* 开始试图存储 */
		active = FALSE;
  	if ( CUR_DK->diskNO != 0x80 )
  	    active = TRUE;
  	else if ( g_Info.fd.primary > 0 )
  	{
  	    for ( i = 0 ; i < g_Info.fd.pri_use ; i++ )
  	        if ( partnArr[i].active == 0x80 )
  	        {
  	            active = TRUE;
  	            break;
  	        } /* end if */
  	} /* end if */
  	/*LBDEBUG*/
  	/*
  	setCur(0,20);
  	c_printf(WHITE,"active=%d,primary=%d,diskNO=%d,pri_use=%d",active,g_Info.fd.primary,CUR_DK->diskNO,g_Info.fd.pri_use);
		*/
		if(!active)															/* 未设置活动分区,提示*/
		{
			ack=0;
			while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n')) 
			{
				setCur(winx+8,winy+40+24);
				c_scanf(DEEPBLUE,WHITE,savepart_str[2],&ack);
			}
		}
		if((ack=='n') || (ack=='N'))
		{
			rectblock(winx,winy,winx+570,winy+240+40,GROUND1);				/* 消除自身 */
			return 1;	
		}
		para.partnArr = partnArr;
    para.doom     = TRUE;
    para.adjust   = FALSE;
    setCur(winx+8,winy+40+24*2);
    if ( Partn_Save_Process(&para,mbr_addr) )				/* 存储 非0为写入失败 */
    	c_printf(WHITE,savepart_str[3]);
    else		c_printf(WHITE,savepart_str[4]);   
    asm xor ax,ax
    asm int 0x16
	}	
	rectblock(winx,winy,winx+570,winy+240+40,GROUND1);				/* 消除自身 */	
	return 0;
}