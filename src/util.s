;.include "hardware.s"

.export _disable_os_ram
.export _enable_os_ram

PORTB    = $d301

; Disable OS RAM
_disable_os_ram:
    lda        PORTB
    pha

    ora        #1
    sta        PORTB
    rts

; Enable OS RAM
_enable_os_ram:
    pla
    sta        PORTB
    rts
