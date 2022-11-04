



#ifndef INCLUDED_KAPS_JR1_MATH_H
#define INCLUDED_KAPS_JR1_MATH_H

#include <stdint.h>
#include <kaps_jr1_externcstart.h>

uint32_t kaps_jr1_crc32(
    uint32_t crc,
    const uint8_t * bits,
    uint32_t num_bytes);
void kaps_jr1_compute_crc10_table(
    void);
uint32_t kaps_jr1_fast_crc10(
    uint8_t * data_p,
    uint32_t numBytes);
void kaps_jr1_crc24(
    uint64_t data,
    int32_t reset_crc,
    uint32_t * crcreg);
uint32_t kaps_jr1_find_next_prime(
    uint32_t lower_bound);

#include <kaps_jr1_externcend.h>

#endif
