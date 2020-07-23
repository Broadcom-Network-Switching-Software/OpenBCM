/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef _ARAD_SIM_H_

#define _ARAD_SIM_H_



#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#if 0
#if !(defined(LINUX) || defined(UNIX))
  #include <appl/dpp/../H/DuneTypeDef.h>
#endif

#if (defined(LINUX) || defined(UNIX))
  #include <appl/diag/dpp/utils_defx.h>
#endif
#endif

#ifdef NUM_FE_ON_BOARD
  #define CHIP_SIM_NOF_CHIPS NUM_FE_ON_BOARD
#else
  #define CHIP_SIM_NOF_CHIPS 2
#endif

#define FUNC_EXIT_POINT exit_point
#define GOTO_FUNC_EXIT_POINT goto exit_point ;


#define INVALID_ADDRESS 0xFFFFFFFF


#ifndef UINT32
  #define UINT32 unsigned int
#endif

#ifndef INT32
  #define INT32 signed int
#endif

#ifndef STATUS
  #define STATUS int
#endif

#ifndef OK
  #define OK 0
#endif

#ifndef ERROR
  #define ERROR 1
#endif

#ifdef _MSC_VER
typedef int (*FUNCPTR) ();     
#endif

#if 0
  #define CHIP_SIM_MALLOC(x) malloc(x)
  #define CHIP_SIM_FREE(x)   free(x)
#else
  #define CHIP_SIM_MALLOC(x,str) soc_sand_os_malloc_any_size(x, str)
  #define CHIP_SIM_FREE(x)   soc_sand_os_free(x)
#endif

#ifdef _MSC_VER
  #define CHIP_SIM_BYTE_SWAP(x) (x)
#else
  #define CHIP_SIM_BYTE_SWAP(x) (x)
#endif




#endif

