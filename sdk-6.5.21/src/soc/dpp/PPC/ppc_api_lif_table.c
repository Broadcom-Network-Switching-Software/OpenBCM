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

#include <soc/dpp/PPC/ppc_api_lif_table.h>





















void
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_PER_TYPE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_ENTRY_PER_TYPE_INFO));
  SOC_PPC_L2_LIF_AC_INFO_clear(&(info->ac));
  SOC_PPC_L2_LIF_PWE_INFO_clear(&(info->pwe));
  SOC_PPC_L2_LIF_ISID_INFO_clear(&(info->isid));
  SOC_PPC_RIF_INFO_clear(&(info->rif));
  SOC_PPC_RIF_IP_TERM_INFO_clear(&(info->ip_term_info));
  SOC_PPC_MPLS_TERM_INFO_clear(&(info->mpls_term_info));
  SOC_PPC_L2_LIF_TRILL_INFO_clear(&(info->trill));
  SOC_PPC_L2_LIF_EXTENDER_INFO_clear(&(info->extender));
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_ENTRY_INFO));
  info->type = SOC_PPC_NOF_LIF_ENTRY_TYPES;
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_clear(&(info->value));
  info->index = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_clear(
    SOC_SAND_OUT SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE));
  info->entries_type_bm = 0;
  info->accessed_only = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO));
  info->accessed = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_LIF_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_LIF_ENTRY_TYPE_EMPTY:
    str = "empty";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_AC:
    str = "ac";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP:
    str = "f_ac";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_MIDDLE_AC_IN_GROUP:
    str = "m_ac";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_PWE:
    str = "pwe";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_ISID:
    str = "isid";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
    str = "ip";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
    str = "mpls";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK:
    str = "trill";
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
    str = "extender";
    break;
  case SOC_PPC_LIF_ENTRY_TYPE_ALL:
    str = "all";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO *info,
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_TYPE  type
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(type)
  {
  case SOC_PPC_LIF_ENTRY_TYPE_AC:
    LOG_CLI((BSL_META_U(unit,
                        "ac:")));
    SOC_PPC_L2_LIF_AC_INFO_print(&(info->ac));
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_PWE:
    LOG_CLI((BSL_META_U(unit,
                        "pwe:")));
    SOC_PPC_L2_LIF_PWE_INFO_print(&(info->pwe));
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_ISID:
    LOG_CLI((BSL_META_U(unit,
                        "isid:")));
    SOC_PPC_L2_LIF_ISID_INFO_print(&(info->isid));
    break;
  case SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF:
  case SOC_PPC_LIF_ENTRY_TYPE_IP_TUNNEL_RIF:
    LOG_CLI((BSL_META_U(unit,
                        "rif:")));
    SOC_PPC_RIF_INFO_print(&(info->rif));
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK:
    LOG_CLI((BSL_META_U(unit,
                        "trill:")));
    SOC_PPC_L2_LIF_TRILL_INFO_print(&(info->trill));
  break;
  case SOC_PPC_LIF_ENTRY_TYPE_EXTENDER:
    LOG_CLI((BSL_META_U(unit,
                        "extender:")));
    SOC_PPC_L2_LIF_EXTENDER_INFO_print(&(info->extender));
  break;
	default:
		break;
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_ENTRY_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "type %s "), SOC_PPC_LIF_ENTRY_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "value:")));
  SOC_PPC_LIF_ENTRY_PER_TYPE_INFO_print(&(info->value),info->type);
  LOG_CLI((BSL_META_U(unit,
                      "index: %u\n\r"),info->index));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE_print(
    SOC_SAND_IN  SOC_PPC_LIF_TBL_TRAVERSE_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "entries_type_bm: %u\n\r"),info->entries_type_bm));
  LOG_CLI((BSL_META_U(unit,
                      "accessed_only: %u\n\r"),info->accessed_only));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO_print(
    SOC_SAND_IN  SOC_PPC_LIF_TABLE_ENTRY_ACCESSED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "accessed: %u\n\r"),info->accessed));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

