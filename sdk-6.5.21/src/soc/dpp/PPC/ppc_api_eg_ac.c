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

#include <soc/dpp/PPC/ppc_api_eg_ac.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>





















void
  SOC_PPC_EG_AC_VBP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_VBP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_AC_VBP_KEY));
  info->vlan_domain = 0;
  info->vsi = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_AC_CEP_PORT_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_CEP_PORT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_AC_CEP_PORT_KEY));
  info->vlan_domain = 0;
  info->cvid = 0;
  info->pep_edit_profile = 0;
  info->vsi = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_CEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_VLAN_EDIT_CEP_INFO));
  info->cvid = 0;
  info->up = 0;
  info->pcp_profile = 0;
  info->edit_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_VLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_VLAN_EDIT_VLAN_INFO));
  info->vid = 0;
  info->pcp = 0;
  info->dei = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_VLAN_EDIT_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_AC_VLAN_EDIT_INFO));
  for (ind = 0; ind < 2; ++ind)
  {
    SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_clear(&(info->vlan_tags[ind]));
  }
  info->nof_tags = 0;
  info->edit_profile = 0;
  info->pcp_profile = 0;
  info->oam_lif_set = 0;
#ifdef BCM_88660_A0
  info->use_as_data_entry = 0;  
  for (ind = 0; ind < SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE; ++ind)
  {
    info->data[ind] = 0;
  }
#endif

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_AC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_AC_INFO));
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_clear(&(info->edit_info));
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_clear(&(info->protection_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_AC_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_AC_MP_INFO));
  info->mp_level = 0;
  info->is_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_EG_AC_VBP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vlan_domain: %u\n\r"),info->vlan_domain));
  LOG_CLI((BSL_META_U(unit,
                      "vsi: %u\n\r"),info->vsi));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_AC_CEP_PORT_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vlan_domain: %u\n\r"),info->vlan_domain));
  LOG_CLI((BSL_META_U(unit,
                      "cvid: %u\n\r"),info->cvid));
  LOG_CLI((BSL_META_U(unit,
                      "pep_edit_profile: %u\n\r"),info->pep_edit_profile));
  LOG_CLI((BSL_META_U(unit,
                      "vsi: %u\n\r"),info->vsi));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_CEP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "cvid: %u\n\r"),info->cvid));
  LOG_CLI((BSL_META_U(unit,
                      "up: %u\n\r"), info->up));
  LOG_CLI((BSL_META_U(unit,
                      "pcp_profile: %u\n\r"), info->pcp_profile));
  LOG_CLI((BSL_META_U(unit,
                      "edit_profile: %u\n\r"),info->edit_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "vid: %u\n\r"),info->vid));
  LOG_CLI((BSL_META_U(unit,
                      "pcp: %u\n\r"), info->pcp));
  LOG_CLI((BSL_META_U(unit,
                      "dei: %u\n\r"), info->dei));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_VLAN_EDIT_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Outer Tag [edit_info_0]: \n\r")));
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_print(&(info->vlan_tags[0]));
  LOG_CLI((BSL_META_U(unit,
                      "Inner Tag [edit_info_1]: \n\r")));
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_print(&(info->vlan_tags[1]));
  LOG_CLI((BSL_META_U(unit,
                      "nof_tags: %u\n\r"),info->nof_tags));
  LOG_CLI((BSL_META_U(unit,
                      "edit_profile: %u\n\r"),info->edit_profile));
  LOG_CLI((BSL_META_U(unit,
                      "pcp_profile: %u\n\r"),info->pcp_profile));
  LOG_CLI((BSL_META_U(unit,
                      "lif_profile: %u\n\r"),info->lif_profile));
  LOG_CLI((BSL_META_U(unit,
                      "oam_lif_set: %u\n\r"),info->oam_lif_set));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_AC_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "edit_info:")));
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_print(&(info->edit_info));
  LOG_CLI((BSL_META_U(unit,
                      "protection_info:")));
  SOC_PPC_EG_ENCAP_PROTECTION_INFO_print(&(info->protection_info));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_AC_MP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "mp_level: %u\n\r"),info->mp_level));
  LOG_CLI((BSL_META_U(unit,
                      "is_valid: %u\n\r"),info->is_valid));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

