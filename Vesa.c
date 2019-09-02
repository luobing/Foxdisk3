/*luobing 2008-10-13 开始编写*/
#include "Vesa.h"
#include "Global.h"
#include "iMath.h"

signed int getVBEInfo(VBEInfo far *vbeinfo);
signed int getModeInfo(unsigned int mode,ModeInfoBlock far *modeinfo);
void setmode(unsigned int mode);
unsigned int getVBEMode(void);
void setbankA(unsigned int bank);
void setbankAB(unsigned int bank);
unsigned int SetLogicWidth(unsigned int pixels);
signed intSetShowBegin(unsigned int x,unsigned int y);
signed int setpaletteVBE(unsigned int start,unsigned int num,SvgaDAC far *palette);
signed int getpaletteVBE(unsigned int start,unsigned int num,SvgaDAC far *palette);
signed int setpaletteVGA(unsigned int start,unsigned int num,SvgaDAC far *palette);
signed int getpaletteVGA(unsigned int start,unsigned int num,SvgaDAC far *palette);
void	SetWriteMode(unsigned int writemode);
void	putpixel256(unsigned int x,unsigned int y,unsigned long color);
unsigned long	getpixel256(unsigned int x,unsigned int y);

unsigned int initGraphics(unsigned int mode);


/* 		设置画点时所需要的设置模式 	*/
/*入口参数: writemode:写模式      */
/*出口参数: 无										*/
/*说明：四种写模式						    */
unsigned int initGraphics(unsigned int mode)
{
	unsigned int err=0;
	unsigned long i,j;
	
	if(mode<0x100)																	/*1 判断入口参数*/
	{
		err|=INVLIADIN;
		return	err;
	}
	if(!getVBEInfo(&vbeinfo))												/*2 获取 VBE信息 */
	{
		err|=NOTSUPPORTVBE;
		return err;
	}
	if(!getModeInfo(mode,&modeinfo))								/*3 获取当前模式的信息 */
	{
		err|=NOTSUPPORTMODE;
		return err;
	}
	if(!(modeinfo.ModeAttr&0x01))										/*4 判断显示内存是否足够 */
	{
		err|=LIMITVMEM;
		return err;
	}
	oldMode=getVBEMode();														/*5 保存当前显示模式,退出时候还原 */
	setmode(mode);																	/*6 设置SVGA模式 */
	xres=modeinfo.XRes;															/*7 初始化全局变量*/
	yres=modeinfo.YRes;
	bankSwitch=modeinfo.BankFunc;
	bytesperline=modeinfo.BytesPerScanline;
	bytesperpixel=((modeinfo.BitsPerPixel+7)>>3);
	bankShift=0;
	while ((unsigned)(64 >> bankShift) != modeinfo.WinGran)
		bankShift++;
	curBank=0;
	
	j=(unsigned long)(vbeinfo.VRAMMemory<<16);
	i=lintmul((unsigned long)bytesperline,(unsigned long)modeinfo.NumberOfPlanes);
	i=lintmul(i,(unsigned long)yres);
	maxpage=lintdiv(i,j);
	//maxpage=((unsigned long)(vbeinfo.VRAMMemory<<16))/((unsigned long)bytesperline*modeinfo.NumberOfPlanes*yres);
	
	VRAMStartX=VRAMStartY=0;
	if(modeinfo.WinBAttr&0x01)
		setbank=setbankAB;
	else  setbank=setbankA;
	
	if(vbeinfo.VBEVersion>=0x0200)
		setpalette=setpaletteVBE;
	else
		setpalette=setpaletteVGA;	
	getpalette=getpaletteVGA;							/*see vbe3.0 4f09 01功能非强制性执行的,尽量不要使用这个函数*/
	
	switch(modeinfo.BitsPerPixel)
	{
		case 8:
			putpixel=putpixel256;
			getpixel=getpixel256;
				break;
		/* 其他显示在此继续添加 ---luobing */
		default:
			break;
	}
	screenPtr=(char far *)( ((unsigned long)0xA000)<<16 | 0);
	SetWriteMode(PUT);
	return err;
}

