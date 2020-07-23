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

#include <soc/dpp/PPC/ppc_api_rif.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>





















void
  SOC_PPC_RIF_MPLS_LABELS_RANGE_clear(
    SOC_SAND_OUT SOC_PPC_RIF_MPLS_LABELS_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_RIF_MPLS_LABELS_RANGE));
  info->first_label = 0;
  info->last_label = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_LABEL_RIF_KEY_clear(
    SOC_SAND_OUT SOC_PPC_MPLS_LABEL_RIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_MPLS_LABEL_RIF_KEY));
  info->label_id = 0;
  info->vsid = 0;
  info->label_id_second = 0;
  info->flags = 0;
  info->label_index = SOC_PPC_MPLS_LABEL_INDEX_ALL;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_RIF_INFO_clear(
    SOC_SAND_OUT SOC_PPC_RIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_RIF_INFO));

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_RIF_IP_TERM_INFO_clear(
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_RIF_IP_TERM_INFO));
  info->rif = 0;
  info->cos_profile = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_RIF_IP_TERM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_RIF_IP_TERM_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_RIF_IP_TERM_KEY));
  info->dip = 0;
  info->sip = 0;
  info->flags = 0;
  info->vrf = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_RIF_ROUTE_ENABLE_TYPE_to_string(
    SOC_SAND_IN  uint32 enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_UC:
    str = "ip_uc";
  break;
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_UC:
    str = "ipv4_uc";
  break;
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_UC:
    str = "ipv6_uc";
  break;
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IP_MC:
    str = "ip_mc";
  break;
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_MC:
    str = "ipv4_mc";
  break;
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV6_MC:
    str = "ipv6_mc";
  break;
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_MPLS:
    str = "mpls";
  break;
  case SOC_PPC_RIF_ROUTE_ENABLE_TYPE_ALL:
    str = "all";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_RIF_MPLS_LABELS_RANGE_print(
    SOC_SAND_IN  SOC_PPC_RIF_MPLS_LABELS_RANGE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "first_label: %u\n\r"),info->first_label));
  LOG_CLI((BSL_META_U(unit,
                      "last_label: %u\n\r"),info->last_label));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_MPLS_LABEL_RIF_KEY_print(
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_RIF_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "label_id: %u\n\r"),info->label_id));
  LOG_CLI((BSL_META_U(unit,
                      "vsid: %u\n\r"),info->vsid));
  LOG_CLI((BSL_META_U(unit,
                      "label_id_second: %u\n\r"),info->label_id_second));
  LOG_CLI((BSL_META_U(unit,
                      "flags: %u\n\r"),info->flags));  
  LOG_CLI((BSL_META_U(unit,
                      "label index: %s \n\r"), SOC_PPC_MPLS_LABEL_INDEX_to_string(info->label_index)));  
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_RIF_INFO_print(
    SOC_SAND_IN  SOC_PPC_RIF_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "\tuc_rpf_enable: %u\n\r"),info->uc_rpf_enable));
  LOG_CLI((BSL_META_U(unit,
                      "\tvrf_id: %u\n\r"),info->vrf_id));
  LOG_CLI((BSL_META_U(unit,
                      "\tcos_profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META_U(unit,
                      "\tttl_scope_index: %u\n\r"),info->ttl_scope_index));
  LOG_CLI((BSL_META_U(unit,
                      "\trouting_enablers_bm: %u\n\r"),info->routing_enablers_bm));

  LOG_CLI((BSL_META_U(unit,"\trouting_enablers: ")));

    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV4UC_ENABLED(info->routing_enablers_bm)){
        LOG_CLI((BSL_META_U(unit,"IPV4UC ")));
    }
    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV4MC_ENABLED(info->routing_enablers_bm)){
        LOG_CLI((BSL_META_U(unit,"IPV4MC ")));
    }
    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV6UC_ENABLED(info->routing_enablers_bm)){
        LOG_CLI((BSL_META_U(unit,"IPV6UC ")));      
    }
    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV6MC_ENABLED(info->routing_enablers_bm)){
        LOG_CLI((BSL_META_U(unit,"IPV6MC ")));
    }
    if(ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_MPLS_ENABLED(info->routing_enablers_bm)){
        LOG_CLI((BSL_META_U(unit,"MPLS")));
    }  
  LOG_CLI((BSL_META_U(unit,"\n")));

  LOG_CLI((BSL_META_U(unit, "\tenable_default_routing: %u\n\r"),info->enable_default_routing));
