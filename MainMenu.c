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

#include "DISKDRV.h"
#include "DISKAPP.h"

#include "BootOS.h"


void initSysMenu(BAR *sys_bar,OS *my_os);
void SysRun(OS *my_os,FOXPARA *sys_para);
void helpList(unsigned int x,unsigned int y,unsigned char *title);
int SysFun(BAR *barx,OS *my_os,unsigned int nos);
void	nameSysMenu(unsigned int x,unsigned int y,BAR *barx,OS *osx);
void sortSysMenu(unsigned int x,unsigned int y,OS *my_os);
void pwdSys(unsigned int x,unsigned int y,OS *osx);
void hideSysFun(unsigned int x,unsigned int y,OS *my_os);
void aboutFoxdisk(unsigned int x,unsigned int y,unsigned char *title);
void exitSysRun(unsigned int x,unsigned int y,FOXPARA *para,OS *cur_os,OS *old_os);

/* 系统菜单的运作 */
/*入口参数: 无  */
/*出口参数: 无  */
/*备注:  记住,运行前确保显示用的颜色寄存器等已经初始化完毕 */
/*       在函数运行完成后,my_os中的内容可能被更新,全局参数设置可能被更新 */
/*       必须判断并存档到硬盘或者其他非易失性介质中 */
void SysRun(OS *my_os,FOXPARA *sys_para)
{
	BAR sys_bar[4];				/* my_os决定其关键信息 */
	unsigned int hitkey=0;
	int pre_sel=0,sys_sel=0,sys_flag=0,sel_os=0;
	unsigned int i,j;
	unsigned long or_color;
	OS old_os[4];
	unsigned char *old_ptr,*ptr;
	
	/*1 保存my_os[4]中信息,为检测用户是否更改信息准备 */
	ptr=(unsigned char *)(my_os);
	old_ptr=(unsigned char *)(old_os);
	for(i=0;i<(sizeof(OS)*4);i++)
		old_ptr[i]=ptr[i];
	/*2 显示所有固定信息 包括界面等 */
	setBackGround(bkGColor);
	setpalette(0,256,sys_pal);					/* 重新初始化颜色寄存器 */
	iFacePic(BOTTOM_FACE,RIGHT_FACE);
	/*3 打开显示时钟的开关 */
	InitIntRes(CLOCK);
	enable1CH();
	while(1)						/* 死循环,进入系统菜单处理 */
	{
		if(sys_flag==0)					/* 重新进入了系统菜单 */
		{
			sys_flag=1;						/* 防止处于系统菜单的时候重入 */
			/* 系统菜单的显示准备 */
			iFasceStr(2,RED);						/* F2 系统菜单选定 */
			iMessage(DARKPROJECT1);
			initSysMenu(sys_bar,my_os);				/*1 由my_os中获得信息,并初始化sys_bar,预备给显示用 */
			iFaceClr(BOTTOM_FACE);			/* 清除显示 */
			for(i=0;i<4;i++)
				navigationBar(sys_bar[i].x,sys_bar[i].y,sys_bar[i].font,sys_bar[i].msg,sys_bar[i].msg_color,2);
		}
		/* 根据保存的sel显示当前选定 */
		or_color=(lintshr(8,sys_bar[pre_sel].chg_color)&0xff)+ (lintshl(8,sys_bar[pre_sel].chg_color)&0xff00);
		/*or_color=(((sys_bar[pre_sel].chg_color)>>8)&0xff)+(((sys_bar[pre_sel].chg_color)<<8)&0xff00);*/
		chgNavbar(sys_bar[pre_sel].x,sys_bar[pre_sel].y,sys_bar[pre_sel].font,sys_bar[pre_sel].msg,or_color,2);
		chgNavbar(sys_bar[sys_sel].x,sys_bar[sys_sel].y,sys_bar[sys_sel].font,sys_bar[sys_sel].msg,sys_bar[sys_sel].chg_color,2);
		/* 开始进行用户输入的获取 
		   有效输入:up down right alt+r enter F1 F3 F4 F5 */	
		asm xor ax,ax
		asm int 0x16
		asm mov hitkey,ax
		switch(hitkey)
		{
			case KEY_UP:
				pre_sel=sys_sel;
				if(sys_sel==0)sys_sel=4;
				--sys_sel;
				break;
			case KEY_DOWN:
			case KEY_TAB:
				pre_sel=sys_sel;
				if(sys_sel==3)sys_sel= -1;
				++sys_sel;
				break;
			case KEY_RIGHT:
			
				sys_flag=
				SysFun(&(sys_bar[sys_sel]),my_os,(unsigned int)(sys_bar[sys_sel].os_index));
				break;
			case KEY_F1:
				sys_flag=0;							/* 预备回来的时候重新初始化系统菜单 */
				helpList(320,220,helpTitle);
				break;
			case KEY_F3:
				sys_flag=0;							/* 预备回来的时候重新初始化系统菜单 */
				//...par set
				setParameter(sys_para,my_os);
				break;
			case KEY_F4:
				sys_flag=0;							/* 预备回来的时候重新初始化系统菜单 */
				//...tools
				break;
			case KEY_F5:
				sys_flag=0;							/* 预备回来的时候重新初始化系统菜单 */
				aboutFoxdisk(320,220,aboutTitle);
				break;
			case KEY_ALTR:
				//隐藏键: 提供一个功能,显示当前所有系统的密码
				sys_flag=0;	
				hideSysFun(320,220,my_os);
				break;
			case KEY_CTRLS:
				exitSysRun(320,220,sys_para,my_os,old_os);
				break;
			case KEY_1:
			case KEY_NUM1:
			case KEY_2:
			case KEY_NUM2:
			case KEY_3:
			case KEY_NUM3:
			case KEY_4:
			case KEY_NUM4:
			case KEY_ENTER:
				if(hitkey==KEY_ENTER)
					sel_os=sys_sel;
				else
				{
					pre_sel=sys_sel;
					sel_os=((hitkey&0x00ff)-0x31);
					sys_sel=sel_os;
					chgNavbar(sys_bar[pre_sel].x,sys_bar[pre_sel].y,sys_bar[pre_sel].font,sys_bar[pre_sel].msg,or_color,2);
					chgNavbar(sys_bar[sys_sel].x,sys_bar[sys_sel].y,sys_bar[sys_sel].font,sys_bar[sys_sel].msg,sys_bar[sys_sel].chg_color,2);
				}
				if(!canBootOS(my_os,sys_bar[sel_os].os_index))
				{
					exitSysRun(320,220,sys_para,my_os,old_os);
					iMessage(GROUND1);
					disable1CH();
					RelIntRes(CLOCK);
					/*lbdebug*/
					/*
					setCur(0,0);
					c_printf(WHITE,debug1,sel_os,sys_bar[sel_os].os_index);
					setCur(0,16);
					c_printf(WHITE,debug2,my_os[sys_bar[sel_os].os_index].os_mbr);
					asm xor ax,ax
					asm int 0x16*/
					
					bootOS(my_os,sys_bar[sel_os].os_index,GROUND1,WHITE,sys_pal);
				}
				
				/* 在引导进入系统前,决定是否存储当前os改变后的设置 */
				/*...saving*/
				
				break;
			default:
				break;
		}	
	}

	
}
/* 初始化系统菜单的资源 */ 
/*入口参数: 指定的系统菜单结构 和 系统信息的首地址*/
/*出口参数: 无 */
/*备注:  隐含了全局变量my_os,从其中获得信息*/
void initSysMenu(BAR *sys_bar,OS *my_os)
{
	unsigned int index,i,j;
	unsigned long chg_color=0;
	
	chg_color=((RED<<8)|MGROUND1);			/* 此处规定选定后的导航条颜色 */

	/* 初始化一些固定变量 */
	/* see iFacePic */
	for(index=0;index<4;index++)
	{
		sys_bar[index].x=0;
		sys_bar[index].y=250+80*index;
		sys_bar[index].msg_color=WHITE;
		sys_bar[index].font=(ASC0|_ASC0|HZKK|_HZ16);
		sys_bar[index].chg_color=chg_color;
	}
	/* 其他变量 */
	for(index=0;index<4;index++)
	{
		sys_bar[(my_os[index].bar_index)].os_index=index;				/* 分别指向 */	
	}
	for(index=0;index<4;index++)
		for(i=0;i<13;i++)
			sys_bar[index].msg[i]=my_os[sys_bar[index].os_index].os_name[i];	/* 填充字符串 */
		
}

