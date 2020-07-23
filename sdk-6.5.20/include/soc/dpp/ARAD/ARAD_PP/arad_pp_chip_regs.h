/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_CHIP_REGS_INCLUDED__

#define __ARAD_PP_CHIP_REGS_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
 
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/arad_chip_defines.h>
#include <soc/dpp/ARAD_TM/ARD_TM/ard_chip_regs.h>





#define ARAD_PP_BLK_NOF_INSTANCES_ECI                                                                (1)
#define ARAD_PP_BLK_NOF_INSTANCES_OLP                                                                (1)
#define ARAD_PP_BLK_NOF_INSTANCES_IHP                                                                (1)
#define ARAD_PP_BLK_NOF_INSTANCES_IHB                                                                (1)
#define ARAD_PP_BLK_NOF_INSTANCES_EGQ                                                                (1)
#define ARAD_PP_BLK_NOF_INSTANCES_EPNI                                                               (1)

#define ARAD_PP_OLP_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                   (1-0+1)
#define ARAD_PP_IHP_REGS_TRAFFIC_CLASS_L4_RANGE_REG_ARRAY_SIZE                                       (2-0+1)
#define ARAD_PP_IHP_REGS_INCOMING_UP_MAP_REG_ARRAY_SIZE                                              (3-0+1)
#define ARAD_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE                                                 (3-0+1)
#define ARAD_PP_IHP_REGS_GENERAL_TRAP_LL_REG_ARRAY_SIZE                                              (3-2+1)
#define ARAD_PP_IHP_REGS_MPLS_LABEL_RANGE_LOW_REG_ARRAY_SIZE                                         (2-0+1)
#define ARAD_PP_IHP_REGS_MPLS_LABEL_RANGE_HIGH_REG_ARRAY_SIZE                                        (2-0+1)
#define ARAD_PP_IHP_REGS_MPLS_LABEL_VALUE_CONFIGS_REG_ARRAY_SIZE                                     (15-0+1)
#define ARAD_PP_IHB_REGS_TRAP_IF_ACCESSED_REG_ARRAY_SIZE                                             (1-0+1)
#define ARAD_PP_IHB_REGS_PACKET_HEADER_SIZE_RANGE_REG_ARRAY_SIZE                                     (2-0+1)

