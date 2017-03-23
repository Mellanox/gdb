        .text

        ; Security instructions
        encr r2, [cm: r1], [cm: r1], r2
        encr r2, [cm: r1], [cm: r1], 1
        encr r2, [cm: r1], [cm: r1], 0x3f
        encr r2, [cm: r1], [cm: r1], 0x80
        decr r2, [cm: r1], [cm: r1], r2
        decr r2, [cm: r1], [cm: r1], 1
        decr r2, [cm: r1], [cm: r1], 0x3f
        decr r2, [cm: r1], [cm: r1], 0x80

        shash r2,[cm:r1],r2,1,1
        shash r2,[cm:r1],r2,1,0
        shash r2,[cm:r1],r2,0,1
        shash r2,[cm:r1],0x1f,1,1
        shash r2,[cm:r1],0x80,1,0

        hmac.ipad r2,[cm:r1],r2
        hmac.ipad r2,[cm:r1],0x32
        hmac.opad r2,[cm:r1],r2
        hmac.opad r2,[cm:r1],0x32

        expskey [sm:r2]

        cpssta [sm:r2],[cm:r1]
        cpssta [cm:r2],[sm:r1]
		cpsiv [sm:r2],[cm:r1]
        cpsiv [cm:r2],[sm:r1]
        cpskey [sm:r2],[cm:r1]
        cpskey [cm:r2],[sm:r1]
        cpsmac [sm:r2],[cm:r1]
        cpsmac [cm:r2],[sm:r1]
        cpsctx [sm:r2],[cm:r1]
        cpsctx [cm:r2],[sm:r1]

        gcm.fin r2,[cm:r1],r2
        gcm.fin r2,[cm:r1],0x7f

        gensiv r2,[cm:r1],r2,1,1
        gensiv r2,[cm:r1],r2,0,1
        gensiv r2,[cm:r1],r2,1,0
        gensiv r2,[cm:r1],0x35,1,1
