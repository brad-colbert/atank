; (C) 2025 Brad Colbert

.import     popa

; Constants
RTCLOK = 18  ; 19, 20

; Exports
.export _get_time_millis
.export _set_time_millis
.export _subtract32

; Macros
.macro cpy32 SRC,DEST
    lda SRC+0
    sta DEST+0
    lda SRC+1
    sta DEST+1
    lda SRC+2
    sta DEST+2
    lda SRC+3
    sta DEST+3
.endmacro

; Shift left 32-bit value in memory
.macro asl32 MEM
    asl MEM+0       ;Shift the LSB
    rol MEM+1       ;Shift one bit, put the carry at the begining
    rol MEM+2       ;Shift one bit, put the carry at the begining
    rol MEM+3       ;Shift one bit, put the carry at the begining
.endmacro

; Shift right 32-bit value in memory
.macro lsr32 MEM
    LSR MEM+3       ;Shift the MSB
    ROR MEM+2       ;Rotate the LSB
    ROR MEM+1       ;Rotate the LSB
    ROR MEM+0       ;Rotate the LSB
.endmacro

; Copy 24-bit value from SRC to DEST
.macro cpy24 SRC,DEST
    lda SRC+0
    sta DEST+0
    lda SRC+1
    sta DEST+1
    lda SRC+2
    sta DEST+2
.endmacro

; Imports
.import _time_millis

; Code
.code

; Get the current time in milliseconds
.proc _get_time_millis
    ; Copy the current JIFFY counter to our work/destination memory
    lda RTCLOK+2
    sta _time_millis+0
    lda RTCLOK+1
    sta _time_millis+1
    lda RTCLOK+0
    sta _time_millis+2


    ; Shift by 4 bits (x16) to convert JIFFY to milliseconds.  One Jiffy is 16 milliseconds.
    clc
    asl32 _time_millis
    asl32 _time_millis
    asl32 _time_millis
    asl32 _time_millis

    rts
.endproc

.proc _set_time_millis
    ; Divide the time by 16 to convert milliseconds to JIFFY
    ; This is done by shifting right by 4 bits
    lsr32 _time_millis
    lsr32 _time_millis
    lsr32 _time_millis
    lsr32 _time_millis

    ; Copy the provided time in milliseconds to the JIFFY counter
    cpy24 _time_millis, RTCLOK

    rts
.endproc

; Not used here but useful for other modules
lhv: .res 4
rhv: .res 4
_lhv = lhv
.export _lhv
.proc _subtract32
    ; Subtract 32-bit value SRC from DEST
    ; Get the command line arguments
    jsr popa
    sta rhv
    jsr popa
    sta rhv+1
    jsr popa
    sta rhv+2
    jsr popa
    sta rhv+3

    jsr popa
    sta lhv
    jsr popa
    sta lhv+1
    jsr popa
    sta lhv+2
    jsr popa
    sta lhv+3

    ; Perform the subtraction
    lda lhv+0
    sec
    sbc rhv+0
    sta lhv+0
    lda lhv+1
    sbc rhv+1
    sta lhv+1
    lda lhv+2
    sbc rhv+2
    sta lhv+2
    lda lhv+3
    sbc rhv+3
    sta lhv+3

    rts
.endproc

