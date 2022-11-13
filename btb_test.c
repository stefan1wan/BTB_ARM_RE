#define _GNU_SOURCE
#include <sys/mman.h>
#include <linux/mman.h>

#include "pmc_utils.h"

#define BTB_BLOCK_FILENAME "btb_block.bin"


void repeat(uint32_t event, uint32_t branch_number, uint32_t align){
    puts("========================================");
    /* Generate test block */
    char buf[4096];
    sprintf(buf, "python3 btb_asm.py %d %d > btb_block.s; as btb_block.s -o btb_block.o; objcopy -O binary btb_block.o btb_block.bin\n", branch_number, align);
    system(buf);
    printf("EXEC: %s\n", buf);
    
    /* Read from file*/
    int fd = open(BTB_BLOCK_FILENAME, O_RDONLY);
    if (fd < 0) {
        printf("open failed");}
    int size = lseek(fd, 0, SEEK_END);
    printf("bin size = %d\n", size);
    lseek(fd, 0, SEEK_SET);
    // read(fd, btb_block, BUFSIZE);

    /* Alloc enough memory*/
    uint64_t BUFSIZE =   1024*1024*1024; //1GB;100*4096*4096; //100M //
    void (*btb_block)() = mmap(NULL, BUFSIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB|MAP_HUGE_1GB, 0, 0); //  mmap(NULL, BUFSIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE, fd, 0); //
    if(btb_block == MAP_FAILED){
        printf("mmap failed\n");
        return;
    }
    read(fd, btb_block, BUFSIZE);

    /*  TODO: flash btb; */
    btb_block();
    btb_block();
    btb_block();

    /* set event */
    set_pmc_events_0(event); 
    /* Calc */
    uint32_t cycles = read_pmccntr();
    uint32_t br_mis_pred = read_PMEVCNTR0_EL0();
    btb_block();
    uint32_t counter_cycles = read_PMEVCNTR0_EL0() - br_mis_pred;
    uint32_t all_cycles = read_pmccntr() - cycles;
    double percent = (double)counter_cycles/(double)branch_number;
    printf("COUNTER diff: %u\n", counter_cycles);
    printf("branch number: %u\n", branch_number);
    printf("CPU cycles diff: 0x%x\n", all_cycles);
    printf("Percent: %lf\n", percent);

    /* Clean */
    munmap(btb_block, BUFSIZE);
    system("rm btb_block.s btb_block.o btb_block.bin");
}

int main(){
    affinity_init();
    affinity_set(2);
    printf("Bind to cpu 2\n");
    int access = has_pmu_access();
    printf("ARM PMU access = %d\n", access);
    if (access == 0){
        printf("No access to PMU\n");
        return 0;
    }
    uint32_t event = ARM_PMU_BR_MIS_PRED;//ARM_PMU_BR_PRED;//ARM_PMU_BF_SPEC;
    ;//ARM_PMU_STALL_FRONTEND;
    // ARM_PMU_STALL_OP_FRONTEND;
    //  ARM_PMU_STALL_FRONTEND;
    // ARM_PMU_BR_MIS_PRED; //ARM_PMU_BR_IMMED_RETIRED; //ARM_PMU_BR_PRED ;//ARM_PMU_BR_MIS_PRED;
    // ARM_PMU_BR_MIS_PRED
    // ARM_PMU_BR_PRED 
    // ARM_PMU_BR_RETIRED
    // repeat(event, 128000, 8);
    // repeat(event, 128000, 4);
    // repeat(event, 128000, 2);
    // repeat(event, 8192, 7);
    // repeat(event, 8192, 6);
    // repeat(event, 8192, 5);
    // repeat(event, 8192, 4);
    // repeat(event, 8192, 3);


    // repeat(event, 6144, 7);
    // repeat(event, 6144, 6);
    // repeat(event, 6144, 5);
    // repeat(event, 6144, 4);
    // repeat(event, 6144, 3);

    // repeat(event, 5120, 7);
    // repeat(event, 5120, 6);
    // repeat(event, 5120, 5);
    // repeat(event, 5120, 4);
    // repeat(event, 5120, 3);
    

    // repeat(event, 4096, 7);
    // repeat(event, 4096, 6);
    // repeat(event, 4096, 5);
    // repeat(event, 4096, 4);
    // repeat(event, 4096, 3);
    // repeat(event, 4096, 2);
    
    // repeat(event, 3072, 7);
    // repeat(event, 3072, 6);
    // repeat(event, 3072, 5);
    // repeat(event, 3072, 4);
    // repeat(event, 3072, 3);
    
    // repeat(event, 2048, 7);
    // repeat(event, 2048, 6);
    // repeat(event, 2048, 5);
    // repeat(event, 2048, 4);
    // repeat(event, 2048, 3);
    

    // repeat(event, 1024, 7);
    // repeat(event, 1024, 6);
    // repeat(event, 1024, 5);
    // repeat(event, 1024, 4);
    // repeat(event, 1024, 3);
    
    // repeat(event, 100, 2);
    
    repeat(event, 1, 19);
    repeat(event, 1, 19);
    repeat(event, 2, 19);
    repeat(event, 2, 19);
    repeat(event, 3, 19);
    repeat(event, 3, 19);
    repeat(event, 4, 19);
    repeat(event, 4, 19);
    repeat(event, 5, 19);
    repeat(event, 5, 19);
    repeat(event, 6, 19);
    repeat(event, 6, 19);
    repeat(event, 7, 19);
    repeat(event, 7, 19);
    repeat(event, 12, 19);
    repeat(event, 12, 19);
    // repeat(event, 8, 16);
    // repeat(event, 9, 16);
    // repeat(event, 10, 16);
    // repeat(event, 20, 16);
    // repeat(event, 50, 16);

    return 0;
}