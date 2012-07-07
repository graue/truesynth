#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

void ts_randinit(uint32_t seed);
uint32_t ts_urand(void);
int32_t ts_rand(void);
double ts_frand(void);

#endif
