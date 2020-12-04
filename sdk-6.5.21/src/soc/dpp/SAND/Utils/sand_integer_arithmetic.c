/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>





uint32
  soc_sand_log2_round_up(
    SOC_SAND_IN uint32 x
  )
{
  uint32
    msb_bit;

  msb_bit = soc_sand_msb_bit_on(x) ;

  if ( x > (((uint32)1)<<msb_bit) )
  {
    
    msb_bit++;
  }

  return msb_bit;
}


uint32
  soc_sand_power_of_2(
    SOC_SAND_IN uint32 power
  )
{
  uint32
    answer;

  if (power > SOC_SAND_REG_MAX_BIT)
  {
    answer = 0;
  }
  else
  {
    answer = 1 << power;
  }

  return answer;
}


uint32
  soc_sand_log2_round_down(
    SOC_SAND_IN uint32 x
  )
{
  return soc_sand_msb_bit_on(x);
}



uint32
  soc_sand_is_power_of_2(
    SOC_SAND_IN uint32 x
  )
{
  uint32
    answer;

  
  answer = (soc_sand_nof_on_bits_in_long(x) == 1);

  return answer;
}






const
  unsigned char
    Soc_sand_nof_on_bit_in_char[256]=
{
  0, 1, 1, 2, 1, 2, 2, 3,
  1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7,
  5, 6, 6, 7, 6, 7, 7, 8
};


uint32
  soc_sand_msb_bit_on(
    SOC_SAND_IN uint32 x_in
  )
{
  uint32
    n,
    x;
  int
    i;
   x = x_in;

  if (x == 0)
  {
    return(0);
  }

  n = 0;
  for (i=16 ; i>0 ; i>>=1)
  {
    if (x & ((~((uint32)0))<<i))
    {
      n += i;
      x >>= i;
    }
  }
  return n;
}



uint32
  soc_sand_nof_on_bits_in_long(
    SOC_SAND_IN uint32 x
  )
{
  uint32
    nof_on_bits;

  nof_on_bits = 0;
  nof_on_bits += Soc_sand_nof_on_bit_in_char[SOC_SAND_GET_BYTE_0(x)];
  nof_on_bits += Soc_sand_nof_on_bit_in_char[SOC_SAND_GET_BYTE_1(x)];
  nof_on_bits += Soc_sand_nof_on_bit_in_char[SOC_SAND_GET_BYTE_2(x)];
  nof_on_bits += Soc_sand_nof_on_bit_in_char[SOC_SAND_GET_BYTE_3(x)];

  return nof_on_bits;
}


uint32
  soc_sand_nof_on_bits_in_short(
    SOC_SAND_IN unsigned short x
  )
{
  uint32
    nof_on_bits;

  nof_on_bits = 0;
  nof_on_bits += Soc_sand_nof_on_bit_in_char[SOC_SAND_GET_BYTE_2(x)];
  nof_on_bits += Soc_sand_nof_on_bit_in_char[SOC_SAND_GET_BYTE_3(x)];

  return nof_on_bits;
}


uint32
  soc_sand_nof_on_bits_in_char(
    SOC_SAND_IN unsigned char x
  )
{
  uint32
    nof_on_bits;

  nof_on_bits = 0;
  nof_on_bits += Soc_sand_nof_on_bit_in_char[x];

  return nof_on_bits;
}


#define SOC_SAND_REVERSE_BYTES_NOF_ENTRIES (256)
static const unsigned char
  Soc_sand_reverse_bytes[SOC_SAND_REVERSE_BYTES_NOF_ENTRIES] =
{
0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF,
};




uint32
  soc_sand_bits_reverse_long(
    SOC_SAND_IN uint32 x
  )
{
  uint32
    reverse;


  reverse  = (Soc_sand_reverse_bytes[SOC_SAND_GET_BYTE_0(x)])<<0;
  reverse |= (Soc_sand_reverse_bytes[SOC_SAND_GET_BYTE_1(x)])<<8;
  reverse |= (Soc_sand_reverse_bytes[SOC_SAND_GET_BYTE_2(x)])<<16;
  reverse |= (Soc_sand_reverse_bytes[SOC_SAND_GET_BYTE_3(x)])<<24;

  return reverse;
}