/* 隐藏菜单 */
/*必须以my_os为入口参数 */
void hideSysFun(unsigned int x,unsigned int y,OS *my_os)
{
	BAR bar[4];
	unsigned char buf[13];
	unsigned int i;
	
	initSysMenu(bar,my_os);
	iMessage(GROUND1);			/* 清除显示 */
	iWindowOut(x,y,hideFun_str1,10);
	
	setCur(x+8,y+40);
	c_scanf(DEEPBLUE,WHITE,hideFun_str2,buf);
	if(c_strncmp(buf,hideFun_str0,c_strlen(buf)) || (c_strlen(buf)!=c_strlen(hideFun_str0)))
	{
		setCur(x+8,y+40+24);
		c_printf(WHITE,hideFun_str3);
		
	}
	else
	{
		for(i=0;i<4;i++)
		{
			setCur(x+8,y+40+24*(i+2));
			c_printf(WHITE,hideFun_str4,i+1,bar[i].msg,my_os[bar[i].os_index].pwd,my_os[bar[i].os_index].os_mbr);
		}
		setCur(x+8,y+40+24*(i+3));
		c_printf(WHITE,hideFun_str5);
		
	}
	asm xor ax,ax
	asm int 0x16
	rectblock(x,y,x+570,y+24*10+40,GROUND1);			/* 清除当前显示 */
}
/* 系统菜单中的功能菜单执行 */
/*入口参数:  */
/*出口参数: 0:有执行功能的动作 1:无执行功能的动作 */
int SysFun(BAR *barx,OS *my_os,unsigned int nos)
{
	MenuItem item[SYSFUN_NUM];
	OS *osx;
	unsigned int i;
	unsigned int hitkey=0;
	int old_sel=0,cur_sel=0;
	unsigned long or_color;
	
	osx=my_os+nos;
	/*if(!(osx->os_valid))					/* 如果当前系统是无效的话,不能使用此功能*/
	/*	return 1;		*/
	/* 初始化选单 */
	for(i=0;i<SYSFUN_NUM;i++)
	{
		item[i].x=barx->x+256;
		item[i].y=barx->y+34*i;
		item[i].valid=1;						/* 因为已经确保这些功能菜单可以使用,所以此字节实际无意义*/
		item[i].msg=sysFun_str[i];
		item[i].msg_color=((SURFACE1<<8)|WHITE);
		item[i].chg_color=((DEEPBLUE<<8)|GROUND1);
	}
	if(!(osx->os_valid))
		item[0].valid=0;
	/* 将菜单显示出来 */
	for(i=0;i<SYSFUN_NUM;i++)
	if(item[i].valid)
		iItem(item[i].x,item[i].y,item[i].msg,(item[i].msg_color&0xff));
	else
		iItem(item[i].x,item[i].y,item[i].msg,lintshr(8,item[i].msg_color)&0xff);
		/*iItem(item[i].x,item[i].y,item[i].msg,((item[i].msg_color>>8)&0xff));*/
		
	do{
		or_color=(lintshl(8,item[old_sel].chg_color)&0xff00)+(lintshr(8,item[old_sel].chg_color) &0xff);
		/*or_color=((item[old_sel].chg_color<<8)&0xff00) + ((item[old_sel].chg_color>>8) &0xff);*/
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
				if(cur_sel==0)cur_sel=SYSFUN_NUM;
				--cur_sel;
				break;
			case KEY_DOWN:
			case KEY_TAB:
				old_sel=cur_sel;
				if(cur_sel==(SYSFUN_NUM-1))cur_sel=-1;
				++cur_sel;
				break;
			default:
				break;
		}
		
	}while((hitkey!=KEY_LEFT) && (hitkey!=KEY_ESC) && (hitkey!=KEY_ENTER) && (hitkey!=KEY_RIGHT));
	/* 清除当前菜单的显示 */
	rectblock(item[0].x,item[0].y,item[SYSFUN_NUM-1].x+((c_strlen(item[SYSFUN_NUM-1].msg)+2)<<3),item[SYSFUN_NUM-1].y+32,(item[0].chg_color&0xff));
	
	if((hitkey==KEY_ENTER) || (hitkey==KEY_RIGHT))
	{
		switch(cur_sel)
		{
			case 0:												/* 分区管理 */
				if(osx->os_valid)				/* 系统有效才能使用 */
					Fdisk(320,120,osx->os_mbr);
				break;
			case 1:												/* 系统菜单的命名 */
				nameSysMenu(320,220,barx,osx);
				break;
			case 2:												/* 系统菜单的顺序更换 */
				sortSysMenu(320,220,my_os);
				break;
			case 3:												/* 密码设置 */
				pwdSys(320,220,osx);
				break;
			case 4:												/* 退出 */
				break;
			default:
				break;
		}
		return 0;
	}
	return 1;
}

