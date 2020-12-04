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

#include <soc/dpp/PPC/ppc_api_eg_filter.h>






#define PPC_API_EG_FILTER_NOF_INLIF_PROFILE_OPTIONS 4

















void
  SOC_PPC_EG_FILTER_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_FILTER_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_FILTER_PORT_INFO));
  
  info->filter_mask = SOC_PPC_EG_FILTER_PORT_ENABLE_MTU | SOC_PPC_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON;
  info->acceptable_frames_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#ifdef BCM_88660
  void
  SOC_PPC_EG_FILTER_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_FILTER_GLOBAL_INFO *info
  ) 
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(*info));

  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif 

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS:
    str = "promiscuous";
  break;
  case SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED:
    str = "isolated";
  break;
  case SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY:
    str = "community";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_EG_FILTER_PORT_ENABLE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_ENABLE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_FILTER_PORT_ENABLE_NONE:
    str = "none";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP:
    str = "vsi_membership";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE:
    str = "same_interface";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA:
    str = "uc_unknow_da";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA:
    str = "mc_unknow_da";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA:
    str = "bc_unknow_da";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_MTU:
    str = "mtu";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_STP:
    str = "stp";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_PEP_ACCEPTABLE_FRM_TYPES:
    str = "pep_acceptable_frm_types";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON:
    str = "split_horizon";
  break;
  case SOC_PPC_EG_FILTER_PORT_ENABLE_ALL:
    str = "all";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_EG_FILTER_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_FILTER_PORT_INFO *info
  )
{
  uint32
    indx;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "acceptable_frames_profile: %u\n\r"),info->acceptable_frames_profile));
  LOG_CLI((BSL_META_U(unit,
                      "enabled_filters: \n\r")));
  if (((info->filter_mask & SOC_PPC_EG_EGQ_FILTER_PORT_ENABLE_NONE) == 0) && (info->filter_mask != SOC_PPC_EG_FILTER_PORT_ENABLE_NONE))
  {
      LOG_CLI((BSL_META_U(unit, "enabled_egq_filters: \n\r")));
  }
  if (((info->filter_mask & SOC_PPC_EG_EPNI_FILTER_PORT_ENABLE_NONE) == 0) && (info->filter_mask != SOC_PPC_EG_FILTER_PORT_ENABLE_NONE))
  {
      LOG_CLI((BSL_META_U(unit, "enabled_epni_filters: \n\r")));
  }

  for (indx = 0; indx < SOC_PPC_NOF_EG_FILTER_PORT_ENABLES-2; ++indx)
  {
    if (SOC_SAND_BIT(indx) & info->filter_mask)
    {
      LOG_CLI((BSL_META_U(unit,
                          " - %s\n\r"), SOC_PPC_EG_FILTER_PORT_ENABLE_to_string((SOC_PPC_EG_FILTER_PORT_ENABLE)(SOC_SAND_BIT(indx)))));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\rdisabled_filters: \n\r")));
  if ((info->filter_mask & SOC_PPC_EG_EGQ_FILTER_PORT_ENABLE_NONE) || (info->filter_mask == SOC_PPC_EG_FILTER_PORT_ENABLE_NONE))
  {
      LOG_CLI((BSL_META_U(unit, "disabled_egq_filters: \n\r")));
  }
  if ((info->filter_mask & SOC_PPC_EG_EPNI_FILTER_PORT_ENABLE_NONE) || (info->filter_mask == SOC_PPC_EG_FILTER_PORT_ENABLE_NONE))
  {
      LOG_CLI((BSL_META_U(unit, "disabled_epni_filters: \n\r")));
  }
  for (indx = 0; indx < SOC_PPC_NOF_EG_FILTER_PORT_ENABLES-2; ++indx)
  {
    if ((SOC_SAND_BIT(indx) & info->filter_mask) == 0)
    {
      LOG_CLI((BSL_META_U(unit,
                          " - %s\n\r"), SOC_PPC_EG_FILTER_PORT_ENABLE_to_string((SOC_PPC_EG_FILTER_PORT_ENABLE)(SOC_SAND_BIT(indx)))));
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);;
}

#ifdef BCM_88660

void 
  SOC_PPC_EG_FILTER_GLOBAL_INFO_print(
    SOC_SAND_IN SOC_PPC_EG_FILTER_GLOBAL_INFO *info
  )
{
  uint32 i;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (i=0; i<PPC_API_EG_FILTER_NOF_INLIF_PROFILE_OPTIONS; i++) {
    LOG_CLI((BSL_META_U(unit,
                        "disable_same_if for inlif_profile %d: %d\n"), i, info->in_lif_profile_disable_same_interface_filter_bitmap & (1 << i)));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);;
}
#endif 

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

