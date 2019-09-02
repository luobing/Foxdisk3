#include "Vesa.h"
#include "Graphic.h"
#include "Font.h"

#include "Common.h"
#include "Painter.h"
#include "SetPara.h"
#include "MainMenu.h"
#include "Fdisk.h"
#include "INT1ch.h"
#include "Foxdisk.h"
#include "Global.h"
#include "iMath.h"
#include "iMem.h"

void setParameter(FOXPARA *para,OS *my_os);
void countDown(unsigned int x,unsigned int y,FOXPARA *para,int flag);
void setDefOS(unsigned int x,unsigned int y,FOXPARA *para,OS *my_os);
void setWallPaper(unsigned int x,unsigned int y,FOXPARA *para,int flag);
void exitParaSet(unsigned int x,unsigned int y,FOXPARA *cur_para,FOXPARA *old_para);

/* 设置程序提供的各种参数 */
void setParameter(FOXPARA *para,OS *my_os)
{
  BAR paraBar[6];					/* 参数设定的菜单为6个 */  
  int i,j,hitkey=0,refresh_flag=1,old_sel=0,cur_sel=0;
  unsigned long chg_color=0,or_color;
	unsigned int sel_flag=0;
	FOXPARA	old_para;
	/* 保存原始的参数信息,在退出前比较并决定是否需要存储 */
	imemcpy(&old_para,para,sizeof(FOXPARA));
	/*old_para=*para;*/
		
	iFasceStr(2,WHITE);		  /* F2 系统菜单清除 */
	iFasceStr(3,RED);						/* F3 系统菜单选定 */
	iFaceClr(BOTTOM_FACE);			/* 清除显示 */
	chg_color=((RED<<8)|MGROUND1);			/* 此处规定选定后的导航条颜色 */
  /* 初始化选单 */ 
  for(i=0;i<6;i++) 
  {
  	paraBar[i].x=0;
  	paraBar[i].y=250+i*64;
  	paraBar[i].msg_color=WHITE;
		paraBar[i].font=(ASC0|_ASC0|HZKK|_HZ24);
		paraBar[i].chg_color=chg_color;
  }	
  for(i=0;i<6;i++)
		navigationBar(paraBar[i].x,paraBar[i].y,paraBar[i].font,paramenu_str[i],paraBar[i].msg_color,1);
  /* 显示菜单 */
  do{
  		if(refresh_flag)
  		{
  			refresh_flag=0;						/* 清除刷新标志 */
				iMessage(DARKPROJECT1);
  		}
  		/* 根据保存的sel显示当前选定 */
  		or_color=(lintshr(8,paraBar[old_sel].chg_color)&0xff) + (lintshl(8,paraBar[old_sel].chg_color)&0xff00);
			/*or_color=(((paraBar[old_sel].chg_color)>>8)&0xff)+(((paraBar[old_sel].chg_color)<<8)&0xff00);
			*/
			chgNavbar(paraBar[old_sel].x,paraBar[old_sel].y,paraBar[old_sel].font,paramenu_str[old_sel],or_color,1);
			chgNavbar(paraBar[cur_sel].x,paraBar[cur_sel].y,paraBar[cur_sel].font,paramenu_str[cur_sel],paraBar[cur_sel].chg_color,1);
			asm xor ax,ax
			asm int 0x16
			asm mov hitkey,ax
			switch(hitkey)
			{
				case KEY_UP:
					old_sel=cur_sel;
					if(cur_sel==0)cur_sel=6;
					--cur_sel;
					break;
				case KEY_DOWN:
				case KEY_TAB:
					old_sel=cur_sel;
					if(cur_sel==5)cur_sel= -1;
					++cur_sel;
					break;
				case KEY_RIGHT:
				case KEY_ENTER:
				if(cur_sel==5)sel_flag=1;
				refresh_flag=1;
				switch(cur_sel)
				{
					case 0:
						countDown(320,220,para,1);
						break;
					case 1:
						countDown(320,220,para,-1);
						break;
					case 2:
						setDefOS(320,220,para,my_os);
						break;
					case 3:
						setWallPaper(320,220,para,1);
						break;
					case 4:
						setWallPaper(320,220,para,-1);
						break;
				}
					break;
				case KEY_CTRLS:
					exitParaSet(320,220,para,&old_para);
					break;
				default:
					break;
			}
	}while((hitkey!=KEY_ESC) && (hitkey!=KEY_F2) && (!sel_flag));
	/* 退出前必须决定是否存储参数设置: 在maxCyl-10+1的开始扇区中*/
	exitParaSet(320,220,para,&old_para);
	iFasceStr(3,WHITE);		    /* F3 设置参数菜单清除 */
}
/* 设置/取消倒计时 */
void countDown(unsigned int x,unsigned int y,FOXPARA *para,int flag)
{
	unsigned char title[17];
	unsigned char *ptr;
	int i,newtime=0;
	char ack=0;
	
	ptr=paramenu_str[((flag==-1)?1:0)];
	while(*ptr ==0x20)
		(++ptr);
	for(i=0;i<12;i++)
		title[i]=ptr[i]	;
	title[12]='\0';	 	
	iMessage(GROUND1);										/* 清除显示 */
	iWindowOut(x,y,title,10);
	setCur(x+8,y+40);
	c_printf(SURFACE1,cntdown_str[0]);
	setCur(x+8,y+40+24);
	c_printf(SURFACE1,cntdown_str[1]);
	if(para->cntdown_time==-1)
		c_printf(WHITE,cntdown_str[3]);
	else
		c_printf(WHITE,cntdown_str[2]);
	setCur(x+8,y+40+24*2);
	c_printf(SURFACE1,cntdown_str[4]);
	if(para->cntdown_time==-1)
		c_printf(WHITE,cntdown_str[5]);
	else
		c_printf(WHITE,cntdown_str[9],para->cntdown_time);
	if(flag==-1)
	{
		while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
		{
			setCur(x+8,y+40+24*4);
			c_scanf(DEEPBLUE,WHITE,cntdown_str[7],&ack);
		}
		if((ack=='Y') || (ack=='y'))
			para->cntdown_time=-1;					/* 隐藏倒计时 */
	}
	else
	{
		while((newtime<3)||(newtime>99))
		{
			setCur(x+8,y+40+24*4);
			c_scanf(DEEPBLUE,WHITE,cntdown_str[6],&newtime);
		}
		para->cntdown_time=newtime;
	}
	setCur(x+8,y+40+24*5);
	c_printf(WHITE,cntdown_str[8]);
	
	rectblock(x+8,y+40+24,x+8+(66<<3),y+40+24*3+16,GROUND1);
	setCur(x+8,y+40+24);
	c_printf(SURFACE1,cntdown_str[1]);
	if(para->cntdown_time==-1)
		c_printf(WHITE,cntdown_str[3]);
	else
		c_printf(WHITE,cntdown_str[2]);
	setCur(x+8,y+40+24*2);
	c_printf(SURFACE1,cntdown_str[4]);
	if(para->cntdown_time==-1)
		c_printf(WHITE,cntdown_str[5]);
	else
		c_printf(WHITE,cntdown_str[9],para->cntdown_time);
	asm xor ax,ax
	asm int 0x16
	rectblock(x,y,x+570,y+24*10+40,GROUND1);			/* 清除当前显示 */
	
}
/* 设置缺省启动系统 */
void setDefOS(unsigned int x,unsigned int y,FOXPARA *para,OS *my_os)
{
	unsigned char title[17];
	unsigned char *ptr;
	int i,j,valid_os=0,temp[4];
	
	for(i=0;i<4;i++)
		for(j=0;j<4;j++)
			if(my_os[j].bar_index==(unsigned char)(i))
				temp[i]=j;
	
	ptr=paramenu_str[2];
	while(*ptr ==0x20)
		(++ptr);
	for(i=0;i<12;i++)
		title[i]=ptr[i]	;
	title[12]='\0';	
	iMessage(GROUND1);										/* 清除显示 */
	iWindowOut(x,y,title,10);
	
	setCur(x+8,y+40);
	c_printf(SURFACE1,setdefos_str[0]);
	for(i=0;i<4;i++)
	{
		setCur(x+8+32,y+40+24*(i+1));
		if(my_os[temp[i]].os_valid)
		{
			c_printf(SURFACE1,setdefos_str[1],i+1,my_os[temp[i]].os_name);
			++valid_os;
		}
		else
			c_printf(SURFACE1,setdefos_str[1],i+1,setdefos_str[2]);
	}
	setCur(x+8,y+40+24*5);
	c_printf(SURFACE1,setdefos_str[3],(para->def_os)+1,my_os[temp[para->def_os]].os_name);
	
	if(valid_os<2)
	{
		setCur(x+8,y+40+24*7);
		c_printf(WHITE,setdefos_str[4]);
		
	}
	else
	{
		i=-1;			/* 接受用户的输入 */
		while((i<1) || (my_os[temp[i-1]].os_valid==0) || (i>4))
		{
			setCur(x+8,y+40+24*7);
			c_scanf(DEEPBLUE,WHITE,setdefos_str[5],&i);
		}
		para->def_os=i-1;
		setCur(x+8,y+40+24*8);
		c_printf(WHITE,setdefos_str[6]);
		rectblock(x+8,y+40+24*5,x+8+(66<<3),y+40+24*5+16,GROUND1);
		setCur(x+8,y+40+24*5);
		c_printf(SURFACE1,setdefos_str[3],(para->def_os)+1,my_os[temp[para->def_os]].os_name);
	}
	asm xor ax,ax
	asm int 0x16
	rectblock(x,y,x+570,y+24*10+40,GROUND1);			/* 清除当前显示 */	 	
}
/* 更换壁纸 */
void setWallPaper(unsigned int x,unsigned int y,FOXPARA *para,int flag)
{
	unsigned char title[17];
	unsigned char *ptr;
	int newwp=-1,i;
	char ack=0;
	
	/* lbdebug */
  /*		rectblock(0,0,500,60,RED);
  		setCur(0,0);
  		c_printf(WHITE,"para: numwp=%d,curwp=%d,times=%d",para->wpaper_num,para->wallpaper,para->cntdown_time);
  */		
	ptr=paramenu_str[((flag==-1)?4:3)];
	while(*ptr ==0x20)
		(++ptr);
		
	for(i=0;i<12;i++)
		title[i]=ptr[i];
	title[12]='\0';	 
	iMessage(GROUND1);										/* 清除显示 */
	iWindowOut(x,y,title,10);
	setCur(x+8,y+40);
	c_printf(SURFACE1,setwallp_str[0]);

	if(para->wpaper_num==0)
		para->wallpaper=-1;
		
	setCur(x+8,y+40+24);
	c_printf(SURFACE1,setwallp_str[1],para->wpaper_num);
	setCur(x+8,y+40+24*2);
	if(para->wallpaper== -1)
	{
		c_printf(SURFACE1,setwallp_str[3]);
		c_printf(SURFACE1,setwallp_str[8],setwallp_str[2]);
	}
	else
	{
		c_printf(SURFACE1,setwallp_str[3]);
		c_printf(SURFACE1,setwallp_str[6],para->wallpaper+1);
	}
	
	if(para->wpaper_num==0)
	{
		setCur(x+8,y+40+24*4);
		c_printf(WHITE,setwallp_str[4]);		
	}
	else if(flag!=-1)
	{
		setCur(x+8,y+40+24*4);
		c_printf(WHITE,setwallp_str[5],para->wpaper_num);
		while((newwp<1) || (newwp>para->wpaper_num))
		{
			setCur(x+8+(c_strlen(setwallp_str[5])<<3),y+40+24*4);
			c_scanf(DEEPBLUE,WHITE,setwallp_str[6],&newwp);	
		}
		para->wallpaper=newwp-1;
		setCur(x+8,y+40+24*5);
		c_printf(WHITE,setwallp_str[7]);
	}
	else 
	{
		while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
		{
			setCur(x+8,y+40+24*4);
			c_scanf(DEEPBLUE,WHITE,setwallp_str[9],&ack);
		}
		if((ack=='Y')||(ack=='y'))
		{
			para->wallpaper=-1;
			setCur(x+8,y+40+24*5);
			c_printf(WHITE,setwallp_str[7]);
		}
	}
	/* 更新显示 */
	if((ack!='N') && (ack!='n'))
	{
		rectblock(x+8,y+40+24,x+8+(66<<3),y+40+24*2+16,GROUND1);
		setCur(x+8,y+40+24);
		c_printf(SURFACE1,setwallp_str[1],para->wpaper_num);
		setCur(x+8,y+40+24*2);
		if(para->wallpaper==-1)
		{
			c_printf(SURFACE1,setwallp_str[3]);
			c_printf(SURFACE1,setwallp_str[8],setwallp_str[2]);
		}
		else
		{
			c_printf(SURFACE1,setwallp_str[3]);
			c_printf(SURFACE1,setwallp_str[6],para->wallpaper+1);
		}
		asm xor ax,ax
		asm int 0x16
	}
	rectblock(x,y,x+570,y+24*10+40,GROUND1);			/* 清除当前显示 */
}

