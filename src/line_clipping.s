; Adapted from the source of Elite
; by David Braben and Ian Bell
; for the BBC Microcomputer System

; This routine clips the line from (x1, y1) to (x2, y2) so it fits on-screen, or
; returns an error if it can't be clipped to fit. The arguments are 16-bit
; coordinates, and the clipped line is returned using 8-bit screen coordinates.
;

; Arguments:
;
;   XX15(1 0)            x1 as a 16-bit coordinate (x1_hi x1_lo)
;
;   XX15(3 2)            y1 as a 16-bit coordinate (y1_hi y1_lo)
;
;   XX15(5 4)            x2 as a 16-bit coordinate (x2_hi x2_lo)
;
;   XX12(1 0)            y2 as a 16-bit coordinate (y2_hi y2_lo)
;
; Returns:
;
;   (X1, Y1)             Screen coordinate of the start of the clipped line
;
;   (X2, Y2)             Screen coordinate of the end of the clipped line
;
;   C flag               Clear if the clipped line fits on-screen, set if it
;                        doesn't
;
;   XX13                 The state of the original coordinates on-screen:
;
;                          * 0   = (x2, y2) on-screen
;
;                          * 95  = (x1, y1) on-screen,  (x2, y2) off-screen
;
;                          * 191 = (x1, y1) off-screen, (x2, y2) off-screen
;
;                        So XX13 is non-zero if the end of the line was clipped,
;                        meaning the next line sent to BLINE can't join onto the
;                        end but has to start a new segment
;
;   SWAP                 The swap status of the returned coordinates:
;
;                          * &FF if we swapped the values of (x1, y1) and
;                            (x2, y2) as part of the clipping process
;
;                          * 0 if the coordinates are still in the same order
;
;   Y                    Y is preserved
;
; Other entry points:
;
;   LL147                Don't initialise the values in SWAP or A
;
.import popa

.export _clip
.export _test_carry_and_store

.zeropage
   XX15:    .res 0 ; x1 as a 16-bit coordinate (x1_hi x1_lo)
                   ; y1 as a 16-bit coordinate (y1_hi y1_lo)
                   ; x2 as a 16-bit coordinate (x2_hi x2_lo)
   X1:      .res 1 ; Resulting clipped X1
   Y1:      .res 1 ; Resulting clipped Y1
   X2:      .res 1 ; Resulting clipped X2
   Y2:      .res 1 ; Resulting clipped Y2
   UNUSED:  .res 2
   XX12:    .res 6 ; y2 as a 16-bit coordinate (y2_hi y2_lo)
   XX13:    .res 1 ; The state of the original coordinates on-screen
   Q:       .res 1 ; T is used to store the gradient of slope
   R:       .res 1 ; T is used to store the gradient of slope
   S:       .res 1 ; T is used to store the gradient of slope
   T:       .res 1 ; T is used to store the gradient of slope
   CARRY:   .res 1 ; T is used to store the result of the carry flag

.data
   SWAP: .res 1 ; The swap status of the returned coordinates

   
_XX15 = XX15
.export _XX15
_X1 = X1
.export _X1
_Y1 = Y1
.export _Y1
_X2 = X2
.export _X2
_Y2 = Y2
.export _Y2
_XX13 = XX13
.export _XX13
_SWAP = SWAP
.export _SWAP
_CARRY = CARRY
.export _CARRY

.code

_Y = 96  ; The centre y-coordinate of the 256 x 192 space view


.proc _test_carry_and_store
   bcc carry_clear      ; Branch if carry is clear
   lda #1               ; If carry is set, load 1
   sta _CARRY           ; Store into C variable
   jmp carry_done       ; Skip over the clear part

carry_clear:
   lda #0               ; Carry is clear, load 0
   sta _CARRY           ; Store into C variable

carry_done:
   rts
.endproc


.proc _clip
 jsr popa
 sta XX12     ; y2
 jsr popa
 sta XX12+1   ; y2

 jsr popa
 sta XX15+4   ; x2
 jsr popa
 sta XX15+5   ; x2

 jsr popa
 sta XX15+2   ; y1
 jsr popa
 sta XX15+3   ; y1

 jsr popa
 sta XX15     ; x1
 jsr popa
 sta XX15+1   ; x1


; This part sets XX13 to reflect which of the two points are on-screen and
; off-screen.
LL145:

 LDA #0                 ; Set SWAP = 0
 STA SWAP

 LDA XX15+5             ; Set A = x2_hi

