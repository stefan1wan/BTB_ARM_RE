### pesudo-code

```
adr x0, next1
BR X0
nop
nop
.......(some other nops)
nop

next1:
adr x0, next2
BR X0
nop
nop
.......(some other nops)
nop

next2:
adr x0, next3
BR X0
nop
nop
.......(some other nops)
nop

next3:
ret
```