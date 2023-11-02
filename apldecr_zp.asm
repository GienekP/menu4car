
.macro GET_COMP_BYTE	
		mwa	#cont	yieldvec
		jmp	(nextbytevec)
cont
.endm

APL_INPUT	equ $f0
APL_OUTPUT	equ $f2
ringbuffer	equ $f2
srcptr		equ $f4
store_y		equ $f6
token		equ $f7
offsetL		equ $f8
offsetH		equ $f9
EBPL		equ $fa
EBPH		equ $fb
bl		equ $fc
yieldvec	equ $fd
nextbytevec	equ $ee



.proc aPL_depack
		lda    #$80
		sta	token
literal		lsr	bl
		jsr	GET_BYTE
write		mwy	#nxt_token	nextbytevec
		jmp	store
nxt_token	jsr	get_token_bit
		bcc	literal                      ; literal     -> 0
		jsr	get_token_bit
		bcc	block                        ; block       -> 10
		jsr	get_token_bit
		bcc	short_block	              ; short block -> 110

single_byte	lsr	bl		           ; single byte -> 111
		lda	#$10
@		pha
		jsr	get_token_bit
		pla
		rol	@
		bcc	@-
		beq	write
		jmp	len01

aPL_done
		sec
		jmp	(yieldvec)

short_block	jsr	GET_BYTE
		lsr	@
		beq	aPL_done
		sta	EBPL
		ldx	#$02
		bcc	@+
		inx
@		sec
		ror	bl
		jmp	len0203

block		jsr	getgamma
		dex	
		lda	bl
		bmi	normalcodepair
		dex
		bne	normalcodepair
		jsr	getgamma
		lda	EBPL
		sta	offsetL
		jmp	lenffff

normalcodepair
		jsr	GET_BYTE
		sta	offsetL
		sta	EBPL
		jsr	getgamma
_ceck7f		lda	offsetL
		bmi	normal1
plus2		inx
		bne	plus1
		iny
plus1		inx
		bne	normal1
		iny
normal1
lenffff		iny
		sec
		ror	bl
		bne	domatch   ;          zawsze

getgamma	lda	#$00
		pha
		lda	#$01
		pha
@		jsr	 get_token_bit
		tsx
		rol	$101,x
		rol	$102,x
		jsr	 get_token_bit
		bcs	 @-
		pla
		tax
		pla
		tay
		rts

get_token_bit	asl	token
		bne	@+
		php
		jsr	GET_BYTE
		plp
		rol	@
		sta	token
@		rts

store		sty	store_y
		ldy	#0
		sta	(ringbuffer),y
		; rts to second adress and
		; fill the continue vector
		; with this static address
		;mwy	#rtsfromthis	nextbytevec
		ldy	store_y
teststop
		inc	ringbuffer
		; Here handler which does something with byte.
		; Even binary load automata may be placed here.
		; this is for testing.
		clc
		jmp	(yieldvec)
rtsfromthis
		rts

len01		ldx	#$01
len0203		ldy	#$00
		sta	offsetL 
		iny

domatch		lda	ringbuffer
		sec
		sbc	offsetL
		sta	srcptr
source		sty	store_y
		ldy	#0
		mwa	#nxt1	nextbytevec
		lda	(srcptr),y
		ldy	store_y
		inc	srcptr
		jmp	store
nxt1
		dex	
		bne	source
		dey	
		bne	source
		jmp	nxt_token

GET_BYTE
		jsr	GET_FROM_CAR
		pha
		;txa
		;pha
		;jsr IncSrc
		inc SRC
		bne @+
		inc SRC+1
		bit SRC+1
		bvc @+
		lda #$A0
		sta SRC+1
		inc BANK
@
		;pla
		;tax
		pla
		rts
.endp

.proc aPL_depack_blk
		lda	#$80
		sta	token
literal		lsr	bl
		jsr	GET_BYTE
write		jsr	store
nxt_token	jsr	get_token_bit
		bcc	literal			; literal	 -> 0
		jsr	get_token_bit
		bcc	block			; block	   -> 10
		jsr	get_token_bit
		bcc	short_block		; short block -> 110

single_byte	lsr	bl			 ; single byte -> 111
		lda	#$10
@		pha
		jsr	get_token_bit
		pla
		rol	@
		bcc	@-
		beq	write
		jmp	len01

aPL_done	rts

short_block	jsr	GET_BYTE
		lsr	@
		beq	aPL_done
		sta	EBPL
		ldx	#0
		stx	EBPH
		ldx	#$02
		bcc	@+
		inx
@		sec
		ror	bl
		jmp	len0203

block		jsr	getgamma
		dex	
		lda	bl
		bmi	normalcodepair
		dex
		bne	normalcodepair
		jsr	getgamma
		lda	EBPL
		sta	offsetL
		lda	EBPH
		sta	offsetH
		jmp	lenffff

normalcodepair	dex
		stx	offsetH
		stx	EBPH
		jsr	GET_BYTE
		sta	offsetL
		sta	EBPL
		jsr	getgamma
		lda	offsetH
		beq	_ceck7f
		cmp	#$7d
		bcs	 plus2
		cmp	#$05
		bcs	 plus1
		bcc	normal1		 ; zawsze
_ceck7f		lda	 offsetL
		bmi	normal1
plus2		inx
		bne	plus1
		iny
plus1		inx
		bne	normal1
		iny
normal1
lenffff		iny
		sec
		ror	bl
		bne	domatch   ;zawsze

getgamma	lda	#$00
		pha
		lda	#$01
		pha
@		jsr	 get_token_bit
		tsx
		rol	$101,x
		rol	$102,x
		jsr	 get_token_bit
		bcs	 @-
		pla
		tax
		pla
		tay
		rts

get_token_bit	asl	token
		bne	@+
		php
		jsr	GET_BYTE
		plp
		rol	@
		sta	token
@		rts

store		sty	store_y
		ldy	#0
		; jsr stack
		sta	(APL_OUTPUT),y
		ldy	store_y
		inw	APL_OUTPUT
		rts

len01		ldx	#$01
len0203		ldy	#$00
		sta	offsetL 
		sty	offsetH
		iny

domatch		lda	APL_OUTPUT
		sec
		sbc	offsetL
		sta	SRCPTR
		lda	APL_OUTPUT+1
		sbc	offsetH
		sta	SRCPTR+1
source		sty	store_y
		ldy	#0
		lda	(SRCPTR),y
		ldy	store_y
		inw	SRCPTR
		jsr	store
		dex	
		bne	source
		dey	
		bne	source
		jmp	nxt_token

GET_BYTE	
		jsr	GET_FROM_CAR
		pha
		;jsr IncSrc
		inc SRC
		bne @+
		inc SRC+1
		bit SRC+1
		bvc @+
		lda #$A0
		sta SRC+1
		inc BANK
@
		pla
		rts
.endp
