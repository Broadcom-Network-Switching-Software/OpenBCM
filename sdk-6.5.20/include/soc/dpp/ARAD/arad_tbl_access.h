/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
 */


#ifndef __ARAD_TBL_ACCESS_H_INCLUDED__

#define __ARAD_TBL_ACCESS_H_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_api_cnt.h>
#include <soc/dpp/ARAD/arad_api_flow_control.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>





#define ARAD_OLP_PGE_MEM_TBL_ENTRY_SIZE                                                 1
#define ARAD_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE                                            1
#define ARAD_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_ENTRY_SIZE                                   4
#define ARAD_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE                                            1
#define ARAD_IRE_PACKET_COUNTERS_TBL_ENTRY_SIZE                                         2
#define ARAD_IDR_COMPLETE_PC_TBL_ENTRY_SIZE                                             1
#define ARAD_IRR_SNOOP_TABLE_TBL_ENTRY_SIZE                                             1
#define ARAD_IRR_GLAG_TO_LAG_RANGE_TBL_ENTRY_SIZE                                       1
#define ARAD_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE                                         1
#define ARAD_IRR_GLAG_MAPPING_TBL_ENTRY_SIZE                                            1
#define ARAD_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE                                       1
#define ARAD_IRR_GLAG_NEXT_MEMBER_TBL_ENTRY_SIZE                                        1
#define ARAD_IRR_RLAG_NEXT_MEMBER_TBL_ENTRY_SIZE                                        1
#define ARAD_IHP_PORT_INFO_TBL_ENTRY_SIZE                                               2
#define ARAD_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_ENTRY_SIZE                                  1
#define ARAD_IHP_STATIC_HEADER_TBL_ENTRY_SIZE                                           3
#define ARAD_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_ENTRY_SIZE                               1
#define ARAD_IHP_PTC_COMMANDS1_TBL_ENTRY_SIZE                                           2
#define ARAD_IHP_PTC_COMMANDS2_TBL_ENTRY_SIZE                                           2
#define ARAD_IHP_PTC_KEY_PROGRAM_LUT_TBL_ENTRY_SIZE                                     3

#define ARAD_IHP_KEY_PROGRAM_TBL_ENTRY_SIZE                                             2
#define ARAD_IHP_KEY_PROGRAM0_TBL_ENTRY_SIZE                                            1
#define ARAD_IHP_KEY_PROGRAM1_TBL_ENTRY_SIZE                                            1
#define ARAD_IHP_KEY_PROGRAM2_TBL_ENTRY_SIZE                                            1
#define ARAD_IHP_KEY_PROGRAM3_TBL_ENTRY_SIZE                                            1
#define ARAD_IHP_KEY_PROGRAM4_TBL_ENTRY_SIZE                                            2
#define ARAD_IHP_PROGRAMMABLE_COS_TBL_ENTRY_SIZE                                        3
#define ARAD_IHP_PROGRAMMABLE_COS1_TBL_ENTRY_SIZE                                       3


#define ARAD_IQM_CNRED_PQ_MAX_QUE_SIZE_MANTISSA                                         6
#define ARAD_IQM_CNRED_PQ_MAX_QUE_BUFF_SIZE_MANTISSA                                    7
#define ARAD_IQM_PQRED_PQ_MAX_QUE_SIZE_MANTISSA                                         6
#define ARAD_IQM_PQRED_PQ_MAX_QUE_BUFF_SIZE_MANTISSA                                    7
#define JERICHO_IQM_CNRED_PQ_MAX_QUE_SIZE_MANTISSA                                      8
#define JERICHO_IQM_CNRED_PQ_MAX_QUE_BUFF_SIZE_MANTISSA                                 8
#define JERICHO_IQM_PQRED_PQ_MAX_QUE_SIZE_MANTISSA                                      8
#define JERICHO_IQM_PQRED_PQ_MAX_QUE_BUFF_SIZE_MANTISSA                                 8

#define ARAD_IQM_BDB_LINK_LIST_TBL_ENTRY_SIZE                                           1
#define ARAD_IQM_DYNAMIC_TBL_ENTRY_SIZE                                                 3
#define ARAD_IQM_STATIC_TBL_ENTRY_SIZE                                                  1
#define ARAD_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_ENTRY_SIZE                               1
#define ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE                           1
#define ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE                                   1
#define ARAD_IQM_FULL_USER_COUNT_MEMORY_TBL_ENTRY_SIZE                                  1
#define ARAD_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_ENTRY_SIZE                        2
#define ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE                       4
#define ARAD_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE                          1
#define ARAD_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_ENTRY_SIZE                              3
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE                       1
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE                       1
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE                       1
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE                       1
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE                                2
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE                                2
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE                                2
#define ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE                                2
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_ENTRY_SIZE                        1
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_ENTRY_SIZE                        1
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_ENTRY_SIZE                        1
#define ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_ENTRY_SIZE                        1
#define ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE                 2
#define ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE                             1
#define ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE                      1
#define ARAD_IQM_SYSTEM_RED_TBL_ENTRY_SIZE                                              6
#define ARAD_QDR_MEM_TBL_ENTRY_SIZE                                                     1
#define ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE                       2
#define ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE                2
#define ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE                                    1
#define ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE                                 1
#define ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE                               1
#define ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE                               2
#define ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                       2
#define ARAD_IPS_SLOW_FACTOR_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                            6
#define ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                   2
#define ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE                        2
#define ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                        1
#define ARAD_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ENTRY_SIZE                                  1
#define ARAD_IPS_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE                                        1
#define ARAD_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ENTRY_SIZE                         1
#define ARAD_IPT_BDQ_TBL_ENTRY_SIZE                                                     1
#define ARAD_IPT_SELECT_SOURCE_SUM_TBL_ENTRY_SIZE                                       2
#define ARAD_IPT_PCQ_TBL_ENTRY_SIZE                                                     2
#define ARAD_IPT_SOP_MMU_TBL_ENTRY_SIZE                                                 2
#define ARAD_IPT_MOP_MMU_TBL_ENTRY_SIZE                                                 1
#define ARAD_IPT_FDTCTL_TBL_ENTRY_SIZE                                                  1
#define ARAD_IPT_FDTDATA_TBL_ENTRY_SIZE                                                 16
#define ARAD_IPT_EGQCTL_TBL_ENTRY_SIZE                                                  1
#define ARAD_IPT_EGQDATA_TBL_ENTRY_SIZE                                                 16
#define ARAD_DPI_DLL_RAM_TBL_ENTRY_SIZE                                                 2
#define ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_ENTRY_SIZE              3
#define ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_ENTRY_SIZE           3
#define ARAD_EGQ_NIF_SCM_TBL_ENTRY_SIZE                                                 2
#define ARAD_EGQ_NIFA_CH0_SCM_TBL_ENTRY_SIZE                                            1
#define ARAD_EGQ_NIFA_CH1_SCM_TBL_ENTRY_SIZE                                            1
#define ARAD_EGQ_NIFA_CH2_SCM_TBL_ENTRY_SIZE                                            1
#define ARAD_EGQ_NIFA_CH3_SCM_TBL_ENTRY_SIZE                                            1
#define ARAD_EGQ_NIFB_CH0_SCM_TBL_ENTRY_SIZE                                            1
#define ARAD_EGQ_NIFB_CH1_SCM_TBL_ENTRY_SIZE                                            1
#define ARAD_EGQ_NIFB_CH2_SCM_TBL_ENTRY_SIZE                                            1
#define ARAD_EGQ_NIFB_CH3_SCM_TBL_ENTRY_SIZE                                            1
#define ARAD_EGQ_NIFAB_NCH_SCM_TBL_ENTRY_SIZE                                           1
#define ARAD_EGQ_RCY_SCM_TBL_ENTRY_SIZE                                                 1
#define ARAD_EGQ_CPU_SCM_TBL_ENTRY_SIZE                                                 1
#define ARAD_EGQ_CCM_TBL_ENTRY_SIZE                                                     1
#define ARAD_EGQ_PMC_TBL_ENTRY_SIZE                                                     1
#define ARAD_EGQ_CBM_TBL_ENTRY_SIZE                                                     1
#define ARAD_EGQ_FBM_TBL_ENTRY_SIZE                                                     1
#define ARAD_EGQ_FDM_TBL_ENTRY_SIZE                                                     2
#define ARAD_EGQ_DWM_TBL_ENTRY_SIZE                                                     1
#define ARAD_EGQ_RRDM_TBL_ENTRY_SIZE                                                    2
#define ARAD_EGQ_RPDM_TBL_ENTRY_SIZE                                                    2
#define ARAD_EGQ_PCT_TBL_ENTRY_SIZE                                                     3
#define ARAD_EGQ_PER_PORT_LB_RANGE_TBL_ENTRY_SIZE                                       1
#define ARAD_EGQ_CFC_FLOW_CONTROL_TBL_ENTRY_SIZE                                        3
#define ARAD_EGQ_NIFA_FLOW_CONTROL_TBL_ENTRY_SIZE                                       1
#define ARAD_EGQ_NIFB_FLOW_CONTROL_TBL_ENTRY_SIZE                                       1
#define ARAD_EGQ_CPU_LAST_HEADER_TBL_ENTRY_SIZE                                         2
#define ARAD_EGQ_IPT_LAST_HEADER_TBL_ENTRY_SIZE                                         2
#define ARAD_EGQ_FDR_LAST_HEADER_TBL_ENTRY_SIZE                                         2
#define ARAD_EGQ_CPU_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define ARAD_EGQ_IPT_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define ARAD_EGQ_FDR_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define ARAD_EGQ_RQP_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define ARAD_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE                              2
#define ARAD_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE                              2
#define ARAD_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE                       2
#define ARAD_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE                        2
#define ARAD_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_ENTRY_SIZE                              2
#define ARAD_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE                         2
#define ARAD_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE                          2
#define ARAD_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_ENTRY_SIZE                       2
#define ARAD_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_ENTRY_SIZE                        2
#define ARAD_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE                          2
#define ARAD_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE                           2
#define ARAD_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_ENTRY_SIZE                        2
#define ARAD_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_ENTRY_SIZE                         2
#define ARAD_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_ENTRY_SIZE                      2
#define ARAD_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_ENTRY_SIZE                    2
#define ARAD_EGQ_FQP_PACKET_COUNTER_TBL_ENTRY_SIZE                                      2
#define ARAD_EGQ_QDCT_TBL_ENTRY_SIZE                                                    5 
#define ARAD_EGQ_QQST_TBL_ENTRY_SIZE                                                    5 
#define ARAD_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_ENTRY_SIZE                   1
#define ARAD_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE                        1
#define ARAD_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL_ENTRY_SIZE                        1
#define ARAD_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE          1
#define ARAD_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_ENTRY_SIZE          1
#define ARAD_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL_ENTRY_SIZE                       1
#define ARAD_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL_ENTRY_SIZE                       1
#define ARAD_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_ENTRY_SIZE                         1
#define ARAD_CFC_HCFC_BITMAP_TBL_SIZE                                                   8
#define ARAD_SCH_CAL_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_PORT_NOF_PORTS_PER_ENPORT_TBL_LINE                                    (8)
#define ARAD_SCH_PORT_NOF_PORTS_PER_FORCE_FC_REG_LINE                                  (32)
#define ARAD_SCH_DRM_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_DSM_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_FDMS_TBL_ENTRY_SIZE                                                    2
#define ARAD_SCH_SHDS_TBL_ENTRY_SIZE                                                    2
#define JERICHO_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_ENTRY_SIZE                          1
#define ARAD_SCH_SEM_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_FSF_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_FGM_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_SHC_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_SCC_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_SCT_TBL_ENTRY_SIZE                                                     2
#define ARAD_SCH_FQM_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_FFM_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_TMC_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_PQS_TBL_ENTRY_SIZE                                                     1
#define ARAD_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE                                          1
#define ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_ENTRY_SIZE                                    1
#define ARAD_MMU_DRAM_ADDRESS_SPACE_TBL_ENTRY_SIZE                                      16
#define ARAD_MMU_IDF_TBL_ENTRY_SIZE                                                     1
#define ARAD_MMU_FDF_TBL_ENTRY_SIZE                                                     1
#define ARAD_MMU_RDFA_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RDFB_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RDFC_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RDFD_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RDFE_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RDFF_WADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RDF_RADDR_TBL_ENTRY_SIZE                                               1
#define ARAD_MMU_WAF_HALFA_WADDR_TBL_ENTRY_SIZE                                         1
#define ARAD_MMU_WAF_HALFB_WADDR_TBL_ENTRY_SIZE                                         1
#define ARAD_MMU_WAFA_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFB_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFC_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFD_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFE_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFF_HALFA_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFA_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFB_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFC_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFD_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFE_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_WAFF_HALFB_RADDR_STATUS_TBL_ENTRY_SIZE                                 1
#define ARAD_MMU_RAF_WADDR_TBL_ENTRY_SIZE                                               1
#define ARAD_MMU_RAFA_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RAFB_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RAFC_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RAFD_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RAFE_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_MMU_RAFF_RADDR_STATUS_TBL_ENTRY_SIZE                                       1
#define ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE                            ( 4)
#define ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE                            ( 5)
#define ARAD_PP_EG_ENCAP_ACCESS_FORMAT_TBL_ENTRY_SIZE                                   ( 3)
#define ARAD_NUM_OF_INDIRECT_MODULES  21
#define ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE                                  2
#define ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE 								6

#define ARAD_BLK_INVALID_ID ARAD_NUM_OF_INDIRECT_MODULES

#define ARAD_IRR_GLAG_DEVISION_HASH_NOF_BITS  8

#define ARAD_SYS_RED_NOF_Q_RNGS                                                         16
#define ARAD_SYS_RED_NOF_Q_RNGS_THS                                                     (ARAD_SYS_RED_NOF_Q_RNGS-1)

#define ARAD_IPS_NOF_QUEUE_PRIORITY_MAPS_TABLES 2

#define ARAD_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL 8
#define ARAD_IHP_KEY_NOF_PROGS                   5
#define ARAD_IHP_KEY_PROG_NOF_INSTR              2
#define ARAD_IHP_KEY_PROG_INSTR_SIZE             13
#define ARAD_IHP_SYS_MY_PORT_TBL_FLDS            4
#define ARAD_IHP_PRGR_COS_TBL_FLDS               16
#define ARAD_NOF_CFC_NIF_CLS2OFP_MAP_FLDS        2
#define ARAD_TBL_FGM_NOF_GROUPS_ONE_LINE         8


#define ARAD_IHB_FEM_MAX_OUTPUT_SIZE  SOC_DPP_DEFS_MAX(FEM_MAX_ACTION_SIZE_NOF_BITS)

#define ARAD_IRR_GLAG_DEVISION_HASH_NOF_BITS  8
#define ARAD_CNM_CP_SAMPLE_BASE_SIZE                    (8)

#define ARAD_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS         (2)

#define ARAD_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS        (3)

#define ARAD_CHIP_REGS_NOF_CNT_PROCESSOR_IDS            (2)


#define ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_LSB                              (3)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MSB                              (5)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_SHIFT                            (ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_LSB)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MASK                             (SOC_SAND_BITS_MASK(ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MSB, ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_LSB))
#define ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MIN                              (0)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MAX                              (ARAD_VSI_DEFAULT_FRWRD_KEY_DA_TYPE_MASK)

#define ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_LSB                          (2)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MSB                          (2)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_SHIFT                        (ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_LSB)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MASK                         (SOC_SAND_BITS_MASK(ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MSB, ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_LSB))
#define ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MIN                          (0)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MAX                          (ARAD_VSI_DEFAULT_FRWRD_KEY_ORIENTATION_MASK)

#define ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_LSB                      (0)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MSB                      (1)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_SHIFT                    (ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_LSB)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MASK                     (SOC_SAND_BITS_MASK(ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MSB, ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_LSB))
#define ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MIN                      (0)
#define ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MAX                      (ARAD_VSI_DEFAULT_FRWRD_KEY_FORWARD_PROFILE_MASK)

#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_LSB                      (0)
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MSB                      (7)
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_SHIFT                    (ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_LSB)
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MASK                     (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MSB, ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_LSB))
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MIN                      (0)
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MAX                      (ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MASK)

#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_LSB                  (8)
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MSB                  (9)
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_SHIFT                (ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_LSB)
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MASK                 (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MSB, ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_LSB))
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MIN                  (0)
#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MAX                  (ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MASK)

#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_LSB                    (0)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MSB                    (1)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_SHIFT                  (ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_LSB)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MASK                   (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MSB, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_LSB))
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MIN                    (0)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MAX                    (ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MASK)

#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_LSB                    (2)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MSB                    (4)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_SHIFT                  (ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_LSB)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MASK                   (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MSB, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_LSB))
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MIN                    (0)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MAX                    (ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MASK)

#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_LSB               (5)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MSB               (6)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_SHIFT             (ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_LSB)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MASK              (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MSB, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_LSB))
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MIN               (0)
#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MAX               (ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MASK)

#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_LSB                 (0)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MSB                 (0)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_SHIFT               (ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_LSB)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MASK                (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MSB, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_LSB))
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MIN                 (0)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MAX                 (ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MASK)

#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_LSB                 (1)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MSB                 (3)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_SHIFT               (ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_LSB)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MASK                (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MSB, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_LSB))
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MIN                 (0)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MAX                 (ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MASK)

#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_LSB           (4)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MSB           (5)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_SHIFT         (ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_LSB)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MASK          (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MSB, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_LSB))
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MIN           (0)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MAX           (ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MASK)

#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_LSB             (6)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MSB             (7)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_SHIFT           (ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_LSB)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MASK            (SOC_SAND_BITS_MASK(ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MSB, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_LSB))
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MIN             (0)
#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MAX             (ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MASK)

#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_LSB     (0)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MSB     (2)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_SHIFT   (ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_LSB)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MASK    (SOC_SAND_BITS_MASK(ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MSB, ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_LSB))
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MIN     (0)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MAX     (ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MASK)

#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_LSB       (3)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MSB       (7)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_SHIFT     (ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_LSB)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MASK      (SOC_SAND_BITS_MASK(ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MSB, ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_LSB))
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MIN       (0)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MAX       (ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUALIFIER_MASK)

#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_LSB      (8)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MSB      (9)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_SHIFT    (ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_LSB)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MASK     (SOC_SAND_BITS_MASK(ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MSB, ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_LSB))
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MIN      (0)
#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MAX      (ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MASK)

#define ARAD_IHP_ID                                                         (ARAD_IHP_ID)
#define ARAD_IHB_ID                                                         (ARAD_IHB_ID)
#define ARAD_EGQ_ID                                                         (ARAD_EGQ_ID)
#define ARAD_EPNI_ID                                                        (ARAD_EPNI_ID)


#define ARAD_MAX_NOF_REPS                                                   (0xfffffff) 
#define ARAD_FIRST_TBL_ENTRY                                                (0)
#define ARAD_SINGLE_OPERATION                                               (0)


#define ARAD_PP_IHB_TCAM_DATA_WIDTH 5

#define ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS    (4)

#define ARAD_PP_IHP_VSI_DA_NOT_FOUND_TBL_NOF_PROFILE_INDEX      (4)
#define ARAD_PP_IHP_BVD_FID_CLASS_TBL_NOF_FID_CLASS             (4)
#define ARAD_PP_IHP_BVD_DA_NOT_FOUND_TBL_NOF_PROFILE_INDEX      (4)

#define ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_ACTION_SELECT_KAPS    (4)
             
#define ARAD_PP_IHB_PROGRAM_INSTRUCTION_TABLE_NOF_TBLS          (4)
#define ARAD_PP_EPNI_SEM_RESULT_NOF_VIDS                        (2)
             
#define ARAD_PP_CHIP_FRWRD_MACT_ENTRY_NOF_IS_OWNED              (2)

#define ARAD_PORTS_FEM_PARSER_PROGRAM_POINTER_SIZE             (4)
#define ARAD_PORTS_FEM_PFQ_0_SIZE                              (3)
#define ARAD_PORTS_FEM_SYSTEM_PORT_SIZE                        (16)

#define ARAD_IHB_TCAM_DATA_WIDTH 5



#define ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx)   ((bank_ndx < SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks))? SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines):SOC_PPC_TCAM_NOF_LINES_ARAD_SMALL)


#define ARAD_SYS_RED_NOF_Q_RNGS          16


#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IPV4UC_BIT      0
#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IPV6UC_BIT      1
#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IPV4MC_BIT      2
#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IPV6MC_BIT      3
#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_MPLS_BIT        4

#define ARAD_PP_RIF_RIF_PROFILE_STRUCTURE_URPF_ENABLE_BIT         5
#define ARAD_PP_RIF_RIF_PROFILE_STRUCTURE_DEFAULT_ROUTING_BIT     4
#define ARAD_PP_RIF_RIF_PROFILE_STRUCTURE_CUSTOM_RIF_PROFILE_BIT  3
#define ARAD_PP_RIF_RIF_PROFILE_STRUCTURE_LSB_RIF_PROFILE_BIT 1

#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV4UC_ENABLED(enablers_bm)        (enablers_bm>>ARAD_PP_RIF_ROUTING_ENABLERS_BM_IPV4UC_BIT & 1)
#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV4MC_ENABLED(enablers_bm)        (enablers_bm>>ARAD_PP_RIF_ROUTING_ENABLERS_BM_IPV4MC_BIT & 1)
#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV6UC_ENABLED(enablers_bm)        (enablers_bm>>ARAD_PP_RIF_ROUTING_ENABLERS_BM_IPV6UC_BIT & 1)
#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_IPV6MC_ENABLED(enablers_bm)        (enablers_bm>>ARAD_PP_RIF_ROUTING_ENABLERS_BM_IPV6MC_BIT & 1)
#define ARAD_PP_RIF_ROUTING_ENABLERS_BM_IS_MPLS_ENABLED(enablers_bm)          (enablers_bm>>ARAD_PP_RIF_ROUTING_ENABLERS_BM_MPLS_BIT & 1)

#define QAX_SCH_FSF_OFFSET (2*1024)
#define QAX_SCH_SHDS_OFFSET (32*1024)
#define QAX_SCH_SCC_OFFSET (8*1024)
#define QAX_SCH_FQM_OFFSET (16*1024)
#define QAX_SCH_FFM_OFFSET (8*1024)
#define QAX_SCH_FDMS_OFFSET (64*1024)

#define QUX_SCH_FSF_OFFSET (3*1024)
#define QUX_SCH_SHDS_OFFSET (48*1024)
#define QUX_SCH_SCC_OFFSET (12*1024)
#define QUX_SCH_FQM_OFFSET (24*1024)
#define QUX_SCH_FDMS_OFFSET (96*1024)


