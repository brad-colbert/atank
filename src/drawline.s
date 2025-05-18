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

.import _map_center
map_center = _map_center
.import _map_center_prev
map_center_prev = _map_center_prev

.define X_val map_center
.define Y_val map_center+2
.define X_val_prev map_center_prev
.define Y_val_prev map_center_prev+2

; DATA
.data
.import _FBLUT_LO
FBLUT_LO = _FBLUT_LO
.import _FBLUT_HI
FBLUT_HI = _FBLUT_HI

.import _SWAP
SWAP = _SWAP

.import _line_count
line_count = _line_count

.import _lines        ; 16-bit coordinate values block.  32 lines. 2 coordinates per line. 2 components per coordinate. 2 bytes per component.
line_coords = _lines

; This stores the result of the clipping and are used to erase the old lines.
.segment "LINE_ERASE_COORDS"
clipped_line_coords_a: .res 128  ; 32 lines. 2 coordinates per line. 2 components per coordinate. 1 bytes per component.
clipped_line_coords_b: .res 128  ; 32 lines. 2 coordinates per line. 2 components per coordinate. 1 bytes per component.
.data
clipped_line_coords_count_a: .res 1
clipped_line_coords_count_b: .res 1
clipped_line_coords_which: .res 1
.export clipped_line_coords_a
.export clipped_line_coords_b
.export clipped_line_coords_count_a
.export clipped_line_coords_count_b
.export clipped_line_coords_which

.code

.macro save_y
        tya                 ; Copy Y to A
        pha                 ; Push Y to stack
.endmacro
.macro restore_y
        pla                 ; Pull Y from stack
        tay                 ; Copy back to Y
.endmacro
.macro save_x
        txa                 ; Copy X to A
        pha                 ; Push X to stack
.endmacro
.macro restore_x
        pla                 ; Pull X from stack
        tax                 ; Copy back to X
.endmacro
.macro save_xy
        save_x
        save_y
.endmacro
.macro restore_xy
        restore_y
        restore_x
.endmacro

.macro lsr16 MEM
        LSR MEM+1       ;Shift the MSB
        ROR MEM+0       ;Rotate the LSB
.endmacro

.macro cmp16 V1,V2
        LDA V1+1
        CMP V2+1
        BNE :+
        LDA V1+0
        CMP V2+0
:
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

.macro clear_mem address
        ldy #$7F            ; Load Y with 127
        lda #$00                ; Load the accumulator with 0
clear_loop:
        sta address,y             ; Store the result back in the framebuffer
        dey                     ; Decrement Y
        bne clear_loop               ; Loop until we are done
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

.proc draw_line
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
.export draw_line

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
        ldx line_count      ; Load the number of lines to process
        bne cont            ; There are lines to process
        rts

cont:
        ldy #0              ; X will be our byte offset (2 bytes per word)

loop:
        ; Translate line coordinates based on player position
        jsr translate_line

        iny                 ; Move to next word (1 bytes forward)

        ; Now clip the line.
        ; We have to save the state of the X and Y registers before calling the clip function.
        save_xy
        clc                 ; Clear the carry
        jsr _clip           ; Call the clip function

        ; If the carry flag is set, the line was not on the screen.
        bcs carry_set       ; Branch if carry is set.  Don't draw the line.

        ; Test if the coordinates were swapped by the clip function.
        lda SWAP            ; Load the swap flag
        beq just_draw_it    ; If SWAP is zero, we don't need to swap the coordinates.
        
        ; Perform the swap
        save_xy
        jsr swap_coords
        restore_xy

just_draw_it:
        ; Now we can draw the line.
        save_xy
        jsr draw_line
        restore_xy

        ; Store these coordinates so we can erase the lines later.
        save_xy
        jsr store_coords
        restore_xy

carry_set:
        ; Restore the X and Y registers from the stack
        restore_xy

        dex
        bne loop

just_erase_previous_lines:
        ; Erase the previous lines
        save_xy
        ; Test code to see if we are getting coordinates out of order.
        jsr erase_previous_lines
        restore_xy

        ; Swap the buffers
swap_coord_buffers:
        ; Swap the buffers
        lda #$01            ; The toggle value
        eor clipped_line_coords_which
        sta clipped_line_coords_which ; Store the new buffer to use

done:
        rts
.endproc

.export translate_line
.proc translate_line
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

        rts
.endproc

.export swap_coords
.proc swap_coords
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
        rts
.endproc

.export test_coords
.proc test_coords
        ; Test code to see if we are getting coordinates out of order.
        lda X2                  ; Load X1 into the accumulator
        cmp X1                  ; Compare with X2
        bcc error
        lda Y2                  ; Load Y1 into the accumulator
        cmp Y1                  ; Compare with Y2
        bcc error

        rts

error:
        nop                     ; Something easy to find and break on
        rts
