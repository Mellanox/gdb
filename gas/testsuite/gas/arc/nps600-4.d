#as: -mcpu=arc700 -mnps600
#objdump: -dr

.*: +file format .*arc.*

Disassembly of section .text:

[0-9a-f]+ <.*>:
   0:	5044 27c0 2028 0000 	dcmac	r2,\[cm:r4\],\[cm:r4\],0x28
   8:	5044 27c0 2224 1234 	dcmac	r2,\[cm:r4\],\[cm:0x1234\],0x24
  10:	5044 27c0 2328 4321 	dcmac	r2,\[cm:0x4321\],\[cm:r4\],0x28
  18:	57c4 27c0 2024 0000 	dcmac	0,\[cm:r4\],\[cm:r4\],0x24
  20:	57c4 27c0 2228 1234 	dcmac	0,\[cm:r4\],\[cm:0x1234\],0x28
  28:	57c4 27c0 2324 4321 	dcmac	0,\[cm:0x4321\],\[cm:r4\],0x24
  30:	5044 27c0 2818 0000 	dcmpls	r2,\[cm:r4\],\[cm:r4\],0x18
  38:	5044 27c0 2a18 1234 	dcmpls	r2,\[cm:r4\],\[cm:0x1234\],0x18
  40:	5044 27c0 2b18 8765 	dcmpls	r2,\[cm:0x8765\],\[cm:r4\],0x18
  48:	57c4 27c0 2818 0000 	dcmpls	0,\[cm:r4\],\[cm:r4\],0x18
  50:	57c4 27c0 2a18 1234 	dcmpls	0,\[cm:r4\],\[cm:0x1234\],0x18
  58:	57c4 27c0 2b18 8765 	dcmpls	0,\[cm:0x8765\],\[cm:r4\],0x18
