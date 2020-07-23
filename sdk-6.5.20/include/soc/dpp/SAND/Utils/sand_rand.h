/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_RAND_H_INCLUDED__

#define __SOC_SAND_RAND_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif


#include <soc/dpp/SAND/Utils/sand_framework.h>

  
  #define SOC_SAND_RAND_N   351
  #define SOC_SAND_RAND_M   175
  #define SOC_SAND_RAND_MATRIX_A  0xEABD75F5
  #define SOC_SAND_RAND_TEMU  11
  #define SOC_SAND_RAND_TEMS  7
  #define SOC_SAND_RAND_TEMT  15
  #define SOC_SAND_RAND_TEML  17
  #define SOC_SAND_RAND_TEMB  0x655E5280UL
  #define SOC_SAND_RAND_TEMC  0xFFD58000



typedef struct
{
  uint32   state[SOC_SAND_RAND_N]; 
  uint32   left;  
  uint32   initf; 
  uint32*  next;

  
  uint32   seed;
} SOC_SAND_RAND;






enum
{
  SOC_SAND_RAND_PERCENT_ONE = 10000,
  SOC_SAND_RAND_PERCENT_100 = 100*SOC_SAND_RAND_PERCENT_ONE
};


void
  soc_sand_rand_seed_set(
    SOC_SAND_OUT SOC_SAND_RAND     *r,
    SOC_SAND_IN  uint32 seed
  );
uint32
  soc_sand_rand_seed_get(
    SOC_SAND_IN SOC_SAND_RAND    *r
  );



uint32
  soc_sand_rand_coin_1(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_IN    uint32 positive_bias
  );


uint32
  soc_sand_rand_coin_2(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_IN    uint32 positive_bias,
    SOC_SAND_IN    uint32 negative_bias
  );


uint32
  soc_sand_rand_range(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_IN    uint32 min,
    SOC_SAND_IN    uint32 max
  );


int
  soc_sand_rand_int(
    SOC_SAND_INOUT SOC_SAND_RAND     *r
  );


uint32
  soc_sand_rand_u_long(
    SOC_SAND_INOUT SOC_SAND_RAND     *r
  );


uint32
  soc_sand_rand_modulo(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_IN    uint32 m
  );









void
  soc_sand_rand_array_permute_u_long(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_INOUT uint32 array[],
    SOC_SAND_IN    uint32 array_size
  );




typedef void (*SOC_SAND_RAND_SWAP_ARR_FUNC)(unsigned char array[], uint32 indx_1, uint32 indx_2);

void
  soc_sand_rand_array_permute(
    SOC_SAND_INOUT SOC_SAND_RAND               *r,
    SOC_SAND_INOUT unsigned char           array[],
    SOC_SAND_IN    uint32           nof_items,
    SOC_SAND_IN    SOC_SAND_RAND_SWAP_ARR_FUNC swap
  );




#if SOC_SAND_DEBUG



void
  soc_sand_rand_print(
    SOC_SAND_IN SOC_SAND_RAND    *r
  );

int soc_sand_rand_test(SOC_SAND_IN uint32);


#endif


#ifdef  __cplusplus
}
#endif



#endif

