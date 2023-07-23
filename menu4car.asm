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

RESETWM = $C290

;-----------------------------------------------------------------------		
; SimplyCART Bank

		OPT h-f+
		
		ORG $A000
;-----------------------------------------------------------------------		
; Table of files
; BH BL HS LS
table	:+(32*4) dta $FF

;-----------------------------------------------------------------------		
; RAW SCREEN

pic		:+(16*32) dta $FF
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
		dta $4E,<pic,>pic
		:+15 dta $0E
		:+26 dta $02
		dta $41,<antic,>antic
	
;-----------------------------------------------------------------------		
; CART MAIN CODE
	
BEGIN	ldx #$15 ; $01EB - $01FF on STACK for LOADER
		lda #$EA
@		pha
		dex
		bne @-

		;--------	
		; Set Menu	
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
		; Menu LOOP
MLOOP	lda #$08
@		cmp VCOUNT
		bne @-
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
		;--------	
		; Load XEX		
		lda #$01
		sta CRITIC
		lda POS
		jsr LOADPOS
		
		;--------	
		; JMP to RUNAD ($02E0) - Back WarmReset
		jsr CopyENT		; Copy ENTRY Procedure
		sei	
		lda #$00
		sta CRITIC	
		lda #<RESETWM
		sta BACK
		lda #>RESETWM
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
ERRORWM	jmp RESETWM 	; Warm Reset if ERROR
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
		
ENDBLK	lda INITAD		; End DOS block
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
		
		lda #$FF		; Clear INITAD for next detection
		sta INITAD
		sta INITAD+1
	
		lda #$01
		sta CRITIC
		lda TRIG3
		sta GINTLK
		jsr CopyCPY
		pla
		sta SRC+1		; Restore MSB
		pla
		sta SRC			; Restore LSB
		pla
		sta BANK		; Restore BANK

		cli				; Allow IRQ
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
PUTBYTE	sta $D580
ADRDST	sta $FFFF
		bcc BACKC

GETBYTE	sta $D500
ADRSRC	lda $FFFF

BACKC	sta $D500
		rts
DTACPYE
;---
ENTRYS	sta $D580
		lda TRIG3
		sta GINTLK
		cli
ADRRUN	jsr RESETWM
		sei
		sta $D500
ADRBCK	jmp EXIT
ENTRYE
;-----------------------------------------------------------------------		
; Cold RESET
COLDRST	sta $D500
		jmp RESETWM
;-----------------------------------------------------------------------		
; INITCART ROUTINE - back from old MaxFlash

		ORG $BFF4
	
INIT	lda #$00
		sta $D500
EXIT	rts

;-----------------------------------------------------------------------		

		ORG $BFFA
		dta <BEGIN, >BEGIN, $00, $04, <INIT, >INIT

;-----------------------------------------------------------------------		
