#ifndef _ISETUP_H
#define _ISETUP_H
#define SAVEKEYPARA 0x01
#define SAVEKEYOS   0x02

extern unsigned int isFirSetup(FOXPARA *para);
extern void updateSetup(FOXPARA *para);
#endif