/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_GENERAL_INCLUDED__

#define __ARAD_GENERAL_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_reg_access.h>





#define ARAD_GEN_ERR_NUM_BASE            2000
#define ARAD_GEN_ERR_NUM_ALLOC           (ARAD_GEN_ERR_NUM_BASE + 0)
#define ARAD_GEN_ERR_NUM_ALLOC_ANY       (ARAD_GEN_ERR_NUM_BASE + 1)
#define ARAD_GEN_ERR_NUM_ALLOC_AND_CLEAR (ARAD_GEN_ERR_NUM_BASE + 2)
#define ARAD_GEN_ERR_NUM_ALLOC_ANY_SET   (ARAD_GEN_ERR_NUM_BASE + 3)
#define ARAD_GENERAL_EXIT_PLACE_TAKE_SEMAPHORE (ARAD_GEN_ERR_NUM_BASE + 4)
#define ARAD_GENERAL_EXIT_PLACE_GIVE_SEMAPHORE (ARAD_GEN_ERR_NUM_BASE + 5)
#define ARAD_GEN_ERR_NUM_CLEAR           (ARAD_GEN_ERR_NUM_BASE + 6)
#define ARAD_GEN_ERR_NUM_COPY            (ARAD_GEN_ERR_NUM_BASE + 7)
#define ARAD_GEN_ERR_NUM_COMP            (ARAD_GEN_ERR_NUM_BASE + 8)


#define SOC_DPP_MSG(string) string






#define ARAD_ALLOC(var, type, count,str)                                     \
  {                                                                       \
    if(var != NULL)                                                       \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(ARAD_ALLOC_TO_NON_NULL_ERR, ARAD_GEN_ERR_NUM_ALLOC, exit); \
    }                                                                     \
    var = (type*)soc_sand_os_malloc((count) * sizeof(type),str);                  \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, ARAD_GEN_ERR_NUM_ALLOC, exit);  \
    }                                                                     \
    res = soc_sand_os_memset(                                                 \
            var,                                                          \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, ARAD_GEN_ERR_NUM_ALLOC, exit);           \
  }
#define ARAD_ALLOC_ANY_SIZE(var, type, count,str)                             \
  {                                                                       \
    if(var != NULL)                                                       \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(ARAD_ALLOC_TO_NON_NULL_ERR, ARAD_GEN_ERR_NUM_ALLOC, exit); \
    }                                                                     \
    var = (type*)soc_sand_os_malloc_any_size((count) * (uint32)sizeof(type),str); \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    res = soc_sand_os_memset(                                                 \
            var,                                                          \
            0x0,                                                          \
            (count) * (uint32)sizeof(type)                                \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, ARAD_GEN_ERR_NUM_ALLOC_ANY_SET, exit);   \
  }

#define ARAD_CLEAR_STRUCT(var, type)                                     \
  {                                                                       \
    if (var == NULL)                                                      \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    arad_##type##_clear(var);                                            \
  }

#define ARAD_ALLOC_AND_CLEAR_STRUCT(var, type, str)                           \
  {                                                                       \
    var = (type*)soc_sand_os_malloc(sizeof(type), str);                            \
    ARAD_CLEAR_STRUCT(var, type);                                        \
  }

#define ARAD_FREE(var)                                                    \
  if (var != NULL)                                                        \
  {                                                                        \
    soc_sand_os_free(var);                                                     \
    var = NULL;                                                           \
  }

#define ARAD_FREE_ANY_SIZE(var)                                           \
  if (var != NULL)                                                        \
  {                                                                        \
    soc_sand_os_free_any_size(var);                                            \
    var = NULL;                                                           \
  }

#define ARAD_CLEAR(var_ptr, type, count)                                 \
  {                                                                       \
    res = soc_sand_os_memset(                                                 \
            var_ptr,                                                      \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, ARAD_GEN_ERR_NUM_CLEAR, exit);           \
  }

#define ARAD_COPY(var_dest_ptr, var_src_ptr, type, count)                \
  {                                                                       \
    res = soc_sand_os_memcpy(                                                 \
            var_dest_ptr,                                                 \
            var_src_ptr,                                                  \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, ARAD_GEN_ERR_NUM_CLEAR, exit);           \
  }

#define ARAD_COMP(var_ptr1, var_ptr2, type, count, is_equal_res)         \
  {                                                                       \
    is_equal_res = SOC_SAND_NUM2BOOL_INVERSE(soc_sand_os_memcmp(                  \
            var_ptr1,                                                     \
            var_ptr2,                                                     \
            (count) * sizeof(type)                                        \
          ));                                                             \
  }

#define ARAD_DEVICE_CHECK(unit, exit_label)                               \
    SOC_SAND_ERR_IF_ABOVE_NOF(unit, SOC_MAX_NUM_DEVICES,             \
        ARAD_DEVICE_ID_OUT_OF_RANGE_ERR, 7777, exit_label);               \

#define ARAD_BIT_TO_U32(nof_bits) (((nof_bits)+31)/32)



















uint32
  arad_sw_db_cfg_ticks_per_sec_get(
    void
  );

uint8
  arad_is_multicast_id_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                multicast_id
  );

uint8
  arad_is_queue_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                queue
  );

uint8
  arad_is_flow_valid(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                flow
  );


uint32
  arad_intern_rate2clock(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  rate_kbps,
    SOC_SAND_IN  uint8 is_for_ips,
    SOC_SAND_OUT uint32  *clk_interval
  );


uint32
  arad_intern_clock2rate(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  clk_interval,
    SOC_SAND_IN  uint8 is_for_ips,
    SOC_SAND_OUT uint32  *rate_kbps
  );




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


