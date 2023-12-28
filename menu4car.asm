;-----------------------------------------------------------------------		
;
; Menu4car starter
; (c) 2023 GienekP, jhusak
;
;-----------------------------------------------------------------------
	;opt ?+	; use local ?variables 
; how many bytes for variables
STORAGE = $4A
; how many bytes for code
CODEBUF = $1E
; where cyclic buffer (page boundary)
BASEE   = $700
CYCLBUF = $800
VARIABLES   = (BASEE+CODEBUF)
FREEMEM	= BASEE+CODEBUF+STORAGE+1

screen_data = $8300 ; defined in ramdata.asm
fonts = $9400 ; defined in ramdata.asm
antic = $8200
DLI_SCREEN_ADDR = $8214

; entry points
GET_FROM_CAR	= (BASEE+GETBYTE-DTACPYS)
PUT_RAM		= (BASEE+PUTBYTE-DTACPYS)
GET_RAM_BYTE	= (BASEE+GETRBTE-DTACPYS)
; parameters
BANK    = (BASEE+GETBYTE+1-DTACPYS)
SRC     = (BASEE+ADRSRC+1-DTACPYS)
RSRC    = (BASEE+ADRRSRC+1-DTACPYS)
DST     = (BASEE+ADRDST+1-DTACPYS)

CADDRESS  = (BASEE+CADR-CLPRS)

NAME	= (BASEE+TNAME-GR0INITS)

; entry points
PUTCB	 = (BASEE+CYCL256-DTA256CPYS)
GETSRCCB = (BASEE+SRCP256-DTA256CPYS)
; parameters
CBUFFER = (BASEE+CYCL256+1-DTA256CPYS)
CBUFSRC = (BASEE+SRCP256+1-DTA256CPYS)


POS     = (VARIABLES)
CNT     = (VARIABLES+1)
nextbytevec = (VARIABLES+3)
yieldvec = (VARIABLES+5)
token	= (VARIABLES+7)
offsetL = (VARIABLES+8)
offsetH = (VARIABLES+9)
EBPL    = (VARIABLES+$a)
lenL	= EBPL
EBPH    = (VARIABLES+$b)
lenH	= EBPH
bl      = (VARIABLES+$c)
PAGE    = (VARIABLES+$d)
TPOS	= (VARIABLES+$e)
STORE   = (VARIABLES+$f)
; place for storing variables before call, 17 bytes
KRPDEL	= STORE+17
oldconsol	= STORE+18
oldjoy	= STORE+19
TMP	= STORE+20
ramcold	= STORE + 22

ENTRY   = (BASEE)
RUN     = (BASEE+ADRRUN+1-ENTRYS)
BACK    = (BASEE+ADRBCK+1-ENTRYS)

RUNCART   = (BASEE)

;-----------------------------------------------------------------------


;ZPALLOC = $24
;TMP     = $00
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
MEMLO   = $02E7
CHBAS   = $02F4

ICCMD   = $0342
ICBUFA  = $0344
ICAX1   = $034A

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

JCIOMAIN = $E456
RESETCD	= $E477

;-----------------------------------------------------------------------		
; Standard Bank

		OPT h-f+
		ORG $A000
		dta 0
		
;
;-----------------------------------------------------------------------
		ORG $B000

;-----------------------------------------------------------------------		
		.print "#define	DATAARRAY_OFFSET	0x",*-$a000
;-----------------------------------------------------------------------		
; Table of files:
; bank part of address
tabbnk	:+(131) dta 0
; in-bank adress lo byte 
tabalo	:+(131) dta 0
; in-bank adress hi byte 
tabahi	:+(131) dta 0
; translate table - position in menu
tabpos	:+(131) dta 0

STARTRAMDATA
; THERE IS DATA TO COPY TO RAM AREA WHEN CART IS OFF.
; IT HANDLES SIMPLE TASKS LIKE WRITE BYTE OR INIT EXE
; MUST BE IN CONTINUOUS MEM AND OCCUPY NOT MORE THAN PAGE OF MEMORY
; DUE TO COPYING PROCEDURE
;-----------------------------------------------------------------------		
; $0400 CODE
; CLR $A000 - $BFFF
CLPRS		; a=0; y=0
		sta $D5FF
