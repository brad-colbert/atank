.segment "FRAMEBUFFER_1"
pad1: 
    .res 16
_framebuffer:
    .res 7680
.export _framebuffer

.segment "PLAYER_IMAGE_DATA"
_player_image_data:
    .include "standard_tank.inc"
    .include "heavy_tank.inc"
    .include "hover_tank.inc"
    .include "squirt_tank.inc"
.export _player_image_data

.segment "FRAMEBUFFER_2"
_framebuffer_2:
    .res 7680
.export _framebuffer_2
