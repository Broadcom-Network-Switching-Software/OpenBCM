/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/






#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_multicast_egress.h>
#include <soc/dpp/TMC/tmc_api_general.h>


























void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE));
  info->mc_id_low = 0;
  info->mc_id_high = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_EG_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_EG_ENTRY));
  info->type = SOC_TMC_MULT_EG_ENTRY_TYPE_OFP;
  info->vlan_mc_id = 0;
  info->cud = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_MULT_EG_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_MULT_EG_ENTRY_TYPE_OFP:
    str = "ofp";
  break;
  case SOC_TMC_MULT_EG_ENTRY_TYPE_VLAN_PTR:
    str = "vlan_ptr";
  break;
  case SOC_TMC_MULT_EG_ENTRY_NOF_TYPES:
    str = "nof_types";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Mc_id_low:  %u, Mc_id_high: %u\n\r"),info->mc_id_low, info->mc_id_high));
  LOG_CLI((BSL_META_U(unit,
                      "Mc_id_high: %u\n\r"),info->mc_id_high));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_EG_ENTRY_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_ENTRY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Type %s "), SOC_TMC_MULT_EG_ENTRY_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "Port-id: %u, Copy-unique-data: %u\n\r"),info->port, info->cud));
  LOG_CLI((BSL_META_U(unit,
                      "Vlan_mc_id: %u\n\r"),info->vlan_mc_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  )
{
  uint32
    ind=0,
    bit_idx,
    reg_idx,
    bit_val,
    cnt = 0;
  uint8
    found = FALSE;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Member FAP Ports:\n\r")));
  for (ind=0; ind<SOC_TMC_NOF_FAP_PORTS_MAX; ++ind) 
  {
    bit_idx = ind % SOC_SAND_REG_SIZE_BITS;
    reg_idx = ind / SOC_SAND_REG_SIZE_BITS;
    bit_val = SOC_SAND_GET_BIT(info->bitmap[reg_idx], bit_idx);
    if (bit_val)
    {
      found = TRUE;
      LOG_CLI((BSL_META_U(unit,
                          "%02u "), ind));

      if ((++cnt % 10) == 0)
      {
        LOG_CLI((BSL_META_U(unit,
                            "\n\r")));
      }
    }
  }
  if (!found)
  {
    LOG_CLI((BSL_META_U(unit,
                        "None")));
  }
  else
  {
    LOG_CLI((BSL_META_U(unit,
                        "\n\rTotal: %u"), cnt));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

