.export _setPixel     ; Provide memory location and bitidx
.export _setPixelXY   ; Provide on screen X and Y
.export _setPixelXYmask  ; Provide on screen X and Y and bitmask (for horizontal lines)
.export _XORLine   ; Export for CC65
.export _calcLeftMask ; Export for CC65
.export _calcRightMask ; Export for CC65

.import popa
.import _framebuffer ; Import framebuffer variable
FB_ADDR = _framebuffer ; alias without the _
;.import _FBLUT_HI ; Import framebuffer variable
;FBLUT_HI = _FBLUT_HI ; alias without the _
;.import _FBLUT_LO ; Import framebuffer variable
;FBLUT_LO = _FBLUT_LO ; alias without the _

; Define Constants
;FB_ADDR = framebuffer    ; Change this if the framebuffer is at a different location
SCREEN_WIDTH = 40        ; 320 pixels wide (40 bytes per row)

.rodata

; Precomputed bitmasks
BitMasks:
    .byte $80, $40, $20, $10, $08, $04, $02, $01
    ;, $C0, $E0, $F0, $F8, $FC, $FE, $FF, $7F, $3F, $1F, $0F, $07, $03
    ;.byte $01, $02, $04, $08, $10, $20, $40, $80

; .segment "ZEROPAGE"
X1:       .res 2
Y1:       .res 2
X2:       .res 2
Y2:       .res 2

.segment "FBLUTHI"
FBLUT_HI:   .res 192
_FBLUT_HI = FBLUT_HI
.export _FBLUT_HI
.segment "FBLUTLO"
_FBLUT_LO = FBLUT_LO
FBLUT_LO:   .res 192
.export _FBLUT_LO

.zeropage
FBLINE:  .res 2
XX:      .res 2
YY:      .res 1
BMIDX:   .res 1


.code

.macro lsr16 MEM
        LSR MEM+1       ;Shift the MSB
        ROR MEM+0       ;Rotate the LSB
.endmacro


; This will calculate a mask for the left most byte of the line.
; A should contain the bit index of the starting pixel.
.proc _calcLeftMask
    STA BMIDX                ; Store the bit (pixel location) number to BMIDX
    LDA #07                  ; Set A to 7.  We will subtract the bit index from this to get the amount to shift.
    SEC                      ; Set the carry flag so that the SBC will properly subtract
    SBC BMIDX                ; Subtract the bit index from 7 to get the amount to shift
    TAX                      ; Transfer A (the amount to shift) to X
    LDA #00                  ; Initialize A
    INX                      ; Increment X to get the bitmask for the right most pixel
rotate_bit_to_left:
    SEC                      ; Set the carry flag so that the ROL will fill the bits.
    ROL A                    ; Shift the bit in A to the left
    DEX                      ; Decrement the amount to shift
    BNE rotate_bit_to_left   ; If not done, loop

    RTS
.endproc

; This will calculate a mask for the right most byte of the line.
; A should contain the bit index of the starting pixel.
.proc _calcRightMask
    TAX                      ; Transfer A to X
    LDA #00                  ; Initialize A
    INX                      ; Increment X to get the bitmask for the right most pixel
rotate_bit_to_right:
    SEC                      ; Set the carry flag so that the ROR will fill the bits.
    ROR A                    ; Shift the bit in A to the right
    DEX
    BNE rotate_bit_to_right   ; If not done, loop

    RTS
.endproc

; Set the memory location pointed to by X,Y to the bitmask
.proc _setPixelXYmask
    ; Pull arguments from the stack (CC65 calling convention)

    ; Get the bitmask (byte) stored in BMIDX
    jsr popa 
    sta BMIDX

    ; Get the Y coordinate
    jsr popa
    sta YY

    ; Get X which is just a byte
    jsr popa 
    sta XX

    ; Use the LUT to get the framebuffer line address
    ldy YY
    LDA FBLUT_LO,y
    STA FBLINE
    LDA FBLUT_HI,y
    STA FBLINE+1

    ; Add the offset in XX to the low byte of the line
    LDA FBLINE
    CLC
    ADC XX
    STA FBLINE
    LDA FBLINE+1
    ADC #00
    STA FBLINE+1

    LDA BMIDX                ; Get the bitmask from the temp store
    LDY #00                  ; Set Y to 0 for no offset
    EOR (FBLINE),y           ; XOR the bit with the framebuffer
    STA (FBLINE),y           ; Store the result back in the framebuffer

    RTS
.endproc

.proc _setPixelXY
    ; Pull arguments from the stack (CC65 calling convention)
    jsr popa
    sta YY

    jsr popa  ; low byte
    sta XX
    ; Low byte of X1 is still in A, Calculate the BITMASK index from the low bits of the address, store for the BITMASK index
    and #07
    sta BMIDX
    jsr popa  ; high byte
    sta XX+1

    ; Shift XX so that the low byte contains the byte offset from the address low byte LUT
    ldy YY
    LDA FBLUT_LO,y
    STA FBLINE
    LDA FBLUT_HI,y
    STA FBLINE+1

    ; Divide XX by 8, getting to the byte offset for the line
    lsr16 XX
    lsr16 XX
    lsr16 XX
    ; Add the offset in XX to the low byte of the line
    LDA FBLINE
    CLC
    ADC XX
    STA FBLINE
    LDA FBLINE+1
    ADC XX+1
    STA FBLINE+1

    ldy BMIDX
    lda BitMasks,y
    ldy #00
    eor (FBLINE),y
    sta (FBLINE),y

    rts
.endproc


