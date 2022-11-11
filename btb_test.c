#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include "affinity.h"

#define BTB_BLOCK_FILENAME "btb_block.bin"


static inline uint64_t
read_pmccntr(void)
{
    /**
     * @brief The PMCCNTR holds the value of the processor Cycle Counter, CCNT, that counts processor clock cycles.
     * @link: https://developer.arm.com/documentation/ddi0406/c/System-Level-Architecture/System-Control-Registers-in-a-VMSA-implementation/VMSA-System-control-registers-descriptions--in-register-order/PMCCNTR--Performance-Monitors-Cycle-Count-Register--VMSA
     */
	uint64_t val;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
	return val;
}

#define ARM_PMU_BR_MIS_PRED   0x0010
#define ARM_PMU_BR_PRED   0x0012
#define ARMV8_PMEVTYPER_EVTCOUNT_MASK 0x3ff

static inline void
write_PMEVTYPER0_EL0(void)
{
    uint32_t val = ARM_PMU_BR_MIS_PRED;
    asm volatile("msr pmevtyper0_el0, %0" :: "r"(val));
}

static inline uint32_t
read_PMEVCNTR0_EL0(void)
{
	uint32_t val;
	asm volatile("mrs %0,  pmevcntr0_el0" : "=r"(val));
	return val;
}

static inline void init_cycles(uint32_t events)
{
    events &= ARMV8_PMEVTYPER_EVTCOUNT_MASK;
    asm volatile("isb");
    /* Just use counter 0 */
    asm volatile("msr pmevtyper0_el0, %0" : : "r"(events));
    /*   Performance Monitors Count Enable Set register bit 30:1 disable, 31,1
    * enable */
    uint32_t r = 0;

    asm volatile("mrs %0, pmcntenset_el0" : "=r"(r));
    asm volatile("msr pmcntenset_el0, %0" : : "r"(r | 1));
}


void play(uint32_t evtCount){
    uint32_t val = 0;
    /* Access cycle counter */
    asm volatile("mrs %0, pmccntr_el0" : "=r" (val));
    printf("pmccntr_el0: %d", val);

    /* Setup PMU counter to record specific event */
    /* evtCount is the event id */
    evtCount &= ARMV8_PMEVTYPER_EVTCOUNT_MASK;
    asm volatile("isb");
    /* Just use counter 0 here */
    asm volatile("msr pmevtyper0_el0, %0" : : "r" (evtCount));
    /*   Performance Monitors Count Enable Set register bit 30:1 disable, 31,1 enable */
    uint32_t r = 0;
    asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
    asm volatile("msr pmcntenset_el0, %0" : : "r" (r|1));

    /* Read counter */
    asm volatile("mrs %0, pmevcntr0_el0" : "=r" (r));

    /*   Disable PMU counter 0. Performance Monitors Count Enable Set register: clear bit 0*/
    r = 0;
    asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
    asm volatile("msr pmcntenset_el0, %0" : : "r" (r&&0xfffffffe));
}


int main(){
    affinity_init();
    affinity_set(2);
    printf("Bind to cpu 2\n");
    int access = has_pmu_access();
    printf("ARM PMU access = %d\n", access);

    int fd = open(BTB_BLOCK_FILENAME, O_RDONLY);
    if (fd < 0) {
        printf("open failed");}
    
    void (*btb_block)() = mmap(NULL, 100*4096*4096, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE, fd, 0);
    // mprotect(btb_block, 4096, PROT_READ | PROT_EXEC);
    read(fd, btb_block, 4096*4096);

    //play(ARM_PMU_BR_MIS_PRED);
    // write_PMEVTYPER0_EL0();
    // printf("debug1");
    // ARM_PMU_BR_MIS_PRED
    init_cycles(ARM_PMU_BR_PRED); 

    //play();
    btb_block();
    btb_block();

    uint32_t cycles = read_pmccntr();
    printf("cycles: %x\n", cycles);

    uint32_t br_mis_pred = read_PMEVCNTR0_EL0();
    // printf("ARM_PMU_BR_MIS_PRED count: %x\n", br_mis_pred);
    btb_block();
    printf("ARM_PMU_BR_MIS_PRED diff: %x\n", read_PMEVCNTR0_EL0() - br_mis_pred);
    printf("cycles diff: %lx\n", read_pmccntr() - cycles);
    printf("Done!\n");

    return 0;
}