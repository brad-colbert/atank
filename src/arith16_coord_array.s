; Make sure we can use this outside
.export _arith16_coord_array

; Zero page variables
.segment "ZEROPAGE"
X_val:      .res 0       ; Callers write their value to add/subtract here
X_lo:        .res 1      ; Low byte of constant value to add/subtract
X_hi:        .res 1      ; High byte of constant value
Y_val:      .res 0       ; Callers write their value to add/subtract here
Y_lo:        .res 1      ; Low byte of constant value to add/subtract
Y_hi:        .res 1      ; High byte of constant value

X_val_prev:  .res 2      ; Previous player position for X
Y_val_prev:  .res 2      ; Previous player position for Y

_X_val = X_val
.export _X_val
_Y_val = Y_val
.export _Y_val
_X_val_prev = X_val_prev
.export _X_val_prev
_Y_val_prev = Y_val_prev
.export _Y_val_prev

.data
.import _line_count
line_count = _line_count
.import _lines
line_coords = _lines

.segment "CODE"

; Inputs:
;   base_ptr = pointer to first 16-bit word
;   X_lo/X_hi,Y_lo/Y_hi = 16-bit constant
;   line_count = number of coords to process (up to 64)
; Carry flag:
;   CLC = addition
;   SEC = subtraction
;
; Modifies: A, X, Y

.proc _arith16_coord_array
        lda line_count     ; Load number of coords to process
        asl                ; Multiply by 2, 2 coords per line
        tax                ; Store in X for loop counter
        beq done            ; No values to process

        ldy #0              ; X will be our byte offset (2 bytes per word)

loop:
        ; Translate X
        clc                 ; Clear carry for addition

        lda line_coords,y    ; Load low byte of current word
        adc X_lo            ; Add/subtract low constant
        sta line_coords,y    ; Store result low byte

        iny                 ; Move to high byte
        lda line_coords,y    ; Load high byte
        adc X_hi            ; Add/subtract high constant + carry
        sta line_coords,y    ; Store result high byte

        ; Translate Y
        clc                 ; Clear carry for addition

        iny                 ; Move to next word (2 bytes forward)
        lda line_coords,y    ; Load low byte of current word
        adc Y_lo            ; Add/subtract low constant
        sta line_coords,y    ; Store result low byte

        iny                 ; Move to high byte
        lda line_coords,y    ; Load high byte
        adc Y_hi            ; Add/subtract high constant + carry
        sta line_coords,y    ; Store result high byte

        iny                 ; Move to next word (2 bytes forward)
        dex
        bne loop

done:
        rts
.endproc