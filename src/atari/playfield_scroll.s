; (C) 2025 Brad Colbert

; Includes
.include "memory.inc"

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
;.export _players
.export _scroll_flag
.export _x_pos_shadow
.export _y_pos_shadow
.export col        ; exported for debug
.export row        ; exported for debug
.export addr_temp  ; exported for debug

; Types
;.struct Player
;        x_pos_prev .word
;        y_pos_prev .word
;        x_pos .word
;        y_pos .word
;        direction .byte ; 0: up, 1: up-right, 2: right, 3: down-right, 4: down, 5: down-left, 6: left, 7: up-left
;        health .byte    ; Health of the player
;        armor .byte     ; Armor of the player
;        ammo .byte      ; Ammo of the player
;        score .word     ; Score of the player
;.endstruct

; Data
;.segment "GAME_DATA2"
;_players: ; Array of players (4 players)
;        player0: .tag Player
;        player1: .tag Player
;        player2: .tag Player
;        player3: .tag Player

.zeropage
_scroll_flag:  .byte $00   ; flag to indicate if scroll is being updated
_x_pos_shadow: .word $0000 ; shadow for x position
_y_pos_shadow: .word $0000 ; shadow for y position
col:           .byte $00   ; column offset
row:           .byte $00   ; row offset
addr_temp:     .word $0000 ; temporary address for display list update

.code

;-----------------------------------------------------------------------------
; Starts calling our scrolling routine during the blanking interval
.proc _enable_scroll_vbi
        lda #$FF
        sta _scroll_flag             ; Set the scroll flag to indicate that scrolling is enabled


        ldy #<update_scroll   ; Low byte of the address of the scroll update routine
        ldx #>update_scroll   ; High byte of the address of the scroll update routine
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
.export update_scroll  ; debug
update_scroll:
        ; check to see if the x, y are being written, skip if so
        lda _scroll_flag
        beq @exit

        ; Get the fine scroll values
        lda _x_pos_shadow   ; Just get the last 4 bits of the x position for fine horizontal scroll
        and #$03                      ; Ensure it is in the range 0-15
        eor #$0F                      ; 0x0F - x_pos & 0x0F    F - pos becasue when HSCROL is enabled, scroll is inverted compared to our situation and there are buffer pixels.
        sta HSCROL

        lda _y_pos_shadow   ; Just get the last 4 bits of the y position for fine vertical scroll
        and #$07                      ; Ensure it is in the range 0-15
        sta VSCROL

        ; Get the course scroll values
        lda _x_pos_shadow     ; LSB
        sta addr_temp
        lda _x_pos_shadow + 1 ; MSB
        sta addr_temp + 1
        lsr16 addr_temp              ; Shift right to get the row offset, 2 shifts is /4
        lsr16 addr_temp              ; Shift right to get the row offset
        lda addr_temp                ; Should only be one byte (0 - 80)
        sta col                      ; Store the column offset

        lda _y_pos_shadow     ; LSB
        sta addr_temp
        lda _y_pos_shadow + 1 ; MSB
        sta addr_temp + 1
        lsr16 addr_temp              ; Shift right to get the row offset, 3 shifts is /8
        lsr16 addr_temp              ; Shift right to get the row offset
        lsr16 addr_temp              ; Shift right to get the row offset
        lda addr_temp                ; Should only be one byte (0 - 24*4)
        asl                          ; Shift left to multiply by 2 (each entry in the LUT is 2 bytes)
        tay                          ; Store the row offset in Y
        adc #50                      ; Add 24*2 (2 bytes per entry in the LUT) to the row offset to get the last row to lookup
        sta row                      ; Store the last row to lookup.  Used for the loop below.

        ; Iterate over the playfield LUT starting at row.
        ldx #4                       ; Start at byte 4 of the display list (the first entry is the ANTIC mode, the second is the LSI address)
@loop:
        ; Get the LUT address for the current row
        clc
        lda _playfield_lut,y         ; Get the address of the playfield for the row
        adc col                      ; Add the column offset to the address
        sta _display_list_antic4,x   ; Copy the address to the display list LSB
        lda _playfield_lut+1,y       ; Get the MSB of the address of the playfield for the row
        adc #$00                     ; Add 0 to the MSB (if carry)
        sta _display_list_antic4+1,x ; Copy the address to the display list MSB

        inx
        inx
        inx                          ; Increment X to the next entry in the display list (3 bytes per entry)

        iny
        iny                          ; Increment Y to get the next row (two bytes per entry in the LUT)
        cpy row                      ; Check if we have processed all 24 rows (0-23)
        bmi @loop                    ; If Y <= row, continue the loop

        lda #$00
        sta _scroll_flag             ; Clear the scroll flag
@exit:
        jmp SYSVBV                   ; Call the system VBI routine