#define ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE 5
#define ARAD_PP_FEC_ENTRY_FORMAT_BY_CHIP(_unit, _name)   \
        ((SOC_IS_JERICHO(unit) && (!SOC_IS_QAX(unit))) ?  PPDB_A_##_name##m : IHB_##_name##m)








#define ARAD_PP_TBL_IHP_LLVP_KEY_ENTRY_OFFSET(port_profile, out_tag_encap, out_is_prio, in_tag_encap)   \
  ((port_profile<<5)+(out_tag_encap<<3)+(out_is_prio<<2)+in_tag_encap)


#define ARAD_PP_TBL_EGQ_ACC_FRM_ENTRY_OFFSET(port_profile, out_tag_encap, out_is_prio, in_tag_encap)   \
  ((((out_tag_encap<<3)+(out_is_prio<<2)+in_tag_encap)<<2) + port_profile)


#define ARAD_TBL_EPNI_LLVP_KEY_ENTRY_OFFSET(port_profile, out_tag_encap, out_is_prio, in_tag_encap)   \
    ((port_profile<<5)+(out_tag_encap<<3)+(out_is_prio<<2)+in_tag_encap)



#define ARAD_PP_TBL_IHP_VTT_LLVP_ENTRY_OFFSET(entry_offset,leaf_context)  \
  ((leaf_context<<8)+(entry_offset))


#define ARAD_TBL_IHP_RESERVED_MC_KEY_ENTRY_OFFSET(reserved_mc_profile, da_mac_address_lsb) \
          ((reserved_mc_profile<<6)+da_mac_address_lsb)


#define ARAD_TBL_IHP_LL_MIRROR_PROFILE_KEY_ENTRY_OFFSET(vid, port) \
          ((vid<<8)+port)


#define ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_KEY_ENTRY_OFFSET(is_l2cp, tag_struct_ndx, edit_profile) \
          ((SOC_SAND_BOOL2NUM(is_l2cp)<<8)+(tag_struct_ndx<<3)+edit_profile)

#define ARAD_IHP_SEM_OPCODE_IP_OFFSETS_KEY_ENTRY_OFFSET(op_code, tos)       \
          SOC_SAND_SET_FLD_IN_PLACE(op_code, ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_SHIFT, ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_OP_CODE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(tos, ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_SHIFT, ARAD_IHP_SEM_OPCODE_IP_OFFSETS_TBL_KEY_TOS_MASK)

#define ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_KEY_ENTRY_OFFSET(op_code, tc, dp)       \
          SOC_SAND_SET_FLD_IN_PLACE(op_code, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_SHIFT, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_OP_CODE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(tc, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_SHIFT, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_TC_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(dp, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_SHIFT, ARAD_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_KEY_DP_MASK)

#define ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_KEY_ENTRY_OFFSET(op_code, outer_tag, pcp, dei)       \
          SOC_SAND_SET_FLD_IN_PLACE(op_code, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_SHIFT, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OP_CODE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(outer_tag, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_SHIFT, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_OUTER_TAG_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(pcp, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_SHIFT, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_PCP_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(dei, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_SHIFT, ARAD_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_KEY_DEI_MASK)

#define ARAD_IHP_LLVP_PROG_SEL_OFFSETS_KEY_ENTRY_OFFSET(key_program_profile, packet_format_qual, small_em_key_profile) \
          SOC_SAND_SET_FLD_IN_PLACE(key_program_profile, ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_SHIFT, ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_KEY_PROGRAM_PROFILE_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE((packet_format_qual.outer_tpid << 3) | (packet_format_qual.is_outer_prio << 2) | (packet_format_qual.inner_tpid << 0), ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_SHIFT, ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_PACKET_FORMAT_QUAL_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(small_em_key_profile, ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_SHIFT, ARAD_IHP_LLVP_PROG_SEL_OFFSETS_TBL_KEY_SMALL_EM_KEY_PROFILE_MASK)


#define ARAD_PCP_DEI_MAP_KEY_STAG(pcp, dei) \
  ((0x3<<4) + (pcp<<1) + dei)
#define ARAD_PCP_DEI_MAP_KEY_CTAG(up) \
  ((0x2<<4) + (up<<1))
#define ARAD_PCP_DEI_MAP_KEY_UNTAGGED(tc, dp) \
  ((tc<<2) + dp)

#define ARAD_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(profile_ndx, pcp, dei) \
  ((profile_ndx<<6)+  ARAD_PCP_DEI_MAP_KEY_STAG(pcp, dei))
#define ARAD_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(profile_ndx, up) \
  ((profile_ndx<<6)+  ARAD_PCP_DEI_MAP_KEY_CTAG(up))
#define ARAD_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(profile_ndx, tc, dp) \
  ((profile_ndx<<6)+  ARAD_PCP_DEI_MAP_KEY_UNTAGGED(tc, dp))


#define ARAD_EPNI_PCP_DEI_TBL_INDX_STAG(profile_ndx, pcp, dei) \
  ARAD_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(profile_ndx, pcp, dei)
#define ARAD_EPNI_PCP_DEI_TBL_INDX_CTAG(profile_ndx, up) \
  ARAD_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(profile_ndx, up)
#define ARAD_EPNI_PCP_DEI_TBL_INDX_UNTAGGED(profile_ndx, tc, dp) \
  ARAD_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(profile_ndx, tc, dp)


#define ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INDX(unit, tag_format, edit_profile) \
  ((tag_format<<(SOC_DPP_DEFS_GET(unit, nof_eve_profile_bits))) + edit_profile)

#define ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_FLD_SIZE   ((SOC_IS_JERICHO_PLUS(unit)) ? 19 : 16)

#define ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INFO(unit, outer_tpid, inner_tpid, outer_vid, inner_vid, outer_pcp_dei, inner_pcp_dei, tags_to_remove, packet_is_tagged_after_eve) \
   ((outer_tpid << ((SOC_IS_JERICHO_PLUS(unit)) ? 15 : 14)) +  (inner_tpid << 12) + (outer_vid << 9) + (inner_vid << 6) + (outer_pcp_dei << 4) \
     + (inner_pcp_dei << 2) + tags_to_remove + (SOC_IS_JERICHO_PLUS(unit) ? (packet_is_tagged_after_eve << 18) : 0))

#define ARAD_PP_EPNI_NATIVE_EGRESS_EDIT_CMD_TBL_FLD_SIZE   (18)

#define ARAD_IHP_TOS_2_COS_KEY_ENTRY_OFFSET(is_ipv4, tbl_index, tos) \
  ((is_ipv4<<9) + (tbl_index<<8) + tos) 


#define ARAD_IHP_PORT_PROTOCOL_TBL_KEY_ENTRY_OFFSET(prtcl_ndx, profile_ndx) \
  ARAD_PP_FLDS_TO_BUFF_2(prtcl_ndx, 4, profile_ndx, 3)


#define ARAD_IHB_PATH_SELECT_TBL_KEY_ENTRY_OFFSET(oam_ins) \
  (oam_ins>>3)


#define ARAD_IHB_DESTINATION_STATUS_TBL_KEY_ENTRY_OFFSET(port_num) \
  (port_num>>3)


#define ARAD_IHB_FEC_ENTRY_ACCESSED_TBL_KEY_ENTRY_OFFSET(fec_id) \
  ((fec_id>>4)*2 + ((fec_id%2==0)?0:1))


#define ARAD_EPNI_IP_EXP_MAP_TBL_KEY_ENTRY_OFFSET(is_ipv4,tos,dp) \
  ((is_ipv4<<9) + (tos<<1) + dp)


#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_COS_PROFILE_NOF_BITS (4)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_NOF_BITS     (10)


#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PREFIX                   (0x2) 
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PREFIX_NOF_BITS          (5)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_OUTER_TAG_NDX_NOF_BITS   (1)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PCP_NOF_BITS             (3)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_DEI_NOF_BITS             (1)

#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2(outer_tag, pcp, dei)    \
  ARAD_PP_FLDS_TO_BUFF_4(                                                  \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PREFIX,                     \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PREFIX_NOF_BITS,            \
    outer_tag,                                                           \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_OUTER_TAG_NDX_NOF_BITS,     \
    pcp,                                                                 \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_PCP_NOF_BITS,               \
    dei,                                                                 \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_L2_DEI_NOF_BITS);



#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV4_PREFIX                 (0x2)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IPV6_PREFIX                 (0x3)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP_PREFIX_NOF_BITS          (2)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP_TOS_NOF_BITS             (8)

#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP(prefix, tos)            \
  ARAD_PP_FLDS_TO_BUFF_2(                                                  \
    prefix,                                                              \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP_PREFIX_NOF_BITS,            \
    tos,                                                                 \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_IP_TOS_NOF_BITS);


#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_PREFIX                 (0x1<<5) 
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_PREFIX_NOF_BITS        (7)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_EXP_NOF_BITS           (3)

#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS(exp)                  \
  ARAD_PP_FLDS_TO_BUFF_2(                                                  \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_PREFIX,                   \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_PREFIX_NOF_BITS,          \
    exp,                                                                 \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_MPLS_EXP_NOF_BITS);


#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP_TC_NOF_BITS           (3)
#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP_DP_NOF_BITS           (2)

#define ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP(tc, dp)              \
  ARAD_PP_FLDS_TO_BUFF_2(                                                  \
    tc,                                                                  \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP_TC_NOF_BITS,             \
    dp,                                                                  \
    ARAD_IHP_TC_DP_MAP_TABLE_TBL_KEY_LSB_TC_DP_DP_NOF_BITS);

#define ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(profile_ndx, pcp, dei) \
  ((profile_ndx<<6)+  ARAD_PCP_DEI_MAP_KEY_STAG(pcp, dei))
#define ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(profile_ndx, up) \
  ((profile_ndx<<6)+  ARAD_PCP_DEI_MAP_KEY_CTAG(up))
#define ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(profile_ndx, tc, dp) \
  ((profile_ndx<<6)+  ARAD_PCP_DEI_MAP_KEY_UNTAGGED(tc, dp))

#define ARAD_PP_EPNI_VLAN_EDIT_PCP_DEI_MAP_DP_TBL_INDX(profile_ndx, dp, pcp_dei) \
	((profile_ndx<<6)+ (dp<<4)+ pcp_dei)


#define ARAD_PCP_DEI_MAP_KEY_STAG(pcp, dei) \
  ((0x3<<4) + (pcp<<1) + dei)
#define ARAD_PCP_DEI_MAP_KEY_CTAG(up) \
  ((0x2<<4) + (up<<1))
#define ARAD_PCP_DEI_MAP_KEY_UNTAGGED(tc, dp) \
  ((tc<<2) + dp)


#define ARAD_PP_EPNI_PCP_DEI_TBL_INDX_STAG(profile_ndx, pcp, dei) \
  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_STAG(profile_ndx, pcp, dei)
#define ARAD_PP_EPNI_PCP_DEI_TBL_INDX_CTAG(profile_ndx, up) \
  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_CTAG(profile_ndx, up)
#define ARAD_PP_EPNI_PCP_DEI_TBL_INDX_UNTAGGED(profile_ndx, tc, dp) \
  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_INDX_UNTAGGED(profile_ndx, tc, dp)

#define ARAD_PP_EGQ_AUX_TABLE_TBL_ORIENTATION_MODE_OFFSET(out_lif_ndx) \
           (out_lif_ndx >> 5) 
#define ARAD_PP_EGQ_AUX_TABLE_TBL_ORIENTATION_MODE_ENTRY_OFFSET(out_lif_ndx) \
            (out_lif_ndx % 32)


#define BASE_QUEUES_PER_QPM_2_ENTRY 4
#define BASE_QUEUES_PER_QPM_FAP_PORT_MSB_OFFSET 7
#define BASE_QUEUES_PER_QPM_FAP_PORT_NO_MSB_MASK 127
extern const soc_field_t qpm2_field_per_base_queue[BASE_QUEUES_PER_QPM_2_ENTRY];

#define ARAD_PP_IN_LIF_TO_BANK_ID(_unit, entry_offset)   \
  ((((entry_offset) & SOC_DPP_DEFS_GET(_unit, inlif_bank_msbs_mask)) >> \
    (SOC_DPP_DEFS_GET(_unit, inlif_bank_msbs_mask_start) - SOC_DPP_DEFS_GET(_unit, inlif_bank_lsbs_mask_end))) \
  | ((entry_offset) & SOC_DPP_DEFS_GET(_unit, inlif_bank_lsbs_mask)))

#define ARAD_PP_IN_LIF_TO_OFFSET_IN_BANK(_unit, entry_offset) \
  (((entry_offset) & ~(SOC_DPP_DEFS_GET(_unit, inlif_bank_msbs_mask) | SOC_DPP_DEFS_GET(_unit, inlif_bank_lsbs_mask))) \
   >> SOC_DPP_DEFS_GET(_unit, inlif_bank_lsbs_mask_end))


#define ARAD_PP_FEC_PATH_SELECT_MEM(_unit)   \
        ((SOC_IS_JERICHO_PLUS(unit)) ?  IHB_FEC_PATH_SELECTm : IHB_PATH_SELECTm)





typedef struct
{
  uint32 pge_mem;
} ARAD_OLP_PGE_MEM_TBL_DATA;

typedef struct
{
  uint32 contexts_bit_mapping[3];
} ARAD_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA;

typedef struct
{
  uint32 cpu_packet_counter[2];
} ARAD_IRE_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pcb_pointer;
  uint32 count;
  uint32 ecc;
} ARAD_IDR_COMPLETE_PC_TBL_DATA;

typedef struct
{
  uint32 mirror_destination;
  uint32 is_queue_number;
  uint32 is_multicast;
  uint32 is_outbound_mirror;
  uint32 mirror_traffic_class;
  uint32 traffic_class_over_write;
  uint32 mirror_drop_precedence;
  uint32 drop_precedence_over_write;
} ARAD_IRR_MIRROR_TABLE_TBL_DATA;

typedef struct
{
  uint32 snoop_destination;
  uint32 is_queue_number;
  uint32 is_multicast;
  uint32 snoop_traffic_class;
  uint32 traffic_class_over_write;
  uint32 snoop_drop_precedence;
  uint32 drop_precedence_over_write;
} ARAD_IRR_SNOOP_TABLE_TBL_DATA;

typedef struct
{
  uint32 glag_to_lag_range;
} ARAD_IRR_GLAG_TO_LAG_RANGE_TBL_DATA;

typedef struct
{
  uint32 glag_mapping;
} ARAD_IRR_GLAG_MAPPING_TBL_DATA;

typedef struct
{
  uint32 offset;
  uint32 rr_lb_mode;
} ARAD_IRR_GLAG_NEXT_MEMBER_TBL_DATA;

typedef struct
{
  uint32 two_lsb;
  uint32 two_msb;
} ARAD_IRR_RLAG_NEXT_MEMBER_TBL_DATA;

typedef struct
{
  uint32 port_type;
  uint32 bytes_to_skip;
  uint32 custom_command_select;
  uint32 statistics_profile_extension;
  uint32 port_type_extension;
  uint32 statistics_profile;
  uint32 use_lag_member;
  uint32 has_mirror;
  uint32 mirror_is_mc;
  uint32 bytes_to_remove;
  uint32 header_remove;
  uint32 append_ftmh;
  uint32 append_prog_header;
} ARAD_IHP_PORT_INFO_TBL_DATA;

typedef struct
{
  uint32 port_to_system_port_id;
} ARAD_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA;

typedef struct
{
  uint32 shaping;
  uint32 shaping_itmh;
  uint32 itmh[2];
} ARAD_IHP_STATIC_HEADER_TBL_DATA;

typedef struct
{
  uint32 system_port_my_port_table[ARAD_IHP_SYS_MY_PORT_TBL_FLDS];
} ARAD_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA;

typedef struct
{
  uint32 ptc_commands[2];
} ARAD_IHP_PTC_COMMANDS_TBL_DATA;

typedef struct
{
  uint32 ptc_key_program_ptr;
  uint32 ptc_key_program_var;
  uint32 ptc_cos_profile;
} ARAD_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA;

typedef struct
{
  uint32 select;
  uint32 length;
  uint32 nibble_shift;
  uint32 byte_shift;
  uint32 offset_select;
} ARAD_IHP_KEY_PROGRAM_TBL_DATA;

typedef struct
{
  uint32 select[ARAD_IHP_KEY_PROG_NOF_INSTR];
  uint32 length[ARAD_IHP_KEY_PROG_NOF_INSTR];
  uint32 nibble_shift[ARAD_IHP_KEY_PROG_NOF_INSTR];
  uint32 byte_shift[ARAD_IHP_KEY_PROG_NOF_INSTR];
  uint32 offset_select[ARAD_IHP_KEY_PROG_NOF_INSTR];
} ARAD_IHP_KEY_PROGRAM0_TBL_DATA;

typedef struct
{
  uint32 key_program1;
} ARAD_IHP_KEY_PROGRAM1_TBL_DATA;

typedef struct
{
  uint32 instruction4;
  uint32 instruction3;
  uint32 exact_match_enable;
  uint32 exact_match_mask_index;
} ARAD_IHP_KEY_PROGRAM2_TBL_DATA;

typedef struct
{
  uint32 key_program3;
} ARAD_IHP_KEY_PROGRAM3_TBL_DATA;

typedef struct
{
  uint32 instruction9;
  uint32 instruction10;
  uint32 tcam_match_enable;
  uint32 tcam_match_select;
  uint32 tcam_key_and_value;
  uint32 tcam_key_or_value;
  uint32 bytes_to_remove_hdr_sel;
  uint32 bytes_to_remove_hdr_size;
  uint32 cos_hdr_var_mask_select;
} ARAD_IHP_KEY_PROGRAM4_TBL_DATA;

typedef struct
{
  uint32 drop_precedence;
  uint32 traffic_class;
} ARAD_IHP_PROGRAMMABLE_COS_TBL_DATA;

typedef struct
{
  uint32 programmable_cos1[3];
} ARAD_IHP_PROGRAMMABLE_COS1_TBL_DATA;

typedef struct
{
  uint32 bdb_link_list;
} ARAD_IQM_BDB_LINK_LIST_TBL_DATA;

typedef struct
{
  uint32 pq_head_ptr;
  uint32 que_not_empty;
  uint32 pq_inst_que_size;
  uint32 pq_avrg_szie;
  uint32 pq_inst_que_buff_size;
} ARAD_IQM_DYNAMIC_TBL_DATA;

typedef struct
{
  uint32 credit_class;
  uint32 rate_class;
  uint32 connection_class;
  uint32 traffic_class;
  uint32 que_signature;
} ARAD_IQM_STATIC_TBL_DATA;

typedef struct
{
  uint32 tail_ptr;
} ARAD_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA;

typedef struct
{
  uint32 pq_weight;
  uint32 avrg_en;
} ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA;

typedef struct
{
  uint32 crdt_disc_val;
  uint32 discnt_sign;
} ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA;

typedef struct
{
  uint32 flus_cnt;
} ARAD_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA;

typedef struct
{
  uint32 mn_us_cnt[2];
} ARAD_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA;

typedef struct
{
  uint32 pq_max_que_size_mnt;
  uint32 pq_max_que_size_exp;
  uint32 pq_max_que_size_bds_mnt;
  uint32 pq_max_que_size_bds_exp;
  uint32 pq_wred_en;
  uint32 pq_c2;
  uint32 pq_c3;
  uint32 pq_c1;
  uint32 pq_avrg_max_th;
  uint32 pq_avrg_min_th;
  uint32 pq_wred_pckt_sz_ignr;
  uint32 addmit_logic;
} ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA;

typedef struct
{
  uint32 tx_pd;
} ARAD_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA;

typedef struct
{
  uint32 tx_dscr[3];
} ARAD_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA;

typedef struct
{
  uint32 vsq_rc_a;
} ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA;

typedef struct
{
  uint32 vsq_rc_b;
} ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA;

typedef struct
{
  uint32 vsq_rc_c;
} ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA;

typedef struct
{
  uint32 vsq_rc_d;
} ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA;

typedef struct
{
  uint32 vsq_size_wrds;
  uint32 vsq_size_bds;
} ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA;

typedef struct
{
  uint32 vsq_size_wrds;
  uint32 vsq_size_bds;
} ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA;

typedef struct
{
  uint32 vsq_size_wrds;
  uint32 vsq_size_bds;
} ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA;

typedef struct
{
  uint32 vsq_size_wrds;
  uint32 vsq_size_bds;
} ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA;

typedef struct
{
  uint32 vsq_avrg_size;
} ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA;

typedef struct
{
  uint32 vsq_avrg_size;
} ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA;

typedef struct
{
  uint32 vsq_avrg_size;
} ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA;

typedef struct
{
  uint32 vsq_avrg_size;
} ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA;

typedef struct
{
  uint32 wred_en;
  uint32 avrg_size_en;
  uint32 red_weight_q;
  uint32 set_threshold_words_mnt;
  uint32 set_threshold_words_exp;
  uint32 clear_threshold_words_mnt;
  uint32 clear_threshold_words_exp;
  uint32 set_threshold_bd_mnt;
  uint32 set_threshold_bd_exp;
  uint32 clear_threshold_bd_mnt;
  uint32 clear_threshold_bd_exp;
} ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA;

#define ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA

typedef struct
{
  uint32 sys_red_en;
  uint32 adm_th;
  uint32 prob_th;
  uint32 drp_th;
  uint32 drp_prob_indx1;
  uint32 drp_prob_indx2;
} ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA;

typedef struct
{
  uint32 drp_prob;
} ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA;

typedef struct
{
  uint32 qsz_rng_th[ARAD_SYS_RED_NOF_Q_RNGS_THS];
} ARAD_IQM_SYSTEM_RED_TBL_DATA;

typedef struct
{
  uint32 data;
} ARAD_QDR_MEM_TBL_DATA;

typedef struct
{
  uint32 sys_phy_port;
} ARAD_BASE_Q_TO_SYS_PORT_TABLE_TBL_DATA;

typedef struct
{
  uint16 fap_id;
  uint16 fap_port_id;
} ARAD_BASE_Q_TO_MOD_PORT_TABLE_TBL_DATA;

typedef struct
{
  uint32 dest_port;
  uint32 dest_dev;
} ARAD_MOD_PORT_TBL_DATA;

typedef struct
{
  uint32 base_flow;
  uint32 sub_flow_mode;
} ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA;

typedef struct
{
  uint32 queue_type_lookup_table;
} ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA;

typedef struct
{
  uint32 queue_priority_map_select;
} ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA;

typedef struct
{
  uint32 queue_priority_maps_table[ARAD_IPS_NOF_QUEUE_PRIORITY_MAPS_TABLES];
} ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA;

typedef struct
{
  uint32 off_to_slow_msg_th;
  uint32 off_to_norm_msg_th;
  uint32 slow_to_norm_msg_th;
  uint32 norm_to_slow_msg_th;
  uint32 fsm_th_mul;
} ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA;

typedef struct
{
  uint32 backoff_enter_qcr_bal_th;
  uint32 backoff_exit_qcr_bal_th;
  uint32 backlog_enter_qcr_bal_th;
  uint32 backlog_exit_qcr_bal_th;
  uint32 backslow_enter_qcr_bal_th;
  uint32 backslow_exit_qcr_bal_th;
} ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA;

typedef struct
{
  uint32 empty_qsatisfied_cr_bal;
  uint32 max_empty_qcr_bal;
  uint32 exceed_max_empty_qcr_bal;
} ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA;

typedef struct
{
  uint32 wd_status_msg_gen_period;
  uint32 wd_delete_qth;
} ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA;

typedef struct
{
  uint32 cr_bal;
  uint32 crs;
  uint32 one_pkt_deq;
  uint32 wd_last_cr_time;
  uint32 in_dqcq;
  uint32 wd_delete;
  uint32 fsmrq_ctrl;
} ARAD_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA;

typedef struct
{
  uint32 exponent;
  uint32 mantissa;
  uint32 qsize_4b;
} ARAD_IPS_QUEUE_SIZE_TABLE_TBL_DATA;

typedef struct
{
  uint32 maxqsz;
  uint32 maxqsz_age;
} ARAD_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA;
typedef struct
{
  uint32 flow_id;
  uint32 dest_pipe;
  uint32 flow_status;
  uint32 dest_port;
  uint32 queue_size_4b;
  uint32 queue_number;
  uint32 orig_fs;
  uint32 disable_timer;
  uint32 valid;
} ARAD_IPS_FMS_MSG_MEM_TABLE_TBL_DATA;
typedef struct
{
  uint32 bdq;
} ARAD_IPT_BDQ_TBL_DATA;

typedef struct
{
  uint32 pcq[2];
} ARAD_IPT_PCQ_TBL_DATA;

typedef struct
{
  uint32 mop_mmu;
} ARAD_IPT_MOP_MMU_TBL_DATA;

typedef struct
{
  uint32 fdtctl;
} ARAD_IPT_FDTCTL_TBL_DATA;

typedef struct
{
  uint32 fdtdata[16];
} ARAD_IPT_FDTDATA_TBL_DATA;

typedef struct
{
  uint32 egqctl;
} ARAD_IPT_EGQCTL_TBL_DATA;

typedef struct
{
  uint32 egqdata[16];
} ARAD_IPT_EGQDATA_TBL_DATA;

typedef struct
{
  uint32 dll_ram[2];
} ARAD_DPI_DLL_RAM_TBL_DATA;

typedef struct
{
  uint32 unicast_distribution_memory_for_data_cells[3];
} ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA;

typedef struct
{
  uint32 unicast_distribution_memory_for_control_cells[3];
} ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFA_CH0_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFA_CH1_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFA_CH2_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFA_CH3_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFB_CH0_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFB_CH1_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFB_CH2_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFB_CH3_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_NIFAB_NCH_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_CPU_SCM_TBL_DATA;

typedef struct
{
  uint32 port_cr_to_add;
  uint32 ofp_index;
} ARAD_EGQ_RCY_SCM_TBL_DATA;

typedef struct
{
  uint32 interface_select;
} ARAD_EGQ_CCM_TBL_DATA;

typedef struct
{
  uint32 port_max_credit;
} ARAD_EGQ_PMC_TBL_DATA;

typedef struct
{
  uint32 cbm;
} ARAD_EGQ_CBM_TBL_DATA;

typedef struct
{
  uint32 free_buffer_memory;
} ARAD_EGQ_FBM_TBL_DATA;

typedef struct
{
  uint32 free_descriptor_memory[2];
} ARAD_EGQ_FDM_TBL_DATA;

typedef struct
{
  uint32 mc_or_mc_low_queue_weight;
  uint32 uc_or_uc_low_queue_weight;
} ARAD_EGQ_DWM_TBL_DATA;

typedef struct
{
  uint32 crcremainder;
  uint32 reas_state;
  uint32 eopfrag_num;
  uint32 nxt_frag_number;
  uint32 stored_seg_size;
  uint32 fix129;
} ARAD_EGQ_RRDM_TBL_DATA;

typedef struct
{
  uint32 packet_start_buffer_pointer;
  uint32 packet_buffer_write_pointer;
  uint32 reas_state;
  uint32 packet_frag_cnt;
} ARAD_EGQ_RPDM_TBL_DATA;


typedef struct
{
  
  uint32 vlan_membership[8];
} ARAD_EGQ_VLAN_TABLE_TBL_DATA; 

typedef struct
{
  uint32 cfc_flow_control[3];
} ARAD_EGQ_CFC_FLOW_CONTROL_TBL_DATA;

typedef struct
{
  uint32 nifa_flow_control;
} ARAD_EGQ_NIFA_FLOW_CONTROL_TBL_DATA;

typedef struct
{
  uint32 nifb_flow_control;
} ARAD_EGQ_NIFB_FLOW_CONTROL_TBL_DATA;

typedef struct
{
  uint32 cpu_last_header[2];
} ARAD_EGQ_CPU_LAST_HEADER_TBL_DATA;

typedef struct
{
  uint32 ipt_last_header[2];
} ARAD_EGQ_IPT_LAST_HEADER_TBL_DATA;

typedef struct
{
  uint32 fdr_last_header[2];
} ARAD_EGQ_FDR_LAST_HEADER_TBL_DATA;

typedef struct
{
  uint32 cpu_packet_counter[2];
} ARAD_EGQ_CPU_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ipt_packet_counter[2];
} ARAD_EGQ_IPT_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 fdr_packet_counter[2];
} ARAD_EGQ_FDR_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 rqp_packet_counter[2];
} ARAD_EGQ_RQP_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 rqp_discard_packet_counter[2];
} ARAD_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ehp_unicast_packet_counter[2];
} ARAD_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ehp_multicast_high_packet_counter[2];
} ARAD_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ehp_multicast_low_packet_counter[2];
} ARAD_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 ehp_discard_packet_counter[2];
} ARAD_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_unicast_high_packet_counter[2];
} ARAD_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_unicast_low_packet_counter[2];
} ARAD_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_multicast_high_packet_counter[2];
} ARAD_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_multicast_low_packet_counter[2];
} ARAD_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_unicast_high_bytes_counter[2];
} ARAD_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_unicast_low_bytes_counter[2];
} ARAD_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_multicast_high_bytes_counter[2];
} ARAD_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_multicast_low_bytes_counter[2];
} ARAD_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_discard_unicast_packet_counter[2];
} ARAD_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 pqp_discard_multicast_packet_counter[2];
} ARAD_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 fqp_packet_counter[2];
} ARAD_EGQ_FQP_PACKET_COUNTER_TBL_DATA;

typedef struct
{
  uint32 egq_ofp_num;
} ARAD_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA;

typedef struct
{
  uint32 egq_ofp_num[ARAD_NOF_CFC_NIF_CLS2OFP_MAP_FLDS];
} ARAD_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA;

typedef struct
{
  uint32 ofp_hr;
  uint32 lp_ofp_valid;
  uint32 hp_ofp_valid;
} ARAD_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA;

typedef struct
{
  uint32 fc_index;
  uint32 fc_dest_sel;
} ARAD_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA;

typedef struct
{
  uint32 fc_index;
  uint32 fc_source_sel;
} ARAD_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA;

typedef struct
{
  uint32 device_rate;
} ARAD_SCH_DRM_TBL_DATA;

typedef struct
{
  uint32 dual_shaper_ena;
} ARAD_SCH_DSM_TBL_DATA;

typedef struct
{
  uint32 sch_number;
  uint32 cos;
  uint32 hrsel_dual;
} ARAD_SCH_FDMS_TBL_DATA;

typedef struct
{
  uint32 peak_rate_man_even;
  uint32 peak_rate_exp_even;
  uint32 max_burst_even;
  uint32 slow_rate2_sel_even;
  uint32 peak_rate_man_odd;
  uint32 peak_rate_exp_odd;
  uint32 max_burst_odd;
  uint32 slow_rate2_sel_odd;
  uint32 max_burst_update_even;
  uint32 max_burst_update_odd;
} ARAD_SCH_SHDS_TBL_DATA;

