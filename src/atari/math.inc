; (C) 2025 Brad Colbert

; 32-bit subtraction routine for Atari 8-bit systems
.macro subtract32 LHV, RHV
    lda LHV+0
    sec
    sbc RHV+0
    sta LHV+0
    lda LHV+1
    sbc RHV+1
    sta LHV+1
    lda LHV+2
    sbc RHV+2
    sta LHV+2
    lda LHV+3
    sbc RHV+3
    sta LHV+3
.endmacro
