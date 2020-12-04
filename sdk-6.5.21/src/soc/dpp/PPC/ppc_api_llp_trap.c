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

#include <soc/dpp/PPC/ppc_api_llp_trap.h>





















void
  SOC_PPC_LLP_TRAP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_TRAP_PORT_INFO));
  info->reserved_mc_profile = 0;
  info->trap_enable_mask = SOC_PPC_LLP_TRAP_PORT_ENABLE_ING_VLAN_MEMBERSHIP;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_ARP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_ARP_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_TRAP_ARP_INFO));
  for (ind = 0; ind < SOC_PPC_TRAP_NOF_MY_IPS; ++ind)
  {
    info->my_ips[ind] = 0;
  }
  info->ignore_da = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_RESERVED_MC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_TRAP_RESERVED_MC_KEY));
  info->reserved_mc_profile = 0;
  info->da_mac_address_lsb = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO));
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&(info->dest_mac));
  info->dest_mac_nof_bits = 0;
  info->ether_type = 0;
  info->sub_type = 0;
  info->sub_type_bitmap = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO));
  info->ip_protocol = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO));
  info->src_port = 0;
  info->src_port_bitmap = 0;
  info->dest_port = 0;
  info->dest_port_bitmap = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_clear(
    SOC_SAND_OUT SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER));
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_clear(&(info->l2_info));
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_clear(&(info->l3_info));
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_clear(&(info->l4_info));
  info->enable_bitmap = 0;
  info->inverse_bitmap = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LLP_TRAP_PORT_ENABLE_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_ENABLE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_NONE:
    str = "none";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_ARP:
    str = "arp";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_IGMP:
    str = "igmp";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_MLD:
    str = "mld";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_DHCP:
    str = "dhcp";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0:
    str = "prog_trap_0";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_1:
    str = "prog_trap_1";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_2:
    str = "prog_trap_2";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_3:
    str = "prog_trap_3";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_ING_VLAN_MEMBERSHIP:
    str = "port_vlan_membership";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_SAME_INTERFACE:
    str = "same_interface";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_ICMP_REDIRECT:
    str = "icmp_redirect";
  break;
  case SOC_PPC_LLP_TRAP_PORT_ENABLE_ALL:
    str = "all";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_to_string(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_NONE:
    str = "none";
  break;
  case SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_DA:
    str = "da";
  break;
  case SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_ETHER_TYPE:
    str = "ether_type";
  break;
  case SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_SUB_TYPE:
    str = "sub_type";
  break;
  case SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_IP_PRTCL:
    str = "ip_prtcl";
  break;
  case SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_L4_PORTS:
    str = "l4_ports";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LLP_TRAP_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PORT_INFO *info
  )
{
  uint32
    indx;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "reserved_mc_profile: %u\n\r"),info->reserved_mc_profile));
  LOG_CLI((BSL_META_U(unit,
                      "trap_enable_mask: %0xlx\n\r"),info->trap_enable_mask));

  LOG_CLI((BSL_META_U(unit,
                      "enabled_traps: \n\r")));
  for (indx = 0; indx < SOC_PPC_NOF_LLP_TRAP_PORT_ENABLES-2; ++indx)
  {
    if (SOC_SAND_BIT(indx) & info->trap_enable_mask)
    {
      LOG_CLI((BSL_META_U(unit,
                          " - %s\n\r"), SOC_PPC_LLP_TRAP_PORT_ENABLE_to_string((SOC_PPC_LLP_TRAP_PORT_ENABLE)(SOC_SAND_BIT(indx)))));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "\n\rdisabled_traps: \n\r")));
  for (indx = 0; indx < SOC_PPC_NOF_LLP_TRAP_PORT_ENABLES-2; ++indx)
  {
    if ((SOC_SAND_BIT(indx) & info->trap_enable_mask) == 0)
    {
      LOG_CLI((BSL_META_U(unit,
                          " - %s\n\r"), SOC_PPC_LLP_TRAP_PORT_ENABLE_to_string((SOC_PPC_LLP_TRAP_PORT_ENABLE)(SOC_SAND_BIT(indx)))));
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_ARP_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_ARP_INFO *info
  )
{
  char
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PPC_TRAP_NOF_MY_IPS; ++ind)
  {
    soc_sand_pp_ip_long_to_string(info->my_ips[ind],1,decimal_ip);
    LOG_CLI((BSL_META_U(unit,
                        "my-IP[%u]: %s\n\r"), ind, decimal_ip));
  }
  LOG_CLI((BSL_META_U(unit,
                      "ignore_da: %u\n\r"),info->ignore_da));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_RESERVED_MC_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "reserved_mc_profile: %u\n\r"),info->reserved_mc_profile));
  LOG_CLI((BSL_META_U(unit,
                      "da_mac_address_lsb: %u\n\r"),info->da_mac_address_lsb));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest_mac: ")));
  soc_sand_SAND_PP_MAC_ADDRESS_print(&(info->dest_mac));
  LOG_CLI((BSL_META_U(unit,
                      "  nof_bits: %u\n\r"), info->dest_mac_nof_bits));
  LOG_CLI((BSL_META_U(unit,
                      "ether_type: 0x%04x\n\r"),info->ether_type));
  LOG_CLI((BSL_META_U(unit,
                      "sub_type: %u  "), info->sub_type));
  LOG_CLI((BSL_META_U(unit,
                      "bitmap: 0x%02x\n\r"), info->sub_type_bitmap));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ip_protocol: %u\n\r"), info->ip_protocol));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "src_port: %u"),info->src_port));
  LOG_CLI((BSL_META_U(unit,
                      "  bitmap: 0x%04x\n\r"),info->src_port_bitmap));
  LOG_CLI((BSL_META_U(unit,
                      "dest_port: %u"),info->dest_port));
  LOG_CLI((BSL_META_U(unit,
                      "  bitmap: 0x%04x\n\r"),info->dest_port_bitmap));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER_print(
    SOC_SAND_IN  SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "\n\r - l2_info:\n\r")));
  SOC_PPC_LLP_TRAP_PROG_TRAP_L2_INFO_print(&(info->l2_info));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r - l3_info:\n\r")));
  SOC_PPC_LLP_TRAP_PROG_TRAP_L3_INFO_print(&(info->l3_info));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r - l4_info:\n\r")));
  SOC_PPC_LLP_TRAP_PROG_TRAP_L4_INFO_print(&(info->l4_info));

  for (indx = 0; indx < SOC_PPC_NOF_LLP_TRAP_PROG_TRAP_COND_SELECTS-1; ++indx)
  {
    LOG_CLI((BSL_META_U(unit,
                        " - %s : "), SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT_to_string((SOC_PPC_LLP_TRAP_PROG_TRAP_COND_SELECT)(SOC_SAND_BIT(indx)))));
    if (SOC_SAND_BIT(indx) & info->enable_bitmap)
    {
      LOG_CLI((BSL_META_U(unit,
                          "  Check")));
      if (SOC_SAND_BIT(indx) & info->inverse_bitmap)
      {
        LOG_CLI((BSL_META_U(unit,
                            " inverse")));
      }
    }
    else
    {
      LOG_CLI((BSL_META_U(unit,
                          "  No check")));
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n\r")));
  }
  

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

