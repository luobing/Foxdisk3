#ifndef _PAINTER_H
#define _PAINTER_H

extern unsigned int x_cur;
extern unsigned int y_cur;

extern void iFacePic(unsigned int ibottom,unsigned int iright);
extern void	iFasceStr(unsigned char turn,unsigned long color);
extern void	iFaceClr(unsigned int ibottom);
extern void iMessage(unsigned long color);
extern void	navigationBar(unsigned int x,unsigned int y,unsigned long font,unsigned char *s,unsigned long color,unsigned char mag);
extern void chgNavbar(unsigned int x,unsigned int y,unsigned long font,unsigned char *s,unsigned long color,unsigned char mag);

extern void gear(unsigned int cenx,unsigned int ceny,unsigned int radius);
extern void	iItem(unsigned int x,unsigned int y,unsigned char *s,unsigned long color);
extern void iItemSel(unsigned int x,unsigned int y,unsigned char *s,unsigned long color);

extern void	iWindowOut(unsigned int x,unsigned int y,unsigned char *wstr,unsigned int lines);
extern void Pic_Wing(unsigned int x,unsigned int y,unsigned int color_in,unsigned int color_out,unsigned char direct);
extern void iScrollBar(unsigned int winx,unsigned int winy,unsigned int num_line,unsigned int lines);
extern void	iWindowSlide(unsigned int winx,unsigned int winy,unsigned int minline,unsigned int maxline,unsigned char up_down);


extern void c_printf(unsigned long color, const char *format,...);
extern void c_scanf(unsigned long bg_color,unsigned long color,const char *format,...);
extern void chgLine(unsigned int x,unsigned int y,unsigned int num_line,unsigned long color);
extern unsigned int	c_strlen(unsigned char *s);
extern unsigned int	c_strlen_ascii(unsigned char *s);
extern int c_strncmp(unsigned char *str1,unsigned char *str2,unsigned int count);

extern void setCur(unsigned int x,unsigned int y);
extern unsigned int getXCur(void);
extern unsigned int getYCur(void);

#endif