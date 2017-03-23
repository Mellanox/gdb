        .text
        dcmac r2,[cm:r4],[cm:r4],0x28
        dcmac r2,[cm:r4],[cm:0x1234],0x24
        dcmac r2,[cm:0x4321],[cm:r4],0x28
        dcmac 0,[cm:r4],[cm:r4],0x24
        dcmac 0,[cm:r4],[cm:0x1234],0x28
        dcmac 0,[cm:0x4321],[cm:r4],0x24

        dcmpls r2,[cm:r4],[cm:r4], 0x18
        dcmpls r2,[cm:r4],[cm:0x1234], 0x18
        dcmpls r2,[cm:0x8765],[cm:r4], 0x18
        dcmpls 0,[cm:r4],[cm:r4], 0x18
        dcmpls 0,[cm:r4],[cm:0x1234], 0x18
        dcmpls 0,[cm:0x8765],[cm:r4], 0x18
