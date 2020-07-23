/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_OAM_INCLUDED__

#define __ARAD_PP_OAM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_oam.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/JER/jer_mgmt.h>





#define _ARAD_PP_OAM_SUBTYPE_DEFAULT 0
#define _ARAD_PP_OAM_SUBTYPE_LM             1
#define _ARAD_PP_OAM_SUBTYPE_DM_1588        2
#define _ARAD_PP_OAM_SUBTYPE_DM_NTP         3
#define _ARAD_PP_OAM_SUBTYPE_CCM            4
#define _ARAD_PP_OAM_SUBTYPE_ETH_LOOPBACK   5
#define _ARAD_PP_OAM_SUBTYPE_DEFAULT_EGRESS_OAM 6
#define _ARAD_PP_OAM_SUBTYPE_SLM            7 
#define _ARAD_PP_OAM_SUBTYPE_ECN_DM         7 

#define _ARAD_PP_OAM_SUBTYPE_DEFAULT_OAM_MESSAGE        _ARAD_PP_OAM_SUBTYPE_CCM
 

#define ARAD_PP_SLM_PASSIVE_PROFILE (15) 

#define _ARAD_PP_OAM_TRAP_STRENGTH \
  ((SOC_DPP_CONFIG(unit)->pp.oam_default_trap_strength)?(SOC_DPP_CONFIG(unit)->pp.oam_default_trap_strength):(soc_property_get(unit, spn_DEFAULT_TRAP_STRENGTH, 6)))

#define _ARAD_PP_OAM_SNOOP_STRENGTH \
  soc_property_get(unit, spn_DEFAULT_SNOOP_STRENGTH, 3)

#define _ARAD_PP_OAM_FORWARD_TRAP_STRENGTH \
  ((SOC_DPP_CONFIG(unit)->pp.oam_forward_trap_strength)?(SOC_DPP_CONFIG(unit)->pp.oam_forward_trap_strength):(_ARAD_PP_OAM_TRAP_STRENGTH))

#define _ARAD_PP_OAM_LEVEL_TRAP_STRENGTH \
  ((SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_level)?(SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_level):(_ARAD_PP_OAM_TRAP_STRENGTH))

#define _ARAD_PP_OAM_PASSIVE_TRAP_STRENGTH \
  ((SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_passive)?(SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_passive):(_ARAD_PP_OAM_TRAP_STRENGTH))

#define _ARAD_PP_OAM_INJECTED_TRAP_STRENGTH \
  ((SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_injected)?(SOC_DPP_CONFIG(unit)->pp.oam_trap_strength_injected):(_ARAD_PP_OAM_TRAP_STRENGTH))

#define OAM_LIF_MAX_VALUE(unit) ((1 <<SOC_PPC_OAM_SIZE_OF_OAM_KEY_IN_BITS(unit)) -1)

#define ARAD_PP_OAM_LARGEST_NUMBER_OF_ENTRIES_FOR_DMA_HOST_MEMORY 0x4000


#define _ARAD_PP_OAM_RMEP_SCAN(unit, freq_arad_khz)    \
      SOC_SAND_DIV_ROUND_DOWN((SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit) * (SOC_IS_JERICHO(unit) ? 10 : 20)), SOC_SAND_DIV_ROUND_DOWN(freq_arad_khz, 1000))



#define ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(_unit) (SOC_DPP_DEFS_GET((_unit), oamp_number_mep_db_entries))


#define ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_SHARED_MAX_CHAIN_LEN(_unit)       4


#define _QUX_PP_OAM_DIRECTION_BIT_ON_ADDITION (SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * SOC_DPP_DEFS_GET(unit, nof_counter_processors))






#define ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_NEXT(_unit, _entry)        ((_entry) + (SOC_IS_QAX(_unit) ? (SOC_IS_QUX(unit) ? 256 : 1024) : 1))
#define ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_CHAIN_PREV(_unit, _entry)        ((_entry) - (SOC_IS_QAX(_unit) ? (SOC_IS_QUX(unit) ? 256 : 1024) : 1))

#define OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(__entry_id)               ((__entry_id)>>(SOC_DPP_DEFS_GET((unit), max_oamp_mep_db_entry_id_bits)))
#define OAMP_MEP_DB_ENTRY_ID_TO_INDEX(__entry_id)               ((__entry_id)&((1<<(SOC_DPP_DEFS_GET((unit), max_oamp_mep_db_entry_id_bits)))-1))

#define _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DBm(unit, blk, index, data) (SOC_IS_QAX(unit) ?\
                                                                         soc_mem_array_read(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data) : \
                                                                         soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_MEP_DBm, 0, blk, index, data))
#define _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DBm(unit, blk, index, data) (SOC_IS_QAX(unit) ?\
                                                                          soc_mem_array_write(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data) :\
                                                                          soc_mem_write(unit, OAMP_MEP_DBm, blk, index, data))
#define _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_LM_STATm(unit, blk, index, data) (SOC_IS_QAX(unit) ?\
                                                                         soc_mem_array_read(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data) : \
                                                                         soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_MEP_DB_LM_STATm, 0, blk, index, data))
#define _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_STATm(unit, blk, index, data) (SOC_IS_QAX(unit) ?\
                                                                          soc_mem_array_write(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data) :\
                                                                          soc_mem_write(unit, OAMP_MEP_DB_LM_STATm, blk, index, data))
#define _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_LM_DBm(unit, blk, index, data) (SOC_IS_QAX(unit) ?\
                                                                         soc_mem_array_read(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data) : \
                                                                         soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, OAMP_MEP_DB_LM_DBm, 0, blk, index, data))
#define _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_LM_DBm(unit, blk, index, data) (SOC_IS_QAX(unit) ?\
                                                                          soc_mem_array_write(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data) :\
                                                                          soc_mem_write(unit, OAMP_MEP_DB_LM_DBm, blk, index, data))

#define _ARAD_PP_OAM_INTERNAL_READ_OAMP_MEP_DB_x_BY_MEM(unit, mem, blk, index, data) (SOC_IS_QAX(unit) ?\
                                                                         soc_mem_array_read(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data) : \
                                                                         soc_mem_read_no_cache(unit, SOC_MEM_NO_FLAGS, mem, 0, blk, index, data))
#define _ARAD_PP_OAM_INTERNAL_WRITE_OAMP_MEP_DB_x_BY_MEM(unit, mem, blk, index, data) (SOC_IS_QAX(unit) ?\
                                                                       soc_mem_array_write(unit, OAMP_MEP_DBm, OAMP_MEP_DB_ENTRY_ID_TO_BLOCK(index), blk, OAMP_MEP_DB_ENTRY_ID_TO_INDEX(index), data) :\
                                                                       soc_mem_write(unit, mem, blk, index, data))





#define ARAD_PP_OAM_MY_MAC_LSB_REF_COUNT_NUM 256*256 

#define ARAD_PP_OAM_SUBTYPE_EGRESS_SNOOP 6

#define _ARAD_PP_OAM_TCAM_MAX_ENTRY_ID 127

#define ARAD_PP_OAM_BFD_SEAMLESS_BFD_FEATURE_ENABLED(unit) ((SOC_DPP_CONFIG(unit))->pp.bfd_sbfd_enable == 1)

#define _ARAD_PP_OAM_SBFD_REFLECTOR_MYMAC_VLAN             101



#define _ARAD_PP_OAM_TCAM_FLAGS_MPLS_IGNORE_MDL_JER        0x1
#define _ARAD_PP_OAM_TCAM_FLAGS_Y1711                      0x2
#define _ARAD_PP_OAM_TCAM_FLAGS_SBFD_INITIATOR              0x4
#define _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR             0x8
#define _ARAD_PP_OAM_TCAM_FLAGS_SBFD_REFLECTOR_OVER_MPLS   0x10
#define _ARAD_PP_OAM_TCAM_FLAGS_BFD_OVER_MPLS_SPECIAL_TTL  0x20