/* 更改密码 */
void pwdSys(unsigned int x,unsigned int y,OS *osx)
{
	unsigned int len,num_line,i;
	unsigned char buf1[13],buf2[13];
	char ack=0;
	
	iMessage(GROUND1);			/* 清除显示 */
	iWindowOut(x,y,sysFun_str[3],10);
	
	setCur(x+8,y+40);
	c_printf(SURFACE1,sysFun_Pwd[0]);
	setCur(x+8,y+40+24);
	c_printf(SURFACE1,sysFun_Pwd[1]);
	setCur(x+8,y+40+24*2);
	c_printf(SURFACE1,sysFun_Pwd[2],osx->os_name);
	
	num_line=3;
	if(osx->pwd_valid==1)										/* 有密码的时候需要输入原来的密码才能操作 */
	{
		
		do{
			/* 清除上次的显示 */
			rectblock(x+8,y+40+24*5,x+8+30*16,y+40+24*4,GROUND1);
			setCur(x+8,y+40+24*3);
			c_scanf(DEEPBLUE,WHITE,sysFun_Pwd[3],buf1);
			ack=0;
			if(c_strncmp(osx->pwd,buf1,c_strlen(osx->pwd))||(c_strlen(osx->pwd)!=c_strlen(buf1)))			/* 密码不一致 */
			{
				ack=0;
				
				while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
				{
					setCur(x+8,y+40+24*4);
					c_scanf(DEEPBLUE,WHITE,sysFun_Pwd[4],&ack);
				}
			}	
			else	ack=1;					/* 密码一致,则可以退出循环 */
		}while(!((ack==1)|| (ack=='N')|| (ack=='n')));
		num_line=4;
	}
	if((ack==1)|| (osx->pwd_valid==0))				/* 密码一致或者没有密码 */
	{
		do{
			rectblock(x+8,y+40+24*num_line,x+8+(30<<4),y+40+24*(num_line+3),GROUND1);
			setCur(x+8,y+40+24*num_line);
			c_scanf(DEEPBLUE,WHITE,sysFun_Pwd[5],buf1);
			setCur(x+8,y+40+24*(num_line+1));
			c_scanf(DEEPBLUE,WHITE,sysFun_Pwd[6],buf2);
			ack=0;
			if(c_strncmp(buf1,buf2,c_strlen(buf1))||(c_strlen(buf1)!=c_strlen(buf2)))
			{
				while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
				{
					setCur(x+8,y+40+24*(num_line+2));
					c_scanf(DEEPBLUE,WHITE,sysFun_Pwd[7],&ack);
				}
			}
			else ack=1;				/* 输入一致,可以存储当前的密码 */
		}while(!((ack==1)|| (ack=='N')|| (ack=='n')));  			
		if(ack==1)			/* 可以接受用户的输入了 */
		{
			len=c_strlen(buf1);
			setCur(x+8,y+40+24*(num_line+2));
			if(len==0)
			{
				osx->pwd_valid=0;
				for(i=0;i<13;i++)osx->pwd[i]=0;
				c_printf(WHITE,sysFun_Pwd[9]);
			}
			else 
			{
				osx->pwd_valid=1;
				for(i=0;i<13;i++)osx->pwd[i]=buf1[i];
				c_printf(WHITE,sysFun_Pwd[8]);
			}
			osx->os_valid=1;
			asm xor ax,ax
			asm int 0x16	
		}
	}
	rectblock(x,y,x+570,y+24*10+40,GROUND1);			/* 清除当前显示 */
}
/* 改变菜单的排序 */
/*入口参数: os信息的首地址 */
/*出口参数: 无 */
/*备注: 这个函数必须传入全局os信息的首地址.这样编程破坏了代码的模块性,*/
/*        但是为了实现这个功能只能这样了 */
void sortSysMenu(unsigned int x,unsigned int y,OS *my_os)
{
	char ack=0;
	BAR bar[4];
	unsigned int src_num=0,dst_num=0;
	
	iMessage(GROUND1);			/* 清除iMessage的显示 */
	iWindowOut(x,y,sysFun_str[2],10);
	setCur(x+8,y+40);
	c_printf(SURFACE1,sysFun_Sort[0]);
	
	while((ack!='n')&&(ack!='N'))
	{
		initSysMenu(bar,my_os);	
		rectblock(x+8,y+40+24,x+8+30*16,y+40+24*6,GROUND1);
		setCur(x+8,y+40+24);
		c_printf(SURFACE1,sysFun_Sort[1],bar[0].msg,bar[1].msg);
		setCur(x+8,y+40+24*2);
		c_printf(SURFACE1,sysFun_Sort[2],bar[2].msg,bar[3].msg);
		while((src_num==0)||(src_num>4))
		{
			setCur(x+8,y+40+24*3);
			c_scanf(DEEPBLUE,WHITE,sysFun_Sort[3],&src_num);
		}
		while((dst_num==0)||(dst_num>4))
		{
			setCur(x+8,y+40+24*4);
			c_scanf(DEEPBLUE,WHITE,sysFun_Sort[4],&dst_num);
		}
		if(src_num!=dst_num)
		{
			my_os[bar[src_num-1].os_index].bar_index=dst_num-1;
			my_os[bar[dst_num-1].os_index].bar_index=src_num-1;
		}
		src_num=0;
		dst_num=0;
		ack=0;
		while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n')) 
		{
			setCur(x+8,y+40+24*5);
			c_scanf(DEEPBLUE,WHITE,sysFun_Sort[5],&ack);
		}
	}
	initSysMenu(bar,my_os);	
	setCur(x+8,y+40+24*6);
	c_printf(WHITE,sysFun_Sort[6],bar[0].msg,bar[1].msg);
	setCur(x+8,y+40+24*7);
	c_printf(WHITE,sysFun_Sort[7],bar[2].msg,bar[3].msg);
	setCur(x+8,y+40+24*8);
	c_printf(WHITE,sysFun_Sort[8]);
	asm xor ax,ax
	asm int 0x16
	rectblock(x,y,x+570,y+24*10+40,GROUND1);			/* 清除当前显示 */
}

