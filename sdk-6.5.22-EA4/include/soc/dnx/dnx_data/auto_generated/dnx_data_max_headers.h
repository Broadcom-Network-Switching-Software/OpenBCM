/** \file dnx_data_max_headers.h
 * 
 * MODULE DATA MAX - 
 * Maintain max value (of all devices) per define
 * 
 * Device Data
 * SW component that maintains per device data
 * For additional details about Device Data Component goto 'dnxc_data_mgmt.h'
 *        
 *     
 * 
 * AUTO-GENERATED BY AUTOCODER!
 * DO NOT EDIT THIS FILE!
 */
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MAX_HEADERS_H_
/*{*/
#define _DNX_DATA_MAX_HEADERS_H_
/*
 * INCLUDE FILES:
 * {
 */
/*
 * }
 */

/*!
* \brief This file is only used by DNX (JR2 family). Including it by
* software that is not specific to DNX is an error.
*/
#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * MAX DEFINES:
 * {
 */
/**
 * \brief
 * packet size offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_PACKET_SIZE_OFFSET (66)

/**
 * \brief
 * traffic class offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TRAFFIC_CLASS_OFFSET (63)

/**
 * \brief
 * source system port aggregate offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_SRC_SYS_PORT_AGGREGATE_OFFSET (47)

/**
 * \brief
 * PP DSP offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_PP_DSP_OFFSET (39)

/**
 * \brief
 * Drop Precedence offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_DROP_PRECEDENCE_OFFSET (37)

/**
 * \brief
 * tm action type offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_ACTION_TYPE_OFFSET (35)

/**
 * \brief
 * tm action is multicast offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_ACTION_IS_MC_OFFSET (34)

/**
 * \brief
 * multicast_id or mc_rep_idx or outlif offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_OUTLIF_OFFSET (12)

/**
 * \brief
 * CNI offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_CNI_OFFSET (11)

/**
 * \brief
 * ECN enable offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ECN_ENABLE_OFFSET (10)

/**
 * \brief
 * tm profile offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_PROFILE_OFFSET (8)

/**
 * \brief
 * visibility offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_VISIBILITY_OFFSET (7)

/**
 * \brief
 * internal header type offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_INTERNAL_HEADER_TYPE_OFFSET (0)

/**
 * \brief
 * TSH enable offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TSH_EN_OFFSET (6)

/**
 * \brief
 * internal header enable offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_INTERNAL_HEADER_EN_OFFSET (5)

/**
 * \brief
 * tm destination extension present offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_DEST_EXT_PRESENT_OFFSET (4)

/**
 * \brief
 * application specific extension present offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_PRESENT_OFFSET (3)

/**
 * \brief
 * flow iD extension present offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_EXT_PRESENT_OFFSET (2)

/**
 * \brief
 * bier bfr extension present offset
 */
#define DNX_DATA_MAX_HEADERS_FTMH_BIER_BFR_EXT_PRESENT_OFFSET (1)

/**
 * \brief
 * stacking route history bitmap offset in stacking header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_STACK_ROUTE_HISTORY_BMP_OFFSET (3)

/**
 * \brief
 * TM destination offset in TM destination extension header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_DST_OFFSET (0)

/**
 * \brief
 * observation information offset in ase header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_OBSERVATION_INFO_OFFSET (42)

/**
 * \brief
 * destination offset in ase header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_DESTINATION_OFFSET (20)

/**
 * \brief
 * source system port offset in ase header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_SRC_SYSPORT_OFFSET (4)

/**
 * \brief
 * trajectory trace type offset in ase header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TRAJECTORY_TRACE_TYPE_OFFSET (0)

/**
 * \brief
 * Port field offset in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_PORT_OFFSET (32)

/**
 * \brief
 * Direction field offset in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_DIRECTION_OFFSET (31)

/**
 * \brief
 * Session_ID field offset in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_SESSION_ID_OFFSET (21)

/**
 * \brief
 * Truncated flag field offset in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TRUNCATED_OFFSET (20)

/**
 * \brief
 * The trunk encapsulation type field offset in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_EN_OFFSET (18)

/**
 * \brief
 * Class of service field offset in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_COS_OFFSET (15)

/**
 * \brief
 * Vlan field offset in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_VLAN_OFFSET (3)

/**
 * \brief
 * ERSPAN type offset in ERSPAN/sFLow header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ERSPAN_TYPE_OFFSET (0)

/**
 * \brief
 * Flow id offset in Flow-ID header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_OFFSET (4)

/**
 * \brief
 * Flow profile offset in Flow-ID header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_FLOW_PROFILE_OFFSET (0)

/**
 * \brief
 * int profile offset in Inband Network Telemetry header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_INT_PROFILE_OFFSET (40)

/**
 * \brief
 * INT type offset in Inband Network Telemetry header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_INT_TYPE_OFFSET (0)

/**
 * \brief
 * packet size bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_PACKET_SIZE_BITS (14)

/**
 * \brief
 * traffic class bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TRAFFIC_CLASS_BITS (3)

/**
 * \brief
 * source system port aggregate bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_SRC_SYS_PORT_AGGREGATE_BITS (16)

/**
 * \brief
 * PP DSP bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_PP_DSP_BITS (8)

/**
 * \brief
 * Drop Precedence bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_DROP_PRECEDENCE_BITS (2)

/**
 * \brief
 * tm action type bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_ACTION_TYPE_BITS (2)

/**
 * \brief
 * tm action is multicast bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_ACTION_IS_MC_BITS (1)

/**
 * \brief
 * multicast_id or mc_rep_idx or outlif bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_OUTLIF_BITS (22)

/**
 * \brief
 * CNI bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_CNI_BITS (1)

/**
 * \brief
 * ECN enable bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ECN_ENABLE_BITS (1)

/**
 * \brief
 * tm profile bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_PROFILE_BITS (2)

/**
 * \brief
 * visibility bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_VISIBILITY_BITS (1)

/**
 * \brief
 * TSH enable bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TSH_EN_BITS (1)

/**
 * \brief
 * internal header enable bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_INTERNAL_HEADER_EN_BITS (1)

/**
 * \brief
 * tm destination extension present bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_DEST_EXT_PRESENT_BITS (1)

/**
 * \brief
 * application specific extension present bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_PRESENT_BITS (1)

/**
 * \brief
 * flow iD extension present bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_EXT_PRESENT_BITS (1)

/**
 * \brief
 * bier bfr extension present bits in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_BIER_BFR_EXT_PRESENT_BITS (1)

/**
 * \brief
 * TM destination bits in TM destination extension header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_DST_BITS (21)

/**
 * \brief
 * observation information bits in ase header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_OBSERVATION_INFO_BITS (6)

/**
 * \brief
 * destination bits in ase header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_DESTINATION_BITS (22)

/**
 * \brief
 * source system port bits in ase header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_SRC_SYSPORT_BITS (16)

/**
 * \brief
 * trajectory trace type bits in ase header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TRAJECTORY_TRACE_TYPE_BITS (4)

/**
 * \brief
 * Port field bits in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_PORT_BITS (16)

/**
 * \brief
 * Direction field bits in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_DIRECTION_BITS (1)

/**
 * \brief
 * Session_ID field bits in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_SESSION_ID_BITS (10)

/**
 * \brief
 * Truncated flag field bits in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TRUNCATED_BITS (1)

/**
 * \brief
 * The trunk encapsulation type field bits in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_EN_BITS (2)

/**
 * \brief
 * Class of service field bits in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_COS_BITS (3)

/**
 * \brief
 * Vlan field bits in ERSPAN header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_VLAN_BITS (12)

/**
 * \brief
 * ERSPAN type bits in ERSPAN/sFlow header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ERSPAN_TYPE_BITS (3)

/**
 * \brief
 * Flow id bits in Flow-ID header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_BITS (20)

/**
 * \brief
 * Flow profile bits in Flow-ID header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_FLOW_PROFILE_BITS (4)

/**
 * \brief
 * FTMH BASE size in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_BASE_HEADER_SIZE (10)

/**
 * \brief
 * FTMH Ase header extension size in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_HEADER_SIZE (6)

/**
 * \brief
 * FTMH TM destination extension header size in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_TM_DST_SIZE (3)

/**
 * \brief
 * FTMH Flow-ID extension header size in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_FLOW_ID_HEADER_SIZE (3)

/**
 * \brief
 * int profile bits in Inband Network Telemetry header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_INT_PROFILE_BITS (8)

/**
 * \brief
 * INT type bits in Inband Network Telemetry header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_INT_TYPE_BITS (4)

/**
 * \brief
 * ASE OAM Type offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_TYPE_OFFSET (0)

/**
 * \brief
 * ASE OAM Offset offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_OFFSET_OFFSET (1)

/**
 * \brief
 * ASE OAM TS_Data(lower 32 bits) offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_TS_DATA_OFFSET (9)

/**
 * \brief
 * ASE OAM TS_Data(upper 2 bits) offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_TS_DATA_UPPER_OFFSET (41)

/**
 * \brief
 * ASE OAM MEP_Type offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_MEP_TYPE_OFFSET (43)

/**
 * \brief
 * ASE OAM Sub_Type offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_SUB_TYPE_OFFSET (44)

/**
 * \brief
 * ASE OAM Type bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_TYPE_BITS (1)

/**
 * \brief
 * ASE OAM Offset bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_OFFSET_BITS (8)

/**
 * \brief
 * ASE OAM TS_Data(lower 32 bits) bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_TS_DATA_BITS (32)

/**
 * \brief
 * ASE OAM TS_Data(upper 2 bits) bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_TS_DATA_UPPER_BITS (2)

/**
 * \brief
 * ASE OAM MEP_Type bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_MEP_TYPE_BITS (1)

/**
 * \brief
 * ASE OAM Sub_Type bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_OAM_SUB_TYPE_BITS (4)

/**
 * \brief
 * ASE Type offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_TYPE_OFFSET (0)

/**
 * \brief
 * ASE 1588 offset offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_1588_OFFSET_OFFSET (4)

/**
 * \brief
 * ASE 1588 TP_Command offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_1588_TP_COMMAND_OFFSET (12)

/**
 * \brief
 * ASE 1588 TS_Encapsulation offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_1588_TS_ENCAPSULATION_OFFSET (15)

/**
 * \brief
 * ASE Type bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_TYPE_BITS (4)

/**
 * \brief
 * ASE 1588 offset bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_1588_OFFSET_BITS (8)

/**
 * \brief
 * ASE 1588 TP_Command bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_1588_TP_COMMAND_BITS (3)

/**
 * \brief
 * ASE 1588 TS_Encapsulation bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_ASE_1588_TS_ENCAPSULATION_BITS (1)

/**
 * \brief
 * TSH header size
 */
#define DNX_DATA_MAX_HEADERS_TSH_BASE_HEADER_SIZE (4)

/**
 * \brief
 * TSH Time_Stamp offset in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_TSH_TIME_STAMP_OFFSET (0)

/**
 * \brief
 * TSH Time_Stamp bits in ASE header header
 */
#define DNX_DATA_MAX_HEADERS_TSH_TIME_STAMP_BITS (32)

/**
 * \brief
 * OTSH header size
 */
#define DNX_DATA_MAX_HEADERS_OTSH_BASE_HEADER_SIZE (6)

/**
 * \brief
 * Optimized FTMH header size
 */
#define DNX_DATA_MAX_HEADERS_OPTIMIZED_FTMH_BASE_HEADER_SIZE (4)

/**
 * \brief
 * packet size offset
 */
#define DNX_DATA_MAX_HEADERS_OPTIMIZED_FTMH_PACKET_SIZE_7_0_OFFSET (24)

/**
 * \brief
 * tm action is multicast offset
 */
#define DNX_DATA_MAX_HEADERS_OPTIMIZED_FTMH_TM_ACTION_IS_MC_OFFSET (23)

/**
 * \brief
 * packet size offset
 */
#define DNX_DATA_MAX_HEADERS_OPTIMIZED_FTMH_PACKET_SIZE_8_OFFSET (22)

/**
 * \brief
 * packet size offset
 */
#define DNX_DATA_MAX_HEADERS_OPTIMIZED_FTMH_USER_DEFINED_OFFSET (19)

/**
 * \brief
 * fap id of multicast_id[18:8] offset
 */
#define DNX_DATA_MAX_HEADERS_OPTIMIZED_FTMH_FAP_ID_OFFSET (8)

/**
 * \brief
 * pp_dsp or multicast_id[7:0] offset
 */
#define DNX_DATA_MAX_HEADERS_OPTIMIZED_FTMH_PP_DSP_OR_MC_ID_OFFSET (0)

/**
 * \brief
 * Parsing start type offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_PARSING_START_TYPE_OFFSET (1)

/**
 * \brief
 * Parsing start type bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_PARSING_START_TYPE_BITS (5)

/**
 * \brief
 * Lif Extenson type offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LIF_EXT_TYPE_OFFSET (13)

/**
 * \brief
 * Lif Extenson type bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LIF_EXT_TYPE_BITS (3)

/**
 * \brief
 * FAI offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FAI_OFFSET (86)

/**
 * \brief
 * FAI bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FAI_BITS (4)

/**
 * \brief
 * tail edit profile offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_TAIL_EDIT_OFFSET (83)

/**
 * \brief
 * tail edit profile bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_TAIL_EDIT_BITS (3)

/**
 * \brief
 * Forward domain in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FORWARD_DOMAIN_OFFSET (57)

/**
 * \brief
 * Forward domain bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FORWARD_DOMAIN_BITS (18)

/**
 * \brief
 * In-LIF offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_INLIF_OFFSET (35)

/**
 * \brief
 * In-LIF bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_INLIF_BITS (22)

/**
 * \brief
 * FHEI header size for internal header stamped
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE (5)

/**
 * \brief
 * FHEI header size of type SZ0
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ0 (3)

/**
 * \brief
 * FHEI header size of type SZ1
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ1 (5)

/**
 * \brief
 * FHEI header size of type SZ2
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_SZ2 (8)

/**
 * \brief
 * FHEI size offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_OFFSET (16)

/**
 * \brief
 * FHEI size bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_FHEI_SIZE_BITS (2)

/**
 * \brief
 * Learning Extension present offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEARNING_EXT_PRESENT_OFFSET (18)

/**
 * \brief
 * Learning Extension present bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEARNING_EXT_PRESENT_BITS (1)

/**
 * \brief
 * Learning Extension header size
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEARNING_EXT_SIZE (19)

/**
 * \brief
 * TTL offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_TTL_OFFSET (19)

/**
 * \brief
 * TTL bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_TTL_BITS (8)

/**
 * \brief
 * fwd layer index offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_ETH_RM_FLI_OFFSET (90)

/**
 * \brief
 * fwd layer index bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_ETH_RM_FLI_BITS (3)

/**
 * \brief
 * parsing start offset offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_ETH_RM_PSO_OFFSET (6)

/**
 * \brief
 * parsing start offset bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_ETH_RM_PSO_BITS (7)

/**
 * \brief
 * If set, require to enable setting of unknown address to FAI
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_CFG_FAI_UNKNOWN_ADDRESS_ENABLE (1)

/**
 * \brief
 * FHEI header size for internal header stamped
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FHEI_SIZE (5)

/**
 * \brief
 * FHEI header size of type SZ0
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FHEI_SIZE_SZ0 (3)

/**
 * \brief
 * FHEI header size of type SZ1
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FHEI_SIZE_SZ1 (5)

/**
 * \brief
 * FHEI header size of type SZ2
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FHEI_SIZE_SZ2 (8)

/**
 * \brief
 * In-LIF offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_INLIF_INRIF_OFFSET (0)

/**
 * \brief
 * In-LIF bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_INLIF_INRIF_BITS (18)

/**
 * \brief
 * Unknown address offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_UNKNOWN_ADDRESS_OFFSET (35)

/**
 * \brief
 * Unknown address bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_UNKNOWN_ADDRESS_BITS (1)

/**
 * \brief
 * Forwarding header offset in internal header, valid in jericho mode only
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FORWARDING_HEADER_OFFSET_OFFSET (41)

/**
 * \brief
 * Forwarding header offset in internal header, valid in jericho mode only
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FORWARDING_HEADER_OFFSET_BITS (7)

/**
 * \brief
 * Forward code offset in internal header, valid in jericho mode only
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FORWARD_CODE_OFFSET (48)

/**
 * \brief
 * Forward code present offset in internal header, valid in jericho mode only
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FORWARD_CODE_BITS (4)

/**
 * \brief
 * FHEI size offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FHEI_SIZE_OFFSET (52)

/**
 * \brief
 * FHEI size bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_FHEI_SIZE_BITS (2)

/**
 * \brief
 * Learning Extension present offset in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_LEARNING_EXT_PRESENT_OFFSET (54)

/**
 * \brief
 * Learning Extension present bits in internal header
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_LEARNING_EXT_PRESENT_BITS (1)

/**
 * \brief
 * Learning Extension header size
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_LEARNING_EXT_SIZE (40)

/**
 * \brief
 * EEI Extension present offset in internal header, valid in jericho mode only
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_EEI_EXT_PRESENT_OFFSET (55)

/**
 * \brief
 * EEI Extension present bits in internal header, valid in jericho mode only
 */
#define DNX_DATA_MAX_HEADERS_INTERNAL_LEGACY_EEI_EXT_PRESENT_BITS (1)

/**
 * \brief
 * FHEI Extension type offset
 */
#define DNX_DATA_MAX_HEADERS_FHEI_SZ1_TRAP_SNIFF_FHEI_TYPE_OFFSET (0)

/**
 * \brief
 * FHEI Extension type bits
 */
#define DNX_DATA_MAX_HEADERS_FHEI_SZ1_TRAP_SNIFF_FHEI_TYPE_BITS (4)

/**
 * \brief
 * FHEI Extension code offset
 */
#define DNX_DATA_MAX_HEADERS_FHEI_SZ1_TRAP_SNIFF_CODE_OFFSET (4)

/**
 * \brief
 * FHEI Extension code bits
 */
#define DNX_DATA_MAX_HEADERS_FHEI_SZ1_TRAP_SNIFF_CODE_BITS (9)

/**
 * \brief
 * FHEI Extension qualifier offset
 */
#define DNX_DATA_MAX_HEADERS_FHEI_SZ1_TRAP_SNIFF_QUALIFIER_OFFSET (13)

/**
 * \brief
 * FHEI Extension qualifier bits
 */
#define DNX_DATA_MAX_HEADERS_FHEI_SZ1_TRAP_SNIFF_QUALIFIER_BITS (27)

/**
 * \brief
 * FHEI Extension code offset
 */
#define DNX_DATA_MAX_HEADERS_FHEI_TRAP_SNIFF_LEGACY_CODE_OFFSET (0)

/**
 * \brief
 * FHEI Extension code bits
 */
#define DNX_DATA_MAX_HEADERS_FHEI_TRAP_SNIFF_LEGACY_CODE_BITS (8)

/**
 * \brief
 * FHEI Extension qualifier offset
 */
#define DNX_DATA_MAX_HEADERS_FHEI_TRAP_SNIFF_LEGACY_QUALIFIER_OFFSET (8)

/**
 * \brief
 * FHEI Extension qualifier bits
 */
#define DNX_DATA_MAX_HEADERS_FHEI_TRAP_SNIFF_LEGACY_QUALIFIER_BITS (16)

/**
 * \brief
 * UDH data type0 offset in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_TYPE0_OFFSET (6)

/**
 * \brief
 * UDH data type1 offset in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_TYPE1_OFFSET (4)

/**
 * \brief
 * UDH data type2 offset in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_TYPE2_OFFSET (2)

/**
 * \brief
 * UDH data type3 offset in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_TYPE3_OFFSET (0)

/**
 * \brief
 * UDH data offset in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_OFFSET (0)

/**
 * \brief
 * UDH data type0 bits in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_TYPE0_BITS (2)

/**
 * \brief
 * UDH data type1 bits in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_TYPE1_BITS (2)

/**
 * \brief
 * UDH data type2 bits in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_TYPE2_BITS (2)

/**
 * \brief
 * UDH data type3 bits in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_TYPE3_BITS (2)

/**
 * \brief
 * UDH data bits in UDH header
 */
#define DNX_DATA_MAX_HEADERS_UDH_DATA_BITS (16)

/**
 * \brief
 * UDH base size
 */
#define DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE (1)

/**
 * \brief
 * packet size offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_PACKET_SIZE_OFFSET (58)

/**
 * \brief
 * traffic class offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_TRAFFIC_CLASS_OFFSET (55)

/**
 * \brief
 * source system port aggregate offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_SRC_SYS_PORT_AGGREGATE_OFFSET (39)

/**
 * \brief
 * PP DSP offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_PP_DSP_OFFSET (31)

/**
 * \brief
 * Drop Precedence offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DROP_PRECEDENCE_OFFSET (29)

/**
 * \brief
 * tm action type offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_TM_ACTION_TYPE_OFFSET (27)

/**
 * \brief
 * OTSH enable offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_OTSH_EN_OFFSET (26)

/**
 * \brief
 * internal header enable offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_INTERNAL_HEADER_EN_OFFSET (25)

/**
 * \brief
 * outbound-mirr-disable parameter offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_OUTBOUND_MIRR_DISABLE_OFFSET (24)

/**
 * \brief
 * tm action is multicast offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_TM_ACTION_IS_MC_OFFSET (23)

/**
 * \brief
 * multicast_id or outlif offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_OUTLIF_OFFSET (4)

/**
 * \brief
 * tm destination extension present offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_PRESENT_OFFSET (3)

/**
 * \brief
 * CNI offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_CNI_OFFSET (2)

/**
 * \brief
 * ECN enable offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_ECN_ENABLE_OFFSET (1)

/**
 * \brief
 * exclude soutrce offset in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_EXCLUDE_SOURCE_OFFSET (0)

/**
 * \brief
 * stacking route history bitmap offset for stacking header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_STACK_ROUTE_HISTORY_BMP_OFFSET (0)

/**
 * \brief
 * DSP offset for TM destination extension header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_HEADER_OFFSET (0)

/**
 * \brief
 * packet size bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_PACKET_SIZE_BITS (14)

/**
 * \brief
 * traffic class bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_TRAFFIC_CLASS_BITS (3)

/**
 * \brief
 * source system port aggregate bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_SRC_SYS_PORT_AGGREGATE_BITS (16)

/**
 * \brief
 * PP DSP bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_PP_DSP_BITS (8)

/**
 * \brief
 * Drop Precedence bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DROP_PRECEDENCE_BITS (2)

/**
 * \brief
 * tm action type bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_TM_ACTION_TYPE_BITS (2)

/**
 * \brief
 * OTSH enable bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_OTSH_EN_BITS (1)

/**
 * \brief
 * internal header enable bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_INTERNAL_HEADER_EN_BITS (1)

/**
 * \brief
 * outbound-mirr-disable parameter bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_OUTBOUND_MIRR_DISABLE_BITS (1)

/**
 * \brief
 * tm action is multicast bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_TM_ACTION_IS_MC_BITS (1)

/**
 * \brief
 * multicast_id or outlif bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_OUTLIF_BITS (19)

/**
 * \brief
 * tm destination extension present bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_PRESENT_BITS (1)

/**
 * \brief
 * CNI bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_CNI_BITS (1)

/**
 * \brief
 * ECN enable bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_ECN_ENABLE_BITS (1)

/**
 * \brief
 * exclude source bits in legacy FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_EXCLUDE_SOURCE_BITS (1)

/**
 * \brief
 * DSP bits for TM destination extension header
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_HEADER_BITS (16)

/**
 * \brief
 * Legacy FTMH BASE size
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_BASE_HEADER_SIZE (9)

/**
 * \brief
 * FTMH DSP extension header size in FTMH
 */
#define DNX_DATA_MAX_HEADERS_FTMH_LEGACY_DSP_EXT_HEADER_SIZE (2)

/**
 * \brief
 * type offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_TYPE_OFFSET (46)

/**
 * \brief
 * type bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_TYPE_BITS (2)

/**
 * \brief
 * oam-sub-type offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_OAM_SUB_TYPE_OFFSET (43)

/**
 * \brief
 * oam-sub-type bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_OAM_SUB_TYPE_BITS (3)

/**
 * \brief
 * mep-type offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_MEP_TYPE_OFFSET (42)

/**
 * \brief
 * mep-type bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_MEP_TYPE_BITS (1)

/**
 * \brief
 * tp-command offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_TP_COMMAND_OFFSET (43)

/**
 * \brief
 * tp-command  bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_TP_COMMAND_BITS (3)

/**
 * \brief
 * ts-encapsulation offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_TS_ENCAPSULATION_OFFSET (42)

/**
 * \brief
 * ts-encapsulation bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_TS_ENCAPSULATION_BITS (1)

/**
 * \brief
 * oam-ts-data higher 2 bits offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_OAM_TS_DATA_0_OFFSET (40)

/**
 * \brief
 * oam-ts-data higher 2 bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_OAM_TS_DATA_0_BITS (2)

/**
 * \brief
 * oam-ts-data lower 32 bits offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_OAM_TS_DATA_1_OFFSET (8)

/**
 * \brief
 * oam-ts-data lower 32 bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_OAM_TS_DATA_1_BITS (32)

/**
 * \brief
 * offset offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_OFFSET_OFFSET (0)

/**
 * \brief
 * offset bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_OFFSET_BITS (8)

/**
 * \brief
 * latency-flow-id offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_LATENCY_FLOW_ID_OFFSET (0)

/**
 * \brief
 * latency-flow-id bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_LATENCY_FLOW_ID_BITS (19)

/**
 * \brief
 * latency-time-stamp offset in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_LATENCY_TIME_STAMP_OFFSET (19)

/**
 * \brief
 * latency-time-stamp bits in legacy OTSH
 */
#define DNX_DATA_MAX_HEADERS_OTSH_LEGACY_LATENCY_TIME_STAMP_BITS (27)

/**
 * \brief
 * System headers mode Jericho
 */
#define DNX_DATA_MAX_HEADERS_SYSTEM_HEADERS_SYSTEM_HEADERS_MODE_JERICHO (0)

/**
 * \brief
 * Sysstem headers mode Jericho2
 */
#define DNX_DATA_MAX_HEADERS_SYSTEM_HEADERS_SYSTEM_HEADERS_MODE_JERICHO2 (1)

/**
 * \brief
 * CRC size counted packet length
 */
#define DNX_DATA_MAX_HEADERS_SYSTEM_HEADERS_CRC_SIZE_COUNTED_IN_PKT_LEN (2)

/**
 * \brief
 * Size of VSI selection field
 */
#define DNX_DATA_MAX_HEADERS_PPH_PPH_VSI_SELECTION_SIZE (3)

/**
 * \brief
 * Size of remark_profile (and ttl model,encap qos model in q2a and above) mapped from mpls swap command
 */
#define DNX_DATA_MAX_HEADERS_FEATURE_FHEI_MPLS_SWAP_REMARK_SIZE (8)

/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_MAX_HEADERS_H_*/
/* *INDENT-ON* */
