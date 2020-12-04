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

#include <soc/dpp/TMC/tmc_api_tcam.h>





















void
  SOC_TMC_TCAM_DB_PREFIX_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_DB_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_DB_PREFIX));
  info->val = 0;
  info->nof_bits = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_DB_USER_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_DB_USER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_DB_USER));
  info->priority = 0;
  info->entry_size = SOC_TMC_NOF_TCAM_BANK_ENTRY_SIZES;
  info->prefix_length = 4;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_DB_INFO_clear(
    SOC_SAND_OUT SOC_TMC_TCAM_DB_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_TCAM_DB_INFO));
  for (ind = 0; ind < SOC_TMC_TCAM_NOF_BANKS; ++ind)
  {
    info->is_present[ind] = 0;
  }
  for (ind = 0; ind < SOC_TMC_TCAM_NOF_BANKS; ++ind)
  {
    SOC_TMC_TCAM_DB_PREFIX_clear(&(info->prefix[ind]));
  }
  SOC_TMC_TCAM_DB_USER_clear(&(info->user));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_BANK_ENTRY_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_TCAM_BANK_ENTRY_SIZE_72_BITS:
    str = "size_72_bits";
  break;
  case SOC_TMC_TCAM_BANK_ENTRY_SIZE_144_BITS:
    str = "size_144_bits";
  break;
  case SOC_TMC_TCAM_BANK_ENTRY_SIZE_288_BITS:
    str = "size_288_bits";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_TCAM_BANK_ENTRY_SIZE_ARAD_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_BANK_ENTRY_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_TCAM_BANK_ENTRY_SIZE_80_BITS:
    str = "size_80_bits";
  break;
  case SOC_TMC_TCAM_BANK_ENTRY_SIZE_160_BITS:
    str = "size_160_bits";
  break;
  case SOC_TMC_TCAM_BANK_ENTRY_SIZE_320_BITS:
    str = "size_320_bits";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_TCAM_USER_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_USER enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_TCAM_USER_FWDING_IPV4_MC:
    str = "fwding_ipv4_mc";
  break;
  case SOC_TMC_TCAM_USER_FWDING_IPV6_UC:
    str = "fwding_ipv6_uc";
  break;
  case SOC_TMC_TCAM_USER_FWDING_IPV6_MC:
    str = "fwding_ipv6_mc";
  break;
  case SOC_TMC_TCAM_USER_FP:
    str = "ingress_fp";
  break;
  case SOC_TMC_TCAM_USER_EGRESS_ACL:
    str = "egress_acl";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_TCAM_BANK_OWNER_to_string(
    SOC_SAND_IN  SOC_TMC_TCAM_BANK_OWNER enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_TCAM_BANK_OWNER_PMF_0:
    str = "pmf-0";
  break;
  case SOC_TMC_TCAM_BANK_OWNER_PMF_1:
    str = "pmf-1";
  break;
  case SOC_TMC_TCAM_BANK_OWNER_FLP_TCAM:
    str = "flp-tcam";
  break;
  case SOC_TMC_TCAM_BANK_OWNER_FLP_TRAPS:
    str = "flp-traps";
  break;
  case SOC_TMC_TCAM_BANK_OWNER_VT:
    str = "vt";
  break;
  case SOC_TMC_TCAM_BANK_OWNER_TT:
    str = "tt";
  break;
  case SOC_TMC_TCAM_BANK_OWNER_EGRESS_ACL:
    str = "egress_acl";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_TCAM_DB_PREFIX_print(
    SOC_SAND_IN  SOC_TMC_TCAM_DB_PREFIX *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "val: %u\n\r"),info->val));
  LOG_CLI((BSL_META_U(unit,
                      "nof_bits: %u\n\r"),info->nof_bits));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_DB_USER_print(
    SOC_SAND_IN  SOC_TMC_TCAM_DB_USER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "priority: %u\n\r"),info->priority));
  LOG_CLI((BSL_META_U(unit,
                      "entry_size %s "), SOC_TMC_TCAM_BANK_ENTRY_SIZE_to_string(info->entry_size)));
  LOG_CLI((BSL_META_U(unit,
                      "prefix_length: %u\n\r"),info->prefix_length));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_TCAM_DB_INFO_print(
    SOC_SAND_IN  SOC_TMC_TCAM_DB_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_TMC_TCAM_NOF_BANKS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "is_present[%u]: %u\n\r"),ind,info->is_present[ind]));
  }
  for (ind = 0; ind < SOC_TMC_TCAM_NOF_BANKS; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "prefix[%u]:"),ind));
    SOC_TMC_TCAM_DB_PREFIX_print(&(info->prefix[ind]));
  }
  LOG_CLI((BSL_META_U(unit,
                      "user:")));
  SOC_TMC_TCAM_DB_USER_print(&(info->user));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