typedef struct
{
  uint32 sch_enable;
} ARAD_SCH_SEM_TBL_DATA;

typedef struct
{
  uint32 sfenable;
} ARAD_SCH_FSF_TBL_DATA;

typedef struct
{
  uint32 flow_group[ARAD_TBL_FGM_NOF_GROUPS_ONE_LINE];
} ARAD_SCH_FGM_TBL_DATA;

typedef struct
{
  uint32 hrmode;
  uint32 hrmask_type;
} ARAD_SCH_SHC_TBL_DATA;

typedef struct
{
  uint32 clsch_type;
} ARAD_SCH_SCC_TBL_DATA;

typedef struct
{
  uint32 clconfig;
  uint32 af0_inv_weight;
  uint32 af1_inv_weight;
  uint32 af2_inv_weight;
  uint32 af3_inv_weight;
  uint32 wfqmode;
  uint32 enh_clen;
  uint32 enh_clsphigh;
} ARAD_SCH_SCT_TBL_DATA;

typedef struct
{
  uint32 base_queue_num;
  uint32 sub_flow_mode;
  uint32 flow_slow_enable;
} ARAD_SCH_FQM_TBL_DATA;

typedef struct
{
  uint32 device_number;
} ARAD_SCH_FFM_TBL_DATA;

typedef struct
{
  uint32 token_count;
  uint32 slow_status;
} ARAD_SCH_TMC_TBL_DATA;

typedef struct
{
  uint32 max_qsz;
  uint32 flow_id;
  uint32 aging_bit;
} ARAD_SCH_PQS_TBL_DATA;

typedef struct
{
  uint32 schinit;
} ARAD_SCH_SCHEDULER_INIT_TBL_DATA;

typedef struct
{
  uint32 message_flow_id;
  uint32 message_type;
} ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_DATA;
typedef struct
{
  uint32 data[16];
} ARAD_MMU_DRAM_ADDRESS_SPACE_TBL_DATA;

typedef struct
{
  uint32 data;
} ARAD_MMU_IDF_TBL_DATA;

typedef struct
{
  uint32 data;
} ARAD_MMU_FDF_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} ARAD_MMU_RDFA_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} ARAD_MMU_RDFB_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} ARAD_MMU_RDFC_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} ARAD_MMU_RDFD_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} ARAD_MMU_RDFE_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
  uint32 status;
} ARAD_MMU_RDFF_WADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
} ARAD_MMU_RDF_RADDR_TBL_DATA;

typedef struct
{
  uint32 waddr_half_a;
} ARAD_MMU_WAF_HALFA_WADDR_TBL_DATA;

typedef struct
{
  uint32 waddr_half_b;
} ARAD_MMU_WAF_HALFB_WADDR_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} ARAD_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} ARAD_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} ARAD_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} ARAD_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} ARAD_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_a;
  uint32 status_half_a;
} ARAD_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} ARAD_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} ARAD_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} ARAD_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} ARAD_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} ARAD_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr_half_b;
  uint32 status_half_b;
} ARAD_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 waddr;
} ARAD_MMU_RAF_WADDR_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} ARAD_MMU_RAFA_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} ARAD_MMU_RAFB_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} ARAD_MMU_RAFC_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} ARAD_MMU_RAFD_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} ARAD_MMU_RAFE_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 raddr;
  uint32 status;
} ARAD_MMU_RAFF_RADDR_STATUS_TBL_DATA;

typedef struct
{
  uint32 select_source_sum[ARAD_IPT_SELECT_SOURCE_SUM_TBL_ENTRY_SIZE];
} ARAD_IPT_SELECT_SOURCE_SUM_TBL_DATA;