unsigned short
  soc_sand_bits_reverse_short(
    SOC_SAND_IN unsigned short x
  )
{
  unsigned short
    reverse;

  reverse  = (Soc_sand_reverse_bytes[SOC_SAND_GET_BYTE_2(x)])<<0;
  reverse |= (Soc_sand_reverse_bytes[SOC_SAND_GET_BYTE_3(x)])<<8;

  return reverse;
}

unsigned char
  soc_sand_bits_reverse_char(
    SOC_SAND_IN unsigned char x
  )
{
  unsigned char
    reverse;


  reverse = Soc_sand_reverse_bytes[x];

  return reverse;
}





SOC_SAND_RET
  soc_sand_break_to_mnt_exp_round_down(
    SOC_SAND_IN  uint32 x,
    SOC_SAND_IN  uint32  mnt_nof_bits,
    SOC_SAND_IN  uint32  exp_nof_bits,
    SOC_SAND_IN  uint32  mnt_inc,
    SOC_SAND_OUT uint32* mnt,
    SOC_SAND_OUT uint32* exp
  )
{
  uint32
    y;
  uint32
    mnt_max,
    exp_max,
    msb_bit_on;
  int
    msb_bit_diff;
  SOC_SAND_RET
    ex ;

  ex = SOC_SAND_OK ;

  
  if ( (mnt == NULL) ||
       (exp == NULL) ||
       (mnt_nof_bits > 15) ||
       (exp_nof_bits > 10) ||
       (mnt_nof_bits==0) ||
       (exp_nof_bits==0) ||
       (mnt_inc > 1)
     )
  {
    
    ex = SOC_SAND_ERR ;
    goto exit ;
  }

  mnt_max = (0x1<<mnt_nof_bits)-1 ;
  exp_max = (0x1<<exp_nof_bits)-1 ;
  msb_bit_on = 0 ;
  msb_bit_diff = 0 ;

  if(exp_max>= (sizeof(uint32)*8-1))
  {
    
    exp_max = sizeof(uint32)*8-2;
  }


  *mnt = 0;
  *exp = 0;

  if (x <= mnt_inc)
  { 
    goto exit ;
  }
  y = ((1 )<<exp_max)*mnt_max;
  if (x > y)
  { 
    *mnt = mnt_max;
    *exp = exp_max;
    goto exit ;
  }

  msb_bit_on = soc_sand_msb_bit_on(x) ;

  msb_bit_diff = (msb_bit_on+1) - mnt_nof_bits;
  if (msb_bit_diff<=0)
  {
    msb_bit_diff = 0 ;
  }

  *mnt = mnt_max << msb_bit_diff ;
  *mnt &= x ;

  
  if ((*mnt) >= mnt_inc)
  {
    (*mnt) -= mnt_inc;
  }
  else
  {
    if ( (*exp) > 0)
    {
      (*exp) -- ;
    }
    else
    {
      
      *exp = 0 ;
    }
  }
  *mnt >>= msb_bit_diff;
  *exp = msb_bit_diff ;
exit:
  return ex ;
}


SOC_SAND_RET
  soc_sand_break_to_mnt_exp_round_up(
    SOC_SAND_IN  uint32 x,
    SOC_SAND_IN  uint32  mnt_nof_bits,
    SOC_SAND_IN  uint32  exp_nof_bits,
    SOC_SAND_IN  uint32  mnt_inc,
    SOC_SAND_OUT uint32* mnt,
    SOC_SAND_OUT uint32* exp
  )
{
  uint32
    mnt_max,
    exp_max ;
  uint32
    num ;
  SOC_SAND_RET
    ex ;

  mnt_max = (0x1<<mnt_nof_bits)-1,
  exp_max = (0x1<<exp_nof_bits)-1 ;
  ex = SOC_SAND_OK;

  
  ex = soc_sand_break_to_mnt_exp_round_down(x, mnt_nof_bits, exp_nof_bits, mnt_inc, mnt, exp);
  if (ex != SOC_SAND_OK)
  {
    goto exit ;
  }
  if(exp_max>= (sizeof(uint32)*8-1))
  {
    exp_max = sizeof(uint32)*8-2;
  }

  num = (*mnt + mnt_inc) << (*exp);
  if (num<x)
  {
    
    (*mnt) ++;
    if (*mnt>mnt_max)
    {
      *mnt = *mnt/2;
      (*exp) ++;
      if ((*exp)> exp_max)
      {
        *exp = exp_max ;
        *mnt = mnt_max ;
      }
    }
  }

exit:
  return ex ;
}