/* 		设置画点时所需要的设置模式 	*/
/*入口参数: writemode:写模式      */
/*出口参数: 无										*/
/*说明：四种写模式						    */
void	SetWriteMode(unsigned int writemode)
{
	WriteMode=writemode;
	return;
}
/*          256色写点函数           */
/*入口参数: x,y坐标  color:颜色索引 */
/*出口参数: 无											*/
/*说明：														*/
void	putpixel256(unsigned int x,unsigned int y,unsigned long color)
{
	asm{
		les	di,screenPtr												/*es:di 指向当前位置*/
		mov	ax,y
	#ifdef MULTI_PAGE
		add	ax,VRAMStartY
	#endif
		mov	bx,x
	#ifdef MULTI_PAGE	
		add	bx,VRAMStartX												/*为多页面显示而写*/
	#endif
		mul	word ptr	bytesperline
		add	ax,bx
		adc	dx,0
		add	di,ax
		/*当前结果中dx包含了需要变换的页面*/ 
		cmp	dx,curBank
		je	short	_PUT_
		mov	curBank,dx
	}
	setbank(_DX);														/*当页面未变时将不会进行页面变换处理,见原函数的处理*/
	_PUT_:asm{
		mov	al,byte ptr color
		mov	bx,WriteMode											/*设置写模式*/
		cmp	bx,0
		jne	short XORPUT
		mov	ES:[di],al
		jmp	short	exit
	}
	XORPUT:asm{
		cmp	bx,1
		jne	short ANDPUT
		xor	ES:[di],al
		jmp	short	exit
	}
	ANDPUT:asm{
		cmp	bx,2
		jne	short ORPUT
		and	ES:[di],al
		jmp	short exit
	}
	ORPUT:asm or ES:[di],al
	exit:
}
/*          256色取点函数           */
/*入口参数: x,y坐标  							  */
/*出口参数: 颜色索引								*/
/*说明：														*/
unsigned long	getpixel256(unsigned int x,unsigned int y)
{
	asm{
		les	di,screenPtr
		mov	ax,y
		add	ax,VRAMStartY
		mov	bx,x
		add	bx,VRAMStartX
		mul	word ptr bytesperline
		add	ax,bx
		adc	dx,0
		add	di,ax
		/*当前结果中dx包含了需要变换的页面*/ 
		cmp	dx,curBank
		je	short GET
		mov	curBank,dx
	}
	setbank(_DX);														/*当页面未变时将不会进行页面变换处理,见原函数的处理*/
	GET:
	asm mov	al,ES:[di]
	return(_AL);
}
/*==========================以下为与硬件结合层面的驱动函数(int 10h)=====================================*/
/*=====查询VESA Super VGA信息=====*/
/*入口参数: 无                    */
/*出口参数: 1:调用成功  0:调用失败*/
/*说明：调用int10h  4f00h 功能    */
signed int getVBEInfo(VBEInfo far *vbeinfo)
{
  asm{
    les di,vbeinfo        /*vbeinfo的段址送es,偏移送di*/      
    /*mov dx,seg vbeinfo*/
    /*mov ES,dx*/
    /*lea di,vbeinfo*/
    mov ax,4f00h
    int 10h
  }
  return(_AX==0x4f);
}

/*===== 查询特定显示模式信息 =====*/
/*入口参数: 显示模式              */
/*出口参数: 1:调用成功  0:调用失败*/
/*说明：调用int10h  4f01h 功能    */
signed int getModeInfo(unsigned int mode,ModeInfoBlock far *modeinfo)
{
  asm{
    /*
    mov dx,seg modeinfo
    mov ES,dx
    lea di,modeinfo */
    les di,modeinfo
    mov cx,mode
    mov ax,4f01h
    int 10h
  }
  return(_AX==0x4f);
}

/*=====  设置选定的显示模式  =====*/
/*入口参数: 显示模式              */
/*出口参数: 无                    */
/*说明：    无                    */
void setmode(unsigned int mode)
{
  asm{
    cmp mode,100h
    jae vesamode
    /*如果不是vesa模式,则使用int 10h的0号功能*/
    xor ax,ax
    mov al,byte ptr mode
    int 10h
    jmp short end
  }
  vesamode:asm{
    /*如果是vesa模式,则使用int 10h的4f02h功能*/
    mov ax,4f02h
    mov bx,mode
    int 10h
  }
  end:
}

