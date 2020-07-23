/** \file kbp_rop.h
 *
 * Functions and defines for handling jericho2 kbp ROP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_KBP)

#ifndef _KBP_ROP_INCLUDED__
/*
 * {
 */

#define _KBP_ROP_INCLUDED__

/*************
 * INCLUDES  *
 */
/*
 * {
 */
#include <soc/register.h> 
#include <soc/kbp/nlm3/arch/nlmarch.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/port.h>
/*
 * }
 */

/*************
 * DEFINES   *
 */
/*
 * {
 */

#define DNX_KBP_CPU_DATA_REGISTER_BYTE_LEN    (64)
#define DNX_KBP_CPU_LOOKUP_REPLY_DATA_REGISTER_BYTE_LEN    (32)

#define DNX_KBP_MAX_NUM_OF_PARALLEL_SEARCHES    (8)

#define DNX_KBP_ERROR_RETRANMISSION    (5)

#define DNX_KBP_LUT_DATA_ADDR_BIT_LEN    (0x8)
#define DNX_KBP_LUT_DATA_ADDR_MASK    (0xFF)
#define DNX_KBP_LUTWR_DATA_ADDR_START_BIT   (0)

#define DNX_KBP_LUT_REC_SIZE_BIT_LEN    (0x7)
#define DNX_KBP_LUT_REC_SIZE_MASK    (0x7F)
#define DNX_KBP_LUTWR_REC_SIZE_START_BIT    (DNX_KBP_LUT_DATA_ADDR_BIT_LEN + DNX_KBP_LUTWR_DATA_ADDR_START_BIT)
#define DNX_KBP_LUTRD_REC_SIZE_START_BIT    (0)

#define DNX_KBP_LUT_REC_VALID_BIT_LEN    (0x1)
#define DNX_KBP_LUT_REC_VALID_MASK    (0x1)
#define DNX_KBP_LUTWR_REC_VALID_START_BIT    (DNX_KBP_LUTWR_REC_SIZE_START_BIT + DNX_KBP_LUT_REC_SIZE_BIT_LEN)
#define DNX_KBP_LUTRD_REC_VALID_START_BIT    (DNX_KBP_LUTRD_REC_SIZE_START_BIT + DNX_KBP_LUT_REC_SIZE_BIT_LEN)

#define DNX_KBP_LUT_MODE_BIT_LEN    (0x1)
#define DNX_KBP_LUT_MODE_MASK    (0x1)
#define DNX_KBP_LUTWR_MODE_START_BIT    (0)
#define DNX_KBP_LUTRD_MODE_START_BIT    (DNX_KBP_LUTRD_REC_VALID_START_BIT + DNX_KBP_LUT_REC_VALID_BIT_LEN)

#define DNX_KBP_LUT_INSTR_BIT_LEN    (0xa)
#define DNX_KBP_LUT_INSTR_MASK    (0x3FF)
#define DNX_KBP_LUTWR_INSTR_START_BIT    (DNX_KBP_LUTWR_MODE_START_BIT + DNX_KBP_LUT_MODE_BIT_LEN)
#define DNX_KBP_LUTRD_INSTR_START_BIT    (DNX_KBP_LUTRD_MODE_START_BIT + DNX_KBP_LUT_MODE_BIT_LEN)

#define DNX_KBP_LUT_LTR_BIT_LEN    (0x7)
#define DNX_KBP_LUT_LTR_MASK    (0x7F)

#define DNX_KBP_LUT_CMP_INSTR_BIT_LEN    (0x1)
#define DNX_KBP_LUT_CMP_INSTR_MASK    (0x1)
#define DNX_KBP_LUTWR_CMP_INSTR_START_BIT    (DNX_KBP_LUTWR_MODE_START_BIT + DNX_KBP_LUT_MODE_BIT_LEN + DNX_KBP_LUT_LTR_BIT_LEN)
#define DNX_KBP_LUTRD_CMP_INSTR_START_BIT    (0)

#define DNX_KBP_LUT_PROG0_REG_ADDR_PORT0    \
    ((Kbp_app_data[unit]->device_type == KBP_DEVICE_OP) ? 0xA005 : 0x2005)
