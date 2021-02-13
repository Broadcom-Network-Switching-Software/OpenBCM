/** \file dnx_data_max_stif.h
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
#ifndef _DNX_DATA_MAX_STIF_H_
/*{*/
#define _DNX_DATA_MAX_STIF_H_
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
 * scrubber sram buffers threshold resolution
 */
#define DNX_DATA_MAX_STIF_CONFIG_SRAM_BUFFERS_RESOLUTION (16)

/**
 * \brief
 * scrubber sram pdbs threshold resolution
 */
#define DNX_DATA_MAX_STIF_CONFIG_SRAM_PDBS_RESOLUTION (16)

/**
 * \brief
 * scrubber dram bdb threshold resolution
 */
#define DNX_DATA_MAX_STIF_CONFIG_DRAM_BDB_RESOLUTION (16)

/**
 * \brief
 * stif-nif port instances
 */
#define DNX_DATA_MAX_STIF_PORT_MAX_NOF_INSTANCES (4)

/**
 * \brief
 * used for dbal - size of stif ethu select in bits
 */
#define DNX_DATA_MAX_STIF_PORT_STIF_ETHU_SELECT_SIZE (6)

/**
 * \brief
 * used for dbal - size of stif to lane bitmap in bits
 */
#define DNX_DATA_MAX_STIF_PORT_STIF_LANE_MAPPING_SIZE (4)

/**
 * \brief
 * default value for qsize idle report size in bytes
 */
#define DNX_DATA_MAX_STIF_REPORT_QSIZE_IDLE_REPORT_SIZE (8)

/**
 * \brief
 * default value for the QSIZE report period
 */
#define DNX_DATA_MAX_STIF_REPORT_QSIZE_IDLE_REPORT_PERIOD (256)

/**
 * \brief
 * disable the idle report generation
 */
#define DNX_DATA_MAX_STIF_REPORT_QSIZE_IDLE_REPORT_PERIOD_INVALID (127)

/**
 * \brief
 * Period between consecutive scrubber records is in units - unit = defined cycle periods
 */
#define DNX_DATA_MAX_STIF_REPORT_QSIZE_SCRUBBER_REPORT_CYCLES_UNIT (16)

/**
 * \brief
 * scrubber buffer thresholds - the thresholds define 16 ranges of buffer occupancy
 */
#define DNX_DATA_MAX_STIF_REPORT_MAX_THRESHOLDS_PER_BUFFER (15)

/**
 * \brief
 * STIF uses incoming TC (tc reported by PP) and cannot be changed by the user - hw value
 */
#define DNX_DATA_MAX_STIF_REPORT_INCOMING_TC (2)

/**
 * \brief
 * two MSB of the record should be 2'b11
 */
#define DNX_DATA_MAX_STIF_REPORT_RECORD_TWO_MSB (3)

/**
 * \brief
 * number of source types - ingress enq/deq/scrubber, egress - regarding bcm_stat_stif_source_type_t struct
 */
#define DNX_DATA_MAX_STIF_REPORT_NOF_SOURCE_TYPES (4)

/**
 * \brief
 * max count of possible statistics ids
 */
#define DNX_DATA_MAX_STIF_REPORT_STAT_ID_MAX_COUNT (4)

/**
 * \brief
 * the size of a statistic object (0,1,2,3) - size in bits
 */
#define DNX_DATA_MAX_STIF_REPORT_STAT_OBJECT_SIZE (20)

/**
 * \brief
 * get size of the  billing element for packet size - size in bits
 */
#define DNX_DATA_MAX_STIF_REPORT_BILLING_FORMAT_ELEMENT_PACKET_SIZE (14)

/**
 * \brief
 * get size of the  billing element opcode which the user cannot change - size in bits
 */
#define DNX_DATA_MAX_STIF_REPORT_BILLING_FORMAT_MSB_OPCODE_SIZE (2)

/**
 * \brief
 * billing record format ETPP metadata size - size in bits
 */
#define DNX_DATA_MAX_STIF_REPORT_BILLING_FORMAT_ETPP_METADATA_SIZE (24)

/**
 * \brief
 * billing record format opsize mask to use one type
 */
#define DNX_DATA_MAX_STIF_REPORT_BILLING_OPSIZE_USE_ONE_TYPE_MASK (3)

/**
 * \brief
 * billing record format mask to use two types
 */
#define DNX_DATA_MAX_STIF_REPORT_BILLING_OPSIZE_USE_TWO_TYPES_MASK (15)

/**
 * \brief
 * billing record format mask to use three types
 */
#define DNX_DATA_MAX_STIF_REPORT_BILLING_OPSIZE_USE_THREE_TYPES_MASK (63)

/**
 * \brief
 * billing record format mask to use four types
 */
#define DNX_DATA_MAX_STIF_REPORT_BILLING_OPSIZE_USE_FOUR_TYPES_MASK (255)

/**
 * \brief
 * billing record ingress enqueu size=96. need to reduce shift value from each element sgift value
 */
#define DNX_DATA_MAX_STIF_REPORT_BILLING_ENQ_ING_SIZE_SHIFT (16)

/**
 * \brief
 * pipe length size in clocks
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_ETPP_PIPE_LENGTH (770)

/**
 * \brief
 * etpp dequeue size of fifo - reports
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_ETPP_DEQ_SIZE_OF_FIFO (1024)

/**
 * \brief
 * cgm pipe length in clocks
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_CGM_PIPE_LENGTH (64)

/**
 * \brief
 * cgm size of fifo - reports
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_CGM_SIZE_OF_FIFO (208)

/**
 * \brief
 * level sampling value - used for billing flow control calculation
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_BILLING_LEVEL_SAMPLING (60)

/**
 * \brief
 * disable flow control value
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_FC_DISABLE_VALUE (0)

/**
 * \brief
 * billing etpp flow control max value - calculated with formula given from the architecture
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_BILLING_ETPP_FLOW_CONTROL_HIGH (91)

/**
 * \brief
 * billing etpp flow control min value - calculated with formula given from the architecture
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_BILLING_ETPP_FLOW_CONTROL_LOW (90)

/**
 * \brief
 * billing cgm flow control max value - calculated with formula given from the architecture
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_BILLING_CGM_FLOW_CONTROL_HIGH (63)

/**
 * \brief
 * billing cgm flow control min value - calculated with formula given from the architecture
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_BILLING_CGM_FLOW_CONTROL_LOW (62)

/**
 * \brief
 * qsize cgm flow control max value - calculated with formula given from the architecture
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_QSIZE_CGM_FLOW_CONTROL_HIGH (123)

/**
 * \brief
 * qsize cgm flow control min value - calculated with formula given from the architecture
 */
#define DNX_DATA_MAX_STIF_FLOW_CONTROL_QSIZE_CGM_FLOW_CONTROL_LOW (122)

/*
 * }
 */

/*}*/
#endif /*_DNX_DATA_MAX_STIF_H_*/
/* *INDENT-ON* */
