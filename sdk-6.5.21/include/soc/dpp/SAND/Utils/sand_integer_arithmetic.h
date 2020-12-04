/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __SOC_SAND_INTEGER_ARITHMETIC_H__

#define __SOC_SAND_INTEGER_ARITHMETIC_H__
#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <sal/limits.h>



#define SOC_SAND_DIV_ROUND_UP(x,y) ( ((x)+((y)-1)) / (y) )

#define SOC_SAND_DIV_ROUND_DOWN(x,y) ( (x) / (y) )

#define SOC_SAND_ALIGN_UP(x, y) \
  (SOC_SAND_DIV_ROUND_UP(x,y) * (y))


#define SOC_SAND_DIV_ROUND(x,y) ( ((x) + ((y)/2) ) / (y) )



#define SOC_SAND_DELTA(x, y) (((x)>(y))?((x)-(y)):((y)-(x)))









#define SOC_SAND_NOF_BITS_IN_CHAR   (SAL_CHAR_BIT)


#define SOC_SAND_NOF_BITS_IN_UINT32 (SOC_SAND_NOF_BITS_IN_CHAR * sizeof(uint32))


#define SOC_SAND_LOHALF(x) ((uint32)((x) & 0xffff))
#define SOC_SAND_HIHALF(x) ((uint32)((x) >> 16 & 0xffff))
#define SOC_SAND_TOHIGH(x) ((uint32)((x) << 16))
#define SOC_SAND_HIBITMASK (0x80000000)







uint32 soc_sand_log2_round_up(SOC_SAND_IN uint32 x);

uint32
  soc_sand_power_of_2(
    SOC_SAND_IN uint32 power
  );


uint32 soc_sand_log2_round_down(SOC_SAND_IN uint32 x);

uint32 soc_sand_is_power_of_2(SOC_SAND_IN uint32 x);





uint32
  soc_sand_msb_bit_on(
    SOC_SAND_IN uint32 x
  );


uint32
  soc_sand_nof_on_bits_in_long(
    SOC_SAND_IN uint32 x
  );


uint32
  soc_sand_nof_on_bits_in_short(
    SOC_SAND_IN unsigned short x
  );


uint32
  soc_sand_nof_on_bits_in_char(
    SOC_SAND_IN unsigned char x
  );


uint32
  soc_sand_bits_reverse_long(
    SOC_SAND_IN uint32 x
  );


unsigned short
  soc_sand_bits_reverse_short(
    SOC_SAND_IN unsigned short x
  );


unsigned char
  soc_sand_bits_reverse_char(
    SOC_SAND_IN unsigned char x
  );





SOC_SAND_RET
  soc_sand_abs_val_to_mnt_binary_fraction_exp(
    SOC_SAND_IN  uint32  abs_val_numerator,
    SOC_SAND_IN  uint32  abs_val_denominator,
    SOC_SAND_IN  uint32   mnt_nof_bits,
    SOC_SAND_IN  uint32   exp_nof_bits,
    SOC_SAND_IN  uint32  max_val,
    SOC_SAND_OUT uint32* mnt_bin_fraction,
    SOC_SAND_OUT uint32* exp
  );

SOC_SAND_RET
  soc_sand_mnt_binary_fraction_exp_to_abs_val(
    SOC_SAND_IN  uint32   mnt_nof_bits,
    SOC_SAND_IN  uint32   exp_nof_bits,
    SOC_SAND_IN  uint32  max_val,
    SOC_SAND_IN  uint32  mnt_bin_fraction,
    SOC_SAND_IN  uint32  exp,
    SOC_SAND_OUT uint32  *abs_val_numerator,
    SOC_SAND_OUT uint32  *abs_val_denominator
  );

SOC_SAND_RET soc_sand_break_to_mnt_exp_round_up(SOC_SAND_IN  uint32 x,
                                        SOC_SAND_IN  uint32  man_nof_bits,
                                        SOC_SAND_IN  uint32  exp_nof_bits,
                                        SOC_SAND_IN  uint32  mnt_inc,
                                        SOC_SAND_OUT uint32* man,
                                        SOC_SAND_OUT uint32* exp);

