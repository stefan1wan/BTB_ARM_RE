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
    uint32_t BUFSIZE = 100*4096*4096; // 1073741824; //1GB
    void (*btb_block)() =  mmap(NULL, BUFSIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE, fd, 0);// mmap(NULL, BUFSIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_HUGETLB, fd, 0);
    
    /* TODO: flash btb; */

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
    uint32_t event = ARM_PMU_BF_SPEC;
    ;//ARM_PMU_STALL_FRONTEND;
    // ARM_PMU_STALL_OP_FRONTEND;
    //  ARM_PMU_STALL_FRONTEND;
    // ARM_PMU_BR_MIS_PRED; //ARM_PMU_BR_IMMED_RETIRED; //ARM_PMU_BR_PRED ;//ARM_PMU_BR_MIS_PRED;
    // ARM_PMU_BR_MIS_PRED
    // ARM_PMU_BR_PRED 
    // ARM_PMU_BR_RETIRED
    repeat(event, 128000, 8);
    repeat(event, 128000, 4);
    repeat(event, 128000, 2);
    repeat(event, 8000, 8);
    repeat(event, 8000, 4);
    repeat(event, 8000, 2);
    repeat(event, 2000, 8);
    repeat(event, 2000, 4);
    repeat(event, 2000, 2);
    repeat(event, 1000, 16);
    repeat(event, 1000, 8);
    repeat(event, 1000, 4);
    repeat(event, 1000, 2);
    return 0;
}