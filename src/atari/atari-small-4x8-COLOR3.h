#ifndef __ATARI-SMALL-4x8-COLOR3_H__
#define __ATARI-SMALL-4x8-COLOR3_H__

#include <stdint.h>

uint8_t font[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   // (0:32)
0x00,0x30,0x30,0x30,0x30,0x00,0x30,0x00,   // (1:33) !
0x00,0xCC,0xCC,0x00,0x00,0x00,0x00,0x00,   // (2:34) "
0x00,0xCC,0xFC,0xCC,0xCC,0xFC,0xCC,0x00,   // (3:35) #
0x30,0x30,0xCC,0x30,0x0C,0xCC,0x30,0x30,   // (4:36) $
0x00,0xCC,0x0C,0x30,0x30,0xC0,0xCC,0x00,   // (5:37) %
0x30,0xCC,0x30,0xCC,0xCC,0xF0,0x3C,0x00,   // (6:38) &
0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,   // (7:39) '
0x00,0x0C,0x30,0x30,0x30,0x30,0x0C,0x00,   // (8:40) (
0x00,0xC0,0x30,0x30,0x30,0x30,0xC0,0x00,   // (9:41) )
0x00,0xCC,0x30,0xFC,0x30,0xCC,0x00,0x00,   // (10:42) *
0x00,0x30,0x30,0xFC,0x30,0x30,0x00,0x00,   // (11:43) +
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0xC0,   // (12:44) ,
0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,   // (13:45) -
0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,   // (14:46) .
0x00,0x0C,0x0C,0x30,0x30,0xC0,0xC0,0x00,   // (15:47) /
0x00,0x30,0xCC,0xFC,0xCC,0xCC,0x30,0x00,   // (16:48) 0
0x00,0x30,0xF0,0x30,0x30,0x30,0xFC,0x00,   // (17:49) 1
0x00,0x30,0xCC,0x0C,0x30,0xC0,0xFC,0x00,   // (18:50) 2
0x00,0xFC,0x0C,0x30,0x0C,0xCC,0x30,0x00,   // (19:51) 3
0x00,0x0C,0x3C,0xCC,0xFC,0x0C,0x0C,0x00,   // (20:52) 4
0x00,0xFC,0xC0,0xF0,0x0C,0xCC,0x30,0x00,   // (21:53) 5
0x00,0x3C,0xC0,0xF0,0xCC,0xCC,0x30,0x00,   // (22:54) 6
0x00,0xFC,0x0C,0x0C,0x30,0x30,0x30,0x00,   // (23:55) 7
0x00,0x30,0xCC,0x30,0xCC,0xCC,0x30,0x00,   // (24:56) 8
0x00,0x30,0xCC,0xCC,0x3C,0x0C,0xF0,0x00,   // (25:57) 9
0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,   // (26:58) :
0x00,0x00,0x30,0x00,0x00,0x30,0xC0,0x00,   // (27:59) ;
0x00,0x00,0x0C,0x30,0xC0,0x30,0x0C,0x00,   // (28:60) <
0x00,0x00,0xFC,0x00,0xFC,0x00,0x00,0x00,   // (29:61) =
0x00,0x00,0xC0,0x30,0x0C,0x30,0xC0,0x00,   // (30:62) >
0x00,0x30,0xCC,0x0C,0x30,0x00,0x30,0x00,   // (31:63) ?
0x00,0x30,0xCC,0xCC,0xC0,0xC0,0x3C,0x00,   // (32:64) @
0x00,0x30,0xCC,0xCC,0xFC,0xCC,0xCC,0x00,   // (33:65) A
0x00,0xF0,0xCC,0xF0,0xCC,0xCC,0xF0,0x00,   // (34:66) B
0x00,0x30,0xCC,0xC0,0xC0,0xCC,0x30,0x00,   // (35:67) C
0x00,0xF0,0xCC,0xCC,0xCC,0xCC,0xF0,0x00,   // (36:68) D
0x00,0xFC,0xC0,0xFC,0xC0,0xC0,0xFC,0x00,   // (37:69) E
0x00,0xFC,0xC0,0xFC,0xC0,0xC0,0xC0,0x00,   // (38:70) F
0x00,0x30,0xCC,0xC0,0xCC,0xCC,0x30,0x00,   // (39:71) G
0x00,0xCC,0xCC,0xFC,0xCC,0xCC,0xCC,0x00,   // (40:72) H
0x00,0xFC,0x30,0x30,0x30,0x30,0xFC,0x00,   // (41:73) I
0x00,0x0C,0x0C,0x0C,0x0C,0xCC,0x30,0x00,   // (42:74) J
0x00,0xCC,0xCC,0xF0,0xCC,0xCC,0xCC,0x00,   // (43:75) K
0x00,0xC0,0xC0,0xC0,0xC0,0xC0,0xFC,0x00,   // (44:76) L
0x00,0xCC,0xFC,0xCC,0xCC,0xCC,0xCC,0x00,   // (45:77) M
0x00,0x0C,0xCC,0xFC,0xFC,0xCC,0xC0,0x00,   // (46:78) N
0x00,0x30,0xCC,0xCC,0xCC,0xCC,0x30,0x00,   // (47:79) O
0x00,0xF0,0xCC,0xCC,0xF0,0xC0,0xC0,0x00,   // (48:80) P
0x00,0x30,0xCC,0xCC,0xCC,0xF0,0x3C,0x00,   // (49:81) Q
0x00,0xF0,0xCC,0xCC,0xF0,0xCC,0xCC,0x00,   // (50:82) R
0x00,0x30,0xCC,0x30,0x0C,0xCC,0x30,0x00,   // (51:83) S
0x00,0xFC,0x30,0x30,0x30,0x30,0x30,0x00,   // (52:84) T
0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0xFC,0x00,   // (53:85) U
0x00,0xCC,0xCC,0xCC,0xCC,0xCC,0x30,0x00,   // (54:86) V
0x00,0xCC,0xCC,0xCC,0xCC,0xFC,0xCC,0x00,   // (55:87) W
0x00,0xCC,0xCC,0x30,0xCC,0xCC,0xCC,0x00,   // (56:88) X
0x00,0xCC,0xCC,0x30,0x30,0x30,0x30,0x00,   // (57:89) Y
0x00,0xFC,0x0C,0x30,0x30,0xC0,0xFC,0x00,   // (58:90) Z
0x00,0x3C,0x30,0x30,0x30,0x30,0x3C,0x00,   // (59:91) [
0x00,0xC0,0xC0,0x30,0x30,0x0C,0x0C,0x00,   // (60:92) \
0x00,0xF0,0x30,0x30,0x30,0x30,0xF0,0x00,   // (61:93) ]
0x00,0x30,0xCC,0x00,0x00,0x00,0x00,0x00,   // (62:94) ^
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,   // (63:95) _
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   // (64:0)
0x00,0x0C,0x3F,0xFF,0x3F,0x0C,0x00,0x00,   // (65:1)
0x00,0x0C,0x33,0x0C,0x33,0x0C,0x33,0x0C,   // (66:2)
0x33,0x33,0x3F,0x33,0x33,0x0F,0x03,0x03,   // (67:3)
0xFC,0xC0,0xF0,0xCF,0xCC,0x0F,0x0C,0x0C,   // (68:4)
0x3C,0xC0,0x3C,0x00,0x0F,0x0C,0x0F,0x0C,   // (69:5)
0xC0,0xC0,0xC0,0xFC,0x0F,0x0C,0x0F,0x0C,   // (70:6)
0x00,0x3F,0x33,0x33,0x3F,0x00,0x00,0x00,   // (71:7)
0x00,0x0C,0x3F,0x0C,0x00,0x3F,0x00,0x00,   // (72:8)
0xC3,0xF3,0xCF,0xC3,0x0C,0x0C,0x0C,0x0F,   // (73:9)
0xCC,0xCC,0xCC,0x30,0x0F,0x03,0x03,0x03,   // (74:10)
0x0C,0x0C,0x0C,0xFC,0x00,0x00,0x00,0x00,   // (75:11)
0x00,0x00,0x00,0xFC,0x0C,0x0C,0x0C,0x0C,   // (76:12)
0x00,0x00,0x00,0x0F,0x0C,0x0C,0x0C,0x0C,   // (77:13)
0x0C,0x0C,0x0C,0x0F,0x00,0x00,0x00,0x00,   // (78:14)
0x0C,0x0C,0x0C,0xFF,0x0C,0x0C,0x0C,0x0C,   // (79:15)
0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,   // (80:16)
0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,   // (81:17)
0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,   // (82:18)
0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,   // (83:19)
0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,   // (84:20)
0x0C,0x0C,0x0C,0x0F,0x0C,0x0C,0x0C,0x0C,   // (85:21)
0x0C,0x0C,0x0C,0xFC,0x0C,0x0C,0x0C,0x0C,   // (86:22)
0x0C,0x0C,0x0C,0x0C,0xFF,0x00,0x00,0x00,   // (87:23)
0x00,0x00,0x00,0x00,0xFF,0x0C,0x0C,0x0C,   // (88:24)
0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,   // (89:25)
0x00,0x03,0x0C,0x30,0x0C,0x03,0x3F,0x00,   // (90:26)
0x00,0x30,0x0C,0x03,0x0C,0x30,0x3F,0x00,   // (91:27)
0x00,0xFF,0x33,0x33,0x33,0x33,0x00,0x00,   // (92:28)
0x00,0x03,0xFF,0x0C,0xFF,0x30,0x00,0x00,   // (93:29)
0x0F,0x30,0xFC,0x30,0x3F,0xF3,0x30,0x00,   // (94:30)
0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,   // (95:31)
0x00,0xC0,0x30,0x00,0x00,0x00,0x00,0x00,   // (96:96) `
0x00,0x00,0xF0,0x0C,0x3C,0xCC,0x3C,0x00,   // (97:97) a
0x00,0xC0,0xC0,0xF0,0xCC,0xCC,0xF0,0x00,   // (98:98) b
0x00,0x00,0x3C,0xC0,0xC0,0xC0,0x3C,0x00,   // (99:99) c
0x00,0x0C,0x0C,0x3C,0xCC,0xCC,0x3C,0x00,   // (100:100) d
0x00,0x00,0x30,0xCC,0xFC,0xC0,0x3C,0x00,   // (101:101) e
0x00,0x0C,0x30,0xFC,0x30,0x30,0x30,0x00,   // (102:102) f
0x00,0x00,0x3C,0xCC,0xCC,0x3C,0x0C,0xF0,   // (103:103) g
0x00,0xC0,0xC0,0xF0,0xCC,0xCC,0xCC,0x00,   // (104:104) h
0x00,0x30,0x00,0xF0,0x30,0x30,0xFC,0x00,   // (105:105) i
0x00,0x0C,0x00,0x3C,0x0C,0x0C,0x0C,0xF0,   // (106:106) j
0x00,0xC0,0xC0,0xCC,0xF0,0xCC,0xCC,0x00,   // (107:107) k
0x00,0x30,0x30,0x30,0x30,0x30,0x30,0x00,   // (108:108) l
0x00,0x00,0xCC,0xFC,0xCC,0xCC,0xCC,0x00,   // (109:109) m
0x00,0x00,0xF0,0xCC,0xCC,0xCC,0xCC,0x00,   // (110:110) n
0x00,0x00,0x30,0xCC,0xCC,0xCC,0x30,0x00,   // (111:111) o
0x00,0x00,0xF0,0xCC,0xCC,0xF0,0xC0,0xC0,   // (112:112) p
0x00,0x00,0x3C,0xCC,0xCC,0x3C,0x0C,0x0C,   // (113:113) q
0x00,0x00,0x3C,0xC0,0xC0,0xC0,0xC0,0x00,   // (114:114) r
0x00,0x00,0x3C,0xC0,0x30,0x0C,0xF0,0x00,   // (115:115) s
0x00,0x30,0x30,0xFC,0x30,0x30,0x30,0x00,   // (116:116) t
0x00,0x00,0xCC,0xCC,0xCC,0xCC,0xFC,0x00,   // (117:117) u
0x00,0x00,0xCC,0xCC,0xCC,0xCC,0x30,0x00,   // (118:118) v
0x00,0x00,0xCC,0xCC,0xCC,0xFC,0xCC,0x00,   // (119:119) w
0x00,0x00,0xCC,0xCC,0x30,0xCC,0xCC,0x00,   // (120:120) x
0x00,0x00,0xCC,0xCC,0xCC,0x3C,0x0C,0xF0,   // (121:121) y
0x00,0x00,0xFC,0x0C,0x30,0xC0,0xFC,0x00,   // (122:122) z
0x0C,0x30,0x30,0xC0,0x30,0x30,0x0C,0x00,   // (123:123) {
0x30,0x30,0x30,0x00,0x30,0x30,0x30,0x00,   // (124:124) |
0xC0,0x30,0x30,0x0C,0x30,0x30,0xC0,0x00,   // (125:125) }
0x00,0x00,0xF0,0x3C,0x00,0x00,0x00,0x00,   // (126:126) ~
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00    // (127:127) 
};
#endif