#define ARAD_PP_IHP_LAST_RECEIVED_HEADER_REG_MULT_NOF_REGS                                           (30)
#define ARAD_PP_ECI_TPID_CONFIGURATION_REGISTER__REG_NOF_REGS                                        (2)
#define ARAD_PP_OLP_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS                                            (2)
#define ARAD_PP_OLP_DSP_HEADER_REG_NOF_REGS                                                          (2)
#define ARAD_PP_OLP_DSP_HEADER_REG_MULT_NOF_REGS                                                     (8)
#define ARAD_PP_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS                                                (6)
#define ARAD_PP_IHP_ISEM_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS                                           (2)
#define ARAD_PP_IHP_LARGE_EM_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS                                       (2)
#define ARAD_PP_IHP_MY_BMAC_UC_MSB_CONFIG_REG_MULT_NOF_REGS                                          (2)
#define ARAD_PP_IHP_MY_BMAC_UC_LSB_BITMAP_REG_MULT_NOF_REGS                                          (8)
#define ARAD_PP_IHP_PCP_DECODING_TABLE_REG_MULT_NOF_REGS                                             (4)
#define ARAD_PP_IHP_L4_PORT_RANGES_2_TC_TABLE_REG_MULT_NOF_REGS                                      (4)
#define ARAD_PP_IHP_DROP_PRECEDENCE_MAP_PCP_REG_MULT_NOF_REGS                                        (2)
#define ARAD_PP_IHP_LARGE_EM_OLP_REQUEST_REQUEST_REG_MULT_NOF_REGS                                   (4)
#define ARAD_PP_IHP_LARGE_EM_EVENT_REG_MULT_NOF_REGS                                                 (4)
#define ARAD_PP_IHP_LARGE_EM_REPLY_REG_MULT_NOF_REGS                                                 (4)
#define ARAD_PP_IHP_VTT_TRAP_ACTION_PROFILES_REG_NOF_REGS                                            (6)
#define ARAD_PP_IHP_MY_MAC_MSB_CONFIG_REG_MULT_NOF_REGS                                              (2)
#define ARAD_PP_IHP_ALL_RBRIDGES_MAC_CONFIG_REG_MULT_NOF_REGS                                        (2)
#define ARAD_PP_IHB_ETHERNET_ACTION_PROFILES_REG_NOF_REGS                                            (2)
#define ARAD_PP_IHB_IPV4_ACTION_PROFILES_REG_NOF_REGS                                                (2)
#define ARAD_PP_IHB_IPV6_ACTION_PROFILES_REG_NOF_REGS                                                (3)
#define ARAD_PP_IHB_CPU_LOOKUP_KEY_MSB_REG_REG_MULT_NOF_REGS                                         (3)
#define ARAD_PP_IHB_ALL_ESADI_RBRIDGES_REG_MULT_NOF_REGS                                             (2)
#define ARAD_PP_IHB_MEF_L2CP_PEER_BITMAP_REG_MULT_NOF_REGS                                           (4)
#define ARAD_PP_IHB_MEF_L2CP_DROP_BITMAP_REG_MULT_NOF_REGS                                           (4)
#define ARAD_PP_IHP_MEF_L2CP_TRANSPARENT_BITMAP_REG_MULT_NOF_REGS                                    (4)
#define ARAD_PP_IHB_QUERY_KEY_REG_MULT_NOF_REGS                                                      (2)
#define ARAD_PP_IHB_PMF_CPU_TRAP_CODE_PROFILE_REG_MULT_NOF_REGS                                      (8)
#define ARAD_PP_EGQ_ETHERNET_TYPES_REG_REG_NOF_REGS                                                  (5)
#define ARAD_PP_EGQ_ACTION_PROFILE_REG_NOF_REGS                                                      (3)
#define ARAD_PP_EGQ_L4_PROTOCOL_CODE_REG_REG_NOF_REGS                                                (3)
#define ARAD_PP_EGQ_OUTLIF_ERROR_CODE_REG_MULT_NOF_REGS                                              (3)
#define ARAD_PP_EGQ_ACCEPTABLE_FRAME_TYPE_TABLE_REG_MULT_NOF_REGS                                    (4)
#define ARAD_PP_EPNI_MIRROR_VID_REG_REG_NOF_REGS                                                     (4)
#define ARAD_PP_EPNI_COUNTER_BASE_REG_REG_NOF_REGS                                                   (2)
#define ARAD_PP_EPNI_COUNTER_RANGE_LOW_REG_REG_NOF_REGS                                              (2)
#define ARAD_PP_EPNI_COUNTER_RANGE_HIGH_REG_REG_NOF_REGS                                             (2)
#define ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_REG_MULT_NOF_REGS                                 (28)
#define ARAD_PP_EPNI_IPV4_EXP_TO_TOS_MAP_REG_MULT_NOF_REGS                                           (8)
#define ARAD_PP_EPNI_IPV6_EXP_TO_TC_MAP_REG_MULT_NOF_REGS                                            (8)
#define ARAD_PP_EPNI_MPLS_PROFILE_REG_MULT_NOF_REGS                                                  (4)
#define ARAD_PP_EPNI_IPV4_TOS_REG_MULT_NOF_REGS                                                      (4)
#define ARAD_PP_EPNI_IPV4_TTL_REG_MULT_NOF_REGS                                                      (4)
#define ARAD_PP_EPNI_IPV4_SIP_REG_MULT_NOF_REGS                                                      (16)
#define ARAD_PP_EPNI_SA_MSB_REG_MULT_NOF_REGS                                                        (2)
#define ARAD_PP_EPNI_BSA_REG_MULT_NOF_REGS                                                           (2)
#define ARAD_PP_EPNI_ITAG_TC_DP_MAP_REG_MULT_NOF_REGS                                                (4)
#define ARAD_PP_EPNI_SEM_OPCODE_OFFSET_REG_MULT_NOF_REGS                                             (6)

#define ARAD_PP_IHP_ACTION_PROFILE_ACCEPTABLE_FRAME_TYPE_NOF_FLDS                                    (4)
#define ARAD_PP_IHP_SEM_RESULT_OPCODES_NOF_FLDS                                                      (4)
#define ARAD_PP_IHP_TRAFFIC_CLASS_L4_RANGE_CFG_REG_NOF_FLDS                                          (3)
#define ARAD_PP_NOF_DA_NOT_FOUND_ACTION_PROFILES                                                     (4)

#define ARAD_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS            (3)
#define ARAD_PP_OLP_DSP_EVENT_NOF_ENTRIES_MAX_PER_REG       (SOC_SAND_REG_SIZE_BITS / ARAD_PP_OLP_DSP_EVENT_ENTRY_SIZE_IN_BITS + 1)

#define ARAD_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS   (4)
#define ARAD_PP_IHP_LARGE_EM_AGE_AGING_NOF_MODES            (SOC_SAND_REG_SIZE_BITS / ARAD_PP_IHP_LARGE_EM_AGE_AGING_ENTRY_SIZE_IN_BITS)

#define ARAD_PP_IHP_LARGE_EM_LOOKUP_MODE_ENTRY_SIZE_IN_BITS (5)
#define ARAD_PP_IHP_LARGE_EM_LOOKUP_NOF_MODES               (4)