typedef struct
{
  uint32 reassembly_context;
  uint32 port_termination_context;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRE_NIF_CTXT_MAP_TBL_DATA;

typedef struct
{
  uint32 contexts_bit_mapping[6];
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA;

typedef struct
{
  uint32 reassembly_context;
  uint32 port_termination_context;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRE_RCY_CTXT_MAP_TBL_DATA;

typedef struct
{
  uint32 reassembly_context;
  uint32 port_termination_context;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRE_CPU_CTXT_MAP_TBL_DATA;

typedef struct
{
  uint32 mode;
  uint32 cpu;
  uint32 header[3];
  uint32 link_mask_ptr;
  uint32 add_packet_crc;
  uint32 parity;
  uint32 mc_replication;
} __ATTRIBUTE_PACKED__ ARAD_IRE_TDM_CONFIG_TBL_DATA;

typedef struct
{
  uint32 org_size;
  uint32 size;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IDR_CONTEXT_MRU_TBL_DATA;


typedef struct
{
  uint32 time_stamp;
  uint32 level;
  uint32 reserved;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IDR_ETHERNET_METER_STATUS_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 level;
} __ATTRIBUTE_PACKED__ ARAD_IDR_GLOBAL_METER_STATUS_TBL_DATA;


typedef struct
{
  uint32 destination;
  uint32 tc;
  uint32 tc_ow;
  uint32 dp;
  uint32 dp_ow;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA;

typedef struct
{
  uint32 meter_ptr0;
  uint32 meter_ptr0_ow;
  uint32 meter_ptr1;
  uint32 meter_ptr1_ow;
  uint32 counter_ptr0;
  uint32 counter_ptr0_ow;
  uint32 counter_ptr1;
  uint32 counter_ptr1_ow;
  uint32 dp_cmd;
  uint32 dp_cmd_ow;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 pcp;
} __ATTRIBUTE_PACKED__ ARAD_IRR_FREE_PCB_MEMORY_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 pcp;
} __ATTRIBUTE_PACKED__ ARAD_IRR_PCB_LINK_TABLE_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 is_pcp;
} __ATTRIBUTE_PACKED__ ARAD_IRR_IS_FREE_PCB_MEMORY_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 is_pcp;
} __ATTRIBUTE_PACKED__ ARAD_IRR_IS_PCB_LINK_TABLE_TBL_DATA;

typedef struct
{
  uint32 ecc;
  uint32 pointer;
} __ATTRIBUTE_PACKED__ ARAD_IRR_RPF_MEMORY_TBL_DATA;

typedef struct
{
  uint32 descriptor[3];
  uint32 ecc;
} __ATTRIBUTE_PACKED__ ARAD_IRR_MCR_MEMORY_TBL_DATA;

typedef struct
{
  uint32 descriptor[2];
  uint32 ecc;
} __ATTRIBUTE_PACKED__ ARAD_IRR_ISF_MEMORY_TBL_DATA;

typedef struct
{
  uint32 queue_number;
  uint32 queue_valid;
  uint32 tc_profile;
  uint8 valid[SOC_DPP_DEFS_MAX(NOF_CORES)];
} __ATTRIBUTE_PACKED__ ARAD_IRR_DESTINATION_TABLE_TBL_DATA;

typedef struct
{
  uint32 range;
  uint32 mode;
  uint32 parity;
#ifdef BCM_88660_A0
  uint8 is_stateful;
#endif 
} __ATTRIBUTE_PACKED__ ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA;


typedef struct
{
  uint32 destination;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRR_LAG_MAPPING_TBL_DATA;

typedef struct
{
  uint32 offset;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRR_LAG_NEXT_MEMBER_TBL_DATA;

typedef struct
{
  uint32 member;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRR_SMOOTH_DIVISION_TBL_DATA;

typedef struct
{
  
  uint32 traffic_class_mapping[ARAD_NOF_TRAFFIC_CLASSES];
} __ATTRIBUTE_PACKED__ ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA;

typedef struct
{
  uint32 stack_lag;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRR_STACK_FEC_RESOLVE_TBL_DATA;

typedef struct
{
  uint32 base_queue;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_DATA;


typedef struct
{
  uint32 lag_port_mine0;
  uint32 lag_port_mine1;
  uint32 lag_port_mine2;
  uint32 lag_port_mine3;
} __ATTRIBUTE_PACKED__ ARAD_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA;

typedef struct
{
  uint32 pp_port_offset1;
  uint32 pp_port_offset2;
  uint32 pp_port_profile;
  uint32 pp_port_use_offset_directly;
} __ATTRIBUTE_PACKED__ ARAD_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA;

typedef struct
{
  uint32 system_port_offset1;
  uint32 system_port_value;
  uint32 system_port_profile;
  uint32 system_port_value_to_use;
} __ATTRIBUTE_PACKED__ ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA;


typedef struct
{
  uint32 egress_pmf_profile;
  uint32 format_code;
  uint32 value_1;
  uint32 value_2;
  uint32 header_code;
  uint32 qualifier;
  uint32 ethernet_tag_format;
  uint32 fall_to_bridge;
  uint32 egress_pmf_profile_mask;
  uint32 format_code_mask;
  uint32 value_1_mask;
  uint32 value_2_mask;
  uint32 header_code_mask;
  uint32 qualifier_mask;
  uint32 ethernet_tag_format_mask;
  uint32 fall_to_bridge_mask;
  uint32 program;
  uint32 valid;
} __ATTRIBUTE_PACKED__ ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_DATA;




typedef struct
{
  uint32 qdr_dll_mem;
} __ATTRIBUTE_PACKED__ ARAD_QDR_QDR_DLL_MEM_TBL_DATA;


typedef struct
{
  uint32 cp_enable;
  uint32 cp_class;
  uint32 cp_id;
} __ATTRIBUTE_PACKED__ ARAD_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA;

typedef struct
{
  uint32 cp_enqued1;
  uint32 cp_qsize_old;
} __ATTRIBUTE_PACKED__ ARAD_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_DATA;

typedef struct
{
  uint32 cp_qeq;
  uint32 cp_w;
  uint32 cp_fb_max_val;
  uint32 cp_quant_div;
  uint32 cp_sample_base[ARAD_CNM_CP_SAMPLE_BASE_SIZE];
  uint32 cp_fixed_sample_base;
} __ATTRIBUTE_PACKED__ ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_DATA;

typedef struct
{
  uint32 cp_enable_1;
  uint32 cp_profile_1;
  uint32 cp_enable_2;
  uint32 cp_profile_2;
  uint32 cp_id;
}__ATTRIBUTE_PACKED__ ARAD_IQM_CNM_DS_TBL_DATA;

typedef struct
{
  uint32 iqm_dp;
  uint32 etm_de;
} __ATTRIBUTE_PACKED__ ARAD_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 cbl;
  uint32 ebl;
} __ATTRIBUTE_PACKED__ ARAD_IQM_NORMAL_DYNAMICA_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 cbl;
  uint32 ebl;
} __ATTRIBUTE_PACKED__ ARAD_IQM_NORMAL_DYNAMICB_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 cbl;
  uint32 ebl;
} __ATTRIBUTE_PACKED__ ARAD_IQM_HIGH_DYNAMICA_TBL_DATA;

typedef struct
{
  uint32 time_stamp;
  uint32 cbl;
  uint32 ebl;
} __ATTRIBUTE_PACKED__ ARAD_IQM_HIGH_DYNAMICB_TBL_DATA;



typedef struct
{
  uint32 packets_counter;
  uint32 octets_counter;
} __ATTRIBUTE_PACKED__ ARAD_IQM_CNTS_MEM_TBL_DATA;

typedef struct
{
  uint32 ovth_counter_bits[2];
} __ATTRIBUTE_PACKED__ ARAD_IQM_OVTH_MEMA_TBL_DATA;

typedef struct
{
  uint32 ovth_counter_bits[2];
} __ATTRIBUTE_PACKED__ ARAD_IQM_OVTH_MEMB_TBL_DATA;

typedef struct
{
  uint32 sop_mmu[3];
} __ATTRIBUTE_PACKED__ ARAD_IPT_SOP_MMU_TBL_DATA;


typedef struct
{
  uint32 cfg_byte_cnt[2];
} __ATTRIBUTE_PACKED__ ARAD_IPT_CFG_BYTE_CNT_TBL_DATA;

typedef struct
{
  uint32 egq_txq_wr_addr;
} __ATTRIBUTE_PACKED__ ARAD_IPT_EGQ_TXQ_WR_ADDR_TBL_DATA;

typedef struct
{
  uint32 egq_txq_rd_addr;
} __ATTRIBUTE_PACKED__ ARAD_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA;

typedef struct
{
  uint32 dtq0_wr_addr;
  uint32 dtq1_wr_addr;
  uint32 dtq2_wr_addr;
  uint32 dtq3_wr_addr;
  uint32 dtq4_wr_addr;
  uint32 dtq5_wr_addr;
  uint32 dtq6_wr_addr;
} __ATTRIBUTE_PACKED__ ARAD_IPT_FDT_TXQ_WR_ADDR_TBL_DATA;

typedef struct
{
  uint32 dtq0_rd_addr;
  uint32 dtq1_rd_addr;
  uint32 dtq2_rd_addr;
  uint32 dtq3_rd_addr;
  uint32 dtq4_rd_addr;
  uint32 dtq5_rd_addr;
  uint32 dtq6_rd_addr;
} __ATTRIBUTE_PACKED__ ARAD_IPT_FDT_TXQ_RD_ADDR_TBL_DATA;

typedef struct
{
  uint32 mask0;
  uint32 mask1;
  uint32 mask2;
  uint32 mask3;
} __ATTRIBUTE_PACKED__ ARAD_IPT_GCI_BACKOFF_MASK_TBL_DATA;


typedef struct
{
  uint32 ipt_contro_l_fifo[2];
} __ATTRIBUTE_PACKED__ ARAD_FDT_IPT_CONTRO_L_FIFO_TBL_DATA;

typedef struct
{
  uint32 read_pointer;
  uint32 packet_size256to_eop;
  uint32 counter_decreament;
  uint32 copy_data;
  uint32 last_seg_size;
  uint32 before_last_seg_size;
  uint32 pqp_qnum;
  uint32 pqp_oc768_qnum;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_QM_TBL_DATA;

typedef struct
{
  uint32 qsm;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_QSM_TBL_DATA;

typedef struct
{
  uint32 dcm;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_DCM_TBL_DATA;

typedef struct
{
  uint32 mc_high_queue_weight;
  uint32 uc_high_queue_weight;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_DWM_NEW_TBL_DATA;


typedef struct
{
  uint32 prog_editor_value; 
  uint32 prog_editor_profile;   
  uint32 outbound_mirr;  
  uint32 port_profile; 
  uint32 port_ch_num;
  uint32 port_type;
  uint32 cr_adjust_type;
  uint32 cos_map_profile;
  uint32 mirror_enable;
  uint32 mirror_cmd;
  uint32 mirror_channel;
  uint32 ecc;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_PCT_TBL_DATA;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  uint32 map_profile; 
  uint32 is_egr_mc;   
  uint32 tc; 
  uint32 dp; 

} __ATTRIBUTE_PACKED__ ARAD_EGQ_TC_DP_MAP_TBL_ENTRY;

typedef struct
{
  uint32 tc;
  uint32 dp;
} ARAD_EGQ_TC_DP_MAP_TBL_DATA;

typedef struct
{
  uint32 fqp_nif_port_mux;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_DATA;

typedef struct
{
  uint32 pqp_nif_port_mux;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_DATA;

typedef struct
{
  uint32 key_profile_map_index;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA;

typedef struct
{
  uint32 key_select;
  uint32 key_and_value;
  uint32 key_or_value;
  uint32 tcam_profile;
} __ATTRIBUTE_PACKED__ ARAD_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA;

typedef struct
{
  uint32 parser_last_sys_record[8];
} __ATTRIBUTE_PACKED__ ARAD_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA;

typedef struct
{
  uint32 parser_last_nwk_record1[8];
} __ATTRIBUTE_PACKED__ ARAD_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA;

typedef struct
{
  uint32 parser_last_nwk_record2[2];
} __ATTRIBUTE_PACKED__ ARAD_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA;

typedef struct
{
  uint32 erpp_debug[8];
} __ATTRIBUTE_PACKED__ ARAD_EGQ_ERPP_DEBUG_TBL_DATA;

typedef struct
{
  uint32 counter_compension;
} __ATTRIBUTE_PACKED__ ARAD_IHB_PINFO_LBP_TBL_DATA;


typedef struct
{
  uint32 port_pmf_profile;
} __ATTRIBUTE_PACKED__ ARAD_IHB_PINFO_PMF_TBL_DATA;


typedef struct  
{  
  uint32 interface_port_pmf_profile;  
} __ATTRIBUTE_PACKED__ ARAD_IHB_PTC_INFO_PMF_TBL_DATA;  

typedef struct
{
  uint32 packet_format_code_profile;
} __ATTRIBUTE_PACKED__ ARAD_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA;

#define ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE 3
#define ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE 3


typedef struct
{
  uint8 parser_leaf_context;
  uint8 in_port_profile;
  uint8 ptc_profile;
  uint32 packet_format_code;
  uint8 packet_format_qualifier_0;
  uint32 packet_format_qualifier_fwd;
  uint8 forwarding_code;
  uint8 forwarding_offset_index;
  uint8 forwarding_offset_index_ext;
  uint8 cpu_trap_code;
  uint32 in_lif_profile;
  uint8 out_lif_range;
  uint8 llvp_incoming_tag_structure;
  uint8 vt_processing_profile;
  uint8 vt_lookup_0_found;
  uint8 vt_lookup_1_found;
  uint8 tt_processing_profile;
  uint8 tt_lookup_0_found;
  uint8 tt_lookup_1_found;
  uint8 fwd_prcessing_profile;
  uint8 lem_1st_lkp_found;
  uint8 lem_2nd_lkp_found;
  uint8 lpm_1st_lkp_found;
  uint8 lpm_2nd_lkp_found;
  uint8 tcam_found;
  uint8 tcam_traps_found;
  uint8 elk_error;
  uint32 elk_found_result;
  uint8 stamp_native_vsi;
  uint8 mask_parser_leaf_context;
  uint8 mask_in_port_profile;
  uint8 mask_ptc_profile;
  uint32 mask_packet_format_code;
  uint8 mask_packet_format_qualifier_0;
  uint32 mask_packet_format_qualifier_fwd;
  uint8 mask_forwarding_code;
  uint8 mask_forwarding_offset_index;
  uint8 mask_forwarding_offset_index_ext;
  uint8 mask_cpu_trap_code;
  uint32 mask_in_lif_profile;
  uint8 mask_out_lif_range;
  uint8 mask_llvp_incoming_tag_structure;
  uint8 mask_vt_processing_profile;
  uint8 mask_vt_lookup_0_found;
  uint8 mask_vt_lookup_1_found;
  uint8 mask_tt_processing_profile;
  uint8 mask_tt_lookup_0_found;
  uint8 mask_tt_lookup_1_found;
  uint8 mask_fwd_prcessing_profile;
  uint8 mask_lem_1st_lkp_found;
  uint8 mask_lem_2nd_lkp_found;
  uint8 mask_lpm_1st_lkp_found;
  uint8 mask_lpm_2nd_lkp_found;
  uint8 mask_tcam_found;
  uint8 mask_tcam_traps_found;
  uint8 mask_elk_error;
  uint32 mask_elk_found_result;
  uint8 mask_stamp_native_vsi;
  uint16 program;
  uint8 valid;
  
  uint8 in_rif_profile;
  uint8 mask_in_rif_profile;
  uint8 tcam_found_1;
  uint8 mask_tcam_found_1;
  uint8 tcam_traps_found_1;
  uint8 mask_tcam_traps_found_1;
  uint8 rpf_stamp_native_vsi;
  uint8 mask_rpf_stamp_native_vsi;
} ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_DATA;

typedef struct
{
  uint32 program_selection_profile;
  uint32 tcam_action_0_lsb;
  uint32 tcam_action_1_lsb;
  uint32 tcam_action_2_lsb;
  uint32 tcam_action_3_lsb;
  uint32 kaps_payload_lsb;
  uint32 oamp_payload_lsb;
  uint32 lem_result_found;
  uint32 lem_result_lsb;
  uint32 mask_program_selection_profile;
  uint32 mask_tcam_action_0_lsb;
  uint32 mask_tcam_action_1_lsb;
  uint32 mask_tcam_action_2_lsb;
  uint32 mask_tcam_action_3_lsb;
  uint32 mask_kaps_payload_lsb;
  uint32 mask_oamp_payload_lsb;
  uint32 mask_lem_result_found;
  uint32 mask_lem_result_lsb;
  uint32 program;
  uint32 valid;

} ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_DATA;

typedef struct
{
  uint32 parser_leaf_context;
  uint32 vsi_profile;
  uint32 in_lif_profile;
  uint32 flp_program;
  uint32 packet_format_code;
  uint32 pfq_next_protocol_fwd;
  uint32 pfq_next_protocol_fwd_next;
  uint32 forwarding_code;
  uint32 forwarding_offset_index;
  uint32 destination_msbs;
  uint32 unknown_addr;

  uint32 parser_leaf_context_mask;
  uint32 vsi_profile_mask;
  uint32 in_lif_profile_mask;
  uint32 flp_program_mask;
  uint32 packet_format_code_mask;
  uint32 pfq_next_protocol_fwd_mask;
  uint32 pfq_next_protocol_fwd_next_mask;
  uint32 forwarding_code_mask;
  uint32 forwarding_offset_index_mask;
  uint32 destination_msbs_mask;
  uint32 unknown_addr_mask;

  uint32 program;
  uint32 valid;
} ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_DATA;

typedef struct
{
  uint32 source_port_min;
  uint32 source_port_max;
  uint32 destination_port_min;
  uint32 destination_port_max;
} __ATTRIBUTE_PACKED__ ARAD_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA;

typedef struct
{
  uint32 direct_key_select;
  uint32 direct_db_and_value;
  uint32 direct_db_or_value;
} __ATTRIBUTE_PACKED__ ARAD_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA;


typedef struct
{
  uint32 program;
  uint32 key_select;
} __ATTRIBUTE_PACKED__ ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA;

typedef struct
{
  uint32 bit_select;
} __ATTRIBUTE_PACKED__ ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA;

typedef struct
{
  uint32 map_index;
  uint32 map_data;
  uint32 is_action_valid;
} __ATTRIBUTE_PACKED__ ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA;

typedef struct
{
  uint32 action_type;
  uint32 field_select_map[ARAD_IHB_FEM_MAX_OUTPUT_SIZE];
  uint32 offset_index;
} __ATTRIBUTE_PACKED__ ARAD_IHB_FEM_MAP_TABLE_TBL_DATA;

typedef struct
{
  uint32 offset;
} __ATTRIBUTE_PACKED__ ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA;





typedef struct
{
  uint32 inst_valid[2];
  uint32 inst_source_select[2];
  uint32 inst_header_offset_select[2];
  uint32 inst_niblle_field_offset[2];
  uint32 inst_bit_count[2];
  uint32 inst_lfem_program[2];
} __ATTRIBUTE_PACKED__ ARAD_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA;

typedef struct
{
  uint32 lfem_field_select_map[3];
} __ATTRIBUTE_PACKED__ ARAD_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA;



typedef struct
{
  uint32 ofp_num;
  uint32 sch_hp_valid;
  uint32 sch_lp_valid;
  uint32 egq_hp_valid;
  uint32 egq_lp_valid;
} __ATTRIBUTE_PACKED__ ARAD_CFC_RCL2_OFP_TBL_DATA;

typedef struct
{
  struct
  {
    uint32 ofp_num;
    struct
    {
      uint32 hp_valid;
      uint32 lp_valid;
    }sch;
    struct
    {
      uint32 hp_valid;
      uint32 lp_valid;
    }egq;
  }data[4];
} __ATTRIBUTE_PACKED__ ARAD_CFC_NIFCLSB2_OFP_TBL_DATA;

typedef struct
{
  uint32 fc_index;
  uint32 fc_dest_sel;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_CFC_CALRX_TBL_DATA;

typedef struct
{
  uint32 bitmap[ARAD_CFC_HCFC_BITMAP_TBL_SIZE];
} __ATTRIBUTE_PACKED__ ARAD_CFC_HCFC_BITMAP_TBL_DATA;

typedef struct
{
  uint32 ofp_hr;
  uint32 lp_ofp_valid;
  uint32 hp_ofp_valid;
} __ATTRIBUTE_PACKED__ ARAD_CFC_OOB_SCH_MAP_TBL_DATA;

typedef struct
{
  uint32 fc_index;
  uint32 fc_source_sel;
} __ATTRIBUTE_PACKED__ ARAD_CFC_CALTX_TBL_DATA;



typedef struct
{
  uint32 destination;
  uint32 format_specific_data;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT1_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 out_lif;
  uint32 out_lif_valid;
  uint32 identifier;
  uint32 p2p_service;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_TP2P_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 learn_destination;
  uint32 isid_domain;
  uint32 learn_type;
  uint32 sa_drop;
  uint32 is_dynamic;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_PBP_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 mpls_label;
  uint32 mpls_command;
  uint32 p2p_service;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ILM_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 out_lif;
  uint32 out_lif_valid;
  uint32 tpid_profile;
  uint32 has_cw;
  uint32 p2p_service;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_SP2P_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 asd;
  uint32 is_dynamic;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT1_ASD_ETHERNET_TBL_DATA;

typedef struct
{
  uint32 permitted_system_port;
  uint32 format_specific_data;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT2_TBL_DATA;

typedef struct
{
  uint32 permitted_system_port;
  uint32 adjacent;
  uint32 drop_if_vid_differ;
  uint32 permit_all_ports;
  uint32 override_vid_in_tagged;
  uint32 use_vid_in_untagged;
  uint32 is_dynamic;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT2_TRILL_TBL_DATA;

typedef struct
{
  uint32 permitted_system_port;
  uint32 vid;
  uint32 accept_untagged;
  uint32 drop_if_vid_differ;
  uint32 permit_all_ports;
  uint32 override_vid_in_tagged;
  uint32 use_vid_in_untagged;
  uint32 is_dynamic;
} ARAD_PP_IHP_LARGE_EM_PLDT_FORMAT2_AUTH_TBL_DATA;

typedef struct
{
  uint32 compare_valid;
  uint32 compare_key_data_location;
  uint32 compare_key_20_data;
  uint32 compare_key_20_mask;
  uint64 compare_payload_data;
  uint64 compare_payload_mask;
  uint32 compare_accessed_data;
  uint32 compare_accessed_mask;
  uint32 action_drop;
  uint64 action_transplant_payload_data;
  uint64 action_transplant_payload_mask;
  uint32 action_transplant_accessed_clear;
} ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA;

typedef struct
{
  uint32 aging_cfg_info_delete_entry;
  uint32 aging_cfg_info_create_aged_out_event;
  uint32 aging_cfg_info_create_refresh_event;
} ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA;


typedef struct
{
  uint32 parser_program_pointer_offset;
  uint32 parser_program_pointer_value;
  uint32 parser_program_pointer_profile;
  uint32 parser_program_pointer_value_to_use;
} ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_DATA;

typedef struct
{
  uint32 pp_port_packet_format_qualifier0_value;
  uint32 pp_port_parser_program_pointer_value;
} ARAD_PP_IHP_PP_PORT_VALUES_TBL_DATA;

typedef struct
{
  uint32 virtual_port_fem_bit_select;
} ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA;

typedef struct
{
  uint32 virtual_port_fem_map_index;
  uint32 virtual_port_fem_map_data;
} ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA;

typedef struct
{
  uint32 virtual_port_fem_field_select[SOC_DPP_DEFS_MAX(VIRTUAL_PORT_NOF_BITS)];
} ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA;

typedef struct
{
  uint32 parser_program_pointer_fem_bit_select;
} ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_DATA;

typedef struct
{
  uint32 parser_program_pointer_fem_map_index;
  uint32 parser_program_pointer_fem_map_data;
} ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_DATA;

typedef struct
{
  uint32 parser_program_pointer_fem_field_select[ARAD_PORTS_FEM_PARSER_PROGRAM_POINTER_SIZE];
} ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_DATA;

typedef struct
{
  uint32 next_addr_base;
  uint32 macro_sel;
} ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA;

typedef struct
{
  uint32 isem_type;
  uint32 isem_stamp;
  uint32 isem_key[2];
  uint32 isem_payload;
  uint32 isem_age_status;
  uint32 isem_self;
  uint32 isem_refreshed_by_dsp;
} ARAD_PP_IHP_ISEM_MANAGEMENT_REQUEST_TBL_DATA;

typedef struct
{
  uint32 vsi_or_isid;
} ARAD_PP_IHP_VSI_ISID_TBL_DATA;

typedef struct
{
  uint32 profile_index[4];
} ARAD_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA;

typedef struct
{
  uint32 vlan_port_member_line[8];
} ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA;

typedef struct
{
  uint32 type;
  uint32 double_data_entry;
  uint32 lif_table_entry[4];
} ARAD_PP_IHP_LIF_TABLE_TBL_DATA;


typedef struct
{
  uint32 destination;
  uint32 destination_valid;
  uint32 out_lif;
  uint32 vlan_edit_vid_2;
  uint32 vlan_edit_vid_1;
  uint32 vlan_edit_pcp_dei_profile;
  uint32 vlan_edit_profile;
  uint32 orientation_is_hub;
  uint32 cos_profile;
  uint32 in_lif_profile;
  uint32 out_lif_valid;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 use_in_lif;
  uint32 type;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 destination_valid;
  uint32 eei_or_out_lif;
  uint32 eei_or_out_lif_identifier;
  uint32 vlan_edit_vid_1;
  uint32 vlan_edit_pcp_dei_profile;
  uint32 vlan_edit_profile;
  uint32 orientation_is_hub;
  uint32 cos_profile;
  uint32 in_lif_profile;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 use_in_lif;
  uint32 type;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_DATA;

typedef struct
{
  uint32 destination;
  uint32 destination_valid;
  uint32 eei;
  uint32 eei_or_out_lif_type;
  uint32 vlan_edit_vid_1;
  uint32 vlan_edit_pcp_dei_profile;
  uint32 vlan_edit_profile;
  uint32 orientation_is_hub;
  uint32 cos_profile;
  uint32 in_lif_profile;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 use_in_lif;
  uint32 type;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_DATA;

typedef struct
{
  uint32 vsi;
  uint32 learn_info;
  uint32 tt_learn_enable;
  uint32 da_not_found_profile;
  uint32 vlan_edit_vid_2;
  uint32 vlan_edit_vid_1;
  uint32 vlan_edit_pcp_dei_profile;
  uint32 vlan_edit_profile;
  uint32 orientation_is_hub;
  uint32 cos_profile;
  uint32 in_lif_profile;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 use_in_lif;
  uint32 type;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA;

typedef struct
{
  uint32 orientation_is_hub;
  uint32 eei_or_out_lif_lsb;
  uint32 tpid_profile;
  uint32 cos_profile;
  uint32 vsi;
  uint32 eei_or_out_lif;
  uint32 destination;
  uint32 destination_valid;
  uint32 eei_or_out_lif_type;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 use_in_lif;
  uint32 service_type_lsb;
  uint32 type;
  uint32 sys_in_lif;
  uint32 in_lif_profile;
} ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA;

typedef struct
{
  uint32 orientation_is_hub;
  uint32 tpid_profile;
  uint32 cos_profile;
  uint32 vsi;
  uint32 da_not_found_profile;
  uint32 destination;
  uint32 destination_valid;
  uint32 tt_learn_enable;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 use_in_lif;
  uint32 service_type_lsb;
  uint32 type;
  uint32 sys_in_lif;
  uint32 in_lif_profile;
} ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA;

typedef struct
{
  uint32 tpid_profile;
  uint32 vsi;
  uint64 learn_info; 
  uint32 service_type;
  uint32 tt_learn_enable;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 orientation_is_hub;
  uint32 orientation_is_hub_valid;
  uint32 in_lif_profile;
  uint32 in_lif_profile_valid;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_DATA;

typedef struct
{
  uint32 orientation_is_hub;
  uint32 orientation_is_hub_valid; 
  uint32 model_is_pipe;
  uint32 in_lif_profile;
  uint32 tpid_profile;
  uint32 cos_profile;
  uint32 da_not_found_profile;
  uint32 vsi;
  uint32 in_rif;
  uint32 in_rif_valid;
  uint32 learn_info;
  uint32 use_in_lif;
  uint32 service_type;
  uint32 tt_learn_enable;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 skip_ethernet;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_DATA;

typedef struct
{
  uint32 orientation_is_hub;
  uint32 action_profile_index;
  uint32 model_is_pipe;
  uint32 termination_profile;
  uint32 in_lif_profile;
  uint32 tpid_profile;
  uint32 cos_profile;
  uint32 in_lif;
  uint32 eei_or_out_lif;
  uint32 destination;
  uint32 destination_valid;
  uint32 eei_or_out_lif_type;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 termination_profile_msb;
  uint32 type;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA;

typedef struct
{
  uint32 orientation_is_hub;
  uint32 action_profile_index;
  uint32 termination_profile_msb;
  uint32 termination_profile;
  uint32 in_lif_profile;
  uint32 tpid_profile;
  uint32 cos_profile;
  uint32 da_not_found_profile;
  uint32 vsi;
  uint64 learn_data;
  uint32 tt_learn_enable;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 in_lif_valid;
  uint32 type;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_DATA;

typedef struct
{
  uint32 orientation_is_hub;
  uint32 action_profile_index;
  uint32 model_is_pipe;
  uint32 termination_profile;
  uint32 in_lif_profile;
  uint32 tpid_profile;
  uint32 cos_profile;
  uint32 da_not_found_profile;
  uint32 vsi;
  uint32 service_type;
  uint32 in_rif;
  uint32 in_rif_valid;
  uint32 forwarding_code;
  uint32 expect_bos;
  uint32 destination;
  uint32 destination_valid;
  uint32 tt_learn_enable;
  uint32 vsi_assignment_mode;
  uint32 oam_lif_set;
  uint32 protection_path;
  uint32 protection_pointer;
  uint32 in_lif_valid;
  uint32 type;
  uint32 sys_in_lif;
} ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_DATA;



typedef struct
{
  uint32 ipv6_sem_offset;
  uint32 ipv6_add_offset_to_base;
  uint32 ipv6_opcode_valid;
  uint32 ipv4_sem_offset;
  uint32 ipv4_add_offset_to_base;
  uint32 ipv4_opcode_valid;
} ARAD_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA;

typedef struct
{
  uint32 tc_dp_sem_offset;
  uint32 tc_dp_add_offset_to_base;
  uint32 tc_dp_opcode_valid;
} ARAD_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA;

typedef struct
{
  uint32 pcp_dei_sem_offset;
  uint32 pcp_dei_add_offset_to_base;
  uint32 pcp_dei_opcode_valid;
} ARAD_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA;

typedef struct
{
  uint32 mpls_label_range_encountered;
} ARAD_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA;

typedef struct
{
  uint32 mpls_tunnel_termination_valid;
} ARAD_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA;

typedef struct
{
  uint32 ip_over_mpls_exp_mapping;
} ARAD_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA;

typedef struct
{
  uint32 in_pp_port_vt_profile;
  uint32 in_pp_port_vt_profile_mask;
  uint32 packet_format_code;
  uint32 packet_format_code_mask;
  uint32 packet_format_qualifier_0;
  uint32 packet_format_qualifier_0_mask;
  uint32 packet_format_qualifier_1;
  uint32 packet_format_qualifier_1_mask;
  uint32 packet_format_qualifier_2;
  uint32 packet_format_qualifier_2_mask;
  uint32 parser_leaf_context;
  uint32 parser_leaf_context_mask;
  uint32 ptc_vt_profile;
  uint32 ptc_vt_profile_mask;
  uint32 valid;
  uint32 llvp_prog_sel;
  uint32 incoming_tag_structure;
  uint32 incoming_tag_structure_mask;
} ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA;

typedef struct
{
  uint32 in_pp_port_tt_profile;
  uint32 in_pp_port_tt_profile_mask;
  uint32 packet_format_code;
  uint32 packet_format_code_mask;
  uint32 packet_format_qualifier_1;
  uint32 packet_format_qualifier_1_mask;
  uint32 packet_format_qualifier_2;
  uint32 packet_format_qualifier_2_mask;
  uint32 packet_format_qualifier_3;
  uint32 packet_format_qualifier_3_mask;
  uint32 packet_format_qualifier_3_outer_vid_bits;
  uint32 packet_format_qualifier_3_outer_vid_bits_mask;
  uint32 my_mac;
  uint32 my_mac_mask;
  uint32 my_b_mac_mc;
  uint32 my_b_mac_mc_mask;
  uint32 parser_leaf_context;
  uint32 parser_leaf_context_mask;
  uint32 ptc_tt_profile;
  uint32 ptc_tt_profile_mask;
  uint32 da_is_all_r_bridges;
  uint32 da_is_all_r_bridges_mask;
  uint32 isa_lookup_found;
  uint32 isa_lookup_found_mask;
  uint32 isb_lookup_found;
  uint32 isb_lookup_found_mask;
  uint32 tcam_lookup_match;
  uint32 tcam_lookup_match_mask;
  uint32 packet_is_compatible_mc;
  uint32 packet_is_compatible_mc_mask;
  uint32 vlan_is_designated;
  uint32 vlan_is_designated_mask;
  uint32 vt_processing_profile;
  uint32 vt_processing_profile_mask;
  uint32 vt_in_lif_profile;
  uint32 vt_in_lif_profile_mask;
  uint32 enable_routing;
  uint32 enable_routing_mask;
  uint32 valid;
  uint32 llvp_prog_sel;
} ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA;

   
typedef struct
{
  uint32 key_16b_inst0;
  uint32 key_16b_inst1;
  uint32 key_16b_inst2; 
  uint32 key_16b_inst3; 
  uint32 key_32b_inst0;
  uint32 key_32b_inst1;
  uint32 isa_key_valids_1;
  uint32 isb_key_valids_1;
  uint32 tcam_key_valids_1;
  uint32 key_program_variable;
  uint32 dbal;
} ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA;

typedef struct
{
  uint32 key_16b_inst2; 
  uint32 key_16b_inst3; 
  uint32 key_16b_inst4; 
  uint32 key_16b_inst5; 
  uint32 key_16b_inst6; 
  uint32 key_16b_inst7; 
  uint32 key_32b_inst2;
  uint32 key_32b_inst3;
  uint32 isa_key_valids_2;
  uint32 isb_key_valids_2;
  uint32 tcam_key_valids_2;
  uint32 isa_and_mask;
  uint32 isa_or_mask;
  uint32 isa_lookup_enable;
  uint32 isb_and_mask;
  uint32 isb_or_mask;
  uint32 isb_lookup_enable;
  uint32 tcam_db_profile;
  uint32 vlan_translation_0_pd_bitmap;
  uint32 vlan_translation_1_pd_bitmap;
  uint32 dbal;
#ifdef BCM_88660_A0
  uint32 vlan_translation_is_use_strength;
  uint32 vlan_translation_0_pd_tcam_strength;
  uint32 vlan_translation_0_pd_isa_strength;
  uint32 vlan_translation_0_pd_isb_strength;
  uint32 vlan_translation_1_pd_tcam_strength;
  uint32 vlan_translation_1_pd_isa_strength;
  uint32 vlan_translation_1_pd_isb_strength;
#endif
  uint32 processing_profile;
} ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA;

typedef struct
{
  uint32 key_16b_inst0;
  uint32 key_16b_inst1;
  uint32 key_32b_inst0;
  uint32 key_32b_inst1;
  uint32 isa_key_valids_1;
  uint32 isb_key_valids_1;
  uint32 tcam_key_valids_1;
  uint32 key_program_variable;
  uint32 key_16b_inst2;
  uint32 key_16b_inst3;
  uint32 key_16b_inst4; 
  uint32 key_16b_inst5; 
  uint32 key_16b_inst6; 
  uint32 key_16b_inst7; 
  uint32 key_32b_inst2;
  uint32 key_32b_inst3;
  uint32 isa_key_valids_2;
  uint32 isb_key_valids_2;
  uint32 tcam_key_valids_2;
  uint32 isa_and_mask;
  uint32 isa_or_mask;
  uint32 isa_lookup_enable;
  uint32 isb_and_mask;
  uint32 isb_or_mask;
  uint32 isb_lookup_enable;
  uint32 tcam_db_profile;
  uint32 vlan_translation_0_pd_bitmap;
  uint32 vlan_translation_1_pd_bitmap;
  uint32 dbal;
#ifdef BCM_88660_A0
  uint32 vlan_translation_is_use_strength;
  uint32 vlan_translation_0_pd_tcam_strength;
  uint32 vlan_translation_0_pd_isa_strength;
  uint32 vlan_translation_0_pd_isb_strength;
  uint32 vlan_translation_1_pd_tcam_strength;
  uint32 vlan_translation_1_pd_isa_strength;
  uint32 vlan_translation_1_pd_isb_strength;
#endif
  uint32 processing_profile;
} ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA;

typedef struct
{
  uint32 key_16b_inst0;
  uint32 key_16b_inst1;
  uint32 key_32b_inst0;
  uint32 key_32b_inst1;
  uint32 isa_key_valids_1;
  uint32 isb_key_valids_1;
  uint32 tcam_key_valids_1;
  uint32 key_program_variable;
  uint32 key_16b_inst2;
  uint32 key_16b_inst3;
  uint32 key_16b_inst4;
  uint32 key_16b_inst5;
  uint32 key_16b_inst6;
  uint32 key_16b_inst7;
  uint32 key_32b_inst2;
  uint32 key_32b_inst3;
  uint32 isa_key_valids_2;
  uint32 isb_key_valids_2;
  uint32 tcam_key_valids_2;
  uint32 isa_and_mask;
  uint32 isa_or_mask;
  uint32 isa_lookup_enable;
  uint32 isb_and_mask;
  uint32 isb_or_mask;
  uint32 isb_lookup_enable;
  uint32 tcam_db_profile;
  uint32 termination_0_pd_bitmap;
  uint32 termination_1_pd_bitmap;
  uint32 processing_profile;
  uint32 isa_key_initial_from_vt;
  uint32 isb_key_initial_from_vt;
  uint32 tcam_key_initial_from_vt;
  uint32 processing_code;
  uint32 result_to_use_0;
  uint32 result_to_use_1;
  uint32 result_to_use_2;
  uint32 second_stage_parsing;
  uint32 dbal;
} ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA;

typedef struct
{
  uint32 pbb_cfm_max_level;
  uint32 pbb_cfm_trap_valid;
} ARAD_PP_IHP_PBB_CFM_TABLE_TBL_DATA;

typedef struct
{
  uint32 sem_result_accessed;
} ARAD_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA;

typedef struct
{
  uint32 physical_port_mine0;
  uint32 physical_port_mine1;
  uint32 physical_port_mine2;
  uint32 physical_port_mine3;
} ARAD_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA;


typedef struct
{
  uint32 fec_entry_accessed;
} ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA;

typedef struct
{
  uint32 program11;
  uint32 program10;
  uint32 program01;
  uint32 program00;
} ARAD_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA;

typedef struct
{
  uint32 key_a_instruction0;
  uint32 key_a_instruction1;
  uint32 key_b_instruction0;
  uint32 key_b_instruction1;
  uint32 key_b_instruction2;
  uint32 key_b_instruction3;
  uint32 elk_lkp_valid;
  uint32 lem_1st_lkp_valid;
  uint32 lem_1st_lkp_key_select;
  uint32 lem_1st_lkp_key_type;
  uint32 lem_1st_lkp_and_value;
  uint32 lem_1st_lkp_or_value;
  uint32 lem_2nd_lkp_valid;
  uint32 lem_2nd_lkp_key_select;
  uint32 lem_2nd_lkp_and_value;
  uint32 lem_2nd_lkp_or_value;
  uint32 lpm_1st_lkp_valid;
  uint32 lpm_1st_lkp_and_value;
  uint32 lpm_1st_lkp_or_value;
  uint32 lpm_2nd_lkp_valid;
  uint32 lpm_2nd_lkp_and_value;
  uint32 lpm_2nd_lkp_or_value;
  uint32 tcam_key_size;
  uint32 tcam_bank_valid;
  uint32 tcam_and_value;
  uint32 tcam_or_value;
  uint32 data_processing_profile;
} ARAD_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA;

typedef struct
{
  uint32 elk_lkp_valid;
  uint32 elk_wait_for_reply;
  uint32 elk_opcode;
  uint32 elk_key_a_valid_bytes;
  uint32 elk_key_b_valid_bytes;
  uint32 elk_key_c_valid_bytes;
  uint32 elk_packet_data_select;
  uint32 lem_1st_lkp_valid;
  uint32 lem_1st_lkp_key_select;
  uint32 lem_1st_lkp_key_type;
  uint32 lem_1st_lkp_and_value;
  uint32 lem_1st_lkp_or_value;
  uint32 lem_2nd_lkp_valid;
  uint32 lem_2nd_lkp_key_select;
  uint32 lem_2nd_lkp_and_value;
  uint32 lem_2nd_lkp_or_value;
  uint32 lpm_1st_lkp_valid;
  uint32 lpm_1st_lkp_key_select;
  uint32 lpm_1st_lkp_and_value;
  uint32 lpm_1st_lkp_or_value;
  uint32 lpm_2nd_lkp_valid;
  uint32 lpm_2nd_lkp_key_select;
  uint32 lpm_2nd_lkp_and_value;
  uint32 lpm_2nd_lkp_or_value;
  uint32 tcam_lkp_key_select;
  uint32 tcam_lkp_db_profile;
  uint32 tcam_traps_lkp_db_profile_0;
  uint32 tcam_traps_lkp_db_profile_1;
  uint32 enable_tcam_identification_ieee_1588;
  uint32 enable_tcam_identification_oam;
  uint32 learn_key_select;

  
  uint32 elk_key_d_lsb_valid_bytes;
  uint32 elk_key_a_msb_valid_bytes;
  uint32 elk_key_b_msb_valid_bytes;
  uint32 elk_key_c_msb_valid_bytes;
  uint32 elk_key_d_msb_valid_bytes;
  uint32 lpm_public_1st_lkp_valid;
  uint32 lpm_public_1st_lkp_key_select;
  uint32 lpm_public_1st_lkp_key_size;
  uint32 lpm_public_1st_lkp_and_value;
  uint32 lpm_public_1st_lkp_or_value;
  uint32 lpm_public_2nd_lkp_valid;
  uint32 lpm_public_2nd_lkp_key_select;
  uint32 lpm_public_2nd_lkp_key_size;
  uint32 lpm_public_2nd_lkp_and_value;
  uint32 lpm_public_2nd_lkp_or_value;
  uint32 tcam_lkp_key_select_1;
  uint32 tcam_lkp_db_profile_1;

  
  uint32 Lpm_1stLkpType;
  uint32 Lpm_2ndLkpType;
  uint32 LpmPublic_1stLkpType;
  uint32 LpmPublic_2ndLkpType;

} ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA;

typedef struct
{
  uint32 instruction_0_16b;
  uint32 instruction_1_16b;
  uint32 instruction_2_16b;
  uint32 instruction_3_32b; 
  uint32 instruction_4_32b;
  uint32 instruction_5_32b;
  uint32 key_a_inst_0_to_5_valid; 
  uint32 key_b_inst_0_to_5_valid; 
  uint32 key_c_inst_0_to_5_valid; 

  
  uint32 instruction_6_32b;
  uint32 instruction_7_32b;
  uint32 key_d_inst_0_to_7_valid; 

} ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA;

typedef struct
{
  uint32 elk_result_format;
  uint32 include_elk_fwd_in_result_a;
  uint32 include_lem_2nd_in_result_a;
  uint32 include_tcam_in_result_a;
  uint32 include_lpm_2nd_in_result_a;
  uint32 include_elk_ext_in_result_a;
  uint32 include_lem_1st_in_result_a;
  uint32 include_lpm_1st_in_result_a;
  uint32 select_default_result_a;
  uint32 result_a_format;
  uint32 include_elk_fwd_in_result_b;
  uint32 include_lem_2nd_in_result_b;
  uint32 include_tcam_in_result_b;
  uint32 include_lpm_2nd_in_result_b;
  uint32 include_elk_ext_in_result_b;
  uint32 include_lem_1st_in_result_b;
  uint32 include_lpm_1st_in_result_b;
  uint32 select_default_result_b;
  uint32 result_b_format;
  uint32 not_found_trap_code;
  uint32 not_found_trap_strength;
  uint32 not_found_snoop_strength;
  uint32 sa_lkp_result_select;
  uint32 apply_fwd_result_a;
  uint32 sa_lkp_process_enable;
  uint32 procedure_ethernet_default;
  uint32 unknown_address;
  uint32 enable_hair_pin_filter;
  uint32 enable_rpf_check;
  uint32 compatible_mc_bridge_fallback;
  uint32 enable_lsr_p2p_service;
  uint32 learn_enable;
  uint32 fwd_processing_profile;

  
  uint32 include_tcam_1_in_result_a;
  uint32 include_tcam_1_in_result_b;
  uint32 lpm_1st_lkp_enable_default;
  uint32 lpm_2nd_lkp_enable_default;
  uint32 lpm_public_1st_lkp_enable_default;
  uint32 lpm_public_2nd_lkp_enable_default;
  uint32 elk_fwd_lkp_enable_default;
  uint32 elk_ext_lkp_enable_default;

} ARAD_PP_IHB_FLP_PROCESS_TBL_DATA;

typedef struct
{
  uint32 parser_leaf_context;
  uint32 port_profile;
  uint32 ptc_profile;
  uint32 packet_format_code;
  uint32 forwarding_header_qualifier;
  uint32 forwarding_code;
  uint32 forwarding_offset_index;
  uint8  l_3_vpn_default_routing;  
  uint32 trill_mc;
  uint32 packet_is_compatible_mc;
  uint8  in_rif_uc_rpf_enable;
  uint32 ll_is_arp;
  uint32 elk_status;
  uint32 cos_profile;
  uint32 service_type;
  uint32 vt_processing_profile;
  uint32 vt_lookup_0_found;
  uint32 vt_lookup_1_found;
  uint32 tt_processing_profile;
  uint32 tt_lookup_0_found;
  uint32 tt_lookup_1_found;
  uint32 parser_leaf_context_mask;
  uint32 port_profile_mask;
  uint32 ptc_profile_mask;
  uint32 packet_format_code_mask;
  uint32 forwarding_header_qualifier_mask;
  uint32 forwarding_code_mask;
  uint32 forwarding_offset_index_mask;
  uint32 l_3_vpn_default_routing_mask;
  uint32 trill_mc_mask;
  uint32 packet_is_compatible_mc_mask;
  uint32 in_rif_uc_rpf_enable_mask;
  uint32 ll_is_arp_mask;
  uint32 elk_status_mask;
  uint32 cos_profile_mask;
  uint32 service_type_mask;
  uint32 vt_processing_profile_mask;
  uint32 vt_lookup_0_found_mask;
  uint32 vt_lookup_1_found_mask;
  uint32 tt_processing_profile_mask;
  uint32 tt_lookup_0_found_mask;
  uint32 tt_lookup_1_found_mask;
  uint32 program;
  uint32 valid;

  
  uint32 forwarding_offset_index_ext;
  uint32 cpu_trap_code;
  uint32 in_lif_profile;
  uint32 llvp_incoming_tag_structure;
   
  uint32 forwarding_plus_1_header_qualifier;
  uint32 tunnel_termination_code;
  uint32 qualifier_0;
  uint32 in_lif_data_index;
  uint32 in_lif_data_msb;
  uint32 forwarding_offset_index_ext_mask;
  uint32 cpu_trap_code_mask;
  uint32 in_lif_profile_mask;
  uint32 llvp_incoming_tag_structure_mask;
   
  uint32 forwarding_plus_1_header_qualifier_mask;
  uint32 tunnel_termination_code_mask;
  uint32 qualifier_0_mask;
  uint32 in_lif_data_index_mask;
  uint32 in_lif_data_msb_mask;
  uint8  custom_rif_profile_bit;   
  uint32 custom_rif_profile_bit_mask; 

  uint8  lsb_rif_profile_bit;
  uint32 lsb_rif_profile_bit_mask; 

  
  uint32 pemContext;
  uint32 pemContextMask;
  uint32 programData;
  uint32 pemContextValue;
  uint32 pemContextValid;


} ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA;

typedef struct
{
  uint32 lpm;
} ARAD_PP_IHB_LPM_TBL_DATA;

typedef struct
{
  uint32 cpu_trap_code_lsb;
  uint32 snp;
  uint32 fwd;
} ARAD_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA;

typedef struct
{
  uint32 pinfo_pmf_key_gen_var;
} ARAD_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA;

typedef struct
{
  uint32 program_selection_map;
} ARAD_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA;


typedef struct
{
  uint32 first_pass_key_profile_index;
  uint32 second_pass_key_profile_index;
  uint32 tag_selection_profile_index;
  uint32 bytes_to_remove_header;
  uint32 bytes_to_remove_offset;
  uint32 system_header_profile;
  uint32 copy_program_variable;
} ARAD_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA;

typedef struct
{
  uint32 inst_valid;
  uint32 inst_source_select;
  uint32 inst_header_offset_select;
  uint32 inst_niblle_field_offset;
  uint32 inst_bit_count;
} ARAD_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA;

typedef struct
{
  uint32 inst0_valid;
  uint32 inst0_source_select;
  uint32 inst0_header_offset_select;
  uint32 inst0_niblle_field_offset;
  uint32 inst0_bit_count;
  uint32 inst1_valid;
  uint32 inst1_source_select;
  uint32 inst1_header_offset_select;
  uint32 inst1_niblle_field_offset;
  uint32 inst1_bit_count;
} ARAD_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA;

typedef struct
{
  uint32 bank_key_select[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
  uint32 bank_db_id_and_value[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
  uint32 bank_db_id_or_value[SOC_DPP_DEFS_MAX_TCAM_NOF_BANKS];
  uint32 tcam_pd1_members;
  uint32 tcam_pd2_members;
  uint32 tcam_sel3_member;
  uint32 tcam_sel4_member;
} ARAD_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA;

typedef struct
{
  uint32 direct_action_table;
} ARAD_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA;

typedef struct
{
  uint32 found;
  uint32 address;
} __ATTRIBUTE_PACKED__ ARAD_PP_IHB_TCAM_COMPARE_DATA;

typedef struct
{
  uint32 vbe;   
  uint32 mskdatae; 
  uint32 wr; 
  uint32 rd; 
  uint32 cmp; 
  uint32 flush; 
  uint32 val[ARAD_PP_IHB_TCAM_DATA_WIDTH]; 
  uint32 vbi; 
  uint32 cfg; 
  uint32 hqsel; 
  uint32 mask[ARAD_PP_IHB_TCAM_DATA_WIDTH]; 
} __ATTRIBUTE_PACKED__ ARAD_PP_IHB_TCAM_TBL_SET_DATA;

typedef struct
{
  uint32 vbo;  
  uint32 dataout[ARAD_PP_IHB_TCAM_DATA_WIDTH];  
  uint32 hadr;  
  uint32 hit;  
  uint32 mhit;  
  uint32 mask[ARAD_PP_IHB_TCAM_DATA_WIDTH];  
} __ATTRIBUTE_PACKED__ ARAD_PP_IHB_TCAM_TBL_GET_DATA;

typedef struct
{
  uint32 action;  
} ARAD_PP_IHB_TCAM_ACTION_TBL_DATA;

typedef struct
{
  uint32 ac_p2p_to_ac_vsi_profile;  
  uint32 ac_p2p_to_pwe_vsi_profile;  
  uint32 ac_p2p_to_pbb_vsi_profile;  
  uint32 label_pwe_p2p_vsi_profile;  
} ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_DATA;

typedef struct
{
  uint32 lb_key_max;
  uint32 lb_key_min;
  uint32 second_range_lb_key_max;
  uint32 second_range_lb_key_min;
  uint32 is_stacking_port;
  uint32 peer_tm_domain_id;
  uint32 port_type;
  uint32 cnm_intrcpt_fc_vec_llfc;
  uint32 cnm_intrcpt_fc_vec_pfc;  
  uint32 cnm_intrcpt_fc_en;
  uint32 cnm_intrcpt_drop_en;
  uint32 ad_count_out_port_flag;
  uint32 cgm_port_profile; 
  uint32 cgm_interface;
  uint32 base_q_pair_num;
  uint32 cos_map_profile;
  uint32 pmf_data;
  uint32 disable_filtering;
} ARAD_EGQ_PPCT_TBL_DATA;

#ifdef BCM_88660_A0 

typedef struct
{  
  uint32 lb_key_max;
  uint32 lb_key_min;
} ARAD_PER_PORT_LB_RANGE_TBL_DATA;

#endif 

typedef struct
{
  uint32 acceptable_frame_type_profile;
  uint32 port_is_pbp;
  uint32 egress_stp_filter_enable;
  uint32 egress_vsi_filter_enable;
  uint32 disable_filter;
  uint32 default_sem_result;
  uint32 exp_map_profile;
  uint32 vlan_domain;
  uint32 cep_c_vlan_edit;
  uint32 llvp_profile;
  uint32 mpls_eth_type_select;
  uint32 prge_profile;
  uint32 prge_var;
  uint32 dst_system_port;
  uint32 mtu;
  uint32 ieee_1588_mac_enable;
  uint32 oam_port_profile;
  uint32 reserved;
} ARAD_PP_EPNI_PP_PCT_TBL_DATA;

typedef struct
{
  uint32 tpid_profile_link;
  uint32 edit_command_outer_vid_source;
  uint32 edit_command_outer_pcp_dei_source;
  uint32 edit_command_inner_vid_source;
  uint32 edit_command_inner_pcp_dei_source;
  uint32 edit_command_bytes_to_remove;
  uint32 outer_tpid_ndx;
  uint32 inner_tpid_ndx;
} ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA;

typedef struct
{
  uint32 eep_orientation;
} ARAD_PP_EGQ_EEP_ORIENTATION_TBL_DATA;

typedef struct
{
  uint32 cfm_max_level;
  uint32 cfm_trap_valid;
} ARAD_PP_EGQ_CFM_TRAP_TBL_DATA;

typedef struct
{
  uint32 entry[3];
} ARAD_PP_EPNI_ENCAPSULATION_DB_BANK2_TBL_DATA;

typedef struct
{
  uint32 pcp_dei_map;
} ARAD_PP_EPNI_PCP_DEI_MAP_TBL_DATA;

typedef struct
{
  uint32 dscp_exp_to_pcp_dei_map;
} ARAD_PP_DSCP_EXP_TO_PCP_DEI_TABLE_DATA;

typedef struct
{
  uint32 pcp_dei;
} ARAD_PP_EPNI_DP_MAP_TBL_DATA;

typedef struct
{
  uint32 outer_tpid_index;
  uint32 inner_tpid_index;
  uint32 outer_vid_source;
  uint32 inner_vid_source;
  uint32 outer_pcp_dei_source;
  uint32 inner_pcp_dei_source;
  uint32 tags_to_remove;
  uint32 packet_is_tagged_after_eve;
} ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA;

typedef struct
{
  uint32 program_index;
  uint32 new_header_size;
  uint32 add_network_header;
  uint32 remove_network_header;
  uint32 system_header_size;
} ARAD_PP_EPNI_PROGRAM_VARS_TBL_DATA;

typedef struct
{
  uint32 tpid_profile_link;
  uint32 edit_command_outer_vid_source;
  uint32 edit_command_outer_pcp_dei_source;
  uint32 edit_command_inner_vid_source;
  uint32 edit_command_inner_pcp_dei_source;
  uint32 edit_command_bytes_to_remove; 
  uint32 outer_tpid_ndx;
  uint32 inner_tpid_ndx;
} ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA;

typedef struct
{
  uint32 ip_exp_map;
} ARAD_PP_EPNI_IP_EXP_MAP_TBL_DATA;

typedef struct
{
  uint32 etpp_debug;
} ARAD_PP_EPNI_ETPP_DEBUG_TBL_DATA;


typedef struct
{
  uint32 path_select[8];
} ARAD_PP_IHB_PATH_SELECT_TBL_DATA;

typedef struct
{
  uint32 stp[8];
} ARAD_PP_EPNI_STP_TBL_DATA;

typedef struct
{
  uint32 src_system_port_fem_field_select[ARAD_PORTS_FEM_SYSTEM_PORT_SIZE];
} ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA;

typedef struct
{
  uint32 per_port_stp_state[(SOC_DPP_DEFS_MAX(NOF_TOPOLOGY_IDS) * 2)/32];
} ARAD_PP_IHP_STP_TABLE_TBL_DATA;

typedef struct
{
  uint32 mask[ARAD_PP_IHB_TCAM_DATA_WIDTH];
  uint32 value[ARAD_PP_IHB_TCAM_DATA_WIDTH];
  uint32 valid;
} ARAD_PP_IHB_TCAM_BANK_TBL_DATA;

typedef struct
{
  uint32 action_select;
  uint32 enable_update_key;
  uint32 key_d_use_compare_result;
  uint32 key_d_mask_select;
  uint32 key_d_xor_enable;
  uint32 key_a_lem_operation_select;
  uint32 key_b_lem_operation_select;
  uint32 key_c_lem_operation_select;
  uint32 key_d_lem_operation_select;
} ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_DATA;


typedef struct
{
  uint32 action_bitmap;
  uint32 key_size;
  uint32 is_direct;
  uint32 prefix_or;
  uint32 prefix_and;
} ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA;

typedef struct
{
  uint32 bitmap;  
} ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA;

typedef struct
{
  uint32 vsi_membership[8];
} ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA;

typedef struct 
{
  uint32 entry[8];
} ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA;

typedef struct 
{
  soc_reg_above_64_val_t vrid_my_mac_map;
} ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA;



typedef struct
{
  uint32 out_lif;
  uint32 destination;
  uint32 link_ptr;
} ARAD_PP_IRR_MCDB_TBL_DATA;

typedef struct
{
  uint32 outlif_1;
  uint32 pp_dsp_1a;
  uint32 pp_dsp_1b;
  uint32 link_ptr;
  uint32 entry_format;
} ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_DATA;

typedef struct
{
  uint32 outlif_1;
  uint32 bmp_ptr;
  uint32 link_ptr;
  uint32 entry_format;
} ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_DATA;

typedef struct
{
  uint32 outlif_1;
  uint32 outlif_2;
  uint32 link_ptr;
  uint32 entry_format;
} ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_DATA;

typedef struct
{
  uint32 outlif_1;
  uint32 pp_dsp_1;
  uint32 outlif_2;
  uint32 pp_dsp_2;
  uint32 entry_format;
} ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_DATA;

typedef struct
{
  uint32 outlif_1;
  uint32 pp_dsp_1;
  uint32 outlif_2;
  uint32 pp_dsp_2;
  uint32 entry_format;
} ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_DATA;

typedef struct
{
  uint32 outlif_1;
  uint32 outlif_2;
  uint32 outlif_3;
  uint32 entry_format;
} ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_DATA;

typedef struct
{
  uint32 outlif_1;
  uint32 outlif_2;
  uint32 outlif_3;
  uint32 entry_format;
} ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_DATA;

typedef struct
{
  uint32 mc_bmp_1;
  uint32 mc_bmp_2;
  uint32 mc_bmp_3;
  uint32 mc_bmp_4;
  uint32 mc_bmp_5;
  uint32 mc_bmp_6;
  uint32 mc_bmp_7;
  uint32 mc_bmp_8;
  uint32 reserved;
} ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_DATA;

typedef struct
{
  uint32 pp_dsp_1;
  uint32 pp_dsp_2;
  uint32 pp_dsp_3;
  uint32 pp_dsp_4;
  uint32 link_ptr;
  uint32 reserved;
} ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_DATA;

typedef struct
{
  uint32 vlan_classification_profile;
  uint32 pcp_profile;
  uint32 ignore_incoming_pcp;
  uint32 tc_dp_tos_enable;
  uint32 tc_dp_tos_index;
  uint32 tc_l_4_protocol_enable;
  uint32 tc_subnet_enable;
  uint32 tc_port_protocol_enable;
  uint32 default_traffic_class;
  uint32 ignore_incoming_up;
  uint32 incoming_up_map_profile;
  uint32 use_dei;
  uint32 drop_precedence_profile;
  uint32 sa_lookup_enable;
  uint32 action_profile_sa_drop_index;
  uint32 enable_sa_authentication;
  uint32 ignore_incoming_vid;
  uint32 vid_ipv4_subnet_enable;
  uint32 vid_port_protocol_enable;
  uint32 protocol_profile;
  uint32 default_initial_vid;
  uint32 non_authorized_mode_8021x;
  uint32 reserved_mc_trap_profile;
  uint32 enable_arp_trap;
  uint32 enable_igmp_trap;
  uint32 enable_mld_trap;
  uint32 enable_dhcp_trap;
  uint32 general_trap_enable;
  uint32 default_cpu_trap_code;
  uint32 default_action_profile_fwd;
  uint32 default_action_profile_snp;
} ARAD_PP_IHP_PINFO_LLR_TBL_DATA;

typedef struct
{
  uint32 incoming_vid_exist;
  uint32 incoming_tag_exist;
  uint32 incoming_tag_index;
  uint32 incoming_s_tag_exist;
  uint32 acceptable_frame_type_action;
  uint32 outer_comp_index;
  uint32 inner_comp_index;
} ARAD_PP_IHP_LLR_LLVP_TBL_DATA;

typedef struct
{
  uint32 step;
  uint32 write_line;
  uint32 write_valid;
  uint32 offset;
  uint32 read_line;
} ARAD_PP_IHP_ECM_STEP_TBL_DATA;

typedef struct
{
  uint32 ll_mirror_profile;
} ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA;

typedef struct
{
  uint32 ipv4_subnet;
  uint32 ipv4_subnet_mask;
  uint32 ipv4_subnet_vid;
  uint32 ipv4_subnet_tc;
  uint32 ipv4_subnet_tc_valid;
  uint32 ipv4_subnet_valid;
} ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_DATA;

typedef struct
{
  uint32 vid;
  uint32 vid_valid;
  uint32 traffic_class;
  uint32 traffic_class_valid;
} ARAD_PP_IHP_PORT_PROTOCOL_TBL_DATA;

typedef struct
{
  uint32 valid;
  uint32 tc;
  uint32 dp;
} ARAD_PP_IHP_TOS_2_COS_TBL_DATA;

typedef struct
{
  uint32 fwd;
  uint32 snp;
  uint32 cpu_trap_code;
} ARAD_PP_IHP_RESERVED_MC_TBL_DATA;

typedef struct
{
  uint32 pp_port_outer_header_start;
  uint32 tpid_profile;
  uint32 default_parser_program_pointer;
  uint32 default_pfq_0;
  uint32 parser_custom_macro_select;
  uint32 mpls_next_address_use_specualte;
} ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA;

typedef struct
{
  uint32 src_system_port_fem_bit_select;
} ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA;

typedef struct
{
  uint32 src_system_port_fem_map_index;
  uint32 src_system_port_fem_map_data;
} ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA;

typedef struct
{
  uint32 packet_format_code;
  uint32 parser_leaf_context;
} ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA;

typedef struct
{
  uint32 cstm_word_select;
  uint32 cstm_mask_left;
  uint32 cstm_mask_right;
  uint32 cstm_condition_select;
  uint32 cstm_comparator_mode;
  uint32 cstm_comparator_mask;
  uint32 cstm_shift_sel;
  uint32 cstm_shift_a;
  uint32 cstm_shift_b;
  uint32 augment_previous_stage;
  uint32 qualifier_or_mask;
  uint32 qualifier_and_mask;
  uint32 header_qualifier_mask;
  uint32 qualifier_shift;
} ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA;

typedef struct
{
  uint32 eth_type_protocol;
  uint32 eth_sap_protocol;
} ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA;

typedef struct
{
  uint32 ip_protocol;
  uint32 extension_valid;
} ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA;

typedef struct
{
  uint32 cstm_protocol;
} ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA;

typedef struct
{
  uint32 lsb[8];
} ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA;

typedef struct
{
  uint32 fid_class[4];
} ARAD_PP_IHP_BVD_FID_CLASS_TBL_DATA;

typedef struct
{
  uint32 fid;
} ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA;

typedef struct
{
  uint32 vlan_range_lower_limit;
  uint32 vlan_range_upper_limit;
} ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA;

typedef struct
{
  uint32 range_valid_0;
  uint32 range_valid_1;
} ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA;

typedef struct
{
  uint32 designated_vlan;
} ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA;

typedef struct
{
  uint32 vlan_domain;
  uint32 vlan_translation_profile;
  uint32 mapped_pp_port;
  uint32 termination_profile;
  uint32 designated_vlan_index;
  uint32 default_port_lif;
  uint32 all_vlan_member;
  uint32 learn_lif;
  uint32 use_in_lif;
  uint32 enable_my_nick_name_bitmap; 
} ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA;

typedef struct
{
  uint32 incoming_tag_structure;
  uint32 ivec;
  uint32 pcp_dei_profile;
} ARAD_PP_IHP_VTT_LLVP_TBL_DATA;

typedef struct
{
  uint32 ivec;
} ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA;

typedef struct
{
  uint32 pcp_dei;
} ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA;

typedef struct
{
  uint32 vrf;
  uint32 uc_rpf_enable;
  uint32 enable_routing_mc;
  uint32 enable_routing_uc;
  uint32 enable_routing_mpls;
  uint32 cos_profile;
  uint32 l_3_vpn_default_routing;
  uint32 rif_profile;  
  
} ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA;

typedef struct
{
  uint32 traffic_class_0;
  uint32 drop_precedence_0;
  uint32 traffic_class_1;
  uint32 drop_precedence_1;
  uint32 traffic_class_2;
  uint32 drop_precedence_2;
  uint32 traffic_class_3;
  uint32 drop_precedence_3;
} ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA;

typedef struct
{
  uint32 ecmp_lb_key_count;
  uint32 lag_lb_key_start;
  uint32 lag_lb_key_count;
  uint32 lb_profile;
  uint32 lb_bos_search;
  uint32 lb_include_bos_hdr;
  uint32 icmp_redirect_enable;
} ARAD_PP_IHB_PINFO_FER_TBL_DATA;

typedef struct
{
  uint32 lb_vector_index_1;
  uint32 lb_vector_index_2;
  uint32 lb_vector_index_3;
  uint32 lb_vector_index_4;
  uint32 lb_vector_index_5;
} ARAD_PP_IHB_LB_PFC_PROFILE_TBL_DATA;

typedef struct
{
  uint64 chunk_bitmap;
  uint32 chunk_size;
} ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA;


#define ARAD_PP_IHB_FEC_FORMAT_A (0) 
#define ARAD_PP_IHB_FEC_FORMAT_B (4) 
#define ARAD_PP_IHB_FEC_FORMAT_C (5) 
#define ARAD_PP_IHB_FEC_FORMAT_NULL (3) 

typedef struct
{
  uint32 destination;
  uint32 uc_rpf_mode;
  uint32 mc_rpf_mode;
  uint32 format; 
  uint32 forwarding_info;
  uint32 eei; 
  uint32 out_lif; 
  uint32 arp_ptr; 
} ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA;

typedef struct
{
  uint32 group_start_pointer;
  uint32 group_size;
  uint32 is_protected;
  uint32 rpf_fec_pointer;
} ARAD_PP_IHB_FEC_ECMP_TBL_DATA;

typedef struct
{
  uint32 protection_pointer;
  uint32 data0[ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE];
  uint32 data1[ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE]; 
} ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA;

typedef struct
{
  uint32 destination_valid[8];
} ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA;

typedef struct
{
  uint32 fwd_act_trap;
  uint32 fwd_act_destination;
  uint32 fwd_act_destination_overwrite;
  uint32 fwd_act_out_lif;
  uint32 fwd_act_out_lif_overwrite;
  uint32 fwd_act_eei;
  uint32 fwd_act_eei_overwrite;
  uint32 fwd_act_destination_add_vsi;
  uint32 fwd_act_destination_add_vsi_shift;
  uint32 fwd_act_traffic_class;
  uint32 fwd_act_traffic_class_overwrite;
  uint32 fwd_act_drop_precedence;
  uint32 fwd_act_drop_precedence_overwrite;
  uint32 fwd_act_meter_pointer_a_overwrite;
  uint32 fwd_act_meter_pointer_b_overwrite;
  uint32 fwd_act_meter_pointer;
  uint32 fwd_act_meter_pointer_update;
  uint32 fwd_act_counter_pointer;
  uint32 fwd_act_counter_pointer_a_overwrite;
  uint32 fwd_act_counter_pointer_b_overwrite;
  uint32 fwd_act_counter_pointer_update;
  uint32 fwd_act_drop_precedence_meter_command;
  uint32 fwd_act_drop_precedence_meter_command_overwrite;
  uint32 fwd_act_forwarding_code;
  uint32 fwd_act_forwarding_code_overwrite;
  uint32 fwd_act_fwd_offset_index;
  uint32 fwd_act_fwd_offset_fix;
  uint32 fwd_act_fwd_offset_overwrite;
  uint32 fwd_act_ethernet_policer_pointer;
  uint32 fwd_act_ethernet_policer_pointer_overwrite;
  uint32 fwd_act_learn_disable;
  uint32 fwd_act_bypass_filtering;
  uint32 fwd_act_da_type_overwrite;
  uint32 fwd_act_da_type;
} ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_DATA;

typedef struct
{
  uint32 snoop_action;
} ARAD_PP_IHB_SNOOP_ACTION_TBL_DATA;

typedef struct
{
  uint32 program_selection_profile; 
  uint32 action_profile_sa_drop_index;
  uint32 action_profile_sa_not_found_index;
  uint32 action_profile_da_not_found_index;
  uint32 enable_unicast_same_interface_filter;
  uint32 learn_enable;
  uint32 port_is_pbp;
  uint32 ieee_1588_profile;
  uint32 enable_pp_inject;
  uint32 key_gen_var;
  uint32 mapped_pp_port;
} ARAD_PP_IHB_PINFO_FLP_TBL_DATA;

typedef struct
{
  uint32 default_action_uc;
  uint32 default_action_mc;
} ARAD_PP_IHB_VRF_CONFIG_TBL_DATA;

typedef struct
{
  uint32 st_vsq_ptr_tc_mode;
  uint32 build_ftmh;
  uint32 build_pph;
  uint32 always_add_pph_learn_ext;
  uint32 never_add_pph_learn_ext;
  uint32 enable_stacking_uc;
  uint32 enable_stacking_mc;
  uint32 user_header_1_size;
  uint32 user_header_2_size;
  uint32 mc_id_in_user_header_2;
  uint32 out_lif_in_user_header_2;
  uint32 ftmh_reserved_value;
} ARAD_PP_IHB_HEADER_PROFILE_TBL_DATA;

typedef struct
{
  uint32 sampling_probability;
} ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_DATA;

typedef struct
{
  uint32 sampling_probability;
} ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA;

typedef struct
{
  uint32 pvlan_port_type;
  uint32 unknown_bc_da_action_filter;
  uint32 unknown_mc_da_action_filter;
  uint32 unknown_uc_da_action_filter;
  uint32 acceptable_frame_type_profile;
  uint32 egress_vsi_filtering_enable;
  uint32 disable_filtering;
  uint32 port_is_pbp;
  uint32 pmf_profile;
  uint32 orientation;
  uint32 mtu;
  uint32 pmf_data;
  uint32 enable_same_interface_filters_lifs;
  uint32 enable_same_interface_filters_ports;
  uint32 enable_unicast_same_interface_filter;
  uint32 counter_profile;
  uint32 counter_compensation;
  uint8  enable_tm_counter_ptr;
} ARAD_PP_EGQ_PP_PPCT_TBL_DATA;

typedef struct
{
  SOC_SAND_PP_IP_TTL ttl_scope; 
} ARAD_PP_EGQ_TTL_SCOPE_REG_TBL_DATA;

typedef struct
{
  uint32 ttl_scope_index; 
} ARAD_PP_EGQ_TTL_SCOPE_TBL_DATA;

typedef struct
{
  uint32 aux_table_data;
} ARAD_PP_EGQ_AUX_TABLE_TBL_DATA;

typedef struct
{
  uint32 out_tm_port_valid;
  uint32 out_tm_port;
  uint32 discard;
  uint32 cud_valid;
  uint32 cud;
  uint32 dp_valid;
  uint32 dp;
  uint32 tc_valid;
  uint32 tc;
  
  uint32 pp_dsp_valid;
  uint32 pp_dsp;
  uint32 cos_profile;
  uint32 cos_profile_valid;
} ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA;

typedef struct
{
  uint32 c_tag_offset;
  uint32 packet_has_pcp_dei;
  uint32 packet_has_up;
  uint32 packet_has_c_tag;
  uint32 incoming_tag_format;
} ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA;

typedef struct
{
  uint32 topology_id;
  uint32 my_mac;
  uint32 my_mac_valid;
} ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA;

typedef struct
{
  uint32 profile;
  uint32 fid_class;
  uint32 da_not_found_destination;
} ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA;

typedef struct
{
  uint32 valid[8];
} ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA;

typedef struct
{
  uint32 index[8];
} ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA;

typedef struct
{
  uint32 da_not_found_destination;
} ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA;

typedef struct
{
  uint32 entry_count;
} ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA;

typedef struct
{
  uint32 profile_pointer[8];
} ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA;

typedef struct
{
  uint32 limit;
  uint32 interrupt_en;
  uint32 message_en;
  uint32 event_forwarding_profile;
  uint32 fid_aging_profile;
} ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA;

typedef struct
{
  uint32 isid;
} ARAD_PP_EPNI_ISID_TABLE_TBL_DATA;

typedef struct
{
  uint32 ttl_exp_label_index;
  uint32 reject_ttl_0;
  uint32 reject_ttl_1;
  uint32 has_cw;
  uint32 labels_to_terminate;
  uint32 skip_ethernet;
#ifdef BCM_88660_A0
  uint32 check_bos;
  uint32 expect_bos; 
#endif 
} ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_DATA;

typedef struct
{
  uint32 cpu_trap_code;
  uint32 cpu_trap_strength;
  uint32 snoop_strength;
} ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA;

typedef struct
{
  uint32 dscp_remark_data;
} ARAD_PP_EPNI_DSCP_REMARK_TBL_DATA;

typedef struct
{
  uint32 exp_remark_data;
} ARAD_PP_EPNI_EXP_REMARK_TBL_DATA;

typedef struct
{
	uint32 dscp_exp_remark_data;
} ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_DATA;

typedef struct
{
	uint32 dscp_exp_remark_data;
} ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_DATA;

typedef struct
{
	uint32 dscp_exp_remark_data;
} ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_DATA;

typedef struct
{
	uint32 dscp_exp_remark_data;
} ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_DATA;


typedef struct
{
  uint32 dscp_exp_remark_data;
} ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_DATA;

typedef struct
{
  uint32 dscp_exp_remark_data;
} ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_DATA;

typedef struct
{
  uint32 fwd_code;
  uint32 prge_tm_profile;
  uint32 prge_pp_profile;
  uint32 system_mc;
  uint32 pph_type;
  uint32 fhei_size;
  uint32 eei_valid;
  uint32 learn_ext_exist;
  uint32 value_1;
  uint32 ace_variable;
  uint32 oam_sub_type;
  uint32 data_entry_lsbs;
  uint32 first_enc;
  uint32 second_enc;
  uint32 link_editor_entry_is_ac;
  uint32 data_entry_cnt;
  
  uint32 routing_over_overlay;
  uint32 first_out_lif_profile;
  uint32 first_out_lif_profile_valid;
  uint32 second_out_lif_profile;
  uint32 second_out_lif_profile_valid;
  uint32 third_out_lif_profile;
  uint32 third_out_lif_profile_valid;
  uint32 forth_out_lif_profile;
  uint32 forth_out_lif_profile_valid;
  
  
  uint32 fifth_out_lif_profile;
  uint32 sixth_out_lif_profile;
  uint32 native_ac_lif_profile;
  uint32 mirror_enable;
  uint32 mirror_command;
  uint32 fwd_enable;
  uint32 data_entry_2_lsbs;
  
  uint32 fwd_code_mask;
  uint32 prge_tm_profile_mask;
  uint32 prge_pp_profile_mask;
  uint32 system_mc_mask;
  uint32 pph_type_mask;
  uint32 fhei_size_mask;
  uint32 eei_valid_mask;
  uint32 learn_ext_exist_mask;
  uint32 value_1_mask;
  uint32 ace_variable_mask;
  uint32 oam_sub_type_mask;
  uint32 data_entry_lsbs_mask;
  uint32 first_enc_mask;
  uint32 second_enc_mask;
  uint32 link_editor_entry_is_ac_mask;
  uint32 data_entry_cnt_mask;
  
  uint32 routing_over_overlay_mask;
  uint32 first_out_lif_profile_mask;
  uint32 first_out_lif_profile_valid_mask;
  uint32 second_out_lif_profile_mask;
  uint32 second_out_lif_profile_valid_mask;
  uint32 third_out_lif_profile_mask;
  uint32 third_out_lif_profile_valid_mask;
  uint32 forth_out_lif_profile_mask;
  uint32 forth_out_lif_profile_valid_mask;
  
  
  uint32 fifth_out_lif_profile_mask;
  uint32 sixth_out_lif_profile_mask;
  uint32 native_ac_lif_profile_mask;
  uint32 mirror_enable_mask;
  uint32 mirror_command_mask;
  uint32 fwd_enable_mask;
  uint32 data_entry_2_lsbs_mask;
  
  uint32 program;
  uint32 valid;
} ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA;

typedef struct
{
  uint32 program_pointer;
  uint32 register_src;
  uint32 bytes_to_remove;
  uint32 remove_network_header;
  uint32 remove_system_header;
  uint64 program_var;
} ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA;

typedef struct
{
  uint32 src_select;
  uint32 size_src;
  uint32 size_base;
  uint32 offset_src;
  uint32 offset_base;
  uint32 fem_select;
  uint32 op_value;
  uint32 op_1_field_select;
  uint32 op_2_field_select;
  uint32 op_3_field_select;
  uint32 alu_action;
  uint32 cmp_action;
  uint32 dst_select;
} ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA;

typedef struct
{
  soc_reg_above_64_val_t prge_data_entry;
} ARAD_PP_EPNI_PRGE_DATA_TBL_DATA;

typedef struct
{
  uint32 out_tm_port;
  uint32 out_pp_port;
  uint32 dst_system_port;
} ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA;





#define ARAD_PMF_LOW_LEVEL_NOF_FEMS_PER_GROUP     (ARAD_PMF_LOW_LEVEL_NOF_FEMS / 2)
#define ARAD_PMF_LOW_LEVEL_NOF_FESS_PER_GROUP     (ARAD_PMF_LOW_LEVEL_NOF_FESS / 2)


#define ARAD_PMF_FEM_SIZE_1ST_SIZE  (4)
#define ARAD_PMF_FEM_SIZE_2ND_SIZE (SOC_IS_JERICHO(unit)? 19: 16)
#define ARAD_PMF_FEM_SIZE_3RD_SIZE (SOC_IS_JERICHO(unit)? 24: 19)







void arad_tbl_default_dynamic_set(void);


int dpp_tbl_is_dynamic(int unit, soc_mem_t mem);

void arad_tbl_mark_cachable(int unit);

void
  arad_tbls_nof_repetitions_clear(void);

void
  arad_iqm_mantissa_exponent_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              reg_val,
    SOC_SAND_IN   uint32              mantissa_size,
    SOC_SAND_OUT  uint32              *mantissa,
    SOC_SAND_OUT  uint32              *exponent
  );

void
  arad_iqm_mantissa_exponent_set(
    SOC_SAND_IN   int             unit,
	SOC_SAND_IN   uint32              mantissa,
	SOC_SAND_IN   uint32              exponent,
    SOC_SAND_IN   uint32              mantissa_size,
	SOC_SAND_OUT  uint32              *reg_val
  );


uint32
  arad_olp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_ire_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_idr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_irr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_ihp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_qdr_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );


uint32
  arad_iqm_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_ips_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

uint32
  arad_ipt_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );

uint32
  arad_dpi_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32  block_id,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_rtp_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_egq_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_cfc_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_sch_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_epni_set_reps_for_tbl_unsafe(
    SOC_SAND_IN   int  unit,
    SOC_SAND_IN   uint32   nof_reps
  );


uint32
  arad_olp_pge_mem_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_OLP_PGE_MEM_TBL_DATA* OLP_pge_mem_tbl_data
  );


uint32
  arad_olp_pge_mem_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_OLP_PGE_MEM_TBL_DATA* OLP_pge_mem_tbl_data
  );


uint32
  arad_ire_nif_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRE_NIF_CTXT_MAP_TBL_DATA* IRE_nif_ctxt_map_tbl_data
  );


uint32
  arad_ire_nif_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRE_NIF_CTXT_MAP_TBL_DATA* IRE_nif_ctxt_map_tbl_data
  );


uint32
  arad_ire_nif_port2ctxt_bit_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA* IRE_nif_port2ctxt_bit_map_tbl_data
  );


uint32
  arad_ire_nif_port2ctxt_bit_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA* IRE_nif_port2ctxt_bit_map_tbl_data
  );


uint32
  arad_ire_rcy_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRE_RCY_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  );


uint32
  arad_ire_rcy_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRE_RCY_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  );


uint32
  arad_ire_cpu_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRE_CPU_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  );


uint32
  arad_ire_cpu_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRE_CPU_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  );


uint32
  arad_idr_complete_pc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IDR_COMPLETE_PC_TBL_DATA* IDR_complete_pc_tbl_data
  );


uint32
  arad_idr_complete_pc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IDR_COMPLETE_PC_TBL_DATA* IDR_complete_pc_tbl_data
  );


uint32
  arad_irr_mirror_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRR_MIRROR_TABLE_TBL_DATA* IRR_mirror_table_tbl_data
  );


uint32
  arad_irr_mirror_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRR_MIRROR_TABLE_TBL_DATA* IRR_mirror_table_tbl_data
  );


uint32
  arad_irr_snoop_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRR_SNOOP_TABLE_TBL_DATA* IRR_snoop_table_tbl_data
  );


uint32
  arad_irr_snoop_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRR_SNOOP_TABLE_TBL_DATA* IRR_snoop_table_tbl_data
  );


uint32
  arad_irr_glag_to_lag_range_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRR_GLAG_TO_LAG_RANGE_TBL_DATA* IRR_glag_to_lag_range_tbl_data
  );


uint32
  arad_irr_glag_to_lag_range_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRR_GLAG_TO_LAG_RANGE_TBL_DATA* IRR_glag_to_lag_range_tbl_data
  );


uint32
  arad_irr_smooth_division_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_indx,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_OUT  ARAD_IRR_SMOOTH_DIVISION_TBL_DATA* IRR_smooth_division_tbl_data
  );


uint32
  arad_irr_smooth_division_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_IN   ARAD_IRR_SMOOTH_DIVISION_TBL_DATA* IRR_smooth_division_tbl_data
  );


uint32
  arad_irr_glag_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_OUT  ARAD_IRR_GLAG_MAPPING_TBL_DATA* IRR_glag_mapping_tbl_data
  );


uint32
  arad_irr_glag_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_IN   ARAD_IRR_GLAG_MAPPING_TBL_DATA* IRR_glag_mapping_tbl_data
  );


