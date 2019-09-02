#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>

extern unsigned long lintmul(unsigned long m1,unsigned long m2);
extern unsigned long lintmod(unsigned long divisor,unsigned long dividend);
extern unsigned long lintdiv(unsigned long divisor,unsigned long dividend);
extern unsigned long lintshr(unsigned int n,unsigned long number);
extern unsigned long lintshl(unsigned int n,unsigned long number);

void main()
{
	unsigned long x1,x2;
	unsigned int x3;
	x1=0xffffffff;
	printf("x1:32 %lx %lx\n",lintshl(32,x1),x1<<32);
	printf("x1:30 %lx %lx\n",lintshl(30,x1),x1<<30);
	printf("x1:24 %lx %lx\n",lintshl(24,x1),x1<<24);
	printf("x1:20 %lx %lx\n",lintshl(20,x1),x1<<20);
	printf("x1:16 %lx %lx\n",lintshl(16,x1),x1<<16);
	printf("x1:8 %lx %lx\n",lintshl(8 ,x1),x1<<8);
	
}