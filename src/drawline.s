_calcLeftMask = calcLeftMask
.export _calcLeftMask ; Export for CC65
_calcRightMask = calcRightMask
.export _calcRightMask ; Export for CC65
.export _draw_line

.import popa
.import _framebuffer ; Import framebuffer variable
FB_ADDR = _framebuffer ; alias without the _
;.segment "FRAMEBUFFER"
;.framebuffer: .res 40*192
;_framebuffer = framebuffer
;.export _framebuffer
;FB_ADDR = framebuffer

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
MASK = Y2 + 1
TEMP1_BYTE = Y2 + 1
TEMP2_BYTE = Y2 + 2

.segment "FBLUTHI"
FBLUT_HI:   .res 192
_FBLUT_HI = FBLUT_HI
.export _FBLUT_HI
.segment "FBLUTLO"
FBLUT_LO:   .res 192
_FBLUT_LO = FBLUT_LO
.export _FBLUT_LO

.zeropage
FBLINE:  .res 2
_FBLINE = FBLINE
.export _FBLINE

.data
;LM_TABLE: .byte $FF, $7F, $3F, $1F, $0F, $07, $03, $01
;RM_TABLE: .byte $80, $C0, $E0, $F0, $F8, $FC, $FE, $FF
; 8x8 bitmask for left and right edges and single byte line (L * 8 + R) or (x1 * 8 + x2)
LR_TABLE: .byte $80, $C0, $E0, $F0, $F8, $FC, $FE, $FF,  $00, $40, $60, $70, $78, $7C, $7E, $7F,  $00, $00, $20, $30, $38, $3C, $3E, $3F,  $00, $00, $00, $10, $18, $1C, $1E, $1F,  $00, $00, $00, $00, $08, $0C, $0E, $0F,  $00, $00, $00, $00, $00, $04, $06, $07,  $00, $00, $00, $00, $00, $00, $02, $03,  $00, $00, $00, $00, $00, $00, $00, $01

.code

.macro lsr16 MEM
        LSR MEM+1       ;Shift the MSB
        ROR MEM+0       ;Rotate the LSB
.endmacro

.macro inc_fbline
        inc FBLINE
        bcc :+
        inc FBLINE+1
        clc
:
.endmacro

.macro multiply_by_8
        asl
        asl
        asl
.endmacro

.macro divide_by_8
        lsr
        lsr
        lsr
.endmacro

; Y must contain the line to select the address for.
.macro load_fbline
        lda FBLUT_LO,y          ; Add the X screen location (byte) (stored in A) to the framebuffer line address
        sta FBLINE              ; Store the result back in the framebuffer line address
        lda FBLUT_HI,y          ; Add the framebuffer line address to the X screen location.  If there was a carry, increment the high byte.
        sta FBLINE+1            ; Store the result back in the framebuffer line address storage
.endmacro

; A must contain the byte offset to use.
.macro offset_fbline
        beq :+                  ; If the result is zero, skip the addition
        adc FBLINE              ; Add the framebuffer line address to the X screen location
        sta FBLINE              ; Store the result back in the framebuffer line address
        bcc :+
        inc FBLINE+1            ; Increment the high byte if there was a carry
        clc                     ; Clear the carry
:
.endmacro

; A should have the byte we want to write.  FBLINE should be set to the memory location to write to.
.macro write_pixel
        eor (FBLINE),y          ; XOR the bit with the framebuffer
        sta (FBLINE),y          ; Store the result back in the framebuffer
.endmacro

; This will calculate a mask for the left most byte of the line.
; A should contain X1
.proc calcLeftMask
    ;TAX
    ;LDA LM_TABLE,X
    multiply_by_8
    clc                         ; Clear the carry
    adc #$07                    ; Add 7 to get the correct index
    tax                         ; Store the result in X
    lda LR_TABLE, x             ; Load the left mask from the table\

    rts                         ; Return
.endproc

; This will calculate a mask for the right most byte of the line.
; A should contain X2
.proc calcRightMask
    ;TAX
    ;LDA RM_TABLE,X
    tax                         ; Copy X2 to X
    lda LR_TABLE, x             ; Load the left mask from the table

    rts                         ; Return
.endproc

; A has X1, result is in A
.proc calc_mask
    asl                         ; Shift left to multiply by 2
    asl                         ; Shift left to multiply by 4
    asl                         ; Shift left to multiply by 8
    clc                         ; Clear the carry
    adc X2                      ; Add X2 to get the correct index
    tax                         ; Store the result in X
    lda LR_TABLE, x             ; Load the left mask from the table

    rts                         ; Return
