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

#include <soc/dpp/PPC/ppc_api_trap_mgmt.h>
#include <soc/dpp/PPC/ppc_api_metering.h>

#include <bcm_int/dpp/rx.h>
#include <bcm/rx.h>























void 
  SOC_PPC_TRAP_MGMT_PACKET_PARSE_INFO_clear(
      SOC_SAND_OUT SOC_PPC_TRAP_MGMT_PACKET_PARSE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_MGMT_PACKET_PARSE_INFO));
  info->dest_ext_mode = SOC_PPC_TRAP_MGMT_FTMH_DEST_EXT_MODE_NEVER;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO));
  info->is_oam_dest = 0;
  SOC_PPC_FRWRD_DECISION_INFO_clear(&(info->frwrd_dest));
  info->add_vsi = 0;
  info->vsi_shift = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO));
  info->counter_select = 0;
  info->counter_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO));
  info->meter_select = 0;
  info->meter_id = 0;
  info->meter_command = SOC_PPC_MTR_RES_USE_NONE;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO));
  info->ethernet_police_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO));
  info->tc = 0;
  info->dp = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO));
  info->enable_learning = TRUE;
  info->is_trap = 0;
  info->is_control = 0;
  info->frwrd_offset_index = 0;
  info->frwrd_type = SOC_PPC_PKT_FRWRD_TYPE_BRIDGE;
  info->frwrd_offset_bytes_fix = 0;
  info->da_type = SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO));
  info->strength = 0;
  info->tunnel_termination_strength = 0;
  info->bitmap_mask = 0;
  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO_clear(&(info->dest_info));
  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_clear(&(info->cos_info));
  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO_clear(&(info->count_info));
  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO_clear(&(info->meter_info));
  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO_clear(&(info->policing_info));
  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO_clear(&(info->processing_info));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO));
  info->strength = 0;
  info->tunnel_termination_strength = 0;
  info->snoop_cmnd = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ETPP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_ETPP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_ETPP_INFO));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_EG_ACTION_HDR_DATA_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_EG_ACTION_HDR_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_EG_ACTION_HDR_DATA));
  info->cos_profile = 0;
  info->dsp_ptr = 0;
  info->mirror_profile = 0;
  info->cud = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO));
  info->bitmap_mask = 0;
  info->out_tm_port = 0;
  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_clear(&(info->cos_info));
  info->cud = 0;
  SOC_PPC_TRAP_EG_ACTION_HDR_DATA_clear(&(info->header_data));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_MACT_EVENT_INFO));
  info->type = SOC_PPC_NOF_TRAP_MACT_EVENT_TYPES;
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&(info->key));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&(info->val));
  info->is_part_of_lag = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_PACKET_INFO_clear(
    SOC_SAND_OUT SOC_PPC_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PPC_TRAP_PACKET_INFO));
  info->is_trapped = 0;
  info->cpu_trap_code = SOC_PPC_NOF_TRAP_CODES;
  info->cpu_trap_qualifier = 0;
  info->src_sys_port = 0;
  info->ntwrk_header_ptr = 0;
  info->user_header_ptr[0] = 0;
  info->user_header_ptr[1] = 0;
  info->size_in_bytes = 0;
  info->snoop_cmnd = 0;
  info->frwrd_type = SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE_NORMAL;
  info->in_ac = 0;
  info->rif = 0;
  info->vrf = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