LL147:

 LDX #_Y*2-1             ; Set X = #Y * 2 - 1. The constant #Y is 96, the
                        ; y-coordinate of the mid-point of the space view, so
                        ; this sets Y2 to 191, the y-coordinate of the bottom
                        ; pixel row of the space view

 ORA XX12+1             ; If one or both of x2_hi and y2_hi are non-zero, jump
 BNE LL107              ; to LL107 to skip the following, leaving X at 191

 CPX XX12               ; If y2_lo > the y-coordinate of the bottom of screen
 BCC LL107              ; then (x2, y2) is off the bottom of the screen, so skip
                        ; the following instruction, leaving X at 191

 LDX #0                 ; Set X = 0

LL107:

 STX XX13               ; Set XX13 = X, so we have:
                        ;
                        ;   * XX13 = 0 if x2_hi = y2_hi = 0, y2_lo is on-screen
                        ;
                        ;   * XX13 = 191 if x2_hi or y2_hi are non-zero or y2_lo
                        ;            is off the bottom of the screen
                        ;
                        ; In other words, XX13 is 191 if (x2, y2) is off-screen,
                        ; otherwise it is 0

 LDA XX15+1             ; If one or both of x1_hi and y1_hi are non-zero, jump
 ORA XX15+3             ; to LL83
 BNE LL83

 LDA #_Y*2-1             ; If y1_lo > the y-coordinate of the bottom of screen
 CMP XX15+2             ; then (x1, y1) is off the bottom of the screen, so jump
 BCC LL83               ; to LL83

                        ; If we get here, (x1, y1) is on-screen

 LDA XX13               ; If XX13 is non-zero, i.e. (x2, y2) is off-screen, jump
 BNE LL108              ; to LL108 to halve it before continuing at LL83

                        ; If we get here, the high bytes are all zero, which
                        ; means the x-coordinates are < 256 and therefore fit on
                        ; screen, and neither coordinate is off the bottom of
                        ; the screen. That means both coordinates are already on
                        ; screen, so we don't need to do any clipping, all we
                        ; need to do is move the low bytes into (X1, Y1) and
                        ; X2, Y2) and return

LL146:
                        ; If we get here then we have clipped our line to the
                        ; screen edge (if we had to clip it at all), so we move
                        ; the low bytes from (x1, y1) and (x2, y2) into (X1, Y1)
                        ; and (X2, Y2), remembering that they share locations
                        ; with XX15:
                        ;
                        ;   X1 = XX15
                        ;   Y1 = XX15+1
                        ;   X2 = XX15+2
                        ;   Y2 = XX15+3
                        ;
                        ; X1 already contains x1_lo, so now we do the rest

 LDA XX15+2             ; Set Y1 (aka XX15+1) = y1_lo
 STA XX15+1

 LDA XX15+4             ; Set X2 (aka XX15+2) = x2_lo
 STA XX15+2

 LDA XX12               ; Set Y2 (aka XX15+3) = y2_lo
 STA XX15+3

 CLC                    ; Clear the C flag as the clipped line fits on-screen

 RTS                    ; Return from the subroutine

LL109:

 SEC                    ; Set the C flag to indicate the clipped line does not
                        ; fit on-screen

 RTS                    ; Return from the subroutine

LL108:

 LSR XX13               ; If we get here then (x2, y2) is off-screen and XX13 is
                        ; 191, so shift XX13 right to halve it to 95


