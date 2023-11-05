;-----------------------------------------------------------------------		
;
; Menu4car starter
; (c) 2023 GienekP
;
;-----------------------------------------------------------------------
ALLOC	= ($1d+$c)
BANK    = ($0200-(DTACPYE-GETBYTE)+1)
SRC     = ($0200-(DTACPYE-ADRSRC)+1)
RSRC    = ($0200-(DTACPYE-ADRRSRC)+1)
DST     = ($0200-(DTACPYE-ADRDST)+1)

CBUFFER = ($0200-(DTA256CPYE-CYCL256)+1)
CBUFSRC = ($0200-(DTA256CPYE-SRCP256)+1)

POS     = ($200-ALLOC)
CNT     = ($0201-ALLOC)
nextbytevec = ($0203-ALLOC)
yieldvec = ($0205-ALLOC)
token	= ($0207-ALLOC)
offsetL = ($0208-ALLOC)
offsetH = ($0209-ALLOC)
EBPL    = ($020a-ALLOC)
EBPH    = ($020b-ALLOC)
bl      = ($020c-ALLOC)
GET_FROM_CAR     = ($0200-(DTACPYE-GETBYTE))
PUT_RAM     = ($0200-(DTACPYE-PUTBYTE))
GET_RAM_BYTE     = ($0200-(DTACPYE-GETRBTE))

PUTCB	 = ($0200-(DTA256CPYE-CYCL256))
GETSRCCB = ($0200-(DTA256CPYE-SRCP256))


RUN     = ($0200-(ENTRYE-ADRRUN)+1)
BACK    = ($0200-(ENTRYE-ADRBCK)+1)
ENTRY   = ($0200-(ENTRYE-ENTRYS))

;-----------------------------------------------------------------------


ZPALLOC = $24
TMP     = $00
CASINI  = $02 
WARMST  = $08
BOOTQ   = $09
DOSVEC  = $0A
DOSINI  = $0C

CRITIC  = $42
RAMTOP  = $6A

DMACTLS = $022F
DLPTRS	= $0230
COLDST  = $0244

COLPM0S	= $02C0
COLPM1S	= $02C1
COLPM2S	= $02C2
COLPM3S	= $02C3

COLPF0S	= $02C4
COLPF1S	= $02C5
COLPF2S	= $02C6
COLBAKS	= $02C8
INITAD  = $02E2
RUNAD   = $02E0
MEMTOP  = $02E5
CHBAS   = $02F4
BASICF  = $03F8
GINTLK  = $03FA

TRIG0   = $D010
TRIG1   = $D011
TRIG3   = $D013
COLPF0  = $D016
COLPF1  = $D017
COLPF2  = $D018
COLPF3  = $D019 
COLBAK  = $D01A
CONSOL  = $D01F
KBCODE  = $D209
RANDOM  = $D20A
SKSTAT  = $D20F
PORTA   = $D300
PORTB   = $D301
DMACTL  = $D400
CHBASE  = $D409
WSYNC   = $D40A
VCOUNT  = $D40B
NMIEN   = $D40E

RESETCD	= $E477

;-----------------------------------------------------------------------		
; Standard Bank

		OPT h-f+
		
;
;-----------------------------------------------------------------------
		ORG $A000
screen_data
		.print "#define	SCREENDATA_OFFSET	0x",*-$a000
		:+97 dta d'                                '

;-----------------------------------------------------------------------		
		.print "#define	DATAARRAY_OFFSET	0x",*-$a000
;-----------------------------------------------------------------------		
; Table of files
; BH BL HS LS
table	:+(97) dta 0,0,0,0

;-----------------------------------------------------------------------		
; ANTIC PROGRAM
antic	:+1 dta $70
		dta $4F,<pic,>pic
		:+15 dta $0F
		:+26 dta $02
		dta $41,<antic,>antic
		

;-----------------------------------------------------------------------		
; CTABLE
		.print "#define	COLORTABLE_OFFSET	0x",*-$a000
ctable		dta $06,$16,$26,$36,$46,$56,$66,$76,$86,$96,$a6,$b6,$c6,$d6,$e6,$f6

;-----------------------------------------------------------------------		
; Menu4CAR Logo
		.align $200,$ff
		.print "#define	PICTURE_DATA_OFFSET	0x",*-$a000