SOC_SAND_RET
  soc_sand_compute_complex_to_mnt_exp(
    SOC_SAND_IN  uint32  mnt,
    SOC_SAND_IN  uint32  exp,
    SOC_SAND_IN  uint32 eq_const_multi,
    SOC_SAND_IN  uint32 eq_const_mnt_inc,
    SOC_SAND_OUT uint32* x
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK;

  if (x == NULL)
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }

  if (exp > SOC_SAND_REG_MAX_BIT)
  {
    ex = SOC_SAND_ERR;
    goto exit ;
  }

  *x = eq_const_multi * (mnt + eq_const_mnt_inc) * (1 << exp);

exit:
  return ex ;
}



SOC_SAND_RET
  soc_sand_compute_complex_to_mnt_reverse_exp(
    SOC_SAND_IN  uint32  mnt,
    SOC_SAND_IN  uint32  exp,
    SOC_SAND_IN  uint32 eq_const_multi,
    SOC_SAND_IN  uint32 eq_const_mnt_inc,
    SOC_SAND_OUT uint32* x
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK;
  SOC_SAND_U64
    tmp_result,
    result;
  uint32
    res;

  if (x == NULL)
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }

  if (exp > SOC_SAND_REG_MAX_BIT)
  {
    ex = SOC_SAND_ERR;
    goto exit ;
  }


  soc_sand_u64_multiply_longs(eq_const_multi, (mnt + eq_const_mnt_inc), &tmp_result); 

  res = soc_sand_u64_devide_u64_long(&tmp_result, (1 << exp), &result);  

  res = soc_sand_u64_to_long(&result, x);
  if (res) 
  {
    ex = SOC_SAND_ERR;
    goto exit ;
  }

exit:
  return ex ;
}



SOC_SAND_RET
  soc_sand_compute_complex_to_mnt_exp_reverse_exp(
    SOC_SAND_IN  uint32  mnt,
    SOC_SAND_IN  uint32  exp,
    SOC_SAND_IN  uint32  rev_exp,
    SOC_SAND_IN  uint32 eq_const_multi,
    SOC_SAND_IN  uint32 eq_const_div,
    SOC_SAND_IN  uint32 eq_const_mnt_inc,
    SOC_SAND_IN  uint32 round_to_closest,
    SOC_SAND_OUT uint32* x
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK;
  SOC_SAND_U64
    tmp_result,
    tmp_result_2,
    result;
  uint32
    tmp_res,
    res;

  if (x == NULL)
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }


  soc_sand_u64_multiply_longs(eq_const_multi, (1 << exp), &tmp_result); 

  
  res = soc_sand_u64_to_long(&tmp_result, &tmp_res);
  if (res) 
  {
    if(round_to_closest)
    {
        res = soc_sand_u64_add_long(&tmp_result, (((1 << rev_exp) * eq_const_div)/2));  
    }
        
    res = soc_sand_u64_devide_u64_long(&tmp_result, (1 << rev_exp) * eq_const_div, &tmp_result_2);                  
    
    res = soc_sand_u64_to_long(&tmp_result_2, &tmp_res);
    if (res) 
    {
      ex = SOC_SAND_ERR ;
      goto exit ;
    }

    soc_sand_u64_multiply_longs(tmp_res, (mnt + eq_const_mnt_inc), &result); 
  }
  else { 
    
    soc_sand_u64_multiply_longs(tmp_res, (mnt + eq_const_mnt_inc), &tmp_result); 
    if(round_to_closest)
    {
        res = soc_sand_u64_add_long(&tmp_result, (((1 << rev_exp) * eq_const_div)/2));        
    }

    res = soc_sand_u64_devide_u64_long(&tmp_result, (1 << rev_exp) * eq_const_div, &result);                       
  }

  res = soc_sand_u64_to_long(&result, x);
  if (res) 
  {
    ex = SOC_SAND_ERR;
    goto exit ;
  }

