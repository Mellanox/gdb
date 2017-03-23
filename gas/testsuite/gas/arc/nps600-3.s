        .text
        nop
        nop
        nop
        nop
        bbitf0 r2, r1, bbit_lab_2
        bbitf0 r2, r1, bbit_lab_2
        bbitf0 r2, r1, bbit_lab_2
        bbitf0 r2, r1, bbit_lab_2
        bbitf1 r2, r1, bbit_lab_2
        bbitf1 r2, r1, bbit_lab_2
        bbitf1 r2, r1, bbit_lab_2
        bbitf1 r2, r1, bbit_lab_2

.align 1024
        .rep 256
          nop               ;; 256 x 4 = 1024 bytes offset
        .endr
bbit_lab_2:
         nop