#define _ARAD_PP_OAM_SBFD_INIT_TCAM_ENTRY_CALC(is_acc, dip_index) (is_acc?(_ARAD_PP_OAM_TCAM_MAX_ENTRY_ID - dip_index):(_ARAD_PP_OAM_TCAM_MAX_ENTRY_ID - 16 - dip_index))

typedef struct
{
  PARSER_HINT_ARR_ARR uint16 **ref_count;
} ARAD_PP_OAM_MY_MAC_LSB;

typedef enum
{
  
  ARAD_PP_OAM_INIT = ARAD_PP_PROC_DESC_BASE_OAM_FIRST,
  ARAD_PP_OAM_INIT_UNSAFE,
  ARAD_PP_OAM_DEINIT,
  ARAD_PP_OAM_DEINIT_UNSAFE,
  ARAD_PP_OAM_ICC_MAP_REGISTER_SET,
  ARAD_PP_OAM_ICC_MAP_REGISTER_SET_UNSAFE,
  ARAD_PP_OAM_ICC_MAP_REGISTER_SET_VERIFY,
  ARAD_PP_OAM_ICC_MAP_REGISTER_GET,
  ARAD_PP_OAM_ICC_MAP_REGISTER_GET_UNSAFE,
  ARAD_PP_OAM_ICC_MAP_REGISTER_GET_VERIFY,
  ARAD_PP_OAM_MY_CFM_MAC_SET,
  ARAD_PP_OAM_MY_CFM_MAC_DELETE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_UNSAFE,  
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_UNSAFE, 
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_SET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_SET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRY_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM2_ENTRY_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_MEP_DELETE,
  ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_MEP_DELETE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_MEP_GET,
  ARAD_PP_OAM_CLASSIFIER_MEP_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_MEP_GET_VERIFY,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_UNSAFE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_VERIFY,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_UNSAFE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_VERIFY,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_UNSAFE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_SET,
  ARAD_PP_OAM_OAMP_RMEP_SET_UNSAFE,
  ARAD_PP_OAM_OAMP_RMEP_SET_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_GET,
  ARAD_PP_OAM_OAMP_RMEP_GET_UNSAFE,
  ARAD_PP_OAM_OAMP_RMEP_GET_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_DELETE,
  ARAD_PP_OAM_OAMP_RMEP_DELETE_UNSAFE,
  ARAD_PP_OAM_OAMP_RMEP_DELETE_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_INDEX_GET,
  ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_VERIFY,
  ARAD_PP_OAM_OAMP_RMEP_INDEX_GET_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL,
  ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL_VERIFY,
  ARAD_PP_OAM_OAMP_INIT_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_INIT_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_UNSAFE,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD_VERIFY,
  ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET_INTERNAL_UNSAFE,
  ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET_INTERNAL_UNSAFE,
  ARAD_PP_OAM_COUNTER_RANGE_SET_UNSAFE,
  ARAD_PP_OAM_COUNTER_RANGE_SET_VERIFY,
  ARAD_PP_OAM_COUNTER_RANGE_SET,
  ARAD_PP_OAM_COUNTER_RANGE_GET_UNSAFE,
  ARAD_PP_OAM_COUNTER_RANGE_GET_VERIFY,
  ARAD_PP_OAM_COUNTER_RANGE_GET,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET,
  ARAD_PP_OAM_BFD_TX_RATE_SET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_RATE_SET_VERIFY,
  ARAD_PP_OAM_BFD_TX_RATE_SET,
  ARAD_PP_OAM_BFD_TX_RATE_GET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_RATE_GET_VERIFY,
  ARAD_PP_OAM_BFD_TX_RATE_GET,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_UNSAFE,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET_VERIFY,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_UNSAFE,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET_VERIFY,
  ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_UNSAFE,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_SET_VERIFY,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_SET,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_UNSAFE,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_GET_VERIFY,
  ARAD_PP_OAM_MPLS_PWE_PROFILE_GET,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_UNSAFE,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET_VERIFY,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_UNSAFE,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET_VERIFY,
  ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_UNSAFE,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET_VERIFY,
  ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_UNSAFE,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET_VERIFY,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_UNSAFE,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET_VERIFY,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_UNSAFE,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET_VERIFY,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_UNSAFE,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET_VERIFY,
  ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_UNSAFE,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET_VERIFY,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_UNSAFE,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET_VERIFY,
  ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET_VERIFY,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET_VERIFY,
  ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET,
  ARAD_PP_OAM_BFD_TX_MPLS_SET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_MPLS_SET_VERIFY,
  ARAD_PP_OAM_BFD_TX_MPLS_SET,
  ARAD_PP_OAM_BFD_TX_MPLS_GET_UNSAFE,
  ARAD_PP_OAM_BFD_TX_MPLS_GET_VERIFY,
  ARAD_PP_OAM_BFD_TX_MPLS_GET,
  ARAD_PP_OAM_BFDCC_TX_MPLS_SET_UNSAFE,
  ARAD_PP_OAM_BFDCC_TX_MPLS_SET_VERIFY,
  ARAD_PP_OAM_BFDCC_TX_MPLS_SET,
  ARAD_PP_OAM_BFDCC_TX_MPLS_GET_UNSAFE,
  ARAD_PP_OAM_BFDCC_TX_MPLS_GET_VERIFY,
  ARAD_PP_OAM_BFDCC_TX_MPLS_GET,
  ARAD_PP_OAM_BFDCV_TX_MPLS_SET_UNSAFE,
  ARAD_PP_OAM_BFDCV_TX_MPLS_SET_VERIFY,
  ARAD_PP_OAM_BFDCV_TX_MPLS_SET,
  ARAD_PP_OAM_BFDCV_TX_MPLS_GET_UNSAFE,
  ARAD_PP_OAM_BFDCV_TX_MPLS_GET_VERIFY,
  ARAD_PP_OAM_BFDCV_TX_MPLS_GET,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_UNSAFE,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET_VERIFY,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_UNSAFE,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET_VERIFY,
  ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_UNSAFE,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET_VERIFY,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_UNSAFE,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET_VERIFY,
  ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET,
  ARAD_PP_OAM_EVENT_FIFO_READ_UNSAFE,
  ARAD_PP_OAM_EVENT_FIFO_READ_VERIFY,
  ARAD_PP_OAM_EVENT_FIFO_READ,
  ARAD_PP_OAM_PP_PCT_PROFILE_SET_UNSAFE,
  ARAD_PP_OAM_PP_PCT_PROFILE_SET_VERIFY,
  ARAD_PP_OAM_PP_PCT_PROFILE_SET,
  ARAD_PP_OAM_PP_PCT_PROFILE_GET_UNSAFE,
  ARAD_PP_OAM_PP_PCT_PROFILE_GET_VERIFY,
  ARAD_PP_OAM_PP_PCT_PROFILE_GET,

  

  
  ARAD_PP_OAM_PROCEDURE_DESC_LAST
} ARAD_PP_OAM_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_OAM_CLASSIFIER_DA_SUFFIX_CONFLICT_ERR = ARAD_PP_ERR_DESC_BASE_OAM_FIRST,
  ARAD_PP_OAM_OAMP_SA_SUFFIX_CONFLICT_ERR,
  ARAD_PP_OAM_CLASSIFIER_INCONSISTENT_LIF_DATA_ERR,
  ARAD_PP_OAM_CLASSIFIER_MDLEVEL_CHECK_FAIL_ERR,
  ARAD_PP_OAM_CLASSIFIER_ENTRY_EXISTS_ERR,
  ARAD_PP_OAM_CLASSIFIER_ENTRY_NOT_FOUND_ERR,
  ARAD_PP_OAM_MEP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_RMEP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_EM_FULL_ERR,
  ARAD_PP_OAM_EM_INTERNAL_ERR,
  ARAD_PP_OAM_EM_INSERTED_EXISTING_ERR,
  ARAD_PP_OAM_MD_LEVEL_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_FORWARDING_SRTENGTH_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_MA_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_MEP_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_NON_ACC_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_ACC_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_PROFILE_OPCODE_MAP_ERROR,
  ARAD_PP_OAM_TRAP_TO_MIRROR_PROFILE_MAP_ERROR,
  ARAD_PP_OAM_TX_RATE_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_REQ_INTERVAL_POINTER_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_DIP_INDEX_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_EXP_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_TOS_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_TTL_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_DP_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_TC_OUT_OF_RANGE_ERR,
  ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_VALUE_OF_RANGE_ERR,
  ARAD_PP_OAM_YOUR_DISC_ABOVE_MAX_ERROR,
  ARAD_PP_OAM_INTERNAL_ERROR,

  

  
  ARAD_PP_OAM_ERR_LAST
} ARAD_PP_OAM_ERR;


