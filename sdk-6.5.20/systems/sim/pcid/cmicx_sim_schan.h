/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * cmicx_sim_schan.h
 *
 * defines functions and defines used for chan functionality of the
 */


#ifndef _CMICX_SIM_SCHAN_H_
#define _CMICX_SIM_SCHAN_H_

/* includes */
#include "cmicx_sim_util.h"


/* bit defines for schan regs */
#define CMIC_SCHAN_CTRL_REG_SCHAN_ERROR_BIT                23
#define CMIC_SCHAN_CTRL_REG_TIMEOUT_BIT                    22
#define CMIC_SCHAN_CTRL_REG_NACK_BIT                       21
#define CMIC_SCHAN_CTRL_REG_SER_CHECK_FAIL_BIT             20
#define CMIC_SCHAN_CTRL_REG_ABORT_BIT                       2
#define CMIC_SCHAN_CTRL_REG_MSG_DONE_BIT                    1
#define CMIC_SCHAN_CTRL_REG_MSG_START_BIT                   0
#define CMIC_SCHAN_ERR_REG_OP_CODE_LSB                     26
#define CMIC_SCHAN_ERR_REG_OP_CODE_MSB                     31
#define CMIC_SCHAN_ERR_REG_DST_PORT_LSB                    20
#define CMIC_SCHAN_ERR_REG_DST_PORT_MSB                    25
#define CMIC_SCHAN_ERR_REG_SRC_PORT_LSB                    14
#define CMIC_SCHAN_ERR_REG_SRC_PORT_MSB                    19
#define CMIC_SCHAN_ERR_REG_DATA_LEN_LSB                     7
#define CMIC_SCHAN_ERR_REG_DATA_LEN_MSB                    13
#define CMIC_SCHAN_ERR_REG_ERRBIT_BIT                       6
#define CMIC_SCHAN_ERR_REG_ERR_CODE_LSB                     4
#define CMIC_SCHAN_ERR_REG_ERR_CODE_MSB                     5
#define CMIC_SCHAN_ERR_REG_NACK_BIT                         0
#define CMIC_SCHAN_ACK_DATA_BEAT_COUNT_REG_BEAT_COUNT_LSB   0
#define CMIC_SCHAN_ACK_DATA_BEAT_COUNT_REG_BEAT_COUNT_MSB   4
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_7_LSB          28
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_7_MSB          30
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_6_LSB          24
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_6_MSB          26
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_5_LSB          20
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_5_MSB          22
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_4_LSB          16
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_4_MSB          18
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_3_LSB          12
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_3_MSB          14
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_2_LSB           8
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_2_MSB          10
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_1_LSB           4
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_1_MSB           6
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_0_LSB           0
#define CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_0_MSB           2
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH0_WEIGHT_LSB    0
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH0_WEIGHT_MSB    3
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH1_WEIGHT_LSB    4
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH1_WEIGHT_MSB    7
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH2_WEIGHT_LSB    8
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH2_WEIGHT_MSB   11
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH3_WEIGHT_LSB   12
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH3_WEIGHT_MSB   15
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH4_WEIGHT_LSB   16
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH4_WEIGHT_MSB   19
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_FIFO_WEIGHT_LSB  20
#define CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_FIFO_WEIGHT_MSB  23

