;; Playfield utils

.export _setup_vbi
.export update_scroll

WSYNC = $D40A
_wait_wsync:
    sta WSYNC
    rts

.segment	"DATA"
_addr_temp:
	.word	$0000
_dl_index:
        .byte   $04

.segment "CODE"

;; An interupt routine for the VBI
XITVBV = $E462
SYSVBV = $E45F
HSCROL   = $d404
VSCROL   = $d405
.PROC update_scroll
        .importzp _scroll_flag
        .importzp _col
        .importzp _row
        .importzp _h_fs         ; horizontal fine scroll
        .importzp _v_fs         ; vertical fine scroll
        .import _address_lut
        .import _DISPLAY_LIST_ANTIC4

        ; check to see if the x, y are being written, skip if so
        lda _scroll_flag
        bne @exit
 
        ; Horizontal fine scroll
@fine_scroll:
        lda _h_fs
        sta HSCROL

        ; Vertical fine scroll
        lda _v_fs
        sta VSCROL

; Here we are testing if we need to perform a course scroll by
; comparing the calculated upper-left corner address to what is already
; in the first entry of the display list.  If it's different, we need
; to update everything.
@test_course_update:
        ; Calc LUT offset
        lda _row
        asl          ; shift left (multiply by 2)
        tax          ; moved index to X

        ; Add the column offset to the address.  Store in a work var.
        clc
        lda _address_lut,x
        adc _col
        sta _addr_temp

        inx
        lda _address_lut,x
        adc #$00
        sta _addr_temp+1       

        ; Compare the LUT address at the row + col (offset) with the first address in _DISPLAY_LIST_ANTIC4
        clc
        lda _addr_temp
        cmp _DISPLAY_LIST_ANTIC4+4      ; LSB of 2nd number (4th byte is MSB of LSI address)
        bne @course_scroll              ; There is a difference so let's update the display list
        lda _addr_temp+1
        cmp _DISPLAY_LIST_ANTIC4+5      ; MSB of 2nd number (5th byte is LSB of LSI address
        beq @exit

; Perform course scroll by updating the display list.
@course_scroll:
        ldy #25
        lda #04
        sta _dl_index

        lda _row
        asl          ; shift left (multiply by 2)
        tax          ; moved index to X

        ; Add the column offset to the address read from the lut.  Store in a work var.
@loop:  clc
        lda _address_lut,x
        adc _col
        sta _addr_temp

        inx
        lda _address_lut,x
        adc #$00
        sta _addr_temp+1       

        ; Copy the work variable to the display list
        ; save X
        txa
        pha

        ; load dl offset into X
        ldx _dl_index

        clc
        lda _addr_temp
        sta _DISPLAY_LIST_ANTIC4,x      ; LSB of 2nd number (4th byte is MSB of LSI address)
        inx
        lda _addr_temp+1
        sta _DISPLAY_LIST_ANTIC4,x
        inx
        inx
        stx _dl_index  ; save current dl index

        ; restore X to address lut index
        pla
        tax
        inx  ; increment the lut index

        dey
        bne @loop

        jsr _wait_wsync

@exit:  jmp XITVBV
.ENDPROC

SETVBV = $E45C
PORTB  = $d301
_setup_vbi:
        .IFDEF ATARIXL
        ;.import _enable_os_ram
        ;.import _disable_os_ram
        ; Disable OS RAM
        lda        PORTB
        pha

        ora        #1
        sta        PORTB
        .ENDIF

        ;jsr _disable_os_ram

        ; Setup VBI
        ldy         #<update_scroll  ; hight byte of address
        ldx         #>update_scroll  ; low byte of address
        lda         #7
        jsr         SETVBV

        .IFDEF ATARIXL
        ; Enable OS RAM
        pla
        sta        PORTB

        ;jsr _enable_os_ram
        .ENDIF

        rts
