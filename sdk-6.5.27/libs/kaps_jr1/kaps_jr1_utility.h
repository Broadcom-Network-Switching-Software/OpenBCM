

#ifndef INCLUDED_KAPS_JR1_UTILITY_H
#define INCLUDED_KAPS_JR1_UTILITY_H

#include <stdint.h>
#include "kaps_jr1_externcstart.h"

#define KAPS_JR1_INLINE __inline

#define KAPS_JR1_MIN(X, Y) (kaps_jr1_assert_((X)==(X), "Side effect in macro parm") kaps_jr1_assert_((Y)==(Y), "Side effect in macro parm") (X) <= (Y) ? (X) : (Y))


#define KAPS_JR1_MAX(X, Y) (kaps_jr1_assert_((X)==(X), "Side effect in macro parm") kaps_jr1_assert_((Y)==(Y), "Side effect in macro parm") (X) >= (Y) ? (X) : (Y))

#define kaps_jr1_NUM_BYTES_PER_80_BITS 10
#define kaps_jr1_BITS_IN_REG           80


void kaps_jr1_convert_dm_to_xy(
    uint8_t * data_d,
    uint8_t * data_m,
    uint8_t * data_x,
    uint8_t * data_y,
    uint32_t datalenInBytes);

void kaps_jr1_convert_xy_to_dm(
    uint8_t * data_x,
    uint8_t * data_y,
    uint8_t * data_d,
    uint8_t * data_m,
    uint32_t dataLenInBytes);


extern uint32_t KapsJr1ReadBitsInArrray(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos);

extern void KapsJr1WriteBitsInArray(
    uint8_t * Arr,
    uint32_t ArrSize,
    uint32_t EndPos,
    uint32_t StartPos,
    uint32_t Value);

void 
KapsJr1WriteBitsGeneric(
    uint8_t *out_data,
    uint32_t out_data_size_in_bits,
    uint32_t out_data_end_bit_pos,
    uint32_t out_data_start_bit_pos,
    uint8_t *in_data,
    uint32_t in_data_size_in_bits,
    uint32_t in_data_end_bit_pos,
    uint32_t in_data_start_bit_pos);


extern void kaps_jr1_FillOnes(
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit);

extern void kaps_jr1_FillZeroes(
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit);

extern void kaps_jr1_CopyData(
    uint8_t * o_data,
    uint8_t * data,
    uint32_t datalen,
    uint32_t startBit,
    uint32_t endBit);



uint32_t
kaps_jr1_hex_to_int(
    uint8_t c);


#include "kaps_jr1_externcend.h"
#endif 
 