; This part does a number of tests to see if the line is on or off the screen.
; 
; If we get here then at least one of (x1, y1) and (x2, y2) is off-screen, with
; XX13 set as follows:
; 
;   * 0   = (x1, y1) off-screen, (x2, y2) on-screen
; 
;   * 95  = (x1, y1) on-screen,  (x2, y2) off-screen
; 
;   * 191 = (x1, y1) off-screen, (x2, y2) off-screen
; 
; where "off-screen" is defined as having a non-zero high byte in one of the
; coordinates, or in the case of y-coordinates, having a low byte > 191, the
; y-coordinate of the bottom of the space view.
LL83:

 LDA XX13               ; If XX13 < 128 then only one of the points is on-screen
 BPL LL115              ; so jump down to LL115 to skip the checks of whether
                        ; both points are in the strips to the right or bottom
                        ; of the screen

                        ; If we get here, both points are off-screen

 LDA XX15+1             ; If both x1_hi and x2_hi have bit 7 set, jump to LL109
 AND XX15+5             ; to return from the subroutine with the C flag set, as
 BMI LL109              ; the entire line is above the top of the screen

 LDA XX15+3             ; If both y1_hi and y2_hi have bit 7 set, jump to LL109
 AND XX12+1             ; to return from the subroutine with the C flag set, as
 BMI LL109              ; the entire line is to the left of the screen

 LDX XX15+1             ; Set A = X = x1_hi - 1
 DEX
 TXA

 LDX XX15+5             ; Set XX12+2 = x2_hi - 1
 DEX
 STX XX12+2

 ORA XX12+2             ; If neither (x1_hi - 1) or (x2_hi - 1) have bit 7 set,
 BPL LL109              ; jump to LL109 to return from the subroutine with the C
                        ; flag set, as the line doesn't fit on-screen

 LDA XX15+2             ; If y1_lo < y-coordinate of screen bottom, clear the C
 CMP #_Y*2               ; flag, otherwise set it

 LDA XX15+3             ; Set XX12+2 = y1_hi - (1 - C), so:
 SBC #0                 ;
 STA XX12+2             ;  * Set XX12+2 = y1_hi - 1 if y1_lo is on-screen
                        ;  * Set XX12+2 = y1_hi     otherwise
                        ;
                        ; We do this subtraction because we are only interested
                        ; in trying to move the points up by a screen if that
                        ; might move the point into the space view portion of
                        ; the screen, i.e. if y1_lo is on-screen

 LDA XX12               ; If y2_lo < y-coordinate of screen bottom, clear the C
 CMP #_Y*2               ; flag, otherwise set it

 LDA XX12+1             ; Set XX12+2 = y2_hi - (1 - C), so:
 SBC #0                 ;
                        ;  * Set XX12+1 = y2_hi - 1 if y2_lo is on-screen
                        ;  * Set XX12+1 = y2_hi     otherwise
                        ;
                        ; We do this subtraction because we are only interested
                        ; in trying to move the points up by a screen if that
                        ; might move the point into the space view portion of
                        ; the screen, i.e. if y1_lo is on-screen

 ORA XX12+2             ; If neither XX12+1 or XX12+2 have bit 7 set, jump to
 BPL LL109              ; LL109 to return from the subroutine with the C flag
                        ; set, as the line doesn't fit on-screen

LL115:

 TYA                    ; Store Y on the stack so we can preserve it through the
 PHA                    ; call to this subroutine

 LDA XX15+4             ; Set XX12+2 = x2_lo - x1_lo
 SEC
 SBC XX15
 STA XX12+2

 LDA XX15+5             ; Set XX12+3 = x2_hi - x1_hi
 SBC XX15+1
 STA XX12+3

 LDA XX12               ; Set XX12+4 = y2_lo - y1_lo
 SEC
 SBC XX15+2
 STA XX12+4

 LDA XX12+1             ; Set XX12+5 = y2_hi - y1_hi
 SBC XX15+3
 STA XX12+5

                        ; So we now have:
                        ;
                        ;   delta_x in XX12(3 2)
                        ;   delta_y in XX12(5 4)
                        ;
                        ; where the delta is (x1, y1) - (x2, y2))

 EOR XX12+3             ; Set S = the sign of delta_x * the sign of delta_y, so
 STA S                  ; if bit 7 of S is set, the deltas have different signs

 LDA XX12+5             ; If delta_y_hi is positive, jump down to LL110 to skip
 BPL LL110              ; the following

 LDA #0                 ; Otherwise flip the sign of delta_y to make it
 SEC                    ; positive, starting with the low bytes
 SBC XX12+4
 STA XX12+4

 LDA #0                 ; And then doing the high bytes, so now:
 SBC XX12+5             ;
 STA XX12+5             ;   XX12(5 4) = |delta_y|

LL110:

 LDA XX12+3             ; If delta_x_hi is positive, jump down to LL111 to skip
 BPL LL111              ; the following

 SEC                    ; Otherwise flip the sign of delta_x to make it
 LDA #0                 ; positive, starting with the low bytes
 SBC XX12+2
 STA XX12+2

 LDA #0                 ; And then doing the high bytes, so now:
 SBC XX12+3             ;
                        ;   (A XX12+2) = |delta_x|

LL111:

                        ; We now keep halving |delta_x| and |delta_y| until
                        ; both of them have zero in their high bytes

 TAX                    ; If |delta_x_hi| is non-zero, skip the following
 BNE LL112

 LDX XX12+5             ; If |delta_y_hi| = 0, jump down to LL113 (as both
 BEQ LL113              ; |delta_x_hi| and |delta_y_hi| are 0)

