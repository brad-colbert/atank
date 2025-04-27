_calcLeftMask = calcLeftMask
.export _calcLeftMask ; Export for CC65
_calcRightMask = calcRightMask
.export _calcRightMask ; Export for CC65
.export _draw_line

.import popa
.import _framebuffer ; Import framebuffer variable
FB_ADDR = _framebuffer ; alias without the _

; Define Constants
SCREEN_WIDTH = 40        ; 320 pixels wide (40 bytes per row)

.rodata

; Precomputed bitmasks
BitMasks:
    .byte $80, $40, $20, $10, $08, $04, $02, $01
_BitMasks = BitMasks
.export _BitMasks

.import _X1
X1 = _X1
.import _Y1
Y1 = _Y1
.import _X2
X2 = _X2
.import _Y2
Y2 = _Y2
MASK = _Y2 + 1

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
_FBLINE = FBLINE
.export _FBLINE

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
.proc calcLeftMask
    TAX
    LDA LM_TABLE,X

    RTS
.endproc

; This will calculate a mask for the right most byte of the line.
; A should contain the bit index of the starting pixel.
.proc calcRightMask
    TAX
    LDA RM_TABLE,X

    RTS
.endproc

.proc _draw_line
        ; Test if we have a horizontal line
        lda Y1                  ; Load Y1 into the accumulator
        cmp Y2                  ; Compare with Y2
        bne vertical_line       ; If equal, must be a horizontal line.  Jump to vertical_line if they aren't equal

        ; Divide X coords by 8.  There are 8 pixels per byte.
        lda X2                  ; Load X2 into the accumulator
        lsr A                   ; Divide by 2
        lsr A                   ; Divide by 4
        lsr A                   ; Divide by 8
        sta X2                  ; Store the result back to X2
        lda X1                  ; Load X1 into the accumulator
        lsr A                   ; Divide by 2
        lsr A                   ; Divide by 4
        lsr A                   ; Divide by 8
        sta X1                  ; Store the result back to X1

        ; Draw the left most pixel.
        and #$07                ; Mask off the lower 3 bits
        jsr calcLeftMask        ; Calculate the left mask
        sta MASK                ; Store the mask to XX
        ldy Y1                  ; Load Y1 into Y
        ldx X1                  ; Load X1 into the accumulator
        jsr set_pixel_from_mask ; Set the pixel from the mask

        ; Draw the middle pixels
        ldy Y1                  ; Load Y1 into Y
        lda FBLUT_LO,y
        sta FBLINE
        lda FBLUT_HI,y
        sta FBLINE+1
        clc
        adc FBLINE              ; Add the framebuffer line address to the X screen location
        sta FBLINE              ; Store the result back in the framebuffer line address
        lda #$00                ; Load the accumulator with 0
        bcc :+                  ; Branch if carry set
        adc FBLINE+1
        sta FBLINE+1
:
        lda X2                  ; Load X2 into the accumulator
        sbc X1                  ; Subtract X1 from X2, this is the number of bytes to draw, but it has to be two less
        tay                     ; Store the result in Y
        dey                     ; Decrement Y to get the number of bytes to draw
        dey                     ; Decrement Y to get the number of bytes to draw
line_loop:        
        lda #$FF                ; Load the accumulator with 0xFF
        sta (FBLINE),y          ; Store the result back in the framebuffer
        dey
        bne line_loop           ; Loop until we are done

        ; Draw the right most pixel
        lda X2                  ; Load X2 into the accumulator
        and #$07                ; Mask off the lower 3 bits
        jsr calcRightMask       ; Calculate the left mask
        sta MASK                ; Store the mask
        ldy Y1                  ; Load Y1 into Y
        ldx X1                  ; Load X1 into the accumulator
        jsr set_pixel_from_mask ; Set the pixel from the mask

        jmp done

