; (C) 2025 Brad Colbert

; Player missile Image Data

.segment "PLAYER_IMAGES0"
_player_0_image_data:
    .include "standard_tank.inc"
_player_1_image_data:
    .include "heavy_tank.inc"
.segment "PLAYER_IMAGES1"
_player_2_image_data:
    .include "hover_tank.inc"
_player_3_image_data:
    .include "squirt_tank.inc"

.export _player_0_image_data
.export _player_1_image_data
.export _player_2_image_data
.export _player_3_image_data