pic		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $03, $80, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $1e, $03, $c0, $00, $00, $00, $00
		dta $07, $80, $7d, $8f, $c0, $ff, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $1f, $07, $c0, $00, $00, $00, $00
		dta $07, $80, $ff, $8f, $c0, $ff, $80, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $07, $07, $00, $00, $00, $00, $00
		dta $0d, $81, $c3, $83, $e0, $61, $c0, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $07, $8f, $07, $c1, $de, $0e, $1c
		dta $0d, $83, $81, $83, $60, $60, $c0, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $07, $8f, $1f, $f1, $ff, $0e, $1c
		dta $19, $83, $01, $83, $60, $60, $c0, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $06, $db, $18, $30, $e1, $86, $0c
		dta $19, $83, $00, $06, $30, $61, $c0, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $06, $db, $30, $18, $c1, $86, $0c
		dta $31, $83, $00, $06, $30, $7f, $80, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $06, $73, $3f, $f8, $c1, $86, $0c
		dta $61, $83, $00, $0f, $f8, $7f, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $06, $73, $3f, $f8, $c1, $86, $0c
		dta $7f, $e3, $00, $0f, $f8, $63, $80, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $06, $23, $30, $00, $c1, $86, $0c
		dta $7f, $e3, $81, $98, $0c, $61, $80, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $06, $03, $18, $18, $c1, $86, $1c
		dta $01, $81, $c3, $98, $0c, $61, $c0, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $1f, $cf, $df, $f9, $e3, $c3, $fe
		dta $07, $e0, $ff, $7e, $3f, $fc, $f0, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $1f, $cf, $c7, $e1, $e3, $c1, $ee
		dta $07, $e0, $7e, $7e, $3f, $fc, $70, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00
;-----------------------------------------------------------------------		
;-----------------------------------------------------------------------		
; FONTS
		ORG $B000
		
		.print "#define	FONT_OFFSET	0x",*-$a000
