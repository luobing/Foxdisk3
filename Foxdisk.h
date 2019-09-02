#ifndef _FOXDISK_H
#define _FOXDISK_H


#define PARALBAERR 	0x01				/* 存储重要参数信息的扇区被破坏或者无法访问 */
#define PARAADRERR	0x02				/* 当前给出的参数存放地址与参数中保存的地址不匹配 */
#define OSMSGERR		0x04				/* 系统的信息资源被破坏 */
#define WPMSGERR		0x08				/* 壁纸存放的信息被破坏 */

extern void foxdisk(void);
extern int SavFoxPara(FOXPARA *para);
extern int SavOSMsg(FOXPARA *para,OS *os);
extern int i_memcmp(void *cs,void *ct,unsigned int count);
extern void	BootEntry(void);

#endif