; Base images
; Up
.byte %00011000
.byte %00011000
.byte %00100100
.byte %01011010
.byte %01011010
.byte %01011010
.byte %01011010
.byte %00111100

; Up-Right
.byte %00000000
.byte %00000110
.byte %00111110
.byte %01000100
.byte %10010100
.byte %10100100  
.byte %01001000
.byte %00110000 

; Right
.byte %00000000
.byte %01111000
.byte %10000100
.byte %10111011
.byte %10111011
.byte %10000100
.byte %01111000
.byte %00000000

; Down-Right
.byte %00110000
.byte %01001000
.byte %10100100  
.byte %10010100
.byte %01000100
.byte %00111110
.byte %00000110
.byte %00000000

; Down
.byte %00111100
.byte %01011010
.byte %01011010
.byte %01011010
.byte %01011010
.byte %00100100
.byte %00011000
.byte %00011000

; Down-Left
.byte %00001100
.byte %00010010
.byte %00100101
.byte %00101001
.byte %00100010
.byte %01111100  
.byte %01100000
.byte %00000000

; Left
.byte %00000000
.byte %00011110
.byte %00100001
.byte %11011101
.byte %11011101
.byte %00100001
.byte %00011110
.byte %00000000

; Upper-Left
.byte %00000000
.byte %01100000
.byte %01111100  
.byte %00100010
.byte %00101001
.byte %00100101
.byte %00010010
.byte %00001100

; Idle images
.res 32

; Other images
.res 32