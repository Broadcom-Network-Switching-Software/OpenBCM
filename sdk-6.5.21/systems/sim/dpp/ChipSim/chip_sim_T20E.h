/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/******************************************************************
*
* FILENAME:       chip_sim_T20E.h
*
* MODULE PREFIX:  AUTO_CODER:TODO
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
 */


#ifndef __CHIP_SIM_T20E_H_INCLUDED__
/* { */ 
#define __CHIP_SIM_T20E_H_INCLUDED__


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
#include <soc/dpp/SAND/Utils/sand_header.h>

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
#define T20E_PP_LLVP_CLASSIFICATION_TBL_ID                 0
#define T20E_PP_IPV4_SUBNET_CAMI_TBL_ID                    1
#define T20E_PP_IPV4_SUBNET_TABLE_TBL_ID                   2
#define T20E_PP_PORT_PROTOCOL_CAMI_TBL_ID                  3
#define T20E_PP_PORT_PROTOCOL_TO_VID_TBL_ID                4
#define T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_ID                5
#define T20E_PP_PCP_DECODING_TBL_ID                        6
#define T20E_PP_TOS_TO_TC_TBL_ID                           7
#define T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_ID     8
#define T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_ID              9
#define T20E_PP_AC_OFFSET_TBL_ID                           10
#define T20E_PP_COS_PROFILE_DECODING_TBL_ID                11
#define T20E_PP_COS_PROFILE_MAPPING_TBL_ID                 12
#define T20E_PP_STP_STATE_TBL_ID                           13
#define T20E_PP_PCP_DEI_MAP_TBL_ID                         14
#define T20E_PP_IVEC_TBL_ID                                15
#define T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_ID           16
#define T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_ID           17
#define T20E_PP_MPLS_COS_MAPPING_TBL_ID                    18
#define T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_ID        19
#define T20E_PP_MPLS_TUNNEL_TBL_ID                         20
#define T20E_PP_PROTECTION_INSTANCE_TBL_ID                 21
#define T20E_PP_DESTINATION_STATUS_TBL_ID                  22
#define T20E_PP_PORT_CONFIG_TBL_ID                         23
#define T20E_PP_MEP_LEVEL_PROFILE_TBL_ID                   24
#define T20E_PP_OAM_ACTION_TBL_ID                          25
#define T20E_PP_FORWARD_ACTION_PROFILE_TBL_ID              26
#define T20E_PP_SNOOP_ACTION_PROFILE_TBL_ID                27
#define T20E_PP_BURST_SIZE_TBL_ID                          28
#define T20E_PP_MANTISSA_DIVISION_TBL_ID                   29
#define T20E_PP_GLAG_RANGE_TBL_ID                          30
#define T20E_PP_MODULO_TBL_ID                              31
#define T20E_PP_GLAG_OUT_PORT_TBL_ID                       32
#define T20E_PP_VID_MIRR_CAMI_TBL_ID                       33
#define T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_ID              34
#define T20E_PP_OUTPUT_PORT_CONFIG1_TBL_ID                 35
#define T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID       36
#define T20E_PP_EGRESS_MPLS_PROFILE1_TBL_ID                37
#define T20E_PP_EGRESS_MPLS_PROFILE2_TBL_ID                38
#define T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_ID     39
#define T20E_PP_EGRESS_OUT_VSI_TBL_ID                      40
#define T20E_PP_EGRESS_LLVP_TABLE_TBL_ID                   41
#define T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ID        42
#define T20E_PP_EGRESS_PCP_DEI_MAP_TBL_ID                  43
#define T20E_PP_EGRESS_TC_DP_TO_UP_TBL_ID                  44
#define T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_ID         45
#define T20E_PP_EGRESS_STP_STATE_TBL_ID                    46
#define T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_ID       47
#define T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_ID            48
#define T20E_PP_EGRESS_OAM_ACTION_TBL_ID                   49
#define T20E_PP_EGRESS_ACTION_PROFILE_TBL_ID               50
#define T20E_PP_EGRESS_SNOOP_PROFILE_TBL_ID                51
#define T20E_PP_MAC_DA_H1_TBL_ID                           52
#define T20E_PP_MAC_DA_H2_TBL_ID                           53
#define T20E_PP_MAC_DA_P_TBL_ID                            54
#define T20E_PP_AC_KEY1_TBL_ID                             55
#define T20E_PP_AC_KEY2_TBL_ID                             56
#define T20E_PP_FEC_LOW_TBL_ID                             57
#define T20E_PP_FEC_HIGH_TBL_ID                            58
#define T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_ID                59
#define T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_ID       60
#define T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_ID     61
#define T20E_PP_MAC_SA_H1_TBL_ID                           62
#define T20E_PP_MAC_SA_H2_TBL_ID                           63
#define T20E_PP_MAC_SA_P_TBL_ID                            64
#define T20E_PP_AC_LOW_TBL_ID                              65
#define T20E_PP_PWE_LOW_TBL_ID                             66
#define T20E_PP_AC_HIGH_TBL_ID                             67
#define T20E_PP_PWE_HIGH_TBL_ID                            68
#define T20E_PP_AGING_TBL_ID                               69
#define T20E_PP_CNT_INP_TBL_ID                             70
#define T20E_PP_EGRESS_COUNTERS_TBL_ID                     71
#define T20E_PP_PLC_TBL_ID                                 72
#define T20E_PP_EGRESS_MPLS_TUNNEL_TBL_ID                  73
#define T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_ID            74
#define T20E_MACT_FID_COUNTER_DB_TBL_ID                    75
#define T20E_OAMP_MOS_DB_TBL_ID                            76
#define T20E_OAMP_MOR_DB_TBL_ID                            77
#define T20E_OAMP_SYS_PORT_TBL_ID                          78
#define T20E_OAMP_LSR_ID_TBL_ID                            79
#define T20E_OAMP_DEFECT_TYPE_TBL_ID                       80
#define T20E_OAMP_MEP_DB_TBL_ID                            81
#define T20E_OAMP_MEP_INDEX_DB_TBL_ID                      82
#define T20E_OAMP_RMEP_DB_TBL_ID                           83
#define T20E_OAMP_RMEP_HASH_0_DB_TBL_ID                    84
#define T20E_OAMP_RMEP_HASH_1_DB_TBL_ID                    85
#define T20E_OAMP_LMDB_TBL_ID                              86
#define T20E_OAMP_MOT_DB_TBL_ID                            87
#define T20E_OAMP_EXT_MEP_DB_TBL_ID                        88
#define T20E_PP_RESERVED_MC_TBL_ID                         89
#define T20E_PP_PCP_DEI_PROFILE_USE_TC_DP_TBL_ID           90
#define T20E_PP_ING_MIB_COUNTER_TBL_ID                     91
#define T20E_PP_EGR_MIB_COUNTER_TBL_ID                     92
#define T20E_TBL_ID_LAST                                   93
/* table index definitions } */
/* } */

/*************
 * GLOBALS   *
 */
/* { */
extern
  CHIP_SIM_INDIRECT_BLOCK
    T20e_indirect_blocks[];
extern
  CHIP_SIM_COUNTER
    T20e_counters[];
extern
  CHIP_SIM_INTERRUPT
    T20e_interrupts[];
/* } */

/*************
 * FUNCTIONS *
 */
/* { */
/*****************************************************
*NAME
* t20e_indirect_init
*TYPE:
*  PROC
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
*    *t20e_tbls.
*REMARKS:
*    None.
*SEE ALSO:
 */
void
  t20e_indirect_init(
  );

void
  t20e_initialize_device_values(
    SOC_SAND_OUT uint32   *base_ptr
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __CHIP_SIM_T20E_INCLUDED__*/
#endif

