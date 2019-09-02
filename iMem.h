#ifndef _IMEM_H
#define _IMEM_H

extern void * imemcpy(void * dest,const void *src,unsigned int count) ;
extern int imemcmp(const void * cs,const void * ct,unsigned int count) ;
extern void * imemset(void * s,int ch,unsigned int count);

#endif