/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <soc/dpp/SAND/Management/sand_api_ssr.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_ssr.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

uint32
  soc_sand_ssr_get_buff_size(
    SOC_SAND_OUT uint32* byte_size_ptr
  )
{
  uint32
    ret = 0;

  if(!byte_size_ptr)
  {
    ret = 1;
    goto exit;
  }
  *byte_size_ptr = sizeof(SOC_SAND_SSR_BUFF);
exit:
  return ret;
}

uint32
  soc_sand_ssr_to_buff(
    SOC_SAND_OUT unsigned char* store_buff,
    SOC_SAND_IN  uint32  buff_byte_size
  )
{
  uint32
    ret = 0;

  SOC_SAND_SSR_BUFF
    *curr_ssr_buff = NULL;

  if(!store_buff)
  {
    ret = 1;
    goto exit;
  }

  if(buff_byte_size != sizeof(SOC_SAND_SSR_BUFF))
  {
    ret = 2;
    goto exit;
  }

  curr_ssr_buff =
    (SOC_SAND_SSR_BUFF *)store_buff;

  curr_ssr_buff->header.buffer_size  = buff_byte_size;
  curr_ssr_buff->header.soc_sand_version = soc_get_sand_ver();

  soc_sand_ssr_save_globals(
    curr_ssr_buff
    );


exit:
  return ret;
}

uint32
  soc_sand_ssr_from_buff(
    SOC_SAND_IN  unsigned char* store_buff,
    SOC_SAND_IN  uint32  buff_byte_size
  )
{
  uint32
    ret = 0;
  const SOC_SAND_SSR_BUFF
    *curr_ssr_buff = NULL;

  if(!store_buff)
  {
    ret = 1;
    goto exit;
  }

  
  if(buff_byte_size > sizeof(SOC_SAND_SSR_BUFF))
  {
    ret = 2;
    goto exit;
  }

  curr_ssr_buff =
    (const SOC_SAND_SSR_BUFF *)store_buff;

  if(buff_byte_size != curr_ssr_buff->header.buffer_size)
  {
    ret = 3;
    goto exit;
  }

  soc_sand_ssr_reload_globals(
      curr_ssr_buff
    );
exit:
  return ret;
}
