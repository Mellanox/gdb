        .text

        ; Atomic operation instructions
        xex.di     r0,r0,[xa:r1]
        xex.di     r0,r0,[sd:r1]
        xex.di     r0,r0,[xd:r1]
        xex.di     r0,r0,[r1]

        ;exc{b,w}
        excb.di    r0,r0,[xa:r1]
        excb.di.f  r0,r0,[xa:r1]
        excb.di    r0,r0,[r1]
        excb.di.f  r0,r0,[r1]

        excw.di    r0,r0,[xa:r1]
        excw.di.f  r0,r0,[xa:r1]
        excw.di    r0,r0,[r1]
        excw.di.f  r0,r0,[r1]

        exc.di     r0,r0,[xa:r1]
        exc.di.f   r0,r0,[xa:r1]
        exc.di     r0,r0,[sd:r1]
        exc.di.f   r0,r0,[sd:r1]
        exc.di     r0,r0,[xd:r1]
        exc.di.f   r0,r0,[xd:r1]
        exc.di     r0,r0,[r1]
        exc.di.f   r0,r0,[r1]

        ;aabb{b,w,l}
        aaddb.di   r0,r0,[xa:r1]
        aaddb.di.f r0,r0,[xa:r1]
        aaddb.di   r0,r0,[r1]
        aaddb.di.f r0,r0,[r1]

        aaddb.di   r0,[xa:r1]
        aaddb.di   r0,[r1]

        aaddw.di   r0,r0,[xa:r1]
        aaddw.di.f r0,r0,[xa:r1]
        aaddw.di   r0,r0,[r1]
        aaddw.di.f r0,r0,[r1]
        aaddw.di   r0,r0,[mbd:r1]

        aaddw.di   r0,[xa:r1]
        aaddw.di   r0,[r1]
        aaddw.di   r0,[mbd:r1]

        aadd.di     r0,r0,[xa:r1]
        aadd.di.f   r0,r0,[xa:r1]
        aadd.di     r0,r0,[sd:r1]
        aadd.di.f   r0,r0,[sd:r1]
        aadd.di     r0,r0,[xd:r1]
        aadd.di.f   r0,r0,[xd:r1]
        aadd.di     r0,r0,[r1]
        aadd.di.f   r0,r0,[r1]

        aadd.di     r0,[xa:r1]
        aadd.di     r0,[sd:r1]
        aadd.di     r0,[xd:r1]
        aadd.di     r0,[r1]

        aaddl.di     r0,[xa:r1]
        aaddl.di     r0,[sd:r1]
        aaddl.di     r0,[xd:r1]
        aaddl.di     r0,[r1]

        ; adadd{l}
        adadd.di     r0,r0,[xa:r1]
        adadd.di.f   r0,r0,[xa:r1]
        adadd.di     r0,r0,[sd:r1]
        adadd.di.f   r0,r0,[sd:r1]
        adadd.di     r0,r0,[xd:r1]
        adadd.di.f   r0,r0,[xd:r1]
        adadd.di     r0,r0,[r1]
        adadd.di.f   r0,r0,[r1]

        adaddl.di     r0,r0,[xa:r1]
        adaddl.di.f   r0,r0,[xa:r1]
        adaddl.di     r0,r0,[sd:r1]
        adaddl.di.f   r0,r0,[sd:r1]
        adaddl.di     r0,r0,[xd:r1]
        adaddl.di.f   r0,r0,[xd:r1]
        adaddl.di     r0,r0,[r1]
        adaddl.di.f   r0,r0,[r1]

        adaddl.di r0, [xa:r1]
        adaddl.di r0, [xd:r1]
        adaddl.di r0, [r1]

        adadd.di     r0,[xa:r1]
        adadd.di     r0,[sd:r1]
        adadd.di     r0,[xd:r1]
        adadd.di     r0,[r1]

        ; aadnd{b,w}
        aand.di     r0,r0,[xa:r1]
        aand.di.f   r0,r0,[xa:r1]
        aand.di     r0,r0,[sd:r1]
        aand.di.f   r0,r0,[sd:r1]
        aand.di     r0,r0,[xd:r1]
        aand.di.f   r0,r0,[xd:r1]
        aand.di     r0,r0,[r1]
        aand.di.f   r0,r0,[r1]

        aandb.di     r0,r0,[xa:r1]
        aandb.di.f   r0,r0,[xa:r1]
        aandb.di     r0,r0,[r1]
        aandb.di.f   r0,r0,[r1]

        aandw.di     r0,r0,[xa:r1]
        aandw.di.f   r0,r0,[xa:r1]
        aandw.di     r0,r0,[r1]
        aandw.di.f   r0,r0,[r1]

        aandb.di     r0,[xa:r1]
        aandb.di     r0,[r1]

        aandw.di     r0,[xa:r1]
        aandw.di     r0,[r1]

        aand.di     r0,[xa:r1]
        aand.di     r0,[sd:r1]
        aand.di     r0,[xd:r1]
        aand.di     r0,[r1]

        ;aor{bw}
        aor.di     r0,r0,[xa:r1]
        aor.di.f   r0,r0,[xa:r1]
        aor.di     r0,r0,[sd:r1]
        aor.di.f   r0,r0,[sd:r1]
        aor.di     r0,r0,[xd:r1]
        aor.di.f   r0,r0,[xd:r1]
        aor.di     r0,r0,[r1]
        aor.di.f   r0,r0,[r1]

        aorb.di     r0,r0,[xa:r1]
        aorb.di.f   r0,r0,[xa:r1]
        aorb.di     r0,r0,[r1]
        aorb.di.f   r0,r0,[r1]

        aorw.di     r0,r0,[xa:r1]
        aorw.di.f   r0,r0,[xa:r1]
        aorw.di     r0,r0,[r1]
        aorw.di.f   r0,r0,[r1]

        aorb.di     r0,[xa:r1]
        aorb.di     r0,[r1]

        aorw.di     r0,[xa:r1]
        aorw.di     r0,[r1]

        aor.di     r0,[xa:r1]
        aor.di     r0,[sd:r1]
        aor.di     r0,[xd:r1]
        aor.di     r0,[r1]

        ;axor{bw}
        axor.di     r0,r0,[xa:r1]
        axor.di.f   r0,r0,[xa:r1]
        axor.di     r0,r0,[sd:r1]
        axor.di.f   r0,r0,[sd:r1]
        axor.di     r0,r0,[xd:r1]
        axor.di.f   r0,r0,[xd:r1]
        axor.di     r0,r0,[r1]
        axor.di.f   r0,r0,[r1]

        axorb.di     r0,r0,[xa:r1]
        axorb.di.f   r0,r0,[xa:r1]
        axorb.di     r0,r0,[r1]
        axorb.di.f   r0,r0,[r1]

        axorw.di     r0,r0,[xa:r1]
        axorw.di.f   r0,r0,[xa:r1]
        axorw.di     r0,r0,[r1]
        axorw.di.f   r0,r0,[r1]

        axorb.di     r0,[xa:r1]
        axorb.di     r0,[r1]

        axorw.di     r0,[xa:r1]
        axorw.di     r0,[r1]

        axor.di     r0,[xa:r1]
        axor.di     r0,[sd:r1]
        axor.di     r0,[xd:r1]
        axor.di     r0,[r1]

        ; atst{b,w}
        atstb.di    r0,[xa:r1]
        atstb.di.f  r0,[xa:r1]
        atstb.di    r0,[r1]
        atstb.di.f  r0,[r1]

        atstw.di    r0,[xa:r1]
        atstw.di.f  r0,[xa:r1]
        atstw.di    r0,[r1]
        atstw.di.f  r0,[r1]

        atst.di     r0,[xa:r1]
        atst.di.f   r0,[xa:r1]
        atst.di     r0,[sd:r1]
        atst.di.f   r0,[sd:r1]
        atst.di     r0,[xd:r1]
        atst.di.f   r0,[xd:r1]
        atst.di     r0,[r1]
        atst.di.f   r0,[r1]

        ; ari{b,w}
        arib.di     r0,[xa:r1]
        arib.di.f   r0,[xa:r1]
        arib.di     r0,[r1]
        arib.di.f   r0,[r1]

        ariw.di     r0,[xa:r1]
        ariw.di.f   r0,[xa:r1]
        ariw.di     r0,[r1]
        ariw.di.f   r0,[r1]
        ariw.di     r0, [mbd:r1]

        ari.di      r0,[xa:r1]
        ari.di.f    r0,[xa:r1]
        ari.di      r0,[sd:r1]
        ari.di.f    r0,[sd:r1]
        ari.di      r0,[xd:r1]
        ari.di.f    r0,[xd:r1]
        ari.di      r0,[r1]
        ari.di.f    r0,[r1]

        ; ardc{b,w}
        ardcb.di    r0,[xa:r1]
        ardcb.di.f  r0,[xa:r1]
        ardcb.di    r0,[r1]
        ardcb.di.f  r0,[r1]

        ardcw.di    r0,[xa:r1]
        ardcw.di.f  r0,[xa:r1]
        ardcw.di    r0,[r1]
        ardcw.di.f  r0,[r1]
        ardcw.di.f r0, [mbd:r1]

        ardc.di     r0,[xa:r1]
        ardc.di.f   r0,[xa:r1]
        ardc.di     r0,[sd:r1]
        ardc.di.f   r0,[sd:r1]
        ardc.di     r0,[xd:r1]
        ardc.di.f   r0,[xd:r1]
        ardc.di     r0,[r1]
        ardc.di.f   r0,[r1]

        ; aric
        aric.di     r0,[xa:r1]
        aric.di.f   r0,[xa:r1]
        aric.di     r0,[sd:r1]
        aric.di.f   r0,[sd:r1]
        aric.di     r0,[xd:r1]
        aric.di.f   r0,[xd:r1]
        aric.di     r0,[r1]
        aric.di.f   r0,[r1]

        ; arcl{b,w,l}
        arclb.di    r0,[xa:r1]
        arclb.di.f  r0,[xa:r1]
        arclb.di    r0,[r1]
        arclb.di.f  r0,[r1]

        arclw.di    r0,[xa:r1]
        arclw.di.f  r0,[xa:r1]
        arclw.di    r0,[r1]
        arclw.di.f  r0,[r1]

        arcl.di     r0,[xa:r1]
        arcl.di.f   r0,[xa:r1]
        arcl.di     r0,[sd:r1]
        arcl.di.f   r0,[sd:r1]
        arcl.di     r0,[xd:r1]
        arcl.di.f   r0,[xd:r1]
        arcl.di     r0,[r1]
        arcl.di.f   r0,[r1]

        ;xld{b,w}
        xld.di      r0,[xa:r1]
        xld.di      r0,[sd:r1]
        xld.di      r0,[xd:r1]
        xldb.di     r0,[xa:r1]
        xldw.di     r0,[xa:r1]
        xldw.di     r0,[mbd:r1]

        ;xst{b,w}
        xst.di      r0,[xa:r1]
        xst.di      r0,[sd:r1]
        xst.di      r0,[xd:r1]
        xstb.di     r0,[xa:r1]
        xstw.di     r0,[xa:r1]
        xstw.di     r0,[mbd:r1]
        xstl.di     r0,[xa:r1]
        xstl.di     r0,[mbd:r1]
