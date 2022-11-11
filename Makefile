default: btb_test

btb_block.bin:
		python3 btb_asm.py > btb_block.s
		as btb_block.s -o btb_block.o
		objcopy -O binary btb_block.o btb_block.bin

affinity.o: affinity.c
		gcc -c affinity.c

btb_test: btb_test.c btb_block.bin affinity.o log.o
		gcc btb_test.c affinity.o -o btb_test

clean:
		rm -f btb_test btb_block.o btb_block.s affinity.o btb_block.bin