fonts		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $18, $18, $18, $18, $00, $18, $00
		dta $00, $66, $66, $66, $00, $00, $00, $00, $00, $66, $ff, $66, $66, $ff, $66, $00
		dta $18, $3e, $60, $3c, $06, $7c, $18, $00, $00, $66, $6c, $18, $30, $66, $46, $00
		dta $1c, $36, $1c, $38, $6f, $66, $3b, $00, $00, $18, $18, $18, $00, $00, $00, $00
		dta $00, $0e, $1c, $18, $18, $1c, $0e, $00, $00, $70, $38, $18, $18, $38, $70, $00
		dta $00, $66, $3c, $ff, $3c, $66, $00, $00, $00, $18, $18, $7e, $18, $18, $00, $00
		dta $00, $00, $00, $00, $00, $18, $18, $30, $00, $00, $00, $7e, $00, $00, $00, $00
		dta $00, $00, $00, $00, $00, $18, $18, $00, $00, $06, $0c, $18, $30, $60, $40, $00
		dta $00, $3c, $66, $6e, $76, $66, $3c, $00, $00, $18, $38, $18, $18, $18, $7e, $00
		dta $00, $3c, $66, $0c, $18, $30, $7e, $00, $00, $7e, $0c, $18, $0c, $66, $3c, $00
		dta $00, $0c, $1c, $3c, $6c, $7e, $0c, $00, $00, $7e, $60, $7c, $06, $66, $3c, $00
		dta $00, $3c, $60, $7c, $66, $66, $3c, $00, $00, $7e, $06, $0c, $18, $30, $30, $00
		dta $00, $3c, $66, $3c, $66, $66, $3c, $00, $00, $3c, $66, $3e, $06, $0c, $38, $00
		dta $00, $00, $18, $18, $00, $18, $18, $00, $00, $00, $18, $18, $00, $18, $18, $30
		dta $06, $0c, $18, $30, $18, $0c, $06, $00, $00, $00, $7e, $00, $00, $7e, $00, $00
		dta $60, $30, $18, $0c, $18, $30, $60, $00, $00, $3c, $66, $0c, $18, $00, $18, $00
		dta $00, $3c, $66, $6e, $6e, $60, $3e, $00, $00, $18, $3c, $66, $66, $7e, $66, $00
		dta $00, $7c, $66, $7c, $66, $66, $7c, $00, $00, $3c, $66, $60, $60, $66, $3c, $00
		dta $00, $78, $6c, $66, $66, $6c, $78, $00, $00, $7e, $60, $7c, $60, $60, $7e, $00
		dta $00, $7e, $60, $7c, $60, $60, $60, $00, $00, $3e, $60, $60, $6e, $66, $3e, $00
		dta $00, $66, $66, $7e, $66, $66, $66, $00, $00, $7e, $18, $18, $18, $18, $7e, $00
		dta $00, $06, $06, $06, $06, $66, $3c, $00, $00, $66, $6c, $78, $78, $6c, $66, $00
		dta $00, $60, $60, $60, $60, $60, $7e, $00, $00, $63, $77, $7f, $6b, $63, $63, $00
		dta $00, $66, $76, $7e, $7e, $6e, $66, $00, $00, $3c, $66, $66, $66, $66, $3c, $00
		dta $00, $7c, $66, $66, $7c, $60, $60, $00, $00, $3c, $66, $66, $66, $6c, $36, $00
		dta $00, $7c, $66, $66, $7c, $6c, $66, $00, $00, $3c, $60, $3c, $06, $06, $3c, $00
		dta $00, $7e, $18, $18, $18, $18, $18, $00, $00, $66, $66, $66, $66, $66, $7e, $00
		dta $00, $66, $66, $66, $66, $3c, $18, $00, $00, $63, $63, $6b, $7f, $77, $63, $00
		dta $00, $66, $66, $3c, $3c, $66, $66, $00, $00, $66, $66, $3c, $18, $18, $18, $00
		dta $00, $7e, $0c, $18, $30, $60, $7e, $00, $00, $1e, $18, $18, $18, $18, $1e, $00
		dta $00, $40, $60, $30, $18, $0c, $06, $00, $00, $78, $18, $18, $18, $18, $78, $00
		dta $00, $08, $1c, $36, $63, $00, $00, $00, $00, $00, $00, $00, $00, $00, $ff, $00
		dta $18, $7e, $36, $0c, $18, $30, $7e, $00, $00, $00, $3c, $06, $3e, $66, $3e, $03
		dta $0c, $18, $7e, $0c, $18, $30, $7e, $00, $0c, $18, $3c, $60, $60, $60, $3c, $00
		dta $18, $3c, $60, $3c, $06, $06, $3c, $00, $00, $00, $3c, $66, $7e, $60, $3c, $06
		dta $00, $66, $00, $3c, $66, $66, $3c, $00, $66, $00, $3c, $66, $66, $66, $3c, $00
		dta $1c, $30, $30, $78, $30, $30, $7e, $00, $00, $66, $00, $66, $66, $66, $3e, $00
		dta $00, $1c, $36, $3c, $36, $37, $3e, $60, $00, $60, $60, $78, $70, $e0, $7e, $00
		dta $00, $38, $18, $1c, $38, $18, $3c, $00, $0c, $5a, $66, $7e, $7e, $6e, $66, $00
		dta $0c, $18, $7c, $66, $66, $66, $66, $00, $0c, $18, $3c, $66, $66, $66, $3c, $00
		dta $06, $3c, $6a, $66, $66, $66, $3c, $00, $00, $61, $66, $66, $66, $3c, $18, $00
		dta $00, $7e, $60, $7c, $60, $60, $7e, $03, $0c, $18, $3e, $60, $3c, $06, $7c, $00
		dta $36, $00, $3c, $06, $3e, $66, $3e, $00, $66, $00, $66, $66, $66, $66, $7e, $00
		dta $0c, $3c, $6e, $60, $60, $66, $3e, $00, $00, $18, $3c, $66, $66, $7e, $66, $03
		dta $18, $7e, $0c, $18, $30, $60, $7e, $00, $66, $66, $18, $3c, $66, $7e, $66, $00
		dta $18, $00, $7e, $0c, $18, $30, $7e, $00, $78, $60, $78, $60, $7e, $18, $1e, $00
		dta $00, $18, $3c, $7e, $18, $18, $18, $00, $00, $18, $18, $18, $7e, $3c, $18, $00
		dta $00, $18, $30, $7e, $30, $18, $00, $00, $00, $18, $0c, $7e, $0c, $18, $00, $00
		dta $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $3c, $06, $3e, $66, $3e, $00
		dta $00, $60, $60, $7c, $66, $66, $7c, $00, $00, $00, $3c, $60, $60, $60, $3c, $00
		dta $00, $06, $06, $3e, $66, $66, $3e, $00, $00, $00, $3c, $66, $7e, $60, $3c, $00
		dta $00, $0e, $18, $3e, $18, $18, $18, $00, $00, $00, $3e, $66, $66, $3e, $06, $7c
		dta $00, $60, $60, $7c, $66, $66, $66, $00, $00, $18, $00, $38, $18, $18, $3c, $00
		dta $00, $06, $00, $06, $06, $06, $06, $3c, $00, $60, $60, $6c, $78, $6c, $66, $00
		dta $00, $38, $18, $18, $18, $18, $3c, $00, $00, $00, $66, $7f, $7f, $6b, $63, $00
		dta $00, $00, $7c, $66, $66, $66, $66, $00, $00, $00, $3c, $66, $66, $66, $3c, $00
		dta $00, $00, $7c, $66, $66, $7c, $60, $60, $00, $00, $3e, $66, $66, $3e, $06, $06
		dta $00, $00, $7c, $66, $60, $60, $60, $00, $00, $00, $3e, $60, $3c, $06, $7c, $00
		dta $00, $18, $7e, $18, $18, $18, $0e, $00, $00, $00, $66, $66, $66, $66, $3e, $00
		dta $00, $00, $66, $66, $66, $3c, $18, $00, $00, $00, $63, $6b, $7f, $3e, $36, $00
		dta $00, $00, $66, $3c, $18, $3c, $66, $00, $00, $00, $66, $66, $66, $3e, $0c, $78
		dta $00, $00, $7e, $0c, $18, $30, $7e, $00, $00, $00, $00, $00, $00, $00, $00, $00
		dta $18, $18, $18, $18, $18, $18, $18, $18, $00, $7e, $78, $7c, $6e, $66, $06, $00
		dta $08, $18, $38, $78, $38, $18, $08, $00, $10, $18, $1c, $1e, $1c, $18, $10, $00
