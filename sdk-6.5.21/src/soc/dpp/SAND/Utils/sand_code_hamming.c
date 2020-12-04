/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_header.h>



#include <soc/dpp/SAND/Utils/sand_code_hamming.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>




























SOC_SAND_RET
  soc_sand_code_hamming_check_data_no_check(
    SOC_SAND_INOUT uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  data_nof_longs,
    SOC_SAND_IN    uint32  rotated_generation_matrix[],
    SOC_SAND_IN    uint32  generation_matrix_bit_search[],
    SOC_SAND_IN    uint32  p_bit_wide,
    SOC_SAND_IN    uint32  e,
    SOC_SAND_INOUT uint32  work_data[],
    SOC_SAND_OUT   uint32* number_of_fixed_errors
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  uint32
    p,
    syndrom,
    bit_to_flip;

  soc_sand_ret = soc_sand_code_hamming_get_p_no_check(
               data,
               data_bit_wide,
               data_nof_longs,
               rotated_generation_matrix,
               p_bit_wide,
               work_data,
               &p
             );
  if(soc_sand_ret)
  {
    goto exit;
  }

  syndrom = p ^ e;
  if(syndrom == 0)
  {
    *number_of_fixed_errors = 0;
    goto exit;
  }

  
  bit_to_flip = generation_matrix_bit_search[syndrom];
  if(bit_to_flip == 0xFFFFFFFF)
  {
    
  }
  else
  {
    
    soc_sand_bitstream_bit_flip(data, bit_to_flip);
  }
  *number_of_fixed_errors = 1;


exit:
  return soc_sand_ret;
}

SOC_SAND_RET
  soc_sand_code_hamming_check_data(
    SOC_SAND_INOUT uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  rotated_generation_matrix[],
    SOC_SAND_IN    uint32  generation_matrix_bit_search[],
    SOC_SAND_IN    uint32  p_bit_wide,
    SOC_SAND_IN    uint32  e,
    SOC_SAND_OUT   uint32* number_of_fixed_errors
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  uint32
    p_local_size,
    data_nof_longs,
    *local_buff = NULL;

  data_nof_longs = SOC_SAND_DIV_ROUND_UP(data_bit_wide, SOC_SAND_NOF_BITS_IN_UINT32);
  local_buff = (uint32*) soc_sand_os_malloc(data_nof_longs * sizeof(uint32));

  soc_sand_code_hamming_get_p_bit_wide(data_bit_wide, &p_local_size);
  if(p_bit_wide != p_local_size)
  {
    soc_sand_ret = SOC_SAND_CODE_HAMMING_P_BIT_WIDE_UN_MATCH_ERR;
    goto exit;
  }

  if( (NULL == data)                         ||
      (NULL == rotated_generation_matrix)    ||
      (NULL == generation_matrix_bit_search) ||
      (NULL == number_of_fixed_errors)
    )
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    goto exit;
  }

  soc_sand_ret = soc_sand_code_hamming_check_data_no_check(
               data,
               data_bit_wide,
               data_nof_longs,
               rotated_generation_matrix,
               generation_matrix_bit_search,
               p_bit_wide,
               e,
               local_buff,
               number_of_fixed_errors
             );
  if(soc_sand_ret)
  {
    goto exit;
  }

exit:
  SOC_SAND_FREE(local_buff);
  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_code_hamming_get_p(
    SOC_SAND_IN  uint32  data[],
    SOC_SAND_IN  uint32  data_bit_wide,
    SOC_SAND_IN  uint32  rotated_generation_matrix[],
    SOC_SAND_IN  uint32  p_bit_wide,
    SOC_SAND_OUT uint32* p
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  uint32
    p_local = 0,
    data_nof_longs,
    *local_buff = NULL;

  data_nof_longs = SOC_SAND_DIV_ROUND_UP(data_bit_wide, SOC_SAND_NOF_BITS_IN_UINT32);
  local_buff = (uint32*) soc_sand_os_malloc(data_nof_longs * sizeof(uint32));

  soc_sand_code_hamming_get_p_bit_wide(data_bit_wide, &p_local);
  if(p_bit_wide != p_local)
  {
    soc_sand_ret = SOC_SAND_CODE_HAMMING_P_BIT_WIDE_UN_MATCH_ERR;
    goto exit;
  }

  if( (NULL == data)  ||
      (NULL == rotated_generation_matrix) ||
      (NULL == p)
    )
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    goto exit;
  }

  soc_sand_ret = soc_sand_code_hamming_get_p_no_check(
               data,
               data_bit_wide,
               data_nof_longs,
               rotated_generation_matrix,
               p_bit_wide,
               local_buff,
               p
             );
  if(soc_sand_ret)
  {
    goto exit;
  }

