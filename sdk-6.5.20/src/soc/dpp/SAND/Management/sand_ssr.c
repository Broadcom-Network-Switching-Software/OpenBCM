/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/dpp/SAND/Management/sand_ssr.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

SOC_SAND_SSR_CFG_VERSION_NUM
  soc_sand_ssr_get_cfg_version(
    SOC_SAND_IN uint32 soc_sand_version
  )
{
  SOC_SAND_SSR_CFG_VERSION_NUM
    ssr_ver_num = SOC_SAND_SSR_CFG_VERSION_INVALID;
  
  if(soc_sand_version < 1776)
  {
    ssr_ver_num = SOC_SAND_SSR_CFG_VERSION_INVALID;
  }
  else if(soc_sand_version >= 1776)
  {
    ssr_ver_num = SOC_SAND_SSR_CFG_VERSION_NUM_0;
  }

  return ssr_ver_num;
}

uint32
  soc_sand_ssr_reload_globals_cfg_ver_0(
      SOC_SAND_IN SOC_SAND_SSR_BUFF *curr_ssr_buff
    )
{
  soc_sand_ssr_set_big_endian(
    curr_ssr_buff->data.soc_sand_big_endian_was_checked,
    curr_ssr_buff->data.soc_sand_big_endian
  );

  soc_sand_set_print_when_writing(
    curr_ssr_buff->data.soc_sand_physical_print_when_writing,
    curr_ssr_buff->data.soc_sand_physical_print_asic_style,
    0
  );

  return 0;
}


uint32
  soc_sand_ssr_reload_globals_cfg_ver_1(
      SOC_SAND_IN SOC_SAND_SSR_BUFF *curr_ssr_buff
    )
{
  
  return 0;
}

uint32
  soc_sand_ssr_reload_globals(
      SOC_SAND_IN SOC_SAND_SSR_BUFF *curr_ssr_buff
    )
{
  SOC_SAND_SSR_CFG_VERSION_NUM
    soc_sand_ssr_cfg_version;

  soc_sand_ssr_cfg_version =
    soc_sand_ssr_get_cfg_version(curr_ssr_buff->header.soc_sand_version);

  if(soc_sand_ssr_cfg_version >= SOC_SAND_SSR_CFG_VERSION_NUM_0)
  {
    soc_sand_ssr_reload_globals_cfg_ver_0(curr_ssr_buff);
  }
  if(soc_sand_ssr_cfg_version >= SOC_SAND_SSR_CFG_VERSION_NUM_1)
  {
    soc_sand_ssr_reload_globals_cfg_ver_1(curr_ssr_buff);
  }

  return 0;
}


uint32
  soc_sand_ssr_save_globals(
      SOC_SAND_SSR_BUFF *curr_ssr_buff
    )
{
  uint32
    indirect_print;
  soc_sand_ssr_get_big_endian(
    &(curr_ssr_buff->data.soc_sand_big_endian_was_checked),
    &(curr_ssr_buff->data.soc_sand_big_endian)
  );

  soc_sand_get_print_when_writing(
    &(curr_ssr_buff->data.soc_sand_physical_print_when_writing),
    &(curr_ssr_buff->data.soc_sand_physical_print_asic_style),
    &indirect_print
  );

  return 0;
}


uint32
  soc_sand_ssr_get_ver_from_header(
      SOC_SAND_IN SOC_SAND_SSR_HEADER* header,
      SOC_SAND_OUT uint32* soc_sand_ver
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SSR_GET_VER_FROM_HEADER);
  SOC_SAND_CHECK_NULL_INPUT(header);
  SOC_SAND_CHECK_NULL_INPUT(soc_sand_ver);

  *soc_sand_ver = header->soc_sand_version;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_sand_ssr_get_ver_from_header", 0, 0);
}


uint32
  soc_sand_ssr_get_size_from_header(
      SOC_SAND_IN SOC_SAND_SSR_HEADER* header,
      SOC_SAND_OUT uint32* buff_size
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_SSR_GET_SIZE_FROM_HEADER);
  SOC_SAND_CHECK_NULL_INPUT(header);
  SOC_SAND_CHECK_NULL_INPUT(buff_size);

  *buff_size = header->buffer_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_sand_ssr_get_size_from_header", 0, 0);
}