;-----------------------------------------------------------------------		
; P/M DATA
;		.print "#define	PM_DATA_OFFSET	0x",*-$a000
		
;pm		:+1024 dta $00

;-----------------------------------------------------------------------		
;
; CART MAIN CODE
;

BEGIN	jsr TESTSEL
		ldx #ALLOC ; $01EB - $01FF (21 bytes) on STACK for LOADER
		lda #$EA
@		pha
		dex
		bne @-

		;--------	
		; Set Menu
		lda TMP
		pha
		lda TMP+1
		pha
		lda DMACTLS
		pha
		lda #$00
		sta DMACTL
		lda #$21
		sta DMACTLS
		
		lda DLPTRS
		pha
		lda #<antic
		sta DLPTRS
		lda DLPTRS+1
		pha
		lda #>antic
		sta DLPTRS+1
		lda COLBAKS
		pha
		lda #$00
		sta COLBAKS
		lda COLPF0S
		pha
		lda #$06
		sta COLPF0S
		lda COLPF1S
		pha
		lda #$0C
		sta COLPF1S
		lda COLPF2S
		pha
		lda #$02
		sta COLPF2S
		
		lda CHBAS
		pha
		lda #>fonts
		sta CHBAS
		sta CHBASE
		
		;--------	
		; Disable BASIC
		lda PORTB
		ora #$02
		sta PORTB
		lda #$01
		sta BASICF
		lda #$1F
		sta MEMTOP
		lda #$BC
		sta MEMTOP+1
		lda #$C0
		sta RAMTOP
		lda #$00
		sta DOSVEC
		sta DOSINI
		sta CASINI	
		lda #$01
		sta DOSVEC+1
		sta DOSINI+1
		sta CASINI+1
		lda #$03
		sta BOOTQ
		lda #$00
		sta COLDST
		ldx #$05
@		lda COLDRST,X
		sta $0100,X
		dex
		bpl @-
		;--------
		; Clear RAM under cart
		jsr CopyCLR
		;--------
		; Chose XEX
		lda #$00
		sta CNT
@		inc CNT
		lda CNT
		asl
		asl
		tax
		lda table,X
		bpl @-
		;--------
		; RUN if only one pos
		ldx CNT
		dex
		sta POS
		beq RESTORE
		lda #$00
		sta TMP
		sta TMP+1
		;--------	
		; Menu LOOP
MLOOP	jsr PAINT
		jsr JOYS
		lda TMP
		tax
		and #$80
		beq @+
		txa
		and #$7F
		beq RANDOPT
		tax
		bne FINDKEY
			
		;--------	
		; Read Key	
@		lda #$FF
		sta KBCODE
		lda SKSTAT
		and #$04
		cmp #$04
		beq MLOOP
		
		lda KBCODE
		ldx #27
