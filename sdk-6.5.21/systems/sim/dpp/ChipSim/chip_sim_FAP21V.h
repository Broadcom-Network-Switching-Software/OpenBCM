/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/




#ifndef __CHIP_SIM_FAP21V_H_INCLUDED__
/* { */
#define __CHIP_SIM_FAP21V_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

/*************
 *  MACROS   *
 */
/* { */

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * INCLUDES  *
 */
/* { */
#include "chip_sim.h"
#include "chip_sim_counter.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_indirect.h"
/* } */

/*************
 * DEFINES   *
 */
/* { */

/* table index definitions { */
#define FAP21V_EGQ_FOP_MULTICAST_LUT0_TBL_ID                                                 0
#define FAP21V_EGQ_FOP_MULTICAST_LUT1_TBL_ID                                                 1
#define FAP21V_EGQ_FOP_MULTICAST_LUT2_TBL_ID                                                 2
#define FAP21V_EGQ_MULTICAST_PRUNING_LUT_TBL_ID                                              3
#define FAP21V_EGQ_SCHEDULED_QUEUES_VS_UNSCHEDULED_QUEUES_WEIGHT_TBL_ID                      4
#define FAP21V_EGQ_FREE_BUFF_NO_RD_CNT_TBL_ID                                                5
#define FAP21V_EGQ_QUEUE_SIZE_DESC_TBL_ID                                                    6
#define FAP21V_EGQ_QUEUE_SIZE_TBL_ID                                                         7
#define FAP21V_EGQ_SHAPER_CALENDAR_TABLE_TBL_ID                                              8
#define FAP21V_EGQ_CREDIT_BALANCE_TBL_ID                                                     9
#define FAP21V_FDT_MESH_SATIAL_MULTICAST_TBL_ID                                             10
#define FAP21V_INQ_IPA_TBL_ID                                                               11
#define FAP21V_INQ_IPB_TBL_ID                                                               12
#define FAP21V_INQ_IPASIZE_TBL_ID                                                           13
#define FAP21V_INQ_IPBSIZE_TBL_ID                                                           14
#define FAP21V_INQ_TDMA_TBL_ID                                                              15
#define FAP21V_INQ_TDMB_TBL_ID                                                              16
#define FAP21V_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ID                                 17
#define FAP21V_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ID                          18
#define FAP21V_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ID                                              19
#define FAP21V_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ID                                           20
#define FAP21V_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ID                                         21
#define FAP21V_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ID                                         22
#define FAP21V_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ID                                 23
#define FAP21V_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ID                             24
#define FAP21V_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ID                                  25
#define FAP21V_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_ID                                            26
#define FAP21V_IPS_QUEUE_SIZE_TABLE_TBL_ID                                                  27
#define FAP21V_IPS_FLOW_STATUS_MESSAGE_REQUEST_QUEUE_CONTROL_TABLE_TBL_ID                   28
#define FAP21V_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_ID                                   29
#define FAP21V_LBP_TREE_ROUTING_ID_MULTICAST_ID_LOOKUP_TABLE_TBL_ID                         30
#define FAP21V_LBP_UNICAST_LOOKUP_TABLE_TBL_ID                                              31
#define FAP21V_LBP_RECYCLING_QUEUES_LOOKUP_TABLE_RECYCLE_FAP_TABLE_TBL_ID                   32
#define FAP21V_QDP_QUEUE_TYPE_TBL_ID                                                        33
#define FAP21V_QDP_QUEUE_TYPE_PARAMETERS_TBL_ID                                             34
#define FAP21V_QDP_QUEUE_WRED_PARAMETERS_TBL_ID                                             35
#define FAP21V_QDP_QUEUE_SIZE_TBL_ID                                                        36
#define FAP21V_QDP_NOT_EMPTY_TBL_ID                                                         37
#define FAP21V_QDP_GLOBAL_STATISTICS_REPORT_THRESHOLDS_SYSTEM_RED_TBL_ID                    38
#define FAP21V_QDP_GLOBAL_STATISTICS_REJECT_THRESHOLDS_TBL_ID                               39
#define FAP21V_RTP_DATA_ROUTE_MEMORY_TBL_ID                                                 40
#define FAP21V_RTP_CONTROL_ROUTE_MEMORY_TBL_ID                                              41
#define FAP21V_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_TBL_ID                          42
#define FAP21V_SCH_DEVICE_RATE_MEMORY_DRM_TBL_ID                                            43
#define FAP21V_SCH_DUAL_SHAPER_MEMORY_DSM_TBL_ID                                            44
#define FAP21V_SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDS_TBL_ID                              45
#define FAP21V_SCH_SCHEDULER_ENABLE_MEMORY_SEM_TBL_ID                                       46
#define FAP21V_SCH_FLOW_SUB_FLOW_FSF_TBL_ID                                                 47
#define FAP21V_SCH_FLOW_GROUP_MEMORY_FGM_TBL_ID                                             48
#define FAP21V_SCH_HR_SCHEDULER_CONFIGURATION_SHC_TBL_ID                                    49
#define FAP21V_SCH_CL_SCHEDULERS_CONFIGURATION_SCC_TBL_ID                                   50
#define FAP21V_SCH_CL_SCHEDULERS_TYPE_SCT_TBL_ID                                            51
#define FAP21V_SCH_FLOW_TO_QUEUE_MAPPING_FQM_TBL_ID                                         52
#define FAP21V_SCH_FLOW_TO_FIP_MAPPING_FFM_TBL_ID                                           53
#define FAP21V_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_ID                                       54
#define FAP21V_SCH_PORT_QUEUE_SIZE_PQS_TBL_ID                                               55
#define FAP21V_SCH_SCHEDULER_INIT_TBL_ID                                                    56
#define FAP21V_SCH_FORCE_STATUS_MESSAGE_TBL_ID                                              57
#define FAP21V_SLA_EGRESS_PORT_TO_SPI4_CHANNEL_TABLE_TBL_ID                                 58
#define FAP21V_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_A_TBL_ID                 59
#define FAP21V_SLA_SPI4_CHANNEL_TO_EGRESS_PORT_TABLE_FOR_INTERFACE_B_TBL_ID                 60
#define FAP21V_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_A_TBL_ID 61
#define FAP21V_SLA_OUT_OF_BAND_FLOW_CONTROL_AUXILIARY_CALENDAR_TABLE_FOR_INTERFACE_B_TBL_ID 62
#define FAP21V_SLA_TDM_MULTICAST_TABLE_TBL_ID                                               63
#define FAP21V_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_A_ID                            64
#define FAP21V_SPI_SOURCE_STATUS_CHANNEL_CALENDAR_TABLE_TBL_B_ID                            65
#define FAP21V_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_A_ID                              66
#define FAP21V_SPI_SINK_STATUS_CHANNEL_CALENDAR_TABLE_TBL_B_ID                              67
#define FAP21V_FDT_MESH_SATIAL_MULTICAST_TABLE_TBL                                          68
#define FAP21V_MMU_DRAM_ADDRESSES_TBL_ID                                                    69
#define FAP21V_MMU_SFM_TBL_ID                                                               70
#define FAP21V_MMU_IQS_TBL_ID                                                               71
#define FAP21V_LBP_INBOUND_MIRROR_DESTINATION_ID_TBL_ID                                     72
#define FAP21V_LBP_OUTBOUND_MIRROR_DESTINATION_ID_TBL_ID                                    73
#define FAP21V_LBP_LAG_SIZE_LOOKUP_TABLE_TBL_ID                                             74
#define FAP21V_LBP_LAG_MODULO_DIVIDE_LOOKUP_TABLE_TBL_ID                                    75
#define FAP21V_LBP_LAG_MEMBERSHIP_LOOKUP_TABLE_TBL_ID                                       76
#define FAP21V_LBP_INCOMING_FAP_PORT_LOOKUP_TABLE_TBL_ID                                    77
#define FAP21V_LBP_SYSTEM_FAP_PORT_LOOKUP_TABLE_TBL_ID                                      78
#define FAP21V_EGQ_DESTINATION_SYSTEM_PORT_LUT_TBL_ID                                       79
#define FAP21V_SLA_DESTINATION_SYSTEM_PORT_LUT_TBL_ID                                       80
#define FAP21V_SCH_FDMS_TBL_TBL_ID                                                          81
#define FAP21V_SPI_PHY_CFG_MIN_ID                                                           82
#define FAP21V_SPI_PHY_CFG_MAX_ID                                                           115
#define FAP21V_LAST_TBL_ID                                                                  116

/* table index definitions } */
/* } */

/*************
 * GLOBALS   *
 */
/* { */
extern
  CHIP_SIM_INDIRECT_BLOCK
    Fap21v_indirect_blocks[];
extern
  CHIP_SIM_COUNTER
    Fap21v_counters[];
extern
  CHIP_SIM_INTERRUPT
    Fap21v_interrupts[];
/* } */

/*************
 * FUNCTIONS *
 */
/* { */
/*****************************************************
*NAME
* fap21v_indirect_init
*TYPE:
*  PROC
*DATE:
*  09/17/07
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *fap21v_tbls.
*REMARKS:
*    None.
*SEE ALSO:
 */
void
  fap21v_indirect_init(
  );

void
  fap21v_initialize_device_values(
    SOC_SAND_OUT uint32   *base_ptr
  );

/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __CHIP_SIM_FAP21V_INCLUDED__*/
#endif
