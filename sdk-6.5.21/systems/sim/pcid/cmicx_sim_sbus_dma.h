/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * cmicx_sim_sbus_dma.h
 *
 * this file contains header definitions for sbus dma functionality
 */

#ifndef _CMICX_SIM_SBUS_DMA_H_
#define _CMICX_SIM_SBUS_DMA_H_

/* includes */
#include "cmicx_sim_util.h"


/* defines for SBUS DMA register bits */
#define CMIC_SBUSDMA_CONTROL_DESC_LAST_BIT                           31
#define CMIC_SBUSDMA_CONTROL_DESC_SKIP_BIT                           30
#define CMIC_SBUSDMA_CONTROL_DESC_JUMP_BIT                           29
#define CMIC_SBUSDMA_CONTROL_DESC_APPEND_BIT                         28
#define CMIC_SBUSDMA_CONTROL_DESC_DESCRIPTORS_REMAINING_MSB          27
#define CMIC_SBUSDMA_CONTROL_DESC_DESCRIPTORS_REMAINING_LSB          24
#define CMIC_SBUSDMA_CONTROL_REG_DESC_PREFETCH_ENABLE_BIT             4
#define CMIC_SBUSDMA_CONTROL_REG_ENDIANNESS_BIT                       3
#define CMIC_SBUSDMA_CONTROL_REG_MODE_BIT                             2
#define CMIC_SBUSDMA_CONTROL_REG_ABORT_BIT                            1
#define CMIC_SBUSDMA_CONTROL_REG_START_BIT                            0
#define CMIC_SBUSDMA_REQUEST_REG_DECR_BIT                            31
#define CMIC_SBUSDMA_REQUEST_REG_REQ_SINGLE_BIT                      30
#define CMIC_SBUSDMA_REQUEST_REG_INCR_NOADD_BIT                      29
#define CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_MSB                      28
#define CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_LSB                      24
#define CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_MSB                     23
#define CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_LSB                     16
#define CMIC_SBUSDMA_REQUEST_REG_DMA_WR_TO_NULLSPACE_BIT             13
#define CMIC_SBUSDMA_REQUEST_REG_WORDSWAP_IN_64BIT_SBUSDATA_BIT      12
#define CMIC_SBUSDMA_REQUEST_REG_MEMWR_ENDIANNESS_BIT                11
#define CMIC_SBUSDMA_REQUEST_REG_MEMRD_ENDIANNESS_BIT                10
#define CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_MSB                        9
#define CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_LSB                        5
#define CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_MSB                        4
#define CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_LSB                        0
#define CMIC_SBUSDMA_STATUS_REG_ECC_2BIT_CHECK_FAIL_BIT              13
#define CMIC_SBUSDMA_STATUS_REG_SBUSDMA_ACTIVE_BIT                   12
#define CMIC_SBUSDMA_STATUS_REG_ACTIVE_BIT                           11
#define CMIC_SBUSDMA_STATUS_REG_DESCRD_ERROR_BIT                     10
#define CMIC_SBUSDMA_STATUS_REG_SBUSACK_TIMEOUT_BIT                   9
#define CMIC_SBUSDMA_STATUS_REG_SBUSACK_ERROR_BIT                     8
#define CMIC_SBUSDMA_STATUS_REG_SBUSACK_NACK_BIT                      7
#define CMIC_SBUSDMA_STATUS_REG_SBUSACK_WRONG_OPCODE_BIT              6
#define CMIC_SBUSDMA_STATUS_REG_SBUSACK_WRONG_BEATCOUNT_BIT           5
#define CMIC_SBUSDMA_STATUS_REG_SER_CHECK_FAIL_BIT                    4
#define CMIC_SBUSDMA_STATUS_REG_HOSTMEMRD_ERROR_BIT                   3
#define CMIC_SBUSDMA_STATUS_REG_HOSTMEMWR_ERROR_BIT                   2
#define CMIC_SBUSDMA_STATUS_REG_ERROR_BIT                             1
#define CMIC_SBUSDMA_STATUS_REG_DONE_BIT                              0
#define CMIC_SBUSDMA_DEBUG_REG_ECC_2BIT_CHECK_FAIL_BIT                1
#define CMIC_SBUSDMA_DEBUG_REG_DESC_DONE_BIT                          0
#define CMIC_SBUSDMA_CONFIG_REG_ENABLE_EARLY_BRESP_BIT                3
#define CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH2_FLOW_CONTROL_BIT   2
#define CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH1_FLOW_CONTROL_BIT   1
#define CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH0_FLOW_CONTROL_BIT   0
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH0_WEIGHT_LSB   0
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH0_WEIGHT_MSB   3
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH1_WEIGHT_LSB   4
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH1_WEIGHT_MSB   7
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH2_WEIGHT_LSB   8
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH2_WEIGHT_MSB  11
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH0_WEIGHT_LSB  12
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH0_WEIGHT_MSB  15
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH1_WEIGHT_LSB  16
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH1_WEIGHT_MSB  19
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH2_WEIGHT_LSB  20
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH2_WEIGHT_MSB  23