typedef enum
{
    ARAD_PP_OAM_DEFAULT_EP_INGRESS_0 = 0,
    ARAD_PP_OAM_DEFAULT_EP_INGRESS_1,
    ARAD_PP_OAM_DEFAULT_EP_INGRESS_2,
    ARAD_PP_OAM_DEFAULT_EP_INGRESS_3,

    ARAD_PP_OAM_DEFAULT_EP_EGRESS_0,
    ARAD_PP_OAM_DEFAULT_EP_EGRESS_1,
    ARAD_PP_OAM_DEFAULT_EP_EGRESS_2,
    ARAD_PP_OAM_DEFAULT_EP_EGRESS_3,

    ARAD_PP_OAM_DEFAULT_EP_NOF_IDS

} ARAD_PP_OAM_DEFAULT_EP_ID;


typedef enum
{
    ARAD_PP_OAM_OPCODE_OTHER = 0,
    ARAD_PP_OAM_OPCODE_CCM   = 1,
    ARAD_PP_OAM_OPCODE_LM    = 2,
    ARAD_PP_OAM_OPCODE_DM    = 3,
    ARAD_PP_OAM_OPCODE_LBM   = 4,
    ARAD_PP_OAM_OPCODE_LBR   = 5,
    ARAD_PP_OAM_OPCODE_SLM   = 6

} ARAD_PP_OAM_OPCODE_TYPE;

typedef struct {
    ARAD_PP_OAM_OPCODE_TYPE     additional_info; 
    uint8                       sub_type;
} ARAD_PP_INTERNAL_OPCODE_INIT;



typedef struct {
    uint32  trap_code_recycle;
    uint32  trap_code_snoop;
    uint32  trap_code_frwrd;
    uint32  trap_code_drop;
    uint32  trap_code_trap_to_cpu_level;
    uint32  trap_code_trap_to_cpu_passive;
    uint32  mirror_profile_snoop_to_cpu;
    uint32  mirror_profile_recycle;
    uint32  mirror_profile_err_level;
    uint32  mirror_profile_err_passive;
    uint32  tcam_last_entry_id;
    uint32  lb_trap_used_cnt;
    uint32  tst_trap_used_cnt;	
} ARAD_PP_OAM_SW_STATE;

typedef struct {
    uint8  oamp_pe_init;
} ARAD_PP_OAMP_PE_SW_STATE;

typedef struct {
    uint32  trap_code_trap_to_cpu;
    uint32  trap_code_oamp_bfd_cc_mpls_tp;
    uint32  trap_code_oamp_bfd_ipv4;
    uint32  trap_code_oamp_bfd_mpls;
    uint32  trap_code_oamp_bfd_pwe;
} ARAD_PP_BFD_SW_STATE;

typedef enum {
ARAD_PP_OAM_DATA_TST_TLV_NULL_NO_CRC            = 0,
ARAD_PP_OAM_DATA_TST_TLV_NULL_WITH_CRC          = 1,
ARAD_PP_OAM_DATA_TST_TLV_PRBS_NO_CRC            = 2,
ARAD_PP_OAM_DATA_TST_TLV_PRBS_WITH_CRC          = 3
} ARAD_PP_OAM_DATA_TST_TLV_TYPE;

typedef enum {
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_CONTROL_ENUM = 0, 
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_PW_ACH_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_DLM_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_ILM_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_DM_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV4_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_IPV6_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_CC_ENUM, 
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_CV_ENUM, 
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_ON_DEMAND_CV_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_PWE_ON_OAM_ENUM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_Y1731_ENUM, 
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_FAULT_OAM,
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_BFD_VCCV, 
    _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM_NOF
} _ARAD_PP_OAM_MPLS_TP_CHANNEL_TYPE_ENUM;

typedef struct {
    
    uint32                          endpoint_id;          
    uint32                          tx_period;            
    uint8                           is_period_in_pps;    
    uint8                           is_added;             
    uint8                           has_tst_tlv;          
    uint16                          tst_tlv_len;          
    ARAD_PP_OAM_DATA_TST_TLV_TYPE   tst_tlv_type;         
    SOC_SAND_PP_MAC_ADDRESS         mac_address;          
    uint8                           pkt_pri;              
    uint8                           inner_pkt_pri;        
    int                             int_pri;              
    uint8                           is_up_mep;
    uint32                          sys_port;             
    uint32                          local_port;            
    
    uint32                         rx_packet_count;
    uint32                         tx_packet_count;
    uint32                         discard_count;
    uint32                         fifo_count;

} ARAD_PP_OAM_LOOPBACK_INFO;


typedef enum ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_E {
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_NONE = 0,

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_ADD_UPDATE, 
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE_LM, 
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_REMOVE_DM,

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE_NOF
} ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE;

typedef enum ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_E {
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_NONE = 0,

    
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM,
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_STAT,
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_LM_WITH_STAT,
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM,
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_LM_STAT,

    
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_1WAY,
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_ADD_DM_2WAY,
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_REMOVE_DM,

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION_NOF
} ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION;


typedef struct ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO_S {

    
    SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *lm_dm_entry;

    
    uint32 endpoint_id;

    
    uint32  entries_to_add[2]; 
    int     num_entries_to_add;

    uint32  entries_to_remove[2]; 
    int     num_entries_to_remove;


    
    uint32 dm_entry; 
    uint32 lm_entry; 
    uint32 lm_stat_entry; 
    uint32 last_entry; 
    uint8 mep_scanned; 
    uint32 is_maid_48; 
    uint32 out_lif;

    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_ACTION_TYPE action_type; 

    
    ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ACTION action; 
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_entry; 

} ARAD_PP_OAM_OAMP_MEP_DB_LM_DM_ENTRIES_INFO;



typedef struct {
    uint8   field1;
    uint8   field2;
    uint8   field3;
    uint8   field4;
    uint8   field5;
    uint8   field6;
    uint8   field7;
    uint8   field8;
    uint16  field9;
    uint32  field10; 
} PPH_base;

#define _ARAD_PP_OAM_OEM2_KEY_TO_KEY_STRUCT(key_struct, key)\
do { key_struct.ingress = key & 0x1;\
	key_struct.mdl = (key & 0xe) >>1;\
	key_struct.oam_lif = (key & (SOC_IS_JERICHO(unit)? 0x3ffff0: 0xffff0)) >>4;\
	key_struct.your_disc = ((key ) >>(20 + 2 * SOC_IS_JERICHO(unit))) & 1 ;\
} while (0)

