#as: -mcpu=arc700 -mnps400
#objdump: -dr

.*: +file format .*arc.*

Disassembly of section .text:

[0-9a-f]+ <.*>:
   0:	4945 8000           	encr	r2,\[cm:r1\],\[cm:r1\],r2
   4:	4945 8028           	encr	r2,\[cm:r1\],\[cm:r1\],0x1
   8:	4945 87e8           	encr	r2,\[cm:r1\],\[cm:r1\],0x3f
   c:	4945 9008           	encr	r2,\[cm:r1\],\[cm:r1\],0x80
  10:	4945 8001           	decr	r2,\[cm:r1\],\[cm:r1\],r2
  14:	4945 8029           	decr	r2,\[cm:r1\],\[cm:r1\],0x1
  18:	4945 87e9           	decr	r2,\[cm:r1\],\[cm:r1\],0x3f
  1c:	4945 9009           	decr	r2,\[cm:r1\],\[cm:r1\],0x80
  20:	4945 a012           	shash	r2,\[cm:r1\],r2,0x1,0x1
  24:	4945 a002           	shash	r2,\[cm:r1\],r2,0x1,0
  28:	4945 8012           	shash	r2,\[cm:r1\],r2,0,0x1
  2c:	4945 a3fa           	shash	r2,\[cm:r1\],0x1f,0x1,0x1
  30:	4945 b00a           	shash	r2,\[cm:r1\],0x80,0x1,0
  34:	4945 e002           	hmac.ipad	r2,\[cm:r1\],r2
  38:	4945 e64a           	hmac.ipad	r2,\[cm:r1\],0x32
  3c:	4945 c012           	hmac.opad	r2,\[cm:r1\],r2
  40:	4945 c65a           	hmac.opad	r2,\[cm:r1\],0x32
  44:	4845 8003           	expskey	\[sm:r2\]
  48:	4945 8044           	cpssta	\[sm:r2\],\[cm:r1\]
  4c:	4a25 8004           	cpssta	\[cm:r2\],\[sm:r1\]
  50:	4945 8054           	cpsiv	\[sm:r2\],\[cm:r1\]
  54:	4a25 8014           	cpsiv	\[cm:r2\],\[sm:r1\]
  58:	4945 804c           	cpskey	\[sm:r2\],\[cm:r1\]
  5c:	4a25 800c           	cpskey	\[cm:r2\],\[sm:r1\]
  60:	4945 8064           	cpsmac	\[sm:r2\],\[cm:r1\]
  64:	4a25 8024           	cpsmac	\[cm:r2\],\[sm:r1\]
  68:	4945 805c           	cpsctx	\[sm:r2\],\[cm:r1\]
  6c:	4a25 801c           	cpsctx	\[cm:r2\],\[sm:r1\]
  70:	4945 8005           	gcm.fin	r2,\[cm:r1\],r2
  74:	4945 8fed           	gcm.fin	r2,\[cm:r1\],0x7f
  78:	4945 a016           	gensiv	r2,\[cm:r1\],r2,0x1,0x1
  7c:	4945 8016           	gensiv	r2,\[cm:r1\],r2,0,0x1
  80:	4945 a006           	gensiv	r2,\[cm:r1\],r2,0x1,0
  84:	4945 a6be           	gensiv	r2,\[cm:r1\],0x35,0x1,0x1