/* 保存信息处理*/
void exitParaSet(unsigned int x,unsigned int y,FOXPARA *cur_para,FOXPARA *old_para)
{
	char ack=0;
	unsigned char *ptr,*old_ptr;
	int i;
	
	if(!i_memcmp(cur_para,old_para,sizeof(FOXPARA)))		/* 内容是否相同,判断是否需要保存 */
		return;
	iMessage(GROUND1);										/* 清除显示 */
	iWindowOut(x,y,exitPara_str[0],5);
	while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
	{
		setCur(x+8,y+40+24);
		c_scanf(DEEPBLUE,WHITE,exitPara_str[1],&ack);
	}
	if((ack=='y') || (ack=='Y'))
	{
		setCur(x+8,y+40+24*2);
		if(!SavFoxPara(cur_para))
		{
			c_printf(WHITE,exitPara_str[2]);
			/* 更新old_para */
			ptr=(unsigned char *)(cur_para);
			old_ptr=(unsigned char *)(old_para);
			for(i=0;i<sizeof(FOXPARA);i++)
				old_ptr[i]=ptr[i];
		}
		else
			c_printf(WHITE,exitPara_str[3]);
		asm xor ax,ax
		asm int 0x16
	}
	else
		imemcpy(cur_para,old_para,sizeof(FOXPARA));		/* 如果不保存的话,将以前的配置还原 */
	rectblock(x,y,x+570,y+24*10+40,GROUND1);			/* 清除当前显示 */
}