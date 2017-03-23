        .text
        /* jobinc */
        jobinc r4,[cjid:r2]
        jobinc r0,[cjid:r2]
        jobinc r17,[cjid:r0]
        /* jobdec */
        jobdec r4,[cjid:r2]
        jobdec r0,[cjid:r2]
        jobdec r17,[cjid:r0]
        /* immnt */
        immnt 0, [r0], 2
        immnt 0, [r0], r1
        immnt 0, [xa:r0], 2
        immnt 0, [xa:r0], r1
        immnt 0, [sd:r0,16,16], 0
        immnt 0, [sd:r0,16,16], r1
        immnt 0, [sd:r0,r1,r1], 2
        immnt 0, [sd:r0,r1,r1], r1

        ldjob.fb   r1,r1,r3
        ldjob.fb   r1,r1,r3,2,1

        transjob   r1,r1,r3
        transjob   r1,r1,r3,0,0,0,0
        transjob   r1,r1,r3,1,1,1,1
        transjob   r1,r1,r3,1,0,1,0
        transjob   r1,r1,r3,0,1,0,1

        bfm.nl     r1,r1,r3
        bfm.nl     r1,r1,r3,1,1,1
        bfm.nl     r1,r1,r3,0,0,0
        bfm.nl     r1,r1,r3,1,0,2

        /* csub */
        csub r0, [cm:r3], [cm:r3], r8
        csub r1, [cm:r2], [cm:r2], r7
        csub 0, [cm:r3], [cm:r3], r8
        csub r2, [cm:r3], [cm:r3], 16,2,0,16
        csub r2, [cm:r3], [cm:r3], 32,54,1,16
        csub r2, [cm:r3], [cm:r3], 32,54,0,16
        csub r2, [cm:r3], [cm:r3], 48,100,1,64
        csub r2, [cm:r3], [cm:r3], 48,100,1,64
        csub r2, [cm:r3], [cm:r3], 48,98,0,0
        csub 0, [cm:r3], [cm:r3], 16,2,0,16
        csub 0, [cm:r3], [cm:r3], 32,54,1,16
        csub 0, [cm:r3], [cm:r3], 32,54,0,16
        csub 0, [cm:r3], [cm:r3], 48,100,1,64
        csub 0, [cm:r3], [cm:r3], 48,100,1,64
        csub 0, [cm:r3], [cm:r3], 48,98,0,0

        bnl label
        nop
label:
        nop