LL112:

 LSR A                  ; Halve the value of delta_x in (A XX12+2)
 ROR XX12+2

 LSR XX12+5             ; Halve the value of delta_y XX12(5 4)
 ROR XX12+4

 JMP LL111              ; Loop back to LL111

LL113:

                        ; By now, the high bytes of both |delta_x| and |delta_y|
                        ; are zero

 STX T                  ; We know that X = 0 as that's what we tested with a BEQ
                        ; above, so this sets T = 0

 LDA XX12+2             ; If delta_x_lo < delta_y_lo, so our line is more
 CMP XX12+4             ; vertical than horizontal, jump to LL114
 BCC LL114

                        ; If we get here then our line is more horizontal than
                        ; vertical, so it is a shallow slope

 STA Q                  ; Set Q = delta_x_lo

 LDA XX12+4             ; Set A = delta_y_lo

 JSR LL28               ; Call LL28 to calculate:
                        ;
                        ;   R = 256 * A / Q
                        ;     = 256 * delta_y_lo / delta_x_lo

 JMP LL116              ; Jump to LL116, as we now have the line's gradient in R

LL114:

                        ; If we get here then our line is more vertical than
                        ; horizontal, so it is a steep slope

 LDA XX12+4             ; Set Q = delta_y_lo
 STA Q
 LDA XX12+2             ; Set A = delta_x_lo

 JSR LL28               ; Call LL28 to calculate:
                        ;
                        ;   R = 256 * A / Q
                        ;     = 256 * delta_x_lo / delta_y_lo

 DEC T                  ; T was set to 0 above, so this sets T = &FF when our
                        ; line is steep

; This part sets things up to call the routine in LL188, which does the actual
; clipping.
;
; If we get here, then R has been set to the gradient of the line (x1, y1) to
; (x2, y2), with T indicating the gradient of slope:
;
;   * 0   = shallow slope (more horizontal than vertical)
;
;   * &FF = steep slope (more vertical than horizontal)
;
; and XX13 has been set as follows:
;
;   * 0   = (x1, y1) off-screen, (x2, y2) on-screen
;
;   * 95  = (x1, y1) on-screen,  (x2, y2) off-screen
;
;   * 191 = (x1, y1) off-screen, (x2, y2) off-screen

LL116:

 LDA R                  ; Store the gradient in XX12+2
 STA XX12+2

 LDA S                  ; Store the type of slope in XX12+3, bit 7 clear means
 STA XX12+3             ; top left to bottom right, bit 7 set means top right to
                        ; bottom left

 LDA XX13               ; If XX13 = 0, skip the following instruction
 BEQ LL138

 BPL LLX117             ; If XX13 is positive, it must be 95. This means
                        ; (x1, y1) is on-screen but (x2, y2) isn't, so we jump
                        ; to LLX117 to swap the (x1, y1) and (x2, y2)
                        ; coordinates around before doing the actual clipping,
                        ; because we need to clip (x2, y2) but the clipping
                        ; routine at LL118 only clips (x1, y1)

LL138:

                        ; If we get here, XX13 = 0 or 191, so (x1, y1) is
                        ; off-screen and needs clipping

 JSR LL118              ; Call LL118 to move (x1, y1) along the line onto the
                        ; screen, i.e. clip the line at the (x1, y1) end

 LDA XX13               ; If XX13 = 0, i.e. (x2, y2) is on-screen, jump down to
 BPL LL124              ; LL124 to return with a successfully clipped line

LL117:

                        ; If we get here, XX13 = 191 (both coordinates are
                        ; off-screen)

 LDA XX15+1             ; If either of x1_hi or y1_hi are non-zero, jump to
 ORA XX15+3             ; LL137 to return from the subroutine with the C flag
 BNE LL137              ; set, as the line doesn't fit on-screen

 LDA XX15+2             ; If y1_lo > y-coordinate of the bottom of the screen
 CMP #_Y*2               ; jump to LL137 to return from the subroutine with the
 BCS LL137              ; C flag set, as the line doesn't fit on-screen