@		sta CADR:$0900,y
		iny
		bne @-
		inc CADDRESS+1
		bit CADDRESS+1
		bvc @-
		bpl @-
		sta $D500
		rts		
CLPRE		
;-----------------------------------------------------------------------		
; RAM CODE FOR NORMAL AND BLOCK COMPRESSED
; length 28
; THREE entry points:
; GETBYTE - gets byte from cart or whatever
; PUTBYTE - puts byte to ram
; GETRBTE - copies byte from ram to ram
; the goal was to keep one instance of ADRSRC and ADRDST
DTACPYS
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
; RAM CODE FOR COMPRESSED 256-byte Windowed
; length 28
; FOUR entry points:
; GETBYTE - gets byte from cart or whatever
; PUTBYTE - puts byte to ram
;  - copies byte from ram to ram
; the goal was to keep one instance of ADRSRC and ADRDST
; names are prefixed to avoid double declaration
; first code part must be a duplicate of previous block
; cyclic buffer procs must fit into place of GETRBTE proc and substitutes it.
DTA256CPYS
_GETBYTE	sta $D500 ; will be updated to bank number; entry point
_ADRSRC	lda $FFFF
_BACKC	sta $D500 
	rts
_PUTBYTE	sta $D5FF	; entry point
_ADRDST	sta $FFFF
	clc
	bcc _BACKC
CYCL256 sta CYCLBUF ; entry point
	rts
SRCP256	lda CYCLBUF ; entry point
	rts
DTA256CPYE
;--------------------------------------------
; length 20
ENTRYS	sta $D5FF
		lda TRIG3
		sta GINTLK
		cli
ADRRUN	jsr RESETCD
		sei
		sta $D500

ADRBCK	jmp EXIT
ENTRYE

;--------------------------------------------
; RUNCART ram procedure
; length 12
; copies procedure to ram.
; In ram:
; sets proper cart bank (passed in A)
; clears stack and jumps to $e477
; this lets the cart to be properly initialized
; dos load etc.
RUNCARTS
		tay
		lda #0
		sta COLDST
		sta $d500,y

;s		beq s
		jmp RESETCD
RUNCARTE

DISCARTS sta $D5FF
		jmp RESETCD
DISCARTE
; END OF CONTINUOUS MEM TO BE STORED IN RAM
		.print "RAMDATALEN: ",*-STARTRAMDATA
;-----------------------------------------------------------------------		
; Copy Clear to BASEE
CopyCLR		ldx #(CLPRE-CLPRS-1)
		ldy #(CLPRE-STARTRAMDATA)
		jsr CopyUniY
		lda #0
		ldy #1
		sty CRITIC
		tay
		jsr BASEE
		ldy TRIG3
		sty GINTLK
		lda #0
		sta CRITIC
		rts
CopyCPY
		ldx #(DTACPYE-DTACPYS)
		ldy #(DTACPYE-STARTRAMDATA)
		bne CopyUniY
CopyCPY256
		ldx #(DTA256CPYE-DTA256CPYS)
		ldy #(DTA256CPYE-STARTRAMDATA)
		bne CopyUniY
CopyENT
		ldx #(ENTRYE-ENTRYS)
		ldy #(ENTRYE-STARTRAMDATA)
;-----------------------------------------------------------------------		
; copy Y-offset x bytes to BASEE
CopyUniY
@		lda STARTRAMDATA-1,Y
		sta BASEE-1,X
		dey
		dex
		bne @-
		rts
		
;-----------------------------------------------------------------------		
; Keyboard Table
;		      A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z
KEYTBLE	dta	$FF,$3F,$15,$12,$3A,$2A,$38,$3D,$39,$0D,$01,$05,$00,$25,$23,$08,$0A,$2F,$28,$3E,$2D,$0B,$10,$2E,$16,$2B,$17
		; 1 2 3 4 5
		dta 31,30,26,24,29
keytbllen	=	*-KEYTBLE

;-----------------------------------------------------------------------		
reinit_e
		ldx #(GR0INITE-GR0INITS)
