default: btb_test btb_capacity btb_way btb_setindex btb_tag

affinity.o: affinity.c
		gcc -c affinity.c

btb_test: btb_test.c affinity.o log.o
		gcc btb_test.c affinity.o -o btb_test

btb_capacity: btb_capacity.c affinity.o log.o
		gcc btb_capacity.c affinity.o -o btb_capacity

btb_way: btb_way.c affinity.o log.o
		gcc btb_way.c affinity.o -o btb_way

btb_setindex: btb_setindex.c affinity.o log.o
		gcc btb_setindex.c affinity.o -o btb_setindex

btb_tag: btb_tag.c affinity.o log.o
		gcc btb_tag.c affinity.o -o btb_tag

clean:
		rm -f btb_test btb_block.o btb_block.s affinity.o btb_capacity btb_way btb_setindex btb_tag