#ifdef BCM_88660_A0
  LOG_CLI((BSL_META_U(unit, "\tuc_rpf_mode: %u\n\r"),info->uc_rpf_mode));
#endif 

  LOG_CLI((BSL_META_U(unit, "\tRIF profile: %u\n\r"),info->routing_enablers_bm_id));
  LOG_CLI((BSL_META_U(unit, "\tintf_class: %u\n\r"),info->intf_class));
  
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_RIF_IP_TERM_INFO_print(
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->rif == SOC_PPC_RIF_NULL) {
      LOG_CLI((BSL_META_U(unit,
                          "rif: null\n")));
  }
  else{
      LOG_CLI((BSL_META_U(unit,
                          "rif: %u\n\r"),info->rif));
  }
  LOG_CLI((BSL_META_U(unit,
                      "term_cos_profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META_U(unit,
                      "tpid_profile: %u\n\r"),info->tpid_profile));
  LOG_CLI((BSL_META_U(unit,
                      "default_forward_profile: %u\n\r"),info->default_forward_profile));
  LOG_CLI((BSL_META_U(unit,
                      "vsi: %u\n\r"),info->vsi));
  LOG_CLI((BSL_META_U(unit,
                      "vsi_assignment_mode: %u\n\r"),info->vsi_assignment_mode));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pointer: %u\n\r"),info->protection_pointer));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pass_value: %u\n\r"),info->protection_pass_value));
  LOG_CLI((BSL_META_U(unit,
                      "oam_valid: %u\n\r"),info->oam_valid));
  LOG_CLI((BSL_META_U(unit,
                      "protection_pass_value: %u\n\r"),info->protection_pass_value));
  LOG_CLI((BSL_META_U(unit,
                      "orientation %s \n\r"), soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(info->orientation)));
  LOG_CLI((BSL_META_U(unit,
                      "model: %s\n\r"),soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(info->processing_type)));
  LOG_CLI((BSL_META_U(unit,
                      "learn_info:")));
  LOG_CLI((BSL_META_U(unit,
                      "learn_enable: %u\n\r"),info->learn_enable));
  SOC_PPC_FRWRD_DECISION_INFO_print(&(info->learn_record));

  
  LOG_CLI((BSL_META_U(unit,
                      "\n\rlif_profile: %u\n\r"),info->lif_profile));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_RIF_IP_TERM_KEY_print(
    SOC_SAND_IN  SOC_PPC_RIF_IP_TERM_KEY *key
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(key);

  LOG_CLI((BSL_META_U(unit,
                      "dip: 0x%08x\n\r"),key->dip));
  LOG_CLI((BSL_META_U(unit,
                      "dip_prefix_len: 0x%08x\n\r"),key->dip_prefix_len));

  LOG_CLI((BSL_META_U(unit,
                      "sip: 0x%08x\n\r"),key->sip));
  LOG_CLI((BSL_META_U(unit,
                      "sip_prefix_len: 0x%08x\n\r"),key->sip_prefix_len));
  
  LOG_CLI((BSL_META_U(unit,
                      "flags: 0x%08x\n\r"),key->flags));

  LOG_CLI((BSL_META_U(unit,
                      "vrf: 0x%08x\n\r"),key->vrf));
  LOG_CLI((BSL_META_U(unit,
                      "ipv4_next_protocol: 0x%08x\n\r"),key->ipv4_next_protocol));
  LOG_CLI((BSL_META_U(unit,
                      "ipv4_next_protocol_prefix_len: 0x%08x\n\r"),key->ipv4_next_protocol_prefix_len));
  LOG_CLI((BSL_META_U(unit,
                      "port_property_en: %d\n\r"), key->port_property_en));
  if (key->port_property_en) {
      LOG_CLI((BSL_META_U(unit,
                          "port_property: %d\n\r"), key->port_property));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  SOC_PPC_MPLS_LABEL_INDEX_to_string(
    SOC_SAND_IN  SOC_PPC_MPLS_LABEL_INDEX enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_MPLS_LABEL_INDEX_ALL:
    str = "unindex";
  break;
  case SOC_PPC_MPLS_LABEL_INDEX_FIRST:
    str = "first";
  break;
  case SOC_PPC_MPLS_LABEL_INDEX_SECOND:
    str = "second";
  break;
  case SOC_PPC_MPLS_LABEL_INDEX_THIRD:
    str = "third";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