label:
        ; aric.r
        aric.r.di     r0,[xa:r1]
        aric.r.di.f   r0,[xa:r1]
        aric.r.di     r0,[sd:r1]
        aric.r.di.f   r0,[sd:r1]
        aric.r.di     r0,[xd:r1]
        aric.r.di.f   r0,[xd:r1]
        aric.r.di     r0,[r1]
        aric.r.di.f   r0,[r1]

        ; bat
        bat label

        ; GPA1 format
        xex.di     [cm:GPA1],r0,[sd:r1]
        xex.di     [cm:GPA1],r0,[xd:r1]
        xex.di     [cm:GPA1],r0,[r1]

        aaddl.di   [cm:GPA1],r0,[sd:r1]
        aaddl.di   [cm:GPA1],r0,[xd:r1]
        aaddl.di   [cm:GPA1],r0,[r1]

        aaddl.di   r0,[cm:GPA1],r0,[sd:r1]
        aaddl.di   r0,[cm:GPA1],r0,[xd:r1]
        aaddl.di   r0,[cm:GPA1],r0,[r1]

        aaddl.di.f   r0,[cm:GPA1],r0,[sd:r1]
        aaddl.di.f   r0,[cm:GPA1],r0,[xd:r1]
        aaddl.di.f   r0,[cm:GPA1],r0,[r1]

        aadd.di     [cm:GPA1],r0,[sd:r1]
        aadd.di     [cm:GPA1],r0,[xd:r1]
        aadd.di     [cm:GPA1],r0,[r1]

        adadd.di    r0,[cm:GPA1],r0,[sd:r1]
        adadd.di    r0,[cm:GPA1],r0,[xd:r1]
        adadd.di    r0,[cm:GPA1],r0,[r1]

        adadd.di.f  r0,[cm:GPA1],r0,[sd:r1]
        adadd.di.f  r0,[cm:GPA1],r0,[xd:r1]
        adadd.di.f  r0,[cm:GPA1],r0,[r1]

        adadd.di    [cm:GPA1],r0,[sd:r1]
        adadd.di    [cm:GPA1],r0,[xd:r1]
        adadd.di    [cm:GPA1],r0,[r1]

        adaddl.di   r0,[cm:GPA1],r0,[sd:r1]
        adaddl.di   r0,[cm:GPA1],r0,[xd:r1]
        adaddl.di   r0,[cm:GPA1],r0,[r1]

        adaddl.di.f   r0,[cm:GPA1],r0,[sd:r1]
        adaddl.di.f   r0,[cm:GPA1],r0,[xd:r1]
        adaddl.di.f   r0,[cm:GPA1],r0,[r1]

        adaddl.di   [cm:GPA1],r0,[sd:r1]
        adaddl.di   [cm:GPA1],r0,[xd:r1]
        adaddl.di   [cm:GPA1],r0,[r1]

        aand.di   [cm:GPA1],r0,[sd:r1]
        aand.di   [cm:GPA1],r0,[xd:r1]
        aand.di   [cm:GPA1],r0,[r1]

        aor.di   [cm:GPA1],r0,[sd:r1]
        aor.di   [cm:GPA1],r0,[xd:r1]
        aor.di   [cm:GPA1],r0,[r1]

        axor.di   [cm:GPA1],r0,[sd:r1]
        axor.di   [cm:GPA1],r0,[xd:r1]
        axor.di   [cm:GPA1],r0,[r1]

        atst.di   [cm:GPA1],[sd:r1]
        atst.di   [cm:GPA1],[xd:r1]
        atst.di   [cm:GPA1],[r1]

        aril.di   r0,[cm:GPA1],[sd:r1]
        aril.di   r0,[cm:GPA1],[xd:r1]
        aril.di   r0,[cm:GPA1],[r1]

        aril.di.f   r0,[cm:GPA1],[sd:r1]
        aril.di.f   r0,[cm:GPA1],[xd:r1]
        aril.di.f   r0,[cm:GPA1],[r1]

        aril.di   [cm:GPA1],[sd:r1]
        aril.di   [cm:GPA1],[xd:r1]
        aril.di   [cm:GPA1],[r1]

        ari.di   [cm:GPA1],[sd:r1]
        ari.di   [cm:GPA1],[xd:r1]
        ari.di   [cm:GPA1],[r1]

        ardcl.di   r0,[cm:GPA1],[sd:r1]
        ardcl.di   r0,[cm:GPA1],[xd:r1]
        ardcl.di   r0,[cm:GPA1],[r1]

        ardcl.di.f   r0,[cm:GPA1],[sd:r1]
        ardcl.di.f   r0,[cm:GPA1],[xd:r1]
        ardcl.di.f   r0,[cm:GPA1],[r1]

        ardcl.di   [cm:GPA1],[sd:r1]
        ardcl.di   [cm:GPA1],[xd:r1]
        ardcl.di   [cm:GPA1],[r1]

        ardc.di   [cm:GPA1],[sd:r1]
        ardc.di   [cm:GPA1],[xd:r1]
        ardc.di   [cm:GPA1],[r1]

        aric.di   r0,[cm:GPA1],[sd:r1]
        aric.di   r0,[cm:GPA1],[xd:r1]
        aric.di   r0,[cm:GPA1],[r1]

        aric.di.f   r0,[cm:GPA1],[sd:r1]
        aric.di.f   r0,[cm:GPA1],[xd:r1]
        aric.di.f   r0,[cm:GPA1],[r1]

        aric.di   [cm:GPA1],[sd:r1]
        aric.di   [cm:GPA1],[xd:r1]
        aric.di   [cm:GPA1],[r1]

        aric.r.di   r0,[cm:GPA1],[sd:r1]
        aric.r.di   r0,[cm:GPA1],[xd:r1]
        aric.r.di   r0,[cm:GPA1],[r1]

        aric.r.di.f   r0,[cm:GPA1],[sd:r1]
        aric.r.di.f   r0,[cm:GPA1],[xd:r1]
        aric.r.di.f   r0,[cm:GPA1],[r1]

        aric.r.di   [cm:GPA1],[sd:r1]
        aric.r.di   [cm:GPA1],[xd:r1]
        aric.r.di   [cm:GPA1],[r1]

        arcll.di   r0,[cm:GPA1],[sd:r1]
        arcll.di   r0,[cm:GPA1],[xd:r1]
        arcll.di   r0,[cm:GPA1],[r1]

        arcll.di.f   r0,[cm:GPA1],[sd:r1]
        arcll.di.f   r0,[cm:GPA1],[xd:r1]
        arcll.di.f   r0,[cm:GPA1],[r1]

        arcll.di [cm:GPA1],[sd:r1]
        arcll.di [cm:GPA1],[xd:r1]
        arcll.di [cm:GPA1],[r1]

        arcl.di  [cm:GPA1],[sd:r1]
        arcl.di  [cm:GPA1],[xd:r1]
        arcl.di  [cm:GPA1],[r1]

        xldl.di  r0,[cm:GPA1],[sd:r1]
        xldl.di  r0,[cm:GPA1],[xd:r1]

        xld.di   [cm:GPA1],[sd:r1]
        xld.di   [cm:GPA1],[xd:r1]

        xldl.di   [cm:GPA1],[sd:r1]
        xldl.di   [cm:GPA1],[xd:r1]

        ;On Demand And Shadow On Demand Statistic Counter Operations
        cinit.di.f r2,[cm:GPA2],r2,[cd:r1]
        cinit.di.f r2,[cm:GPA2],r2,[scd:r1]
        cinit.di r2,[cm:GPA2],r2,[cd:r1]
        cinit.di r2,[cm:GPA2],r2,[scd:r1]
        cinit.di [cm:GPA2],r2,[cd:r1]
        cinit.di [cm:GPA2],r2,[scd:r1]

        cminit.di.f r2,[cm:GPA2],r2,[cd:r1]
        cminit.di.f r2,[cm:GPA2],r2,[scd:r1]
        cminit.di r2,[cm:GPA2],r2,[cd:r1]
        cminit.di r2,[cm:GPA2],r2,[scd:r1]
        cminit.di [cm:GPA2],r2,[cd:r1]
        cminit.di [cm:GPA2],r2,[scd:r1]
        cminit.di r2,[cd:r1]
        cminit.di r2,[scd:r1]

        cminit.rst.di.f r2,[cm:GPA2],r2,[cd:r1]
        cminit.rst.di.f r2,[cm:GPA2],r2,[scd:r1]
        cminit.rst.di r2,[cm:GPA2],r2,[cd:r1]
        cminit.rst.di r2,[cm:GPA2],r2,[scd:r1]
        cminit.rst.di r2,[cd:r1]
        cminit.rst.di r2,[scd:r1]

        crd.di.f r2,[cm:GPA2],[cd:r1]
        crd.di.f r2,[cm:GPA2],[scd:r1]
        crd.di r2,[cm:GPA2],[cd:r1]
        crd.di r2,[cm:GPA2],[scd:r1]
        crd.di [cm:GPA2],[cd:r1]
        crd.di [cm:GPA2],[scd:r1]

        cwrdb.di.f r2,[cm:GPA2],[cd:r1]
        cwrdb.di.f r2,[cm:GPA2],[scd:r1]
        cwrdb.di r2,[cm:GPA2],[cd:r1]
        cwrdb.di r2,[cm:GPA2],[scd:r1]
        cwrdb.di [cm:GPA2],[cd:r1]
        cwrdb.di [cm:GPA2],[scd:r1]

        cwrde.di.f r2,[cm:GPA2],[cd:r1]
        cwrde.di.f r2,[cm:GPA2],[scd:r1]
        cwrde.di r2,[cm:GPA2],[cd:r1]
        cwrde.di r2,[cm:GPA2],[scd:r1]
        cwrde.di [cm:GPA2],[cd:r1]
        cwrde.di [cm:GPA2],[scd:r1]

        crst.di.f r2,[cm:GPA2],[cd:r1]
        crst.di.f r2,[cm:GPA2],[scd:r1]
        crst.di r2,[cm:GPA2],[cd:r1]
        crst.di r2,[cm:GPA2],[scd:r1]
        crst.di [cd:r1]
        crst.di [scd:r1]

        cincr.di.f r1,r1,[cd:r2],r1
        cincr.di r1,r1,[cd:r2],r1
        cincr.di.f r1,[cm:GPA2],r1,[cd:r2]
        cincr.di r1,[cm:GPA2],r1,[cd:r2]
        cincr.di [cm:GPA2],r1,[cd:r2]
        cincr.di r1,[cd:r2]

        cdecr.di.f r1,r1,[cd:r2],r1
        cdecr.di r1,r1,[cd:r2],r1
        cdecr.di.f r1,[cm:GPA2],r1,[cd:r2]
        cdecr.di r1,[cm:GPA2],r1,[cd:r2]
        cdecr.di [cm:GPA2],r1,[cd:r2]
        cdecr.di r1,[cd:r2]

        cincr1.di.f r1,r1,[cd:r2]
        cincr1.di r1,r1,[cd:r2]
        cincr1.di.f r1,[cm:GPA2],r1,[cd:r2]
        cincr1.di r1,[cm:GPA2],r1,[cd:r2]
        cincr1.di [cm:GPA2],r1,[cd:r2]
        cincr1.di r1,[cd:r2]

        cdecr1.di.f r1,r1,[cd:r2]
        cdecr1.di r1,r1,[cd:r2]
        cdecr1.di.f r1,[cm:GPA2],r1,[cd:r2]
        cdecr1.di r1,[cm:GPA2],r1,[cd:r2]
        cdecr1.di [cm:GPA2],r1,[cd:r2]
        cdecr1.di r1,[cd:r2]

        cdecrc.di.f r1,[cm:GPA2],r1,[cd:r2]
        cdecrc.di r1,[cm:GPA2],r1,[cd:r2]
        cdecrc.di [cm:GPA2],r1,[cd:r2]
        cdecrc.di r1,[cd:r2]

        cdincr.di.f r1,[cm:GPA2],r1,[cd:r2]
        cdincr.di r1,[cm:GPA2],r1,[cd:r2]
        cdincr.di [cm:GPA2],r1,[cd:r2]
        cdincr.di r1,[cd:r2]

        cbset.di.f r1,r1,[cd:r2]
        cbset.di r1,r1,[cd:r2]
        cbset.di.f r1,[cm:GPA2],r1,[cd:r2]
        cbset.di r1,[cm:GPA2],r1,[cd:r2]
        cbset.di [cm:GPA2],r1,[cd:r2]
        cbset.di r1,[cd:r2]

        cbclr.di.f r1,r1,[cd:r2]
        cbclr.di r1,r1,[cd:r2]
        cbclr.di.f r1,[cm:GPA2],r1,[cd:r2]
        cbclr.di r1,[cm:GPA2],r1,[cd:r2]
        cbclr.di [cm:GPA2],r1,[cd:r2]
        cbclr.di r1,[cd:r2]

        cbwr.di.f r1,r1,[cd:r2]
        cbwr.di r1,r1,[cd:r2]
        cbwr.di.f r1,[cm:GPA2],r1,[cd:r2]
        cbwr.di r1,[cm:GPA2],r1,[cd:r2]
        cbwr.di [cm:GPA2],r1,[cd:r2]
        cbwr.di r1,[cd:r2]

        cbrd.di.f r1,r1,[cd:r2]
        cbrd.di r1,r1,[cd:r2]
        cbrd.di.f r1,[cm:GPA2],r1,[cd:r2]
        cbrd.di r1,[cm:GPA2],r1,[cd:r2]
        cbrd.di [cm:GPA2],r1,[cd:r2]

        cbcswp.di.f r1,r1,[cd:r2]
        cbcswp.di r1,r1,[cd:r2]
        cbcswp.di.f r1,[cm:GPA2],r1,[cd:r2]
        cbcswp.di r1,[cm:GPA2],r1,[cd:r2]
        cbcswp.di [cm:GPA2],r1,[cd:r2]

        cftch.di [cd:r2]

        cgetc.di r1,r2,[cd:r2]
        cgetc.di.f r1,r2,[cd:r2]
        cgetc.di r2,[cd:r2]
        cgetc.di.f r2,[cd:r2]
        cgetc.di [cm:GPA2],r1,[cd:r2]

        cchkc.di r2,r2,[cd:r2]
        cchkc.di r1,[cd:r2]
        cchkc.di [cm:GPA2],r1,[cd:r2]

        cld.di r1,[cm:GPA2],[cd:r2]
        cld.di r1,[cm:GPA2],[scd:r2]
        cld.di [cm:GPA2],[cd:r2]
        cld.di [cm:GPA2],[scd:r2]

        cmld.di r1,[cm:GPA2],[cd:r2]
        cmld.di r1,[cm:GPA2],[scd:r2]
        cmld.di [cm:GPA2],[cd:r2]
        cmld.di [cm:GPA2],[scd:r2]

        cst.di r1,[cm:GPA2],r1,[cd:r2]
        cst.di r1,[cm:GPA2],r1,[scd:r2]
        cst.di [cm:GPA2],r1,[cd:r2]
        cst.di [cm:GPA2],r1,[scd:r2]
        cst.di r2,[cd:r2]
        cst.di r2,[scd:r2]

        cmst.di r1,[cm:GPA2],r1,[cd:r2]
        cmst.di r1,[cm:GPA2],r1,[scd:r2]
        cmst.di [cm:GPA2],r1,[cd:r2]
        cmst.di [cm:GPA2],r1,[scd:r2]
        cmst.di r2,[cd:r2]
        cmst.di r2,[scd:r2]

        cwcfg.di r1,[cm:GPA2],r1,[cd:r2]
        cwcfg.di r1,[cm:GPA2],r1,[scd:r2]
        cwcfg.di.f r1,[cm:GPA2],r1,[cd:r2]
        cwcfg.di.f r1,[cm:GPA2],r1,[scd:r2]
        cwcfg.di [cm:GPA2],r1,[cd:r2]
        cwcfg.di [cm:GPA2],r1,[scd:r2]
        cwcfg.di r2,[cd:r2]
        cwcfg.di r2,[scd:r2]

        cwchk.di r1,r1,[cm:GPA2],[cd:r2]
        cwchk.di.f r1,r1,[cm:GPA2],[cd:r2]
        cwchk.di r1,[cm:GPA2],[cd:r2]
        cwchk.di r2,[cd:r2]

        cgi.di r1,r1,[cm: GPA2],[cd:r2],0x10,0x18
        cgi.di.f r1,r1,[cm: GPA2],[cd:r2],0x10,0x18
        cgi.di.f r1,r1,[cm: GPA2],[cd:r2],0x8,0x1e
        cgi.di.f r1,r1,[cm: GPA2],[cd:r2],0x4,0x1
        cgi.di.f r1,r1,[cm: GPA2],[cd:r2],0x2,0x8
        cgi.di.f r1,r1,[cm: GPA2],[cd:r2],0x1,0xf
        cgi.di r1,[cm: GPA2],[cd:r2],0x10,0x18
        cgi.di r1,[cm: GPA2],[cd:r2],0x8,0x16
        cgi.di r1,[cm: GPA2],[cd:r2],0x4,0x1
        cgi.di r1,[cm: GPA2],[cd:r2],0x2,0x18
        cgi.di r1,[cm: GPA2],[cd:r2],0x1,0x6
        cgi.di r1,[cd:r2],0x10,0x18
        cgi.di r1,[cd:r2],0x8,0x18
        cgi.di r1,[cd:r2],0x4,0x18
        cgi.di r1,[cd:r2],0x2,0x1
        cgi.di r1,[cd:r2],0x1,0x18
        cgi.di r1,r1,[cd:r2],0x10,0x1
        cgi.di r1,r1,[cd:r2],0x8,0x18
        cgi.di r1,r1,[cd:r2],0x4,0x18
        cgi.di r1,r1,[cd:r2],0x2,0x18
        cgi.di r1,r1,[cd:r2],0x1,0x18
