/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_PURE_DEFI_GSA_PETRA_API_INCLUDED__
/* { */
#define __UI_PURE_DEFI_GSA_PETRA_API_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>

#define UI_MC_GRP_MEMBERS_MAX   10


/*
 * Note:
 * the following definitions must range between PARAM_GSA_PETRA_API_START_RANGE_ID
 * and PARAM_GSA_PETRA_API_END_RANGE_ID.
 * See ui_pure_defi.h
 */
typedef enum
{
  PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_ID = PARAM_GSA_PETRA_START_RANGE_ID,
  PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_ID_ID,
  PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_TYPE_ID,
  PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_GRP_MEMBERS_ID,
  PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_PER_MEMBER_CUD_ID,
  PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_ID,
  PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_TYPE_ID,
  PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_ID_ID,
  PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_ID,
  PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_MC_ID_ID,
  PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_MC_TYPE_ID,
  PARAM_GSA_PETRA_API_MULTICAST_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_WEIGHT_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_SWP_P_TM_SCH_HR_CLASS_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_MAX_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_MIN_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_CLASS_NDX_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_PORT_NDX_ID,
  PARAM_GSA_PETRA_SCH_SCHEMES_ID,
  PARAM_GSA_PETRA_CONNECTIVITY_MAP_GET_ID,
  PARAM_GSA_PETRA_CONNECTIVITY_MAP_GET_FAP_NDX_ID,
  PARAM_GSA_PETRA_CONNECTIVITY_MAP_GET_ALL_FAPS_ID,
  PARAM_GSA_PETRA_SERDES_POWER_ID,
  PARAM_GSA_PETRA_SERDES_POWER_DOWN_ID,
  PARAM_GSA_PETRA_SERDES_POWER_DOWN_FE_NDX_ID,
  PARAM_GSA_PETRA_SERDES_POWER_DOWN_SERDES_NDX_ID,
  PARAM_GSA_PETRA_SERDES_POWER_UP_ID,
  PARAM_GSA_PETRA_SERDES_POWER_UP_FE_NDX_ID,
  PARAM_GSA_PETRA_SERDES_POWER_UP_SERDES_NDX_ID,
  PARAM_GSA_PETRA_LINKS_ID,
}
PARAM_GSA_PETRA_API_IDS;

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
/* } __UI_PURE_DEFI_GSA_PETRA_API_INCLUDED__*/
#endif
