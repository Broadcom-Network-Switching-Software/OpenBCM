
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_OAM_INCLUDED__

#define __SOC_PPD_API_OAM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_oam.h>

#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#include <soc/dpp/drv.h>









#define SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MULTIPLE_MEPS_ON_LIF_MODE(unit)   (SOC_DPP_CONFIG(unit)->pp.oam_classifier_advanced_mode==2)
#define SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit)                        ((SOC_DPP_CONFIG(unit)->pp.oam_classifier_advanced_mode==1) || (SOC_DPP_CONFIG(unit)->pp.oam_classifier_advanced_mode==2))
#define SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit)                            (!SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) && SOC_IS_ARADPLUS_AND_BELOW(unit))
#define SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)                         (SOC_IS_JERICHO(unit))
#define SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_PLUS_MODE(unit)                    (SOC_IS_JERICHO_PLUS(unit))

#define SOC_PPD_OAM_IS_Y1711_LM(unit,endpoint_info)  ((SOC_IS_QAX(unit))&&(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_1711_enable",0) == 1)&&(endpoint_info->type == bcmOAMEndpointTypeMPLSNetwork))

#define SOC_PPD_OAM_CNT_LSP_OUT_PKT(unit)  ((SOC_IS_QAX(unit))&&(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_injected_over_lsp_cnt",0) == 1))

#define SOC_PPD_OAM_SERVER_CLIENT_SIDE_BIT 24

#define SOC_PPD_OAM_MEP_INDEX_IS_SERVER_CLIENT_SIDE(endpoint) \
	(((endpoint >> SOC_PPD_OAM_SERVER_CLIENT_SIDE_BIT) & 1) == 1)





