#ifndef __AFFINITY_H
#define __AFFINITY_H 1

void affinity_init(void);
void affinity_set(int cpu);
int has_pmu_access(void);

#endif /* __AFFINITY_H */