vertical_line:
        ; Test if we have a vertical line
        lda X1                  ; Load X1 into the accumulator
        cmp X2                  ; Compare with X2
        bne diagonal_line       ; If equal, must be a vertical line.  Jump to diagonal_line if they aren't equal

        ; Calculate the bit mask.  A will have the X1 component in it.
        tay                     ; Copy original value of X1 which is in A to Y, so we don't have to load it again.
        and #$07                ; Get the lower 3 bits by masking off the upper bits.
        tax                     ; Copy the lower 3 bits to X to use as an index into the bit mask table
        lda BitMasks,x          ; Load the bit mask from the table
        sta MASK                ; Store the mask to MASK
        tya                     ; Copy the original value of X1, which is currently in Y, to A

        ; MASK now has the bit mask for the pixel to light up on the line/byte we are drawing.
        ; The byte address for the first pixel is calculated in the following section.

        ; Calculate the x byte offset.  Position / 8 (8 bits per byte).
        lsr                     ; Divide by 2
        lsr                     ; Divide by 4
        lsr                     ; Divide by 8

        ; Look up the framebuffer line address
        ; Register A now holds which byte of the line we are drawing.
        clc                     ; Clear the carry, just in case
        ldy Y1                  ; Load the starting y coordingate, Y1, into Y
        adc FBLUT_LO,y          ; Add the X screen location (byte) (stored in A) to the framebuffer line address
        sta FBLINE              ; Store the result back in the framebuffer line address
        lda #$00                ; Load the accumulator with 0
        adc FBLUT_HI,y          ; Add the framebuffer line address to the X screen location.  If there was a carry, increment the high byte.
        sta FBLINE+1            ; Store the result back in the framebuffer line address storage

        ; FBLINE now has the framebuffer line address with the X (byte) offset added to it.
        ; We should be able to just add 40 to FBLINE each iteration to get the next line address.

        ; Calculate the number of lines to draw a pixel on (delta Y, Y2 - Y1)
        clc                     ; Clear the carry
        lda Y2                  ; Load Y2 into the accumulator
        sbc Y1                  ; Subtract Y1 from Y2
        tax                     ; Copy Y2-Y1 to X

        ; Begin setup for the loop.  X holds the number of lines to draw a pixel on. Y is clear for the indirect lookups.

        ldy #$00                ; Clear Y

vert_loop: ; X now holds the number of lines to draw a pixel on.
        lda MASK                ; Load the mask into A
        eor (FBLINE),y          ; XOR the bit with the framebuffer
        sta (FBLINE),y          ; Store the result back in the framebuffer

        ; Set FBLINE to point to the next line which will be 40 bytes from here.
        clc                     ; Clear the carry
        lda #$28                ; Load the accumulator with 40 (bytes)
        adc FBLINE              ; Calculate the address (lo byte) of the next line
        sta FBLINE              ; Store the result back in the framebuffer line address
        bcc no_carry
        lda #$00                ; Load the accumulator with 0.  If the Carry was set, this will increment the high byte.
        adc FBLINE+1            ; Add the framebuffer line address to the X screen location.  If there was a carry, increment the high byte.
        sta FBLINE+1            ; Store the result back in the framebuffer line address
        and #$0F                ; Here we begin to test is we crossed a 4K boundary. Check the lower 4 bits of the address to see if they are zero.
        bne no_zero             ; We are testing if we have crossed a 4K boundary (ANTIC address issue).  If we have, we need to add 16 more bytes to the lower address.
        clc                     ; Clear the carry
        lda #$10                ; Get ready to add 16 bytes to the lower address
        adc FBLINE              ; Add the 16 bytes to the lower address
        sta FBLINE              ; Store the result back in the framebuffer line address

no_carry:
no_zero:
        dex                     ; Decrement x to update the remaining number of lines to draw
        bne vert_loop           ; Loop until we are done

        rts
        
diagonal_line:
        ; Diagonal line (Bresenham's line algorithm)
done:        
        rts
.endproc

; Set Y to the framebuffer line address
; Set X to the pixel offset in the line
; Set XX to the bitmask
.proc set_pixel_from_mask
        ; Use the LUT to get the framebuffer line address
        lda FBLUT_LO,y
        sta FBLINE
        lda FBLUT_HI,y
        sta FBLINE+1

        ; Add the offset in XX to the low byte of the line
        ;lda FBLINE
        ;clc
        ;adc XX
        ;sta FBLINE
        ;bcc :+
        ;inc FBLINE+1

:
        txa                      ; Transfer the X screen location (byte) to A
        tay                      ; Transfer the X screen location (byte) to Y
        lda MASK
        eor (FBLINE),y           ; XOR the bit with the framebuffer
        sta (FBLINE),y           ; Store the result back in the framebuffer

        rts
.endproc
