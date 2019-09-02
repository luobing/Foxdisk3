#ifndef _MENUMAIN_H
#define _MENUMAIN_H


#define BOTTOM_FACE	72					/* FacePic¿Îµ◊≤øµƒæ‡¿Î */
#define RIGHT_FACE	64					/* FacePic¿Î”“±ﬂµƒæ‡¿Î */



extern void initSysMenu(BAR *sys_bar,OS *my_os);
extern void SysRun(OS *my_os,FOXPARA *sys_para);
extern void helpList(unsigned int x,unsigned int y,unsigned char *title);
extern int SysFun(BAR *barx,OS *my_os,unsigned int nos);

#endif