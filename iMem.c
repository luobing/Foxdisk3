/* iMem.c */
#include "iMem.h"

/* 内存拷贝 */
/*注意：不能拷贝超过64K的内存 */
void * imemcpy(void * dest,const void *src,unsigned int count) 
{ 
	unsigned char *tmp = (unsigned char *) dest, *s = (unsigned char *) src; 

	while (count--) 
  	*tmp++ = *s++; 

	return dest; 
} 

/* 内存比较 */
int imemcmp(const void * cs,const void * ct,unsigned int count) 
{ 
	const unsigned char *su1, *su2; 
	int res = 0; 

	for( su1 = (unsigned char *)cs, su2 = (unsigned char *)ct; 0 < count; ++su1, ++su2, count--) 
		if ((res = *su1 - *su2) != 0) 
			break; 

	return res; 
} 
/* 内存填充 */
void * imemset(void * s,int ch,unsigned int count) 
{ 
	unsigned char *xs = (unsigned char *) s; 
	while (count--) 
  	*xs++ = ((unsigned char)(ch)); 
	return s; 
} 