@		lda GR0INITS-1,X
		sta BASEE-1,X
		dex
		bne @-

		lda #$c0
		sta RAMTOP
		sta MEMTOP+1
		stx MEMTOP

		lda #$0c ; close
		; ldx #$00 x is 0 already
		sta ICCMD,X
		jsr JCIOMAIN

		;ldx #$01
		;stx CRITIC

		lda #3	; open
		ldx #$00
		sta ICCMD,X
		lda # <NAME
		sta ICBUFA,X
		lda # >NAME
		sta ICBUFA+1,X
		lda #$0C
		sta ICAX1,X
		jsr BASEE

		;lda #0
		;sta CRITIC
		rts
GR0INITS
		sei
		sta $D5FF
		lda TRIG3
		sta GINTLK
		cli
		jsr JCIOMAIN
		sei
		sta $D500
		lda TRIG3
		sta GINTLK
		cli
		rts
TNAME		dta 'E:',$9b
GR0INITE

;-----------------------------------------------------------------------		
;-----------------------------------------------------------------------		
; CTABLE
		.print "#define	COLORTABLE_OFFSET	0x",*-$a000
ctable		dta $06,$16,$26,$36,$46,$56,$66,$76,$86,$96,$a6,$b6,$c6,$d6,$e6,$f6
;-----------------------------------------------------------------------	
;-----------------------------------------------------------------------		
; P/M DATA
;		.print "#define	PM_DATA_OFFSET	0x",*-$a000
		
;pm		:+1024 dta $00


;-----------------------------------------------------------------------		
;
; CART MAIN CODE
;

BEGIN
		; Test /Select/ and Disable Cartridge
		lda CONSOL
		and #$02
		bne CONTIN
		ldx #(DISCARTE-DISCARTS)
		ldy #(DISCARTE-STARTRAMDATA)
		jsr CopyUniY
		jmp $0700
CONTIN		
		; MEMLO
		lda #>FREEMEM
		sta MEMLO+1
		lda #<FREEMEM
		sta MEMLO
		;--------
		; Clear RAM under cart
		jsr CopyCLR
		; Disable BASIC
		lda PORTB
		ora #$02
		sta PORTB
		lda #$01
		sta BASICF

		lda #1
		sta CRITIC
		lda #$00
		sta DMACTL
		jsr CopyCPY
		mwa #$8000 DST
		mwa #$A000 SRC
		jsr dzx0_standard_decomp_blk
		lda #$21
		sta DMACTLS
		lda TRIG3
		sta GINTLK
		lda #0
		sta CRITIC

		jsr reinit_e

		; Set Menu
		lda TMP
		sta STORE
		lda TMP+1
		sta STORE+1
		lda DMACTLS
		sta STORE+2

		lda #$00
		sta DMACTL
		lda #$21
		sta DMACTLS
		
		.print "PAGE offset ",*+1-$A000
		ldx #0
		stx PAGE ; <-0
		;--------
	
		lda DLPTRS
		sta STORE+3
		lda #<antic
		sta DLPTRS
		lda DLPTRS+1
		sta STORE+4
		lda #>antic
		sta DLPTRS+1
		lda COLBAKS
		sta STORE+5
		lda #$00
		sta COLBAKS
		lda COLPF0S
		sta STORE+6
		lda #$06
		sta COLPF0S
		lda COLPF1S
		sta STORE+7
		lda #$0C
		sta COLPF1S
		lda COLPF2S
		sta STORE+8
		lda #$02
		sta COLPF2S
		
		lda CHBAS
		sta STORE+9
		lda #>fonts
		sta CHBAS
		sta CHBASE
		
		;--------	
		; Disable BASIC
		;lda PORTB
		;ora #$02
		;sta PORTB
		;lda #$01
		;sta BASICF
		;lda #$1F
		;sta MEMTOP
		;lda #$BC
		;sta MEMTOP+1
		;lda #$C0
		;sta RAMTOP
		lda #<ramcold
		sta DOSVEC
		sta DOSINI
		;sta CASINI
		lda #>ramcold
		sta DOSVEC+1
		sta DOSINI+1
		;sta CASINI+1
		lda #$01
		sta BOOTQ
		lda #$00
		sta COLDST
		ldx #$05
