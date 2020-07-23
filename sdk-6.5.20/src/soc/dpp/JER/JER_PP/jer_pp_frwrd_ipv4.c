/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_L3




#include <soc/dcmn/error.h>
#include <soc/dcmn/utils.h>
#include <shared/bsl.h>
#include <soc/dpp/JER/JER_PP/jer_pp_frwrd_ipv4.h>
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>
#endif




























