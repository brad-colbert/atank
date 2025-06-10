; (C) 2025 Brad Colbert

; 32bit copy
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

; Copy 24-bit value from SRC to DEST
.macro cpy24 SRC,DEST
    lda SRC+0
    sta DEST+0
    lda SRC+1
    sta DEST+1
    lda SRC+2
    sta DEST+2
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

; 16bit lsr
.macro lsr16 MEM
        LSR MEM+1       ;Shift the MSB
        ROR MEM+0       ;Rotate the LSB
.endmacro