/* defines for sbus dma functionality */
#define CMIC_SBUS_DMA_ACCESS_COUNTDOWN_VALUE    4
#define CMIC_SBUS_DMA_ID_STRING_LENGTH         32
#define CMIC_SBUS_DMA_WORDS_PER_DESCRIPTOR      8
#define CMIC_SBUS_DMA_BYTES_PER_WORD            4
#define CMIC_SBUS_DMA_DESCRIPTOR_MODE        true
#define CMIC_SBUS_DMA_REGISTER_MODE         false


/* sbus dma data struct - contains all the data necessary for sbus dma handling */
typedef struct {

    /* identification string for this sbus dma module */
    char                 id_string[CMIC_SBUS_DMA_ID_STRING_LENGTH];

    /* control register values */
    bool                 desc_prefetch_enable;
    bool                 endianness;
    bool                 mode;
    bool                 start;
    bool                 command_is_write;

    /* internal control values */
    bool                 last_desc;                   /* indicates the last descriptor in a descriptor stream */
    bool                 timer_running;               /* high when we are in timer mode and an access has been started (by the control register), whether an access is currently running or not */
    bool                 access_running;              /* high when we are running an sbus dma access of any kind */
    int                  timer_countdown;             /* current countdown value to next sbus dma initiation. taken from the timer register initially. */
    bool                 sequence_pending;            /* high when we have an sbus dma sequence running (pending) */
    int                  sbus_command_countdown;      /* countdown for a single sbus command completion */
    int                  pend_clocks_countdown;       /* countdown for the pending clocks waiting period in between sbus commands */

    /* request register values */
    bool                 decrement;
    bool                 req_single;
    bool                 incr_noadd;
    int                  incr_shift;
    int                  pend_clocks;
    bool                 dma_wr_to_nullspace;
    bool                 wordswap_in_64bit_sbus_data;
    bool                 memwr_endianness;
    bool                 memrd_endianness;
    int                  req_words;
    int                  resp_words;

    /* count register values */
    unsigned int         cur_count;

    /* opcode register value */
    reg_t                opcode;

    /* start register address value */
    addr_t               cur_sbus_address;

    /* hostmem register address value */
    uint64_t             cur_hostmem_address;

    /* hostmem descriptor start address value */
    uint64_t             cur_desc_address;

    /* status bits */
    bool                 ecc_2bit_check_fail;
    bool                 sbusdma_active;
    bool                 active;
    bool                 descrd_error;
    bool                 sbusack_timeout;
    bool                 sbusack_error;
    bool                 sbusack_nack;
    bool                 sbusack_wrong_opcode;
    bool                 sbusack_wrong_beatcount;
    bool                 ser_check_fail;
    bool                 hostmemrd_error;
    bool                 hostmemwr_error;
    bool                 error;
    bool                 done;

    /* debug register values */
    bool                 desc_done;

    /* iter count register value */
    reg_t                iter_count;

    /* timer register value */
    reg_t                timer;

    /* last configured values for things */
    reg_t                cur_sbusdma_config_request;
    reg_t                cur_sbusdma_config_count;
    reg_t                cur_sbusdma_config_opcode;
    reg_t                cur_sbusdma_config_sbus_start_address;
    uint64_t             cur_sbusdma_config_hostmem_start_address;
    uint64_t             cur_sbusdma_config_desc_start_address;

    /* per cmc sbusdma config values */
    bool                 enable_sbusdma_flow_control;

} cmicx_sbus_dma_data_t;


