;loader.asm: foxdisk的引导代码
RETRY_TIMES  EQU      5	
CODE_SEG     EQU      7000h
DATA_SEG     EQU      8000h			
	
          .model small,c
          .386
          public beg_load, end_load
					extrn	 gFoxdisk: byte,BootEntry: near ptr byte  ;BootEntry为程序入口,见foxdisk.c
					;extrn iFoxVer: word
          .code

beg_load	proc	near
          jmp     near ptr load_start                                
sig       db     'LUOB'
nCodeSect db      0     ; code size.    inited in setup.c 			           
nDataSect db      0     ; data size.     inited in setup.c                                                                              
iDisk     db      0 		; disk which the image on.80|81  inited in setup.c
iBegin    dd      1     ; start sector of image.  inited in setup.c
                                         
iCyl      dw      0                                               
iHead     db      0                                              
iSect     db      0           
maxSect   db      0
maxHead   db      0

pack			db 			16,15 dup(0)
	
load_start:
			    mov  ax, 07C0h			;
          mov  ds, ax         
          cli
          mov  ss, ax					;setup stack
          mov  sp, 0
          sti

          
          mov  dl, ds:[iDisk - beg_load]     ;get disk                         
          push dx
          push dx
          xor  ax, ax
          int  13h

          pop  dx
          mov  ah, 8
          int  13h
          mov  ds:[maxHead - beg_load], dh	;store max head

          pop  dx
          jc   error                                       

          and  cl, 3Fh
          mov  ds:[maxSect - beg_load], cl	;store max sector
          
          mov  dl,byte ptr ds:[iDisk - beg_load]
          mov	 ax,4100h
          mov	 bx,55aah
          int	 13h
          jc	 load_chs
          cmp  bx,0aa55h		;check extend int 13
          jnz	 load_chs
          
   				mov  ax, DATA_SEG
          mov  es, ax 
          lea  bx, gFoxdisk	;load address,see global.c

   				;now we setup parameters to get data.
					mov	 si,pack - beg_load
					mov	 word ptr ds:[si+4],bx
					mov	 word ptr ds:[si+6],es
					mov	 eax,ds:[iBegin - beg_load]
					mov	 dword ptr ds:[si+8],eax
					xor	 eax,eax
					mov	 al, byte ptr ds:[nDataSect - beg_load]
					mov	 byte ptr ds:[si+2],al
					push eax
					mov	 dl,byte ptr ds:[iDisk - beg_load]
					mov	 ah,42h
					int	 13h
					jc	 error
					;call CheckData			;check valid data
					
					;now setup parameters to get code.
					pop	 eax
					add	 dword ptr ds:[si+8],eax
			    mov  al, byte ptr ds:[nCodeSect - beg_load]        
          mov  byte ptr ds:[si + 2], al 
          lea	 bx,BootEntry
          mov  word ptr ds:[si + 4], bx         
          mov  word ptr ds:[si + 6], CODE_SEG  
          mov  ah, 42h
          int  13h
          jc	 error				;error ? yes,jmp.
          jmp	 load_ok			;no 		          
load_chs:									;get data and code with int13(chs mode).
   				mov  ax, DATA_SEG
          mov  es, ax 
          lea  bx, gFoxdisk
				  mov  di,DATA_SEG
					mov  si,nDataSect - beg_load
				  mov	 eax,dword ptr ds:[iBegin - beg_load]
rr:			 	  
				  push eax
				  xor	 edx,edx
				  xor	 ecx,ecx
				  mov	 cl,maxSect		;
				  div	 ecx
				  push dx
				  xor	 edx,edx
				  mov  cl,maxHead		;
				  div  ecx
				  pop  cx
				  push ax
				  shr  ax,2
				  and	 al,0c0h
				  or	 al,cl
				  pop  ax
				  mov  ch,al
				  mov  dl,byte ptr ds:[iDisk - beg_load]
				  mov	 es,di
				  mov  al,byte ptr ds:[si]
				  mov  ah,02h
				  int  13h
				  jc   error
				  pop  eax
				  cmp	 eax,dword ptr ds:[iBegin - beg_load]
				  jnz	 load_ok		;error? no,jmp.
				  ;push eax
				  ;call CheckData
					;pop  eax
				  xor	 ecx,ecx
				  mov  cl,ds:[nDataSect - beg_load]
				  add  eax,ecx
				  mov	 di,CODE_SEG
				  mov  si,nCodeSect - beg_load
	        lea	 bx,BootEntry
          mov  word ptr ds:[si + 4], bx    
				  jmp  rr
load_ok:
          mov  ax, DATA_SEG
          mov  ds, ax
          mov  es, ax
          cli
          mov  ss, ax
          mov  sp, 0
          sti
          push 7c0h
          push i_am_back - beg_load
          push CODE_SEG
          push bx
          retf				;jmp CODE_SEG:BX Bootentry的入口， sp+4
i_am_back:
					int	19h
					int	18h          

error:
					mov	si,msg - beg_load
					cld
error_loop:			
					lodsb
					or	al,al
					jz	$
					mov	ah,0eh
					mov	bx,7
					int	10h
					jmp	error_loop
				

msg  			DB	'Loading Foxdisk System Failed!'
          db  07h, 0Ah, 0Dh, 0

beg_load	endp    
end_load:	                              
          END
				
				
				