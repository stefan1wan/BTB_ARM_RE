#define _GNU_SOURCE
#include <sys/mman.h>
#include <linux/mman.h>
#include <string.h>

#include "pmc_utils.h"

#define BTB_BLOCK_FILENAME "btb_block.bin"
#define BUFSIZE (1024*1024*1024) //1GB hugepage
void (*btb_block)() = NULL;

uint32_t repeat(uint32_t event, uint32_t branch_number, uint32_t align){
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

    // don't need to clear the memory, cause we have `ret` in the end
    read(fd, btb_block, BUFSIZE);
    
    /* TODO: flash btb; */
    for(int i=0; i<10; i++){
        btb_block();
    }

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
    // munmap(btb_block, BUFSIZE);
    system("rm btb_block.s btb_block.o btb_block.bin");
    return counter_cycles; // Return the diff of event counter;
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
    uint32_t event = ARM_PMU_BR_MIS_PRED;

    /* Alloc enough memory*/
    btb_block = mmap(NULL, BUFSIZE, PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE| MAP_ANONYMOUS|MAP_HUGETLB|MAP_HUGE_1GB, 0, 0); 
    // 
    if(btb_block == MAP_FAILED){
        printf("mmap failed\n");
        exit(1);
        return 0;
    }

    /* Analyze BTB entry capacity */
    uint32_t branch_vec[] = {1, 2, 4,5,6,7, 8,9,10,11,12,13,14,15, 16, 32, 64}; // modified a little for eviction buffer;
    uint32_t align_vec[] = {12, 13, 14, 15, 16, 17, 18, 19};
    int branch_len = sizeof(branch_vec)/sizeof(branch_vec[0]);
    int align_len = sizeof(align_vec)/sizeof(align_vec[0]);
    uint32_t counter_map[branch_len][align_len];
    for (int i = 0; i <branch_len ; i++){
        for(int j = 0; j < align_len; j++){
            uint32_t counter_diff =  repeat(event, branch_vec[i], align_vec[j]);
            counter_map[i][j] = counter_diff;
        }
    }

    // for(int i=0; i<sizeof branch_vec/sizeof(u_int32_t); i++)
    //     branch_vec[i]  = branch_vec[i] * 2;
    /* Capacity result */
    printf("Capacity result(branch mispred / branches):\n");
    printf("branch/align\t");
    for(int j=0; j<align_len; j++){
        printf("%d\t", align_vec[j]);
    }
    printf("\n");
    for (int i = 0; i <branch_len ; i++){
        printf("%u Branches:\t", branch_vec[i]);
        for(int j = 0; j < align_len; j++){
            printf("%.2lf\t", (double)counter_map[i][j]/(double)branch_vec[i]); // 0 means no mispred; 1 means all mispred
        }
        printf("\n");
    }
    printf("\n");


    /* Capacity result2 */
    printf("Capacity result(branch size - branch mispred):\n");
    printf("branch/align\t");
    for(int j=0; j<align_len; j++){
        printf("%d\t", align_vec[j]);
    }
    printf("\n");
    for (int i = 0; i <branch_len ; i++){
        printf("%u Branches:\t", branch_vec[i]);
        for(int j = 0; j < align_len; j++){
            printf("%d\t", branch_vec[i] - counter_map[i][j]); // 0 means no mispred; 1 means all mispred
        }
        printf("\n");
    }

    /* Write to txt */
    FILE *fp = fopen("btb_miss_pred_map.txt", "w");
    for (int i = 0; i <branch_len ; i++){
        fprintf(fp, "%u \t", branch_vec[i]);
        for(int j = 0; j < align_len; j++){
            fprintf(fp, "%.2lf\t", (double)counter_map[i][j]/(double)branch_vec[i]); // 0 means no mispred; 1 means all mispred
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
    fclose(fp);
    return 0;
}