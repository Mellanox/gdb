#as: -mcpu=arc700 -mnps600
#objdump: -dr

.*: +file format .*arc.*

Disassembly of section .text:

[0-9a-f]+ <.*>:
   0:	3c2f 00a6           	jobinc	r4,\[cjid:r2\]
   4:	382f 00a6           	jobinc	r0,\[cjid:r2\]
   8:	392f 2026           	jobinc	r17,\[cjid:r0\]
   c:	3c2f 00a7           	jobdec	r4,\[cjid:r2\]
  10:	382f 00a7           	jobdec	r0,\[cjid:r2\]
  14:	392f 2027           	jobdec	r17,\[cjid:r0\]
  18:	57c0 07c5 2000 0028 	immnt	0,\[r0\],0x2
  20:	57c0 0fc5 0000 0020 	immnt	0,\[r0\],r1
  28:	57c0 07c5 2000 0029 	immnt	0,\[xa:r0\],0x2
  30:	57c0 0fc5 0000 0021 	immnt	0,\[xa:r0\],r1
  38:	57c0 07c5 0100 103b 	immnt	0,\[sd:r0,0x10,0x10\],0
  40:	57c0 0fc5 0100 1033 	immnt	0,\[sd:r0,0x10,0x10\],r1
  48:	57c0 0fc5 2000 002b 	immnt	0,\[sd:r0,r1,r1\],0x2
  50:	57c0 0fc5 0000 0023 	immnt	0,\[sd:r0,r1,r1\],r1
  58:	4960 0018           	ldjob.fb	r1,r1,r3
  5c:	4960 80d8           	ldjob.fb	r1,r1,r3,0x2,0x1
  60:	4960 0019           	transjob	r1,r1,r3
  64:	4960 8019           	transjob	r1,r1,r3,0,0,0,0
  68:	4960 81f9           	transjob	r1,r1,r3,0x1,0x1,0x1,0x1
  6c:	4960 80b9           	transjob	r1,r1,r3,0x1,0,0x1,0
  70:	4960 8159           	transjob	r1,r1,r3,0,0x1,0,0x1
  74:	4960 001a           	bfm.nl	r1,r1,r3
  78:	4960 817a           	bfm.nl	r1,r1,r3,0x1,0x1,0x1
  7c:	4960 801a           	bfm.nl	r1,r1,r3,0,0,0
  80:	4960 80ba           	bfm.nl	r1,r1,r3,0x1,0,0x2
  84:	3b26 0200           	csub	r0,\[cm:r3\],\[cm:r3\],r8
  88:	3a26 01c1           	csub	r1,\[cm:r2\],\[cm:r2\],r7
  8c:	3b26 023e           	csub	0,\[cm:r3\],\[cm:r3\],r8
  90:	3b26 0f82 1000 0210 	csub	r2,\[cm:r3\],\[cm:r3\],0x10,0x2,0,0x10
  98:	3b26 0f82 1001 3620 	csub	r2,\[cm:r3\],\[cm:r3\],0x20,0x36,0x1,0x10
  a0:	3b26 0f82 1000 3620 	csub	r2,\[cm:r3\],\[cm:r3\],0x20,0x36,0,0x10
  a8:	3b26 0f82 4001 6430 	csub	r2,\[cm:r3\],\[cm:r3\],0x30,0x64,0x1,0x40
  b0:	3b26 0f82 4001 6430 	csub	r2,\[cm:r3\],\[cm:r3\],0x30,0x64,0x1,0x40
  b8:	3b26 0f82 0000 6230 	csub	r2,\[cm:r3\],\[cm:r3\],0x30,0x62,0,0
  c0:	3b26 0fbe 1000 0210 	csub	0,\[cm:r3\],\[cm:r3\],0x10,0x2,0,0x10
  c8:	3b26 0fbe 1001 3620 	csub	0,\[cm:r3\],\[cm:r3\],0x20,0x36,0x1,0x10
  d0:	3b26 0fbe 1000 3620 	csub	0,\[cm:r3\],\[cm:r3\],0x20,0x36,0,0x10
  d8:	3b26 0fbe 4001 6430 	csub	0,\[cm:r3\],\[cm:r3\],0x30,0x64,0x1,0x40
  e0:	3b26 0fbe 4001 6430 	csub	0,\[cm:r3\],\[cm:r3\],0x30,0x64,0x1,0x40
  e8:	3b26 0fbe 0000 6230 	csub	0,\[cm:r3\],\[cm:r3\],0x30,0x62,0,0
  f0:	0008 0019           	bnl	8
  f4:	264a 7000           	mov	0,0
000000f8 <label>:
  f8:	264a 7000           	mov	0,0
