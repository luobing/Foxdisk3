#ifndef _IMATH_H
#define _IMATH_H
/* ====iMath==== */
extern unsigned long lintmul(unsigned long m1,unsigned long m2);		/* 长整形乘法 */
extern unsigned long lintmod(unsigned long divisor,unsigned long dividend); /* 除法 */
extern unsigned long lintdiv(unsigned long divisor,unsigned long dividend); /* 模运算 */

extern unsigned long lintshr(unsigned int n,unsigned long number);	/* 右移 */
extern unsigned long lintshl(unsigned int n,unsigned long number);	/* 左移 */
#endif