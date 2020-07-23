/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __GSA_FRAMEWORK_H_INCLUDED__
/* { */
#define __GSA_FRAMEWORK_H_INCLUDED__

/*************
 * INCLUDES  *
 */
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
/* } */
/*************
 * DEFINES   *
 */
/* { */


  /*
   *  Reserved error numbers
   */

#define GSA_MAX_NOF_DEVICES     10
#define GSA_MAX_NOF_SYS_PORTS   (GSA_MAX_NOF_DEVICES * SOC_PETRA_NOF_FAP_PORTS)

/* } */

/*************
 * MACROS    *
 */
/* { */

/************************************************************************/
/* Sys Device Defs                                                      */
/************************************************************************/






/************************************************************************/
/* Error Handling and Printing                                          */
/************************************************************************/
#define GSA_DEBUG 1

#if GSA_DEBUG
  #define GSA_DBG_ID "**DEBUG--> "
  #define GSA_DBG  \
  {                 \
    soc_sand_os_printf("\n%sFile    : %s \n%sFunction: %s line %u\n", GSA_DBG_ID, __FILE__, GSA_DBG_ID, FUNCTION_NAME(), __LINE__);\
  }
#else
  #define GSA_DBG
#endif
















/*
 * Macro to handle procedure call which returns standard soc_sand
 * error code and, in case of error, sets error code, performs an exit function and quits.
 * Assumes local variables: ret, error_id, soc_sand_err
 * Assumes label: exit
 */






/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

uint32
  gsa_sys_port2local_port_translate(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  sys_prt_ndx,
    SOC_SAND_OUT uint32  *loc_dev_ndx,
    SOC_SAND_OUT uint32  *loc_prt_ndx
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* __GSA_FRAMEWORK_H_INCLUDED__ */