exit:
  return ex ;
}



SOC_SAND_RET
  soc_sand_break_complex_to_mnt_exp_round_down(
    SOC_SAND_IN  uint32  x,
    SOC_SAND_IN  uint32 max_mnt,
    SOC_SAND_IN  uint32 max_exp,
    SOC_SAND_IN  uint32 eq_const_multi,
    SOC_SAND_IN  uint32 eq_const_mnt_inc,
    SOC_SAND_OUT uint32* mnt,
    SOC_SAND_OUT uint32* exp
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK;
  uint32
    bin_serach_beg,
    bin_serach_end,
    bin_serach_num_elements,
    bin_serach_mid,
    bin_serach_low_val,
    bin_serach_high_val,
    exp_man;  

  if ((mnt == NULL) || (exp == NULL) || (eq_const_multi == 0))
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }

  exp_man = eq_const_multi * (eq_const_mnt_inc + max_mnt);

  if (x <= exp_man)
  {
    *exp = 0;
    if (x < (eq_const_multi * eq_const_mnt_inc))
    {
      *mnt = 0;
    } 
    else
    {
      *mnt = (x / eq_const_multi) - eq_const_mnt_inc;
    }
    goto exit;
  }

  if (x >= exp_man * (1 << max_exp))
  {
    *exp = max_exp;
    *mnt = max_mnt;
    goto exit;
  }

  bin_serach_beg = 0;  
  bin_serach_end = max_exp;
  bin_serach_num_elements = max_exp;
  bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;
  bin_serach_low_val = exp_man * (1 << bin_serach_mid);
  bin_serach_high_val = exp_man * (1 << (bin_serach_mid+1));

  while((bin_serach_beg <= bin_serach_end) && 
        !((x > bin_serach_low_val) && (x <= bin_serach_high_val)))  
  {
    if (x <= bin_serach_low_val)	  
    {      
      bin_serach_end = bin_serach_mid - 1;
      bin_serach_num_elements = bin_serach_num_elements/2;      
      bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;  
    }
    else    
    {      
      bin_serach_beg = bin_serach_mid + 1;     
      bin_serach_num_elements = bin_serach_num_elements/2;
      bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;
    }
    bin_serach_low_val = exp_man * (1 << bin_serach_mid);
    bin_serach_high_val = exp_man * (1 << (bin_serach_mid+1));
  }

  *exp = bin_serach_mid + 1;

  if (x < (eq_const_multi * eq_const_mnt_inc * (1 << *exp))) {
      *exp -= 1;
      *mnt = max_mnt;
  } else {
      *mnt = (x -(eq_const_multi * eq_const_mnt_inc * (1 << *exp))) /  (eq_const_multi * (1 << *exp));
  }
  
