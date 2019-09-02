#ifndef _SETUP_H
#define _SETUP_H

#define SETUPMBR  		0x01
#define SETUPFOX  		0x02
#define WRITEDISK_ERR 0x04 
#define READDISK_ERR 	0x08 
	
typedef	struct	loadBootPk{
				unsigned char		jmpCode[3];	
				unsigned char		sig[4];
				unsigned char		nCodeSect;
				unsigned char		nDataSect;
				unsigned char		iDisk;
				unsigned long		iBegin;
				unsigned char		res[512-3-4-7-2];
				unsigned int		magic;		
}BootPk;

extern void	MoveData(unsigned int dstSeg,unsigned int dstOff,unsigned int srcSeg,unsigned int srcOff,unsigned int n);
extern int savWallPaper(iWallPaper *wp,int nwp,char *file);
extern int saviFoxPic(iWallPaper *wp,int nwp,char *file);
extern int Check_Bmp(bmp_picture_ptr bmp_ptr);
extern int	Setup(unsigned long	addr,unsigned int flag);
extern void firstSetup(void);
extern void initKeyData(FOXPARA *para,OS *os);
extern int dirWallPaper(unsigned char *file_array);

#endif