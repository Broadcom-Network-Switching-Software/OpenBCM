/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UTILS_STRING_H_INCLUDED__
/* { */
#define __UTILS_STRING_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif


#ifdef __DUNE_HRP_LINUX__
/* { */
  #include "DHRP/include/dhrp_defs.h"
/* } */
#else
/* { */
  #include <appl/dpp/UserInterface/ui_pure_defi.h>
/* } */
#endif


char*
  sprint_ip(
          char          decimal_ip[MAX_IP_STRING],
    const unsigned long ip_addr,
    const unsigned int  short_format
  );

int
  str_to_lower(
    char *in_str
  ) ;

unsigned
  int
    str_max_match(
      const char         *in_str_01,
      const char         *in_str_02,
      const unsigned int max_to_compare
    );

#ifdef  __cplusplus
}
#endif


/* } __UTILS_STRING_H_INCLUDED__*/
#endif