#define DNX_KBP_LUT_PROG1_REG_ADDR_PORT0    \
    ((Kbp_app_data[unit]->device_type == KBP_DEVICE_OP) ? 0xA006 : 0x2006)
#define DNX_KBP_LUT_PROG_REG_DEVAD_PORT0     0x1

#define DNX_KBP_LUT_RDADDR_REG_ADDR_PORT0    0xA007
#define DNX_KBP_LUT_RDATA0_REG_ADDR_PORT0    0xA008
#define DNX_KBP_LUT_RDATA1_REG_ADDR_PORT0    0xA009

#define DNX_KBP_LUT_PROG0_REG_ADDR_PORT1    \
    ((Kbp_app_data[unit]->device_type == KBP_DEVICE_OP) ? 0xB005 : 0x2005)
#define DNX_KBP_LUT_PROG1_REG_ADDR_PORT1    \
    ((Kbp_app_data[unit]->device_type == KBP_DEVICE_OP) ? 0xB006 : 0x2006)
#define DNX_KBP_LUT_PROG_REG_DEVAD_PORT1    \
    ((Kbp_app_data[unit]->device_type == KBP_DEVICE_OP) ? 0x1 : 0x2)

#define DNX_KBP_LUT_RDADDR_REG_ADDR_PORT1    0xB007
#define DNX_KBP_LUT_RDATA0_REG_ADDR_PORT1    0xB008
#define DNX_KBP_LUT_RDATA1_REG_ADDR_PORT1    0xB009

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */
#define DNX_KBP_ROP_REVERSE_DATA(data_in, data_out, len) \
{ \
    int i; \
    for (i = 0; i < len; i++) \
    { \
        data_out[i] = data_in[len - 1 - i]; \
    } \
} \

#define DNX_KBP_ROP_LUT_LTR_GET(instr) \
    ((instr & 0x3F) | (instr & 0x8000) >> 9)

#define DNX_KBP_ROP_LUT_INSTR_GET(ltr) \
    ((ltr & 0x3f) | (0x001 << 6) | ((ltr & 0x40) << 9))

/*
 * Get ROP packet from DNX side
 */
#define DNX_KBP_ROP_GET_DNX_SIDE           0x1
/*
 * Get ROP packet from KBP side
 */
#define DNX_KBP_ROP_GET_KBP_SIDE           0x2

#define DNX_KBP_ROP_OPCODE_LEN_IN_BYTE     0x1
#define DNX_KBP_ROP_SEQ_NUM_LEN_IN_BYTE    0x2

#define DNX_KBP_ROP_OPCODE_OFFSET          0x0
#define DNX_KBP_ROP_SEQ_NUM_OFFSET         (DNX_KBP_ROP_OPCODE_OFFSET + DNX_KBP_ROP_OPCODE_LEN_IN_BYTE)
#define DNX_KBP_ROP_PAYLOAD_OFFSET         (DNX_KBP_ROP_SEQ_NUM_OFFSET + DNX_KBP_ROP_SEQ_NUM_LEN_IN_BYTE)

/*
 * }
 */

/*************
 * ENUMS     *
 */
/*
 * {
 */
