#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>

extern unsigned long lintmul(unsigned long m1,unsigned long m2);
extern unsigned long lintmod(unsigned long divisor,unsigned long dividend);
extern unsigned long lintdiv(unsigned long divisor,unsigned long dividend);
void main()
{
	unsigned long x1,x2;
	unsigned int x3;
	x1=50000;
	x2=900;
	x3=3;
	printf("%ld\n",x1%x2);
	printf("%ld\n",lintmod(x2,x1));
	printf("%ld\n",x1/x3);
	printf("%ld\n",lintdiv(x3,x1));
	
	x1=x1+x2;
	x1=x1-x2;
}