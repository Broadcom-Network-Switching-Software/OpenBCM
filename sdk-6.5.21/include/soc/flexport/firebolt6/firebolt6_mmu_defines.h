/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        firebolt6_mmu_defines.h
 * Purpose:
 * Requires:
 */

#ifndef FIREBOLT6_MMU_DEFINES_H
#define FIREBOLT6_MMU_DEFINES_H

/*! @file FIREBOLT6_mmu_defines.h
 *  @brief Macros used in APIs of MMU for Tomahawk2.
 *  Details are shown below.
 */
#define FB6_MMU_NUM_DL_PORTS 		    48
#define FB6_MMU_NUM_UP_PORTS 		    8
#define FB6_MMU_NUM_SL_PORTS 		    8
#define FB6_MMU_NUM_NORMAL_PORTS	    70
#define FB6_MMU_MAX_L0_DW_PORT              1
#define FB6_MMU_MAX_L0_UP_PORT              4
#define FB6_MMU_MAX_L0_ST_PORT              3
#define FB6_MMU_MAX_L0_CH_PORT              48
#define FB6_MMU_NUM_PHY_PORTS 		    68

#define FB6_MMU_NUM_L0_NODES_PER_HSP_DL_PORT 1
#define FB6_MMU_NUM_L0_NODES_PER_HSP_UP_PORT 4
#define FB6_MMU_NUM_L0_NODES_PER_HSP_SL_PORT 3
#define FB6_MMU_NUM_L0_NODES_CPU_PORT 1
#define FB6_MMU_NUM_L0_NODES_LPBK_PORT 1
#define FB6_MMU_NUM_L1_NODES_PER_HSP_DL_PORT 8
#define FB6_MMU_NUM_L1_NODES_PER_HSP_UP_PORT 8
#define FB6_MMU_NUM_L1_NODES_PER_HSP_SL_PORT 8
#define FB6_MMU_NUM_L1_NODES_CPU_PORT 10
#define FB6_MMU_NUM_L1_NODES_LPBK_PORT 10
#define FB6_MMU_NUM_L2_NODES_PER_HSP_DL_PORT 8
#define FB6_MMU_NUM_L2_NODES_PER_HSP_UP_PORT 32
#define FB6_MMU_NUM_L2_NODES_PER_HSP_SL_PORT 24
#define FB6_MMU_NUM_L2_NODES_CPU_PORT 10
#define FB6_MMU_NUM_L2_NODES_LPBK_PORT 10
/* #define FB6_MMU_NUM_L3_NODES_CPU_PORT 48 */
/* #define FB6_MMU_NUM_L3_NODES_LPBK_PORT 10 */
/* #define FB6_MMU_NUM_L3_NODES_PER_HSP_DL_PORT 32 */
/* #define FB6_MMU_NUM_L3_NODES_PER_HSP_UP_PORT 52 */
#define FB6_MMU_NUM_UCQ_PER_HSP_DL_PORT 8
#define FB6_MMU_NUM_MCQ_PER_HSP_DL_PORT 8
#define FB6_MMU_NUM_UCQ_PER_HSP_UP_PORT 32
#define FB6_MMU_NUM_MCQ_PER_HSP_UP_PORT 32
#define FB6_MMU_NUM_UCQ_PER_HSP_SL_PORT 24
#define FB6_MMU_NUM_MCQ_PER_HSP_SL_PORT 24
/* #define FB6_MMU_NUM_L0_NODES_PER_HSP_PORT 10     */
/* #define FB6_MMU_NUM_L0_NODES_FOR_CPU_PORT 48 */
/* #define FB6_MMU_NUM_UCQ_PER_HSP_PORT 10 */
/* #define FB6_MMU_NUM_MCQ_PER_HSP_PORT 10 */
#define FB6_MMU_NUM_MCQ_FOR_CPU_PORT 48
#define FB6_MMU_CPU_PORT_NUM 70
/* #define FB6_MMU_MGMT_PORT_NUM 64 */
#define FB6_MMU_MAX_PORTS_PER_PIPE 72
#define FB6_MMU_NUM_HSP_PORTS_PER_PIPE 72
#define FB6_MMU_LPBK_PORT_NUM 71
#define FB6_INVALID_MMU_PORT 255
#define FB6_INVALID_PHY_PORT_NUMBER 255
#define FB6_INVALID_DEV_PORT_NUMBER 255
#define FB6_MMU_PORT_PIPE_OFFSET 72
#define FB6_MMU_MCQ_NUMBER_START 3680
#define FB6_MMU_NUM_UCQ 3680
#define FB6_MMU_NUM_MC_QUEUES 3738
#define FB6_MMU_NUM_MCQ_FP_PORTS 3737
#define FB6_MMU_LPBK_PORT_L0_START   3680
#define FB6_MMU_LPBK_PORT_L1_START   3680 
/* #define FB6_MMU_LPBK_PORT_L2_START   FB6_MMU_NUM_DL_PORTS*FB6_MMU_NUM_L2_NODES_PER_HSP_DL_PORT + \
*                                      FB6_MMU_NUM_UP_PORTS*FB6_MMU_NUM_L2_NODES_PER_HSP_UP_PORT
* #define FB6_MMU_LPBK_PORT_L3_START   FB6_MMU_MCQ_NUMBER_START+ \
*                                     FB6_MMU_NUM_MCQ_FP_PORTS
*/
#define FB6_MMU_CPU_PORT_L0_START  3690 
#define FB6_MMU_CPU_PORT_L1_START  3690
/* #define FB6_MMU_CPU_PORT_L2_START  FB6_MMU_LPBK_PORT_L2_START+ \
*                                    FB6_MMU_NUM_L2_NODES_LPBK_PORT
* #define FB6_MMU_CPU_PORT_L3_START  FB6_MMU_LPBK_PORT_L3_START+ \
*                                    FB6_MMU_NUM_L3_NODES_LPBK_PORT
*/
/*#define FB6_MMU_MGMT_CPU_PORT_L3_START FB6_MMU_LPBK_PORT_L3_START+ \
 *  FB6_MMU_NUM_MCQ_PER_HSP_PORT
 *      #define FB6_MMU_LPBK_PORT_L1_START FB6_MMU_MCQ_NUMBER_START+ \
 *  FB6_MMU_NUM_MCQ_FP_PORTS
 *      #define FB6_MMU_MGMT_CPU_PORT_L1_START FB6_MMU_LPBK_PORT_L1_START+ \
 *  FB6_MMU_NUM_MCQ_PER_HSP_PORT
 *      #define FB6_MMU_PIPE1_QUEUE_START FB6_MMU_NUM_UC_QUEUES+FB6_MMU_NUM_MC_QUEUES
*/

