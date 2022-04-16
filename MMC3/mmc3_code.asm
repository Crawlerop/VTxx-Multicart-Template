.define A12_INVERT $80

.segment "ZEROPAGE"
	;A12_INVERT: .res 1
	BP_BANK_8000: .res 1
	BP_BANK_A000: .res 1
    mmc3_8000_PRG:	.res 1
	mmc3_8001_PRG:	.res 1
	mmc3_8000_CHR:	.res 1
	mmc3_8001_CHR:	.res 1
	mmc3_ptr:		.res 2 ; array for the irq parser
	mmc3_index:		.res 1 ; index to this array
	irq_done:		.res 1	
	
    ;.exportzp A12_INVERT, BP_BANK_8000, mmc3_8000_PRG, mmc3_8001_PRG, mmc3_8000_CHR, mmc3_8001_CHR
	.exportzp BP_BANK_8000, BP_BANK_A000, mmc3_8000_PRG, mmc3_8001_PRG, mmc3_8000_CHR, mmc3_8001_CHR
	.exportzp mmc3_ptr, mmc3_index, irq_done
	;.export _A12_INVERT = A12_INVERT
		
.segment "STARTUP"
;needs to be mapped to the fixed bank

.export _set_prg_8000, _get_prg_8000, _set_prg_a000, _get_prg_a000
.export _set_chr_mode_0, _set_chr_mode_1, _set_chr_mode_2, _set_chr_mode_3
.export _set_chr_mode_4, _set_chr_mode_5

.export _set_mirroring, _set_wram_mode, _disable_irq
.export _set_irq_ptr, _is_irq_done

;VTxx extensions
.export _set_extended_gfx, _set_extended_gfx_params, _invert_c

_invert_c:		
	;sta A12_INVERT
	rts

; sets the bank at $8000-9fff
; only changes the A register
_set_prg_8000:
    sta BP_BANK_8000
	sta mmc3_8001_PRG
	lda #(6 | A12_INVERT)
	sta mmc3_8000_PRG
	jmp safe_bank_swapping


; returns the current bank at $8000-9fff	
_get_prg_8000:
    lda BP_BANK_8000
	ldx #0
    rts

_get_prg_a000:
    lda BP_BANK_A000
	ldx #0
    rts
	
	
; sets the bank at $a000-bfff
_set_prg_a000:
	sta BP_BANK_A000
	sta mmc3_8001_PRG
	lda #(7 | A12_INVERT)
	sta mmc3_8000_PRG
	jmp safe_bank_swapping


; only changes the A register, all these
; see chart in README for what these do
_set_chr_mode_0:
	sta mmc3_8001_CHR
	lda #(0 | A12_INVERT)
	sta mmc3_8000_CHR
	jmp safe_bank_swapping

	
_set_chr_mode_1:
	sta mmc3_8001_CHR
	lda #(1 | A12_INVERT)
	sta mmc3_8000_CHR
	jmp safe_bank_swapping

	
_set_chr_mode_2:
	sta mmc3_8001_CHR
	lda #(2 | A12_INVERT)
	sta mmc3_8000_CHR
	jmp safe_bank_swapping

	
_set_chr_mode_3:
	sta mmc3_8001_CHR
	lda #(3 | A12_INVERT)
	sta mmc3_8000_CHR
	jmp safe_bank_swapping

	
_set_chr_mode_4:
	sta mmc3_8001_CHR
	lda #(4 | A12_INVERT)
	sta mmc3_8000_CHR
	jmp safe_bank_swapping

	
_set_chr_mode_5:
	sta mmc3_8001_CHR
	lda #(5 | A12_INVERT)
	sta mmc3_8000_CHR
	jmp safe_bank_swapping

	
	
	
	
; borrowing idea (but not code) from tepples/pinobatch
; because the CHR bank swap and PRG bank swap use the same
; register, the IRQ code and the MAIN code might try to
; use this register at the same time, and conflict	
; thus we need redundant writes to make sure that
; both go through.
;
; This only works if the MAIN code only ever changes
; the PRG bank and the IRQ code only ever changes the
; CHR banks.
; However, the main code can safely do CHR changes if  
; the IRQ system is not changing CHR
;

safe_bank_swapping:
    ; Begin Bankswitch
	tya
	pha

	; CHR decode type
	lda #A12_INVERT	
	sta $4105	

	; CHR bankswitch
	lda mmc3_8000_CHR	
	and #$7f
	tay
	lda mmc3_8001_CHR
	cpy #$02
	bcs @bs_2 ; If Greather than 2
	@bs_1:
	sta $2016,y
	jmp @bs_p
	@bs_2:
	sta $2010,y
	@bs_p:
	
	; PRG bankswitch
	lda mmc3_8000_PRG	
	and #$7f
	tay
	lda mmc3_8001_PRG
	sta $4101,y
	
	pla
	tay
	; End Bankswitch
	rts
	
	