.proc _setPixel
    ; Pull arguments from the stack (CC65 calling convention)
    ; jsr popa
    ; sta YY

    jsr popa
    sta XX
    ; Low byte of X1 is still in A, Calculate the BITMASK index from the low bits of the address, store for the BITMASK index
    and #07
    sta BMIDX
    jsr popa
    sta XX+1

    pha
    tya
    pha
    ldy BMIDX
    lda BitMasks,y
    ; ldy X1
    ;ldx X1
    ldy #00
    eor (XX),y
    sta (XX),y
    pla
    tay
    pla

    rts
.endproc

.proc _XORLine
    ; Pull arguments from the stack (CC65 calling convention)
    jsr popa
    sta Y2
    jsr popa
    sta Y2+1

    jsr popa
    sta X2
    jsr popa
    sta X2+1
    
    jsr popa
    sta Y1
    jsr popa
    sta Y1+1

    jsr popa
    sta X1
    jsr popa
    sta X1+1

    ; Check for Horizontal Line (Y1 == Y2)
    LDA Y1
    CMP Y2
    BNE check_vertical
    LDA Y1+1
    CMP Y2+1
    BEQ draw_horizontal

check_vertical:
    ; Check for Vertical Line (X1 == X2)
    LDA X1
    CMP X2
    BNE check_diagonal
    LDA X1+1
    CMP X2+1
    BEQ draw_vertical

check_diagonal:
    SEC
    LDA X2
    SBC X1
    BCC neg_dx
    STA $10      ; DX (Positive)
    JMP dx_done
neg_dx:
    EOR #$FF
    CLC
    ADC #1
    STA $10      ; DX (Absolute Value)
dx_done:

    SEC
    LDA Y2
    SBC Y1
    BCC neg_dy
    STA $12      ; DY (Positive)
    JMP dy_done
neg_dy:
    EOR #$FF
    CLC
    ADC #1
    STA $12      ; DY (Absolute Value)
dy_done:

    ; Now compare absolute DX == absolute DY
    LDA $10
    CMP $12
    BNE fallback  ; If not equal, go to fallback
    JMP draw_diagonal  ; Otherwise, draw 45-degree line

    ; If no match, fallback to Bresenham (slow path)
fallback:
    ;JSR BresenhamLine
    RTS

;----------------------------------------------------------
; Draw Fast Horizontal Line
;----------------------------------------------------------
draw_horizontal:
    LDA X1
    LSR
    LSR
    LSR  ; Convert to byte offset
    STA $12  ; Byte position
    LDA X1+1
    ADC #0
    STA $13

    ; Compute row start
    LDA Y1
    LDX SCREEN_WIDTH  ; Multiply Y1 by SCREEN_WIDTH
    JSR Multiply
    CLC
    ADC #<FB_ADDR
    STA $14
    LDA #>FB_ADDR
    ADC $15
    STA $15

    ; XOR entire byte range
    LDX X2
    LSR
    LSR
    LSR
    STX $16

loop_horizontal:
    LDY #0
    LDA ($14),Y
    EOR #$FF
    STA ($14),Y
    INC $14
    CPX $16
    BNE loop_horizontal
    RTS

;----------------------------------------------------------
; Draw Fast Vertical Line
;----------------------------------------------------------
draw_vertical:
    ; Compute column bit position
    LDA X1
    AND #7
    TAX
    LDA BitMasks,X
    STA $12

    ; Compute column byte position
    LDA X1
    LSR
    LSR
    LSR  ; X / 8
    STA $13

    ; Compute starting memory location
    LDA Y1
    LDX SCREEN_WIDTH
    JSR Multiply
    CLC
    ADC #<FB_ADDR
    STA $14
    LDA #>FB_ADDR
    ADC $15
    STA $15

loop_vertical:
    LDY #0
    LDA ($14),Y
    EOR $12
    STA ($14),Y

    ; Move down one row
    CLC
    LDA $14
    ADC #SCREEN_WIDTH
    STA $14
    LDA $15
    ADC #0
    STA $15

    LDA Y1
    CMP Y2
    BNE loop_vertical
    RTS

;----------------------------------------------------------
; Draw Fast 45-degree Diagonal Line
;----------------------------------------------------------
draw_diagonal:
    ; Compute start address
    LDA Y1
    LDX SCREEN_WIDTH
    JSR Multiply
    CLC
    ADC #<FB_ADDR
    STA $14
    LDA #>FB_ADDR
    ADC $15
    STA $15

    ; Compute bit position
    LDA X1
    AND #7
    TAX
    LDA BitMasks,X
    STA $12

    ; Compute byte offset
    LDA X1
    LSR
    LSR
    LSR  ; X / 8
    CLC
    ADC $14
    STA $14
    LDA $15
    ADC #0
    STA $15

loop_diagonal:
    LDY #0
    LDA ($14),Y
    EOR $12
    STA ($14),Y

    ; Move diagonally
    LDA X1
    CLC
    ADC #1
    STA X1
    LDA Y1
    CLC
    ADC #1
    STA Y1

    ; Move to next byte if necessary
    LDA X1
    AND #7
    TAX
    LDA BitMasks,X
    STA $12
    LDA X1
    LSR
    LSR
    LSR
    CLC
    ADC $14
    STA $14
    LDA $15
    ADC #0
    STA $15

    ; Loop until done
    LDA X1
    CMP X2
    BNE loop_diagonal
    RTS
.endproc



;----------------------------------------------------------
; Multiplication Routine (A * X -> 16-bit result in $14/$15)
;----------------------------------------------------------
.proc Multiply
    STA $14
    LDA #0
    STA $15
    TAY    ; Transfer A to Y
    BEQ done

loop_multiply:
    CLC
    LDA $14
    TXA    ; Transfer X to A
    ADC $14  ; Add A (now X) to $14 (low byte of result)
    STA $14
    LDA $15
    ADC #0
    STA $15
    DEY
    BNE loop_multiply
done:
    RTS
.endproc
