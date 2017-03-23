#as: -mcpu=arc700 -mnps400
#objdump: -dr

.*: +file format .*arc.*

Disassembly of section .text:

[0-9a-f]+ <.*>:
   0:	3815 0042           	whash	r2,\[cm:r0\],r1
   4:	3b15 0385           	whash	r5,\[cm:r3\],r14
   8:	3815 007e           	whash	0,\[cm:r0\],r1
   c:	3b15 03be           	whash	0,\[cm:r3\],r14
  10:	3855 01c2           	whash	r2,\[cm:r0\],0x7
  14:	3855 01fe           	whash	0,\[cm:r0\],0x7
  18:	3855 0002           	whash	r2,\[cm:r0\],0x40
  1c:	3855 003e           	whash	0,\[cm:r0\],0x40
  20:	4822 4000           	mcmp	r0,\[cm:r0\],\[cm:r1\],r1
  24:	4822 6000           	mcmp\.s	r0,\[cm:r0\],\[cm:r1\],r1
  28:	4822 4080           	mcmp\.m	r0,\[cm:r0\],\[cm:r1\],r1
  2c:	4822 6080           	mcmp\.s\.m	r0,\[cm:r0\],\[cm:r1\],r1
  30:	4822 0000           	mcmp	r0,\[cm:r0,r0\],\[cm:r1\],r1
  34:	4822 2000           	mcmp\.s	r0,\[cm:r0,r0\],\[cm:r1\],r1
  38:	4822 0080           	mcmp\.m	r0,\[cm:r0,r0\],\[cm:r1\],r1
  3c:	4822 2080           	mcmp\.s\.m	r0,\[cm:r0,r0\],\[cm:r1\],r1
  40:	4822 4100           	mcmp	r0,\[cm:r0,0x4\],\[cm:r1\],r1
  44:	4822 6100           	mcmp\.s	r0,\[cm:r0,0x4\],\[cm:r1\],r1
  48:	4822 4180           	mcmp\.m	r0,\[cm:r0,0x4\],\[cm:r1\],r1
  4c:	4822 6180           	mcmp\.s\.m	r0,\[cm:r0,0x4\],\[cm:r1\],r1
  50:	4822 4200           	mcmp	r0,\[cm:r0,0x8\],\[cm:r1\],r1
  54:	4822 4300           	mcmp	r0,\[cm:r0,0xc\],\[cm:r1\],r1
  58:	4822 c004           	mcmp	r0,\[cm:r0\],\[cm:r1\],0x4
  5c:	4822 e004           	mcmp\.s	r0,\[cm:r0\],\[cm:r1\],0x4
  60:	4822 c084           	mcmp\.m	r0,\[cm:r0\],\[cm:r1\],0x4
  64:	4822 e088           	mcmp\.s\.m	r0,\[cm:r0\],\[cm:r1\],0x8
  68:	4822 c07f           	mcmp	r0,\[cm:r0\],\[cm:r1\],0x7f
  6c:	4822 c204           	mcmp	r0,\[cm:r0,0x8\],\[cm:r1\],0x4
  70:	4822 e204           	mcmp\.s	r0,\[cm:r0,0x8\],\[cm:r1\],0x4
  74:	4822 c284           	mcmp\.m	r0,\[cm:r0,0x8\],\[cm:r1\],0x4
  78:	4822 e284           	mcmp\.s\.m	r0,\[cm:r0,0x8\],\[cm:r1\],0x4
  7c:	4822 802e           	mcmp	r0,\[cm:r0,r0\],\[cm:r1\],0x2e
  80:	4822 a046           	mcmp\.s	r0,\[cm:r0,r0\],\[cm:r1\],0x46
  84:	4822 80c8           	mcmp\.m	r0,\[cm:r0,r0\],\[cm:r1\],0x48
  88:	4822 a0fd           	mcmp\.s\.m	r0,\[cm:r0,r0\],\[cm:r1\],0x7d
  8c:	3856 003e           	asri	0,r0
  90:	3856 007e           	asri\.core	0,r0
  94:	3856 00be           	asri\.clsr	0,r0
  98:	3856 00fe           	asri\.all	0,r0
  9c:	3856 013e           	asri\.gic	0,r0
  a0:	3856 017e           	rspi\.gic	0,r0
  a4:	385b 003e           	wkup	0,r0
  a8:	385b 013e           	wkup\.cl
  ac:	3a2f 0024           	getsti	r2,\[cm:r0\]
  b0:	3e2f 7024           	getsti	0,\[cm:r0\]
000000b4 <label>:
  b4:	3a2f 0025           	getrtc	r2,\[cm:r0\]
  b8:	3e2f 7025           	getrtc	0,\[cm:r0\]
  bc:	07f8 ffd5           	bnj	-8
  c0:	07f4 ffd7           	bnm	-12
  c4:	07f0 ffd8           	bnt	-16
  c8:	57c1 0fc5 2000 0008 	mmnt	0,\[r1\],0x2
  d0:	57c1 17c5 0000 0000 	mmnt	0,\[r1\],r2
  d8:	57c1 0fc5 a000 0009 	mmnt	0,\[xa:r1\],0xa
  e0:	57c1 0fc5 4000 0009 	mmnt	0,\[xa:r1\],0x4
  e8:	57c1 0fc5 2300 101b 	mmnt	0,\[sd:r1,0x10,0x30\],0x2
  f0:	57c1 17c5 0400 2013 	mmnt	0,\[sd:r1,0x20,0x40\],r2
  f8:	57c1 17c5 b000 000b 	mmnt	0,\[sd:r1,r2,r2\],0xb
 100:	57c1 17c5 0000 0003 	mmnt	0,\[sd:r1,r2,r2\],r2
 108:	57c1 0fc5 8200 401c 	mmnt	0,\[xd:r1,0x40,0x20\],0x8
 110:	57c1 17c5 0400 1014 	mmnt	0,\[xd:r1,0x10,0x40\],r2
 118:	57c1 17c5 9000 000c 	mmnt	0,\[xd:r1,r2,r2\],0x9
 120:	57c1 17c5 0000 0004 	mmnt	0,\[xd:r1,r2,r2\],r2
 128:	390e 0083           	lkpitcm	r3,\[cm:r1\],\[cm:r1\],r2
 12c:	390e 00be           	lkpitcm	0,\[cm:r1\],\[cm:r1\],r2
 130:	390e 0f83 0038 041e 	lkpitcm	r3,\[cm:r1\],\[cm:r1\],0x4,0x7,0x1e
 138:	390e 0f83 03b8 120a 	lkpitcm	r3,\[cm:r1\],\[cm:r1\],0x12,0x77,0xa
 140:	390e 0f83 1ff8 0650 	lkpitcm	r3,\[cm:r1\],\[cm:r1\],0x6,0x3ff,0x50
 148:	390e 0fbe 1ff8 0650 	lkpitcm	0,\[cm:r1\],\[cm:r1\],0x6,0x3ff,0x50
 150:	0764 ffd3           	blb2	-156
 154:	390f 0083           	lkpetcm	r3,\[cm:r1\],\[cm:r1\],r2
 158:	390f 00be           	lkpetcm	0,\[cm:r1\],\[cm:r1\],r2
 15c:	390f 0f83 1080 1220 	lkpetcm	r3,\[cm:r1\],\[cm:r1\],0x12,0x2,0x20,0x10
 164:	390f 0fbe 1080 1250 	lkpetcm	0,\[cm:r1\],\[cm:r1\],0x12,0x2,0x50,0x10
 16c:	0748 ffd2           	blb	-184