/*===== 获取Vesa SVGA显示模式=====*/
/*入口参数: 无                    */
/*出口参数: 显示模式              */
/*说明：													*/
unsigned int getVBEMode(void)
{
  asm{
    mov ax,4f03h
    int 10h
    and bx,3fffh
  }
  return(_BX);
}
/*=====   设置新的读写页面   =====*/
/*入口参数: bank                  */
/*出口参数: 无                    */
/*说明：通过显示模式信息决定用哪个函数
全局变量curBank,bankShift,bankSwitch()必须已经准备好*/
void setbankA(unsigned int bank)
{
	/*
  if(curBank==bank)return;
  curBank=bank;*/
  asm{
    push  ds
    mov cx,bankShift
    mov dx,bank
    shl dx,cl
    xor bx,bx
  }
  #ifdef DIRECT_BANKING
    bankSwitch();
  #else
  asm{
    mov ax,4f05h
    int 10h
  }
  #endif
  asm pop ds
}
void setbankAB(unsigned int bank)
{
	/*
  if(curBank==bank)return;
  curBank=bank;*/
  asm{
    push  ds
    mov cx,bankShift
    mov dx,bank
    shl dx,cl
  }
  #ifdef DIRECT_BANKING
    asm push dx
    asm xor bx,bx
    bankSwitch();
    asm pop dx
    asm mov bx,1
    bankSwitch();
  #else
  asm{
    push ds
    xor bx,bx
    mov ax,4f05h
    int 10h
    pop ds
    mov bx,1
    mov ax,4f05h
    int 10h
  }
  #endif
  asm pop ds
}
/*=====     设置逻辑屏宽     =====*/
/*入口参数: pixels:每行的象素数目 */
/*出口参数: 实际的每行象素数      */
/*说明: 4f06h功能,确保bytesperpixel已经被初始化*/
unsigned int SetLogicWidth(unsigned int pixels)
{
  asm{
    mov cx,pixels
    xor bl,bl
    mov ax,04f06h
    int 10h
    /*cx=每行实际的象素数*/
    xor ax,ax
    mov al,bytesperpixel
    mul cx
    mov bytesperline,ax         /*重新更新每行字节数*/ 
  }
  return(_CX);
}

/*===== 设置显示内存起始地址 =====*/
/*入口参数: (x,y)显示内存起始坐标,以象素为单位 */
/*出口参数: 1:成功 0:失败         */
/*说明: 4f07h功能                 */
signed intSetShowBegin(unsigned int x,unsigned int y)
{
  if((vbeinfo.VBEVersion>>8)>=2){
    asm{
      mov bl,0
      /* 查询VBE信息,是否需要等待显示器垂直回扫 */
      mov ax,word ptr vbeinfo.Capabilities
      test ax,4
      jz  short SetIt
      or  bl,080h
      jmp short SetIt
    }
  }
  else{     /* 按VGA处理,等待显示器垂直回扫 */
    asm mov dx,03dah      /* 视频状态端口 */
    WaitDE:asm{           /* 等待显示器允许 */
      in  al,dx
      test al,1
      jnz short WaitDE
    }
    WaitVR:asm{           /* 等待显示器垂直回扫 */
      in  al,dx
      test  al,8
      jz  short WaitVR
      xor bl,bl
    }
  }
  /* 按VGA处理,不等待显示器垂直回扫 */
  SetIt:asm{
    mov cx,x
    mov dx,y
    mov ax,4f07h
    int 10h
  }
  if(_AX==0x4f){
    VRAMStartX=x;
    VRAMStartY=y;
    return 1;
  }
  return 0;
}