/* bit defines for schan opcode access */
/* see appendix A of CMICD A0 programmer reference guide */
#define CMIC_SCHAN_COMMAND_OPCODE_LSB     26
#define CMIC_SCHAN_COMMAND_OPCODE_MSB     31
#define CMIC_SCHAN_COMMAND_DPORT_LSB      19
#define CMIC_SCHAN_COMMAND_DPORT_MSB      25
#define CMIC_SCHAN_COMMAND_DLEN_LSB        7
#define CMIC_SCHAN_COMMAND_DLEN_MSB       13
#define CMIC_SCHAN_COMMAND_ERR_BIT         6
#define CMIC_SCHAN_COMMAND_ECODE_LSB       4
#define CMIC_SCHAN_COMMAND_ECODE_MSB       5
#define CMIC_SCHAN_COMMAND_DMA_BIT         3
#define CMIC_SCHAN_COMMAND_BANK_LSB        1
#define CMIC_SCHAN_COMMAND_BANK_MSB        2
#define CMIC_SCHAN_COMMAND_NACK_BIT        0
#define CMIC_SCHAN_COMMAND_STRIP_OPCODE(command)     REG_FIELD_GET(command,CMIC_SCHAN_COMMAND_OPCODE_MSB,  CMIC_SCHAN_COMMAND_OPCODE_LSB)
#define CMIC_SCHAN_COMMAND_STRIP_DPORT(command)      REG_FIELD_GET(command,CMIC_SCHAN_COMMAND_DPORT_MSB,   CMIC_SCHAN_COMMAND_DPORT_LSB)
#define CMIC_SCHAN_COMMAND_STRIP_DLEN(command)       REG_FIELD_GET(command,CMIC_SCHAN_COMMAND_DLEN_MSB,    CMIC_SCHAN_COMMAND_DLEN_LSB)
#define CMIC_SCHAN_COMMAND_STRIP_ERR(command)        REG_BIT_GET(command,CMIC_SCHAN_COMMAND_ERR_BIT)
#define CMIC_SCHAN_COMMAND_STRIP_ECODE(command)      REG_FIELD_GET(command,CMIC_SCHAN_COMMAND_ECODE_MSB,   CMIC_SCHAN_COMMAND_ECODE_LSB)
#define CMIC_SCHAN_COMMAND_STRIP_DMA(command)        REG_BIT_GET(command,CMIC_SCHAN_COMMAND_DMA_BIT)
#define CMIC_SCHAN_COMMAND_STRIP_BANK(command)       REG_FIELD_GET(command,CMIC_SCHAN_COMMAND_BANK_MSB,    CMIC_SCHAN_COMMAND_BANK_LSB)
#define CMIC_SCHAN_COMMAND_STRIP_NACK(command)       REG_BIT_GET(command,CMIC_SCHAN_COMMAND_NACK_BIT)

/* defines for the schan functionality */
#define CMIC_SCHAN_ACCESS_COUNTDOWN_VALUE  4
#define CMIC_SCHAN_ID_STRING_LENGTH       32
#define CMIC_SCHAN_NUM_MESSAGE_REGS       22
#define CMIC_SBUS_RINGS_PER_MAP_REG        8


/* enum for schan command opcodes */
typedef enum {
    SCHAN_MEM_RD     = 0x07,   SCHAN_MEM_RD_ACK     = 0x08,
    SCHAN_MEM_WR     = 0x09,   SCHAN_MEM_WR_ACK     = 0x0A,
    SCHAN_REG_RD     = 0x0B,   SCHAN_REG_RD_ACK     = 0x0C,
    SCHAN_REG_WR     = 0x0D,   SCHAN_REG_WR_ACK     = 0x0E,
    SCHAN_L2_INSERT  = 0x0F,   SCHAN_L2_INSERT_ACK  = 0x10,
    SCHAN_L2_DELETE  = 0x11,   SCHAN_L2_DELETE_ACK  = 0x12,
    SCHAN_L3_INSERT  = 0x1A,   SCHAN_L3_INSERT_ACK  = 0x1B,
    SCHAN_L3_DELETE  = 0x1C,   SCHAN_L3_DELETE_ACK  = 0x1D,
    SCHAN_FB_L2_LKUP = 0x20,   SCHAN_FB_L2_LKUP_ACK = 0x21,
    SCHAN_FB_L3_LKUP = 0x20,   SCHAN_FB_L3_LKUP_ACK = 0x21,
    SCHAN_TBL_INSERT = 0x24,   SCHAN_TBL_INSERT_ACK = 0x25,
    SCHAN_TBL_DELETE = 0x26,   SCHAN_TBL_DELETE_ACK = 0x27,
    SCHAN_TBL_LOOKUP = 0x28,   SCHAN_TBL_LOOKUP_ACK = 0x29,
    SCHAN_POP        = 0x2A,   SCHAN_POP_ACK        = 0x2B,
    SCHAN_PUSH       = 0x2C,   SCHAN_PUSH_ACK       = 0x2D
} schan_command_opcode;


/* structure definition for schan resources */
typedef struct {

    char                      id_string[CMIC_SCHAN_ID_STRING_LENGTH];

    /* control register states */
    bool                      ctrl_schan_error;
    bool                      ctrl_timeout;
    bool                      ctrl_nack;
    bool                      ctrl_ser_check_fail;
    bool                      ctrl_abort;
    bool                      ctrl_msg_done;
    bool                      ctrl_msg_start;

    /* error register states */
    schan_command_opcode      err_op_code;
    char                      err_dst_port;
    char                      err_src_port;
    char                      err_data_len;
    bool                      err_errbit;
    char                      err_err_code;
    bool                      err_nack;

    /* ack data beat count register states */
    char                      ack_data_beat_count;

    /* message registers */
    reg_t                     message_reg[CMIC_SCHAN_NUM_MESSAGE_REGS];

    /* internal states */
    bool                      access_running;
    schan_command_opcode      access_opcode;
    int                       access_countdown;   /* update countdown until schan access completes */

} cmicx_schan_data_t;


