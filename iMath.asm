;为了与平台无关,制作了长整形数的运算函数

.model small
.386p
.data
.code

public _lintmul
public _lintdiv
public _lintmod
public _lintshr     
public _lintshl
;unsigned long lintmul(unsigned long m1,unsigned long m2);
;入口参数:两个长整形数
;出口参数:两数的乘积
;注意:只适合乘积不超过一个长整形数最大值
_lintmul	proc	near
	push	bp
	mov	bp,sp
	;
	mov	eax,dword ptr [bp+4]
	mov	edx,dword ptr [bp+8]
	mul	edx
	mov	edx,eax
	shr	edx,16				;dx:ax返回长整形数
	;
	mov	sp,bp
	pop	bp
	ret
_lintmul	endp

;unsigned long lintdiv(unsigned long divisor,unsigned long dividend);
;入口参数:divisor:除数   dividend:被除数
;出口参数:两数的商
_lintdiv	proc	near
	push	bp
	mov	bp,sp
	push	ecx
	;
	xor	edx,edx
	mov	eax,dword ptr [bp+8]		;被除数
	mov	ecx,dword ptr [bp+4]		;除数
	div	ecx											;商->eax ,余数->edx
	mov	edx,eax
	shr	edx,16				;dx:ax返回长整形数
	
	pop	ecx
	mov	sp,bp
	pop	bp
	ret
_lintdiv	endp

;unsigned long lintmod(unsigned long divisor,unsigned long dividend);
;入口参数:divisor:除数   dividend:被除数
;出口参数:dividend mod divisor
_lintmod	proc	near
	push	bp
	mov	bp,sp
	push	ecx
	;
	xor	edx,edx
	mov	eax,dword ptr [bp+8]		;被除数
	mov	ecx,dword ptr [bp+4]		;除数
	div	ecx											;商->eax ,余数->edx
	mov	ax,dx
	shr	edx,16				;dx:ax返回长整形数
	;	
	pop	ecx
	mov	sp,bp
	pop	bp
	ret
_lintmod	endp

;unsigned long lintshr(unsigned int n,unsigned long number)
;入口参数: n:移位数 number:源数
;出口参数 number >>n
_lintshr	proc	near
	push	bp
	mov bp,sp
	push	cx
	;
	mov eax,dword ptr [bp+6]
	mov cx,word ptr [bp+4]
	mov	ch,0
	shr eax,cl
	mov	edx,eax
	shr	edx,16				;dx:ax返回长整形数
	
	pop	cx
	mov	sp,bp
	pop	bp
	ret
_lintshr endp

;unsigned long lintshr(unsigned int n,unsigned long number)
;入口参数: n:移位数 number:源数
;出口参数 number >>n
_lintshl	proc	near
	push	bp
	mov bp,sp
	push	cx
	;
	mov eax,dword ptr [bp+6]
	mov cx,word ptr [bp+4]
	mov	ch,0
	shl eax,cl
	mov	edx,eax
	shr	edx,16				;dx:ax返回长整形数
	
	pop	cx
	mov	sp,bp
	pop	bp
	ret
_lintshl endp

end