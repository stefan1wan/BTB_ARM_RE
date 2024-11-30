default: btb_capacity btb_way btb_setindex

affinity.o: affinity.c
		gcc -c affinity.c


btb_capacity: btb_capacity.c affinity.o
		gcc btb_capacity.c affinity.o -o btb_capacity

btb_way: btb_way.c affinity.o
		gcc btb_way.c affinity.o -o btb_way

btb_setindex: btb_setindex.c affinity.o
		gcc btb_setindex.c affinity.o -o btb_setindex


clean:
		rm -f btb_block.o btb_block.s affinity.o btb_capacity btb_way btb_setindex