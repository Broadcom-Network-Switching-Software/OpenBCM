/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  trident3_mmu_defines.h
*/


#ifndef TRIDENT3_MMU_DEFINES_H
#define TRIDENT3_MMU_DEFINES_H

/*! @file trident3_mmu_defines.h
 *  @brief Macros used in APIs of MMU for Tomahawk2.
 *  Details are shown below.
 */

#define TD3_MMU_NUM_L0_NODES_PER_HSP_PORT 10                                        
#define TD3_MMU_NUM_L0_NODES_FOR_CPU_PORT 48
#define TD3_MMU_NUM_UCQ_PER_HSP_PORT 10
#define TD3_MMU_NUM_MCQ_PER_HSP_PORT 10
#define TD3_MMU_NUM_MCQ_FOR_CPU_PORT 48
#define TD3_MMU_CPU_PORT_NUM 64
#define TD3_MMU_MGMT_PORT_NUM 64
#define TD3_MMU_MAX_PORTS_PER_PIPE 66
#define TD3_MMU_NUM_HSP_PORTS_PER_PIPE 66
#define TD3_MMU_LPBK_PORT_NUM 65
#define TD3_INVALID_MMU_PORT 255
#define TD3_INVALID_PHY_PORT_NUMBER 255
#define TD3_INVALID_DEV_PORT_NUMBER 255
#define TD3_MMU_PORT_PIPE_OFFSET 128
#define TD3_MMU_MCQ_NUMBER_START 660
#define TD3_MMU_NUM_UCQ 650
#define TD3_MMU_NUM_MC_QUEUES 698
#define TD3_MMU_NUM_MCQ_FP_PORTS 640
#define TD3_MMU_LPBK_PORT_L1_START TD3_MMU_MCQ_NUMBER_START+ \
    TD3_MMU_NUM_MCQ_FP_PORTS
#define TD3_MMU_MGMT_CPU_PORT_L1_START TD3_MMU_LPBK_PORT_L1_START+ \
    TD3_MMU_NUM_MCQ_PER_HSP_PORT
#define TD3_MMU_PIPE1_QUEUE_START TD3_MMU_NUM_UC_QUEUES+TD3_MMU_NUM_MC_QUEUES


#define TD3_MMU_BST_TRACK_MODE 0
#define TD3_MMU_BST_WMARK_MODE 1
#define TD3_MMU_MEM_ADDR__MEMIDX_WIDTH 16
#define TD3_MMU_NUM_PGS 8
#define TD3_MMU_NUM_SPS 4
#define TD3_MMU_NUM_COS 10
#define TD3_MMU_NUM_UC_QUEUES 650
#define TD3_MMU_NUM_PORTS_PER_PIPE 66
#define TD3_MMU_ACC_TYPE__ADDR_SPLIT_DIST 10
#define TD3_MMU_ACC_TYPE__ADDR_SPLIT_SPLIT 12
#define TD3_MMU_ACC_TYPE__DATA_SPLIT 14
#define TD3_MMU_ACC_TYPE__DUPLICATE 9
#define TD3_MMU_ACC_TYPE__SINGLE 20
#define TD3_MMU_ACC_TYPE__UNIQUE 31
#define TD3_MMU_ACC_TYPE__UNIQUE_PIPE0 0
#define TD3_MMU_ACC_TYPE__UNIQUE_PIPE1 1
#define TD3_MMU_ACC_TYPE__UNIQUE_PIPE2 2
#define TD3_MMU_ACC_TYPE__UNIQUE_PIPE3 3
#define TD3_MMU_BASE_TYPE__XPE 5
#define TD3_MMU_BASE_TYPE__CHIP 4
#define TD3_MMU_BASE_TYPE__SLICE 6
#define TD3_MMU_BASE_TYPE__EPORT 1
#define TD3_MMU_BASE_TYPE__IPIPE 2
#define TD3_MMU_BASE_TYPE__EPIPE 3
#define TD3_MMU_BASE_TYPE__IPORT 0
#define TD3_MMU_BASE_TYPE__LAYER 7
#define TD3_MMU_THDU_UCQ_PER_PORT 10
#define TD3_MMU_THDI_PG_MIN_LIMIT_LOSSLESS 8
#define TD3_MMU_THDI_PG_MIN_LIMIT_LOSSY 0
#define TD3_MMU_THDI_PG_HDRM_LIMIT_LOSSY 0
#define TD3_MMU_THDI_SP_MAX_LIMIT 53248
#define TD3_MMU_THDI_SP_RESUME_LIMIT 53232
#define TD3_MMU_NUM_SPEEDS 7
#define TD3_MMU_NUM_PORT_COLOR_DROP_ENTRIES 5