typedef enum
{
  
  

  SOC_PPD_OAM_ETH_ING_AC_KEY_MP_INFO_SET = SOC_PPD_PROC_DESC_BASE_OAM_FIRST,
  SOC_PPD_OAM_ETH_ING_AC_KEY_MP_INFO_GET,
  SOC_PPD_OAM_ETH_EGR_PORT_VSI_MP_INFO_SET,
  SOC_PPD_OAM_ETH_EGR_PORT_VSI_MP_INFO_GET,
  SOC_PPD_OAM_ETH_EGR_PORT_CVID_MP_INFO_SET,
  SOC_PPD_OAM_ETH_EGR_PORT_CVID_MP_INFO_GET,
  SOC_PPD_OAM_ETH_EGR_OUT_AC_MP_INFO_SET,
  SOC_PPD_OAM_ETH_EGR_OUT_AC_MP_INFO_GET,
  SOC_PPD_OAM_ETH_TRAP_CODE_SET,
  SOC_PPD_OAM_ETH_TRAP_CODE_GET,
  
  SOC_PPD_OAM_INIT,
  SOC_PPD_OAM_DEINIT,
  SOC_PPD_OAM_ICC_MAP_REGISTER_SET,
  SOC_PPD_OAM_CLASSIFIER_MEP_SET,
  SOC_PPD_OAM_CLASSIFIER_MEP_DELETE,
  SOC_PPD_OAMP_RMEP_SET,
  SOC_PPD_OAMP_RMEP_GET,
  SOC_PPD_OAMP_RMEP_DELETE,
  SOC_PPD_OAMP_RMEP_INDEX_GET,
  SOC_PPD_OAMP_MEP_DB_ENTRY_SET,
  SOC_PPD_OAMP_MEP_DB_ENTRY_GET,
  SOC_PPD_OAMP_MEP_DB_ENTRY_DELETE,
  SOC_PPD_OAM_CLASSIFIER_OEM_MEP_ADD,
  SOC_PPD_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL,
  SOC_PPD_CLASSIFIER_OEM1_ENTRY_SET,
  SOC_PPD_CLASSIFIER_OEM1_ENTRY_GET,
  SOC_PPD_CLASSIFIER_OEM1_ENTRY_DELETE,
  SOC_PPD_CLASSIFIER_OEM2_ENTRY_SET,
  SOC_PPD_CLASSIFIER_OEM2_ENTRY_GET,
  SOC_PPD_CLASSIFIER_OEM2_ENTRY_DELETE,
  SOC_PPD_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE,
  SOC_PPD_OAM_CLASSIFIER_OAM1_2_ENTRIES_INSERT_ACCORDING_TO_PROFILE,
  SOC_PPD_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE,
  SOC_PPD_OAM_COUNTER_RANGE_SET,
  SOC_PPD_OAM_COUNTER_RANGE_GET,
  SOC_PPD_OAM_BFD_IPV4_TOS_TTL_SELECT_SET,
  SOC_PPD_OAM_BFD_IPV4_TOS_TTL_SELECT_GET,
  SOC_PPD_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET,
  SOC_PPD_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET,
  SOC_PPD_OAM_BFD_TX_RATE_SET,
  SOC_PPD_OAM_BFD_TX_RATE_GET,
  SOC_PPD_OAM_BFD_REQ_INTERVAL_POINTER_SET,
  SOC_PPD_OAM_BFD_REQ_INTERVAL_POINTER_GET,
  SOC_PPD_OAM_MPLS_PWE_PROFILE_SET,
  SOC_PPD_OAM_MPLS_PWE_PROFILE_GET,
  SOC_PPD_OAM_BFD_MPLS_UDP_SPORT_SET,
  SOC_PPD_OAM_BFD_MPLS_UDP_SPORT_GET,
  SOC_PPD_OAM_BFD_IPV4_UDP_SPORT_SET,
  SOC_PPD_OAM_BFD_IPV4_UDP_SPORT_GET,
  SOC_PPD_OAM_BFD_PDU_STATIC_REGISTER_SET,
  SOC_PPD_OAM_BFD_PDU_STATIC_REGISTER_GET,
  SOC_PPD_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET,
  SOC_PPD_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET,
  SOC_PPD_OAM_BFD_MY_BFD_DIP_IPV4_SET,
  SOC_PPD_OAM_BFD_MY_BFD_DIP_IPV4_GET,
  SOC_PPD_OAM_BFD_TX_IPV4_MULTI_HOP_SET,
  SOC_PPD_OAM_BFD_TX_IPV4_MULTI_HOP_GET,
  SOC_PPD_OAM_BFD_TX_MPLS_SET,
  SOC_PPD_OAM_BFD_TX_MPLS_GET,
  SOC_PPD_OAM_BFDCC_TX_MPLS_SET,
  SOC_PPD_OAM_BFDCC_TX_MPLS_GET,
  SOC_PPD_OAM_BFDCV_TX_MPLS_SET,
  SOC_PPD_OAM_BFDCV_TX_MPLS_GET,
  SOC_PPD_OAM_OAMP_TX_PRIORITY_REGISTERS_SET,
  SOC_PPD_OAM_OAMP_TX_PRIORITY_REGISTERS_GET,
  SOC_PPD_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET,
  SOC_PPD_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET,
  SOC_PPD_OAM_EVENT_FIFO_READ,
  SOC_PPD_OAM_PP_PCT_PROFILE_SET,
  SOC_PPD_OAM_PP_PCT_PROFILE_GET,
  
  SOC_PPD_OAM_PROCEDURE_DESC_LAST
} SOC_PPD_OAM_PROCEDURE_DESC;









uint32
  soc_ppd_oam_eth_ing_ac_key_mp_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY           *ac_key,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_INFO         *info
  );


uint32
  soc_ppd_oam_eth_ing_ac_key_mp_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_L2_LIF_AC_KEY           *ac_key,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_OUT SOC_PPC_OAM_ETH_MP_INFO         *info
  );


uint32
  soc_ppd_oam_eth_egr_port_vsi_mp_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY           *port_vsi_key,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_INFO         *info
  );


uint32
  soc_ppd_oam_eth_egr_port_vsi_mp_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY           *port_vsi_key,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_OUT SOC_PPC_OAM_ETH_MP_INFO         *info
  );


