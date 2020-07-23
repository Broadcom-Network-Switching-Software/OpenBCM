/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

 




#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>


uint32
  soc_sand_long_to_u64(
    SOC_SAND_IN  uint32      		u32,
    SOC_SAND_OUT SOC_SAND_U64* 		u64
  )
{
  uint32
    result = TRUE;

  if(!u64)
  {
	  result = FALSE;
	  goto exit;
  }

  soc_sand_u64_clear(u64);

  u64->arr[0] = u32;

exit:
  return result;
}


uint32
  soc_sand_u64_copy(
    SOC_SAND_IN  SOC_SAND_U64*      src,
    SOC_SAND_OUT SOC_SAND_U64* 		dst
  )
{
  uint32
	i,
    result = TRUE;

  if(!src || !dst)
  {
	  result = FALSE;
	  goto exit;
  }

  for (i = 0; i < SOC_SAND_U64_NOF_UINT32S; i++) {
	  dst->arr[i] = src->arr[i];
  }

exit:
  return result;
}


void
  soc_sand_u64_clear(
    SOC_SAND_INOUT SOC_SAND_U64* u64
  )
{
  if(NULL == u64)
  {
    goto exit;
  }

  soc_sand_os_memset(
    u64,
    0x0,
    sizeof(*u64)
  );

exit:
  return;
}


uint32
  soc_sand_u64_to_long(
    SOC_SAND_IN  SOC_SAND_U64*      u64,
    SOC_SAND_OUT uint32* ulong
  )
{
  uint32
    overflowed;
  overflowed = FALSE;

  if(NULL == u64)
  {
    goto exit;
  }

  if(u64->arr[1] != 0)
  {
    overflowed = TRUE;
    *ulong     = 0xFFFFFFFF;
  }
  else
  {
    *ulong     = u64->arr[0];
  }

exit:
  return overflowed;
}


void
  soc_sand_u64_multiply_longs(
    SOC_SAND_IN  uint32 x,
    SOC_SAND_IN  uint32 y,
    SOC_SAND_OUT SOC_SAND_U64*     result
  )
{  
  
  uint32
    x0,
    y0,
    x1,
    y1;
  uint32
    t,
    u,
    carry;

  if(NULL == result)
  {
    goto exit;
  }

  

  x0 = SOC_SAND_LOHALF(x);
  x1 = SOC_SAND_HIHALF(x);
  y0 = SOC_SAND_LOHALF(y);
  y1 = SOC_SAND_HIHALF(y);

  
  result->arr[0] = x0 * y0;

  
  t = x0 * y1;
  u = x1 * y0;
  t += u;
  if (t < u)
  {
    carry = 1;
  }
  else
  {
    carry = 0;
  }

  
  carry = SOC_SAND_TOHIGH(carry) + SOC_SAND_HIHALF(t);

  
  t = SOC_SAND_TOHIGH(t);
  result->arr[0] += t;
  if (result->arr[0] < t)
  {
    carry++;
  }

  result->arr[1] = x1 * y1;
  result->arr[1] += carry;

exit:
  return;
}



STATIC void
  soc_sand_u64_device_u64_long_high_inner(
    uint32 uu[2],
    uint32 qhat,
    uint32 v1,
    uint32 v0
  )
{
  
  uint32 p0, p1, t;

  p0 = qhat * v0;
  p1 = qhat * v1;
  t = p0 + SOC_SAND_TOHIGH(SOC_SAND_LOHALF(p1));
  uu[0] -= t;
  if (uu[0] > 0xffffffff - t)
    uu[1]--;  
  uu[1] -= SOC_SAND_HIHALF(p1);
}



