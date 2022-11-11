
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include "affinity.h"


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


#define ARM_PMU_BR_IMMED_RETIRED   0x000D
#define ARM_PMU_BR_MIS_PRED_RETIRED   0x0022
#define ARM_PMU_BR_RETIRED   0x0021
#define ARM_PMU_BR_RETURN_RETIRED   0x000E
#define ARM_PMU_STALL_FRONTEND   0x0023
#define ARM_PMU_STALL_OP_FRONTEND   0x003E
#define ARM_PMU_BF_SPEC   0x0105


#define ARMV8_PMEVTYPER_EVTCOUNT_MASK 0x3ff


static inline void set_pmc_events_0(uint32_t events)
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

static inline void disable_pmc_events_0(){
	/*   Disable PMU counter 0. Performance Monitors Count Enable Set register: clear bit 0*/
    uint32_t r = 0;
    asm volatile("mrs %0, pmcntenset_el0" : "=r" (r));
    asm volatile("msr pmcntenset_el0, %0" : : "r" (r&&0xfffffffe));
}


static inline uint32_t
read_PMEVCNTR0_EL0(void)
{
	uint32_t val;
	asm volatile("mrs %0,  pmevcntr0_el0" : "=r"(val));
	return val;
}

static inline uint32_t
read_PMEVCNTR0_EL1(void)
{
	uint32_t val;
	asm volatile("mrs %0,  pmevcntr1_el0" : "=r"(val));
	return val;
}

static inline uint32_t
read_PMEVCNTR0_EL2(void)
{
	uint32_t val;
	asm volatile("mrs %0,  pmevcntr2_el0" : "=r"(val));
	return val;
}

static inline uint32_t
read_PMEVCNTR0_EL3(void)
{
	uint32_t val;
	asm volatile("mrs %0,  pmevcntr3_el0" : "=r"(val));
	return val;
}


static inline uint32_t
read_PMEVCNTR0_EL4(void)
{
	uint32_t val;
	asm volatile("mrs %0,  pmevcntr4_el0" : "=r"(val));
	return val;
}