uint32
  soc_ppd_oam_eth_egr_port_cvid_mp_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY      *port_cep_key,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_INFO         *info
  );


uint32
  soc_ppd_oam_eth_egr_port_cvid_mp_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY      *port_cep_key,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_OUT SOC_PPC_OAM_ETH_MP_INFO         *info
  );


uint32
  soc_ppd_oam_eth_egr_out_ac_mp_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_INFO         *info
  );


uint32
  soc_ppd_oam_eth_egr_out_ac_mp_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    out_ac_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_MP_LEVEL        level_ndx,
    SOC_SAND_OUT SOC_PPC_OAM_ETH_MP_INFO         *info
  );


uint32
  soc_ppd_oam_eth_trap_code_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE   func_type_ndx,
    SOC_SAND_IN  uint32                   opcode_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE          *action_profile
  );


uint32
  soc_ppd_oam_eth_trap_code_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_OAM_ETH_ACC_FUNC_TYPE   func_type_ndx,
    SOC_SAND_IN  uint32                   opcode_ndx,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE          *action_profile
  );






uint32
  soc_ppd_oam_init(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint8                     is_bfd
  );


uint32
  soc_ppd_oam_deinit(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint8                     is_bfd,
    SOC_SAND_IN  uint8                    tcam_db_destroy
  );


uint32
  soc_ppd_oam_icc_map_register_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                        icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ICC_MAP_DATA     * data
  );


uint32
  soc_ppd_oam_classifier_oam1_entries_insert_default_profile(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                            is_bfd
  );


uint32
  soc_ppd_oam_classifier_oam1_2_entries_insert_according_to_profile(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN uint8                         is_server,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data_acc
  );


uint32
  soc_ppd_oam_classifier_oem_mep_profile_replace(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
	);


uint32
  soc_ppd_oam_classifier_oem_mep_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY  * classifier_mep_entry,
    SOC_SAND_IN  uint8                    update,
    SOC_SAND_IN  uint8                    is_arm
  );


uint32
  soc_ppd_oam_classifier_mep_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );




uint32
  soc_ppd_oam_oamp_rmep_set(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry,
    SOC_SAND_IN  uint8                    update
  );


uint32
  soc_ppd_oam_oamp_rmep_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  );


uint32
  soc_ppd_oam_oamp_rmep_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type
  );


uint32
  soc_ppd_oam_oamp_rmep_index_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
	SOC_SAND_OUT uint32                   *rmep_index,
	SOC_SAND_OUT  uint8                   *is_found
  );


uint32
  soc_ppd_oam_oamp_mep_db_entry_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
    SOC_SAND_IN  uint8                    allocate_icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_MA_NAME      name,
    SOC_SAND_IN  int                      is_new_ep
  );


uint32
  soc_ppd_oam_oamp_mep_db_entry_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  );


uint32
  soc_ppd_oam_oamp_mep_db_entry_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry,
	SOC_SAND_IN  uint8                    deallocate_icc_ndx,
	SOC_SAND_IN  uint8                    is_last_mep
  );


uint32
  soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(
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
  soc_ppd_oam_classifier_oem1_entry_set(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload
  );

uint32
  soc_ppd_oam_classifier_oem1_entry_get(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  );


uint32
  soc_ppd_oam_classifier_oem1_entry_delete(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key
  );

uint32
  soc_ppd_oam_classifier_oem2_entry_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   *oem2_payload
  );

uint32
  soc_ppd_oam_classifier_oem2_entry_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  );


uint32
  soc_ppd_oam_classifier_oem2_entry_delete(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY      *oem2_key
  );


uint32
  soc_ppd_oam_counter_range_set(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   uint32                                     counter_range_min,
	SOC_SAND_IN   uint32                                     counter_range_max
  );

uint32
  soc_ppd_oam_counter_range_get(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_OUT  uint32                                     *counter_range_min,
	SOC_SAND_OUT  uint32                                     *counter_range_max
  );