#define TD3_MMU_THDI_PORT_XON_ENABLE_VAL 65535
#define TD3_MMU_THDU_QGRP_MIN_LIMIT_LOSELESS 16
#define TD3_MMU_THDU_QGRP_MIN_LIMIT_LOSSY TD3_MMU_THDU_QGRP_MIN_LIMIT_LOSELESS
#define TD3_MMU_THDU_QUEUE_SHARED_ALPHA_LOSSY 8
#define TD3_MMU_THDU_QUEUE_SHARED_ALPHA_LOSSLESS 52296
#define TD3_MMU_THDU_QUEUE_SHARED_LIMIT_LOSSY 8
#define TD3_MMU_THDU_QUEUE_SHARED_LIMIT_LOSSLESS 52296
#define TD3_MMU_THDU_QUEUE_LIMIT_DYNAMIC_LOSSY 1
#define TD3_MMU_THDU_QUEUE_LIMIT_DYNAMIC_LOSSLESS 0
#define TD3_MMU_THDU_QUEUE_COLOR_LIMIT_DYNAMIC_LOSSY 1
#define TD3_MMU_THDU_QUEUE_COLOR_LIMIT_DYNAMIC_LOSSLESS 0
#define TD3_MMU_THDU_QUEUE_LIMIT_ENABLE_LOSSY 1
#define TD3_MMU_THDU_QUEUE_LIMIT_ENABLE_LOSSLESS 0
#define TD3_MMU_NUM_CT_SPEED_GROUPS 12
#define TD3_MAX_NUM_CELLS 37 /* (9416-192)/256 + 1 SOP Cell */
#define TD3_CT_FIFO_WAIT 15 /*Refer to port_down seq. in TH2 flexport arch */
#define TD3_MMU_PAUSE_PG 7  /*PG7-Used for Initial PG_Min/hdrm + for Pause */

typedef enum pipe_s
{
    TD3_IPIPE,
    TD3_EPIPE
} pipe_t;

#define TD3_MMU_MEM_ADDR__MEMIDX_WIDTH 16
#define TD3_MMU_FLUSH_OFF 0
#define TD3_MMU_FLUSH_ON  1
#define TD3_PKTSTATS_ON   1
#define TD3_MMU_THD_RX_ENABLED 1
#define TD3_MMU_THD_RX_DISABLED 0

#define TD3_MMU_IS_CPU_PORT(pipe, port) ((pipe == 0) && \
                                         (port == TD3_MMU_CPU_PORT_NUM))
#define TD3_MMU_IS_MGMT_PORT(pipe, \
                             port) ((pipe == 1) && \
                                    (port == TD3_MMU_MGMT_PORT_NUM))
#define TD3_MMU_IS_LPBK_PORT(port) (port == TD3_MMU_LPBK_PORT_NUM)
#define TD3_MMU_GET_PIPE_NUM(port, pipe) \
    pipe = port/TD3_MMU_PORT_PIPE_OFFSET
#define TD3_MMU_PIPE_NUM(port) port/TD3_MMU_PORT_PIPE_OFFSET

#define TD3_MMU_HANDLE_ENUMS_EPIPE_DUPLICATE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _PIPE0m, \
      -1}, \
     {mem_name ## _PIPE1m, \
      -1}, {mem_name ## _PIPE2m, -1}, {mem_name ## _PIPE3m, -1}}

#define TD3_MMU_HANDLE_ENUMS_IPIPE_DUPLICATE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _PIPE0m, \
      -1}, \
     {mem_name ## _PIPE1m, \
      -1}, {mem_name ## _PIPE2m, -1}, {mem_name ## _PIPE3m, -1}}

#define TD3_MMU_HANDLE_ENUMS_EPIPE_UNIQUE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _XPE0_PIPE0m, \
      mem_name ## _XPE2_PIPE0m}, \
     {mem_name ## _XPE0_PIPE1m, \
      mem_name ## _XPE2_PIPE1m}, \
     {mem_name ## _XPE1_PIPE2m, \
      mem_name ## _XPE3_PIPE2m}, \
     {mem_name ## _XPE1_PIPE3m, mem_name ## _XPE3_PIPE3m}}; \

#define TD3_MMU_HANDLE_ENUMS_IPIPE_UNIQUE(mem_name) \
    soc_mem_t mem_name ## _enums[4][2] = \
    {{mem_name ## _XPE0_PIPE0m, \
      mem_name ## _XPE1_PIPE0m}, \
     {mem_name ## _XPE2_PIPE1m, \
      mem_name ## _XPE3_PIPE1m}, \
     {mem_name ## _XPE2_PIPE2m, \
      mem_name ## _XPE3_PIPE2m}, \
     {mem_name ## _XPE0_PIPE3m, mem_name ## _XPE1_PIPE3m}}; \

#define TD3_MMU_GET_HANDLE(mem_name, base_index, acc_index, handle) \
    handle = mem_name ## _enums[base_index][acc_index]

/* START - FlexPort types as defined in SDK. */
typedef enum {
    TD3_SPEED_10G_ETH_DECODE  = 1,
    TD3_SPEED_10G_HG_DECODE   = 2,
    TD3_SPEED_20G_ETH_DECODE  = 3,
    TD3_SPEED_20G_HG_DECODE   = 4,
    TD3_SPEED_25G_ETH_DECODE  = 5,
    TD3_SPEED_25G_HG_DECODE   = 6,
    TD3_SPEED_40G_ETH_DECODE  = 7,
    TD3_SPEED_40G_HG_DECODE   = 8,
    TD3_SPEED_50G_ETH_DECODE  = 9,
    TD3_SPEED_50G_HG_DECODE   = 10,
    TD3_SPEED_100G_ETH_DECODE = 11,
    TD3_SPEED_100G_HG_DECODE  = 12
} _soc_td3_ct_speed_decode_e;

#endif /* TRIDENT3_MMU_DEFINES_H */