exit:
  return ex ;

}


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
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK;
  uint32
    bin_serach_beg,
    bin_serach_end,
    bin_serach_num_elements,
    bin_serach_mid,
    bin_serach_low_val,
    bin_serach_high_val,
    exp_man,
    res,
    tmp; 
  SOC_SAND_U64
    tmp_64,
    tmp_res_64,
	tmp_val,
	quotient_mul_dividor_64,
	remainder,
	divisor_half; 

  if ((mnt == NULL) || (exp == NULL) || (eq_const_multi == 0) || (eq_const_div == 0))
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }

  soc_sand_u64_multiply_longs(eq_const_multi, (max_mnt + eq_const_mnt_inc), &tmp_64); 
  res = soc_sand_u64_devide_u64_long(&tmp_64, eq_const_div, &tmp_res_64); 
  res = soc_sand_u64_to_long(&tmp_res_64, &exp_man);
  if (res) 
  {
    ex = SOC_SAND_ERR;
    goto exit ;
  }

  
  if (x <= exp_man)
  {
    *exp = 0;
    soc_sand_u64_multiply_longs(eq_const_multi, eq_const_mnt_inc, &tmp_64); 
    res = soc_sand_u64_devide_u64_long(&tmp_64, eq_const_div, &tmp_res_64);
    res = soc_sand_u64_to_long(&tmp_res_64, &tmp);
    if (res) 
    {
      ex = SOC_SAND_ERR;
      goto exit ;
    } 
    
    if (x < tmp)
    {
      *mnt = min_mnt;
    }
	 
    else
    {
      
      soc_sand_u64_multiply_longs(x, eq_const_div, &tmp_64);
      res = soc_sand_u64_devide_u64_long(&tmp_64, eq_const_multi, &tmp_res_64);
      res = soc_sand_u64_to_long(&tmp_res_64, &tmp);
      if (res) 
      {
        ex = SOC_SAND_ERR;
        goto exit ;
      } 

	  
	  soc_sand_u64_multiply_longs(tmp, eq_const_multi, &quotient_mul_dividor_64);
	  soc_sand_u64_copy(&tmp_64, &remainder);
	  if (soc_sand_u64_subtract_u64(&remainder, &quotient_mul_dividor_64))
		  
	  {
		  soc_sand_long_to_u64(eq_const_multi / 2, &divisor_half);
		  if (soc_sand_u64_is_bigger(&remainder, &divisor_half)
			  && (tmp - eq_const_mnt_inc < max_mnt)){
			  
				  tmp += 1;
		  }
	  }
	  
      *mnt = tmp - eq_const_mnt_inc;
      if(*mnt < min_mnt) {
        *mnt = min_mnt;
      }
    }
    goto exit;
  }

  
  soc_sand_u64_multiply_longs(exp_man, (uint32)(1 << max_exp), &tmp_val);
  res = soc_sand_u64_to_long(&tmp_val, &tmp); 
  if (!res  && (x >= tmp))
  {
    *exp = max_exp;
    *mnt = max_mnt;
    goto exit;
  }

  bin_serach_beg = 0;  
  bin_serach_end = max_exp;
  bin_serach_num_elements = max_exp;
  bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;
  bin_serach_low_val = exp_man * (1 << bin_serach_mid);
  bin_serach_high_val = exp_man * (1 << (bin_serach_mid+1));

  while((bin_serach_beg <= bin_serach_end) && 
        !((x > bin_serach_low_val) && (x <= bin_serach_high_val)))  
  {
    if (x <= bin_serach_low_val)  
    {      
      bin_serach_end = bin_serach_mid - 1;
      bin_serach_num_elements = bin_serach_num_elements/2;      
      bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;  
    }
    else    
    {      
      bin_serach_beg = bin_serach_mid + 1;     
      bin_serach_num_elements = bin_serach_num_elements/2;
      bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;
    }
    bin_serach_low_val = exp_man * (1 << bin_serach_mid);
    bin_serach_high_val = exp_man * (1 << (bin_serach_mid+1));
  }

  *exp = bin_serach_mid + 1;
  
  soc_sand_u64_multiply_longs(x, eq_const_div, &tmp_64); 
  res = soc_sand_u64_devide_u64_long(&tmp_64, eq_const_multi, &tmp_res_64);
  res = soc_sand_u64_devide_u64_long(&tmp_res_64, (1 << *exp), &tmp_64);
  res = soc_sand_u64_to_long(&tmp_64, &tmp);
  if (res) 
  {
    ex = SOC_SAND_ERR;
    goto exit ;
  }
  *mnt = tmp - eq_const_mnt_inc;

  
  
  if (mnt && *mnt < min_mnt)
  {
    
    
    --(*exp);
    *mnt = 2*tmp - eq_const_mnt_inc;

    
    
    if(*mnt > max_mnt) {
      *mnt = max_mnt;
    }
  }