uint32
  soc_ppd_oam_eth_oam_opcode_map_set(
    SOC_SAND_IN   int                                     unit
  );



uint32
  soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_IN  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );

uint32
  soc_ppd_oam_bfd_ipv4_tos_ttl_select_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_OUT SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  );


uint32
  soc_ppd_oam_bfd_ipv4_src_addr_select_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_IN  uint32                                       src_addr
  );

uint32
  soc_ppd_oam_bfd_ipv4_src_addr_select_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_OUT uint32                                       *src_addr
  );



uint32
  soc_ppd_oam_bfd_tx_rate_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_IN  uint32                                       tx_rate
  );

uint32
  soc_ppd_oam_bfd_tx_rate_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_OUT uint32                                       *tx_rate
  );



uint32
  soc_ppd_oam_bfd_req_interval_pointer_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_IN  uint32                                       req_interval
  );

uint32
  soc_ppd_oam_bfd_req_interval_pointer_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_OUT uint32                                       *req_interval
  );



uint32
  soc_ppd_oam_mpls_pwe_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_IN  SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  );

uint32
  soc_ppd_oam_mpls_pwe_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_OUT SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  );



uint32
  soc_ppd_oam_bfd_mpls_udp_sport_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  soc_ppd_oam_bfd_mpls_udp_sport_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT uint16                                       *udp_sport
  );


uint32
  soc_ppd_oam_bfd_ipv4_udp_sport_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  );

uint32
  soc_ppd_oam_bfd_ipv4_udp_sport_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint16                                      *udp_sport
  );


uint32
  soc_ppd_oam_bfd_pdu_static_register_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );

uint32
  soc_ppd_oam_bfd_pdu_static_register_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  );


uint32
  soc_ppd_oam_bfd_cc_packet_static_register_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );

uint32
  soc_ppd_oam_bfd_cc_packet_static_register_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  );



uint32
  soc_ppd_oam_bfd_discriminator_range_registers_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32  						              range
  );

uint32
  soc_ppd_oam_bfd_discriminator_range_registers_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint32  						              *range
  );



uint32
  soc_ppd_oam_bfd_my_bfd_dip_ip_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                dip_index,
	SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS                                 *dip							
  );

uint32
  soc_ppd_oam_bfd_my_bfd_dip_ip_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_OUT  SOC_SAND_PP_IPV6_ADDRESS                                  *dip							
  );


uint32
  soc_ppd_oam_bfd_tx_ipv4_multi_hop_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  );

uint32
  soc_ppd_oam_bfd_tx_ipv4_multi_hop_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES               *tx_ipv4_multi_hop_att
  );



uint32
  soc_ppd_oam_oamp_tx_priority_registers_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32                     	              priority,
	SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  );

uint32
  soc_ppd_oam_oamp_tx_priority_registers_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32                     	              priority,
	SOC_SAND_OUT  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES             *tx_oam_att
  );


uint32
  soc_ppd_oam_oamp_enable_interrupt_message_event_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  );

uint32
  soc_ppd_oam_oamp_enable_interrupt_message_event_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint8                                       *interrupt_message_event_bmp
  );


uint32
  soc_ppd_oam_event_fifo_read(
    SOC_SAND_IN   int                                           unit,
    SOC_SAND_IN   SOC_PPC_OAM_DMA_EVENT_TYPE                 event_type,
	SOC_SAND_OUT  uint32                                       *rmeb_db_ndx,
	SOC_SAND_OUT  uint32                                       *event_id,
	SOC_SAND_OUT  uint32                                       *valid,
	SOC_SAND_OUT  uint32                                       *event_data,
    SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA               *interrupt_data
    );


uint32
  soc_ppd_oam_pp_pct_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  );

uint32
  soc_ppd_oam_pp_pct_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *oam_profile
  );



uint32
  soc_ppd_oam_bfd_diag_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       diag_profile
  );

uint32
  soc_ppd_oam_bfd_diag_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                       *diag_profile
  );


uint32
  soc_ppd_oam_bfd_flags_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       flags_profile
  );