STATIC uint32
  soc_sand_u64_devide_u64_long_high(
    SOC_SAND_IN  SOC_SAND_U64*      x,
    SOC_SAND_IN  uint32  y,
    SOC_SAND_OUT uint32* result,
    SOC_SAND_OUT uint32* remainder
  )
{  
  uint32
    qhat,
    rhat,
    t,
    v0,
    v1,
    u0,
    u1,
    u2,
    u3,
    B;
  uint32
    uu[2],
    q2;
  uint32
    overflowed;

  overflowed = FALSE;
  B  = (0xffff + 1);

  if( (NULL == result) ||
      (NULL == remainder)
    )
  {
    goto exit;
  }

  
  if (!(y & SOC_SAND_HIBITMASK))
  {
    *result = *remainder = 0;
    overflowed = TRUE;
    goto exit;
  }

  
  v0 = SOC_SAND_LOHALF(y);
  v1 = SOC_SAND_HIHALF(y);
  u0 = SOC_SAND_LOHALF(x->arr[0]);
  u1 = SOC_SAND_HIHALF(x->arr[0]);
  u2 = SOC_SAND_LOHALF(x->arr[1]);
  u3 = SOC_SAND_HIHALF(x->arr[1]);

  

  
  
  qhat = u3 / v1;
  if (qhat > 0)
  {
    rhat = u3 - qhat * v1;
    t = SOC_SAND_TOHIGH(rhat) | u2;
    if (qhat * v0 > t)
    {
      qhat--;
    }
  }
  uu[1] = 0;    
  uu[0] = x->arr[1];  
  if (qhat > 0)
  {
    
    soc_sand_u64_device_u64_long_high_inner(uu, qhat, v1, v0);
    if (SOC_SAND_HIHALF(uu[1]) != 0)
    {  
      qhat--;
      uu[0] += y;
      uu[1] = 0;
    }
  }
  q2 = qhat;

  
  
  t = uu[0];
  qhat = t / v1;
  rhat = t - qhat * v1;
  
  t = SOC_SAND_TOHIGH(rhat) | u1;
  if ((qhat == B) || (qhat * v0 > t))
  {
    qhat--;
    rhat += v1;
    t = SOC_SAND_TOHIGH(rhat) | u1;
    if ((rhat < B) && (qhat * v0 > t))
      qhat--;
  }

  
  uu[1] = SOC_SAND_HIHALF(uu[0]);  
  uu[0] = SOC_SAND_TOHIGH(SOC_SAND_LOHALF(uu[0])) | u1;  
  soc_sand_u64_device_u64_long_high_inner(uu, qhat, v1, v0);
  if (SOC_SAND_HIHALF(uu[1]) != 0)
  {  
    qhat--;
    uu[0] += y;
    uu[1] = 0;
  }

  
  *result = SOC_SAND_TOHIGH(qhat);

  
  
  t = uu[0];
  qhat = t / v1;
  rhat = t - qhat * v1;
  
  t = SOC_SAND_TOHIGH(rhat) | u0;
  if ((qhat == B) || (qhat * v0 > t))
  {
    qhat--;
    rhat += v1;
    t = SOC_SAND_TOHIGH(rhat) | u0;
    if ((rhat < B) && (qhat * v0 > t))
      qhat--;
  }

  
  uu[1] = SOC_SAND_HIHALF(uu[0]);  
  uu[0] = SOC_SAND_TOHIGH(SOC_SAND_LOHALF(uu[0])) | u0;  
  soc_sand_u64_device_u64_long_high_inner(uu, qhat, v1, v0);
  if (SOC_SAND_HIHALF(uu[1]) != 0)
  {  
    qhat--;
    uu[0] += y;
    uu[1] = 0;
  }

  
  *result |= SOC_SAND_LOHALF(qhat);

  
  *remainder = uu[0];

  overflowed = q2;
exit:
  return overflowed;
}