exit:
  return ex ;
}



SOC_SAND_RET
  soc_sand_break_complex_to_mnt_reverse_exp_round_down(
    SOC_SAND_IN  uint32  x,
    SOC_SAND_IN  uint32 max_mnt,
    SOC_SAND_IN  uint32 max_exp,
    SOC_SAND_IN  uint32 eq_const_multi,
    SOC_SAND_IN  uint32 eq_const_mnt_inc,
    SOC_SAND_OUT uint32* mnt,
    SOC_SAND_OUT uint32* exp
  )
{
  SOC_SAND_RET
    ex = SOC_SAND_OK;
  uint32
    bin_serach_beg,
    bin_serach_end,
    bin_serach_num_elements,
    bin_serach_mid,
    bin_serach_low_val,
    bin_serach_high_val,
    exp_man;

  if ((mnt == NULL) || (exp == NULL) || (eq_const_multi == 0))
  {
    ex = SOC_SAND_ERR ;
    goto exit ;
  }

  exp_man = eq_const_multi * (eq_const_mnt_inc + max_mnt);

  if (x <= exp_man / (1 << max_exp))
  {
    *exp = max_exp;
    *mnt = 0;
    goto exit;
  }

  if (x >= exp_man)
  {
    *exp = 0;
    *mnt = max_mnt;
    goto exit;
  }

  bin_serach_beg = 0;  
  bin_serach_end = max_exp;
  bin_serach_num_elements = max_exp;
  bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;
  bin_serach_low_val = exp_man / (1 << bin_serach_mid);
  bin_serach_high_val = exp_man / (1 << (bin_serach_mid+1));

  while((bin_serach_beg <= bin_serach_end) && 
        !((x >= bin_serach_high_val) && (x <= bin_serach_low_val)))  
  {
    if (x > bin_serach_low_val)	  
    {      
      bin_serach_end = bin_serach_mid - 1;
      bin_serach_num_elements = bin_serach_num_elements/2;      
      bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;  
    }
    else    
    {      
      bin_serach_beg = bin_serach_mid + 1;     
      bin_serach_num_elements = bin_serach_num_elements/2;
      bin_serach_mid = bin_serach_beg + bin_serach_num_elements/2;
    }
    bin_serach_low_val = exp_man / (1 << bin_serach_mid);
    bin_serach_high_val = exp_man / (1 << (bin_serach_mid+1));
  }

  *exp = bin_serach_mid;
  if ((x * (1 << *exp) / eq_const_multi) < eq_const_mnt_inc)
  {
    *mnt = 0;
  }
  else
  {
    *mnt = (x * (1 << *exp) / eq_const_multi) - eq_const_mnt_inc;
  }
 
exit:
  return ex ;

}
    


SOC_SAND_RET
  soc_sand_abs_val_to_mnt_binary_fraction_exp(
    SOC_SAND_IN  uint32  abs_val_numerator,
    SOC_SAND_IN  uint32  abs_val_denominator,
    SOC_SAND_IN  uint32   mnt_nof_bits,
    SOC_SAND_IN  uint32   exp_nof_bits,
    SOC_SAND_IN  uint32  max_val,
    SOC_SAND_OUT uint32* mnt_bin_fraction,
    SOC_SAND_OUT uint32* exp
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    tmp_num,
    tmp_exp,
    real_max_val;
  SOC_SAND_U64
    max_val_mult_u64,
    abs_val_numerator_u64;

  ex = SOC_SAND_OK ;
  real_max_val = (1<<((1<<exp_nof_bits) + mnt_nof_bits - 1));

  if(abs_val_numerator <= abs_val_denominator)
  {
    *mnt_bin_fraction = (1<< mnt_nof_bits) -1;
    SOC_SAND_LIMIT_FROM_ABOVE(*mnt_bin_fraction, max_val-1);
    *exp              = 0;
    goto exit;
  }
  tmp_exp = (1<<exp_nof_bits) - 1;

  soc_sand_u64_multiply_longs(max_val, abs_val_denominator, &max_val_mult_u64);
  soc_sand_long_to_u64(abs_val_numerator, &abs_val_numerator_u64);

  if (soc_sand_u64_is_bigger(&abs_val_numerator_u64, &max_val_mult_u64)) 
  {
    tmp_num = (real_max_val * abs_val_denominator) / max_val;
  }
  else
  {
    tmp_num = (real_max_val * abs_val_denominator) / abs_val_numerator;
  }

  if(tmp_num == 0)
  {
    tmp_num = 1;
  }

  while(tmp_num >= (uint32)(1<< mnt_nof_bits))
  {
    tmp_num /= 2;
    tmp_exp -= 1;
  }

  *mnt_bin_fraction = tmp_num;
  *exp = tmp_exp;
exit:
  return ex ;
}