@		cmp KEYTBLE,X
		beq	FINDKEY
		dex
		bne @-
RANDOPT	lda RANDOM
		and #$1F
		cmp CNT
		bcs RANDOPT
		sta POS
		bcc RESTORE
FINDKEY	dex
		cpx CNT
		bcs RANDOPT
		stx POS

		;--------	
		; Restore Screen	
RESTORE	lda #$00
		sta DMACTL
		pla
		sta CHBAS
		pla
		sta COLPF2S
		pla
		sta COLPF1S
		pla
		sta COLPF0S
		pla
		sta COLBAKS
		pla
		sta DLPTRS+1
		pla
		sta DLPTRS
		pla
		sta DMACTLS
		pla
		sta TMP+1
		pla
		sta TMP

		;--------	
		; Load XEX		
		lda #$01
		sta CRITIC
		lda #$00
		sta DMACTL
		jsr LOADPOS
		
		;--------	
		; JMP to RUNAD ($02E0) - Back WarmReset
		jsr CopyENT		; Copy ENTRY Procedure
		sei	
		lda #$00
		sta CRITIC	
		lda #<RESETCD
		sta BACK
		lda #>RESETCD
		sta BACK+1
		lda RUNAD
		sta RUN
		lda RUNAD+1
		sta RUN+1
		jmp ENTRY
;-----------------------------------------------------------------------		
; Paint colors
PAINT	lda TMP
		beq @+
		asl
		asl
		adc #$0C
@		tay
		
		lda #$07
@		cmp VCOUNT
		bne @-
		lda #$0E
		sta COLPF1		
		
		clc
		ldx #$00
@		lda ctable,x
		sta COLPF2
		add #$10
		inx
		sta WSYNC
		cpx #$0F
		bne @-
		
		sta WSYNC
		sta WSYNC
		tya
		cmp #$10
		beq @+
		
		sta WSYNC

		beq STDCLR
		jsr STDCLR

@		cmp VCOUNT
		bne @-			
		lda #$08
		sta COLPF2
		lda #$0E
		sta COLPF1
		ldx #$08
		lda #$04
@		sta WSYNC
		dex
		bne @-
		;--------	
		; Standard Colors
STDCLR 	ldx #$04
		stx COLPF2		
		ldx #$0C
		stx COLPF1
NOBAR	rts	

;-----------------------------------------------------------------------		
; /Start/Select/Option/ & JOY 1+2
JOYS	lda CONSOL
		cmp #$05
		beq NEXT
		cmp #$03
		beq PREV
		cmp #$06
		beq CLICK
		
		lda PORTA
		cmp #$FD
		beq NEXT
		cmp #$DF
		beq NEXT
		cmp #$FE
		beq PREV
		cmp #$EF
		beq PREV
		lda TRIG0
		beq CLICK
		lda TRIG1
		beq CLICK
			
		lda #$00
		sta TMP+1
		rts	
		;--------	
		; Next
NEXT	lda TMP+1
		bne @+
		inc TMP+1
		inc TMP
		ldx TMP
		dex 
		cpx CNT
		bne @+
		lda #$01
		sta TMP
@		rts
		;--------	
		; Previous
PREV	lda TMP+1
		bne @+
		inc TMP+1
		lda TMP
		beq PRLAST
		dec TMP
		bne @+
PRLAST	ldx CNT
		stx TMP
@		rts		
		;--------	
		; Click
CLICK	lda TMP
		ora #$80
		sta TMP
@		rts
;-----------------------------------------------------------------------		
; Load POS=A

		;--------	
		; 4 bytes - MUL 4	
LOADPOS
		asl POS
		asl POS
		
		lda #$FF		; Clear RUNAD & INITAD
		sta RUNAD
		sta RUNAD+1
		sta INITAD
		sta INITAD+1

		ldx POS
		lda table,X
		and #7
		cmp	#0
		beq	LOADXEX
		cmp	#1
		beq	LOADBOOT
		cmp	#2
		beq	LOADATR
		cmp	#3
		beq	LOADBASIC
		cmp	#4
		beq	LOADCAR

LOADBOOT
LOADATR
LOADBASIC
LOADCAR
		jmp	RESETCD

		
LOADXEX		ldx	POS
		lda	table,X
		and	#$70
		beq	READRAWXEX
		cmp	#$20
		beq	READRAWXEX
		cmp	#$10
		jeq	READAPL256XEX
		jmp	RESETCD
