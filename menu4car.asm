;-----------------------------------------------------------------------		
;
; Menu4car starter
; (c) 2023 GienekP
;
;-----------------------------------------------------------------------

BANK    = ($0200-(DTACPYE-GETBYTE)+1)
SRC     = ($0200-(DTACPYE-ADRSRC)+1)
DST     = ($0200-(DTACPYE-ADRDST)+1)
POS     = $01EB
CNT     = $01EC
GET     = ($0200-(DTACPYE-GETBYTE))
PUT     = ($0200-(DTACPYE-PUTBYTE))
RUN     = ($0200-(ENTRYE-ADRRUN)+1)
BACK    = ($0200-(ENTRYE-ADRBCK)+1)
ENTRY   = ($0200-(ENTRYE-ENTRYS))

;-----------------------------------------------------------------------

TMP		= $00
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
BASICF  = $03F8
GINTLK  = $03FA

TRIG0   = $D010
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
PORTB   = $D301
DMACTL  = $D400
WSYNC   = $D40A
VCOUNT  = $D40B
NMIEN   = $D40E

RESETCD	= $C2C8

;-----------------------------------------------------------------------		
; SimplyCART Bank

		OPT h-f+
		
		ORG $A000
;-----------------------------------------------------------------------		
; Table of files
; BH BL HS LS
table	:+(32*4) dta $00

;-----------------------------------------------------------------------		
; Menu4CAR Logo
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
		dta d'   A.                           '
		dta d'   B.                           '
		dta d'   C.                           '
		dta d'   D.                           '
		dta d'   E.                           '
		dta d'   F.                           '
		dta d'   G.                           '
		dta d'   H.                           '
		dta d'   I.                           '
		dta d'   J.                           '
		dta d'   K.                           '
		dta d'   L.                           '
		dta d'   M.                           '
		dta d'   N.                           '
		dta d'   O.                           '
		dta d'   P.                           '
		dta d'   Q.                           '
		dta d'   R.                           '
		dta d'   S.                           '
		dta d'   T.                           '
		dta d'   U.                           '
		dta d'   V.                           '
		dta d'   W.                           '
		dta d'   X.                           '
		dta d'   Y.                           '
		dta d'   Z.                           '

;-----------------------------------------------------------------------		
; ANTIC PROGRAM

antic	:+1 dta $70
		dta $4F,<pic,>pic
		:+15 dta $0F
		:+26 dta $02
		dta $41,<antic,>antic

;-----------------------------------------------------------------------		
; P/M DATA
pm		:+1024 dta $A5
	
;-----------------------------------------------------------------------		
; CART MAIN CODE
	
BEGIN	ldx #$15 ; $01EB - $01FF on STACK for LOADER
		lda #$EA
@		pha
		dex
		bne @-

		;--------	
		; Set Menu
		lda TMP
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
		; Chose XEX
		lda #$00
		sta CNT
@		inc CNT
		lda CNT
		asl
		asl
		tax
		lda table,X
		and #$80
		cmp #$80
		bne @-
		;--------
		; RUN if only one pos
		ldx CNT
		dex
		sta POS
		beq RESTORE
		;--------	
		; Menu LOOP
MLOOP	lda #$07
@		cmp VCOUNT
		bne @-
		lda #$0E
		sta COLPF1		
		
		clc
		ldx #$00
		lda #$06
@		sta COLPF2
		add #$10
		inx
		sta WSYNC
		cpx #$0F
		bne @-
	
		lda #$10
@		cmp VCOUNT
		bne @-
		lda #$04
		sta COLPF2		
		lda #$0C
		sta COLPF1		
				
		lda #$FF
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
		sta TMP

		;--------	
		; Load XEX		
		lda #$01
		sta CRITIC
		lda #$00
		sta DMACTL
		lda POS
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
; Load POS=A

		;--------	
		; 4 bytes - MUL 4	
LOADPOS	asl
		asl
		sta POS
		jsr CopyCPY
		lda POS
		tax
		clc
		adc #$04
		sta POS
		lda table+1,X	; BANK
		pha
		lda table+2,X	; MSB
		pha
		lda table+3,X	; LSB
		tax
		pla
		tay
		pla
		jsr SetSrc
		
		lda #$FF		; Clear RUNAD & INITAD
		sta RUNAD
		sta RUNAD+1
		sta INITAD
		sta INITAD+1
		
		jsr GET
		cmp #$FF		; Chceck DOS Header
		beq @+
