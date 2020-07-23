/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr.c
 * Purpose:     Resource allocation manager for SOC_SAND chips.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>

#include <shared/swstate/sw_state_resmgr.h>
#include <shared/shr_template.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <bcm_int/dpp/field.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/rx.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/ipmc.h>
#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/bfd.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/failover.h>
#include <bcm_int/dpp/policer.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_llp_vid_assign.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/dpp_config_defs.h>


#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>
#include <soc/dpp/TMC/tmc_api_action_cmd.h>
#include <soc/dpp/TMC/tmc_api_egr_queuing.h>

#include <soc/dpp/mbcm.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>

/*
 *  Alloc manager is designed to allow some flexibility in how things are
 *  allocated and tracked.  Several options are available for underlying
 *  resource managers, and a number of these options are designed to be used
 *  for specific purposes, optimising certain behaviours for example.
 *
 *  The initial configuration is established here, and exported to the
 *  rest of the DPP code.
 */

/* Egress thresh */
#define _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_LOW_ID (0)
#define _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_TM_PORTS(unit))
#define _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_SIZE   (sizeof(bcm_dpp_cosq_egress_thresh_key_info_t))

/* Mirror profile */
#define _DPP_AM_TEMPLATE_MIRROR_ACTION_MAX_ENTITIES (SOC_PPC_NOF_TRAP_CODES)  
#define _DPP_AM_TEMPLATE_MIRROR_ACTION_SIZE (sizeof(uint32))


/* Egress Interface Unicast thresh  - ARAD only*/
#define _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_LOW_ID (0)
#define _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_COUNT (8)
#define _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_MAX_ENTITIES (SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces))
#define _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_SIZE   (sizeof(bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_t))

/* Egress Interface Multicast thresh - ARAD only*/
#define _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_LOW_ID (0)
#define _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_COUNT (4)
#define _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_MAX_ENTITIES (SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces))
#define _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_SIZE   (sizeof(bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_t))

/* User defined traps */
/*#define _BCM_PETRA_MULTI_SET_UD_TRAP_LEN         (4)*/
#define _BCM_PETRA_MULTI_SET_UD_TRAP_LEN         (6)

#define _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_LOW_ID (0)
#define _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_COUNT (SOC_PPC_TRAP_CODE_USER_DEFINED_LAST - SOC_PPC_TRAP_CODE_USER_DEFINED_0 + 1)
#define _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit))
#define _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_SIZE   (sizeof(uint32)*_BCM_PETRA_MULTI_SET_UD_TRAP_LEN)

/* Snoop command */



#define _DPP_AM_TEMPLATE_SNOOP_CMD_LOW_ID (0)
#define _DPP_AM_TEMPLATE_SNOOP_CMD_COUNT (16)

#define _DPP_AM_TEMPLATE_SNOOP_CMD_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit) + SOC_PPC_NOF_TRAP_CODES)
#define _DPP_AM_TEMPLATE_SNOOP_CMD_SIZE   (sizeof(bcm_dpp_snoop_t)) 

/* COSQ Egress mapping */
#define _DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_LOW_ID (0)
#define _DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_COUNT (8)
#define _DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_TM_PORTS(unit))
#define _DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_SIZE   (sizeof(bcm_dpp_cosq_egress_queue_mapping_info_t))

/* NRDY threshold mapping */
#define _DPP_AM_TEMPLATE_NRDY_THRESHOLD_LOW_ID (0)
#define _DPP_AM_TEMPLATE_NRDY_THRESHOLD_COUNT (12)
#define _DPP_AM_TEMPLATE_NRDY_THRESHOLD_MAX_ENTITIES(unit) (SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES))
#define _DPP_AM_TEMPLATE_NRDY_THRESHOLD_SIZE   (sizeof(uint32))

/* L2 reserved MC */
#define _DPP_AM_TEMPLATE_L2_RSRVD_MC_LOW_ID (0)
#define _DPP_AM_TEMPLATE_L2_RSRVD_MC_COUNT (4)
#define _DPP_AM_TEMPLATE_L2_RSRVD_MC_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit))
#define _DPP_AM_TEMPLATE_L2_RSRVD_MC_SIZE   (sizeof(_bcm_petra_l2_rsrvd_mc_profile_info_t))


/* port TPID class: LLVP */
#define _DPP_AM_TEMPLATE_PORT_TPID_CLASS_LOW_ID (0)
#define _DPP_AM_TEMPLATE_PORT_TPID_CLASS_COUNT (8)
#define _DPP_AM_TEMPLATE_PORT_TPID_CLASS_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit))
#define _DPP_AM_TEMPLATE_PORT_TPID_CLASS_SIZE   (sizeof(_bcm_dpp_port_tpid_class_info_t))

/* port TPID class: egress Acceptable frame type */
#define _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_LOW_ID    (0)   
#define _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_COUNT     (4)   
#define _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit))   
#define _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_SIZE      (sizeof(_bcm_dpp_port_tpid_class_acceptable_frame_type_info_t))

#define _DPP_AM_TEMPLATE_L2_FID_RPOFILES_COUNT (8)

/* l2 event handling, from event handling to even-handle-profile  */
#define _DPP_AM_TEMPLATE_L2_EVENT_HANDLE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_L2_EVENT_HANDLE_COUNT ((SOC_DPP_CONFIG(unit))->l2.mac_nof_event_handlers) 
#define _DPP_AM_TEMPLATE_L2_EVENT_HANDLE_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l2.nof_vsis)  /* nof vsis*/
#define _DPP_AM_TEMPLATE_L2_EVENT_HANDLE_SIZE   (sizeof(int))

/* l2 VSI learn profile, from VSI/FID to learn-profile  */
#define _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_COUNT _DPP_AM_TEMPLATE_L2_FID_RPOFILES_COUNT
#define _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l2.nof_vsis)  /* nof vsis*/
#define _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_SIZE   (sizeof(int))

/* FID aging profile, from VSI/FID to aging-profile  */

/* l2 flooding, Retrieve LIF information  */
#define _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_COUNT (4)
#define _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l2.nof_lifs + 1)  /* nof lifs + 1 dummy */
#define _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_SIZE (sizeof(bcm_dpp_vlan_flooding_profile_info_t))

/* vlan port protocol, from port profile to ether,vlan,tc */
#define _DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_COUNT (NOS_PRTCL_PORT_PROFILES + 1) /* NOF protocols + 1 dummy profile */
#define _DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_MAX_ENTITIES (_BCM_PETRA_NOF_LOCAL_PORTS(unit) + 1) /* 1 dummy port so NOF protocol will be reserved */  
#define _DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_SIZE (sizeof(bcm_dpp_vlan_port_protocol_profile_info_t)*DPP_PRTCL_NOF_ETHER_TYPE_IDS)


/* ip tunnel, from tunnel profile to SIP*/
#define _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_LOW_ID (0)
#define _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_COUNT (16 + 1) /* NOF profiles + 1 dummy profile */
#define _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l3.eep_db_size + 1) /* 1 dummy tunnel so NOF-profiles entry reserved for non allocated*/  
#define _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_SIZE (sizeof(SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP))   /* sip zero is not valid  */


/* ip tunnel, from tunnel profile to TTL*/
#define _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_LOW_ID (0)
#define _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_COUNT (4 + 1) /* NOF profiles + 1 dummy profile */
#define _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l3.eep_db_size + 1) /* 1 dummy tunnel so NOF-profiles entry reserved for non allocated*/  
#define _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_SIZE (sizeof(uint32))   /* ttl zero is not valid  */


/* ip tunnel, from tunnel profile to TOS*/
#define _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_LOW_ID (0)
#define _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_COUNT ((SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set)?(8+1):(16 + 1)) /* NOF profiles + 1 dummy profile */
#define _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l3.eep_db_size + 1) /* 1 dummy tunnel so NOF-profiles entry reserved for non allocated*/  
#define _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_SIZE (sizeof(uint32))   /* tos zero is not valid  */



/* VLAN Edit Profile */
#define _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_LOW_ID (0)
#define _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_COUNT  (8)
#define _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_MAX_ENTITIES(unit) (SOC_DPP_CONFIG(unit)->l2.nof_lifs + 1) /* Number of In-ACs + 1 dummy for VLAN-port application */
#define _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_SIZE   (sizeof(bcm_dpp_vlan_edit_profile_mapping_info_t))

#define _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_LOW_ID (0)
#define _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_COUNT  (16)
#define _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_MAX_ENTITIES(unit) (SOC_DPP_CONFIG(unit)->l2.nof_lifs) /* Number of Out-ACs */
#define _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_SIZE   (sizeof(bcm_dpp_vlan_egress_edit_profile_info_t))

/* Egress Priority Table */

/* Trap of reserved MC */
#define _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_LOW_ID (0)
#define _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_COUNT (8+1)
#define _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit)*_BCM_PETRA_L2_NOF_RSRV_MC)
#define _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_SIZE   (sizeof(bcm_dpp_user_defined_traps_t))

/* L2CP Egress Profile */
#define _DPP_AM_TEMPLATE_EGRESS_L2CP_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_EGRESS_L2CP_PROFILE_COUNT (4)
#define _DPP_AM_TEMPLATE_EGRESS_L2CP_PROFILE_MAX_ENTITIES(unit) BCM_RX_TRAP_L2CP_NOF_ENTRIES /* adding one dummy entry for 0 profile */
#define _DPP_AM_TEMPLATE_EGRESS_L2CP_PROFILE_SIZE (sizeof(bcm_dpp_l2cp_egress_profile_t))

/* VSI Egress Profile */
#define _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_COUNT (4)
#define _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_MAX_ENTITIES(unit) (SOC_DPP_CONFIG(unit)->l2.nof_vsis + 1) /* adding one dummy entry for 0 profile */
#define _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_SIZE (sizeof(bcm_dpp_vsi_egress_profile_t))
 
/* VSI Ingress Profile */
#define _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_COUNT (4)
#define _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_MAX_ENTITIES(unit) (SOC_DPP_CONFIG(unit)->l2.nof_vsis + 1) /* adding one dummy entry for 0 profile */
#define _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_SIZE (sizeof(bcm_dpp_vsi_ingress_profile_t))
 

/* OAM MEP ID SHORT.*/
#define _DPP_AM_TEMPLATE_MEP_ID_SHORT_LOW_ID (SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit))
#define _DPP_AM_TEMPLATE_MEP_ID_SHORT_COUNT (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) - _DPP_AM_TEMPLATE_MEP_ID_SHORT_LOW_ID )

/*For 48B MAID In Jericho and Arad+ OAMP_MEP_DB is divided to 3 logical banks: */

/* 1. longs MEPs non 48b maid: range 1 - size of umc table, all MEPs except every 4th MEP*/
#define _DPP_AM_TEMPLATE_MEP_ID_LONG_COUNT_NON_48_MAID ((SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit)) * 3 / 4)

/* 2. longs MEPs 48b maid: range 1 - size of umc table, every 4sth MEP*/
#define _DPP_AM_TEMPLATE_MEP_ID_LONG_COUNT_48_MAID_JERICHO ((SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit) - 1) / 4)
#define _DPP_AM_TEMPLATE_MEP_ID_LONG_COUNT_48_MAID_ARAD (SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit) / 4)

/* 1. Jumbo TLV  MEPs : every 16th MEP, total 1k entries*/
#define _DPP_AM_TEMPLATE_MEP_ID_COUNT_JUMBO_TLV (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) /16)
#define _DPP_AM_TEMPLATE_MEP_ID_COUNT_NO_JUMBO_TLV (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))

/* OAM icc map ndx */
#define _DPP_AM_TEMPLATE_OAM_ICC_MAP_NDX_LOW_ID (0)
#define _DPP_AM_TEMPLATE_OAM_ICC_MAP_NDX_COUNT (15)
#define _DPP_AM_TEMPLATE_OAM_ICC_MAP_NDX_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_OAM_ICC_MAP_NDX_SIZE (sizeof(SOC_PPC_OAM_ICC_MAP_DATA))

/* OAM SA MSB profiles (Jericho only)*/
#define _DPP_AM_TEMPLATE_OAM_SA_MSB_LOW_ID (0)
#define _DPP_AM_TEMPLATE_SA_MSB_NDX_COUNT (2)
#define _DPP_AM_TEMPLATE_OAM_SA_MSB_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_OAM_SA_MSB_SIZE (5) /* representing the 5 MSBs of the SA MAC address.*/


/* OAM OAMP punt event handling profile */
#define _DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_COUNT (SOC_PPC_OAM_OAMP_NUMBER_OF_PUNT_PROFILES(unit))
#define _DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit)) 
#define _DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_SIZE (sizeof(SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA))

/* OAM mep profile non accelerated */
/* MP-Profile 0 is used by the default MEPs, to prevent conflicts, starting from 1.
   Arad is limited to 3 profiles only and cannot have this privilege, starting from 0 */
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_LOW_ID (SOC_PPD_OAM_IS_CLASSIFIER_ARAD_MODE(unit) ? 0 : 1)
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_COUNT (SOC_PPC_OAM_NON_ACC_PROFILES_NUM - 1) /* Arad only Highest profile is reseved for passive */
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_COUNT_ARAD_PLUS (SOC_PPC_OAM_NON_ACC_PROFILES_ARAD_PLUS_NUM - 1) /* Arad+ and above */
#define _DPP_AM_TEMPLATE_BFD_MEP_PROFILE_NON_ACC_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)*8/*levels per lif*/*2/*directions*/)
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_SIZE (sizeof(SOC_PPC_OAM_LIF_PROFILE_DATA))

/* OAM mep profile accelerated */
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_LOW_ID (0)
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_COUNT (SOC_PPC_OAM_ACC_PROFILES_NUM) /* Highest profile is reseved for passive */
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_SIZE (sizeof(SOC_PPC_OAM_LIF_PROFILE_DATA))

/* OAM priority profil */
#define _DPP_AM_TEMPLATE_OAM_TX_PRIORITY_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_OAM_TX_PRIORITY_PROFILE_COUNT (8)
#define _DPP_AM_TEMPLATE_OAM_TX_PRIORITY_PROFILE_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_OAM_TX_PRIORITY_PROFILE_SIZE (sizeof(SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES))

/* OAM sd sf Profile */
#define _DPP_AM_TEMPLATE_OAM_SD_SF_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_OAM_SD_SF_PROFILE_COUNT (SOC_DPP_DEFS_GET(unit,oamp_number_of_sd_sf_profiles))
#define _DPP_AM_TEMPLATE_OAM_SD_SF_PROFILE_MAX_ENTITIES (SOC_DPP_DEFS_GET(unit,oamp_number_of_sd_sf_profiles))
#define _DPP_AM_TEMPLATE_OAM_SD_SF_PROFILE_SIZE (sizeof(SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB))
/* BFD Profiles */
#define _DPP_AM_TEMPLATE_BFD_IP_DIP_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_BFD_IP_DIP_PROFILE_COUNT (16)
#define _DPP_AM_TEMPLATE_BFD_IP_DIP_PROFILE_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_BFD_IP_DIP_PROFILE_SIZE (sizeof(SOC_SAND_PP_IPV6_ADDRESS)) 

#define _DPP_AM_TEMPLATE_BFD_PUSH_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_BFD_PUSH_PROFILE_COUNT (16)
#define _DPP_AM_TEMPLATE_BFD_PUSH_PROFILE_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_BFD_PUSH_PROFILE_SIZE (sizeof(SOC_PPC_MPLS_PWE_PROFILE_DATA))

#define _DPP_AM_TEMPLATE_BFD_REQ_INTERVAL_LOW_ID (0)
#define _DPP_AM_TEMPLATE_BFD_REQ_INTERVAL_COUNT (8)
#define _DPP_AM_TEMPLATE_BFD_REQ_INTERVAL_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_BFD_REQ_INTERVAL_SIZE (sizeof(uint32))

#define _DPP_AM_TEMPLATE_BFD_TOS_TTL_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_BFD_TOS_TTL_PROFILE_COUNT \
         (16 - (1 && SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop) - (1 && SOC_DPP_CONFIG(unit)->pp.bfd_echo_min_interval))
#define _DPP_AM_TEMPLATE_BFD_TOS_TTL_PROFILE_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_BFD_TOS_TTL_PROFILE_SIZE (sizeof(SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA))

#define _DPP_AM_TEMPLATE_BFD_SRC_IP_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_BFD_SRC_IP_PROFILE_COUNT (16)
#define _DPP_AM_TEMPLATE_BFD_SRC_IP_PROFILE_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_BFD_SRC_IP_PROFILE_SIZE (sizeof(uint32))

/* flexible maid 48 byte */
#define _DPP_AM_TEMPLATE_MAID_48_LOW_ID (0)
#define _DPP_AM_TEMPLATE_MAID_48_COUNT (682) /* In jericho there is 32k in OAMP-PE-GEN-MEM. Max Number of MAIDs = 32768/48 = ~682 */
#define _DPP_AM_TEMPLATE_MAID_48_COUNT_ARAD (5) /* In Arad+ there is 256B in OAMP-PE-GEN-MEM. Max Number of MAIDs = 256/48 = ~5 */
#define _DPP_AM_TEMPLATE_MAID_48_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_MAID_48_SIZE (sizeof(uint8)*48)


#define _DPP_AM_TEMPLATE_BFD_FLAGS_PROFILE_LOW_ID (1) /* HW constraint */
#define _DPP_AM_TEMPLATE_BFD_FLAGS_PROFILE_COUNT (7)
#define _DPP_AM_TEMPLATE_BFD_FLAGS_PROFILE_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_BFD_FLAGS_PROFILE_SIZE (sizeof(uint32))

#define _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_LOW_ID 0 
#define _DPP_AM_TEMPLATE_OAM_LM_DA_OUI_TABLES_COUNT 8
#define _DPP_AM_TEMPLATE_OAM_LM_DA_NIC_TABLES_COUNT 128
#define _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_MAX_ENTITIES (ARAD_PP_OAM_OAMP_MEP_DB_MEP_ENTRIES_NOF(unit))
#define _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_SIZE (sizeof(uint32))

/* Usage of MEP_PROFILE 0 in REPORT_MODE causes DMM generation by all entries of MEP_DB in Jericho.
   So we don't use this entry, leaving the total number of MEP_PROFILE entries 127. */
#define _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_LOW_ID SOC_IS_JERICHO(unit)   /* (Jericho ?) 1:0 */
#define _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_COUNT (SOC_PPC_OAM_MAX_NUMBER_OF_ETH1731_MEP_PROFILES(unit) - SOC_IS_JERICHO(unit)) /* if(Jericho) decrease 1 */
#define _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_SIZE (sizeof(SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY))

#define _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_LOW_ID           0
#define _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_COUNT            16
#define _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_MAX_ENTITIES     1
#define _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_SIZE             (sizeof(SOC_PPC_CRC_SELECT_INFO))
    
    
#define _DPP_AM_TEMPLATE_OAMP_FLEX_VER_MASK_LOW_ID          0
#define _DPP_AM_TEMPLATE_OAMP_FLEX_VER_MASK_COUNT           8
#define _DPP_AM_TEMPLATE_OAMP_FLEX_VER_MASK_MAX_ENTITIES    _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_COUNT
#define _DPP_AM_TEMPLATE_OAMP_FLEX_VER_MASK_SIZE            (sizeof(SOC_PPC_OAM_OAMP_CRC_MASK))


/*LIF MTU profile size*/
#define _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_LOW_ID       (0)
#define _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_COUNT_JB0        (4)  /*For JB0 its 4 for QAX and QUX its 8*/
#define _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_COUNT_QAX        (8)  /*For JB0 its 4 for QAX and QUX its 8*/
#define _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l2.nof_lifs + 1)  /* nof lifs + 1 dummy */
#define _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_CLS_SIZE     (sizeof(uint32))


/* Struct used internally by
   dpp_am_template_oam_local_port_2_system_port.
   The template alloce manager requires that all datas be unique, but the internal logic of
   dpp_am_template_oam_local_port_2_system_port
   may require common data for different profiles.*/
typedef struct {
   int data;
   int profile;
} _dpp_oam_am_local_port_2_sys_port_internal_data_t;

typedef _dpp_oam_am_local_port_2_sys_port_internal_data_t _dpp_oam_oamp_pe_gen_mem_internal_data_t;

#define _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_LOW_ID 0
#define _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_COUNT SOC_PPC_OAM_SIZE_OF_LOCAL_PORT_2_SYS_PORT_TABLE(unit)
#define _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_SIZE (sizeof(_dpp_oam_am_local_port_2_sys_port_internal_data_t))

#define _DPP_AM_TEMPLATE_OAM_OAMP_PE_GEN_MEM_LOW_ID 0
#define _DPP_AM_TEMPLATE_OAM_OAMP_PE_GEN_MEM_COUNT SOC_PPC_OAM_SIZE_OF_LOCAL_PORT_2_SYS_PORT_TABLE(unit)
#define _DPP_AM_TEMPLATE_OAM_OAMP_PE_GEN_MEM_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_OAM_OAMP_PE_GEN_MEM_SIZE (sizeof(_dpp_oam_oamp_pe_gen_mem_internal_data_t))



/* Trap egress */
#define _DPP_AM_TEMPLATE_TRAP_EGRESS_LOW_ID (0) /*profile 0 is default profile that means NO ACTION*/
#define _DPP_AM_TEMPLATE_TRAP_EGRESS_COUNT (_BCM_PPD_TRAP_MGMT_EG_ACTION_PROFILE_MAX) 

/* Profile 0 means means NO ACTION
   Profile 1 acts as a dummy profile, that means DROP ACTION
   Profile 2 acts as a dummy profile, that means CPU DROP ACTION
   Entity SOC_PPC_NOF_TRAP_EG_TYPES_ARAD will make sure NO ACTION wil always exist, and wont be deleted
   Entity SOC_PPC_NOF_TRAP_EG_TYPES_ARAD + 1 will make sure DROP ACTION wil always exist, and wont be deleted
   Entity SOC_PPC_NOF_TRAP_EG_TYPES_ARAD + 2 will make sure CPU DROP ACTION wil always exist, and wont be deleted
   Entity SOC_PPC_NOF_TRAP_EG_TYPES_ARAD + 3 + entry-id indicates the Field Processor entry action profile  */
#define _DPP_AM_TEMPLATE_TRAP_EGRESS_MAX_ENTITIES (BCM_DPP_AM_TRAP_EGRESS_MAX_ENTITIES_NO_FIELD + SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit)) 

#define _DPP_AM_TEMPLATE_TRAP_EGRESS_SIZE   (sizeof(SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO))  


/* Prog traps */
#define _DPP_AM_TEMPLATE_PROG_TRAP_LOW_ID (0)
#define _DPP_AM_TEMPLATE_PROG_TRAP_COUNT (4)
#define _DPP_AM_TEMPLATE_PROG_TRAP_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit)*4)
#define _DPP_AM_TEMPLATE_PROG_TRAP_SIZE   (sizeof(bcm_dpp_prog_trap_t))

/* Queue Rate class */
#define _DPP_AM_TEMPLATE_QUEUE_RATE_CLS_LOW_ID (0)
#define _DPP_AM_TEMPLATE_QUEUE_RATE_CLS_COUNT (SOC_TMC_ITM_RATE_CLASS_MAX+1)
#define _DPP_AM_TEMPLATE_QUEUE_RATE_CLS_MAX_ENTITIES(unit) (SOC_DPP_DEFS_GET(unit, nof_queues) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)
#define _DPP_AM_TEMPLATE_QUEUE_RATE_CLS_SIZE   (sizeof(bcm_dpp_cosq_ingress_rate_class_info_t))

/* System RED drop probability */
#define _DPP_AM_TEMPLATE_SYS_RED_DP_PR_LOW_ID             (0)
#define _DPP_AM_TEMPLATE_SYS_RED_DP_PR_COUNT              (SOC_TMC_ITM_SYS_RED_DRP_PROBS)
#define _DPP_AM_TEMPLATE_SYS_RED_DP_PR_MAX_ENTITIES(unit) (SOC_DPP_DEFS_GET(unit, nof_queues) * 2) /* Mulipale NOF Queues by 2 since we have 2 pointer per Queue */
#define _DPP_AM_TEMPLATE_SYS_RED_DP_PR_SIZE               (sizeof(bcm_dpp_cosq_ingress_system_red_dp_pr_info_t))

/* VSQ PG TC mapping profile */
#define _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING_LOW_ID (0)
#define _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING_COUNT (SOC_TMC_NOF_VSQ_PG_MAPPING_PROFILES)
/*nof VSQFs*/
#define _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING_MAX_ENTITIES(unit) (SOC_DPP_DEFS_GET(unit, nof_vsq_f))
#define _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING (sizeof(uint32))

/* VSQ Rate class */
#define _DPP_AM_TEMPLATE_VSQ_RATE_CLS_LOW_ID (0)
#define _DPP_AM_TEMPLATE_VSQ_A_RATE_CLS_COUNT(unit) (SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit) + 1)
#define _DPP_AM_TEMPLATE_VSQ_RATE_CLS_COUNT(unit) (SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit) + 1)
#define _DPP_AM_TEMPLATE_VSQ_RATE_CLS_MAX_ENTITIES(unit, vsq_group) (SOC_TMC_ITM_NOF_VSQS_IN_GROUP(unit, vsq_group) * SOC_DPP_DEFS_GET(unit, nof_cores))
#define _DPP_AM_TEMPLATE_VSQ_RATE_CLS_SIZE(vsq_group)                                                               (\
            (vsq_group < SOC_TMC_ITM_VSQ_GROUP_SRC_PORT ) ? (sizeof(bcm_dpp_cosq_queue_vsq_rate_class_info_t)   ) : (\
            (vsq_group == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) ? (sizeof(bcm_dpp_cosq_src_port_vsq_rate_class_info_t[SOC_TMC_ITM_NOF_RSRC_POOLS])) : (\
            (vsq_group == SOC_TMC_ITM_VSQ_GROUP_PG      ) ? (sizeof(bcm_dpp_cosq_pg_vsq_rate_class_info_t)      ) : 0)))

void* _bcm_dpp_am_template_vsq_rate_cls_data (
        const SOC_TMC_ITM_VSQ_GROUP vsq_group, 
        const bcm_dpp_cosq_vsq_rate_class_info_t* data_rate_cls) {
    if (vsq_group < SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        return (void *)&(data_rate_cls->queue[vsq_group]);
    }
    if (vsq_group == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        return (void *)&(data_rate_cls->port);
    }
    if (vsq_group == SOC_TMC_ITM_VSQ_GROUP_PG) {
        return (void *)&(data_rate_cls->pg);
    }
    return (void *)NULL;
}

/* Ingress Queue Discount Class */
#define _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_LOW_ID (0)
/* in new mechanism only half(8) profiles are actually active, in legacy mode all(16) profiles are active */
#define _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_COUNT(unit)  (SOC_TMC_ITM_PER_PACKET_COMPENSATION_ENABLED(unit) ? \
                                                            (SOC_TMC_ITM_NOF_CR_DISCNT_CLS_NDXS/2) : \
                                                             SOC_TMC_ITM_NOF_CR_DISCNT_CLS_NDXS)

#define _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_MAX_ENTITIES(unit) SOC_DPP_DEFS_GET(unit, nof_queues)
#define _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_SIZE   (sizeof(SOC_TMC_ITM_CR_DISCOUNT_INFO))

/* Ingress PP Port Discount Class */
#define _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_LOW_ID (0)
#define _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_COUNT  (8)

#define _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_MAX_ENTITIES(unit) (SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core))
#define _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_SIZE   (sizeof(SOC_TMC_ITM_CR_DISCOUNT_INFO))

/* Egress Port Discount Class */
#define _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_LOW_ID (0)
#define _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_COUNT  (SOC_TMC_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES)
/* nof entities is been done by UC/MC * header_type (port) */
#define _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_MAX_ENTITIES(unit) (SOC_TMC_NOF_FAP_PORTS_ARAD)
#define _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_SIZE   (sizeof(SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO))

/* PUSH profile */
/* We support only profiles from 1-7.
 * profile 0 is not supported since In EEDB Second MPLS label can have push-profile 1-7.
 */
#define _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_LOW_ID(unit) (1)
#define _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_COUNT(unit) (SOC_IS_JERICHO_B0_AND_ABOVE(unit)? 15 : 7)
#define _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_MAX_ENTITIES (128 * 1024)
#define _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_SIZE   (sizeof(SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO))

/* LIF Termination profile */
#define _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_LOW_ID       (0)
#define _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT       (4)
#define _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MAX_ENTITIES (64 * 1024) 
#define _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_SIZE   (sizeof(SOC_PPC_MPLS_TERM_PROFILE_INFO))
#define _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MASK        0x3
#define _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_ID_MSB_AS_PW 0x4


/* PORT MACT SA DROP profile */ 
#define _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_COUNT (_BCM_DPP_PORT_TRAP_CODE_SA_DROP_NOF_PROFILES)
#define _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit) + _BCM_DPP_PORT_TRAP_CODE_SA_DROP_NOF_PROFILES) /* With 4 dummies one per profile*/ 
#define _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_SIZE   (sizeof(_bcm_petra_port_mact_mgmt_action_profile_t))

/* PORT MACT DA UNKNOWN profile */ 
#define _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_COUNT (_BCM_DPP_PORT_TRAP_CODE_DA_UNKNOWN_NOF_PROFILES)
#define _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_MAX_ENTITIES(unit) (_BCM_PETRA_NOF_LOCAL_PORTS(unit)+_BCM_DPP_PORT_TRAP_CODE_DA_UNKNOWN_NOF_PROFILES) /* With 4 dummies one per profile*/ 
#define _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_SIZE   (sizeof(_bcm_petra_port_mact_mgmt_action_profile_t))


/* METER profile */
#define _DPP_AM_TEMPLATE_METER_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_METER_PROFILE_COUNT(unit) (SOC_DPP_CONFIG(unit)->meter.nof_meter_profiles - SOC_DPP_CONFIG(unit)->meter.nof_high_rate_profiles) /* each group */
#define _DPP_AM_TEMPLATE_METER_PROFILE_MAX_ENTITIES(unit) (SOC_DPP_CONFIG(unit)->meter.nof_meter_a)  /* each group */
#define _DPP_AM_TEMPLATE_METER_PROFILE_SIZE   (sizeof(bcm_dpp_am_meter_entry_t))


/* e2e port hr flow control profile */
#define _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_LOW_ID        (0)
#define _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_COUNT         (SOC_TMC_SCH_LOW_FC_NOF_AVAIL_CONFS)
#define _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_MAX_ENTRIES   (SOC_TMC_NOF_FAP_PORTS_MAX)
#define _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_MAX_ENTRIES_PER_DEVICE(unit) (SOC_DPP_DEFS_GET(unit, nof_logical_ports)+1)
#define _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_SIZE          (sizeof(int))

/* sched (cl) class profile */
#define _DPP_AM_TEMPLATE_COSQ_SCHED_CLASS_LOW_ID        (0)
#define _DPP_AM_TEMPLATE_COSQ_SCHED_CLASS_COUNT         (SOC_TMC_SCH_NOF_CLASS_TYPES)
#define _DPP_AM_TEMPLATE_COSQ_SCHED_CLASS_MAX_ENTRIES(unit) ((SOC_TMC_CL_SE_ID_MAX_ARAD + 1) * 2)

#define _DPP_AM_TEMPLATE_COSQ_SCHED_CLASS_SIZE          (sizeof(bcm_dpp_cosq_sched_class_data_t))

/* TDM direct routing link pointer */
#define _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_LOW_ID (0)
#define _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_COUNT (36)
#define _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_MAX_ENTITIES (SOC_TMC_NOF_FAP_PORTS_ARAD) 
#define _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_SIZE   (sizeof(SOC_TMC_TDM_DIRECT_ROUTING_INFO))


/* Ingress Flow TC Mapping - ARAD ONLY */
#define _DPP_AM_TEMPLATE_INGRESS_FLOW_TC_MAPPING_LOW_ID (0)
#define _DPP_AM_TEMPLATE_INGRESS_FLOW_TC_MAPPING_COUNT (SOC_TMC_NOF_INGRESS_FLOW_TC_MAPPING_PROFILES)
#define _DPP_AM_TEMPLATE_INGRESS_FLOW_TC_MAPPING_CLS_SIZE  (sizeof(SOC_TMC_ITM_TC_MAPPING))

/* Ingress UC TC Mapping - ARAD ONLY */
#define _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_LOW_ID (0)
#define _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_COUNT (SOC_TMC_NOF_INGRESS_UC_TC_MAPPING_PROFILES)
#define _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_MAX_ENTITIES(unit) (SOC_DPP_DEFS_GET(unit, nof_queues))
#define _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_CLS_SIZE  (sizeof(SOC_TMC_ITM_TC_MAPPING))

/* FC PFC Generic Bitmap Mapping - ARAD ONLY */
#define _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_LOW_ID (0)
#define _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_COUNT (SOC_DPP_DEFS_GET(unit, nof_fc_pfc_generic_bitmaps) + 1) /* NOF profiles + 1 dummy profile */
#define _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_MAX_ENTITIES (1024)
#define _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_CLS_SIZE (sizeof(SOC_TMC_FC_PFC_GENERIC_BITMAP))

/* TTL scope index for RIF TTL thershold */
#define _DPP_AM_TEMPLATE_TTL_SCOPE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_TTL_SCOPE_COUNT (8)
#define _DPP_AM_TEMPLATE_TTL_SCOPE_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l3.nof_rifs)  
#define _DPP_AM_TEMPLATE_TTL_SCOPE_SIZE (sizeof(SOC_SAND_PP_IP_TTL))

/* CNM Queue Profiles - ARAD ONLY */
#define _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_LOW_ID (0)
#define _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_COUNT (SOC_TMC_CNM_NOF_PROFILES)
#define _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_MAX_ENTITIES (SOC_TM_CNM_MAX_CP_QUEUES)
#define _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_CLS_SIZE (sizeof(SOC_TMC_CNM_CP_PROFILE_INFO))

/* PTP port Profiles - ARAD ONLY */
#define _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_LOW_ID       (0)
#define _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_COUNT        (SOC_PPC_PTP_IN_PP_PORT_PROFILES_NUM)
#define _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_MAX_ENTITIES (_BCM_PETRA_NOF_LOCAL_PORTS(unit))
#define _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_CLS_SIZE     (sizeof(SOC_PPC_PTP_PORT_INFO))

/* VRRP CAM Profiles - ARADPLUS ONLY */
#ifdef   BCM_88660_A0

#define _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_LOW_ID       (0)
#define _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_COUNT(_unit)        (_BCM_PETRA_L3_VRRP_CAM_MAX_NOF_ENTIRES(_unit))
#define _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_MAX_ENTITIES(_unit) (_BCM_PETRA_L3_VRRP_MAX_VSI(_unit))    /* Number of avaiable VSIs*/
#define _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_GLOBAL_MAX(_unit)   (_DPP_AM_TEMPLATE_L3_VRRP_PROFILE_COUNT(_unit) \
                                                            * _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_MAX_ENTITIES(_unit))
#define _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_CLS_SIZE     (sizeof(SOC_PPC_VRRP_CAM_INFO))

#endif  /*BCM_88660_A0*/

/* l3 rif mac termination combination */
#define _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_LOW_ID       (0)
#define _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_COUNT        ((SOC_DPP_CONFIG(unit))->l3.actual_nof_routing_enablers_bm)
#define _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_MAX_ENTITIES ((SOC_DPP_CONFIG(unit))->l3.nof_rifs+1)
#define _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_CLS_SIZE     (sizeof(uint32))

/* ethTypeIndex db. Used by ROO LL format - JERICHO ONLY */

#define _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_LOW_ID       (0)
#define _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_COUNT        (SOC_DPP_DEFS_GET(unit, nof_roo_ll_format_eth_type_indexs))
#define _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_MAX_ENTITIES (SOC_DPP_DEFS_GET(unit, nof_eedb_lines))
#define _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_CLS_SIZE     (sizeof(SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO))

/* OUT-RIF Profiles - QAX ONLY */
#define _DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_LOW_ID              (0)
#define _DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_COUNT(_unit)        (SOC_DPP_DEFS_GET(unit, nof_rif_profiles))
#define _DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_MAX_ENTITIES(_unit) ((SOC_DPP_CONFIG(_unit))->l3.nof_rifs)
#define _DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_CLS_SIZE            (sizeof(QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT))

/* IP tunnel encapsulation mode - Jericho B0, QAX and above */
#define _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_LOW_ID         (0)
#define _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_COUNT          (4 + 1) /* support up to 4 encapsulation mode + 1 dummy profile */
#define _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_MAX_ENTITIES   (SOC_DPP_DEFS_GET(unit, nof_eedb_lines))
#define _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_CLS_SIZE       (sizeof(SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO))

#define ALLOC_MNGR_ACCESS                   sw_state_access[unit].dpp.bcm.alloc_mngr

/* Seamless BFD non accelerated SRC IP Profile */
#define _DPP_AM_TEMPLATE_BFD_SBFD_NON_ACC_SRC_IP_PROF_LOW_ID 0
#define _DPP_AM_TEMPLATE_BFD_SBFD_NON_ACC_SRC_IP_PROF_COUNT  16
#define _DPP_AM_TEMPLATE_BFD_SBFD_NON_ACC_SRC_IP_PROF_MAX_ENTITIES (SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))
#define _DPP_AM_TEMPLATE_BFD_SBFD_NON_ACC_SRC_IP_PROF_SIZE sizeof(uint32) 
typedef enum {
  /* All regular FECs. */
  _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_A,

  /* FECs pointed by other FECs, relevant for Jericho only */
  _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_B
} _bcm_dpp_pp_fec_group_type;

/*
 * Function:
 *     _bcm_dpp_pp_resource_setup
 * Purpose:
 *      Initialize resource management pool data structures for each pool type (not including cosq related pools
 *      which handle warmboot within the cosq module)
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_dpp_pp_resource_setup(int unit, int res_id, int core_id, int pool_id) {

    char *desc_str;
    int rv = BCM_E_NONE;
    bcm_dpp_am_pool_info_t p_info;
    const _bcm_dpp_field_device_info_t *devInfo;
    sw_state_res_tagged_bitmap_extras_t tagBitMapExtras;
    void *extraPtr = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&p_info, 0, sizeof(bcm_dpp_am_pool_info_t));
    desc_str = "init";
    p_info.pool_id = pool_id;
    p_info.res_id =  res_id;
    p_info.core_id = core_id;

    switch (res_id) {
        case dpp_am_res_vsi_vlan:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = SOC_PPC_VSI_LOW_CNFG_VALUE;
            p_info.count = 4096;
            p_info.max_elements_per_allocation = 1;
            desc_str = "VSIs for TB VLANS";
            break;
        case dpp_am_res_vsi_mstp:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 4096;
            p_info.count = ((SOC_DPP_CONFIG(unit))->l2.nof_vsis)-4096;
            p_info.max_elements_per_allocation = 1;
            desc_str = "VSIs for MSTP";
            break;     
        case dpp_am_res_lif_pwe:
            BCM_EXIT;
            break;
        case dpp_am_res_lif_ip_tnl:
            BCM_EXIT;
            break;
        case dpp_am_res_trap_single_instance:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = SOC_PPC_NOF_TRAP_CODES;
            p_info.max_elements_per_allocation = 1;
            desc_str = "Single instance trap";
            break;
        case dpp_am_res_trap_user_define:
            /* mostly direct allocation */
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = SOC_PPC_TRAP_CODE_USER_DEFINED_0;
            p_info.count = SOC_DPP_CONFIG(unit)->dpp_general.nof_user_define_traps;
            p_info.max_elements_per_allocation = 1;
            desc_str = "User define traps";
            break;
        case dpp_am_res_trap_virtual:
            /* mostly direct allocation */
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = _BCM_NUMBER_OF_VIRTUAL_TRAPS;
            p_info.max_elements_per_allocation = 1;
            desc_str = "Virtual traps";
            break;
        case dpp_am_res_trap_reserved_mc:
            /* mostly direct allocation */
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = pool_id;
            p_info.count = 8;
            p_info.max_elements_per_allocation = 0; /* currently unused */
            desc_str = "Reserved mc traps";
            break;
        case dpp_am_res_trap_prog:
            /* mostly direct allocation */
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = 4;
            p_info.max_elements_per_allocation = 0; /* currently unused */
            desc_str = "Prog traps";
            break;
        case dpp_am_res_trap_egress:
            /* mostly direct allocation */
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = _DPP_AM_TEMPLATE_TRAP_EGRESS_MAX_ENTITIES;  
            p_info.max_elements_per_allocation = 1;
            desc_str = "ERPP traps";
            break;
        case dpp_am_res_trap_etpp:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = SOC_PPC_NOF_TRAP_ETPP_TYPES_ARAD ;  
            p_info.max_elements_per_allocation = 1; 
            desc_str = "ETPP traps";
            break;
        case dpp_am_res_snoop_cmd:
            /* mostly direct allocation */
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1;
            p_info.count = 15;
            p_info.max_elements_per_allocation = 1;
            desc_str = "Snoop command";
            break;
        case dpp_am_res_lif_dynamic:
            BCM_EXIT;
            break;
        case dpp_am_res_mc_dynamic:
            if (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count = 1;
                p_info.max_elements_per_allocation = 1;
                desc_str = "MC groups for dynamic use";
            } else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_fec_global:
            {
                int bank_id = core_id;
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                if (bank_id == 0) {
                    p_info.start = SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved;
                    p_info.count = SOC_PPD_FRWRD_FEC_NOF_FECS_PER_BANK(unit) - p_info.start; 
                } else {
                    p_info.count = SOC_PPD_FRWRD_FEC_NOF_FECS_PER_BANK(unit); 
                    p_info.start = bank_id * p_info.count;
                }
                desc_str = "FECs for global use";
                p_info.max_elements_per_allocation = 2;
            }
            break;

        /* case dpp_am_res_failover_id: */
        case dpp_am_res_failover_common_id:
            if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 1;
                p_info.count = SOC_DPP_DEFS_GET(unit, nof_failover_fec_ids) - 1;
                p_info.max_elements_per_allocation = 1;
                desc_str = "Failover common id";
                break;
             } else {
                BCM_EXIT;
             }
        case dpp_am_res_failover_ingress_id:
            if (SOC_IS_JERICHO(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 1;
                p_info.count = SOC_DPP_DEFS_GET(unit, nof_failover_ingress_ids) - 1;
                p_info.max_elements_per_allocation = 1;
                desc_str = "Failover Ingress Id";
                break;
            } else {
                BCM_EXIT;
            }

        case dpp_am_res_failover_fec_id:
            if (SOC_IS_JERICHO(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 1;
                p_info.count = SOC_DPP_DEFS_GET(unit, nof_failover_fec_ids) - 1;
                p_info.max_elements_per_allocation = 1;
                desc_str = "Failover FEC Id";
                break;
            } else {
                BCM_EXIT;
            }

        case dpp_am_res_failover_egress_id:
            if (SOC_IS_JERICHO(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 1;
                p_info.count = SOC_DPP_DEFS_GET(unit, nof_failover_egress_ids) - 1;
                p_info.max_elements_per_allocation = 2;
                desc_str = "Failover Egress Id";
                break;
            } else {
                BCM_EXIT;
            }

        case dpp_am_res_ecmp_id:
           if (!soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"pon_without_rif_support",0))  {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 1;
                p_info.count = SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved - 1;
                p_info.max_elements_per_allocation = 1;
                desc_str = "ECMP id";
            } else {
                BCM_EXIT;
            }
            break;

    case dpp_am_res_rp_id:
        if (_BCM_DPP_IPMC_NOF_RPS(unit) != 0)  {
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = _BCM_DPP_IPMC_NOF_RPS(unit);
            p_info.max_elements_per_allocation = 1;
            desc_str = "RP id";
        } else {
            BCM_EXIT;
        }
    break;
        /* the following eep pools should be used only in PP mode */
        case dpp_am_res_eep_global:
        case dpp_am_res_eep_ip_tnl:
        case dpp_am_res_eep_mpls_tunnel:
            BCM_EXIT;
            break;
        case dpp_am_res_glbl_src_ip:
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count = 16;
                p_info.max_elements_per_allocation = 1;
                desc_str = "IP tunnel src ip";
            break;
        case dpp_am_res_glbl_ttl:
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count = 4;
                p_info.max_elements_per_allocation = 1;
                desc_str = "IP tunnel ttl";
            break;
        case dpp_am_res_glbl_tos:
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count = 16;
                p_info.max_elements_per_allocation = 1;
                desc_str = "IP tunnel tos";
            break;
        case dpp_am_res_eg_out_ac:
        case dpp_am_res_eg_out_rif:
		case dpp_am_res_eg_data_erspan:
		case dpp_am_res_eg_data_l2tp:
        case dpp_am_res_ipv6_tunnel:
        case dpp_am_res_eg_data_trill_invalid_entry:
        case dpp_am_res_eep_trill:
                BCM_EXIT;
            break;
        case dpp_am_res_qos_ing_elsp:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = SOC_DPP_CONFIG(unit)->qos.nof_ing_elsp;
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS INGRESS LABEL MAP ID";
            break;
        case dpp_am_res_qos_ing_lif_cos:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1;
            p_info.count = SOC_DPP_CONFIG(unit)->qos.nof_ing_lif_cos - 1;
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS INGRESS LIF/COS IDs";
            break;
        case dpp_am_res_qos_ing_pcp_vlan:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1;
            p_info.count = SOC_DPP_CONFIG(unit)->qos.nof_ing_pcp_vlan - 1;
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS INGRESS PCP PROFILE IDs";
            break;
        case dpp_am_res_qos_ing_color:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1;
            p_info.count = SOC_DPP_CONFIG(unit)->qos.nof_ing_dp - 1;
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS INGRESS DP PROFILE IDs";
            break;
        case dpp_am_res_qos_egr_remark_id: 
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1;
            p_info.count = SOC_DPP_CONFIG(unit)->qos.nof_egr_remark_id - 1;
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS EGRESS REMARK QOS IDs";
            break;
        case dpp_am_res_qos_egr_mpls_php_id: 
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1;
            p_info.count = SOC_DPP_CONFIG(unit)->qos.nof_egr_mpls_php - 1;
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS EGRESS MPLS PHP QOS IDs";
            break;
        case dpp_am_res_qos_egr_pcp_vlan:
            /*
             * 1. Default profile
             *    Profile 0 is used for the defalut egress vlan editing for ARAD and Petra.
             *    For ARAD, the default profile can be configured.
             *    For PETRA, the default profile can't be configured.
             * 2. nof_egr_pcp_vlan
             *    The total number of Egr pcp vlan profile is 16 for Arad, 8 for Petra.
             * So, the start profile ID is 0 for Arad, 1 for petra.
             * the count of egr pcp vlan profile which can be allocated is 16 for ARAD and 7 for PETRA.
             */
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = (SOC_DPP_CONFIG(unit)->qos.nof_egr_pcp_vlan);
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS EGRESS PCP PROFILE IDs";
            break;
        case dpp_am_res_qos_ing_cos_opcode:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count =  SOC_DPP_CONFIG(unit)->qos.nof_ing_cos_opcode;
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS INGRESS COS OPCODE IDs";
            break;
        case dpp_am_res_qos_egr_l2_i_tag:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count =  SOC_DPP_CONFIG(unit)->qos.nof_egr_l2_i_tag;
            p_info.max_elements_per_allocation = 1;
            desc_str = "QOS EGRESS L2 I TAG PROFILE IDs";
            break;
#ifdef BCM_88660
        case dpp_am_res_qos_egr_dscp_exp_marking:
            if (SOC_IS_ARADPLUS(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count =  SOC_DPP_CONFIG(unit)->qos.nof_egr_dscp_exp_marking;
                p_info.max_elements_per_allocation = 1;
                desc_str = "QOS EGRESS DSCP/EXP MARKING PROFILE IDs";
            } else{
                BCM_EXIT;
            }
            break;
#endif /* BCM_88660 */
        case dpp_am_res_meter_a:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = SOC_IS_JERICHO(unit) ? 0 : 1; /* don't use meter-0 in Arad*/
            p_info.count =  SOC_IS_JERICHO(unit) ? SOC_DPP_CONFIG(unit)->meter.nof_meter_a : SOC_DPP_CONFIG(unit)->meter.nof_meter_a - 1;
            p_info.max_elements_per_allocation = 4;
            desc_str = "number of meters in processor A";
            break;
        case dpp_am_res_meter_b:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = SOC_IS_JERICHO(unit) ? 0 : 1; /* don't use meter-0 in Arad*/
            p_info.count =  SOC_IS_JERICHO(unit) ? SOC_DPP_CONFIG(unit)->meter.nof_meter_b : SOC_DPP_CONFIG(unit)->meter.nof_meter_b - 1;
            p_info.max_elements_per_allocation = 4;
            desc_str = "number of meters in processor B";
			break;
        case dpp_am_res_ether_policer:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1; /* don't use global policer-0*/;
            p_info.count = 8 - 1;
            desc_str = "SW handles of policer";
            p_info.max_elements_per_allocation = 4;
            break;
        case dpp_am_res_oam_ma_index:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 0;
            p_info.count = SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit);
            desc_str = "OAM MA Index";
            p_info.max_elements_per_allocation = 1;
            break;
    case dpp_am_res_oam_mep_id_short:
        if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit) && !SOC_IS_FLAIR(unit)) {
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = _DPP_AM_TEMPLATE_MEP_ID_SHORT_LOW_ID;
            p_info.count = _DPP_AM_TEMPLATE_MEP_ID_SHORT_COUNT; /* upper half of the maximal number of MEPs*/
            desc_str = "OAM MEP Index for short MA name format";
            p_info.max_elements_per_allocation = 1;
        } else {
            BCM_EXIT;
        }
        break;
    case dpp_am_res_oam_mep_id_long:
        if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit) && !SOC_IS_FLAIR(unit)
            && !(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0))) {
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            if (SOC_IS_JERICHO(unit)) {
                p_info.start = 1;
                p_info.count = SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit) - 1; /* Long ID MEPs are limited to the size of UMC_TABLE */
            } else {
                p_info.start = 0;
                p_info.count = SOC_PPC_OAM_SIZE_OF_UMC_TABLE(unit); /* Long ID MEPs are limited to the size of UMC_TABLE */
            }
            desc_str = "OAM MEP Index for ICC MA name format";
            p_info.max_elements_per_allocation = 1;
        } else {
            BCM_EXIT;
        }
        break;
    case dpp_am_res_oam_mep_id_long_non_48_maid:
        /* longs MEPs non 48b maid: range 1 - size of umc table, all MEPs except every 4th MEP*/
        if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit) && !SOC_IS_FLAIR(unit)
            && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            if (SOC_IS_JERICHO(unit)) {
                p_info.start = 1;
                p_info.count = _DPP_AM_TEMPLATE_MEP_ID_LONG_COUNT_NON_48_MAID; /* Long ID MEPs are limited to the size of UMC_TABLE */
            } else {
                p_info.start = 0;
                p_info.count = _DPP_AM_TEMPLATE_MEP_ID_LONG_COUNT_NON_48_MAID; /* Long ID MEPs are limited to the size of UMC_TABLE */
            }
            desc_str = "OAM MEP Index for ICC MA name format";
            p_info.max_elements_per_allocation = 1;
        } else {
            BCM_EXIT;
        }
        break;
    case dpp_am_res_oam_mep_id_jumbo_tlv:
        /* Jumbo tlv MEPs : range 1 - size of 1K , every 16th MEP*/
        if (_BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1; 
            p_info.count = _DPP_AM_TEMPLATE_MEP_ID_COUNT_JUMBO_TLV;
            desc_str = "OAM MEP Index for Jumbo TLV";
            p_info.max_elements_per_allocation = 1;
        } else {
            BCM_EXIT;
        }
        break;
    case dpp_am_res_oam_mep_id_no_jumbo_tlv:
        if (_BCM_OAM_IS_JUMBO_TLV_DM_ENABLE(unit)) {
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1; 
            p_info.count = _DPP_AM_TEMPLATE_MEP_ID_COUNT_NO_JUMBO_TLV;
            desc_str = "OAM MEP Index for NON Jumbo TLV";
            p_info.max_elements_per_allocation = 1;
        } else {
            BCM_EXIT;
        }
        break;
    case dpp_am_res_oam_mep_id_long_48_maid:
        /* longs MEPs 48b maid: range 1 - size of umc table, every 4th MEP*/
        if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit) && !SOC_IS_FLAIR(unit)
            && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)) {
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            if (SOC_IS_JERICHO(unit)) {
                p_info.start = 1;
                p_info.count = _DPP_AM_TEMPLATE_MEP_ID_LONG_COUNT_48_MAID_JERICHO; /* Long ID MEPs are limited to the size of UMC_TABLE */
            } else {
                p_info.start = 0;
                p_info.count = _DPP_AM_TEMPLATE_MEP_ID_LONG_COUNT_48_MAID_ARAD; /* Long ID MEPs are limited to the size of UMC_TABLE */
            }
            desc_str = "OAM MEP Index for ICC MA name format";
            p_info.max_elements_per_allocation = 1;
        } else {
            BCM_EXIT;
        }
        break;
        case dpp_am_res_oam_mep_id:
            if (SOC_IS_JERICHO_PLUS(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 1;
                p_info.count = SOC_DPP_DEFS_GET(unit, oamp_number_of_meps) - 1; 
                desc_str = "OAM MEP Index";
                p_info.max_elements_per_allocation = 1;
            }
            else {
                BCM_EXIT;
            }
            break;

        case dpp_am_res_oam_mep_db_id:
            if (SOC_IS_QAX(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 1;
                /* Number of entries in MEP DB (In QAX there are 4k entries that have no counterpart in classifier) */
                p_info.count = SOC_DPP_DEFS_GET(unit, oamp_number_mep_db_entries) - 1;
                desc_str = "OAM MEP DB Index";
                p_info.max_elements_per_allocation = 1;
            }
            else {
                BCM_EXIT;
            }
            break;

        case dpp_am_res_oam_rmep_id:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            if(SOC_IS_JERICHO(unit)) {
                p_info.start = (1 << _BCM_OAM_REMOTE_MEP_INDEX_BIT) + 1;
                p_info.count = SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit) - 1; /* maximal number of RMEPs is 16k*/
            } else {
                p_info.start = (1 << _BCM_OAM_REMOTE_MEP_INDEX_BIT);
                p_info.count = SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit); /* maximal number of RMEPs is 16k*/
            }
            desc_str = "OAM RMEP Index";
            p_info.max_elements_per_allocation = 1;
            break;
            
        case dpp_am_res_oam_sd_sf_id:
            if (SOC_IS_QAX(unit)) {
               p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
               p_info.start =0;
               p_info.count = SOC_DPP_DEFS_GET(unit,oamp_number_of_sd_sf_db); /* maximal number of SD/SF DB is 4.5k*/
               desc_str = "OAM SD/SF Index";
               p_info.max_elements_per_allocation = 1;
            }
            else {
               BCM_EXIT;
            }
        break;
            
       case dpp_am_res_oam_y_1711_sd_sf_id:
                if (SOC_IS_QAX(unit)) {
                   p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                   p_info.start =0;
                   p_info.count = SOC_DPP_DEFS_GET(unit,oamp_number_of_sd_sf_y_1711_db); /* maximal number of SD/SF 1711 DB is 4.5k*/
                   desc_str = "OAM Y1711 SD/SF Index";
                   p_info.max_elements_per_allocation = 1;
                }
                else {
                   BCM_EXIT;
                }
            break;

        case dpp_am_res_oam_trap_code_upmep_ftmh_header:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_FIRST;
            p_info.count = SOC_PPC_TRAP_CODE_OAM_FTMH_MIRROR_TRAP_CODES_NUM;
            desc_str = "OAM trap code upmep ftmh header";
            p_info.max_elements_per_allocation = 1;
            break;
           
        case dpp_am_res_pon_channel_profile:
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count = 16;
                desc_str = "Profiles for PON use";
                p_info.max_elements_per_allocation = 1;
            break;

        case dpp_am_res_vlan_edit_action_ingress:
            if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)
                && SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(unit) != 0) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count =  SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(unit) ;
                p_info.max_elements_per_allocation = 1;
                desc_str = "VLAN translation ingress usage";
            } else {
                BCM_EXIT;
            }
            break;

        case dpp_am_res_vlan_edit_action_egress:
            if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)
                && SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_IDS(unit) != 0) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count =  SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_IDS(unit);
                p_info.max_elements_per_allocation = 1;
                desc_str = "VLAN translation egress usage";
            } else {
                BCM_EXIT;
            }
            break;

        case dpp_am_res_trill_virtual_nick_name:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.start = 1;
            p_info.count =  SOC_PPC_MYMAC_VIRTUAL_NICK_NAMES_SIZE-1;
            p_info.max_elements_per_allocation = 1;
            desc_str = "trill virtual nickname";
            break;

        case dpp_am_res_local_inlif_common:
        case dpp_am_res_local_inlif_wide:
            if (SOC_IS_JERICHO(unit) && SOC_DPP_PP_ENABLE(unit))
            {
                int bank_id = core_id;
                p_info.res_type = SW_STATE_RES_ALLOCATOR_TAGGED_BITMAP;
                p_info.count = ( res_id == dpp_am_res_local_inlif_common ) ? ( _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK / 2) : ( _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK / 3 ); 
                p_info.start = ( bank_id * p_info.count);
                desc_str = "Local wide/common Inlif Id";
                p_info.max_elements_per_allocation = 1;
                tagBitMapExtras.grain_size = 1;
                tagBitMapExtras.tag_length = sizeof(uint8);
                extraPtr = &tagBitMapExtras;
            }
            else {
                BCM_EXIT;
            }
            break;
         case dpp_am_res_local_outlif:
            if (SOC_IS_JERICHO(unit) && SOC_DPP_PP_ENABLE(unit))
            {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_TAGGED_BITMAP;
                p_info.count = _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK(unit); 
                p_info.start =  0;
                desc_str = "Local outlif Id";
                p_info.max_elements_per_allocation = 2;
                tagBitMapExtras.grain_size = 1;
                tagBitMapExtras.tag_length = sizeof(uint8);
                extraPtr = &tagBitMapExtras;
            }
            else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_global_outlif:
        case dpp_am_res_global_sync_lif:
            if (SOC_IS_JERICHO(unit) && SOC_DPP_PP_ENABLE(unit)){
                p_info.res_type = SW_STATE_RES_ALLOCATOR_TAGGED_BITMAP;
                p_info.start = SOC_DPP_CONFIG(unit)->l3.nof_rifs;
                p_info.count = _BCM_DPP_AM_NOF_GLOBAL_OUT_LIFS(unit) - SOC_DPP_CONFIG(unit)->l3.nof_rifs;
                tagBitMapExtras.grain_size = _BCM_DPP_AM_GLOBAL_LIF_NOF_ENTRIES_PER_BANK;
                tagBitMapExtras.tag_length = sizeof(uint8);
                extraPtr = &tagBitMapExtras;
                if (res_id == dpp_am_res_global_outlif) {
                    desc_str = "Egress global lif allocation";
                } else {
                    desc_str = "Sync global lif allocation";
                }
            }
            else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_global_inlif:
            if (SOC_IS_JERICHO(unit) && SOC_DPP_PP_ENABLE(unit)){
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = 0;
                p_info.count = _BCM_DPP_AM_NOF_GLOBAL_IN_LIFS(unit);
                desc_str = "Ingress global LIF allocation";
            }
            else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_obs_inlif:
            if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_PP_ENABLE(unit)){
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = _BCM_DPP_AM_INGRESS_LIF_BANK_LOW_ID(core_id);
                p_info.count = _BCM_DPP_AM_INGRESS_LIF_NOF_ENTRIES_PER_BANK;
                desc_str = "Ingress LIF bank allocation";
            }
            else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_obs_eg_encap:
            if (SOC_IS_ARADPLUS_AND_BELOW(unit) && SOC_DPP_PP_ENABLE(unit)){
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.start = _BCM_DPP_AM_EGRESS_ENCAP_BANK_LOW_ID(core_id);
                p_info.count = _BCM_DPP_AM_EGRESS_ENCAP_NOF_ENTRIES_PER_BANK;
                desc_str = "Egress encapsulation bank allocation";
            }
            else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_vsq_src_port:
            if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.count = SOC_DPP_DEFS_GET(unit, nof_vsq_e) - 1;
                p_info.start = 1;
                desc_str = "source port VSQ ID";
                p_info.max_elements_per_allocation = 1;
            } else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_vsq_pg:
            if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.count = SOC_DPP_DEFS_GET(unit, nof_vsq_f);
                p_info.start = 0;
                desc_str = "VSQ PG ID";
                p_info.max_elements_per_allocation = BCM_COSQ_VSQ_NOF_PG;
            } else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_map_encap_intpri_color:
            if (!SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
                p_info.count = _BCM_DPP_AM_EGRESS_MAP_ENCAP_INTPRI_COLOR_NOF_ENTRIES; 
                p_info.start = 0;
                desc_str = "Profiles for QOS";
                p_info.max_elements_per_allocation = 1;
            } else {
                BCM_EXIT;
            }
            break;
        case dpp_am_res_field_entry_id:
            bcm_dpp_field_get_dev_info(unit, &devInfo);
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.count = devInfo->stage[0].maxEntriesInternalTcam 
                + devInfo->stage[0].maxEntriesExternalTcam;
            p_info.start = 0;
            desc_str = "Field HW entry ID";
            p_info.max_elements_per_allocation = 1;
            break;
        case dpp_am_res_field_direct_extraction_entry_id:
            bcm_dpp_field_get_dev_info(unit, &devInfo);
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.count = devInfo->stage[0].maxEntriesDe;
            p_info.start = 0;
            desc_str = "Field HW dir ext ent ID";
            p_info.max_elements_per_allocation = 1;
            break;
        case dpp_am_res_ipsec_sa_id:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.count = BCM_IPSEC_MAX_NUM_OF_SA;
            p_info.start = 0;
            desc_str = "IPSEC SA ID";
            p_info.max_elements_per_allocation = 1;
            break;
        case dpp_am_res_ipsec_tunnel_id:
            p_info.res_type = SW_STATE_RES_ALLOCATOR_BITMAP;
            p_info.count = BCM_IPSEC_MAX_NUM_OF_IPSEC_TUNNEL;
            p_info.start = 0;
            desc_str = "IPSEC TUNNEL ID";
            p_info.max_elements_per_allocation = 1;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Resource id %d not supported"), res_id));
    }


    rv = dpp_am_res_set_pool(unit,
                            p_info.core_id,
                            p_info.res_id,
                            p_info.res_type,
                            p_info.start,
                            p_info.count,
                            extraPtr,
                            desc_str);

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



int
bcm_dpp_am_l2_vsi_vlan_alloc(
    int unit,
    uint32 flags,
    bcm_vlan_t *vsi)
{
    int rv = BCM_E_NONE;
    int elem = *vsi;
    int count;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_vsi_vlan,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    if(!SOC_WARM_BOOT(unit)) {
        /* Update the allocated vlan count */
        rv = VLAN_ACCESS.vlan_info.count.get(unit, &count);
        BCMDNX_IF_ERR_EXIT(rv);
        count++;
        rv = VLAN_ACCESS.vlan_info.count.set(unit, count);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    *vsi = elem;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_vsi_vlan_dealloc(
    int unit,
    uint32 flags,
    bcm_vlan_t vsi)
{
    int rv = BCM_E_NONE;
    int elem = vsi;
    int count;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_vsi_vlan,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update the allocated vlan count */
    VLAN_ACCESS.vlan_info.count.get(unit, &count);
    count--;
    VLAN_ACCESS.vlan_info.count.set(unit, count);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_vlan_vsi_alloc(
    int unit,
    bcm_vlan_t vid,
    SOC_PPC_VSI_ID *vsi,
    SOC_PPC_LIF_ID *lif)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vlan;

    BCMDNX_INIT_FUNC_DEFS;

    *vsi = vid;

    /* mark that vsi(=vlan) is allocated */
    vlan = *vsi;
    rv = bcm_dpp_am_l2_vsi_vlan_alloc(unit, SW_STATE_RES_ALLOC_WITH_ID, &vlan);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_vlan_vsi_dealloc(
    int unit,
    bcm_vlan_t vid,
    SOC_PPC_VSI_ID vsi,
    SOC_PPC_LIF_ID lif)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_l2_vsi_vlan_dealloc(unit, SW_STATE_RES_ALLOC_WITH_ID, vsi);
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* VSI resources for L2 VPN usage*/

int
bcm_dpp_am_l2_vpn_vsi_alloc(
        int unit,
        uint32 flags,
        bcm_vlan_t *vsi)
{
    int rv = BCM_E_NONE;
    int elem = *vsi;

    BCMDNX_INIT_FUNC_DEFS;

    if ((flags & SW_STATE_RES_ALLOC_WITH_ID) &&
        *vsi <= BCM_VLAN_MAX) {
        rv = bcm_dpp_am_l2_vsi_vlan_alloc(unit, SW_STATE_RES_ALLOC_WITH_ID, vsi);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_vsi_mstp,flags,1,&elem);
        BCMDNX_IF_ERR_EXIT(rv);

    }

    *vsi = elem;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_vpn_vsi_dealloc(
        int unit,
        bcm_vlan_t vsi)
{
    int rv = BCM_E_NONE;
    int elem = vsi;
    BCMDNX_INIT_FUNC_DEFS;

    if (vsi <= BCM_VLAN_MAX) {
        rv = bcm_dpp_am_l2_vsi_vlan_dealloc(unit, SW_STATE_RES_ALLOC_WITH_ID, vsi);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_vsi_mstp,1,elem);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_l2_vpn_vsi_is_alloced(
    int unit,
    bcm_vlan_t vsi)
{
    int elem = vsi;
    BCMDNX_INIT_FUNC_DEFS;

    if (vsi <= BCM_VLAN_MAX) {
        BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_vsi_vlan,1,elem));
    }
    else {
        BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_vsi_mstp,1,elem));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* failover */

/*
 * Function:
 *      _bcm_dpp_am_failover_type_to_pool_id
 * Purpose:
 *      Internal alloc manager function to convert a failover type into an alloc
 *      manager buffer pool ID.
 * Parameters:
 *      unit    - Device Number
 *      failover_type - The failover type.
 *      failover_pool_id - The returned failover alloc manager buffer pool ID
 * Returns:
 *      BCM_E_PARAM - In case of an unsupported failover type
 */
int
_bcm_dpp_am_failover_type_to_pool_id(
    int unit,
    int32 failover_type,
    _dpp_am_res_t *failover_pool_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    /* Retrieve the failover alloc manager buffer pool ID according to the
       supplied failover type */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* All devices up till Arad+ use a common table and a common buffer pool */
        *failover_pool_id = dpp_am_res_failover_common_id;
    } else {
        /* In Jericho and above, the failover ID is per failover type */
        switch (failover_type) {
        case DPP_FAILOVER_TYPE_FEC:
            *failover_pool_id = dpp_am_res_failover_fec_id;
            break;
        case DPP_FAILOVER_TYPE_INGRESS:
            *failover_pool_id = dpp_am_res_failover_ingress_id;
            break;
        case DPP_FAILOVER_TYPE_ENCAP:
            *failover_pool_id = dpp_am_res_failover_egress_id;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failover type %d not supported"), failover_type));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dpp_am_failover_alloc
 * Purpose:
 *      Allocates a failover object using the alloc manager mechanism
 * Parameters:
 *      unit    - Device Number
 *      flags   - alloc manager control flags
 *      failover_type - The failover type.
 *      nof_alloc_ids - NOF consequent IDs to allocate
 *      failover_id - The returned allocated failover ID. Also used to supply
 *                the failover ID in case of a WITH_ID operation.
 * Returns:
 *      BCM_E_PARAM - In case of an unsupported failover type
 *      BCM_E_NONE - In case there are no allocation mechanism failures
 */
int
bcm_dpp_am_failover_alloc(
    int unit,
    uint32 flags,
    int32 failover_type,
    int32 nof_alloc_ids,
    bcm_failover_t *failover_id)
{
    int rv = BCM_E_NONE;
    int elem = *failover_id;
    _dpp_am_res_t alloc_buffer_pool_id;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get an alloc manager buffer pool ID from the supplied failover type */
    rv = _bcm_dpp_am_failover_type_to_pool_id(unit, failover_type, &alloc_buffer_pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Allocation of a pair of failover IDs for Egress protection should be aligned so that the first ID will be even */
    if (nof_alloc_ids == 2) {
        flags |= SW_STATE_RES_ALLOC_ALIGN_ZERO;
        rv = dpp_am_res_alloc_align(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, alloc_buffer_pool_id, flags, 2, 0, nof_alloc_ids, &elem);
    } else {
        /* Call the general alloc manager function to perform the allocation */
        rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, alloc_buffer_pool_id, flags, nof_alloc_ids, &elem);
    }
    BCMDNX_IF_ERR_EXIT(rv);

    /* Return the allocated Failover ID */
    *failover_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dpp_am_failover_dealloc
 * Purpose:
 *      De-allocates a failover object from the alloc manager mechanism
 * Parameters:
 *      unit    - Device Number
 *      failover_type - The failover type.
 *      nof_dalloc_ids - NOF consequent IDs to de-allocate
 *      failover_id - The failover ID to de-allocate.
 * Returns:
 *      BCM_E_PARAM - In case of an unsupported failover type
 *      BCM_E_NONE - In case there are no allocation mechanism failures
 */
int
bcm_dpp_am_failover_dealloc(
    int unit,
    int32 failover_type,
    int32 nof_dealloc_ids,
    bcm_failover_t failover_id)
{
    int rv = BCM_E_NONE;
    int elem = failover_id;
    _dpp_am_res_t alloc_buffer_pool_id;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get an alloc manager buffer pool ID from the supplied failover type */
    rv = _bcm_dpp_am_failover_type_to_pool_id(unit, failover_type, &alloc_buffer_pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Call the general alloc manager function to perform the de-allocation */
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, alloc_buffer_pool_id, nof_dealloc_ids, elem);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_dpp_am_failover_is_alloced
 * Purpose:
 *      Checks whether a failover object is allocated
 * Parameters:
 *      unit    - Device Number
 *      failover_type - The failover type.
 *      failover_id - The failover ID to check for allocation.
 * Returns:
 *      BCM_E_PARAM - In case of an unsupported failover type
 *      BCM_E_NONE - In case there are no allocation mechanism failures
 */
int
bcm_dpp_am_failover_is_alloced(
    int unit,
    int32 failover_type,
    bcm_failover_t failover_id)
{
    int elem = failover_id;
    _dpp_am_res_t alloc_buffer_pool_id;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get an alloc manager buffer pool ID from the supplied failover type */
    rv = _bcm_dpp_am_failover_type_to_pool_id(unit, failover_type, &alloc_buffer_pool_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, alloc_buffer_pool_id, 1, elem));
exit:
    BCMDNX_FUNC_RETURN;
}



/* ECMP*/


int
bcm_dpp_am_ecmp_alloc(
    int unit,
    uint32 flags,
    bcm_ecmp_t *ecmp_id)
{
    int rv = BCM_E_NONE;
    int elem = *ecmp_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_ecmp_id,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *ecmp_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ecmp_dealloc(
    int unit,
    bcm_ecmp_t ecmp_id)
{
    int rv = BCM_E_NONE;
    int elem = ecmp_id;

    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_ecmp_id,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ecmp_is_alloced(
    int unit,
    bcm_ecmp_t ecmp_id)
{
    int elem = ecmp_id;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_ecmp_id,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


/* RP */


int
bcm_dpp_am_rp_alloc(
    int unit,
    uint32 flags,
    int *rp_id)
{
    int rv = BCM_E_NONE;
    int elem = *rp_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_rp_id,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *rp_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_rp_dealloc(
    int unit,
    int rp_id)
{
    int rv = BCM_E_NONE;
    int elem = rp_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_rp_id,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_rp_is_alloced(
    int unit,
    int rp_id)
{
    int elem = rp_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_rp_id,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


/* PWE: EEP + LIF */
int
bcm_dpp_am_l2_vpn_pwe_alloc(
        int unit,
        uint32 lif_term_types,
        uint32 flags,
        int count,
        SOC_PPC_LIF_ID *lif)
{
    int rv = BCM_E_NONE;
    int elem;
    bcm_dpp_am_sync_lif_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(lif);
    /* alloc  > 4k lif */
    elem = *lif;


    sal_memset(&allocation_info,0,sizeof(allocation_info));
    allocation_info.egress_lif.application_type = bcm_dpp_am_egress_encap_app_pwe;
    allocation_info.egress_lif.pool_id = dpp_am_res_eep_mpls_tunnel;
    allocation_info.ingress_lif.pool_id = dpp_am_res_lif_pwe;
    allocation_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS;

    /* types parameter is only needed for ARAD information */
    if (lif_term_types == _BCM_DPP_AM_L2_PWE_TERM_DEFAULT) { /* Default MPLS termination */
        allocation_info.ingress_lif.application_type = bcm_dpp_am_ingress_lif_app_mpls_term;            
    } else if (lif_term_types == _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1) { /* PWE termination indexed 1 */
        allocation_info.ingress_lif.application_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1;        
    } else if (lif_term_types == _BCM_DPP_AM_L2_PWE_TERM_INDEXED_2) { /* PWE termination indexed 2 */
        allocation_info.ingress_lif.application_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2;        
    } else if (lif_term_types == _BCM_DPP_AM_L2_PWE_TERM_INDEXED_3) { /* PWE termination indexed 3 */
        allocation_info.ingress_lif.application_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3;        
    } else if (lif_term_types == _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1_3) {
        allocation_info.ingress_lif.application_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3;        
    } else if (lif_term_types == _BCM_DPP_AM_L2_PWE_TERM_INDEXED_1_2) {
        allocation_info.ingress_lif.application_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2;        
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("types %d not exist"), lif_term_types));
    }
    
    rv = _bcm_dpp_am_sync_lif_alloc_align(unit,&allocation_info, flags,2,0,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);
    *lif = elem;
    
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_vpn_pwe_lif_dealloc(
        int unit,
        SOC_PPC_LIF_ID lif)
{
    int rv = BCM_E_NONE;
    int elem = lif;
    bcm_dpp_am_sync_lif_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));
    allocation_info.egress_lif.application_type = bcm_dpp_am_egress_encap_app_pwe;
    allocation_info.egress_lif.pool_id = dpp_am_res_eep_mpls_tunnel;
    allocation_info.ingress_lif.pool_id = dpp_am_res_lif_pwe;
    allocation_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS;
    rv = _bcm_dpp_am_sync_lif_dealloc(unit,&allocation_info,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_vpn_pwe_eep_dealloc(
        int unit,
        uint32 eep)
{
    int rv = BCM_E_NONE;
    int elem = eep;
    bcm_dpp_am_sync_lif_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;
    
    sal_memset(&allocation_info,0,sizeof(allocation_info));
    allocation_info.egress_lif.application_type = bcm_dpp_am_egress_encap_app_pwe;
    allocation_info.egress_lif.pool_id = dpp_am_res_eep_mpls_tunnel;
    allocation_info.ingress_lif.pool_id = dpp_am_res_lif_pwe;
    allocation_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_EGRESS;
    rv = _bcm_dpp_am_sync_lif_dealloc(unit,&allocation_info,2,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* L3 */

int
bcm_dpp_am_l3_lif_type_to_app_type(uint32 type, bcm_dpp_am_ingress_lif_app_t *app_type){

    if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM) { /* MPLS termination */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_term;
    } else if (type == _BCM_DPP_AM_L3_LIF_IP_TERM) { /* IP termination*/
        *app_type = bcm_dpp_am_ingress_lif_app_ip_term;
    } else if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1) { /* MPLS termination indexed 1 */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1;        
    } else if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_2) { /* MPLS termination indexed 2 */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2;        
    } else if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_3) { /* MPLS termination indexed 3 */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3;        
    } else if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM_FRR) { /* MPLS FRR termination */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_frr_term;        
    } else if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_2) { /* MPLS termination indexed 1,2 */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2;        
    } else if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM_INDEXED_1_3) { /* MPLS termination indexed 1,3 */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3;        
    } else if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM_EXPLICIT_NULL) { /* MPLS termination Explicit NULL */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_term_explicit_null;        
    }else if (type == _BCM_DPP_AM_L3_LIF_MPLS_TERM_DUMMY) { /* Dummy MPLS termination - mLDP */
        *app_type = bcm_dpp_am_ingress_lif_app_mpls_term_mldp;
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
bcm_dpp_am_l3_lif_alloc(
        int unit,
        uint32 types,
        uint32 flags,
        int *lif)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;
    

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    /* types parameter is only needed for ARAD information */
    rv = bcm_dpp_am_l3_lif_type_to_app_type(types, &allocation_info.application_type);
    BCMDNX_IF_ERR_EXIT(rv);

    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    
    rv = _bcm_dpp_am_ingress_lif_alloc(unit,&allocation_info,flags,1,lif);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_l3_lif_is_allocated(
        int unit,
        uint32 types,
        uint32 flags,
        int lif)
{

    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;
    
    sal_memset(&allocation_info,0,sizeof(allocation_info));

    /* types parameter is only needed for ARAD information */
    BCMDNX_IF_ERR_EXIT(bcm_dpp_am_l3_lif_type_to_app_type(types, &allocation_info.application_type));

    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_ingress_lif_is_allocated(unit,&allocation_info,1,lif));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l3_lif_dealloc(
        int unit,
        int lif)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    rv = _bcm_dpp_am_ingress_lif_dealloc(unit,&allocation_info,1,lif);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l3_eep_alloc(
        int unit,
        uint32 types,
        uint32 flags,
        int *eep)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_global;
     if (types == _BCM_DPP_AM_L3_EEP_TYPE_ROO_LINKER_LAYER) {
         allocation_info.application_type = bcm_dpp_am_egress_encap_app_linker_layer;
     }

    rv = _bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,eep);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l3_eep_is_alloced(
        int unit,
        int eep)
{
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_global;
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_is_allocated(unit,&allocation_info,2,eep));
    
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_l3_eep_dealloc(
        int unit,
        int eep)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_global;
    rv = _bcm_dpp_am_egress_encap_dealloc(unit,&allocation_info,2,eep);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_l3_lif_ip_tnl_alloc(
        int unit,
        uint32 flags,
        SOC_PPC_LIF_ID *lif)
{
    int rv = BCM_E_NONE;
    int elem;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    if(lif) {

        /* alloc  > 4k lif */
        elem = *lif;

        sal_memset(&allocation_info,0,sizeof(allocation_info));
        allocation_info.pool_id = dpp_am_res_lif_ip_tnl;
        rv = _bcm_dpp_am_ingress_lif_alloc(unit,&allocation_info,flags,1,&elem);
        BCMDNX_IF_ERR_EXIT(rv);
        *lif = elem;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l3_lif_ip_tnl_dealloc(
        int unit,
        SOC_PPC_LIF_ID lif)
{
    int rv = BCM_E_NONE;
    int elem = lif;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));
    allocation_info.pool_id = dpp_am_res_lif_ip_tnl;
    rv = _bcm_dpp_am_ingress_lif_dealloc(unit,&allocation_info,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_encap_eep_alloc(
        int unit,
        uint32 flags,
        int *eep)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;



    sal_memset(&allocation_info,0,sizeof(allocation_info));
    allocation_info.pool_id = dpp_am_res_eep_ip_tnl; /*use ip tunnel pool,consider using own pool*/
    /*rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eep_ip_tnl,flags,1,eep);*/

    allocation_info.application_type = bcm_dpp_am_egress_encap_app_l2_encap;

    rv = _bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,eep);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_encap_eep_dealloc(
        int unit,
        int eep)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));
    allocation_info.pool_id = dpp_am_res_eep_ip_tnl; /*use ip tunnel pool,consider using own pool*/
 
    /* rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eep_ip_tnl,1,eep); */
    rv = _bcm_dpp_am_egress_encap_dealloc(unit,&allocation_info,2,eep);

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_encap_eep_is_alloced(
        int unit,
        int eep)
{
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_ip_tnl;
    /* BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eep_ip_tnl,1,eep));*/
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_is_allocated(unit,&allocation_info,2,eep));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ip_tunnel_eep_alloc(
        int unit,
        uint32 types, 
        uint32 flags,
        int *eep)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    uint32 outlif=*eep;
    BCMDNX_INIT_FUNC_DEFS;

    /* if ipv6 alloc */
    if(flags & (BCM_DPP_AM_FLAG_ALLOC_IPV6)) {
        rv = bcm_dpp_am_ipv6_tunnel_alloc(unit,flags &(~BCM_DPP_AM_FLAG_ALLOC_IPV6), &outlif);
        *eep = outlif;
        BCM_EXIT;
    }

    sal_memset(&allocation_info,0,sizeof(allocation_info));
    allocation_info.pool_id = dpp_am_res_eep_ip_tnl;
    /*rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eep_ip_tnl,flags,1,eep);*/

    if (types == _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_ROO) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_ip_tunnel_roo;
    }

    rv = _bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,eep);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ip_tunnel_eep_is_alloced(
        int unit,
        uint32 flags,
        int eep)
{
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    /* if ipv6 alloc */
    if(flags & (BCM_DPP_AM_FLAG_ALLOC_IPV6)) {
        return bcm_dpp_am_ipv6_tunnel_is_alloced(unit,eep);
    }

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_ip_tnl;
    /* BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eep_ip_tnl,1,eep));*/
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_is_allocated(unit,&allocation_info,2,eep));

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_ip_tunnel_eep_dealloc(
        int unit,
        uint32 flags,
        int eep)
{
    int rv = BCM_E_NONE;


    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;

    /* if ipv6 alloc */
    if(flags & (BCM_DPP_AM_FLAG_ALLOC_IPV6)) {
        return bcm_dpp_am_ipv6_tunnel_dealloc(unit,flags &(~BCM_DPP_AM_FLAG_ALLOC_IPV6), eep);
    }

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_ip_tnl;
    /* rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eep_ip_tnl,1,eep); */
    rv = _bcm_dpp_am_egress_encap_dealloc(unit,&allocation_info,2,eep);

    return rv;
}

int
bcm_dpp_am_mpls_eep_alloc(
        int unit,
        uint32 type,
        uint32 flags,
        int count,
        int *eep)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    bcm_dpp_am_sync_lif_alloc_info_t sync_allocation_info;

    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));
    sal_memset(&sync_allocation_info,0,sizeof(sync_allocation_info));

    /* type parameter is only needed for ARAD Egress encapsulation information */
    /* per application access_phase is different */
    if (type == _BCM_DPP_AM_MPLS_EEP_TYPE_PWE) {
        sync_allocation_info.ingress_lif.pool_id = dpp_am_res_lif_pwe;
        sync_allocation_info.egress_lif.application_type = bcm_dpp_am_egress_encap_app_pwe;
        sync_allocation_info.egress_lif.pool_id = dpp_am_res_eep_mpls_tunnel;
        sync_allocation_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_EGRESS;
        rv = _bcm_dpp_am_sync_lif_alloc_align(unit,&sync_allocation_info, flags,2,0,2,eep); 
    } else if (type == _BCM_DPP_AM_MPLS_EEP_TYPE_SIMPLE_MPLS_TUNNEL) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_mpls_tunnel;
        allocation_info.pool_id = dpp_am_res_eep_mpls_tunnel;
        rv = _bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,eep);
    } else if (type == _BCM_DPP_AM_MPLS_EEP_TYPE_SECOND_MPLS_TUNNEL) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_second_mpls_tunnel;
        allocation_info.pool_id = dpp_am_res_eep_mpls_tunnel;
        rv = _bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,eep);
    } else if (type == _BCM_DPP_AM_MPLS_EEP_TYPE_PWE_EGRESS_ONLY) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_second_mpls_tunnel;
        allocation_info.pool_id = dpp_am_res_eep_mpls_tunnel;
        rv = _bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,eep);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("type %d not exist"), type));
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_mpls_eep_is_alloced(
        int unit,
        int eep)
{
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_is_allocated(unit,&allocation_info,2,eep));

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_mpls_eep_dealloc(
        int unit,
        int eep)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_mpls_tunnel;
    rv = _bcm_dpp_am_egress_encap_dealloc(unit,&allocation_info,2,eep);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_ip_tunnel_glbl_src_ip_alloc(
        int unit,
        uint32 flags,
        int *idx)
{
    int rv = BCM_E_NONE;
    int elem = *idx;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_src_ip,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *idx = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ip_tunnel_glbl_src_ip_is_alloced(
        int unit,
        int idx)
{
    int elem = idx;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_src_ip,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_ip_tunnel_glbl_src_ip_dealloc(
        int unit,
        int idx)
{
    int rv = BCM_E_NONE;
    int elem = idx;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_src_ip,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ip_tunnel_glbl_ttl_alloc(
        int unit,
        uint32 flags,
        int *idx)
{
    int rv = BCM_E_NONE;
    int elem = *idx;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_ttl,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);


    *idx = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ip_tunnel_glbl_ttl_is_alloced(
        int unit,
        int idx)
{
    int elem = idx;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_ttl,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_ip_tunnel_glbl_ttl_dealloc(
        int unit,
        int idx)
{
    int rv = BCM_E_NONE;
    int elem = idx;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_ttl,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ip_tunnel_glbl_tos_alloc(
        int unit,
        uint32 flags,
        int *idx)
{
    int rv = BCM_E_NONE;
    int elem = *idx;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_tos,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *idx = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ip_tunnel_glbl_tos_is_alloced(
        int unit,
        int idx)
{
    int elem = idx;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_tos,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_ip_tunnel_glbl_tos_dealloc(
        int unit,
        int idx)
{
    int rv = BCM_E_NONE;
    int elem = idx;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_glbl_tos,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Get a simple indexing for egress traps */
int
_bcm_dpp_am_eg_trap_index_get(
        int unit,
        SOC_PPC_TRAP_EG_TYPE trap_id, /* from the enum */
        uint32 *trap_index) /* 0...NOF_TRAPS */
{
    int rv=0;

    BCMDNX_INIT_FUNC_DEFS;

    *trap_index = 0;


    if (trap_id <= (1 << SOC_PPC_NOF_TRAP_EG_TYPES_PB)) {
        /* Soc_petra-B style, bitmap */
        *trap_index = soc_sand_log2_round_down(trap_id);
    }
    else if (trap_id <= SOC_PPC_TRAP_EG_TYPE_HIGHEST_ARAD) {
        /* Arad style  */
        *trap_index = SOC_PPC_NOF_TRAP_EG_TYPES_PB + (trap_id >> 16);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("egress trap id %d not exist"), trap_id));
    }

    return rv;

exit:
    BCMDNX_FUNC_RETURN;

}

/*****************************************************************************
* Function:  _bcm_dpp_am_trap_check_resource
* Purpose:   check if trap defined by trap_id_sw was already configured in
* an alternative configuration way, thus preventing multiple configurations
* Params:
* unit (IN)      - Device Number
* trap_id_sw (IN)   - Trap software id
* Return:    (int)
 */
STATIC int
_bcm_dpp_am_trap_check_resource(
        int unit,
        int trap_id_sw)
{
    switch (trap_id_sw) 
    {
    case SOC_PPC_TRAP_CODE_MPLS_TTL0:
        /* If tunnel termination trap or forwarding trap was configured independently, return error */
        if (_bcm_dpp_am_trap_is_alloced(unit, SOC_PPC_TRAP_CODE_MPLS_TUNNEL_TERMINATION_TTL0) == BCM_E_EXISTS ||
            _bcm_dpp_am_trap_is_alloced(unit, SOC_PPC_TRAP_CODE_MPLS_FORWARDING_TTL0) == BCM_E_EXISTS)
        {
            return BCM_E_RESOURCE;
        }
        break;
    case SOC_PPC_TRAP_CODE_MPLS_TTL1:
        /* If tunnel termination trap or forwarding trap was configured independently, return error */
        if (_bcm_dpp_am_trap_is_alloced(unit, SOC_PPC_TRAP_CODE_MPLS_TUNNEL_TERMINATION_TTL1) == BCM_E_EXISTS ||
            _bcm_dpp_am_trap_is_alloced(unit, SOC_PPC_TRAP_CODE_MPLS_FORWARDING_TTL1) == BCM_E_EXISTS)
        {
            return BCM_E_RESOURCE;
        }
        break;
    case SOC_PPC_TRAP_CODE_MPLS_TUNNEL_TERMINATION_TTL0:
    case SOC_PPC_TRAP_CODE_MPLS_FORWARDING_TTL0:
        /* If MPLS ttl0 trap was configured, return error */
        if(_bcm_dpp_am_trap_is_alloced(unit, SOC_PPC_TRAP_CODE_MPLS_TTL0) == BCM_E_EXISTS) 
        {
            return BCM_E_RESOURCE;
        }
        break;
    case SOC_PPC_TRAP_CODE_MPLS_TUNNEL_TERMINATION_TTL1:
    case SOC_PPC_TRAP_CODE_MPLS_FORWARDING_TTL1:
        /* If MPLS ttl1 trap was configured, return error */
        if(_bcm_dpp_am_trap_is_alloced(unit, SOC_PPC_TRAP_CODE_MPLS_TTL1) == BCM_E_EXISTS) 
        {
            return BCM_E_RESOURCE;
        }
        break;
    default: 
        break;
    }

    return BCM_E_NONE;
}

/*****************************************************************************
* Function:  _bcm_dpp_am_trap_alloc
* Purpose:   allocate trap bit with id = trap_id_sw, according to flags
* Params:
* unit (IN)         - Device Number
* flags (IN)        - With id
* trap_id_sw (IN)   - Trap software id
* element  (OUT)    - allocated element (trap_id_sw or trap_index)
* Return:    (int)
 */
int
_bcm_dpp_am_trap_alloc(
        int unit,
        int flags,
        int trap_id_sw,
        int *elem)
{
    int rv=0;
    bcm_dpp_user_defined_traps_t  data={{0}};
    int template=0;    
    int is_allocated=0;
    uint32 trap_index;

    BCMDNX_INIT_FUNC_DEFS;
    
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&(data.trap));
    
    *elem = trap_id_sw;

    /* If in the range of PPD user defined*/
    if ( (SOC_PPC_TRAP_CODE_USER_DEFINED_0 <= trap_id_sw) && (trap_id_sw <= _BCM_LAST_USER_DEFINE) ) { 
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, SOC_PPC_TRAP_CODE_USER_DEFINED alloc , trap id %d\n"), unit,trap_id_sw));
        /* In this case we have to options:
           1. The user transfered the BCM_USER_DEFINED_TRAP with BCM_RX_TRAP_WITH_ID
           2. The user transfered an exposed bcm trap that uses a user-defined trap
        */
        if( trap_id_sw != SOC_PPC_TRAP_CODE_USER_DEFINED_0 )
        {
            flags |= SW_STATE_RES_ALLOC_WITH_ID;
        }
        rv = _bcm_dpp_am_trap_user_define_alloc(unit, flags, elem);
        BCMDNX_IF_ERR_EXIT(rv);
        
        /*Sync rx.c traps allocations with port.c traps allocations (templates) for USER_DEFINED traps*/
        data.ud_unique = *elem;
        template = *elem-SOC_PPC_TRAP_CODE_USER_DEFINED_0; 
        rv = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_user_defined_traps, SHR_TEMPLATE_MANAGE_SET_WITH_ID, &data, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(rv);

    } else if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id_sw)) { /* If virtual bit is marked */
        /*Clears virtual bit before allocation*/
        trap_id_sw = _BCM_TRAP_ID_TO_VIRTUAL(trap_id_sw);
        *elem = trap_id_sw;
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, VIRTUAL alloc , trap id %d\n"), unit,trap_id_sw));
        rv = _bcm_dpp_am_trap_virtual_alloc(unit, flags, elem);
        BCMDNX_IF_ERR_EXIT(rv);        
        
    } else if (_BCM_TRAP_ID_IS_EGRESS(trap_id_sw)) { /* egress create, with id*/
        
        trap_id_sw = _bcm_dpp_rx_trap_id_to_egress(trap_id_sw);
        /* Translate the indexing between enum and int for egress traps */
        rv = _bcm_dpp_am_eg_trap_index_get(unit, 
                                           trap_id_sw, /* from the enum */
                                           &trap_index); /* 0...NOF_TRAPS */
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, Egress alloc , egress trap-id %d ,trap index %d\n"), 
                              unit, trap_id_sw, trap_index));
        *elem = trap_index;
        rv = _bcm_dpp_am_trap_egress_alloc(unit,SW_STATE_RES_ALLOC_WITH_ID,elem);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else if (_BCM_TRAP_ID_IS_ETPP(trap_id_sw))  /* etpp create */
    {
        trap_id_sw =  _bcm_dpp_rx_trap_id_to_etpp(trap_id_sw);
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, Single Instance alloc (ETPP) , trap id %d\n"), unit,trap_id_sw));
        *elem = trap_id_sw;
        rv = _bcm_dpp_am_trap_single_instance_etpp_alloc(unit, SW_STATE_RES_ALLOC_WITH_ID, elem);
        
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    { /* Otherwise, use single instance pool (ingress) */        
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, Single Instance alloc , trap id %d\n"), unit,trap_id_sw));
        *elem = trap_id_sw;

        rv = _bcm_dpp_am_trap_check_resource(unit, trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);

        /* If resource is free, allocate trap */ 
        rv = _bcm_dpp_am_trap_single_instance_alloc(unit, SW_STATE_RES_ALLOC_WITH_ID, elem);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
    return rv;

exit:
    BCMDNX_FUNC_RETURN;

}

int
_bcm_dpp_am_trap_virtual_alloc(int unit,
                               int flags,
                               int *elem)
{
    int rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Virtual alloc, trap id %d\n"), unit, *elem));

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_virtual, flags, 1, elem);
    
    BCMDNX_IF_ERR_EXIT(rv);
    
 exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_trap_egress_alloc(int unit,
                              int flags,
                              int *elem)
{
    int rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Egress alloc, trap id %d\n"), unit, *elem));

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_egress, flags, 1, elem);
    
    BCMDNX_IF_ERR_EXIT(rv);
    
 exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_trap_user_define_alloc(int unit,
                                   int flags,
                                   int *elem)
{
    int rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, User Define alloc, trap id %d\n"), unit, *elem));

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_user_define, flags, 1, elem);
    
    BCMDNX_IF_ERR_EXIT(rv);
    
 exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_trap_single_instance_alloc(
                               int unit,
                               int flags,
                               int *elem)
{
    int rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Single Instance alloc, trap id %d\n"), unit,*elem));

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_single_instance,flags,1,elem);
    
    BCMDNX_IF_ERR_EXIT(rv);
    
 exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_trap_single_instance_etpp_alloc(
                               int unit,
                               int flags,
                               int *elem)
{
    int rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Single Instance alloc (ETPP), trap id %d\n"), unit,*elem));

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_etpp,flags,1,elem);
    
    BCMDNX_IF_ERR_EXIT(rv);
    
 exit:
    BCMDNX_FUNC_RETURN;
}

/*****************************************************************************
* Function:  _bcm_dpp_am_trap_dealloc
* Purpose:   deallocate trap bit with id = trap_id_sw, according to flags
* Params:
* unit (IN)         - Device Number
* flags (IN)        - With id
* trap_id_sw (IN)   - Trap software id
* element (IN)          - allocated element (trap_id_sw or trap_index)
* Return:    (int)
 */
int
_bcm_dpp_am_trap_dealloc(
        int unit,
        int flags,
        int trap_id_sw, 
        int elem)
{
    int rv = 0;
    int template;    
    int is_last;
    uint32 trap_index;

    BCMDNX_INIT_FUNC_DEFS;
    
    LOG_DEBUG(BSL_LS_BCM_COMMON,
              (BSL_META_U(unit,
                          "Unit:%d, Starting resources dealloc, trap id is = %d\n"), unit,trap_id_sw));

    /* If in the range of PPD user defined*/
    if ( (SOC_PPC_TRAP_CODE_USER_DEFINED_0 <= trap_id_sw) && (trap_id_sw <= _BCM_LAST_USER_DEFINE) ) { 
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, SOC_PPC_TRAP_CODE_USER_DEFINED dealloc, trap id is = %d\n"), unit,trap_id_sw));
        rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_user_define,SW_STATE_RES_ALLOC_WITH_ID,trap_id_sw);

        BCMDNX_IF_ERR_EXIT(rv);

        /*Sync rx.c traps allocations with port.c traps allocations (templates) for USER_DEFINED traps*/
        template = trap_id_sw-SOC_PPC_TRAP_CODE_USER_DEFINED_0; /*template counting is from 0-59*/
        rv = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_user_defined_traps, template, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);

    } else if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id_sw)) { /* If virtual bit is marked */
       /*Clears virtual bit before allocation*/
        trap_id_sw = _BCM_TRAP_ID_TO_VIRTUAL(trap_id_sw);
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, VIRTUAL dealloc, trap id is = %d\n"), unit,trap_id_sw));
        rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_virtual,SW_STATE_RES_ALLOC_WITH_ID,trap_id_sw);
        BCMDNX_IF_ERR_EXIT(rv);

    } else if (_BCM_TRAP_ID_IS_EGRESS(trap_id_sw)) { /* egress create, with id*/
        trap_id_sw = _bcm_dpp_rx_trap_id_to_egress(trap_id_sw);
        /* Translate the indexing between enum and int for egress traps */
        rv = _bcm_dpp_am_eg_trap_index_get(unit, 
                                           trap_id_sw, /* from the enum */
                                           &trap_index); /* 0...NOF_TRAPS */
        BCMDNX_IF_ERR_EXIT(rv);
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, Egress dealloc , egress trap-id %d ,trap index %d\n"), 
                              unit, trap_id_sw, trap_index));
        rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_egress,SW_STATE_RES_ALLOC_WITH_ID,trap_index);    
        BCMDNX_IF_ERR_EXIT(rv);

    }
    else if (_BCM_TRAP_ID_IS_ETPP(trap_id_sw))  
    {
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, Single Instance dealloc (ETPP), trap id is = %d\n"), unit,trap_id_sw));
        trap_id_sw = _bcm_dpp_rx_trap_id_to_etpp(trap_id_sw);
        rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_etpp,SW_STATE_RES_ALLOC_WITH_ID,trap_id_sw);    
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else { /* Otherwise, use single instance pool (ingress) */
        LOG_DEBUG(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "Unit:%d, Single Instance dealloc, trap id is = %d\n"), unit,trap_id_sw));
        rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_single_instance,SW_STATE_RES_ALLOC_WITH_ID,trap_id_sw);    
        BCMDNX_IF_ERR_EXIT(rv);

    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



int
_bcm_dpp_am_trap_is_alloced(
        int unit,
        int trap_id)
{
 
    int rv = 0;
    uint32 trap_index;

    BCMDNX_INIT_FUNC_DEFS;

    /* If in the range of PPD user defined*/
    if ( (SOC_PPC_TRAP_CODE_USER_DEFINED_0 <= trap_id) && (trap_id <= _BCM_LAST_USER_DEFINE) ) { 
        rv = dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_user_define,SW_STATE_RES_ALLOC_WITH_ID,trap_id);

    } else if (_BCM_TRAP_ID_IS_VIRTUAL(trap_id)) { /* If virtual bit is marked */
       /*Clears virtual bit before allocation*/
        trap_id = _BCM_TRAP_ID_TO_VIRTUAL(trap_id);
        rv = dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_virtual,SW_STATE_RES_ALLOC_WITH_ID,trap_id);

    }  else if (_BCM_TRAP_ID_IS_EGRESS(trap_id)) { /* egress create, with id*/
        trap_id = _bcm_dpp_rx_trap_id_to_egress(trap_id);
        /* Translate the indexing between enum and int for egress traps */
        rv = _bcm_dpp_am_eg_trap_index_get(unit, 
                                           trap_id, /* from the enum */
                                           &trap_index); /* 0...NOF_TRAPS */
        BCMDNX_IF_ERR_EXIT(rv);
        rv = dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_egress,SW_STATE_RES_ALLOC_WITH_ID,trap_index);    

    }
    else if (_BCM_TRAP_ID_IS_ETPP(trap_id)) 
    {
        trap_id = _bcm_dpp_rx_trap_id_to_etpp(trap_id);
        rv = dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_etpp,SW_STATE_RES_ALLOC_WITH_ID,trap_id);  
    }
    else { /* Otherwise, use single instance pool (ingress) */
        rv = dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_single_instance,SW_STATE_RES_ALLOC_WITH_ID,trap_id);  
    }

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_snoop_cmd_alloc(
        int unit,
        int flags,
        int *elem)
{
    int rv=0;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_snoop_cmd,flags,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_snoop_alloc(
        int unit,
        int flags,
        int snoop_cmnd,
        int *elem)
{
    int rv=0,res_flags=0;
    bcm_dpp_snoop_t data;
    int template;    
    int is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&data, 0, sizeof(data));
    /*Check if allocation is with ID or without ID*/
    if(flags & BCM_RX_SNOOP_WITH_ID) {
        res_flags = SW_STATE_RES_ALLOC_WITH_ID; 
        if(flags & BCM_RX_SNOOP_REPLACE) {
            res_flags |= SW_STATE_RES_ALLOC_REPLACE;
        }
        *elem = snoop_cmnd;
    }
    if (!SOC_WARM_BOOT(unit))
    {
        rv = _bcm_dpp_am_snoop_cmd_alloc(unit,res_flags,elem);
        BCMDNX_IF_ERR_EXIT(rv);
        /*Sync rx.c snoop allocations with port.c snoop allocations (templates) for USER_DEFINED traps*/
        data.snoop_unique = *elem;
        template = *elem; 

        rv = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_snoop_cmd, SHR_TEMPLATE_MANAGE_SET_WITH_ID, &data, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_am_snoop_dealloc(
        int unit,
        int flags,
        int snoop_cmnd, 
        int elem)
{
    int rv = BCM_E_NONE;
    int template;    
    int is_last;

    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_snoop_cmd,SW_STATE_RES_ALLOC_WITH_ID,snoop_cmnd);
    BCMDNX_IF_ERR_EXIT(rv);

    /*Sync rx.c traps allocations with port.c traps allocations (templates) for USER_DEFINED traps*/
    template = snoop_cmnd; 
    rv = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_snoop_cmd, template, &is_last);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_am_snoop_is_alloced(
        int unit,
        int snoop_cmnd)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_snoop_cmd,SW_STATE_RES_ALLOC_WITH_ID,snoop_cmnd));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_vpn_ac_alloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_LIF_ID *lif)
{
    int rv = BCM_E_NONE;
    int elem = *lif;
    bcm_dpp_am_sync_lif_alloc_info_t allocation_info;
    int nof_entries;
    BCMDNX_INIT_FUNC_DEFS; 

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.ingress_lif.pool_id = dpp_am_res_lif_dynamic;
    allocation_info.ingress_lif.application_type = bcm_dpp_am_ingress_lif_app_vpn_ac;
    allocation_info.egress_lif.pool_id = dpp_am_res_lif_dynamic;
    /* Allocate both Ingress and Egress */
    allocation_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS | _BCM_DPP_AM_SYNC_LIF_EGRESS;

    if (types == _BCM_DPP_AM_L2_VPN_AC_TYPE_DEFAULT) {
        allocation_info.egress_lif.application_type = bcm_dpp_am_egress_encap_app_out_ac;
        nof_entries = 1;
#ifdef BCM_88660_A0
    } else if (types == _BCM_DPP_AM_L2_VPN_AC_TYPE_BIG_OUT_AC) {
        allocation_info.egress_lif.application_type = bcm_dpp_am_egress_encap_app_out_ac;
        nof_entries = 2; /* 3 Tags PON Encapsulation requires a big AC entry */
#endif
    } else if (types == _BCM_DPP_AM_L2_VPN_AC_TYPE_PON_3_TAGS_DATA) {
        allocation_info.egress_lif.application_type = bcm_dpp_am_egress_encap_app_3_tags_data;
        nof_entries = 2; /* 3 Tags PON Encapsulation requires a Data entry and an Out-AC entry */
    } else if (types == _BCM_DPP_AM_L2_VPN_AC_TYPE_PON_3_TAGS_OUT_AC) {
        allocation_info.egress_lif.application_type = bcm_dpp_am_egress_encap_app_3_tags_out_ac;
        nof_entries = 1;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("types %d not exist"), types));
    }

    if (nof_entries == 1) {
        rv = _bcm_dpp_am_sync_lif_alloc(unit,&allocation_info, flags,1,&elem);
    } else {/* nof_entries must be 2 */
        rv = _bcm_dpp_am_sync_lif_alloc_align(unit,&allocation_info, flags,2,0,2,&elem);
    }
    BCMDNX_IF_ERR_EXIT(rv);

    *lif = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_vpn_ac_dealloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_LIF_ID lif)
{
    int rv = BCM_E_NONE;
    int elem = lif;
    int count = 0;
    bcm_dpp_am_sync_lif_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.ingress_lif.pool_id = dpp_am_res_lif_dynamic;
    allocation_info.ingress_lif.application_type = bcm_dpp_am_ingress_lif_app_vpn_ac;
    allocation_info.egress_lif.pool_id = dpp_am_res_lif_dynamic;
    /* Deallocate both Ingress and Egress */
    allocation_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS | _BCM_DPP_AM_SYNC_LIF_EGRESS;
    if (types == _BCM_DPP_AM_L2_VPN_AC_TYPE_PON_3_TAGS_DATA || types == _BCM_DPP_AM_L2_VPN_AC_TYPE_BIG_OUT_AC) {
        count = 2;
    } else {
        count = 1;
    } 

    rv = _bcm_dpp_am_sync_lif_dealloc(unit, &allocation_info, count, elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_trill_lif_alloc(
        int unit,
        uint32 flags,
        SOC_PPC_LIF_ID *lif)
{
    int rv = BCM_E_NONE;
    int elem = *lif;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.application_type = bcm_dpp_am_ingress_lif_app_trill_nick; /* NICK allocation */
    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    
    rv = _bcm_dpp_am_ingress_lif_alloc(unit,&allocation_info,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);
    *lif = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_trill_lif_is_alloced(
        int unit,
        SOC_PPC_LIF_ID lif)
{
    int elem = lif;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_ingress_lif_is_allocated(unit,&allocation_info,1,elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_trill_lif_dealloc(
        int unit,
        uint32 flags,
        SOC_PPC_LIF_ID lif)
{
    int rv = BCM_E_NONE;
    int elem = lif;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    rv = _bcm_dpp_am_ingress_lif_dealloc(unit,&allocation_info,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_l2_ac_alloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_LIF_ID *lif)
{
    int rv = BCM_E_NONE;
    int elem = *lif;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    /* type parameter is only needed for ARAD information */

    if (types == _BCM_DPP_AM_L2_AC_TYPE_VLAN_VSI) { /* Ingress LIF for VLAN-VSI (0-4K) */
        allocation_info.application_type = bcm_dpp_am_ingress_lif_app_ingress_ac;
    } else if (types == _BCM_DPP_AM_L2_AC_TYPE_ISID) { /* Ingress LIF for ISID */
        allocation_info.application_type = bcm_dpp_am_ingress_lif_app_ingress_isid;
    } else if (types == _BCM_DPP_AM_L2_AC_TYPE_VLAN_EDITING || types == _BCM_DPP_AM_L2_AC_TYPE_INITIAL_VLAN) { /* Ingress LIF for special VLAN editing properties */
        allocation_info.application_type = bcm_dpp_am_ingress_lif_app_ingress_ac;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("types %d not exist"), types));
    }
    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    rv = _bcm_dpp_am_ingress_lif_alloc(unit,&allocation_info,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *lif = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_ac_dealloc(
        int unit,
        uint32 lif_type,
        SOC_PPC_LIF_ID lif)
{
    int rv = BCM_E_NONE;
    int elem = lif;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;
    int count;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    count = (lif_type == _BCM_DPP_AM_L2_AC_TYPE_DOUBLE) ? 2 : 1;

    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    rv = _bcm_dpp_am_ingress_lif_dealloc(unit,&allocation_info,count,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_l2_ac_is_alloced(
        int unit,
        SOC_PPC_AC_ID in_ac)
{
    int elem = in_ac;
    bcm_dpp_am_ingress_lif_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_lif_dynamic;
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_ingress_lif_is_allocated(unit,&allocation_info,1,elem));

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_out_ac_alloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_AC_ID *out_ac)
{
    int rv = BCM_E_NONE;
    int elem = *out_ac;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    int nof_entries; 
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eg_out_ac;

    if (types == _BCM_DPP_AM_OUT_AC_TYPE_DEFAULT) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_out_ac;
        nof_entries = 1;
#ifdef BCM_88660_A0
    } else if (types == _BCM_DPP_AM_OUT_AC_TYPE_BIG_OUT_AC) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_out_ac;
        nof_entries = 2; /* 3 Tags PON Encapsulation requires a big AC entry */
    } else if (types == _BCM_DPP_AM_OUT_AC_TYPE_DATA_MPLS_PHASE) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_mpls_tunnel;
        nof_entries = 2; /* data entry allocated  as MPLS tunnel for phase considerations*/
        allocation_info.pool_id = dpp_am_res_eep_mpls_tunnel; 
#endif
    } else if (types == _BCM_DPP_AM_OUT_AC_TYPE_PON_3_TAGS_DATA) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_3_tags_data;
        nof_entries = 2; /* 3 Tags PON Encapsulation requires a Data entry and an Out-AC entry */
    } else if (types == _BCM_DPP_AM_OUT_AC_TYPE_PON_3_TAGS_OUT_AC) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_3_tags_out_ac;
        nof_entries = 1;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("types %d not exist"), types));
    }

    if (nof_entries == 1) {
        /*rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eg_out_ac,flags,1,&elem);*/
        rv = _bcm_dpp_am_egress_encap_alloc(unit,&allocation_info,flags,1,&elem);
    } else { /* nof_entries must be 2 */
        rv = _bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,&elem);
    }
    BCMDNX_IF_ERR_EXIT(rv);

    *out_ac = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_out_ac_dealloc(
        int unit,
        uint32 types,
        uint32 flags,
        SOC_PPC_AC_ID out_ac)
{
    int rv = BCM_E_NONE;
    int elem = out_ac;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    int count = 0;

    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eg_out_ac;
    /* rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eg_out_ac,1,elem); */
    /* 3 Tags PON Encapsulation allocated a Data entry and an Out-AC entry */
        if ((types == _BCM_DPP_AM_OUT_AC_TYPE_PON_3_TAGS_DATA) || (types == _BCM_DPP_AM_OUT_AC_TYPE_BIG_OUT_AC) ||
                                                                  (types == _BCM_DPP_AM_OUT_AC_TYPE_DATA_MPLS_PHASE)) {
        count = 2;
    } else {
        count = 1;
    }
    
    rv = _bcm_dpp_am_egress_encap_dealloc(unit,&allocation_info,count,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_out_ac_is_alloced(
        int unit,
        SOC_PPC_AC_ID out_ac)
{
    int elem = out_ac;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eg_out_ac;
    /*  BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_eg_out_ac,1,elem));*/
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_is_allocated(unit,&allocation_info,1,elem));

exit:
    BCMDNX_FUNC_RETURN;
}

/* DATA, Mirror , ERSPAN */
int
bcm_dpp_am_eg_data_erspan_alloc(
        int unit,
        uint32 flags,
        uint32 *outlif)
{
    int elem = *outlif;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eg_data_erspan;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,&elem));
    *outlif = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_eg_data_erspan_dealloc(
        int unit,
        uint32 flags,
        uint32 eg_data_erspan)
{
    int elem = eg_data_erspan;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;


    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eg_data_erspan;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_egress_encap_dealloc(unit,&allocation_info,2,elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_eg_data_erspan_is_alloced(
        int unit,
        uint32 eg_data_erspan)
{
    int elem = eg_data_erspan;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eg_data_erspan;
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_is_allocated(unit,&allocation_info,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

/* end ERSPAN */
/* DATA, IPV6 TUNNEL */
int
bcm_dpp_am_ipv6_tunnel_alloc(
        int unit,
        uint32 flags,
        uint32 *outlif)
{
    int elem = *outlif;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_ipv6_tunnel;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_egress_encap_alloc_align(unit,&allocation_info,flags,2,0,2,&elem));
    *outlif = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ipv6_tunnel_dealloc(
        int unit,
        uint32 flags,
        uint32 ipv6_tunnel)
{
    int elem = ipv6_tunnel;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_ipv6_tunnel;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_egress_encap_dealloc(unit,&allocation_info,2,elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ipv6_tunnel_is_alloced(
        int unit,
        uint32 ipv6_tunnel)
{
    int elem = ipv6_tunnel;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_ipv6_tunnel;
    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_is_allocated(unit,&allocation_info,2,elem));
exit:
    BCMDNX_FUNC_RETURN;
}
/* end IPv6 tunnel*/


int
bcm_dpp_am_qos_ing_elsp_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_elsp,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_elsp_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_elsp,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_elsp_is_alloced(
        int unit,
        int qos_id)
{
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_elsp,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_qos_ing_lif_cos_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_lif_cos,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_lif_cos_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_lif_cos,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_lif_cos_is_alloced(
        int unit,
        int qos_id)
{
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_lif_cos,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_qos_ing_pcp_vlan_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_pcp_vlan,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_pcp_vlan_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_pcp_vlan,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_pcp_vlan_is_alloced(
        int unit,
        int qos_id)
{
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_pcp_vlan,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_color_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_color,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_color_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_color,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_color_is_alloced(
        int unit,
        int qos_id)
{
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_color,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_remark_id_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_remark_id,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_qos_egr_mpls_php_id_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_mpls_php_id,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_remark_id_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_remark_id,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_qos_egr_mpls_php_id_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_mpls_php_id,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_remark_id_is_alloced(
        int unit,
        int qos_id)
{
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_remark_id,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_mpls_php_id_is_alloced(
        int unit,
        int qos_id)
{
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_mpls_php_id,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_pcp_vlan_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_pcp_vlan,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_pcp_vlan_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_pcp_vlan,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_pcp_vlan_is_alloced(
        int unit,
        int qos_id)
{
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_pcp_vlan,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_qos_ing_cos_opcode_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_cos_opcode,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_cos_opcode_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_cos_opcode,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_ing_cos_opcode_is_alloced(
        int unit,
        int qos_id)
{
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_ing_cos_opcode,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_l2_i_tag_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_l2_i_tag,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_l2_i_tag_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_l2_i_tag,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_l2_i_tag_is_alloced(
        int unit,
        int qos_id)
{

    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_l2_i_tag,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_88660
int
bcm_dpp_am_qos_egr_dscp_exp_marking_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_dscp_exp_marking,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_dscp_exp_marking_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_dscp_exp_marking,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qos_egr_dscp_exp_marking_is_alloced(
        int unit,
        int qos_id)
{

    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_qos_egr_dscp_exp_marking,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

#endif /* BCM_88660 */

/*
 *   meter {
 */
int
bcm_dpp_am_meter_alloc(
        int unit,
        uint32 flags,
		int core,
        int meter_group, /* a=0 b=1 */
        int nof_meters, /* number of meters to allocate */
		int *act_meter_core,
        int *act_meter_group,
        int *meter_id) /* if nof_metrs != 1: this is the meter_id of the first one allocated. the next ones will be meter_id+1...meter_id+nof_meters-1 */
{
    int rv = BCM_E_NONE;
    int elem = *meter_id;

    BCMDNX_INIT_FUNC_DEFS;

	if (nof_meters < 1) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("nof_meters to allocate must be at least 1")));
	}

	if (core < BCM_DPP_AM_METER_CORE_ANY) {
		/* specific core*/
		*act_meter_core = core;

		if (meter_group < BCM_DPP_AM_METER_GROUP_ANY) {
			/* specific group*/
			*act_meter_group = meter_group;

			if (nof_meters == 1) {
				rv = dpp_am_res_alloc(unit, (*act_meter_core), dpp_am_res_meter_a + (*act_meter_group), flags, nof_meters, &elem);
				BCMDNX_IF_ERR_EXIT(rv);
			}
			else {
				rv = dpp_am_res_alloc_align(unit, (*act_meter_core), dpp_am_res_meter_a + (*act_meter_group), flags | SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO, 
										 nof_meters, 0, nof_meters, &elem);
				BCMDNX_IF_ERR_EXIT(rv);
			}
		}
		else {
			/* try from any group */
			for (*act_meter_group = 0; *act_meter_group < BCM_DPP_AM_METER_GROUP_ANY; ++(*act_meter_group)) {

				if (nof_meters == 1) {
					rv = dpp_am_res_alloc(unit, (*act_meter_core), dpp_am_res_meter_a + (*act_meter_group), flags, nof_meters, &elem);
				}
				else {
					rv = dpp_am_res_alloc_align(unit, (*act_meter_core), dpp_am_res_meter_a + (*act_meter_group), flags | SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO, 
											 nof_meters, 0, nof_meters, &elem);
				}

				/* if found then done */
				if (rv == BCM_E_NONE) {
					break;
				}

				/* if full try next table */
				if (rv == BCM_E_RESOURCE && ((*act_meter_group + 1) < BCM_DPP_AM_METER_GROUP_ANY)) {
					continue;
				}

				BCMDNX_IF_ERR_EXIT(rv);
			}
		}
	}
	else{
		/* try any core*/
		for (*act_meter_core = 0; *act_meter_core < BCM_DPP_AM_METER_CORE_ANY; ++(*act_meter_core)) {

			if (meter_group < BCM_DPP_AM_METER_GROUP_ANY) {
				/* specific group*/
				*act_meter_group = meter_group;

				if (nof_meters == 1) {
					rv = dpp_am_res_alloc(unit, (*act_meter_core), dpp_am_res_meter_a + (*act_meter_group), flags, nof_meters, &elem);
				}
				else {
					rv = dpp_am_res_alloc_align(unit, (*act_meter_core), dpp_am_res_meter_a + (*act_meter_group), flags | SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO, 
												nof_meters, 0, nof_meters, &elem);
				}
			}
			else{
				/* try from any group */
				for (*act_meter_group = 0; *act_meter_group < BCM_DPP_AM_METER_GROUP_ANY; ++(*act_meter_group)) {

					if (nof_meters == 1) {
						rv = dpp_am_res_alloc(unit, (*act_meter_core), dpp_am_res_meter_a + (*act_meter_group), flags, nof_meters, &elem);
					}
					else {
						rv = dpp_am_res_alloc_align(unit, (*act_meter_core), dpp_am_res_meter_a + (*act_meter_group), flags | SW_STATE_RES_BITMAP_ALLOC_ALIGN_ZERO, 
												 nof_meters, 0, nof_meters, &elem);
					}

					/* if found then done */
					if (rv == BCM_E_NONE) {
						break;
					}

					/* if full try next table or next core */
					if (rv == BCM_E_RESOURCE){
						if ((*act_meter_group + 1) < BCM_DPP_AM_METER_GROUP_ANY) {
							continue;
						}
						else{
							break;
						}
					}

					BCMDNX_IF_ERR_EXIT(rv);
				}			
			}

			/* if found then done */
			if (rv == BCM_E_NONE) {
				break;
			}

			/* if full try next core  */
			if (rv == BCM_E_RESOURCE && ((*act_meter_core + 1) < BCM_DPP_AM_METER_CORE_ANY)) {
				continue;
			}
			BCMDNX_IF_ERR_EXIT(rv);
		}
	}

    *meter_id = elem;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_meter_dealloc(
        int unit,
		int core,
        uint32 flags,
        int meter_group,
        int nof_meters, /* number of meters to deallocate */
        int meter_id) /* if nof_metrs != 1: this is the meter_id of the first one to deallocate. the next ones will be meter_id+1...meter_id+nof_meters-1 */
{
    int rv = BCM_E_NONE;
    int elem = meter_id;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free(unit, core, dpp_am_res_meter_a + meter_group,nof_meters,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_meter_is_alloced(
        int unit,
		int core,
        int meter_group,
        int nof_meters, /* number of meters to check */
        int meter_id) /* if nof_metrs != 1: this is the meter_id of the first one. the next ones will be meter_id+1...meter_id+nof_meters-1 */
{
    int elem = meter_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, core, dpp_am_res_meter_a + meter_group,nof_meters,elem));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   } meter
 */
int
bcm_dpp_am_mc_alloc(
        int unit,
        uint32 flags, /* flags should be SW_STATE_RES_ALLOC_WITH_ID; */
        SOC_TMC_MULT_ID  *mc_id,
        uint8 is_egress)
{
    int elem = *mc_id;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    if (is_egress != FALSE) {
            elem += SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;

    }
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_mc_dynamic,flags,1,&elem);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_mc_dealloc(
        int unit,
        SOC_TMC_MULT_ID mc_id,
        uint8 is_egress)
{
    int rv;
    int elem = mc_id;
    BCMDNX_INIT_FUNC_DEFS;
    if (is_egress != FALSE) {
            elem += SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;
    }
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_mc_dynamic, 1, elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_mc_is_alloced(
        int unit,
        SOC_TMC_MULT_ID mc_id,
        uint8 is_egress)
{
    int elem = mc_id;
    if (is_egress != FALSE) {
            elem += SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;
    }
    return dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_mc_dynamic, 1, elem);
}


/* 
 * OAM
 */
int
bcm_dpp_am_oam_ma_id_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *ma_id) 
{
    int elem = *ma_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_ma_index,flags,1,&elem));
    *ma_id = elem;

exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_oam_ma_id_dealloc(
        int unit,
        uint32 ma_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_ma_index, 1, ma_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_ma_id_is_alloced(
        int unit,
        uint32 ma_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_ma_index, 1, ma_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_short_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *mep_id) 
{
    int elem = *mep_id;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_short, flags, 1, &elem);
    if (rv != BCM_E_NONE) {
        BCM_ERR_EXIT_NO_MSG(rv);
    }
    *mep_id = elem;
exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_oam_mep_id_short_dealloc(
        int unit,
        uint32 mep_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_short, 1, mep_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_short_is_alloced(
        int unit,
        uint32 mep_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_short, 1, mep_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_long_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *mep_id) 
{
    int elem = *mep_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_mep_id_long, flags, 1, &elem));
    *mep_id = elem;
exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_oam_mep_id_long_dealloc(
        int unit,
        uint32 mep_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_long, 1, mep_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_long_is_alloced(
        int unit,
        uint32 mep_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_long, 1, mep_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_long_alloc_non_48_maid(
    int unit,
    uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
    uint32  *mep_id) {
    int elem = *mep_id;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    /*  In Jericho and Arad+ OAMP_MEP_DB is divided to 3 logical banks
       for "long MEPs non 48b maid" bank range 1 - size of umc table,
       all MEPs except every 4th MEP */
    /* for WITH_ID flag, need to allocate the corresponding id in sw bank. 7/8 size of id */
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        elem =  elem - ((elem) / 4);
    }
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_long_non_48_maid, flags, 1, &elem);
    if (rv != BCM_E_NONE) {
        BCM_ERR_EXIT_NO_MSG(rv);
    }
    /* when WITH_ID flag is not present, need to return the actual HW MEP id */
    if (!(flags & SW_STATE_RES_ALLOC_WITH_ID)) {
        *mep_id = elem + ((elem - 1) / 3);
    }
exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_oam_mep_id_long_dealloc_non_48_maid(
    int unit,
    uint32 mep_id) {
    /* deallocate the corresponding id in sw bank */
    int elem =  mep_id - ((mep_id) / 4);
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_long_non_48_maid, 1, elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_long_is_alloced_non_48_maid(
    int unit,
    uint32 mep_id) {
    /* check the corresponding id in sw bank */
    int elem =  mep_id - ((mep_id) / 4);
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_long_non_48_maid, 1, elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_long_alloc_48_maid(
    int unit,
    uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
    uint32  *mep_id) {
    int elem = *mep_id;
    BCMDNX_INIT_FUNC_DEFS;
    /* In Jericho and Arad+ OAMP_MEP_DB is divided to 3 logical banks
       for "short MEPs 48b maid" bank size of umc table - size of MEP DB
       , all MEPs except every 4th MEP*/
    /* for WITH_ID flag, need to allocate the corresponding id in sw bank. 1/8 size of id */
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        elem = elem / 4;
    }
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_long_48_maid, flags, 1, &elem));
    /* when WITH_ID flag is not present, need to return the actual HW MEP id */
    if (!(flags & SW_STATE_RES_ALLOC_WITH_ID)) {
        *mep_id = elem * 4;;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_long_dealloc_48_maid(
        int unit,
        uint32 mep_id)
{
    /* dealloc the corresponding id in sw bank */
    int elem =  mep_id / 4;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_long_48_maid, 1, elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_long_is_alloced_48_maid(
        int unit,
        uint32 mep_id)
{
    /* check the corresponding id in sw bank */
    int elem =  mep_id / 4;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_long_48_maid, 1, elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_jumbo_tlv_alloc(
    int unit,
    uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
    uint32  *mep_id) {
    int elem = *mep_id;
    int steps = 0;
    BCMDNX_INIT_FUNC_DEFS;
    /* For Jumbo tlv, steps should be 16*/
    steps = 16;
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        elem = elem / steps;
    }
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_jumbo_tlv, flags, 1, &elem));
    /* when WITH_ID flag is not present, need to return the actual HW MEP id */
    if (!(flags & SW_STATE_RES_ALLOC_WITH_ID)) {
        *mep_id = elem * steps;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_jumbo_tlv_dealloc(
        int unit,
        uint32 mep_id)
{
    /* dealloc the corresponding id in sw bank */
    /* For Jumbo tlv, steps should be 16*/
    int steps = 0;
     int elem =0; 
    BCMDNX_INIT_FUNC_DEFS;
    steps = 16;
    elem =  mep_id / steps;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_jumbo_tlv, 1, elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_jumbo_tlv_is_alloced(
        int unit,
        uint32 mep_id)
{
    /* check the corresponding id in sw bank */
    int steps = 0;
     int elem =0; 
    BCMDNX_INIT_FUNC_DEFS;
    steps = 16;
    elem =  mep_id / steps;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_jumbo_tlv, 1, elem));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_no_jumbo_tlv_alloc(
    int unit,
    uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
    uint32  *mep_id) {
    int elem = *mep_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_no_jumbo_tlv, flags, 1, &elem));
    *mep_id = elem;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_no_jumbo_tlv_dealloc(
        int unit,
        uint32 mep_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_no_jumbo_tlv, 1, mep_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_mep_id_no_jumbo_tlv_is_alloced(
        int unit,
        uint32 mep_id)
{
    /* check the corresponding id in sw bank */
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id_no_jumbo_tlv, 1, mep_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qax_oam_mep_id_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint8  type, /* Indicates if the entry is a MEP or not */
        uint32  *mep_id)
{
    int elem = *mep_id;
    BCMDNX_INIT_FUNC_DEFS;

    /* In QAX the MEP-DB is 12K deep, but only the first 8K can be used by MEPs, whereas
     * the entire table can be used by LM/DM/additional data. To handle this 2 S/w resources
     * are used MEP-DB (12K) and MEP(8K)
     */
    if (type == BCM_DPP_AM_OAM_MEP_DB_ENTRY_TYPE_MEP) {
        /* A MEP entry is being created, alloc it both the MEP table and the MEP-DB table */
        BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_mep_id, flags, 1, &elem));
        *mep_id = elem;
        BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_mep_db_id, SW_STATE_RES_ALLOC_WITH_ID, 1, &elem));
    } else {
        /* A non-MEP type entry is being created, alloc from the MEP-DB table first, if the entry
         * returned is within the MEP range, then alloc from the MEP table as well.
         */
        BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_mep_db_id, flags, 1, &elem));
        *mep_id = elem;
        if (*mep_id < SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)) {
            BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_mep_id, SW_STATE_RES_ALLOC_WITH_ID, 1, &elem));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_qax_oam_mep_id_dealloc(
        int unit,
        uint32 mep_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_db_id, 1, mep_id));

    if (mep_id < SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)) {
        /* This entry is present on both resources, see bcm_dpp_am_qax_oam_mep_id_alloc(..) */
        BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_id, 1, mep_id));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_qax_oam_mep_id_is_alloced(
        int unit,
        uint32 mep_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_mep_db_id, 1, mep_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_sd_sf_id_alloc(
        int unit,
        uint32 flags, 
        uint32  *sd_sf_id)
{
    int elem = *sd_sf_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_sd_sf_id, flags, 1, &elem));
    *sd_sf_id = elem;
exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_oam_sd_sf_id_dealloc(
        int unit,
        uint32 sd_sf_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_sd_sf_id, 1, sd_sf_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_sd_sf_id_is_alloced(
        int unit,
        uint32 sd_sf_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_sd_sf_id, 1, sd_sf_id));

exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_am_oam_y_1711_sd_sf_id_alloc(
        int unit,
        uint32 flags, 
        uint32  *y_1711_sd_sf_id)
{
    int elem = *y_1711_sd_sf_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_y_1711_sd_sf_id, flags, 1, &elem));
    *y_1711_sd_sf_id = elem;
exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_oam_y_1711_sd_sf_id_dealloc(
        int unit,
        uint32 y_1711_sd_sf_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_y_1711_sd_sf_id, 1, y_1711_sd_sf_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_y_1711_sd_sf_id_is_alloced(
        int unit,
        uint32 y_1711_sd_sf_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_y_1711_sd_sf_id, 1, y_1711_sd_sf_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_rmep_id_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *rmep_id) 
{
    int elem = *rmep_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_oam_rmep_id,flags,1,&elem));
    *rmep_id = elem;
exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_oam_rmep_id_dealloc(
        int unit,
        uint32 rmep_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_rmep_id, 1, rmep_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_oam_rmep_id_is_alloced(
        int unit,
        uint32 rmep_id)
{
    return dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_rmep_id, 1, rmep_id);
}

int
bcm_dpp_am_oam_trap_code_upmep_ftmh_header_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32 *trap_code)
{
    int elem = *trap_code;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_trap_code_upmep_ftmh_header, flags, 1, &elem));
    *trap_code = elem;

exit:
    BCMDNX_FUNC_RETURN;

}

int
bcm_dpp_am_oam_trap_code_upmep_ftmh_header_dealloc(
        int unit,
        uint32 trap_code)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_oam_trap_code_upmep_ftmh_header, 1, trap_code));

exit:
    BCMDNX_FUNC_RETURN;
}

/* OAM - End */



int
bcm_dpp_am_fec_alloc(
    int unit,
    uint32 flags,
    _bcm_dpp_am_fec_alloc_usage usage,
    int size,/*non-zero for protection or ECMP*/
    SOC_PPC_FEC_ID *fec_id)
{
    int rv;
    int elem = *fec_id;
    int align = FALSE;
    
    BCMDNX_INIT_FUNC_DEFS;

    /* We don't support allocating more than 2 FECs at a time. */
    BCMDNX_VERIFY(size <= 2);
      
    if(usage!= 1) { /* only if not ECMP */
        if (size == 1) {
            rv = bcm_dpp_am_fec_global_alloc(unit, flags, usage, size, align, &elem);
            BCMDNX_IF_ERR_EXIT(rv);

            *fec_id = elem;
        }
        else if (size == 2) {
            align = TRUE;
            rv = bcm_dpp_am_fec_global_alloc(unit, flags, usage, size, align, &elem);
            BCMDNX_IF_ERR_EXIT(rv);

            *fec_id = elem+1;
        }
    }
    /* for ecmp */
    else {
        align = TRUE;
        rv = bcm_dpp_am_fec_global_alloc(unit, flags, usage, size, align, &elem);
        BCMDNX_IF_ERR_EXIT(rv);
        
        *fec_id = elem;
    } 

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_fec_verify_fec_id(int unit, SOC_PPC_FEC_ID fec_id)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (fec_id >= SOC_DPP_DEFS_GET(unit,nof_fecs)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The supplied FEC id (0x%x) is out of bounds (0-%d)."), fec_id, SOC_DPP_DEFS_GET(unit, nof_fecs)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_fec_global_alloc(
                            int unit,
                            uint32 flags,
                            _bcm_dpp_am_fec_alloc_usage usage,
                            int size, 
                            int aligned,
                            int *elem)                        
{
    int rv;
    uint32 bank_id = 0;
#ifdef BCM_88470_A0
    uint32 is_cascaded;
#endif /* BCM_88470_A0 */

    BCMDNX_INIT_FUNC_DEFS;

    /* We don't support allocating more than 2 FECs at a time. */
    BCMDNX_VERIFY(size <= 2);

    /* In Jerciho there is different handling for FEC allocations due to the FEC typing restrictions for Hierarchical FEC. */
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        uint8 bank_group;
        _bcm_dpp_pp_fec_group_type fec_group;

        fec_group = usage == 2 ? _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_B : _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_A;

        /* If WITH_ID in Jericho, then we need to make sure the selected bank group matches the fec group (or empty). */
        if ((flags & SW_STATE_RES_ALLOC_WITH_ID) != 0) {
            int used_count;
            uint32 fec_bank_id = SOC_PPD_FRWRD_FEC_BANK_ID(unit, *elem); 

            rv = _bcm_dpp_am_fec_verify_fec_id(unit, *elem);
            BCMDNX_IF_ERR_EXIT(rv);
            
            rv = ALLOC_MNGR_ACCESS.fec_bank_groups.get(unit, fec_bank_id, &bank_group); 
            BCMDNX_IF_ERR_EXIT(rv);

            rv = dpp_am_res_used_count_get(unit, fec_bank_id, dpp_am_res_fec_global, &used_count); 
            BCMDNX_IF_ERR_EXIT(rv);

            if (fec_group != bank_group && used_count > 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The group of FEC 0x%x (%s) differs from the group of the FEC to be allocated (%s)."),
                                                                  *elem,
                                                                  fec_group == _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_A ? "A" : "B",
                                                                  bank_group == _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_A ? "A" : "B"));
            }

            rv = dpp_am_res_alloc_align(unit, fec_bank_id, dpp_am_res_fec_global, flags, aligned ? 2 : 1, 0, size, elem);
            BCMDNX_IF_ERR_EXIT(rv);

            /* If the bank was empty we need to update the bank type with the FEC type. */
            if (used_count == 0) {
                bank_group = fec_group;
                rv = ALLOC_MNGR_ACCESS.fec_bank_groups.set(unit, fec_bank_id, bank_group);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* If the following assert does not hold, then we expect the alloc_mgr to catch it (i.e. unaligned fec id). */
            /* BCMDNX_VERIFY(size == 1 || fec_bank_id == SOC_PPD_FRWRD_FEC_BANK_ID(unit, *elem + 1); */

            BCM_EXIT;
        } else {
            /* If the user did not specify a FEC index, then we need to find a free (or several free) FEC index(es). */
            /* First we try to find a bank with a matching group. */
            int bank_idx;

            /* First we try to allocate in banks of the same group as the FEC. */
            for (bank_idx = 0; bank_idx < SOC_DPP_DEFS_GET(unit, nof_fec_banks); bank_idx++) {
                uint8 bank_group;
                int used_count;

                rv = ALLOC_MNGR_ACCESS.fec_bank_groups.get(unit, bank_idx, &bank_group);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = dpp_am_res_used_count_get(unit, bank_idx, dpp_am_res_fec_global, &used_count); 
                BCMDNX_IF_ERR_EXIT(rv);

                /* Check that the group matches and that the bank is not free. */
                if (used_count > 0 && bank_group == fec_group) {
                    rv = dpp_am_res_silent_alloc_align(unit,bank_idx,dpp_am_res_fec_global,flags, aligned ? 2 : 1, 0, size, elem);

                    /* The bank is full, try the next one.*/
                    if (rv == BCM_E_RESOURCE) {
                        continue;
                    }

                    /* Other errors are reported. */
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* No error means the FEC was allocated. */
                    /* Remark: The FEC id is returned as global and not as offset in bank. */
                    BCM_EXIT;
                }
            }

            /* Next we try to find a free bank. */
            for (bank_idx = 0; bank_idx < SOC_DPP_DEFS_GET(unit, nof_fec_banks); bank_idx++) {
                int used_count;

                rv = dpp_am_res_used_count_get(unit, bank_idx, dpp_am_res_fec_global, &used_count); 
                BCMDNX_IF_ERR_EXIT(rv);

                if (used_count == 0) {
                    rv = dpp_am_res_alloc_align(unit, bank_idx, dpp_am_res_fec_global, flags, aligned ? 2 : 1, 0, size, elem);
                    BCMDNX_IF_ERR_EXIT(rv);

                    bank_group = fec_group;
                    /* On success we need to update the bank type with the FEC type. */
                    rv = ALLOC_MNGR_ACCESS.fec_bank_groups.set(unit, bank_idx, bank_group);
                    BCMDNX_IF_ERR_EXIT(rv);

                    /* Remark: The FEC id is returned as global and not as offset in bank. */
                    BCM_EXIT;
                }
            }

            /* If still no success then all banks are full and we produce an error. */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
                                (_BSL_BCM_MSG("Could not allocate %s - Could not find enough room in banks with the same group (%s) or no bank is of group %s."), 
                                                              size == 1 ? "a FEC" : "2 FECs", 
                                                              fec_group == _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_A ? "A" : "B",
                                                              fec_group == _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_A ? "A" : "B"));
        }
    } else {

        if (!aligned) {
            rv = dpp_am_res_alloc(unit,bank_id,dpp_am_res_fec_global,flags,size,elem);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else {
            rv = dpp_am_res_alloc_align(unit,bank_id,dpp_am_res_fec_global,flags,2, 0, size, elem);
            BCMDNX_IF_ERR_EXIT(rv);
        }

#ifdef BCM_88470_A0
        /*In QAX, save cascaded status in sw_state*/
        if (SOC_IS_QAX(unit)) {
            is_cascaded = (usage == _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED) ? 1 : 0;
            rv = ALLOC_MNGR_ACCESS.fec_is_cascaded.bit_range_write(
               unit,
                *elem,
                0,
                size,
                &is_cascaded
            );
            BCMDNX_IF_ERR_EXIT(rv);
        }
#endif /* BCM_88470_A0 */
        BCM_EXIT;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_fec_dealloc(
    int unit,
    uint32 flags,
    int size,/*non-zero for protection or ECMP*/
    SOC_PPC_FEC_ID fec_id)
{
    int rv;
#ifdef BCM_88470_A0
	uint32 is_cascaded;
#endif /* BCM_88470_A0 */
    
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_fec_verify_fec_id(unit, fec_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = dpp_am_res_free(unit, SOC_PPD_FRWRD_FEC_BANK_ID(unit, fec_id), dpp_am_res_fec_global, size, fec_id);
    BCMDNX_IF_ERR_EXIT(rv);

#ifdef BCM_88470_A0
    if (SOC_IS_QAX(unit)) {
        is_cascaded = 0;
        rv = ALLOC_MNGR_ACCESS.fec_is_cascaded.bit_range_write(
           unit,
           fec_id,
           0,
           size,
           &is_cascaded
        );
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /* BCM_88470_A0 */

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_fec_is_alloced(
    int unit,
    uint32 flags,
    int size,/*non-zero for protection or ECMP*/
    SOC_PPC_FEC_ID fec_id)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_fec_verify_fec_id(unit, fec_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = dpp_am_res_check(unit, SOC_PPD_FRWRD_FEC_BANK_ID(unit, fec_id), dpp_am_res_fec_global, size, fec_id);
    BCM_RETURN_VAL_EXIT(rv);

 exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Get the FEC usage (only valid for usages 0 and 2).
 * Errors if the FEC does not exist.
 */
int bcm_dpp_am_fec_get_usage(int unit, SOC_PPC_FEC_ID fec_id, _bcm_dpp_am_fec_alloc_usage *usage)
{
    int rv;
    int bank_id = 0;
    int used_count;
    uint8 bank_group;
#ifdef BCM_88470_A0
    uint8 is_cascaded;
#endif /* BCM_88470_A0 */
    int size = 1;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_fec_verify_fec_id(unit, fec_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check that the FEC exists. */
    rv = dpp_am_res_check(unit, SOC_PPD_FRWRD_FEC_BANK_ID(unit, fec_id), dpp_am_res_fec_global, size, fec_id);
    if (rv != BCM_E_EXISTS) {
      BCMDNX_IF_ERR_EXIT(rv); 
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The requested FEC id (0x%x) does not exist."), fec_id));
    }

    *usage = 0;
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        bank_id = SOC_PPD_FRWRD_FEC_BANK_ID(unit, fec_id); 

        rv = dpp_am_res_used_count_get(unit, bank_id, dpp_am_res_fec_global, &used_count); 
        BCMDNX_IF_ERR_EXIT(rv);
        BCMDNX_VERIFY(used_count > 0);

        rv = ALLOC_MNGR_ACCESS.fec_bank_groups.get(unit, bank_id, &bank_group);
        BCMDNX_IF_ERR_EXIT(rv);

        *usage = bank_group == _BCM_DPP_PP_FEC_GROUP_TYPE_GROUP_A ? 0 : 2;
    }
#ifdef BCM_88470_A0
    if (SOC_IS_QAX(unit)) {
        is_cascaded = 0;
        rv = ALLOC_MNGR_ACCESS.fec_is_cascaded.bit_get(
           unit,
           fec_id,
           &is_cascaded
        );
        BCMDNX_IF_ERR_EXIT(rv);

        *usage = (is_cascaded ? _BCM_DPP_AM_FEC_ALLOC_USAGE_CASCADED : _BCM_DPP_AM_FEC_ALLOC_USAGE_STANDARD);
    }
#endif /* BCM_88470_A0 */

exit:
    BCMDNX_FUNC_RETURN;
}

/*Local InLIF*/
int
bcm_dpp_am_local_inlif_alloc(
    int unit,
    int core_id,/*bank_id*/
    uint32 flags,
    uint8 *tag,
    int *inlif_id)
{
    int rv;
    uint32 alloc_flags = 0;
    int res_pool;
 
    BCMDNX_INIT_FUNC_DEFS;

    if (flags & BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID) {
        alloc_flags = SW_STATE_RES_ALLOC_WITH_ID;
    }
    if (flags & BCM_DPP_AM_IN_LIF_FLAG_ALLOC_CHECK_ONLY) {
        alloc_flags |= SW_STATE_RES_ALLOC_CHECK_ONLY;
    }

    /*core id = bank_id*/
    res_pool = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? dpp_am_res_local_inlif_wide : dpp_am_res_local_inlif_common ;

    rv = dpp_am_res_alloc_tag(unit, core_id, res_pool, alloc_flags | SW_STATE_RES_ALLOC_ALWAYS_CHECK_TAG, tag, 1, inlif_id);
    DPP_AM_RES_CHECK_SILENT_MODE(rv,alloc_flags);
 
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_local_inlif_dealloc(
    int unit,
    int core_id,
    uint32 flags,
    int inlif_id)
{
    int rv, res_pool;
    uint8 tag;
    BCMDNX_INIT_FUNC_DEFS;

    res_pool = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? dpp_am_res_local_inlif_wide : dpp_am_res_local_inlif_common ;
 
    rv = dpp_am_res_tag_get(unit, core_id, res_pool, 0, inlif_id, (void*)&tag);
    BCMDNX_IF_ERR_EXIT(rv);
  
    rv = dpp_am_res_free(unit, core_id, res_pool, 1, inlif_id);;
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_dpp_am_local_inlif_range_set(unit, core_id, flags, tag, inlif_id, 1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_local_inlif_tag_get(
    int unit,
    int core_id,
    uint32 flags,
    int inlif_id,
    uint8 *tag_id)
{
    int rv, res_pool;

    BCMDNX_INIT_FUNC_DEFS;

    res_pool = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? dpp_am_res_local_inlif_wide : dpp_am_res_local_inlif_common ;
 
    rv = dpp_am_res_tag_get(unit, core_id, res_pool, 0, inlif_id, (void *)tag_id);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_local_inlif_is_alloc(
    int unit,
    int core_id,
    uint32 flags,
    int inlif_id)
{
    int res_pool;
    BCMDNX_INIT_FUNC_DEFS;

    res_pool = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? dpp_am_res_local_inlif_wide : dpp_am_res_local_inlif_common ;

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, core_id, res_pool, 1 ,inlif_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_dpp_am_local_inlif_range_set(int unit, int core_id, uint32 flags, uint8 tag_id, int start, int count)
{
    void *tag = (void*)(&tag_id);
    int res_pool;

    BCMDNX_INIT_FUNC_DEFS;

    res_pool = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? dpp_am_res_local_inlif_wide : dpp_am_res_local_inlif_common ;

    BCMDNX_IF_ERR_EXIT(dpp_am_res_tag_set(unit, core_id, res_pool, 0, start, count, tag));

exit:
    BCMDNX_FUNC_RETURN;
}

/*Local OutLIF*/
int
bcm_dpp_am_local_outlif_alloc(
    int unit,
    int core_id,/*bank_id*/
    uint32 flags,
    uint8 *tag,
    int *outlif_id)
{
    int rv;
 
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_alloc_tag(unit, core_id, dpp_am_res_local_outlif, flags | SW_STATE_RES_ALLOC_ALWAYS_CHECK_TAG, tag, 1, outlif_id);
    DPP_AM_RES_CHECK_SILENT_MODE(rv,flags);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_local_outlif_alloc_align(
    int unit,
    int core_id,/*bank_id*/
    uint32 flags,
    uint8 *tag, 
    int align, 
    int offset,
    int count,
    int *outlif_id)
{
    int rv;
 
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_alloc_align_tag(unit, core_id, dpp_am_res_local_outlif, flags | SW_STATE_RES_ALLOC_ALWAYS_CHECK_TAG, align,offset, tag, count, outlif_id);
    DPP_AM_RES_CHECK_SILENT_MODE(rv,flags);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_local_outlif_dealloc(
    int unit,
    int core_id,
    uint32 flags,
    int count,
    int outlif_id)
{
    int rv;
    uint8 tag;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_tag_get(unit, core_id, dpp_am_res_local_outlif, 0, outlif_id, (void*)&tag);
    BCMDNX_IF_ERR_EXIT(rv);
  
    rv = dpp_am_res_free(unit, core_id, dpp_am_res_local_outlif, count, outlif_id);
    BCMDNX_IF_ERR_EXIT(rv);  

    rv = bcm_dpp_am_local_outlif_range_set(unit, core_id, flags, tag, outlif_id, count);
    BCMDNX_IF_ERR_EXIT(rv);
 
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_am_local_outlif_is_alloc(
    int unit,
    int core_id,
    uint32 flags,
    int outlif_id)
{

    BCMDNX_INIT_FUNC_DEFS;

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, core_id, dpp_am_res_local_outlif, 1 ,outlif_id));
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_dpp_am_local_outlif_range_set(int unit, int core_id, uint32 flags, uint8 tag_id, int start, int count) 
{
    void *tag = (void*)(&tag_id);

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(dpp_am_res_tag_set(unit, core_id, dpp_am_res_local_outlif, 0, start, count, tag));

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * policer
 */
int
bcm_dpp_am_policer_alloc(
    int unit,
	int core_id,
    uint32 flags,
    int size,
    int *policer_id)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
	rv = dpp_am_res_alloc(unit, core_id, dpp_am_res_ether_policer,flags,size,policer_id);
	BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_policer_dealloc(
    int unit,
	int core_id,
    uint32 flags,
    int size,
    int policer_id)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

	rv = dpp_am_res_free(unit, core_id,dpp_am_res_ether_policer,size,policer_id);
	BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_policer_is_alloc(
    int unit,
	int core_id,
    int size,
    int policer_id)
{
    BCMDNX_INIT_FUNC_DEFS;

	BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, core_id,dpp_am_res_ether_policer,1,policer_id));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * PON MOdule - Start
 */
int
bcm_dpp_am_pon_channel_profile_alloc(
        int unit,
        uint32 flags, /* flags should be SW_STATE_RES_ALLOC_WITH_ID; */
        uint16 *pon_channel_profile)
{
    int elem = *pon_channel_profile;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_pon_channel_profile, flags, 1, &elem);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_pon_channel_profile_dealloc(
        int unit,
        uint16 pon_channel_profile)
{
    int rv;
    int elem = pon_channel_profile;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_pon_channel_profile, 1, elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_pon_channel_profile_is_alloced(
        int unit,
        uint16 pon_channel_profile)
{
    int elem = pon_channel_profile;

    return dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_pon_channel_profile, 1, elem);
}
/*
 * PON MOdule - End
 */


/*
 * Ingress and Egress VLAN edit
 */
int
bcm_dpp_am_vlan_edit_action_id_alloc(
    int unit,
    uint32 flags,
    int *action_id)
{
    int rv, res_pool;
    uint32 alloc_flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (flags & BCM_VLAN_ACTION_SET_WITH_ID) {
        alloc_flags = SW_STATE_RES_ALLOC_WITH_ID;
    }

    /* Determine if it's an Ingress or Egress allocation */
    res_pool = (flags & BCM_VLAN_ACTION_SET_INGRESS) ?
        dpp_am_res_vlan_edit_action_ingress : dpp_am_res_vlan_edit_action_egress;

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, res_pool, alloc_flags, 1, action_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_vlan_edit_action_id_dealloc(
    int unit,
    uint32 flags,
    int action_id)
{
    int rv, res_pool;

    BCMDNX_INIT_FUNC_DEFS;

    /* Determine if it's an Ingress or Egress allocation */
    res_pool = (flags & BCM_VLAN_ACTION_SET_INGRESS) ?
        dpp_am_res_vlan_edit_action_ingress : dpp_am_res_vlan_edit_action_egress;

    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, res_pool, 1, action_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_vlan_edit_action_id_is_alloc(
    int unit,
    uint32 flags,
    int action_id)
{
    int res_pool;
    BCMDNX_INIT_FUNC_DEFS;

    /* Determine if it's an Ingress or Egress allocation */
    res_pool = (flags & BCM_VLAN_ACTION_SET_INGRESS) ?
        dpp_am_res_vlan_edit_action_ingress : dpp_am_res_vlan_edit_action_egress;

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, res_pool, 1 ,action_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_vlan_edit_action_id_dealloc_all(
    int unit,
    uint32 flags)
{
    int action_idx, nof_action_ids, nof_reserved_action_ids;

    BCMDNX_INIT_FUNC_DEFS;

    /* Determine if it's an Ingress or Egress allocation */
    if (flags & BCM_VLAN_ACTION_SET_INGRESS) {
        nof_action_ids = SOC_DPP_NOF_INGRESS_VLAN_EDIT_ACTION_IDS(unit);
        nof_reserved_action_ids = SOC_DPP_NOF_INGRESS_VLAN_EDIT_RESERVED_ACTION_IDS(unit);
    } else {
        nof_action_ids = SOC_DPP_NOF_EGRESS_VLAN_EDIT_ACTION_IDS(unit);
        nof_reserved_action_ids = SOC_DPP_NOF_EGRESS_VLAN_EDIT_RESERVED_ACTION_IDS(unit);
    }

    /* Deallocate all the action IDs */
    for (action_idx = nof_reserved_action_ids; action_idx < nof_action_ids; action_idx++) {
        if (bcm_dpp_am_vlan_edit_action_id_is_alloc(unit, flags, action_idx) == BCM_E_EXISTS) {
            BCMDNX_IF_ERR_EXIT(bcm_dpp_am_vlan_edit_action_id_dealloc(unit, flags, action_idx));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * End of Ingress and Egress VLAN edit
 */


/*
 * virtual nickname 
 */
int
bcm_dpp_am_trill_virtual_nick_name_alloc(
    int unit,
    uint32 flags,
    int *virtual_nick_name_index)
{
    int rv;
    uint32 alloc_flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_trill_virtual_nick_name, alloc_flags, 1, virtual_nick_name_index);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_trill_virtual_nick_name_dealloc(
    int unit,
    uint32 flags,
    int virtual_nick_name_index)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_trill_virtual_nick_name, 1, virtual_nick_name_index);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_trill_virtual_nick_name_is_alloc(
    int unit,
    uint32 flags,
    int virtual_nick_name_index)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_trill_virtual_nick_name, 1 ,virtual_nick_name_index));
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * End of virtual nickname 
 */

STATIC int
bcm_dpp_am_global_lif_flags_decode(uint32 flags, uint8 *is_ingress, uint8 *is_egress){
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    *is_ingress = flags & BCM_DPP_AM_FLAG_ALLOC_INGRESS;
    *is_egress = flags & BCM_DPP_AM_FLAG_ALLOC_EGRESS;

    /* Check that at least one of is_ingress or is_egress is set */
    if ((!*is_ingress && !*is_egress)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Global lif allocation flags must contain either ingress or egress.")));
    }

exit:
    BCMDNX_FUNC_RETURN_NO_UNIT;
}

/* Global lif */
int 
bcm_dpp_am_global_lif_alloc(int unit, uint32 flags, int *global_lif){
    uint8 is_ingress, is_egress;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(bcm_dpp_am_global_lif_flags_decode(flags, &is_ingress, &is_egress));

    flags &= ~(BCM_DPP_AM_FLAG_ALLOC_INGRESS | BCM_DPP_AM_FLAG_ALLOC_EGRESS); /* Remove the ingress egress flags */

    if (is_ingress && is_egress) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_global_sync_lif_internal_alloc(unit, flags, global_lif));
    } else if (is_egress) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_global_lif_non_sync_internal_alloc(unit, flags, FALSE, global_lif));
    } else /*is_ingress*/ { 
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_global_lif_non_sync_internal_alloc(unit, flags, TRUE, global_lif););
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dpp_am_global_lif_is_alloced(int unit, uint32 flags, int global_lif){
    uint8 is_ingress, is_egress;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(bcm_dpp_am_global_lif_flags_decode(flags, &is_ingress, &is_egress));

    if (is_ingress && is_egress) {
        /* Not supported. User should check for ingress and egress seperately. */
        BCM_RETURN_VAL_EXIT(BCM_E_UNAVAIL); 
    } else if (is_egress) {
        BCM_RETURN_VAL_EXIT(_bcm_dpp_am_global_egress_lif_internal_is_allocated(unit, global_lif));
    } else /*is_ingress*/ { 
        BCM_RETURN_VAL_EXIT(_bcm_dpp_am_global_ingress_lif_internal_is_allocated(unit, global_lif));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dpp_am_global_lif_dealloc(int unit, uint32 flags, int global_lif){
    uint8 is_ingress, is_egress;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(bcm_dpp_am_global_lif_flags_decode(flags, &is_ingress, &is_egress));

    if (is_ingress && is_egress) {
        BCM_RETURN_VAL_EXIT(_bcm_dpp_am_global_sync_lif_internal_dealloc(unit, global_lif));
    } else if (is_egress) {
        BCM_RETURN_VAL_EXIT(_bcm_dpp_am_global_lif_non_sync_internal_dealloc(unit, FALSE, global_lif));
    } else /*is_ingress*/ { 
        BCM_RETURN_VAL_EXIT(_bcm_dpp_am_global_lif_non_sync_internal_dealloc(unit, TRUE, global_lif));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Global lif functions - end. 
 */

/* 
 * trill eep functions - start
 */ 

int
bcm_dpp_am_trill_eep_alloc(
   int unit, 
   uint32 types, 
   uint32 flags, 
   int *eep)
{
    int rv = BCM_E_NONE;
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_trill; 
    if (types == _BCM_DPP_AM_TRILL_EEP_TYPE_ROO) {
        allocation_info.application_type = bcm_dpp_am_egress_encap_app_trill_roo; 
    }  
    rv = _bcm_dpp_am_egress_encap_alloc_align(unit, 
                                              &allocation_info,
                                              flags,
                                              2, /* align*/
                                              0, /* offset */
                                              2, /* count */
                                              eep);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dpp_am_trill_eep_is_alloc(
   int unit, 
   int eep)
{
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_trill; 

    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_is_allocated(unit,&allocation_info,2,eep));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dpp_am_trill_eep_dealloc(
   int unit, 
   int eep) 
{
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    BCMDNX_INIT_FUNC_DEFS; 
    
    sal_memset(&allocation_info,0,sizeof(allocation_info));

    allocation_info.pool_id = dpp_am_res_eep_trill; 

    BCM_RETURN_VAL_EXIT(_bcm_dpp_am_egress_encap_dealloc(unit,&allocation_info,2,eep));

exit:
    BCMDNX_FUNC_RETURN; 
}


/*
 * trill eep  functions - end. 
 */

/* 
 * encap intpri color functions - start
 */ 


int
bcm_dpp_am_map_encap_intpri_color_alloc(
        int unit,
        uint32 flags,
        int *qos_id)
{
    int rv = BCM_E_NONE;
    int elem = *qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_map_encap_intpri_color,flags,1,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    *qos_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_map_encap_intpri_color_dealloc(
        int unit,
        uint32 flags,
        int qos_id)
{
    int rv = BCM_E_NONE;
    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_map_encap_intpri_color,1,elem);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_map_encap_intpri_color_is_alloced(
        int unit,
        int qos_id)
{

    int elem = qos_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_map_encap_intpri_color,1,elem));
exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * encap intpri color functions - End
 */ 

/*
 * IPSEC
 */
int
bcm_dpp_am_ipsec_sa_id_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *sa_id)
{
    int elem = *sa_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_ipsec_sa_id,flags,1,&elem));
    *sa_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ipsec_sa_id_dealloc(
        int unit,
        uint32 sa_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_ipsec_sa_id, 1, sa_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ipsec_sa_id_is_alloced(
        int unit,
        uint32 sa_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_ipsec_sa_id, 1, sa_id));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ipsec_tunnel_id_alloc(
        int unit,
        uint32 flags, /* flags may be SW_STATE_RES_ALLOC_WITH_ID or 0; */
        uint32  *tunnel_id)
{
    int elem = *tunnel_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_ipsec_tunnel_id,flags,1,&elem));
    *tunnel_id = elem;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ipsec_tunnel_id_dealloc(
        int unit,
        uint32 tunnel_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_ipsec_tunnel_id, 1, tunnel_id));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_ipsec_tunnel_id_is_alloced(
        int unit,
        uint32 tunnel_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_ipsec_tunnel_id, 1, tunnel_id));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * IPSEC - End
 */

/* 
 * Template management - Start
 */ 
/* Util functions compare */
int
_bcm_dpp_am_template_memcpy_to_stream(const void* data, void *to_stream, size_t data_size)
{
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    sal_memcpy(to_stream,data,data_size);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_memcpy_from_stream(void* data,const void* from_stream, size_t data_size)
{
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    sal_memcpy(data,from_stream,data_size);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* User defined streams */
int 
 _bcm_dpp_am_template_user_defined_trap_to_stream(const void* data,void* to_stream, size_t data_size)    
{
    bcm_dpp_user_defined_traps_t *user_defined_trap = (bcm_dpp_user_defined_traps_t *)data;
    int flags;
    uint32 *out_buffer = (uint32*)to_stream;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    sal_memset(out_buffer,0x0,data_size);
    
    out_buffer[0] = user_defined_trap->trap.dest_info.frwrd_dest.dest_id;
    out_buffer[1] = user_defined_trap->trap.dest_info.frwrd_dest.type;
    flags = user_defined_trap->trap.processing_info.is_control;
    flags |= user_defined_trap->trap.processing_info.is_trap << 1;
    out_buffer[2] = flags;
    out_buffer[3] = user_defined_trap->snoop_cmd;

    /*Added for sync*/
    out_buffer[4] = user_defined_trap->gport_trap_id;
    out_buffer[5] = user_defined_trap->ud_unique;



    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int 
 _bcm_dpp_am_template_user_defined_trap_from_stream(void* data,const void* from_stream, size_t data_size)
{
    int flags;
    bcm_dpp_user_defined_traps_t *user_defined_trap = (bcm_dpp_user_defined_traps_t *)data;
    const uint32 *out_buffer = (const uint32*)from_stream;

   
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&(user_defined_trap->trap));
    user_defined_trap->trap.dest_info.frwrd_dest.dest_id = out_buffer[0];
    user_defined_trap->trap.dest_info.frwrd_dest.type = out_buffer[1];

    flags = out_buffer[2];
    user_defined_trap->snoop_cmd = out_buffer[3];

    user_defined_trap->trap.processing_info.is_control = flags & 1;
    user_defined_trap->trap.processing_info.is_trap = flags & 2;

    /*Added for sync*/
    user_defined_trap->gport_trap_id = out_buffer[4];
    user_defined_trap->ud_unique = out_buffer[5];



    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Snoop command streams */
int 
  _bcm_dpp_am_template_snoop_cmd_to_stream(const void* data,void* to_stream, size_t data_size)   
{
    uint32 *out_buffer = (uint32*)to_stream;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    sal_memset(out_buffer,0x0,data_size);

    sal_memcpy(out_buffer, data, data_size);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int 
  _bcm_dpp_am_template_discount_cls_from_stream(void* data,const void* from_stream, size_t data_size)
{
    SOC_TMC_ITM_CR_DISCOUNT_INFO *discount_info = (SOC_TMC_ITM_CR_DISCOUNT_INFO *)data;
    const int *in_buffer = (const int*)from_stream;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    SOC_TMC_ITM_CR_DISCOUNT_INFO_clear(discount_info);
    discount_info->discount = in_buffer[0];
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Discount class streams */
int 
  _bcm_dpp_am_template_discount_cls_to_stream(const void* data,void* to_stream, size_t data_size)    
{
    const SOC_TMC_ITM_CR_DISCOUNT_INFO *discount_info = (const SOC_TMC_ITM_CR_DISCOUNT_INFO *)data;
    int *out_buffer = (int*)to_stream;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    sal_memset(out_buffer,0x0,data_size);

    out_buffer[0] = discount_info->discount;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int 
  _bcm_dpp_am_template_snoop_cmd_from_stream(void* data,const void* from_stream, size_t data_size)
{
    bcm_dpp_snoop_t *snoop_struct = (bcm_dpp_snoop_t*)data;

    const uint32 *in_buffer = (const uint32*)from_stream;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&snoop_struct->snoop_action_info);

    sal_memcpy(snoop_struct, in_buffer, data_size);    

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* These arrays are used to intialize the shr_template callback arrays with the bcm template callbacks. */
shr_template_to_stream_t _bcm_dpp_am_template_to_stream_arr[_bcm_dpp_am_template_hash_compare_cb_idx_count] = {
    _bcm_dpp_am_template_memcpy_to_stream,
    _bcm_dpp_am_template_user_defined_trap_to_stream,
    _bcm_dpp_am_template_snoop_cmd_to_stream,
    _bcm_dpp_am_template_discount_cls_to_stream
};

shr_template_from_stream_t _bcm_dpp_am_template_from_stream_arr[_bcm_dpp_am_template_hash_compare_cb_idx_count] = {
    _bcm_dpp_am_template_memcpy_from_stream,
    _bcm_dpp_am_template_user_defined_trap_from_stream,
    _bcm_dpp_am_template_snoop_cmd_from_stream,
    _bcm_dpp_am_template_discount_cls_from_stream
};
                               
/*OAM MP profile template dump callback function */
void _bcm_dpp_am_template_oam_profile_dump(int unit, const void* data)
{

    int i = 0, rv = BCM_E_NONE;
    SOC_PPC_OAM_LIF_PROFILE_DATA *oam_profile;
    char *opcode[SOC_PPC_OAM_OPCODE_MAP_COUNT] = {"BFD", "CCM", "LBR", "LBM", "LTR","LTM","LMR","LMM","DMR","DMM","1DM","SLM_SLR", "AIS","LCK","LINEAR_APS","DEAFULT"};
    int hw_trap_code;

    oam_profile = (SOC_PPC_OAM_LIF_PROFILE_DATA *) data;
    LOG_CLI((BSL_META("  OAM profile data : \n")));
    LOG_CLI((BSL_META("  is_1588 %d \n"), (int)( oam_profile-> is_1588)));
    LOG_CLI((BSL_META("  MEP Profile: \n")));
    LOG_CLI((BSL_META("  count enabled for opcodes: ")));
    for (i = 0; i <= SOC_PPC_OAM_ETHERNET_PDU_DUMMY_MAX_DIAG_OPCODE; i++) {
        if (!SHR_BITGET(oam_profile->mep_profile_data.counter_disable, i)) {
            LOG_CLI((BSL_META("  %d "), i));
        }
    }
    LOG_CLI((BSL_META("\n  Unicast - opcode to trap code mapping: \n")));
    LOG_CLI((BSL_META("  Internal OPCODE: ")));
    for (i = 0; i < SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
        LOG_CLI((BSL_META("%10s "), opcode[i]));
    }
    LOG_CLI((BSL_META("\n  Trap Code      : ")));
    for (i = 0; i < SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
        rv = _bcm_dpp_rx_trap_sw_id_to_hw_id(unit, oam_profile->mep_profile_data.opcode_to_trap_code_unicast_map[i], &hw_trap_code);
        if ( rv != BCM_E_NONE) {
            LOG_CLI((BSL_META(" Error in converting SW to HW trap code")));
            return;
        }
        LOG_CLI((BSL_META("     0x%03x "), hw_trap_code));
    }
    LOG_CLI((BSL_META("\n  Multicast - opcode to trap code mapping: \n")));
    LOG_CLI((BSL_META("  Internal OPCODE: ")));
    for (i = 0; i < SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
        LOG_CLI((BSL_META("%10s "), opcode[i]));
    }
    LOG_CLI((BSL_META("\n  Trap Code      : ")));
    for (i = 0; i < SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
        rv = _bcm_dpp_rx_trap_sw_id_to_hw_id(unit, oam_profile->mep_profile_data.opcode_to_trap_code_multicast_map[i], &hw_trap_code);
        if ( rv != BCM_E_NONE) {
            LOG_CLI((BSL_META(" Error in converting SW to HW trap code")));
            return;
        }
        LOG_CLI((BSL_META("     0x%03x "), hw_trap_code));
    }
    LOG_CLI((BSL_META("\n  MIP profile: \n")));
    LOG_CLI((BSL_META("  count enabled for opcodes: ")));
    for (i = 0; i <= SOC_PPC_OAM_ETHERNET_PDU_DUMMY_MAX_DIAG_OPCODE; i++) {
        if (!SHR_BITGET(oam_profile->mip_profile_data.counter_disable, i)) {
            LOG_CLI((BSL_META("  %d "), i));
        }
    }
    LOG_CLI((BSL_META("\n  Unicast - opcode to trap code mapping: \n")));
    LOG_CLI((BSL_META("  Internal OPCODE: ")));
    for (i = 0; i < SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
        LOG_CLI((BSL_META("%10s "), opcode[i]));
    }
    LOG_CLI((BSL_META("\n  Trap Code      : ")));
    for (i = 0; i < SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
        rv = _bcm_dpp_rx_trap_sw_id_to_hw_id(unit, oam_profile->mip_profile_data.opcode_to_trap_code_unicast_map[i], &hw_trap_code);
        if ( rv != BCM_E_NONE) {
            LOG_CLI((BSL_META(" Error in converting SW to HW trap code")));
            return;
        }
        LOG_CLI((BSL_META("     0x%03x "), hw_trap_code));
    }
    LOG_CLI((BSL_META("\n  Multicast - opcode to trap code mapping: \n")));
    LOG_CLI((BSL_META("  Internal OPCODE: ")));
    for (i = 0; i < SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
        LOG_CLI((BSL_META("%10s "), opcode[i]));
    }
    LOG_CLI((BSL_META("\n  Trap Code      : ")));
    for (i = 0; i < SOC_PPC_OAM_OPCODE_MAP_COUNT; i++) {
        rv = _bcm_dpp_rx_trap_sw_id_to_hw_id(unit, oam_profile->mip_profile_data.opcode_to_trap_code_multicast_map[i], &hw_trap_code);
        if ( rv != BCM_E_NONE) {
            LOG_CLI((BSL_META(" Error in converting SW to HW trap code")));
            return;
        }
        LOG_CLI((BSL_META("     0x%03x "), hw_trap_code));
    }

   
    LOG_CLI((BSL_META("\n  Flags: 0x%03x \n"), oam_profile->flags));
    LOG_CLI((BSL_META("  MP type: 0x%03x \n"), oam_profile->mp_type_passive_active_mix));
    LOG_CLI((BSL_META("  Is piggybacked: 0x%03x \n"), oam_profile->is_piggybacked));
    LOG_CLI((BSL_META("  Is slm: 0x%03x \n"), oam_profile->is_slm));
    LOG_CLI((BSL_META("  Is default: 0x%03x \n"), oam_profile->is_default));

}

/*OAM ETH1731 profile template dump callback function */
void _bcm_dpp_am_template_oam_eth1731_profile_dump(int unit, const void* data)
{

    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *eth1731_profile;

    eth1731_profile = (SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *)data;
    LOG_CLI((BSL_META("  OAM ETH1731 profile data : \n")));
    LOG_CLI((BSL_META(" piggy_back_lm    %u\n"), eth1731_profile->piggy_back_lm));
    LOG_CLI((BSL_META(" slm_lm    %u\n"), eth1731_profile->slm_lm));
    LOG_CLI((BSL_META(" maid_check_dis    %u\n"), eth1731_profile->maid_check_dis));
    LOG_CLI((BSL_META(" report_mode    %u\n"), eth1731_profile->report_mode));
    LOG_CLI((BSL_META(" rdi_gen_method    %u\n"), eth1731_profile->rdi_gen_method));
    LOG_CLI((BSL_META(" lmm_da_oui_prof    %u\n"), eth1731_profile->lmm_da_oui_prof));
    LOG_CLI((BSL_META(" dmm_rate    %u\n"), eth1731_profile->dmm_rate));
    LOG_CLI((BSL_META(" lmm_rate    %u\n"), eth1731_profile->lmm_rate));
    LOG_CLI((BSL_META(" opcode_0_rate    %u\n"), eth1731_profile->opcode_0_rate));
    LOG_CLI((BSL_META(" opcode_1_rate    %u\n"), eth1731_profile->opcode_1_rate));
    LOG_CLI((BSL_META(" lmm_offset    %u\n"), eth1731_profile->lmm_offset));
    LOG_CLI((BSL_META(" lmr_offset    %u\n"), eth1731_profile->lmr_offset));
    LOG_CLI((BSL_META(" dmm_offset    %u\n"), eth1731_profile->dmm_offset));
    LOG_CLI((BSL_META(" dmr_offset    %u\n"), eth1731_profile->dmr_offset));
    LOG_CLI((BSL_META(" ccm_cnt    %u\n"), eth1731_profile->ccm_cnt));
    LOG_CLI((BSL_META(" dmm_cnt    %u\n"), eth1731_profile->dmm_cnt));
    LOG_CLI((BSL_META(" lmm_cnt    %u\n"), eth1731_profile->lmm_cnt));
    LOG_CLI((BSL_META(" op_0_cnt    %u\n"), eth1731_profile->op_0_cnt));
    LOG_CLI((BSL_META(" op_1_cnt    %u\n"), eth1731_profile->op_1_cnt));

}

shr_template_print_func_t _bcm_dpp_am_template_print_func[dpp_am_template_count] = { NULL };

/* 
 * Initialize template management pool data structures for each pool type (not including some cosq related pools
 * which handle warmboot within the cosq module)
 */
int
_bcm_dpp_template_setup(int unit, int template_id, int core_id, int pool_id) {
    int rv = BCM_E_NONE;
    char *desc_str = "Unknown template";
    bcm_dpp_am_template_info_t t_info;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    sal_memset(&t_info, 0, sizeof(bcm_dpp_am_template_info_t));
    t_info.pool_id = pool_id;
    t_info.template_id =  template_id;
    t_info.core_id = core_id;
    t_info.hashExtra = _bcm_dpp_am_template_hash_compare_cb_idx_memcpy;

    switch (template_id) 
    {
        case dpp_am_template_egress_thresh:
            /* mostly direct allocation */
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_SIZE;
            desc_str = "Egress threshold for drop and flow control";
            break;
    case dpp_am_template_mirror_action_profile:
        /* mostly direct allocation */
        t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
        t_info.start = DPP_MIRROR_ACTION_NDX_MIN;
        t_info.count = DPP_MIRROR_ACTION_NDX_MAX;
        t_info.max_entities = _DPP_AM_TEMPLATE_MIRROR_ACTION_MAX_ENTITIES;
        t_info.data_size = _DPP_AM_TEMPLATE_MIRROR_ACTION_SIZE;
        desc_str = "Mirror profile";
        break;
    case dpp_am_template_egress_interface_unicast_thresh:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_SIZE;
            desc_str = "Egress threshold for interface uc flow control";
            break;
        case dpp_am_template_egress_interface_multicast_thresh:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_SIZE;
            desc_str = "Egress threshold for interface mc flow control";
            break;
        case dpp_am_template_user_defined_traps:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_SIZE;
            t_info.hashExtra = _bcm_dpp_am_template_hash_compare_cb_idx_user_defined_trap;
            desc_str = "User defined traps Per UD trap";
            break;
        case dpp_am_template_snoop_cmd:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_SNOOP_CMD_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_SNOOP_CMD_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_SNOOP_CMD_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_SNOOP_CMD_SIZE;
            t_info.hashExtra = _bcm_dpp_am_template_hash_compare_cb_idx_snoop_cmd;
            desc_str = "Snoop info Per PP port";
            break;
        case dpp_am_template_tpid_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_TPID_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_TPID_PROFILE_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_TPID_PROFILE_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_TPID_PROFILE_SIZE;
            desc_str = "TPID profile Per PP port";
            break;
        case dpp_am_template_egress_queue_mapping:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_SIZE;
            desc_str = "Egress queue mapping Per TM port";
            break;
        case dpp_am_template_nrdy_threshold:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_NRDY_THRESHOLD_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_NRDY_THRESHOLD_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_NRDY_THRESHOLD_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_NRDY_THRESHOLD_SIZE;
            desc_str = "NRDY threshold profile";
            break;                     
        case dpp_am_template_vlan_edit_profile_mapping:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_SIZE;
            desc_str = "VLAN Edit Profile per In-AC";
            break;
        case dpp_am_template_vlan_edit_profile_mapping_eg:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_SIZE;
            desc_str = "VLAN Edit Profile per Out-AC";
            break;
        case dpp_am_template_trap_reserved_mc:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_SIZE;
            desc_str = "traps of L2 reserved MC";
            break;
        case dpp_am_template_l2cp_egress_profile:
             t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
             t_info.start = _DPP_AM_TEMPLATE_EGRESS_L2CP_PROFILE_LOW_ID;
             t_info.count = soc_property_get(unit, spn_NOF_L2CP_EGRESS_PROFILES_MAX, 1);
             t_info.max_entities = _DPP_AM_TEMPLATE_EGRESS_L2CP_PROFILE_MAX_ENTITIES(unit);
             t_info.data_size = _DPP_AM_TEMPLATE_EGRESS_L2CP_PROFILE_SIZE;
             desc_str ="egress l2cp profile";
             break;
        case dpp_am_template_vsi_egress_profile:
             t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
             t_info.start = _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_LOW_ID;
             t_info.count = _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_COUNT;
             t_info.max_entities = _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_MAX_ENTITIES(unit);
             t_info.data_size = _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_SIZE;
             desc_str ="egress vsi profile";
             break; 
        case dpp_am_template_vsi_ingress_profile:
             t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
             t_info.start = _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_LOW_ID;
             t_info.count = _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_COUNT;
             t_info.max_entities = _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_MAX_ENTITIES(unit);
             t_info.data_size = _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_SIZE;
             desc_str ="ingress vsi profile";
             break; 
        case dpp_am_template_trap_egress:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_TRAP_EGRESS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_TRAP_EGRESS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_TRAP_EGRESS_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_TRAP_EGRESS_SIZE;
            desc_str = "traps of Egress";
            break;
        case dpp_am_template_reserved_mc:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_L2_RSRVD_MC_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_L2_RSRVD_MC_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_L2_RSRVD_MC_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_L2_RSRVD_MC_SIZE;
            desc_str = "L2 reserved MC ";
            break;
        case dpp_am_template_tpid_class:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_SIZE;
            desc_str = "PORT TPID CLASS";
            break;
        case dpp_am_template_tpid_class_eg:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_SIZE;
            desc_str = "PORT TPID CLASS FOR EGRESS";
            break;
        case dpp_am_template_port_tpid_class_egress_acceptable_frame_type:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_SIZE;
            desc_str = "EGRESS ACCEPTABLE FRAME TYPE ";
            break;
        case dpp_am_template_oam_tx_priority:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_OAM_TX_PRIORITY_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_OAM_TX_PRIORITY_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_OAM_TX_PRIORITY_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_OAM_TX_PRIORITY_PROFILE_SIZE;
            desc_str ="oam_tx_priority";
            break;
        case dpp_am_template_bfd_ip_dip:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_BFD_IP_DIP_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_BFD_IP_DIP_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_BFD_IP_DIP_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_BFD_IP_DIP_PROFILE_SIZE;
            desc_str ="bfd_ipv4_dip";
            break; 
        case dpp_am_template_oam_sd_sf_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_OAM_SD_SF_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_OAM_SD_SF_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_OAM_SD_SF_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_OAM_SD_SF_PROFILE_SIZE;
            desc_str ="OAM sd sf profile";
            break;
        case dpp_am_template_oam_sa_mac:
            if (SOC_IS_JERICHO(unit)) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_OAM_SA_MSB_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_SA_MSB_NDX_COUNT;
                t_info.max_entities = _DPP_AM_TEMPLATE_OAM_SA_MSB_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_OAM_SA_MSB_SIZE;
                desc_str ="oam SA MAC address profile";
            } else {
                BCM_EXIT;
            }
          break;             
        case dpp_am_template_oam_icc_map:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_OAM_ICC_MAP_NDX_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_OAM_ICC_MAP_NDX_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_OAM_ICC_MAP_NDX_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_OAM_ICC_MAP_NDX_SIZE;
            desc_str ="oam icc index map";
            break; 
        case dpp_am_template_oam_punt_event_hendling_profile:
            if (SOC_IS_ARADPLUS(unit)) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_COUNT;
                t_info.max_entities = _DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_SIZE;
                desc_str ="oam punt event handling profile";
            } else {
                BCM_EXIT;
            }
          break; 

        case dpp_am_template_bfd_flags_profile:
            if (SOC_IS_ARADPLUS(unit)) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_BFD_FLAGS_PROFILE_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_BFD_FLAGS_PROFILE_COUNT;
                t_info.max_entities = _DPP_AM_TEMPLATE_BFD_FLAGS_PROFILE_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_BFD_FLAGS_PROFILE_SIZE;
                desc_str ="BFD flags profile";
            } else {
                BCM_EXIT;
            }
          break; 

        case dpp_am_template_oam_mep_profile_non_accelerated:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_LOW_ID;
            t_info.count = (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_MODE(unit)) ?
                            _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_COUNT_ARAD_PLUS :
                            _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_COUNT;

            t_info.count -= SOC_PPD_OAM_IS_CLASSIFIER_JERICHO_PLUS_MODE(unit); /* In QAX, the last profile (0xf) is statically allocated for 'Passive SLM' */
            t_info.max_entities = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_SIZE;
            desc_str ="oam mep profile non accelerated";
            _bcm_dpp_am_template_print_func[dpp_am_template_oam_mep_profile_non_accelerated] = _bcm_dpp_am_template_oam_profile_dump;
            break; 
        case dpp_am_template_oam_mep_profile_accelerated:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_SIZE;
            desc_str ="oam mep profile accelerated";
            _bcm_dpp_am_template_print_func[dpp_am_template_oam_mep_profile_accelerated] = _bcm_dpp_am_template_oam_profile_dump;
            break; 

        case dpp_am_template_bfd_mep_profile_accelerated:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_ACC_SIZE;
            desc_str ="bfd mep profile accelerated";
            break; 

        case dpp_am_template_bfd_mep_profile_non_accelerated:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_LOW_ID;
            t_info.count = (SOC_PPD_OAM_IS_CLASSIFIER_ADVANCED_MODE(unit) || SOC_IS_JERICHO(unit)) ? _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_COUNT_ARAD_PLUS : _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_BFD_MEP_PROFILE_NON_ACC_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_OAM_MEP_PROFILE_NON_ACC_SIZE;
            desc_str ="bfd mep profile non accelerated";
            break; 
             
        case dpp_am_template_mpls_pwe_push_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_BFD_PUSH_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_BFD_PUSH_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_BFD_PUSH_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_BFD_PUSH_PROFILE_SIZE;
            desc_str ="bfd push profile";
            break; 
        case dpp_am_template_bfd_req_interval_pointer:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_BFD_REQ_INTERVAL_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_BFD_REQ_INTERVAL_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_BFD_REQ_INTERVAL_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_BFD_REQ_INTERVAL_SIZE;
            desc_str ="bfd req interval pointer";
            break; 
        case dpp_am_template_bfd_tos_ttl_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_BFD_TOS_TTL_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_BFD_TOS_TTL_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_BFD_TOS_TTL_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_BFD_TOS_TTL_PROFILE_SIZE;
            desc_str ="bfd tos ttl profile";
            break; 
        case dpp_am_template_bfd_src_ip_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_BFD_SRC_IP_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_BFD_SRC_IP_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_BFD_SRC_IP_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_BFD_SRC_IP_PROFILE_SIZE;
            desc_str ="bfd src ip profile";
            break; 
        case dpp_am_template_oamp_pe_gen_mem_maid_48:
            if (SOC_IS_JERICHO(unit)) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_MAID_48_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_MAID_48_COUNT;
                t_info.max_entities = _DPP_AM_TEMPLATE_MAID_48_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_MAID_48_SIZE;
                desc_str = "oamp pe gen mem maid 48";
            } else if (SOC_IS_ARADPLUS(unit)) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_MAID_48_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_MAID_48_COUNT_ARAD;
                t_info.max_entities = _DPP_AM_TEMPLATE_MAID_48_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_MAID_48_SIZE;
                desc_str = "oamp pe gen mem maid 48";
            } else {
                BCM_EXIT;
            }
          break;
        case dpp_am_template_oam_lmm_nic_tables_profile:
            if (SOC_IS_ARADPLUS(unit) && 
                !(_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit))) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_OAM_LM_DA_NIC_TABLES_COUNT;
                t_info.max_entities = _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_SIZE;
                desc_str = "oam lm nic da tables";
            } else {
                BCM_EXIT;
            }
            break; 
        case dpp_am_template_oam_lmm_oui_tables_profile:            
            if (SOC_IS_ARADPLUS(unit) && 
                !(_BCM_OAM_IS_CUSTOM_FEATURE_OAMP_FLEXIBLE_DA_SET(unit))) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_OAM_LM_DA_OUI_TABLES_COUNT;
                t_info.max_entities = _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_OAM_LM_DA_TABLES_SIZE;
                desc_str ="oam lm oui da tables";
            } else {
                BCM_EXIT;
            }
          break;
        case dpp_am_template_oam_eth1731_profile:
            if (SOC_IS_ARADPLUS(unit)) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_COUNT;
                t_info.max_entities = _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_SIZE;
                desc_str ="oam eth1731 profile";
                _bcm_dpp_am_template_print_func[dpp_am_template_oam_eth1731_profile] = _bcm_dpp_am_template_oam_eth1731_profile_dump;
            } else {
                BCM_EXIT;
            }
          break;
        case dpp_am_template_oam_local_port_2_system_port:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_SIZE;
            desc_str ="oam localPort to SysPort mapping";
            break;
        case dpp_am_template_oam_oamp_pe_gen_mem:
            if (SOC_IS_ARADPLUS(unit)) {
                t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
                t_info.start = _DPP_AM_TEMPLATE_OAM_OAMP_PE_GEN_MEM_LOW_ID;
                t_info.count = _DPP_AM_TEMPLATE_OAM_OAMP_PE_GEN_MEM_COUNT;
                t_info.max_entities = _DPP_AM_TEMPLATE_OAM_OAMP_PE_GEN_MEM_MAX_ENTITIES;
                t_info.data_size = _DPP_AM_TEMPLATE_OAM_OAMP_PE_GEN_MEM_SIZE;
                desc_str ="oam oamp pe gen mem";
            } else {
                BCM_EXIT;
            }
          break;
        case dpp_am_template_prog_trap:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_PROG_TRAP_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_PROG_TRAP_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_PROG_TRAP_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_PROG_TRAP_SIZE;
            desc_str ="pool prog trap";
            break;
        case dpp_am_template_queue_rate_cls:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_QUEUE_RATE_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_QUEUE_RATE_CLS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_QUEUE_RATE_CLS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_QUEUE_RATE_CLS_SIZE;
            desc_str = "Queue Rate class";
            break;
        case dpp_am_template_system_red_dp_pr:
            t_info.manager       = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_SYS_RED_DP_PR_LOW_ID;
            t_info.count        = _DPP_AM_TEMPLATE_SYS_RED_DP_PR_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_SYS_RED_DP_PR_MAX_ENTITIES(unit);
            t_info.data_size    = _DPP_AM_TEMPLATE_SYS_RED_DP_PR_SIZE;
            desc_str = "System RED drop probability";
            break;
    case dpp_am_template_vsq_pg_tc_mapping:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING;
            desc_str = "PG TC mapping profile";
            break;
        case dpp_am_template_vsq_rate_cls_ct:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VSQ_A_RATE_CLS_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_MAX_ENTITIES(unit, SOC_TMC_ITM_VSQ_GROUP_CTGRY);
            t_info.data_size = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_SIZE(SOC_TMC_ITM_VSQ_GROUP_CTGRY);
            desc_str = "Category VSQ Rate class";
            break;
        case dpp_am_template_vsq_rate_cls_cttc:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_MAX_ENTITIES(unit, SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS);
            t_info.data_size = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_SIZE(SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS);
            desc_str = "Traffic class VSQ Rate class";
            break;
        case dpp_am_template_vsq_rate_cls_ctcc:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_MAX_ENTITIES(unit, SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS);
            t_info.data_size = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_SIZE(SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS);
            desc_str = "Connection class VSQ Rate class";
            break;
        case dpp_am_template_vsq_rate_cls_pp:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_MAX_ENTITIES(unit, SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG);
            t_info.data_size = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_SIZE(SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG);
            desc_str = "Statistics VSQ Rate class";
            break;
        case dpp_am_template_vsq_rate_cls_src_port:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_MAX_ENTITIES(unit, SOC_TMC_ITM_VSQ_GROUP_SRC_PORT);
            t_info.data_size = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_SIZE(SOC_TMC_ITM_VSQ_GROUP_SRC_PORT);
            desc_str = "Source Port VSQ Rate class";
            break;
        case dpp_am_template_vsq_rate_cls_pg:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_MAX_ENTITIES(unit, SOC_TMC_ITM_VSQ_GROUP_PG);
            t_info.data_size = _DPP_AM_TEMPLATE_VSQ_RATE_CLS_SIZE(SOC_TMC_ITM_VSQ_GROUP_PG);
            desc_str = "PG VSQ Rate class";
            break;
        case dpp_am_template_queue_discount_cls:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_SIZE;
            t_info.hashExtra = _bcm_dpp_am_template_hash_compare_cb_idx_discount_cls;
            desc_str = "Queue discount class";
            break;
        case dpp_am_template_pp_port_discount_cls:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_SIZE;
            desc_str = "In pport discount class";
            break;
        case dpp_am_template_egress_port_discount_cls_type_raw:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_SIZE;
            desc_str = "Egress port discount class type raw";
            break;
        case dpp_am_template_egress_port_discount_cls_type_cpu:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_SIZE;
            desc_str = "Egress port discount class type cpu";
            break;
        case dpp_am_template_egress_port_discount_cls_type_eth:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_SIZE;
            desc_str = "Egress port discount class type eth";
            break;
        case dpp_am_template_egress_port_discount_cls_type_tm:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_EGRESS_PORT_DISCNT_CLS_SIZE;
            desc_str = "Egress port discount class type tm";
            break;
        case dpp_am_template_cosq_port_hr_flow_control:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_MAX_ENTRIES_PER_DEVICE(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_SIZE;
            desc_str = "Cosq port hr fc";
            break;
        case dpp_am_template_cosq_sched_class:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_COSQ_SCHED_CLASS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_COSQ_SCHED_CLASS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_COSQ_SCHED_CLASS_MAX_ENTRIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_COSQ_SCHED_CLASS_SIZE;
            desc_str = "sched class";
            break;
            
        case dpp_am_template_mpls_push_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_LOW_ID(unit);
            t_info.count = _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_MPLS_PUSH_PROFILE_SIZE;
            desc_str = "push profile";
            break;
        case dpp_am_template_lif_term_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_SIZE;
            desc_str = "lif termination profile";
            break;
        case dpp_am_template_lif_term_pw_with_cw_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_SIZE;
            desc_str = "lif termination profile for pw with cw";
            break;
        case dpp_am_template_port_mact_sa_drop_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_SIZE;
            desc_str = "port mact sa drop profile";
            break;
        case dpp_am_template_port_mact_da_unknown_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_SIZE;
            desc_str = "port mact da unknown profile";
            break;
        case dpp_am_template_meter_profile_a_low:
        case dpp_am_template_meter_profile_b_low:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_METER_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_METER_PROFILE_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_METER_PROFILE_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_METER_PROFILE_SIZE;
            desc_str = "meter profile";
            break;
    
        case dpp_am_template_fabric_tdm_link_ptr:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_SIZE;
            desc_str = "TDM direct routing";
            break;
        case dpp_am_template_ingress_flow_tc_mapping:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_INGRESS_FLOW_TC_MAPPING_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_INGRESS_FLOW_TC_MAPPING_COUNT;
            t_info.max_entities = SOC_DPP_DEFS_GET(unit, nof_flows);
            t_info.data_size = _DPP_AM_TEMPLATE_INGRESS_FLOW_TC_MAPPING_CLS_SIZE;
            desc_str = "Ingress Flow TC Mapping Profiles";
            break;
        case dpp_am_template_ingress_uc_tc_mapping:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_MAX_ENTITIES(unit);
            t_info.data_size = _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_CLS_SIZE;
            desc_str = "Ingress UC TC Mapping Profiles";
            break;
        case dpp_am_template_fc_generic_pfc_mapping:
        case dpp_am_template_fc_generic_pfc_mapping_c0:
        case dpp_am_template_fc_generic_pfc_mapping_c1:
        case dpp_am_template_fc_generic_pfc_mapping_c2:
        case dpp_am_template_fc_generic_pfc_mapping_c3:
        case dpp_am_template_fc_generic_pfc_mapping_c4:
        case dpp_am_template_fc_generic_pfc_mapping_c5:
        case dpp_am_template_fc_generic_pfc_mapping_c6:
        case dpp_am_template_fc_generic_pfc_mapping_c7:  
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_CLS_SIZE;
            desc_str = "FC PFC Generic Bitmap Mapping Profiles";
            break;
        case dpp_am_template_l2_event_handle:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_L2_EVENT_HANDLE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_L2_EVENT_HANDLE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_L2_EVENT_HANDLE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_L2_EVENT_HANDLE_SIZE;
            desc_str = "L2 event handle profiles";
            break;
        case dpp_am_template_l2_vsi_learn_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_SIZE;
            desc_str = "L2 learn Profiles";
            break;
        case dpp_am_template_l2_flooding_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_SIZE;
            desc_str = "Flooding per LIF";
            break;
        case dpp_am_template_ip_tunnel_tos:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_SIZE;
            desc_str = "IP tunnel TOS";
            break;
        case dpp_am_template_ip_tunnel_src_ip:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_SIZE;
            desc_str = "IP tunnel SRC IP";
            break;
        case dpp_am_template_ip_tunnel_ttl:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_SIZE;
            desc_str = "IP tunnel TTL";
          break;
        case dpp_am_template_vlan_port_protocol_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_SIZE;
            desc_str = "VLAN port protocol";
            break;
        case dpp_am_template_ttl_scope_index:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_TTL_SCOPE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_TTL_SCOPE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_TTL_SCOPE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_TTL_SCOPE_SIZE;
            desc_str = "TTL scope";
            break;
         case dpp_am_template_cnm_queue_profile:
            t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start = _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_LOW_ID;
            t_info.count = _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_MAX_ENTITIES;
            t_info.data_size = _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_CLS_SIZE;
            desc_str = "CNM Queue Profiles";
            break;
         case dpp_am_template_ptp_port_profile:
             t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
             t_info.start        = _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_LOW_ID;
             t_info.count        = _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_COUNT;
             t_info.max_entities = _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_MAX_ENTITIES;
             t_info.data_size    = _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_CLS_SIZE;
             desc_str = "PTP port Profiles";
             break;

          case dpp_am_template_l3_rif_mac_termination_combination:
             t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
             t_info.start        = _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_LOW_ID;
             t_info.count        = _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_COUNT;
             t_info.max_entities = _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_MAX_ENTITIES;
             t_info.data_size    = _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_CLS_SIZE;
             desc_str = "l3 rif mac termination combination";
             break;

#ifdef BCM_88660_A0
    case dpp_am_template_l3_vrrp:
        if (SOC_IS_ARADPLUS(unit)) {
            t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_LOW_ID;
            t_info.count        = _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_MAX_ENTITIES(unit);
            t_info.global_max   = _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_GLOBAL_MAX(unit);
            t_info.data_size    = _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_CLS_SIZE;
            desc_str = "L3 VRRP cam Profiles";
        } else {
            BCM_EXIT;
       }
       break;
#endif /*BCM_88660_A0 */


    case dpp_am_template_eedb_roo_ll_format_eth_type_index:
        if (SOC_IS_JERICHO(unit)) {
            t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_LOW_ID;
            t_info.count        = _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_MAX_ENTITIES;
            t_info.global_max   = _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_MAX_ENTITIES;
            t_info.data_size    = _DPP_AM_TEMPLATE_EEDB_ROO_LL_FORMAT_ETH_TYPE_INDEX_CLS_SIZE;
            desc_str = "Eth Type Index. For ROO LL format";
        } else {
            BCM_EXIT;
       }
       break;

    case dpp_am_template_out_rif_profile:
        if (SOC_IS_JERICHO_PLUS(unit) && (_DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_MAX_ENTITIES(unit) > 0)) {
            t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_LOW_ID;
            t_info.count        = _DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_COUNT(unit);
            t_info.max_entities = _DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_MAX_ENTITIES(unit);
            t_info.data_size    = _DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_CLS_SIZE;
            desc_str = "L3 OUT-RIF Profile";
        } else {
            BCM_EXIT;
       }
       break;
    case dpp_am_template_ip_tunnel_encapsulation_mode: 
        if (SOC_IS_JERICHO_B0_AND_ABOVE(unit)) {
            t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_LOW_ID;
            t_info.count        = _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_MAX_ENTITIES;
            t_info.global_max   = _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_MAX_ENTITIES;
            t_info.data_size    = _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_CLS_SIZE;
            desc_str = "Encapsulation mode. For IP tunnels";
        } else {
            BCM_EXIT;
        }

        break;
                        
    case dpp_am_template_crps_inpp_port_compensation_profile:
    case dpp_am_template_crps_intm_port_compensation_profile:
        t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
        t_info.start = 0;
        t_info.count = _DPP_CRPS_COMPENSATION_PROFILES_SIZE;
        t_info.max_entities = MAX_PORTS_IN_CORE*2; /* 2- for two commands */
        t_info.data_size = sizeof(int);
        desc_str = "CRPS packetSize compensation profile management";
        break;
    case dpp_am_template_stat_interface_ing_port_compensation_profile:
    case dpp_am_template_stat_interface_egr_port_compensation_profile:
        t_info.manager = SHR_TEMPLATE_MANAGE_HASH;
        t_info.start = 0;
        t_info.count = _DPP_STAT_INTERFACE_COMPENSATION_PROFILES_SIZE;
        t_info.max_entities = MAX_PORTS_IN_CORE*2; /* 2- for two commands */
        t_info.data_size = sizeof(int);
        desc_str = "Stat intf packetSize compensation profile mgmt";
        break;
    case dpp_am_template_scheduler_adjust_size_final_delta: 
        if (SOC_IS_JERICHO(unit)) {
            t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_PROFILE_LOW_ID;
            t_info.count        = _DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_PROFILE_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_MAX_ENTITIES;
            t_info.data_size    = _DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_CLS_SIZE;
            desc_str = "Scheduler final delta mapping profile";
        } else {
            BCM_EXIT;
        }

        break;
    case dpp_am_template_lif_mtu_profile:
        if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && soc_property_get(unit, spn_TRAP_LIF_MTU_ENABLE, 0))
        {
            t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_LOW_ID;
            if(SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit))
            {
                t_info.count        = _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_COUNT_JB0;
            }
            else
            {
                t_info.count        = _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_COUNT_QAX;
            }
            t_info.max_entities = _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_MAX_ENTITIES;
            t_info.data_size    = _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_CLS_SIZE;
            desc_str = "LIF MTU profile";
        }
        else
        {
            BCM_EXIT;
        }
        break;
    case dpp_am_template_oamp_flex_ver_mask:
        if (SOC_IS_QAX(unit)) {
            t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_OAMP_FLEX_VER_MASK_LOW_ID;
            t_info.count        = _DPP_AM_TEMPLATE_OAMP_FLEX_VER_MASK_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_OAMP_FLEX_VER_MASK_MAX_ENTITIES;
            t_info.data_size    = _DPP_AM_TEMPLATE_OAMP_FLEX_VER_MASK_SIZE;
            desc_str = "OAMP flexible verification mask.";
        } else {
            BCM_EXIT;
        }
        break;
    case dpp_am_template_oamp_cls_flex_crc:
        if (SOC_IS_QAX(unit)) {
            t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
            t_info.start        = _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_LOW_ID;
            t_info.count        = _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_COUNT;
            t_info.max_entities = _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_MAX_ENTITIES;
            t_info.data_size    = _DPP_AM_TEMPLATE_OAMP_CLS_FLEX_CRC_SIZE;
            desc_str = "OAMP cls felxible crc.";
        } else {
            BCM_EXIT;
        }
        break;
    case dpp_am_template_bfd_sbfd_non_acc_src_ip_profile:
        t_info.manager      = SHR_TEMPLATE_MANAGE_HASH;
        t_info.start        = _DPP_AM_TEMPLATE_BFD_SBFD_NON_ACC_SRC_IP_PROF_LOW_ID;
        t_info.count        = _DPP_AM_TEMPLATE_BFD_SBFD_NON_ACC_SRC_IP_PROF_COUNT;
        t_info.max_entities = _DPP_AM_TEMPLATE_BFD_SBFD_NON_ACC_SRC_IP_PROF_MAX_ENTITIES;
        t_info.data_size    = _DPP_AM_TEMPLATE_BFD_SBFD_NON_ACC_SRC_IP_PROF_SIZE;
        desc_str = "Seamless BFD non acc src IP profile";
        break;
    default:
        rv = BCM_E_INTERNAL;
    }
 
    BCMDNX_IF_ERR_EXIT(rv);
    if (t_info.max_entities) {
        rv = shr_template_pool_set(unit,
                                   t_info.pool_id,
                                   t_info.manager,
                                   t_info.start,
                                   t_info.count,
                                   t_info.max_entities,
                                   t_info.global_max,
                                   t_info.data_size,
                                   &t_info.hashExtra,
                                   desc_str);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = shr_template_type_set(unit,
                                   t_info.pool_id,
                                   t_info.pool_id,
                                   desc_str);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_tinfo_get(int unit, int pool_idx, int template_id, bcm_dpp_am_template_info_t* t_info) {

    int rv, pool_id;
    sw_state_template_name_t desc_str;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(t_info);
    
    rv = bcm_dpp_am_template_id_to_pool_id_get(unit, pool_idx, template_id, &pool_id);
    if (rv != BCM_E_NONE) {
        BCM_RETURN_VAL_EXIT(rv);
    }

    BCMDNX_IF_ERR_EXIT(shr_template_pool_get( unit,
                                              pool_id,
                                              &t_info->manager,
                                              &t_info->start,
                                              &t_info->count,
                                              &t_info->max_entities,
                                              &t_info->data_size,
                                              &t_info->hashExtra,
                                              &desc_str));

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_cosq_egr_thresh_init(int unit, int core ,int template_init_id,bcm_dpp_cosq_egress_thresh_key_info_t *egr_thresh_data_init)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template = template_init_id;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    


    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    
    result = dpp_am_template_allocate_group(unit, core, dpp_am_template_egress_thresh, flags, egr_thresh_data_init, 
                                         _DPP_AM_TEMPLATE_COSQ_EGR_THRESH_MAX_ENTITIES(unit), &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);
    /* Add all entities to template init id with given data  */

    /* We are not going to change the macro SOC_DPP_DEFS_MAX to avoid such cases */
    /* coverity[identical_branches] */
    for (index = 0; index < SOC_DPP_DEFS_MAX(NOF_LOGICAL_PORTS); index++) {
        result = ALLOC_MNGR_ACCESS.egr_thresh_map.set(unit, index, template_init_id);
        BCMDNX_IF_ERR_EXIT(result);
    }
     
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_cosq_egr_thresh_data_get(int unit, int core, int port, bcm_dpp_cosq_egress_thresh_key_info_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = ALLOC_MNGR_ACCESS.egr_thresh_map.get(unit, port, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    rc = dpp_am_template_data_get(unit, core, dpp_am_template_egress_thresh,old_profile,data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_cosq_egr_thresh_exchange(int unit, int core, int port, bcm_dpp_cosq_egress_thresh_key_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int temp;
    int temp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = ALLOC_MNGR_ACCESS.egr_thresh_map.get(unit, port, &temp);
    BCMDNX_IF_ERR_EXIT(rc);

    if (temp < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    if (old_template != NULL) {
        *old_template = temp;
    }

    rc = dpp_am_template_exchange(unit, core, dpp_am_template_egress_thresh,0,data,temp,&temp_is_last,template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = temp_is_last;
    }

    /* Since port-profile mapping is SW, update also the mapping. */
    rc = ALLOC_MNGR_ACCESS.egr_thresh_map.set(unit,port,*template);
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_egr_thresh_ref_get(int unit, int template, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_egress_thresh, template, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_egr_thresh_allocate_group(int unit, uint32 flags, bcm_dpp_cosq_egress_thresh_key_info_t *data, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_egress_thresh, flags, (void *)data, ref_count, is_allocated, template);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_egr_thresh_tdata_get(int unit, int template, bcm_dpp_cosq_egress_thresh_key_info_t *data)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_egress_thresh, template, (void *)data);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Cosq Egress Thresh - End */

STATIC int
_bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_port_profile_mapping_get(int unit, int core, int port, int *profile)
{
    int         index;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    index = port + core*SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);
    rv = ALLOC_MNGR_ACCESS.egr_interface_unicast_thresh_map.get(unit, index, profile);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_port_profile_mapping_get(int unit, int core, int port, int *profile)
{
    int         index;
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    index = port + core*SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);
    rv = ALLOC_MNGR_ACCESS.egr_interface_multicast_thresh_map.get(unit, index, profile);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_port_profile_mapping_set(int unit, int core, int port, int profile)
{
    int rv, index;

    BCMDNX_INIT_FUNC_DEFS;

    /* _bcm_dpp_egr_interface_unicast_thresh_map[unit].map_port_thresh_type[port] = profile; */
    index = port + core*SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);
    rv = ALLOC_MNGR_ACCESS.egr_interface_unicast_thresh_map.set(unit, index, profile);
    BCMDNX_IF_ERR_EXIT(rv);      


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_init(int unit,
                                                                int core,
                                              int template_init_id,
                                              bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_t *egr_interface_unicast_thresh_data_init)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template = template_init_id;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;    
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, core, dpp_am_template_egress_interface_unicast_thresh, flags, egr_interface_unicast_thresh_data_init, 
                                         _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_MAX_ENTITIES,&is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);
    for (index = 0; index < _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_UNICAST_THRESH_MAX_ENTITIES; index++) {
        result = ALLOC_MNGR_ACCESS.egr_interface_unicast_thresh_map.set(unit,
            index + core*SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces),
            template_init_id);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_data_get(int unit, int core, int interface_id, bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_port_profile_mapping_get(unit, core, interface_id,&old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    rc = dpp_am_template_data_get(unit, core, dpp_am_template_egress_interface_unicast_thresh,old_profile,data);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_exchange(int unit, int core, int interface_id, bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int temp;
    int temp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_port_profile_mapping_get(unit,core,interface_id,&temp);
    BCMDNX_IF_ERR_EXIT(rc);

    if (temp < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    if (old_template != NULL) {
        *old_template = temp;
    }

    rc = dpp_am_template_exchange(unit, core ,dpp_am_template_egress_interface_unicast_thresh,0,data,temp,&temp_is_last,template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = temp_is_last;
    }

    /* Since port-profile mapping is SW, update also the mapping. */
    _bcm_dpp_am_template_cosq_egr_interface_unicast_thresh_port_profile_mapping_set(unit,core,interface_id,*template);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/* Cosq Egress Interface Unicast Thresh - End */

STATIC int
_bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_port_profile_mapping_set(int unit, int core, int interface_id, int profile)
{
    int rv,index;

    BCMDNX_INIT_FUNC_DEFS;

    /* _bcm_dpp_egr_interface_multicast_thresh_map[unit].map_port_thresh_type[interface_id] = profile; */
    index = interface_id + core*SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);
    rv = ALLOC_MNGR_ACCESS.egr_interface_multicast_thresh_map.set(unit, index, profile);
    BCMDNX_IF_ERR_EXIT(rv);      

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_init(int unit,
                                                                  int core,
                                              int template_init_id,
                                              bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_t *egr_interface_multicast_thresh_data_init)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template = template_init_id;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;    
    
    result = dpp_am_template_allocate_group(unit, core, dpp_am_template_egress_interface_multicast_thresh, flags, egr_interface_multicast_thresh_data_init, 
                                         _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);    
    /* Add all entities to template init id with given data */
    for (index = 0; index < _DPP_AM_TEMPLATE_COSQ_EGR_INTERFACE_MULTICAST_THRESH_MAX_ENTITIES; index++) {
        result = ALLOC_MNGR_ACCESS.egr_interface_multicast_thresh_map.set(unit,
            index + core*SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces),
            template_init_id);
        BCMDNX_IF_ERR_EXIT(result);
    }
 
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_data_get(int unit, int core, int interface_id, bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_port_profile_mapping_get(unit,core,interface_id,&old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    rc = dpp_am_template_data_get(unit, core,dpp_am_template_egress_interface_multicast_thresh,old_profile,data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_exchange(int unit, int core, int interface_id, bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int temp;
    int temp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_port_profile_mapping_get(unit,core,interface_id,&temp);
    BCMDNX_IF_ERR_EXIT(rc);

    if (temp < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    if (old_template != NULL) {
        *old_template = temp;
    }

    rc = dpp_am_template_exchange(unit, core,dpp_am_template_egress_interface_multicast_thresh,0,data,temp,&temp_is_last,template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = temp_is_last;
    }

    /* Since port-profile mapping is SW, update also the mapping. */
    _bcm_dpp_am_template_cosq_egr_interface_multicast_thresh_port_profile_mapping_set(unit,core, interface_id,*template);
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}
/* Cosq Egress Interface Multicast Thresh - End */

/*Mirror action start */
int
_bcm_dpp_am_template_mirror_action_profile_alloc(int unit, int flags, uint32 *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;
	uint32 data_dummy = 0xfff ;/*Insignificant number*/
	int is_allocated_dummy;
	int *is_allocateded_to_give=is_allocated? is_allocated : &is_allocated_dummy;
	uint32 *data_to_give=data? data: &data_dummy;
    BCMDNX_INIT_FUNC_DEFS;

	if (flags & SHR_TEMPLATE_MANAGE_IGNORE_DATA) {
		/* simply look for a new profile*/
		int  prof_ind, is_alloced=0;
		uint32 ref_count;
		for (prof_ind=DPP_MIRROR_ACTION_NDX_MIN ; prof_ind<= DPP_MIRROR_ACTION_NDX_MAX ; ++prof_ind) {
			if ( (flags &SHR_TEMPLATE_MANAGE_SET_WITH_ID)==0 || ((flags &SHR_TEMPLATE_MANAGE_SET_WITH_ID) && prof_ind==*profile ) ) {
				/* If the _WITH_ID flag is set try allocating only the specific profile selected.*/
				rc = _bcm_dpp_am_template_mirror_action_profile_ref_get(unit,prof_ind,&ref_count);
				BCMDNX_IF_ERR_EXIT(rc);
				if (ref_count==0) {
					/* Found a free one. Alloc and move on.*/
					data_dummy +=prof_ind ; /* insignificant number, but should be distinct for every index*/
					rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mirror_action_profile, SHR_TEMPLATE_MANAGE_SET_WITH_ID, data_to_give, is_allocateded_to_give, &prof_ind);
					BCMDNX_IF_ERR_EXIT(rc); 
					is_alloced=1;
					*profile = prof_ind;
					break;
				}
			}
		}
		if (is_alloced==0) {
			BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Mirror profile not found.")));
		}
	} else {
		rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mirror_action_profile, flags, data, is_allocateded_to_give, profile);
		BCMDNX_IF_ERR_EXIT(rc);
	}

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_mirror_action_profile_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mirror_action_profile, profile, is_last);
	BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_am_template_mirror_action_profile_get(int unit, int mirror_profile, uint32 *data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mirror_action_profile, mirror_profile, data);
    if (rc == BCM_E_NOT_FOUND) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_mirror_action_profile_ref_get(int unit, int profile, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mirror_action_profile, profile, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*Mirror action end */


/* User defined traps - Start */
int _bcm_dpp_am_template_user_defined_traps_init(int unit,int template_init_id,const bcm_dpp_user_defined_traps_t* data)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_user_defined_traps, flags, data, _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_MAX_ENTITIES(unit), &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_template_user_defined_port_trap_mapping_get(int unit, int port,int *user_defined_trap)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;
    SOC_PPC_PORT_INFO port_info;
    SOC_PPC_TRAP_CODE trap_code;
    uint32  pp_port;
    int     core;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_PORT_INFO_clear(&port_info);

    soc_sand_dev_id = (unit);
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id,core,pp_port,&port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    trap_code = port_info.initial_action_profile.trap_code;


    if ((trap_code < SOC_PPC_TRAP_CODE_USER_DEFINED_0) || (trap_code - SOC_PPC_TRAP_CODE_USER_DEFINED_0 >= _DPP_AM_TEMPLATE_USER_DEFINED_TRAPS_COUNT)) {
        /* Expect trap code is not user defined */
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Expect trap code is not user defined")));
    }

    *user_defined_trap = trap_code - SOC_PPC_TRAP_CODE_USER_DEFINED_0;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_user_defined_trap_allocate(int unit, uint32 flags, int port, const bcm_dpp_user_defined_traps_t* data, int *is_allocated, int *user_defined_trap)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_user_defined_traps, flags, data, is_allocated, user_defined_trap);
   
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_user_defined_trap_free(int unit, int port, int* is_last, int *user_defined_trap)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_user_defined_port_trap_mapping_get (unit,port,user_defined_trap);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_user_defined_traps,*user_defined_trap,is_last);
  
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_user_defined_trap_exchange(int unit, int port, bcm_dpp_user_defined_traps_t * data, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int old_ud;
    int trap_id;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_user_defined_port_trap_mapping_get(unit,port,&old_ud);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_template != NULL) {
        *old_template = old_ud;
    }

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_user_defined_traps,SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,data,old_ud,is_last,template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);


    /*Sync rx.c traps allocations with port.c traps allocations (templates) for USER_DEFINED traps*/
    if (*is_last) {
       /*is_last means not in use anymore, there for free allocation*/
       trap_id = old_ud + SOC_PPC_TRAP_CODE_USER_DEFINED_0;
       rc = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_user_define,SW_STATE_RES_ALLOC_WITH_ID,trap_id);
       BCMDNX_IF_ERR_EXIT(rc);

    }   

    if (*is_allocated) {
       /*is_allocated means first time in use, there for allocate only the first time*/
       trap_id = *template + SOC_PPC_TRAP_CODE_USER_DEFINED_0;
       rc = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_user_define,SW_STATE_RES_ALLOC_WITH_ID,1,&trap_id);
       BCMDNX_IF_ERR_EXIT(rc);

    }

    
exit:
    BCMDNX_FUNC_RETURN;
}

/* User defined traps - End */

/* Snoop Command - Start */
typedef struct _dpp_template_snoop_cmd_mapping_e {
    /* Map port -> thresh_type */
    int map_port_snoop_cmd[_BCM_PETRA_NOF_MAX_PP_PORTS];
} _dpp_template_snoop_cmd_mapping_t;

static _dpp_template_snoop_cmd_mapping_t _bcm_dpp_snoop_cmd_map[BCM_MAX_NUM_UNITS];

STATIC int
_bcm_dpp_am_template_snoop_cmd_port_snoop_mapping_get(int unit, int port,int *snoop_cmd)
{
    
    BCMDNX_INIT_FUNC_DEFS;
    if(port < 0 || port >= _BCM_PETRA_NOF_MAX_PP_PORTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid port. Range : 0 - %d.\n"), _BCM_PETRA_NOF_MAX_PP_PORTS));
    } 
    *snoop_cmd = _bcm_dpp_snoop_cmd_map[unit].map_port_snoop_cmd[port];

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set mapping - since get mapping it is SW check */ 
int _bcm_dpp_am_template_snoop_cmd_init(int unit,int template_init_id,const bcm_dpp_snoop_t* data)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_snoop_cmd, flags, data,_DPP_AM_TEMPLATE_SNOOP_CMD_MAX_ENTITIES(unit), &is_allocated,&template_init_id);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_template_snoop_cmd_port_snoop_mapping_set(int unit, int port, int snoop_cmd)
{
    BCMDNX_INIT_FUNC_DEFS;

    _bcm_dpp_snoop_cmd_map[unit].map_port_snoop_cmd[port] = snoop_cmd;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_snoop_cmd_allocate(int unit, uint32 flags, int port, const bcm_dpp_snoop_t* data, int *is_allocated, int *snoop_cmd)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_snoop_cmd, flags, data, is_allocated, snoop_cmd);
    BCMDNX_IF_ERR_EXIT(rc);

    /* Since mapping is SW only alloc also updates SW state */
    rc = _bcm_dpp_am_template_snoop_cmd_port_snoop_mapping_set(unit,port,*snoop_cmd);
   
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_snoop_cmd_free(int unit, int port, int* is_last, int *snoop_cmd)
{
    int rc = BCM_E_NONE;
    int snoop;
    uint32 flags = 0;

    /*SOC_TMC_ACTION_CMD_SNOOP_INFO snoop_tmp; */

    bcm_dpp_snoop_t snoop_tmp;

    int is_new ,new_snoop_cmd;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_snoop_cmd_port_snoop_mapping_get(unit,port,&snoop);
    BCMDNX_IF_ERR_EXIT(rc);

    if (snoop < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get snoop")));
    }

    if (snoop_cmd != NULL) {
        *snoop_cmd = snoop;
    }

    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_snoop_cmd,snoop,is_last);
    BCMDNX_IF_ERR_EXIT(rc);

    SOC_TMC_ACTION_CMD_SNOOP_INFO_clear(&(snoop_tmp.snoop_action_info));
    snoop_tmp.snoop_action_info.prob = (uint32) -1;
    snoop_tmp.snoop_unique = _BCM_DPP_TRAP_PREVENT_SWAPPING;

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;

    rc = _bcm_dpp_am_template_snoop_cmd_allocate(unit,flags,port,&snoop_tmp, &is_new ,&new_snoop_cmd);
    BCMDNX_IF_ERR_EXIT(rc);
  
    /* Since mapping is SW also updates SW state */
    rc = _bcm_dpp_am_template_snoop_cmd_port_snoop_mapping_set(unit,port,-1);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_snoop_cmd_exchange(int unit, int port, bcm_dpp_snoop_t * data, int *old_snoop_cmd, int *is_last, int *new_snoop_cmd,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int snoop_cmd;
    int is_last_temp;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(new_snoop_cmd);    
    BCMDNX_NULL_CHECK(is_allocated);

    rc = _bcm_dpp_am_template_snoop_cmd_port_snoop_mapping_get(unit,port,&snoop_cmd);
    BCMDNX_IF_ERR_EXIT(rc);

    if (snoop_cmd < 0) {
        /* Port object didn't allocate before */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("snoop_cmd is not valid")));
    }

    if (old_snoop_cmd != NULL) {
        *old_snoop_cmd = snoop_cmd;
    }

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_snoop_cmd,0,data,snoop_cmd,&is_last_temp,new_snoop_cmd,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);


    if (is_last != NULL) {
        *is_last = is_last_temp;
    }
    /* Since port-profile mapping is SW, update also the mapping. */
    rc = _bcm_dpp_am_template_snoop_cmd_port_snoop_mapping_set(unit,port,*new_snoop_cmd);
    BCMDNX_IF_ERR_EXIT(rc);


    /*Sync rx.c traps allocations with port.c traps allocations (templates) for USER_DEFINED traps*/
    if (is_last_temp) {
        /*is_last means not in use anymore, there for free allocation*/
        rc = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_snoop_cmd,SW_STATE_RES_ALLOC_WITH_ID,snoop_cmd);
        BCMDNX_IF_ERR_EXIT(rc);

    }
    
    if (*is_allocated) {
        /*is_allocated means first time in use, there for allocate only the first time*/
        rc = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_snoop_cmd,SW_STATE_RES_ALLOC_WITH_ID,1,new_snoop_cmd);
        BCMDNX_IF_ERR_EXIT(rc);

    }  

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_snoop_cmd_data_get(int unit, int port, bcm_dpp_snoop_t * data)
{
    int snoop_cmd;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_snoop_cmd_port_snoop_mapping_get(unit,port,&snoop_cmd);
    BCMDNX_IF_ERR_EXIT(rc);

    if (snoop_cmd < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get snoop")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_snoop_cmd,snoop_cmd,data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/* Snoop Command - End */

/* TPID profile - Start */
STATIC int
_bcm_dpp_am_template_tpid_profile_port_tpid_profile_mapping_get(int unit,
                                    int pp_port, int core, int *tpid_profile)
{
    uint32 soc_sand_rv;
    int soc_sand_dev_id;
    SOC_PPC_PORT_INFO port_info;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_PORT_INFO_clear(&port_info);

    soc_sand_dev_id = (unit);
    soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id,core,pp_port,&port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *tpid_profile = port_info.port_profile;

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_tpid_profile_init(int unit,int template_init_id,_bcm_petra_tpid_profile_t profile_type, int accept_mode, _bcm_petra_dtag_mode_t dtag_mode, _bcm_petra_ac_key_map_type_t ac_key_map_type)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    int data = 0;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    data |= (accept_mode  & _DPP_AM_TEMPLATE_PORT_ACC_MODE_MASK)  << _DPP_AM_TEMPLATE_PORT_ACC_MODE_SHIFT;
    data |= (profile_type & _DPP_AM_TEMPLATE_PORT_TAG_TYPE_MASK)  << _DPP_AM_TEMPLATE_PORT_TAG_TYPE_SHIFT;
    data |= (dtag_mode    & _DPP_AM_TEMPLATE_PORT_DTAG_MODE_MASK) << _DPP_AM_TEMPLATE_PORT_DTAG_MODE_SHIFT;
    data |= (ac_key_map_type & _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_MASK) << _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_SHIFT;
    
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    for (index = 0; index < _DPP_AM_TEMPLATE_TPID_PROFILE_MAX_ENTITIES(unit); index++) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_tpid_profile, flags, &data, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_tpid_profile_allocate(int unit, uint32 flags, int port, _bcm_petra_tpid_profile_t profile_type, int accept_mode, _bcm_petra_dtag_mode_t dtag_mode, _bcm_petra_ac_key_map_type_t ac_key_map_type, int *is_allocated, int *tpid_profile)
{
    int rc = BCM_E_NONE;
    int data = 0;

    BCMDNX_INIT_FUNC_DEFS;

    data |= (accept_mode & _DPP_AM_TEMPLATE_PORT_ACC_MODE_MASK) << _DPP_AM_TEMPLATE_PORT_ACC_MODE_SHIFT;
    data |= (profile_type & _DPP_AM_TEMPLATE_PORT_TAG_TYPE_MASK) << _DPP_AM_TEMPLATE_PORT_TAG_TYPE_SHIFT;
    data |= (dtag_mode & _DPP_AM_TEMPLATE_PORT_DTAG_MODE_MASK) << _DPP_AM_TEMPLATE_PORT_DTAG_MODE_SHIFT;
    data |= (ac_key_map_type & _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_MASK) << _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_SHIFT;

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_tpid_profile, flags, &data, is_allocated, tpid_profile);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_tpid_profile_free(int unit, int pp_port, int core, int* is_last, int *tpid_profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_tpid_profile_port_tpid_profile_mapping_get(unit,pp_port,core,tpid_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_profile,*tpid_profile,is_last);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_tpid_profile_exchange(int unit, int pp_port, int core, _bcm_petra_tpid_profile_t profile_type, int accept_mode, _bcm_petra_dtag_mode_t dtag_mode, _bcm_petra_ac_key_map_type_t ac_key_map_type, int *old_tpid_profile, int *is_last, int *new_tpid_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int data = 0;
    int tmp_old_profile;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_tpid_profile_port_tpid_profile_mapping_get(unit,pp_port,core,&tmp_old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    if(old_tpid_profile) {
        *old_tpid_profile = tmp_old_profile;
    }

    data |= (accept_mode & _DPP_AM_TEMPLATE_PORT_ACC_MODE_MASK) << _DPP_AM_TEMPLATE_PORT_ACC_MODE_SHIFT;
    data |= (profile_type & _DPP_AM_TEMPLATE_PORT_TAG_TYPE_MASK) << _DPP_AM_TEMPLATE_PORT_TAG_TYPE_SHIFT;
    data |= (dtag_mode & _DPP_AM_TEMPLATE_PORT_DTAG_MODE_MASK) << _DPP_AM_TEMPLATE_PORT_DTAG_MODE_SHIFT;
    data |= (ac_key_map_type & _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_MASK) << _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_SHIFT;

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_profile,0,&data,tmp_old_profile,is_last,new_tpid_profile,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_tpid_profile_data_get(int unit, int pp_port, int core, _bcm_petra_tpid_profile_t *profile_type, int *accept_mode, _bcm_petra_dtag_mode_t *dtag_mode, _bcm_petra_ac_key_map_type_t *ac_key_map_type)
{
    int index;
    int val=0;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_tpid_profile_port_tpid_profile_mapping_get(unit,pp_port,core,&index);
    BCMDNX_IF_ERR_EXIT(rc);

    if (index < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_profile,index,&val);
    BCMDNX_IF_ERR_EXIT(rc);

    *accept_mode = (val >> _DPP_AM_TEMPLATE_PORT_ACC_MODE_SHIFT) & _DPP_AM_TEMPLATE_PORT_ACC_MODE_MASK;
    *profile_type = (val >> _DPP_AM_TEMPLATE_PORT_TAG_TYPE_SHIFT) & _DPP_AM_TEMPLATE_PORT_TAG_TYPE_MASK;
    *dtag_mode = (val >> _DPP_AM_TEMPLATE_PORT_DTAG_MODE_SHIFT) & _DPP_AM_TEMPLATE_PORT_DTAG_MODE_MASK;
    *ac_key_map_type = (val >> _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_SHIFT) & _DPP_AM_TEMPLATE_PORT_AC_KEY_MAP_TYPE_MASK;

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_tpid_profile_ref_get(int unit, int template, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_tpid_profile, template, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_tpid_profile_allocate_group(int unit, uint32 flags, uint32 *data, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_tpid_profile, flags, (void *)data, ref_count, is_allocated, template);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_tpid_profile_tdata_get(int unit, int template, uint32 *data)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_tpid_profile, template, data);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    BCMDNX_FUNC_RETURN;
}




/* Cosq Egress Queue Mapping - Start */
int _bcm_dpp_am_template_egress_queue_mapping_init(int unit,int template_init_id, int core, const bcm_dpp_cosq_egress_queue_mapping_info_t* mapping_profile)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, core, dpp_am_template_egress_queue_mapping, flags, mapping_profile,_DPP_AM_TEMPLATE_EGR_QUEUE_MAPPING_MAX_ENTITIES(unit), &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);
   

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_egress_queue_mapping_data_get(int unit, uint32 tm_port, int core, bcm_dpp_cosq_egress_queue_mapping_info_t * mapping_profile)
{
    int index;
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_egr_q_profile_map_get,(unit, core, tm_port, (uint32*)&index));
    BCMDNX_IF_ERR_EXIT(rc);

    if (index < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    rc = dpp_am_template_data_get(unit, core, dpp_am_template_egress_queue_mapping, index, mapping_profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_egress_queue_mapping_exchange(int unit, uint32 tm_port, int core, CONST bcm_dpp_cosq_egress_queue_mapping_info_t * mapping_profile, int *old_profile, int *is_last, int *new_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_profile;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = MBCM_DPP_SOC_DRIVER_CALL(unit,mbcm_dpp_egr_q_profile_map_get,(unit, core, tm_port, (uint32*)&tmp_old_profile));
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, core, dpp_am_template_egress_queue_mapping, 0, mapping_profile, tmp_old_profile, &tmp_is_last, new_profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile != NULL) {
        *old_profile = tmp_old_profile;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Cosq Egress Queue Mapping - End */

/* NRDY threshold - Start */

int
_bcm_dpp_am_template_nrdy_threshold_alloc(int unit, int core, int flags, uint32 *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, core, dpp_am_template_nrdy_threshold, flags, data, is_allocated, profile);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_nrdy_threshold_free(int unit, int core, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, core, dpp_am_template_nrdy_threshold, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_nrdy_threshold_exchange(int unit, int core, uint32 tm_port, int flags, uint32 *data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated)
{
    int rc = BCM_E_NONE;
    soc_port_t port;

    BCMDNX_INIT_FUNC_DEFS;

    /* get old profile */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_tm_to_local_port_get, (unit, core, tm_port, &port)));
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_nrdy_th_profile_get, (unit, port, (uint32*)old_profile)));

    /* exchange */
    rc = dpp_am_template_exchange(unit, core, dpp_am_template_nrdy_threshold, flags, data, *old_profile, is_last, profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* NRDY threshold - End */

/* VLAN Edit Profile Mapping - Start */
STATIC int
_bcm_dpp_am_template_vlan_edit_profile_mapping_get(int unit, SOC_PPC_LIF_ID lif_id, int *profile)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;    
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;
    uint8 found = 1;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    soc_sand_dev_id = (unit);

    /* Get the Profile from the HW In-AC attribute */
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, lif_id, &lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(lif_entry_info.type != SOC_PPC_LIF_ENTRY_TYPE_AC && lif_entry_info.type != SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP
       && lif_entry_info.type != SOC_PPC_LIF_ENTRY_TYPE_EMPTY) {
        found = 0;
    }

    if (found) {
        *profile = lif_entry_info.value.ac.ing_edit_info.ing_vlan_edit_profile;
    }
    else {
        /* Get the default profile */
        *profile = 0;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_mapping_init(int unit,int template_init_id,const bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
   
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_edit_profile_mapping,flags, mapping_profile,_DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_MAX_ENTITIES(unit),&is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_mapping_data_get_by_template(int unit,int index, bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    if (index < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid index")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_edit_profile_mapping,index,mapping_profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_mapping_data_get(int unit,SOC_PPC_LIF_ID lif_id, bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile)
{
    int index;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    rc = _bcm_dpp_am_template_vlan_edit_profile_mapping_get(unit,lif_id,&index);
    BCMDNX_IF_ERR_EXIT(rc);

    if (index < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid index")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_edit_profile_mapping,index,mapping_profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_mapping_exchange(int unit, SOC_PPC_LIF_ID lif_id, bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile, int *old_profile, int *is_last, int *new_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_profile;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    rc = _bcm_dpp_am_template_vlan_edit_profile_mapping_get(unit,lif_id,&tmp_old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_edit_profile_mapping,0,mapping_profile,tmp_old_profile,&tmp_is_last,new_profile,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile != NULL) {
        *old_profile = tmp_old_profile;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_mapping_alloc(int unit, int flags, SOC_PPC_LIF_ID lif_id, bcm_dpp_vlan_edit_profile_mapping_info_t* mapping_profile, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vlan_edit_profile_mapping, flags, mapping_profile, is_allocated, template);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_mapping_free(int unit, SOC_PPC_LIF_ID lif_id, int *old_template,int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    rc = _bcm_dpp_am_template_vlan_edit_profile_mapping_get(unit,lif_id,old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vlan_edit_profile_mapping, *old_template, is_last);
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocate only for reserved profiles - dummy entries */
int _bcm_dpp_am_template_vlan_edit_profile_mapping_dummy_allocate(int unit, uint32 flags, bcm_dpp_vlan_edit_profile_mapping_info_t *mapping_profile, int *template_id, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif  

    BCMDNX_NULL_CHECK(template_id);

    template = (int)*template_id;

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vlan_edit_profile_mapping, flags, mapping_profile, is_allocated, template_id);
    BCMDNX_IF_ERR_EXIT(rc);

    *template_id = template;
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dpp_am_template_vlan_edit_profile_eg_mapping_get(int unit, SOC_PPC_AC_ID out_ac, int *profile)
{
    uint32 soc_sand_rv;
    int soc_sand_dev_id;
    SOC_PPC_EG_AC_INFO ac_info;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    soc_sand_dev_id = (unit);

    /* Get the Profile from the HW In-AC attribute */
    SOC_PPC_EG_AC_INFO_clear(&ac_info);
    soc_sand_rv = soc_ppd_eg_ac_info_get(soc_sand_dev_id, out_ac, &ac_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *profile = ac_info.edit_info.edit_profile;

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_init(int unit,
                                                        int template_init_id,
                                                        const bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_edit_profile_mapping_eg,flags,mapping_profile,_DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_EG_MAX_ENTITIES(unit),&is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocate only for reserved profiles - dummy entries */
int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_allocate(int unit, uint32 flags, bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile, uint32 *template_id, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    BCMDNX_NULL_CHECK(template_id);

    template = (int)*template_id;

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vlan_edit_profile_mapping_eg, flags, mapping_profile, is_allocated, &template);
    BCMDNX_IF_ERR_EXIT(rc);

    *template_id = template;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_data_get(int unit,
                                                            SOC_PPC_AC_ID out_ac, 
                                                            bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile)
{
    int index;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    rc = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_get(unit,out_ac,&index);
    BCMDNX_IF_ERR_EXIT(rc);

    if (index < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid index")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_edit_profile_mapping_eg,index,mapping_profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_data_get_by_template(int unit,
                                                            int index, 
                                                            bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    if (index < _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_LOW_ID || index >= _DPP_AM_TEMPLATE_VLAN_EDIT_PROFILE_MAPPING_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Egress vlan editing profile is not in valid range")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_edit_profile_mapping_eg,index,mapping_profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_exchange(int unit, 
                                                            SOC_PPC_AC_ID out_ac, 
                                                            bcm_dpp_vlan_egress_edit_profile_info_t *mapping_profile, 
                                                            int *old_profile, 
                                                            int *is_last, 
                                                            int *new_profile,
                                                            int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_profile;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DPP_IS_VLAN_TRANSLATE_MODE_ADVANCED(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("The function isn't suitable for VLAN edit advanced mode")));
    }

    rc = _bcm_dpp_am_template_vlan_edit_profile_eg_mapping_get(unit,out_ac,&tmp_old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_edit_profile_mapping_eg,0,mapping_profile,tmp_old_profile,&tmp_is_last,new_profile,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile != NULL) {
        *old_profile = tmp_old_profile;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

exit:
    BCMDNX_FUNC_RETURN;
}



/* VLAN Edit Profile Mapping - End */
/* L2 reserved MC - Start */

int _bcm_dpp_am_template_l2_cache_rm_init(int unit,int template_init_id,_bcm_petra_l2_rsrvd_mc_profile_info_t* init_data)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    /*flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;*/
    
    /* Add all entities to template init id with given data */
                            
    for (index = 0; index < _DPP_AM_TEMPLATE_L2_RSRVD_MC_MAX_ENTITIES(unit); index++) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_reserved_mc, flags, init_data, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_template_l2_cache_rm_port_profile_mapping_get(int unit, int port, int *profile)
{
 
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_LLP_TRAP_PORT_INFO
        trap_port_info;
    uint32  pp_port;
    int     core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id, core, pp_port, &trap_port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *profile = trap_port_info.reserved_mc_profile;

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_l2_cache_rm_data_get(int unit, int port, _bcm_petra_l2_rsrvd_mc_profile_info_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_l2_cache_rm_port_profile_mapping_get(unit,port,&old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_reserved_mc,old_profile,data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2_cache_rm_exchange(int unit, int flags, int port, _bcm_petra_l2_rsrvd_mc_profile_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_l2_cache_rm_port_profile_mapping_get(unit,port,old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    if(!(flags & BCM_DPP_AM_TEMPLATE_FLAG_CHECK)) {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_reserved_mc,0,data,*old_template,is_last,template,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_reserved_mc,0,data,*old_template,is_last,template,is_allocated);
    }

    BCMDNX_IF_ERR_EXIT(rc);



exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2_cache_rm_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_reserved_mc));
exit:
    BCMDNX_FUNC_RETURN;
}

/* l2 reserved MC - End */


/* port TPID class - Start */

int _bcm_dpp_am_template_port_tpid_class_init(int unit,int template_init_id,_bcm_dpp_port_tpid_class_info_t* init_data)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    /*flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;*/
    
    /* Add all entities to template init id with given data */
                            
    for (index = 0; index < _DPP_AM_TEMPLATE_PORT_TPID_CLASS_MAX_ENTITIES(unit); index++) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_tpid_class, flags, init_data, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_tpid_class_eg, flags, init_data, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_template_port_tpid_class_port_profile_mapping_get(int unit, int port, int *profile)
{
 
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_PORT_INFO
        port_info;
    uint32  pp_port;
    int     core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *profile = port_info.port_profile;

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_template_port_tpid_class_port_eg_profile_mapping_get(int unit, int port, int *profile)
{
 
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_PORT_INFO
        port_info;
    uint32  pp_port;
    int     core;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *profile = port_info.port_profile_eg;

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_port_tpid_class_data_get(int unit, int port, _bcm_dpp_port_tpid_class_info_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_port_tpid_class_port_profile_mapping_get(unit,port,&old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_class,old_profile,data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_tpid_class_eg_data_get(int unit, int port, _bcm_dpp_port_tpid_class_info_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_port_tpid_class_port_eg_profile_mapping_get(unit,port,&old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_class_eg,old_profile,data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_tpid_class_exchange(int unit, int flags, int port, _bcm_dpp_port_tpid_class_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_port_tpid_class_port_profile_mapping_get(unit,port,old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    if(!(flags & BCM_DPP_AM_TEMPLATE_FLAG_CHECK)) {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_class,0,data,*old_template,is_last,template,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);

    }
    else
    {
        rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_class,0,data,*old_template,is_last,template,is_allocated);
    }

    BCMDNX_IF_ERR_EXIT(rc);



exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_tpid_class_eg_exchange(int unit, int flags, int port, _bcm_dpp_port_tpid_class_info_t * data, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_port_tpid_class_port_eg_profile_mapping_get(unit,port,old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    if(!(flags & BCM_DPP_AM_TEMPLATE_FLAG_CHECK))
    {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_class_eg,0,data,*old_template,is_last,template,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_class_eg,0,data,*old_template,is_last,template,is_allocated);
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_tpid_class_eg_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_class_eg));
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_tpid_class_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_tpid_class));
exit:
    BCMDNX_FUNC_RETURN;
}

/* port TPID class - End */

/* tpid classify acceptable frame type  - Start */

int _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_init(int unit,int template_init_id, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *init_data) {
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    int template = template_init_id;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_port_tpid_class_egress_acceptable_frame_type, flags, init_data, 
                                         _DPP_AM_TEMPLATE_PORT_TPID_CLASS_EGRESS_ACCEPTABLE_FRAME_TYPE_MAX_ENTITIES(unit),
                                         &is_allocated, &template);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;


}

STATIC int
_bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_port_profile_mapping_get(int unit, int port, int *profile) {

    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_EG_FILTER_PORT_INFO port_info;
    int core=0;
    SOC_PPC_PORT pp_port = 0;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core));
    soc_sand_rv = soc_ppd_eg_filter_port_info_get(soc_sand_dev_id, core, pp_port, &port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *profile = port_info.acceptable_frames_profile;

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_data_get(int unit, int port, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *data) {
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_port_profile_mapping_get(unit,port,&old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_port_tpid_class_egress_acceptable_frame_type,old_profile,data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_exchange(int unit, int flags, int port, _bcm_dpp_port_tpid_class_acceptable_frame_type_info_t *data, int *old_template, int *is_last, int *template,int *is_allocated) {
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_port_profile_mapping_get(unit,port,old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    if(!(flags & BCM_DPP_AM_TEMPLATE_FLAG_CHECK)) {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_port_tpid_class_egress_acceptable_frame_type,0,data,*old_template,is_last,template,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);

    }
    else
    {
        rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_port_tpid_class_egress_acceptable_frame_type,0,data,*old_template,is_last,template,is_allocated);
    }

    BCMDNX_IF_ERR_EXIT(rc);



exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_tpid_class_egress_acceptable_frame_type_clear(int unit) {
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_port_tpid_class_egress_acceptable_frame_type));
exit:
    BCMDNX_FUNC_RETURN;

}

/* tpid classify acceptable frame type  - End */


/* Trap- egress - Start */
int _bcm_dpp_am_template_trap_egress_init(int unit,int template_init_id,SOC_SAND_OUT SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *init_data)
{
    int is_allocated, rc = BCM_E_NONE;
    uint32 flags = 0;
    uint8 cpu_ports_exist;
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO eg_drop_info;
    int template;
    uint32 soc_sand_rv;
    unsigned int    soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    soc_sand_dev_id = (unit); 
    
    /* Add all entities to template init id with given data
       entity SOC_PPC_NOF_TRAP_EG_TYPES_ARAD will always point to this data, so it
       will never be cleared (this is the reason for using <= and not <) */
    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_trap_egress, flags, init_data,SOC_PPC_NOF_TRAP_EG_TYPES_ARAD, &is_allocated,&template_init_id);
    BCMDNX_IF_ERR_EXIT(rc);
    
    rc = _bcm_dpp_rx_egress_set_to_drop_info(unit,&eg_drop_info);
    BCMDNX_IF_ERR_EXIT(rc);
    template = _DPP_SWITCH_EG_TRAP_DROP_PROFILE_INDEX;

    /*entity number _DPP_AM_TEMPLATE_TRAP_EGRESS_MAX_ENTITIES will point to drop action */
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_trap_egress,flags,&eg_drop_info,&is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(rc);

    if (!SOC_WARM_BOOT(unit)) {
        soc_sand_rv =soc_ppd_trap_eg_profile_info_set(soc_sand_dev_id,_DPP_SWITCH_EG_TRAP_DROP_PROFILE_INDEX,
                                                      &eg_drop_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        rc = _bcm_dpp_rx_egress_set(unit,SOC_PPC_TRAP_EG_TYPE_HAIR_PIN,&eg_drop_info);
        BCMDNX_IF_ERR_EXIT(rc);
        
        rc = _bcm_dpp_rx_egress_set(unit,SOC_PPC_TRAP_EG_TYPE_SPLIT_HORIZON,&eg_drop_info);
        BCMDNX_IF_ERR_EXIT(rc);
        
        rc = _bcm_dpp_rx_egress_set(unit,SOC_PPC_TRAP_EG_TYPE_PRIVATE_VLAN,&eg_drop_info);    
        BCMDNX_IF_ERR_EXIT(rc);
        /*set invalid out port to discard pkts*/
        rc = _bcm_dpp_rx_egress_set(unit,SOC_PPC_TRAP_EG_TYPE_INVALID_OUT_PORT,&eg_drop_info);    
        BCMDNX_IF_ERR_EXIT(rc);
    }

    rc = arad_pp_trap_cpu_ports_on_all_cores( unit, &cpu_ports_exist);
    BCMDNX_IF_ERR_EXIT(rc);
    
    /* we configure a cpu redirect action profile only if the there is a cpu port in each core */
    if(cpu_ports_exist) {
        rc = _bcm_dpp_rx_egress_set_to_cpu_drop_info(unit,&eg_drop_info);
        BCMDNX_IF_ERR_EXIT(rc);
        template = _DPP_SWITCH_TRAP_TO_CPU_PROFILE_INDEX;

        /*entity number _DPP_AM_TEMPLATE_TRAP_EGRESS_MAX_ENTITIES +1 will point to cpu drop action */
        rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_trap_egress,flags,&eg_drop_info,&is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(rc);


        if (!SOC_WARM_BOOT(unit)) {
            soc_sand_rv = soc_ppd_trap_eg_profile_info_set(soc_sand_dev_id,_DPP_SWITCH_TRAP_TO_CPU_PROFILE_INDEX,&eg_drop_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_trap_egress_data_get(int unit,
                                              SOC_PPC_TRAP_EG_TYPE trap_id, /* Enum trap */
                                              uint32 egress_trap_entry, /* Egress Trap or Field Entry */                                              
                                              SOC_SAND_OUT SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *data)
{
    int rc = BCM_E_NONE;
    SOC_SAND_OUT uint32 old_profile;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_trap_egress_profile_mapping_get(unit, trap_id, egress_trap_entry, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX ,dpp_am_template_trap_egress, old_profile, data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_am_template_trap_egress_allocate(int unit,
                                          int flags,
                                          SOC_SAND_IN SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *data,
                                          int *template)
{
    int isAlloc;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                                          dpp_am_template_trap_egress,
                                          flags,
                                          data,
                                          &isAlloc,
                                          template));
    if (isAlloc) {
        if (!SOC_WARM_BOOT(unit)) {
            BCM_SAND_IF_ERR_EXIT(soc_ppd_trap_eg_profile_info_set((unit),
                                                                  *template,
                                                                  data));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_trap_egress_free(int unit,
                                      int template)
{
    SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO defaults;
    int isLast;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                                      dpp_am_template_trap_egress,
                                      template,
                                      &isLast));
    if (isLast) {
        SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(&defaults);
        if (!SOC_WARM_BOOT(unit)) {        
            BCM_SAND_IF_ERR_EXIT(soc_ppd_trap_eg_profile_info_set((unit),
                                                                  template,
                                                                  &defaults));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_trap_egress_exchange(int unit, int flags, 
                                              SOC_PPC_TRAP_EG_TYPE trap_id, /* Enum trap */
                                              uint32 egress_trap_entry, /* Egress Trap or Field Entry */
                                              SOC_SAND_IN SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO *data, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int temp_is_last;
    SOC_SAND_OUT uint32 temp;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_trap_egress_profile_mapping_get(unit, trap_id, egress_trap_entry, &temp);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_template != NULL) {
        *old_template = temp;
    }

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_trap_egress,flags,data,temp,&temp_is_last,template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = temp_is_last;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_am_template_trap_egress_profile_mapping_get(int unit,
                                                     SOC_PPC_TRAP_EG_TYPE trap_id, /* Enum trap */
                                                     uint32 egress_trap_entry, /* Egress Trap or Field Entry */
                                                     SOC_SAND_OUT uint32 *profile)
{  
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    
    soc_sand_dev_id = (unit);

    /* Verify if for Field, otherwise get the profile */
    if (egress_trap_entry >= BCM_DPP_AM_TRAP_EGRESS_MAX_ENTITIES_NO_FIELD) {
        
        *profile = _BCM_DPP_TRAP_EG_NO_ACTION;
    }
    else if (egress_trap_entry != SOC_PPC_TRAP_EG_TYPE_NONE)
    {
        soc_sand_rv = soc_ppd_trap_to_eg_action_map_get(soc_sand_dev_id, trap_id, profile);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("does not expect change profile for EG_TYPE_NONE trap")));
    }

    if (*profile == SOC_PPC_TRAP_EG_NO_ACTION) {
        /* Set BCM NO ACTION */
        *profile = _BCM_DPP_TRAP_EG_NO_ACTION;
    }

exit:
    BCMDNX_FUNC_RETURN;

}


/* Trap- egress - End */



/* Trap- L2 reserved MC - Start */

int _bcm_dpp_am_template_l2_trap_rm_init(int unit,int template_init_id,bcm_dpp_user_defined_traps_t* init_data)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    
    /*flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;*/
    
    /* Add all entities to template init id with given data */
    for (index = 0; index < _DPP_AM_TEMPLATE_L2_TRAP_RSRVD_MC_MAX_ENTITIES(unit); index++) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_trap_reserved_mc, flags, init_data, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* return trap code (0-7) given port and mac-lsb */
int
_bcm_dpp_am_template_l2_trap_rm_port_profile_mapping_get(int unit, int port, int mac_lsb, int *trap_code)
{
  
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_LLP_TRAP_PORT_INFO
        trap_port_info;
    SOC_PPC_ACTION_PROFILE
        action_profile;
    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY rm_mac_key;
    uint32  pp_port;
    int     core;


    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    SOC_PPC_LLP_TRAP_RESERVED_MC_KEY_clear(&rm_mac_key);
    rm_mac_key.da_mac_address_lsb = mac_lsb;

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    /* get reserved mc profile */
    soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id, core, pp_port, &trap_port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rm_mac_key.reserved_mc_profile = trap_port_info.reserved_mc_profile;

    /* get action  profile */
    soc_sand_rv = soc_ppd_llp_trap_reserved_mc_info_get(soc_sand_dev_id,&rm_mac_key, &action_profile);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(action_profile.frwrd_action_strength == 0 && action_profile.snoop_action_strength == 0) {
        *trap_code = 0;
    }
    else
    {
        *trap_code = action_profile.trap_code - SOC_PPC_TRAP_CODE_RESERVED_MC_0 + 1;
    }

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_l2_trap_rm_data_get(int unit, int port, int mac_lsb, bcm_dpp_user_defined_traps_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_l2_trap_rm_port_profile_mapping_get(unit,port, mac_lsb, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_trap_reserved_mc,old_profile,data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 *  reserved MC traps info
 *  port + mac --> trap-info (bcm_dpp_user_defined_traps_t)
 */

int _bcm_dpp_am_template_l2_trap_rm_exchange(int unit, int flags, int port, int mac_lsb, bcm_dpp_user_defined_traps_t * data,
                                             int *old_template, int *is_last, int *template,int *is_allocated, int flag_with_id)
{
    int rc = BCM_E_NONE;
   
    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_l2_trap_rm_port_profile_mapping_get(unit,port,mac_lsb, old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    if(!(flags & BCM_DPP_AM_TEMPLATE_FLAG_CHECK)) {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_trap_reserved_mc,(SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE |flag_with_id) ,data,*old_template,is_last,template,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);   

    }
    else
    {
        /* BCM_DPP_AM_TEMPLATE_FLAG_CHECK is marked */
        rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_trap_reserved_mc,(SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE |flag_with_id),data,*old_template,is_last,template,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l2_trap_rm_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_trap_reserved_mc));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Trap- L2 reserved MC - End */


int
_bcm_dpp_am_template_l2cp_egress_profile_init(int unit,
                                    bcm_dpp_vsi_egress_profile_t* init_data)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    /* Add all entities to template init id with given data */
    for (index = 0; index < _DPP_AM_TEMPLATE_EGRESS_L2CP_PROFILE_MAX_ENTITIES(unit); index++) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                        dpp_am_template_l2cp_egress_profile, flags, init_data,
                        &is_allocated, &template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* return profile code (0-3) given outlif */
int
_bcm_dpp_am_template_l2cp_egress_profile_mapping_get(int unit,
                                                     int outlif, int *profile)
{
    int local_outlif_id;
    uint32 soc_sand_rv;
    uint32 outlif_profile = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /*Format outlif to local_outlif.*/
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_global_lif_mapping_global_to_local_get(unit,
                _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, outlif, &local_outlif_id));

    BCM_SAND_IF_ERR_EXIT(
        soc_ppd_eg_encap_lif_field_get(unit,local_outlif_id,
                SOC_PPC_EG_ENCAP_ENTRY_UPDATE_OUT_LIF_PROFILE,&outlif_profile));

    /*Pick l2cp_profile out from Out-LIF-profile (get the needed bits)*/
    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_get,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF,
                                      SOC_OCC_MGMT_INLIF_APP_ORIENTATION,
                                      &outlif_profile, (uint32 *)profile));
    BCMDNX_IF_ERR_EXIT(soc_sand_rv);

exit:
  BCMDNX_FUNC_RETURN;
}

/*outlif can be global_outlif or local_outlif here.*/
int
_bcm_dpp_am_template_l2cp_egress_profile_data_get(int unit, int outlif,
                                        bcm_dpp_l2cp_egress_profile_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /*Format outlif to local_outlif.*/
    /*rc = _bcm_dpp_global_lif_mapping_global_to_local_get(unit,
                _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, outlif, &local_outlif_id);
    BCMDNX_IF_ERR_EXIT(rc);*/

    /*Retrieve outlif_profile and l2cp_profile.*/
    rc = _bcm_dpp_am_template_l2cp_egress_profile_mapping_get(unit,
                                                outlif, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Get l2cp_egress_profile_data.*/
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                        dpp_am_template_l2cp_egress_profile, old_profile, data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_l2cp_egress_profile_exchange(int unit, int flags,
                            int outlif, bcm_dpp_l2cp_egress_profile_t * data,
                            int *old_template, int *is_last, int *template,
                            int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_l2cp_egress_profile_mapping_get(unit,
                                                        outlif, old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                            dpp_am_template_l2cp_egress_profile,
                            SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,
                            data, *old_template, is_last, template,
                            is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2cp_egress_profile_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2cp_egress_profile));
exit:
    BCMDNX_FUNC_RETURN;
}

/* L2CP profile Egress - End */


/* VSI Profile Egress - Start */

int _bcm_dpp_am_template_vsi_egress_profile_init(int unit, bcm_dpp_vsi_egress_profile_t* init_data)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    
    

    /* Add all entities to template init id with given data */
    for (index = 0; index < _DPP_AM_TEMPLATE_EGRESS_VSI_PROFILE_MAX_ENTITIES(unit); index++) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vsi_egress_profile, flags, init_data, &is_allocated, &template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* return profile code (0-3) given vsi */
int
_bcm_dpp_am_template_vsi_egress_profile_mapping_get(int unit, int vsi, int *profile)
{
  
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_VSI_INFO vsi_info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* get profile */
    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *profile = vsi_info.profile_egress;
    
    BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_vsi_egress_profile_data_get(int unit, int vsi, bcm_dpp_vsi_egress_profile_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_vsi_egress_profile_mapping_get(unit, vsi, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mtu_advanced_mode", 0)) {
        int soc_sand_rv;

        COMPILER_64_ZERO(data->eve_da_lsbs_bitmap);
        data->mtu = 0;

        if (old_profile != 0) {
            soc_sand_rv = soc_ppd_vsi_egress_mtu_get(unit, FALSE, old_profile, &(data->mtu));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    } else {
        rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vsi_egress_profile, old_profile, data);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vsi_egress_profile_exchange(int unit, int flags, int vsi, bcm_dpp_vsi_egress_profile_t * data,
                                             int *old_template, int *is_last, int *template, int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_vsi_egress_profile_mapping_get(unit, vsi, old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mtu_advanced_mode", 0))
    {
        *is_last = FALSE;
        *template = (old_template != 0) ? *old_template : 0;
        *is_allocated = (old_template != 0) ? TRUE : FALSE;
    }
    else
    {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vsi_egress_profile, SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,
                                   data, *old_template, is_last, template, is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN;
}
/* VSI PROFILE Egress - End */


/* VSI Profile Ingress - Start */

int _bcm_dpp_am_template_vsi_ingress_profile_init(int unit, bcm_dpp_vsi_ingress_profile_t* init_data)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    
    

    /* Add all entities to template init id with given data */
    for (index = 0; index < _DPP_AM_TEMPLATE_INGRESS_VSI_PROFILE_MAX_ENTITIES(unit); index++) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vsi_ingress_profile, flags, init_data, &is_allocated, &template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* return profile code (0-3) given vsi */
int
_bcm_dpp_am_template_vsi_ingress_profile_mapping_get(int unit, int vsi, int *profile)
{
  
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_VSI_INFO vsi_info;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    /* get profile */
    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *profile = vsi_info.profile_ingress & 0x3; /* only 2 lsbs are relevant */
    
    BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_vsi_ingress_profile_data_get(int unit, int vsi, bcm_dpp_vsi_ingress_profile_t * data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_vsi_ingress_profile_mapping_get(unit, vsi, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vsi_ingress_profile, old_profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vsi_ingress_profile_exchange(int unit, int flags, int vsi, bcm_dpp_vsi_ingress_profile_t * data,
                                             int *old_template, int *is_last, int *template, int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_vsi_ingress_profile_mapping_get(unit, vsi, old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vsi_ingress_profile, SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,
                               data, *old_template, is_last, template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* VSI PROFILE Ingress - End */


/* OAM ICC MAP - Start */
int _bcm_dpp_am_template_oam_icc_map_ndx_get(int unit, int mep_id, int *icc_map_reg_index)
{
    uint32 soc_sand_rv;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY mep_db_entry;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY_clear(&mep_db_entry);

    /* get icc_ndx */
    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, mep_id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *icc_map_reg_index = mep_db_entry.icc_ndx;
    
    BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_icc_map_ndx_data_get(int unit, int mep_id, SOC_PPC_OAM_ICC_MAP_DATA* data)
{
    int old_icc_ndx;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_icc_map_ndx_get(unit, mep_id, &old_icc_ndx);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_icc_map, old_icc_ndx, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_icc_map_ndx_exchange(int unit, int flags, int mep_id, SOC_PPC_OAM_ICC_MAP_DATA * data,
                                             int *old_icc_ndx, int *is_last, int *icc_ndx, int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_icc_map_ndx_get(unit, mep_id, old_icc_ndx);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_icc_map, SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,
                               data, *old_icc_ndx, is_last, icc_ndx, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_oam_icc_map_ndx_alloc(int unit, int flags, SOC_PPC_OAM_ICC_MAP_DATA *data, int *is_allocated, int *icc_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_icc_map, flags, data, is_allocated, icc_ndx);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_oam_icc_map_ndx_free(int unit, int icc_ndx, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_icc_map, icc_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* OAM ICC MAP - End */


/*OAM SA address (jericho only) - Start*/

int
_bcm_dpp_am_template_oam_sa_mac_address_alloc(int unit, int flags, uint8 *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_sa_mac, flags, data, is_allocated, profile);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_am_template_oam_sa_mac_address_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_sa_mac, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_oam_sa_mac_address_get(int unit, int profile, uint8* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_sa_mac, profile, data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/*OAM SA address (jericho only) - End*/


/* OAMP punt event handling profile - Start */

/** Comapre 2 profiles. 
 *  Returns 1 if they match, 0 otherwise. */
#define _DPP_ALLOC_MNGR_OAM_PUNT_EVENT_handling_PROFILES_MATCH(profile_1, profile_2) \
    (profile_1.punt_rate==profile_2.punt_rate && profile_1.punt_enable== profile_2.punt_enable&& profile_1.rx_state_update_enable ==profile_2.rx_state_update_enable \
    && profile_1.scan_state_update_enable ==profile_2.scan_state_update_enable && profile_1.mep_rdi_update_loc_enable ==profile_2.mep_rdi_update_loc_enable\
        && profile_1.mep_rdi_update_loc_clear_enable ==profile_2.mep_rdi_update_loc_clear_enable&& profile_1.mep_rdi_update_rx_enable ==profile_2.mep_rdi_update_rx_enable)

int _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc_internal(int unit, int flags, SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA *data, int *is_allocated, int *profile) {
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_punt_event_hendling_profile, flags, data, is_allocated, profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc(int unit, int flags, SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    
/* profile 0 must remain all zeros.*/
    if (data->punt_rate || data->punt_enable || data->rx_state_update_enable || data->scan_state_update_enable || data->mep_rdi_update_loc_enable 
        || data->mep_rdi_update_loc_clear_enable || data->mep_rdi_update_rx_enable) {
        /* A non zero profile must be managed by hand.*/
        int i;
        SOC_PPC_OAM_OAMP_PUNT_PROFILE_DATA existing_profile;
        for (i=1 ; i<_DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_COUNT ; ++i) {
            rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_oam_punt_event_hendling_profile, i ,&existing_profile );
            if (rc == BCM_E_NONE) {
                /* check if the existing profile matches the given profile. If so get outta here*/
                if (_DPP_ALLOC_MNGR_OAM_PUNT_EVENT_handling_PROFILES_MATCH((*data), existing_profile)) {
                    *profile = i;
                    return _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc_internal(unit,SW_STATE_RES_ALLOC_WITH_ID,data,is_allocated,profile);
                }
            }
        }

        /* reach this point if no existing profile matches. Search for a new one.*/
        for (i=1 ; i<_DPP_AM_TEMPLATE_OAM_OAMP_PUNT_PROFILE_COUNT ; ++i) {
             rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_oam_punt_event_hendling_profile, i ,&existing_profile );
             if (rc ==BCM_E_NOT_FOUND) {
                 /* This one is empty. Use it */
                 *profile = i;
                 return _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc_internal(unit,SW_STATE_RES_ALLOC_WITH_ID,data,is_allocated,profile);
             }
        }


    } else {
        /* Return profile 0.*/
        *profile=0;
        return _bcm_dpp_am_template_oam_punt_event_hendling_profile_alloc_internal(unit,SW_STATE_RES_ALLOC_WITH_ID,data,is_allocated,profile);
    }

    /*Reach this point if no profile was found (only in the non zero case)*/
    return BCM_E_FULL;
}

int _bcm_dpp_am_template_oam_punt_event_hendling_profile_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_punt_event_hendling_profile, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* OAMP punt event handling profile - End */

/* OAM&BFD mep profile general - Start */
int
_bcm_dpp_am_template_oam_mep_profile_alloc(int unit, _dpp_am_template_t template_type, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_type, flags, data, is_allocated, profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_oam_mep_profile_free(int unit, _dpp_am_template_t template_type, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_type, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_data_from_profile_get(int unit, _dpp_am_template_t template_type, int profile, SOC_PPC_OAM_LIF_PROFILE_DATA* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_type, profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_mapping_get(int unit, int mep_id, int is_accelerated, int is_passive, int *profile)
{
    int rc = BCM_E_NONE;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;

    /* get profile */
    rc = _bcm_dpp_oam_bfd_mep_info_db_get(unit, mep_id, &mep_info, &found);
    BCMDNX_IF_ERR_EXIT(rc);

    if (found) {
        *profile = is_accelerated ? mep_info.acc_profile : is_passive ? mep_info.non_acc_profile_passive : mep_info.non_acc_profile;
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG_NO_UNIT("Endpoint with id=%d wasn't found \n"),mep_id));
    }
    
    BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_data_get(int unit, int mep_id, _dpp_am_template_t template_type, int is_accelerated, int is_passive, SOC_PPC_OAM_LIF_PROFILE_DATA* data)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_oam_mep_profile_mapping_get(unit, mep_id, is_accelerated, is_passive, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_type, old_profile, data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_exchange(int unit, _dpp_am_template_t template_type, int is_accelerated, int is_passive, int flags, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_mapping_get(unit, mep_id, is_accelerated, is_passive, old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_type, SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,
                                  data, *old_profile, is_last, profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* OAM&BFD mep profile general - End */

/* OAM mep profile accelerated - Start */

int
_bcm_dpp_am_template_oam_mep_profile_accelerated_alloc(int unit, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_alloc(unit, dpp_am_template_oam_mep_profile_accelerated, flags, data, is_allocated, profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_oam_mep_profile_accelerated_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_free(unit, dpp_am_template_oam_mep_profile_accelerated, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_accelerated_data_get(int unit, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_data_get(unit, mep_id, dpp_am_template_oam_mep_profile_accelerated, 1, 0/*is_passive*/, data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_accelerated_mapping_get(int unit, int mep_id, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_mapping_get(unit, mep_id, 1, 0/*is_passive*/, profile);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_accelerated_exchange(int unit, int flags, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_exchange(unit, dpp_am_template_oam_mep_profile_accelerated, 1, 0/*is_passive*/, flags, mep_id, data,
                                             old_profile, is_last, profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* OAM mep profile accelerated - End */


/* OAM mep profile non accelerated - Start */
int
_bcm_dpp_am_template_oam_mep_profile_non_accelerated_alloc(int unit, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_alloc(unit, dpp_am_template_oam_mep_profile_non_accelerated, flags, data, is_allocated, profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_oam_mep_profile_non_accelerated_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_free(unit, dpp_am_template_oam_mep_profile_non_accelerated, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_from_profile_get(int unit, int profile, SOC_PPC_OAM_LIF_PROFILE_DATA* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_oam_mep_profile_data_from_profile_get(unit, dpp_am_template_oam_mep_profile_non_accelerated, profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_data_get(int unit, int mep_id, uint8 is_passive, SOC_PPC_OAM_LIF_PROFILE_DATA* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_data_get(unit, mep_id, dpp_am_template_oam_mep_profile_non_accelerated, 0, is_passive, data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Checks if a mep has a valid profile assigned
   Returns:
   0 - No valid mep profile was found
   1 - Valid non-accelerated profile was found
   2 - Valid accelerated profile was found
*/
int _bcm_dpp_am_template_oam_mep_profile_data_validate(int unit, int mep_id, uint8 is_passive, SOC_PPC_OAM_LIF_PROFILE_DATA* data, int *valid)
{
    int old_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_oam_mep_profile_mapping_get(unit, mep_id, 0 /*is_accelerated*/, is_passive, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile > 0) {
       *valid = 1;
    } else {
       /* Non-accelerates profile was not found, checking for accelerated profile */
       rc = _bcm_dpp_am_template_oam_mep_profile_mapping_get(unit, mep_id, 1 /*is_accelerated*/, is_passive, &old_profile);
       BCMDNX_IF_ERR_EXIT(rc);
       if (old_profile > 0) {
          *valid = 2;
       } else {
          *valid = 0;
       }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_mapping_get(int unit, int mep_id, int is_passive, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_mapping_get(unit, mep_id, 0, is_passive, profile);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_mep_profile_non_accelerated_exchange(int unit, int flags, int mep_id, int is_passive, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_exchange(unit, dpp_am_template_oam_mep_profile_non_accelerated, 0, is_passive, flags, mep_id, data,
                                             old_profile, is_last, profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}/* OAM mep profile non accelerated - End */

/* BFD mep profile accelerated - Start */
int
_bcm_dpp_am_template_bfd_mep_profile_accelerated_alloc(int unit, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_alloc(unit, dpp_am_template_bfd_mep_profile_accelerated, flags, data, is_allocated, profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_bfd_mep_profile_accelerated_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_free(unit, dpp_am_template_bfd_mep_profile_accelerated, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_mep_profile_accelerated_data_get(int unit, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_data_get(unit, mep_id, dpp_am_template_bfd_mep_profile_accelerated, 1, 0/*is_passive*/, data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_mep_profile_accelerated_mapping_get(int unit, int mep_id, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_mapping_get(unit, mep_id, 1, 0/*is_passive*/, profile);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_mep_profile_accelerated_exchange(int unit, int flags, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_exchange(unit, dpp_am_template_bfd_mep_profile_accelerated, 1, 0/*is_passive*/, flags, mep_id, data,
                                             old_profile, is_last, profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* BFD mep profile accelerated - End */

/* BFD mep profile non accelerated - Start */
int
_bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(int unit, int flags, SOC_PPC_OAM_LIF_PROFILE_DATA *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_alloc(unit, dpp_am_template_bfd_mep_profile_non_accelerated, flags, data, is_allocated, profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_bfd_mep_profile_non_accelerated_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_free(unit, dpp_am_template_bfd_mep_profile_non_accelerated, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_data_get(int unit, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_data_get(unit, mep_id, dpp_am_template_bfd_mep_profile_non_accelerated, 0, 0/*is_passive*/, data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_mapping_get(int unit, int mep_id, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_mapping_get(unit, mep_id, 0, 0/*is_passive*/, profile);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_exchange(int unit, int flags, int mep_id, SOC_PPC_OAM_LIF_PROFILE_DATA * data,
                                             int *old_profile, int *is_last, int *profile, int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_mep_profile_exchange(unit, dpp_am_template_bfd_mep_profile_non_accelerated, 0, 0/*is_passive*/, flags, mep_id, data,
                                             old_profile, is_last, profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}/* BFD mep profile non accelerated - End */


/* OAM SD SF - Start */
int _bcm_dpp_am_template_oam_sd_sf_profile_alloc(int unit, int flags,const SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB *data, int *is_allocated, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_sd_sf_profile, flags, data, is_allocated, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_sd_sf_profile_free(int unit, int profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_sd_sf_profile, profile_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_sd_sf_profile_data_get(int unit, int profile_ndx, SOC_PPC_OAM_OAMP_SD_SF_PROFILE_DB* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_sd_sf_profile, profile_ndx, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* OAM SD SF  - End */
/* OAM Priority - Start */
int _bcm_dpp_am_template_oam_tx_priority_alloc(int unit, int flags, const SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES *data, int *is_allocated, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_tx_priority, flags, data, is_allocated, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_tx_priority_free(int unit, int profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_tx_priority, profile_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_tx_priority_exchange(int unit, int flags, const SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES *data, int *is_allocated, int *profile_ndx, int old_profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_tx_priority, flags, data, old_profile_ndx, is_last, profile_ndx, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_tx_priority_data_get(int unit, int profile_ndx, SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_tx_priority, profile_ndx, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* OAM Priority - End */

/* BFD IPV4 DIP - Start */
int _bcm_dpp_am_template_bfd_ip_dip_alloc(int unit, int flags,const SOC_SAND_PP_IPV6_ADDRESS *data, int *is_allocated, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_ip_dip, flags, data, is_allocated, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_ip_dip_free(int unit, int profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_ip_dip, profile_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_ip_dip_data_get(int unit, int profile_ndx, SOC_SAND_PP_IPV6_ADDRESS* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_ip_dip, profile_ndx, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* BFD IPV4 DIP - End */

/* BFD Push profile - Start */
int _bcm_dpp_am_template_mpls_pwe_push_profile_alloc(int unit, int flags, const SOC_PPC_MPLS_PWE_PROFILE_DATA *data, int *is_allocated, int *mpls_pwe_profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mpls_pwe_push_profile, flags, data, is_allocated, mpls_pwe_profile);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_mpls_pwe_push_profile_data_get(int unit, int mpls_pwe_profile, SOC_PPC_MPLS_PWE_PROFILE_DATA* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mpls_pwe_push_profile, mpls_pwe_profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_mpls_pwe_push_profile_free(int unit, int mpls_pwe_profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mpls_pwe_push_profile, mpls_pwe_profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/* BFD Push profile - End */

/* BFD Req interval pointer - Start */
int _bcm_dpp_am_template_bfd_req_interval_pointer_alloc(int unit, int flags, const uint32 *data, int *is_allocated, int *rx_rate_pointer)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_req_interval_pointer, flags, data, is_allocated, rx_rate_pointer);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_req_interval_pointer_data_get(int unit, int rx_rate_pointer, uint32* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_req_interval_pointer, rx_rate_pointer, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_req_interval_pointer_free(int unit, int rx_rate_pointer, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_req_interval_pointer, rx_rate_pointer, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* BFD Req interval pointer - End */

/* BFD TOS TTL profile - Start */
int _bcm_dpp_am_template_bfd_tos_ttl_profile_alloc(int unit, int flags, const SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA *data, int *is_allocated, int *tos_ttl_profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_tos_ttl_profile, flags, data, is_allocated, tos_ttl_profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_tos_ttl_profile_data_get(int unit, int tos_ttl_profile, SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_tos_ttl_profile, tos_ttl_profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_tos_ttl_profile_free(int unit, int tos_ttl_profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_tos_ttl_profile, tos_ttl_profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* BFD TOS TTL profile - End */


/* Seamless BFD non acc src ip profile - Start */ 
int _bcm_dpp_am_template_sbfd_non_acc_src_ip_profile_alloc(int unit, int flags, const uint32 *data, int *is_allocated, int *src_ip_add_profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_sbfd_non_acc_src_ip_profile, flags, data, is_allocated, src_ip_add_profile);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_sbfd_non_acc_src_ip_profile_data_get(int unit, int src_ip_add_profile, uint32* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_sbfd_non_acc_src_ip_profile, src_ip_add_profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_sbfd_non_acc_src_ip_profile_free(int unit, int src_ip_add_profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_sbfd_non_acc_src_ip_profile, src_ip_add_profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_sbfd_non_acc_src_ip_profile_exchange(int unit, int flags, const uint32 *data, int *is_allocated, int *src_ip_add_profile, int old_src_ip_add_profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_sbfd_non_acc_src_ip_profile, flags, data, old_src_ip_add_profile, is_last, src_ip_add_profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* Seamless BFD non acc src ip profile - End */

/* BFD src ip profile - Start */ 
int _bcm_dpp_am_template_bfd_src_ip_profile_alloc(int unit, int flags, const uint32 *data, int *is_allocated, int *src_ip_add_profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_src_ip_profile, flags, data, is_allocated, src_ip_add_profile);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_src_ip_profile_data_get(int unit, int src_ip_add_profile, uint32* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_src_ip_profile, src_ip_add_profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_src_ip_profile_free(int unit, int src_ip_add_profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_src_ip_profile, src_ip_add_profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* BFD src ip profile - End */


/* oamp_pe_gen_mem MAID48  - Start */
int _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_alloc(int unit, int flags, uint8 *data, int *is_allocated, int *maid_48)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oamp_pe_gen_mem_maid_48, flags, data, is_allocated, maid_48);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_data_get(int unit, int maid_48, uint8* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oamp_pe_gen_mem_maid_48, maid_48, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_free(int unit, int maid_48, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oamp_pe_gen_mem_maid_48, maid_48, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* oamp_pe_gen_mem MAID48 - End */

/* BFD flags profile*/
int _bcm_dpp_am_template_bfd_flags_profile_alloc(int unit, int flags, const uint32 *data, int *is_allocated, int *profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_flags_profile, flags, data, is_allocated,profile);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_flags_profile_data_get(int unit, int profile, uint32* data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_flags_profile,profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_bfd_flags_profile_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_bfd_flags_profile, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* BFD flags profile*/



/**OAM - manage LMM DA NIC tables, ARAD+, start */
int _bcm_dpp_am_template_oam_lmm_nic_tables_alloc(int unit, int flags, uint32* data, int *is_allocated, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_lmm_nic_tables_profile, flags, data, is_allocated, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_oam_lmm_nic_tables_free(int unit, int mep_id, int *profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;
    uint32 soc_sand_rv, profile;
    BCMDNX_INIT_FUNC_DEFS;

    /* first, get the actual profile index.*/
    soc_sand_rv = soc_ppd_oam_oamp_nic_profile_get(unit, mep_id,&profile );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *profile_ndx = profile;

    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_lmm_nic_tables_profile, *profile_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_oam_lmm_nic_tables_get(int unit, int mep_id, int *profile_ndx, uint32* data)
{
    int rc = BCM_E_NONE;
    uint32 soc_sand_rv, profile;

    BCMDNX_INIT_FUNC_DEFS;

    /* first, get the actual profile index.*/
    soc_sand_rv = soc_ppd_oam_oamp_nic_profile_get(unit, mep_id, &profile);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *profile_ndx = profile; 

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_lmm_nic_tables_profile, *profile_ndx, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/**OAM - manage LMM  DA NIC/OUI tables, ARAD+, end */

/**OAM - manage LMM DA OUI tables, ARAD+, start */
int _bcm_dpp_am_template_oam_lmm_oui_tables_alloc(int unit, int flags, uint32* data, int *is_allocated, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_lmm_oui_tables_profile, flags, data, is_allocated, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_oam_lmm_oui_tables_free(int unit, int mep_id, int *profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;
    uint32 soc_sand_rv, profile;
    uint32 who_cares;
    BCMDNX_INIT_FUNC_DEFS;

    /*first, get the actual profile*/
    soc_sand_rv = soc_ppd_oam_oamp_eth1731_and_oui_profiles_get(unit, mep_id, &who_cares, &profile);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *profile_ndx = profile;


    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_lmm_oui_tables_profile, *profile_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_oam_lmm_oui_tables_get(int unit, int mep_id, int *profile_ndx, uint32* data)
{
    int rc = BCM_E_NONE;
    uint32 soc_sand_rv, profile;
    uint32 who_cares;
    BCMDNX_INIT_FUNC_DEFS;

        /*first, get the actual profile*/
    soc_sand_rv = soc_ppd_oam_oamp_eth1731_and_oui_profiles_get(unit, mep_id, &who_cares, &profile);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *profile_ndx = profile;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_lmm_oui_tables_profile, *profile_ndx, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/*OAM - manage LMM  DA OUI tables, ARAD+, end */

/*OAM - manage local_port 2 system port table,  start */
int _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get_internal(int unit, int local_port, _dpp_oam_am_local_port_2_sys_port_internal_data_t *data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_local_port_2_system_port, local_port, data);
    if (rc == BCM_E_NOT_FOUND) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(int unit, int flags, const uint32* data, int *is_allocated, int *profile_ndx)
{
    int rc = BCM_E_NONE;
    _dpp_oam_am_local_port_2_sys_port_internal_data_t internal_data;

    BCMDNX_INIT_FUNC_DEFS;

    internal_data.data = *data;

    if (flags & BCM_DPP_AM_FLAG_ALLOC_RESERVE) {
        internal_data.profile  = *profile_ndx;
        internal_data.data     = SOC_DPP_OAM_LOCAL_PORT_2_SYSTEM_PORT_RESERVED_VALUE;
        flags                  = SW_STATE_RES_ALLOC_WITH_ID;
    } else if (flags & SHR_RES_ALLOC_WITH_ID) {
        /* check if entry is reserved */
        _dpp_oam_am_local_port_2_sys_port_internal_data_t cur_internal_data;
        int is_last;

        rc = _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get_internal(unit, *profile_ndx, &cur_internal_data);
        if (rc!=BCM_E_NOT_FOUND) {
            BCMDNX_IF_ERR_EXIT(rc);
        }
        if (rc==BCM_E_NONE && cur_internal_data.data == SOC_DPP_OAM_LOCAL_PORT_2_SYSTEM_PORT_RESERVED_VALUE) {
            /* Entry was reserved  - free entry and re-allocate */
           rc = _bcm_dpp_am_template_oam_local_port_2_sys_port_free(unit, *profile_ndx, &is_last);
           if (rc!=BCM_E_NOT_FOUND) {
               BCMDNX_IF_ERR_EXIT(rc);
           }
        }
        internal_data.profile = *profile_ndx;
    } else {
        /* gulp!*/
        /* Search for a profile with the same data by hand.
           See remarks on the internal_data struct definition.*/
        int local_port;
        _dpp_oam_am_local_port_2_sys_port_internal_data_t cur_internal_data;

        /*
         * Use an invalid profile. Placing 0 here will not allow us to use profile 0 WITH_ID if the same port has
         * already been used without ID.
         */
        internal_data.profile=_DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_COUNT;
        for (local_port=0; local_port < _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_COUNT ; ++local_port) {
            rc = _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get_internal(unit, local_port, &cur_internal_data);
            if (rc!=BCM_E_NOT_FOUND) {
                BCMDNX_IF_ERR_EXIT(rc); 
            }
            if (rc==BCM_E_NONE && cur_internal_data.data == internal_data.data) {
                /* Found a match. Set internal data and get out.*/
                internal_data.profile = cur_internal_data.profile;
                break;
            }
        }
    }

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_local_port_2_system_port, flags, &internal_data, is_allocated, profile_ndx);
    if (rc!=BCM_E_NONE) {
        BCMDNX_IF_ERR_EXIT(rc); 
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get(int unit, int local_port, uint32 *data)
{
    int rc = BCM_E_NONE;
    _dpp_oam_am_local_port_2_sys_port_internal_data_t internal_data;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get_internal(unit, local_port,&internal_data);
    if (rc == BCM_E_NOT_FOUND) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rc);

    *data = internal_data.data;

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_oam_local_port_2_sys_port_free(int unit, int profile, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_local_port_2_system_port, profile, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Reserves entry for SW_STATE_RES_ALLOC_WITH_ID allocations (for OAM) */
int _bcm_dpp_am_template_oam_local_port_2_sys_port_entry_reserve(int unit, int local_port)
{
int      rc = BCM_E_NONE;
int      is_allocated;
uint32   data;

   BCMDNX_INIT_FUNC_DEFS;   

   if ((local_port < 0)||(local_port>=SOC_PPC_OAM_SIZE_OF_LOCAL_PORT_2_SYS_PORT_TABLE(unit))) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error - Attempt to reserve illegal entry. \n")));
   }

   rc = _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get(unit, local_port, &data);
   if (rc == BCM_E_NOT_FOUND) {
      data = SOC_DPP_OAM_LOCAL_PORT_2_SYSTEM_PORT_RESERVED_VALUE;
      rc = _bcm_dpp_am_template_oam_local_port_2_sys_port_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_RESERVE , &data, &is_allocated, &local_port);
      BCMDNX_IF_ERR_EXIT(rc); 
   }
   else
   {
      if (rc!=BCM_E_NONE) {
          BCMDNX_IF_ERR_EXIT(rc); 
      }
      else
      {  /* entry was already allocated */
         if (data == SOC_DPP_OAM_LOCAL_PORT_2_SYSTEM_PORT_RESERVED_VALUE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error - Attempt to re-reserve a reserved entry. \n")));
         }
         else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error - Attempt to reserve an allocated entry. \n")));
         }
      }
   }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Unreserves entry */
int _bcm_dpp_am_template_oam_local_port_2_sys_port_entry_unreserve(int unit, int local_port)
{
int  rc = BCM_E_NONE;
int   is_last;
_dpp_oam_am_local_port_2_sys_port_internal_data_t internal_data;
   
   BCMDNX_INIT_FUNC_DEFS;   

   if ((local_port < 0)||(local_port>=SOC_PPC_OAM_SIZE_OF_LOCAL_PORT_2_SYS_PORT_TABLE(unit))) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error - Attempt to unreserve illegal entry. \n")));
   }

   rc = _bcm_dpp_am_template_oam_local_port_2_sys_port_data_get_internal(unit, local_port, &internal_data);
   if (rc == BCM_E_NOT_FOUND) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error - Attempt to unreserve a not reserved entry. \n")));
   }
   if (rc!=BCM_E_NONE) {
       BCMDNX_IF_ERR_EXIT(rc); 
   }
   if (internal_data.data != SOC_DPP_OAM_LOCAL_PORT_2_SYSTEM_PORT_RESERVED_VALUE) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error - Attempt to unreserve an allocated entry. \n")));
   }
    rc = _bcm_dpp_am_template_oam_local_port_2_sys_port_free(unit, local_port,&is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/*OAM - manage local_port 2 system port table,  end */


/* OAM OAMP PE GEN memory start */
int _bcm_dpp_am_template_oam_oamp_pe_gen_mem_data_get_internal(int unit, int gen_mem_data, _dpp_oam_oamp_pe_gen_mem_internal_data_t *data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_oamp_pe_gen_mem, gen_mem_data, data);
    if (rc == BCM_E_NOT_FOUND) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rc);


exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_oamp_pe_gen_mem_alloc(int unit, int flags, const uint32* data, int *is_allocated, int *profile_ndx)
{
    int rc = BCM_E_NONE;
    _dpp_oam_oamp_pe_gen_mem_internal_data_t internal_data;

    BCMDNX_INIT_FUNC_DEFS;

    internal_data.data = *data;

    if (flags &SW_STATE_RES_ALLOC_WITH_ID) {
        internal_data.profile = *profile_ndx;
    } else {
        /* gulp!*/
        /* Search for a profile with the same data by hand.
           See remarks on the internal_data struct definition.*/
        int gen_mem_data;
        _dpp_oam_oamp_pe_gen_mem_internal_data_t cur_internal_data; 

        internal_data.profile=0;
        for (gen_mem_data=0; gen_mem_data < _DPP_AM_TEMPLATE_OAM_LOCAL_PORT_2_SYS_PORT_COUNT ; ++gen_mem_data) {
            rc = _bcm_dpp_am_template_oam_oamp_pe_gen_mem_data_get_internal(unit, gen_mem_data, &cur_internal_data);
            if (rc!=BCM_E_NOT_FOUND) {
                BCMDNX_IF_ERR_EXIT(rc); 
            }
            if (rc==BCM_E_NONE && cur_internal_data.data == internal_data.data) {
                /* Found a match. Set internal data and get out.*/
                internal_data.profile = cur_internal_data.profile;
                break;
            }
        }
    }

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_oamp_pe_gen_mem, flags, &internal_data, is_allocated, profile_ndx);
    if (rc!=BCM_E_NONE) {
        BCMDNX_IF_ERR_EXIT(rc); 
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_oamp_pe_gen_mem_data_get(int unit, int local_port, uint32 *data)
{
    int rc = BCM_E_NONE;
    _dpp_oam_oamp_pe_gen_mem_internal_data_t internal_data;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_oam_oamp_pe_gen_mem_data_get_internal(unit, local_port,&internal_data);
    if (rc == BCM_E_NOT_FOUND) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rc);

    *data = internal_data.data;

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_oam_oamp_pe_gen_mem_free(int unit, int mep_id, int *is_last)
{
    int rc = BCM_E_NONE;
    SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  mep_db_entry;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_rv = soc_ppd_oam_oamp_mep_db_entry_get(unit, mep_id, &mep_db_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_oamp_pe_gen_mem, mep_db_entry.local_port, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* OAM PE GEN memory end */


/*OAM - manage LMM/DMM mep profile, ARAD+, start */

/** Always first  (for compilation)   */
int _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc(int unit, SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY* data,int flags, int *is_allocated, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, flags, data, is_allocated, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc_with_id(int unit, int profile_ndx)
{
    int rc = BCM_E_NONE;
    int who_cares;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY data;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, profile_ndx, &data);
    BCMDNX_IF_ERR_EXIT(rc);

     return _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc(unit, &data, SW_STATE_RES_ALLOC_WITH_ID, &who_cares, &profile_ndx);
exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_oam_eth_1731_mep_profile_RDI_alloc(int unit,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data , int * is_allocated, int * profile_ndx)
{
    int rc,profile;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY  old_eth1731_profile;

    /* First, search for an existing entry with the same RDI. */
    for (profile =_DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_LOW_ID ; profile< _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_COUNT ; ++profile) {
        rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, profile, &old_eth1731_profile);
        if (rc == BCM_E_NONE && old_eth1731_profile.rdi_gen_method == data->rdi_gen_method && old_eth1731_profile.piggy_back_lm==0 &&
            !old_eth1731_profile.report_mode && !old_eth1731_profile.slm_lm && old_eth1731_profile.maid_check_dis == data->maid_check_dis
            && old_eth1731_profile.ccm_cnt == data->ccm_cnt && old_eth1731_profile.dmm_cnt == data->dmm_cnt && old_eth1731_profile.lmm_cnt == data->lmm_cnt) {
            /* Report mode, SLM, Piggy back bits must be off. Otherwise CCMs may not be sent.*/
            /* This one will do fine.*/
            *profile_ndx = profile;
            *is_allocated = 0;
            return _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc_with_id(unit,*profile_ndx);
        }
    }

    /* No existing profile matches. Allocate a new one.*/
    return _bcm_dpp_am_template_oam_eth1731_mep_profile_alloc(unit,data,0,is_allocated,profile_ndx);
}

int _bcm_dpp_am_template_oam_eth_1731_mep_profile_alloc_by_component( int unit, int mep_id,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data , int *is_allocated, int *profile_ndx, 
                                                             int *old_profile, int *is_last, int is_first , int one_lm_two_dm_three_loopback, uint8 is_scan_count_updated)
{
    int rc,profile;
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY old_eth1731_profile;

    BCMDNX_INIT_FUNC_DEFS;


    rc =  _bcm_dpp_am_template_oam_eth1731_mep_profile_get(unit, mep_id, old_profile, &old_eth1731_profile);
    BCMDNX_IF_ERR_EXIT(rc);
    data->rdi_gen_method = old_eth1731_profile.rdi_gen_method; 
    if(!is_scan_count_updated) {
        data->ccm_cnt = old_eth1731_profile.ccm_cnt;
    }
    if (one_lm_two_dm_three_loopback!=2) {
        /* Loopback or LM. Fill DMM fields*/
        data->dmm_rate = old_eth1731_profile.dmm_rate;
        if(!is_scan_count_updated) {
            data->dmm_cnt = old_eth1731_profile.dmm_cnt;
        }
        data->dmm_offset = old_eth1731_profile.dmm_offset ;
        data->dmr_offset = old_eth1731_profile.dmr_offset ;
        data->maid_check_dis = old_eth1731_profile.maid_check_dis ;
        data->dmm_measure_one_way = old_eth1731_profile.dmm_measure_one_way ;
    }

    if (one_lm_two_dm_three_loopback!=1) {
        /* Loopback or DM. Fill LMM fields*/
        data->lmm_rate = old_eth1731_profile.lmm_rate;
        if(!is_scan_count_updated) {
            data->lmm_cnt = old_eth1731_profile.lmm_cnt;
        }
        data->piggy_back_lm = old_eth1731_profile.piggy_back_lm;
        data->slm_lm = old_eth1731_profile.slm_lm;
        data->lmm_offset = old_eth1731_profile.lmm_offset ;
        data->lmr_offset = old_eth1731_profile.lmr_offset ;
        data->maid_check_dis = old_eth1731_profile.maid_check_dis ;
        one_lm_two_dm_three_loopback +=2;
    }
    /* In Jericho the phase has been set for data in any case*/


    if (is_first) {
        /* Search for the profile with only the relevant fields matching*/
        for (profile = _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_LOW_ID; profile < _DPP_AM_TEMPLATE_OAM_ETH1731_PROFILE_COUNT; ++profile) {
            rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, profile, &old_eth1731_profile);
            if (rc == BCM_E_NONE) {
                /* Found an active profile. See if it fits.*/
                if (old_eth1731_profile.piggy_back_lm != data->piggy_back_lm || 
                    old_eth1731_profile.lmm_da_oui_prof != data->lmm_da_oui_prof ||
                    old_eth1731_profile.slm_lm != data->slm_lm ||
                    old_eth1731_profile.report_mode != data->report_mode ||
                    old_eth1731_profile.rdi_gen_method != data->rdi_gen_method)  {
                    /* Can't have an entry with the piggy back, slm , report mode fields set - otherwise CCMs may not be sent.*/
                    /*Likewise lmm_da_oui profile must match */
                    continue;
                }
                if (one_lm_two_dm_three_loopback == 1 && old_eth1731_profile.lmm_rate == data->lmm_rate &&
                    old_eth1731_profile.piggy_back_lm == data->piggy_back_lm && old_eth1731_profile.lmm_offset == data->lmm_offset &&
                                                                                old_eth1731_profile.lmm_cnt == data->lmm_cnt) {
                    /* LM -it fits. */
                    *profile_ndx = profile;
                    *is_allocated = *is_last =0;
                    if (*old_profile != *profile_ndx) {
                        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, 
                                                      SHR_TEMPLATE_MANAGE_IGNORE_DATA | SHR_TEMPLATE_MANAGE_SET_WITH_ID ,
                                                   &old_eth1731_profile, *old_profile, is_last, profile_ndx, is_allocated); 
                        BCMDNX_IF_ERR_EXIT(rc);
                    }


                    BCM_EXIT;

                }  
                if (one_lm_two_dm_three_loopback == 2 && old_eth1731_profile.dmm_rate == data->dmm_rate
                    && old_eth1731_profile.dmm_offset == data->dmm_offset &&
                        old_eth1731_profile.dmm_cnt == data->dmm_cnt) {
                    /*DM - it fits */
                    *profile_ndx = profile;
                    *is_allocated = *is_last =0;
                    if (*old_profile != *profile_ndx) {
                        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, 
                                                      SHR_TEMPLATE_MANAGE_IGNORE_DATA | SHR_TEMPLATE_MANAGE_SET_WITH_ID,
                                                      &old_eth1731_profile, *old_profile, is_last, profile_ndx, is_allocated);
                        BCMDNX_IF_ERR_EXIT(rc); 
                    }

                    BCM_EXIT;
                } 
                if (one_lm_two_dm_three_loopback == 3) {
                    /* Loopback - fields other than oui profile don't matter*/
                    *profile_ndx = profile;
                    *is_allocated = *is_last = 0; 
                    if (*old_profile != *profile_ndx) {
                        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, 
                                                      SHR_TEMPLATE_MANAGE_IGNORE_DATA | SHR_TEMPLATE_MANAGE_SET_WITH_ID,
                                                      &old_eth1731_profile, *old_profile, is_last, profile_ndx, is_allocated);
                        BCMDNX_IF_ERR_EXIT(rc); 
                    }

                    BCM_EXIT;
                }

            }
        }
    }

    /*Reach this point if no existing profile fits. Simply exchange.*/
    rc = _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(unit, *old_profile, data, is_allocated, is_last, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc); 

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_eth_1731_mep_profile_loss_alloc(int unit, int mep_id,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data , int *is_allocated, int *profile_ndx, 
                                                             int *old_profile, int *is_last, int is_first, uint8 is_scan_count_updated )
{

    return _bcm_dpp_am_template_oam_eth_1731_mep_profile_alloc_by_component(unit, mep_id, data,is_allocated, profile_ndx, old_profile, is_last,is_first,1 /* one for lm*/,is_scan_count_updated);
}

int _bcm_dpp_am_template_oam_eth_1731_mep_profile_delay_alloc(int unit, int mep_id ,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data, int *is_allocated, int *profile_ndx, 
                                                             int *old_profile, int *is_last, int is_first, uint8 is_scan_count_updated )
{
    return _bcm_dpp_am_template_oam_eth_1731_mep_profile_alloc_by_component(unit, mep_id, data, is_allocated, profile_ndx, old_profile, is_last,is_first ,2 /* two for dm*/,is_scan_count_updated);
}
int _bcm_oam_am_template_oam_eth_1731_mep_profile_loopback_alloc(int unit, int mep_id ,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY *data, int *is_allocated, int *profile_ndx, 
                                                             int *old_profile, int *is_last , int is_first, uint8 is_scan_count_updated)
{
    return _bcm_dpp_am_template_oam_eth_1731_mep_profile_alloc_by_component(unit, mep_id, data, is_allocated, profile_ndx, old_profile, is_last, is_first,3 /* three for LB*/,is_scan_count_updated);
}



int _bcm_dpp_am_template_oam_eth1731_mep_profile_free_internal(int unit, int profile_ndx, int * is_last)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, profile_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_oam_eth1731_mep_profile_free(int unit,int mep_id, int *profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 dont_care, eth_profile;
    BCMDNX_INIT_FUNC_DEFS;

        /* first, get the actual profile. */
    soc_sand_rv = soc_ppd_oam_oamp_eth1731_and_oui_profiles_get(unit, mep_id, &eth_profile, &dont_care);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *profile_ndx = eth_profile;

    /* now free*/
    rc = _bcm_dpp_am_template_oam_eth1731_mep_profile_free_internal(unit,*profile_ndx,is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_oam_eth1731_mep_profile_get(int unit ,int mep_id, int* profile_ndx, SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY* data)
{
    int rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 dont_care, eth_profile;

    BCMDNX_INIT_FUNC_DEFS;

    /* first, get the actual profile. */
    soc_sand_rv = soc_ppd_oam_oamp_eth1731_and_oui_profiles_get(unit, mep_id, &eth_profile, &dont_care);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *profile_ndx = eth_profile;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, *profile_ndx, data);

    if (rc == BCM_E_NOT_FOUND) {
        BCM_ERR_EXIT_NO_MSG(BCM_E_NOT_FOUND);
    }
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_oam_eth1731_mep_profile_exchange(int unit, int old_profile_indx,SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY* data, int *is_allocated,int *is_last, int *profile_ndx )
{
    int rc = 0;
    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_oam_eth1731_profile, SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,
                               data, old_profile_indx, is_last, profile_ndx, is_allocated); 
	BCMDNX_IF_ERR_EXIT(rc);

BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*OAM - manage LMM/DMM mep profile, ARAD+, start */



/* Prog Trap - Start */

int _bcm_dpp_am_template_l2_prog_trap_init(int unit,int template_init_id,SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *init_data)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_prog_trap, flags, init_data,_DPP_AM_TEMPLATE_PROG_TRAP_MAX_ENTITIES(unit), &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* return trap code (0-7) given port and mac-lsb */
STATIC int
_bcm_dpp_am_template_l2_prog_trap_port_profile_mapping_get(int unit, int port, int type, int *profile)
{
  
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_LLP_TRAP_PORT_INFO
        trap_port_info;
    int trap_bit;
    uint32  pp_port;
    int     core;


    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    /* get reserved mc profile */
    soc_sand_rv = soc_ppd_llp_trap_port_info_get(soc_sand_dev_id, core, pp_port, &trap_port_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    trap_bit = SOC_PPC_LLP_TRAP_PORT_ENABLE_PROG_TRAP_0 << type;

    if(trap_port_info.trap_enable_mask & trap_bit) {
        *profile = type;
    }
    else
    {
        *profile = -1; /* pointing to null-profile*/
    }
    
  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l2_prog_trap_data_get(int unit, int port, int type, SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *qual, bcm_dpp_user_defined_traps_t *trap_res)
{
    bcm_dpp_prog_trap_t
        trap_t;
    int old_profile=0;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_l2_prog_trap_port_profile_mapping_get(unit,port, type, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_prog_trap,old_profile,&trap_t);
    BCMDNX_IF_ERR_EXIT(rc);

    sal_memcpy(qual, &trap_t.qual, sizeof(SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER));
    sal_memcpy(trap_res, &trap_t.result, sizeof(bcm_dpp_user_defined_traps_t));

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2_prog_trap_alloc(int unit, int flags, SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER *qual,
                                             bcm_dpp_user_defined_traps_t *trap_res, int *template,int *is_allocated,
                                             bcm_dpp_rx_virtual_traps_t *virtual_traps_p,
                                             int is_virtual_traps_p_set)
{
    bcm_dpp_prog_trap_t trap_t;
    int rc = BCM_E_NONE, virtual_trap_id = 0, new_trap_code;
    bcm_dpp_rx_virtual_traps_t new_virtual_traps_p;

    BCMDNX_INIT_FUNC_DEFS;
    sal_memcpy(&trap_t.qual, qual, sizeof(SOC_PPC_LLP_TRAP_PROG_TRAP_QUALIFIER));
    sal_memcpy(&trap_t.result, trap_res, sizeof(bcm_dpp_user_defined_traps_t));
        
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_prog_trap,flags,&trap_t,is_allocated,template);
    BCMDNX_IF_ERR_EXIT(rc);

    /*PPD trap code*/
    new_trap_code = *template + SOC_PPC_TRAP_CODE_PROG_TRAP_0;

    /*is_virtual_traps_p_set true when the gprot was a virtual trap id, meaning rx_trap_create was used 
        hence allocation needed only when virtual trap was not created yet*/
    if (is_virtual_traps_p_set == FALSE) 
    {
        /* Virtual trap does not exist. Allocate if needed */
        if (*is_allocated) {
            /*is_allocated means first time in use, there for allocate only the first time
               allocation without ID*/
            rc = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_res_trap_virtual,0,1,&virtual_trap_id);
            BCMDNX_IF_ERR_EXIT(rc);

            /*update _rx_virtual_traps array with PPD prog virtual trap*/ 
            if (_bcm_dpp_rx_virtual_trap_get(unit,virtual_trap_id,&new_virtual_traps_p) != BCM_E_NONE)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_get failed, on unit:%d \n"), unit));
            }
            new_virtual_traps_p.soc_ppc_trap_code = new_trap_code;
            if (_bcm_dpp_rx_virtual_trap_set(unit,virtual_trap_id,&new_virtual_traps_p) != BCM_E_NONE)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_set failed, on unit:%d \n"), unit));
            }              
        }    
    }
    else 
    {
        if(virtual_traps_p != NULL)
        {
            /* assumes allocation was done because gport exist
                       there for only update PPD trap code*/
            virtual_traps_p->soc_ppc_trap_code = new_trap_code;

            /* virtual_traps_p should be written back to sw_db by the calling function */
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("If virtual trap was set the pointer cant be NULL ERROR! \n")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2_prog_trap_ppc_index_get(int unit, int type, int *trap_index)
{
    int index;
    bcm_dpp_rx_virtual_traps_t new_virtual_traps_p;

    BCMDNX_INIT_FUNC_DEFS;

    /* Find the appropriate trap_index in which the matching ppd_prog was stored */
    for (index = 0; index < _BCM_NUMBER_OF_VIRTUAL_TRAPS ; index++)
    {
        if (_bcm_dpp_rx_virtual_trap_get(unit, index, &new_virtual_traps_p) != BCM_E_NONE)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_get failed, on unit:%d \n"), unit));
        }

        if (new_virtual_traps_p.soc_ppc_trap_code == type + SOC_PPC_TRAP_CODE_PROG_TRAP_0)
        {
            *trap_index = index;
            break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2_prog_trap_free(int unit, int is_virtual_traps_p_set, int type, int *template,int *is_last)
{
        
    int rc = BCM_E_NONE;
    int trap_index;
    bcm_dpp_rx_virtual_traps_t new_virtual_traps_p;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_prog_trap,type,is_last);
    BCMDNX_IF_ERR_EXIT(rc);

    *template = type;

    if (*is_last) {
        /* Find the appropriate trap_index in which the matching ppd_prog was stored */
        rc = _bcm_dpp_am_template_l2_prog_trap_ppc_index_get(unit, type, &trap_index);

        if (_bcm_dpp_rx_virtual_trap_get(unit, trap_index, &new_virtual_traps_p) != BCM_E_NONE)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_get failed, on unit:%d \n"), unit));
        }

        /*update _rx_virtual_traps array with PPD prog virtual trap*/
        if (is_virtual_traps_p_set == FALSE) 
        {
            /*is_last means not in use anymore, there for free allocation*/
            rc = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_trap_virtual, SW_STATE_RES_ALLOC_WITH_ID, trap_index);
            BCMDNX_IF_ERR_EXIT(rc);
        }

        /*update _rx_virtual_traps array with PPD prog virtual trap*/
        new_virtual_traps_p.soc_ppc_trap_code = 0;
        /* Update the deleted content for the appropriate trap_index */
        if (_bcm_dpp_rx_virtual_trap_set(unit, trap_index, &new_virtual_traps_p) != BCM_E_NONE)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Function _bcm_dpp_rx_virtual_trap_set failed, on unit:%d \n"), unit));
        }

    }  
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2_prog_trap_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_prog_trap));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Prog Trap - End */



/* L2, FID-profile --> even-handle-profile --> handle info (BCM_L2_ADDR_DIST_XXX flags) - Start */

int _bcm_dpp_am_template_l2_event_handle_init(int unit,int template_init_id, int handle_flags)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template = template_init_id;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l2_event_handle, flags, &handle_flags,_DPP_AM_TEMPLATE_L2_EVENT_HANDLE_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);
    

exit:
    BCMDNX_FUNC_RETURN;
}

/* return event handle given fid-profile */
int
_bcm_dpp_am_template_l2_event_handle_fid_profile_mapping_get(int unit, int vsi, int *event_handle_profile)
{
    SOC_PPC_VSI_INFO vsi_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    uint32
        event_handle_tmp;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = soc_ppd_frwrd_mact_event_handle_profile_get(soc_sand_dev_id,vsi_info.mac_learn_profile_id,&event_handle_tmp);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *event_handle_profile = event_handle_tmp;

  BCM_EXIT;
exit:
  BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_l2_event_handle_data_get(int unit, int vsi, int *handle_flags)
{
    int old_handle_profile;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_l2_event_handle_fid_profile_mapping_get(unit,vsi, &old_handle_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_event_handle,old_handle_profile,handle_flags);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l2_event_handle_exchange(int unit, int flags, int vsi, int handle_flags, int *old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_l2_event_handle_fid_profile_mapping_get(unit,vsi, old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    if(!(flags & BCM_DPP_AM_TEMPLATE_FLAG_CHECK)) {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_event_handle,SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,&handle_flags,*old_template,is_last,template,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_event_handle,SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,&handle_flags,*old_template,is_last,template,is_allocated);
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l2_event_handle_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_event_handle));
exit:
    BCMDNX_FUNC_RETURN;
}


/* L2, FID-profile --> event-handle-profile --> handle info (BCM_L2_ADDR_DIST_XXX flags) - End */


/* L2, VSI/FID --> learn-profile --> <fid-aging-profile, event-handle-profile, limit> - Start */

#define LEARN_PROFILE_DATA_LIMIT_OFFSET                (0 )
#define LEARN_PROFILE_DATA_LIMIT_SIZE                  (24)
#define LEARN_PROFILE_DATA_LIMIT_MASK                  (0xFFFFFF)

#define LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_OFFSET ( LEARN_PROFILE_DATA_LIMIT_OFFSET + LEARN_PROFILE_DATA_LIMIT_SIZE)
#define LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_SIZE   (4 )
#define LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_MASK   (0xF )

#define LEARN_PROFILE_DATA_FID_AGING_PROFILE_OFFSET    (LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_OFFSET + LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_SIZE)
#define LEARN_PROFILE_DATA_FID_AGING_PROFILE_SIZE      (4 )
#define LEARN_PROFILE_DATA_FID_AGING_PROFILE_MASK      (0xF )

STATIC 
int _bcm_dpp_am_template_l2_learn_profile_data_encode(int limit, int event_handle_profile, int fid_aging_profile)
{
  int data = 0;
  data |= ((limit                 & LEARN_PROFILE_DATA_LIMIT_MASK               ) << LEARN_PROFILE_DATA_LIMIT_OFFSET                );
  data |= ((event_handle_profile & LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_MASK ) << LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_OFFSET );
  data |= ((fid_aging_profile    & LEARN_PROFILE_DATA_FID_AGING_PROFILE_MASK    ) << LEARN_PROFILE_DATA_FID_AGING_PROFILE_OFFSET    );
  return data;
}

STATIC 
int _bcm_dpp_am_template_l2_learn_profile_data_decode_limit(int data)
{
  int limit =  ((data >> LEARN_PROFILE_DATA_LIMIT_OFFSET) & LEARN_PROFILE_DATA_LIMIT_MASK);
  return (limit == LEARN_PROFILE_DATA_LIMIT_MASK)?-1:limit;
}

STATIC 
int _bcm_dpp_am_template_l2_learn_profile_data_decode_event_handle(int data)
{
  return ((data >> LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_OFFSET) & LEARN_PROFILE_DATA_EVENT_HANDLE_PROFILE_MASK);
}

STATIC 
int _bcm_dpp_am_template_l2_learn_profile_data_decode_fid_aging(int data)
{
    return ((data >> LEARN_PROFILE_DATA_FID_AGING_PROFILE_OFFSET) & LEARN_PROFILE_DATA_FID_AGING_PROFILE_MASK);
}

int _bcm_dpp_am_template_l2_learn_profile_init(int unit,int learn_profile_id, int limit, int event_handle_profile, int fid_aging_profile)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template = learn_profile_id;
    int data = 0;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    
    data = _bcm_dpp_am_template_l2_learn_profile_data_encode(limit, event_handle_profile, fid_aging_profile);
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l2_vsi_learn_profile, flags, &data, _DPP_AM_TEMPLATE_L2_VSI_LEARN_PROFILE_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* return event handle given fid-profile */
int _bcm_dpp_am_template_l2_learn_profile_mapping_get(int unit, int vsi, int *learn_profile)
{
    SOC_PPC_VSI_INFO vsi_info;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    soc_sand_rv = soc_ppd_vsi_info_get(soc_sand_dev_id, vsi, &vsi_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *learn_profile = vsi_info.mac_learn_profile_id;

exit:
  BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l2_learn_limit_active(int unit, int *is_active)
{
    int rc = BCM_E_NONE;
    int data;
    int limit,index;

    BCMDNX_INIT_FUNC_DEFS;

    *is_active = 0;


    /* scan all entities to template init id with given data */
    for (index = 0; index < _DPP_AM_TEMPLATE_L2_FID_RPOFILES_COUNT; index++) {
        rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l2_vsi_learn_profile, index, &data);
        if (rc == BCM_E_NOT_FOUND) {
            continue;
        }
        BCMDNX_IF_ERR_EXIT(rc);
        limit = _bcm_dpp_am_template_l2_learn_profile_data_decode_limit(data);
        if (limit != -1) {
            *is_active = 1;
            break;
        }
    }
   
    
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l2_learn_profile_data_get(int unit, int vsi, int *limit, int *event_handle_profile, int *fid_aging_profile)
{
    int old_handle_profile;
    int rc = BCM_E_NONE;
    int data;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_l2_learn_profile_mapping_get(unit,vsi, &old_handle_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_vsi_learn_profile,old_handle_profile,&data);
    BCMDNX_IF_ERR_EXIT(rc);
    
    *limit = _bcm_dpp_am_template_l2_learn_profile_data_decode_limit(data);
    *event_handle_profile = _bcm_dpp_am_template_l2_learn_profile_data_decode_event_handle(data);
    *fid_aging_profile    = _bcm_dpp_am_template_l2_learn_profile_data_decode_fid_aging(data);

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l2_learn_profile_exchange(int unit, int flags, int vsi, int limit,int event_handle_profile, int fid_aging_profile, int *old_template, int *is_last, int *new_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int data;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_l2_learn_profile_mapping_get(unit,vsi, old_template);
    BCMDNX_IF_ERR_EXIT(rc);
    
    data = _bcm_dpp_am_template_l2_learn_profile_data_encode(limit, event_handle_profile, fid_aging_profile);

    if(!(flags & BCM_DPP_AM_TEMPLATE_FLAG_CHECK)) {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_vsi_learn_profile,SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,&data,*old_template,is_last,new_profile,is_allocated);

    }
    else
    {
        rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_vsi_learn_profile,SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,&data,*old_template,is_last,new_profile,is_allocated);
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l2_learn_profile_clear(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(dpp_am_template_clear(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_vsi_learn_profile));
exit:
    BCMDNX_FUNC_RETURN;
}

/* L2, VSI/FID --> learn-profile --> <even-handle-profile, limit> - End */


/* L2 fid-aging profile - Start */
/* L2, fid-aging-profile --> aging cycle */


    /* In warmboot we already restored entries, no need to init */

    /* Add all entities to template init id with given data */



/* return aging profile given fid-profile */

/* COSQ Ingress rate class - Start */
STATIC int
_bcm_dpp_am_template_queue_rate_cls_mapping_get(int unit, int core, int queue, int *rate_cls)
{
    uint32 soc_sand_rv;    
    SOC_TMC_ITM_QUEUE_INFO queue_info;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(rate_cls);

    SOC_TMC_ITM_QUEUE_INFO_clear(&queue_info);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_queue_info_get,(unit, core, queue,&queue_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *rate_cls = queue_info.rate_cls;

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_queue_rate_cls_init(int unit,int template_init_id,const bcm_dpp_cosq_ingress_rate_class_info_t* data_rate_cls)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    BCMDNX_INIT_FUNC_DEFS;
    /*Relevant only for only for simples queue to rate cls mapping mode*/
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;   
    template = template_init_id;
        
    /* Add all entities to template init id with given data */ 
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_queue_rate_cls,flags,data_rate_cls,_DPP_AM_TEMPLATE_QUEUE_RATE_CLS_MAX_ENTITIES(unit),&is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_am_template_queue_rate_cls_data_get(int unit, int core, int queue,bcm_dpp_cosq_ingress_rate_class_info_t * data_rate_cls)
{
    int rate_cls;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_queue_rate_cls_mapping_get(unit, core, queue, &rate_cls);
    BCMDNX_IF_ERR_EXIT(rc);

    if (rate_cls < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("failed to get rate class")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_queue_rate_cls,rate_cls,data_rate_cls);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Exchange the rate class with a new one with the given data, return the new rate class and if the previous rate class is now unused. */
int _bcm_dpp_am_template_queue_rate_cls_exchange(int unit, int core, int queue, bcm_dpp_cosq_ingress_rate_class_info_t * data_rate_cls, int *old_rate_cls, int *is_last, int *new_rate_cls,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_rate_cls;
    int tmp_is_last, core_index;
    BCMDNX_INIT_FUNC_DEFS;
    /* Get current rate class for queue from HW */
    rc = _bcm_dpp_am_template_queue_rate_cls_mapping_get(unit, core, queue, &tmp_old_rate_cls);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_queue_rate_cls,0,data_rate_cls,tmp_old_rate_cls,&tmp_is_last,new_rate_cls,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (core == BCM_CORE_ALL) {
        /*If we map all cores then exchange with ID for the rest of the core*/
        for (core_index = 0; core_index < (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores - 1); core_index++) {
            rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_queue_rate_cls,SHR_TEMPLATE_MANAGE_SET_WITH_ID,data_rate_cls,tmp_old_rate_cls,&tmp_is_last,new_rate_cls,is_allocated);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    if (old_rate_cls != NULL) {
        *old_rate_cls = tmp_old_rate_cls;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


/* Test if an exchange the rate class with a new one with the given data is possible, return the new rate class and if the previous rate class is now unused. */
int _bcm_dpp_am_template_queue_rate_cls_exchange_test(int unit, int core, int queue, bcm_dpp_cosq_ingress_rate_class_info_t * data_rate_cls, int *old_rate_cls, int *is_last, int *new_rate_cls,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_rate_cls;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    /* Get current rate class for queue from HW */
    rc = _bcm_dpp_am_template_queue_rate_cls_mapping_get(unit, core, queue, &tmp_old_rate_cls);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_queue_rate_cls,0,data_rate_cls,tmp_old_rate_cls,&tmp_is_last,new_rate_cls,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_rate_cls != NULL) {
        *old_rate_cls = tmp_old_rate_cls;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_queue_rate_cls_ref_get(int unit, int template, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_queue_rate_cls, template, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_queue_rate_cls_allocate_group(int unit, uint32 flags, bcm_dpp_cosq_ingress_rate_class_info_t *data_rate_cls, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;
    int is_last;

    BCMDNX_INIT_FUNC_DEFS;
    if (ref_count > 0) {
        rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_queue_rate_cls, flags, (void *)data_rate_cls, ref_count, is_allocated, template);
        BCMDNX_IF_ERR_EXIT(rc);
    } else {
        rc = dpp_am_template_free_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_queue_rate_cls, *template, -ref_count, &is_last);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_queue_rate_cls_tdata_get(int unit, int template, bcm_dpp_cosq_ingress_rate_class_info_t *data_rate_cls)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_queue_rate_cls, template, (void *)data_rate_cls);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* COSQ Ingress rate class - End */

/* COSQ VSQ rate class - Start */
int
bcm_dpp_am_vsq_src_port_alloc(
    int unit,
    int core_id,
    uint32 flags,
    int src_pp_port,
    int *vsq_index)
{
    int rv = BCM_E_NONE;
    int src_port_vsq_index;
    int pg_base;
    uint8 enable;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(vsq_index);
    if (src_pp_port >= SOC_MAX_NUM_PORTS) {
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_src_vsqs_mapping_get ,(unit, core_id, src_pp_port, &src_port_vsq_index, &pg_base, &enable)));
    BCMDNX_IF_ERR_EXIT(rv);
    if (enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Source PP port %d, is already mapped."), src_pp_port));
    }
    rv = dpp_am_res_alloc(unit, core_id, dpp_am_res_vsq_src_port, flags, 1, vsq_index);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_vsq_src_port_free(
    int unit,
    int core_id,
    int vsq_index)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    if (vsq_index >= SOC_TMC_ITM_VSQ_GROUPE_SZE(unit)) {
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    rv = dpp_am_res_free(unit, core_id, dpp_am_res_vsq_src_port, 1, vsq_index);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_am_pg_init(int unit)
{
    int rv = BCM_E_NONE, core_index;
    int pg_base = 0;
    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core_index) {
        rv = bcm_dpp_am_pg_alloc(unit, core_index, SHR_TEMPLATE_MANAGE_SET_WITH_ID, 1, &pg_base);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.pg_numq.set(unit, core_index, pg_base, 1);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_am_pg_alloc(
    int unit,
    int core_id,
    uint32 flags,
    int numq,
    int *pg_base)
{
    int rv = BCM_E_NONE, core_index;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(pg_base);
    
    rv = dpp_am_res_alloc(unit, core_id, dpp_am_res_vsq_pg, flags, numq, pg_base);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_DPP_CORES_ITER(core_id, core_index) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.pg_numq.set(unit, core_index, *pg_base, numq);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_am_pg_check(
    int unit,
    int core_id,
    int pg_base
    )
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, core_id, dpp_am_res_vsq_pg, 1, pg_base));
exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_am_pg_get(
    int unit,
    int core_id,
    int pg_base,
    int *numq
    )
{
    int rv = BCM_E_NONE, core_index;
    uint8 numq_get;
    BCMDNX_INIT_FUNC_DEFS;
    core_index = (core_id == BCM_CORE_ALL) ? 0 : core_id;
    rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.pg_numq.get(unit, core_index, pg_base, &numq_get);
    SOCDNX_IF_ERR_EXIT(rv);
    *numq = numq_get;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_am_pg_free(
    int unit,
    int core_id,
    int pg_base)
{
    int rv = BCM_E_NONE, core_index;
    int numq;
    BCMDNX_INIT_FUNC_DEFS;

    rv = bcm_dpp_am_pg_get(unit, core_id, pg_base, &numq);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = dpp_am_res_free(unit, core_id, dpp_am_res_vsq_pg, numq, pg_base);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_DPP_CORES_ITER(core_id, core_index) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.tm_info.pg_numq.set(unit, core_index, pg_base, 0);
        SOCDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vsq_pg_tc_mapping_profile_init(int unit, int core_index, uint32 pg_tc_bitmap){
    int core_id = 0;
    bcm_error_t rc = BCM_E_NONE;
    int is_allocated;
    uint32 flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    int template = 0; /*Default profile for all PGs*/
    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    BCM_DPP_CORES_ITER(core_index, core_id) {
        rc = dpp_am_template_allocate_group(
           unit,
           core_id,
           dpp_am_template_vsq_pg_tc_mapping,
           flags,
           &pg_tc_bitmap,
           _DPP_AM_TEMPLATE_VSQ_PG_TC_MAPPING_MAX_ENTITIES(unit),
           &is_allocated,
           &template);
        BCMDNX_IF_ERR_EXIT(rc);
    }
exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_am_template_vsq_pg_tc_mapping_profile_data_get(int unit, int core_id, int pg_tc_profile, uint32 *pg_tc_bitmap){
    bcm_error_t rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(pg_tc_bitmap);

    rc = dpp_am_template_data_get(unit, core_id, dpp_am_template_vsq_pg_tc_mapping, pg_tc_profile, pg_tc_bitmap);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
    
int _bcm_dpp_am_template_vsq_pg_tc_mapping_exchange(int unit, int core_id, uint32 pg_tc_bitmap, int old_pg_tc_profile, int *is_last, int *new_pg_tc_profile,int *is_allocated) {
    int rc = BCM_E_NONE;
    uint32 flags = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_last);
    BCMDNX_NULL_CHECK(new_pg_tc_profile);
    BCMDNX_NULL_CHECK(is_allocated);

    rc = dpp_am_template_exchange(unit, core_id, dpp_am_template_vsq_pg_tc_mapping, flags, &pg_tc_bitmap, old_pg_tc_profile, is_last, new_pg_tc_profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_dpp_am_template_vsq_rate_cls_mapping_get(int unit,int core_id, SOC_TMC_ITM_VSQ_NDX vsq_id, SOC_TMC_ITM_VSQ_GROUP *vsq_group, int *rate_cls)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_TMC_ITM_VSQ_NDX vsq_in_group_ndx = 0;
    uint32 tmp = 0;
    uint8 is_ocb_only = FALSE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(rate_cls);
    BCMDNX_NULL_CHECK(vsq_group);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL_WITHOUT_DEV_ID(unit,mbcm_dpp_itm_vsq_index_global2group,(unit, vsq_id, vsq_group, &vsq_in_group_ndx, &is_ocb_only)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_vsq_qt_rt_cls_get,(unit, core_id, is_ocb_only, *vsq_group, vsq_in_group_ndx, &tmp)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    *rate_cls = tmp;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vsq_rate_cls_init(int unit, int template_init_id, const bcm_dpp_cosq_vsq_rate_class_info_t* data_rate_cls)
{
    SOC_TMC_ITM_VSQ_GROUP vsq_group;
    const int vsq_template[SOC_TMC_NOF_VSQ_GROUPS] = {dpp_am_template_vsq_rate_cls_ct,
                                                      dpp_am_template_vsq_rate_cls_cttc,
                                                      dpp_am_template_vsq_rate_cls_ctcc,
                                                      dpp_am_template_vsq_rate_cls_pp,
                                                      dpp_am_template_vsq_rate_cls_src_port,
                                                      dpp_am_template_vsq_rate_cls_pg};
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    
    template = template_init_id;
    
    for (vsq_group = 0; vsq_group < SOC_TMC_NOF_VSQ_GROUPS; vsq_group++) {
        /* Add all entities to template init id with given data */
        result = dpp_am_template_allocate_group(
                    unit, 
                    BCM_DPP_AM_DEFAULT_POOL_IDX, 
                    vsq_template[vsq_group], 
                    flags, 
                    _bcm_dpp_am_template_vsq_rate_cls_data(vsq_group, data_rate_cls), 
                    _DPP_AM_TEMPLATE_VSQ_RATE_CLS_MAX_ENTITIES(unit, vsq_group), 
                    &is_allocated, 
                    &template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vsq_rate_cls_data_get(int unit, int core_id, int vsq_id, bcm_dpp_cosq_vsq_rate_class_info_t * data_rate_cls)
{
    int rate_cls;
    int rc = BCM_E_NONE;
    SOC_TMC_ITM_VSQ_GROUP vsq_group;
    const int vsq_template[SOC_TMC_NOF_VSQ_GROUPS] = {dpp_am_template_vsq_rate_cls_ct,
                                                      dpp_am_template_vsq_rate_cls_cttc,
                                                      dpp_am_template_vsq_rate_cls_ctcc,
                                                      dpp_am_template_vsq_rate_cls_pp,
                                                      dpp_am_template_vsq_rate_cls_src_port,
                                                      dpp_am_template_vsq_rate_cls_pg};
    BCMDNX_INIT_FUNC_DEFS;

    if ((core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core_id != BCM_CORE_ALL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit(%d) core out of range %d"), unit, core_id));
    }

    rc = _bcm_dpp_am_template_vsq_rate_cls_mapping_get(unit, core_id, vsq_id, &vsq_group, &rate_cls);
    BCMDNX_IF_ERR_EXIT(rc);

    if (rate_cls < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get rate class")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, vsq_template[vsq_group], rate_cls, _bcm_dpp_am_template_vsq_rate_cls_data(vsq_group, data_rate_cls));
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vsq_rate_cls_exchange(int unit, int core_id, int vsq_id, bcm_dpp_cosq_vsq_rate_class_info_t * data_rate_cls, int *old_rate_cls, int *is_last, int *new_rate_cls, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_rate_cls;
    int tmp_is_last;
    const int vsq_template[SOC_TMC_NOF_VSQ_GROUPS] = {dpp_am_template_vsq_rate_cls_ct,
                                                      dpp_am_template_vsq_rate_cls_cttc,
                                                      dpp_am_template_vsq_rate_cls_ctcc,
                                                      dpp_am_template_vsq_rate_cls_pp,
                                                      dpp_am_template_vsq_rate_cls_src_port,
                                                      dpp_am_template_vsq_rate_cls_pg};
    SOC_TMC_ITM_VSQ_GROUP vsq_group;
    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_vsq_rate_cls_mapping_get(unit, core_id, vsq_id, &vsq_group, &tmp_old_rate_cls);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, vsq_template[vsq_group], 0, _bcm_dpp_am_template_vsq_rate_cls_data(vsq_group, data_rate_cls), tmp_old_rate_cls, &tmp_is_last, new_rate_cls, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_rate_cls != NULL) {
        *old_rate_cls = tmp_old_rate_cls;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_vsq_rate_cls_ref_get(int unit, int template, SOC_TMC_ITM_VSQ_GROUP vsq_group, uint32 *ref_count)
{
    int rc = BCM_E_NONE;
    const int vsq_template[SOC_TMC_NOF_VSQ_GROUPS] = {dpp_am_template_vsq_rate_cls_ct,
                                                      dpp_am_template_vsq_rate_cls_cttc,
                                                      dpp_am_template_vsq_rate_cls_ctcc,
                                                      dpp_am_template_vsq_rate_cls_pp,
                                                      dpp_am_template_vsq_rate_cls_src_port,
                                                      dpp_am_template_vsq_rate_cls_pg};

    BCMDNX_INIT_FUNC_DEFS;
    if(vsq_group < 0 || vsq_group >= SOC_TMC_NOF_VSQ_GROUPS) {
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, vsq_template[vsq_group], template, ref_count);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_vsq_rate_cls_allocate_group(int unit, uint32 flags, SOC_TMC_ITM_VSQ_GROUP vsq_group, bcm_dpp_cosq_vsq_rate_class_info_t *data_rate_cls, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;
    const int vsq_template[SOC_TMC_NOF_VSQ_GROUPS] = {dpp_am_template_vsq_rate_cls_ct,
                                                      dpp_am_template_vsq_rate_cls_cttc,
                                                      dpp_am_template_vsq_rate_cls_ctcc,
                                                      dpp_am_template_vsq_rate_cls_pp,
                                                      dpp_am_template_vsq_rate_cls_src_port,
                                                      dpp_am_template_vsq_rate_cls_pg};
    BCMDNX_INIT_FUNC_DEFS;
    if(vsq_group < 0 || vsq_group >= SOC_TMC_NOF_VSQ_GROUPS) {
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, vsq_template[vsq_group], flags, _bcm_dpp_am_template_vsq_rate_cls_data(vsq_group, data_rate_cls), ref_count, is_allocated, template);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_vsq_rate_cls_tdata_get(int unit, int template, SOC_TMC_ITM_VSQ_GROUP vsq_group, bcm_dpp_cosq_vsq_rate_class_info_t *data_rate_cls)
{
    int rc = BCM_E_NONE;
    const int vsq_template[SOC_TMC_NOF_VSQ_GROUPS] = {dpp_am_template_vsq_rate_cls_ct,
                                              dpp_am_template_vsq_rate_cls_cttc,
                                              dpp_am_template_vsq_rate_cls_ctcc,
                                              dpp_am_template_vsq_rate_cls_pp,
                                              dpp_am_template_vsq_rate_cls_src_port,
                                              dpp_am_template_vsq_rate_cls_pg};
    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, vsq_template[vsq_group], template, _bcm_dpp_am_template_vsq_rate_cls_data(vsq_group, data_rate_cls));
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


/* COSQ vsq rate class - End */


/* System RED drop probability value class - Start */
STATIC int
_bcm_dpp_am_template_system_red_dp_pr_mapping_get(int unit, int dp_pr_ndx, int *dp_pr_template_ndx)
{
    SOC_TMC_ITM_SYS_RED_DROP_PROB info;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(dp_pr_template_ndx);

    SOC_TMC_ITM_SYS_RED_DROP_PROB_clear(&info);

    *dp_pr_template_ndx = dp_pr_ndx;

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_system_red_dp_pr_init(int unit,int template_init_id,const bcm_dpp_cosq_ingress_system_red_dp_pr_info_t* data_sys_red_dp_pr)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif    

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;   
    template = template_init_id;
    
    /* Add all entities to template init id with given data */ 
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_system_red_dp_pr, flags,data_sys_red_dp_pr, 
                                         _DPP_AM_TEMPLATE_SYS_RED_DP_PR_MAX_ENTITIES(unit), &is_allocated, &template);
    BCMDNX_IF_ERR_EXIT(result);
 
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_system_red_dp_pr_data_get(int unit, int dp_pr_ndx, bcm_dpp_cosq_ingress_system_red_dp_pr_info_t * data_sys_red_dp_pr)
{
    int sys_red_dp_pr_template_ndx;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_system_red_dp_pr_mapping_get(unit, dp_pr_ndx, &sys_red_dp_pr_template_ndx);
    BCMDNX_IF_ERR_EXIT(rc);

    if (dp_pr_ndx < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("failed to get Drop pr")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_system_red_dp_pr, sys_red_dp_pr_template_ndx, data_sys_red_dp_pr);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Exchange the DP PR with a new one with the given data, return the new DP PR  and if the previous DP PR is now unused. */
int _bcm_dpp_am_template_system_red_dp_pr_exchange(int unit, int dp_pr_ndx, bcm_dpp_cosq_ingress_system_red_dp_pr_info_t *data_sys_red_dp_pr, 
                                                   int *old_sys_red_dp_pr_ndx, int *is_last, int *new_sys_red_dp_pr_ndx, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_sys_red_dp_pr_ndx;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    
    rc = _bcm_dpp_am_template_system_red_dp_pr_mapping_get(unit ,dp_pr_ndx, &tmp_old_sys_red_dp_pr_ndx);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_system_red_dp_pr, 0, data_sys_red_dp_pr, tmp_old_sys_red_dp_pr_ndx,
                               &tmp_is_last, new_sys_red_dp_pr_ndx, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_sys_red_dp_pr_ndx != NULL) {
        *old_sys_red_dp_pr_ndx = tmp_old_sys_red_dp_pr_ndx;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_system_red_dp_pr_ref_get(int unit, int template, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_system_red_dp_pr, template, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_system_red_dp_pr_allocate_group(int unit, uint32 flags, bcm_dpp_cosq_ingress_system_red_dp_pr_info_t *data_sys_red_dp_pr, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_system_red_dp_pr, flags, (void *)data_sys_red_dp_pr, ref_count, is_allocated, template);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_system_red_dp_pr_tdata_get(int unit, int template, bcm_dpp_cosq_ingress_system_red_dp_pr_info_t *data_sys_red_dp_pr)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_system_red_dp_pr, template, (void *)data_sys_red_dp_pr);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* System RED drop probability value class - END */

/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/* COSQ Ingress Queue discount class - Start */
int
_bcm_dpp_am_template_queue_discount_cls_mapping_get(int unit, int core, int queue, int *discount_cls)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_TMC_ITM_QUEUE_INFO queue_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(discount_cls);

    SOC_TMC_ITM_QUEUE_INFO_clear(&queue_info);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_queue_info_get,(unit,core ,queue,&queue_info)));    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *discount_cls = queue_info.credit_cls;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_queue_discount_cls_init(int unit, int template_init_id,const SOC_TMC_ITM_CR_DISCOUNT_INFO* data_discount_cls)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    int core;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
        result = dpp_am_template_allocate_group(unit, core, dpp_am_template_queue_discount_cls, flags, data_discount_cls, 
                                                _DPP_AM_TEMPLATE_QUEUE_DISCNT_CLS_MAX_ENTITIES(unit), &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_queue_discount_cls_data_get(int unit, int core, int queue,SOC_TMC_ITM_CR_DISCOUNT_INFO * data_discount_cls)
{
    int discount_cls;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_queue_discount_cls_mapping_get(unit, core, queue,&discount_cls);
    BCMDNX_IF_ERR_EXIT(rc);

    if (discount_cls < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get discount class")));
    }

    rc = dpp_am_template_data_get(unit, core, dpp_am_template_queue_discount_cls,discount_cls,data_discount_cls);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_queue_discount_cls_exchange(int unit, int core, int queue, SOC_TMC_ITM_CR_DISCOUNT_INFO * data_discount_cls, int *old_discount_cls, int *is_last, int *new_discount_cls,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_discount_cls;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_queue_discount_cls_mapping_get(unit, core, queue,&tmp_old_discount_cls);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, core, dpp_am_template_queue_discount_cls,0,data_discount_cls,tmp_old_discount_cls,&tmp_is_last,new_discount_cls,is_allocated);
    if (rc != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("failed to get a free credit discount template")));
    }

    if (old_discount_cls != NULL) {
        *old_discount_cls = tmp_old_discount_cls;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_queue_discount_cls_ref_get(int unit, int core, int template, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, core, dpp_am_template_queue_discount_cls, template, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_queue_discount_cls_allocate_group(int unit, int core, uint32 flags, SOC_TMC_ITM_CR_DISCOUNT_INFO *data_discount_cls, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate_group(unit, core, dpp_am_template_queue_discount_cls, flags, (void *)data_discount_cls, ref_count, is_allocated, template);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_queue_discount_cls_tdata_get(int unit, int core, int template, SOC_TMC_ITM_CR_DISCOUNT_INFO *data_discount_cls)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, core, dpp_am_template_queue_discount_cls, template, (void *)data_discount_cls);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* COSQ Ingress Queue discount class - End */
/* ----------------------------------------------------------------------------------------------*/

/* COSQ Ingress PP Port discount class - Start */
int
_bcm_dpp_am_template_pp_port_discount_cls_mapping_get(int unit, int core, int port, int *discount_cls)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(discount_cls);

    BCMDNX_IF_ERR_EXIT (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_in_pp_port_scheduler_compensation_profile_get,(unit, core ,port ,discount_cls)));    

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_pp_port_discount_cls_init(int unit, int template_init_id, const SOC_TMC_ITM_CR_DISCOUNT_INFO* data_discount_cls)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    int core;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
        result = dpp_am_template_allocate_group(unit, core, dpp_am_template_pp_port_discount_cls, flags, data_discount_cls, 
                                                _DPP_AM_TEMPLATE_PP_PORT_DISCNT_CLS_MAX_ENTITIES(unit), &is_allocated, &template);
        BCMDNX_IF_ERR_EXIT(result);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_pp_port_discount_cls_data_get(int unit, int core, int port, SOC_TMC_ITM_CR_DISCOUNT_INFO * data_discount_cls)
{
    int discount_cls;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_pp_port_discount_cls_mapping_get(unit, core, port, &discount_cls);
    BCMDNX_IF_ERR_EXIT(rc);

    if (discount_cls < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get discount class")));
    }

    rc = dpp_am_template_data_get(unit, core, dpp_am_template_pp_port_discount_cls, discount_cls, data_discount_cls);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_pp_port_discount_cls_exchange(int unit, int core, int port, SOC_TMC_ITM_CR_DISCOUNT_INFO * data_discount_cls, int *old_discount_cls, 
                                                       int *is_last, int *new_discount_cls, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_discount_cls;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_pp_port_discount_cls_mapping_get(unit, core, port, &tmp_old_discount_cls);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, core, dpp_am_template_pp_port_discount_cls, 0, data_discount_cls, tmp_old_discount_cls, 
                                  &tmp_is_last, new_discount_cls,is_allocated);
    if (rc != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("failed to get a free credit discount template")));
    }

    if (old_discount_cls != NULL) {
        *old_discount_cls = tmp_old_discount_cls;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_pp_port_discount_cls_ref_get(int unit, int core, int template, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, core, dpp_am_template_pp_port_discount_cls, template, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_pp_port_discount_cls_allocate_group(int unit, int core, uint32 flags, SOC_TMC_ITM_CR_DISCOUNT_INFO *data_discount_cls, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate_group(unit, core, dpp_am_template_pp_port_discount_cls, flags, (void *)data_discount_cls, ref_count, is_allocated, template);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_pp_port_discount_cls_tdata_get(int unit, int core, int template, SOC_TMC_ITM_CR_DISCOUNT_INFO *data_discount_cls)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, core, dpp_am_template_pp_port_discount_cls, template, (void *)data_discount_cls);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* COSQ Ingress PP Port discount class - End */


/* --------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/* COSQ Egress discount class - Start */

STATIC int _bcm_dpp_hedear_type_map(int unit ,SOC_TMC_PORT_HEADER_TYPE header_type, SOC_TMC_PORT_HEADER_TYPE *header_type_val){
    
    bcm_error_t rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    switch (header_type) {
        case SOC_TMC_PORT_HEADER_TYPE_RAW:
        case SOC_TMC_PORT_HEADER_TYPE_DSA_RAW:
        case SOC_TMC_PORT_HEADER_TYPE_RAW_DSA:
        case SOC_TMC_PORT_HEADER_TYPE_NONE:
        case SOC_TMC_PORT_HEADER_TYPE_STACKING:
        case SOC_TMC_PORT_HEADER_TYPE_TDM:
        case SOC_TMC_PORT_HEADER_TYPE_TDM_RAW:        
        case SOC_TMC_PORT_HEADER_TYPE_XGS_HQoS: 
        case SOC_TMC_PORT_HEADER_TYPE_XGS_MAC_EXT:
        case SOC_TMC_PORT_HEADER_TYPE_MIRROR_RAW:
            *header_type_val =  SOC_TMC_PORT_HEADER_TYPE_RAW;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_ETH:
        case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:
        case SOC_TMC_PORT_HEADER_TYPE_UDH_ETH:
            *header_type_val =  SOC_TMC_PORT_HEADER_TYPE_ETH;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TM:
            *header_type_val =  SOC_TMC_PORT_HEADER_TYPE_TM;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_INJECTED:
        case SOC_TMC_PORT_HEADER_TYPE_INJECTED_PP:
        case SOC_TMC_PORT_HEADER_TYPE_INJECTED_2: 
        case SOC_TMC_PORT_HEADER_TYPE_INJECTED_2_PP: 
        case SOC_TMC_PORT_HEADER_TYPE_PROG:            
        case SOC_TMC_PORT_HEADER_TYPE_CPU:
            *header_type_val = SOC_TMC_PORT_HEADER_TYPE_CPU;
            break;  
		 case SOC_TMC_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU:
            *header_type_val =  SOC_TMC_PORT_HEADER_TYPE_L2_ENCAP_EXTERNAL_CPU;
            break;	      
        default:
            rc = BCM_E_PARAM;
            break;
   }
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_am_template_egress_port_discount_cls_mapping_get(int unit, int core, uint32 tm_port, int *discount_profile,SOC_TMC_PORT_HEADER_TYPE *header_type_outgoing)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE discount_type = SOC_TMC_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES;
    SOC_TMC_PORT_HEADER_TYPE header_type_incoming;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(discount_profile);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_egr_hdr_credit_discount_select_get,(unit, core, tm_port,&discount_type)));    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);   
    *discount_profile = discount_type;

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_header_type_get,(unit,core,tm_port,&header_type_incoming,header_type_outgoing)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_egress_port_discount_cls_free(int unit, int core, uint32 tm_port, int* is_last)
{
    int rc = BCM_E_NONE;
    int template;
    SOC_TMC_PORT_HEADER_TYPE header_type, header_type_map;
    _dpp_am_template_t template_type = -1;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_egress_port_discount_cls_mapping_get(unit, core, tm_port,&template, &header_type);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_hedear_type_map(unit,header_type, &header_type_map);
    BCMDNX_IF_ERR_EXIT(rc);

    switch (header_type_map) {
        case SOC_TMC_PORT_HEADER_TYPE_RAW:
            template_type = dpp_am_template_egress_port_discount_cls_type_raw;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_CPU:
            template_type = dpp_am_template_egress_port_discount_cls_type_cpu;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_ETH:
        case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:
            template_type = dpp_am_template_egress_port_discount_cls_type_eth;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TM:
            template_type = dpp_am_template_egress_port_discount_cls_type_tm;
            break;
        default: 
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    rc = dpp_am_template_free(unit, core, template_type,template,is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_egress_port_discount_cls_init(int unit, int core, uint32 tm_port,const SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO* data_discount_cls)
{
    int is_allocated, result = BCM_E_NONE;
    bcm_error_t rc;
    uint32 flags = 0;
    int template;
    SOC_TMC_PORT_HEADER_TYPE header_type, header_type_map;
    _dpp_am_template_t template_type = -1;

    BCMDNX_INIT_FUNC_DEFS;
    /*flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;*/
    rc = _bcm_dpp_am_template_egress_port_discount_cls_mapping_get(unit, core, tm_port,&template, &header_type);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = _bcm_dpp_hedear_type_map(unit,header_type, &header_type_map);
    BCMDNX_IF_ERR_EXIT(rc);

    switch (header_type_map) {
        case SOC_TMC_PORT_HEADER_TYPE_RAW:
            template_type = dpp_am_template_egress_port_discount_cls_type_raw;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_CPU:
            template_type = dpp_am_template_egress_port_discount_cls_type_cpu;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_ETH:
        case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:  
            template_type = dpp_am_template_egress_port_discount_cls_type_eth;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TM:
            template_type = dpp_am_template_egress_port_discount_cls_type_tm;
            break;
        default: 
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    result = dpp_am_template_allocate(unit, core, template_type, flags, data_discount_cls, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);

    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_egr_hdr_credit_discount_select_set,(unit, core, tm_port,template)));

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_egress_port_discount_cls_data_get(int unit, int core, uint32 tm_port,SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO * data_discount_cls)
{
    int discount_profile;
    SOC_TMC_PORT_HEADER_TYPE header_type,header_type_map;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_egress_port_discount_cls_mapping_get(unit, core, tm_port, &discount_profile, &header_type);
    
    BCMDNX_IF_ERR_EXIT(rc);

    rc = _bcm_dpp_hedear_type_map(unit,header_type, &header_type_map);
    BCMDNX_IF_ERR_EXIT(rc);

    if (discount_profile < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get discount profile")));
    }
    switch (header_type_map) {
        case SOC_TMC_PORT_HEADER_TYPE_RAW:
            rc = dpp_am_template_data_get(unit, core,dpp_am_template_egress_port_discount_cls_type_raw,discount_profile,data_discount_cls);
            break;    
        case SOC_TMC_PORT_HEADER_TYPE_CPU:
            rc = dpp_am_template_data_get(unit, core,dpp_am_template_egress_port_discount_cls_type_cpu,discount_profile,data_discount_cls);
            break;   
        case SOC_TMC_PORT_HEADER_TYPE_ETH:
        case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:
            rc = dpp_am_template_data_get(unit, core,dpp_am_template_egress_port_discount_cls_type_eth,discount_profile,data_discount_cls);
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TM:
            rc = dpp_am_template_data_get(unit, core,dpp_am_template_egress_port_discount_cls_type_tm,discount_profile,data_discount_cls);
            break;
        default: 
            rc = BCM_E_PARAM;

    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_egress_port_discount_cls_exchange(int unit, int core, uint32 tm_port, SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO * data_discount_cls, int *old_discount_profile, int *is_last, int *new_discount_cls,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_discount_profile;
    SOC_TMC_PORT_HEADER_TYPE header_type,header_type_map;
    int tmp_is_last;
    _dpp_am_template_t template_type = -1;

    BCMDNX_INIT_FUNC_DEFS;
    
    rc = _bcm_dpp_am_template_egress_port_discount_cls_mapping_get(unit, core, tm_port, &tmp_old_discount_profile,&header_type);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = _bcm_dpp_hedear_type_map(unit,header_type, &header_type_map);
    BCMDNX_IF_ERR_EXIT(rc);


    switch (header_type_map) {
        case SOC_TMC_PORT_HEADER_TYPE_RAW:
            template_type = dpp_am_template_egress_port_discount_cls_type_raw;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_CPU:
            template_type = dpp_am_template_egress_port_discount_cls_type_cpu;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_ETH:
        case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:
            template_type = dpp_am_template_egress_port_discount_cls_type_eth;
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TM:
            template_type = dpp_am_template_egress_port_discount_cls_type_tm;
            break;
        default: 
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    rc = dpp_am_template_exchange(unit,core,template_type,0,data_discount_cls,tmp_old_discount_profile,&tmp_is_last,new_discount_cls,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);


    if (old_discount_profile != NULL) {
        *old_discount_profile = tmp_old_discount_profile;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_egress_port_discount_cls_per_header_type_ref_get(int unit, int core, int template, SOC_TMC_PORT_HEADER_TYPE header_type, uint32 *ref_count)
{
    SOC_TMC_PORT_HEADER_TYPE header_type_map;
    int rc = BCM_E_NONE;
  
    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_hedear_type_map(unit,header_type, &header_type_map);
    BCMDNX_IF_ERR_EXIT(rc);
    switch (header_type_map) {
        case SOC_TMC_PORT_HEADER_TYPE_RAW: 
            rc = dpp_am_template_ref_count_get(unit, core, dpp_am_template_egress_port_discount_cls_type_raw, template, ref_count);
            break;
        case SOC_TMC_PORT_HEADER_TYPE_CPU:
            rc = dpp_am_template_ref_count_get(unit, core, dpp_am_template_egress_port_discount_cls_type_cpu, template, ref_count);
            break;
        case SOC_TMC_PORT_HEADER_TYPE_ETH:
        case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:
            rc = dpp_am_template_ref_count_get(unit, core, dpp_am_template_egress_port_discount_cls_type_eth, template, ref_count);
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TM:
            rc = dpp_am_template_ref_count_get(unit, core, dpp_am_template_egress_port_discount_cls_type_tm, template, ref_count);
            break;
        default: 
            rc = BCM_E_PARAM;
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_egress_port_discount_cls_per_header_type_tdata_get(int unit, int core, int template, SOC_TMC_PORT_HEADER_TYPE header_type, SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO *data_discount_cls)
{
    SOC_TMC_PORT_HEADER_TYPE header_type_map;
    int rc = BCM_E_NONE;
    

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_hedear_type_map(unit,header_type, &header_type_map);
    BCMDNX_IF_ERR_EXIT(rc);
    switch (header_type_map) {
        case SOC_TMC_PORT_HEADER_TYPE_RAW:
            rc = dpp_am_template_data_get(unit, core, dpp_am_template_egress_port_discount_cls_type_raw, template, (void *)data_discount_cls);
            break;
        case SOC_TMC_PORT_HEADER_TYPE_CPU:
            rc = dpp_am_template_data_get(unit, core, dpp_am_template_egress_port_discount_cls_type_cpu, template, (void *)data_discount_cls);
            break;
        case SOC_TMC_PORT_HEADER_TYPE_ETH:
        case SOC_TMC_PORT_HEADER_TYPE_MPLS_RAW:
            rc = dpp_am_template_data_get(unit, core, dpp_am_template_egress_port_discount_cls_type_eth, template, (void *)data_discount_cls);
            break;
        case SOC_TMC_PORT_HEADER_TYPE_TM:
            rc = dpp_am_template_data_get(unit, core, dpp_am_template_egress_port_discount_cls_type_tm, template, (void *)data_discount_cls);
            break;
        default: 
            rc = BCM_E_PARAM;
    }
    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}




/* COSQ Egress discount class - End */

/* COSQ Port hr flow control profile - Start */

/* port ->  flow control profile */
typedef struct _dpp_cosq_port_hr_fc_map_s {
    int profile[_DPP_AM_TEMPLATE_COSQ_PORT_HR_FC_MAX_ENTRIES];
} _dpp_cosq_port_hr_fc_map_t;

static _dpp_cosq_port_hr_fc_map_t _bcm_dpp_cosq_port_hr_fc_map[BCM_MAX_NUM_UNITS][SOC_DPP_DEFS_MAX(NOF_CORES)];

STATIC int
_bcm_dpp_am_template_cosq_port_hr_fc_profile_mapping_get(int unit, int core, int port, int *profile)
{
    BCMDNX_INIT_FUNC_DEFS;

    (*profile) = _bcm_dpp_cosq_port_hr_fc_map[unit][core].profile[port];

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set mapping - since get mapping is implemented as s/w cache */ 
int
_bcm_dpp_am_template_cosq_port_hr_fc_profile_mapping_set(int unit, int core, int port, int profile)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    _bcm_dpp_cosq_port_hr_fc_map[unit][core].profile[port] = profile;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Template_init_id will always be present in the system */
int
_bcm_dpp_am_template_cosq_port_hr_fc_init(int unit, int core, int template_init_id, int *fc_data_init)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;


    BCMDNX_INIT_FUNC_DEFS;
    /* template_init_id will always be present in the system */

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;

    result =
        dpp_am_template_allocate_group(
            unit, core, dpp_am_template_cosq_port_hr_flow_control,
            flags, fc_data_init,_BCM_PETRA_NOF_TM_PORTS(unit), &is_allocated, &template);
    BCMDNX_IF_ERR_EXIT(result);

    /* Add all entities to template init id with given data */
    for (index = 0; index <= _BCM_PETRA_NOF_TM_PORTS(unit); index++) {
        result = _bcm_dpp_am_template_cosq_port_hr_fc_profile_mapping_set(unit, core, index, template_init_id);
        BCMDNX_IF_ERR_EXIT(result);
    }
   
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Note: this procedure is currently not in use.
 */
int
_bcm_dpp_am_template_cosq_port_hr_fc_data_get(int unit, int core, int port, int *data)
{
    int old_profile;
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_am_template_cosq_port_hr_fc_profile_mapping_get(unit, core, port, &old_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    rc = dpp_am_template_data_get(unit, core, dpp_am_template_cosq_port_hr_flow_control, old_profile, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_port_hr_fc_exchange(int unit, int core, int port, int *data, int *old_template, int *is_last, int *template, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int temp;
    int temp_is_last;


    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_cosq_port_hr_fc_profile_mapping_get(unit, core, port, &temp);
    BCMDNX_IF_ERR_EXIT(rc);

    if (temp < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    if (old_template != NULL) {
        *old_template = temp;
    }

    rc = dpp_am_template_exchange(unit, core, dpp_am_template_cosq_port_hr_flow_control, 0, data, temp, &temp_is_last, template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = temp_is_last;
    }

    /* Since port-profile mapping is implemented as s/w cache. */
    _bcm_dpp_am_template_cosq_port_hr_fc_profile_mapping_set(unit, core, port, (*template));
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_hr_fc_ref_get(int unit, int core, int template, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, core, dpp_am_template_cosq_port_hr_flow_control, template, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_hr_fc_allocate_group(int unit, int core, uint32 flags, int *data, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate_group(unit, core, dpp_am_template_cosq_port_hr_flow_control, flags, (void *)data, ref_count, is_allocated, template);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



/* COSQ Port Hr flow control profile - End */


/* COSQ CL scheduler profile - Start */

int
_bcm_dpp_am_template_cosq_sched_class_init(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_sched_class_allocate(int unit, int core, int flags, int *data, int *is_allocated, int *class_template)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, core /* BCM_DPP_AM_DEFAULT_POOL_IDX */, dpp_am_template_cosq_sched_class, flags, data, is_allocated, class_template);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_sched_class_free(int unit, int core, int class_template, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, core /* BCM_DPP_AM_DEFAULT_POOL_IDX */, dpp_am_template_cosq_sched_class, class_template, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_sched_class_data_get(int unit, int core, int class_template, int *data)
{
    int rc = BCM_E_NONE;

    
    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, core /* BCM_DPP_AM_DEFAULT_POOL_IDX */, dpp_am_template_cosq_sched_class, class_template, data);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_sched_class_exchange(int unit, int core, int flags, int *data, int old_class_template, int *is_last, int *class_template, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int temp_is_last;

    
    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_exchange(unit, core /* BCM_DPP_AM_DEFAULT_POOL_IDX */, dpp_am_template_cosq_sched_class, flags, data, old_class_template, &temp_is_last, class_template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        (*is_last) = temp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_sched_class_ref_get(int unit, int core, int template, uint32 *ref_count)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, core /* BCM_DPP_AM_DEFAULT_POOL_IDX */, dpp_am_template_cosq_sched_class, template, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_cosq_sched_class_allocate_group(int unit, int core, uint32 flags, int *data, int ref_count, int *is_allocated, int *template)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_allocate_group(unit, core /* was BCM_DPP_AM_DEFAULT_POOL_IDX */, dpp_am_template_cosq_sched_class, flags, (void *)data, ref_count, is_allocated, template);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


/* COSQ CL scheduler profile - End */


/* MPLS push profile - Start */


int _bcm_dpp_am_template_mpls_push_profile_exchange(int unit, int push_profile, SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *push_profile_info,  int *is_last, 
                                                    int *new_push_profile,int *is_allocated, uint8 with_id)
{
    int rc = BCM_E_NONE;
    int tmp_is_last;
    int flags=0;

    BCMDNX_INIT_FUNC_DEFS;

    if (with_id) {
        flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    }
    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_mpls_push_profile,
                                  flags,push_profile_info,push_profile,&tmp_is_last,new_push_profile,is_allocated);

    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_am_template_mpls_push_profile_alloc(int unit, int flags, int push_profile, SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO *push_profile_info, int *new_push_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mpls_push_profile, flags, push_profile_info, is_allocated, new_push_profile);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_mpls_push_profile_free(int unit, int push_profile, int *is_last)
{
    int rc = BCM_E_NONE;


    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_mpls_push_profile,push_profile,is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_mpls_push_profile_is_profile_allocated(int unit, int profile, int *is_allocated)
{
    int rc = BCM_E_NONE;
    uint32 ref_count;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_mpls_push_profile, profile, &ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

    *is_allocated = (ref_count > 0) ? 1 : 0;

exit:
    BCMDNX_FUNC_RETURN;
}

/* MPLS push profile - End */


/* LIF Termination profile - Start */

STATIC int
_bcm_dpp_am_template_lif_term_mapping_get(int unit, SOC_PPC_LIF_ID lif_id,int *template, SOC_PPC_LIF_ENTRY_TYPE * lif_entry_type)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;      
    SOC_PPC_LIF_ENTRY_INFO lif_entry_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(template);

    *template = 0;
    soc_sand_dev_id = (unit);

    SOC_PPC_LIF_ENTRY_INFO_clear(&lif_entry_info);

    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, lif_id, &lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(lif_entry_info.type != SOC_PPC_LIF_ENTRY_TYPE_PWE && lif_entry_info.type != SOC_PPC_LIF_ENTRY_TYPE_EMPTY
       && lif_entry_info.type != SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("LIF entry type is not as expected PWE,MPLS")));      /* expected to be PWE type*/
    }

    if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_PWE) {
      *template = lif_entry_info.value.pwe.term_profile;
    } else if (lif_entry_info.type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {      
      *template = lif_entry_info.value.mpls_term_info.term_profile;
    }

    if (lif_entry_type) {
      *lif_entry_type = lif_entry_info.type;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_lif_term_init(int unit,int template_init_id,const SOC_PPC_MPLS_TERM_PROFILE_INFO* term_profile)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_lif_term_profile, flags, term_profile, _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);

    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_lif_term_pw_with_cw_profile, flags, term_profile, _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);
   
exit:
    BCMDNX_FUNC_RETURN;
}

/* In this case, we assume:
 * dpp_am_template_lif_term_pw_with_cw_profile (template1) is used for profile 0 - 3 for both tunnel and PW.
 * dpp_am_template_lif_term_profile (template2) is used for profile 4 - 7 for PW only.
 */
static int
_bcm_dpp_am_template_lif_term_all_cw_data_get(int unit, SOC_PPC_LIF_ID lif_id, SOC_PPC_MPLS_TERM_PROFILE_INFO *term_profile_info)
{
    int template;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* Only in case of EVPN, can tunnel LIF has CW.*/
    if (!(SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("invalid term profile exchange")));
    }

    rc = _bcm_dpp_am_template_lif_term_mapping_get(unit,lif_id,&template,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    if (template < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get lif term")));
    }

    if (template >= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT)
    {
        /*The profile id is larger than total cound, then it must be for pw, get it from template2*/
        template &= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MASK;
        rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_profile,template,term_profile_info);
    } else {
        /*The profile id is less than total cound, then it may be for tunnel or pw, get it from template1*/
        rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_pw_with_cw_profile,template,term_profile_info);
    }

    BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Failed to get term profile info for lif %d (term_profile %d)"), lif_id, template));

exit:
    BCMDNX_FUNC_RETURN;
}


/* term profile range is 0~7, the msb of term profile id is used indicate whether has control words.
  term profile for pw with cw use 4~7, lsp and pw without cw share 0~3.
  there are two template groups: one is for regular template (0~3), another group is for pw with cw (also is 0~3)
  allocated term profile id always is 0~3, if it is for pw with cw, then set msb of profile id, use profile 4~7
*/
int _bcm_dpp_am_template_lif_term_data_get(int unit, SOC_PPC_LIF_ID lif_id, SOC_PPC_MPLS_TERM_PROFILE_INFO *term_profile_info)
{
    int template;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_lif_term_mapping_get(unit,lif_id,&template,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    if (template < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get lif term")));
    }

    /* Only in case of EVPN, can tunnel LIF has CW.*/
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_lif_term_all_cw_data_get(unit,lif_id,term_profile_info));
        BCM_EXIT;
    }

    /*The profile id is larger than total cound, then it is is for pw with cw*/
    if (template >= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT)
    {
        template &= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MASK;
        rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_pw_with_cw_profile,template,term_profile_info);
        BCMDNX_IF_ERR_EXIT(rc);
        term_profile_info->flags |= SOC_PPC_MPLS_TERM_HAS_CW;
    } else {
        rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_profile,template,term_profile_info);
        BCMDNX_IF_ERR_EXIT(rc);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* In this case, we assume:
 * dpp_am_template_lif_term_pw_with_cw_profile (template1) is used for profile 0 - 3 for both tunnel and PW.
 * dpp_am_template_lif_term_profile (template2) is used for profile 4 - 7 for PW only.
 */
static int
_bcm_dpp_am_template_lif_term_profile_exchange_all_cw(int unit, SOC_PPC_LIF_ID lif_id, int is_pwe_lif, SOC_PPC_MPLS_TERM_PROFILE_INFO *term_profile_info,  int *old_term_profile, int *is_last, int *new_term_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int old_profile;
    int new_profile;
    int tmp_is_last, tmp_is_allocated, tmp_new_profile;
    SOC_PPC_MPLS_TERM_PROFILE_INFO dft_profile_info;

    BCMDNX_INIT_FUNC_DEFS;

    /* Only in case of EVPN, can tunnel LIF has CW.*/
    if (!(SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("invalid term profile exchange")));
    }

    rc = _bcm_dpp_am_template_lif_term_mapping_get(unit,lif_id,&old_profile,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_term_profile != NULL) {
        *old_term_profile = old_profile;
    }

    if (is_pwe_lif) {
        /* 
         * Available termination_profile is 0 -7 (all)
         * Check if there is a existing profile available in range 0 - 3 alreay.
         */
        SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(&dft_profile_info);

        rc = dpp_am_template_template_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_lif_term_pw_with_cw_profile, term_profile_info, &new_profile);
        if (rc == BCM_E_NONE) {
            /* Proper profile exist, exchange in this template1.*/

            if (old_profile < _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT) {
                /* If old_profile in this template1 too, exchange directly.*/
                rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_pw_with_cw_profile,
                                              0,term_profile_info,old_profile,&tmp_is_last,&new_profile,is_allocated);
                BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Failed to exchange in dpp_am_template_lif_term_pw_with_cw_profile for lif %d"), lif_id));
            } else {
                /* If old_profile is in template2, exchange in both templates for allocation and de-allocation.*/
                old_profile -= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT;

                /* Exchange in template1 for allocation with default profile.*/
                rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_pw_with_cw_profile,
                                              0,term_profile_info,0,&tmp_is_last,&new_profile,is_allocated);
                BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Failed to allocate in dpp_am_template_lif_term_pw_with_cw_profile for lif %d"), lif_id));

                /* Exchange in template2 for de-allocation with default data.*/
                rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_profile,
                                              0,&dft_profile_info,old_profile,&tmp_is_last,&tmp_new_profile,&tmp_is_allocated);
                BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Failed to de-allocate in dpp_am_template_lif_term_profile for lif %d"), lif_id));
            }
        } else if (rc == BCM_E_NOT_FOUND) {
            /* Proper profile doesn't exist, exchange in template2.*/
            if (old_profile >= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT) {
                old_profile -= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT;
            }
            rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_profile,
                                          0,term_profile_info,old_profile,&tmp_is_last,&new_profile,is_allocated);
            if (rc == BCM_E_MEMORY) {
                /* If failed in template2, also try to exchange in template1.*/
                rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_pw_with_cw_profile,
                                              0,term_profile_info,old_profile,&tmp_is_last,&new_profile,is_allocated);
                BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Failed to exchange in dpp_am_template_lif_term_pw_with_cw_profile for lif %d"), lif_id));
            } else {
                BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Failed to exchange in dpp_am_template_lif_term_profile for lif %d"), lif_id));
                /* exchanged in template2, range is 4 - 7.*/
                new_profile += _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT;
            }
        } else {
            BCMDNX_IF_ERR_EXIT(rc);
        }
    } else {
        /* If MPLS Tunnel or unknown(EMPTY), It can exchange in template1 only.*/
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_term_pw_with_cw_profile,
                                      0,term_profile_info,old_profile,&tmp_is_last,&new_profile,is_allocated);
        BCMDNX_IF_ERR_EXIT_MSG(rc, (_BSL_BCM_MSG("Failed to exchange in dpp_am_template_lif_term_pw_with_cw_profile for lif %d"), lif_id));
    }

    *new_term_profile = new_profile;

    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_lif_term_profile_exchange(int unit, SOC_PPC_LIF_ID lif_id, int is_pwe_lif, SOC_PPC_MPLS_TERM_PROFILE_INFO *term_profile_info,  int *old_template, int *is_last, int *new_term_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_template;
    int tmp_new_template;
    int new_has_cw = 0;
    int old_has_cw = 0;
    int request_null =0;
    int tmp_is_last;
    SOC_PPC_MPLS_TERM_PROFILE_INFO tmp_profile_info;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
        rc = _bcm_dpp_am_template_lif_term_profile_exchange_all_cw(unit, lif_id, is_pwe_lif, term_profile_info, old_template, is_last, new_term_profile, is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
        BCM_EXIT;
    }

    rc = _bcm_dpp_am_template_lif_term_mapping_get(unit,lif_id,&tmp_old_template,NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    SOC_PPC_MPLS_TERM_PROFILE_INFO_clear(&tmp_profile_info);

    if(!memcmp(&tmp_profile_info,term_profile_info,sizeof(tmp_profile_info))) {
        request_null = 1; /* term delete*/
    } else {
        /*if term_profile_info has SOC_PPC_MPLS_TERM_HAS_CW flag, then is requesting for new pw with cw*/
        if (term_profile_info->flags & SOC_PPC_MPLS_TERM_HAS_CW){
            new_has_cw = 1;
        }
    }
    /*if old template is larger than total, then old template is for pw with cw*/
    if (tmp_old_template && (tmp_old_template >= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_COUNT)){
        old_has_cw = 1;
        tmp_old_template &= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_MASK;
    }

    /*if getting a new term profile for pw with cw (old is default 0) or updating term profile for pw with cw (both new and old have cw)
      then exchange old and new template from profile group for pw with cw*/
    if(new_has_cw &&(!tmp_old_template || old_has_cw)){
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
            dpp_am_template_lif_term_pw_with_cw_profile,0,term_profile_info,tmp_old_template,&tmp_is_last,&tmp_new_template,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
        *new_term_profile = tmp_new_template | _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_ID_MSB_AS_PW;
    } else if ((!new_has_cw && (!tmp_old_template || !old_has_cw))
        || (request_null && !old_has_cw)) {
        /*if getting a new term profile for lsp or pw without cw (old is default 0),
         or if updating term profile for lsp or pw without cw,
         or if deleting lsp or pw without cw (request is default and old has not cw), 
         then exchange old and new template from regular term profile template group*/
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
            dpp_am_template_lif_term_profile,0,term_profile_info,tmp_old_template,&tmp_is_last,new_term_profile,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
    } else if(request_null && old_has_cw) {
        /*if deleting pw without cw (request is default and old has cw)
        exchange old template to default in  term profile template group for pw with cw*/
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
            dpp_am_template_lif_term_pw_with_cw_profile,0,term_profile_info,tmp_old_template,&tmp_is_last,new_term_profile,is_allocated);
        BCMDNX_IF_ERR_EXIT(rc);
    } else { 
        /*updating pw, only old or new term profile has cw*/
        /*if old has cw but new has not, exchange old to default in template group for pw with cw
          and then exchange default to new in regular template group*/
        if(old_has_cw && !new_has_cw) {
            rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                dpp_am_template_lif_term_pw_with_cw_profile,0,&tmp_profile_info,tmp_old_template,&tmp_is_last,new_term_profile,is_allocated);
            BCMDNX_IF_ERR_EXIT(rc);
            rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                dpp_am_template_lif_term_profile,0,term_profile_info,0,&tmp_is_last,new_term_profile,is_allocated);
            BCMDNX_IF_ERR_EXIT(rc);
        } else if ( !old_has_cw && new_has_cw) {
            /*if old has not cw but new has, exchange old to default in regular template group
             and then exchange default to new in  template group for pw with cw*/
            rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                dpp_am_template_lif_term_profile,0,&tmp_profile_info,tmp_old_template,&tmp_is_last,new_term_profile,is_allocated);
            BCMDNX_IF_ERR_EXIT(rc);
            rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,
                dpp_am_template_lif_term_pw_with_cw_profile,0,term_profile_info,0,&tmp_is_last,&tmp_new_template,is_allocated);
            BCMDNX_IF_ERR_EXIT(rc);
            *new_term_profile = tmp_new_template | _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_ID_MSB_AS_PW;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("invalid term profile exchange")));
        }
    }

    if (old_template != NULL) {
        if (old_has_cw) {
           tmp_old_template |= _DPP_AM_TEMPLATE_LIF_TERM_PROFILE_ID_MSB_AS_PW;
        }
        *old_template = tmp_old_template;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* MPLS push profile - End */

/* SA Drop profile - Start */
STATIC int
_bcm_dpp_am_template_port_mact_sa_drop_mapping_get(int unit, int core_id, int port,int *template)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;
    SOC_PPC_FRWRD_MACT_PORT_INFO port_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(template);

    soc_sand_dev_id = (unit);

    SOC_PPC_FRWRD_MACT_PORT_INFO_clear(&port_info);

    soc_sand_rv = soc_ppd_frwrd_mact_port_info_get(soc_sand_dev_id,core_id,port,&port_info);      
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *template = port_info.sa_drop_action_profile;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_mact_sa_drop_init(int unit,int template_init_id,const _bcm_petra_port_mact_mgmt_action_profile_t* action_profile)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    int ref_cnt = _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_MAX_ENTITIES(unit) - _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_COUNT;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif   
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_port_mact_sa_drop_profile, flags, action_profile, ref_cnt, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);

    /* Add 4 dummies one for each template.template = action_profile */
    for (template = _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_LOW_ID; template < _DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_LOW_ID+_DPP_AM_TEMPLATE_PORT_MACT_SA_DROP_PROFILE_COUNT; ++template) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_port_mact_sa_drop_profile, flags, &template, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);

    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_mact_sa_drop_data_get(int unit,int core_id, int port,_bcm_petra_port_mact_mgmt_action_profile_t* action_profile)
{
    int template;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_port_mact_sa_drop_mapping_get(unit,core_id,port,&template);
    BCMDNX_IF_ERR_EXIT(rc);

    if (template < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get port mact source address drop")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_port_mact_sa_drop_profile,template,action_profile);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_mact_sa_drop_exchange(int unit, int core_id, int port, _bcm_petra_port_mact_mgmt_action_profile_t* action_profile, int *old_template, int *is_last, int *new_template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_template;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_port_mact_sa_drop_mapping_get(unit,core_id,port,&tmp_old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_port_mact_sa_drop_profile,0,action_profile,tmp_old_template,&tmp_is_last,new_template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_template != NULL) {
        *old_template = tmp_old_template;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/* SA drop profile - End */

/* DA unknown profile - Start */
STATIC int
_bcm_dpp_am_template_port_mact_da_unknown_mapping_get(int unit, int core_id, int port,int *template)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;      
    SOC_PPC_FRWRD_MACT_PORT_INFO port_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(template);

    soc_sand_dev_id = (unit);

    SOC_PPC_FRWRD_MACT_PORT_INFO_clear(&port_info);

    soc_sand_rv = soc_ppd_frwrd_mact_port_info_get(soc_sand_dev_id,core_id,port,&port_info);      
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    

    
    *template = port_info.da_unknown_action_profile;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_mact_da_unknown_init(int unit,int template_init_id,const _bcm_petra_port_mact_mgmt_action_profile_t* action_profile)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    int ref_cnt = _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_MAX_ENTITIES(unit) - _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_COUNT;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif   

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_port_mact_da_unknown_profile, flags, action_profile,ref_cnt, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(result);
    
    /* Add 4 dummies */
    for (template = _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_LOW_ID; template < _DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_LOW_ID+_DPP_AM_TEMPLATE_PORT_MACT_DA_UNKNOWN_PROFILE_COUNT; ++template) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_port_mact_da_unknown_profile, flags, &template, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);

    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_mact_da_unknown_data_get(int unit,int core_id, int port,_bcm_petra_port_mact_mgmt_action_profile_t* action_profile)
{
    int template;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_port_mact_da_unknown_mapping_get(unit,core_id,port,&template);
    BCMDNX_IF_ERR_EXIT(rc);

    if (template < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get port mact destination address unknown")));
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_port_mact_da_unknown_profile,template,action_profile);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_port_mact_da_unknown_exchange(int unit, int core_id, int port, _bcm_petra_port_mact_mgmt_action_profile_t* action_profile, int *old_template, int *is_last, int *new_template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_template;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_port_mact_da_unknown_mapping_get(unit,core_id,port,&tmp_old_template);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_port_mact_da_unknown_profile,0,action_profile,tmp_old_template,&tmp_is_last,new_template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_template != NULL) {
        *old_template = tmp_old_template;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/* SA drop profile - End */


/* meter profile - Start */

/* init both group  0:a or 1:b */
int _bcm_dpp_am_template_meter_init(int unit, int template_init_id, bcm_dpp_am_meter_entry_t *meter_profile_info)
{
    int is_allocated, core_id;
    uint32 flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    int template = template_init_id;
    int template_type;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif   

    /* Add all entities to template init id with given data */
    template_type = dpp_am_template_meter_profile_a_low;

    /* Add all entities to template init id with given data */
	_DPP_POLICER_METER_CORES_ITER(core_id){
		BCMDNX_IF_ERR_EXIT(dpp_am_template_allocate_group(unit, core_id, template_type, flags, meter_profile_info, SOC_DPP_CONFIG(unit)->meter.nof_meter_a, &is_allocated,&template));
	}

    template_type = dpp_am_template_meter_profile_b_low;

    /* Add all entities to template init id with given data */
	_DPP_POLICER_METER_CORES_ITER(core_id){
		BCMDNX_IF_ERR_EXIT(dpp_am_template_allocate_group(unit, core_id, template_type, flags, meter_profile_info, SOC_DPP_CONFIG(unit)->meter.nof_meter_b, &is_allocated,&template));
	}

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get the data (meter_profile_info) of meter_profile. 
 * Returns BCM_E_NOT_FOUND if the meter profile does not exist (not allocated). 
 */
int _bcm_dpp_am_template_meter_data_get(
   int unit,
   int core_id,
   int meter_profile,
   int group,
   bcm_dpp_am_meter_entry_t *meter_profile_info
   )
{
    int rc = BCM_E_NONE;
    _dpp_am_template_t tmp_tmplt_type = dpp_am_template_meter_profile_a_low;

    BCMDNX_INIT_FUNC_DEFS;
    tmp_tmplt_type += group;

    rc = dpp_am_template_data_get(unit, core_id,tmp_tmplt_type,meter_profile,meter_profile_info);
    if (rc == BCM_E_NOT_FOUND) {
        BCM_ERR_EXIT_NO_MSG(rc);
    }
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN; 
}

/* group can be 0:a or 1:b */
/* 
 * If *new_meter_profile != 0:
 * 1) If change_profile != 0, then the profile data (hash key) is replaced. 
 * 2) Increment reference count (allocate). 
 *  
 * If the old profile did not exist and change_profile == 0 then BCM_E_NOT_FOUND is returned. 
 * If change_profile != 0 and the data already exists in another index 
 * then an error is returned (BCM_E_EXISTS).
 */
int _bcm_dpp_am_template_meter_exchange(
    int unit, 
	int core_id,
    int meter_profile, 
    int group, 
    bcm_dpp_am_meter_entry_t *meter_profile_info,  
    int *is_last, 
    int *new_meter_profile,
    int change_profile,
    int *is_allocated
    )
{
    int rc = BCM_E_NONE;
    int tmp_is_last;
    _dpp_am_template_t tmp_tmplt_type = dpp_am_template_meter_profile_a_low;
    uint32 flags = 0;

    BCMDNX_INIT_FUNC_DEFS;
    tmp_tmplt_type += group;

    if (*new_meter_profile != 0) {
        flags |= SHR_TEMPLATE_MANAGE_SET_WITH_ID;

        if (!change_profile) {
            flags |= SHR_TEMPLATE_MANAGE_IGNORE_DATA;
        }
    }

    rc = dpp_am_template_exchange(unit, core_id,tmp_tmplt_type,flags,meter_profile_info,meter_profile,&tmp_is_last,new_meter_profile,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


/* meter profile - End */


/* Fabric TDM direct routing - Start */
STATIC int
_bcm_dpp_am_template_fabric_tdm_link_ptr_mapping_get(int unit, int port,int *link_ptr)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    uint32 link_pointer = 0; 
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(link_ptr);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_profile_map_get,(unit,port,&link_pointer)));    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    

    *link_ptr = link_pointer;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_fabric_tdm_link_ptr_init(int unit,int template_init_id,const SOC_TMC_TDM_DIRECT_ROUTING_INFO* routing_info)
{
    int index, is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif   
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    for (index = 0; index < _DPP_AM_TEMPLATE_FABRIC_TDM_LINK_PTR_MAX_ENTITIES; index++) {
        result = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_fabric_tdm_link_ptr, flags, routing_info, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_fabric_tdm_link_ptr_data_get(int unit,int port,SOC_TMC_TDM_DIRECT_ROUTING_INFO* routing_info)
{
    int link_pointer;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_fabric_tdm_link_ptr_mapping_get(unit,port,&link_pointer);
    BCMDNX_IF_ERR_EXIT(rc);

    if (link_pointer < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get link pointer")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_fabric_tdm_link_ptr_exchange(int unit, int port, SOC_TMC_TDM_DIRECT_ROUTING_INFO * routing_info, int *old_link_ptr, int *is_last, int *new_link_ptr,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_link_ptr;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;
    rc = _bcm_dpp_am_template_fabric_tdm_link_ptr_mapping_get(unit,port,&tmp_old_link_ptr);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_fabric_tdm_link_ptr,0,routing_info,tmp_old_link_ptr,&tmp_is_last,new_link_ptr,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_link_ptr != NULL) {
        *old_link_ptr = tmp_old_link_ptr;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/* Fabric TDM direct routing - End */

/* Flooding per LIF - start */
STATIC int
_bcm_dpp_am_template_l2_flooding_mapping_get(int unit, SOC_PPC_LIF_ID lif_index,int *profile)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;   
    SOC_PPC_LIF_ENTRY_INFO *lif_entry_info = NULL;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ALLOC(lif_entry_info, sizeof(SOC_PPC_LIF_ENTRY_INFO), "_bcm_dpp_am_template_l2_flooding_mapping_get.lif_entry_info");
    if (lif_entry_info == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    SOC_PPC_LIF_ENTRY_INFO_clear(lif_entry_info);

    soc_sand_dev_id = (unit);

    /* Retrieve LIF default profile */
    soc_sand_rv = soc_ppd_lif_table_entry_get(soc_sand_dev_id, lif_index, lif_entry_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    /* expected to be AC, PWE, Extender, Tunnel type*/
    if ((lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_AC) && (lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_FIRST_AC_IN_GROUP) &&
        (lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_EMPTY) && (lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_PWE) &&
        (lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_EXTENDER) && (lif_entry_info->type != SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("LIF entry type is not as expected AC, PWE, Extender, Tunnel")));
    }

    if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_PWE) {
        *profile = lif_entry_info->value.pwe.default_forward_profile;
    } else if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_EXTENDER) {
        *profile = lif_entry_info->value.extender.default_forward_profile;
    } else if (lif_entry_info->type == SOC_PPC_LIF_ENTRY_TYPE_MPLS_TUNNEL_RIF) {
        *profile = lif_entry_info->value.mpls_term_info.default_forward_profile;
    } else {
        *profile = lif_entry_info->value.ac.default_forward_profile;
    }    

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCM_FREE(lif_entry_info);
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_am_template_l2_flooding_init(int unit, int template_init_id, const bcm_dpp_vlan_flooding_profile_info_t* flooding_info)
{
    int rc = BCM_E_NONE;
    uint32 flags = 0;
    int template;    
    int is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif   

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;

    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l2_flooding_profile, flags, flooding_info, _DPP_AM_TEMPLATE_L2_FLOODING_LIF_PROFILE_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2_flooding_data_get(int unit, SOC_PPC_LIF_ID lif_index, bcm_dpp_vlan_flooding_profile_info_t* flooding_info)
{
    int rc = BCM_E_NONE;
    int template;
    
    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_l2_flooding_mapping_get(unit,lif_index,&template);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_flooding_profile,template,flooding_info);
    BCMDNX_IF_ERR_EXIT(rc);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l2_flooding_exchange(int unit, SOC_PPC_LIF_ID lif_index, bcm_dpp_vlan_flooding_profile_info_t* flooding_info, int *old_template, int *is_last, int *new_template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_template = 0, tmp_new_template;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_l2_flooding_mapping_get(unit,lif_index,&tmp_old_template);
    BCMDNX_IF_ERR_EXIT(rc);
   
    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l2_flooding_profile,0,flooding_info,tmp_old_template,&tmp_is_last,&tmp_new_template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);


    if (old_template != NULL) {        
        *old_template = tmp_old_template;
    }

    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    *new_template = tmp_new_template;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/* Flooding per LIF - end */

/* Port protocol to vlan,tc - start */
STATIC int _bcm_dpp_am_template_vlan_port_protocol_mapping_get(int unit, int port, int *template)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;   
    SOC_PPC_PORT_INFO port_info;
    SOC_PPC_LLP_VID_ASSIGN_PORT_INFO port_vid_assign_info;
    uint32  pp_port;
    int     core;
    
    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_PORT_INFO_clear(&port_info);
    SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(&port_vid_assign_info);

    soc_sand_dev_id = (unit);
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    soc_sand_rv = soc_ppd_llp_vid_assign_port_info_get(soc_sand_dev_id, core, pp_port, &port_vid_assign_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if (port_vid_assign_info.enable_protocol) {
        /* Ether type based profile */
        soc_sand_rv = soc_ppd_port_info_get(soc_sand_dev_id, core, pp_port, &port_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *template = port_info.ether_type_based_profile;
    } else {
        *template = NOS_PRTCL_PORT_PROFILES;
    }
       

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vlan_port_protocol_init(int unit,int template_init_id,const bcm_dpp_vlan_port_protocol_entries_t* port_protocol_info)
{
    int rc = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    int is_allocated;
    
    BCMDNX_INIT_FUNC_DEFS;


#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif   
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;

    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_vlan_port_protocol_profile, flags, port_protocol_info,_DPP_AM_TEMPLATE_VLAN_PORT_PROTOCOL_PROFILE_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_am_template_vlan_port_protocol_data_get(int unit, int port, bcm_dpp_vlan_port_protocol_entries_t* port_protocol_info)
{
    int rc = BCM_E_NONE;
    int template;
    
    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_vlan_port_protocol_mapping_get(unit,port,&template);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_port_protocol_profile,template,port_protocol_info);
    BCMDNX_IF_ERR_EXIT(rc);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
int _bcm_dpp_am_template_vlan_port_protocol_exchange(int unit, int port, bcm_dpp_vlan_port_protocol_entries_t* port_protocol_info, int *old_template, int *is_last, int *new_template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_template = 0, tmp_new_template;
    int tmp_is_last;
    
    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_vlan_port_protocol_mapping_get(unit,port,&tmp_old_template);
    BCMDNX_IF_ERR_EXIT(rc);
   
    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_vlan_port_protocol_profile,0,port_protocol_info,tmp_old_template,&tmp_is_last,&tmp_new_template,is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_template != NULL) {        
        *old_template = tmp_old_template;
    }

    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

    *new_template = tmp_new_template;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/* Port protocol to vlan,tc - end */



/* IP tunnel to SIP, TTL, TOS - start */
STATIC int _bcm_dpp_am_template_ip_tunnel_mapping_get(int unit, int tunnel_id, int is_new_ip_tunnel, int *sip_profile, int *ttl_profile, int *tos_profile)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int soc_sand_dev_id;   
    SOC_PPC_EG_ENCAP_ENTRY_INFO   *encap_entry_info = NULL;
    uint32         next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32         nof_entries;
    
    BCMDNX_INIT_FUNC_DEFS;


    /* if this is new IP tunnel to allocate, then it use dummy profiles */
    if(is_new_ip_tunnel) {
        *sip_profile = _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_COUNT-1;
        *ttl_profile = _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_COUNT-1;
        *tos_profile = _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_COUNT-1;
        BCM_EXIT;
    }

    /* otherwise read hardware */
    soc_sand_dev_id = (unit);
    
    BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "_bcm_dpp_am_template_ip_tunnel_mapping_get.encap_entry_info");
    if(encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                               tunnel_id, 1,
                               encap_entry_info, next_eep,
                               &nof_entries);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if(encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("given Tunnel ID has to point to IP tunnel encapsulation")));
    }

    *sip_profile = encap_entry_info[0].entry_val.ipv4_encap_info.dest.src_index;
    *ttl_profile = encap_entry_info[0].entry_val.ipv4_encap_info.dest.ttl_index;
    *tos_profile = encap_entry_info[0].entry_val.ipv4_encap_info.dest.tos_index;

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ip_tunnel_init(int unit, int sip_profile , SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *sip_val, int ttl_profile, int ttl_val,
                                        int tos_profile, int tos_val, int tunnel_template_profile,
                                        SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *tunnel_template_val)
{
    int rc = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    int is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif   

    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    

    /* init SIP */
    /* -1 to indicate use dummy profile, good for driver init */
    template = (sip_profile != -1)?sip_profile:_DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_COUNT-1;
   
    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ip_tunnel_src_ip, flags, sip_val, _DPP_AM_TEMPLATE_IP_TUNNEL_SRC_IP_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(rc);
    
    /* init TTL */
    /* -1 to indicate use dummy profile, good for driver init */
    template = (ttl_profile != -1)?ttl_profile:_DPP_AM_TEMPLATE_IP_TUNNEL_TTL_COUNT-1;
    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ip_tunnel_ttl, flags, &ttl_val, _DPP_AM_TEMPLATE_IP_TUNNEL_TTL_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(rc);


    /* init TOS */
    /* -1 to indicate use dummy profile, good for driver init */
    template = (tos_profile != -1)?tos_profile:_DPP_AM_TEMPLATE_IP_TUNNEL_TOS_COUNT-1;
    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ip_tunnel_tos, flags, &tos_val, _DPP_AM_TEMPLATE_IP_TUNNEL_TOS_MAX_ENTITIES, &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(rc);

    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
        /* From jericho B0 / QAX, template represent the tunnel type.
           init Tunnel Encapsulation Mode */
        template = (tunnel_template_profile != -1)?tunnel_template_profile:_DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_COUNT-1;   /* dummy template */
        rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ip_tunnel_encapsulation_mode, flags, tunnel_template_val, _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_MAX_ENTITIES, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ip_tunnel_data_get(int unit, int tunnel_id, SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *sip_val, int *ttl_val, int *tos_val)
{
    int rc = BCM_E_NONE;
    int sip_profile, ttl_profile, tos_profile;
    
    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_ip_tunnel_mapping_get(unit,tunnel_id,0,&sip_profile,&ttl_profile,&tos_profile);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_src_ip,sip_profile,sip_val);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_ttl,ttl_profile,ttl_val);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_tos,tos_profile,tos_val);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* set DF flag in tos-index*/
#define _BCM_DPP_IP_TUNNEL_DF_IN_TOS_SET(__tos_index, _df_value)  (__tos_index |= (_df_value) << 3)
#define _BCM_DPP_IP_TUNNEL_DF_FROM_TOS_GET(__tos_index)  (((__tos_index) >>  3) & 1)
#define _BCM_DPP_DF_TOS_MASK 0x7




/* check if sip, ttl and tos template allocations will suceed */
int _bcm_dpp_am_template_ip_tunnel_exchange_test(int unit, int tunnel_id, int is_new_ip_tunnel, SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *sip, int ttl, int tos, int *sip_template, int *ttl_template, int *tos_template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int old_profile_sip, old_profile_ttl, old_profile_tos;
    int tmp_is_last;
    int cur_is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_ip_tunnel_mapping_get(unit,tunnel_id, is_new_ip_tunnel,&old_profile_sip,&old_profile_ttl,&old_profile_tos);
    BCMDNX_IF_ERR_EXIT(rc);

    if(SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set && ( old_profile_tos !=(_DPP_AM_TEMPLATE_IP_TUNNEL_TOS_COUNT-1))){
        /*when ip_tunnel_defrag_set, only 8 profile at most*/
        old_profile_tos = old_profile_tos & _BCM_DPP_DF_TOS_MASK;
    }

    /* exchange test to check all allocation will succeed */
    rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_src_ip,0,sip,old_profile_sip,&tmp_is_last,sip_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_ttl,0,&ttl,old_profile_ttl,&tmp_is_last,ttl_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_tos,0,&tos,old_profile_tos,&tmp_is_last,tos_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* is_allocated is tru if any template is new */
int _bcm_dpp_am_template_ip_tunnel_exchange(int unit, int tunnel_id, int is_new_ip_tunnel, SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP *sip, int ttl, int tos, int *sip_template, int *ttl_template, int *tos_template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int old_profile_sip, old_profile_ttl, old_profile_tos;
    int tmp_is_last;
    int cur_is_allocated = 0;
    uint8 df_flag = 0;
    int update_df = 0;
    
    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_ip_tunnel_mapping_get(unit,tunnel_id, is_new_ip_tunnel,&old_profile_sip,&old_profile_ttl,&old_profile_tos);
    BCMDNX_IF_ERR_EXIT(rc);
   
    if(SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set && ( old_profile_tos !=(_DPP_AM_TEMPLATE_IP_TUNNEL_TOS_COUNT-1))){
        df_flag = _BCM_DPP_IP_TUNNEL_DF_FROM_TOS_GET(old_profile_tos);
        update_df = 1;
        /*when ip_tunnel_defrag_set, only 8 profile at most*/
        old_profile_tos = old_profile_tos & _BCM_DPP_DF_TOS_MASK;
    }
    /* exchange test to check all allocation will succeed */
    rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_src_ip,0,sip,old_profile_sip,&tmp_is_last,sip_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_ttl,0,&ttl,old_profile_ttl,&tmp_is_last,ttl_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_tos,0,&tos,old_profile_tos,&tmp_is_last,tos_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);


    /* if arrive here all tests pass, so now go for actuall allocation */
    *is_allocated = 0;

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_src_ip,0,sip,old_profile_sip,&tmp_is_last,sip_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    *is_allocated |= cur_is_allocated;

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_ttl,0,&ttl,old_profile_ttl,&tmp_is_last,ttl_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    *is_allocated |= cur_is_allocated;

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_ip_tunnel_tos,0,&tos,old_profile_tos,&tmp_is_last,tos_template,&cur_is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if(SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set && update_df){
        _BCM_DPP_IP_TUNNEL_DF_IN_TOS_SET(*tos_template,df_flag);
    }
    *is_allocated |= cur_is_allocated;
    
exit:
    BCMDNX_FUNC_RETURN;
}
/* IP tunnel */


/* Ingress Flow TC Mapping - Start */
STATIC int
_bcm_dpp_am_template_ingress_flow_tc_mapping_get(int unit, int core_id, int dest_ndx ,uint32 *profile_ndx)
{
  bcm_error_t rc = BCM_E_NONE;
  uint32 soc_sand_rv;

  BCMDNX_INIT_FUNC_DEFS;
  BCMDNX_NULL_CHECK(profile_ndx);

  soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_tc_profile_get,(unit, core_id, 1, dest_ndx , profile_ndx)));
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  BCMDNX_IF_ERR_EXIT(rc);
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ingress_flow_tc_mapping_data_get(int unit, int core_id, int dest_ndx,SOC_TMC_ITM_TC_MAPPING *tc_mapping)
{
  uint32 profile_ndx;
  int rc = BCM_E_NONE;
  int core = (core_id == BCM_CORE_ALL) ? BCM_DPP_AM_DEFAULT_POOL_IDX : core_id;
  BCMDNX_INIT_FUNC_DEFS;
  rc = _bcm_dpp_am_template_ingress_flow_tc_mapping_get(unit, core, dest_ndx, &profile_ndx);
  BCMDNX_IF_ERR_EXIT(rc);

  rc = dpp_am_template_data_get(unit, core, dpp_am_template_ingress_flow_tc_mapping, profile_ndx, tc_mapping);
  BCMDNX_IF_ERR_EXIT(rc);

exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ingress_flow_tc_mapping_init(int unit,int template_init_id,const SOC_TMC_ITM_TC_MAPPING* tc_mapping)
{
  int is_allocated, result = BCM_E_NONE;
  uint32 flags = 0;
  int template;
  int core_id;
  BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif 

  flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
  template = template_init_id;

  /* Add all entities to template init id with given data */
  SOC_DPP_ASSYMETRIC_CORES_ITER(BCM_CORE_ALL, core_id) {
      result = dpp_am_template_allocate_group(unit, core_id, dpp_am_template_ingress_flow_tc_mapping, flags, tc_mapping, SOC_DPP_DEFS_GET(unit, nof_flows), &is_allocated,&template);
      BCMDNX_IF_ERR_EXIT(result);
  }
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ingress_flow_tc_mapping_exchange(int unit, int core_id, int dest_ndx, SOC_TMC_ITM_TC_MAPPING *tc_mapping, uint32 *old_mapping_profile, int *is_last, int *new_mapping_profile,int *is_allocated)
{
  int rc = BCM_E_NONE;
  uint32 tmp_old_mapping_profile;
  int tmp_is_last;

  BCMDNX_INIT_FUNC_DEFS;
  rc = _bcm_dpp_am_template_ingress_flow_tc_mapping_get(unit, core_id, dest_ndx, &tmp_old_mapping_profile);
  BCMDNX_IF_ERR_EXIT(rc);

  rc = dpp_am_template_exchange(unit, core_id, dpp_am_template_ingress_flow_tc_mapping, 0, tc_mapping,tmp_old_mapping_profile,&tmp_is_last,new_mapping_profile,is_allocated);
  BCMDNX_IF_ERR_EXIT(rc);

  if (old_mapping_profile != NULL) {
    *old_mapping_profile = tmp_old_mapping_profile;
  }
  if (is_last != NULL) {
    *is_last = tmp_is_last;
  }

  BCMDNX_IF_ERR_EXIT(rc);
exit:
  BCMDNX_FUNC_RETURN;
}

/* Ingress Flow TC Mapping - End */

/* Ingress UC TC Mapping - Start */
STATIC int
_bcm_dpp_am_template_ingress_uc_tc_mapping_get(int unit, int core_id, int dest_ndx ,uint32 *profile_ndx)
{
  bcm_error_t rc = BCM_E_NONE;
  uint32 soc_sand_rv;

  BCMDNX_INIT_FUNC_DEFS;
  BCMDNX_NULL_CHECK(profile_ndx);

  soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_tc_profile_get,(unit, core_id, 0, dest_ndx , profile_ndx)));    
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  BCMDNX_IF_ERR_EXIT(rc);
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ingress_uc_tc_mapping_data_get(int unit, int core_id, int dest_ndx, SOC_TMC_ITM_TC_MAPPING *tc_mapping)
{
  uint32 profile_ndx;
  int rc = BCM_E_NONE;
  int core = (core_id == BCM_CORE_ALL) ? BCM_DPP_AM_DEFAULT_POOL_IDX : core_id;
  BCMDNX_INIT_FUNC_DEFS;
  rc = _bcm_dpp_am_template_ingress_uc_tc_mapping_get(unit, core, dest_ndx, &profile_ndx);
  BCMDNX_IF_ERR_EXIT(rc);

  rc = dpp_am_template_data_get(unit, core, dpp_am_template_ingress_uc_tc_mapping, profile_ndx, tc_mapping);
  BCMDNX_IF_ERR_EXIT(rc);

exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ingress_uc_tc_mapping_init(int unit, int template_init_id,const SOC_TMC_ITM_TC_MAPPING* tc_mapping)
{
  /*int index, is_allocated, result = BCM_E_NONE;*/
  int is_allocated, result = BCM_E_NONE;
  uint32 flags = 0;
  int template;
  int core_id;
  BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif 

  /*flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;*/
  flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
  template = template_init_id;

  /* Add all entities to template init id with given data */
  SOC_DPP_ASSYMETRIC_CORES_ITER(BCM_CORE_ALL, core_id) {
      result = dpp_am_template_allocate_group(unit, core_id, dpp_am_template_ingress_uc_tc_mapping, flags, tc_mapping, _DPP_AM_TEMPLATE_INGRESS_UC_TC_MAPPING_MAX_ENTITIES(unit), &is_allocated,&template);
      BCMDNX_IF_ERR_EXIT(result);
  }
  
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ingress_uc_tc_mapping_exchange(int unit, int core_id, int dest_ndx, SOC_TMC_ITM_TC_MAPPING *tc_mapping, uint32 *old_mapping_profile, int *is_last, int *new_mapping_profile,int *is_allocated)
{
  int rc = BCM_E_NONE;
  uint32 tmp_old_mapping_profile;
  int tmp_is_last;

  BCMDNX_INIT_FUNC_DEFS;
  rc = _bcm_dpp_am_template_ingress_uc_tc_mapping_get(unit, core_id, dest_ndx, &tmp_old_mapping_profile);
  BCMDNX_IF_ERR_EXIT(rc);

  rc = dpp_am_template_exchange(unit, core_id, dpp_am_template_ingress_uc_tc_mapping, 0, tc_mapping, tmp_old_mapping_profile, &tmp_is_last, new_mapping_profile, is_allocated);
  BCMDNX_IF_ERR_EXIT(rc);

  if (old_mapping_profile != NULL) {
    *old_mapping_profile = tmp_old_mapping_profile;
  }
  if (is_last != NULL) {
    *is_last = tmp_is_last;
  }

exit:
  BCMDNX_FUNC_RETURN;
}

/* Ingress UC TC Mapping - End */

/* Flow Control Generic PFC Mapping - Start */

int _bcm_dpp_am_template_fc_generic_pfc_mapping_init(int unit, int priority, int template_id, SOC_TMC_FC_PFC_GENERIC_BITMAP *generic_bm)
{
    int rc = BCM_E_NONE;
    int is_allocated;
    int template_pool;
    
    BCMDNX_INIT_FUNC_DEFS;

#ifdef  BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        template_pool = dpp_am_template_fc_generic_pfc_mapping_c0 + priority;
    }
    else 
#endif
    {
        template_pool = dpp_am_template_fc_generic_pfc_mapping;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif

    /* -1 to indicate use dummy profile, good for driver init */
    template_id = (template_id != -1) ? template_id : (_DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_COUNT -1);

    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_pool, SHR_TEMPLATE_MANAGE_SET_WITH_ID, 
                                       generic_bm, _DPP_AM_TEMPLATE_FC_PFC_GENERIC_BITMAP_MAPPING_MAX_ENTITIES, &is_allocated, &template_id);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_fc_generic_pfc_mapping_data_get(int unit, int priority, int generic_bm_id, SOC_TMC_FC_PFC_GENERIC_BITMAP *generic_bm)
{
    int rc = BCM_E_NONE;
    int template_pool;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef  BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        template_pool = dpp_am_template_fc_generic_pfc_mapping_c0 + priority;
    }
    else 
#endif
    {
        template_pool = dpp_am_template_fc_generic_pfc_mapping;
    }

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_pool, generic_bm_id,generic_bm);
    BCMDNX_IF_ERR_EXIT(rc);
  
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_fc_generic_pfc_mapping_exchange(int unit, int priority, int old_generic_bm_id, SOC_TMC_FC_PFC_GENERIC_BITMAP *new_generic_bm, int *is_last, int *new_mapping_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int flags;
    int tmp_is_last;
    int tmp_old_generic_bm_id = 0;
    int template_pool;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef  BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        template_pool = dpp_am_template_fc_generic_pfc_mapping_c0 + priority;
    }
    else 
#endif
    {
        template_pool = dpp_am_template_fc_generic_pfc_mapping;
    }

    flags = SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE;
    tmp_old_generic_bm_id = old_generic_bm_id;

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_pool, flags, new_generic_bm,
                              tmp_old_generic_bm_id, &tmp_is_last, new_mapping_profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

exit:
    BCMDNX_FUNC_RETURN;
}
/* Flow Control Generic PFC Mapping - End */


/* TTL Scoping Mapping */
int _bcm_dpp_am_template_ttl_scope_init(int unit, int template_init_id, uint16 *ttl)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    int template = template_init_id;
    int template_type = dpp_am_template_ttl_scope_index;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif   
    if (_DPP_AM_TEMPLATE_TTL_SCOPE_MAX_ENTITIES) {
        /* Add all entities to template init id with given data */
        result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_type, flags, ttl, _DPP_AM_TEMPLATE_TTL_SCOPE_MAX_ENTITIES, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ttl_scope_exchange(int unit, int ttl_scope_index, uint16 *ttl, int *is_last, int *new_ttl_scope_index, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_is_last;
    int template_type = dpp_am_template_ttl_scope_index;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_type, 0, ttl, ttl_scope_index, &tmp_is_last, new_ttl_scope_index, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_am_template_ttl_scope_index_ttl_mapping_get(int unit, int ttl_scope_index, uint16 *ttl)
{
    int rc = BCM_E_NONE;
    int template_type = dpp_am_template_ttl_scope_index;
    
    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, template_type, ttl_scope_index, ttl);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* TTL Scoping Mapping - End */

/* CNM Queue Profile - Start */

int _bcm_dpp_am_template_pool_cnm_queue_init(int unit, SOC_TMC_CNM_CP_PROFILE_INFO *profile_info, int template)
{
  int rc = BCM_E_NONE;
  int is_allocated;
  BCMDNX_INIT_FUNC_DEFS;

  rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_cnm_queue_profile, SHR_TEMPLATE_MANAGE_SET_WITH_ID, 
                                       profile_info, _DPP_AM_TEMPLATE_CNM_QUEUE_PROFILE_MAX_ENTITIES, &is_allocated, &template);

  BCMDNX_IF_ERR_EXIT(rc);

exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_pool_cnm_queue_profile_data_get(int unit, int profile_id, SOC_TMC_CNM_CP_PROFILE_INFO *profile_info)
{
  int rc = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

  rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_cnm_queue_profile, profile_id, profile_info);
  BCMDNX_IF_ERR_EXIT(rc);
  
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_pool_cnm_queue_profile_exchange(int unit, int old_profile_id, SOC_TMC_CNM_CP_PROFILE_INFO *new_profile_info, int *new_mapping_profile,int *is_allocated)
{
  int rc = BCM_E_NONE;
  int tmp_is_last;
  int tmp_old_profile_id;
  BCMDNX_INIT_FUNC_DEFS;

  tmp_old_profile_id = old_profile_id;

  rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_cnm_queue_profile, 0, new_profile_info,
                              tmp_old_profile_id, &tmp_is_last, new_mapping_profile, is_allocated);
  BCMDNX_IF_ERR_EXIT(rc);

exit:
  BCMDNX_FUNC_RETURN;
}
/* CNM Queue Profile - End */


/* PTP port profile - Start */
STATIC int
_bcm_dpp_am_template_ptp_port_profile_mapping_get(int unit, int port_ndx, SOC_PPC_PTP_IN_PP_PORT_PROFILE *profile_ndx)
{
  uint32 soc_sand_rv;

  BCMDNX_INIT_FUNC_DEFS;

  BCMDNX_NULL_CHECK(profile_ndx);

  soc_sand_rv = soc_ppd_ptp_port_get(unit, port_ndx, profile_ndx);
  BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            
exit:
  BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_ptp_port_profile_init(int unit, SOC_PPC_PTP_IN_PP_PORT_PROFILE profile_ndx, SOC_PPC_PTP_PORT_INFO *profile_data)
{
  int rc = BCM_E_NONE;
  int is_allocated;
  uint32 flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
  int template = profile_ndx;

  BCMDNX_INIT_FUNC_DEFS;

  rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ptp_port_profile, flags, profile_data, 
                                   _DPP_AM_TEMPLATE_PTP_PORT_PROFILE_MAX_ENTITIES, &is_allocated, &template);
  BCMDNX_IF_ERR_EXIT(rc);

exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ptp_port_profile_deinit(int unit, SOC_PPC_PTP_IN_PP_PORT_PROFILE profile_ndx)
{
  int rc = BCM_E_NONE;
  int is_last;
  int template = profile_ndx;

  BCMDNX_INIT_FUNC_DEFS;

  rc = dpp_am_template_free_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ptp_port_profile, template, -1,  &is_last);
  BCMDNX_IF_ERR_EXIT(rc);

exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_ptp_port_profile_exchange(int unit, int port_ndx, SOC_PPC_PTP_PORT_INFO *new_profile_data, 
                                                   SOC_PPC_PTP_IN_PP_PORT_PROFILE *new_profile_ndx)
{
    int rc = 0;
    int is_last;
    int is_allocated;
    SOC_PPC_PTP_IN_PP_PORT_PROFILE old_profile_ndx;

    BCMDNX_INIT_FUNC_DEFS;

    rc = _bcm_dpp_am_template_ptp_port_profile_mapping_get(unit, port_ndx, &old_profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ptp_port_profile, 0, new_profile_data, old_profile_ndx, &is_last, (int *)new_profile_ndx, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_ptp_port_profile_tdata_get(int unit, SOC_PPC_PTP_IN_PP_PORT_PROFILE profile_ndx, SOC_PPC_PTP_PORT_INFO* profile_data)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ptp_port_profile, profile_ndx, profile_data);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}
/* PTP port profile - End */


/* l3_rif_mac_termination_combination - Start */

int _bcm_dpp_am_template_l3_rif_mact_ermination_combination_init(int unit, uint32 *profile_info, int template)
{
  int rc = BCM_E_NONE;
  int is_allocated;
  BCMDNX_INIT_FUNC_DEFS;

  rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_rif_mac_termination_combination, SHR_TEMPLATE_MANAGE_SET_WITH_ID, 
                                       profile_info, _DPP_AM_TEMPLATE_L3_RIF_MAC_TERMINATION_COMBINTION_MAX_ENTITIES, &is_allocated, &template);

  BCMDNX_IF_ERR_EXIT(rc);

exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l3_rif_mac_termination_combination_data_get(int unit, int profile_id, uint32 *profile_info)
{
  int rc = BCM_E_NONE;

  BCMDNX_INIT_FUNC_DEFS;

  rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_rif_mac_termination_combination, profile_id, profile_info);
  BCMDNX_IF_ERR_EXIT(rc);
  
exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l3_rif_mac_termination_combination_alloc(int unit, int flags, uint32 *profile_info, uint8 *new_profile, int *is_allocated)
{
    int rc = BCM_E_NONE;
    int profile = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_rif_mac_termination_combination, flags, profile_info, is_allocated, &profile);
    (*new_profile) = profile;    
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l3_rif_mac_termination_combination_profile_get(int unit, uint32 profile_info, uint8* profile_id)
{
  int rc = BCM_E_NONE;
  int profile_ndx;
  
  rc = dpp_am_template_template_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_rif_mac_termination_combination, &profile_info, &profile_ndx);
  if (rc == BCM_E_NONE) {
      *profile_id = profile_ndx;
  }

  return rc;
}

int _bcm_dpp_am_template_l3_rif_mac_termination_combination_free(int unit, uint32 profile, int *is_last)
{
    int rc = BCM_E_NONE;
    int profile_as_int = profile;


    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l3_rif_mac_termination_combination,profile_as_int,is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_am_template_l3_rif_mac_termination_combination_exchange(int unit, int old_profile_id, int *old_is_last, uint32 *new_profile_info, uint8 *new_mapping_profile,int *is_allocated, uint8 with_id)
{
  int rc = BCM_E_NONE;
  int new_profile = 0;
  int flags = 0;
  BCMDNX_INIT_FUNC_DEFS;

  if (with_id) {
      flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
      new_profile = *new_mapping_profile;
  }

  rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_l3_rif_mac_termination_combination, flags, new_profile_info,
                              old_profile_id, old_is_last, &new_profile, is_allocated);
  BCMDNX_IF_ERR_EXIT(rc);
  *new_mapping_profile = new_profile;

exit:
  BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_l3_rif_mac_termination_combination_ref_get(int unit, int profile, uint32 *ref_count)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_rif_mac_termination_combination, profile, ref_count);

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/* l3_rif_mac_termination_combination - End */



/*
 * VRRP
 */
#ifdef BCM_88660_A0

int _bcm_dpp_am_template_vrrp_alloc(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, int *is_first, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_vrrp, 0, cam_info, is_first, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_am_template_vrrp_alloc_all
 * Purpose:
 *      Given a profile index, allocates all entries in template_manager.
 * Parameters: 
 *      unit            - (IN) Device id to be configured 
 *      profile_index   - (IN) Profile index to be configured.
 *      alloc_count     - (OUT)Number of entries configured.
 * Returns:
 *      BCM_E_*
 */
int _bcm_dpp_am_template_vrrp_alloc_all(int unit, int profile_index)
{
    int rc = BCM_E_NONE;
    uint32 ref_count;
    int is_allocated;
    uint32 flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    SOC_PPC_VRRP_CAM_INFO cam_info;
    int alloc_count;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get the cam info data */
    SOC_PPC_VRRP_CAM_INFO_clear(&cam_info);

    rc = _bcm_dpp_am_template_vrrp_get(unit, profile_index, &cam_info);
    BCMDNX_IF_ERR_EXIT(rc);

    /* Get how many entries exist. Then, allocate MAX_ENTRIES minus this number. */
    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_vrrp, profile_index, &ref_count);
    BCMDNX_IF_ERR_EXIT(rc);

    alloc_count = _DPP_AM_TEMPLATE_L3_VRRP_PROFILE_MAX_ENTITIES(unit) - ref_count;

    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_vrrp, flags, &cam_info, alloc_count, &is_allocated, &profile_index);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vrrp_get(int unit, int profile_ndx, SOC_PPC_VRRP_CAM_INFO *cam_info)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_vrrp, profile_ndx, cam_info);
    BCM_RETURN_VAL_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_vrrp_dealloc(int unit, int profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_vrrp, profile_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_dpp_am_template_vrrp_dealloc_all
 * Purpose:
 *      Given a profile index, deallocates all entries in template_manager.
 * Parameters: 
 *      unit            - (IN) Device id to be configured 
 *      profile_index   - (IN) Profile index to be configured.
 *      dealloc_count   - (OUT)Number of entries configured.
 * Returns:
 *      BCM_E_*
 */
int _bcm_dpp_am_template_vrrp_dealloc_all(int unit, int profile_index)
{
    int rv = BCM_E_NONE;
    int dealloc_count;
    BCMDNX_INIT_FUNC_DEFS;

    rv = dpp_am_template_free_all(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_vrrp, profile_index, &dealloc_count);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/* Used to get the cam index by the cam info before deleting entries. */
int _bcm_dpp_am_template_vrrp_index_get(int unit, SOC_PPC_VRRP_CAM_INFO *cam_info, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_template_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_l3_vrrp, cam_info, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}



#endif /*BCM_88660_A0 */

/*
 * VRRP - end 
 */


/*
 * eth type index 
 */


int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_get(int unit, int eth_type_index_ndx, SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO* data)
{
    int rc = BCM_E_NONE;

    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_eedb_roo_ll_format_eth_type_index, eth_type_index_ndx, data);

    return rc;
}


int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_alloc(int unit, int flags, SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *data, int *is_allocated, int *eth_type_index_ndx)
{    
    int rv;    

    BCMDNX_INIT_FUNC_DEFS;     

    rv = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_eedb_roo_ll_format_eth_type_index, flags, data, is_allocated, eth_type_index_ndx);    
    BCMDNX_IF_ERR_EXIT(rv); 

exit:    
    BCMDNX_FUNC_RETURN;
}
 
int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_free(int unit, int eth_type_index_ndx, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_eedb_roo_ll_format_eth_type_index, eth_type_index_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_exchange(int unit, int flags, int old_index_ndx,SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *data,int *new_index_ndx,int * is_last, int *is_allocated)
{    
    int rv;    

    BCMDNX_INIT_FUNC_DEFS;     

    rv = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_eedb_roo_ll_format_eth_type_index, flags, data,old_index_ndx, is_last, new_index_ndx,is_allocated);    
    BCMDNX_IF_ERR_EXIT(rv); 

exit:    
    BCMDNX_FUNC_RETURN;
}

int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_ref_count_get(int unit, int eth_type_index_ndx, uint32 *count)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_ref_count_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_eedb_roo_ll_format_eth_type_index, eth_type_index_ndx, count);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
 
int bcm_dpp_am_template_eedb_roo_ll_format_eth_type_index_index_get(int unit,SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO *data, int *index)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_template_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_eedb_roo_ll_format_eth_type_index, data, index);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
} 

/*
 * eth type index - End 
 */



/*
 * RIF profiles
 */
int _bcm_dpp_am_template_out_rif_profile_init(int unit,int template_id,QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *rif_profile)
{
    int is_allocated, rc = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_id;

    /* Add all entities to template init id with given data */
    rc = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_out_rif_profile, flags, rif_profile,_DPP_AM_TEMPLATE_L3_OUT_RIF_PROFILE_MAX_ENTITIES(unit), &is_allocated,&template);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_out_rif_profile_alloc(int unit, QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *rif_profile, int *is_first, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_out_rif_profile, 0/*flags*/, rif_profile, is_first, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_out_rif_profile_get(int unit, int profile_ndx, QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *rif_profile)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_out_rif_profile, profile_ndx, rif_profile);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_out_rif_profile_dealloc(int unit, int profile_ndx, int *is_last)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_out_rif_profile, profile_ndx, is_last);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_out_rif_profile_index_get(int unit, QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *rif_profile, int *profile_ndx)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_template_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_out_rif_profile, rif_profile, profile_ndx);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_out_rif_profile_exchange(int unit, int flags,int outrif_profile_index,QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT *new_profile, int *is_last, int *new_profile_index,int *is_first)
{
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(rc);

    if(!(flags & BCM_DPP_AM_TEMPLATE_FLAG_CHECK)) {
        rc = dpp_am_template_exchange(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_out_rif_profile,SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,new_profile,outrif_profile_index,is_last,new_profile_index,is_first);
    }
    else
    {
        rc = dpp_am_template_exchange_test(unit, BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_out_rif_profile,SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE,new_profile,outrif_profile_index,is_last,new_profile_index,is_first);
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * RIF profiles - end
 */

/*
 * IP tunnel encapsulation mode 
 */
int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_get(int unit, int encapsulation_mode, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO* data) {
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_data_get(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ip_tunnel_encapsulation_mode, encapsulation_mode, data);

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_alloc(int unit, int flags, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO* data, int* is_allocated, int *encapsulation_mode) {
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;     

    rc = dpp_am_template_allocate(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ip_tunnel_encapsulation_mode, flags, data, is_allocated, encapsulation_mode);    
    BCMDNX_IF_ERR_EXIT(rc); 

exit:    
    BCMDNX_FUNC_RETURN;
}

/* check if encapsulation mode template allocation will suceed */
int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_exchange_test(int unit, int old_encapsulation_mode, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO* data, int *old_is_last, int *new_encapsulation_mode,int *is_allocated, uint32 flags) {
   int rc = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;

  rc = dpp_am_template_exchange_test(unit,
                                BCM_DPP_AM_DEFAULT_POOL_IDX,
                                dpp_am_template_ip_tunnel_encapsulation_mode,
                                flags,
                                data,
                                old_encapsulation_mode,
                                old_is_last,
                                new_encapsulation_mode,
                                is_allocated);
  BCMDNX_IF_ERR_EXIT(rc);

  /* allocation of dummy mode is illegal - occurs when out of memory */
  if (*new_encapsulation_mode == _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_DUMMY_TEMPLATE) {
     rc = BCM_E_FULL;
     BCMDNX_IF_ERR_EXIT(rc);
  }


exit:
  BCMDNX_FUNC_RETURN;
}

int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_exchange(int unit, int old_encapsulation_mode, SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO* data, int *old_is_last, int *new_encapsulation_mode,int *is_allocated, uint32 flags) {
   int rc = BCM_E_NONE;

   BCMDNX_INIT_FUNC_DEFS;


  rc = dpp_am_template_exchange(unit, 
                                BCM_DPP_AM_DEFAULT_POOL_IDX,
                                dpp_am_template_ip_tunnel_encapsulation_mode, 
                                flags,
                                data,
                                old_encapsulation_mode, 
                                old_is_last, 
                                new_encapsulation_mode, 
                                is_allocated);
  BCMDNX_IF_ERR_EXIT(rc);



exit:
  BCMDNX_FUNC_RETURN;


}


 int bcm_dpp_am_template_ip_tunnel_encapsulation_mode_free(int unit, int encapsulation_mode, int *is_last) {
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rc = dpp_am_template_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_ip_tunnel_encapsulation_mode, encapsulation_mode, is_last);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}


 /* DESCRIPTION:  init template for packet size compensation feature for CRPS use */
int _bcm_dpp_am_template_crps_pktSize_compensation_init(int unit, int core ,_dpp_am_template_t template_id, int delta, int template)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif

    result = dpp_am_template_allocate_group(unit, core, template_id, flags, &delta, MAX_PORTS_IN_CORE*2, &is_allocated, &template);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}


 /* DESCRIPTION:  init template for packet size compensation feature for CRPS use */
int _bcm_dpp_am_template_stat_interface_pktSize_compensation_init(int unit, int core ,_dpp_am_template_t template_id, int delta, int template)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;

    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
#endif

    result = dpp_am_template_allocate_group(unit, core, template_id, flags, &delta, MAX_PORTS_IN_CORE*2, &is_allocated, &template);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

/* DESCRIPTION:  exchange template for packet size compensation feature for CRPS use */
int _bcm_dpp_am_template_crps_pktSize_compensation_exchange(int unit, int core, _dpp_am_template_t template_id, int port, int data, int old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_exchange(unit, core, template_id, 0, &data, old_template, is_last, template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);
    if(*template < 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("template is negative")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}
/* DESCRIPTION:  exchange template for packet size compensation feature for statistics interface use */
int _bcm_dpp_am_template_stat_interface_pktSize_compensation_exchange(int unit, int core, _dpp_am_template_t template_id, int port, int data, int old_template, int *is_last, int *template,int *is_allocated)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = dpp_am_template_exchange(unit, core, template_id, 0, &data, old_template, is_last, template, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);
    if(*template < 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("template is negative")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Cosq scheduler adjust size (compensation) final delta mapping - Start */
int _bcm_dpp_am_template_scheduler_adjust_size_final_delta_mapping_init(int unit, int template_init_id, int *final_delta)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;
    int core;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    
    /* Add all entities to template init id with given data */
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
        result = dpp_am_template_allocate_group(unit, core, dpp_am_template_scheduler_adjust_size_final_delta, flags, final_delta,_DPP_AM_TEMPLATE_SCHEDULER_ADJUST_SIZE_FINAL_DELTA_MAX_ENTITIES, &is_allocated,&template);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_scheduler_adjust_size_final_delta_mapping_data_get(int unit, int core, int delta , int *final_delta)
{
    int index;
    int rc = BCM_E_NONE;
    int core_index;

    BCMDNX_INIT_FUNC_DEFS;

    core_index = (core == BCM_CORE_ALL) ? 0 : core;
    rc = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_itm_sch_final_delta_map_get,(unit, core_index, delta, (uint32*)&index));
    BCMDNX_IF_ERR_EXIT(rc);

    if (index < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Failed to get the old profile")));
    }

    rc = dpp_am_template_data_get(unit, core_index, dpp_am_template_scheduler_adjust_size_final_delta, index, final_delta);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_am_template_scheduler_adjust_size_final_delta_mapping_exchange(int unit, int core, int delta, int *final_delta, int *old_profile, int *is_last, int *new_profile,int *is_allocated)
{
    int rc = BCM_E_NONE;
    int tmp_old_profile;
    int core_index;
    int tmp_is_last;

    BCMDNX_INIT_FUNC_DEFS;

    core_index = (core == BCM_CORE_ALL) ? 0 : core;
    rc = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_itm_sch_final_delta_map_get,(unit, core_index, delta, (uint32*)&tmp_old_profile));
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dpp_am_template_exchange(unit, core_index, dpp_am_template_scheduler_adjust_size_final_delta, 0, final_delta, tmp_old_profile, &tmp_is_last, new_profile, is_allocated);
    BCMDNX_IF_ERR_EXIT(rc);

    if (old_profile != NULL) {
        *old_profile = tmp_old_profile;
    }
    if (is_last != NULL) {
        *is_last = tmp_is_last;
    }

exit:
    BCMDNX_FUNC_RETURN;
}
/* Cosq scheduler adjust size (compensation) final delta mapping - End */



/*****************************************************************************
* Function:  _bcm_dpp_am_template_lif_mtu_profile_init
* Purpose:   Init all template of LIF MTU with final data
* Params:
* unit     (IN)         - Device number
* template_init_id (IN) - init id of template, to which id all referance point
* mtu_val     (IN)  - the init (uniqe) data of template
* Return:    (int)
 */
int _bcm_dpp_am_template_lif_mtu_profile_init(int unit, uint32 template_init_id, uint32 *mtu_val)
{
    int is_allocated, result = BCM_E_NONE;
    uint32 flags = 0;
    int template;

    BCMDNX_INIT_FUNC_DEFS;
    flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    template = template_init_id;
    #ifdef BCM_WARM_BOOT_SUPPORT
    /* In warmboot we already restored entries, no need to init */
    if (SOC_WARM_BOOT(unit)) BCM_EXIT;
    #endif 
    /* Add all entities to template init id with given data */
    result = dpp_am_template_allocate_group(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_template_lif_mtu_profile, 
                                                flags, mtu_val,
                                                _DPP_AM_TEMPLATE_LIF_MTU_PROFILE_MAX_ENTITIES,
                                                &is_allocated, &template);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

/*****************************************************************************
* Function:  _bcm_dpp_am_template_lif_mtu_profile_exchange
* Purpose:  Exchanhe between old and new profile, at init all profiles were set to profile 0 (since 0 means no MTU fiter)
* Purpose:   Init all template of LIF MTU with final data
* Params:
* unit             - Device number
* mtu_val (IN)      - the init (uniqe) data of template
* old_profile (IN) - Old profile id
* is_last (OUT) - TRUE in case was a last referance to old profile
* new_profile (OUT) - new profile id
* is_allocated (OUT) - was a new template allocated or it already existed
* Return:    (int _bcm_dpp_am_template_lif_mtu_profile_exchange)
 */
int _bcm_dpp_am_template_lif_mtu_profile_exchange(int unit, uint32 *mtu_val,
                                        uint32 *old_profile, int *is_last, uint32 *new_profile,int *is_allocated)
{
    int rv = BCM_E_NONE;
    int flag = 0;
    int template;
    BCMDNX_INIT_FUNC_DEFS;
    /*Get old profile*/
    /*Exchange with new profile*/
    rv = dpp_am_template_exchange(unit,BCM_DPP_AM_DEFAULT_POOL_IDX,dpp_am_template_lif_mtu_profile,
                                flag,mtu_val,*old_profile,is_last,&template,is_allocated);

    *new_profile = template;
    BCMDNX_IF_ERR_EXIT(rv);                            
    
exit:
    BCMDNX_FUNC_RETURN;

}


/* 
 * Template management - End
 */ 

