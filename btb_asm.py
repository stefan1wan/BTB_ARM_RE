import sys

def btb_size_x86(name, num_branches, align):
    test_code = """
%macro OneJump 0
jmp %%next
align {align}
%%next:
%endmacro
jmp BtbLoop
align 4 * 1024 * 1024; align to a 4MB boundary
BtbLoop:
%rep {num_branches}
OneJump
%endrep
%rep 64
nop
%endrep
""".format(num_branches=num_branches, align=align)
    print(test_code)
    return test_code

def btb_size_arm_indirect(name, num_branches, align): 
    test_code = """
.macro OneJump index
adr x0, next_\index
BR X0
.align {align}
next_\index:
.endm

b BtbLoop
.align 22 //; align to a 4MB boundary

BtbLoop:
.irp label, {sequence}
OneJump index=\label
.endr

ret
    """.format(sequence=",".join([str(x) for x in range(0, num_branches)]),align=align)
    # print(test_code)
    return test_code


def btb_size_arm_for_evict_buffer(name, num_branches, align): 
    test_code = """
.macro OneJump index
adr x0, next_\index
BR X0
.align {align}
next_\index:
.endm

b BtbLoop
.align 22 //; align to a 4MB boundary

BtbLoop:
.irp label, {sequence}
OneJump index=\label
.endr

.align 24 //; align to make it in different tag

BtbLoop2:
.irp label, {sequence2}
OneJump index=\label
.endr


ret
    """.format(sequence=",".join([str(x) for x in range(0, num_branches)]), sequence2=",".join([str(x) for x in range(0+num_branches, num_branches+num_branches)]), align=align)
    # print(test_code)
    return test_code


def btb_size_arm(name, num_branches, align): 
    test_code = """
.macro OneJump index
b next_\index
.align {align}
next_\index:
.endm

b BtbLoop
.align 22 //; align to a 4MB boundary

BtbLoop:
.irp label, {sequence}
OneJump index=\label
.endr

ret
    """.format(sequence=",".join([str(x) for x in range(0, num_branches)]),align=align)
    # print(test_code)
    return test_code


def btb_size_arm_conditional(name, num_branches, align): 
    test_code = """
.macro OneJump index
mov x0, #0
cmp x0, #0
beq next_\index
.align {align}
next_\index:
.endm

b BtbLoop
.align 22 //; align to a 4MB boundary

BtbLoop:
.irp label, {sequence}
OneJump index=\label
.endr

ret
    """.format(sequence=",".join([str(x) for x in range(0, num_branches)]),align=align)
    # print(test_code)
    return test_code

if __name__=="__main__":
    if len(sys.argv) < 2:
        branch_size = 8000
        align = 8
    else:
        branch_size = int(sys.argv[1])
        align = int(sys.argv[2])

    # CODE = btb_size_arm("abc", branch_size, align) # '128000:0x1f400' bingo
    # print(CODE)
    # CODE = btb_size_arm("abc", branch_size, align)
    # CODE = btb_size_arm_conditional("abc", branch_size, align)
    CODE = btb_size_arm_indirect("abc", branch_size, align) # '128000:0x1f400' bingo
    # CODE = btb_size_arm_for_evict_buffer("abc", branch_size, align) # '128000:0x1f400' bingo
    print(CODE)