LLX117:

                        ; If we get here, XX13 = 95 or 191, and in both cases
                        ; (x2, y2) is off-screen, so we now need to swap the
                        ; (x1, y1) and (x2, y2) coordinates around before doing
                        ; the actual clipping, because we need to clip (x2, y2)
                        ; but the clipping routine at LL118 only clips (x1, y1)

 LDX XX15               ; Swap x1_lo = x2_lo
 LDA XX15+4
 STA XX15
 STX XX15+4

 LDA XX15+5             ; Swap x2_lo = x1_lo
 LDX XX15+1
 STX XX15+5
 STA XX15+1

 LDX XX15+2             ; Swap y1_lo = y2_lo
 LDA XX12
 STA XX15+2
 STX XX12

 LDA XX12+1             ; Swap y2_lo = y1_lo
 LDX XX15+3
 STX XX12+1
 STA XX15+3

 JSR LL118              ; Call LL118 to move (x1, y1) along the line onto the
                        ; screen, i.e. clip the line at the (x1, y1) end

 DEC SWAP               ; Set SWAP = &FF to indicate that we just clipped the
                        ; line at the (x2, y2) end by swapping the coordinates
                        ; (the DEC does this as we set SWAP to 0 at the start of
                        ; this subroutine)

LL124:

 PLA                    ; Restore Y from the stack so it gets preserved through
 TAY                    ; the call to this subroutine

 JMP LL146              ; Jump up to LL146 to move the low bytes of (x1, y1) and
                        ; (x2, y2) into (X1, Y1) and (X2, Y2), and return from
                        ; the subroutine with a successfully clipped line

LL137:

 PLA                    ; Restore Y from the stack so it gets preserved through
 TAY                    ; the call to this subroutine

 SEC                    ; Set the C flag to indicate the clipped line does not
                        ; fit on-screen

 RTS                    ; Return from the subroutine


; This part clips the line at the (x1, y1) end, so we
; need to move (x1, y1) along the line until it is on
; screen. We do this by moving the coordinates along the
; line by a distance of 256 * delta_y / delta_x, where
; delta_x and delta_y are the differences between the
; coordinates of the two points

; Arguments:
;
;   XX15(1 0)            x1 as a 16-bit coordinate (x1_hi x1_lo)
;
;   XX15(3 2)            y1 as a 16-bit coordinate (y1_hi y1_lo)
;
;   XX12+2               The line's gradient * 256 (so 1.0 = 256)
;
;   XX12+3               The direction of slope:
;
;                          * Positive (bit 7 clear) = top left to bottom right
;
;                          * Negative (bit 7 set) = top right to bottom left
;
;   T                    The gradient of slope:
;
;                          * 0 if it's a shallow slope
;
;                          * &FF if it's a steep slope
;
; Returns:
;
;   XX15                 x1 as an 8-bit coordinate
;
;   XX15+2               y1 as an 8-bit coordinate

LL118:
 LDA XX15+1             ; If x1_hi is positive, jump down to LL119 to skip the
 BPL LL119              ; following

 STA S                  ; Otherwise x1_hi is negative, i.e. off the left of the
                        ; screen, so set S = x1_hi

 JSR LL120              ; Call LL120 to calculate:
                        ;
                        ;   (Y X) = (S x1_lo) * XX12+2      if T = 0
                        ;         = x1 * gradient
                        ;
                        ;   (Y X) = (S x1_lo) / XX12+2      if T <> 0
                        ;         = x1 / gradient
                        ;
                        ; with the sign of (Y X) set to the opposite of the
                        ; line's direction of slope

 TXA                    ; Set y1 = y1 + (Y X)
 CLC                    ;
 ADC XX15+2             ; starting with the low bytes
 STA XX15+2

 TYA                    ; And then adding the high bytes
 ADC XX15+3
 STA XX15+3

 LDA #0                 ; Set x1 = 0
 STA XX15
 STA XX15+1

 TAX                    ; Set X = 0 so the next instruction becomes a JMP

LL119:

 BEQ LL134              ; If x1_hi = 0 then jump down to LL134 to skip the
                        ; following, as the x-coordinate is already on-screen
                        ; (as 0 <= (x_hi x_lo) <= 255)

 STA S                  ; Otherwise x1_hi is positive, i.e. x1 >= 256 and off
 DEC S                  ; the right side of the screen, so set S = x1_hi - 1

 JSR LL120              ; Call LL120 to calculate:
                        ;
                        ;   (Y X) = (S x1_lo) * XX12+2      if T = 0
                        ;         = (x1 - 256) * gradient
                        ;
                        ;   (Y X) = (S x1_lo) / XX12+2      if T <> 0
                        ;         = (x1 - 256) / gradient
                        ;
                        ; with the sign of (Y X) set to the opposite of the
                        ; line's direction of slope

 TXA                    ; Set y1 = y1 + (Y X)
 CLC                    ;
 ADC XX15+2             ; starting with the low bytes
 STA XX15+2

 TYA                    ; And then adding the high bytes
 ADC XX15+3
 STA XX15+3

 LDX #255               ; Set x1 = 255
 STX XX15
 INX
 STX XX15+1