uint32
  arad_irr_destination_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRR_DESTINATION_TABLE_TBL_DATA* IRR_destination_table_tbl_data
  );


uint32
  arad_irr_destination_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRR_DESTINATION_TABLE_TBL_DATA* IRR_destination_table_tbl_data
  );


uint32
  arad_irr_glag_next_member_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRR_GLAG_NEXT_MEMBER_TBL_DATA* IRR_glag_next_member_tbl_data
  );


uint32
  arad_irr_glag_next_member_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRR_GLAG_NEXT_MEMBER_TBL_DATA* IRR_glag_next_member_tbl_data
  );


uint32
  arad_irr_rlag_next_member_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRR_RLAG_NEXT_MEMBER_TBL_DATA* IRR_rlag_next_member_tbl_data
  );


uint32
  arad_irr_rlag_next_member_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRR_RLAG_NEXT_MEMBER_TBL_DATA* IRR_rlag_next_member_tbl_data
  );


uint32
  arad_ihp_port_info_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_PORT_INFO_TBL_DATA* IHP_port_info_tbl_data
  );


uint32
  arad_ihp_port_info_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_PORT_INFO_TBL_DATA* IHP_port_info_tbl_data
  );


uint32
  arad_ihp_port_to_system_port_id_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA* IHP_port_to_system_port_id_tbl_data
  );


