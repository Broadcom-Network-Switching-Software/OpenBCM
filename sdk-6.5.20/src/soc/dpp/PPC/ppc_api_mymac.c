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

#include <soc/dpp/PPC/ppc_api_mymac.h>





















void
  SOC_PPC_MYMAC_VRRP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MYMAC_VRRP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MYMAC_VRRP_INFO));
  info->enable = 0;
  info->mode = SOC_PPC_NOF_MYMAC_VRRP_MODES;
  info->ipv6_enable = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MYMAC_TRILL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_MYMAC_TRILL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MYMAC_TRILL_INFO));
  info->my_nick_name = 0;
  soc_sand_os_memset(info->virtual_nick_names, 0x0, sizeof(uint32)*SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE);
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->all_rbridges_mac));
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->all_esadi_rbridges));
  info->skip_ethernet_my_nickname = 0;
  soc_sand_os_memset(info->skip_ethernet_virtual_nick_names, 0x0, sizeof(uint8)*SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#ifdef BCM_88660_A0
void 
  SOC_PPC_VRRP_CAM_INFO_clear(
   SOC_SAND_OUT SOC_PPC_VRRP_CAM_INFO *info
  ){
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    sal_memset(info, 0x0, sizeof(SOC_PPC_VRRP_CAM_INFO));
    soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->mac_addr));
    SOC_SAND_MAGIC_NUM_SET;
  exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void 
  SOC_PPC_VRRP_CAM_INFO_print(
   SOC_SAND_OUT SOC_PPC_VRRP_CAM_INFO *info
  ){
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

    LOG_CLI((BSL_META_U(unit,
                        "mac_addr: \n")));
    soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->mac_addr));
    LOG_CLI((BSL_META_U(unit,
                        "is_ipv4_entry: %d\n"), info->is_ipv4_entry));
    LOG_CLI((BSL_META_U(unit,
                        "vrrp_cam_index: %d\n"), info->vrrp_cam_index));

    SOC_SAND_MAGIC_NUM_SET;
  exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 


#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_MYMAC_VRRP_MODE_to_string(
    SOC_SAND_IN  SOC_PPC_MYMAC_VRRP_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MYMAC_VRRP_MODE_ALL_VSI_BASED:
    str = "vsi_based_all";
  break;
  case SOC_PPC_MYMAC_VRRP_MODE_256_VSI_BASED:
    str = "vsi_based_256";
  break;
  case SOC_PPC_MYMAC_VRRP_MODE_512_VSI_BASED:
    str = "vsi_based_512";
  break;
  case SOC_PPC_MYMAC_VRRP_MODE_1K_VSI_BASED:
    str = "vsi_based_1K";
  break;
  case SOC_PPC_MYMAC_VRRP_MODE_2K_VSI_BASED:
    str = "vsi_based_2K";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_MYMAC_VRRP_INFO_print(
    SOC_SAND_IN  SOC_PPC_MYMAC_VRRP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable: %u\n\r"),info->enable));
  LOG_CLI((BSL_META_U(unit,
                      "mode %s \n\r"), SOC_PPC_MYMAC_VRRP_MODE_to_string(info->mode)));
  LOG_CLI((BSL_META_U(unit,
                      "vrid_my_mac_msb: ")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->vrid_my_mac_msb));
  LOG_CLI((BSL_META_U(unit,
                      "\n\ripv6_enable: %u\n\r"),info->ipv6_enable));
  if(info->ipv6_enable) {
      LOG_CLI((BSL_META_U(unit,
                          "ipv6_vrid_my_mac_msb: ")));
      soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->ipv6_vrid_my_mac_msb));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MYMAC_TRILL_INFO_print(
    SOC_SAND_IN  SOC_PPC_MYMAC_TRILL_INFO *info
  )
{
  int i;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "my_nick_name: %u\n\r"),info->my_nick_name));
  for (i=0;i<SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE;i++) {
      LOG_CLI((BSL_META_U(unit,
                          "virtual_nick_name: %u\n\r"),info->virtual_nick_names[i]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "all_rbridges_mac: ")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->all_rbridges_mac));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "all_esadi_rbridges: ")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->all_esadi_rbridges));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

