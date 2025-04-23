.export _setPixel     ; Provide memory location and bitidx
.export _setPixelXY   ; Provide on screen X and Y
.export _setPixelXYmask  ; Provide on screen X and Y and bitmask (for horizontal lines)
.export _XORLine_16   ; Export for CC65
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
_BitMasks = BitMasks
.export _BitMasks

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
TEMP:    .res 0
TEMP_A:  .res 1
TEMP_B:  .res 1
TEMP_C:  .res 1
TEMP_D:  .res 1
TEMP_E:  .res 1
DX:      .res 1

.data
LM_TABLE: .byte $FF, $7F, $3F, $1F, $0F, $07, $03, $01
RM_TABLE: .byte $80, $C0, $E0, $F0, $F8, $FC, $FE, $FF

.code

.macro lsr16 MEM
        LSR MEM+1       ;Shift the MSB
        ROR MEM+0       ;Rotate the LSB
.endmacro


; This will calculate a mask for the left most byte of the line.
; A should contain the bit index of the starting pixel.
.proc _calcLeftMask
    TAX
    LDA LM_TABLE,X

    RTS
.endproc

; This will calculate a mask for the right most byte of the line.
; A should contain the bit index of the starting pixel.
.proc _calcRightMask
    TAX
    LDA RM_TABLE,X

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
    ;LDA FBLINE+1
    ;ADC #00
    ;STA FBLINE+1
    bcc :+
    inc FBLINE+1
:

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
    ;LDA FBLINE+1
    ;ADC XX+1
    ;STA FBLINE+1
    bcc :+
    inc FBLINE+1
:

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

;----------------------------------------------------------
; _XORLine_16: Draws a line using XOR operation.
; Optimizes for horizontal, vertical, and 45-degree diagonal lines.
; Uses 16-bit coordinates passed via stack (CC65 calling convention).
;----------------------------------------------------------
.proc _XORLine_16

    ;----------------------------------------
    ; Pull arguments from the C call stack (right to left)
    ; Format: _XORLine_16(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2)
    ;----------------------------------------
    jsr popa          ; Pop low byte of Y2
    sta Y2

    jsr popa          ; Pop high byte of Y2
    sta Y2+1

    jsr popa          ; Pop low byte of X2
    sta X2

    jsr popa          ; Pop high byte of X2
    sta X2+1

    jsr popa          ; Pop low byte of Y1
    sta Y1

    jsr popa          ; Pop high byte of Y1
    sta Y1+1

    jsr popa          ; Pop low byte of X1
    sta X1

    jsr popa          ; Pop high byte of X1
    sta X1+1

    ;----------------------------------------
    ; Check if the line is perfectly horizontal: Y1 == Y2
    ;----------------------------------------
    LDA Y1            ; Load low byte of Y1
    CMP Y2            ; Compare to low byte of Y2
    BNE check_vertical ; If not equal, it's not horizontal

    LDA Y1+1          ; Load high byte of Y1
    CMP Y2+1          ; Compare to high byte of Y2
    BEQ draw_horizontal ; If equal, jump to fast horizontal drawing

check_vertical:
    ;----------------------------------------
    ; Check if the line is perfectly vertical: X1 == X2
    ;----------------------------------------
    LDA X1            ; Load low byte of X1
    CMP X2            ; Compare to low byte of X2
    BNE check_diagonal ; If not equal, it's not vertical

    LDA X1+1          ; Load high byte of X1
    CMP X2+1          ; Compare to high byte of X2
    BEQ draw_vertical ; If equal, jump to fast vertical drawing

check_diagonal:
    ;----------------------------------------
    ; Calculate absolute DX = |X2 - X1|
    ;----------------------------------------
    SEC               ; Set carry before subtraction
    LDA X2            ; Load low byte of X2
    SBC X1            ; Subtract low byte of X1 (X2 - X1)
    BCC neg_dx        ; If result is negative, go handle sign

    STA DX           ; Store absolute value of DX
    JMP dx_done       ; Skip negative handling

neg_dx:
    EOR #$FF          ; Invert bits
    CLC
    ADC #1            ; Add 1 → Two's complement to get absolute value
    STA DX           ; Store absolute value of DX