typedef enum
{
    /*
     * CB Write 
     */
    NLM_DNX_CB_INST_WRITE,
    /*
     * CB Write and Compare 1 
     */
    NLM_DNX_CB_INST_CMP1,
    /*
     * CB Write and Compare 2 
     */
    NLM_DNX_CB_INST_CMP2,
    /*
     * CB Write and Compare 3 
     */
    NLM_DNX_CB_INST_CMP3,
    /*
     * CB Write and LPM 
     */
    NLM_DNX_CB_INST_LPM,
    /*
     * This does not represent any instruction and is used for marking end of the enum 
     */
    NLM_DNX_CB_INST_NONE
} NlmDnxCBInstType;
typedef enum NlmDnxReadMode
{
    /*
     * The read of a database X value is performed 
     */
    NLM_DNX_READ_MODE_DATA_X,
    /*
     * The read of a database Y value is performed 
     */
    NLM_DNX_READ_MODE_DATA_Y
} NlmDnxReadMode;
typedef enum NlmDnxWriteMode
{
    /*
     * The write mode is database-data mask mode 
     */
    NLM_DNX_WRITE_MODE_DATABASE_DM,
    /*
     * The write mode is database-XY mode 
     */
    NLM_DNX_WRITE_MODE_DATABASE_XY
} NlmDnxWriteMode;
typedef enum NlmDnxCompareResponseFormat
{
    /*
     * Return only index without Associated Data in the result 
     */
    NLM_DNX_ONLY_INDEX_NO_AD = 0x0,
    /*
     * Return index + 32b Associated Data in the result 
     */
    NLM_DNX_INDEX_AND_32B_AD = 0x1,
    /*
     * Return index + 64b Associated Data in the result 
     */
    NLM_DNX_INDEX_AND_64B_AD = 0x2,
    /*
     * Return index + 128b Associated Data in the result 
     */
    NLM_DNX_INDEX_AND_128B_AD = 0x3,
    /*
     * Return index + 256b Associated Data in the result 
     */
    NLM_DNX_INDEX_AND_256B_AD = 0x4,
    /*
     * No result 
     */
    NLM_DNX_NO_INDEX_NO_AD = -1
} NlmDnxCompareResponseFormat;

/*
 * }
 */
/*************
 * TYPE DEFS *
 */
/*
 * {
 */
/*
 * Request Decoder 
 */
