/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPC

#include <shared/bsl.h>




#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_vsi.h>





















void
  SOC_PPC_VSI_L2CP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_VSI_L2CP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_VSI_L2CP_KEY));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_VSI_INFO_clear(
    SOC_SAND_OUT SOC_PPC_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_VSI_INFO));
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->default_forwarding));
  info->stp_topology_id = 0;
  info->enable_my_mac = 0;
  info->fid_profile_id = 0;
  info->mac_learn_profile_id = 0;
  info->clear_on_destroy = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_VSI_INFO_print(
    SOC_SAND_IN  SOC_PPC_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "default_forwarding:")));
  SOC_PPC_FRWRD_DECISION_INFO_print(&(info->default_forwarding));
  LOG_CLI((BSL_META_U(unit,
                      "default_forward_profile: %u\n\r"),info->default_forward_profile));
  LOG_CLI((BSL_META_U(unit,
                      "stp_topology_id: %u\n\r"),info->stp_topology_id));
  LOG_CLI((BSL_META_U(unit,
                      "enable_my_mac: %u\n\r"),info->enable_my_mac));
  if (info->fid_profile_id == SOC_PPC_VSI_FID_IS_VSID)
  {
    LOG_CLI((BSL_META_U(unit,
                        "fid_profile_id: VSID\n\r")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "fid_profile_id: %u\n\r"),info->fid_profile_id));
  }
  
  LOG_CLI((BSL_META_U(unit,
                      "mac_learn_profile_id: %u\n\r"),info->mac_learn_profile_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

