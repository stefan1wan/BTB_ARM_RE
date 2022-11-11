
.macro OneJump index
b next_\index
.align 4
next_\index:
.endm

b BtbLoop
.align 8 //22; align to a 4MB boundary

BtbLoop:
.irp label, 0,1,2,3,4
OneJump index=\label
.endr

ret
    
