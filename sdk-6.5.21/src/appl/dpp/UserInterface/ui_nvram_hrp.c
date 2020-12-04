/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__)
/* { */

int
  app_source_checker(
    void           *current_line_ptr,
    unsigned long  source_id,
    char           *err_msg,
    unsigned long  partial
    )
{
  return 0;
}

int
  get_telnet_activity_timeout_cont(
    unsigned char *period_ptr
  )
{
  return 0;
}

int
  get_telnet_activity_timeout_std(
    unsigned char *period_ptr
  )
{
  return 0;
}

/*} LINUX */
#endif
