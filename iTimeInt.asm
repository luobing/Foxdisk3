.model small,c   ;使用了c关键字,注意函数均不用加下划线
.386
extrn _int_Clock:near ptr byte,_int_countDown:near ptr byte,_int_Cursor:near ptr byte
.data

.code
public setNewVect
public retOldVect
public enable1CH
public disable1CH

new1ch:
	pusha
	push	es
	push	ds
	mov	ax,cs:main_proc_ds
	mov	ds,ax
	mov	es,ax

	JMP NEW1CH_MYCODE
  main_proc_ds      DW    ?
  old_int1ch dd     0ffffffffh
  db	'luobingxxxxluobing'
NEW1CH_MYCODE:
	call	near ptr _int_Clock             ;加上near ptr 很重要
	call	near ptr _int_Cursor
	call	near ptr _int_countDown

	pop	ds
	pop	es
	popa
	iret	
;new1ch	PROC	FAR
; 	PUSHA
;  PUSH  DS
;  PUSH  ES
;  
;  JMP NEW1CH_MYCODE
;  main_proc_ds      DW    ?
;  old_int1ch dd     0ffffffffh
;NEW1CH_MYCODE:
;	MOV AX,CS:MAIN_PROC_DS
;  MOV DS,AX
;  MOV ES,AX
;  CLI
;  
;  call	_int_Clock
;  call	_int_countDown
;  call	_int_Cursor
;  
;	STI		
;																				;防止中断被无意关闭，实际调试发现
;  POP ES
;  POP DS
;  POPA
;  IRET 
;new1ch	endp	

;void setNewVect(unsigned int int_vect,unsigned int new_seg,unsigned int new_offs);
; 设置新的中断向量 
; 旧向量保存在cs:old_int1ch中
setNewVect	proc	near
	push	bp
	mov	bp,sp
	push	ax
	push	bx
	push	cx
	push	es
	
	cli
	xor	ax,ax
	mov	es,ax
	mov	al,4
	mov	cx,word ptr [bp+4]
	mul cl
	mov	bx,ax    	;es:[bx] ->int vect
	;save old int
	mov	ax,word ptr es:[bx]
	mov	si,offset old_int1ch
	mov	word ptr cs:[si],ax
	mov	ax,word ptr es:[bx+2]	  
	mov	word ptr cs:[si+2],ax
	;set new int
	mov	ax,word ptr [bp+8]   ;offs
	mov	word ptr es:[bx],ax
	mov	ax,word ptr [bp+6]   ;seg
	mov	word ptr es:[bx+2],ax
	;pass ds value to int1c
	push	ds
	pop	ax
	mov word ptr cs:main_proc_ds,ax
	sti
	pop	es
	pop	cx
	pop	bx
	pop	ax
	mov	sp,bp
	pop	bp
	ret
setNewVect endp

;	unsigned int retOldVect(unsigned int vect)
; 还回以前的向量 
; 0表示设置失败,没有设置新的向量 
retOldVect	proc	near
	push	bp
	mov	bp,sp
	push	bx
	push	es
	push	ecx
	push	si
	cli
	mov	bx,offset old_int1ch
	xor	ax,ax
	mov	ecx,dword ptr cs:[bx]
	cmp	ecx,0ffffffffh
	jz	_retOVexit
	xor	ecx,ecx
	mov	cx,word ptr [bp+4]      ;vect
	mov	es,ax
	mov	al,4
	mul	cl
	mov	si,ax        ;es:si -> vect int
	mov	ax,word ptr cs:[bx]
	mov	word ptr es:[si],ax
	mov	ax,word ptr cs:[bx+2]
	mov	word ptr es:[si+2],ax
	;mov	ecx,0ffffffffh
	;mov	dword ptr cs:[bx],ecx
	mov	word ptr cs:[bx],0ffffh
	mov	word ptr cs:[bx+2],0ffffh
	mov	ax,1
_retOVexit:
	sti
	pop	si
	pop	ecx
	pop	es
	pop	bx
	mov	sp,bp
	pop	bp
	ret	
retOldVect	endp
;unsigned int enable1CH(void)
enable1CH	proc	near
	push	bp
	mov	bp,sp
	push	bx
	push	ecx
	;
	mov	bx,offset old_int1ch
	xor	ax,ax
	mov	ecx,dword ptr cs:[bx]
	cmp	ecx,0ffffffffh
	jnz	_enab1c_exit
	mov	ax,offset new1ch
	;mov	ax,offset i_int
	push	ax
	push	cs
	mov	ax,1ch
	push	ax
	call setNewVect
	add	sp,6
	mov	ax,1
	;
_enab1c_exit:
	pop	ecx
	pop	bx
	mov	sp,bp
	pop	bp
	ret
enable1CH	endp
;unsigned int disable1CH(void)
disable1CH	proc	near
	push	bp
	mov	bp,sp
	;
	mov	ax,1ch
	push	ax
	call	retOldVect
	add	sp,2
	;
	mov sp,bp
	pop	bp
	ret
disable1CH endp
end

