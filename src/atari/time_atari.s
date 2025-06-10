; (C) 2025 Brad Colbert

.include "memory.inc"

; Constants
RTCLOK = 18  ; 19, 20

; Exports
.export _get_time_millis
.export _set_time_millis

; Macros

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