uint32
  soc_sand_u64_shift_left(
    SOC_SAND_OUT SOC_SAND_U64*    a,
    SOC_SAND_IN  SOC_SAND_U64*    b,
    SOC_SAND_IN  uint32 x
  )
{
  uint32
    i,
    y;
  uint32
    mask,
    carry,
    nextcarry;

  carry = 0;

  if( (NULL == a) ||
      (NULL == b)
    )
  {
    goto exit;
  }


  
  if (x >= SOC_SAND_NOF_BITS_IN_UINT32)
    return 0;

  
  mask = SOC_SAND_HIBITMASK;
  for (i = 1; i < x; i++)
  {
    mask = (mask >> 1) | mask;
  }
  if (x == 0) {
    *a = *b;
    goto exit;
  }

  y = SOC_SAND_NOF_BITS_IN_UINT32 - x;
  carry = 0;
  for (i = 0; i < SOC_SAND_U64_NOF_UINT32S; i++)
  {
    nextcarry = (b->arr[i] & mask) >> y;
    a->arr[i] = b->arr[i] << x | carry;
    carry = nextcarry;
  }

exit:
  return carry;
}


uint32
  soc_sand_u64_devide_u64_long(
    SOC_SAND_IN  SOC_SAND_U64*     x,
    SOC_SAND_IN  uint32 y,
    SOC_SAND_OUT SOC_SAND_U64*     result
  )
{
  
  int
    j;
  SOC_SAND_U64
    t;
  uint32
    r;
  uint32
    shift;
  uint32
    bitmask,
    overflow,
    *uu,
    ndigits,
    local_y;



  local_y = y;
  ndigits = SOC_SAND_U64_NOF_UINT32S;

  if (local_y == 0)
  {
    return 0; 
  }


  
  
  bitmask = SOC_SAND_HIBITMASK;
  for (shift = 0; shift < SOC_SAND_NOF_BITS_IN_UINT32; shift++)
  {
    if (local_y & bitmask)
    {
      break;
    }
    bitmask >>= 1;
  }

  
  
  local_y <<= shift;
  overflow = soc_sand_u64_shift_left(result, x, shift);
  uu = result->arr;


  
  r = overflow;  
  j = ndigits;

  for( j=ndigits-1; j >= 0; j--)
  {
    
    t.arr[1] = r;
    t.arr[0] = uu[j];
    overflow = soc_sand_u64_devide_u64_long_high( &t, local_y, &result->arr[j], &r);
  }

  
  r >>= shift;


  return r;
}



uint32
  soc_sand_u64_add_long(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     uint32 y
  )
{
  SOC_SAND_U64
    y_u64;
  uint32
    overflowed;

  soc_sand_u64_clear(&y_u64);
  y_u64.arr[0] = y;

  overflowed = soc_sand_u64_add_u64(x, &y_u64);

  return overflowed;
}


uint32
  soc_sand_u64_add_u64(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  )
{
  uint32
    old_val_low,
    old_val_high,
    overflowed;
  uint32
    overflow_low,
    overflow_high;

  overflow_low  = FALSE;
  overflow_high = FALSE;
  old_val_low   = 0;
  old_val_high  = 0;
  overflowed    = FALSE;

  if( (NULL == x)       ||
      (NULL == y)
    )
  {
    goto exit;
  }

  
  old_val_low = x->arr[0];
  x->arr[0] += y->arr[0];
  if(x->arr[0] < old_val_low)
  {
    overflow_low = TRUE;
  }

  
  old_val_high = x->arr[1];
  x->arr[1] += y->arr[1];
  if(overflow_low)
  {
    x->arr[1]++;
  }

  if(x->arr[1] < old_val_high)
  {
    overflow_high = TRUE;
  }

  
  if( TRUE == overflow_high )
  {
    overflowed = TRUE;
  }

exit:
  return overflowed;
}



