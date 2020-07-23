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

#include <soc/dpp/PPC/ppc_api_eg_qos.h>





















void
  SOC_PPC_EG_QOS_MAP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_QOS_MAP_KEY));
  info->in_dscp_exp = 0;
  info->dp = 0;
  info->remark_profile = 0;  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_QOS_MAP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_QOS_MAP_KEY));
  info->in_dscp_exp = 0;
  info->remark_profile = 0;  
  info->pkt_hdr_type = 0;

  info->dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_QOS_PARAMS));
  info->ipv4_tos = 0;
  info->ipv6_tc = 0;
  info->mpls_exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_QOS_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_ENCAP_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_ENCAP_QOS_PARAMS));
  info->ip_dscp = 0;
  info->mpls_exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PHP_REMARK_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PHP_REMARK_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_QOS_PHP_REMARK_KEY));
  info->exp_map_profile = 0;
  info->php_type = SOC_PPC_NOF_EG_QOS_UNIFORM_PHP_TYPES;
  info->exp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_EG_QOS_PORT_INFO));
  info->exp_map_profile = 0;
#ifdef BCM_88660
  info->marking_profile = 0;
#endif
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#ifdef BCM_88660
void
  SOC_PPC_EG_QOS_MARKING_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MARKING_KEY *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(*info));

    SOC_SAND_MAGIC_NUM_SET;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_MARKING_PARAMS_clear(
    SOC_SAND_OUT SOC_PPC_EG_QOS_MARKING_PARAMS *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);
    sal_memset(info, 0x0, sizeof(*info));

    SOC_SAND_MAGIC_NUM_SET;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void SOC_PPC_EG_QOS_GLOBAL_INFO_clear(SOC_SAND_OUT SOC_PPC_EG_QOS_GLOBAL_INFO *info)
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
  SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV4:
    str = "soc_ppc_eg_qos_uniform_php_pop_into_ipv4";
  break;
  case SOC_PPC_EG_QOS_UNIFORM_PHP_POP_INTO_IPV6:
    str = "soc_ppc_eg_qos_uniform_php_pop_into_ipv6";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_EG_QOS_MAP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_dscp_exp: %u\n\r"),info->in_dscp_exp));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
  LOG_CLI((BSL_META_U(unit,
                      "remark_profile: %u\n\r"), info->remark_profile));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_QOS_MAP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_MAP_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "in_dscp_exp: %u\n\r"),info->in_dscp_exp));
  LOG_CLI((BSL_META_U(unit,
                      "remark_profile: %u\n\r"), info->remark_profile));
  LOG_CLI((BSL_META_U(unit,
                      "pkt_header_type: %u\n\r"), info->pkt_hdr_type));
  if (info->pkt_hdr_type == SOC_PPC_PKT_HDR_TYPE_ETH) 
  {
    
    LOG_CLI((BSL_META_U(unit,
                        "dp: %u\n\r"), info->dp));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PARAMS_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ipv4_tos: %u\n\r"),info->ipv4_tos));
  LOG_CLI((BSL_META_U(unit,
                      "ipv6_tc: %u\n\r"), info->ipv6_tc));
  LOG_CLI((BSL_META_U(unit,
                      "mpls_exp: %u\n\r"), info->mpls_exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_ENCAP_QOS_PARAMS_print(
    SOC_SAND_IN  SOC_PPC_EG_ENCAP_QOS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ip_dscp: %u\n\r"),info->ip_dscp));
  LOG_CLI((BSL_META_U(unit,
                      "mpls_exp: %u\n\r"), info->mpls_exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PHP_REMARK_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PHP_REMARK_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "exp_map_profile: %u\n\r"),info->exp_map_profile));
  LOG_CLI((BSL_META_U(unit,
                      "php_type %s "), SOC_PPC_EG_QOS_UNIFORM_PHP_TYPE_to_string(info->php_type)));
  LOG_CLI((BSL_META_U(unit,
                      "exp: %u\n\r"), info->exp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_PORT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_QOS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "exp_map_profile: %u\n\r"),info->exp_map_profile));
#ifdef BCM_88660
  LOG_CLI((BSL_META_U(unit,
                      "marking_profile: %u\n\r"),info->marking_profile));
#endif
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#ifdef BCM_88660
void
  SOC_PPC_EG_QOS_MARKING_KEY_print(
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
    
  LOG_CLI((BSL_META_U(unit,
                      "resolved_dp_ndx: %u\n\r"), info->resolved_dp_ndx ));
  LOG_CLI((BSL_META_U(unit,
                      "tc_ndx: %u\n\r"), info->tc_ndx ));
  LOG_CLI((BSL_META_U(unit,
                      "in_lif_profile: %u\n\r"), info->in_lif_profile ));
  LOG_CLI((BSL_META_U(unit,
                      "marking_profile: %u\n\r"), info->marking_profile ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_EG_QOS_MARKING_PARAMS_print(
    SOC_SAND_IN SOC_PPC_EG_QOS_MARKING_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ip_dscp: %u\n\r"), info->ip_dscp ));
  LOG_CLI((BSL_META_U(unit,
                      "mpls_exp: %u\n\r"), info->mpls_exp ));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void SOC_PPC_EG_QOS_GLOBAL_INFO_print(SOC_SAND_IN SOC_PPC_EG_QOS_GLOBAL_INFO *info)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dp_map_mode: 0x%x\n\r"), info->dp_map_mode));
  LOG_CLI((BSL_META_U(unit,
                      "resolved_in_lif_profile_bitmap: 0x%x\n\r"), info->resolved_in_lif_profile_bitmap)); 
  LOG_CLI((BSL_META_U(unit,
                      "inlif_profile_bitmap: 0x%x\n\r"), info->in_lif_profile_bitmap));
  LOG_CLI((BSL_META_U(unit,
                      "resolved_dp_bitmap: 0x%x\n\r"), info->resolved_dp_bitmap));
 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
 
}

#endif 

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

