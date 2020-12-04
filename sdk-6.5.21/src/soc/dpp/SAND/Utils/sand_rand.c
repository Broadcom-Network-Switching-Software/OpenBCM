/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/





#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_rand.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>




#define SOC_SAND_RAND_UMASK 0x80000000UL 
#define SOC_SAND_RAND_LMASK 0x7fffffffUL 
#define SOC_SAND_RAND_MIXBITS(u,v) ( ((u) & SOC_SAND_RAND_UMASK) | ((v) & SOC_SAND_RAND_LMASK) )
#define SOC_SAND_RAND_TWIST(u,v) ((SOC_SAND_RAND_MIXBITS(u,v) >> 1) ^ ((v)&1UL ? SOC_SAND_RAND_MATRIX_A : 0UL))

#define SOC_SAND_RAND_INIT_INDICATOR 0x12FF87A5




STATIC void
  soc_sand_rand_init(
    SOC_SAND_INOUT SOC_SAND_RAND*    r,
    SOC_SAND_IN    uint32 s
  )
{
  int
    j;

  if(NULL == r)
  {
    goto exit;
  }

  r->state[0]= s & 0xffffffffUL;
  for (j=1; j<SOC_SAND_RAND_N; j++)
  {
    r->state[j] = (1812433253UL * (r->state[j-1] ^ (r->state[j-1] >> 30)) + j);
    r->state[j] &= 0xffffffffUL;
  }
  r->left = 1;
  r->initf = SOC_SAND_RAND_INIT_INDICATOR;

exit:
  return;
}



STATIC void
  soc_sand_rand_check_init(
    SOC_SAND_INOUT SOC_SAND_RAND*    r
  )
{
  if (r->initf!=SOC_SAND_RAND_INIT_INDICATOR)
  {
    soc_sand_rand_init(r, 5489UL);
  }
}

STATIC void
  soc_sand_rand_next_state(
    SOC_SAND_INOUT SOC_SAND_RAND *r
  )
{
  uint32
    *p=NULL;
  int
    j;

  if(NULL == r)
  {
    goto exit;
  }

  soc_sand_rand_check_init(r);

  p=r->state;


  r->left = SOC_SAND_RAND_N;
  r->next = r->state;

  for (j=SOC_SAND_RAND_N-SOC_SAND_RAND_M+1; --j; p++)
  {
    *p = p[SOC_SAND_RAND_M] ^ SOC_SAND_RAND_TWIST(p[0], p[1]);
  }

  for (j=SOC_SAND_RAND_M; --j; p++)
  {
    *p = p[SOC_SAND_RAND_M-SOC_SAND_RAND_N] ^ SOC_SAND_RAND_TWIST(p[0], p[1]);
  }

  *p = p[SOC_SAND_RAND_M-SOC_SAND_RAND_N] ^ SOC_SAND_RAND_TWIST(p[0], r->state[0]);
exit:
  return;
}


STATIC uint32
  soc_sand_rand_genrand_int32(
    SOC_SAND_INOUT SOC_SAND_RAND *r
  )
{
  uint32
    y = 0;

  if(NULL == r)
  {
    goto exit;
  }

  soc_sand_rand_check_init(r);

  if (--(r->left) == 0)
  {
    soc_sand_rand_next_state(r);
  }
  y = *(r->next)++;

  
  y ^= (y >> SOC_SAND_RAND_TEMU);
  y ^= (y << SOC_SAND_RAND_TEMS) & SOC_SAND_RAND_TEMB;
  y ^= (y << SOC_SAND_RAND_TEMT) & SOC_SAND_RAND_TEMC;
  y ^= (y >> SOC_SAND_RAND_TEML);

exit:
  return y;
}


