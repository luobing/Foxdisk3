#ifndef __BMP__H_
#define __BMP__H_


/*BMP的数据结构 320*200 256色*/
typedef struct BMP_file						/*文件信息区*/
{
	unsigned int bfType;						/*文件类型 "BM"*/
	unsigned long bfSize;						/*文件长度*/
	unsigned int Reserved1;
	unsigned int Reserved2;
	unsigned long bfOffset;					/*文件描述区长度，16色118，256色1024*/
}bitmapfile;	

typedef struct BMP_info						/*图象信息区*/
{
	unsigned long biSize;						/*图形尺寸*/
	unsigned long biWidth;					/*图形宽度*/
	unsigned long biHeight;					/*图形高度*/
	unsigned int biPlanes;
	unsigned int biBitCount;				/*每个象素占二进制位数*/
	unsigned long biCompression;		/*是否压缩格式*/
	unsigned long biSizeImage;
	unsigned long biXpolsPerMeter;
	unsigned long biYpelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
}bitmapinfo;

typedef struct RGB_BMP_typ				/*调色板区*//*256色=256x4byte*/				
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char reserved;
}RGB_BMP,*RGB_BMP_ptr;

typedef struct bmp_picture_typ
{
	bitmapfile file;
	bitmapinfo info;
	RGB_BMP palette[256];
	unsigned	char far *buffer;
} bmp_picture, *bmp_picture_ptr;

#endif