exit:
  SOC_SAND_FREE(local_buff);
  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_code_hamming_get_p_no_check(
    SOC_SAND_IN    uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  data_nof_longs,
    SOC_SAND_IN    uint32  rotated_generation_matrix[],
    SOC_SAND_IN    uint32  p_bit_wide,
    SOC_SAND_INOUT uint32  work_data[],
    SOC_SAND_OUT   uint32* p
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  uint32
    p_i,
    nof_ones;
  const uint32
    *generation_matrix_local= NULL;

  generation_matrix_local = rotated_generation_matrix;
  *p=0;

  for(p_i=0; p_i<p_bit_wide; ++p_i)
  {
    soc_sand_os_memcpy(work_data, data, data_nof_longs * sizeof(uint32));
    soc_sand_bitstream_and(work_data, generation_matrix_local, data_nof_longs);
    nof_ones = soc_sand_bitstream_get_nof_on_bits(work_data, data_nof_longs);
    if(soc_sand_is_even(nof_ones) == FALSE)
    {
      *p |= SOC_SAND_BIT(p_i);
    }

    generation_matrix_local+=data_nof_longs;
  }

  return soc_sand_ret;
}


SOC_SAND_RET
  soc_sand_code_hamming_get_p_bit_wide(
    SOC_SAND_IN  uint32  data_bit_wide,
    SOC_SAND_OUT uint32* p_bit_wide
  )
{
  uint32
    p_local,
    two_power_m_local;
  uint32
    found = FALSE;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

  
  for(p_local=1; p_local<31; ++p_local)
  {
    two_power_m_local = 2 << p_local;

    if( two_power_m_local >= (data_bit_wide + p_local + 1) )
    {
      found = TRUE;
      break;
    }
  }

  if(found)
  {
    *p_bit_wide = p_local+1;
    soc_sand_ret = SOC_SAND_OK;
  }
  else
  {
    soc_sand_ret = SOC_SAND_CODE_HAMMING_UN_SUPPORTED_DATA_BIT_WIDE_ERR;
  }

  return soc_sand_ret;
}



SOC_SAND_RET
  soc_sand_code_hamming_generate_gen_matrix(
    SOC_SAND_IN  uint32 data_bit_wide,
    SOC_SAND_IN  uint32 search_gen_mat_nof_entires,
    SOC_SAND_OUT uint32 gen_mat[],
    SOC_SAND_OUT uint32 search_gen_mat[]
  )
{
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;
  uint32
    not_power_of_2,
    bit_i;

  if(gen_mat == NULL)
  {
    soc_sand_ret = SOC_SAND_NULL_POINTER_ERR;
    goto exit;
  }

  for(bit_i=0; bit_i<search_gen_mat_nof_entires; ++bit_i)
  {
    search_gen_mat[bit_i] = 0xFFFFFFFF;
  }

  
  not_power_of_2 = 3;
  for(bit_i=0; bit_i<data_bit_wide; ++bit_i)
  {
    while( soc_sand_is_power_of_2(not_power_of_2) == TRUE )
    {
      not_power_of_2++;
    }
    gen_mat[bit_i] = not_power_of_2;
    search_gen_mat[not_power_of_2] = bit_i;
    not_power_of_2++;
  }

exit:
  return soc_sand_ret;
}









#if SOC_SAND_DEBUG




