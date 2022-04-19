LOADROM_OFFSET = $0300
LOADROM_INIT_BITS = $FF

.define	db	.byte
.define	dw	.addr
.define dd	.faraddr
.define VBANK16(no)  .LOBYTE(no>>11)
.define VBANK4(no)  .LOBYTE(no>>10)
.define VBANK16DATA(no)  .LOBYTE(no>>11),.LOBYTE(no>>11)+1,.LOBYTE(no>>11)+2,.LOBYTE(no>>11)+3
.define VBANK4DATA(no)  .byte	.LOBYTE(no>>10),.LOBYTE(no>>10)+1,.LOBYTE(no>>10)+2,.LOBYTE(no>>10)+3

.macro	assign begin,this,num       
	.if(.LOBYTE(this - begin) & (num - 1))
	.res	(num - (.LOBYTE(this- begin) & (num - 1))),$FF
	.endif
.endmacro	
	
.macro bind_vrom vrom,bpp
db (.LOBYTE(bpp << 4) | .LOBYTE((((vrom >> 13) & $20) * 8) & $0f))
.endmacro

.macro gametable prom,prom_end,vrom,vrom_end,nrom,bpp
	
    ;.if prom_end
    ;db $01
    ;.else
    ;db $00 ; Not required for non 412c carts
    .if(VT_412C_BANKSWITCH)
    db .LOBYTE(prom>>24) & $03; 412c Carts
	db	(.LOBYTE(prom>>17) & $70) | (.LOBYTE(vrom>>21) & $07) ;for 412c systems
    .else
    .if(VT_SYSTEM_TYPE = 2)
    db  ((.LOBYTE(vrom>>25) & $01) << 1) | (.LOBYTE(prom>>25) & $01); VT32/VT33 system
    .else
    db $00
    .endif
    .if(bpp)
    db	(.LOBYTE(prom>>17) & $F0) | (.LOBYTE(vrom>>22) & $07)			;$4100, VT03
    .else
	db	(.LOBYTE(prom>>17) & $F0) | (.LOBYTE(vrom>>21) & $0F)			;$4100	
    .endif	
    .endif	
    ;db	(.LOBYTE(prom>>17) & $70) | (.LOBYTE(vrom>>21) & $07) ;for 412c systems
	db	.LOBYTE(prom>>13)
    .if	((prom_end - prom) <= $2000)
	db	.LOBYTE(nrom<<3) | $06
	.else    
	.if	((prom_end - prom) <= $4000)
	db	.LOBYTE(nrom<<3) | $05
	.else
	.if	((prom_end - prom) <= $8000)
	db	.LOBYTE(nrom<<3) | $04
	.else
	.if	((prom_end - prom) <= $10000)
	db  .LOBYTE(nrom<<3) | $03 
	.else 
	.if	((prom_end - prom) <= $20000) 
	db	.LOBYTE(nrom<<3) | $02 
	.else 
	.if	((prom_end - prom) <= $40000)
	db	.LOBYTE(nrom<<3) | $01 
	.else 
	.if	((prom_end - prom) <= $80000) 
	db	.LOBYTE(nrom<<3) | $00 
	.endif 
	.endif 
	.endif 
	.endif 
	.endif 
	.endif 
    .endif
	.if	((vrom_end - vrom) <= $2000) 	
    .if(bpp)
    db	.LOBYTE(vrom>>11)+$06 
    db	.LOBYTE((vrom>>15)&$70) 
    .else
    db	.LOBYTE(vrom>>10)+$06 
	db	.LOBYTE((vrom>>14)&$70) 
    .endif
	.else 
	.if	((vrom_end - vrom) <= $4000) 	
	.if(bpp)
    db	.LOBYTE(vrom>>11)+$05 
    db	.LOBYTE((vrom>>15)&$70) 
    .else
    db	.LOBYTE(vrom>>10)+$05 
	db	.LOBYTE((vrom>>14)&$70) 
    .endif
	.else 
	.if	((vrom_end - vrom) <= $8000) 	
	.if(bpp)
    db	.LOBYTE(vrom>>11)+$04 
    db	.LOBYTE((vrom>>15)&$70) 
    .else
    db	.LOBYTE(vrom>>10)+$04 
	db	.LOBYTE((vrom>>14)&$70) 
    .endif
	.else 
	.if	((vrom_end - vrom) <= $10000)	
	.if(bpp)
    db	.LOBYTE(vrom>>11)+$02 
    db	.LOBYTE((vrom>>15)&$70) 
    .else
    db	.LOBYTE(vrom>>10)+$02 
	db	.LOBYTE((vrom>>14)&$70) 
    .endif
	.else 
	.if	((vrom_end - vrom) <= $20000) 	
	.if(bpp)
    db	.LOBYTE(vrom>>11)+$01 
    db	.LOBYTE((vrom>>15)&$70) 
    .else
    db	.LOBYTE(vrom>>10)+$01 
	db	.LOBYTE((vrom>>14)&$70) 
    .endif
	.else 
	.if	((vrom_end - vrom) <= $40000) 	
	.if(bpp)
    db	.LOBYTE(vrom>>11)+$00 
    db	.LOBYTE((vrom>>15)&$70) 
    .else
    db	.LOBYTE(vrom>>10)+$00 
	db	.LOBYTE((vrom>>14)&$70) 
    .endif
	.endif 
	.endif 
	.endif 
	.endif 
	.endif 
	.endif 