uint32
  arad_ihp_port_to_system_port_id_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA* IHP_port_to_system_port_id_tbl_data
  );


uint32
  arad_ihp_static_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_STATIC_HEADER_TBL_DATA* IHP_static_header_tbl_data
  );


uint32
  arad_ihp_static_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_STATIC_HEADER_TBL_DATA* IHP_static_header_tbl_data
  );


uint32
  arad_ihp_system_port_my_port_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA* IHP_system_port_my_port_table_tbl_data
  );


uint32
  arad_ihp_system_port_my_port_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA* IHP_system_port_my_port_table_tbl_data
  );


uint32
  arad_ihp_ptc_commands1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands1_tbl_data
  );


uint32
  arad_ihp_ptc_commands1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands1_tbl_data
  );


uint32
  arad_ihp_ptc_commands2_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands2_tbl_data
  );


uint32
  arad_ihp_ptc_commands2_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_PTC_COMMANDS_TBL_DATA* IHP_ptc_commands2_tbl_data
  );


uint32
  arad_ihp_ptc_key_program_lut_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA* IHP_ptc_key_program_lut_tbl_data
  );


uint32
  arad_ihp_ptc_key_program_lut_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA* IHP_ptc_key_program_lut_tbl_data
  );



uint32
  arad_ihp_key_program_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             program_ndx,
    SOC_SAND_IN   uint32             instr_ndx,
    SOC_SAND_OUT   ARAD_IHP_KEY_PROGRAM_TBL_DATA* IHP_key_program_tbl_data
  );



uint32
  arad_ihp_key_program_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             program_ndx,
    SOC_SAND_IN   uint32             instr_ndx,
    SOC_SAND_IN   ARAD_IHP_KEY_PROGRAM_TBL_DATA* IHP_key_program_tbl_data
  );


uint32
  arad_ihp_key_program0_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_KEY_PROGRAM0_TBL_DATA* IHP_key_program0_tbl_data
  );


uint32
  arad_ihp_key_program0_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_KEY_PROGRAM0_TBL_DATA* IHP_key_program0_tbl_data
  );


uint32
  arad_ihp_key_program1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_KEY_PROGRAM1_TBL_DATA* IHP_key_program1_tbl_data
  );


uint32
  arad_ihp_key_program1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_KEY_PROGRAM1_TBL_DATA* IHP_key_program1_tbl_data
  );


uint32
  arad_ihp_key_program2_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_KEY_PROGRAM2_TBL_DATA* IHP_key_program2_tbl_data
  );


uint32
  arad_ihp_key_program2_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_KEY_PROGRAM2_TBL_DATA* IHP_key_program2_tbl_data
  );


uint32
  arad_ihp_key_program3_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_KEY_PROGRAM3_TBL_DATA* IHP_key_program3_tbl_data
  );


uint32
  arad_ihp_key_program3_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_KEY_PROGRAM3_TBL_DATA* IHP_key_program3_tbl_data
  );


uint32
  arad_ihp_key_program4_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_KEY_PROGRAM4_TBL_DATA* IHP_key_program4_tbl_data
  );


uint32
  arad_ihp_key_program4_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_KEY_PROGRAM4_TBL_DATA* IHP_key_program4_tbl_data
  );


uint32
  arad_ihp_programmable_cos_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             set_ndx,
    SOC_SAND_OUT  ARAD_IHP_PROGRAMMABLE_COS_TBL_DATA* IHP_programmable_cos_tbl_data
  );



uint32
  arad_ihp_programmable_cos_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             set_ndx,
    SOC_SAND_IN   ARAD_IHP_PROGRAMMABLE_COS_TBL_DATA* IHP_programmable_cos_tbl_data
  );


uint32
  arad_ihp_programmable_cos1_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IHP_PROGRAMMABLE_COS1_TBL_DATA* IHP_programmable_cos1_tbl_data
  );


uint32
  arad_ihp_programmable_cos1_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IHP_PROGRAMMABLE_COS1_TBL_DATA* IHP_programmable_cos1_tbl_data
  );


uint32
  arad_iqm_bdb_link_list_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_BDB_LINK_LIST_TBL_DATA* IQM_bdb_link_list_tbl_data
  );


uint32
  arad_iqm_bdb_link_list_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_BDB_LINK_LIST_TBL_DATA* IQM_bdb_link_list_tbl_data
  );


int
  arad_iqm_dynamic_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  );


uint32
  arad_iqm_dynamic_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  );


int
  arad_iqm_static_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  );


int
  arad_iqm_static_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  );


uint32
  arad_iqm_packet_queue_tail_pointer_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA* IQM_packet_queue_tail_pointer_tbl_data
  );


uint32
  arad_iqm_packet_queue_tail_pointer_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_PACKET_QUEUE_TAIL_POINTER_TBL_DATA* IQM_packet_queue_tail_pointer_tbl_data
  );


uint32
  arad_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA* IQM_packet_queue_red_weight_table_tbl_data
  );


uint32
  arad_iqm_packet_queue_red_weight_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA* IQM_packet_queue_red_weight_table_tbl_data
  );


uint32
  arad_iqm_credit_discount_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA* IQM_credit_discount_table_tbl_data
  );


uint32
  arad_iqm_credit_discount_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA* IQM_credit_discount_table_tbl_data
  );


uint32
  arad_iqm_full_user_count_memory_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA* IQM_full_user_count_memory_tbl_data
  );


uint32
  arad_iqm_full_user_count_memory_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_FULL_USER_COUNT_MEMORY_TBL_DATA* IQM_full_user_count_memory_tbl_data
  );


uint32
  arad_iqm_mini_multicast_user_count_memory_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA* IQM_mini_multicast_user_count_memory_tbl_data
  );


uint32
  arad_iqm_mini_multicast_user_count_memory_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL_DATA* IQM_mini_multicast_user_count_memory_tbl_data
  );

uint32
    arad_iqm_packet_queue_red_parameters_tail_drop_mantissa_nof_bits (
       SOC_SAND_IN   int             unit,
       SOC_SAND_IN   uint32             drop_precedence_ndx,
       SOC_SAND_OUT  uint32*            max_mantissa_size,
       SOC_SAND_OUT  uint32*            max_bds_mantissa_size
    );

uint32
  arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   uint32                                              rt_cls_ndx,
    SOC_SAND_IN   uint32                                             drop_precedence_ndx,
    SOC_SAND_OUT  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  );


uint32
  arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   uint32                                              rt_cls_ndx,
    SOC_SAND_IN   uint32                                             drop_precedence_ndx,
    SOC_SAND_IN   ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  );


uint32
  arad_iqm_packet_queue_red_parameters_table_tbl_fill_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  );

uint32
  arad_ihb_fem_map_index_table_table_tbl_fill_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA* ihb_fem0_4b_1st_pass_map_index_table_tbl_data
  );

uint32
  arad_ihb_fem_map_table_table_tbl_fill_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   ARAD_IHB_FEM_MAP_TABLE_TBL_DATA* arad_ihb_fem_map_table_table_tbl_fill_unsafe
  );

uint32
  arad_IQM_packet_descriptor_fifos_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_IQM_packet_descriptor_fifos_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_IQM_tx_descriptor_fifos_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_IQM_tx_descriptor_fifos_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL_DATA  *tbl_data
  );


uint32
  arad_iqm_vsq_descriptor_rate_class_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA* IQM_vsq_descriptor_rate_class_group_a_tbl_data
  );


uint32
  arad_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA* IQM_vsq_descriptor_rate_class_group_a_tbl_data
  );


uint32
  arad_iqm_vsq_descriptor_rate_class_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA* IQM_vsq_descriptor_rate_class_group_b_tbl_data
  );


uint32
  arad_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA* IQM_vsq_descriptor_rate_class_group_b_tbl_data
  );


uint32
  arad_iqm_vsq_descriptor_rate_class_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA* IQM_vsq_descriptor_rate_class_group_c_tbl_data
  );


uint32
  arad_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA* IQM_vsq_descriptor_rate_class_group_c_tbl_data
  );


uint32
  arad_iqm_vsq_descriptor_rate_class_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA* IQM_vsq_descriptor_rate_class_group_d_tbl_data
  );


uint32
  arad_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA* IQM_vsq_descriptor_rate_class_group_d_tbl_data
  );


uint32
  arad_iqm_vsq_qsize_memory_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_qsize_memory_group_a_tbl_data
  );


uint32
  arad_iqm_vsq_qsize_memory_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_qsize_memory_group_a_tbl_data
  );


uint32
  arad_iqm_vsq_qsize_memory_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_qsize_memory_group_b_tbl_data
  );


uint32
  arad_iqm_vsq_qsize_memory_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_qsize_memory_group_b_tbl_data
  );


uint32
  arad_iqm_vsq_qsize_memory_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_qsize_memory_group_c_tbl_data
  );


uint32
  arad_iqm_vsq_qsize_memory_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_qsize_memory_group_c_tbl_data
  );


uint32
  arad_iqm_vsq_qsize_memory_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_qsize_memory_group_d_tbl_data
  );


uint32
  arad_iqm_vsq_qsize_memory_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_qsize_memory_group_d_tbl_data
  );


uint32
  arad_iqm_vsq_average_qsize_memory_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_average_qsize_memory_group_a_tbl_data
  );


uint32
  arad_iqm_vsq_average_qsize_memory_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL_DATA* IQM_vsq_average_qsize_memory_group_a_tbl_data
  );


uint32
  arad_iqm_vsq_average_qsize_memory_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_average_qsize_memory_group_b_tbl_data
  );


uint32
  arad_iqm_vsq_average_qsize_memory_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL_DATA* IQM_vsq_average_qsize_memory_group_b_tbl_data
  );


uint32
  arad_iqm_vsq_average_qsize_memory_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_average_qsize_memory_group_c_tbl_data
  );


uint32
  arad_iqm_vsq_average_qsize_memory_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL_DATA* IQM_vsq_average_qsize_memory_group_c_tbl_data
  );


uint32
  arad_iqm_vsq_average_qsize_memory_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_average_qsize_memory_group_d_tbl_data
  );


uint32
  arad_iqm_vsq_average_qsize_memory_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL_DATA* IQM_vsq_average_qsize_memory_group_d_tbl_data
  );


uint32
  arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_flow_control_parameters_table_group_tbl_data
  );


uint32
  arad_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_flow_control_parameters_table_group_tbl_data
  );


uint32
  arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_OUT  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  );


uint32
  arad_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_IN   ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  );


uint32
  arad_iqm_vsq_queue_parameters_table_group_tbl_fill_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  );

uint32
  arad_iqm_vsq_flow_control_parameters_table_group_tbl_fill_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* iqm_vsq_flow_control_parameters_table_group_tbl_data
  );


uint32
  arad_iqm_system_red_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN   int                                       unit,
    SOC_SAND_IN   uint32                                        rt_cls_ndx,
    SOC_SAND_IN   uint32                                       drop_precedence_ndx,
    SOC_SAND_OUT  ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA* IQM_system_red_parameters_table_tbl_data
  );


uint32
  arad_iqm_system_red_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN   int                                       unit,
    SOC_SAND_IN   uint32                                        rt_cls_ndx,
    SOC_SAND_IN   uint32                                       drop_precedence_ndx,
    SOC_SAND_IN   ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA* IQM_system_red_parameters_table_tbl_data
  );


uint32
  arad_iqm_system_red_drop_probability_values_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA* IQM_system_red_drop_probability_values_tbl_data
  );


uint32
  arad_iqm_system_red_drop_probability_values_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA* IQM_system_red_drop_probability_values_tbl_data
  );


uint32
  arad_iqm_system_red_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_SYSTEM_RED_TBL_DATA* IQM_system_red_tbl_data
  );


uint32
  arad_iqm_system_red_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_SYSTEM_RED_TBL_DATA* IQM_system_red_tbl_data
  );




uint32
  arad_qdr_memory_tbl_get_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   uint32                  entry_offset,
    SOC_SAND_OUT  ARAD_QDR_MEM_TBL_DATA*   QDR_memory_tbl_data
  );


uint32
  arad_qdr_memory_tbl_set_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   uint32                  entry_offset,
    SOC_SAND_IN  ARAD_QDR_MEM_TBL_DATA*   QDR_memory_tbl_data
  );


uint32
  arad_qdr_qdr_dll_mem_tbl_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       entry_offset,
    SOC_SAND_OUT ARAD_QDR_QDR_DLL_MEM_TBL_DATA *tbl_data
  );


uint32
  arad_qdr_qdr_dll_mem_tbl_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       entry_offset,
    SOC_SAND_IN  ARAD_QDR_QDR_DLL_MEM_TBL_DATA *tbl_data
  );


uint32
  arad_indirect_base_queue_to_system_physical_port_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_BASE_Q_TO_SYS_PORT_TABLE_TBL_DATA* system_physical_port_tbl_data
  );


uint32
  arad_indirect_base_queue_to_system_physical_port_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_BASE_Q_TO_SYS_PORT_TABLE_TBL_DATA* system_physical_port_tbl_data
  );

uint32
  arad_indirect_base_queue_to_system_physical_port_tbl_region_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32              region_size,
    SOC_SAND_IN   ARAD_BASE_Q_TO_SYS_PORT_TABLE_TBL_DATA* system_physical_port_tbl_data
  );



uint32
  arad_direct_base_queue_to_system_physical_port_tbl_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core,
    SOC_SAND_IN  uint32 base_queue,
    SOC_SAND_OUT ARAD_BASE_Q_TO_MOD_PORT_TABLE_TBL_DATA* mod_port_tbl_data
  );



uint32
  arad_direct_base_queue_to_system_physical_port_tbl_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 entry_offset,
    SOC_SAND_IN ARAD_BASE_Q_TO_MOD_PORT_TABLE_TBL_DATA* mod_port_tbl_data
  );


uint32
  arad_direct_base_queue_to_system_physical_port_tbl_region_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 entry_offset,
    SOC_SAND_IN uint32 region_size,
    SOC_SAND_IN ARAD_BASE_Q_TO_MOD_PORT_TABLE_TBL_DATA* mod_port_tbl_data
  );

uint32
  arad_indirect_sysport_to_modport_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MOD_PORT_TBL_DATA* IPS_destination_device_and_port_lookup_table_tbl_data
  );


uint32
  arad_indirect_sysport_to_modport_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MOD_PORT_TBL_DATA* IPS_destination_device_and_port_lookup_table_tbl_data
  );


uint32
  arad_ips_flow_id_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  );


uint32
  arad_ips_flow_id_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  );


uint32
  arad_ips_flow_id_lookup_table_tbl_region_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32              region_size,
    SOC_SAND_IN   ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  );


uint32
  arad_ips_queue_type_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA* IPS_queue_type_lookup_table_tbl_data
  );


uint32
  arad_ips_queue_type_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA* IPS_queue_type_lookup_table_tbl_data
  );


uint32
  arad_ips_queue_priority_map_select_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA* IPS_queue_priority_map_select_tbl_data
  );


uint32
  arad_ips_queue_priority_map_select_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA* IPS_queue_priority_map_select_tbl_data
  );


uint32
  arad_ips_queue_priority_maps_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA* IPS_queue_priority_maps_table_tbl_data
  );


uint32
  arad_ips_queue_priority_maps_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA* IPS_queue_priority_maps_table_tbl_data
  );


uint32
  arad_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_queue_size_based_thresholds_table_tbl_data
  );


uint32
  arad_ips_queue_size_based_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_queue_size_based_thresholds_table_tbl_data
  );


uint32
  arad_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_balance_based_thresholds_table_tbl_data
  );


uint32
  arad_ips_credit_balance_based_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_balance_based_thresholds_table_tbl_data
  );


uint32
  arad_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA* IPS_empty_queue_credit_balance_table_tbl_data
  );


uint32
  arad_ips_empty_queue_credit_balance_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA* IPS_empty_queue_credit_balance_table_tbl_data
  );


uint32
  arad_ips_credit_watchdog_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_watchdog_thresholds_table_tbl_data
  );


uint32
  arad_ips_credit_watchdog_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_watchdog_thresholds_table_tbl_data
  );


uint32
  arad_ips_queue_descriptor_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA* IPS_queue_descriptor_table_tbl_data
  );


uint32
  arad_ips_queue_descriptor_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA* IPS_queue_descriptor_table_tbl_data
  );


uint32
  arad_ips_queue_size_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_SIZE_TABLE_TBL_DATA* IPS_queue_size_table_tbl_data
  );


uint32
  arad_ips_queue_size_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_SIZE_TABLE_TBL_DATA* IPS_queue_size_table_tbl_data
  );


uint32
  arad_ips_system_red_max_queue_size_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA* IPS_system_red_max_queue_size_table_tbl_data
  );


uint32
  arad_ips_system_red_max_queue_size_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA* IPS_system_red_max_queue_size_table_tbl_data
  );


uint32
  arad_ipt_bdq_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPT_BDQ_TBL_DATA* IPT_bdq_tbl_data
  );


uint32
  arad_ipt_bdq_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPT_BDQ_TBL_DATA* IPT_bdq_tbl_data
  );


uint32
  arad_ipt_pcq_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPT_PCQ_TBL_DATA* IPT_pcq_tbl_data
  );


uint32
  arad_ipt_pcq_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPT_PCQ_TBL_DATA* IPT_pcq_tbl_data
  );


uint32
  arad_ipt_sop_mmu_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPT_SOP_MMU_TBL_DATA* IPT_sop_mmu_tbl_data
  );


uint32
  arad_ipt_sop_mmu_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPT_SOP_MMU_TBL_DATA* IPT_sop_mmu_tbl_data
  );