typedef struct dnx_kbp_lut_data_t
{

    /*
     * Record Size (max upto 672b=84B) 640b of data + 16b of instr + 16b of contextid 
     */
    uint32 rec_size;

    /*
     * Record Type  Options:  DNX_KBP_LUT_REC_TYPE_INFO  DNX_KBP_LUT_REC_TYPE_REQUEST  
     */
    uint32 rec_type;

    /*
     * Record Is Valid 
     */
    uint32 rec_is_valid;

    /*
     * Request Control
     */

    /*
     * Mode - To be used to construct the instr, contextId and Key to core based on table 1
     * Options:
     * DNX_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE  
     * DNX_KBP_LUT_INSTR_LUT1_REC_DATA_CONTEXID_SEQ_NUM_MODE 
     * DNX_KBP_LUT_INSTR_REC_DATA_CONTEXID_SEQ_NUM_MODE 
     * DNX_KBP_LUT_INSTR_LUT1_CONTEXID_REC_DATA_MODE 
     * DNX_KBP_LUT_INSTR_REC_DATA_CONTEXID_REC_DATA_MODE 
     */
    uint32 mode;

    /*
     * Key_config
     * Options:
     * DNX_KBP_LUT_KEY_CONFIG_APPEND_INCOMING_DATA - This feature makes sense only for Compare kind of instructions
     * DNX_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA
     */
    uint32 key_config;

    /*
     * Lut Key Data
     * Data to be used for final key construction.
     * max value - 0xff
     */
    uint32 lut_key_data;

    /*
     * Instruction Bits
     * max_value - 0x3ff
     */
    uint32 instr;

    /*
     * key_w_cpd_gt_80 
     *    Length of key if record data is between 80b and 128b and copy_data > 80b
     * Options:
     *    DNX_KBP_LUT_KEY_SIZE - 
     *    DNX_KBP_LUT_80b_KEY_SIZE - 80b  
     */
    uint32 key_w_cpd_gt_80;

    /*
     * Copy Data Config
     *    Number of bits of Copy Data to be extracted from MSB portion of Record Data (after instr/ctxtId have been extracted 
     *    if applicable but before lut_key_data has been appended to form the final key) and attached to reply.
     * Options:
     *     DNX_KBP_LUT_EXTR_MSB_PORTION_DATA_16b 
     *     DNX_KBP_LUT_EXTR_MSB_PORTION_DATA_32b 
     *     DNX_KBP_LUT_EXTR_MSB_PORTION_DATA_48b 
     *     DNX_KBP_LUT_EXTR_MSB_PORTION_DATA_64b 
     *     DNX_KBP_LUT_EXTR_MSB_PORTION_DATA_80b 
     *     DNX_KBP_LUT_EXTR_MSB_PORTION_DATA_96b 
     *     DNX_KBP_LUT_EXTR_MSB_PORTION_DATA_112b 
     *     DNX_KBP_LUT_EXTR_MSB_PORTION_DATA_128b 
     */
    uint32 copy_data_cfg;

    /*
     * Reply Format
     */
    /*
     * Resultx Index or AD
     * Options:
     *     DNX_KBP_LUT_TRANSFER_INDX_ONLY
     *     DNX_KBP_LUT_TRANSFER_AD_ONLY    
     */

    /*
     * Resultx Index Ad Cfg
     * Optiond: 
     *     If Index is to be transferred ( resultx_idx_or_ad = DNX_KBP_LUT_TRANSFER_INDX_ONLY):
     *         DNX_KBP_LUT_INDX_TRANSFER_1B 
     *         DNX_KBP_LUT_INDX_TRANSFER_2B
     *         DNX_KBP_LUT_INDX_TRANSFER_2B  
     *     If AD is to be transferred  ( resultx_idx_or_ad = DNX_KBP_LUT_TRANSFER_AD_ONLY): 
     *         DNX_KBP_LUT_AD_TRANSFER_1B, DNX_KBP_LUT_AD_TRANSFER_2B... DNX_KBP_LUT_AD_TRANSFER_16B 
     */
    uint32 result_idx_ad_cfg[DNX_KBP_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint32 result_idx_or_ad[DNX_KBP_MAX_NUM_OF_PARALLEL_SEARCHES];
} dnx_kbp_lut_data_t;
typedef struct dnx_kbp_rop_write_s
{
    /*
     * Address of location where data should be written. includes all :vbit, wrmode ...
     */
    uint8 addr[NLMDEV_REG_ADDR_LEN_IN_BYTES];
    /*
     * The data that should be written 
     */
    uint8 data[NLM_DATA_WIDTH_BYTES];
    /*
     * The mask that should be written if it is a database entry 
     */
    uint8 mask[NLM_DATA_WIDTH_BYTES];
    /*
     * Address of location where data should be written. includes nothing 
     */
    uint8 addr_short[NLMDEV_REG_ADDR_LEN_IN_BYTES];
    /*
     * The valid bit which indicates if the database entry should be enabled or disabled 
     */
    uint8 vBit;
    /*
     * Write mode which indicates if it is Database X or Y 
     */
    NlmDnxWriteMode writeMode;
    uint32 loop;

} dnx_kbp_rop_write_t;

typedef struct dnx_kbp_rop_read_s
{
    /*
     * Address of the memory location to be read 
     */
    uint8 addr[NLMDEV_REG_ADDR_LEN_IN_BYTES];
    /*
     * valid bit for database read 
     */
    uint8 vBit;
    /*
     * reading data either X or Y 
     */
    NlmDnxReadMode dataType;
    /*
     * data read will be stored here 
     */
    uint8 data[NLM_DATA_WIDTH_BYTES + 1];
} dnx_kbp_rop_read_t;

typedef struct dnx_kbp_rop_cbw_s
{
    /*
     * max 640-bit data valid data specified by datalen 
     */
    uint8 data[NLM_MAX_CMP_DATA_LEN_BYTES];
    /*
     * data length in Bytes (should be multiple of 10, max value = 80) 
     */
    uint32 data_len;
} dnx_kbp_rop_cbw_t;

typedef struct NlmDnxCompareResult
{
    /*
     * Indicates if the hit or miss are valid or not  if no blocks have * been configured for an output port,  then
     * the hit/miss returned is * not valid 
     */
    uint8 isResultValid[NLM_MAX_NUM_RESULTS];
    /*
     * Indicates if a hit or miss occured 
     */
    uint8 hitOrMiss[NLM_MAX_NUM_RESULTS];
    /*
     * Hit indexes indicating DBA/UDA address 
     */
    uint32 hitIndex[NLM_MAX_NUM_RESULTS];
    /*
     * Indicates the size of AD in the result 
     */
    NlmDnxCompareResponseFormat responseFormat[NLM_MAX_NUM_RESULTS];
    /*
     * Associated data. If result 0 has 16b associated data, then it will be stored in m_assocData[0][0] and
     * m_assocData[0][1]
     */
    uint8 assocData[NLM_MAX_NUM_RESULTS][NLM_MAX_SRAM_SB_WIDTH_IN_BYTES];
    uint8 data_raw[NLMDEV_MAX_RESP_LEN_IN_BYTES];
} NlmDnxCompareResult;

typedef struct dnx_kbp_rop_cbw_cmp_s
{
    NlmDnxCBInstType type;
    uint32 opcode;
    uint32 ltr;
    dnx_kbp_rop_cbw_t cbw_data;
    NlmDnxCompareResult result;
} dnx_kbp_rop_cbw_cmp_t;

typedef struct dnx_kbp_rop_blk_s
{
    uint32 opcode;
    uint16 loop_cnt;
    /*
     * First DB address location 
     */
    uint32 src_addr;
    /*
     * destination address for copy/move instructions 
     */
    uint32 dst_addr;
    /*
     * validate or invalidate for clear/invalidate instructions 
     */
    uint8 set_not_clr;
    /*
     * specifies if address is incremented or decremented in the loop 
     */
    uint8 copy_dir;
    /*
     * data length in Bytes 
     */
    uint32 data_len;
} dnx_kbp_rop_blk_t;

typedef struct dnx_kbp_record_trigger_s
{
    int msb_trigger;
    int lsb_trigger;
    int record_trigger;
} dnx_kbp_record_trigger_t;

typedef struct dnx_kbp_record_reply_s
{
    uint32 reply_valid;
    soc_reg_above_64_val_t reply_data;
    uint32 rop_error;
    uint32 elk_error;
} dnx_kbp_record_reply_t;

typedef struct dnx_kbp_rop_packet_s
{
    uint8 opcode;
    uint16 sequence_num;
    uint8 payload[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES];
    uint32 payload_len;
} dnx_kbp_rop_packet_t;

typedef enum
{
    DNX_KBP_ROP_REQUEST,
    DNX_KBP_ROP_RESPONSE,
} dnx_kbp_rop_packet_type_e;

/*
 * }
 */

/*************
 * GLOBALS   *
 */
/*
 * {
 */
/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */
/* 
 *  Utility functions
 */
/*
 *    ELK CPU/ROP access functions
 */
shr_error_e dnx_kbp_cpu_lookup_reply(
    int unit,
    uint32 core,
    dnx_kbp_record_reply_t * reccord_reply);

shr_error_e dnx_kbp_cpu_record_send(
    int unit,
    uint32 core,
    uint32 opcode,
    soc_reg_above_64_val_t msb_data,
    soc_reg_above_64_val_t lsb_data,
    int lsb_enable,
    soc_reg_above_64_val_t read_data);

shr_error_e dnx_kbp_lut_write(
    int unit,
    uint32 core,
    uint8 addr,
    dnx_kbp_lut_data_t * lut_data);

shr_error_e dnx_kbp_lut_read(
    int unit,
    uint32 core,
    uint8 addr,
    dnx_kbp_lut_data_t * lut_data);

shr_error_e dnx_kbp_rop_read(
    int unit,
    uint32 core,
    dnx_kbp_rop_read_t * rd_data);

shr_error_e dnx_kbp_rop_write(
    int unit,
    uint32 core,
    dnx_kbp_rop_write_t * wr_data);

shr_error_e dnx_kbp_rop_scratch_write_read(
    int unit,
    bcm_core_t core);

shr_error_e dnx_kbp_rop_cbw_cmp(
    int unit,
    uint32 core,
    dnx_kbp_rop_cbw_cmp_t * cbw_cmp_data);

shr_error_e dnx_kbp_init_rop_test(
    int unit);

shr_error_e dnx_kbp_rop_last_request_get(
    int unit,
    bcm_core_t core,
    uint32 flags,
    dnx_kbp_rop_packet_t * pkt);

shr_error_e dnx_kbp_rop_last_response_get(
    int unit,
    bcm_core_t core,
    uint32 flags,
    dnx_kbp_rop_packet_t * pkt);

#if defined(BE_HOST)
void dnx_kbp_buffer_endians_fix(
    int unit,
    soc_reg_above_64_val_t buffer);
#endif
/*
 * }
 */
#endif /* __KBP_ROP_INCLUDED__ */
#endif /* defined(INCLUDE_KBP) */