#define ARAD_PP_OLP_REGS_DSP_ENGINE_CONFIGURATION_REG_ARRAY_SIZE                                        (1-0+1)
#define ARAD_PP_IHP_REGS_TRAFFIC_CLASS_L4_RANGE_REG_ARRAY_SIZE                                          (2-0+1)
#define ARAD_PP_IHP_REGS_INCOMING_UP_MAP_REG_ARRAY_SIZE                                                 (3-0+1)
#define ARAD_PP_IHP_REGS_GENERAL_TRAP_REG_ARRAY_SIZE                                                    (3-0+1)
#define ARAD_PP_IHP_REGS_MPLS_LABEL_RANGE_LOW_REG_ARRAY_SIZE                                            (2-0+1)
#define ARAD_PP_IHP_REGS_MPLS_LABEL_RANGE_HIGH_REG_ARRAY_SIZE                                           (2-0+1)
#define ARAD_PP_IHP_REGS_MPLS_RANGE_CONFIGS_REG_ARRAY_SIZE                                              (2-0+1)
#define ARAD_PP_IHP_REGS_MPLS_LABEL_VALUE_CONFIGS_REG_ARRAY_SIZE                                        (15-0+1)
#define ARAD_PP_IHB_REGS_TRAP_IF_ACCESSED_REG_ARRAY_SIZE                                                (1-0+1)
#define ARAD_PP_IHB_REGS_PACKET_HEADER_SIZE_RANGE_REG_ARRAY_SIZE                                        (2-0+1)
#define ARAD_PP_IHB_REGS_DBG_FEMKEY_REG_ARRAY_SIZE                                                      (7-0+1)
#define ARAD_PP_IHB_REGS_DBG_FEM_REG_ARRAY_SIZE                                                         (7-0+1)
#define ARAD_PP_IHP_DBG_LLR_TRAP_REG_NOF_REGS                                                           2
#define ARAD_PP_IHP_VTT_TRAP_STRENGTHS_REG_NOF_REGS                                                     12
#define ARAD_PP_IHP_VTT_TRAP_ENCOUNTERED_REG_NOF_REGS                                                   1
#define ARAD_PP_IHP_VRID_MY_MAC_CONFIG_REG_MULT_NOF_REGS                                                2
#define ARAD_PP_IHB_DBG_FLP_TRAP_REG_NOF_REGS                                                           3
#define ARAD_PP_IHB_CPU_LOOKUP_KEY_REG_MULT_NOF_REGS                                                    2
#define ARAD_PP_IHB_LOOKUP_REPLY_DATA_REG_MULT_NOF_REGS                                                 2
#define ARAD_PP_IHB_ICMP_REDIRECT_ENABLE_REG_MULT_NOF_REGS                                              2
#define ARAD_PP_IHB_DBG_ETH_KEY_REG_MULT_NOF_REGS                                                       5
#define ARAD_PP_IHB_DBG_IPV4_KEY_REG_MULT_NOF_REGS                                                      5
#define ARAD_PP_IHB_DBG_IPV6_KEY_REG_MULT_NOF_REGS                                                      9
#define ARAD_PP_EGQ_TTL_SCOPE_REG_MULT_NOF_REGS                                                         2
#define ARAD_PP_EPNI_MPLS_EXP_MAP_TABLE_REG_MULT_NOF_REGS                                               2
#define ARAD_PP_EPNI_IP_DSCP_TO_EXP_MAP_REG_MULT_NOF_REGS                                               6
#define ARAD_PP_EPNI_ACCEPTABLE_FRAME_TYPE_REG_MULT_NOF_REGS                                            2
#define PP_ARAD_NOF_IPV4_EXP_TO_TOS_MAP_REGS                                                            8
#define PP_ARAD_NOF_IPV6_EXP_TO_TC_MAP_REGS                                                             8
#define PP_ARAD_NOF_EGQ_L4_PROTOCOL_CODE_REGS                                                           (4)

#define ARAD_PP_REGS_TCAM_NOF_BANKS   4
#define ARAD_PP_REGS_TCAM_NOF_CYCLES  2

#define ARAD_PP_IHP_PMF_CPU_TRAP_CODE_PRO_NOF_REGS       (256/32)

#define ARAD_PP_IHP_PMF_PKT_HDR_SZ_RNG_NOF_REGS          (3)
#define ARAD_PP_EGQ_ETHERNET_TYPES_NOF_REGS              (4)
#define ARAD_PP_EGQ_ETHERNET_TYPES_NOF_FLDS              (2)

#define PP_ARAD_NOF_EGQ_L4_PROTOCOL_CODE_FLDS            (4)





#define ARAD_PP_EGQ_ACCEPTABLE_FRM_TYPE_ACC(port_profile,out_tag_encap,out_is_prio,in_tag_encap)  \
  ((((out_tag_encap<<3)+(out_is_prio<<2)+in_tag_encap)<<2)|port_profile)















uint32
  arad_pp_regs_get(
    SOC_SAND_OUT ARAD_PP_REGS  **arad_pp_regs
  );

uint32
  arad_pp_regs_init(void);

ARAD_PP_REGS*
  arad_pp_regs();



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