.endproc

.proc _draw_line
        ; Test if we have a horizontal line
        ;lda Y1                  ; Load Y1 into the accumulator
        ;tay                     ; Copy Y1 to Y
        ;cmp Y2                  ; Compare with Y2
        ;bne vertical_line       ; If equal, must be a horizontal line.  Jump to vertical_line if they aren't equal

horizontal_line:
        ; First let's see how many bytes we need to draw.
        lda X1                  ; Load X1 into the accumulator
        divide_by_8             ; Divide by 8 to get the byte offset
        sta TEMP1_BYTE          ; Store the X1 byte offset in TEMP1_BYTE
        tax                     ; Copy the result to X
        lda X2                  ; Load X2 into the accumulator
        divide_by_8             ; Divide by 8 to get the byte offset
        tay                     ; Copy the result to Y

        ; Subtract X1 byte from X2 byte to get the number of bytes to draw
        sec                     ; Set the carry
        sbc TEMP1_BYTE          ; Subtract X1 from X2
        bne multi_byte_draw     ; If not zero it's more than one byte, so jump to multi_byte_draw

single_byte_draw:
        clc                     ; Clear the carry
        lda #$07                ; Load mask into the accumulator
        and X2                  ; And with X2 to get the lower 3 bits by masking off the upper bits.
        sta X2                  ; Store the result back in X2
        lda #$07                ; Load mask into the accumulator
        and X1                  ; And with X1 to get the lower 3 bits by masking off the upper bits.
        jsr calc_mask           ; Calculate the bit mask.  A will have the X1 component in it.
        tax                     ; Copy the the mask to X

        ; Load up FBLINE with the framebuffer line address
        load_fbline
        
        ;  Probably can optimize this by combining the operations above into one.
        lda X1
        divide_by_8
        offset_fbline

        txa                     ; Transfer the mask to A
        write_pixel

multi_byte_draw:
        ; A contains the number of bytes to draw.  We need to save this for the middle pixels.
        sta TEMP1_BYTE           ; Store the number of bytes to draw in TEMP_BYTE.  Overwriting X1 that is in TEMP1_BYTE.

        ; Calculate the FBLINE address for the first byte
        tya                     ; Save Y to temp
        sta TEMP2_BYTE          ; Store X2 in bytes to TEMP2_BYTE
        lda Y1                  ; Load Y1 into the accumulator
        tay                     ; Copy Y1 to Y

        load_fbline

        txa                     ; Transfer X1 byte position to A
        tay                     ; Copy X1 byte position to Y

        ; Calculate and write the left mask.
        ; We will load A with X1, which is the left most pixel. 
        lda X1                  ; Load X1 into the accumulator
        jsr calcLeftMask
        write_pixel             ; Y has the X1 byte address

        ; Loop to write the middle pixels.  We already wrote the left pixels so there will be one less.
        lda TEMP1_BYTE          ; Load the number of bytes to draw into A
        tax                     ; Copy the number of bytes to draw to X
        dex                     ; Decrement the number of bytes to draw since we already wrote one
        beq :+                  ; If there are no more bytes to draw, skip the loop
horizonal_loop:
        lda #$FF                ; Load the accumulator with FF.  This is a full byte line.
        iny                     ; Increment Y to get the next byte address
        write_pixel
        dex                     ; Decrement the number of bytes to draw
        bne horizonal_loop      ; Loop until we are done
:        
        ; Calculate and write the right pixels.
        lda TEMP2_BYTE          ; Load X2 into the accumulator
        tay                     ; Copy X2 to Y
        lda X2                  ; Load X2 into the accumulator
        and #$07                ; Get the lower 3 bits by masking off the upper bits.
        jsr calcRightMask
        write_pixel             ; Y has the X2 byte address

        jmp done
        
diagonal_line:
        ; Diagonal line (Bresenham's line algorithm)
done:        
        rts
.endproc

.proc vertical_line
        ; Test if we have a vertical line
        ;lda X1                  ; Load X1 into the accumulator
        ;cmp X2                  ; Compare with X2
        ;bne diagonal_line       ; If equal, must be a vertical line.  Jump to diagonal_line if they aren't equal

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
        sec                     ; Set the carry
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
        inc FBLINE+1

no_carry:
no_zero:
        dex                     ; Decrement x to update the remaining number of lines to draw
        bne vert_loop           ; Loop until we are done

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