SOC_SAND_RET soc_sand_break_to_mnt_exp_round_down(SOC_SAND_IN  uint32 x,
                                          SOC_SAND_IN  uint32  man_nof_bits,
                                          SOC_SAND_IN  uint32  exp_nof_bits,
                                          SOC_SAND_IN  uint32  mnt_inc,
                                          SOC_SAND_OUT uint32* man,
                                          SOC_SAND_OUT uint32* exp);


SOC_SAND_RET
  soc_sand_break_complex_to_mnt_exp_round_down(SOC_SAND_IN  uint32 x,
                                           SOC_SAND_IN  uint32  max_mnt,
                                           SOC_SAND_IN  uint32  max_exp,
                                           SOC_SAND_IN  uint32  eq_const_multi,
                                           SOC_SAND_IN  uint32  eq_const_man_inc,
                                           SOC_SAND_OUT uint32* mnt,
                                           SOC_SAND_OUT uint32* exp);
											  

SOC_SAND_RET
  soc_sand_compute_complex_to_mnt_exp(SOC_SAND_IN  uint32  mnt,
                                  SOC_SAND_IN  uint32  exp,
                                  SOC_SAND_IN  uint32 eq_const_multi,
                                  SOC_SAND_IN  uint32 eq_const_mnt_inc,
                                  SOC_SAND_OUT uint32* x);


SOC_SAND_RET
  soc_sand_break_complex_to_mnt_reverse_exp_round_down(SOC_SAND_IN  uint32 x,
                                                   SOC_SAND_IN  uint32  max_mnt,
                                                   SOC_SAND_IN  uint32  max_exp,
                                                   SOC_SAND_IN  uint32  eq_const_multi,
                                                   SOC_SAND_IN  uint32  eq_const_man_inc,
                                                   SOC_SAND_OUT uint32* mnt,
                                                   SOC_SAND_OUT uint32* exp);


SOC_SAND_RET
  soc_sand_compute_complex_to_mnt_reverse_exp(SOC_SAND_IN  uint32  mnt,
                                          SOC_SAND_IN  uint32  exp,
                                          SOC_SAND_IN  uint32 eq_const_multi,
                                          SOC_SAND_IN  uint32 eq_const_mnt_inc,
                                          SOC_SAND_OUT uint32* x);


SOC_SAND_RET
  soc_sand_compute_complex_to_mnt_exp_reverse_exp(
    SOC_SAND_IN  uint32  mnt,
    SOC_SAND_IN  uint32  exp,
    SOC_SAND_IN  uint32  rev_exp,
    SOC_SAND_IN  uint32  eq_const_multi,
    SOC_SAND_IN  uint32  eq_const_div,
    SOC_SAND_IN  uint32  eq_const_mnt_inc,
    SOC_SAND_IN  uint32 round_to_closest,
    SOC_SAND_OUT uint32* x);


SOC_SAND_RET
  soc_sand_break_complex_to_mnt_exp_round_down_2(
    SOC_SAND_IN  uint32  x,
    SOC_SAND_IN  uint32  max_mnt,
    SOC_SAND_IN  uint32  min_mnt,
    SOC_SAND_IN  uint32  max_exp,
    SOC_SAND_IN  uint32  eq_const_multi,
    SOC_SAND_IN  uint32  eq_const_div,
    SOC_SAND_IN  uint32  eq_const_mnt_inc,
    SOC_SAND_OUT uint32* mnt,
    SOC_SAND_OUT uint32* exp
  );


uint32 soc_sand_abs(SOC_SAND_IN int);


int soc_sand_sign(SOC_SAND_IN int);


int soc_sand_is_even(SOC_SAND_IN uint32 x);




#define SOC_SAND_MIN(x,y) ( (x)<(y)? (x) : (y) )
#define SOC_SAND_MAX(x,y) ( (x)>(y)? (x) : (y) )
#define SOC_SAND_MAX3(x,y,z) ( SOC_SAND_MAX(x, SOC_SAND_MAX(y, z)) )


#if defined (SOC_SAND_DEBUG)


uint32
  soc_sand_integer_arithmetic_test(
    SOC_SAND_IN uint32 silent
  );
uint32
  soc_sand_mnt_binary_fraction_test(
    SOC_SAND_IN uint32 silent
  );

void
  soc_sand_print_u_long_binary_format(
    SOC_SAND_IN uint32 x,
    SOC_SAND_IN uint32 max_digits_in_number
  );

#endif

#ifdef  __cplusplus
}
#endif


#endif