uint32
  soc_sand_u64_subtract_u64(
    SOC_SAND_INOUT  SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  )
{
  uint32
    old_val_low,
    old_val_high;
  uint32
    overflow_low,
    overflow_high,
    pass;

  overflow_low  = FALSE;
  overflow_high = FALSE;
  old_val_low   = 0;
  old_val_high  = 0;
  pass        = TRUE;

  if( (NULL == x)       ||
      (NULL == y)
    )
  {
    goto exit;
  }

  
  old_val_low = x->arr[0];
  x->arr[0] -= y->arr[0];
  if(x->arr[0] > old_val_low)
  {
    overflow_low = TRUE;
  }

  
  old_val_high = x->arr[1];
  x->arr[1] -= y->arr[1];
  if(overflow_low)
  {
    x->arr[1]--;
  }

  if(x->arr[1] > old_val_high)
  {
    overflow_high = TRUE;
  }

  
  if( TRUE == overflow_high )
  {
    pass = FALSE;
    x->arr[0] = x->arr[1] = 0;
  }

exit:
  return pass;
}


uint32
  soc_sand_u64_is_bigger(
    SOC_SAND_IN     SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  )
{
  uint32
    bigger;

  bigger = FALSE;

  if( (NULL == x)       ||
      (NULL == y)
    )
  {
    goto exit;
  }

  if( x->arr[1] >  y->arr[1] )
  {
    bigger = TRUE;
  }
  else if( x->arr[1] ==  y->arr[1] )
  {
    if( x->arr[0] >  y->arr[0] )
    {
      bigger = TRUE;
    }
  }

exit:
  return bigger;
}


uint32
  soc_sand_u64_is_zero(
    SOC_SAND_IN     SOC_SAND_U64*     x
  )
{
  uint32
    is_zero;

  is_zero = FALSE;

  if( (NULL == x)  )
  {
    goto exit;
  }

  if( ( x->arr[0] == 0 ) &&
      ( x->arr[1] == 0 )
    )
  {
    is_zero = TRUE;
  }

exit:
  return is_zero;
}


uint32
  soc_sand_u64_are_equal(
    SOC_SAND_IN     SOC_SAND_U64*     x,
    SOC_SAND_IN     SOC_SAND_U64*     y
  )
{
  uint32
    are_equal;

  are_equal = FALSE;

  if( (NULL == x) ||  (NULL == y))
  {
    goto exit;
  }

  if( ( x->arr[0] ==  y->arr[0] ) &&
      ( x->arr[1] ==  y->arr[1] )
    )
  {
    are_equal = TRUE;
  }

exit:
  return are_equal;
}

uint32
  soc_sand_u64_log2_round_up(
    SOC_SAND_IN SOC_SAND_U64 *x
  )
{
  uint32
    msb_bit;
  SOC_SAND_U64
    calc;
  soc_sand_u64_clear(&calc);

  if (x->arr[1]==0)
  {
    msb_bit = soc_sand_msb_bit_on(x->arr[0]);
    calc.arr[0] = 1u<<msb_bit;
    calc.arr[1] = 0;
  }
  else
  {
    msb_bit = soc_sand_msb_bit_on(x->arr[1]);
    calc.arr[0] = 0;
    calc.arr[1] = 1u<<msb_bit;
    msb_bit += sizeof(x->arr[0])*8; 
  }

  if ( soc_sand_u64_is_bigger(x, &calc) )
  {
    
    msb_bit++;
  }

  return msb_bit;
}


#if SOC_SAND_DEBUG




