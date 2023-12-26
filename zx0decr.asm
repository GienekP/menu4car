;mwa #packed_data ZX0_INPUT
.proc	dzx0_standard_decomp_blk
              lda   #$ff
              sta   offsetL
              sta   offsetH
              ldy   #$00
              sty   lenL
              sty   lenH
              lda   #$80

dzx0s_literals
              jsr   dzx0s_elias
              pha
cop0          jsr   copy_byte
              lda   lenL
              bne   @+
              dec   lenH
@             dec   lenL
              bne   cop0
              lda   lenH
              bne   cop0
              pla
              asl   @
              bcs   dzx0s_new_offset
              jsr   dzx0s_elias
dzx0s_copy    pha
              lda   DST
              clc
              adc   offsetL
              sta   RSRC
              lda   DST+1
              adc   offsetH
              sta   RSRC+1
              ldy   #$00
              ldx   lenH
              beq   Remainder
Page          jsr   copy_ram
              iny
              bne   Page
              dex
              bne   Page
Remainder     ldx   lenL
              beq   copyDone
copyByte      jsr   copy_ram
              dex
              bne   copyByte
copyDone      stx   lenH
              stx   lenL
              pla
              asl   @
              bcc   dzx0s_literals
dzx0s_new_offset
              ldx   #$fe
              stx   lenL
              jsr   dzx0s_elias_loop
              pha
 php ; stream
              ldx   lenL
              inx
              stx   offsetH
              bne   @+
 plp ; stream
              pla
              rts           ; koniec
@             jsr   get_byte
 plp ; stream
              sta   offsetL
              ror   offsetH
              ror   offsetL
              ldx   #$00
              stx   lenH
              inx
              stx   lenL
              pla
              bcs   @+
              jsr   dzx0s_elias_backtrack
@             inc   lenL
              bne   @+
              inc   lenH
@             jmp   dzx0s_copy
dzx0s_elias   inc   lenL
dzx0s_elias_loop
              asl   @
              bne   dzx0s_elias_skip
              jsr   get_byte
              sec ; stream
              rol   @
dzx0s_elias_skip
              bcc   dzx0s_elias_backtrack
              rts
dzx0s_elias_backtrack
              asl   @
              rol   lenL
              rol   lenH
              jmp   dzx0s_elias_loop

		; ram code
get_byte      clc ;1
              dta { bit.b 0 } ;1
copy_byte     sec ;1
	      php
	      JSR   GET_FROM_CAR
	      ;sta   $D500 ; set bank to this with data ; 3
              ;lda   SRC ;3
		inc SRC
		bne @+
		inc SRC+1
		bit SRC+1
		bvc @+
		pha
		lda #$A0
		sta SRC+1
		pla
		inc BANK
@
	      plp
              bcs put_byte ;2
              ;bcc _rts ;2
	      rts

copy_ram      ;sta   $D5FF ; cart off ;3
              ;lda   RSRC ;3
	      jsr   GET_RAM_BYTE
              inw   RSRC ;8

put_byte      ; never standalone 
	      jsr   PUT_RAM
              inw   DST ;8
              rts ;1
.endp