STATIC uint32
  soc_sand_rand_genrand_int31(
    SOC_SAND_INOUT SOC_SAND_RAND *r
  )
{
  uint32
    y = 0;

  if(NULL == r)
  {
    goto exit;
  }

  soc_sand_rand_check_init(r);

  if (--(r->left) == 0)
  {
    soc_sand_rand_next_state(r);
  }
  y = *(r->next)++;

  
  y ^= (y >> SOC_SAND_RAND_TEMU);
  y ^= (y << SOC_SAND_RAND_TEMS) & SOC_SAND_RAND_TEMB;
  y ^= (y << SOC_SAND_RAND_TEMT) & SOC_SAND_RAND_TEMC;
  y ^= (y >> SOC_SAND_RAND_TEML);

exit:
  return y>>1;
}







void
  soc_sand_rand_seed_set(
    SOC_SAND_OUT SOC_SAND_RAND     *r,
    SOC_SAND_IN  uint32 seed
  )
{
  if(NULL == r)
  {
    goto exit;
  }

  r->seed = seed;
  soc_sand_rand_init(r, seed);

exit:
  return;
}


uint32
  soc_sand_rand_seed_get(
    SOC_SAND_IN SOC_SAND_RAND    *r
  )
{
  uint32
    seed = 0;
  if(NULL == r)
  {
    goto exit;
  }
  seed = r->seed;

exit:
  return seed;
}



uint32
  soc_sand_rand_coin_1(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_IN    uint32 positive_bias
  )
{
  uint32
    x = 0;
  if(NULL == r)
  {
    goto exit;
  }

  if(positive_bias >= SOC_SAND_RAND_PERCENT_100)
  {
    x=1;
  }
  else
  {
    if(soc_sand_rand_modulo(r, SOC_SAND_RAND_PERCENT_100) <= positive_bias)
    {
      x=1;
    }
    else
    {
      x = 0;
    }
  }

exit:
  return x;
}



uint32
  soc_sand_rand_coin_2(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_IN    uint32 positive_bias,
    SOC_SAND_IN    uint32 negative_bias
  )
{
  uint32
    x = 0;
  if(NULL == r)
  {
    goto exit;
  }

  if(soc_sand_rand_modulo(r, positive_bias+negative_bias) <= positive_bias)
  {
    x=1;
  }
  else
  {
    x = 0;
  }

exit:
  return x;
}



uint32
  soc_sand_rand_range(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_IN    uint32 min,
    SOC_SAND_IN    uint32 max
  )
{
  uint32
    x = 0;
  if(NULL == r)
  {
    goto exit;
  }

  if(min >= max)
  {
    x = min;
  }
  else
  {
    x = min + soc_sand_rand_modulo(r, max-min);
  }

exit:
  return x;
}



int
  soc_sand_rand_int(
    SOC_SAND_INOUT SOC_SAND_RAND     *r
  )
{
  uint32
    x = 0;
  if(NULL == r)
  {
    goto exit;
  }

  x = soc_sand_rand_genrand_int31(r);


exit:
  return x;
}



uint32
  soc_sand_rand_u_long(
    SOC_SAND_INOUT SOC_SAND_RAND     *r
  )
{
  uint32
    x = 0;
  if(NULL == r)
  {
    goto exit;
  }

  x = soc_sand_rand_genrand_int32(r);


exit:
  return x;
}



uint32
  soc_sand_rand_modulo(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_IN    uint32 m
  )
{
  uint32
    x = 0;
  if(NULL == r)
  {
    goto exit;
  }

  x = soc_sand_rand_genrand_int32(r) % m;

exit:
  return x;
}









STATIC void
  soc_sand_rand_array_permute_u_long_swap(
    unsigned char arr_char[],
    uint32 indx_1,
    uint32 indx_2
  )
{
  uint32
    temp_val;
  uint32
    *array = (uint32*)arr_char;

  temp_val      = array[indx_1];
  array[indx_1] = array[indx_2];
  array[indx_2] = temp_val;
}


void
  soc_sand_rand_array_permute_u_long(
    SOC_SAND_INOUT SOC_SAND_RAND     *r,
    SOC_SAND_INOUT uint32 array[],
    SOC_SAND_IN    uint32 nof_items
  )
{
  if( (NULL == r) ||
      (NULL == array)
    )
  {
    goto exit;
  }

  soc_sand_rand_array_permute(
    r,
    (unsigned char*)array,
    nof_items,
    soc_sand_rand_array_permute_u_long_swap
  );

exit:
  return;
}