@		lda COLDRST,X
		sta ramcold,X
		dex
		bpl @-
		;--------
		; Chose XEX
		ldx #$ff
@		inx
		lda tabbnk,X
		bpl @-
		;--------
		; RUN if only one pos
		stx CNT	; store counted entries
		sta POS ; just in case if one pos
		dex
		jeq RESTORE
		lda #$0
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
		ldx #keytbllen
@		cmp KEYTBLE,X
		beq	FINDKEY
		dex
		bne @-
RANDOPT	lda RANDOM
		cmp CNT
		bcs RANDOPT
		sta POS	; random pos, 0<=CNT<=129
		bcc RESTORE ; with random  pos without key translation
FINDKEY		dex
		cpx #keytbllen-6
		bcc @+
		txa
		sec
		sbc #keytbllen-5
		cmp NUMPAGES
		bpl @+
		clc
		adc #1
		sta PAGE
		jsr setScreen
		jmp MLOOP


@		cpx CNT
		bcs RANDOPT ; random if bigger than CNT
		stx POS

		 ldx PAGE
		 beq keynxt
		 lda POS
@		 clc
		 adc PAGES,x
		 sta POS
keynxt
		; ----
		; find the right slot
		ldx CNT
		dex 
@		lda tabpos,x
		cmp POS
		beq @+
		dex
		bne @-
@
		stx POS ; set pos by key pressed

		;--------	
		; Restore Screen	
RESTORE
		jsr CopyCLR
		lda #$00
		sta DMACTL
		LDA STORE+9
		sta CHBAS
		LDA STORE+8
		sta COLPF2S
		LDA STORE+7
		sta COLPF1S
		LDA STORE+6
		sta COLPF0S
		LDA STORE+5
		sta COLBAKS
		LDA STORE+4
		sta DLPTRS+1
		lda STORE+3
		sta DLPTRS
		lda STORE+2
		sta DMACTLS
		lda STORE+1
		sta TMP+1
		lda STORE
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
;BREAK
		jmp ENTRY
;-----------------------------------------------------------------------		
; Paint colors
PAINT	lda TMP
		beq @+
		asl
		asl
		adc #$0C ; top offset for highlight bar
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
		adc #$11
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
		lda #$b6
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
		cmp #$07
		beq	consolcont
		cmp	oldconsol
		bne	consolchk 
		dec	KRPDEL
		bne	checkskip
consolchk
		mvy #5 KRPDEL
consolcont
		sta	oldconsol
		cmp	#$05
		jeq	NEXT
		cmp	#$03
		jeq	UP
		cmp	#$06
		beq	CLICK
checkskip
		lda	PORTA
		:4 lsr
		and	PORTA
		and	#$f
		cmp	#$f
		beq	joycont
		cmp	oldjoy
		bne	joycheck
		dec	KRPDEL
		bne	joyskip
joycheck
		mvy	#5 KRPDEL
joycont
		sta	oldjoy

		cmp #$D
		beq NEXT
		cmp #$E
		beq PREV
		cmp #$B
		beq DOWN
		cmp #$7
		beq UP
joyskip
		lda TRIG0
		and TRIG1
		beq CLICK
			
		lda #$00
		sta TMP+1
		rts
		;-------
		; Down - page next
DOWN
		ldx	PAGE
		beq	@+
		dec	PAGE
		lda	PAGE
		jmp	setScreen
@		lda	NUMPAGES
		sta	PAGE
		jmp	setScreen
		;-------
		; Up - page prev
UP
@		lda	PAGE
		:5 asl
		tax
		lda	screen_data+32,x
		bne	jmp
		lda	#$ff
		sta	PAGE
jmp
		inc	PAGE
		lda	#1
		sta	TMP
		lda	PAGE
		jmp	setScreen

		;--------	
		; Click
CLICK	asl	TMP
		sec
		ror	TMP
		rts
		;--------	
		; Next
