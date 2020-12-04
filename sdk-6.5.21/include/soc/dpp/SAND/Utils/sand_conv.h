/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef SOC_SAND_CONV_H_INCLUDED

#define SOC_SAND_CONV_H_INCLUDED
#ifdef  __cplusplus
extern "C" {
#endif
#include <soc/dpp/SAND/Utils/sand_framework.h>

#define SOC_SAND_IP_STR_SIZE (4*3+3+1+2)

SOC_SAND_RET
  soc_sand_kbits_per_sec_to_clocks(
    SOC_SAND_IN       uint32    rate,     
    SOC_SAND_IN       uint32     credit,   
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   interval  
  );


SOC_SAND_RET
  soc_sand_clocks_to_kbits_per_sec(
    SOC_SAND_IN       uint32    interval, 
    SOC_SAND_IN       uint32     credit,   
    SOC_SAND_IN       uint32    ticks_per_sec,
    SOC_SAND_OUT      uint32*   rate      
  );




uint32
  soc_sand_ip_addr_numeric_to_string(
    SOC_SAND_IN uint32 ip_addr,
    SOC_SAND_IN uint8  short_format,
    SOC_SAND_OUT char   decimal_ip[SOC_SAND_IP_STR_SIZE]
  );



uint32
  soc_sand_ip_addr_string_to_numeric(
    SOC_SAND_IN char   decimal_ip[SOC_SAND_IP_STR_SIZE],
    SOC_SAND_OUT uint32 *ip_addr
  );


#ifdef  __cplusplus
}
#endif


#endif

