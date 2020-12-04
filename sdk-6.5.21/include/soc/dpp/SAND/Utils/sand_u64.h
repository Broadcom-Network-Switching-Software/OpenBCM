/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_U64_H_INCLUDED__

#define __SOC_SAND_U64_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>

#define SOC_SAND_U64_NOF_UINT32S (2)

typedef struct
{
  
  uint32 arr[SOC_SAND_U64_NOF_UINT32S];

} SOC_SAND_U64;



uint32
  soc_sand_long_to_u64(
    SOC_SAND_IN  uint32      		u32,
    SOC_SAND_OUT SOC_SAND_U64* 		u64
  );


uint32
  soc_sand_u64_copy(
    SOC_SAND_IN  SOC_SAND_U64*      src,
    SOC_SAND_OUT SOC_SAND_U64* 		dst
  );

void
  soc_sand_u64_clear(
    SOC_SAND_INOUT SOC_SAND_U64* u64
  );


uint32
  soc_sand_u64_to_long(
    SOC_SAND_IN  SOC_SAND_U64*      u64,
    SOC_SAND_OUT uint32* ulong
  );



void
  soc_sand_u64_multiply_longs(
    SOC_SAND_IN  uint32 x,
    SOC_SAND_IN  uint32 y,
    SOC_SAND_OUT SOC_SAND_U64*     result
  );



uint32
  soc_sand_u64_devide_u64_long(
    SOC_SAND_IN  SOC_SAND_U64*     x,
    SOC_SAND_IN  uint32 y,
    SOC_SAND_OUT SOC_SAND_U64*     result
  );


uint32
  soc_sand_u64_add_long(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     uint32 y
  );


uint32
  soc_sand_u64_add_u64(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  );



uint32
  soc_sand_u64_subtract_u64(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  );


uint32
  soc_sand_u64_is_bigger(
    SOC_SAND_IN     SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  );


uint32
  soc_sand_u64_is_zero(
    SOC_SAND_IN     SOC_SAND_U64*     x
  );


uint32
  soc_sand_u64_are_equal(
    SOC_SAND_IN     SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  );


uint32
  soc_sand_u64_log2_round_up(
    SOC_SAND_IN SOC_SAND_U64 *x
  );


uint32
  soc_sand_u64_shift_left(
    SOC_SAND_OUT SOC_SAND_U64*    a,
    SOC_SAND_IN  SOC_SAND_U64*    b,
    SOC_SAND_IN  uint32 x
  );

#if SOC_SAND_DEBUG




void
  soc_sand_u64_print(
    SOC_SAND_IN SOC_SAND_U64*    u64,
    SOC_SAND_IN uint32 print_type,
    SOC_SAND_IN uint32 short_format
  );



uint32
  soc_sand_u64_test(uint32 silent);


#endif

#ifdef  __cplusplus
}
#endif



#endif