.endproc

.export store_coords
.proc store_coords
        ; Store the clipped line coordinates in the appropriate buffer.
        lda clipped_line_coords_which ; Load the current buffer to use
        bne call_store_b    ; If zero, store in buffer A

        ; Store in buffer A
        jsr store_a
        jmp done

call_store_b:
        ; Store in buffer B
        jsr store_b

done:
        rts
.endproc
.proc store_a
        ldy clipped_line_coords_count_a ; Load the count of clipped coordinates components
        lda X1              ; Load X1 into the accumulator
        sta clipped_line_coords_a,y ; Store X1 in the buffer
        iny                 ; Increment Y to the next byte
        lda Y1              ; Load Y1 into the accumulator
        sta clipped_line_coords_a,y ; Store X2 in the buffer
        iny                 ; Increment Y to the next byte
        lda X2              ; Load X2 into the accumulator
        sta clipped_line_coords_a,y ; Store Y1 in the buffer
        iny                 ; Increment Y to the next byte
        lda Y2              ; Load Y2 into the accumulator
        sta clipped_line_coords_a,y ; Store Y2 in the buffer
        iny                 ; Increment Y to the next byte
        tya                 ; Copy Y to A
        sta clipped_line_coords_count_a ; Store the count of clipped coordinates components
        rts
.endproc
.proc store_b
        ldy clipped_line_coords_count_b ; Load the count of clipped coordinates components
        lda X1              ; Load X1 into the accumulator
        sta clipped_line_coords_b,y ; Store X1 in the buffer
        iny                 ; Increment Y to the next byte
        lda Y1              ; Load X2 into the accumulator
        sta clipped_line_coords_b,y ; Store X2 in the buffer
        iny                 ; Increment Y to the next byte
        lda X2              ; Load Y1 into the accumulator
        sta clipped_line_coords_b,y ; Store Y1 in the buffer
        iny                 ; Increment Y to the next byte
        lda Y2              ; Load Y2 into the accumulator
        sta clipped_line_coords_b,y ; Store Y2 in the buffer
        iny                 ; Increment Y to the next byte
        tya                 ; Copy Y to A
        sta clipped_line_coords_count_b ; Store the count of clipped coordinates components
        rts
.endproc

.export erase_previous_lines
.proc erase_previous_lines
        ; Check which buffer to use
        lda clipped_line_coords_which ; Load the current buffer to use
        bne erase_a         ; Erase B if A is active, and vice versa
        ; Erase buffer B
        jsr erase_b
done:
        rts
.endproc
.proc erase_a
        lda clipped_line_coords_count_a ; Load the count of clipped lines
        beq done           ; If zero we are done

        ldy #0             ; Clear Y to use as an index
loop:
        lda clipped_line_coords_a,y ; Load X1 into the accumulator
        sta X1             ; Update X1
        iny                 ; Increment Y to the next byte
        lda clipped_line_coords_a,y ; Load Y1 into the accumulator
        sta Y1             ; Update Y1
        iny                 ; Increment Y to the next byte
        lda clipped_line_coords_a,y ; Load X2 into the accumulator
        sta X2             ; Update X2
        iny                 ; Increment Y to the next byte
        lda clipped_line_coords_a,y ; Load Y2 into the accumulator
        sta Y2             ; Update Y2
        iny                 ; Increment Y to the next byte

        save_y
        jsr draw_line ; Draw the line
        restore_y

        cpy clipped_line_coords_count_a  ; Compare with the count of clipped lines
        bne loop            ; If not equal, jump to loop

done:
        ; Clear the count of clipped lines
        lda #0
        sta clipped_line_coords_count_a ; Store the count of clipped lines

        rts
.endproc
.proc erase_b
        lda clipped_line_coords_count_b ; Load the count of clipped lines
        beq done           ; If zero we are done

        ldy #0             ; Clear Y to use as an index
loop:
        lda clipped_line_coords_b,y ; Load X1 into the accumulator
        sta X1             ; Update X1
        iny                 ; Increment Y to the next byte
        lda clipped_line_coords_b,y ; Load Y1 into the accumulator
        sta Y1             ; Update Y1
        iny                 ; Increment Y to the next byte
        lda clipped_line_coords_b,y ; Load X2 into the accumulator
        sta X2             ; Update X2
        iny                 ; Increment Y to the next byte
        lda clipped_line_coords_b,y ; Load Y2 into the accumulator
        sta Y2             ; Update Y2
        iny                 ; Increment Y to the next byte

        save_y
        jsr draw_line      ; Draw the line
        restore_y

        cpy clipped_line_coords_count_b  ; Compare with the count of clipped lines
        bne loop            ; If not equal, jump to loop
done:
        ; Clear the count of clipped lines
        lda #0
        sta clipped_line_coords_count_b ; Store the count of clipped lines

        rts
.endproc
