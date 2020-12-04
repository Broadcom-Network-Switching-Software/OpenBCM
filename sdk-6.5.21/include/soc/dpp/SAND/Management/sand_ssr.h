/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_SAND_SSR_H_INCLUDED__

#define __SOC_SAND_SSR_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Management/sand_api_ssr.h>

typedef enum{
  SOC_SAND_SSR_CFG_VERSION_INVALID = 0,
  SOC_SAND_SSR_CFG_VERSION_NUM_0 = 1,
  SOC_SAND_SSR_CFG_VERSION_NUM_1 = 2
}SOC_SAND_SSR_CFG_VERSION_NUM;


typedef struct
{
  uint32 soc_sand_big_endian_was_checked;
  uint32 soc_sand_big_endian;
  uint32 soc_sand_physical_print_when_writing;
  uint32 soc_sand_physical_print_asic_style;
}SOC_SAND_SSR_DATA;

typedef struct
{
  SOC_SAND_SSR_HEADER header;
  SOC_SAND_SSR_DATA   data;
}SOC_SAND_SSR_BUFF;

uint32
  soc_sand_ssr_reload_globals(
      SOC_SAND_IN SOC_SAND_SSR_BUFF *curr_ssr_buff
    );
uint32
  soc_sand_ssr_save_globals(
      SOC_SAND_SSR_BUFF *curr_ssr_buff
    );


uint32
  soc_sand_ssr_get_ver_from_header(
      SOC_SAND_IN SOC_SAND_SSR_HEADER* header,
      SOC_SAND_OUT uint32* soc_sand_ver
  );


uint32
  soc_sand_ssr_get_size_from_header(
      SOC_SAND_IN SOC_SAND_SSR_HEADER* header,
      SOC_SAND_OUT uint32* buff_size
  );

#ifdef  __cplusplus
}
#endif


#endif
