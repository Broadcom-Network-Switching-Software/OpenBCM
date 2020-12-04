/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __UI_PURE_DEFI_PPA_API_INCLUDED__
/* { */
#define __UI_PURE_DEFI_PPA_API_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
/*
 * Note:
 * the following definitions must range between PARAM_PPA_API_START_RANGE_ID
 * and PARAM_PPA_API_END_RANGE_ID.
 * See ui_pure_defi.h
 */
typedef enum
{
  PARAM_PPA_VPLS_APP_ID = PARAM_PPA_API_START_RANGE_ID,
  PARAM_PPA_VPLS_APP_NOF_MP_SERVICES_ID,
  PARAM_PPA_VPLS_APP_NOF_P2P_SERVICES_ID,
  PARAM_PPA_VPLS_APP_FLOW_BASED_MODE_ID,
  PARAM_PPA_VPLS_APP_OAM_APP_ID,
  PARAM_PPA_VPLS_APP_POLL_INTERRUPT_ID,
  PARAM_PPA_BRIDGE_ROUTER_APP_ID,
  PARAM_PPA_BRIDGE_ROUTER_APP_FLOW_BASED_MODE_ID,

}PARAM_PPA_API_IDS;
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __UI_PURE_DEFI_PPA_API_INCLUDED__*/
#endif