dx_done:
    ;----------------------------------------
    ; Calculate absolute DY = |Y2 - Y1|
    ;----------------------------------------
    SEC               ; Set carry before subtraction
    LDA Y2            ; Load low byte of Y2
    SBC Y1            ; Subtract low byte of Y1 (Y2 - Y1)
    BCC neg_dy        ; If result is negative, handle sign

    STA TEMP_A           ; Store absolute value of DY
    JMP dy_done       ; Skip negative handling

neg_dy:
    EOR #$FF          ; Invert bits
    CLC
    ADC #1            ; Add 1 → Two's complement to get absolute value
    STA TEMP_A           ; Store absolute value of DY

dy_done:
    ;----------------------------------------
    ; Check if absolute DX == DY (i.e. 45-degree diagonal)
    ;----------------------------------------
    LDA DX           ; Load absolute DX
    CMP TEMP_A           ; Compare to absolute DY
    BNE fallback      ; If not equal, use fallback (Bresenham)
    JMP draw_diagonal ; If equal, draw optimized diagonal line

fallback:
    ;----------------------------------------
    ; Fallback for arbitrary lines (non-optimized case)
    ; e.g., Bresenham's algorithm would go here
    ;----------------------------------------
    ;JSR BresenhamLine ; (currently disabled/stubbed)
    RTS               ; Return without drawing (no-op if fallback not implemented)


;----------------------------------------------------------
; Draw Fast Horizontal Line (XORs bytes from X1 to X2 on row Y1)
; Assumes each screen byte represents 8 horizontal pixels
;----------------------------------------------------------
draw_horizontal:

    LDA X1            ; Load low byte of start X coordinate
    LSR               ; Shift right 3 times to divide by 8 (bit → byte)
    LSR
    LSR               ; Now A holds byte offset from X1
    STA TEMP_A           ; Store byte offset low into temporary location $12

    LDA X1+1          ; Load high byte of X1
    ADC #0            ; Add carry if any from shifting (likely unnecessary)
    STA TEMP_B           ; Store byte offset high into TEMP_B

    ;----------------------------------------------------------
    ; Compute the starting address of the row in screen memory
    ; row_offset = Y1 * SCREEN_WIDTH
    ;----------------------------------------------------------
    LDA Y1            ; Load row number (Y1)
    LDX #SCREEN_WIDTH ; Load screen width (bytes per row) into X
    JSR Multiply      ; Call 8-bit multiply: result in A (low), and TEMP_D (high)

    CLC               ; Clear carry before address addition
    ADC #<FB_ADDR     ; Add framebuffer base address (low byte)
    STA TEMP_C           ; Store resulting address low byte in TEMP_C

    LDA #>FB_ADDR     ; Load high byte of framebuffer address
    ADC TEMP_D           ; Add high byte of row offset (from Multiply)
    STA TEMP_D           ; Store resulting address high byte in TEMP_D
                      ; Now (TEMP_C),Y points to the start of the target row

    ;----------------------------------------------------------
    ; Compute byte offset of X2 (end X), also converted to byte index
    ;----------------------------------------------------------
    LDX X2            ; Load low byte of X2 (end X position)
    LSR               ; Divide X2 by 8 to convert bit → byte
    LSR
    LSR
    STX TEMP_E        ; Store byte offset of X2 in TEMP_E