/*=====     设置DAC调色板    =====*/
/*入口参数: palette:指向256色调色板数据的远指针*/
/*          start:起始颜色号 num:要设置的颜色数*/
/*出口参数: 1:成功 0:失败                      */
/*说明：4f09h 00功能 VBE2.0以上                */
signed int setpaletteVBE(unsigned int start,unsigned int num,SvgaDAC far *palette)
{
  asm{
    mov bl,0
    /*查询VBE信息,是否需要等待显示器垂直回扫/*/
    mov ax,word ptr vbeinfo.Capabilities
    test ax,4
    jz short NoWaitVR
    or bl,80h
  }
  NoWaitVR:asm{
    mov ax,04f09h
    mov dx,start
    /*计算DI*/
    les di,palette
    mov cx,dx
    shl cx,2
    add di,cx
    mov cx,num
    int 10h
  }
  return(_AX==0x4f);
}
/*=====     读取DAC调色板    =====*/
/*入口参数: palette:指向256色调色板数据的远指针*/
/*          start:起始颜色号 num:要设置的颜色数*/
/*出口参数: 1:成功 0:失败                      */
/*说明：4f09h 01功能 VBE2.0以上                */
signed int getpaletteVBE(unsigned int start,unsigned int num,SvgaDAC far *palette) 
{
  asm{
    mov bl,1
    /*查询VBE信息,是否需要等待显示器垂直回扫/*/
    mov ax,word ptr vbeinfo.Capabilities
    test ax,4
    jz short NoWaitVR
    or bl,80h
  }
  NoWaitVR:asm{
    mov ax,04f09h
    mov dx,start
    /*计算DI*/
    les di,palette
    mov cx,dx
    shl cx,2
    add di,cx
    mov cx,num
    int 10h
  }
  return(_AX==0x4f);
}

/*=====     设置DAC调色板    =====*/
/*入口参数: palette:指向256色调色板数据的远指针*/
/*          start:起始颜色号 num:要设置的颜色数*/
/*出口参数: 1:成功 0:失败                      */
/*说明：VGA通过IO端口设置调色板                */
signed int setpaletteVGA(unsigned int start,unsigned int num,SvgaDAC far *palette)
{
	asm{
		push	ds
		lds	si,palette
		/*等待CRT垂直回扫*/
		/*输入状态寄存器03dah第三位=1表示CRT正在垂直回扫*/
		mov	dx,03dah
	}
	WaitNotVsync:asm{
		in	al,dx
		test	al,8
		jnz	short WaitNotVsync
	}
	WaitVsync:asm{
		in	al,dx
		test	al,8
		jz	short	WaitVsync
		mov	dx,3c8h
		mov	cx,num
		mov	ax,start
		out	dx,al
		inc	dx
		shl	ax,2
		add	si,ax
	}
	nextColor:asm{
		mov	al,[si+2]
		out	dx,al
		mov	al,[si+1]
		out	dx,ax
		mov	al,[si+0]
		out	dx,al
		add	si,4
		loop	short nextColor
		pop	ds
	}
	return 1;
}

/*=====     获取DAC调色板    =====*/
/*入口参数: palette:指向256色调色板数据的远指针*/
/*          start:起始颜色号 num:要设置的颜色数*/
/*出口参数: 1:成功 0:失败                      */
/*说明：VGA通过IO端口设置调色板                */

signed int getpaletteVGA(unsigned int start,unsigned int num,SvgaDAC far *palette)
{
	asm{
		push	ds
		lds	di,palette
		/*等待CRT垂直回扫*/
		/*输入状态寄存器03dah第三位=1表示CRT正在垂直回扫*/
		mov	dx,03dah
	}
	WaitNotVsync:asm{
		in	al,dx
		test	al,8
		jnz	short WaitNotVsync
	}
	WaitVsync:asm{
		in	al,dx
		test	al,8
		jz	short	WaitVsync
		mov	ax,start
		mov	bx,ax
		shl	bx,2
		add	si,bx
		mov	cx,num
	}
	nextColor:asm{
		push	ax
		mov	dx,3c7h
		out	dx,al
		mov	byte ptr [di+3],0
		add	dx,2
		in	al,dx
		mov	[di+2],al
		in	al,dx
		mov	[di+1],al
		in	al,dx
		mov	[di+0],al
		add	di,4
		pop	ax
		inc	ax
		loop	short	nextColor
		pop	ds
	}
	return	1;
}
/*
signed int getpaletteVGA(unsigned int start,unsigned int num,SvgaDAC far *palette)
{
  asm{
    push  ds
    lds di,palette
    mov ax,start
    mov dx,3c7h
    out	dx,al
    mov bx,ax
    shl bx,2
    add di,bx
    mov cx,num
  }
  NextColor:asm{
    push  ax
    mov dx,3c9h
    mov byte ptr [di+3],0 /*保留字节*//*
    in  al,dx
    mov [di+2],al         /*红色*//*
    in  al,dx
    mov [di+1],al         /*绿色*//*
    in  al,dx
    mov [di+0],al         /*蓝色*//*
    pop ax
    inc ax
    loop  short NextColor
    pop ds
  }
  return(1);
}*/