void
  soc_sand_rand_array_permute(
    SOC_SAND_INOUT SOC_SAND_RAND               *r,
    SOC_SAND_INOUT unsigned char           array[],
    SOC_SAND_IN    uint32           nof_items,
    SOC_SAND_IN    SOC_SAND_RAND_SWAP_ARR_FUNC swap
  )
{
  uint32
    arr_end;
  uint32
    indx_to_switch;

  if( (NULL == r) ||
      (NULL == array) ||
      (NULL == swap)
    )
  {
    goto exit;
  }

  for(arr_end=nof_items; arr_end>1;)
  {
    indx_to_switch = soc_sand_rand_modulo(r, arr_end--);
    (*swap)(array, arr_end, indx_to_switch);
  }

exit:
  return;
}





#if SOC_SAND_DEBUG




void
  soc_sand_rand_print(
    SOC_SAND_IN SOC_SAND_RAND    *r
  )
{
  if(NULL == r)
  {
    LOG_CLI((BSL_META("ERROR: soc_sand_rand_print - got NULL -\n\r")));
    goto exit;
  }
  LOG_CLI((BSL_META("%u"),
           soc_sand_rand_seed_get(r)
           ));

exit:
  return;
}



int soc_sand_rand_test(SOC_SAND_IN uint32 silent)
{
  int
    i,
    pass;
  uint32
    seed=0x65464;
  SOC_SAND_RAND
    soc_sand_rand;
#define SOC_SAND_RAND_TEST_ARR_SIZE 200
  uint32
    array[SOC_SAND_RAND_TEST_ARR_SIZE];

  pass = TRUE;

  soc_sand_rand_seed_set(&soc_sand_rand, seed);

  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("1000 outputs of soc_sand_rand_u_long()\n")));
  for (i=0; i<1000; i++)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("%10u "), soc_sand_rand_u_long(&soc_sand_rand)));
    if (i%5==4)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("\n")));
    }
  }

  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("1000 outputs of soc_sand_rand_coin_1(, 50%%)\n")));
  for (i=0; i<1000; i++)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("%1u"), soc_sand_rand_coin_1(&soc_sand_rand, 50*SOC_SAND_RAND_PERCENT_ONE)));
    if (i%40==39)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("\n")));
    }
    else
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META(" ")));
    }
  }

  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("1000 outputs of soc_sand_rand_coin_2(, 1%%, 10%%)\n")));
  for (i=0; i<1000; i++)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("%1u"), soc_sand_rand_coin_2(&soc_sand_rand, 1, 10)));
    if (i%40==39)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("\n")));
    }
    else
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META(" ")));
    }
  }

  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("2 outputs of soc_sand_rand_array_permute_u_long()\n")));
  for (i=0; i<SOC_SAND_RAND_TEST_ARR_SIZE; i++)
  {
    array[i] = i;
  }

  soc_sand_rand_array_permute_u_long(&soc_sand_rand, array, SOC_SAND_RAND_TEST_ARR_SIZE);
  for (i=0; i<SOC_SAND_RAND_TEST_ARR_SIZE; i++)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("%3u"), array[i]));
    if (i%20==19)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("\n")));
    }
    else
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META(" ")));
    }
  }

  LOG_INFO(BSL_LS_SOC_COMMON,
           (BSL_META("\n")));
  soc_sand_rand_array_permute_u_long(&soc_sand_rand, array, SOC_SAND_RAND_TEST_ARR_SIZE);
  for (i=0; i<SOC_SAND_RAND_TEST_ARR_SIZE; i++)
  {
    LOG_INFO(BSL_LS_SOC_COMMON,
             (BSL_META("%3u"), array[i]));
    if (i%20==19)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("\n")));
    }
    else
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META(" ")));
    }
  }



  return pass;
}


#endif