#define FB6_MMU_BST_TRACK_MODE 0
#define FB6_MMU_BST_WMARK_MODE 1
#define FB6_MMU_MEM_ADDR__MEMIDX_WIDTH 16
#define FB6_MMU_NUM_PGS 8
#define FB6_MMU_NUM_SPS 4
/* #define FB6_MMU_NUM_COS 10 */
/* #define FB6_MMU_NUM_COS 10 */
#define FB6_MMU_NUM_UC_QUEUES 1280
/* define FB6_MMU_ACC_TYPE__ADDR_SPLIT_DIST 10
 * #define FB6_MMU_ACC_TYPE__ADDR_SPLIT_SPLIT 12
 */
#define FB6_MMU_ACC_TYPE__DATA_SPLIT 14
#define FB6_MMU_ACC_TYPE__DUPLICATE 9
#define FB6_MMU_ACC_TYPE__SINGLE 20
#define FB6_MMU_ACC_TYPE__UNIQUE 31
#define FB6_MMU_ACC_TYPE__UNIQUE_PIPE0 0
#define FB6_MMU_ACC_TYPE__UNIQUE_PIPE1 1
#define FB6_MMU_ACC_TYPE__UNIQUE_PIPE2 2
#define FB6_MMU_ACC_TYPE__UNIQUE_PIPE3 3
#define FB6_MMU_BASE_TYPE__XPE 5
#define FB6_MMU_BASE_TYPE__CHIP 4
#define FB6_MMU_BASE_TYPE__SLICE 6
#define FB6_MMU_BASE_TYPE__EPORT 1
#define FB6_MMU_BASE_TYPE__IPIPE 2
#define FB6_MMU_BASE_TYPE__EPIPE 3
#define FB6_MMU_BASE_TYPE__IPORT 0
#define FB6_MMU_BASE_TYPE__LAYER 7
/* #define FB6_MMU_THDU_UCQ_PER_PORT 10 */
#define FB6_MMU_THDU_UCQ_PER_UP_PORT 32
#define FB6_MMU_THDU_UCQ_PER_DL_PORT 8
#define FB6_MMU_THDI_PG_MIN_LIMIT_LOSSLESS 7
#define FB6_MMU_THDI_PG_MIN_LIMIT_LOSSY 0
#define FB6_MMU_THDI_PG_HDRM_LIMIT_LOSSY 0
#define FB6_MMU_THDI_SP_MAX_LIMIT 131072
#define FB6_MMU_THDI_SP_RESUME_LIMIT 131058
#define FB6_MMU_NUM_SPEEDS 9
#define FB6_MMU_NUM_PORT_COLOR_DROP_ENTRIES 5