uint32
  soc_sand_code_hamming_TEST(
    SOC_SAND_IN uint32 silent
  )
{
  uint32
    pass = TRUE;
  uint32
    p_1,
    p_2,
    number_of_fixed_errors;
  SOC_SAND_RET
    soc_sand_ret = SOC_SAND_OK;

#define TEST_CODE_HAMMING_P                  (7)
#define TEST_CODE_HAMMING_DATA_BIT_WIDE      (68)
#define TEST_CODE_HAMMING_DATA_BIT_NOF_UINT32S (SOC_SAND_DIV_ROUND_UP(TEST_CODE_HAMMING_DATA_BIT_WIDE, SOC_SAND_NOF_BITS_IN_UINT32))
  const uint32
    TEST_code_hamming_gen_mat[TEST_CODE_HAMMING_P][TEST_CODE_HAMMING_DATA_BIT_NOF_UINT32S] =
    {
      {0x56AAAD5B, 0xAB555555, 0x0000000A },
      {0x9B33366D, 0xCD999999, 0x0000000C },
      {0xE3C3C78E, 0xF1E1E1E1, 0x00000000 },
      {0x03FC07F0, 0x01FE01FE, 0x0000000F },
      {0x03FFF800, 0x01FFFE00, 0x00000000 },
      {0xFC000000, 0x01FFFFFF, 0x00000000 },
      {0x00000000, 0xFE000000, 0x0000000F }
    };
  uint32
    TEST_code_hamming_search_mat[] =
    {
     0xFFFFFFFF,   0xFFFFFFFF,   0xFFFFFFFF,
     0x00000000,   0xFFFFFFFF,   0x00000001,
     0x00000002,   0x00000003,   0xFFFFFFFF,
     0x00000004,   0x00000005,   0x00000006,
     0x00000007,   0x00000008,   0x00000009,
     0x0000000A,   0xFFFFFFFF,   0x0000000B,
     0x0000000C,   0x0000000D,   0x0000000E,
     0x0000000F,   0x00000010,   0x00000011,
     0x00000012,   0x00000013,   0x00000014,
     0x00000015,   0x00000016,   0x00000017,
     0x00000018,   0x00000019,   0xFFFFFFFF,
     0x0000001A,   0x0000001B,   0x0000001C,
     0x0000001D,   0x0000001E,   0x0000001F,
     0x00000020,   0x00000021,   0x00000022,
     0x00000023,   0x00000024,   0x00000025,
     0x00000026,   0x00000027,   0x00000028,
     0x00000029,   0x0000002A,   0x0000002B,
     0x0000002C,   0x0000002D,   0x0000002E,
     0x0000002F,   0x00000030,   0x00000031,
     0x00000032,   0x00000033,   0x00000034,
     0x00000035,   0x00000036,   0x00000037,
     0x00000038,   0xFFFFFFFF,   0x00000039,
     0x0000003A,   0x0000003B,   0x0000003C,
     0x0000003D,   0x0000003E,   0x0000003F,
     0x00000040,   0x00000041,   0x00000042,
     0x00000043,
    };

#define TEST_CODE_HAMMING_NOF_TESTS (5)
  uint32
    TEST_data_to_test[TEST_CODE_HAMMING_NOF_TESTS][TEST_CODE_HAMMING_DATA_BIT_NOF_UINT32S] =
    {
      {0x00000000, 0x00000000, 0x8         }, 
      {0x00000001, 0x00000000, 0x8         }, 
      {0x00000000, 0x00001000, 0x8         }, 
      {0x56AAAD5B, 0xAB555555, 0x0000000A, }, 
      {0x00000000, 0x40000000, 0x0         }, 
    },
    TEST_data_to_test_p[TEST_CODE_HAMMING_NOF_TESTS] =
    {
      0x4B,
      0x48,
      0x78,
      0x03,
      0x46,
    },
    TEST_data_p_from_hw[TEST_CODE_HAMMING_NOF_TESTS] =
    {
      0x4B,
      0x4B,
      0x4B,
      0x03,
      0x46,
    },
    TEST_data_answer[TEST_CODE_HAMMING_NOF_TESTS][TEST_CODE_HAMMING_DATA_BIT_NOF_UINT32S] =
    {
      {0x00000000, 0x00000000, 0x8         }, 
      {0x00000000, 0x00000000, 0x8         },
      {0x00000000, 0x00000000, 0x8         },
      {0x56AAAD5B, 0xAB555555, 0x0000000A, },
      {0x00000000, 0x40000000, 0x0         }, 
    };
  uint32
    work_data[TEST_CODE_HAMMING_DATA_BIT_NOF_UINT32S],
    test_i;


    for(test_i=0; test_i<TEST_CODE_HAMMING_NOF_TESTS; ++test_i)
    {
      soc_sand_ret = soc_sand_code_hamming_get_p(
                   TEST_data_to_test[test_i],
                   TEST_CODE_HAMMING_DATA_BIT_WIDE,
                   (uint32*)TEST_code_hamming_gen_mat,
                   TEST_CODE_HAMMING_P,
                   &p_1
                 );
      if(soc_sand_ret)
      {
        goto exit;
      }

      soc_sand_ret = soc_sand_code_hamming_get_p_no_check(
                   TEST_data_to_test[test_i],
                   TEST_CODE_HAMMING_DATA_BIT_WIDE,
                   TEST_CODE_HAMMING_DATA_BIT_NOF_UINT32S,
                   (uint32*)TEST_code_hamming_gen_mat,
                   TEST_CODE_HAMMING_P,
                   work_data,
                   &p_2
                 );
      if(soc_sand_ret)
      {
        goto exit;
      }

      if(p_1 != p_2)
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("soc_sand_code_hamming_TEST(): ERROR 10\n\r")));
        pass = FALSE;
        goto exit;
      }
      if(p_1 != TEST_data_to_test_p[test_i])
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("soc_sand_code_hamming_TEST(): ERROR 20\n\r")));
        pass = FALSE;
        goto exit;
      }

      soc_sand_os_memcpy(work_data, TEST_data_to_test[test_i], TEST_CODE_HAMMING_DATA_BIT_NOF_UINT32S*sizeof(uint32));
      soc_sand_ret = soc_sand_code_hamming_check_data(
                   work_data,
                   TEST_CODE_HAMMING_DATA_BIT_WIDE,
                   (uint32*)TEST_code_hamming_gen_mat,
                   (uint32*)TEST_code_hamming_search_mat,
                   TEST_CODE_HAMMING_P,
                   TEST_data_p_from_hw[test_i],
                   &number_of_fixed_errors
                );
      if(soc_sand_ret)
      {
        goto exit;
      }
      if(
          soc_sand_os_memcmp(
            work_data,
            TEST_data_answer[test_i],
            TEST_CODE_HAMMING_DATA_BIT_NOF_UINT32S*sizeof(uint32)
          ) != 0
        )
      {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META("soc_sand_code_hamming_TEST(): ERROR 30\n\r")));
        pass = FALSE;
        goto exit;
      }


    }