LL134:

                        ; We have moved the point so the x-coordinate is on
                        ; screen (i.e. in the range 0-255), so now for the
                        ; y-coordinate

 LDA XX15+3             ; If y1_hi is positive, jump down to LL119 to skip
 BPL LL135              ; the following

 STA S                  ; Otherwise y1_hi is negative, i.e. off the top of the
                        ; screen, so set S = y1_hi

 LDA XX15+2             ; Set R = y1_lo
 STA R

 JSR LL123              ; Call LL123 to calculate:
                        ;
                        ;   (Y X) = (S R) / XX12+2      if T = 0
                        ;         = y1 / gradient
                        ;
                        ;   (Y X) = (S R) * XX12+2      if T <> 0
                        ;         = y1 * gradient
                        ;
                        ; with the sign of (Y X) set to the opposite of the
                        ; line's direction of slope

 TXA                    ; Set x1 = x1 + (Y X)
 CLC                    ;
 ADC XX15               ; starting with the low bytes
 STA XX15

 TYA                    ; And then adding the high bytes
 ADC XX15+1
 STA XX15+1

 LDA #0                 ; Set y1 = 0
 STA XX15+2
 STA XX15+3

LL135:

;BNE LL139              ; This instruction is commented out in the original
                        ; source

 LDA XX15+2             ; Set (S R) = (y1_hi y1_lo) - screen height
 SEC                    ;
 SBC #_Y*2               ; starting with the low bytes
 STA R

 LDA XX15+3             ; And then subtracting the high bytes
 SBC #0
 STA S

 BCC LL136              ; If the subtraction underflowed, i.e. if y1 < screen
                        ; height, then y1 is already on-screen, so jump to LL136
                        ; to return from the subroutine, as we are done

LL139:

                        ; If we get here then y1 >= screen height, i.e. off the
                        ; bottom of the screen

 JSR LL123              ; Call LL123 to calculate:
                        ;
                        ;   (Y X) = (S R) / XX12+2      if T = 0
                        ;         = (y1 - screen height) / gradient
                        ;
                        ;   (Y X) = (S R) * XX12+2      if T <> 0
                        ;         = (y1 - screen height) * gradient
                        ;
                        ; with the sign of (Y X) set to the opposite of the
                        ; line's direction of slope

 TXA                    ; Set x1 = x1 + (Y X)
 CLC                    ;
 ADC XX15               ; starting with the low bytes
 STA XX15

 TYA                    ; And then adding the high bytes
 ADC XX15+1
 STA XX15+1

 LDA #_Y*2-1             ; Set y1 = 2 * #Y - 1. The constant #Y is 96, the
 STA XX15+2             ; y-coordinate of the mid-point of the space view, so
 LDA #0                 ; this sets Y2 to 191, the y-coordinate of the bottom
 STA XX15+3             ; pixel row of the space view

LL136:

 RTS                    ; Return from the subroutine

; Calculate the following, where A < Q:
;
;   R = 256 * A / Q
;
; This is a sister routine to LL61, which does the division when A >= Q.
;
; If A >= Q then 255 is returned and the C flag is set to indicate an overflow
; (the C flag is clear if the division was a success).
;
; The result is returned in one byte as the result of the division multiplied
; by 256, so we can return fractional results using integers.
;
; This routine uses the same shift-and-subtract algorithm that's documented in
; TIS2, but it leaves the fractional result in the integer range 0-255.
;
;
; Returns:
;
;   C flag               Set if the answer is too big for one byte, clear if the
;                        division was a success
;
;
; Other entry points:
;
;   LL28+4               Skips the A >= Q check and always returns with C flag
;                        cleared, so this can be called if we know the division
;                        will work
;
;   LL31                 Skips the A >= Q check and does not set the R counter,
;                        so this can be used for jumping straight into the
;                        division loop if R is already set to 254 and we know the
;                        division will work
LL28:

 CMP Q                  ; If A >= Q, then the answer will not fit in one byte,
 BCS LL2                ; so jump to LL2 to return 255

 LDX #%11111110         ; Set R to have bits 1-7 set, so we can rotate through 7
 STX R                  ; loop iterations, getting a 1 each time, and then
                        ; getting a 0 on the 8th iteration... and we can also
                        ; use R to catch our result bits into bit 0 each time

LL31:

 ASL A                  ; Shift A to the left

 BCS LL29               ; If bit 7 of A was set, then jump straight to the
                        ; subtraction

 CMP Q                  ; If A < Q, skip the following subtraction
 BCC :+ ; P%+4

 SBC Q                  ; A >= Q, so set A = A - Q
