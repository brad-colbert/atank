; (C) 2025 Brad Colbert

; Includes
.include "memory.inc"
.include "atari.inc"

; Constants
SRC_OFFSET = 1
DST_OFFSET = 3
NUM_BYTES_OFFSET = 0

; Imports
.import popa
.importzp sp

; Exports
.export _memcpyXOR

; Memory
.zeropage
addr1: .word 0 ; Temporary address for source
addr2: .word 0 ; Temporary address for destination

.code

; Per (https://cc65.github.io/doc/cc65-intern.html#ss2.1) the stack should have all of our
; arguments.  This will be a cdecl function.
; (sp) and (sp)+1 should contain argument 1, destination address
; (sp)+2 and (sp)+3 should contain argument 2, source address
; (sp)+4 should contain argument 3, number of bytes to xor_copy (255 max).
.proc _memcpyXOR
    ldy #NUM_BYTES_OFFSET ; Start with the number of bytes to copy
    lda (sp),y           ; Load the number of bytes to copy
    beq done             ; If zero, nothing to copy, exit

    tax                  ; Transfer count to X for use as a counter

    ; Load the source and destination addresses from the stack
    ldy #SRC_OFFSET     ; Load the source data offset
    lda (sp),y          ; Load the source address
    sta addr1+0         ; Store low byte of source address
    iny
    lda (sp),y          ; Load the high byte of the source address
    sta addr1+1         ; Store high byte of source address
    ldy #DST_OFFSET     ; Load destination offset
    lda (sp),y          ; Load the destination address
    sta addr2+0         ; Store low byte of destination address
    iny
    lda (sp),y          ; Load the high byte of the destination address
    sta addr2+1         ; Store high byte of destination address

    ; Now we can use the addresses in addr1 and addr2 for the XOR operation
    dex
    txa
    tay
loop:
    lda (addr1),y        ; Load the value to be exored to
    eor (addr2),y        ; XOR the value at the source address with the value at the destination address
    sta (addr2),y        ; Store the result back to the destination address

    dey
    bpl loop             ; If not done, continue looping

done:
    rts
.endproc