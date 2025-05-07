_calcLeftMask = calcLeftMask
.export _calcLeftMask ; Export for CC65
_calcRightMask = calcRightMask
.export _calcRightMask ; Export for CC65
.export _draw_line
.export _translate_clip_draw_all_lines

.import popa
.import _clip
.import _framebuffer ; Import framebuffer variable
FB_ADDR = _framebuffer ; alias without the _

; Define Constants
SCREEN_WIDTH = 40        ; 320 pixels wide (40 bytes per row)

; READ ONLY DATA
.rodata
; Precomputed bitmasks
BitMasks:
    .byte $80, $40, $20, $10, $08, $04, $02, $01
_BitMasks = BitMasks
.export _BitMasks
; 8x8 bitmask for left and right edges and single byte line (L * 8 + R) or (x1 * 8 + x2)
LR_TABLE: .byte $80, $C0, $E0, $F0, $F8, $FC, $FE, $FF,  $00, $40, $60, $70, $78, $7C, $7E, $7F,  $00, $00, $20, $30, $38, $3C, $3E, $3F,  $00, $00, $00, $10, $18, $1C, $1E, $1F,  $00, $00, $00, $00, $08, $0C, $0E, $0F,  $00, $00, $00, $00, $00, $04, $06, $07,  $00, $00, $00, $00, $00, $00, $02, $03,  $00, $00, $00, $00, $00, $00, $00, $01

; ZEROPAGE
.zeropage
FBLINE:  .res 2
_FBLINE = FBLINE
.export _FBLINE

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
.import _X1_16
X1_16 = _X1_16
.import _Y1_16
Y1_16 = _Y1_16
.import _X2_16
X2_16 = _X2_16
.import _Y2_16
Y2_16 = _Y2_16

.segment "FBLUTHI"
FBLUT_HI:   .res 192
_FBLUT_HI = FBLUT_HI
.export _FBLUT_HI
.segment "FBLUTLO"
FBLUT_LO:   .res 192
_FBLUT_LO = FBLUT_LO
.export _FBLUT_LO

.import _X_val
X_val = _X_val
.import _Y_val
Y_val = _Y_val

; DATA
.data
.import _X_val
.import _Y_val
.import _SWAP
SWAP = _SWAP

line_count: .res 1      ; Number of 16-bit coordinates to process
_line_count = line_count
.export _line_count
;.segment "LINE_COORDS"
;line_coords: .res 256    ; 16-bit coordinate values block.  32 lines. 2 coordinates per line. 2 components per coordinate. 2 bytes per component.
;_line_coords = line_coords
;.export _line_coords
.import _lines
line_coords = _lines

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
        lda Y1                  ; Load Y1 into the accumulator
        tay                     ; Copy Y1 to Y
        cmp Y2                  ; Compare with Y2
        bne vertical            ; If equal, must be a horizontal line.  Jump to vertical_line if they aren't equal
        jsr horizontal_line     ; Call the horizontal line function
        jmp done

vertical:        
        ; Test if we have a vertical line
        lda X1                  ; Load X1 into the accumulator
        cmp X2                  ; Compare with X2
        bne diagonal            ; If equal, must be a vertical line.  Jump to diagonal_line if they aren't equal
        jsr vertical_line       ; Call the horizontal line function
        jmp done

diagonal:
        jsr diagonal_line
        
done:        
        rts
.endproc

.proc horizontal_line
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

        jmp done

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
        and #$07                ; Get the lower 3 bits by masking off the upper bits.
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

done:
        rts
.endproc

.proc vertical_line
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

.proc diagonal_line
        ; Diagonal line (Bresenham's line algorithm)
        rts
.endproc

.proc _translate_clip_draw_all_lines
;_line_coords = line_coords
;.export _line_coords
        ldx line_count      ; Load the number of lines to process
        beq done            ; No values to process

        ldy #0              ; X will be our byte offset (2 bytes per word)
loop:
        ; Translate X1
        clc                 ; Clear carry for addition

        lda line_coords,y   ; Load low byte of current word
        adc X_val           ; Add/subtract low constant
        sta X1_16           ; Store result low byte

        iny                 ; Move to high byte
        lda line_coords,y   ; Load high byte
        adc X_val+1         ; Add/subtract high constant + carry
        sta X1_16+1         ; Store result high byte

        ; Translate Y1
        clc                 ; Clear carry for addition

        iny                 ; Move to next word (2 bytes forward)
        lda line_coords,y   ; Load low byte of current word
        adc Y_val           ; Add/subtract low constant
        sta Y1_16           ; Store result low byte

        iny                 ; Move to high byte
        lda line_coords,y   ; Load high byte
        adc Y_val+1         ; Add/subtract high constant + carry
        sta Y1_16+1         ; Store result high byte

        ; Translate X2
        clc                 ; Clear carry for addition

        iny                 ; Move to next word (2 bytes forward)
        lda line_coords,y   ; Load low byte of current word
        adc X_val           ; Add/subtract low constant
        sta X2_16           ; Store result low byte

        iny                 ; Move to high byte
        lda line_coords,y   ; Load high byte
        adc X_val+1         ; Add/subtract high constant + carry
        sta X2_16+1         ; Store result high byte

        ; Translate Y2
        clc                 ; Clear carry for addition

        iny                 ; Move to next word (2 bytes forward)
        lda line_coords,y   ; Load low byte of current word
        adc Y_val           ; Add/subtract low constant
        sta Y2_16           ; Store result low byte

        iny                 ; Move to high byte
        lda line_coords,y   ; Load high byte
        adc Y_val+1         ; Add/subtract high constant + carry
        sta Y2_16+1         ; Store result high byte

        iny                 ; Move to next word (2 bytes forward)

        ; Now clip the line.
        ; We have to save the state of the X and Y registers before calling the clip function.
        tya                 ; Copy Y to A
        pha                 ; Push Y to stack
        txa                 ; Copy X to A
        pha                 ; Push X to stack
        clc                 ; Clear the carry
        jsr _clip           ; Call the clip function

        ; If the carry flag is set, the line was not on the screen.
        bcs carry_set       ; Branch if carry is set.  Don't draw the line.

        ; Test if the coordinates were swapped by the clip function.
        lda SWAP            ; Load the swap flag
        beq just_draw_it    ; Branch if carry is set.  Don't draw the line.

        ; Perform the swap
        ldx X1              ; Load X1 into the X
        ldy X2              ; Load X2 into the Y
        txa                 ; Copy X1 to A
        sta X2              ; Store X1 in X2
        tya                 ; Copy X2 to A
        sta X1              ; Store X2 in X1

        ldx Y1              ; Load Y1 into the X
        ldy Y2              ; Load Y2 into the Y
        txa                 ; Copy Y1 to A
        sta Y2              ; Store Y1 in Y2
        tya                 ; Copy Y2 to A
        sta Y1              ; Store Y2 in Y1

just_draw_it:
        ; Now we can draw the line.
        jsr _draw_line
        
carry_set:
        ; Restore the X and Y registers from the stack
        pla                 ; Pull X from stack
        tax                 ; Copy back to X
        pla                 ; Pull Y from stack
        tay                 ; Copy back to Y

        dex
        bne loop
done:
        rts
.endproc