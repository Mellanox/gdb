        .text

        ; cp BD to BD
        cp.e.m  r3,[cbd:r1,r2],[cbd:r1,r2],r2
        cp.m    r3,[cbd:r1,r2],[cbd:r1,r2],r2
        cp.e    r3,[cbd:r1,r2],[cbd:r1,r2],r2
        cp     	r3,[cbd:r1,r2],[cbd:r1,r2],r2

        cp.e.m  r3,[cbd:r1,r2,r2],[cbd:r1,r2,r2],r2
        cp.m    r3,[cbd:r1,r2,r2],[cbd:r1,r2,r2],r2
        cp.e    r3,[cbd:r1,r2,r2],[cbd:r1,r2,r2],r2
        cp      r3,[cbd:r1,r2,r2],[cbd:r1,r2,r2],r2

        cp.e.m  r3,[cbd:r1,r2],[cbd:0x12,r2],r2
        cp.m    r3,[cbd:r1,r2],[cbd:0x12,r2],r2
        cp.e    r3,[cbd:r1,r2],[cbd:0x12,r2],r2
        cp      r3,[cbd:r1,r2],[cbd:0x12,r2],r2

        cp.e.m  r3,[cbd:0x23,r2],[cbd:r1,r2],r2
        cp.m    r3,[cbd:0x23,r2],[cbd:r1,r2],r2
        cp.e    r3,[cbd:0x23,r2],[cbd:r1,r2],r2
        cp      r3,[cbd:0x23,r2],[cbd:r1,r2],r2

        cp.m    r3,[cbd:r1,r2],[cbd:0x33,r2], 0x28
        cp      r3,[cbd:r1,r2],[cbd:0x33,r2], 0x28

        cp.m    r3,[cbd:0x42,r2],[cbd:r1,r2], 0x48
        cp      r3,[cbd:0x42,r2],[cbd:r1,r2], 0x48

        cp.m    r3,[cbd:r1,0x44],[cbd:0x1234,r2],r2
        cp      r3,[cbd:r1,0x44],[cbd:0x1234,r2],r2

        cp.m    r3,[cbd:0x1234,0x54],[cbd:r1,r2],r2
        cp      r3,[cbd:0x1234,0x54],[cbd:r1,r2],r2

        cp.m    r3,[cbd:r1,0x254],[cbd:r1,r2],r2
        cp      r3,[cbd:r1,0x254],[cbd:r1,r2],r2

        cp.m    r3,[cbd:r1,0x254,r2],[cbd:r1,r2,r2],r2
        cp      r3,[cbd:r1,0x254,r2],[cbd:r1,r2,r2],r2

        cp.m    r3,[cbd:r1,0x254],[cbd:r1,r2],0x222
        cp      r3,[cbd:r1,0x254],[cbd:r1,r2],0x222

        cp.m    r3,[cbd:r1,0x254, r2],[cbd:r1,r2, r2],0x111
        cp      r3,[cbd:r1,0x254, r2],[cbd:r1,r2, r2],0x111

        cp.m.e  r3,[cbd:r1,0x254],[cbd:r1,0x254],r2
        cp.e    r3,[cbd:r1,0x254],[cbd:r1,0x254],r2

        cp.m.e  r3,[cbd:r1,0x254,r2],[cbd:r1,0x254,r2],r2
        cp.e    r3,[cbd:r1,0x254,r2],[cbd:r1,0x254,r2],r2

        cp.m.e  r3,[cbd:r1,0x254],[cbd:r1,0x254],0x123
        cp.e    r3,[cbd:r1,0x254],[cbd:r1,0x254],0x123

        cp.m.e  r3,[cbd:r1,0x254,r2],[cbd:r1,0x254, r2],0x123
        cp.e    r3,[cbd:r1,0x254,r2],[cbd:r1,0x254, r2],0x123

        cp.e.m  r3,[cbd:r1,r2],[cbd:r1,r2],0x123
        cp.m    r3,[cbd:r1,r2],[cbd:r1,r2],0x123
        cp.e    r3,[cbd:r1,r2],[cbd:r1,r2],0x123
        cp      r3,[cbd:r1,r2],[cbd:r1,r2],0x123

        cp.e.m  r3,[cbd:r1,r2,r2],[cbd:r1,r2,r2],0x123
        cp.m    r3,[cbd:r1,r2,r2],[cbd:r1,r2,r2],0x123
        cp.e    r3,[cbd:r1,r2,r2],[cbd:r1,r2,r2],0x123
        cp      r3,[cbd:r1,r2,r2],[cbd:r1,r2,r2],0x123

        ; cp BD to XA
        cp.m r3,[cxa:r1],[cbd:r1,r2],r2
        cp   r3,[cxa:r1],[cbd:r1,r2],r2

        cp.m r3,[cxa:r1,r2],[cbd:r1,r2,r2],r2
        cp   r3,[cxa:r1,r2],[cbd:r1,r2,r2],r2

        cp.m r3,[cxa:r1],[cbd:0x2222,r2],r2
        cp   r3,[cxa:r1],[cbd:0x2222,r2],r2

        cp.m r3,[cxa:0x1234],[cbd:r1,r2],r2
        cp   r3,[cxa:0x1234],[cbd:r1,r2],r2

        cp.m r3,[cxa:r1],[cbd:0x2222,r2],0x46
        cp   r3,[cxa:r1],[cbd:0x2222,r2],0x46

        cp.m r3,[cxa:0x1234],[cbd:r1,r2],0x46
        cp   r3,[cxa:0x1234],[cbd:r1,r2],0x46

        cp.m r3,[cxa:r1],[cbd:r1,r2],0xaa
        cp   r3,[cxa:r1],[cbd:r1,r2],0xaa

        cp.m r3,[cxa:r1,r2],[cbd:r1,r2,r2],0x44
        cp   r3,[cxa:r1,r2],[cbd:r1,r2, r2],0x44

        ; cp XA to BD
        cp.m r3,[cbd:r1,r2],[cxa:r1],r2
        cp   r3,[cbd:r1,r2],[cxa:r1],r2

        cp.m r3,[cbd:r1,r2,r2],[cxa:r1,r2],r2
        cp   r3,[cbd:r1,r2,r2],[cxa:r1,r2],r2

        cp.m r3,[cbd:r1,r2],[cxa:0x1234],r2
        cp   r3,[cbd:r1,r2],[cxa:0x1234],r2

        cp.m r3,[cbd:0x4321,r2],[cxa:r1],r2
        cp r3,[cbd:0x4321,r2],[cxa:r1],r2

        cp.m r3,[cbd:r1,r2],[cxa:0x1234],0x66
        cp   r3,[cbd:r1,r2],[cxa:0x1234],0x66

        cp.m r3,[cbd:0x4321,r2],[cxa:r1],0xbb
        cp r3,[cbd:0x4321,r2],[cxa:r1],0xbb

        cp.m r3,[cbd:r1,0x88],[cxa:0x1234],r2
        cp   r3,[cbd:r1,0x88],[cxa:0x1234],r2

        cp.m r3,[cbd:0x1234,0x55],[cxa:r1],r2
        cp   r3,[cbd:0x1234,0x55],[cxa:r1],r2

        cp.m r3,[cbd:r1,0x222],[cxa:r1],r2
        cp   r3,[cbd:r1,0x222],[cxa:r1],r2

        cp.m r3,[cbd:r1,0x222,r2],[cxa:r1,r2],r2
        cp   r3,[cbd:r1,0x222,r2],[cxa:r1,r2],r2

        cp.m r3,[cbd:r1,0x222],[cxa:r1],0x111
        cp   r3,[cbd:r1,0x222],[cxa:r1],0x111

        cp.m r3,[cbd:r1,0x333,r2],[cxa:r1,r2],0x111
        cp r3,[cbd:r1,0x333,r2],[cxa:r1,r2],0x111

        cp.m r3,[cbd:r1,r2],[cxa:r1],0x123
        cp r3,[cbd:r1,r2],[cxa:r1],0x123

        cp.m r3,[cbd:r1,r2,r2],[cxa:r1,r2],0x123
        cp r3,[cbd:r1,r2,r2],[cxa:r1,r2],0x123

        ; cp BD to CM
        cp r3, [cm:r1],[cbd:r1,r2],r2
        cp r3, [cm:r1],[cbd:r1,r2,r2],r2
        cp r3, [cm:r1],[cbd:0x1234,r2],r2
        cp r3, [cm:0x1234], [cbd:r1,r2],r2
        cp r3, [cm:r1],[cbd:0x1234,r2],0x44
        cp r3, [cm:0x1234], [cbd:r1,r2],0x44
        cp r3, [cm:r1],[cbd:0x1234,0x88],r2
        cp r3, [cm:0x8787], [cbd:r1,0x88],r2
        cp r3, [cm:r1],[cbd:r1,0x222],r2
        cp r3, [cm:r1],[cbd:r1,0x222,r2],r2
        cp r3, [cm:r1],[cbd:r1,0x222],0x234
        cp r3, [cm:r1],[cbd:r1,0x222, r2],0x234
        cp r3, [cm:r1],[cbd:r1,r2],0x234
        cp r3, [cm:r1],[cbd:r1,r2,r2],0x234

        ; cp CM to BD
        cp.m r3,[cbd:r1,r2],[cm:r1],r2
        cp   r3,[cbd:r1,r2],[cm:r1],r2

        cp.m r3,[cbd:r1,r2,r2],[cm:r1],r2
        cp   r3,[cbd:r1,r2,r2],[cm:r1],r2

        cp.m r3,[cbd:r1,r2],[cm:0x4545],r2
        cp r3,[cbd:r1,r2],[cm:0x4545],r2

        cp.m r3,[cbd:0x4545,r2],[cm:r1],r2
        cp   r3,[cbd:0x4545,r2],[cm:r1],r2

        cp.m r3,[cbd:r1,r2],[cm:0x4545],0x81
        cp   r3,[cbd:r1,r2],[cm:0x4545],0x81

        cp.m r3,[cbd:0x4545,r2],[cm:r1],0x88
        cp   r3,[cbd:0x4545,r2],[cm:r1],0x88

        cp.m r3,[cbd:r1,0x84],[cm:0x1234],r2
        cp   r3,[cbd:r1,0x84],[cm:0x1234],r2

        cp.m r3,[cbd:0x1234,0x84],[cm:r1],r2
        cp   r3,[cbd:0x1234,0x84],[cm:r1],r2

        cp.m r3,[cbd:r1,0x222],[cm:r1],r2
        cp   r3,[cbd:r1,0x222],[cm:r1],r2

        cp.m r3,[cbd:r1,0x222,r2],[cm:r1],r2
        cp   r3,[cbd:r1,0x222,r2],[cm:r1],r2

        cp.m r3,[cbd:r1,0x111],[cm:r1],0x222
        cp   r3,[cbd:r1,0x111],[cm:r1],0x222

        cp.m r3,[cbd:r1,0x222,r2],[cm:r1],0x222
        cp   r3,[cbd:r1,0x222,r2],[cm:r1],0x222

        cp.m r3,[cbd:r1,r2],[cm:r1],0x222
        cp   r3,[cbd:r1,r2],[cm:r1],0x222

        cp.m r3,[cbd:r1,r2,r2],[cm:r1],0x222
        cp   r3,[cbd:r1,r2,r2],[cm:r1],0x222

        ; cp CJID to CM
        cp r3,[cm:r1],[cjid:r1],r2
        cp r3,[cm:r1],[cjid:0x1234], r2
        cp r3,[cm:0x1234],[cjid:r1],r2
        cp r3,[cm:r1],[cjid:0x1234],0x44
        cp r3,[cm:0x1234],[cjid:r1],0x44
        cp r3,[cm:r1],[cjid:r1],0x222

        ; cp CM to CJID
        cp r3,[cjid:r1],[cm:r1],r2
        cp r3,[cjid: r1],[cm:0x1234],r2
        cp r3,[cjid:0x1234],[cm: r1],r2
        cp r3,[cjid:r1],[cm:0x1234 ],0x44
        cp r3,[cjid:0x1234],[cm:r1],0x44
        cp r3,[cjid:r1],[cm:r1],0x222

        ; cp BD to NULL
        cp r3,[cbd:r1,r2],r2
        cp r3,[cbd:r1,r2,r2],r2
        cp r3,[cbd:0x1234,r2],r2
        cp r3,[cbd:0x1234,r2],0x44
        cp r3,[cbd:r1,r2],0x222
        cp r3,[cbd:r1,r2,r2],0x222

        ;cp LBD to LBD
        cp.e r3, [clbd:r1,r2],[clbd:r1,r2],r2
        cp   r3, [clbd:r1,r2],[clbd:r1,r2],r2

        cp.e r3, [clbd:r1,r2,r2],[clbd:r1,r2,r2],r2
        cp   r3, [clbd:r1,r2,r2],[clbd:r1,r2,r2],r2

        cp.e r3, [clbd:r1,r2],[clbd:0x1234,r2],r2
        cp   r3, [clbd:r1,r2],[clbd:0x1234,r2],r2

        cp.e r3, [clbd:0x1234,r2],[clbd:r1,r2],r2
        cp   r3, [clbd:0x1234,r2],[clbd:r1,r2],r2

        cp   r3, [clbd:r1,r2],[clbd:0x4321,r2],0x22
        cp   r3, [clbd:0x1234,r2],[clbd:r1,r2],0x44
        cp   r3, [clbd:r1,0x45],[clbd:0x1234,r2],r2
        cp   r3, [clbd:0x1234,0x55],[clbd:r1,r2],r2
        cp   r3, [clbd:r1,0x222],[clbd:r1,r2],r2
        cp   r3, [clbd:r1,0x222,r2],[clbd:r1,r2,r2],r2
        cp   r3, [clbd:r1,0x222],[clbd:r1,r2],0x111
        cp   r3, [clbd:r1,0x222,r2],[clbd:r1,r2,r2],0x111

        cp.e  r3, [clbd:r1,0x222],[clbd:r1,0x222],r2
        cp.e  r3, [clbd:r1,0x222,r2],[clbd:r1,0x222,r2],r2
        cp.e  r3, [clbd:r1,0x222],[clbd:r1,0x222],0x111
        cp.e  r3, [clbd:r1,0x222,r2],[clbd:r1,0x222,r2],0x111
        cp.e  r3, [clbd:r1,r2],[clbd:r1,r2],0x111
        cp    r3, [clbd:r1,r2],[clbd:r1,r2],0x111
        cp.e  r3, [clbd:r1,r2,r2],[clbd:r1,r2,r2],0x111
        cp    r3, [clbd:r1,r2,r2],[clbd:r1,r2,r2],0x111

        ;cp LBD to XA
        cp r3,[cxa:r1],[clbd:r1,r2],r2
        cp r3,[cxa:r1,r2],[clbd:r1,r2,r2],r2
        cp r3,[cxa:r1],[clbd:0x1234,r2],r2
        cp r3,[cxa:0x1234],[clbd:r1,r2],r2
        cp r3,[cxa:r1],[clbd:0x1234,r2],r2
        cp r3,[cxa:0x1234],[clbd:r1,r2],0x44
        cp r3,[cxa:r1],[clbd:r1,r2],0x222
        cp r3,[cxa:r1,r2],[clbd:r1,r2,r2],0x222

        ;cp XA to LBD
        cp r3,[clbd:r1,r2],[cxa:r1],r2
        cp r3,[clbd:r1,r2,r2],[cxa:r1,r2],r2
        cp r3,[clbd:r1,r2],[cxa:0x1234],r2
        cp r3,[clbd:0x1234,r2],[cxa:r1],r2
        cp r3,[clbd:r1,r2],[cxa:0x1234],0x88
        cp r3,[clbd:0x1234,r2],[cxa:r1],0x88
        cp r3,[clbd:r1,0x44],[cxa:0x1234],r2
        cp r3,[clbd:0x1234,0x44],[cxa:r1],r2
        cp r3,[clbd:r1,0x222],[cxa:r1],r2
        cp r3,[clbd:r1,0x222,r2],[cxa:r1,r2],r2
        cp r3,[clbd:r1,0x222],[cxa:r1],0x333
        cp r3,[clbd:r1,0x222,r2],[cxa:r1,r2],0x333
        cp r3,[clbd:r1,r2],[cxa:r1],0x333
        cp r3,[clbd:r1,r2,r2],[cxa:r1,r2],0x333

        ;cp LBD to CM *
        cp r3,[cm:r1],[clbd:r1,r2],r2
        cp r3,[cm:r1],[clbd:r1,r2,r2],r2
        cp r3,[cm:r1],[clbd:0x1234,r2],r2
        cp r3,[cm:0x1234],[clbd:r1,r2],r2
        cp r3,[cm:r1],[clbd:0x1234,r2],0x44
        cp r3,[cm:0x4321],[clbd:r1,r2],0x44
        cp r3,[cm:r1],[clbd:0x1234,0x55],r2
        cp r3,[cm:0x1234],[clbd:r1,0x55],r2
        cp r3,[cm:r1],[clbd:r1,0x222],r2
        cp r3,[cm:r1],[clbd:r1,0x222,r2],r2
        cp r3,[cm:r1],[clbd:r1,0x222],0x111
        cp r3,[cm:r1],[clbd:r1,0x222,r2],0x111
        cp r3,[cm:r1],[clbd:r1,r2],0x111
        cp r3,[cm:r1],[clbd:r1,r2,r2],0x111

        ;cp CM to LBD
        cp r3, [clbd:r1,r2],[cm:r1],r2
        cp r3, [clbd:r1,r2,r2],[cm:r1],r2
        cp r3, [clbd:r1,r2],[cm:0x1234],r2
        cp r3, [clbd:0x1234,r2],[cm:r2],r2
        cp r3, [clbd:r1,r2],[cm:0x1234],0x44
        cp r3, [clbd:0x1234,r2],[cm:r1],0x44
        cp r3, [clbd:r1,0x44],[cm:0x1234],r2
        cp r3, [clbd:0x1234,0x44],[cm:r1],r2
        cp r3, [clbd:r1,0x222],[cm:r1],r2
        cp r3, [clbd:r1,0x222,r2],[cm:r1],r2
        cp r3, [clbd:r1,0x222],[cm:r1],0x111
        cp r3, [clbd:r1,0x222,r2],[cm:r1],0x111
        cp r3, [clbd:r1,r2],[cm:r1],0x111
        cp r3, [clbd:r1,r2,r2],[cm:r1],0x111

        ; cp XA to XA
        cp.m.na.x r3,[cxa:r1],[cxa:r1],r2
        cp.m.na r3,[cxa:r1],[cxa:r1],r2
        cp.m r3,[cxa:r1],[cxa:r1],r2
        cp r3,[cxa:r1],[cxa:r1],r2

        cp.m.na.x r3,[cxa:r1],[cxa:0x1234],r2
        cp.m.na r3,[cxa:r1],[cxa:0x1234],r2
        cp.m r3,[cxa:r1],[cxa:0x1234],r2
        cp r3,[cxa:r1],[cxa:0x1234],r2

        cp.m.na.x r3,[cxa:0x1234],[cxa:r1],r2
        cp.m.na r3,[cxa:0x1234],[cxa:r1],r2
        cp.m r3,[cxa:0x1234],[cxa:r1],r2
        cp r3,[cxa:0x1234],[cxa:r1],r2

        cp.m.na.x r3,[cxa:r1],[cxa:r1],0x4321
        cp.m.na r3,[cxa:r1],[cxa:r1],0x4321
        cp.m r3,[cxa:r1],[cxa:r1],0x4321
        cp r3,[cxa:r1],[cxa:r1],0x4321

        cp.m.na.x r3,[cxa:r1,r2],[cxa:r1,r2],r2
        cp.m.na r3,[cxa:r1,r2],[cxa:r1,r2],r2
        cp.m r3,[cxa:r1,r2],[cxa:r1,r2],r2
        cp r3,[cxa:r1,r2],[cxa:r1,r2],r2

        cp.m.na.x r3,[cxa:r1,r2],[cxa:0x1234,r2],r2
        cp.m.na r3,[cxa:r1,r2],[cxa:0x1234,r2],r2
        cp.m r3,[cxa:r1,r2],[cxa:0x1234,r2],r2
        cp r3,[cxa:r1,r2],[cxa:0x1234,r2],r2

        cp.m.na.x r3,[cxa:0x1234,r2],[cxa:r1,r2],r2
        cp.m.na r3,[cxa:0x1234,r2],[cxa:r1,r2],r2
        cp.m r3,[cxa:0x1234,r2],[cxa:r1,r2],r2
        cp r3,[cxa:0x1234,r2],[cxa:r1,r2],r2

        cp.m.na.x r3,[cxa:r1,r2],[cxa:r1,r2],0x1234
        cp.m.na r3,[cxa:r1,r2],[cxa:r1,r2],0x1234
        cp.m r3,[cxa:r1,r2],[cxa:r1,r2],0x1234
        cp r3,[cxa:r1,r2],[cxa:r1,r2],0x1234

        ; cp XA to CM
        cp.na.x r3,[cm:r1],[cxa:r1],r2
        cp.na r3,[cm:r1],[cxa:r1],r2
        cp r3,[cm:r1],[cxa:r1],r2

        cp.na.x r3,[cm:r1],[cxa:0x1234],r2
        cp.na r3,[cm:r1],[cxa:0x1234],r2
        cp r3,[cm:r1],[cxa:0x1234],r2

        cp.na.x r3,[cm:0x1234],[cxa:r1],r2
        cp.na r3,[cm:0x1234],[cxa:r1],r2
        cp r3,[cm:0x1234],[cxa:r1],r2

        cp.na.x r3,[cm:r1],[cxa:r1],0x4321
        cp.na r3,[cm:r1],[cxa:r1],0x4321
        cp r3,[cm:r1],[cxa:r1],0x4321

        cp.na.x r3,[cm:r1],[cxa:r1,r2],r2
        cp.na r3,[cm:r1],[cxa:r1,r2],r2
        cp r3,[cm:r1],[cxa:r1,r2],r2

        cp.na.x r3,[cm:r1],[cxa:0x1234,r2],r2
        cp.na r3,[cm:r1],[cxa:0x1234,r2],r2
        cp r3,[cm:r1],[cxa:0x1234,r2],r2

        cp.na.x r3,[cm:0x1234],[cxa:r1,r2],r2
        cp.na r3,[cm:0x1234],[cxa:r1,r2],r2
        cp r3,[cm:0x1234],[cxa:r1,r2],r2

        cp.na.x r3,[cm:r1],[cxa:r1,r2],0x1234
        cp.na r3,[cm:r1],[cxa:r1,r2],0x1234
        cp r3,[cm:r1],[cxa:r1,r2],0x1234

        ;cp CM to CX
        cp.m.na.x r3,[cxa:r1],[cm:r1],r2
        cp.m.na r3,[cxa:r1],[cm:r1],r2
        cp.m r3,[cxa:r1],[cm:r1],r2
        cp r3,[cxa:r1],[cm:r1],r2

        cp.m.na.x r3,[cxa:r1],[cm:0x1234],r2
        cp.m.na r3,[cxa:r1],[cm:0x1234],r2
        cp.m r3,[cxa:r1],[cm:0x1234],r2
        cp r3,[cxa:r1],[cm:0x1234],r2

        cp.m.na.x r3,[cxa:0x1234],[cm:r1],r2
        cp.m.na r3,[cxa:0x1234],[cm:r1],r2
        cp.m r3,[cxa:0x1234],[cm:r1],r2
        cp r3,[cxa:0x1234],[cm:r1],r2

        cp.m.na.x r3,[cxa:r1],[cm:r1],0x8888
        cp.m.na r3,[cxa:r1],[cm:r1],0x8888
        cp.m r3,[cxa:r1],[cm:r1],0x8888
        cp r3,[cxa:r1],[cm:r1],0x8888

        cp.m.na.x r3,[cxa:r1,r2],[cm:r1],r2
        cp.m.na r3,[cxa:r1,r2],[cm:r1],r2
        cp.m r3,[cxa:r1,r2],[cm:r1],r2
        cp r3,[cxa:r1,r2],[cm:r1],r2

        cp.m.na.x r3,[cxa:r1,r2],[cm:0x1234],r2
        cp.m.na r3,[cxa:r1,r2],[cm:0x1234],r2
        cp.m r3,[cxa:r1,r2],[cm:0x1234],r2
        cp r3,[cxa:r1,r2],[cm:0x1234],r2

        cp.m.na.x r3,[cxa:0x1234,r2],[cm:r1],r2
        cp.m.na r3,[cxa:0x1234,r2],[cm:r1],r2
        cp.m r3,[cxa:0x1234,r2],[cm:r1],r2
        cp r3,[cxa:0x1234,r2],[cm:r1],r2

        cp.m.na.x r3,[cxa:r1,r2],[cm:r1],0x8888
        cp.m.na r3,[cxa:r1,r2],[cm:r1],0x8888
        cp.m r3,[cxa:r1,r2],[cm:r1],0x8888
        cp r3,[cxa:r1,r2],[cm:r1],0x8888

        ; XA to NULL
        cp.na.x r3,[cxa:r1],r2
        cp.na r3,[cxa:r1],r2
        cp r3,[cxa:r1],r2

        cp.na.x r3,[cxa:0x1234],r2
        cp.na r3,[cxa:0x1234],r2
        cp r3,[cxa:0x1234],r2

        cp.na.x r3,[cxa:r1],0x1234
        cp.na r3,[cxa:r1],0x1234
        cp r3,[cxa:r1],0x1234

        cp.na.x r3,[cxa:r1,r2],r2
        cp.na r3,[cxa:r1,r2],r2
        cp r3,[cxa:r1,r2],r2

        cp.na.x r3,[cxa:0x1234,r2],r2
        cp.na r3,[cxa:0x1234,r2],r2
        cp r3,[cxa:0x1234,r2],r2

        cp.na.x r3,[cxa:r1,r2],0x5555
        cp.na r3,[cxa:r1,r2],0x5555
        cp r3,[cxa:r1,r2],0x5555

        ; CM to NULL
        cp.na r3,[cm:r1],r2
        cp r3,[cm:r1],r2

        cp.na r3,[cm:0x1234],r2
        cp r3,[cm:0x1234],r2

        cp.na r3,[cm:r1],0x5555
        cp r3,[cm:r1],0x5555

        ; cp CM to SD
        cp.m.na.x r3,[csd:r1,0x20,0x30],[cm:r1],0x10
        cp.m.na r3,[csd:r1,0x20,0x30],[cm:r1],0x10
        cp.m r3,[csd:r1,0x20,0x30],[cm:r1],0x10
        cp r3,[csd:r1,0x20,0x30],[cm:r1],0x10

        cp.m.na.x r3,[csd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp.m.na r3,[csd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp.m r3,[csd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp r3,[csd:r1,0x20,0x30,r2],[cm:r1],0x10

        cp.m.na.x r3,[csd:r1,0x20,r2],[cm:r1],0x10
        cp.m.na r3,[csd:r1,0x20,r2],[cm:r1],0x10
        cp.m r3,[csd:r1,0x20,r2],[cm:r1],0x10
        cp r3,[csd:r1,0x20,r2],[cm:r1],0x10

        cp.m.na.x r3,[csd:r1,0x20,r2,r2],[cm:r1],0x40
        cp.m.na r3,[csd:r1,0x20,r2,r2],[cm:r1],0x40
        cp.m r3,[csd:r1,0x20,r2,r2],[cm:r1],0x40
        cp r3,[csd:r1,0x20,r2,r2],[cm:r1],0x40

        cp.m.na.x r3,[csd:r1,0x20,0x80],[cm:r1],r2
        cp.m.na r3,[csd:r1,0x20,0x80],[cm:r1],r2
        cp.m r3,[csd:r1,0x20,0x80],[cm:r1],r2
        cp r3,[csd:r1,0x20,0x80],[cm:r1],r2

        cp.m.na.x r3,[csd:r1,0x20,0x80,r2],[cm:r1],r2
        cp.m.na r3,[csd:r1,0x20,0x80,r2],[cm:r1],r2
        cp.m r3,[csd:r1,0x20,0x80,r2],[cm:r1],r2
        cp r3,[csd:r1,0x20,0x80,r2],[cm:r1],r2

        cp.m.na.x r3,[csd:r1,r2,0x40],[cm:r1],0x60
        cp.m.na r3,[csd:r1,r2,0x40],[cm:r1],0x60
        cp.m r3,[csd:r1,r2,0x40],[cm:r1],0x60
        cp r3,[csd:r1,r2,0x40],[cm:r1],0x60

        cp.m.na.x r3,[csd:r1,r2,0x40,r2],[cm:r1],0x60
        cp.m.na r3,[csd:r1,r2,0x40,r2],[cm:r1],0x60
        cp.m r3,[csd:r1,r2,0x40,r2],[cm:r1],0x60
        cp r3,[csd:r1,r2,0x40,r2],[cm:r1],0x60

        cp.m.na.x r3,[csd:r1,r2,r2],[cm:r1],0x60
        cp.m.na r3,[csd:r1,r2,r2],[cm:r1],0x60
        cp.m r3,[csd:r1,r2,r2],[cm:r1],0x60
        cp r3,[csd:r1,r2,r2],[cm:r1],0x60

        cp.m.na.x r3,[csd:r1,r2,r2,r2],[cm:r1],0x60
        cp.m.na r3,[csd:r1,r2,r2,r2],[cm:r1],0x60
        cp.m r3,[csd:r1,r2,r2,r2],[cm:r1],0x60
        cp r3,[csd:r1,r2,r2,r2],[cm:r1],0x60

        cp.m.na.x r3,[csd:r1,0x20,r2],[cm:r1],r2
        cp.m.na r3,[csd:r1,0x20,r2],[cm:r1],r2
        cp.m r3,[csd:r1,0x20,r2],[cm:r1],r2
        cp r3,[csd:r1,0x20,r2],[cm:r1],r2

        cp.m.na.x r3,[csd:r1,0x20,r2,r2],[cm:r1],r2
        cp.m.na r3,[csd:r1,0x20,r2,r2],[cm:r1],r2
        cp.m r3,[csd:r1,0x20,r2,r2],[cm:r1],r2
        cp r3,[csd:r1,0x20,r2,r2],[cm:r1],r2

        cp.m.na.x r3,[csd:r1,r2,0x40],[cm:r1],r2
        cp.m.na r3,[csd:r1,r2,0x40],[cm:r1],r2
        cp.m r3,[csd:r1,r2,0x40],[cm:r1],r2
        cp r3,[csd:r1,r2,0x40],[cm:r1],r2

        cp.m.na.x r3,[csd:r1,r2,0x40,r2],[cm:r1],r2
        cp.m.na r3,[csd:r1,r2,0x40,r2],[cm:r1],r2
        cp.m r3,[csd:r1,r2,0x40,r2],[cm:r1],r2
        cp r3,[csd:r1,r2,0x40,r2],[cm:r1],r2

        cp.m.na.x r3,[csd:r1,r2,r2],[cm:r1],r2
        cp.m.na r3,[csd:r1,r2,r2],[cm:r1],r2
        cp.m r3,[csd:r1,r2,r2],[cm:r1],r2
        cp r3,[csd:r1,r2,r2],[cm:r1],r2

        cp.m.na.x r3,[csd:r1,r2,r2,r2],[cm:r1],r2
        cp.m.na r3,[csd:r1,r2,r2,r2],[cm:r1],r2
        cp.m r3,[csd:r1,r2,r2,r2],[cm:r1],r2
        cp r3,[csd:r1,r2,r2,r2],[cm:r1],r2

        ; cp CM to XD
        cp.m.na.x r3,[cxd:r1,0x20,0x30],[cm:r1],0x10
        cp.m.na r3,[cxd:r1,0x20,0x30],[cm:r1],0x10
        cp.m r3,[cxd:r1,0x20,0x30],[cm:r1],0x10
        cp r3,[cxd:r1,0x20,0x30],[cm:r1],0x10

        cp.m.na.x r3,[cxd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp.m.na r3,[cxd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp.m r3,[cxd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp r3,[cxd:r1,0x20,0x30,r2],[cm:r1],0x10

        cp.m.na.x r3,[cxd:r1,0x20,r2],[cm:r1],0x10
        cp.m.na r3,[cxd:r1,0x20,r2],[cm:r1],0x10
        cp.m r3,[cxd:r1,0x20,r2],[cm:r1],0x10
        cp r3,[cxd:r1,0x20,r2],[cm:r1],0x10

        cp.m.na.x r3,[cxd:r1,0x20,r2,r2],[cm:r1],0x40
        cp.m.na r3,[cxd:r1,0x20,r2,r2],[cm:r1],0x40
        cp.m r3,[cxd:r1,0x20,r2,r2],[cm:r1],0x40
        cp r3,[cxd:r1,0x20,r2,r2],[cm:r1],0x40

        cp.m.na.x r3,[cxd:r1,0x20,0x80],[cm:r1],r2
        cp.m.na r3,[cxd:r1,0x20,0x80],[cm:r1],r2
        cp.m r3,[cxd:r1,0x20,0x80],[cm:r1],r2
        cp r3,[cxd:r1,0x20,0x80],[cm:r1],r2

        cp.m.na.x r3,[cxd:r1,0x20,0x80,r2],[cm:r1],r2
        cp.m.na r3,[cxd:r1,0x20,0x80,r2],[cm:r1],r2
        cp.m r3,[cxd:r1,0x20,0x80,r2],[cm:r1],r2
        cp r3,[cxd:r1,0x20,0x80,r2],[cm:r1],r2

        cp.m.na.x r3,[cxd:r1,r2,0x40],[cm:r1],0x60
        cp.m.na r3,[cxd:r1,r2,0x40],[cm:r1],0x60
        cp.m r3,[cxd:r1,r2,0x40],[cm:r1],0x60
        cp r3,[cxd:r1,r2,0x40],[cm:r1],0x60

        cp.m.na.x r3,[cxd:r1,r2,0x40,r2],[cm:r1],0x60
        cp.m.na r3,[cxd:r1,r2,0x40,r2],[cm:r1],0x60
        cp.m r3,[cxd:r1,r2,0x40,r2],[cm:r1],0x60
        cp r3,[cxd:r1,r2,0x40,r2],[cm:r1],0x60

        cp.m.na.x r3,[cxd:r1,r2,r2],[cm:r1],0x60
        cp.m.na r3,[cxd:r1,r2,r2],[cm:r1],0x60
        cp.m r3,[cxd:r1,r2,r2],[cm:r1],0x60
        cp r3,[cxd:r1,r2,r2],[cm:r1],0x60

        cp.m.na.x r3,[cxd:r1,r2,r2,r2],[cm:r1],0x60
        cp.m.na r3,[cxd:r1,r2,r2,r2],[cm:r1],0x60
        cp.m r3,[cxd:r1,r2,r2,r2],[cm:r1],0x60
        cp r3,[cxd:r1,r2,r2,r2],[cm:r1],0x60

        cp.m.na.x r3,[cxd:r1,0x20,r2],[cm:r1],r2
        cp.m.na r3,[cxd:r1,0x20,r2],[cm:r1],r2
        cp.m r3,[cxd:r1,0x20,r2],[cm:r1],r2
        cp r3,[cxd:r1,0x20,r2],[cm:r1],r2

        cp.m.na.x r3,[cxd:r1,0x20,r2,r2],[cm:r1],r2
        cp.m.na r3,[cxd:r1,0x20,r2,r2],[cm:r1],r2
        cp.m r3,[cxd:r1,0x20,r2,r2],[cm:r1],r2
        cp r3,[cxd:r1,0x20,r2,r2],[cm:r1],r2

        cp.m.na.x r3,[cxd:r1,r2,0x40],[cm:r1],r2
        cp.m.na r3,[cxd:r1,r2,0x40],[cm:r1],r2
        cp.m r3,[cxd:r1,r2,0x40],[cm:r1],r2
        cp r3,[cxd:r1,r2,0x40],[cm:r1],r2

        cp.m.na.x r3,[cxd:r1,r2,0x40,r2],[cm:r1],r2
        cp.m.na r3,[cxd:r1,r2,0x40,r2],[cm:r1],r2
        cp.m r3,[cxd:r1,r2,0x40,r2],[cm:r1],r2
        cp r3,[cxd:r1,r2,0x40,r2],[cm:r1],r2

        cp.m.na.x r3,[cxd:r1,r2,r2],[cm:r1],r2
        cp.m.na r3,[cxd:r1,r2,r2],[cm:r1],r2
        cp.m r3,[cxd:r1,r2,r2],[cm:r1],r2
        cp r3,[cxd:r1,r2,r2],[cm:r1],r2

        cp.m.na.x r3,[cxd:r1,r2,r2,r2],[cm:r1],r2
        cp.m.na r3,[cxd:r1,r2,r2,r2],[cm:r1],r2
        cp.m r3,[cxd:r1,r2,r2,r2],[cm:r1],r2
        cp r3,[cxd:r1,r2,r2,r2],[cm:r1],r2

        ; cp CM to SD
        cp.na.x r3,[cm:r1],[csd:r1,0x40,0x60],0x10
        cp.na r3,[cm:r1],[csd:r1,0x40,0x60],0x10
        cp r3,[cm:r1],[csd:r1,0x40,0x60],0x10

        cp.na.x r3,[cm:r1],[csd:r1,0x40,0x60,r2],0x10
        cp.na r3,[cm:r1],[csd:r1,0x40,0x60,r2],0x10
        cp r3,[cm:r1],[csd:r1,0x40,0x60,r2],0x10

        cp.na.x r3,[cm:r1],[csd:r1,0x40,r2],0x10
        cp.na r3,[cm:r1],[csd:r1,0x40,r2],0x10
        cp r3,[cm:r1],[csd:r1,0x40,r2],0x10

        cp.na.x r3,[cm:r1],[csd:r1,0x40,r2,r2],0x10
        cp.na r3,[cm:r1],[csd:r1,0x40,r2,r2],0x10
        cp r3,[cm:r1],[csd:r1,0x40,r2,r2],0x10

        cp.na.x r3,[cm:r1],[csd:r1,0x40,0x60],r2
        cp.na r3,[cm:r1],[csd:r1,0x40,0x60],r2
        cp r3,[cm:r1],[csd:r1,0x40,0x60],r2

        cp.na.x r3,[cm:r1],[csd:r1,0x40,0x60,r2],r2
        cp.na r3,[cm:r1],[csd:r1,0x40,0x60,r2],r2
        cp r3,[cm:r1],[csd:r1,0x40,0x60,r2],r2

        cp.na.x r3,[cm:r1],[csd:r1,r2,0x60],0x20
        cp.na r3,[cm:r1],[csd:r1,r2,0x60],0x20
        cp r3,[cm:r1],[csd:r1,r2,0x60],0x20

        cp.na.x r3,[cm:r1],[csd:r1,r2,0x60,r2],0x20
        cp.na r3,[cm:r1],[csd:r1,r2,0x60,r2],0x20
        cp r3,[cm:r1],[csd:r1,r2,0x60,r2],0x20

        cp.na.x r3,[cm:r1],[csd:r1,r2,r2],0x20
        cp.na r3,[cm:r1],[csd:r1,r2,r2],0x20
        cp r3,[cm:r1],[csd:r1,r2,r2],0x20

        cp.na.x r3,[cm:r1],[csd:r1,r2,r2,r2],0x20
        cp.na r3,[cm:r1],[csd:r1,r2,r2,r2],0x20
        cp r3,[cm:r1],[csd:r1,r2,r2,r2],0x20

        cp.na.x r3,[cm:r1],[csd:r1,0x20,r2],r2
        cp.na r3,[cm:r1],[csd:r1,0x20,r2],r2
        cp r3,[cm:r1],[csd:r1,0x20,r2],r2

        cp.na.x r3,[cm:r1],[csd:r1,0x20,r2,r2],r2
        cp.na r3,[cm:r1],[csd:r1,0x20,r2,r2],r2
        cp r3,[cm:r1],[csd:r1,0x20,r2,r2],r2

        cp.na.x r3,[cm:r1],[csd:r1,r2,0x40],r2
        cp.na r3,[cm:r1],[csd:r1,r2,0x40],r2
        cp r3,[cm:r1],[csd:r1,r2,0x40],r2

        cp.na.x r3,[cm:r1],[csd:r1,r2,0x40,r2],r2
        cp.na r3,[cm:r1],[csd:r1,r2,0x40,r2],r2
        cp r3,[cm:r1],[csd:r1,r2,0x40,r2],r2

        cp.na.x r3,[cm:r1],[csd:r1,r2,r2],r2
        cp.na r3,[cm:r1],[csd:r1,r2,r2],r2
        cp r3,[cm:r1],[csd:r1,r2,r2],r2

        cp.na.x r3,[cm:r1],[csd:r1,r2,r2,r2],r2
        cp.na r3,[cm:r1],[csd:r1,r2,r2,r2],r2
        cp r3,[cm:r1],[csd:r1,r2,r2,r2],r2

        ; cp CM to XD
        cp.na.x r3,[cm:r1],[cxd:r1,0x40,0x60],0x10
        cp.na r3,[cm:r1],[cxd:r1,0x40,0x60],0x10
        cp r3,[cm:r1],[cxd:r1,0x40,0x60],0x10

        cp.na.x r3,[cm:r1],[cxd:r1,0x40,0x60,r2],0x10
        cp.na r3,[cm:r1],[cxd:r1,0x40,0x60,r2],0x10
        cp r3,[cm:r1],[cxd:r1,0x40,0x60,r2],0x10

        cp.na.x r3,[cm:r1],[cxd:r1,0x40,r2],0x10
        cp.na r3,[cm:r1],[cxd:r1,0x40,r2],0x10
        cp r3,[cm:r1],[cxd:r1,0x40,r2],0x10

        cp.na.x r3,[cm:r1],[cxd:r1,0x40,r2,r2],0x10
        cp.na r3,[cm:r1],[cxd:r1,0x40,r2,r2],0x10
        cp r3,[cm:r1],[cxd:r1,0x40,r2,r2],0x10

        cp.na.x r3,[cm:r1],[cxd:r1,0x40,0x60],r2
        cp.na r3,[cm:r1],[cxd:r1,0x40,0x60],r2
        cp r3,[cm:r1],[cxd:r1,0x40,0x60],r2

        cp.na.x r3,[cm:r1],[cxd:r1,0x40,0x60,r2],r2
        cp.na r3,[cm:r1],[cxd:r1,0x40,0x60,r2],r2
        cp r3,[cm:r1],[cxd:r1,0x40,0x60,r2],r2

        cp.na.x r3,[cm:r1],[cxd:r1,r2,0x60],0x20
        cp.na r3,[cm:r1],[cxd:r1,r2,0x60],0x20
        cp r3,[cm:r1],[cxd:r1,r2,0x60],0x20

        cp.na.x r3,[cm:r1],[cxd:r1,r2,0x60,r2],0x20
        cp.na r3,[cm:r1],[cxd:r1,r2,0x60,r2],0x20
        cp r3,[cm:r1],[cxd:r1,r2,0x60,r2],0x20

        cp.na.x r3,[cm:r1],[cxd:r1,r2,r2],0x20
        cp.na r3,[cm:r1],[cxd:r1,r2,r2],0x20
        cp r3,[cm:r1],[cxd:r1,r2,r2],0x20

        cp.na.x r3,[cm:r1],[cxd:r1,r2,r2,r2],0x20
        cp.na r3,[cm:r1],[cxd:r1,r2,r2,r2],0x20
        cp r3,[cm:r1],[cxd:r1,r2,r2,r2],0x20

        cp.na.x r3,[cm:r1],[cxd:r1,0x20,r2],r2
        cp.na r3,[cm:r1],[cxd:r1,0x20,r2],r2
        cp r3,[cm:r1],[cxd:r1,0x20,r2],r2

        cp.na.x r3,[cm:r1],[cxd:r1,0x20,r2,r2],r2
        cp.na r3,[cm:r1],[cxd:r1,0x20,r2,r2],r2
        cp r3,[cm:r1],[cxd:r1,0x20,r2,r2],r2

        cp.na.x r3,[cm:r1],[cxd:r1,r2,0x40],r2
        cp.na r3,[cm:r1],[cxd:r1,r2,0x40],r2
        cp r3,[cm:r1],[cxd:r1,r2,0x40],r2

        cp.na.x r3,[cm:r1],[cxd:r1,r2,0x40,r2],r2
        cp.na r3,[cm:r1],[cxd:r1,r2,0x40,r2],r2
        cp r3,[cm:r1],[cxd:r1,r2,0x40,r2],r2

        cp.na.x r3,[cm:r1],[cxd:r1,r2,r2],r2
        cp.na r3,[cm:r1],[cxd:r1,r2,r2],r2
        cp r3,[cm:r1],[cxd:r1,r2,r2],r2

        cp.na.x r3,[cm:r1],[cxd:r1,r2,r2,r2],r2
        cp.na r3,[cm:r1],[cxd:r1,r2,r2,r2],r2
        cp r3,[cm:r1],[cxd:r1,r2,r2,r2],r2

        ; cp BD to BD no dst register
        cp.e.m  0,[cbd:r1,r2],[cbd:r1,r2],r2
        cp.m    0,[cbd:r1,r2],[cbd:r1,r2],r2
        cp.e    0,[cbd:r1,r2],[cbd:r1,r2],r2
        cp     	0,[cbd:r1,r2],[cbd:r1,r2],r2

        cp.e.m  0,[cbd:r1,r2,r2],[cbd:r1,r2,r2],r2
        cp.m    0,[cbd:r1,r2,r2],[cbd:r1,r2,r2],r2
        cp.e    0,[cbd:r1,r2,r2],[cbd:r1,r2,r2],r2
        cp      0,[cbd:r1,r2,r2],[cbd:r1,r2,r2],r2

        cp.e.m  0,[cbd:r1,r2],[cbd:0x12,r2],r2
        cp.m    0,[cbd:r1,r2],[cbd:0x12,r2],r2
        cp.e    0,[cbd:r1,r2],[cbd:0x12,r2],r2
        cp      0,[cbd:r1,r2],[cbd:0x12,r2],r2

        cp.e.m  0,[cbd:0x23,r2],[cbd:r1,r2],r2
        cp.m    0,[cbd:0x23,r2],[cbd:r1,r2],r2
        cp.e    0,[cbd:0x23,r2],[cbd:r1,r2],r2
        cp      0,[cbd:0x23,r2],[cbd:r1,r2],r2

        cp.m    0,[cbd:r1,r2],[cbd:0x33,r2], 0x28
        cp      0,[cbd:r1,r2],[cbd:0x33,r2], 0x28

        cp.m    0,[cbd:0x42,r2],[cbd:r1,r2], 0x48
        cp      0,[cbd:0x42,r2],[cbd:r1,r2], 0x48

        cp.m    0,[cbd:r1,0x44],[cbd:0x1234,r2],r2
        cp      0,[cbd:r1,0x44],[cbd:0x1234,r2],r2

        cp.m    0,[cbd:0x1234,0x54],[cbd:r1,r2],r2
        cp      0,[cbd:0x1234,0x54],[cbd:r1,r2],r2

        cp.m    0,[cbd:r1,0x254],[cbd:r1,r2],r2
        cp      0,[cbd:r1,0x254],[cbd:r1,r2],r2

        cp.m    0,[cbd:r1,0x254,r2],[cbd:r1,r2,r2],r2
        cp      0,[cbd:r1,0x254,r2],[cbd:r1,r2,r2],r2

        cp.m    0,[cbd:r1,0x254],[cbd:r1,r2],0x222
        cp      0,[cbd:r1,0x254],[cbd:r1,r2],0x222

        cp.m    0,[cbd:r1,0x254, r2],[cbd:r1,r2, r2],0x111
        cp      0,[cbd:r1,0x254, r2],[cbd:r1,r2, r2],0x111

        cp.m.e  0,[cbd:r1,0x254],[cbd:r1,0x254],r2
        cp.e    0,[cbd:r1,0x254],[cbd:r1,0x254],r2

        cp.m.e  0,[cbd:r1,0x254,r2],[cbd:r1,0x254,r2],r2
        cp.e    0,[cbd:r1,0x254,r2],[cbd:r1,0x254,r2],r2

        cp.m.e  0,[cbd:r1,0x254],[cbd:r1,0x254],0x123
        cp.e    0,[cbd:r1,0x254],[cbd:r1,0x254],0x123

        cp.m.e  0,[cbd:r1,0x254,r2],[cbd:r1,0x254, r2],0x123
        cp.e    0,[cbd:r1,0x254,r2],[cbd:r1,0x254, r2],0x123

        cp.e.m  0,[cbd:r1,r2],[cbd:r1,r2],0x123
        cp.m    0,[cbd:r1,r2],[cbd:r1,r2],0x123
        cp.e    0,[cbd:r1,r2],[cbd:r1,r2],0x123
        cp      0,[cbd:r1,r2],[cbd:r1,r2],0x123

        cp.e.m  0,[cbd:r1,r2,r2],[cbd:r1,r2,r2],0x123
        cp.m    0,[cbd:r1,r2,r2],[cbd:r1,r2,r2],0x123
        cp.e    0,[cbd:r1,r2,r2],[cbd:r1,r2,r2],0x123
        cp      0,[cbd:r1,r2,r2],[cbd:r1,r2,r2],0x123

        ; cp BD to XA no dst register
        cp.m 0,[cxa:r1],[cbd:r1,r2],r2
        cp   0,[cxa:r1],[cbd:r1,r2],r2

        cp.m 0,[cxa:r1,r2],[cbd:r1,r2,r2],r2
        cp   0,[cxa:r1,r2],[cbd:r1,r2,r2],r2

        cp.m 0,[cxa:r1],[cbd:0x2222,r2],r2
        cp   0,[cxa:r1],[cbd:0x2222,r2],r2

        cp.m 0,[cxa:0x1234],[cbd:r1,r2],r2
        cp   0,[cxa:0x1234],[cbd:r1,r2],r2

        cp.m 0,[cxa:r1],[cbd:0x2222,r2],0x46
        cp   0,[cxa:r1],[cbd:0x2222,r2],0x46

        cp.m 0,[cxa:0x1234],[cbd:r1,r2],0x46
        cp   0,[cxa:0x1234],[cbd:r1,r2],0x46

        cp.m 0,[cxa:r1],[cbd:r1,r2],0xaa
        cp   0,[cxa:r1],[cbd:r1,r2],0xaa

        cp.m 0,[cxa:r1,r2],[cbd:r1,r2,r2],0x44
        cp   0,[cxa:r1,r2],[cbd:r1,r2, r2],0x44

        ; cp XA to BD no dst register
        cp.m 0,[cbd:r1,r2],[cxa:r1],r2
        cp   0,[cbd:r1,r2],[cxa:r1],r2

        cp.m 0,[cbd:r1,r2,r2],[cxa:r1,r2],r2
        cp   0,[cbd:r1,r2,r2],[cxa:r1,r2],r2

        cp.m 0,[cbd:r1,r2],[cxa:0x1234],r2
        cp   0,[cbd:r1,r2],[cxa:0x1234],r2

        cp.m 0,[cbd:0x4321,r2],[cxa:r1],r2
        cp 0,[cbd:0x4321,r2],[cxa:r1],r2

        cp.m 0,[cbd:r1,r2],[cxa:0x1234],0x66
        cp   0,[cbd:r1,r2],[cxa:0x1234],0x66

        cp.m 0,[cbd:0x4321,r2],[cxa:r1],0xbb
        cp 0,[cbd:0x4321,r2],[cxa:r1],0xbb

        cp.m 0,[cbd:r1,0x88],[cxa:0x1234],r2
        cp   0,[cbd:r1,0x88],[cxa:0x1234],r2

        cp.m 0,[cbd:0x1234,0x55],[cxa:r1],r2
        cp   0,[cbd:0x1234,0x55],[cxa:r1],r2

        cp.m 0,[cbd:r1,0x222],[cxa:r1],r2
        cp   0,[cbd:r1,0x222],[cxa:r1],r2

        cp.m 0,[cbd:r1,0x222,r2],[cxa:r1,r2],r2
        cp   0,[cbd:r1,0x222,r2],[cxa:r1,r2],r2

        cp.m 0,[cbd:r1,0x222],[cxa:r1],0x111
        cp   0,[cbd:r1,0x222],[cxa:r1],0x111

        cp.m 0,[cbd:r1,0x333,r2],[cxa:r1,r2],0x111
        cp 0,[cbd:r1,0x333,r2],[cxa:r1,r2],0x111

        cp.m 0,[cbd:r1,r2],[cxa:r1],0x123
        cp 0,[cbd:r1,r2],[cxa:r1],0x123

        cp.m 0,[cbd:r1,r2,r2],[cxa:r1,r2],0x123
        cp 0,[cbd:r1,r2,r2],[cxa:r1,r2],0x123

        ; cp BD to CM no dst register
        cp 0, [cm:r1],[cbd:r1,r2],r2
        cp 0, [cm:r1],[cbd:r1,r2,r2],r2
        cp 0, [cm:r1],[cbd:0x1234,r2],r2
        cp 0, [cm:0x1234], [cbd:r1,r2],r2
        cp 0, [cm:r1],[cbd:0x1234,r2],0x44
        cp 0, [cm:0x1234], [cbd:r1,r2],0x44
        cp 0, [cm:r1],[cbd:0x1234,0x88],r2
        cp 0, [cm:0x8787], [cbd:r1,0x88],r2
        cp 0, [cm:r1],[cbd:r1,0x222],r2
        cp 0, [cm:r1],[cbd:r1,0x222,r2],r2
        cp 0, [cm:r1],[cbd:r1,0x222],0x234
        cp 0, [cm:r1],[cbd:r1,0x222, r2],0x234
        cp 0, [cm:r1],[cbd:r1,r2],0x234
        cp 0, [cm:r1],[cbd:r1,r2,r2],0x234

        ; cp CM to BD no dst register
        cp.m 0,[cbd:r1,r2],[cm:r1],r2
        cp   0,[cbd:r1,r2],[cm:r1],r2

        cp.m 0,[cbd:r1,r2,r2],[cm:r1],r2
        cp   0,[cbd:r1,r2,r2],[cm:r1],r2

        cp.m 0,[cbd:r1,r2],[cm:0x4545],r2
        cp 0,[cbd:r1,r2],[cm:0x4545],r2

        cp.m 0,[cbd:0x4545,r2],[cm:r1],r2
        cp   0,[cbd:0x4545,r2],[cm:r1],r2

        cp.m 0,[cbd:r1,r2],[cm:0x4545],0x81
        cp   0,[cbd:r1,r2],[cm:0x4545],0x81

        cp.m 0,[cbd:0x4545,r2],[cm:r1],0x88
        cp   0,[cbd:0x4545,r2],[cm:r1],0x88

        cp.m 0,[cbd:r1,0x84],[cm:0x1234],r2
        cp   0,[cbd:r1,0x84],[cm:0x1234],r2

        cp.m 0,[cbd:0x1234,0x84],[cm:r1],r2
        cp   0,[cbd:0x1234,0x84],[cm:r1],r2

        cp.m 0,[cbd:r1,0x222],[cm:r1],r2
        cp   0,[cbd:r1,0x222],[cm:r1],r2

        cp.m 0,[cbd:r1,0x222,r2],[cm:r1],r2
        cp   0,[cbd:r1,0x222,r2],[cm:r1],r2

        cp.m 0,[cbd:r1,0x111],[cm:r1],0x222
        cp   0,[cbd:r1,0x111],[cm:r1],0x222

        cp.m 0,[cbd:r1,0x222,r2],[cm:r1],0x222
        cp   0,[cbd:r1,0x222,r2],[cm:r1],0x222

        cp.m 0,[cbd:r1,r2],[cm:r1],0x222
        cp   0,[cbd:r1,r2],[cm:r1],0x222

        cp.m 0,[cbd:r1,r2,r2],[cm:r1],0x222
        cp   0,[cbd:r1,r2,r2],[cm:r1],0x222

        ; cp CJID to CM no dst register
        cp 0,[cm:r1],[cjid:r1],r2
        cp 0,[cm:r1],[cjid:0x1234], r2
        cp 0,[cm:0x1234],[cjid:r1],r2
        cp 0,[cm:r1],[cjid:0x1234],0x44
        cp 0,[cm:0x1234],[cjid:r1],0x44
        cp 0,[cm:r1],[cjid:r1],0x222

        ; cp CM to CJID no dst register
        cp 0,[cjid:r1],[cm:r1],r2
        cp 0,[cjid: r1],[cm:0x1234],r2
        cp 0,[cjid:0x1234],[cm: r1],r2
        cp 0,[cjid:r1],[cm:0x1234 ],0x44
        cp 0,[cjid:0x1234],[cm:r1],0x44
        cp 0,[cjid:r1],[cm:r1],0x222

        ; cp BD to NULL no dst register
        cp 0,[cbd:r1,r2],r2
        cp 0,[cbd:r1,r2,r2],r2
        cp 0,[cbd:0x1234,r2],r2
        cp 0,[cbd:0x1234,r2],0x44
        cp 0,[cbd:r1,r2],0x222
        cp 0,[cbd:r1,r2,r2],0x222

        ;cp LBD to LBD no dst register
        cp.e 0, [clbd:r1,r2],[clbd:r1,r2],r2
        cp   0, [clbd:r1,r2],[clbd:r1,r2],r2

        cp.e 0, [clbd:r1,r2,r2],[clbd:r1,r2,r2],r2
        cp   0, [clbd:r1,r2,r2],[clbd:r1,r2,r2],r2

        cp.e 0, [clbd:r1,r2],[clbd:0x1234,r2],r2
        cp   0, [clbd:r1,r2],[clbd:0x1234,r2],r2

        cp.e 0, [clbd:0x1234,r2],[clbd:r1,r2],r2
        cp   0, [clbd:0x1234,r2],[clbd:r1,r2],r2

        cp   0, [clbd:r1,r2],[clbd:0x4321,r2],0x22
        cp   0, [clbd:0x1234,r2],[clbd:r1,r2],0x44
        cp   0, [clbd:r1,0x45],[clbd:0x1234,r2],r2
        cp   0, [clbd:0x1234,0x55],[clbd:r1,r2],r2
        cp   0, [clbd:r1,0x222],[clbd:r1,r2],r2
        cp   0, [clbd:r1,0x222,r2],[clbd:r1,r2,r2],r2
        cp   0, [clbd:r1,0x222],[clbd:r1,r2],0x111
        cp   0, [clbd:r1,0x222,r2],[clbd:r1,r2,r2],0x111

        cp.e  0, [clbd:r1,0x222],[clbd:r1,0x222],r2
        cp.e  0, [clbd:r1,0x222,r2],[clbd:r1,0x222,r2],r2
        cp.e  0, [clbd:r1,0x222],[clbd:r1,0x222],0x111
        cp.e  0, [clbd:r1,0x222,r2],[clbd:r1,0x222,r2],0x111
        cp.e  0, [clbd:r1,r2],[clbd:r1,r2],0x111
        cp    0, [clbd:r1,r2],[clbd:r1,r2],0x111
        cp.e  0, [clbd:r1,r2,r2],[clbd:r1,r2,r2],0x111
        cp    0, [clbd:r1,r2,r2],[clbd:r1,r2,r2],0x111

        ;cp LBD to XA no dst register
        cp 0,[cxa:r1],[clbd:r1,r2],r2
        cp 0,[cxa:r1,r2],[clbd:r1,r2,r2],r2
        cp 0,[cxa:r1],[clbd:0x1234,r2],r2
        cp 0,[cxa:0x1234],[clbd:r1,r2],r2
        cp 0,[cxa:r1],[clbd:0x1234,r2],r2
        cp 0,[cxa:0x1234],[clbd:r1,r2],0x44
        cp 0,[cxa:r1],[clbd:r1,r2],0x222
        cp 0,[cxa:r1,r2],[clbd:r1,r2,r2],0x222

        ;cp XA to LBD no dst register
        cp 0,[clbd:r1,r2],[cxa:r1],r2
        cp 0,[clbd:r1,r2,r2],[cxa:r1,r2],r2
        cp 0,[clbd:r1,r2],[cxa:0x1234],r2
        cp 0,[clbd:0x1234,r2],[cxa:r1],r2
        cp 0,[clbd:r1,r2],[cxa:0x1234],0x88
        cp 0,[clbd:0x1234,r2],[cxa:r1],0x88
        cp 0,[clbd:r1,0x44],[cxa:0x1234],r2
        cp 0,[clbd:0x1234,0x44],[cxa:r1],r2
        cp 0,[clbd:r1,0x222],[cxa:r1],r2
        cp 0,[clbd:r1,0x222,r2],[cxa:r1,r2],r2
        cp 0,[clbd:r1,0x222],[cxa:r1],0x333
        cp 0,[clbd:r1,0x222,r2],[cxa:r1,r2],0x333
        cp 0,[clbd:r1,r2],[cxa:r1],0x333
        cp 0,[clbd:r1,r2,r2],[cxa:r1,r2],0x333

        ;cp LBD to CM no dst register
        cp 0,[cm:r1],[clbd:r1,r2],r2
        cp 0,[cm:r1],[clbd:r1,r2,r2],r2
        cp 0,[cm:r1],[clbd:0x1234,r2],r2
        cp 0,[cm:0x1234],[clbd:r1,r2],r2
        cp 0,[cm:r1],[clbd:0x1234,r2],0x44
        cp 0,[cm:0x4321],[clbd:r1,r2],0x44
        cp 0,[cm:r1],[clbd:0x1234,0x55],r2
        cp 0,[cm:0x1234],[clbd:r1,0x55],r2
        cp 0,[cm:r1],[clbd:r1,0x222],r2
        cp 0,[cm:r1],[clbd:r1,0x222,r2],r2
        cp 0,[cm:r1],[clbd:r1,0x222],0x111
        cp 0,[cm:r1],[clbd:r1,0x222,r2],0x111
        cp 0,[cm:r1],[clbd:r1,r2],0x111
        cp 0,[cm:r1],[clbd:r1,r2,r2],0x111

        ;cp CM to LBD no dst register
        cp 0, [clbd:r1,r2],[cm:r1],r2
        cp 0, [clbd:r1,r2,r2],[cm:r1],r2
        cp 0, [clbd:r1,r2],[cm:0x1234],r2
        cp 0, [clbd:0x1234,r2],[cm:r2],r2
        cp 0, [clbd:r1,r2],[cm:0x1234],0x44
        cp 0, [clbd:0x1234,r2],[cm:r1],0x44
        cp 0, [clbd:r1,0x44],[cm:0x1234],r2
        cp 0, [clbd:0x1234,0x44],[cm:r1],r2
        cp 0, [clbd:r1,0x222],[cm:r1],r2
        cp 0, [clbd:r1,0x222,r2],[cm:r1],r2
        cp 0, [clbd:r1,0x222],[cm:r1],0x111
        cp 0, [clbd:r1,0x222,r2],[cm:r1],0x111
        cp 0, [clbd:r1,r2],[cm:r1],0x111
        cp 0, [clbd:r1,r2,r2],[cm:r1],0x111

        ; cp XA to XA no dst register
        cp.m.na.x 0,[cxa:r1],[cxa:r1],r2
        cp.m.na 0,[cxa:r1],[cxa:r1],r2
        cp.m 0,[cxa:r1],[cxa:r1],r2
        cp 0,[cxa:r1],[cxa:r1],r2

        cp.m.na.x 0,[cxa:r1],[cxa:0x1234],r2
        cp.m.na 0,[cxa:r1],[cxa:0x1234],r2
        cp.m 0,[cxa:r1],[cxa:0x1234],r2
        cp 0,[cxa:r1],[cxa:0x1234],r2

        cp.m.na.x 0,[cxa:0x1234],[cxa:r1],r2
        cp.m.na 0,[cxa:0x1234],[cxa:r1],r2
        cp.m 0,[cxa:0x1234],[cxa:r1],r2
        cp 0,[cxa:0x1234],[cxa:r1],r2

        cp.m.na.x 0,[cxa:r1],[cxa:r1],0x4321
        cp.m.na 0,[cxa:r1],[cxa:r1],0x4321
        cp.m 0,[cxa:r1],[cxa:r1],0x4321
        cp 0,[cxa:r1],[cxa:r1],0x4321

        cp.m.na.x 0,[cxa:r1,r2],[cxa:r1,r2],r2
        cp.m.na 0,[cxa:r1,r2],[cxa:r1,r2],r2
        cp.m 0,[cxa:r1,r2],[cxa:r1,r2],r2
        cp 0,[cxa:r1,r2],[cxa:r1,r2],r2

        cp.m.na.x 0,[cxa:r1,r2],[cxa:0x1234,r2],r2
        cp.m.na 0,[cxa:r1,r2],[cxa:0x1234,r2],r2
        cp.m 0,[cxa:r1,r2],[cxa:0x1234,r2],r2
        cp 0,[cxa:r1,r2],[cxa:0x1234,r2],r2

        cp.m.na.x 0,[cxa:0x1234,r2],[cxa:r1,r2],r2
        cp.m.na 0,[cxa:0x1234,r2],[cxa:r1,r2],r2
        cp.m 0,[cxa:0x1234,r2],[cxa:r1,r2],r2
        cp 0,[cxa:0x1234,r2],[cxa:r1,r2],r2

        cp.m.na.x 0,[cxa:r1,r2],[cxa:r1,r2],0x1234
        cp.m.na 0,[cxa:r1,r2],[cxa:r1,r2],0x1234
        cp.m 0,[cxa:r1,r2],[cxa:r1,r2],0x1234
        cp 0,[cxa:r1,r2],[cxa:r1,r2],0x1234

        ; cp XA to CM no dst register
        cp.na.x 0,[cm:r1],[cxa:r1],r2
        cp.na 0,[cm:r1],[cxa:r1],r2
        cp 0,[cm:r1],[cxa:r1],r2

        cp.na.x 0,[cm:r1],[cxa:0x1234],r2
        cp.na 0,[cm:r1],[cxa:0x1234],r2
        cp 0,[cm:r1],[cxa:0x1234],r2

        cp.na.x 0,[cm:0x1234],[cxa:r1],r2
        cp.na 0,[cm:0x1234],[cxa:r1],r2
        cp 0,[cm:0x1234],[cxa:r1],r2

        cp.na.x 0,[cm:r1],[cxa:r1],0x4321
        cp.na 0,[cm:r1],[cxa:r1],0x4321
        cp 0,[cm:r1],[cxa:r1],0x4321

        cp.na.x 0,[cm:r1],[cxa:r1,r2],r2
        cp.na 0,[cm:r1],[cxa:r1,r2],r2
        cp 0,[cm:r1],[cxa:r1,r2],r2

        cp.na.x 0,[cm:r1],[cxa:0x1234,r2],r2
        cp.na 0,[cm:r1],[cxa:0x1234,r2],r2
        cp 0,[cm:r1],[cxa:0x1234,r2],r2

        cp.na.x 0,[cm:0x1234],[cxa:r1,r2],r2
        cp.na 0,[cm:0x1234],[cxa:r1,r2],r2
        cp 0,[cm:0x1234],[cxa:r1,r2],r2

        cp.na.x 0,[cm:r1],[cxa:r1,r2],0x1234
        cp.na 0,[cm:r1],[cxa:r1,r2],0x1234
        cp 0,[cm:r1],[cxa:r1,r2],0x1234

        ;cp CM to CX no dst register
        cp.m.na.x 0,[cxa:r1],[cm:r1],r2
        cp.m.na 0,[cxa:r1],[cm:r1],r2
        cp.m 0,[cxa:r1],[cm:r1],r2
        cp 0,[cxa:r1],[cm:r1],r2

        cp.m.na.x 0,[cxa:r1],[cm:0x1234],r2
        cp.m.na 0,[cxa:r1],[cm:0x1234],r2
        cp.m 0,[cxa:r1],[cm:0x1234],r2
        cp 0,[cxa:r1],[cm:0x1234],r2

        cp.m.na.x 0,[cxa:0x1234],[cm:r1],r2
        cp.m.na 0,[cxa:0x1234],[cm:r1],r2
        cp.m 0,[cxa:0x1234],[cm:r1],r2
        cp 0,[cxa:0x1234],[cm:r1],r2

        cp.m.na.x 0,[cxa:r1],[cm:r1],0x8888
        cp.m.na 0,[cxa:r1],[cm:r1],0x8888
        cp.m 0,[cxa:r1],[cm:r1],0x8888
        cp 0,[cxa:r1],[cm:r1],0x8888

        cp.m.na.x 0,[cxa:r1,r2],[cm:r1],r2
        cp.m.na 0,[cxa:r1,r2],[cm:r1],r2
        cp.m 0,[cxa:r1,r2],[cm:r1],r2
        cp 0,[cxa:r1,r2],[cm:r1],r2

        cp.m.na.x 0,[cxa:r1,r2],[cm:0x1234],r2
        cp.m.na 0,[cxa:r1,r2],[cm:0x1234],r2
        cp.m 0,[cxa:r1,r2],[cm:0x1234],r2
        cp 0,[cxa:r1,r2],[cm:0x1234],r2

        cp.m.na.x 0,[cxa:0x1234,r2],[cm:r1],r2
        cp.m.na 0,[cxa:0x1234,r2],[cm:r1],r2
        cp.m 0,[cxa:0x1234,r2],[cm:r1],r2
        cp 0,[cxa:0x1234,r2],[cm:r1],r2

        cp.m.na.x 0,[cxa:r1,r2],[cm:r1],0x8888
        cp.m.na 0,[cxa:r1,r2],[cm:r1],0x8888
        cp.m 0,[cxa:r1,r2],[cm:r1],0x8888
        cp 0,[cxa:r1,r2],[cm:r1],0x8888

        ; CM to NULL no dst register
        cp.na 0,[cm:r1],r2
        cp 0,[cm:r1],r2

        cp.na 0,[cm:0x1234],r2
        cp 0,[cm:0x1234],r2

        cp.na 0,[cm:r1],0x5555
        cp 0,[cm:r1],0x5555

        ; cp CM to SD no dst register
        cp.m.na.x 0,[csd:r1,0x20,0x30],[cm:r1],0x10
        cp.m.na 0,[csd:r1,0x20,0x30],[cm:r1],0x10
        cp.m 0,[csd:r1,0x20,0x30],[cm:r1],0x10
        cp 0,[csd:r1,0x20,0x30],[cm:r1],0x10

        cp.m.na.x 0,[csd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp.m.na 0,[csd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp.m 0,[csd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp 0,[csd:r1,0x20,0x30,r2],[cm:r1],0x10

        cp.m.na.x 0,[csd:r1,0x20,r2],[cm:r1],0x10
        cp.m.na 0,[csd:r1,0x20,r2],[cm:r1],0x10
        cp.m 0,[csd:r1,0x20,r2],[cm:r1],0x10
        cp 0,[csd:r1,0x20,r2],[cm:r1],0x10

        cp.m.na.x 0,[csd:r1,0x20,r2,r2],[cm:r1],0x40
        cp.m.na 0,[csd:r1,0x20,r2,r2],[cm:r1],0x40
        cp.m 0,[csd:r1,0x20,r2,r2],[cm:r1],0x40
        cp 0,[csd:r1,0x20,r2,r2],[cm:r1],0x40

        cp.m.na.x 0,[csd:r1,0x20,0x80],[cm:r1],r2
        cp.m.na 0,[csd:r1,0x20,0x80],[cm:r1],r2
        cp.m 0,[csd:r1,0x20,0x80],[cm:r1],r2
        cp 0,[csd:r1,0x20,0x80],[cm:r1],r2

        cp.m.na.x 0,[csd:r1,0x20,0x80,r2],[cm:r1],r2
        cp.m.na 0,[csd:r1,0x20,0x80,r2],[cm:r1],r2
        cp.m 0,[csd:r1,0x20,0x80,r2],[cm:r1],r2
        cp 0,[csd:r1,0x20,0x80,r2],[cm:r1],r2

        cp.m.na.x 0,[csd:r1,r2,0x40],[cm:r1],0x60
        cp.m.na 0,[csd:r1,r2,0x40],[cm:r1],0x60
        cp.m 0,[csd:r1,r2,0x40],[cm:r1],0x60
        cp 0,[csd:r1,r2,0x40],[cm:r1],0x60

        cp.m.na.x 0,[csd:r1,r2,0x40,r2],[cm:r1],0x60
        cp.m.na 0,[csd:r1,r2,0x40,r2],[cm:r1],0x60
        cp.m 0,[csd:r1,r2,0x40,r2],[cm:r1],0x60
        cp 0,[csd:r1,r2,0x40,r2],[cm:r1],0x60

        cp.m.na.x 0,[csd:r1,r2,r2],[cm:r1],0x60
        cp.m.na 0,[csd:r1,r2,r2],[cm:r1],0x60
        cp.m 0,[csd:r1,r2,r2],[cm:r1],0x60
        cp 0,[csd:r1,r2,r2],[cm:r1],0x60

        cp.m.na.x 0,[csd:r1,r2,r2,r2],[cm:r1],0x60
        cp.m.na 0,[csd:r1,r2,r2,r2],[cm:r1],0x60
        cp.m 0,[csd:r1,r2,r2,r2],[cm:r1],0x60
        cp 0,[csd:r1,r2,r2,r2],[cm:r1],0x60

        cp.m.na.x 0,[csd:r1,0x20,r2],[cm:r1],r2
        cp.m.na 0,[csd:r1,0x20,r2],[cm:r1],r2
        cp.m 0,[csd:r1,0x20,r2],[cm:r1],r2
        cp 0,[csd:r1,0x20,r2],[cm:r1],r2

        cp.m.na.x 0,[csd:r1,0x20,r2,r2],[cm:r1],r2
        cp.m.na 0,[csd:r1,0x20,r2,r2],[cm:r1],r2
        cp.m 0,[csd:r1,0x20,r2,r2],[cm:r1],r2
        cp 0,[csd:r1,0x20,r2,r2],[cm:r1],r2

        cp.m.na.x 0,[csd:r1,r2,0x40],[cm:r1],r2
        cp.m.na 0,[csd:r1,r2,0x40],[cm:r1],r2
        cp.m 0,[csd:r1,r2,0x40],[cm:r1],r2
        cp 0,[csd:r1,r2,0x40],[cm:r1],r2

        cp.m.na.x 0,[csd:r1,r2,0x40,r2],[cm:r1],r2
        cp.m.na 0,[csd:r1,r2,0x40,r2],[cm:r1],r2
        cp.m 0,[csd:r1,r2,0x40,r2],[cm:r1],r2
        cp 0,[csd:r1,r2,0x40,r2],[cm:r1],r2

        cp.m.na.x 0,[csd:r1,r2,r2],[cm:r1],r2
        cp.m.na 0,[csd:r1,r2,r2],[cm:r1],r2
        cp.m 0,[csd:r1,r2,r2],[cm:r1],r2
        cp 0,[csd:r1,r2,r2],[cm:r1],r2

        cp.m.na.x 0,[csd:r1,r2,r2,r2],[cm:r1],r2
        cp.m.na 0,[csd:r1,r2,r2,r2],[cm:r1],r2
        cp.m 0,[csd:r1,r2,r2,r2],[cm:r1],r2
        cp 0,[csd:r1,r2,r2,r2],[cm:r1],r2

        ; cp CM to XD no dst register
        cp.m.na.x 0,[cxd:r1,0x20,0x30],[cm:r1],0x10
        cp.m.na 0,[cxd:r1,0x20,0x30],[cm:r1],0x10
        cp.m 0,[cxd:r1,0x20,0x30],[cm:r1],0x10
        cp 0,[cxd:r1,0x20,0x30],[cm:r1],0x10

        cp.m.na.x 0,[cxd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp.m.na 0,[cxd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp.m 0,[cxd:r1,0x20,0x30,r2],[cm:r1],0x10
        cp 0,[cxd:r1,0x20,0x30,r2],[cm:r1],0x10

        cp.m.na.x 0,[cxd:r1,0x20,r2],[cm:r1],0x10
        cp.m.na 0,[cxd:r1,0x20,r2],[cm:r1],0x10
        cp.m 0,[cxd:r1,0x20,r2],[cm:r1],0x10
        cp 0,[cxd:r1,0x20,r2],[cm:r1],0x10

        cp.m.na.x 0,[cxd:r1,0x20,r2,r2],[cm:r1],0x40
        cp.m.na 0,[cxd:r1,0x20,r2,r2],[cm:r1],0x40
        cp.m 0,[cxd:r1,0x20,r2,r2],[cm:r1],0x40
        cp 0,[cxd:r1,0x20,r2,r2],[cm:r1],0x40

        cp.m.na.x 0,[cxd:r1,0x20,0x80],[cm:r1],r2
        cp.m.na 0,[cxd:r1,0x20,0x80],[cm:r1],r2
        cp.m 0,[cxd:r1,0x20,0x80],[cm:r1],r2
        cp 0,[cxd:r1,0x20,0x80],[cm:r1],r2

        cp.m.na.x 0,[cxd:r1,0x20,0x80,r2],[cm:r1],r2
        cp.m.na 0,[cxd:r1,0x20,0x80,r2],[cm:r1],r2
        cp.m 0,[cxd:r1,0x20,0x80,r2],[cm:r1],r2
        cp 0,[cxd:r1,0x20,0x80,r2],[cm:r1],r2

        cp.m.na.x 0,[cxd:r1,r2,0x40],[cm:r1],0x60
        cp.m.na 0,[cxd:r1,r2,0x40],[cm:r1],0x60
        cp.m 0,[cxd:r1,r2,0x40],[cm:r1],0x60
        cp 0,[cxd:r1,r2,0x40],[cm:r1],0x60

        cp.m.na.x 0,[cxd:r1,r2,0x40,r2],[cm:r1],0x60
        cp.m.na 0,[cxd:r1,r2,0x40,r2],[cm:r1],0x60
        cp.m 0,[cxd:r1,r2,0x40,r2],[cm:r1],0x60
        cp 0,[cxd:r1,r2,0x40,r2],[cm:r1],0x60

        cp.m.na.x 0,[cxd:r1,r2,r2],[cm:r1],0x60
        cp.m.na 0,[cxd:r1,r2,r2],[cm:r1],0x60
        cp.m 0,[cxd:r1,r2,r2],[cm:r1],0x60
        cp 0,[cxd:r1,r2,r2],[cm:r1],0x60

        cp.m.na.x 0,[cxd:r1,r2,r2,r2],[cm:r1],0x60
        cp.m.na 0,[cxd:r1,r2,r2,r2],[cm:r1],0x60
        cp.m 0,[cxd:r1,r2,r2,r2],[cm:r1],0x60
        cp 0,[cxd:r1,r2,r2,r2],[cm:r1],0x60

        cp.m.na.x 0,[cxd:r1,0x20,r2],[cm:r1],r2
        cp.m.na 0,[cxd:r1,0x20,r2],[cm:r1],r2
        cp.m 0,[cxd:r1,0x20,r2],[cm:r1],r2
        cp 0,[cxd:r1,0x20,r2],[cm:r1],r2

        cp.m.na.x 0,[cxd:r1,0x20,r2,r2],[cm:r1],r2
        cp.m.na 0,[cxd:r1,0x20,r2,r2],[cm:r1],r2
        cp.m 0,[cxd:r1,0x20,r2,r2],[cm:r1],r2
        cp 0,[cxd:r1,0x20,r2,r2],[cm:r1],r2

        cp.m.na.x 0,[cxd:r1,r2,0x40],[cm:r1],r2
        cp.m.na 0,[cxd:r1,r2,0x40],[cm:r1],r2
        cp.m 0,[cxd:r1,r2,0x40],[cm:r1],r2
        cp 0,[cxd:r1,r2,0x40],[cm:r1],r2

        cp.m.na.x 0,[cxd:r1,r2,0x40,r2],[cm:r1],r2
        cp.m.na 0,[cxd:r1,r2,0x40,r2],[cm:r1],r2
        cp.m 0,[cxd:r1,r2,0x40,r2],[cm:r1],r2
        cp 0,[cxd:r1,r2,0x40,r2],[cm:r1],r2

        cp.m.na.x 0,[cxd:r1,r2,r2],[cm:r1],r2
        cp.m.na 0,[cxd:r1,r2,r2],[cm:r1],r2
        cp.m 0,[cxd:r1,r2,r2],[cm:r1],r2
        cp 0,[cxd:r1,r2,r2],[cm:r1],r2

        cp.m.na.x 0,[cxd:r1,r2,r2,r2],[cm:r1],r2
        cp.m.na 0,[cxd:r1,r2,r2,r2],[cm:r1],r2
        cp.m 0,[cxd:r1,r2,r2,r2],[cm:r1],r2
        cp 0,[cxd:r1,r2,r2,r2],[cm:r1],r2

        ; cp CM to SD no dst register
        cp.na.x 0,[cm:r1],[csd:r1,0x40,0x60],0x10
        cp.na 0,[cm:r1],[csd:r1,0x40,0x60],0x10
        cp 0,[cm:r1],[csd:r1,0x40,0x60],0x10

        cp.na.x 0,[cm:r1],[csd:r1,0x40,0x60,r2],0x10
        cp.na 0,[cm:r1],[csd:r1,0x40,0x60,r2],0x10
        cp 0,[cm:r1],[csd:r1,0x40,0x60,r2],0x10

        cp.na.x 0,[cm:r1],[csd:r1,0x40,r2],0x10
        cp.na 0,[cm:r1],[csd:r1,0x40,r2],0x10
        cp 0,[cm:r1],[csd:r1,0x40,r2],0x10

        cp.na.x 0,[cm:r1],[csd:r1,0x40,r2,r2],0x10
        cp.na 0,[cm:r1],[csd:r1,0x40,r2,r2],0x10
        cp 0,[cm:r1],[csd:r1,0x40,r2,r2],0x10

        cp.na.x 0,[cm:r1],[csd:r1,0x40,0x60],r2
        cp.na 0,[cm:r1],[csd:r1,0x40,0x60],r2
        cp 0,[cm:r1],[csd:r1,0x40,0x60],r2

        cp.na.x 0,[cm:r1],[csd:r1,0x40,0x60,r2],r2
        cp.na 0,[cm:r1],[csd:r1,0x40,0x60,r2],r2
        cp 0,[cm:r1],[csd:r1,0x40,0x60,r2],r2

        cp.na.x 0,[cm:r1],[csd:r1,r2,0x60],0x20
        cp.na 0,[cm:r1],[csd:r1,r2,0x60],0x20
        cp 0,[cm:r1],[csd:r1,r2,0x60],0x20

        cp.na.x 0,[cm:r1],[csd:r1,r2,0x60,r2],0x20
        cp.na 0,[cm:r1],[csd:r1,r2,0x60,r2],0x20
        cp 0,[cm:r1],[csd:r1,r2,0x60,r2],0x20

        cp.na.x 0,[cm:r1],[csd:r1,r2,r2],0x20
        cp.na 0,[cm:r1],[csd:r1,r2,r2],0x20
        cp 0,[cm:r1],[csd:r1,r2,r2],0x20

        cp.na.x 0,[cm:r1],[csd:r1,r2,r2,r2],0x20
        cp.na 0,[cm:r1],[csd:r1,r2,r2,r2],0x20
        cp 0,[cm:r1],[csd:r1,r2,r2,r2],0x20

        cp.na.x 0,[cm:r1],[csd:r1,0x20,r2],r2
        cp.na 0,[cm:r1],[csd:r1,0x20,r2],r2
        cp 0,[cm:r1],[csd:r1,0x20,r2],r2

        cp.na.x 0,[cm:r1],[csd:r1,0x20,r2,r2],r2
        cp.na 0,[cm:r1],[csd:r1,0x20,r2,r2],r2
        cp 0,[cm:r1],[csd:r1,0x20,r2,r2],r2

        cp.na.x 0,[cm:r1],[csd:r1,r2,0x40],r2
        cp.na 0,[cm:r1],[csd:r1,r2,0x40],r2
        cp 0,[cm:r1],[csd:r1,r2,0x40],r2

        cp.na.x 0,[cm:r1],[csd:r1,r2,0x40,r2],r2
        cp.na 0,[cm:r1],[csd:r1,r2,0x40,r2],r2
        cp 0,[cm:r1],[csd:r1,r2,0x40,r2],r2

        cp.na.x 0,[cm:r1],[csd:r1,r2,r2],r2
        cp.na 0,[cm:r1],[csd:r1,r2,r2],r2
        cp 0,[cm:r1],[csd:r1,r2,r2],r2

        cp.na.x 0,[cm:r1],[csd:r1,r2,r2,r2],r2
        cp.na 0,[cm:r1],[csd:r1,r2,r2,r2],r2
        cp 0,[cm:r1],[csd:r1,r2,r2,r2],r2

        ; cp CM to XD no dst register
        cp.na.x 0,[cm:r1],[cxd:r1,0x40,0x60],0x10
        cp.na 0,[cm:r1],[cxd:r1,0x40,0x60],0x10
        cp 0,[cm:r1],[cxd:r1,0x40,0x60],0x10

        cp.na.x 0,[cm:r1],[cxd:r1,0x40,0x60,r2],0x10
        cp.na 0,[cm:r1],[cxd:r1,0x40,0x60,r2],0x10
        cp 0,[cm:r1],[cxd:r1,0x40,0x60,r2],0x10

        cp.na.x 0,[cm:r1],[cxd:r1,0x40,r2],0x10
        cp.na 0,[cm:r1],[cxd:r1,0x40,r2],0x10
        cp 0,[cm:r1],[cxd:r1,0x40,r2],0x10

        cp.na.x 0,[cm:r1],[cxd:r1,0x40,r2,r2],0x10
        cp.na 0,[cm:r1],[cxd:r1,0x40,r2,r2],0x10
        cp 0,[cm:r1],[cxd:r1,0x40,r2,r2],0x10

        cp.na.x 0,[cm:r1],[cxd:r1,0x40,0x60],r2
        cp.na 0,[cm:r1],[cxd:r1,0x40,0x60],r2
        cp 0,[cm:r1],[cxd:r1,0x40,0x60],r2

        cp.na.x 0,[cm:r1],[cxd:r1,0x40,0x60,r2],r2
        cp.na 0,[cm:r1],[cxd:r1,0x40,0x60,r2],r2
        cp 0,[cm:r1],[cxd:r1,0x40,0x60,r2],r2

        cp.na.x 0,[cm:r1],[cxd:r1,r2,0x60],0x20
        cp.na 0,[cm:r1],[cxd:r1,r2,0x60],0x20
        cp 0,[cm:r1],[cxd:r1,r2,0x60],0x20

        cp.na.x 0,[cm:r1],[cxd:r1,r2,0x60,r2],0x20
        cp.na 0,[cm:r1],[cxd:r1,r2,0x60,r2],0x20
        cp 0,[cm:r1],[cxd:r1,r2,0x60,r2],0x20

        cp.na.x 0,[cm:r1],[cxd:r1,r2,r2],0x20
        cp.na 0,[cm:r1],[cxd:r1,r2,r2],0x20
        cp 0,[cm:r1],[cxd:r1,r2,r2],0x20

        cp.na.x 0,[cm:r1],[cxd:r1,r2,r2,r2],0x20
        cp.na 0,[cm:r1],[cxd:r1,r2,r2,r2],0x20
        cp 0,[cm:r1],[cxd:r1,r2,r2,r2],0x20

        cp.na.x 0,[cm:r1],[cxd:r1,0x20,r2],r2
        cp.na 0,[cm:r1],[cxd:r1,0x20,r2],r2
        cp 0,[cm:r1],[cxd:r1,0x20,r2],r2

        cp.na.x 0,[cm:r1],[cxd:r1,0x20,r2,r2],r2
        cp.na 0,[cm:r1],[cxd:r1,0x20,r2,r2],r2
        cp 0,[cm:r1],[cxd:r1,0x20,r2,r2],r2

        cp.na.x 0,[cm:r1],[cxd:r1,r2,0x40],r2
        cp.na 0,[cm:r1],[cxd:r1,r2,0x40],r2
        cp 0,[cm:r1],[cxd:r1,r2,0x40],r2

        cp.na.x 0,[cm:r1],[cxd:r1,r2,0x40,r2],r2
        cp.na 0,[cm:r1],[cxd:r1,r2,0x40,r2],r2
        cp 0,[cm:r1],[cxd:r1,r2,0x40,r2],r2

        cp.na.x 0,[cm:r1],[cxd:r1,r2,r2],r2
        cp.na 0,[cm:r1],[cxd:r1,r2,r2],r2
        cp 0,[cm:r1],[cxd:r1,r2,r2],r2

        cp.na.x 0,[cm:r1],[cxd:r1,r2,r2,r2],r2
        cp.na 0,[cm:r1],[cxd:r1,r2,r2,r2],r2
        cp 0,[cm:r1],[cxd:r1,r2,r2,r2],r2

        ; test cmem offset
        cp r3,[cbd:r1,r2],[cbd:0x57f00000,r2],r2
        cp.e.m  r3,[cbd:0x57f00002,r2],[cbd:r1,r2],r2
        cp.m    r3,[cbd:r1,r2],[cbd:0x57f00002,r2], 0x28
        cp r3, [cm:r1],[cbd:0x57f00002,r2],r2
        cp.m r3,[cbd:0x57f00008,r2],[cm:r1],r2
        cp.m r3,[cbd:0x57f00002,0x84],[cm:r1],r2
        cp r3,[cm:0x57f00002],[cjid:r1],r2
        cp r3,[cjid:0x57f0001c],[cm: r1],r2
        cp r3,[cbd:0x57f00002,r2],r2
        cp   r3, [clbd:0x57f00008,r2],[clbd:r1,r2],r2
        cp r3,[cxa:0x57f00002],[clbd:r1,r2],r2
        cp r3,[clbd:0x57f00002,r2],[cxa:r1],r2
        cp r3,[cm:0x57f00002],[clbd:r1,r2],r2
        cp r3, [clbd:0x57f00002,r2],[cm:r2],r2
        cp r3,[cxa:r1],[cxa:0x57f00002],r2
        cp r3,[cxa:r1,r2],[cxa:0x57f00002,r2],r2
        cp r3,[cm:r1],[cxa:0x57f000e2],r2
        cp r3,[cm:0x57f00002],[cxa:r1,r2],r2
        cp r3,[cxa:r1,r2],[cm:0x57f00002],r2
        cp r3,[cxa:0x57f00052,r2],r2
        cp r3,[cm:0x57f00042],r2