;----------------------------------------------------------
; Loop to XOR each byte between X1 and X2
;----------------------------------------------------------
loop_horizontal:

    LDY #0            ; Set Y to 0 to dereference (TEMP_C),Y address

    LDA (TEMP_C),Y    ; Load current screen byte from framebuffer row
    EOR #$FF          ; XOR with 0xFF → inverts all 8 pixels in the byte
    STA (TEMP_C),Y    ; Write modified byte back to framebuffer

    INC TEMP_C        ; Move to next byte on the row
                      ; (assumes line doesn't cross page boundary)

    CPX TEMP_E        ; Compare current X byte offset with end offset
    BNE loop_horizontal ; Loop if not reached the end yet

    RTS               ; Return from subroutine


;----------------------------------------------------------
; Draw Fast Vertical Line
; XORs a single pixel column from Y1 to Y2
; Assumes 1-bit per pixel packed in bytes (8 pixels/byte horizontally)
;----------------------------------------------------------
draw_vertical:

    ;----------------------------------------------------------
    ; Compute column bit mask for X1
    ;----------------------------------------------------------
    LDA X1             ; Load the low byte of X1 (horizontal position)
    AND #7             ; Mask to get bit index within the byte (0–7)
    TAX                ; Use as index into bitmask table
    LDA BitMasks,X     ; Load bitmask (e.g., %10000000 for bit 0, etc.)
    STA $12            ; Store mask in $12 for pixel toggling

    ;----------------------------------------------------------
    ; Compute column byte offset = X1 / 8
    ;----------------------------------------------------------
    LDA X1             ; Reload X1
    LSR                ; Divide by 8 by shifting right 3 times
    LSR
    LSR
    STA TEMP_B         ; Store byte offset (horizontal byte column index)

    ;----------------------------------------------------------
    ; Compute starting address for row Y1
    ; result = Y1 * SCREEN_WIDTH + FB_ADDR
    ;----------------------------------------------------------
    LDA Y1             ; Load Y1 (row index)
    LDX SCREEN_WIDTH   ; Load screen width in bytes
    JSR Multiply       ; Multiply Y1 * SCREEN_WIDTH
                       ; Result: A = low byte, TEMP_D = high byte

    CLC
    ADC #<FB_ADDR      ; Add low byte of framebuffer base address
    STA TEMP_C         ; Store low byte of screen address

    LDA #>FB_ADDR      ; Load high byte of framebuffer base
    ADC TEMP_D         ; Add high byte of offset
    STA TEMP_D         ; Store high byte of screen address
                       ; TEMP_C:TEMP_D now points to screen memory for (X1,Y1)

loop_vertical:
    ;----------------------------------------------------------
    ; XOR one pixel in column (toggle bit at current row)
    ;----------------------------------------------------------
    LDY #0             ; Y = 0 to use (ZP),Y indirect addressing
    LDA (TEMP_C),Y        ; Load byte at current screen memory address
    EOR $12            ; XOR with bitmask (flip only the target bit)
    STA (TEMP_C),Y        ; Store modified byte back

    ;----------------------------------------------------------
    ; Move to the next row (add SCREEN_WIDTH bytes)
    ;----------------------------------------------------------
    CLC
    LDA TEMP_C            ; Load current address low byte
    ADC #SCREEN_WIDTH  ; Add screen width to move down one row
    STA TEMP_C            ; Store updated low byte

    LDA TEMP_D            ; Load current address high byte
    ADC #0             ; Add carry if needed (no extra high byte from SCREEN_WIDTH)
    STA TEMP_D            ; Store updated high byte

    ;----------------------------------------------------------
    ; Check if we've reached Y2
    ;----------------------------------------------------------
    LDA Y1             ; Load current Y position
    CMP Y2             ; Compare to target Y2
    BNE loop_vertical  ; If not equal, repeat loop (move down)

    RTS                ; Done drawing vertical line


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
    STA TEMP_C
    LDA #>FB_ADDR
    ADC TEMP_D
    STA TEMP_D

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
    ADC TEMP_C
    STA TEMP_C
    LDA TEMP_D
    ADC #0
    STA TEMP_D

loop_diagonal:
    LDY #0
    LDA (TEMP_C),Y
    EOR $12
    STA (TEMP_C),Y

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
    ADC TEMP_C
    STA TEMP_C
    LDA TEMP_D
    ADC #0
    STA TEMP_D

    ; Loop until done
    LDA X1
    CMP X2
    BNE loop_diagonal
    RTS
.endproc


;----------------------------------------------------------
; Multiplication Routine (A * X -> 16-bit result in TEMP_C/TEMP_D)
;----------------------------------------------------------
.proc Multiply
    STA TEMP_C
    LDA #0
    STA TEMP_D
    TAY    ; Transfer A to Y
    BEQ done

loop_multiply:
    CLC
    LDA TEMP_C
    TXA    ; Transfer X to A
    ADC TEMP_C  ; Add A (now X) to TEMP_C (low byte of result)
    STA TEMP_C
    LDA TEMP_D
    ADC #0
    STA TEMP_D
    DEY
    BNE loop_multiply
done:
    RTS
.endproc