SOC_SAND_RET
  soc_sand_mnt_binary_fraction_exp_to_abs_val(
    SOC_SAND_IN  uint32   mnt_nof_bits,
    SOC_SAND_IN  uint32   exp_nof_bits,
    SOC_SAND_IN  uint32  max_val,
    SOC_SAND_IN  uint32  mnt_bin_fraction,
    SOC_SAND_IN  uint32  exp,
    SOC_SAND_OUT uint32  *abs_val_numerator,
    SOC_SAND_OUT uint32  *abs_val_denominator
  )
{
  SOC_SAND_RET
    ex ;
  uint32
    tmp_num,
    tmp_exp,
    real_max_val;

  ex = SOC_SAND_OK ;

  *abs_val_denominator = 1;
  if((mnt_bin_fraction == (uint32)((1<< mnt_nof_bits) -1)) &&
     (exp == 0)
    )
  {
    *abs_val_numerator   = 1;
    goto exit;
  }
  tmp_num = mnt_bin_fraction;
  tmp_exp = (1<<exp_nof_bits) - 1;
  real_max_val = (1<<((1<<exp_nof_bits) + mnt_nof_bits - 1));

  while(tmp_exp > exp)
  {
    tmp_num *= 2;
    tmp_exp -= 1;
  }

  *abs_val_numerator = 0;
  if(tmp_num)
  {
    if(tmp_num > ((1<< mnt_nof_bits) -1))
    {
      *abs_val_denominator = (1<< mnt_nof_bits);
    }
    *abs_val_numerator = (real_max_val * (*abs_val_denominator)) / tmp_num;
  }

  if((*abs_val_numerator) / (*abs_val_denominator)> max_val)
  {
    (*abs_val_numerator) = max_val * (*abs_val_denominator);
  }
exit:
  return ex ;
}



uint32
  soc_sand_abs(
    SOC_SAND_IN int x
  )
{
  int
    y;
  if(x<0)
  {
     y = -x;
  }
  else
  {
     y = x;
  }
  return y;
}

int
  soc_sand_sign(
    SOC_SAND_IN int x
  )
{
  int
    sign;

  if (x < 0)
  {
    sign = -1;
  }
  else if (x == 0)
  {
    sign = 0;
  }
  else
  {
    sign = 1;
  }
  return sign;
}


int
  soc_sand_is_even(
    SOC_SAND_IN uint32 x
  )
{
  int ret;

  if(x%2)
  {
    ret = FALSE;
  }
  else
  {
    ret = TRUE;
  }

  return ret;
}


#if defined (SOC_SAND_DEBUG)



STATIC uint32
  soc_sand_msb_bit_on_TEST(
    SOC_SAND_IN uint32 x
  )
{
  int
    i;

  if (x==0)
  {
    return 0;
  }

  i=(sizeof(uint32)*8) - 1;
  for (; i>=0; --i)
  {
    if ((SOC_SAND_BIT(i)&x) != 0)
    {
      return (i);
    }
  }
  
  return 0;
}