:
 ROL R                  ; Rotate the counter in R to the left, and catch the
                        ; result bit into bit 0 (which will be a 0 if we didn't
                        ; do the subtraction, or 1 if we did)

 BCS LL31               ; If we still have set bits in R, loop back to LL31 to
                        ; do the next iteration of 7

 RTS                    ; R left with remainder of division

LL29:

 SBC Q                  ; A >= Q, so set A = A - Q

 SEC                    ; Set the C flag to rotate into the result in R

 ROL R                  ; Rotate the counter in R to the left, and catch the
                        ; result bit into bit 0 (which will be a 0 if we didn't
                        ; do the subtraction, or 1 if we did)

 BCS LL31               ; If we still have set bits in R, loop back to LL31 to
                        ; do the next iteration of 7

 RTS                    ; Return from the subroutine with R containing the
                        ; remainder of the division

LL2:

 LDA #255               ; The division is very close to 1, so return the closest
 STA R                  ; possible answer to 256, i.e. R = 255

 RTS                    ; Return from the subroutine


; Calculate the following:
;
;   * If T = 0, this is a shallow slope, so calculate (Y X) = (S R) / XX12+2
;
;   * If T <> 0, this is a steep slope, so calculate (Y X) = (S R) * XX12+2
;
; giving (Y X) the opposite sign to the slope direction in XX12+3.
;
;
; Arguments:
;
;   XX12+2               The line's gradient * 256 (so 1.0 = 256)
;
;   XX12+3               The direction of slope:
;
;                          * Bit 7 clear means top left to bottom right
;
;                          * Bit 7 set means top right to bottom left
;
;   T                    The gradient of slope:
;
;                          * 0 if it's a shallow slope
;
;                          * &FF if it's a steep slope
;
;
; Other entry points:
;
;   LL121                Calculate (Y X) = (S R) / Q and set the sign to the
;                        opposite of the top byte on the stack
;
;   LL133                Negate (Y X) and return from the subroutine
;
;   LL128                Contains an RTS
LL123:

 JSR LL129              ; Call LL129 to do the following:
                        ;
                        ;   Q = XX12+2
                        ;     = line gradient
                        ;
                        ;   A = S EOR XX12+3
                        ;     = S EOR slope direction
                        ;
                        ;   (S R) = |S R|
                        ;
                        ; So A contains the sign of S * slope direction

 PHA                    ; Store A on the stack so we can use it later

 LDX T                  ; If T is non-zero, then it's a steep slope, so jump up
 BNE LL122              ; to LL122 to calculate this instead:
                        ;
                        ;   (Y X) = (S R) * Q

LL121:

                        ; The following calculates:
                        ;
                        ;   (Y X) = (S R) / Q
                        ;
                        ; using the same shift-and-subtract algorithm that's
                        ; documented in TIS2

 LDA #%11111111         ; Set Y = %11111111
 TAY

 ASL A                  ; Set X = %11111110
 TAX

                        ; This sets (Y X) = %1111111111111110, so we can rotate
                        ; through 15 loop iterations, getting a 1 each time, and
                        ; then getting a 0 on the 16th iteration... and we can
                        ; also use it to catch our result bits into bit 0 each
                        ; time

LL130:

 ASL R                  ; Shift (S R) to the left
 ROL S

 LDA S                  ; Set A = S

 BCS LL131              ; If bit 7 of S was set, then jump straight to the
                        ; subtraction

 CMP Q                  ; If A < Q (i.e. S < Q), skip the following subtractions
 BCC LL132

LL131:

 SBC Q                  ; A >= Q (i.e. S >= Q) so set:
 STA S                  ;
                        ;   S = (A R) - Q
                        ;     = (S R) - Q
                        ;
                        ; starting with the low bytes (we know the C flag is
                        ; set so the subtraction will be correct)

 LDA R                  ; And then doing the high bytes
 SBC #0
 STA R

 SEC                    ; Set the C flag to rotate into the result in (Y X)

LL132:

 TXA                    ; Rotate the counter in (Y X) to the left, and catch the
 ROL A                  ; result bit into bit 0 (which will be a 0 if we didn't
 TAX                    ; do the subtraction, or 1 if we did)
 TYA
 ROL A
 TAY

 BCS LL130              ; If we still have set bits in (Y X), loop back to LL130
                        ; to do the next iteration of 15, until we have done the
                        ; whole division

 PLA                    ; Restore A, which we calculated above, from the stack

 BMI LL128              ; If A is negative jump to LL128 to return from the
                        ; subroutine with (Y X) as is