const char*
  SOC_PPC_TRAP_SNOOP_ACTION_SIZE_to_string(
    SOC_SAND_IN  SOC_PPC_TRAP_SNOOP_ACTION_SIZE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_TRAP_SNOOP_ACTION_SIZE_64B:
    str = "size_64b";
  break;
  case SOC_PPC_TRAP_SNOOP_ACTION_SIZE_128B:
    str = "size_128b";
  break;
  case SOC_PPC_TRAP_SNOOP_ACTION_SIZE_FULL_PACKET:
    str = "full_packet";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_to_string(
    SOC_SAND_IN  SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_NONE:
    str = "none";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST:
    str = "dest";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_TC:
    str = "tc";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP:
    str = "dp";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_POLICER:
    str = "policer";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_OFFSET:
    str = "frwrd_offset";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_CUD:
    str = "cud";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DP_METER_CMD:
    str = "meter_cmd";
    break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_ALL:
    str = "all";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST_EEI:
    str = "dest_eei";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DEST_OUTLIF:
    str = "dest_outlif";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_FRWRD_TYPE:
    str = "frwrd_type";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_DA_TYPE:
    str = "da_type";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_METER_0:
    str = "meter_0";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_METER_1:
    str = "meter_1";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_0:
    str = "counter_0";
  break;
  case SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_COUNTER_1:
    str = "counter_1";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_TRAP_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_TRAP_MACT_EVENT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_NONE:
    str = "none";
  break;
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_AGED_OUT:
    str = "aged_out";
  break;
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_LEARN:
    str = "learn";
  break;
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_TRANSPLANT:
    str = "transplant";
  break;
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_REFRESH:
    str = "refresh";
  break;
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_ACK:
    str = "ack";
  break;
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_RETRIEVE:
    str = "retrieve";
  break;
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_LIMIT_EXCEED:
    str = "limit_exceed";
  break;
  case SOC_PPC_TRAP_MACT_EVENT_TYPE_CPU_REQ_FAIL:
    str = "cpu_req_fail";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
SOC_PPC_TRAP_CODE_to_api_string(SOC_SAND_IN SOC_PPC_TRAP_CODE ppc_trap_code){
    const char* str = NULL;
    uint8 found;
    bcm_rx_trap_t bcm_trap_code;

    _bcm_dpp_rx_trap_type_get_from_array(ppc_trap_code, &bcm_trap_code, &found);

    switch (bcm_trap_code) {
      case bcmRxTrapMimDiscardMacsaFwd:
        str = "bcmRxTrapMimDiscardMacsaFwd";
        break;
      case bcmRxTrapMimDiscardMacsaDrop:
        str = "bcmRxTrapMimDiscardMacsaDrop";
        break;
      case bcmRxTrapMimDiscardMacsaTrap:
        str = "bcmRxTrapMimDiscardMacsaTrap";
        break;
      case bcmRxTrapMimDiscardMacsaSnoop:
        str = "bcmRxTrapMimDiscardMacsaSnoop";
        break;
      case bcmRxTrapMimTeSaMove:
        str = "bcmRxTrapMimTeSaMove";
        break;
      case bcmRxTrapMimTeSaUnknown:
        str = "bcmRxTrapMimTeSaUnknown";
        break;
      case bcmRxTrapMimSaMove:
        str = "bcmRxTrapMimSaMove";
        break;
      case bcmRxTrapMimSaUnknown:
        str = "bcmRxTrapMimSaUnknown";
        break;
      case bcmRxTrapAuthSaLookupFail:
        str = "bcmRxTrapAuthSaLookupFail";
        break;
      case bcmRxTrapAuthSaPortFail:
        str = "bcmRxTrapAuthSaPortFail";
        break;
      case bcmRxTrapAuthSaVlanFail:
        str = "bcmRxTrapAuthSaVlanFail";
        break;
      case bcmRxTrapSaMulticast:
        str = "bcmRxTrapSaMulticast";
        break;
      case bcmRxTrapSaEqualsDa:
        str = "bcmRxTrapSaEqualsDa";
        break;
      case bcmRxTrap8021xFail:
        str = "bcmRxTrap8021xFail";
        break;
      case bcmRxTrapArpMyIp:
        str = "bcmRxTrapArpMyIp";
        break;
      case bcmRxTrapArp:
        str = "bcmRxTrapArp";
        break;
      case bcmRxTrapIgmpMembershipQuery:
        str = "bcmRxTrapIgmpMembershipQuery";
        break;
      case bcmRxTrapIgmpReportLeaveMsg:
        str = "bcmRxTrapIgmpReportLeaveMsg";
        break;
      case bcmRxTrapIgmpUndefined:
        str = "bcmRxTrapIgmpUndefined";
        break;
      case bcmRxTrapIcmpv6MldMcListenerQuery:
        str = "bcmRxTrapIcmpv6MldMcListenerQuery";
        break;
      case bcmRxTrapL2Cache:
        str = "bcmRxTrapL2Cache";
        break;
      case bcmRxTrapDhcpv6Server:
        str = "bcmRxTrapDhcpv6Server";
        break;
      case bcmRxTrapDhcpv6Client:
        str = "bcmRxTrapDhcpv6Client";
        break;
      case bcmRxTrapPortNotVlanMember:
        str = "bcmRxTrapPortNotVlanMember";
        break;
      case bcmRxTrapHeaderSizeErr:
        str = "bcmRxTrapHeaderSizeErr";
        break;
      case bcmRxTrapMyBmacUknownTunnel:
        str = "bcmRxTrapMyBmacUknownTunnel";
        break;
      case bcmRxTrapMyBmacUnknownISid:
        str = "bcmRxTrapMyBmacUnknownISid";
        break;
      case bcmRxTrapStpStateBlock:
        str = "bcmRxTrapStpStateBlock";
        break;
      case bcmRxTrapStpStateLearn:
        str = "bcmRxTrapStpStateLearn";
        break;
      case bcmRxTrapIpCompMcInvalidIp:
        str = "bcmRxTrapIpCompMcInvalidIp";
        break;
      case bcmRxTrapMyMacAndIpDisabled:
        str = "bcmRxTrapMyMacAndIpDisabled";
        break;
      case bcmRxTrapTrillVersion:
        str = "bcmRxTrapTrillVersion";
        break;
      case bcmRxTrapTrillTtl0:
        str = "bcmRxTrapTrillTtl0";
        break;
      case bcmRxTrapTrillChbh:
        str = "bcmRxTrapTrillChbh";
        break;
      case bcmRxTrapTrillUnknonwnIngressNickname:
        str = "bcmRxTrapTrillUnknonwnIngressNickname";
        break;
      case bcmRxTrapTrillCite:
        str = "bcmRxTrapTrillCite";
        break;
      case bcmRxTrapTrillIllegalInnerMc:
        str = "bcmRxTrapTrillIllegalInnerMc";
        break;
      case bcmRxTrapMyMacAndMplsDisable:
        str = "bcmRxTrapMyMacAndMplsDisable";
        break;
      case bcmRxTrapArpReply:
        str = "bcmRxTrapArpReply";
        break;
      case bcmRxTrapMyMacAndUnknownL3:
        str = "bcmRxTrapMyMacAndUnknownL3";
        break;
      case bcmRxTrapMplsLabel0150:
        str = "bcmRxTrapMplsLabel0150";
        break;
      case bcmRxTrapMplsLabel0151:
        str = "bcmRxTrapMplsLabel0151";
        break;
      case bcmRxTrapMplsLabel0152:
        str = "bcmRxTrapMplsLabel0152";
        break;
      case bcmRxTrapMplsLabel0153:
        str = "bcmRxTrapMplsLabel0153";
        break;
      case bcmRxTrapMplsTerminationFail:
        str = "bcmRxTrapMplsTerminationFail";
        break;
      case bcmRxTrapMplsUnexpectedBos:
        str = "bcmRxTrapMplsUnexpectedBos";
        break;
      case bcmRxTrapMplsUnexpectedNoBos:
        str = "bcmRxTrapMplsUnexpectedNoBos";
        break;
      case bcmRxTrapCfmAcceleratedIngress:
        str = "bcmRxTrapCfmAcceleratedIngress";
        break;
      case bcmRxTrapIllegelPacketFormat:
        str = "bcmRxTrapIllegelPacketFormat";
        break;
      case bcmRxTrapL2DiscardMacsaFwd:
        str = "bcmRxTrapL2DiscardMacsaFwd";
        break;
      case bcmRxTrapL2DiscardMacsaDrop:
        str = "bcmRxTrapL2DiscardMacsaDrop";
        break;
      case bcmRxTrapL2DiscardMacsaTrap:
        str = "bcmRxTrapL2DiscardMacsaTrap";
        break;
      case bcmRxTrapL2DiscardMacsaSnoop:
        str = "bcmRxTrapL2DiscardMacsaSnoop";
        break;
      case bcmRxTrapL2Learn0:
        str = "bcmRxTrapL2Learn0";
        break;
      case bcmRxTrapL2Learn1:
        str = "bcmRxTrapL2Learn1";
        break;
      case bcmRxTrapL2Learn2:
        str = "bcmRxTrapL2Learn2";
        break;
      case bcmRxTrapL2Learn3:
        str = "bcmRxTrapL2Learn3";
        break;
      case bcmRxTrapVlanUnknownDa:
        str = "bcmRxTrapVlanUnknownDa";
        break;
      case bcmRxTrapExternalLookupError:
        str = "bcmRxTrapExternalLookupError";
        break;
      case bcmRxTrapL2DlfFwd:
        str = "bcmRxTrapL2DlfFwd";
        break;
      case bcmRxTrapL2DlfDrop:
        str = "bcmRxTrapL2DlfDrop";
        break;
      case bcmRxTrapL2DlfTrap:
        str = "bcmRxTrapL2DlfTrap";
        break;
      case bcmRxTrapL2DlfSnoop:
        str = "bcmRxTrapL2DlfSnoop";
        break;
      case bcmRxTrapExtendedP2pLookupFail:
        str = "bcmRxTrapExtendedP2pLookupFail";
        break;
      case bcmRxTrapSameInterface:
        str = "bcmRxTrapSameInterface";
        break;
      case bcmRxTrapTrillUnknownUc:
        str = "bcmRxTrapTrillUnknownUc";
        break;
      case bcmRxTrapTrillUnknownMc:
        str = "bcmRxTrapTrillUnknownMc";
        break;
      case bcmRxTrapUcLooseRpfFail:
        str = "bcmRxTrapUcLooseRpfFail";
        break;
      case bcmRxTrapDefaultUcv6:
        str = "bcmRxTrapDefaultUcv6";
        break;
      case bcmRxTrapDefaultMcv6:
        str = "bcmRxTrapDefaultMcv6";
        break;
      case bcmRxTrapMplsP2pNoBos:
        str = "bcmRxTrapMplsP2pNoBos";
        break;
      case bcmRxTrapMplsControlWordTrap:
        str = "bcmRxTrapMplsControlWordTrap";
        break;
      case bcmRxTrapMplsControlWordDrop:
        str = "bcmRxTrapMplsControlWordDrop";
        break;
      case bcmRxTrapMplsUnknownLabel:
        str = "bcmRxTrapMplsUnknownLabel";
        break;
      case bcmRxTrapMplsExtendP2pMplsx4:
        str = "bcmRxTrapMplsExtendP2pMplsx4";
        break;
      case bcmRxTrapFrwrdIgmpMembershipQuery:
        str = "bcmRxTrapFrwrdIgmpMembershipQuery";
        break;
      case bcmRxTrapFrwrdIgmpReportLeaveMsg:
        str = "bcmRxTrapFrwrdIgmpReportLeaveMsg";
        break;
      case bcmRxTrapFrwrdIgmpUndefined:
        str = "bcmRxTrapFrwrdIgmpUndefined";
        break;
      case bcmRxTrapFrwrdIcmpv6MldMcListenerQuery:
        str = "bcmRxTrapFrwrdIcmpv6MldMcListenerQuery";
        break;
      case bcmRxTrapFrwrdIcmpv6MldReportDone:
        str = "bcmRxTrapFrwrdIcmpv6MldReportDone";
        break;
      case bcmRxTrapFrwrdIcmpv6MldUndefined:
        str = "bcmRxTrapFrwrdIcmpv6MldUndefined";
        break;
      case bcmRxTrapIpv4VersionError:
        str = "bcmRxTrapIpv4VersionError";
        break;
      case bcmRxTrapIpv4ChecksumError:
        str = "bcmRxTrapIpv4ChecksumError";
        break;
      case bcmRxTrapIpv4HeaderLengthError:
        str = "bcmRxTrapIpv4HeaderLengthError";
        break;
      case bcmRxTrapIpv4TotalLengthError:
        str = "bcmRxTrapIpv4TotalLengthError";
        break;
      case bcmRxTrapIpv4Ttl0:
        str = "bcmRxTrapIpv4Ttl0";
        break;
      case bcmRxTrapIpv4HasOptions:
        str = "bcmRxTrapIpv4HasOptions";
        break;
      case bcmRxTrapIpv4Ttl1:
        str = "bcmRxTrapIpv4Ttl1";
        break;
      case bcmRxTrapIpv4SipEqualDip:
        str = "bcmRxTrapIpv4SipEqualDip";
        break;
      case bcmRxTrapIpv4DipZero:
        str = "bcmRxTrapIpv4DipZero";
        break;
      case bcmRxTrapIpv4SipIsMc:
        str = "bcmRxTrapIpv4SipIsMc";
        break;
      case bcmRxTrapIpv4TunnelTerminationAndFragmented:
        str = "bcmRxTrapIpv4TunnelTerminationAndFragmented";
        break;
      case bcmRxTrapIpv6VersionError:
        str = "bcmRxTrapIpv6VersionError";
        break;
      case bcmRxTrapIpv6HopCount0:
        str = "bcmRxTrapIpv6HopCount0";
        break;
      case bcmRxTrapIpv6HopCount1:
        str = "bcmRxTrapIpv6HopCount1";
        break;
      case bcmRxTrapIpv6UnspecifiedDestination:
        str = "bcmRxTrapIpv6UnspecifiedDestination";
        break;
      case bcmRxTrapIpv6LoopbackAddress:
        str = "bcmRxTrapIpv6LoopbackAddress";
        break;
      case bcmRxTrapIpv6MulticastSource:
        str = "bcmRxTrapIpv6MulticastSource";
        break;
      case bcmRxTrapIpv6NextHeaderNull:
        str = "bcmRxTrapIpv6NextHeaderNull";
        break;
      case bcmRxTrapIpv6UnspecifiedSource:
        str = "bcmRxTrapIpv6UnspecifiedSource";
        break;
      case bcmRxTrapIpv6LocalLinkDestination:
        str = "bcmRxTrapIpv6LocalLinkDestination";
        break;
      case bcmRxTrapIpv6LocalSiteDestination:
        str = "bcmRxTrapIpv6LocalSiteDestination";
        break;
      case bcmRxTrapIpv6LocalLinkSource:
        str = "bcmRxTrapIpv6LocalLinkSource";
        break;
      case bcmRxTrapIpv6LocalSiteSource:
        str = "bcmRxTrapIpv6LocalSiteSource";
        break;
      case bcmRxTrapIpv6Ipv4CompatibleDestination:
        str = "bcmRxTrapIpv6Ipv4CompatibleDestination";
        break;
      case bcmRxTrapIpv6Ipv4MappedDestination:
        str = "bcmRxTrapIpv6Ipv4MappedDestination";
        break;
      case bcmRxTrapIpv6MulticastDestination:
        str = "bcmRxTrapIpv6MulticastDestination";
        break;
      case bcmRxTrapMplsTtl0:
        str = "bcmRxTrapMplsTtl0";
        break;
      case bcmRxTrapMplsTtl1:
        str = "bcmRxTrapMplsTtl1";
        break;
      case bcmRxTrapTcpSnFlagsZero:
        str = "bcmRxTrapTcpSnFlagsZero";
        break;
      case bcmRxTrapTcpSnZeroFlagsSet:
        str = "bcmRxTrapTcpSnZeroFlagsSet";
        break;
      case bcmRxTrapTcpSynFin:
        str = "bcmRxTrapTcpSynFin";
        break;
      case bcmRxTrapTcpEqualPorts:
        str = "bcmRxTrapTcpEqualPorts";
        break;
      case bcmRxTrapTcpFragmentIncompleteHeader:
        str = "bcmRxTrapTcpFragmentIncompleteHeader";
        break;
      case bcmRxTrapTcpFragmentOffsetLt8:
        str = "bcmRxTrapTcpFragmentOffsetLt8";
        break;
      case bcmRxTrapUdpEqualPorts:
        str = "bcmRxTrapUdpEqualPorts";
        break;
      case bcmRxTrapIcmpDataGt576:
        str = "bcmRxTrapIcmpDataGt576";
        break;
      case bcmRxTrapIcmpFragmented:
        str = "bcmRxTrapIcmpFragmented";
        break;
      case bcmRxTrapFailoverFacilityInvalid:
        str = "bcmRxTrapFailoverFacilityInvalid";
        break;
      case bcmRxTrapUcStrictRpfFail:
        str = "bcmRxTrapUcStrictRpfFail";
        break;
      case bcmRxTrapMcExplicitRpfFail:
        str = "bcmRxTrapMcExplicitRpfFail";
        break;
      case bcmRxTrapMcUseSipRpfFail:
        str = "bcmRxTrapMcUseSipRpfFail";
        break;
      case bcmRxTrapMcUseSipMultipath:
        str = "bcmRxTrapMcUseSipMultipath";
        break;
      case bcmRxTrapIcmpRedirect:
        str = "bcmRxTrapIcmpRedirect";
        break;
      case bcmRxTrapOamp:
        str = "bcmRxTrapOamp";
        break;
      case bcmRxTrapOampTrapErr:
        str = "bcmRxTrapOampTrapErr";
        break;
      case bcmRxTrapOampTypeErr:
        str = "bcmRxTrapOampTypeErr";
        break;
      case bcmRxTrapOampRmepErr:
        str = "bcmRxTrapOampRmepErr";
        break;
      case bcmRxTrapOampMaidErr:
        str = "bcmRxTrapOampMaidErr";
        break;
      case bcmRxTrapOampMdlErr:
        str = "bcmRxTrapOampMdlErr";
        break;
      case bcmRxTrapOampCcmIntrvErr:
        str = "bcmRxTrapOampCcmIntrvErr";
        break;
      case bcmRxTrapOampMyDiscErr:
        str = "bcmRxTrapOampMyDiscErr";
        break;
      case bcmRxTrapOampSrcIpErr:
        str = "bcmRxTrapOampSrcIpErr";
        break;
      case bcmRxTrapOampYourDiscErr:
        str = "bcmRxTrapOampYourDiscErr";
        break;
      case bcmRxTrapOampSrcPortErr:
        str = "bcmRxTrapOampSrcPortErr";
        break;
      case bcmRxTrapOampRmepStateChange:
        str = "bcmRxTrapOampRmepStateChange";
        break;
      case bcmRxTrapOampParityErr:
        str = "bcmRxTrapOampParityErr";
        break;
      case bcmRxTrapOampTimestampErr:
        str = "bcmRxTrapOampTimestampErr";
        break;
      case bcmRxTrapOampProtection:
        str = "bcmRxTrapOampProtection";
        break;
      case bcmRxTrapOamEthAccelerated:
        str = "bcmRxTrapOamEthAccelerated";
        break;
      case bcmRxTrapOamMplsAccelerated:
        str = "bcmRxTrapOamMplsAccelerated";
        break;
      case bcmRxTrapOamBfdIpTunnelAccelerated:
        str = "bcmRxTrapOamBfdIpTunnelAccelerated";
        break;
      case bcmRxTrapOamBfdPweAccelerated:
        str = "bcmRxTrapOamBfdPweAccelerated";
        break;
      case bcmRxTrapOamEthUpAccelerated:
        str = "bcmRxTrapOamEthUpAccelerated";
        break;
      case bcmRxTrapEgPortNotVlanMember:
        str = "bcmRxTrapEgPortNotVlanMember";
        break;
      case bcmRxTrapEgHairPinFilter:
        str = "bcmRxTrapEgHairPinFilter";
        break;
      case bcmRxTrapEgSplitHorizonFilter:
        str = "bcmRxTrapEgSplitHorizonFilter";
        break;
      case bcmRxTrapEgUnknownDa:
        str = "bcmRxTrapEgUnknownDa";
        break;
      case bcmRxTrapEgMtuFilter:
        str = "bcmRxTrapEgMtuFilter";
        break;
      case bcmRxTrapEgPvlanFilter:
        str = "bcmRxTrapEgPvlanFilter";
        break;
      case bcmRxTrapEgDiscardFrameTypeFilter:
        str = "bcmRxTrapEgDiscardFrameTypeFilter";
        break;
      case bcmRxTrapEgTrillHairPinFilter:
        str = "bcmRxTrapEgTrillHairPinFilter";
        break;
      case bcmRxTrapEgTrillBounceBack:
        str = "bcmRxTrapEgTrillBounceBack";
        break;
      case bcmRxTrapEgTrillTtl0:
        str = "bcmRxTrapEgTrillTtl0";
        break;
      case bcmRxTrapEgIpmcTtlErr:
        str = "bcmRxTrapEgIpmcTtlErr";
        break;
      case bcmRxTrapEgCfmAccelerated:
        str = "bcmRxTrapEgCfmAccelerated";
        break;
      case bcmRxTrapDssStacking:
        str = "bcmRxTrapDssStacking";
        break;
      case bcmRxTrapLagMulticast:
        str = "bcmRxTrapLagMulticast";
        break;
      case bcmRxTrapExEgCnm:
        str = "bcmRxTrapExEgCnm";
        break;
      case bcmRxTrapTrillDesignatedVlanNoMymac:
        str = "bcmRxTrapTrillDesignatedVlanNoMymac";
        break;
      case bcmRxTrapAdjacentCheckFail:
        str = "bcmRxTrapAdjacentCheckFail";
        break;
      case bcmRxTrapFcoeSrcIdMismatchSa:
        str = "bcmRxTrapFcoeSrcIdMismatchSa";
        break;
      case bcmRxTrapSipMove:
        str = "bcmRxTrapSipMove";
        break;
      case bcmRxTrapOamY1731MplsTp:
        str = "bcmRxTrapOamY1731MplsTp";
        break;
      case bcmRxTrapOamY1731Pwe:
        str = "bcmRxTrapOamY1731Pwe";
        break;
      case bcmRxTrapOamBfdIpv4:
        str = "bcmRxTrapOamBfdIpv4";
        break;
      case bcmRxTrapOamBfdMpls:
        str = "bcmRxTrapOamBfdMpls";
        break;
      case bcmRxTrapOamBfdPwe:
        str = "bcmRxTrapOamBfdPwe";
        break;
      case bcmRxTrapOamBfdCcMplsTp:
        str = "bcmRxTrapOamBfdCcMplsTp";
        break;
      case bcmRxTrapOamBfdCvMplsTp:
        str = "bcmRxTrapOamBfdCvMplsTp";
        break;
      case bcmRxTrapOamLevel:
        str = "bcmRxTrapOamLevel";
        break;
      case bcmRxTrapOamPassive:
        str = "bcmRxTrapOamPassive";
        break;
      case bcmRxTrap1588:
        str = "bcmRxTrap1588";
        break;
      case bcmRxTrapMplsLabelIndexNoSupport:
        str = "bcmRxTrapMplsLabelIndexNoSupport";
        break;
      case bcmRxTrapDhcpv4Client:
        str = "bcmRxTrapDhcpv4Client";
        break;
      case bcmRxTrapDhcpv4Server:
        str = "bcmRxTrapDhcpv4Server";
        break;
      case bcmRxTrapEtherIpVersion:
        str = "bcmRxTrapEtherIpVersion";
        break;
      case bcmRxTrapEgressObjectAccessed:
        str = "bcmRxTrapEgressObjectAccessed";
        break;
      case bcmRxTrapIcmpv6MldReportDone:
        str = "bcmRxTrapIcmpv6MldReportDone";
        break;
      case bcmRxTrapIcmpv6MldUndefined:
        str = "bcmRxTrapIcmpv6MldUndefined";
        break;
      case bcmRxTrapFailover1Plus1Fail:
        str = "bcmRxTrapFailover1Plus1Fail";
        break;
      case bcmRxTrapEgInvalidDestPort:
        str = "bcmRxTrapEgInvalidDestPort";
        break;
      case bcmRxTrapEgIpv4VersionError:
        str = "bcmRxTrapEgIpv4VersionError";
        break;
      case bcmRxTrapEgIpv4ChecksumError:
        str = "bcmRxTrapEgIpv4ChecksumError";
        break;
      case bcmRxTrapEgIpv4HeaderLengthError:
        str = "bcmRxTrapEgIpv4HeaderLengthError";
        break;
      case bcmRxTrapEgIpv4TotalLengthError:
        str = "bcmRxTrapEgIpv4TotalLengthError";
        break;
      case bcmRxTrapEgIpv4Ttl0:
        str = "bcmRxTrapEgIpv4Ttl0";
        break;
      case bcmRxTrapEgIpv4HasOptions:
        str = "bcmRxTrapEgIpv4HasOptions";
        break;
      case bcmRxTrapEgIpv4Ttl1:
        str = "bcmRxTrapEgIpv4Ttl1";
        break;
      case bcmRxTrapEgIpv4SipEqualDip:
        str = "bcmRxTrapEgIpv4SipEqualDip";
        break;
      case bcmRxTrapEgIpv4DipZero:
        str = "bcmRxTrapEgIpv4DipZero";
        break;
      case bcmRxTrapEgIpv4SipIsMc:
        str = "bcmRxTrapEgIpv4SipIsMc";
        break;
      case bcmRxTrapEgIpv6UnspecifiedDestination:
        str = "bcmRxTrapEgIpv6UnspecifiedDestination";
        break;
      case bcmRxTrapEgIpv6LoopbackAddress:
        str = "bcmRxTrapEgIpv6LoopbackAddress";
        break;
      case bcmRxTrapEgIpv6MulticastSource:
        str = "bcmRxTrapEgIpv6MulticastSource";
        break;
      case bcmRxTrapEgIpv6UnspecifiedSource:
        str = "bcmRxTrapEgIpv6UnspecifiedSource";
        break;
      case bcmRxTrapEgIpv6LocalLinkDestination:
        str = "bcmRxTrapEgIpv6LocalLinkDestination";
        break;
      case bcmRxTrapEgIpv6LocalSiteDestination:
        str = "bcmRxTrapEgIpv6LocalSiteDestination";
        break;
      case bcmRxTrapEgIpv6LocalLinkSource:
        str = "bcmRxTrapEgIpv6LocalLinkSource";
        break;
      case bcmRxTrapEgIpv6LocalSiteSource:
        str = "bcmRxTrapEgIpv6LocalSiteSource";
        break;
      case bcmRxTrapEgIpv6Ipv4CompatibleDestination:
        str = "bcmRxTrapEgIpv6Ipv4CompatibleDestination";
        break;
      case bcmRxTrapEgIpv6Ipv4MappedDestination:
        str = "bcmRxTrapEgIpv6Ipv4MappedDestination";
        break;
      case bcmRxTrapEgIpv6MulticastDestination:
        str = "bcmRxTrapEgIpv6MulticastDestination";
        break;
      case bcmRxTrapEgIpv6NextHeaderNull:
        str = "bcmRxTrapEgIpv6NextHeaderNull";
        break;
      case bcmRxTrapUserDefine:
        str = "bcmRxTrapUserDefine";
        break;
      case bcmRxTrapReserved:
        str = "bcmRxTrapReserved";
        break;
      case bcmRxTrapFcoeZoneCheckFail:
        str = "bcmRxTrapFcoeZoneCheckFail";
        break;
      case bcmRxTrapArplookupFail:
        str = "bcmRxTrapArplookupFail";
        break;
      case bcmRxTrapL2cpPeer:
        str = "bcmRxTrapL2cpPeer";
        break;
      case bcmRxTrapL2cpDrop:
        str = "bcmRxTrapL2cpDrop";
        break;
      case bcmRxTrapMimMyBmacMulticastContinue:
        str = "bcmRxTrapMimMyBmacMulticastContinue";
        break;
      case bcmRxTrapBfdEchoOverIpv4:
        str = "bcmRxTrapBfdEchoOverIpv4";
        break;
      case bcmRxTrapDfltDroppedPacket:
        str = "bcmRxTrapDfltDroppedPacket";
        break;
      case bcmRxTrapDfltRedirectToCpuPacket:
        str = "bcmRxTrapDfltRedirectToCpuPacket";
        break;
      case bcmRxTrapRedirectToCpuOamPacket:
        str = "bcmRxTrapRedirectToCpuOamPacket";
        break;
      case bcmRxTrapSnoopOamPacket:
        str = "bcmRxTrapSnoopOamPacket";
        break;
      case bcmRxTrapRecycleOamPacket:
        str = "bcmRxTrapRecycleOamPacket";
        break;
      case bcmRxTrapItmhSnoop0:
        str = "bcmRxTrapItmhSnoop0";
        break;
      case bcmRxTrapItmhSnoop1:
        str = "bcmRxTrapItmhSnoop1";
        break;
      case bcmRxTrapItmhSnoop2:
        str = "bcmRxTrapItmhSnoop2";
        break;
      case bcmRxTrapItmhSnoop3:
        str = "bcmRxTrapItmhSnoop3";
        break;
      case bcmRxTrapItmhSnoop4:
        str = "bcmRxTrapItmhSnoop4";
        break;
      case bcmRxTrapItmhSnoop5:
        str = "bcmRxTrapItmhSnoop5";
        break;
      case bcmRxTrapItmhSnoop6:
        str = "bcmRxTrapItmhSnoop6";
        break;
      case bcmRxTrapItmhSnoop7:
        str = "bcmRxTrapItmhSnoop7";
        break;
      case bcmRxTrapItmhSnoop8:
        str = "bcmRxTrapItmhSnoop8";
        break;
      case bcmRxTrapItmhSnoop9:
        str = "bcmRxTrapItmhSnoop9";
        break;
      case bcmRxTrapItmhSnoop10:
        str = "bcmRxTrapItmhSnoop10";
        break;
      case bcmRxTrapItmhSnoop11:
        str = "bcmRxTrapItmhSnoop11";
        break;
      case bcmRxTrapItmhSnoop12:
        str = "bcmRxTrapItmhSnoop12";
        break;
      case bcmRxTrapItmhSnoop13:
        str = "bcmRxTrapItmhSnoop13";
        break;
      case bcmRxTrapItmhSnoop14:
        str = "bcmRxTrapItmhSnoop14";
        break;
      case bcmRxTrapItmhSnoop15:
        str = "bcmRxTrapItmhSnoop15";
        break;
      case bcmRxTrapFcoeFcfPacket:
        str = "bcmRxTrapFcoeFcfPacket";
        break;
      case bcmRxTrapVlanTagDiscard:
        str = "bcmRxTrapVlanTagDiscard";
        break;
      case bcmRxTrapVlanTagAccept:
        str = "bcmRxTrapVlanTagAccept";
        break;
      case bcmRxTrap1588Discard:
        str = "bcmRxTrap1588Discard";
        break;
      case bcmRxTrap1588Accepted:
        str = "bcmRxTrap1588Accepted";
        break;
      case bcmRxTrapInnerIpCompMcInvalidIp:
        str = "bcmRxTrapInnerIpCompMcInvalidIp";
        break;
      case bcmRxTrapIllegalReservedLabel:
        str = "bcmRxTrapIllegalReservedLabel";
        break;
      case bcmRxTrapInnerMyMacAndIpDisabled:
        str = "bcmRxTrapInnerMyMacAndIpDisabled";
        break;
      case bcmRxTrapEgTxFieldSnoop0:
        str = "bcmRxTrapEgTxFieldSnoop0";
        break;
      case bcmRxTrapEgTxFieldSnoop1:
        str = "bcmRxTrapEgTxFieldSnoop1";
        break;
      case bcmRxTrapEgTxFieldSnoop2:
        str = "bcmRxTrapEgTxFieldSnoop2";
        break;
      case bcmRxTrapEgTxFieldSnoop3:
        str = "bcmRxTrapEgTxFieldSnoop3";
        break;
      case bcmRxTrapEgTxFieldSnoop4:
        str = "bcmRxTrapEgTxFieldSnoop4";
        break;
      case bcmRxTrapEgTxFieldSnoop5:
        str = "bcmRxTrapEgTxFieldSnoop5";
        break;
      case bcmRxTrapEgTxFieldSnoop6:
        str = "bcmRxTrapEgTxFieldSnoop6";
        break;
      case bcmRxTrapEgTxFieldSnoop7:
        str = "bcmRxTrapEgTxFieldSnoop7";
        break;
      case bcmRxTrapEgTxFieldSnoop8:
        str = "bcmRxTrapEgTxFieldSnoop8";
        break;
      case bcmRxTrapEgTxFieldSnoop9:
        str = "bcmRxTrapEgTxFieldSnoop9";
        break;
      case bcmRxTrapEgTxFieldSnoop10:
        str = "bcmRxTrapEgTxFieldSnoop10";
        break;
      case bcmRxTrapEgTxFieldSnoop11:
        str = "bcmRxTrapEgTxFieldSnoop11";
        break;
      case bcmRxTrapEgTxFieldSnoop12:
        str = "bcmRxTrapEgTxFieldSnoop12";
        break;
      case bcmRxTrapEgTxFieldSnoop13:
        str = "bcmRxTrapEgTxFieldSnoop13";
        break;
      case bcmRxTrapEgTxFieldSnoop14:
        str = "bcmRxTrapEgTxFieldSnoop14";
        break;
      case bcmRxTrapEgTxFieldSnoop15:
        str = "bcmRxTrapEgTxFieldSnoop15";
        break;
      case bcmRxTrapOutVPortDiscard:
        str = "bcmRxTrapOutVPortDiscard";
        break;
      case bcmRxTrapEgTxStpStateFail:
        str = "bcmRxTrapEgTxStpStateFail";
        break;
      case bcmRxTrapEgTxProtectionPathUnexpected:
        str = "bcmRxTrapEgTxProtectionPathUnexpected";
        break;
      case bcmRxTrapOutVPortLookupFail:
        str = "bcmRxTrapOutVPortLookupFail";
        break;
      case bcmRxTrapOamBfdIpv6:
        str = "bcmRxTrapOamBfdIpv6";
        break;
      case bcmRxTrapOamMipSnoop2ndPass:
        str = "bcmRxTrapOamMipSnoop2ndPass";
        break;
      case bcmRxTrapSat0:
        str = "bcmRxTrapSat0";
        break;
      case bcmRxTrapSat1:
        str = "bcmRxTrapSat1";
        break;
      case bcmRxTrapSat2:
        str = "bcmRxTrapSat2";
        break;
      case bcmRxTrapEgTxPortNotVlanMember:
        str = "bcmRxTrapEgTxPortNotVlanMember";
        break;
      case bcmRxTrapEgTxDiscardFrameTypeFilter:
        str = "bcmRxTrapEgTxDiscardFrameTypeFilter";
        break;
      case bcmRxTrapEgTxMtuFilter:
        str = "bcmRxTrapEgTxMtuFilter";
        break;
      case bcmRxTrapEgTxSplitHorizonFilter:
        str = "bcmRxTrapEgTxSplitHorizonFilter";
        break;
      case bcmRxTrapOamUpMEP:
        str = "bcmRxTrapOamUpMEP";
        break;
      case bcmRxTrapBfdOamDownMEP:
        str = "bcmRxTrapBfdOamDownMEP";
        break;
      case bcmRxTrapCount:
        str = "bcmRxTrapCount";
        break;
      case bcmRxTrapSer:
        str = "bcmRxTrapSer";
        break;
      default:
         str = "Unknown";
         break;
    }
    return str;
}

const char*
  SOC_PPC_TRAP_CODE_to_string(
    SOC_SAND_IN  SOC_PPC_TRAP_CODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_TRAP_CODE_PBP_SA_DROP_0:
    str = "pbp_sa_drop_0";
  break;
  case SOC_PPC_TRAP_CODE_PBP_SA_DROP_1:
    str = "pbp_sa_drop_1";
  break;
  case SOC_PPC_TRAP_CODE_PBP_SA_DROP_2:
    str = "pbp_sa_drop_2";
  break;
  case SOC_PPC_TRAP_CODE_PBP_SA_DROP_3:
    str = "pbp_sa_drop_3";
  break;
  case SOC_PPC_TRAP_CODE_PBP_TE_TRANSPLANT:
    str = "pbp_te_transplant";
  break;
  case SOC_PPC_TRAP_CODE_PBP_TE_UNKNOWN_TUNNEL:
    str = "pbp_te_unknown_tunnel";
  break;
  case SOC_PPC_TRAP_CODE_PBP_TRANSPLANT:
    str = "pbp_transplant";
  break;
  case SOC_PPC_TRAP_CODE_PBP_LEARN_SNOOP:
    str = "pbp_learn_snoop";
  break;
  case SOC_PPC_TRAP_CODE_SA_AUTHENTICATION_FAILED:
    str = "sa_authentication_failed";
  break;
  case SOC_PPC_TRAP_CODE_PORT_NOT_PERMITTED:
    str = "port_not_permitted";
  break;
  case SOC_PPC_TRAP_CODE_UNEXPECTED_VID:
    str = "unexpected_vid";
  break;
  case SOC_PPC_TRAP_CODE_SA_MULTICAST:
    str = "sa_multicast";
  break;
  case SOC_PPC_TRAP_CODE_SA_EQUALS_DA:
    str = "sa_equals_da";
  break;
  case SOC_PPC_TRAP_CODE_8021X:
    str = "code_8021x";
  break;
  case SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_DROP:
    str = "frame_drop";
  break;
  case SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_ACCEPT:
    str = "frame_accept";
  break;
  case SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_1:
    str = "frame_custom_1";
  break;
  case SOC_PPC_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_CUSTOM_2:
    str = "frame_custom_2";
  break;
  case SOC_PPC_TRAP_CODE_MY_ARP:
    str = "my_arp";
  break;
  case SOC_PPC_TRAP_CODE_ARP:
    str = "arp";
  break;
  case SOC_PPC_TRAP_CODE_IGMP_MEMBERSHIP_QUERY:
    str = "igmp_membership_query";
  break;
  case SOC_PPC_TRAP_CODE_IGMP_REPORT_LEAVE_MSG:
    str = "igmp_report_leave_msg";
  break;
  case SOC_PPC_TRAP_CODE_IGMP_UNDEFINED:
    str = "igmp_undefined";
  break;
  case SOC_PPC_TRAP_CODE_ICMPV6_MLD_MC_LISTENER_QUERY:
    str = "icmpv6_mld_mc_listener_query";
  break;
  case SOC_PPC_TRAP_CODE_RESERVED_MC_0:
    str = "reserved_mc_0";
  break;
  case SOC_PPC_TRAP_CODE_RESERVED_MC_1:
    str = "reserved_mc_1";
  break;
  case SOC_PPC_TRAP_CODE_RESERVED_MC_2:
    str = "reserved_mc_2";
  break;
  case SOC_PPC_TRAP_CODE_RESERVED_MC_3:
    str = "reserved_mc_3";
  break;
  case SOC_PPC_TRAP_CODE_RESERVED_MC_4:
    str = "reserved_mc_4";
  break;
  case SOC_PPC_TRAP_CODE_RESERVED_MC_5:
    str = "reserved_mc_5";
  break;
  case SOC_PPC_TRAP_CODE_RESERVED_MC_6:
    str = "reserved_mc_6";
  break;
  case SOC_PPC_TRAP_CODE_RESERVED_MC_7:
    str = "reserved_mc_7";
  break;
  case SOC_PPC_TRAP_CODE_ICMPV6_MLD_REPORT_DONE_MSG:
    str = "icmpv6_mld_report_done_msg";
  break;
  case SOC_PPC_TRAP_CODE_ICMPV6_MLD_UNDEFINED:
    str = "icmpv6_mld_undefined";
  break;
  case SOC_PPC_TRAP_CODE_DHCP_SERVER:
    str = "dhcp_server";
  break;
  case SOC_PPC_TRAP_CODE_DHCP_CLIENT:
    str = "dhcp_client";
  break;
  case SOC_PPC_TRAP_CODE_DHCPV6_SERVER:
    str = "dhcpv6_server";
  break;
  case SOC_PPC_TRAP_CODE_DHCPV6_CLIENT:
    str = "dhcpv6_client";
  break;
  case SOC_PPC_TRAP_CODE_PROG_TRAP_0:
    str = "prog_trap_0";
  break;
  case SOC_PPC_TRAP_CODE_PROG_TRAP_1:
    str = "prog_trap_1";
  break;
  case SOC_PPC_TRAP_CODE_PROG_TRAP_2:
    str = "prog_trap_2";
  break;
  case SOC_PPC_TRAP_CODE_PROG_TRAP_3:
    str = "prog_trap_3";
  break;
  case SOC_PPC_TRAP_CODE_PORT_NOT_VLAN_MEMBER:
    str = "port_not_vlan_member";
  break;
  case SOC_PPC_TRAP_CODE_HEADER_SIZE_ERR:
    str = "header_size_err";
    break;
  case SOC_PPC_TRAP_CODE_HEADER_SIZE_ERR_O_MPLS:
    str = "header_size_err_o_mpls";
  break;
  case SOC_PPC_TRAP_CODE_MY_B_MAC_AND_LEARN_NULL:
    str = "my_b_mac_and_learn_null";
  break;
  case SOC_PPC_TRAP_CODE_MY_B_DA_UNKNOWN_I_SID:
    str = "my_b_da_unknown_i_sid";
  break;
  case SOC_PPC_TRAP_CODE_MY_B_MAC_MC_CONTINUE:
    str = "my_b_mac_mc_continue";
  break;
  case SOC_PPC_TRAP_CODE_STP_STATE_BLOCK:
    str = "stp_state_block";
  break;
  case SOC_PPC_TRAP_CODE_STP_STATE_LEARN:
    str = "stp_state_learn";
  break;
  case SOC_PPC_TRAP_CODE_IP_COMP_MC_INVALID_IP:
    str = "ip_comp_mc_invalid_ip";
  break;
  case SOC_PPC_TRAP_CODE_MY_MAC_AND_IP_DISABLE:
    str = "my_mac_and_ip_disable";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_VERSION:
    str = "trill_version";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_INVALID_TTL:
    str = "trill_invalid_ttl";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_CHBH:
    str = "trill_chbh";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_NO_REVERSE_FEC:
    str = "trill_no_reverse_fec";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_CITE:
    str = "trill_cite";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_ILLEGAL_INNER_MC:
    str = "trill_illegal_inner_mc";
  break;
  case SOC_PPC_TRAP_CODE_MY_MAC_AND_MPLS_DISABLE:
    str = "my_mac_and_mpls_disable";
  break;
  case SOC_PPC_TRAP_CODE_MY_MAC_AND_ARP:
    str = "my_mac_and_arp";
  break;
  case SOC_PPC_TRAP_CODE_MY_MAC_AND_UNKNOWN_L3:
    str = "my_mac_and_unknown_l3";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_0:
    str = "mpls_label_value_0";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_1:
    str = "mpls_label_value_1";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_2:
    str = "mpls_label_value_2";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_LABEL_VALUE_3:
    str = "mpls_label_value_3";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_NO_RESOURCES:
    str = "mpls_no_resources";
  break;
  case SOC_PPC_TRAP_CODE_INVALID_LABEL_IN_RANGE:
    str = "invalid_label_in_range";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_INVALID_LABEL_IN_SEM:
    str = "mpls_invalid_label_in_sem";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_LSP_BOS:
    str = "mpls_lsp_bos";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS_LABEL_14:
    str = "mpls_pwe_no_bos_label_14";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_PWE_NO_BOS:
    str = "mpls_pwe_no_bos";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_VRF_NO_BOS:
    str = "mpls_vrf_no_bos";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_0:
    str = "mpls_term_ttl_0";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_TRAP:
    str = "mpls_term_control_word_trap";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_TERM_CONTROL_WORD_DROP:
    str = "mpls_term_control_word_drop";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_VERSION_ERROR:
    str = "ipv4_term_version_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_CHECKSUM_ERROR:
    str = "ipv4_term_checksum_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_HEADER_LENGTH_ERROR:
    str = "ipv4_term_header_length_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_TOTAL_LENGTH_ERROR:
    str = "ipv4_term_total_length_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_TTL0:
    str = "ipv4_term_ttl0";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_HAS_OPTIONS:
    str = "ipv4_term_has_options";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_TTL1:
    str = "ipv4_term_ttl1";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_EQUAL_DIP:
    str = "ipv4_term_sip_equal_dip";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_DIP_ZERO:
    str = "ipv4_term_dip_zero";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TERM_SIP_IS_MC:
    str = "ipv4_term_sip_is_mc";
  break;
  case SOC_PPC_TRAP_CODE_CFM_ACCELERATED_INGRESS:
    str = "cfm_accelerated_ingress";
  break;
  case SOC_PPC_TRAP_CODE_ILLEGEL_PFC:
    str = "illegel_pfc";
  break;
  case SOC_PPC_TRAP_CODE_SA_DROP_0:
    str = "sa_drop_0";
  break;
  case SOC_PPC_TRAP_CODE_SA_DROP_1:
    str = "sa_drop_1";
  break;
  case SOC_PPC_TRAP_CODE_SA_DROP_2:
    str = "sa_drop_2";
  break;
  case SOC_PPC_TRAP_CODE_SA_DROP_3:
    str = "sa_drop_3";
  break;
  case SOC_PPC_TRAP_CODE_SA_NOT_FOUND_0:
    str = "sa_not_found_0";
  break;
  case SOC_PPC_TRAP_CODE_SA_NOT_FOUND_1:
    str = "sa_not_found_1";
  break;
  case SOC_PPC_TRAP_CODE_SA_NOT_FOUND_2:
    str = "sa_not_found_2";
  break;
  case SOC_PPC_TRAP_CODE_SA_NOT_FOUND_3:
    str = "sa_not_found_3";
  break;
  case SOC_PPC_TRAP_CODE_UNKNOWN_DA_0:
    str = "unknown_da_0";
  break;
  case SOC_PPC_TRAP_CODE_UNKNOWN_DA_1:
    str = "unknown_da_1";
  break;
  case SOC_PPC_TRAP_CODE_UNKNOWN_DA_2:
    str = "unknown_da_2";
  break;
  case SOC_PPC_TRAP_CODE_UNKNOWN_DA_3:
    str = "unknown_da_3";
  break;
  case SOC_PPC_TRAP_CODE_UNKNOWN_DA_4:
    str = "unknown_da_4";
    break;
  case SOC_PPC_TRAP_CODE_UNKNOWN_DA_5:
    str = "unknown_da_5";
    break;
  case SOC_PPC_TRAP_CODE_UNKNOWN_DA_6:
    str = "unknown_da_6";
    break;
  case SOC_PPC_TRAP_CODE_UNKNOWN_DA_7:
    str = "unknown_da_7";
    break;
  case SOC_PPC_TRAP_CODE_ELK_ERROR:
    str = "elk_error";
  break;
  case SOC_PPC_TRAP_CODE_DA_NOT_FOUND_0:
    str = "da_not_found_0";
  break;
  case SOC_PPC_TRAP_CODE_DA_NOT_FOUND_1:
    str = "da_not_found_1";
  break;
  case SOC_PPC_TRAP_CODE_DA_NOT_FOUND_2:
    str = "da_not_found_2";
  break;
  case SOC_PPC_TRAP_CODE_DA_NOT_FOUND_3:
    str = "da_not_found_3";
  break;
  case SOC_PPC_TRAP_CODE_P2P_MISCONFIGURATION:
    str = "p2p_misconfiguration";
  break;
  case SOC_PPC_TRAP_CODE_SAME_INTERFACE:
    str = "same_interface";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_UNKNOWN_UC:
    str = "trill_unknown_uc";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_UNKNOWN_MC:
    str = "trill_unknown_mc";
  break;
  case SOC_PPC_TRAP_CODE_UC_LOOSE_RPF_FAIL:
    str = "uc_loose_rpf_fail";
  break;
  case SOC_PPC_TRAP_CODE_DEFAULT_UCV6:
    str = "default_ucv6";
  break;
  case SOC_PPC_TRAP_CODE_DEFAULT_MCV6:
    str = "default_mcv6";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_P2P_NO_BOS:
    str = "mpls_p2p_no_bos";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_TRAP:
    str = "mpls_control_word_trap";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_CONTROL_WORD_DROP:
    str = "mpls_control_word_drop";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_UNKNOWN_LABEL:
    str = "mpls_unknown_label";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_P2P_MPLSX4:
    str = "mpls_p2p_mplsx4";
    break;
  case SOC_PPC_TRAP_CODE_ETH_L2CP_PEER:
    str = "eth_l2cp_peer";
  break;
  case SOC_PPC_TRAP_CODE_ETH_L2CP_DROP:
    str = "eth_l2cp_drop";
  break;
  case SOC_PPC_TRAP_CODE_ETH_FL_IGMP_MEMBERSHIP_QUERY:
    str = "eth_fl_igmp_membership_query";
  break;
  case SOC_PPC_TRAP_CODE_ETH_FL_IGMP_REPORT_LEAVE_MSG:
    str = "eth_fl_igmp_report_leave_msg";
  break;
  case SOC_PPC_TRAP_CODE_ETH_FL_IGMP_UNDEFINED:
    str = "eth_fl_igmp_undefined";
  break;
  case SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_MC_LISTENER_QUERY:
    str = "eth_fl_icmpv6_mld_mc_listener_query";
  break;
  case SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_REPORT_DONE:
    str = "eth_fl_icmpv6_mld_report_done";
  break;
  case SOC_PPC_TRAP_CODE_ETH_FL_ICMPV6_MLD_UNDEFINED:
    str = "eth_fl_icmpv6_mld_undefined";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_VERSION_ERROR:
    str = "ipv4_version_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_CHECKSUM_ERROR:
    str = "ipv4_checksum_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_HEADER_LENGTH_ERROR:
    str = "ipv4_header_length_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TOTAL_LENGTH_ERROR:
    str = "ipv4_total_length_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TTL0:
    str = "ipv4_ttl0";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_HAS_OPTIONS:
    str = "ipv4_has_options";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TTL1:
    str = "ipv4_ttl1";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_SIP_EQUAL_DIP:
    str = "ipv4_sip_equal_dip";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_DIP_ZERO:
    str = "ipv4_dip_zero";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_SIP_IS_MC:
    str = "ipv4_sip_is_mc";
  break;
  case SOC_PPC_TRAP_CODE_IPV4_TUNNEL_TERMINATION_AND_FRAGMENTED:
    str = "ipv4_tunnel_and_frag";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_VERSION_ERROR:
    str = "ipv6_version_error";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT0:
    str = "ipv6_hop_count0";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_HOP_COUNT1:
    str = "ipv6_hop_count1";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_DESTINATION:
    str = "ipv6_unspecified_destination";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_LOOPBACK_ADDRESS:
    str = "ipv6_loopback_address";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_MULTICAST_SOURCE:
    str = "ipv6_multicast_source";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_NEXT_HEADER_NULL:
    str = "ipv6_next_header_null";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_UNSPECIFIED_SOURCE:
    str = "ipv6_unspecified_source";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_LOCAL_LINK_DESTINATION:
    str = "ipv6_local_link_destination";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_LOCAL_SITE_DESTINATION:
    str = "ipv6_local_site_destination";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_LOCAL_LINK_SOURCE:
    str = "ipv6_local_link_source";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_LOCAL_SITE_SOURCE:
    str = "ipv6_local_site_source";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_IPV4_COMPATIBLE_DESTINATION:
    str = "ipv6_ipv4_compatible_destination";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_IPV4_MAPPED_DESTINATION:
    str = "ipv6_ipv4_mapped_destination";
  break;
  case SOC_PPC_TRAP_CODE_IPV6_MULTICAST_DESTINATION:
    str = "ipv6_multicast_destination";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_TTL0:
    str = "mpls_ttl0";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_TTL1:
    str = "mpls_ttl1";
  break;
  case SOC_PPC_TRAP_CODE_TCP_SN_FLAGS_ZERO:
    str = "tcp_sn_flags_zero";
  break;
  case SOC_PPC_TRAP_CODE_TCP_SN_ZERO_FLAGS_SET:
    str = "tcp_sn_zero_flags_set";
  break;
  case SOC_PPC_TRAP_CODE_TCP_SYN_FIN:
    str = "tcp_syn_fin";
  break;
  case SOC_PPC_TRAP_CODE_TCP_EQUAL_PORTS:
    str = "tcp_equal_ports";
  break;
  case SOC_PPC_TRAP_CODE_TCP_FRAGMENT_INCOMPLETE_HEADER:
    str = "tcp_fragment_incomplete_header";
  break;
  case SOC_PPC_TRAP_CODE_TCP_FRAGMENT_OFFSET_LT8:
    str = "tcp_fragment_offset_lt8";
  break;
  case SOC_PPC_TRAP_CODE_UDP_EQUAL_PORTS:
    str = "udp_equal_ports";
  break;
  case SOC_PPC_TRAP_CODE_ICMP_DATA_GT_576:
    str = "icmp_data_gt_576";
  break;
  case SOC_PPC_TRAP_CODE_ICMP_FRAGMENTED:
    str = "icmp_fragmented";
  break;
  case SOC_PPC_TRAP_CODE_GENERAL:
    str = "general";
  break;
  case SOC_PPC_TRAP_CODE_FACILITY_INVALID:
    str = "facility_invalid";
  break;
  case SOC_PPC_TRAP_CODE_FEC_ENTRY_ACCESSED:
    str = "fec_entry_accessed";
  break;
  case SOC_PPC_TRAP_CODE_UC_STRICT_RPF_FAIL:
    str = "uc_strict_rpf_fail";
  break;
  case SOC_PPC_TRAP_CODE_MC_EXPLICIT_RPF_FAIL:
    str = "mc_explicit_rpf_fail";
  break;
  case SOC_PPC_TRAP_CODE_MC_USE_SIP_AS_IS_RPF_FAIL:
    str = "mc_use_sip_as_is_rpf_fail";
  break;
  case SOC_PPC_TRAP_CODE_MC_USE_SIP_RPF_FAIL:
    str = "mc_use_sip_rpf_fail";
  break;
  case SOC_PPC_TRAP_CODE_MC_USE_SIP_ECMP:
    str = "mc_use_sip_ecmp";
  break;
  case SOC_PPC_TRAP_CODE_ICMP_REDIRECT:
    str = "icmp_redirect";
  break;
  case SOC_PPC_TRAP_CODE_MP_NON_ACCELERATED:
    str = "mp_non_accelerated";
  break;
  case SOC_PPC_TRAP_CODE_MORE_THAN_ONE_ING_COUNTER:
    str = "more_than_one_ing_counter";
  break;
  case SOC_PPC_TRAP_CODE_PWE_INVALID_PROTOCOL:
    str = "pwe_invalid_protocol";
  break;
  case SOC_PPC_TRAP_CODE_IP_OVER_MPLS:
    str = "ip_over_mpls";
  break;
  case SOC_PPC_TRAP_CODE_TIME_SYNC:
    str = "time_sync";
  break;
  case SOC_PPC_TRAP_CODE_AC_KEY_MISS:
    str = "ac_key_miss";
  break;
  case SOC_PPC_TRAP_CODE_EG_MEP_NON_ACCELERATED:
    str = "eg_mep_non_accelerated";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_VSI_MEMBERSHIP:
    str = "eg_filter_vsi_membership";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_HAIR_PIN:
    str = "eg_filter_hair_pin";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_SPLIT_HORIZON:
    str = "eg_filter_split_horizon";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_UNKNOWN_DA_UC:
    str = "eg_filter_unknown_da_uc";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_UNKNOWN_DA_MC:
    str = "eg_filter_unknown_da_mc";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_UNKNOWN_DA_BC:
    str = "eg_filter_unknown_da_bc";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_MTU_VIOLATE:
    str = "eg_filter_mtu_violate";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_STP:
    str = "eg_filter_stp";
  break;
  case SOC_PPC_TRAP_CODE_EG_FILTER_ACCEPT_FRAME:
    str = "eg_filter_accept_frame";
  break;
  case SOC_PPC_TRAP_CODE_USER_OAMP:
    str = "user_oamp";
  break;
  case SOC_PPC_TRAP_CODE_USER_ETH_OAM_ACCELERATED:
    str = "user_eth_oam_accelerated";
  break;
  case SOC_PPC_TRAP_CODE_USER_MPLS_OAM_ACCELERATED:
    str = "user_mpls_oam_accelerated";
  break;
  case SOC_PPC_TRAP_CODE_USER_BFD_IP_OAM_TUNNEL_ACCELERATED:
    str = "user_bfd_ip_oam_tunnel_accelerated";
  break;
  case SOC_PPC_TRAP_CODE_USER_BFD_PWE_OAM_ACCELERATED:
    str = "user_bfd_pwe_oam_accelerated";
  break;
  case SOC_PPC_TRAP_CODE_USER_ETH_OAM_UP_ACCELERATED:
    str = "user_eth_oam_up_accelerated";
  break;
  
  case SOC_PPC_TRAP_CODE_ETHER_IP_VERSION_ERROR:
    str = "ether_ip_version_error";
  break;
  case SOC_PPC_TRAP_CODE_LIF_PROTECT_PATH_INVALID:
    str = "lif_protect_path_invalid";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_NO_ADJACENT:
    str = "trill_no_adjacent";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_INVALID_LIF:
    str = "mpls_invalid_lif";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_TERM_ERROR:
    str = "mpls_term_error";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_NO_BOS:
    str = "mpls_unexpected_no_bos";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_UNEXPECTED_BOS:
    str = "mpls_unexpected_bos";
  break;
  case SOC_PPC_TRAP_CODE_MPLS_TERM_TTL_1:
    str = "mpls_term_ttl_1";
  break;
  case SOC_PPC_TRAP_CODE_1588_PACKET_0:
    str = "1588_packet_0";
  break;
  case SOC_PPC_TRAP_CODE_1588_PACKET_1:
    str = "1588_packet_1";
  break;
  case SOC_PPC_TRAP_CODE_1588_PACKET_2:
    str = "1588_packet_2";
  break;
  case SOC_PPC_TRAP_CODE_1588_PACKET_3:
    str = "1588_packet_3";
  break;
  case SOC_PPC_TRAP_CODE_1588_PACKET_4:
    str = "1588_packet_4";
  break;
  case SOC_PPC_TRAP_CODE_1588_PACKET_5:
    str = "1588_packet_5";
  break;
  case SOC_PPC_TRAP_CODE_1588_PACKET_6:
    str = "1588_packet_6";
  break;
  case SOC_PPC_TRAP_CODE_1588_PACKET_7:
    str = "1588_packet_7";
  break;
  case SOC_PPC_TRAP_CODE_FCOE_SRC_SA_MISMATCH:
    str = "fcoe_src_sa_mismatch";
  break;
  case SOC_PPC_TRAP_CODE_SIP_TRANSLANT:
    str = "sip_translant";
  break;
  case SOC_PPC_TRAP_CODE_ELK_REJECTED:
    str = "elk_rejected";
  break;
  case SOC_PPC_TRAP_CODE_TRILL_DISABLE_BRIDGE_IF_DESIGNATED:
    str = "trill_disable_bridge_if_designated";
  break;
  case SOC_PPC_TRAP_CODE_TRAP_ETH_OAM:
    str = "trap_eth_oam";
  break;
  case SOC_PPC_TRAP_CODE_TRAP_Y1731_O_MPLS_TP:
    str = "trap_y1731_o_mpls_tp";
  break;
  case SOC_PPC_TRAP_CODE_TRAP_Y1731_O_PWE:
    str = "trap_y1731_o_pwe";
  break;
  case SOC_PPC_TRAP_CODE_TRAP_BFD_O_IPV4:
    str = "trap_bfd_o_ipv4";
  break;
  case SOC_PPC_TRAP_CODE_TRAP_BFD_O_MPLS:
    str = "trap_bfd_o_mpls";
  break;
  case SOC_PPC_TRAP_CODE_TRAP_BFD_O_PWE:
    str = "trap_bfd_o_pwe";
  break;
  case SOC_PPC_TRAP_CODE_TRAP_BFDCC_O_MPLS_TP:
    str = "trap_bfdcc_o_mpls_tp";
  break;
  case SOC_PPC_TRAP_CODE_TRAP_BFDCV_O_MPLS_TP:
    str = "trap_bfdcv_o_mpls_tp";
  break;
  case SOC_PPC_TRAP_CODE_OAM_LEVEL:
    str = "trap_oam_level";
  break;
  case SOC_PPC_TRAP_CODE_OAM_PASSIVE:
    str = "trap_oam_passive";
  break;
  case SOC_PPC_TRAP_CODE_ARP_FLP_FAIL:
    str = "arp_flp_fail";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_0:
    str = "user_defined_0";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_1:
    str = "user_defined_1";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_2:
    str = "user_defined_2";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_3:
    str = "user_defined_3";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_4:
    str = "user_defined_4";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_5:
    str = "user_defined_5";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_6:
    str = "user_defined_6";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_7:
    str = "user_defined_7";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_8:
    str = "user_defined_8";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_9:
    str = "user_defined_9";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_10:
    str = "user_defined_10";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_11:
    str = "user_defined_11";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_12:
    str = "user_defined_12";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_13:
    str = "user_defined_13";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_14:
    str = "user_defined_14";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_15:
    str = "user_defined_15";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_16:
    str = "user_defined_16";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_17:
    str = "user_defined_17";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_18:
    str = "user_defined_18";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_19:
    str = "user_defined_19";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_20:
    str = "user_defined_20";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_21:
    str = "user_defined_21";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_22:
    str = "user_defined_22";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_23:
    str = "user_defined_23";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_24:
    str = "user_defined_24";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_25:
    str = "user_defined_25";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_26:
    str = "user_defined_26";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_27:
    str = "user_defined_27";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_28:
    str = "user_defined_28";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_29:
    str = "user_defined_29";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_30:
    str = "user_defined_30";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_31:
    str = "user_defined_31";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_32:
    str = "user_defined_32";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_33:
    str = "user_defined_33";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_34:
    str = "user_defined_34";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_35:
    str = "user_defined_35";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_36:
    str = "user_defined_36";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_37:
    str = "user_defined_37";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_38:
    str = "user_defined_38";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_39:
    str = "user_defined_39";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_40:
    str = "user_defined_40";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_41:
    str = "user_defined_41";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_42:
    str = "user_defined_42";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_43:
    str = "user_defined_43";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_44:
    str = "user_defined_44";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_45:
    str = "user_defined_45";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_46:
    str = "user_defined_46";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_47:
    str = "user_defined_47";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_48:
    str = "user_defined_48";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_49:
    str = "user_defined_49";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_50:
    str = "user_defined_50";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_51:
    str = "user_defined_51";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_52:
    str = "user_defined_52";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_53:
    str = "user_defined_53";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_54:
    str = "user_defined_54";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_55:
    str = "user_defined_55";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_56:
    str = "user_defined_56";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_57:
    str = "user_defined_57";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_58:
    str = "user_defined_58";
  break;
  case SOC_PPC_TRAP_CODE_USER_DEFINED_59:
    str = "user_defined_59";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_TRAP_EG_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_TRAP_EG_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_TRAP_EG_TYPE_NONE:
    str = "none";
    break;
  case SOC_PPC_TRAP_EG_TYPE_NO_VSI_TRANSLATION:
    str = "no_vsi_translation";
  break;
  case SOC_PPC_TRAP_EG_TYPE_VSI_MEMBERSHIP:
    str = "vsi_membership";
  break;
  case SOC_PPC_TRAP_EG_TYPE_ACC_FRM:
    str = "acc_frm";
  break;
  case SOC_PPC_TRAP_EG_TYPE_HAIR_PIN:
    str = "hair_pin";
  break;
  case SOC_PPC_TRAP_EG_TYPE_UNKNOWN_DA:
    str = "unknown_da";
  break;
  case SOC_PPC_TRAP_EG_TYPE_SPLIT_HORIZON:
    str = "split_horizon";
  break;
  case SOC_PPC_TRAP_EG_TYPE_PRIVATE_VLAN:
    str = "private_vlan";
  break;
  case SOC_PPC_TRAP_EG_TYPE_TTL_SCOPE:
    str = "ttl_scope";
  break;
  case SOC_PPC_TRAP_EG_TYPE_MTU_VIOLATION:
    str = "mtu_violation";
  break;
  case SOC_PPC_TRAP_EG_TYPE_TRILL_TTL_0:
    str = "trill_ttl_0";
  break;
  case SOC_PPC_TRAP_EG_TYPE_TRILL_SAME_INTERFACE:
    str = "trill_same_interface";
  break;
  case SOC_PPC_TRAP_EG_TYPE_TRILL_BOUNCE_BACK:
    str = "trill_bounce_back";
  break;
  case SOC_PPC_TRAP_EG_TYPE_DSS_STACKING:
    str = "dss_stacking";
    break;
  case SOC_PPC_TRAP_EG_TYPE_LAG_MULTICAST:
    str = "lag_multicast";
    break;
  case SOC_PPC_TRAP_EG_TYPE_EXCLUDE_SRC:
    str = "exclude_src";
    break;
  case SOC_PPC_TRAP_EG_TYPE_CFM_TRAP:
    str = "cfm_trap";
    break;
  case SOC_PPC_TRAP_EG_TYPE_ALL:
    str = "all";
  break;
  case SOC_PPC_TRAP_EG_TYPE_INVALID_OUT_PORT:
    str = "invalid_out_port";
  break;
  case SOC_PPC_TRAP_EG_TYPE_CNM_PACKET:
    str = "cnm_packet";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV4_VERSION_ERROR:
    str = "ipv4_version_error";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV4_CHECKSUM_ERROR:
    str = "ipv4_checksum_error";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV4_HEADER_LENGTH_ERROR:
    str = "ipv4_header_length_error";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV4_TOTAL_LENGTH_ERROR:
    str = "ipv4_total_length_error";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IP_TTL0:
    str = "ip_ttl0";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IP_HAS_OPTIONS:
    str = "ip_has_options";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IP_TTL1:
    str = "ip_ttl1";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV4_SIP_EQUAL_DIP:
    str = "ipv4_sip_equal_dip";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV4_DIP_ZERO:
    str = "ipv4_dip_zero";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_UNSPECIFIED_DESTINATION:
    str = "ipv6_unspecified_destination";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_LOOPBACK_ADDRESS:
    str = "ipv6_loopback_address";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_MULTICAST_SOURCE:
    str = "ipv6_multicast_source";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_UNSPECIFIED_SOURCE:
    str = "ipv6_unspecified_source";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_LOCAL_LINK_DESTINATION:
    str = "ipv6_local_link_destination";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_LOCAL_SITE_DESTINATION:
    str = "ipv6_local_site_destination";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_LOCAL_LINK_SOURCE:
    str = "ipv6_local_link_source";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_LOCAL_SITE_SOURCE:
    str = "ipv6_local_site_source";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_IPV4_COMPATIBLE_DESTINATION:
    str = "ipv6_ipv4_compatible_destination";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_IPV4_MAPPED_DESTINATION:
    str = "ipv6_ipv4_mapped_destination";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_MULTICAST_DESTINATION:
    str = "ipv6_multicast_destination";
  break;
  case SOC_PPC_TRAP_EG_TYPE_IPV6_HOP_BY_HOP:
    str = "ipv6_hop_by_hop";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE_NORMAL:
    str = "normal";
  break;
  case SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE_TRAP:
    str = "trap";
  break;
  case SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE_SNOOP:
    str = "snoop";
  break;
  case SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE_IN_MIRROR:
    str = "in_mirror";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_oam_dest: %u\n\r"),info->is_oam_dest));
  LOG_CLI((BSL_META_U(unit,
                      "frwrd_dest:")));
  SOC_PPC_FRWRD_DECISION_INFO_print(&(info->frwrd_dest));
  LOG_CLI((BSL_META_U(unit,
                      "add_vsi: %u\n\r"),info->add_vsi));
  LOG_CLI((BSL_META_U(unit,
                      "vsi_shift: %u\n\r"),info->vsi_shift));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "counter_select: %u\n\r"),info->counter_select));
  LOG_CLI((BSL_META_U(unit,
                      "counter_id: %u\n\r"),info->counter_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "meter_select: %u\n\r"),info->meter_select));
  LOG_CLI((BSL_META_U(unit,
                      "meter_id: %u\n\r"),info->meter_id));
  LOG_CLI((BSL_META_U(unit,
                      "meter_command: %u\n\r"),info->meter_command));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ethernet_police_id: %u\n\r"),info->ethernet_police_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"), info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"), info->dp));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "enable_learning: %u\n\r"),info->enable_learning));
  LOG_CLI((BSL_META_U(unit,
                      "is_trap: %u\n\r"),info->is_trap));
  LOG_CLI((BSL_META_U(unit,
                      "is_control: %u\n\r"),info->is_control));
  LOG_CLI((BSL_META_U(unit,
                      "frwrd_offset_index: %u\n\r"),info->frwrd_offset_index));
  LOG_CLI((BSL_META_U(unit,
                      "frwrd_type: %s\n\r"),SOC_TMC_PKT_FRWRD_TYPE_to_string(info->frwrd_type)));
  LOG_CLI((BSL_META_U(unit,
                      "frwrd_offset_bytes_fix: %u\n\r"),info->frwrd_offset_bytes_fix));
  LOG_CLI((BSL_META_U(unit,
                      "da_type: %s\n\r"),soc_sand_SAND_PP_ETHERNET_DA_TYPE_to_string(info->da_type)));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "bitmap_mask:\n\r")));
  for (indx = 0; indx < SOC_PPC_NOF_TRAP_ACTION_PROFILE_OVERWRITES_ARAD; ++indx)
  {
    if (SOC_SAND_BIT(indx) & info->bitmap_mask)
    {
      LOG_CLI((BSL_META_U(unit,
                          "Event type: %s \n\r"), SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_to_string(SOC_SAND_BIT(indx))));
    }
  }
  LOG_CLI((BSL_META_U(unit,
                      "strength: %u\n\r"),info->strength));
  LOG_CLI((BSL_META_U(unit,
                      "dest_info:")));
  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO_print(&(info->dest_info));
  LOG_CLI((BSL_META_U(unit,
                      "cos_info:")));
  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_print(&(info->cos_info));
  LOG_CLI((BSL_META_U(unit,
                      "count_info:")));
  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO_print(&(info->count_info));
  LOG_CLI((BSL_META_U(unit,
                      "meter_info:")));
  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO_print(&(info->meter_info));
  LOG_CLI((BSL_META_U(unit,
                      "policing_info:")));
  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO_print(&(info->policing_info));
  LOG_CLI((BSL_META_U(unit,
                      "processing_info:")));
  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO_print(&(info->processing_info));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "strength: %u\n\r"),info->strength));
  LOG_CLI((BSL_META_U(unit,
                      "snoop_cmnd: %u\n\r"),info->snoop_cmnd));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_EG_ACTION_HDR_DATA_print(
    SOC_SAND_IN  SOC_PPC_TRAP_EG_ACTION_HDR_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "CUD: %u\n\r"),info->cud));
  LOG_CLI((BSL_META_U(unit,
                      "DSP ptr: %u\n\r"),info->dsp_ptr));
  LOG_CLI((BSL_META_U(unit,
                      "COS Profile: %u\n\r"),info->cos_profile));
  LOG_CLI((BSL_META_U(unit,
                      "Mirror Profile: %u\n\r"),info->mirror_profile));
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "bitmap_mask: %u\n\r"),info->bitmap_mask));
  LOG_CLI((BSL_META_U(unit,
                      "out_tm_port: %u\n\r"),info->out_tm_port));
  LOG_CLI((BSL_META_U(unit,
                      "cos_info:")));
  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_print(&(info->cos_info));
  LOG_CLI((BSL_META_U(unit,
                      "cud: %u\n\r"),info->cud));
  SOC_PPC_TRAP_EG_ACTION_HDR_DATA_print(&(info->header_data));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_MACT_EVENT_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Event type: %s \n\r"), SOC_PPC_TRAP_MACT_EVENT_TYPE_to_string(info->type)));
  LOG_CLI((BSL_META_U(unit,
                      "key:\n\r")));
  SOC_PPC_FRWRD_MACT_ENTRY_KEY_print(&(info->key));
  LOG_CLI((BSL_META_U(unit,
                      "val:\n\r")));
  SOC_PPC_FRWRD_MACT_ENTRY_VALUE_print(&(info->val));
  LOG_CLI((BSL_META_U(unit,
                      "is_part_of_lag: %u\n\r"),info->is_part_of_lag));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PPC_TRAP_PACKET_INFO_print(
    SOC_SAND_IN  SOC_PPC_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Event type: %s \n\r"), SOC_PPC_TRAP_MGMT_PKT_FRWRD_TYPE_to_string(info->frwrd_type)));
  LOG_CLI((BSL_META_U(unit,
                      "is_trapped: %u\n\r"),info->is_trapped));
  if (info->is_hw_trap_code == TRUE)
  {
      LOG_CLI((BSL_META_U(unit,
                          "cpu_trap_code: %u (HW ID)\n\r"), info->cpu_trap_code));  
  }
  else
  {
      LOG_CLI((BSL_META_U(unit,
                          "cpu_trap_code: %s (%u) (BCM API: %s)\n\r"), SOC_PPC_TRAP_CODE_to_string(info->cpu_trap_code), info->cpu_trap_code, SOC_PPC_TRAP_CODE_to_api_string(info->cpu_trap_code)));
  }
  LOG_CLI((BSL_META_U(unit,
                      "cpu_trap_qualifier: %u\n\r"),info->cpu_trap_qualifier));
  LOG_CLI((BSL_META_U(unit,
                      "src_sys_port: %u\n\r"),info->src_sys_port));
  LOG_CLI((BSL_META_U(unit,
                      "OTM_port: %u\n\r"),info->otm_port));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_mcid: %u\n\r"),info->outlif_mcid));
  LOG_CLI((BSL_META_U(unit,
                      "tc: %u\n\r"),info->tc));
  LOG_CLI((BSL_META_U(unit,
                      "dp: %u\n\r"),info->dp));

  LOG_CLI((BSL_META_U(unit,
                      "size_in_bytes: %u [Byte]\n\r"),info->size_in_bytes));
  LOG_CLI((BSL_META_U(unit,
                      "snoop_cmnd: %u \n\r"),info->snoop_cmnd));
  LOG_CLI((BSL_META_U(unit,
                      "ntwrk_header_ptr: %u \n\r"),info->ntwrk_header_ptr));
  LOG_CLI((BSL_META_U(unit,
                      "user_header_ptr[0]: %u \n\r"),info->user_header_ptr[0]));
  LOG_CLI((BSL_META_U(unit,
                      "user_header_ptr[1]: %u \n\r"),info->user_header_ptr[1]));
  LOG_CLI((BSL_META_U(unit,
                      "lb_key: %u \n\r"),info->lb_key));
  LOG_CLI((BSL_META_U(unit,
                      "internal_unknown_addr: %u\n\r"),info->internal_unknown_addr));
  LOG_CLI((BSL_META_U(unit,
                      "internal_fwd_hdr_offset: %u \n\r"),info->internal_fwd_hdr_offset));
  LOG_CLI((BSL_META_U(unit,
                      "dsp_ext: %u \n\r"),info->dsp_ext));
  LOG_CLI((BSL_META_U(unit,
                      "stacking_ext: %u \n\r"),info->stacking_ext));
  LOG_CLI((BSL_META_U(unit,
                      "outlif_mcid: %u\n\r"),info->outlif_mcid));
  LOG_CLI((BSL_META_U(unit,
                      "internal_inlif_inrif: %u\n\r"),info->internal_inlif_inrif));

  if (info->in_ac != SOC_PPC_AC_ID_INVALID) {
	  LOG_CLI((BSL_META_U(unit,
                              "in_ac: %u [Byte]\n\r"), info->in_ac));
  }
  else {
	  LOG_CLI((BSL_META_U(unit,
                              "in_ac: Invalid\n\r")));
  }
  LOG_CLI((BSL_META_U(unit,
                      "rif: %u [Byte]\n\r"), info->rif));
  if (info->vrf != SOC_PPC_VRF_INVALID) {
	  LOG_CLI((BSL_META_U(unit,
                              "vrf: %u [Byte]\n\r"), info->vrf));
  }
  else {
	  LOG_CLI((BSL_META_U(unit,
                              "vrf: Invalid\n\r")));
  }

  LOG_CLI((BSL_META_U(unit,
                      "dsp_ext: %u \n\r"),info->dsp_ext));
  LOG_CLI((BSL_META_U(unit,
                      "stacking_ext: %u \n\r"),info->stacking_ext));

  LOG_CLI((BSL_META_U(unit,
                      "oam_type: %u \n\r"),info->oam_type));
  LOG_CLI((BSL_META_U(unit,
                      "oam_offset: %u \n\r"),info->oam_offset));
  LOG_CLI((BSL_META_U(unit,
                      "oam_sub_type: %u \n\r"),info->oam_sub_type));
  LOG_CLI((BSL_META_U(unit,
                      "mep_type: %u \n\r"),info->mep_type));
  LOG_CLI((BSL_META_U(unit,
                      "oam_tp_cmd: %u \n\r"),info->oam_tp_cmd));
  LOG_CLI((BSL_META_U(unit,
                      "oam_ts_encaps: %u \n\r"),info->oam_ts_encaps));
#ifdef COMPILER_HAS_LONGLONG					  
  LOG_CLI((BSL_META_U(unit,
                      "oam_data: %llu (0x%llx) \n\r"),(long long unsigned)info->oam_data, (long long unsigned)info->oam_data ));
#else
  LOG_CLI((BSL_META_U(unit,
                      "oam_data: %u %u \n\r"),COMPILER_64_HI(info->oam_data), COMPILER_64_LO(info->oam_data) ));

#endif
  LOG_CLI((BSL_META_U(unit,
                      "ext_mac_1588: %u \n\r"),info->ext_mac_1588));

  LOG_CLI((BSL_META_U(unit,
                      "tdm_pkt_size: %u \n\r"),info->tdm_pkt_size));
  LOG_CLI((BSL_META_U(unit,
                      "tdm_type: %u \n\r"),info->tdm_type));
  LOG_CLI((BSL_META_U(unit,
                      "tdm_dest_fap_id: %u \n\r"),info->tdm_dest_fap_id));
  LOG_CLI((BSL_META_U(unit,
                      "tdm_otm_port: %u \n\r"),info->tdm_otm_port));
  LOG_CLI((BSL_META_U(unit,
                      "tdm_mc_id: %u \n\r"),info->tdm_mc_id));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>