uint32
  arad_ipt_mop_mmu_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPT_MOP_MMU_TBL_DATA* IPT_mop_mmu_tbl_data
  );


uint32
  arad_ipt_mop_mmu_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPT_MOP_MMU_TBL_DATA* IPT_mop_mmu_tbl_data
  );


uint32
  arad_ipt_fdtctl_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPT_FDTCTL_TBL_DATA* IPT_fdtctl_tbl_data
  );


uint32
  arad_ipt_fdtctl_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPT_FDTCTL_TBL_DATA* IPT_fdtctl_tbl_data
  );


uint32
  arad_ipt_fdtdata_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPT_FDTDATA_TBL_DATA* IPT_fdtdata_tbl_data
  );


uint32
  arad_ipt_fdtdata_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPT_FDTDATA_TBL_DATA* IPT_fdtdata_tbl_data
  );


uint32
  arad_ipt_egqctl_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPT_EGQCTL_TBL_DATA* IPT_egqctl_tbl_data
  );


uint32
  arad_ipt_egqctl_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPT_EGQCTL_TBL_DATA* IPT_egqctl_tbl_data
  );


uint32
  arad_ipt_egqdata_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPT_EGQDATA_TBL_DATA* IPT_egqdata_tbl_data
  );


uint32
  arad_ipt_egqdata_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPT_EGQDATA_TBL_DATA* IPT_egqdata_tbl_data
  );


uint32
  arad_dpi_dll_ram_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             dram_ndx,
    SOC_SAND_OUT  ARAD_DPI_DLL_RAM_TBL_DATA* DPI_dll_ram_tbl_data
  );


uint32
  arad_dpi_dll_ram_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32             dram_ndx,
    SOC_SAND_IN  ARAD_DPI_DLL_RAM_TBL_DATA* DPI_dll_ram_tbl_data
  );


uint32
  arad_rtp_unicast_distribution_memory_for_data_cells_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_data_cells_tbl_data
  );


uint32
  arad_rtp_unicast_distribution_memory_for_data_cells_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_data_cells_tbl_data
  );


uint32
  arad_rtp_unicast_distribution_memory_for_control_cells_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_control_cells_tbl_data
  );


uint32
  arad_rtp_unicast_distribution_memory_for_control_cells_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL_DATA* RTP_unicast_distribution_memory_for_control_cells_tbl_data
  );


uint32
  arad_egq_scm_tbl_get_unsafe(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   int                     core,
    SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_IN   uint32                  entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_SCM_TBL_DATA*  EGQ_nif_scm_tbl_data
  );


uint32
  arad_egq_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_SCM_TBL_DATA* EGQ_nif_scm_tbl_data
  );


uint32
  arad_egq_nifab_nch_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_NIFAB_NCH_SCM_TBL_DATA* EGQ_nifab_nch_scm_tbl_data
  );


uint32
  arad_egq_nifab_nch_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_NIFAB_NCH_SCM_TBL_DATA* EGQ_nifab_nch_scm_tbl_data
  );


uint32
  arad_egq_rcy_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_SCM_TBL_DATA* EGQ_rcy_scm_tbl_data
  );


uint32
  arad_egq_rcy_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_SCM_TBL_DATA* EGQ_rcy_scm_tbl_data
  );


uint32
  arad_egq_cpu_scm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_SCM_TBL_DATA* EGQ_cpu_scm_tbl_data
  );


uint32
  arad_egq_cpu_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_SCM_TBL_DATA* EGQ_cpu_scm_tbl_data
  );


uint32
  arad_egq_ccm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_CCM_TBL_DATA* EGQ_ccm_tbl_data
  );


uint32
  arad_egq_ccm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          entry_offset,
    SOC_SAND_IN   ARAD_EGQ_CCM_TBL_DATA* EGQ_ccm_tbl_data
  );


uint32
  arad_egq_pmc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  );


uint32
  arad_egq_pmc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  );


uint32
  arad_egq_cbm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_CBM_TBL_DATA* EGQ_cbm_tbl_data
  );


uint32
  arad_egq_cbm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_CBM_TBL_DATA* EGQ_cbm_tbl_data
  );


uint32
  arad_egq_fbm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_FBM_TBL_DATA* EGQ_fbm_tbl_data
  );


uint32
  arad_egq_fbm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_FBM_TBL_DATA* EGQ_fbm_tbl_data
  );


uint32
  arad_egq_fdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_FDM_TBL_DATA* EGQ_fdm_tbl_data
  );


uint32
  arad_egq_fdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_FDM_TBL_DATA* EGQ_fdm_tbl_data
  );


uint32
  arad_egq_dwm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_DWM_TBL_DATA* EGQ_dwm_tbl_data
  );


uint32
  arad_egq_dwm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          entry_offset,
    SOC_SAND_IN   ARAD_EGQ_DWM_TBL_DATA* EGQ_dwm_tbl_data
  );


uint32
  arad_egq_rrdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_RRDM_TBL_DATA* EGQ_rrdm_tbl_data
  );


uint32
  arad_egq_rrdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_RRDM_TBL_DATA* EGQ_rrdm_tbl_data
  );


uint32
  arad_egq_rpdm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_RPDM_TBL_DATA* EGQ_rpdm_tbl_data
  );


uint32
  arad_egq_rpdm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_RPDM_TBL_DATA* EGQ_rpdm_tbl_data
  );


int
  arad_egq_pct_tbl_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 q_pair,
    SOC_SAND_OUT ARAD_EGQ_PCT_TBL_DATA  *tbl_data
    );


int
  arad_egq_pct_tbl_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 q_pair,
    SOC_SAND_IN  ARAD_EGQ_PCT_TBL_DATA  *tbl_data
  );



int
  arad_egq_ppct_tbl_get_unsafe(
    SOC_SAND_IN   int                       unit,
    SOC_SAND_IN   int                       core_id,
    SOC_SAND_IN   uint32                    base_q_pair,
    SOC_SAND_OUT  ARAD_EGQ_PPCT_TBL_DATA*   EGQ_ppct_tbl_data
  );



int
  arad_egq_ppct_tbl_set_unsafe(
    SOC_SAND_IN   int                       unit,
    SOC_SAND_IN   int                       core_id,
    SOC_SAND_IN   uint32                    base_q_pair,
    SOC_SAND_IN   ARAD_EGQ_PPCT_TBL_DATA*   EGQ_ppct_tbl_data
  );

#ifdef BCM_88660_A0



uint32
  arad_egq_per_port_lb_range_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32             entry_offset,
    SOC_SAND_OUT  ARAD_PER_PORT_LB_RANGE_TBL_DATA* EGQ_per_port_lb_range_tbl_data
  );



uint32
  arad_egq_per_port_lb_range_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_PER_PORT_LB_RANGE_TBL_DATA* EGQ_per_port_lb_range_tbl_data
  );
  
#endif 


uint32
  arad_egq_vlan_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_VLAN_TABLE_TBL_DATA* EGQ_vlan_table_tbl_data
  );


uint32
  arad_egq_vlan_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_VLAN_TABLE_TBL_DATA* EGQ_vlan_table_tbl_data
  );



uint32
  arad_egq_cfc_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_CFC_FLOW_CONTROL_TBL_DATA* EGQ_cfc_flow_control_tbl_data
  );


uint32
  arad_egq_cfc_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_CFC_FLOW_CONTROL_TBL_DATA* EGQ_cfc_flow_control_tbl_data
  );


uint32
  arad_egq_nifa_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_NIFA_FLOW_CONTROL_TBL_DATA* EGQ_nifa_flow_control_tbl_data
  );


uint32
  arad_egq_nifa_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_NIFA_FLOW_CONTROL_TBL_DATA* EGQ_nifa_flow_control_tbl_data
  );


uint32
  arad_egq_nifb_flow_control_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_NIFB_FLOW_CONTROL_TBL_DATA* EGQ_nifb_flow_control_tbl_data
  );


uint32
  arad_egq_nifb_flow_control_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_NIFB_FLOW_CONTROL_TBL_DATA* EGQ_nifb_flow_control_tbl_data
  );


uint32
  arad_egq_cpu_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_CPU_LAST_HEADER_TBL_DATA* EGQ_cpu_last_header_tbl_data
  );


uint32
  arad_egq_cpu_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_CPU_LAST_HEADER_TBL_DATA* EGQ_cpu_last_header_tbl_data
  );


uint32
  arad_egq_ipt_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_IPT_LAST_HEADER_TBL_DATA* EGQ_ipt_last_header_tbl_data
  );


uint32
  arad_egq_ipt_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_IPT_LAST_HEADER_TBL_DATA* EGQ_ipt_last_header_tbl_data
  );


uint32
  arad_egq_fdr_last_header_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_FDR_LAST_HEADER_TBL_DATA* EGQ_fdr_last_header_tbl_data
  );


uint32
  arad_egq_fdr_last_header_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_FDR_LAST_HEADER_TBL_DATA* EGQ_fdr_last_header_tbl_data
  );


uint32
  arad_egq_cpu_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_CPU_PACKET_COUNTER_TBL_DATA* EGQ_cpu_packet_counter_tbl_data
  );


uint32
  arad_egq_cpu_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_CPU_PACKET_COUNTER_TBL_DATA* EGQ_cpu_packet_counter_tbl_data
  );


uint32
  arad_egq_ipt_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_IPT_PACKET_COUNTER_TBL_DATA* EGQ_ipt_packet_counter_tbl_data
  );


uint32
  arad_egq_ipt_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_IPT_PACKET_COUNTER_TBL_DATA* EGQ_ipt_packet_counter_tbl_data
  );


uint32
  arad_egq_fdr_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_FDR_PACKET_COUNTER_TBL_DATA* EGQ_fdr_packet_counter_tbl_data
  );


uint32
  arad_egq_fdr_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_FDR_PACKET_COUNTER_TBL_DATA* EGQ_fdr_packet_counter_tbl_data
  );


uint32
  arad_egq_rqp_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_RQP_PACKET_COUNTER_TBL_DATA* EGQ_rqp_packet_counter_tbl_data
  );


uint32
  arad_egq_rqp_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_RQP_PACKET_COUNTER_TBL_DATA* EGQ_rqp_packet_counter_tbl_data
  );


uint32
  arad_egq_rqp_discard_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_rqp_discard_packet_counter_tbl_data
  );


uint32
  arad_egq_rqp_discard_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_rqp_discard_packet_counter_tbl_data
  );


uint32
  arad_egq_ehp_unicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_ehp_unicast_packet_counter_tbl_data
  );


uint32
  arad_egq_ehp_unicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_ehp_unicast_packet_counter_tbl_data
  );


uint32
  arad_egq_ehp_multicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_high_packet_counter_tbl_data
  );


uint32
  arad_egq_ehp_multicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_high_packet_counter_tbl_data
  );


uint32
  arad_egq_ehp_multicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_low_packet_counter_tbl_data
  );


uint32
  arad_egq_ehp_multicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_ehp_multicast_low_packet_counter_tbl_data
  );


uint32
  arad_egq_ehp_discard_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_ehp_discard_packet_counter_tbl_data
  );


uint32
  arad_egq_ehp_discard_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL_DATA* EGQ_ehp_discard_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_unicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_unicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_unicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_unicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_multicast_high_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_multicast_high_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_multicast_low_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_multicast_low_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_unicast_high_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_bytes_counter_tbl_data
  );


uint32
  arad_egq_pqp_unicast_high_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_high_bytes_counter_tbl_data
  );


uint32
  arad_egq_pqp_unicast_low_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_bytes_counter_tbl_data
  );


uint32
  arad_egq_pqp_unicast_low_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_unicast_low_bytes_counter_tbl_data
  );


uint32
  arad_egq_pqp_multicast_high_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_bytes_counter_tbl_data
  );


uint32
  arad_egq_pqp_multicast_high_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_high_bytes_counter_tbl_data
  );


uint32
  arad_egq_pqp_multicast_low_bytes_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_bytes_counter_tbl_data
  );


uint32
  arad_egq_pqp_multicast_low_bytes_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL_DATA* EGQ_pqp_multicast_low_bytes_counter_tbl_data
  );


uint32
  arad_egq_pqp_discard_unicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_unicast_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_discard_unicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_unicast_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_discard_multicast_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_multicast_packet_counter_tbl_data
  );


uint32
  arad_egq_pqp_discard_multicast_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL_DATA* EGQ_pqp_discard_multicast_packet_counter_tbl_data
  );


uint32
  arad_egq_fqp_packet_counter_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_FQP_PACKET_COUNTER_TBL_DATA* EGQ_fqp_packet_counter_tbl_data
  );


uint32
  arad_egq_fqp_packet_counter_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_FQP_PACKET_COUNTER_TBL_DATA* EGQ_fqp_packet_counter_tbl_data
  );



uint32
  arad_cfc_recycle_to_out_going_fap_port_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA* CFC_recycle_to_out_going_fap_port_mapping_tbl_data
  );


uint32
  arad_cfc_recycle_to_out_going_fap_port_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA* CFC_recycle_to_out_going_fap_port_mapping_tbl_data
  );


uint32
  arad_cfc_nif_a_class_based_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_a_class_based_to_ofp_mapping_tbl_data
  );


uint32
  arad_cfc_nif_a_class_based_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_a_class_based_to_ofp_mapping_tbl_data
  );


uint32
  arad_cfc_nif_b_class_based_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_b_class_based_to_ofp_mapping_tbl_data
  );


uint32
  arad_cfc_nif_b_class_based_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA* CFC_nif_b_class_based_to_ofp_mapping_tbl_data
  );


uint32
  arad_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  );


uint32
  arad_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_a_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  );


uint32
  arad_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  );


uint32
  arad_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA* CFC_b_schedulers_based_flow_control_to_ofp_mapping_tbl_data
  );


uint32
  arad_cfc_out_of_band_rx_a_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_a_calendar_mapping_tbl_data
  );


uint32
  arad_cfc_out_of_band_rx_a_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_a_calendar_mapping_tbl_data
  );


uint32
  arad_cfc_out_of_band_rx_b_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_b_calendar_mapping_tbl_data
  );


uint32
  arad_cfc_out_of_band_rx_b_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_rx_b_calendar_mapping_tbl_data
  );


uint32
  arad_cfc_out_of_band_tx_calendar_mapping_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_tx_calendar_mapping_tbl_data
  );


uint32
  arad_cfc_out_of_band_tx_calendar_mapping_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA* CFC_out_of_band_tx_calendar_mapping_tbl_data
  );


int
  arad_sch_cal_tbl_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_get ,
    SOC_SAND_IN   uint32                 slots_count,
    SOC_SAND_OUT  uint32*                slots
  );

int
  arad_sch_cal_max_size_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_OUT  uint32*                max_cal_size
   );


int
  arad_sch_cal_tbl_set(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 chan_arb_id,
    SOC_SAND_IN   uint32                 sch_to_set ,
    SOC_SAND_IN   uint32                 slots_count,
                  uint32*                slots
  );


uint32
  arad_sch_drm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  );


uint32
  arad_sch_drm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  );


uint32
  arad_sch_dsm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_DSM_TBL_DATA* SCH_dsm_tbl_data
  );


uint32
  arad_sch_dsm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_DSM_TBL_DATA* SCH_dsm_tbl_data
  );


uint32
  arad_sch_fdms_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FDMS_TBL_DATA* SCH_fdms_tbl_data
  );


uint32
  arad_sch_fdms_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FDMS_TBL_DATA* SCH_fdms_tbl_data
  );


uint32
  arad_sch_shds_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  );


uint32
  arad_sch_shds_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  );


uint32
  arad_sch_sem_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SEM_TBL_DATA* SCH_sem_tbl_data
  );


uint32
  arad_sch_sem_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SEM_TBL_DATA* SCH_sem_tbl_data
  );


uint32
  arad_sch_fsf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FSF_TBL_DATA* SCH_fsf_tbl_data
  );


uint32
  arad_sch_fsf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FSF_TBL_DATA* SCH_fsf_tbl_data
  );


uint32
  arad_sch_fgm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FGM_TBL_DATA* SCH_fgm_tbl_data
  );


uint32
  arad_sch_fgm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FGM_TBL_DATA* SCH_fgm_tbl_data
  );


uint32
  arad_sch_shc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SHC_TBL_DATA* SCH_shc_tbl_data
  );


uint32
  arad_sch_shc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SHC_TBL_DATA* SCH_shc_tbl_data
  );


uint32
  arad_sch_scc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SCC_TBL_DATA* SCH_scc_tbl_data
  );


uint32
  arad_sch_scc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SCC_TBL_DATA* SCH_scc_tbl_data
  );


uint32
  arad_sch_sct_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SCT_TBL_DATA* SCH_sct_tbl_data
  );


uint32
  arad_sch_sct_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SCT_TBL_DATA* SCH_sct_tbl_data
  );


uint32
  arad_sch_fqm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FQM_TBL_DATA* SCH_fqm_tbl_data
  );


uint32
  arad_sch_fqm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FQM_TBL_DATA* SCH_fqm_tbl_data
  );


uint32
  arad_sch_ffm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FFM_TBL_DATA* SCH_ffm_tbl_data
  );


uint32
  arad_sch_ffm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FFM_TBL_DATA* SCH_ffm_tbl_data
  );


uint32
  arad_sch_tmc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_TMC_TBL_DATA* SCH_tmc_tbl_data
  );


uint32
  arad_sch_tmc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_TMC_TBL_DATA* SCH_tmc_tbl_data
  );


uint32
  arad_sch_pqs_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_PQS_TBL_DATA* SCH_pqs_tbl_data
  );

uint32
  arad_sch_pqs_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_PQS_TBL_DATA* SCH_pqs_tbl_data
  );


uint32
  arad_sch_scheduler_init_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SCHEDULER_INIT_TBL_DATA* SCH_scheduler_init_tbl_data
  );


uint32
  arad_sch_force_status_message_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_DATA* SCH_force_status_message_tbl_data
  );



uint32
  arad_mmu_dram_address_space_info_get_unsafe(
    SOC_SAND_IN    int                               unit,
    SOC_SAND_OUT   uint32                                *nof_drams,
    SOC_SAND_OUT   uint32                                *nof_banks,
    SOC_SAND_OUT   uint32                                *nof_cols
  );

uint32
  arad_mmu_dram_address_space_tbl_verify(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx
  );


uint32
  arad_mmu_dram_address_space_tbl_get_unsafe(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx,
    SOC_SAND_OUT  ARAD_MMU_DRAM_ADDRESS_SPACE_TBL_DATA*  MMU_dram_address_space_tbl_data
  );


uint32
  arad_mmu_dram_address_space_tbl_set_unsafe(
    SOC_SAND_IN   int                               unit,
    SOC_SAND_IN   uint32                                dram_ndx,
    SOC_SAND_IN   uint32                                bank_ndx,
    SOC_SAND_IN   uint32                                row_ndx,
    SOC_SAND_IN   uint32                                col_ndx,
    SOC_SAND_IN  ARAD_MMU_DRAM_ADDRESS_SPACE_TBL_DATA*  MMU_dram_address_space_tbl_data
  );


uint32
  arad_mmu_idf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_IDF_TBL_DATA* MMU_idf_tbl_data
  );


uint32
  arad_mmu_idf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_IDF_TBL_DATA* MMU_idf_tbl_data
  );


uint32
  arad_mmu_fdf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_FDF_TBL_DATA* MMU_fdf_tbl_data
  );


uint32
  arad_mmu_fdf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_FDF_TBL_DATA* MMU_fdf_tbl_data
  );


uint32
  arad_mmu_rdfa_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RDFA_WADDR_STATUS_TBL_DATA* MMU_rdfa_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfa_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RDFA_WADDR_STATUS_TBL_DATA* MMU_rdfa_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfb_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RDFB_WADDR_STATUS_TBL_DATA* MMU_rdfb_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfb_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RDFB_WADDR_STATUS_TBL_DATA* MMU_rdfb_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfc_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RDFC_WADDR_STATUS_TBL_DATA* MMU_rdfc_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfc_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RDFC_WADDR_STATUS_TBL_DATA* MMU_rdfc_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfd_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RDFD_WADDR_STATUS_TBL_DATA* MMU_rdfd_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfd_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RDFD_WADDR_STATUS_TBL_DATA* MMU_rdfd_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfe_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RDFE_WADDR_STATUS_TBL_DATA* MMU_rdfe_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdfe_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RDFE_WADDR_STATUS_TBL_DATA* MMU_rdfe_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdff_waddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RDFF_WADDR_STATUS_TBL_DATA* MMU_rdff_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdff_waddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RDFF_WADDR_STATUS_TBL_DATA* MMU_rdff_waddr_status_tbl_data
  );


uint32
  arad_mmu_rdf_raddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RDF_RADDR_TBL_DATA* MMU_rdf_raddr_tbl_data
  );


uint32
  arad_mmu_rdf_raddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RDF_RADDR_TBL_DATA* MMU_rdf_raddr_tbl_data
  );


uint32
  arad_mmu_waf_halfa_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAF_HALFA_WADDR_TBL_DATA* MMU_waf_halfa_waddr_tbl_data
  );


uint32
  arad_mmu_waf_halfa_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAF_HALFA_WADDR_TBL_DATA* MMU_waf_halfa_waddr_tbl_data
  );


uint32
  arad_mmu_waf_halfb_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAF_HALFB_WADDR_TBL_DATA* MMU_waf_halfb_waddr_tbl_data
  );


uint32
  arad_mmu_waf_halfb_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAF_HALFB_WADDR_TBL_DATA* MMU_waf_halfb_waddr_tbl_data
  );


uint32
  arad_mmu_wafa_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafa_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafa_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFA_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafa_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafb_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafb_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafb_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFB_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafb_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafc_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafc_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafc_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFC_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafc_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafd_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafd_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafd_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFD_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafd_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafe_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafe_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafe_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFE_HALFA_RADDR_STATUS_TBL_DATA* MMU_wafe_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_waff_halfa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA* MMU_waff_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_waff_halfa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFF_HALFA_RADDR_STATUS_TBL_DATA* MMU_waff_halfa_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafa_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafa_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafa_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFA_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafa_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafb_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafb_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafb_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFB_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafb_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafc_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafc_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafc_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFC_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafc_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafd_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafd_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafd_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFD_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafd_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafe_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafe_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_wafe_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFE_HALFB_RADDR_STATUS_TBL_DATA* MMU_wafe_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_waff_halfb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA* MMU_waff_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_waff_halfb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_WAFF_HALFB_RADDR_STATUS_TBL_DATA* MMU_waff_halfb_raddr_status_tbl_data
  );


uint32
  arad_mmu_raf_waddr_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RAF_WADDR_TBL_DATA* MMU_raf_waddr_tbl_data
  );


uint32
  arad_mmu_raf_waddr_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RAF_WADDR_TBL_DATA* MMU_raf_waddr_tbl_data
  );


uint32
  arad_mmu_rafa_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RAFA_RADDR_STATUS_TBL_DATA* MMU_rafa_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafa_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RAFA_RADDR_STATUS_TBL_DATA* MMU_rafa_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafb_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RAFB_RADDR_STATUS_TBL_DATA* MMU_rafb_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafb_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RAFB_RADDR_STATUS_TBL_DATA* MMU_rafb_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafc_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RAFC_RADDR_STATUS_TBL_DATA* MMU_rafc_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafc_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RAFC_RADDR_STATUS_TBL_DATA* MMU_rafc_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafd_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RAFD_RADDR_STATUS_TBL_DATA* MMU_rafd_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafd_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RAFD_RADDR_STATUS_TBL_DATA* MMU_rafd_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafe_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RAFE_RADDR_STATUS_TBL_DATA* MMU_rafe_raddr_status_tbl_data
  );


uint32
  arad_mmu_rafe_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RAFE_RADDR_STATUS_TBL_DATA* MMU_rafe_raddr_status_tbl_data
  );


uint32
  arad_mmu_raff_raddr_status_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MMU_RAFF_RADDR_STATUS_TBL_DATA* MMU_raff_raddr_status_tbl_data
  );


uint32
  arad_mmu_raff_raddr_status_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MMU_RAFF_RADDR_STATUS_TBL_DATA* MMU_raff_raddr_status_tbl_data
  );


uint32
  arad_ipt_select_source_sum_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_OUT  ARAD_IPT_SELECT_SOURCE_SUM_TBL_DATA* IPT_select_source_sum_tbl_data
  );



uint32
  arad_mmu_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );


uint32
  arad_fdt_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );


uint32
  arad_ihb_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  );




uint32
  arad_ire_nif_port_to_ctxt_bit_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA  *IRE_nif_port_to_ctxt_bit_map_tbl_data
  );


uint32
  arad_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA  *IRE_nif_port_to_ctxt_bit_map_tbl_data
  );


uint32
  arad_ire_tdm_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  );


uint32
  arad_ire_tdm_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  );


uint32
  arad_idr_context_mru_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IDR_CONTEXT_MRU_TBL_DATA  *tbl_data
  );


uint32
  arad_idr_context_mru_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IDR_CONTEXT_MRU_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_free_pcb_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_free_pcb_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_pcb_link_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_pcb_link_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_is_free_pcb_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_IS_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_is_free_pcb_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_IS_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_is_pcb_link_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_IS_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_is_pcb_link_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_IS_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_rpf_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_RPF_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_rpf_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_RPF_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_mcr_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_MCR_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_mcr_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_MCR_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_isf_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_ISF_MEMORY_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_isf_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_ISF_MEMORY_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_lag_to_lag_range_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_lag_to_lag_range_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_lag_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_OUT ARAD_IRR_LAG_MAPPING_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_lag_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_IN  ARAD_IRR_LAG_MAPPING_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_lag_next_member_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_LAG_NEXT_MEMBER_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_lag_next_member_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_LAG_NEXT_MEMBER_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_traffic_class_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA  *tbl_data
  );


uint32
  arad_irr_traffic_class_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_stack_trunk_resolve_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_OUT ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_DATA  *tbl_data
  );

uint32
  arad_irr_stack_trunk_resolve_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihp_port_mine_table_lag_port_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA  *tbl_data
  );

uint32
  arad_ihp_port_mine_table_lag_port_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA  *tbl_data
  );


uint32
  arad_ihp_tm_port_pp_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  );


uint32
  arad_ihp_tm_port_pp_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  );


int
  arad_ihp_tm_port_sys_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 entry_offset,
    SOC_SAND_OUT ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA  *tbl_data
  );


int
  arad_ihp_tm_port_sys_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core_id,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_IN  ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA  *tbl_data
  );


uint32
  arad_iqm_cnts_mem_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID   proc_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IQM_CNTS_MEM_TBL_DATA  *tbl_data
  );


