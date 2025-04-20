; Make sure we can use this outside
.export _arith16_coord_array

; Zero page variables
.segment "ZEROPAGE"
base_ptr:   .res 2      ; Pointer to start of 16-bit coordinate values block
coord_count: .res 1     ; Number of 16-bit coordinates to process
X_val:.res 0            ; Callers write their value to add/subtract here
X_lo:   .res 1          ; Low byte of constant value to add/subtract
X_hi:   .res 1          ; High byte of constant value
Y_val:.res 0            ; Callers write their value to add/subtract here
Y_lo:   .res 1          ; Low byte of constant value to add/subtract
Y_hi:   .res 1          ; High byte of constant value

_base_ptr = base_ptr
.export _base_ptr
_coord_count = coord_count
.export _coord_count
_X_val = X_val
.export _X_val
_Y_val = Y_val
.export _Y_val

.segment "CODE"

; Inputs:
;   base_ptr = pointer to first 16-bit word
;   const_lo/const_hi = 16-bit constant
;   X = number of 16-bit words to process
; Carry flag:
;   CLC = addition
;   SEC = subtraction
;
; Modifies: A, X, Y

.proc _arith16_coord_array
        ldx coord_count     ; Load number of coords to process
        cpx #0              ; Check if we have any values to process
        beq done            ; No values to process

        ldy #0              ; X will be our byte offset (2 bytes per word)

loop:
        ; Translate X
        clc                 ; Clear carry for addition

        lda (base_ptr),y    ; Load low byte of current word
        adc X_lo            ; Add/subtract low constant
        sta (base_ptr),y    ; Store result low byte

        iny                 ; Move to high byte
        lda (base_ptr),y    ; Load high byte
        adc X_hi            ; Add/subtract high constant + carry
        sta (base_ptr),y    ; Store result high byte

        ; Translate Y
        iny                 ; Move to next word (2 bytes forward)

        clc                 ; Clear carry for addition

        lda (base_ptr),y    ; Load low byte of current word
        adc Y_lo            ; Add/subtract low constant
        sta (base_ptr),y    ; Store result low byte

        iny                 ; Move to high byte
        lda (base_ptr),y    ; Load high byte
        adc Y_hi            ; Add/subtract high constant + carry
        sta (base_ptr),y    ; Store result high byte

        iny                 ; Move to next word (2 bytes forward)
        dex
        bne loop

done:
        rts
.endproc