; --------------------------------------------------
; read binary here
; --------------------------------------------------
READRAWXEX
		jsr CopyCPY
		jsr SETPOSSRC

		jsr GET_FROM_CAR
		cmp #$FF		; Chceck DOS Header
		beq @+
; --------------------------------------------------
; uncompressed/raw read binary part
; --------------------------------------------------
ERRORWM	jmp RESETCD 	; Warm Reset if ERROR
@		jsr IncSrc
		bcs ERRORWM
		jsr GET_FROM_CAR
		cmp #$FF
		beq LOOP
		bne ERRORWM
LOOP	jsr IncSrc
		bcs ERRORWM
		
READBLC	jsr GET_FROM_CAR			; Read LSB
		STA DST
		jsr IncSrc		; SRC++
		bcs ERRORWM		; ERROR NoDATA
		jsr GET_FROM_CAR			; Read HSB
		sta DST+1
		jsr IncSrc
		bcs ERRORWM		; ERROR NoDATA
		
		jsr GET_FROM_CAR
		sta CNT			; Set Last Write LSB
		jsr IncSrc
		bcs ERRORWM
		jsr GET_FROM_CAR
		sta CNT+1		; Set Last Write MSB
		jsr IncSrc
		bcs ERRORWM	
		lda CNT
		ora CNT+1
		bne TRANSF
DECRTRANSF
		; decomp stuff
		jsr aPL_depack_blk
		; compressed blocks never contain init/run addresses
		jmp READBLC

TRANSF		jsr GET_FROM_CAR			; Read BYTE
		;sta COLBAK		; Write to "noise"
		;clc				; For Smart Stack Procedure
		jsr PUT_RAM			; Write BYTE
		jsr CmpDst		; Check Destination
		bcs ENDBLK		; If last
		inc DST			; Prepare Destination for next write
		bne @+
		inc DST+1
@		jsr IncSrc		; Increment Source
		bcs ERRORWM		; ERROR NoDATA
		bcc	TRANSF		; Repeat transfer byte
		
ENDBLK 	lda INITAD		; End DOS block
		cmp #$FF		; New INITAD?
		bne RUNPART		; Run INIT Procedure
		lda INITAD+1
		cmp #$FF
		bne RUNPART		; Run INIT Procedure
		jsr IncSrc		; Increment Source
		bcc READBLC		; No EOF read next block
		rts				; All data readed, back to RUNAD procedure
		
RUNPART	lda BANK
		pha				; Store BANK
		lda	SRC			; Store LSB
		pha
		lda SRC+1		; Store MSB
		pha
		txa
		pha
		tya
		pha

		jsr CopyENT		; Copy ENTRY Procedure
		lda INITAD
		sta RUN
		lda INITAD+1
		sta RUN+1		; Copy INITAD
		sei
		lda #$00
		sta CRITIC
		jsr ENTRY

		lda #$01
		sta CRITIC
		lda #$00
		sta DMACTL
		lda TRIG3
		sta GINTLK
		cli				; Allow IRQ
				
		lda #$FF		; Clear INITAD for next detection
		sta INITAD
		sta INITAD+1

		jsr CopyCPY
		pla
		tay
		pla
		tax
		pla
		sta SRC+1		; Restore MSB
		pla
		sta SRC			; Restore LSB
		pla
		sta BANK		; Restore BANK

		jmp LOOP
; --------------------------------------------------
; compressed read binary part
; --------------------------------------------------
COMPRESSED_READ
READAPL256XEX
		jsr CopyCPY256
		jsr SETPOSSRC

		; configure decompresor
		lda	#$ff
		sta	bl
		; uncompress first byte
		mwa	#continue	yieldvec
		jmp	aPL_depack
continue
		; we have got the first byte
		; uncompress second byte
		GET_COMP_BYTE

		jmp CLOOP


CERRORWM	jmp RESETCD		; fill-up code
		bcs CERRORWM
		GET_COMP_BYTE
		cmp #$FF
		bne CREADBLC
CLOOP	;jsr IncSrc
		bcs CERRORWM
		
CREADBLC	GET_COMP_BYTE			; Read LSB
		bcc @+		;ok, there is data
		rts				; All data readed, back to RUNAD procedure