; MIRROR_VERTICAL 0
; MIRROR_HORIZONTAL 1	
_set_mirroring:
    ;eor #$01
	sta $4106
	rts
	
	
; WRAM_OFF $40
; WRAM_ON $80
; WRAM_READ_ONLY $C0
_set_wram_mode:
	sta $a001
	rts
		
_set_extended_gfx:
	pha
	cmp #$01
	bne @2
@1:
	lda #%10000110	
	jmp @3
@2:
	lda #$00
@3:
	sta $2010	
	pla
	sta GFX_MODE
	rts

_set_extended_gfx_params:
	pha
	sta $2010
	and #$80
	beq @store_gfx	
	lda #$01
@store_gfx:
	sta GFX_MODE
	pla
@store_gfx_ext:
	lsr a
	lsr a
	lsr a
	and #$03	
	sta VT_XGFX_MODE
	rts

_disable_irq:
	sta $4103 ;any value
	lda #<default_array
	ldx #>default_array
	;jmp _set_irq_ptr ; fall through
	
	
_set_irq_ptr:
;ax = pointer
	sta mmc3_ptr
	stx mmc3_ptr+1
	rts	
	
	
_is_irq_done:
	lda irq_done
	ldx #0
	rts

	
default_array: ;just an eof terminator
.byte $ff



	
irq:
	pha
	txa
	pha
	tya
	pha
	
	sta $4103	; disable mmc3 irq
				; any value will do
	
	jsr irq_parser
	
	pla
	tay
	pla
	tax
	pla
	rti
	
	
;format
;value < 0xf0, it's a scanline count
;zero is valid, it triggers an IRQ at the end of the current line

;if >= 0xf0...
;f0 = 2000 write, next byte is write value
;f1 = 2001 write, next byte is write value
;f2-f4 unused - future TODO ?
;f5 = 2005 write, next byte is H Scroll value
;f6 = 2006 write, next 2 bytes are write values


;f7 = change CHR mode 0, next byte is write value
;f8 = change CHR mode 1, next byte is write value
;f9 = change CHR mode 2, next byte is write value
;fa = change CHR mode 3, next byte is write value
;fb = change CHR mode 4, next byte is write value
;fc = change CHR mode 5, next byte is write value

;fd = very short wait, no following byte 
;fe = short wait, next byte is quick loop value
;(for fine tuning timing of things)

;ff = end of data set

	
irq_parser:
	ldy mmc3_index
;	ldx #0
@loop:
	lda (mmc3_ptr), y ; get value from array
	iny
	cmp #$fd ;very short wait
	beq @loop
	
	cmp #$fe ;fe-ff wait or exit
	bcs @wait
	
	cmp #$f0
	bcs @1
	jmp @scanline ;below f0
@1:	
	
	cmp #$f7
	bcs @chr_change
;f0-f6	
	tax
	lda (mmc3_ptr), y ; get value from array
	iny
	cpx #$f0
	bne @2
	sta $2000 ; f0
	jmp @loop
@2:
	cpx #$f1
	bne @3
	sta $2001 ; f1
	jmp @loop
@3:
	cpx #$f5 
	bne @4
	ldx #4
@better_timing: ; don't change till near the end of the line
	dex
	bne @better_timing
	
	sta $2005 ; f5
	sta $2005 ; second value doesn't matter
	jmp @loop
@4:
	sta $2006 ; f6
	lda (mmc3_ptr), y ; get 2nd value from array
	iny	
	sta $2006
	jmp @loop
	
@wait: ; fe-ff wait or exit
	cmp #$ff
	beq @exit	
	lda (mmc3_ptr), y ; get value from array
	iny
	tax
	beq @loop ; if zero, just exit
@wait_loop: ; the timing of this wait could change if this crosses a page boundary
	dex
	bne @wait_loop		
	jmp @loop	

@chr_change:
;f7-fc change a CHR set
	sec
	sbc #$f7 ;should result in 0-5
	ora #A12_INVERT
	sta mmc3_8000_CHR
	lda (mmc3_ptr), y ; get next value
	iny
	sta mmc3_8001_CHR
	jsr safe_bank_swapping
	jmp @loop
	
@scanline:
	nop ;trying to improve stability
	nop
	nop
	nop
	jsr set_scanline_count ;this terminates the set
	sty mmc3_index
	rts
	
@exit:
	sta irq_done ;value 0xff
	dey ; undo the previous iny, keep it pointed to ff
	sty mmc3_index
	rts
	
	
set_scanline_count:
; any value will do for most of these registers
	sta $4103 ; disable mmc3 irq
	sta $4101 ; set the irq counter reload value
	sta $4102 ; reload the reload value
	sta $4104 ; enable the mmc3 irq
    cli ;make sure irqs are enabled
	rts


	

	


	
	
	