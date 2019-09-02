#ifndef _BOOTOS_H
#define _BOOTOS_H

void bootOS(OS *os,int nos,unsigned long bg,unsigned long fore,SvgaDAC *pal);;
void bootAbExit(void);
int canBootOS(OS *os,int nos);
#endif