exit:
  if(soc_sand_ret)
  {
    pass = FALSE;
  }

  if(pass == FALSE)
  {
    if(!silent)
    {
      LOG_INFO(BSL_LS_SOC_COMMON,
               (BSL_META("soc_sand_code_hamming_TEST(): Fail \n\r")));
    }
  }
  return pass;
}



void
  soc_sand_code_hamming_print_gen_matrix(
    SOC_SAND_IN uint32 data_bit_wide
  )
{
  uint32
    bit_i,
    p_bit_wide,
    search_gen_mat_nof_entires,
    p_i,
    data_to_print,
    print_flag;
  uint32
    *gen_mat = NULL,
    *search_gen_mat = NULL;

  if( soc_sand_code_hamming_get_p_bit_wide(data_bit_wide, &p_bit_wide) )
  {
    LOG_CLI((BSL_META("soc_sand_code_hamming_generate_gen_matrix():\n\r")));
    LOG_CLI((BSL_META("   ERROR: Unsupported data_bit_wide - %lu \n\r"), data_bit_wide));
    goto exit;
  }
  search_gen_mat_nof_entires = p_bit_wide + data_bit_wide+32;

  gen_mat        = (uint32*) soc_sand_os_malloc(sizeof(uint32) * data_bit_wide);
  search_gen_mat = (uint32*) soc_sand_os_malloc(sizeof(uint32) * search_gen_mat_nof_entires);
  if(NULL == gen_mat)
  {
    LOG_CLI((BSL_META("soc_sand_code_hamming_generate_gen_matrix():\n\r")));
    LOG_CLI((BSL_META("   ERROR: malloc failed: data_bit_wide - %lu \n\r"), data_bit_wide));
    goto exit;
  }

  soc_sand_os_memset(gen_mat, 0x0, sizeof(uint32) * data_bit_wide);

  
  if(soc_sand_code_hamming_generate_gen_matrix(data_bit_wide, search_gen_mat_nof_entires, gen_mat, search_gen_mat))
  {
    goto exit;
  }

  
  LOG_CLI((BSL_META("/* {\n\r")));
  for(bit_i=0; bit_i<data_bit_wide; ++bit_i)
  {
    data_to_print = gen_mat[bit_i];

    LOG_CLI((BSL_META("%4lu: 0x%08lX  "), bit_i, data_to_print));
    soc_sand_print_u_long_binary_format(data_to_print, p_bit_wide);
    LOG_CLI((BSL_META("\n\r")));
  }
  LOG_CLI((BSL_META("}*/\n\r")));

  
  LOG_CLI((BSL_META("#define XXX_CODE_HAMMING_P                  (%lu)\n\r"), p_bit_wide));
  LOG_CLI((BSL_META("#define XXX_CODE_HAMMING_DATA_BIT_WIDE      (%lu)\n\r"), data_bit_wide));
  LOG_CLI((BSL_META("#define XXX_CODE_HAMMING_DATA_BIT_NOF_UINT32S (SOC_SAND_DIV_ROUND_UP(XXX_CODE_HAMMING_DATA_BIT_WIDE, SOC_SAND_NOF_BITS_IN_UINT32))\n\r")));
  LOG_CLI((BSL_META("static const uint32\n\r")));
  LOG_CLI((BSL_META("  XXX_code_hamming_gen_mat[XXX_CODE_HAMMING_P][XXX_CODE_HAMMING_DATA_BIT_NOF_UINT32S] =\n\r")));
  LOG_CLI((BSL_META("{\n\r")));
  for(p_i=0; p_i<p_bit_wide; ++p_i)
  {
    data_to_print = 0;
    print_flag = FALSE;
    LOG_CLI((BSL_META("  {")));
    for(bit_i=0x0; bit_i<data_bit_wide; ++bit_i)
    {
      data_to_print |= ((gen_mat[bit_i] & SOC_SAND_BIT(p_i)) >> p_i) << (bit_i%SOC_SAND_NOF_BITS_IN_UINT32);
      if( ( bit_i+1 == data_bit_wide)  ||
          ((bit_i+1)%SOC_SAND_NOF_BITS_IN_UINT32 == 0) 
        )
      {
        print_flag = TRUE;
      }

      if(print_flag)
      {
        
        LOG_CLI((BSL_META("0x%08lX, "), data_to_print));
        print_flag = FALSE;
        data_to_print = 0;
      }
    }
    LOG_CLI((BSL_META("},\n\r")));
  }
  LOG_CLI((BSL_META("};\n\r")));

  
  LOG_CLI((BSL_META("\n\r")));
  LOG_CLI((BSL_META("static const uint32\n\r")));
  LOG_CLI((BSL_META("  XXX_code_hamming_search_mat[] =\n\r")));
  LOG_CLI((BSL_META("{\n\r")));
  for(bit_i=0; bit_i<search_gen_mat_nof_entires; ++bit_i)
  {
    data_to_print = search_gen_mat[bit_i];
    LOG_CLI((BSL_META("/*%4lu:*/ 0x%08lX,  "), bit_i, data_to_print));
    if((bit_i+1)%3 == 0)
    {
      LOG_CLI((BSL_META("\n\r")));
    }
    if(search_gen_mat[bit_i] == (data_bit_wide-1))
    {
      break;
    }
  }
  LOG_CLI((BSL_META("\n\r")));
  LOG_CLI((BSL_META("};\n\r")));
  LOG_CLI((BSL_META("\n\r")));


exit:
  SOC_SAND_FREE(gen_mat);
  SOC_SAND_FREE(search_gen_mat);

  return;
}


#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>
