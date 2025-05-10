.export _mul16

.import     popa

.segment "BSS"
multiplier:   .res 2
multiplicand: .res 2
product:      .res 4

_product = product ; alias for the product
.export _product

.segment "CODE"

.proc _mul16

            ; Get the command line arguments
            jsr popa
            sta multiplicand
            jsr popa
            sta multiplicand+1

            jsr popa
            sta multiplier
            jsr popa
            sta multiplier+1

            ; Perform the multiplication
            lda	#00
            sta	product+2       ; clear upper bits of product
            sta	product+3 
            ldx	#16             ; set binary count to 16 
shift_r:    lsr	multiplier+1    ; divide multiplier by 2 
            ror	multiplier
            bcc	rotate_r 
            lda	product+2       ; get upper half of product and add multiplicand
            clc
            adc	multiplicand
            sta	product+2
            lda	product+3 
            adc	multiplicand+1
rotate_r:	ror                 ; rotate partial product 
            sta	product+3 
            ror	product+2
            ror	product+1 
            ror	product 
            dex
            bne	shift_r
            ; ldx product+2  ; low byte of product
            ; lda product+1  ; high byte of product
            rts

.endproc