/* schan register access functions */
pcid_read_func_t  cmic_top_sbus_ring_arb_ctrl_schan_read;
pcid_write_func_t cmic_top_sbus_ring_arb_ctrl_schan_write;
pcid_read_func_t  cmic_top_sbus_timeout_read;
pcid_write_func_t cmic_top_sbus_timeout_write;
pcid_read_func_t  cmic_top_sbus_ring_map_0_7_read;
pcid_write_func_t cmic_top_sbus_ring_map_0_7_write;
pcid_read_func_t  cmic_top_sbus_ring_map_8_15_read;
pcid_write_func_t cmic_top_sbus_ring_map_8_15_write;
pcid_read_func_t  cmic_top_sbus_ring_map_16_23_read;
pcid_write_func_t cmic_top_sbus_ring_map_16_23_write;
pcid_read_func_t  cmic_top_sbus_ring_map_24_31_read;
pcid_write_func_t cmic_top_sbus_ring_map_24_31_write;
pcid_read_func_t  cmic_top_sbus_ring_map_32_39_read;
pcid_write_func_t cmic_top_sbus_ring_map_32_39_write;
pcid_read_func_t  cmic_top_sbus_ring_map_40_47_read;
pcid_write_func_t cmic_top_sbus_ring_map_40_47_write;
pcid_read_func_t  cmic_top_sbus_ring_map_48_55_read;
pcid_write_func_t cmic_top_sbus_ring_map_48_55_write;
pcid_read_func_t  cmic_top_sbus_ring_map_56_63_read;
pcid_write_func_t cmic_top_sbus_ring_map_56_63_write;
pcid_read_func_t  cmic_top_sbus_ring_map_64_71_read;
pcid_write_func_t cmic_top_sbus_ring_map_64_71_write;
pcid_read_func_t  cmic_top_sbus_ring_map_72_79_read;
pcid_write_func_t cmic_top_sbus_ring_map_72_79_write;
pcid_read_func_t  cmic_top_sbus_ring_map_80_87_read;
pcid_write_func_t cmic_top_sbus_ring_map_80_87_write;
pcid_read_func_t  cmic_top_sbus_ring_map_88_95_read;
pcid_write_func_t cmic_top_sbus_ring_map_88_95_write;
pcid_read_func_t  cmic_top_sbus_ring_map_96_103_read;
pcid_write_func_t cmic_top_sbus_ring_map_96_103_write;
pcid_read_func_t  cmic_top_sbus_ring_map_104_111_read;
pcid_write_func_t cmic_top_sbus_ring_map_104_111_write;
pcid_read_func_t  cmic_top_sbus_ring_map_112_119_read;
pcid_write_func_t cmic_top_sbus_ring_map_112_119_write;
pcid_read_func_t  cmic_top_sbus_ring_map_120_127_read;
pcid_write_func_t cmic_top_sbus_ring_map_120_127_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message0_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message0_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message1_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message1_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message2_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message2_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message3_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message3_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message4_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message4_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message5_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message5_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message6_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message6_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message7_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message7_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message8_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message8_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message9_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message9_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message10_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message10_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message11_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message11_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message12_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message12_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message13_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message13_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message14_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message14_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message15_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message15_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message16_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message16_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message17_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message17_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message18_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message18_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message19_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message19_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message20_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message20_write;
pcid_read_func_t  cmic_common_pool_schan_ch0_message21_read;
pcid_write_func_t cmic_common_pool_schan_ch0_message21_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message0_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message0_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message1_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message1_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message2_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message2_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message3_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message3_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message4_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message4_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message5_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message5_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message6_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message6_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message7_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message7_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message8_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message8_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message9_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message9_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message10_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message10_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message11_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message11_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message12_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message12_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message13_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message13_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message14_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message14_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message15_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message15_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message16_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message16_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message17_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message17_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message18_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message18_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message19_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message19_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message20_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message20_write;
pcid_read_func_t  cmic_common_pool_schan_ch1_message21_read;
pcid_write_func_t cmic_common_pool_schan_ch1_message21_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message0_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message0_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message1_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message1_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message2_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message2_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message3_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message3_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message4_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message4_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message5_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message5_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message6_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message6_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message7_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message7_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message8_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message8_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message9_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message9_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message10_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message10_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message11_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message11_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message12_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message12_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message13_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message13_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message14_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message14_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message15_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message15_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message16_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message16_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message17_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message17_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message18_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message18_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message19_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message19_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message20_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message20_write;
pcid_read_func_t  cmic_common_pool_schan_ch2_message21_read;
pcid_write_func_t cmic_common_pool_schan_ch2_message21_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message0_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message0_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message1_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message1_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message2_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message2_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message3_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message3_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message4_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message4_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message5_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message5_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message6_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message6_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message7_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message7_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message8_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message8_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message9_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message9_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message10_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message10_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message11_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message11_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message12_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message12_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message13_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message13_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message14_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message14_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message15_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message15_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message16_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message16_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message17_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message17_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message18_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message18_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message19_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message19_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message20_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message20_write;
pcid_read_func_t  cmic_common_pool_schan_ch3_message21_read;
pcid_write_func_t cmic_common_pool_schan_ch3_message21_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message0_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message0_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message1_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message1_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message2_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message2_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message3_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message3_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message4_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message4_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message5_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message5_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message6_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message6_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message7_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message7_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message8_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message8_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message9_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message9_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message10_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message10_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message11_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message11_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message12_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message12_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message13_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message13_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message14_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message14_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message15_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message15_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message16_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message16_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message17_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message17_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message18_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message18_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message19_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message19_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message20_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message20_write;
pcid_read_func_t  cmic_common_pool_schan_ch4_message21_read;
pcid_write_func_t cmic_common_pool_schan_ch4_message21_write;
pcid_read_func_t cmic_common_pool_schan_ch0_ctrl_read;
pcid_write_func_t cmic_common_pool_schan_ch0_ctrl_write;
pcid_read_func_t cmic_common_pool_schan_ch0_ack_data_beat_count_read;
pcid_write_func_t cmic_common_pool_schan_ch0_ack_data_beat_count_write;
pcid_read_func_t cmic_common_pool_schan_ch0_err_read;
pcid_write_func_t cmic_common_pool_schan_ch0_err_write;
pcid_read_func_t cmic_common_pool_schan_ch1_ctrl_read;
pcid_write_func_t cmic_common_pool_schan_ch1_ctrl_write;
pcid_read_func_t cmic_common_pool_schan_ch1_ack_data_beat_count_read;
pcid_write_func_t cmic_common_pool_schan_ch1_ack_data_beat_count_write;
pcid_read_func_t cmic_common_pool_schan_ch1_err_read;
pcid_write_func_t cmic_common_pool_schan_ch1_err_write;
pcid_read_func_t cmic_common_pool_schan_ch2_ctrl_read;
pcid_write_func_t cmic_common_pool_schan_ch2_ctrl_write;
pcid_read_func_t cmic_common_pool_schan_ch2_ack_data_beat_count_read;
pcid_write_func_t cmic_common_pool_schan_ch2_ack_data_beat_count_write;
pcid_read_func_t cmic_common_pool_schan_ch2_err_read;
pcid_write_func_t cmic_common_pool_schan_ch2_err_write;
pcid_read_func_t cmic_common_pool_schan_ch3_ctrl_read;
pcid_write_func_t cmic_common_pool_schan_ch3_ctrl_write;
pcid_read_func_t cmic_common_pool_schan_ch3_ack_data_beat_count_read;
pcid_write_func_t cmic_common_pool_schan_ch3_ack_data_beat_count_write;
pcid_read_func_t cmic_common_pool_schan_ch3_err_read;
pcid_write_func_t cmic_common_pool_schan_ch3_err_write;
pcid_read_func_t cmic_common_pool_schan_ch4_ctrl_read;
pcid_write_func_t cmic_common_pool_schan_ch4_ctrl_write;
pcid_read_func_t cmic_common_pool_schan_ch4_ack_data_beat_count_read;
pcid_write_func_t cmic_common_pool_schan_ch4_ack_data_beat_count_write;
pcid_read_func_t cmic_common_pool_schan_ch4_err_read;
pcid_write_func_t cmic_common_pool_schan_ch4_err_write;

#endif   /* SCHAN_H */

