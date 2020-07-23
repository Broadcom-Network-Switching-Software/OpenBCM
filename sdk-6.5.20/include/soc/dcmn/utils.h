/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOCDNX_GENERAL_INCLUDED__

#define __SOCDNX_GENERAL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>










#define SOCDNX_ALLOC_ANY_SIZE(var, type, count,str)                       \
  do {                                                                    \
    if(var != NULL)                                                       \
    {                                                                     \
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_SOCDNX_MSG("Trying to allocate to a non null ptr is forbidden"))); \
    }                                                                     \
    var = (type*)soc_sand_os_malloc_any_size((count) * (uint32)sizeof(type),str); \
    if (var == NULL)                                                      \
    {                                                                     \
      SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_SOCDNX_MSG("Failed to allocate memory")));\
    }                                                                     \
    _rv = soc_sand_os_memset(                                                 \
            var,                                                          \
            0x0,                                                          \
            (count) * (uint32)sizeof(type)                                \
          );                                                              \
    SOCDNX_SAND_IF_ERR_EXIT(_rv);                                          \
  } while (0);


#define SOCDNX_CLEAR(var_ptr, type, count)                                \
  do {                                                                    \
    _rv = soc_sand_os_memset(                                             \
            var_ptr,                                                      \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOCDNX_IF_ERR_EXIT(_rv);                                              \
  } while (0);

#define SOCDNX_COPY(var_dest_ptr, var_src_ptr, type, count)                \
  do {                                                                    \
    _rc = soc_sand_os_memcpy(                                             \
            var_dest_ptr,                                                 \
            var_src_ptr,                                                  \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOCDNX_IF_ERR_EXIT(_rv);                                              \
  } while (0);

#define SOCDNX_COMP(var_ptr1, var_ptr2, type, count, is_equal_res)         \
  do {                                                                     \
    is_equal_res = SOC_SAND_NUM2BOOL_INVERSE(soc_sand_os_memcmp(                  \
            var_ptr1,                                                     \
            var_ptr2,                                                     \
            (count) * sizeof(type)                                        \
          ));                                                             \
  } while (0);


















#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



