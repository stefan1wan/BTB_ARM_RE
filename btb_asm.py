

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


if __name__=="__main__":
    CODE = btb_size_arm("abc", 128000, 8) # '0x1f400' bingo
    print(CODE)