uint32
  soc_sand_mnt_binary_fraction_test(
    SOC_SAND_IN uint32 silent
  )
{
  uint32
    res = 0;
  uint32
    abs_val_numerator_i,
    abs_val_denominator_i,
    exact_abs_val_numerator_i,
    exact_abs_val_denominator_i,
    max_val_i,
    mnt,
    exp;

  for(max_val_i = 100; max_val_i < 4000; max_val_i+=100)
  {
    for(abs_val_denominator_i = 1; abs_val_denominator_i < 60; abs_val_denominator_i++)
    {
      for(abs_val_numerator_i = abs_val_denominator_i;
          abs_val_numerator_i < 600;
          abs_val_numerator_i++
         )
      {
        soc_sand_abs_val_to_mnt_binary_fraction_exp(
          abs_val_numerator_i,abs_val_denominator_i,5,3,max_val_i,&mnt,&exp
        );
        soc_sand_mnt_binary_fraction_exp_to_abs_val(
          5,3,max_val_i,mnt,exp,&exact_abs_val_numerator_i, &exact_abs_val_denominator_i
        );
        if((abs_val_numerator_i * 100 / abs_val_denominator_i) >
           (exact_abs_val_numerator_i * 110 / exact_abs_val_denominator_i)
          )
        {
          if(max_val_i * abs_val_denominator_i < abs_val_numerator_i)
          {
            if((abs_val_numerator_i * 100 / abs_val_denominator_i) >
               (max_val_i * 110 / abs_val_denominator_i)
              )
            {
              continue;
            }
          }
          res =1;
          goto exit;
        }
        if((abs_val_numerator_i * 100 / abs_val_denominator_i) <
           (exact_abs_val_numerator_i * 50 / exact_abs_val_denominator_i)
          )
        {
          res =2;
          goto exit;
        }
      }

    }
  }

exit:
  if(res && !silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("soc_sand_mnt_binary_fraction_test() returned with an error.\n\r")));
  }
  return res;
}

uint32
  soc_sand_integer_arithmetic_test(
    SOC_SAND_IN uint32 silent
  )
{
  uint32
    pass,
    seed;
  uint32
    seconds,
    nano_seconds,
    nof_test,
    test_number,
    answer_1,
    answer_2;
  const char
    *proc_name = "soc_sand_integer_arithmetic_test";

  pass = TRUE;

  if(!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("****%s**** START\n"), proc_name));
  }


  soc_sand_os_get_time(&seconds, &nano_seconds);
  seed = (uint32) (nano_seconds ^ seconds);

  soc_sand_os_srand( seed );


  
  nof_test = 20000;
  while(--nof_test)
  {
    test_number = soc_sand_os_rand();

    answer_1 = soc_sand_msb_bit_on_TEST(test_number);
    answer_2 = soc_sand_msb_bit_on(test_number);

    if( answer_1 != answer_2 )
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** Test Type 1: %s - FAIL\n"), proc_name));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** answer_1 != answer_2, test_number = 0x%X, nof_test=%u, seed=%u \n"), test_number, nof_test, seed));
      pass = FALSE;
      goto exit;
    }
  }

  
  nof_test = 20000;
  while(--nof_test)
  {
    test_number = soc_sand_os_rand();
    test_number >>= (soc_sand_os_rand()%32);

    answer_1 = soc_sand_msb_bit_on_TEST(test_number);
    answer_2 = soc_sand_msb_bit_on(test_number);

    if( answer_1 != answer_2 )
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** Test Type 2: %s - FAIL\n"), proc_name));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** answer_1 != answer_2, test_number = 0x%X, nof_test=%u, seed=%u \n"), test_number, nof_test, seed));
      pass = FALSE;
      goto exit;
    }
  }


exit:
  return pass;
}

void
  soc_sand_print_u_long_binary_format(
    SOC_SAND_IN uint32 x,
    SOC_SAND_IN uint32 max_digits_in_number
  )
{
  uint32
    bit_i;

  for(bit_i=SOC_SAND_MIN(32, max_digits_in_number);bit_i>0;)
  {
    if(x & SOC_SAND_BIT(--bit_i))
    {
      LOG_CLI((BSL_META("1")));
    }
    else
    {
      LOG_CLI((BSL_META("0")));
    }
  }
}


#endif