@		sta DST

		GET_COMP_BYTE			; Read HSB
		bcs CERRORWM		; ERROR NoDATA
		sta DST+1
		
		GET_COMP_BYTE
		bcs CERRORWM
		sta CNT			; Set Last Write LSB

		GET_COMP_BYTE
		bcs CERRORWM	
		sta CNT+1		; Set Last Write MSB
		
CTRANSF		GET_COMP_BYTE			; Read BYTE
		nop
		bcs CERRORWM		; ERROR NoDATA
		;sta COLBAK		; Write to "noise"
		;clc				; For Smart Stack Procedure
		jsr PUT_RAM			; Write BYTE
		; Check Destination
		lda DST
		cmp CNT
		bne @+
		lda DST+1
		cmp CNT+1
		beq CENDBLK		; If last
@		inc DST			; Prepare Destination for next write
		bne @+
		inc DST+1
@
		bne	CTRANSF		; Repeat transfer byte
		
CENDBLK 
		lda INITAD		; End DOS block
		cmp #$FF		; New INITAD?
		bne CRUNPART		; Run INIT Procedure
		lda INITAD+1
		cmp #$FF
		bne CRUNPART		; Run INIT Procedure
		jmp CREADBLC		; No EOF read next block
		
CRUNPART
		lda	BANK
		pha				; Store BANK
		lda	SRC			; Store LSB
		pha
		lda	SRC+1		; Store MSB
		pha
		lda	CBUFFER
		pha
		lda	CBUFSRC
		pha
		txa
		pha
		tya
		pha


		jsr CopyENT		; Copy ENTRY Procedure
		lda INITAD
		sta RUN
		lda INITAD+1
		sta RUN+1		; Copy INITAD
		
		sei
		lda #$00
		sta CRITIC
		jsr ENTRY

		lda #$01
		sta CRITIC
		lda #$00
		sta DMACTL
		lda TRIG3
		sta GINTLK
		cli				; Allow IRQ
				
		lda #$FF		; Clear INITAD for next detection
		sta INITAD
		sta INITAD+1

		jsr CopyCPY256
		pla
		tay
		pla
		tax
		pla
		sta	CBUFSRC
		pla
		sta	CBUFFER
		pla
		sta	SRC+1		; Restore MSB
		pla
		sta	SRC			; Restore LSB
		pla
		sta	BANK		; Restore BANK
		clc
		jmp CLOOP
		
;-----------------------------------------------------------------------		
; Set Source by POSx4 value
SETPOSSRC
		ldx POS
		lda table+1,X	; BANK ->A
		sta BANK
		lda table+2,X	; MSB ->Y
		sta SRC+1
		lda table+3,X	; LSB ->X
		sta SRC
		rts

;-----------------------------------------------------------------------		
; Set Source
;SetSrc	sta BANK
;		stx SRC
;		sty SRC+1
;		rts

;-----------------------------------------------------------------------		
; Inc Source
IncSrc	inc SRC
		bne @+
		inc SRC+1
		bit SRC+1
		bvc @+
		lda #$A0
		sta SRC+1
		inc BANK
@
		; Cmp Source
		ldx POS
		lda table+3+4,X
		cmp SRC
		bne @+
		lda table+2+4,X
		cmp SRC+1
		bne @+
		lda table+1+4,X
		cmp BANK
		bne @+
		sec
		rts
@		clc
		rts
		
;-----------------------------------------------------------------------		
; Set Destination
SetDst	stx DST
		sty DST+1
		rts

;-----------------------------------------------------------------------		
; Inc Destination
;IncDst	inc DST
;		bne @+
;		inc DST+1
;@		rts

;-----------------------------------------------------------------------		
; Cmp Destination
CmpDst	lda DST
		cmp CNT
		bne @+
		lda DST+1
		cmp CNT+1
		bne @+
		sec
		rts
@		clc
		rts

;-----------------------------------------------------------------------		
; Copy Clear to $0400
CopyCLR	ldx #(CLPRE-CLPRS-1)
@		lda CLPRS,X
		sta $0400,X
		dex
		bpl @-
		jsr $0400	
		ldx #(CLPRE-CLPRS-1)
@		lda #$00
		sta $0400,X
		dex
		bpl @-		
		rts

;-----------------------------------------------------------------------		
; Copy Copy to Stack
CopyCPY	ldx #(DTACPYE-DTACPYS)
@		lda DTACPYS-1,X
		sta $0200-(DTACPYE-DTACPYS+1),X
		dex
		bne @-
		rts
		