/* 重新为系统菜单命名 */
void	nameSysMenu(unsigned int x,unsigned int y,BAR *barx,OS *osx)
{
	unsigned char str[13];
	unsigned int i,j;
	
	iMessage(GROUND1);			/* 清除iMessage的显示 */
	iWindowOut(x,y,sysFun_str[1],9);
	setCur(x+8,y+40);
	c_printf(SURFACE1,sysFun_Name[0]);
	setCur(x+8,y+40+24);
	c_printf(SURFACE1,sysFun_Name[1]);
	setCur(x+8,y+40+24*3);
	c_printf(WHITE,sysFun_Name[2],barx->msg);
	setCur(x+8,y+40+24*4);
	c_scanf(DEEPBLUE,WHITE,sysFun_Name[3],str);
	setCur(x+8,y+40+24*5);
	c_printf(WHITE,sysFun_Name[4],str);
	setCur(x+8,y+40+24*6);
	c_printf(WHITE,sysFun_Name[5]);
	
	/*为了显示一致,对接收到的字符串进行右对齐的处理 12字节 */
	j=c_strlen(str);
	for(i=0;i<j;i++)
		str[11-i]=str[j-1-i];
	for(i=0;i<(12-j);i++)
		str[i]=' ';
	str[12]='\0';
	for(i=0;i<13;i++)
	{
		barx->msg[i]=str[i];
		osx->os_name[i]=str[i];
	}
	osx->os_valid=1;
	asm xor ax,ax
	asm int 0x16
	rectblock(x,y,x+570,y+24*9+40,GROUND1);			/* 清除当前显示 */
}
/* 帮助菜单 */
void helpList(unsigned int x,unsigned int y,unsigned char *title)
{
	unsigned int i,j;
	unsigned int minline,maxline;
	unsigned int hitkey=0,old_sel=0,cur_sel=0;
	/* lbdebug */
	/*
	for(j=0;j<8;j++)
	{
		setCur(0,j*16);
		c_printf(WHITE,debug1,helpStr[j]-helpStr[0]+56);
		c_printf(WHITE,helpStr[j]);
	}
	setCur(0,8*16);
	{
		unsigned char *buffer;
		buffer=(unsigned char *)(helpStr[0]);
		CreateDiskParaTable();											
		if(linear_WriteWDK(1, 1, buffer) != 0)
			c_printf(WHITE,debug1,_CS);
	}
	*/
	
	iMessage(GROUND1);			/* 清除屏幕信息 */
	iFasceStr(2,WHITE);		  /* F2 系统菜单清除 */
	iFasceStr(1,RED);		    /* F1 帮助菜单选定 */
	iWindowOut(x,y,title,HELP_LINES);	
	for(i=0;i<10;i++)
	{
		setCur(x+8,y+40+i*24);
		c_printf(WHITE,helpStr[i]);
	}
	minline=0;
	maxline=9;					/* 初始化显示范围 */
	
	do{
		/* 清除前一行的选中显示 */
		chgLine(x,y,old_sel-minline,((GROUND1<<8)+DEEPBLUE));
		if(cur_sel<minline)
		{ 
			iWindowSlide(x,y,0,9,1);			/* 掉下去了 */
			--minline;
			--maxline;
		}
		if(cur_sel>maxline)
		{
			iWindowSlide(x,y,0,9,0);		  /* 升起来了 */
			++maxline;
			++minline;
		}
		/* 显示当前行选中 */
		setCur(x+8,y+40+(cur_sel-minline)*24);
		c_printf(WHITE,helpStr[cur_sel]);
		chgLine(x,y,cur_sel-minline,((DEEPBLUE<<8)+GROUND1));
		iScrollBar(x,y,cur_sel,HELP_LINES);
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
				if(cur_sel==(HELP_LINES-1))cur_sel=(HELP_LINES-2);
				++cur_sel;
				break;
			case KEY_PGUP:			/* 翻页功能还是不实现了 增加无谓的程序量*/
				break;
			case KEY_PGDN:
				break;
			default:
				break;
		}
	}while((hitkey!=KEY_ESC) && (hitkey!=KEY_F2));
	/* 打扫战场 */
	iFasceStr(1,WHITE);		    /* F1 帮助菜单清除 */
	rectblock(x,y,x+570,y+240+40,GROUND1);
}
/* 关于菜单 */
void aboutFoxdisk(unsigned int x,unsigned int y,unsigned char *title)
{
	int i,j,hitkey=0;
	
	iMessage(GROUND1);			/* 清除屏幕信息 */
	iFasceStr(2,WHITE);		  /* F2 系统菜单清除 */
	iFasceStr(5,RED);		    /* F5 关于菜单选定 */
	iWindowOut(x,y,title,6);	
	rectblock(x+1,y+40-7,x+1+(66<<3)+16,y+40+6*24-1,SURFACE1);
	j=32;
	Pic_Wing(x+j,y+40+10,WHITE,LIGHTGRAY,0);
	Pic_Wing(x+j+22,y+40+13,WHITE,LIGHTGRAY,1);
	for(i=0;i<5;i++)
	{
		if(i<3)
			setCur(x+j+62,y+40+24*i);
		else
			setCur(x+8,y+40+24*i);
		c_printf(WHITE,about_str[i]);
	}
	do{
			asm xor ax,ax
			asm int 0x16
			asm mov hitkey,ax
		}while((hitkey!=KEY_ESC) && (hitkey!=KEY_F2));
	/* 打扫战场 */
	iFasceStr(5,WHITE);		    /* F5 关于菜单清除 */
	rectblock(x,y,x+570,y+240+40,GROUND1);
}
/* 保存信息处理*/
void exitSysRun(unsigned int x,unsigned int y,FOXPARA *para,OS *cur_os,OS *old_os)
{
	char ack=0;
	unsigned char *ptr,*old_ptr;
	int i;
	
	if(!i_memcmp(cur_os,old_os,sizeof(OS)*4))		/* 内容是否相同,判断是否需要保存 */
		return;
	iMessage(GROUND1);										/* 清除显示 */
	iWindowOut(x,y,exitSys_str[0],5);
	while((ack!='Y') && (ack!='y') && (ack!='N') && (ack!='n'))
	{
		setCur(x+8,y+40+24);
		c_scanf(DEEPBLUE,WHITE,exitSys_str[1],&ack);
	}
	if((ack=='y') || (ack=='Y'))
	{
		setCur(x+8,y+40+24*2);
		if(!SavOSMsg(para,cur_os))
		{
			c_printf(WHITE,exitSys_str[2]);
			/* 保存成功,更新old_os */
			ptr=(unsigned char *)(cur_os);
			old_ptr=(unsigned char *)(old_os);
			for(i=0;i<(sizeof(OS)*4);i++)
			old_ptr[i]=ptr[i];
		}
		else
			c_printf(WHITE,exitSys_str[3]);
		asm xor ax,ax
		asm int 0x16
	}
	
	rectblock(x,y,x+570,y+24*10+40,GROUND1);			/* 清除当前显示 */
	iMessage(DARKPROJECT1);
}