ERRORWM	jmp RESETCD 	; Warm Reset if ERROR
@		jsr IncSrc
		jsr CmpSrc
		bcs ERRORWM
		jsr GET
		cmp #$FF
		bne ERRORWM
LOOP	jsr IncSrc
		jsr CmpSrc
		bcs ERRORWM
		
READBLC	jsr GET			; Read LSB
		pha
		jsr IncSrc		; SRC++
		jsr CmpSrc		; EOF(SRC)
		bcs ERRORWM		; ERROR NoDATA
		jsr GET			; Read HSB
		pha
		jsr IncSrc
		jsr CmpSrc
		bcs ERRORWM		; ERROR NoDATA
		pla
		tay
		pla
		tax
		jsr SetDst		; Set Destination
		
		jsr GET
		sta CNT			; Set Last Write LSB
		jsr IncSrc
		jsr CmpSrc
		bcs ERRORWM
		jsr GET
		sta CNT+1		; Set Last Write MSB
		jsr IncSrc
		jsr CmpSrc
		bcs ERRORWM	
		
TRANSF	jsr GET			; Read BYTE
		;sta COLBAK		; Write to "noise"
		clc				; For Smart Stack Procedure
		jsr PUT			; Write BYTE
		jsr CmpDst		; Check Destination
		bcs ENDBLK		; If last
		jsr IncDst		; Prepare Destination for next write
		jsr IncSrc		; Increment Source
		jsr CmpSrc		; EOF(SRC)
		bcs ERRORWM		; ERROR NoDATA
		bcc	TRANSF		; Repeat transfer byte
		
ENDBLK 	lda INITAD		; End DOS block
		cmp #$FF		; New INITAD?
		bne RUNPART		; Run INIT Procedure
		lda INITAD+1
		cmp #$FF
		bne RUNPART		; Run INIT Procedure
		jsr IncSrc		; Increment Source
		jsr CmpSrc		; EOF(SRC)
		bcc READBLC		; No EOF read next block
		rts				; All data readed, back to RUNAD procedure
		
RUNPART	lda BANK
		pha				; Store BANK
		lda	SRC			; Store LSB
		pha
		lda SRC+1		; Store MSB
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
		sta SRC+1		; Restore MSB
		pla
		sta SRC			; Restore LSB
		pla
		sta BANK		; Restore BANK

		jmp LOOP
		
;-----------------------------------------------------------------------		
; Set Source
SetSrc	sta BANK
		stx SRC
		sty SRC+1
		rts

;-----------------------------------------------------------------------		
; Inc Source
IncSrc	inc SRC
		bne @+
		inc SRC+1
		lda SRC+1
		cmp #$C0
		bne @+
		lda #$A0
		sta SRC+1
		inc BANK
@		rts

;-----------------------------------------------------------------------		
; Cmp Source
CmpSrc	ldx POS
		lda table+3,X
		cmp SRC
		bne @+
		lda table+2,X
		cmp SRC+1
		bne @+
		lda table+1,X
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
IncDst	inc DST
		bne @+
		inc DST+1
@		rts

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
; Copy Copy to Stack
CopyCPY	ldx #(DTACPYE-DTACPYS)
@		lda DTACPYS-1,X
		sta $0200-(DTACPYE-DTACPYS+1),X
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
; Keyboard Table
		;         A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z
KEYTBLE	dta $FF,$3F,$15,$12,$3A,$2A,$38,$3D,$39,$0D,$01,$05,$00,$25,$23,$08,$0A,$2F,$28,$3E,$2D,$0B,$10,$2E,$16,$2B,$17	
;-----------------------------------------------------------------------		
; FONTS
		ORG $BB00
		
fonts	:+1024 dta $00		

;-----------------------------------------------------------------------		
; STACK CODE
		ORG $BF80
		
DTACPYS
PUTBYTE	sta $D5FF
ADRDST	sta $FFFF
		bcc BACKC

GETBYTE	sta $D500
ADRSRC	lda $FFFF

BACKC	sta $D500
		rts
DTACPYE
;---
ENTRYS	sta $D5FF
		lda TRIG3
		sta GINTLK
		cli
ADRRUN	jsr RESETCD
		sei
		sta $D500
ADRBCK	jmp EXIT
ENTRYE

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
