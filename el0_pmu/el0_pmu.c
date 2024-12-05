/*
 * el0_pmu.c - ARM64 Allow access to PMU counters in EL0
 *
 * usage: insmod el0_pmu.ko
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cpu.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>

#define ARMV8_PMCR_MASK         0x3f
#define ARMV8_PMCR_E            (1 << 0) /* Enable all counters */
#define ARMV8_PMCR_P            (1 << 1) /* Reset all counters */
#define ARMV8_PMCR_C            (1 << 2) /* Cycle counter reset */
#define ARMV8_PMCR_D            (1 << 3) /* CCNT counts every 64th cpu cycle */
#define ARMV8_PMCR_X            (1 << 4) /* Export to ETM */
#define ARMV8_PMCR_DP           (1 << 5) /* Disable CCNT if non-invasive debug*/
#define ARMV8_PMCR_LC           (1 << 6) /* Cycle Counter 64bit overflow*/
#define ARMV8_PMCR_N_SHIFT      11       /* Number of counters supported */
#define ARMV8_PMCR_N_MASK       0x1f

#define ARMV8_PMUSERENR_EN_EL0  (1 << 0) /* EL0 access enable */
#define ARMV8_PMUSERENR_CR      (1 << 2) /* Cycle counter read enable */
#define ARMV8_PMUSERENR_ER      (1 << 3) /* Event counter read enable */

static inline u64 armv8pmu_pmcr_read(void)
{
    u64 val = 0;
    asm volatile ("mrs %0, pmcr_el0" : "=r" (val));
    return val;
}

static inline void armv8pmu_pmcr_write(u64 val)
{
    val &= ARMV8_PMCR_MASK;
    isb();
    asm volatile("msr pmcr_el0, %0" : : "r" (val));
}

static void enable_counters(void *data)
{
    u64 val0 = -1, val1 = -1;
    
    asm volatile("msr pmuserenr_el0, %0" : : "r" (ARMV8_PMUSERENR_CR|ARMV8_PMUSERENR_EN_EL0|ARMV8_PMUSERENR_ER));
    asm volatile("msr pmccfiltr_el0, %0" : : "r" (0));
    asm volatile("msr pmcntenset_el0, %0" : : "r" ((u32)(1 << 31)));
    armv8pmu_pmcr_write(armv8pmu_pmcr_read() | (ARMV8_PMCR_E | ARMV8_PMCR_LC));

    isb();
    val0 = armv8pmu_pmcr_read();
    asm volatile("mrs %0, pmccfiltr_el0" : "=r" (val1));
    
    printk(KERN_INFO "el0_pmu, CPU: %d enabled (%llx,%llx)\n", smp_processor_id(), val0, val1);
}

static void disable_counters(void *data)
{
    asm volatile("msr pmuserenr_el0, %0" : : "r" (0));
    armv8pmu_pmcr_write(armv8pmu_pmcr_read() & (~ARMV8_PMCR_E));
    printk(KERN_INFO "el0_pmu, CPU: %d disabled\n", smp_processor_id());
}

static int __init el0_pmu_module_init(void)
{
    on_each_cpu(enable_counters, NULL, 1);
    printk(KERN_INFO "el0_pmu installed\n");
	return 0;
}

static void __exit el0_pmu_module_exit(void)
{
    on_each_cpu(disable_counters, NULL, 1);
    printk(KERN_INFO "el0_pmu uninstalled\n");
}

module_init(el0_pmu_module_init);
module_exit(el0_pmu_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("C.G.Ritson");
MODULE_DESCRIPTION("EL0 PMU counter access");