.endmacro	

.segment "STARTUP"
    LoadSub:
        rts
    LoadGame:        
        .export _StartGame = LoadGame             
        sta $80
        stx $81         
        
        ; Load
        LDX #$00
        STX $2001

        STX $2000       
        lda #$00

        ldx #$00
        txa
        @APU_LOOP:
        STA $4000,x
        INX
        CPX #$14
        BNE @APU_LOOP

        lda #$00
        STA $4015

        ;ldy #$04
        ;jsr WAIT_VBLANK

        LDX #$00
        STX $2010
        stX $2012        

        LDA #LOADROM_INIT_BITS
        ldx #$00        
    @ClearRAM_M:
        ;sta $0000,x
        sta $0100,x
        sta $0200,x
        sta $0300,x
        sta $0400,x
        sta $0500,x
        sta $0600,x
        sta $0700,x
        inx
        bne @ClearRAM_M

        LDA #$00
        STA $2005
        STA $2005

        LDA #$00
        STA $2006
        STA $2006
    
        ; Loadd

        LDX #%00000000
        STX $4105

        ldx #$00
        STX $2010
        STX $2005
        STX $2005
        STX $2006
        STX $2005
        STX $2003
        STX $2001
        STX $2000
        STX $2011
        STX $4103  

        ldy #$04
        jsr WAIT_VBLANK      
    @0:        
        lda LOADROM,X
        STA LOADROM_OFFSET, X
        inx
        cpx #(LOADROM_END-LOADROM)
        bcc @0
        ldy #$00
    @1:
        lda ($80), y
        sta $0400, y
        iny
        cpy #$0b
        bcc @1
        ldy #$00

		; Probably the 412c bankswitch used by 620-in-1
    .if(VT_412C_BANKSWITCH)
		lda $0401

		CMP #$01
		BEQ B1
		CMP #$02
		BEQ B2
		CMP #$03
		BEQ B3
    B0:
        LDA #$00
		JMP STORE
	B1:
		LDA #%10
		JMP STORE
	B2:
		LDA #%01
		JMP STORE
	B3:
		LDA #%11
    STORE:	
		sta $0401
    .endif
		; End BS Setup

        lda $0407
        sta LOADROM_OFFSET+1
        lda $0408
        sta LOADROM_OFFSET+2

        ldx #$00        
        lda #$00
    @ClearRAM_M_2:
        sta $0000,x
        inx
        bne @ClearRAM_M_2
        
        jmp LOADROM_OFFSET

    LOADROM:
        ; Load Routines
        jsr $ffff
    @LOADROM_START:
		.if(VT_412C_BANKSWITCH)
        LDA $0401

        ; 412c bankswitch, select the x half of 32mb ROM
        LDX #$FF ; Try to set UIO to output in 412b
        STX $412B
			
		
        STA $412C; Sup 400-in-1 version
		.endif

    @LOADROM_1:
		.if(VT_412C_BANKSWITCH)
        LDA $0409
		AND #$01

        sta $4242 ; CHR RAM support was added on 412c Bankswitch mapper
		.endif
    @LOADROM_2:
		.if(VT_SYSTEM_TYPE = 2)
		LDA $0409
		
		LSR A
		LSR A
		LSR A
		LSR A

        LDX #LOADROM_INIT_BITS
		STA $00        

        LDA $0409
		AND #$01

        ASL A
        ASL A

        ORA $00
        STA $411d
        STX $00

        LDX $0401
        STX $412C

        LDA $0409
		
		LSR A
		LSR A
		LSR A
		LSR A

        CMP #$01
        BNE @LD

        ; Reset MMC1
        LDX #$ff
        STX $9FFF
        ; End Reset

        @LD:
		.endif

    ; Clear CHR-RAM
        LDA $0409
        AND #$01
        BEQ @ROMLOAD_INIT

        ;LDA #$00
        ;STA $2002
        ;STA $2002
        LDA #$00
        STA $2006
        STA $2006


        LDX #$20
        LDY #$00

    @ROM_CLR_RAM_C:
        STA $2007
        INY
        BNE @ROM_CLR_RAM_C
        DEX
        BNE @ROM_CLR_RAM_C

        LDA #$00
        STA $2006
        STA $2006

    @ROMLOAD_INIT:

        ;ldx #$96
        ;stx $2010
        ; Initialize PPU
        ;ldx $040a
        ldx #$00
        stx $2016
        inx
        inx
        stx $2017
        inx
        inx
        stx $2012
        inx
        stx $2013
        inx
        stx $2014
        inx
        stx $2015
        ; end Initialize PPU

        LDX $0406
        STX $2018 ; Intermediate Bank
        LDX $0405
        STX $201A ; CHR Size
        LDX $0400
        STX $4106 ; Mirroring
        LDX $0403
        STX $410A ; Middle Bank
        LDX $0402
        STX $4100 ; Outer
        LDX $0404
        STX $410B ; ROM Size
        LDX #$00

        STX $4107 ; Set PRG Bank
        INX
        STX $4108 ; Set PRG Bank 1
        INX
        ;STX $4109 ; Set PRG Bank 2
        ;INX
        ;STX $410a ; Set PRG Bank 3

        lda $040a
        and #$10
        beq @r_skip        
        ldx #%10000110
        stx $2010

        @r_skip:
        lda $00
        LDX #$ff

        STX $4103
        LDX #$00
        lda #LOADROM_INIT_BITS
    @LOADROM_RAMCLR_2:
        sta $0400,x
        inx        
        bne @LOADROM_RAMCLR_2

    @LOADROM_WIPE:
        ldx #$00
        lda #LOADROM_INIT_BITS
    @LOADROM_WIPE_LOOP:
        sta LOADROM_OFFSET,x
        inx
        cpx #(@LOADROM_WIPE_LOOP-LOADROM)
        bcc @LOADROM_WIPE_LOOP

        ldx #$00
		lda #$00
		ldy #$00
        
        JMP ($FFFC) ; Reset
    LOADROM_END:
    WAIT_VBLANK:        
        bit $2002
    @vblank_w:        
        lda $2002
        bpl @vblank_w
        dey
        bne WAIT_VBLANK
        rts