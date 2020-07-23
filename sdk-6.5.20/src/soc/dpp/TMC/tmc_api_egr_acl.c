/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_egr_acl.h>





















void
  SOC_TMC_EGR_ACL_DP_VALUES_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_DP_VALUES *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_EGR_ACL_DP_VALUES));
  for (ind = 0; ind < SOC_TMC_EGR_ACL_NOF_DP_VALUES; ++ind)
  {
    info->val[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_EGR_ACL_PORT_INFO));
  info->profile = 0;
  info->acl_data = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_EGR_ACL_ACTION_VAL));
  info->trap_en = 0;
  info->trap_code = 0;
  info->ofp_ov = 0;
  info->ofp = 0;
  info->dp_tc_ov = 0;
  info->tc = 0;
  info->dp = 0;
  info->cud = 0xFFFF;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_TMC_EGR_ACL_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_EGR_ACL_ENTRY_INFO));
  info->type = SOC_TMC_EGR_NOF_ACL_DB_TYPES;
  info->priority = 0;
  SOC_TMC_TCAM_KEY_clear(&(info->key));
  SOC_TMC_EGR_ACL_ACTION_VAL_clear(&(info->action_val));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

void
  SOC_TMC_EGR_ACL_DP_VALUES_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_DP_VALUES *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_EGR_ACL_NOF_DP_VALUES; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "val[%u]: %u\n\r"),ind,info->val[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_PORT_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "profile: %u\n\r"),info->profile));
  LOG_CLI((BSL_META_U(unit,
                      "acl_data: %u\n\r"),info->acl_data));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_ACTION_VAL_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "trap_en: %u\n\r"),info->trap_en));
  LOG_CLI((BSL_META_U(unit,
                      "trap_code: %u\n\r"),info->trap_code));
  LOG_CLI((BSL_META_U(unit,
                      "ofp_ov: %u\n\r"),info->ofp_ov));
  LOG_CLI((BSL_META_U(unit,
                      "ofp: %u\n\r"),info->ofp));
  LOG_CLI((BSL_META_U(unit,
                      "dp_tc_ov: %u\n\r"),info->dp_tc_ov));
  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"),info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "cud: %u\n\r"),info->cud));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_EGR_ACL_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_TMC_EGR_ACL_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_TMC_EGR_ACL_DB_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "priority: %u\n\r"),info->priority));
  LOG_CLI((BSL_META_U(unit,
                      "action_val:")));
  SOC_TMC_EGR_ACL_ACTION_VAL_print(&(info->action_val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