#define _ARAD_PP_OAM_INDEX_TO_HW_LEGAL_INDEX(mep_db_index) \
      (SOC_IS_QUX(unit) ? ((((mep_db_index) & 0x3F00) << 2) + ((mep_db_index) & 0xFF)) : (mep_db_index))

#define _ARAD_PP_OAM_HW_INDEX_TO_ACTUAL_INDEX(mep_db_index) \
      (SOC_IS_QUX(unit) ? ((((mep_db_index) & 0x3F00) >> 2) + ((mep_db_index) & 0xFF)) : (mep_db_index))



#define OAM_2_ID_WIDTH(unit) \
     ((SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? (0x3fff) : (0x1fff))

#define _ARAD_PP_OAM_OEM2_PAYLOAD_STRUCT_TO_PAYLOAD(payload_struct, payload) \
    payload = (payload_struct.mp_profile) + (payload_struct.oam_id << 4)

#define _ARAD_PP_OAM_OEM2_PAYLOAD_TO_PAYLOAD_STRUCT(payload, payload_struct)         \
  do {                                                                               \
    payload_struct.mp_profile  = (payload)       & 0xf;                              \
    payload_struct.oam_id      = (payload >> 4)  & OAM_2_ID_WIDTH(unit);             \
  } while (0)

#define _ARAD_PP_OAM_OEM1_KEY_TO_KEY_STRUCT(key_struct, key) \
	   do {\
       key_struct.ingress = key & 1; \
       if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
          key_struct.oam_lif = (key & 0x7FFFE) >>1;\
          key_struct.your_discr = (key & 0x80000) >> 19;\
       } else {\
          key_struct.oam_lif = (key & 0xfffe ) >>1;}\
       } while (0)

#define _ARAD_PP_OAM_PRINT_OEM1_KEY(oem1_key)\
do {\
    if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
        LOG_CLI((BSL_META_U(unit,\
                                  "\tOEMA key: ingress: %d, Your-Discriminator: %d ,OAM LIF: 0x%x\n"),  oem1_key.ingress, oem1_key.your_discr,oem1_key.oam_lif)); \
    } else {\
        LOG_CLI((BSL_META_U(unit,\
                                  "\tOEMA key: ingress: %d, OAM LIF: 0x%x\n"),  oem1_key.ingress, oem1_key.oam_lif)); \
    }\
} while (0)

#define _ARAD_PP_OAM_OEM1_PAYLOAD_TO_PAYLOAD_STRUCT(payload, payload_struct)                    \
  do {                                                                                          \
      if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
          payload_struct.counter_ndx = (payload[0] >> 16) & 0x7fff;\
          payload_struct.mp_type_vector = payload[0] & 0xffff;\
          payload_struct.mp_profile = (payload[0] >>31) | ((payload[1] & 0x7)<<1);\
      }\
      else if (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)) {                                                         \
          payload_struct.mp_profile  = (((payload[0] >> 29) & 0x3) + ((payload[0] & 0x3) << 2)) ;       \
          payload_struct.counter_ndx = (payload[0] >> 16) & 0x1fff;    \
          payload_struct.mep_bitmap  = (payload[0] >> 8)  & 0xff;      \
          payload_struct.mip_bitmap  = (payload[0] >> 2)  & 0x3f;      \
      }                                                             \
      else {                                                        \
        payload_struct.mp_profile  = (payload[0] >> 29) & 0x3;         \
        payload_struct.counter_ndx = (payload[0] >> 16) & 0x1fff;      \
        payload_struct.mep_bitmap  = (payload[0] >> 8)  & 0xff;        \
        payload_struct.mip_bitmap  = (payload[0])       & 0xff;        \
      }                                                                                        \
  } while (0)

#define _ARAD_PP_OAM_PRINT_OEM1_PAYLOAD(oam1_payload)\
do {\
    if (SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) {\
    int mdl, mdl_mp_type;\
        LOG_CLI((BSL_META_U(unit,\
                            "\t\tOEMA payload: MP profile: 0x%x, MP-type-vector: 0x%x, counter index: 0x%x\n"), oem1_payload.mp_profile, oem1_payload.mp_type_vector,\
                                        oem1_payload.counter_ndx)); \
                          for (mdl=0; mdl<8 ;++mdl) {\
                              mdl_mp_type =  JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(oem1_payload.mp_type_vector, mdl);\
                              if (mdl_mp_type == _JER_PP_OAM_MDL_MP_TYPE_MIP) {\
                                  LOG_CLI((BSL_META_U(unit, "\t\t\tMIP on Level %d\n"),mdl));\
                              }\
                              if (mdl_mp_type == _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH) {\
                                  LOG_CLI((BSL_META_U(unit, "\t\t\tActive MEP on Level %d\n"),mdl));\
                              }\
                              if (mdl_mp_type == _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH) {\
                                  LOG_CLI((BSL_META_U(unit, "\t\t\tPassive MEP on Level %d\n"),mdl));\
                              }\
                         }\
} else {\
            LOG_CLI((BSL_META_U(unit,\
                                "\t\tOEMA payload: MP profile: 0x%x, MEP bitmap: 0x%x, MIP bitmap: 0x%x, counter index: 0x%x\n"), oem1_payload.mp_profile, oem1_payload.mep_bitmap,oem1_payload.mip_bitmap,\
                                            oem1_payload.counter_ndx)); \
    }\
} while (0)







extern volatile sal_sem_t     jer_oam_event_fifo_sem[SOC_SAND_MAX_DEVICE];
extern volatile sal_sem_t     jer_oam_stat_fifo_sem[SOC_SAND_MAX_DEVICE];




  

CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_oam_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_oam_get_errs_ptr(void);


uint32
  arad_pp_oam_init_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  uint8                                  is_bfd
  );




uint32
  arad_pp_oam_deinit_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  is_bfd,
    SOC_SAND_IN  uint8                                  tcam_db_destroy
  );


soc_error_t _arad_pp_oam_trap_code_to_internal(int unit, uint32 trap_code, uint32 *internal_trap_code);


uint32
  arad_pp_oam_icc_map_register_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ICC_MAP_DATA     * data
  );

uint32
  arad_pp_oam_icc_map_register_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ICC_MAP_DATA     * data
  );

uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                            is_bfd
  );

uint32
  arad_pp_oam_classifier_oam1_entries_insert_default_profile_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                            is_bfd
  );

uint32  arad_pp_oam_classifier_insert_according_to_profile_verify( SOC_SAND_IN  int                           unit,
                                                                   SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                   SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data);


uint32 arad_pp_oam_classifier_entries_insert_passive_non_accelerated(SOC_SAND_IN int unit,
                                                                     SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                     SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data);

uint32 arad_pp_oam_classifier_entries_insert_active_non_accelerated(SOC_SAND_IN int unit,
                                                                    SOC_SAND_IN int is_server,
                                                                    SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                    SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
                                                                    SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data_acc);


uint32 arad_pp_oam_classifier_rfc6374_entries_insert_active_non_accelerated(SOC_SAND_IN int                                unit,
                                                                            SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                            SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data);

uint32 arad_pp_oam_classifier_entries_insert_accelerated(SOC_SAND_IN int unit,
                                                         SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                         SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data);


uint32 arad_pp_oam_classifier_rfc6374_entries_insert_accelerated(SOC_SAND_IN int                                unit,
                                                                 SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                 SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data);


uint32 arad_pp_oam_classifier_oam1_entries_insert_bfd_according_to_profile( SOC_SAND_IN int                           unit,
                                                                            SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                            SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data);