uint32
  soc_ppd_oam_bfd_flags_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                       *flags_profile
  );


uint32
  soc_ppd_oam_mep_passive_active_enable_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  );

uint32
  soc_ppd_oam_mep_passive_active_enable_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  uint8                                 *enable
  );


uint32
  soc_ppd_oam_oamp_punt_event_hendling_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );

uint32
  soc_ppd_oam_oamp_punt_event_hendling_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA     *punt_profile_data
  );


uint32
  soc_ppd_oam_oamp_error_trap_id_and_destination_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  SOC_TMC_DEST_INFO                         dest_info
  );

uint32
  soc_ppd_oam_oamp_error_trap_id_and_destination_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_OUT  uint32                                *trap_id,
    SOC_SAND_OUT  SOC_TMC_DEST_INFO                        *dest_info
  );


uint32
  soc_ppd_oam_oamp_lm_dm_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  );


uint32
  soc_ppd_oam_oamp_lm_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_LM_INFO_GET     *lm_info
  );


uint32
  soc_ppd_oam_oamp_next_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  );


uint32
  soc_ppd_oam_oamp_eth1731_and_oui_profiles_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  );



uint32
    soc_ppd_oam_oamp_nic_profile_get(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       );





uint32
  soc_ppd_oam_oamp_create_new_eth1731_profile(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );


uint32
  soc_ppd_oam_oamp_set_oui_nic_registers(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  );




uint32
  soc_ppd_oam_oamp_lm_dm_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry,
    SOC_SAND_IN uint8                           exists_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  );


uint32
  soc_ppd_oam_classifier_counter_disable_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  );

uint32
  soc_ppd_oam_classifier_counter_disable_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_OUT  uint8                                 *counter_enable
  );




uint32 
    soc_ppd_oam_oamp_loopback_remove(
       SOC_SAND_IN  int                                                  unit
       );


uint32 
    soc_ppd_oam_dma_reset(
       SOC_SAND_IN  int                                                  unit
       );


uint32 
    soc_ppd_oam_dma_clear(
       SOC_SAND_IN  int                                                  unit
       );



uint32 
    soc_ppd_oam_register_dma_event_handler_callback(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        dma_event_handler_cb_t          event_handler_cb
       );




void 
   soc_ppd_oam_dma_event_handler(
      SOC_SAND_INOUT  void        *     unit_ptr,
      SOC_SAND_INOUT  void        *     event_type_ptr,
      SOC_SAND_INOUT  void        *     cmc_ptr,
      SOC_SAND_INOUT  void        *     ch_ptr,
       SOC_SAND_INOUT  void       *     unused4
       );










uint32
  soc_ppd_oam_get_crps_counter(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                           crps_counter_number,
    SOC_SAND_IN  uint32                                  reg_number,
    SOC_SAND_OUT uint32*                               value
  );




uint32
  soc_ppd_oam_diag_print_lookup(
     SOC_SAND_IN int unit
   );


uint32
  soc_ppd_oam_diag_print_dma_dump_unsafe(
     SOC_SAND_IN int unit
   );


uint32
  soc_ppd_oam_diag_print_rx(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id

   );



uint32
  soc_ppd_oam_diag_print_em(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int LIF
   );



uint32
  soc_ppd_oam_diag_print_ak(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_ACTION_KEY_PARAMS *key_params
   );


uint32
  soc_ppd_oam_diag_print_ks(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_KEY_SELECT_PARAMS *key_params
   );


uint32
  soc_ppd_oam_diag_print_oamp_counter(
     SOC_SAND_IN int unit
   );


uint32
  soc_ppd_oam_diag_print_debug(
     SOC_SAND_IN int unit,
	 SOC_SAND_IN int cfg,
     SOC_SAND_IN int mode
   );


uint32
  soc_ppd_oam_diag_print_oam_id(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id
   );


uint32
  soc_ppd_oam_diag_print_tcam_entries(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id
   );

uint32
  soc_ppd_oam_diag_print_hw_tcam_entries(
     SOC_SAND_IN int unit
   );

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
