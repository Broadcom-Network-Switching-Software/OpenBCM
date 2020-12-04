/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if !DUNE_BCM
	#include "DuneDriver/SAND/Utils/include/sand_os_interface.h"
#else
	#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#endif

#if !DUNE_BCM 
#else

int
  gen_err(
        int compare,
        int reverse_logic,
        int status,
        int reference,
        const char *msg,
        const char *proc_name,
        int severity,
        int err_id,
        unsigned int store_in_nv,
        unsigned int alarm_flags,
        short min_time_between_events,
        unsigned int send_trap
        )
{
  int
    error_flag = TRUE;

  if (compare)
  {
    if (reverse_logic)
    {
      if (status != reference)
      {
        error_flag = FALSE;
      }
    }
    else
    {
      if (status == reference)
      {
        error_flag = FALSE;
      }
    }
  }

  if (error_flag == TRUE)
  {
    soc_sand_os_printf("*** gen_err: %s, proc: %s\n\r",
                   msg,
                   proc_name);
  }

  return error_flag;
}

#endif /* !DUNE_BCM */