uint32 arad_pp_oam_classifier_oam1_entries_insert_bfd_acccelerated_to_profile( SOC_SAND_IN int                           unit,
                                                                               SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                               SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data);



uint32 arad_pp_oam_classifier_oam1_entries_insert_oam_non_acc_according_to_profile( SOC_SAND_IN  int                               unit,
                                                                                    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY  *classifier_mep_entry,
                                                                                    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA      *profile_data,
                                                                                    SOC_SAND_IN  uint8 is_passive,
                                                                                    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t       *oam_buffer);


uint32 arad_pp_oam_classifier_oam1_rfc6374_entries_insert_oam_non_acc_according_to_profile( SOC_SAND_IN  int                               unit,
                                                                                            SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY  *classifier_mep_entry,
                                                                                            SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA      *profile_data,
                                                                                            SOC_SAND_OUT _oam_oam_a_b_table_buffer_t       *oam_buffer);


uint32 arad_pp_oam_classifier_oam2_entries_insert_accelerated_according_to_profile( SOC_SAND_IN int                           unit,
                                                                                    SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                                    SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
                                                                                    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t       *oam_buffer);


uint32 arad_pp_oam_classifier_oam2_rfc6374_entries_insert_accelerated_according_to_profile( SOC_SAND_IN int                                unit,
                                                                                            SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                                            SOC_SAND_IN SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
                                                                                            SOC_SAND_OUT _oam_oam_a_b_table_buffer_t       *oam_buffer);


uint32 arad_pp_oam_classifier_oam1_2_entries_insert_injected_according_to_profile( SOC_SAND_IN    int                                unit,
                                                                                   SOC_SAND_IN    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
                                                                                   SOC_SAND_IN    SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
                                                                                   SOC_SAND_INOUT  _oam_oam_a_b_table_buffer_t       *oama_buffer,
                                                                                   SOC_SAND_INOUT  _oam_oam_a_b_table_buffer_t       *oamb_buffer);
uint32 arad_pp_oam_classifier_oam2_entry_set_on_buffer( SOC_SAND_IN  int                                 unit,
                                                        SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM2_ENTRY_KEY    *oam2_key,
                                                        SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD *oam2_payload,
                                                        SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t            *oamb_buffer);


soc_error_t arad_pp_oam_classifier1_mip_passive_entries(int unit, const  SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data) ;



void arad_pp_oam_classifier_internal_opcode_init( SOC_SAND_IN  int                          unit,
                                                  SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data);

uint32
  arad_pp_oam_classifier_oem_mep_profile_replace_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  );

uint32
  arad_pp_oam_classifier_oem_mep_profile_replace_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  );


soc_error_t
soc_arad_pp_oam_classifier_oem_mep_mip_conflict_check(SOC_SAND_IN  int       unit,
                                                      SOC_SAND_IN  uint32    oam_lif,
                                                      SOC_SAND_IN  uint8     is_mip);
uint32
  arad_pp_oam_classifier_oem_mep_add_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry,
    SOC_SAND_IN  uint8                    update,
    SOC_SAND_IN  uint8                    is_arm
  );

uint32
  arad_pp_oam_classifier_oem_mep_add_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry
  );

uint32
  arad_pp_oam_classifier_mep_delete_unsafe(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
	  SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );

uint32
  arad_pp_oam_classifier_mep_delete_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
	  SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
    SOC_SAND_IN  uint8                    allocate_icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_MA_NAME      name,
    SOC_SAND_IN  int                      is_new_ep
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_set_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
	  SOC_SAND_IN  uint8                    allocate_icc_ndx,
	  SOC_SAND_IN  SOC_PPC_OAM_MA_NAME      name
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_delete_unsafe(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
      SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry,
	  SOC_SAND_IN  uint8                    deallocate_icc_ndx,
	  SOC_SAND_IN  uint8                    is_last_mep
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_delete_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
	  SOC_SAND_IN  uint8                    deallocate_icc_ndx,
	  SOC_SAND_IN  uint8                    is_last_mep
  );

uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   lif,
    SOC_SAND_IN  uint8                    md_level,
    SOC_SAND_IN  uint8                    is_upmep
  );

uint32
  arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   lif,
	SOC_SAND_IN  uint8                    md_level,
	SOC_SAND_IN  uint8                    is_upmep,
	SOC_SAND_OUT uint8                    *found_mep,
	SOC_SAND_OUT uint32                   *profile,
	SOC_SAND_OUT uint8                    *found_profile,
	SOC_SAND_OUT uint8                    *is_mp_type_flexible,
    SOC_SAND_OUT uint8                    *is_mip
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_get_unsafe(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_mep_db_entry_get_verify(
    SOC_SAND_IN  int                   unit,
	  SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_rmep_set_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
  SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry,
	SOC_SAND_IN  uint8                    update
  );

uint32
  arad_pp_oam_oamp_rmep_set_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  );

uint32
  arad_pp_oam_oamp_rmep_get_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  );

uint32
  arad_pp_oam_oamp_rmep_get_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  );

uint32
  arad_pp_oam_oamp_rmep_delete_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type
  );

uint32
  arad_pp_oam_oamp_rmep_delete_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type
  );

uint32
  arad_pp_oam_oamp_rmep_index_get_verify(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type
  );

uint32
  arad_pp_oam_oamp_rmep_index_get_unsafe(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
	SOC_SAND_OUT uint32                   *rmep_index,
	SOC_SAND_OUT  uint8                   *is_found
  );

uint32
  arad_pp_oam_classifier_oem1_entry_set_unsafe(
    SOC_SAND_IN   int                                     unit,
	  SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	  SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload
  );

uint32
  arad_pp_oam_classifier_oem1_entry_get_unsafe(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  );

uint32
  arad_pp_oam_classifier_oem1_entry_delete_unsafe(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key
  );

uint32
  arad_pp_oam_classifier_oem2_entry_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   *oem2_payload
  );

uint32
  arad_pp_oam_classifier_oem1_init(
    SOC_SAND_IN  int        unit
  );

uint32
  arad_pp_oam_classifier_oem2_entry_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  );

uint32
  arad_pp_oam_classifier_oem2_entry_delete_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key
  );

uint32
  arad_pp_oam_counter_range_set_unsafe(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   uint32                                     counter_range_min,
	SOC_SAND_IN   uint32                                     counter_range_max
  );

uint32
  arad_pp_oam_counter_range_set_verify(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   uint32                                     counter_range_min,
	SOC_SAND_IN   uint32                                     counter_range_max
  );

uint32
  arad_pp_oam_counter_range_get_unsafe(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_OUT  uint32                                     *counter_range_min,
	SOC_SAND_OUT  uint32                                     *counter_range_max
  );

uint32
  arad_pp_oam_counter_range_get_verify(
    SOC_SAND_IN   int                                     unit
  );

uint32
  arad_pp_oam_event_fifo_read_unsafe(
    SOC_SAND_IN   int                                           unit,
    SOC_SAND_IN   int                                           event_type,
	SOC_SAND_OUT  uint32                                       *rmeb_db_ndx,
	SOC_SAND_OUT  uint32                                       *event_id,
	SOC_SAND_OUT  uint32                                       *valid,
    SOC_SAND_OUT  uint32                                       *event_data,
    SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA               *interrupt_data
  );


uint32
  arad_pp_oam_event_fifo_read_verify(
    SOC_SAND_IN  int                                        unit
  );

uint32
  arad_pp_oam_pp_pct_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  );

uint32
  arad_pp_oam_pp_pct_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  );

uint32
  arad_pp_oam_pp_pct_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx
  );

uint32
  arad_pp_oam_pp_pct_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *oam_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       diag_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       diag_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                      *diag_profile
  );

uint32
  arad_pp_oam_bfd_diag_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx
  );



uint32
  arad_pp_oam_bfd_flags_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       flags_profile
  );

uint32
  arad_pp_oam_bfd_flags_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       flags_profile
  );

uint32
  arad_pp_oam_bfd_flags_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                      *flags_profile
  );

uint32
  arad_pp_oam_bfd_flags_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx
  );


uint32
  arad_pp_oam_eth_oam_opcode_map_set_unsafe(
    SOC_SAND_IN   int                                     unit
  );




uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_IN  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_IN  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_OUT SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

uint32
  arad_pp_oam_bfd_ipv4_tos_ttl_select_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index
  );


uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_IN  uint32                                       src_addr
  );

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_IN  uint32                                       src_addr
  );

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_OUT uint32                                       *src_addr
  );

uint32
  arad_pp_oam_bfd_ipv4_src_addr_select_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index
  );


uint32
  arad_pp_oam_bfd_tx_rate_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_IN  uint32                                       tx_rate
  );

uint32
  arad_pp_oam_bfd_tx_rate_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_IN  uint32                                       tx_rate
  );

uint32
  arad_pp_oam_bfd_tx_rate_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_OUT uint32                                       *tx_rate
  );

uint32
  arad_pp_oam_bfd_tx_rate_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index
  );


uint32
  arad_pp_oam_bfd_req_interval_pointer_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_IN  uint32                                       req_interval
  );

uint32
  arad_pp_oam_bfd_req_interval_pointer_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_IN  uint32                                       req_interval
  );

uint32
  arad_pp_oam_bfd_req_interval_pointer_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_OUT uint32                                       *req_interval
  );

uint32
  arad_pp_oam_bfd_req_interval_pointer_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer
  );


uint32
  arad_pp_oam_mpls_pwe_profile_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_IN  SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  );

uint32
  arad_pp_oam_mpls_pwe_profile_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_IN  SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  );

uint32
  arad_pp_oam_mpls_pwe_profile_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_OUT SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  );

uint32
  arad_pp_oam_mpls_pwe_profile_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile
  );


uint32
  arad_pp_oam_bfd_mpls_udp_sport_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  arad_pp_oam_bfd_mpls_udp_sport_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT uint16                                       *udp_sport
  );

uint32
  arad_pp_oam_bfd_mpls_udp_sport_get_verify(
    SOC_SAND_IN  int                                       unit
  );


uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );


uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint16                                      *udp_sport
  );

uint32
  arad_pp_oam_bfd_ipv4_udp_sport_get_verify(
    SOC_SAND_IN  int                                       unit

  );


uint32
  arad_pp_oam_bfd_pdu_static_register_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

uint32
  arad_pp_oam_bfd_pdu_static_register_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

uint32
  arad_pp_oam_bfd_pdu_static_register_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

uint32
  arad_pp_oam_bfd_pdu_static_register_get_verify(
    SOC_SAND_IN  int                                       unit
  );


uint32
  arad_pp_oam_bfd_cc_packet_static_register_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );

uint32
  arad_pp_oam_bfd_cc_packet_static_register_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );

uint32
  arad_pp_oam_bfd_cc_packet_static_register_get_verify(
    SOC_SAND_IN  int                                       unit
  );


uint32
  arad_pp_oam_bfd_discriminator_range_registers_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32  						              range
  );

uint32
  arad_pp_oam_bfd_discriminator_range_registers_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32  						              range
  );

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint32  						              *range
  );

uint32
  arad_pp_oam_bfd_discriminator_range_registers_get_verify(
    SOC_SAND_IN  int                                       unit
  );


soc_error_t arad_pp_oam_bfd_discriminator_rx_range_get(
   int unit,
   int * range_min,
   int * range_max);
soc_error_t arad_pp_oam_bfd_discriminator_rx_range_set(
   int unit,
   int  range_min,
   int  range_max);


uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS			*dip							
  );

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS			*dip		
  );


uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS			*dip								
  );

uint32
  arad_pp_oam_bfd_my_bfd_dip_ip_get_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16							              dip_index
  );


uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  );

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  );

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES               *tx_ipv4_multi_hop_att
  );

uint32
  arad_pp_oam_bfd_tx_ipv4_multi_hop_get_verify(
    SOC_SAND_IN  int                                       unit
  );



uint32
  arad_pp_oam_oamp_tx_priority_registers_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	  SOC_SAND_IN  uint32                     	                priority,
	  SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  );

uint32
  arad_pp_oam_oamp_tx_priority_registers_set_verify(
    SOC_SAND_IN  int                                       unit,
	  SOC_SAND_IN  uint32                       	              priority,
	  SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  );

uint32
  arad_pp_oam_oamp_tx_priority_registers_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	  SOC_SAND_IN  uint32                       	              priority,
	  SOC_SAND_OUT  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES             *tx_oam_att
  );

uint32
  arad_pp_oam_oamp_tx_priority_registers_get_verify(
    SOC_SAND_IN  int                                       unit,
	  SOC_SAND_IN  uint32                     	                priority
  );


uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set_verify(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  );

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  );

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get_verify(
    SOC_SAND_IN  int                                       unit
  );

uint32
  arad_pp_oam_oamp_enable_interrupt_message_event_get_unsafe(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint8                                        *interrupt_message_event_bmp
  );



uint32
  arad_pp_oam_mep_passive_active_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  );

uint32
  arad_pp_oam_mep_passive_active_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  );

uint32
  arad_pp_oam_mep_passive_active_enable_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx
  );

uint32
  arad_pp_oam_mep_passive_active_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  uint8                                 *enable
  );


uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  arad_pp_oam_oamp_punt_event_hendling_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx
  );


uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  SOC_TMC_DEST_INFO                         dest_info
  );

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  SOC_TMC_DEST_INFO                        dest_info
  );

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_OUT  uint32                                *trap_id,
    SOC_SAND_OUT SOC_TMC_DEST_INFO                        *dest_info
  );

uint32
  arad_pp_oam_oamp_error_trap_id_and_destination_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type
  );




soc_error_t
  arad_pp_oam_oamp_lm_get(
    int                                 unit,
     SOC_PPC_OAM_OAMP_LM_INFO_GET     *lm_info
  );

soc_error_t
  arad_pp_oam_oamp_dm_get(
    int                                 unit,
     SOC_PPC_OAM_OAMP_DM_INFO_GET     *dm_info,
    uint8                                      * is_1dm
  );
uint32
  arad_pp_oam_oamp_lm_dm_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_lm_dm_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_slm_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_slm_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  );

uint32
  arad_pp_oam_oamp_lm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_LM_INFO_GET     *lm_info
  );

uint32
  arad_pp_oam_oamp_lm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_LM_INFO_GET     *lm_info
  );

uint32
  arad_pp_oam_oamp_dm_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  );

uint32
  arad_pp_oam_oamp_dm_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_DM_INFO_GET     *dm_info,
    SOC_SAND_OUT uint8                                      * is_1dm
  );


uint32
  arad_pp_oam_oamp_next_index_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  );
uint32
  arad_pp_oam_oamp_next_index_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  );



uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  );

uint32
  arad_pp_oam_oamp_eth1731_and_oui_profiles_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  );


uint32
    arad_pp_oam_oamp_nic_profile_get_verify(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       );

uint32
    arad_pp_oam_oamp_nic_profile_get_unsafe(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       );



soc_error_t
    arad_pp_oam_oamp_search_for_lm_dm_unsafe(
       int                                 unit,
       uint32                                 endpoint_id,
       uint32                               * found_bitmap
       );

soc_error_t
    arad_pp_oam_oamp_search_for_lm_dm_verify(
       int                                 unit,
       uint32                                 endpoint_id,
       uint32                               * found_bitmap
       );

soc_error_t
    arad_pp_oam_oamp_search_for_lm_dm(
       int                                 unit,
       uint32                                 endpoint_id,
       uint32                               * found_bitmap
       );




uint32
  arad_pp_oam_oamp_create_new_eth1731_profile_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );

uint32
  arad_pp_oam_oamp_create_new_eth1731_profile_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );


uint32
  arad_pp_oam_oamp_set_oui_nic_registers_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  );

uint32
  arad_pp_oam_oamp_set_oui_nic_registers_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  );




uint32
  arad_pp_oam_oamp_lm_dm_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN uint8                           exists_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  );

uint32
  arad_pp_oam_oamp_lm_dm_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry,
    SOC_SAND_IN uint8                           exists_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  );



soc_error_t arad_pp_oam_oamp_1dm_set(
   int unit,
   int endpoint_id,
   uint8 use_1dm);

soc_error_t arad_pp_oam_oamp_1dm_get(
   int unit,
   int endpoint_id,
   uint8 *use_1dm);



soc_error_t arad_pp_oamp_pe_use_1dm_check(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                              endpoint_id
);
  

uint32 
    arad_pp_oam_oamp_loopback_set(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                              *loopback_info
       );

uint32 
    arad_pp_oam_oamp_loopback_set_unsafe(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                              *loopback_info
       );

uint32 
    arad_pp_oam_oamp_loopback_set_verify(
       SOC_SAND_IN  int                                                    unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                              *loopback_info
       );


uint32 
    arad_pp_oam_oamp_loopback_get_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                         *loopback_info
       );

uint32 
    arad_pp_oam_oamp_loopback_get_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                         *loopback_info
       );

uint32 
    arad_pp_oam_oamp_loopback_get(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                         *loopback_info
       );




uint32
    arad_pp_oam_oamp_loopback_get_config_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT ARAD_PP_OAM_LOOPBACK_INFO                         *loopback_info
       );


uint32 
    arad_pp_oam_oamp_loopback_remove_verify(
       SOC_SAND_IN  int                                                  unit
       );

uint32 
    arad_pp_oam_oamp_loopback_remove_unsafe(
       SOC_SAND_IN  int                                                  unit
       );



uint32 
    arad_pp_oam_dma_reset_verify(
       SOC_SAND_IN  int                                                  unit
       );

uint32 
    arad_pp_oam_dma_reset_unsafe(
       SOC_SAND_IN  int                                                  unit
       );



uint32 
    arad_pp_oam_dma_clear_verify(
       SOC_SAND_IN  int                                                  unit
       );

uint32 
    arad_pp_oam_dma_clear_unsafe(
       SOC_SAND_IN  int                                                  unit
       );

uint32 soc_arad_pp_oam_dma_clear(int unit);


uint32 
    arad_pp_oam_register_dma_event_handler_callback_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        dma_event_handler_cb_t          event_handler_cb
       );

uint32 
    arad_pp_oam_register_dma_event_handler_callback_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        dma_event_handler_cb_t          event_handler_cb
       );


soc_error_t 
    arad_pp_oam_oamp_rx_trap_codes_set(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
       );

soc_error_t 
    arad_pp_oam_oamp_rx_trap_codes_set_verify(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
       );

soc_error_t 
    arad_pp_oam_oamp_rx_trap_codes_delete(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
       );

soc_error_t 
    arad_pp_oam_oamp_rx_trap_codes_delete_verify(
                 int                                 unit,
                 SOC_PPC_OAM_MEP_TYPE                mep_type,
                 uint32                              trap_code
       );


uint32 
   arad_pp_oam_dma_event_handler_verify(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_IN   int                                           event_type
       );

soc_error_t 
   arad_pp_oam_dma_event_handler_unsafe(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_IN   int                                           event_type,
       SOC_SAND_IN   int                                           cmc,
       SOC_SAND_IN   int                                           ch       
       );



uint32
  arad_pp_oam_classifier_counter_disable_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  );

uint32
  arad_pp_oam_classifier_counter_disable_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  );

uint32
  arad_pp_oam_classifier_counter_disable_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile
  );

uint32
  arad_pp_oam_classifier_counter_disable_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_OUT  uint8                                 *counter_enable
  );

soc_error_t _arad_pp_oam_set_counter_disable(int unit,
                                             int  internal_opcode,
                                             SOC_PPC_OAM_MEP_TYPE mep_type,
                                             SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD * oam_payload,
                                             const SOC_PPC_OAM_MEP_PROFILE_DATA *profile_data,
                                             int is_piggy_backed);


uint32
  arad_pp_oam_diag_print_lookup_verify(
     SOC_SAND_IN int unit
   );

uint32
  arad_pp_oam_diag_print_lookup_unsafe(
     SOC_SAND_IN int unit
   );


uint32
  arad_pp_get_crps_counter_verify(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN uint8 crps_counter_number, 
      SOC_SAND_IN uint32 reg_number, 
      SOC_SAND_OUT uint32* value
      );

uint32
  arad_pp_get_crps_counter_unsafe(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN uint8 crps_counter_number, 
      SOC_SAND_IN uint32 reg_number, 
      SOC_SAND_OUT uint32* value
      );


uint32
  arad_pp_oam_diag_print_rx_verify(
     SOC_SAND_IN int unit
   );

uint32
  arad_pp_oam_diag_print_rx_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id

   );



uint32
  arad_pp_oam_diag_print_em_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int LIF
   );

uint32
  arad_pp_oam_diag_print_em_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int LIF
   );



uint32
  arad_pp_oam_diag_print_ak_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_ACTION_KEY_PARAMS *key_params
   );

uint32
  arad_pp_oam_diag_print_ak_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_ACTION_KEY_PARAMS *provided_key_params
   );


uint32
  arad_pp_oam_diag_print_ks_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_KEY_SELECT_PARAMS *key_params
   );

uint32
  arad_pp_oam_diag_print_ks_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_KEY_SELECT_PARAMS *provided_key_params
   );


uint32
  arad_pp_oam_diag_print_oamp_counter_verify(
     SOC_SAND_IN int unit
   );

uint32
  arad_pp_oam_diag_print_oamp_counter_unsafe(
     SOC_SAND_IN int unit
   );


uint32
  arad_pp_oam_diag_print_debug_verify(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int mode
   );

uint32
  arad_pp_oam_diag_print_debug_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int cfg,
     SOC_SAND_IN int mode
   );


uint32
  arad_pp_oam_diag_print_oam_id_verify(
     SOC_SAND_IN int unit
   );

uint32
  arad_pp_oam_diag_print_oam_id_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id
   );

 
uint32
  arad_pp_oam_diag_print_tcam_entries_verify(
     SOC_SAND_IN int unit
   );

uint32
  arad_pp_oam_diag_print_tcam_entries_unsafe(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id
   );


uint32
  arad_pp_oam_diag_print_hw_tcam_entries_verify(
     SOC_SAND_IN int unit
   );

uint32
  arad_pp_oam_diag_print_hw_tcam_entries_unsafe(
     SOC_SAND_IN int unit
   );


soc_error_t 
    arad_pp_oam_my_cfm_mac_set(int unit, int core, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index);


soc_error_t
  arad_pp_oam_my_cfm_mac_get(int unit, int core, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index);




soc_error_t
  arad_pp_oam_my_cfm_mac_delete(int unit, int core, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index);