#define FB6_MMU_THDI_PORT_XON_ENABLE_VAL 65535
#define FB6_MMU_THDU_QGRP_MIN_LIMIT_LOSELESS 14
#define FB6_MMU_THDU_QGRP_MIN_LIMIT_LOSSY FB6_MMU_THDU_QGRP_MIN_LIMIT_LOSELESS
#define FB6_MMU_THDU_QUEUE_SHARED_ALPHA_LOSSY 8
#define FB6_MMU_THDU_QUEUE_SHARED_ALPHA_LOSSLESS 130995
#define FB6_MMU_THDU_QUEUE_SHARED_LIMIT_LOSSY 8
#define FB6_MMU_THDU_QUEUE_SHARED_LIMIT_LOSSLESS 130995
#define FB6_MMU_THDU_QUEUE_LIMIT_DYNAMIC_LOSSY 1
#define FB6_MMU_THDU_QUEUE_LIMIT_DYNAMIC_LOSSLESS 0
#define FB6_MMU_THDU_QUEUE_COLOR_LIMIT_DYNAMIC_LOSSY 1
#define FB6_MMU_THDU_QUEUE_COLOR_LIMIT_DYNAMIC_LOSSLESS 0
#define FB6_MMU_THDU_QUEUE_LIMIT_ENABLE_LOSSY 1
#define FB6_MMU_THDU_QUEUE_LIMIT_ENABLE_LOSSLESS 0
#define FB6_MMU_NUM_CT_SPEED_GROUPS 12
#define FB6_MAX_NUM_CELLS 37 /* (9416-192)/256 + 1 SOP Cell */
#define FB6_CT_FIFO_WAIT 15 /*Refer to port_down seq. in TH2 flexport arch */

typedef enum pipe_s
{
    FB6_IPIPE,
    FB6_EPIPE
} pipe_t;

#define FB6_MMU_MEM_ADDR__MEMIDX_WIDTH 16
#define FB6_MMU_FLUSH_OFF 0
#define FB6_MMU_FLUSH_ON  1
#define FB6_PKTSTATS_ON   1
#define FB6_MMU_THD_RX_ENABLED 1
#define FB6_MMU_THD_RX_DISABLED 0

#define FB6_MMU_IS_CPU_PORT(pipe, port) ((pipe == 0) && \
                                         (port == FB6_MMU_CPU_PORT_NUM))
/*#define FB6_MMU_IS_MGMT_PORT(pipe, \
 *                           port) ((pipe == 1) && \
 *                                  (port == FB6_MMU_MGMT_PORT_NUM))
*/
#define FB6_MMU_IS_LPBK_PORT(port) (port == FB6_MMU_LPBK_PORT_NUM)
#define FB6_MMU_GET_PIPE_NUM(port, pipe) \
    pipe = port/FB6_MMU_PORT_PIPE_OFFSET
#define FB6_MMU_PIPE_NUM(port) port/FB6_MMU_PORT_PIPE_OFFSET

#define FB6_MMU_HANDLE_ENUMS_EPIPE_DUPLICATE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _PIPE0m, \
      -1}, \
     {mem_name ## _PIPE1m, \
      -1}, {mem_name ## _PIPE2m, -1}, {mem_name ## _PIPE3m, -1}}

#define FB6_MMU_HANDLE_ENUMS_IPIPE_DUPLICATE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _PIPE0m, \
      -1}, \
     {mem_name ## _PIPE1m, \
      -1}, {mem_name ## _PIPE2m, -1}, {mem_name ## _PIPE3m, -1}}

#define FB6_MMU_HANDLE_ENUMS_EPIPE_UNIQUE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _XPE0_PIPE0m, \
      mem_name ## _XPE2_PIPE0m}, \
     {mem_name ## _XPE0_PIPE1m, \
      mem_name ## _XPE2_PIPE1m}, \
     {mem_name ## _XPE1_PIPE2m, \
      mem_name ## _XPE3_PIPE2m}, \
     {mem_name ## _XPE1_PIPE3m, mem_name ## _XPE3_PIPE3m}}; \

#define FB6_MMU_HANDLE_ENUMS_IPIPE_UNIQUE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _XPE0_PIPE0m, \
      mem_name ## _XPE1_PIPE0m}, \
     {mem_name ## _XPE2_PIPE1m, \
      mem_name ## _XPE3_PIPE1m}, \
     {mem_name ## _XPE2_PIPE2m, \
      mem_name ## _XPE3_PIPE2m}, \
     {mem_name ## _XPE0_PIPE3m, mem_name ## _XPE1_PIPE3m}}; \

#define FB6_MMU_GET_HANDLE(mem_name, base_index, acc_index, handle) \
    handle = mem_name ## _enums[base_index][acc_index]

/* START - FlexPort types as defined in SDK. */
typedef enum {
    FB6_SPEED_10G_ETH_DECODE  = 1,
    FB6_SPEED_10G_HG_DECODE   = 2,
    FB6_SPEED_20G_ETH_DECODE  = 3,
    FB6_SPEED_20G_HG_DECODE   = 4,
    FB6_SPEED_25G_ETH_DECODE  = 5,
    FB6_SPEED_25G_HG_DECODE   = 6,
    FB6_SPEED_40G_ETH_DECODE  = 7,
    FB6_SPEED_40G_HG_DECODE   = 8,
    FB6_SPEED_50G_ETH_DECODE  = 9,
    FB6_SPEED_50G_HG_DECODE   = 10,
    FB6_SPEED_100G_ETH_DECODE = 11,
    FB6_SPEED_100G_HG_DECODE  = 12
} _soc_td3_ct_speed_decode_e;

#endif /* FIREBOLT6_MMU_DEFINES_H */