NEXT		ldy PAGE
		lda TMP+1
		bne @+
		inc TMP+1
		inc TMP
		lda TMP
		cmp FILLPAGES,y
		bne @+
		lda #$01
		sta TMP
@		rts
		;--------	
		; Previous
PREV		lda TMP+1
		bne @+
		inc TMP+1
		lda TMP
		beq PRLAST
		dec TMP
		bne @+
PRLAST		ldy PAGE
		ldx FILLPAGES,y
		dex
		stx TMP
@		rts		
PAGES		dta	0,26,52,78,104
		.print "#define	FILL_PAGES_OFFSET	0x",*-$a000
FILLPAGES	dta	0,0,0,0,0
NUMPAGES	dta	0

setScreen
		ldy	VCOUNT
		cpy	#20
		bcc	setScreen
		tay
		lda	pageaddrs_lo,y
		sta	DLI_SCREEN_ADDR+1
		lda	pageaddrs_hi,y
		sta	DLI_SCREEN_ADDR+2
		rts
pageaddrs_lo
		dta	<(0+screen_data),<($340+screen_data),<($340*2+screen_data),<($340*3+screen_data),<($340*4+screen_data)
pageaddrs_hi
		dta	>(0+screen_data),>($340+screen_data),>($340*2+screen_data),>($340*3+screen_data),>($340*4+screen_data)


		
;-----------------------------------------------------------------------		
; Load POS=A

		;--------	
LOADPOS
		lda #$FF		; Clear RUNAD & INITAD
		sta RUNAD
		sta RUNAD+1
		sta INITAD
		sta INITAD+1

		ldx POS
		lda tabahi,X
		and #$e0
		; must correspond to #define in .c file
		cmp	#$80
		beq	LOADBOOT
		cmp	#$a0
		beq	LOADATR
		cmp	#$c0
		beq	LOADBASIC
		cmp	#$e0
		beq	LOADCAR
		sta	TPOS
		bne	LOADXEX
		

LOADBOOT
LOADATR
LOADBASIC
		jmp	RESETCD
LOADCAR		; 8 kb car area straight mapping
		ldx #(RUNCARTE-RUNCARTS)
		ldy #(RUNCARTE-STARTRAMDATA)
		jsr	CopyUniY
		ldx	POS
		lda	tabbnk,X ; bank
		jmp	RUNCART
		
LOADXEX		lda	TPOS
		cmp	#$20
		jeq	READAPL256XEX
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
ERRORWM		jmp RESETCD 	; Warm Reset if ERROR
@		jsr IncSrc
		beq ERRORWM
		jsr GET_FROM_CAR
		cmp #$FF
		beq LOOP
		bne ERRORWM
LOOP		jsr IncSrc
		beq ERRORWM
		
READBLC		jsr GET_FROM_CAR			; Read LSB
		STA DST
		jsr IncSrc		; SRC++
		beq ERRORWM		; ERROR NoDATA

		jsr GET_FROM_CAR			; Read HSB
		sta DST+1
		jsr IncSrc
		beq ERRORWM		; ERROR NoDATA
		
		jsr GET_FROM_CAR
		sta CNT			; Set Last Write LSB
		jsr IncSrc
		beq ERRORWM
		jsr GET_FROM_CAR
		sta CNT+1		; Set Last Write MSB
		jsr IncSrc
		beq ERRORWM
		lda DST			; this handles
		ora DST+1		; reading run
		beq TRANSF		; of zeros at the end mostly
		lda CNT
		ora CNT+1
		bne TRANSF
DECRTRANSF	ldx TPOS
		cpx	#$40 ; eq 2
		beq	aplgo
zx0go		; X==#$60 eq 3
		jsr dzx0_standard_decomp_blk
		clc
		bcc compgo
aplgo
		; decomp stuff
		jsr aPL_depack_blk
		; compressed blocks never contain init/run addresses
compgo
		jsr CheckSrc
		bne READBLC
		rts

TRANSF		jsr GET_FROM_CAR			; Read BYTE
		;sta COLBAK		; Write to "noise"
		;clc			; For Smart Stack Procedure
		jsr PUT_RAM		; Write BYTE

		jsr CmpDst		; Check Destination
		beq ENDBLK		; If last

		inc DST			; Prepare Destination for next write
		bne @+
		inc DST+1