soc_error_t
  arad_pp_oam_inlif_profile_map_set(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_IN  uint32                                               oam_profile
  );


soc_error_t
  arad_pp_oam_inlif_profile_map_get(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_OUT uint32                                               *oam_profile
  );


soc_error_t
  arad_pp_oam_classifier_default_profile_add(
     SOC_SAND_IN  int                                unit,
     SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
     SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
     SOC_SAND_IN  uint8                              update_action_only
  );


soc_error_t
  arad_pp_oam_classifier_default_profile_remove(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index
  );


soc_error_t
  arad_pp_oam_classifier_default_profile_action_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );


uint32
  arad_pp_oam_ecn_init(
    SOC_SAND_IN  int                                unit
  );


soc_error_t arad_pp_oam_tod_set(
    int                                 unit,
   uint8                                is_ntp,
   uint64                               data
   );

soc_error_t arad_pp_oam_tod_get(
   int                                 unit,
   uint8                               is_ntp,
   uint64                              *data
   );


soc_error_t arad_pp_oam_tod_leap_control_set(
    int                                 unit,
    bcm_oam_timestamp_format_t          ts_format,
    bcm_oam_tod_leap_control_t          leap_info
   );

soc_error_t arad_pp_oam_tod_leap_control_get(
    int                                 unit,
    bcm_oam_timestamp_format_t          ts_format,
    bcm_oam_tod_leap_control_t          *leap_info
   );







uint32
  arad_pp_oam_classifier_oam1_allocate_sw_buffer(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t        *oama_buffer
  );




uint32
  arad_pp_oam_classifier_oam1_entry_set_on_buffer(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM1_ENTRY_KEY      *oam1_key,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OAM_ENTRY_PAYLOAD  *oam_payload,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t                           *oama_buffer
  );



void arad_pp_oam_classifier_oam1_2_buffer_free(
   SOC_SAND_IN int                                           unit, 
   SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t * buffer
   );


uint32
  arad_pp_oam_oamp_mep_db_entry_get_internal_unsafe(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_IN   uint32                   mep_index,
    SOC_SAND_OUT  uint32                   *short_name,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  );


uint32
  arad_pp_oam_classifier_oam1_set_hw_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT _oam_oam_a_b_table_buffer_t          *oama_buffer
  );


soc_error_t
  arad_pp_oamp_report_mode_set(int unit, SOC_PPC_OAM_REPORT_MODE mode);


soc_error_t
  arad_pp_oamp_report_mode_get(int unit, SOC_PPC_OAM_REPORT_MODE *mode);

soc_error_t
  soc_arad_pp_oamp_control_punt_packet_int_pri_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *DP_and_TC
  );

soc_error_t
  soc_arad_pp_oamp_control_ccm_weight_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *ccm_weight
  );

soc_error_t
  soc_arad_pp_oamp_control_sat_weight_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *sat_weight
  );

soc_error_t
  soc_arad_pp_oamp_control_response_weight_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *response_weight
  );



int
  arad_pp_oam_bfd_acc_endpoint_tx_disable(int unit, uint32 mep_index, const SOC_PPC_OAM_OAMP_MEP_DB_ENTRY* mep_db_entry);



soc_error_t
    arad_pp_oam_slm_set(int unit, int is_slm);



soc_error_t arad_pp_oam_counter_increment_bitmap_set(int unit, int enable_counting_subtype_1);


uint32
  arad_pp_oam_oamp_protection_packet_header_raw_set(
      SOC_SAND_IN  int                                    unit,
	  SOC_SAND_IN  SOC_PPC_OAM_OAMP_PROTECTION_HEADER     *packet_protection_header
  );
  

uint32
  arad_pp_oam_oamp_protection_packet_header_raw_get(
      int                                    unit,
	  SOC_PPC_OAM_OAMP_PROTECTION_HEADER     *packet_protection_header
  );


uint32 
  arad_pp_oam_oamp_read_pph_header(
     SOC_SAND_IN  int  unit,
     SOC_SAND_IN  uint8  field1,
     SOC_SAND_IN  uint8  field2,
     SOC_SAND_IN  uint8  field3,
     PPH_base *pph_header 
     );

int soc_arad_oamp_sat_arb_weight_set(int unit,uint32 sat_arbiter_weight);

int arad_pp_oam_channel_type_rx_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 channel_type_ndx,
    SOC_SAND_IN  int                                 num_values,
    SOC_SAND_IN  int                                 *list_of_values
    );

int arad_pp_oam_channel_type_rx_get(
    SOC_SAND_IN    int                                 unit,
    SOC_SAND_IN    int                                 channel_type_ndx,
    SOC_SAND_IN    int                                 num_values,
    SOC_SAND_INOUT int                                 *list_of_values,
    SOC_SAND_OUT   int                                 *value_count
    );

int arad_pp_oam_channel_type_rx_delete(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 channel_type_ndx,
    SOC_SAND_IN  int                                 num_values,
    SOC_SAND_IN  int                                 *list_of_values
    );

int arad_pp_oam_channel_type_tx_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 channel_type_ndx,
    SOC_SAND_IN  int                                 value
    );

int arad_pp_oam_channel_type_tx_get(
    SOC_SAND_IN    int                                 unit,
    SOC_SAND_IN    int                                 channel_type_ndx,
    SOC_SAND_OUT   int                                 *value
    );

int arad_pp_oam_channel_type_tx_delete(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 channel_type_ndx,
    SOC_SAND_IN  int                                 value
    );

uint32 arad_pp_oam_tcam_y1711_lm_entry_add_unsafe(
    SOC_SAND_IN  int    unit
  );


uint32
arad_pp_oam_tcam_bfd_sbfd_entry_add_unsafe(
        SOC_SAND_IN  int    unit,
        SOC_SAND_IN  uint8	  flags,
        SOC_SAND_IN uint32 tcam_entry_id,
        SOC_SAND_IN SOC_SAND_PP_IPV4_ADDRESS *ipv4_dip
        );

uint32
arad_pp_oam_tcam_bfd_sbfd_entry_delete_unsafe(
        SOC_SAND_IN  int    unit,
        SOC_SAND_IN  uint8	  flags,
        SOC_SAND_IN uint32 tcam_entry_id
        );

    uint32
arad_pp_seamless_bfd_rx_dst_port_set(
        SOC_SAND_IN  int                                       unit,
        SOC_SAND_IN  uint32                                     flags,
        SOC_SAND_IN  uint32                                     udp_port
        );


    uint32
arad_pp_seamless_bfd_rx_dst_port_get(
        SOC_SAND_IN  int                                       unit,
        SOC_SAND_OUT  uint32                                    *udp_port
        );


char *arad_pp_oam_diag_mp_type_to_mp_type_str_get(uint8 mp_type);
char *arad_plus_pp_oam_diag_mp_type_to_mp_type_str_get(uint8 mp_type);
char *jericho_pp_oam_diag_mp_type_to_mp_type_str_get(uint8 mp_type);
char *qax_jer_plus_pp_oam_diag_mp_type_to_mp_type_str_get(uint8 mp_type);


soc_error_t
soc_arad_pp_oam_classifier_find_ep_id_by_lif_dir_and_mdl(int unit,
                                                         uint32 lif,
                                                         uint8  mdl,
                                                         uint8  is_upmep,
                                                         uint8  *is_mip,
                                                         uint8  *found,
                                                         int    *oam_id);

uint32
  arad_pp_oam_dma_set_unsafe(
     SOC_SAND_IN  int                                 unit
     );

uint32
  arad_pp_oam_report_dma_set_unsafe(
	 SOC_SAND_IN  int                                 unit
	 );


uint32 soc_arad_pp_oam_dma_set(int unit);

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