uint32
  arad_iqm_cnts_mem_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID   proc_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IQM_CNTS_MEM_TBL_DATA  *tbl_data
  );


uint32
  arad_iqm_ovth_mema_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IQM_OVTH_MEMA_TBL_DATA  *tbl_data
  );


uint32
  arad_iqm_ovth_mema_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IQM_OVTH_MEMA_TBL_DATA  *tbl_data
  );


uint32
  arad_iqm_ovth_memb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IQM_OVTH_MEMB_TBL_DATA  *tbl_data
  );


uint32
  arad_iqm_ovth_memb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IQM_OVTH_MEMB_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_cfg_byte_cnt_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IPT_CFG_BYTE_CNT_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_cfg_byte_cnt_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IPT_CFG_BYTE_CNT_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_egq_txq_wr_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IPT_EGQ_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_egq_txq_wr_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IPT_EGQ_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_egq_txq_rd_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_egq_txq_rd_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_fdt_txq_wr_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IPT_FDT_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_fdt_txq_wr_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IPT_FDT_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_fdt_txq_rd_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IPT_FDT_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_fdt_txq_rd_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IPT_FDT_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_gci_backoff_mask_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IPT_GCI_BACKOFF_MASK_TBL_DATA  *tbl_data
  );

uint32
  arad_ipt_gci_backoff_mask_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IPT_GCI_BACKOFF_MASK_TBL_DATA  *tbl_data
  );


uint32
  arad_fdt_ipt_contro_l_fifo_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_FDT_IPT_CONTRO_L_FIFO_TBL_DATA  *tbl_data
  );



uint32
  arad_egq_qm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_QM_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_qm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_QM_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_qsm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_QSM_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_qsm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_QSM_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_dcm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_DCM_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_dcm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_DCM_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_dwm_new_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_DWM_NEW_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_dwm_new_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_DWM_NEW_TBL_DATA  *tbl_data
  );

void
  ARAD_EGQ_TC_DP_MAP_TBL_ENTRY_clear(
    SOC_SAND_OUT ARAD_EGQ_TC_DP_MAP_TBL_ENTRY       *info
  );
void 
  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(
     ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA *tbl_data
     );

int
  arad_egq_tc_dp_map_tbl_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGQ_TC_DP_MAP_TBL_ENTRY   *entry,
    SOC_SAND_OUT ARAD_EGQ_TC_DP_MAP_TBL_DATA    *tbl_data
  );


int
  arad_egq_tc_dp_map_tbl_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGQ_TC_DP_MAP_TBL_ENTRY   *entry,
    SOC_SAND_IN  ARAD_EGQ_TC_DP_MAP_TBL_DATA    *tbl_data
  );



uint32
  arad_egq_fqp_nif_port_mux_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  );


int
  arad_egq_fqp_nif_port_mux_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_pqp_nif_port_mux_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  );


int
  arad_egq_pqp_nif_port_mux_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_key_profile_map_index_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fwd_type_ndx,
    SOC_SAND_IN  uint32              acl_profile_ndx,
    SOC_SAND_OUT ARAD_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_key_profile_map_index_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fwd_type_ndx,
    SOC_SAND_IN  uint32              acl_profile_ndx,
    SOC_SAND_IN  ARAD_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_tcam_key_resolution_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA  *tbl_data
  );


uint32
  arad_egq_tcam_key_resolution_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_parser_last_sys_record_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_parser_last_sys_record_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_parser_last_nwk_record1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_parser_last_nwk_record1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_parser_last_nwk_record2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_parser_last_nwk_record2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_erpp_debug_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EGQ_ERPP_DEBUG_TBL_DATA  *tbl_data
  );

uint32
  arad_egq_erpp_debug_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_ERPP_DEBUG_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_pinfo_lbp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_PINFO_LBP_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_pinfo_lbp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_PINFO_LBP_TBL_DATA  *tbl_data
  );



uint32
  arad_pp_ihb_pmf_program_selection_cam_tbl_data_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT  uint32 data[SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)]
  );

uint32
  arad_pp_ihb_pmf_2nd_pass_program_selection_cam_tbl_data_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT  uint32 data[SOC_DPP_IMP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)]
  );

uint32
  arad_pp_ihb_slb_program_selection_cam_tbl_data_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT  uint32 data[ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE]
  );

uint32
  arad_pp_ihb_slb_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_pmf_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_pmf_2nd_pass_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_pmf_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_pmf_2nd_pass_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PMF_2ND_PASS_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );

uint32
  arad_ihb_pinfo_pmf_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_PINFO_PMF_TBL_DATA  *tbl_data
  );
uint32
  arad_ihb_pinfo_pmf_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id, 
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_PINFO_PMF_TBL_DATA  *tbl_data
  );



uint32
  arad_ihb_ptc_info_pmf_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,   
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_PTC_INFO_PMF_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_ptc_info_pmf_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_PTC_INFO_PMF_TBL_DATA  *tbl_data
  );



uint32
  arad_ihb_packet_format_code_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_packet_format_code_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_src_dest_port_for_l3_acl_key_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_src_dest_port_for_l3_acl_key_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA  *tbl_data
  );




uint32
  arad_ihb_fem_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem1_4b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem0_4b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem1_4b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem0_4b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem1_4b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem0_4b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_ihb_fem1_4b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );




uint32
  arad_ihb_direct_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_direct_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem0_4b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem1_4b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem2_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem3_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem4_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem5_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem6_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_ihb_fem7_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  );





uint32
  arad_epni_copy_engine_program_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA  *tbl_data
  );


uint32
  arad_epni_copy_engine_program_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA  *tbl_data
  );

uint32
  arad_epni_lfem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_epni_lfem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_pmf_program_selection_cam_tbl_data_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT uint32   data[ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE]
  );


uint32
  arad_pp_egq_pmf_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );

uint32
  arad_pp_egq_pmf_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );


uint32
  arad_cfc_rcl2_ofp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_RCL2_OFP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_rcl2_ofp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_RCL2_OFP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_nifclsb2_ofp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_NIFCLSB2_OFP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_nifclsb2_ofp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_NIFCLSB2_OFP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_oob_calrx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_CALRX_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_oob_calrx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_CALRX_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_oob_sch_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_oob_sch_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_oob_caltx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_CALTX_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_oob_caltx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_CALTX_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_ilkn_calrx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_CALRX_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_ilkn_calrx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_CALRX_TBL_DATA  *tbl_data
  );

uint32
  arad_cfc_hcfc_bitmap_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry_offset,
    SOC_SAND_IN  ARAD_CFC_HCFC_BITMAP_TBL_DATA  *tbl_data
  );

uint32
  arad_cfc_hcfc_bitmap_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry_offset,
    SOC_SAND_OUT ARAD_CFC_HCFC_BITMAP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_ilkn_sch_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_ilkn_sch_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_ilkn_caltx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_CALTX_TBL_DATA  *tbl_data
  );


uint32
  arad_cfc_ilkn_caltx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_CALTX_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_pinfo_llr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PINFO_LLR_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_pinfo_llr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PINFO_LLR_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_llr_llvp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LLR_LLVP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_llr_llvp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LLR_LLVP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_llr_llvp_tbl_fill_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LLR_LLVP_TBL_DATA *tbl_data
  );

uint32
  arad_tbl_access_em_step_tbls_init(
    int  unit
  );

uint32
  arad_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_subnet_classify_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_subnet_classify_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_port_protocol_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PORT_PROTOCOL_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_port_protocol_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PORT_PROTOCOL_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_tos_2_cos_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_TOS_2_COS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_tos_2_cos_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_TOS_2_COS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_reserved_mc_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_RESERVED_MC_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_reserved_mc_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_RESERVED_MC_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_flush_db_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_flush_db_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_ihp_mact_aging_configuration_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_mact_aging_configuration_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA *tbl_data
  );


int
  arad_pp_ihp_tm_port_parser_program_pointer_config_tbl_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_DATA  *tbl_data
   );

int
  arad_pp_ihp_tm_port_parser_program_pointer_config_tbl_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_pp_port_info_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_pp_port_info_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA  *tbl_data
  );
uint32
  arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_virtual_port_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_virtual_port_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_src_system_port_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_src_system_port_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_program_pointer_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_program_pointer_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_program_pointer_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_program_pointer_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_program_pointer_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_program_pointer_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_program_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              array_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA  *tbl_data
  );
uint32
  arad_pp_ihp_parser_program_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              array_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_packet_format_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_packet_format_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_custom_macro_parameters_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_ip_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_ip_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_custom_macro_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_fid_class_2_fid_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int              core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_designated_vlan_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_designated_vlan_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int            core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_pwe_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_pwe_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_pbb_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_pbb_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ac_mp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ac_mp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_isid_p2p_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_isid_p2p_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_isid_mp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_isid_mp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_trill_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_trill_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ip_tt_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_ip_tt_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_label_pwe_p2p_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_label_pwe_p2p_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_label_pwe_mp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_label_pwe_mp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_DATA *tbl_data
  );



uint32
  arad_pp_ihp_stp_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_STP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_stp_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_STP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vrid_my_mac_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vrid_my_mac_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_ihp_vtt_llvp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT_LLVP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt_llvp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT_LLVP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_isem_1st_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_isem_1st_program_selection_convert_tbl_data_to_data(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT uint32 *data
  );

uint32
  arad_pp_ihp_isem_1st_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_isem_2nd_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_isem_2nd_program_selection_convert_tbl_data_to_data(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT uint32 *data
  );

uint32
  arad_pp_ihp_isem_2nd_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA  *tbl_data
  );
uint32
  arad_pp_ihp_vtt1st_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt1st_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_ingress_vlan_edit_command_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_rif_profile_to_rif_info_convert(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 rif_profile,
    SOC_SAND_OUT uint8  *uc_rpf_enable,
    SOC_SAND_OUT uint8  *enable_default_routing,
    SOC_SAND_OUT uint8  *ipmc_bridge_v4mc_enable,
    SOC_SAND_OUT uint8  *intf_class,
    SOC_SAND_OUT uint8  *routing_enablers_bm_id,
    SOC_SAND_OUT uint32 *routing_enablers_bm,
    SOC_SAND_IN  uint8  is_routing_enablers_needed
  );

uint32
  arad_pp_rif_info_to_rif_profile_convert(
    SOC_SAND_IN  int    unit,    
    SOC_SAND_IN  uint32 uc_rpf_enable,
    SOC_SAND_IN  uint32 enable_default_routing,
    SOC_SAND_IN  uint32 ipmc_bridge_v4mc_enable,
    SOC_SAND_IN  uint32 intf_class,    
    SOC_SAND_IN  uint32 routing_enablers_bm_id,
    SOC_SAND_OUT uint32 *rif_profile
  );

uint32
  arad_pp_ihp_map_rif_profile_to_routing_enable_tbl_set_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 entry_offset,
    SOC_SAND_IN  uint32 tbl_data
  );

uint32
  arad_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vlan_edit_pcp_dei_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_in_rif_config_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_in_rif_config_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_tc_dp_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_tc_dp_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA  *tbl_data
  );

int
  arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  uint32     entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PINFO_FER_TBL_DATA *tbl_data
  );


int
  arad_pp_ihb_pinfo_fer_tbl_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  uint32     entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PINFO_FER_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_lb_pfc_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_LB_PFC_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_lb_pfc_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_LB_PFC_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_lb_vector_program_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_lb_vector_program_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_fec_ecmp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FEC_ECMP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_fec_ecmp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_ECMP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_fec_super_entry_utilize_desc_dma(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN uint8 enable
  );

uint32
  arad_pp_ihb_fec_super_entry_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fec_index,
    SOC_SAND_OUT ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_fec_super_entry_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fec_index,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_fec_super_entry_tbl_fill_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_fec_entry_general_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_fec_entry_general_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA *tbl_data
  );

uint32
arad_pp_ihb_fec_entry_general_tbl_uc_rpf_mode_set(
    int unit,
	uint32 entry_offset,
	uint32 uc_rpf_mode
);

uint32
  arad_pp_ihb_fec_entry_accessed_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
	SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  int                                      core_id,
    SOC_SAND_IN  uint32                                   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_path_select_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PATH_SELECT_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_path_select_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PATH_SELECT_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_destination_status_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_destination_status_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_destination_status_tbl_fill_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA *tbl_data
  );


uint32
  arad_pp_ihb_fwd_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_fwd_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *tbl_data,
    SOC_SAND_IN  int                                    core_id
  );


uint32
  arad_pp_ihb_snoop_action_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_SNOOP_ACTION_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_snoop_action_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_SNOOP_ACTION_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_ihb_pinfo_flp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PINFO_FLP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_pinfo_flp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PINFO_FLP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_flp_ce_resources_per_program_get_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 entry_offset,
    SOC_SAND_OUT uint32 *ce_rsrc
  );

uint32
  arad_pp_ihb_flp_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_flp_key_construction_ext_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  int  is_msb,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA *tbl_data
  );


uint32
  arad_pp_ihb_flp_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_flp_lookups_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_flp_lookups_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_flp_process_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_PROCESS_TBL_DATA *tbl_data
  );

soc_field_t
  arad_pp_ihb_flp_key_field_get(
     SOC_SAND_IN  int      unit,
     SOC_SAND_IN  uint32   instruction_id
  );

uint32
  arad_pp_ihb_flp_process_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_vrf_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_VRF_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_vrf_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_VRF_CONFIG_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_header_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_HEADER_PROFILE_TBL_DATA  *tbl_data
  );
uint32
  arad_pp_ihb_header_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_HEADER_PROFILE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_lpm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_LPM_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_lpm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  uint32              nof_entries,
    SOC_SAND_OUT uint32              *tbl_data
  );


uint32
  arad_pp_ihb_tcam_action_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  uint8               is_for_hit_bit,
    SOC_SAND_OUT ARAD_PP_IHB_TCAM_ACTION_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihb_tcam_action_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              bank_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  uint8               is_for_hit_bit,
    SOC_SAND_IN  ARAD_PP_IHB_TCAM_ACTION_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_egq_pp_ppct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_PP_PPCT_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_pp_ppct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_PP_PPCT_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_vsi_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_vsi_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_egress_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_egress_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_ttl_scope_reg_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_TTL_SCOPE_REG_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_ttl_scope_reg_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_TTL_SCOPE_REG_TBL_DATA  *tbl_data
  );
uint32
  arad_pp_egq_ttl_scope_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   rif_id,
    SOC_SAND_OUT ARAD_PP_EGQ_TTL_SCOPE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_egq_ttl_scope_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   rif_id,
    SOC_SAND_IN  ARAD_PP_EGQ_TTL_SCOPE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_egq_aux_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_AUX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_aux_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_AUX_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_cfm_trap_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_CFM_TRAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_cfm_trap_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_CFM_TRAP_TBL_DATA  *tbl_data
  );
uint32
  arad_pp_egq_action_profile_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_egq_action_profile_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA  *tbl_data
  );
uint32
  arad_pp_epni_tx_tag_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_tx_tag_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA  *tbl_data
  );


uint32
  arad_pp_epni_stp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_STP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_stp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_STP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_pcp_dei_map_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PCP_DEI_MAP_TBL_DATA *tbl_data
  );


uint32
  arad_pp_epni_pcp_dei_map_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PCP_DEI_MAP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_dp_map_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_DP_MAP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_dp_map_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_DP_MAP_TBL_DATA *tbl_data
  );
  

int
  arad_pp_epni_pp_pct_tbl_get_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  uint32     pp_port,
    SOC_SAND_OUT ARAD_PP_EPNI_PP_PCT_TBL_DATA *tbl_data
  );

int
  arad_pp_epni_pp_pct_tbl_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  uint32     pp_port,
    SOC_SAND_IN  ARAD_PP_EPNI_PP_PCT_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_llvp_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_llvp_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_egress_edit_cmd_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_egress_edit_cmd_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA  *tbl_data
  );
uint32
  arad_pp_epni_ingress_vlan_edit_command_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_exp_remark_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_EXP_REMARK_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_exp_remark_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_EXP_REMARK_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_dscp_remark_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_DSCP_REMARK_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_dscp_remark_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_DSCP_REMARK_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_mpls_to_exp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_mpls_to_exp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_mpls_to_dscp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_mpls_to_dscp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_ipv6_to_exp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_ipv6_to_exp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_ipv6_to_dscp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_ipv6_to_dscp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_ipv4_to_exp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_ipv4_to_exp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_ipv4_to_dscp_tbl_get_unsafe(
	SOC_SAND_IN  int  unit,
	SOC_SAND_IN  uint32   entry_offset,
	SOC_SAND_OUT ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_remark_ipv4_to_dscp_tbl_set_unsafe(
	SOC_SAND_IN  int  unit,
	SOC_SAND_IN  uint32   entry_offset,
	SOC_SAND_IN  ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_DATA *tbl_data
  );



uint32 arad_pp_ihb_tcam_tbl_cache_move(
            SOC_SAND_IN   int       unit,
            SOC_SAND_IN   uint32    bank,
            SOC_SAND_IN   uint32    move_couple,
            SOC_SAND_IN   uint32    src_hw_entry,
            SOC_SAND_IN   uint32    dest_hw_entry,
            SOC_SAND_IN   uint32    mov_num
        );



uint32 arad_set_tcam_cache(int unit, uint32 enable);


int arad_tcam_bank_entry_rewrite_from_shadow(int unit, uint32 bank, uint32 entry);


int arad_tcam_bank_entry_shadow_info_dump(int unit, uint32 bank, uint32 entry, uint32 from_cache);


uint32 arad_tcam_shadow_content_compare_validate(int unit);


uint32 arad_tcam_bank_shadow_info_summary(int unit);


int arad_pp_ihb_tcam_flush_all_bank_shadow(int unit);



uint32
  arad_pp_ihb_tcam_tbl_read_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           entry_offset,
    SOC_SAND_OUT  ARAD_PP_IHB_TCAM_BANK_TBL_DATA    *data
  );



#define ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE   (0x3)

uint32
  arad_pp_ihb_tcam_tbl_write_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                            entry_offset,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_BANK_TBL_DATA     *data
  );

uint32
  arad_pp_ihb_pmf_pass_2_key_update_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihb_pmf_pass_2_key_update_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_DATA *tbl_data
  );


uint32
  arad_pp_ihb_tcam_access_profile_tbl_read_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           access_profile_id,
    SOC_SAND_OUT  ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA    *data
  );


uint32
  arad_pp_ihb_tcam_access_profile_tbl_write_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           access_profile_id,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA     *data
  );

uint32
  arad_pp_ihb_tcam_pd_profile_tbl_read_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           pd_profile_id,
    SOC_SAND_OUT  ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA    *data
  );

uint32
  arad_pp_ihb_tcam_pd_profile_tbl_write_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           pd_profile_id,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA     *data
  );


uint32
  arad_pp_ihb_tcam_invalid_bit_set_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                            entry_offset
  );



uint32
  arad_pp_ihb_tcam_tbl_flush_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx
  );


uint32
  arad_pp_ihb_tcam_tbl_compare_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_BANK_TBL_DATA     *compare_data,
    SOC_SAND_OUT  ARAD_PP_IHB_TCAM_COMPARE_DATA      *found_data
  );


uint32
  arad_pp_ihb_tcam_tbl_move_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           move_couple,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           source_entry_start_ndx,
    SOC_SAND_IN   uint32                           source_entry_end_ndx,
    SOC_SAND_IN   uint32                           dest_entry_start_ndx
  );

uint32
  arad_pp_ihb_tcam_action_mem_and_index_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              action_tbl_id,
    SOC_SAND_IN  uint8               is_for_hit_bit,
    SOC_SAND_OUT soc_mem_t           *mem_tbl,
    SOC_SAND_OUT uint32              *arr_index
  );


uint32
  arad_pp_ihp_vsi_low_cfg_1_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_vsi_low_cfg_1_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA *tbl_data
  );


uint32
  arad_pp_ihp_vsi_low_cfg_2_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_vsi_low_cfg_2_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_vsi_my_mac_tbl_set_unsafe(int unit, uint32 entry_offset, uint8 enable_my_mac);

uint32
  arad_pp_ihp_vsi_my_mac_tbl_get_unsafe(int unit, uint32 entry_offset, uint8* enable_my_mac);

uint32
  arad_pp_ihp_vsi_high_info_tbl_set_unsafe(int unit, uint32 entry_offset, uint32 frwrd_dest, uint32 profile_ingress);

uint32
  arad_pp_ihp_vsi_high_info_tbl_get_unsafe(int unit, uint32 entry_offset, uint32* frwrd_dest, uint32* profile_ingress);

uint32
  arad_pp_ihp_vsi_high_my_mac_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_vsi_high_my_mac_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA *tbl_data
  );


uint32
  arad_pp_ihp_vsi_high_profile_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_vsi_high_profile_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_vsi_high_da_not_found_destination_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_vsi_high_da_not_found_destination_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_mact_fid_counter_db_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_mact_entry_count_db_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_mact_fid_counter_db_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_mact_fid_profile_db_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_mact_fid_profile_db_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_mact_fid_counter_profile_db_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_vlan_port_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vlan_port_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_epni_isid_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_ISID_TABLE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_isid_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_ISID_TABLE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_termination_profile_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_termination_profile_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_action_profile_mpls_value_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_action_profile_mpls_value_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_0_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_0_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_1_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_1_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_2_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_2_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_4_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_4_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_5_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_5_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_6_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_6_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_7_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_format_7_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_special_format_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_special_format_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_tdm_format_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_egress_tdm_format_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_TBL_DATA *tbl_data
  );

uint32
  arad_pp_irr_mcdb_tbl_get_raw_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT uint64 *tbl_data
  );

uint32
  arad_pp_irr_mcdb_tbl_set_raw_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  uint64 tbl_data
  );
  

uint32
  arad_pp_irr_mcdb_tbl_get_next_entry_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  uint8  is_ingress,
    SOC_SAND_OUT uint32   *link_ptr
  );
  

uint32
  arad_pp_irr_mcdb_tbl_set_next_entry_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry_offset,
    SOC_SAND_IN  uint8   is_ingress,
    SOC_SAND_OUT uint32  next_entry
  );


void
  ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA_init(
     int unit,
    ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA   *tbl_data
  );

uint32
  arad_pp_epni_prge_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );
  
uint32
  arad_pp_epni_prge_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  );
  
uint32
  arad_pp_epni_prge_program_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA *tbl_data
  );
  
uint32
  arad_pp_epni_prge_program_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA *tbl_data
  );
  
uint32
  arad_pp_epni_prge_instruction_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  soc_mem_t	memory_offset,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA *tbl_data
  );
  
uint32
  arad_pp_epni_prge_instruction_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  soc_mem_t	memory_offset,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_prge_data_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PRGE_DATA_TBL_DATA *tbl_data
  );

uint32
  arad_pp_epni_prge_data_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PRGE_DATA_TBL_DATA *tbl_data
  );

int
  arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  uint32     tm_port,
    SOC_SAND_OUT ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA *tbl_data
  );

int
  arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  uint32     tm_port,
    SOC_SAND_IN  ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_lif_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_DATA  *tbl_data
  );

uint32
  arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  int             core_id,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_DATA  *tbl_data
  );

uint32
  arad_iqm_cnm_profile_tbl_get_unsafe (
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                   profile_ndx,
    SOC_SAND_OUT ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_DATA   *tbl_data
  );

uint32
  arad_iqm_cnm_profile_tbl_set_unsafe (
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                   profile_ndx,
    SOC_SAND_IN  ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_DATA   *tbl_data
  );

uint32
  arad_iqm_cnm_ds_tbl_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     entry_offset,
    SOC_SAND_OUT ARAD_IQM_CNM_DS_TBL_DATA   *tbl_data
  );

uint32
  arad_iqm_cnm_ds_tbl_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     entry_offset,
    SOC_SAND_IN  ARAD_IQM_CNM_DS_TBL_DATA   *tbl_data
  );


uint32
  arad_fill_table_with_entry(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN int       copyno,
    SOC_SAND_IN void      *data
  );

int
arad_fill_memory_with_incremental_field(const int unit, const soc_mem_t mem, const soc_field_t field,
                                unsigned array_index_min, unsigned array_index_max,
                                const int copyno,
                                int index_min, int index_max,
                                const void *initial_entry);


uint32
  arad_fill_partial_table_with_entry(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN unsigned  array_index_start,
    SOC_SAND_IN unsigned  array_index_end,
    SOC_SAND_IN int       copyno,
    SOC_SAND_IN int       index_start,
    SOC_SAND_IN int       index_end,
    SOC_SAND_IN void      *data
  );

soc_mem_t
  arad_egq_scm_chan_arb_id2scm_id(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 chan_arb_id
  );

typedef int (*arad_fill_table_with_variable_values_by_caching_callback)(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int copyno, 
    SOC_SAND_IN int array_index, 
    SOC_SAND_IN int index, 
    SOC_SAND_OUT uint32 *value, 
    SOC_SAND_IN int entry_sz,
    SOC_SAND_IN void *opaque);


int arad_update_table_with_cache(int unit, soc_mem_t mem);


soc_error_t
  arad_fill_table_with_variable_values_by_caching(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN unsigned  array_index,
    SOC_SAND_IN int       copyno,
    SOC_SAND_IN int       index_start,
    SOC_SAND_IN int       index_end,
    SOC_SAND_IN arad_fill_table_with_variable_values_by_caching_callback callback,
    SOC_SAND_IN void *    opaque
  );

soc_error_t arad_tbl_access_deinit(SOC_SAND_IN int unit);

soc_error_t arad_tbl_access_init_unsafe(SOC_SAND_IN int unit);

soc_error_t _arad_palladium_backdoor_dispatch_full_table_write(
           SOC_SAND_IN int unit,
           SOC_SAND_IN char *sw_mem_name,
           SOC_SAND_IN uint32 entry_data[],
           SOC_SAND_IN uint32 entry_data_len
           );


void arad_tbl_access_tcam_print(SOC_SAND_IN ARAD_PP_IHB_TCAM_BANK_TBL_DATA *data);



SOC_ERROR_DESC_ELEMENT*
  arad_tbl_access_get_errs_ptr(void);

      
int arad_tbl_mem_cache_mem_set( int unit, soc_mem_t mem, void* en);
int arad_tbl_mem_cache_enable_parity_tbl(int unit, soc_mem_t mem, void* en);
int arad_tbl_mem_cache_enable_ecc_tbl(int unit, soc_mem_t mem, void* en);
int arad_tbl_mem_cache_enable_specific_tbl(int unit, soc_mem_t mem, void* en);
int arad_tbl_cache_enable_predefined_tbl(int unit);

                                                              
#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif
