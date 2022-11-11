#define _GNU_SOURCE
#include "log.h"
#include <sched.h>

static cpu_set_t default_affinity;

void affinity_init(void) {
    int r = pthread_getaffinity_np(pthread_self(), sizeof(default_affinity), &default_affinity);
    if (r != 0) {
        log_error("unable to get default affinity");
        assert(0);
    }
}

void affinity_set(int cpu) {
    if (cpu >= 0) {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(cpu, &mask);
        
        int r = pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
        if (r != 0) {
            log_error("unable to set affinity %d", cpu);
        }
    } else {
        int r = pthread_setaffinity_np(pthread_self(), sizeof(default_affinity), &default_affinity);
        if (r != 0) {
            log_error("unable to default affinity");
        }
    } 
}


static volatile int caught_signal = 0;

#define ARM_HOST
static void signal_action(int sig, siginfo_t *si, void *data) {
#if defined(ARM_HOST)
    ucontext_t *uc = (ucontext_t *)data;
    caught_signal = 1;
    /* skip instruction */
    uc->uc_mcontext.pc += 4;
#endif /* ARM_HOST */
}

int has_pmu_access(void) {
    struct sigaction act, oldact;
    caught_signal = 0;

    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signal_action;
    act.sa_flags = SA_ONSTACK | SA_RESTART | SA_SIGINFO;

    sigaction(SIGILL, &act, &oldact);
#if defined(ARM_HOST)
    asm volatile (".word 0xd53b9d00" : : : "x0", "memory");
#endif /* ARM_HOST */
    sigaction(SIGILL, &oldact, NULL);

    return caught_signal == 0 ? 1 : 0;
}
