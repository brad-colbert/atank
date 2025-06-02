; (C) 2025 Brad Colbert

; Constants
HSCROL = $D404
VSCROL = $D405
WSYNC  = $D40A
XITVBV = $E462
SETVBV = $E45C
SYSVBV = $E45F


; Imports
.import _playfield_lut
.import _display_list_antic4

; Exports
.export _enable_scroll_vbi
.export _disable_scroll_vbi
.export _players
.export _scroll_flag
.export h_fs
.export v_fs
.export col
.export row
.export dl_index
.export addr_temp

; Types
.struct Player
        x_pos_prev .word
        y_pos_prev .word
        x_pos .word
        y_pos .word
        direction .byte ; 0: up, 1: up-right, 2: right, 3: down-right, 4: down, 5: down-left, 6: left, 7: up-left
        health .byte    ; Health of the player
        armor .byte     ; Armor of the player
        ammo .byte      ; Ammo of the player
        score .word     ; Score of the player
.endstruct

.segment "GAME_DATA2"
_players: ; Array of players (4 players)
        player0: .tag Player
        player1: .tag Player
        player2: .tag Player
        player3: .tag Player

; Variables
.zeropage

_scroll_flag: .byte $00 ; flag to indicate if scroll is being updated
h_fs:         .byte $07 ; horizontal fine scroll
v_fs:         .byte $0F ; vertical fine scroll
col:          .byte $00 ; column offset
row:          .byte $00 ; row offset
dl_index:     .byte $00 ; index for display list update
addr_temp:    .word $0000 ; temporary address for display list update

; Macros
.macro lsr16 MEM
        LSR MEM+1       ;Shift the MSB
        ROR MEM+0       ;Rotate the LSB
.endmacro


.code

;-----------------------------------------------------------------------------
; Starts calling tour scrolling routine during the blanking interval
.proc _enable_scroll_vbi
        ldy #<test_update_scroll   ; Low byte of the address of the scroll update routine
        ldx #>test_update_scroll   ; High byte of the address of the scroll update routine
        lda #6
        jsr SETVBV
        rts
.endproc

;-----------------------------------------------------------------------------
; Returns the vblank routine to the system routine
.proc _disable_scroll_vbi
        ldy #<SYSVBV   ; Low byte of the address of the scroll update routine
        ldx #>SYSVBV   ; High byte of the address of the scroll update routine
        lda #6
        jsr SETVBV
        rts
.endproc

;-----------------------------------------------------------------------------
; Wait for horizontal SYNChronization
wsync:
    sta WSYNC
    rts

;-----------------------------------------------------------------------------
; Performs the scroll update
.export test_update_scroll  ; debug
test_update_scroll:
        ; check to see if the x, y are being written, skip if so
        lda _scroll_flag
        beq @exit

        ; Get the fine scroll values
        lda player0 + Player::x_pos   ; Just get the last 4 bits of the x position for fine horizontal scroll
        and #$0F                      ; Ensure it is in the range 0-7
        eor #$0F                      ; 0x0F - x_pos & 0x0F
        ;sta h_fs                     ; Store in horizontal fine scroll
        ;lda #$0F
        sta HSCROL

        lda player0 + Player::y_pos   ; Just get the last 4 bits of the y position for fine vertical scroll
        and #$0F                      ; Ensure it is in the range 0-7
        eor #$0F                      ; 0x0F - x_pos & 0x0F
        ;lda #$0F
        sta VSCROL

        ; Get the course scroll values
        lda player0 + Player::y_pos     ; LSB
        sta addr_temp
        lda player0 + Player::y_pos + 1 ; MSB
        sta addr_temp + 1
        lsr16 addr_temp              ; Shift right to get the column offset, 4 shifts is /16
        lsr16 addr_temp              ; Shift right to get the column offset
        lsr16 addr_temp              ; Shift right to get the column offset
        lsr16 addr_temp              ; Shift right to get the column offset
        lda addr_temp                ; Should only be one byte (0 - 24*4)
        asl                          ; Shift left to multiply by 2 (each entry in the LUT is 2 bytes)
        tay                          ; Store the row offset in Y
        adc #48                      ; Add 24*2 (2 bytes per entry in the LUT) to the row offset to get the last row to lookup
        sta row                      ; Store the last row to lookup.  Used for the loop below.

        ; Iterate over the playfield LUT starting at row.
        ldx #4                       ; Start at byte 4 of the display list (the first entry is the ANTIC mode, the second is the LSI address)
@loop:
        ; Get the LUT address for the current row
        lda _playfield_lut,y         ; Get the address of the playfield for the row
        sta _display_list_antic4,x   ; Copy the address to the display list LSB
        lda _playfield_lut+1,y       ; Get the MSB of the address of the playfield for the row
        sta _display_list_antic4+1,x ; Copy the address to the display list MSB

        inx
        inx
        inx                          ; Increment X to the next entry in the display list (3 bytes per entry)

        iny
        iny                          ; Increment Y to get the next row (two bytes per entry in the LUT)
        cpy row                      ; Check if we have processed all 24 rows (0-23)
        bmi @loop                    ; If Y <= row, continue the loop

@exit:
        lda #$00
        sta _scroll_flag             ; Clear the scroll flag
        jmp SYSVBV                   ; Call the system VBI routine

update_scroll_OLD:

        ; check to see if the x, y are being written, skip if so
        lda _scroll_flag
        bne @exit

        ; Horizontal fine scroll
@fine_scroll:
        lda h_fs
        sta HSCROL

        ; Vertical fine scroll
        lda v_fs
        sta VSCROL

; Here we are testing if we need to perform a course scroll by
; comparing the calculated upper-left corner address to what is already
; in the first entry of the display list.  If it's different, we need
; to update everything.
@test_course_update:
        ; Calc LUT offset
        lda row
        asl          ; shift left (multiply by 2)
        tax          ; moved index to X

        ; Add the column offset to the address.  Store in a work var.
        clc
        lda _playfield_lut,x
        adc col
        sta addr_temp

        inx
        lda _playfield_lut,x
        adc #$00
        sta addr_temp+1       

        ; Compare the LUT address at the row + col (offset) with the first address in _DISPLAY_LIST_ANTIC4
        clc
        lda addr_temp
        cmp _display_list_antic4+4      ; LSB of 2nd number (4th byte is MSB of LSI address)
        bne @course_scroll              ; There is a difference so let's update the display list
        lda addr_temp+1
        cmp _display_list_antic4+5      ; MSB of 2nd number (5th byte is LSB of LSI address
        beq @exit

; Perform course scroll by updating the display list.
@course_scroll:
        ldy #25
        lda #04
        sta dl_index

        lda row
        asl          ; shift left (multiply by 2)
        tax          ; moved index to X

        ; Add the column offset to the address read from the lut.  Store in a work var.
@loop:  clc
        lda _playfield_lut,x
        adc col
        sta addr_temp

        inx
        lda _playfield_lut,x
        adc #$00
        sta addr_temp+1       

        ; Copy the work variable to the display list
        ; save X
        txa
        pha

        ; load dl offset into X
        ldx dl_index

        clc
        lda addr_temp
        sta _display_list_antic4,x      ; LSB of 2nd number (4th byte is MSB of LSI address)
        inx
        lda addr_temp+1
        sta _display_list_antic4,x
        inx
        inx
        stx dl_index  ; save current dl index

        ; restore X to address lut index
        pla
        tax
        inx  ; increment the lut index

        dey
        bne @loop

        jsr wsync

@exit:  jmp XITVBV