;-----------------------------------------------------------------------		
; Copy Copy256 to Stack
CopyCPY256	ldx #(DTA256CPYE-DTA256CPYS)
@		lda DTA256CPYS-1,X
		sta $0200-(DTA256CPYE-DTA256CPYS+1),X
		dex
		bne @-
		rts
		
;-----------------------------------------------------------------------		
; Copy Entry to Stack
CopyENT	ldx #(ENTRYE-ENTRYS)
@		lda ENTRYS-1,X
		sta $0200-(ENTRYE-ENTRYS+1),X
		dex
		bne @-
		rts
;-----------------------------------------------------------------------		
; Test /Select/ and Disable Cartridge
TESTSEL	lda CONSOL
		and #$02
		bne CONTIN
		ldx #(CONTIN-DISCART-1)
@		lda DISCART,X
		sta $0400,x
		dex
		bpl @-
		jmp $0400
DISCART sta $D5FF
		jmp RESETCD
CONTIN	rts		
;-----------------------------------------------------------------------		
; Keyboard Table
;		A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z
KEYTBLE	dta	$FF,$3F,$15,$12,$3A,$2A,$38,$3D,$39,$0D,$01,$05,$00,$25,$23,$08,$0A,$2F,$28,$3E,$2D,$0B,$10,$2E,$16,$2B,$17
;-----------------------------------------------------------------------
		icl "apldecr_zp.asm"
		
;-----------------------------------------------------------------------		
; $0400 CODE
; CLR $A000 - $BFFF
CLPRS	lda #$A0
		sta CRITIC
		sta $D5FF
		sta TMP+1
		lda #$00
		sta TMP
		tay
NEWPAG	lda #$00
@		sta (TMP),Y
		iny
		bne @-
		inc TMP+1
		bit TMP+1
		bvc NEWPAG
		sta $D500
		ldy TRIG3
		sty GINTLK
		sta CRITIC
		rts
CLPRE		
;-----------------------------------------------------------------------		
; STACK CODE FOR NORMAL AND BLOCK COMPRESSED
DTACPYS
; THREE entry points:
; GETBYTE - gets byte from cart or whatever
; PUTBYTE - puts byte to ram
; GETRBTE - copies byte from ram to ram
; the goal was to keep one instance of ADRSRC and ADRDST

GETBYTE	sta $D500 ; will be updated to bank number; entry point
ADRSRC	lda $FFFF
BACKC	sta $D500 
	rts
PUTBYTE	sta $D5FF	; entry point
ADRDST	sta $FFFF
	clc
	bcc BACKC
GETRBTE sta $D5FF ; entry point
	clc
ADRRSRC	lda $FFFF
	bcc BACKC

DTACPYE
;-----------------------------------------------------------------------		
; STACK CODE FOR COMPRESSED 256-byte Windowed
DTA256CPYS
; FOUR entry points:
; GETBYTE - gets byte from cart or whatever
; PUTBYTE - puts byte to ram
;  - copies byte from ram to ram
; the goal was to keep one instance of ADRSRC and ADRDST
; names are prefixed to avoid double declaration
; first code part must be a duplicate of previous block
; cyclic buffer procs must fit into place of GETRBTE proc and substitutes it.
_GETBYTE	sta $D500 ; will be updated to bank number; entry point
_ADRSRC	lda $FFFF
_BACKC	sta $D500 
	rts
_PUTBYTE	sta $D5FF	; entry point
_ADRDST	sta $FFFF
	clc
	bcc _BACKC
CYCL256 sta $500 ; entry point
	rts
SRCP256	lda $500 ; entry point
	rts
	nop	;IMPORTANT to get the same size as previous code section

DTA256CPYE
;--------------------------------------------
ENTRYS	sta $D5FF
		lda TRIG3
		sta GINTLK
		cli
ADRRUN	jsr RESETCD
		sei
		sta $D500
ADRBCK	jmp EXIT
ENTRYE
	.print"End of code: ",*
;-----------------------------------------------------------------------		
; INITCART ROUTINE - back from old MaxFlash

		ORG $BFED

COLDRST	jsr INIT		; catch lost code on unused bank, set bank 0
		jmp RESETCD		; and ColdReset
		nop				; fill Bank Number
INIT	lda #$00
		sta $D500
EXIT	rts

;-----------------------------------------------------------------------		

		ORG $BFFA
		dta <BEGIN, >BEGIN, $00, $04, <INIT, >INIT

;-----------------------------------------------------------------------		