@		jsr IncSrc		; Increment Source
		beq ERRORWM		; ERROR NoDATA
		bne TRANSF		; Repeat transfer byte
		
ENDBLK		lda INITAD		; End DOS block
		and INITAD+1
		cmp #$FF		; New INITAD?
		bne INITPART		; Run INIT Procedure
		jsr IncSrc		; Increment Source
		bne READBLC		; No EOF read next block
		rts				; All data readed, back to RUNAD procedure
		
INITPART	clc
		jsr preparetoinit
		jsr ENTRY
		clc
		jsr restoreinit
		jsr IncSrc
		jne READBLC
		rts
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
CLOOP
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
		jsr PUT_RAM			; Write BYTE
		jsr CmpDst		; Check Destination
		beq CENDBLK		; If last

		inc DST			; Prepare Destination for next write
		bne @+
		inc DST+1
@
		bne	CTRANSF		; Repeat transfer byte
		
CENDBLK
		lda INITAD		; End DOS block
		and  INITAD+1
		cmp #$FF		; New INITAD?
		bne CINITPART		; Run INIT Procedure

		jmp CREADBLC		; No EOF read next block
		
CINITPART	sec
		jsr preparetoinit
		jsr ENTRY
		sec
		jsr restoreinit
		clc
		jmp CLOOP

preparetoinit
		; if carry set - 256 bytes decompress init
		; if carry clr - normal block apl init
		lda BANK
		sta STORE+10				; Store BANK
		lda SRC			; Store LSB
		sta STORE+11
		lda SRC+1		; Store MSB
		sta STORE+12
		bcc @+
			lda CBUFFER	; 256
			sta STORE+13	; 256
			lda CBUFSRC	; 256
			sta STORE+14	; 256
@
		stx STORE+15
		sty STORE+16
		jsr CopyENT		; Copy ENTRY Procedure
		lda INITAD
		sta RUN
		lda INITAD+1
		sta RUN+1		; Copy INITAD
		
		sei
		lda #$00
		sta CRITIC
		rts


restoreinit
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

		scc
		jsr CopyCPY256

		scs
		jsr CopyCPY

		ldy STORE+16
		ldx STORE+15
		bcc @+
			lda STORE+14	; 256
			sta CBUFSRC	; 256
			lda STORE+13	; 256
			sta CBUFFER	; 256
@
		lda STORE+12
		sta SRC+1		; Restore MSB
		lda STORE+11
		sta SRC			; Restore LSB
		lda STORE+10
		sta BANK		; Restore BANK
		rts
;-----------------------------------------------------------------------		
; Set Source by POSx4 value
SETPOSSRC
		ldx POS
		lda tabbnk,X	; BANK ->A
		sta BANK
		lda tabahi,X	; MSB ->Y
		and #$1f
		ora #$a0
		sta SRC+1
		lda tabalo,X	; LSB ->X
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
IncSrc		inc SRC
		bne CheckSrc
		inc SRC+1
		bit SRC+1
		bvc CheckSrc
		lda #$A0
		sta SRC+1
		inc BANK
CheckSrc
		; Cmp Source
		ldx POS
		lda tabalo+1,X
		cmp SRC
		bne @+
		lda tabahi+1,X
		and #$1f
		ora #$a0
		cmp SRC+1
		bne @+
		lda tabbnk+1,X
		and #$7f
		cmp BANK
@		
		rts
		
;-----------------------------------------------------------------------		
; Set Destination
;SetDst	stx DST
;		sty DST+1
;		rts

;-----------------------------------------------------------------------		
; Inc Destination
;IncDst	inc DST
;		bne @+
;		inc DST+1
;@		rts

;-----------------------------------------------------------------------		
; Cmp Destination
CmpDst		lda DST
		cmp CNT
		bne @+
		lda DST+1
		cmp CNT+1
@		rts

;-----------------------------------------------------------------------		
;-----------------------------------------------------------------------
		icl "apldecr.asm"
		icl "zx0decr.asm"
		
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