void
  soc_sand_u64_print(
    SOC_SAND_IN SOC_SAND_U64*    u64,
    SOC_SAND_IN uint32 print_type,
    SOC_SAND_IN uint32 short_format
  )
{

  if(NULL == u64)
  {
    LOG_CLI((BSL_META("soc_sand_u64_print received NULL ptr\n\r")));
    goto exit;
  }

  if(print_type == 0)
  {
    
    if(short_format)
    {
      if(0 != u64->arr[1])
      {
        LOG_CLI((BSL_META("%u * 2^32 + "),
                 u64->arr[1]
                 ));
      }
      LOG_CLI((BSL_META("%u"),
               u64->arr[0]
               ));
    }
    else
    {
      LOG_CLI((BSL_META("arr[1]: %u, arr[0]: %u\n\r"),
               u64->arr[1],
               u64->arr[0]
               ));
    }
  }
  else
  {
    
    if(short_format)
    {
      if(0 != u64->arr[1])
      {
        LOG_CLI((BSL_META("0x%X "),
                 u64->arr[1]
                 ));
      }
      LOG_CLI((BSL_META("0x%X"),
               u64->arr[0]
               ));
    }
    else
    {
      LOG_CLI((BSL_META("arr[1]: 0x%8X, arr[0]: 0x%8X\n\r"),
               u64->arr[1],
               u64->arr[0]
               ));
    }
  }

exit:
  return;
}



uint32
  soc_sand_u64_test(uint32 silent)
{
  uint32
    pass,
    seed;
  SOC_SAND_U64
    u64_1,
    u64_2,
    u64_3;
  uint32
    seconds,
    nano_seconds,
    x,
    y,
    r,
    nof_test;

#if defined(_MSC_VER)
  
  unsigned __int64
    win_u64_1,
    win_u64_2,
    win_u64_3;
#endif

  pass = TRUE;

  if(!silent)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("****soc_sand_u64_test**** START\n")));
  }

  soc_sand_u64_clear(&u64_1);
  soc_sand_u64_clear(&u64_2);

  soc_sand_os_get_time(&seconds, &nano_seconds);
  seed = (uint32) (nano_seconds ^ seconds);

  soc_sand_os_srand( seed );
  nof_test = 20000;


  
  while(--nof_test)
  {
    x = soc_sand_os_rand();
    y = soc_sand_os_rand();
    if(y==0)
    {
      y = 1;
    }
    soc_sand_u64_multiply_longs(x, y, &u64_1);
#if defined(_MSC_VER)
    win_u64_1 = (__int64)x*y;
    if( ( (win_u64_1&0xFFFFFFFF) != u64_1.arr[0]) ||
        ( (win_u64_1>>32) != u64_1.arr[1])
      )
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** Test Type 1: soc_sand_u64_test - FAIL\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** win_u64_1!=u64_1, nof_test=%u, seed=%u \n"),nof_test, seed));
      pass = FALSE;
      goto exit;
    }
#endif


    r = soc_sand_u64_devide_u64_long(&u64_1, y, &u64_2);
#if defined(_MSC_VER)
    win_u64_2 = win_u64_1/y;
    if( ( (win_u64_2&0xFFFFFFFF) != u64_2.arr[0]) ||
        ( (win_u64_2>>32) != u64_2.arr[1])
      )
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** Test Type 1: soc_sand_u64_test - FAIL\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** win_u64_2!=u64_2, nof_test=%u, seed=%u \n"),nof_test, seed));
      pass = FALSE;
      goto exit;
    }