/* sbus dma support functions */
struct cmicx_cmc_struct;
bool cmic_sbus_dma_access_start(cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbus_dma_access_abort(cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_control_read(reg_t *reg,cmicx_sbus_dma_data_t *);
bool cmic_sbusdma_control_write(reg_t reg,cmicx_sbus_dma_data_t *);
bool cmic_sbusdma_request_read(reg_t *reg,cmicx_sbus_dma_data_t *);
bool cmic_sbusdma_request_write(reg_t reg,cmicx_sbus_dma_data_t *);
bool cmic_sbusdma_count_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_opcode_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_sbus_start_address_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_hostmem_start_address_lo_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_hostmem_start_address_hi_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_desc_start_address_lo_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_desc_start_address_hi_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_status_read(reg_t *reg,cmicx_sbus_dma_data_t *);
bool cmic_sbusdma_shared_config_read(reg_t *val,struct cmicx_cmc_struct *cmc);
bool cmic_sbusdma_shared_config_write(reg_t val,struct cmicx_cmc_struct *cmc);
bool cmic_sbusdma_debug_read(reg_t *val,cmicx_sbus_dma_data_t *sbus_dma_data);
bool cmic_sbusdma_debug_clr_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data);


/* sbus dma register access functions */
pcid_read_func_t  cmic_top_sbus_ring_arb_ctrl_sbusdma_read;
pcid_write_func_t cmic_top_sbus_ring_arb_ctrl_sbusdma_write;
pcid_read_func_t  cmic_cmc0_shared_config_read;
pcid_write_func_t cmic_cmc0_shared_config_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_control_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_control_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_request_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_request_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_opcode_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_opcode_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_sbus_start_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_sbus_start_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_desc_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_desc_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_desc_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_desc_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_status_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_status_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_hostmem_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_hostmem_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_hostmem_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_hostmem_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_sbus_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_sbus_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_desc_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_desc_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_desc_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_desc_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_request_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_request_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_opcode_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_opcode_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_sbusdma_debug_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_sbusdma_debug_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_sbusdma_debug_clr_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_sbusdma_debug_clr_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_control_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_control_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_iter_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_iter_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_timer_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_timer_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_tm_control_0_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_tm_control_0_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_tm_control_1_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_tm_control_1_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch0_tm_control_2_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch0_tm_control_2_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_control_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_control_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_request_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_request_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_opcode_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_opcode_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_sbus_start_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_sbus_start_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_desc_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_desc_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_desc_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_desc_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_status_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_status_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_hostmem_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_hostmem_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_hostmem_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_hostmem_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_sbus_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_sbus_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_desc_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_desc_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_desc_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_desc_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_request_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_request_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_opcode_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_opcode_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_sbusdma_debug_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_sbusdma_debug_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_sbusdma_debug_clr_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_sbusdma_debug_clr_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_control_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_control_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_iter_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_iter_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_timer_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_timer_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_tm_control_0_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_tm_control_0_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_tm_control_1_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_tm_control_1_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch1_tm_control_2_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch1_tm_control_2_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_control_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_control_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_request_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_request_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_opcode_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_opcode_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_sbus_start_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_sbus_start_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_desc_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_desc_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_desc_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_desc_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_status_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_status_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_hostmem_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_hostmem_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_hostmem_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_hostmem_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_sbus_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_sbus_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_desc_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_desc_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_desc_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_desc_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_request_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_request_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_opcode_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_opcode_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_sbusdma_debug_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_sbusdma_debug_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_sbusdma_debug_clr_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_sbusdma_debug_clr_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_control_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_control_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_iter_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_iter_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_timer_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_timer_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_tm_control_0_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_tm_control_0_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_tm_control_1_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_tm_control_1_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch2_tm_control_2_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch2_tm_control_2_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_control_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_control_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_request_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_request_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_opcode_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_opcode_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_sbus_start_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_sbus_start_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_desc_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_desc_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_desc_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_desc_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_status_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_status_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_hostmem_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_hostmem_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_hostmem_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_hostmem_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_sbus_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_sbus_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_desc_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_desc_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_desc_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_desc_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_request_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_request_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_opcode_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_opcode_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_sbusdma_debug_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_sbusdma_debug_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_sbusdma_debug_clr_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_sbusdma_debug_clr_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_address_lo_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_address_lo_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_address_hi_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_address_hi_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_control_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_control_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_iter_count_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_iter_count_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_timer_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_timer_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_tm_control_0_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_tm_control_0_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_tm_control_1_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_tm_control_1_write;
pcid_read_func_t  cmic_cmc0_sbusdma_ch3_tm_control_2_read;
pcid_write_func_t cmic_cmc0_sbusdma_ch3_tm_control_2_write;
pcid_read_func_t  cmic_cmc1_shared_config_read;
pcid_write_func_t cmic_cmc1_shared_config_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_control_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_control_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_request_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_request_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_opcode_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_opcode_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_sbus_start_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_sbus_start_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_desc_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_desc_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_desc_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_desc_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_status_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_status_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_hostmem_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_hostmem_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_hostmem_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_hostmem_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_sbus_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_sbus_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_desc_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_desc_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_desc_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_desc_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_request_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_request_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_opcode_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_opcode_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_sbusdma_debug_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_sbusdma_debug_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_sbusdma_debug_clr_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_sbusdma_debug_clr_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_control_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_control_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_iter_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_iter_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_timer_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_timer_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_tm_control_0_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_tm_control_0_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_tm_control_1_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_tm_control_1_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch0_tm_control_2_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch0_tm_control_2_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_control_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_control_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_request_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_request_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_opcode_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_opcode_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_sbus_start_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_sbus_start_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_desc_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_desc_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_desc_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_desc_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_status_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_status_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_hostmem_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_hostmem_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_hostmem_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_hostmem_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_sbus_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_sbus_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_desc_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_desc_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_desc_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_desc_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_request_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_request_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_opcode_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_opcode_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_sbusdma_debug_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_sbusdma_debug_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_sbusdma_debug_clr_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_sbusdma_debug_clr_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_control_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_control_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_iter_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_iter_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_timer_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_timer_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_tm_control_0_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_tm_control_0_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_tm_control_1_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_tm_control_1_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch1_tm_control_2_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch1_tm_control_2_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_control_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_control_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_request_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_request_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_opcode_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_opcode_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_sbus_start_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_sbus_start_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_desc_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_desc_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_desc_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_desc_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_status_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_status_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_hostmem_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_hostmem_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_hostmem_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_hostmem_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_sbus_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_sbus_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_desc_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_desc_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_desc_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_desc_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_request_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_request_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_opcode_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_opcode_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_sbusdma_debug_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_sbusdma_debug_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_sbusdma_debug_clr_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_sbusdma_debug_clr_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_control_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_control_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_iter_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_iter_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_timer_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_timer_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_tm_control_0_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_tm_control_0_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_tm_control_1_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_tm_control_1_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch2_tm_control_2_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch2_tm_control_2_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_control_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_control_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_request_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_request_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_opcode_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_opcode_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_sbus_start_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_sbus_start_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_desc_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_desc_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_desc_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_desc_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_status_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_status_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_hostmem_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_hostmem_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_hostmem_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_hostmem_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_sbus_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_sbus_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_desc_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_desc_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_desc_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_desc_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_request_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_request_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_opcode_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_opcode_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_sbusdma_debug_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_sbusdma_debug_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_sbusdma_debug_clr_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_sbusdma_debug_clr_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_address_lo_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_address_lo_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_address_hi_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_address_hi_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_control_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_control_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_iter_count_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_iter_count_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_timer_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_timer_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_tm_control_0_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_tm_control_0_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_tm_control_1_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_tm_control_1_write;
pcid_read_func_t  cmic_cmc1_sbusdma_ch3_tm_control_2_read;
pcid_write_func_t cmic_cmc1_sbusdma_ch3_tm_control_2_write;


#endif   /* _SBUS_DMA_H_ */