LL133:

 TXA                    ; Otherwise negate (Y X) using two's complement by first
 EOR #%11111111         ; setting the low byte to ~X + 1
;CLC                    ;
 ADC #1                 ; The CLC instruction is commented out in the original
 TAX                    ; source. It would have no effect as we know the C flag
                        ; is clear from when we passed through the BCS above

 TYA                    ; Then set the high byte to ~Y + C
 EOR #%11111111
 ADC #0
 TAY

LL128:

 RTS                    ; Return from the subroutine


; Calculate the following:
;
;   * If T = 0, this is a shallow slope, so calculate (Y X) = (S x1_lo) * XX12+2
;
;   * If T <> 0, this is a steep slope, so calculate (Y X) = (S x1_lo) / XX12+2
;
; giving (Y X) the opposite sign to the slope direction in XX12+3.
;
;
; Arguments:
;
;   T                    The gradient of slope:
;
;                          * 0 if it's a shallow slope
;
;                          * &FF if it's a steep slope
;
;
; Other entry points:
;
;   LL122                Calculate (Y X) = (S R) * Q and set the sign to the
;                        opposite of the top byte on the stack
LL120:

 LDA XX15               ; Set R = x1_lo
 STA R

;LL120                 ; This label is commented out in the original source

 JSR LL129              ; Call LL129 to do the following:
                        ;
                        ;   Q = XX12+2
                        ;     = line gradient
                        ;
                        ;   A = S EOR XX12+3
                        ;     = S EOR slope direction
                        ;
                        ;   (S R) = |S R|
                        ;
                        ; So A contains the sign of S * slope direction

 PHA                    ; Store A on the stack so we can use it later

 LDX T                  ; If T is non-zero, then it's a steep slope, so jump
 BNE LL121              ; down to LL121 to calculate this instead:
                        ;
                        ;   (Y X) = (S R) / Q

LL122:

                        ; The following calculates:
                        ;
                        ;   (Y X) = (S R) * Q
                        ;
                        ; using the same shift-and-add algorithm that's
                        ; documented in MULT1

 LDA #0                 ; Set A = 0

 TAX                    ; Set (Y X) = 0 so we can start building the answer here
 TAY

 LSR S                  ; Shift (S R) to the right, so we extract bit 0 of (S R)
 ROR R                  ; into the C flag

 ASL Q                  ; Shift Q to the left, catching bit 7 in the C flag

 BCC LL126              ; If C (i.e. the next bit from Q) is clear, do not do
                        ; the addition for this bit of Q, and instead skip to
                        ; LL126 to just do the shifts

LL125:

 TXA                    ; Set (Y X) = (Y X) + (S R)
 CLC                    ;
 ADC R                  ; starting with the low bytes
 TAX

 TYA                    ; And then doing the high bytes
 ADC S
 TAY

LL126:

 LSR S                  ; Shift (S R) to the right
 ROR R

 ASL Q                  ; Shift Q to the left, catching bit 7 in the C flag

 BCS LL125              ; If C (i.e. the next bit from Q) is set, loop back to
                        ; LL125 to do the addition for this bit of Q

 BNE LL126              ; If Q has not yet run out of set bits, loop back to
                        ; LL126 to do the "shift" part of shift-and-add until
                        ; we have done additions for all the set bits in Q, to
                        ; give us our multiplication result

 PLA                    ; Restore A, which we calculated above, from the stack

 BPL LL133              ; If A is positive jump to LL133 to negate (Y X) and
                        ; return from the subroutine using a tail call

 RTS                    ; Return from the subroutine


;  Do the following, in this order:
;
;   Q = XX12+2
;
;   A = S EOR XX12+3
;
;   (S R) = |S R|
;
; This sets up the variables required above to calculate (S R) / XX12+2 and give
; the result the opposite sign to XX13+3.
LL129:

 LDX XX12+2             ; Set Q = XX12+2
 STX Q

 LDA S                  ; If S is positive, jump to LL127
 BPL LL127

 LDA #0                 ; Otherwise set R = -R
 SEC
 SBC R
 STA R

 LDA S                  ; Push S onto the stack
 PHA

 EOR #%11111111         ; Set S = ~S + 1 + C
 ADC #0
 STA S

 PLA                    ; Pull the original, negative S from the stack into A

LL127:

 EOR XX12+3             ; Set A = original argument S EOR'd with XX12+3

 RTS                    ; Return from the subroutine

.endproc