#endif

    if( r!= 0)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** soc_sand_u64_test - FAIL\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** r!=0, nof_test=%u, seed=%u \n"),nof_test, seed));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** u64_2=u64_1/y=")));
      soc_sand_u64_print(&u64_2, 1, 1);
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META(" <=Check u64_2 == x\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** r=u_64_1 mod y=0x%08X <=Check r == 0\n"), r));
      pass = FALSE;
      goto exit;
    }
    if( (u64_2.arr[0] != x) ||
        (u64_2.arr[1] != 0)
      )
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** Test Type 1: soc_sand_u64_test - FAIL\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** x = %u, y = %u,\n\r"), x, y));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** u64_1 = ")));
      soc_sand_u64_print(&u64_1, 0, TRUE);
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("\n\r")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** u64_2 = ")));
      soc_sand_u64_print(&u64_2, 0, TRUE);
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("\n\r")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** (u64_2.arr[0] != x).., nof_test=%u, seed=%u \n"),nof_test, seed));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** u64_2=u64_1/y=")));
      soc_sand_u64_print(&u64_2, 1, 1);
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META(" <=Check u64_2 == x\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** r=u_64_1 mod y=0x%08X <=Check r == 0\n"), r));
      pass = FALSE;
      goto exit;
    }
  }

  nof_test = 20000;

  
  while(--nof_test)
  {
    x = soc_sand_os_rand();
    if(x==0)
    {
      x = 1;
    }
    u64_1.arr[0] = soc_sand_os_rand();
    u64_1.arr[1] = soc_sand_os_rand();

    r = soc_sand_u64_devide_u64_long(&u64_1, x, &u64_2);
#if defined(_MSC_VER)
    win_u64_1 = u64_1.arr[1];
    win_u64_1 <<= 32;
    win_u64_1 |= u64_1.arr[0];
    win_u64_2 = win_u64_1/x;
    if( ( (win_u64_2&0xFFFFFFFF) != u64_2.arr[0]) ||
        ( (win_u64_2>>32) != u64_2.arr[1])
      )
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** Test Type 2: soc_sand_u64_test - FAIL\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** win_u64_2!=u64_2, nof_test=%u, seed=%u \n"),nof_test, seed));
      pass = FALSE;
      goto exit;
    }
#endif


    if(u64_2.arr[1] != 0)
    {
      continue;
    }


    soc_sand_u64_to_long(&u64_2, &y);
    soc_sand_u64_multiply_longs(x, y, &u64_3);
#if defined(_MSC_VER)
    win_u64_3 = (__int64)x*win_u64_2;
    if( ( (win_u64_3&0xFFFFFFFF) != u64_3.arr[0]) ||
        ( (win_u64_3>>32) != u64_3.arr[1])
      )
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** Test Type 2: soc_sand_u64_test - FAIL\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** win_u64_1!=u64_1, nof_test=%u, seed=%u \n"),nof_test, seed));
      pass = FALSE;
      goto exit;
    }
#endif

    soc_sand_u64_add_long(&u64_3, r);

    if( (u64_3.arr[0] != u64_1.arr[0]) ||
        (u64_3.arr[1] != u64_1.arr[1])
      )
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** soc_sand_u64_test - FAIL\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** (u64_3.arr[0] != u64_1.arr[0]).., nof_test=%u, seed=%u \n"),nof_test, seed));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** u64_2=u64_1/y=")));
      soc_sand_u64_print(&u64_2, 1, 1);
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META(" <=Check u64_2 == x\n")));
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("**** r=u_64_1 mod y=0x%08X <=Check r == 0\n"), r));
      pass = FALSE;
      goto exit;
    }
  }

  u64_1.arr[1] = 5;
  u64_1.arr[0] = 4;
  u64_2.arr[1] = 4;
  u64_2.arr[0] = 5;
  u64_3.arr[1] = 0;
  u64_3.arr[0] = (uint32)-1;

  pass = soc_sand_u64_subtract_u64(&u64_1, &u64_2);
  if(soc_sand_u64_is_bigger(&u64_1, &u64_3) ||
     soc_sand_u64_is_bigger(&u64_3, &u64_1) ||
     !pass
    )
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("soc_sand_u64_subtract_u64 failed(1).\n\r")));
    pass = FALSE;
    goto exit;
  }
  u64_1.arr[1] = 5;
  u64_1.arr[0] = 4;
  u64_2.arr[1] = 4;
  u64_2.arr[0] = 5;

  pass = soc_sand_u64_subtract_u64(&u64_2, &u64_1);
  if(!soc_sand_u64_is_zero(&u64_2))
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("soc_sand_u64_subtract_u64 failed(2).\n\r")));
    pass = FALSE;
    goto exit;
  }
  if(!pass)
  {
    pass = TRUE;
  }
  else
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("soc_sand_u64_subtract_u64 failed(3).\n\r")));
    pass = FALSE;
    goto exit;
  }


exit:
  return pass;
}


#endif


