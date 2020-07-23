/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#include <shared/pack.h>

#if defined(BCM_88650_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM



#define JER_KBP_ROP_OPTIMIZATION





#ifdef ARAD_KBP_DISABLE_IHB_LOOKUP_REPLY_FOR_ROP_TRANSMIT
#ifndef ARAD_KBP_ROP_OPTIMIZATION
#define ARAD_KBP_ROP_OPTIMIZATION
#endif 
#endif

#define KBP_ROP_DMA_NUM_TRANS 1000



#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/drv.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef ARAD_KBP_ROP_TIME_MEASUREMENTS
#include <soc/dpp/SAND/Management/sand_low_level.h>
#endif
 


int _soc_cmic_uc_msg_process_status(int unit, int uC);




#define KBP_ERROR_RETRANMISSION 5

#define ARAD_KBP_CPU_DATA_REGISTER_BIT_LEN   (512)

#define ARAD_KBP_LUTWR_DATA_ADDR_BIT_LEN   (8)
#define ARAD_KBP_LUTWR_DATA_ADDR_BIT_START (ARAD_KBP_CPU_DATA_REGISTER_BIT_LEN - ARAD_KBP_LUTWR_DATA_ADDR_BIT_LEN)

#define ARAD_KBP_LUTWR_LUT_DATA_LEN   72
#define ARAD_KBP_LUTWR_LUT_DATA_START ARAD_KBP_CPU_DATA_REGISTER_BIT_LEN - 80

#ifdef ARAD_KBP_ROP_OPTIMIZATION

    #define KBP_ROP_DATA_SIZE                        16

    #define KBP_SCHAN_OPCODE_64_IHB_WRITE            0x37502000
    #define KBP_SCHAN_ADDR_IHB_MSB_WRITE_ADDR        0x0000022f
    #define KBP_SCHAN_ADDR_IHB_LSB_WRITE_ADDR        0x0000021f

    #define KBP_SCHAN_OPCODE_32_IHB_WRITE            0x37500200
    #define KBP_SCHAN_ADDR_ROP_OPCODE_ADDR            0x0000023f
    #define KBP_SCHAN_ADDR_ROP_TRIGGER_ADDR            0x0000021e    

    #define KBP_SCHAN_OPCODE_32_IHB_READ            0x2f500200
    #define KBP_SCHAN_ADDR_IHB_REPLY_ADDR            0x00000240

    #define KBP_SCHAN_OPCODE_IHB_ROP_READ           0x2f501400
    #define KBP_SCHAN_ADDR_ROP_REPLY_ADDR            0x00000241

#endif
#ifdef JER_KBP_ROP_OPTIMIZATION

    #define KBP_SCHAN_JER_OPCODE_64_IHB_WRITE            (SOC_IS_JERICHO_PLUS_ONLY(unit) ? \
                                                          ((core == 2 ) ? 0x37B02000 : (core ? 0x36082000 : 0x36002000)) : \
                                                          ((core == 2 ) ? 0x37882000 : (core ? 0x35e82000 : 0x35e02000)))
    #define KBP_SCHAN_JER_ADDR_IHB_MSB_WRITE_ADDR        0x0000024f
    #define KBP_SCHAN_JER_ADDR_IHB_LSB_WRITE_ADDR        0x0000023f

    #define KBP_SCHAN_JER_OPCODE_32_IHB_WRITE            (SOC_IS_JERICHO_PLUS_ONLY(unit) ? \
                                                          ((core == 2 ) ? 0x37B00200 : (core ? 0x36080200 : 0x36000200)) : \
                                                          ((core == 2 ) ? 0x37880200 : (core ? 0x35e80200 : 0x35e00200)))
    #define KBP_SCHAN_JER_ADDR_ROP_OPCODE_ADDR           0x0000025f
    #define KBP_SCHAN_JER_ADDR_ROP_TRIGGER_ADDR          0x0000023e    

    #define KBP_SCHAN_JER_OPCODE_IHB_ROP_READ            (SOC_IS_JERICHO_PLUS_ONLY(unit) ? \
                                                          ((core == 2 ) ? 0x2FB01400 : (core ? 0x2E081400 : 0x2E001400)) : \
                                                          ((core == 2 ) ? 0x2F881400 : (core ? 0x2de81400 : 0x2de01400)))
    #define KBP_SCHAN_JER_ADDR_ROP_REPLY_ADDR            0x00000260

#endif

#define ARAD_KBP_LUT_REC_SIZE_BIT_LEN         0x7
#define ARAD_KBP_LUTWR_REC_SIZE_BIT_START       ARAD_KBP_LUTWR_LUT_DATA_START
#define ARAD_KBP_LUT_REC_TYPE_BIT_LEN         0x1
#define ARAD_KBP_LUTWR_REC_TYPE_BIT_START       ARAD_KBP_LUTWR_REC_SIZE_BIT_START + ARAD_KBP_LUT_REC_SIZE_BIT_LEN
#define ARAD_KBP_LUT_REC_IS_VALID_BIT_LEN     0x1
#define ARAD_KBP_LUTWR_REC_IS_VALID_BIT_START   ARAD_KBP_LUTWR_REC_TYPE_BIT_START + ARAD_KBP_LUT_REC_TYPE_BIT_LEN 

#define ARAD_KBP_LUTWR_REC_SIZE_BIT_START_OP     ARAD_KBP_CPU_DATA_REGISTER_BIT_LEN - 32
#define ARAD_KBP_LUTWR_REC_IS_VALID_BIT_START_OP ARAD_KBP_LUTWR_REC_SIZE_BIT_START_OP + ARAD_KBP_LUT_REC_SIZE_BIT_LEN
#define ARAD_KBP_LUTWR_MODE_BIT_START_OP     ARAD_KBP_LUTWR_REC_IS_VALID_BIT_START_OP + ARAD_KBP_LUT_REC_TYPE_BIT_LEN
#define ARAD_KBP_LUTRD_INSTR_BIT_START_OP        ARAD_KBP_LUTWR_MODE_BIT_START_OP + ARAD_KBP_LUT_REC_TYPE_BIT_LEN


#define ARAD_KBP_LUTRD_REC_SIZE_BIT_START       ((SOC_REG_ABOVE_64_INFO(unit, IHB_LOOKUP_REPLY_DATAr).size)*sizeof(uint32)*8 - 80)
#define ARAD_KBP_LUTRD_REC_TYPE_BIT_START       ARAD_KBP_LUTRD_REC_SIZE_BIT_START + ARAD_KBP_LUT_REC_SIZE_BIT_LEN
#define ARAD_KBP_LUTRD_REC_IS_VALID_BIT_START   ARAD_KBP_LUTRD_REC_TYPE_BIT_START + ARAD_KBP_LUT_REC_TYPE_BIT_LEN 


#define ARAD_KBP_LUT_MODE_BIT_LEN                0x3        
#define ARAD_KBP_LUTWR_MODE_BIT_START              ARAD_KBP_LUTWR_REC_IS_VALID_BIT_START + ARAD_KBP_LUT_REC_IS_VALID_BIT_LEN
#define ARAD_KBP_LUT_KEY_CONFIG_BIT_LEN          0x1
#define ARAD_KBP_LUTWR_KEY_CONFIG_BIT_START        ARAD_KBP_LUTWR_MODE_BIT_START + ARAD_KBP_LUT_MODE_BIT_LEN 
#define ARAD_KBP_LUT_LUT_KEY_DATA_BIT_LEN        0x8
#define ARAD_KBP_LUTWR_LUT_KEY_DATA_BIT_START      ARAD_KBP_LUTWR_KEY_CONFIG_BIT_START + ARAD_KBP_LUT_KEY_CONFIG_BIT_LEN 
#define ARAD_KBP_LUT_INSTR_BIT_LEN               0xa
#define ARAD_KBP_LUTWR_INSTR_BIT_START             ARAD_KBP_LUTWR_LUT_KEY_DATA_BIT_START + ARAD_KBP_LUT_LUT_KEY_DATA_BIT_LEN
#define ARAD_KBP_LUT_KEY_W_CPD_GT_80_BIT_LEN     0x1
#define ARAD_KBP_LUTWR_KEY_W_CPD_GT_80_BIT_START   ARAD_KBP_LUTWR_INSTR_BIT_START + ARAD_KBP_LUT_INSTR_BIT_LEN
#define ARAD_KBP_LUT_COPY_DATA_CFG_BIT_LEN       0x4
#define ARAD_KBP_LUTWR_COPY_DATA_CFG_BIT_START     ARAD_KBP_LUTWR_KEY_W_CPD_GT_80_BIT_START + ARAD_KBP_LUT_KEY_W_CPD_GT_80_BIT_LEN
       
#define ARAD_KBP_LUTRD_MODE_BIT_START              ARAD_KBP_LUTRD_REC_IS_VALID_BIT_START + ARAD_KBP_LUT_REC_IS_VALID_BIT_LEN
#define ARAD_KBP_LUTRD_KEY_CONFIG_BIT_START        ARAD_KBP_LUTRD_MODE_BIT_START + ARAD_KBP_LUT_MODE_BIT_LEN 
#define ARAD_KBP_LUTRD_LUT_KEY_DATA_BIT_START      ARAD_KBP_LUTRD_KEY_CONFIG_BIT_START + ARAD_KBP_LUT_KEY_CONFIG_BIT_LEN 
#define ARAD_KBP_LUTRD_INSTR_BIT_START             ARAD_KBP_LUTRD_LUT_KEY_DATA_BIT_START + ARAD_KBP_LUT_LUT_KEY_DATA_BIT_LEN
#define ARAD_KBP_LUTRD_KEY_W_CPD_GT_80_BIT_START   ARAD_KBP_LUTRD_INSTR_BIT_START + ARAD_KBP_LUT_INSTR_BIT_LEN
#define ARAD_KBP_LUTRD_COPY_DATA_CFG_BIT_START     ARAD_KBP_LUTRD_KEY_W_CPD_GT_80_BIT_START + ARAD_KBP_LUT_KEY_W_CPD_GT_80_BIT_LEN


#define ARAD_KBP_LUT_RESULT0_IDX_AD_CFG_BIT_LEN    0x4
#define ARAD_KBP_LUTWR_RESULT0_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTWR_COPY_DATA_CFG_BIT_START + ARAD_KBP_LUT_COPY_DATA_CFG_BIT_LEN
#define ARAD_KBP_LUT_RESULT0_IDX_OR_AD_BIT_LEN     0x1    
#define ARAD_KBP_LUTWR_RESULT0_IDX_OR_AD_BIT_START   ARAD_KBP_LUTWR_RESULT0_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT0_IDX_AD_CFG_BIT_LEN
#define ARAD_KBP_LUT_RESULT1_IDX_AD_CFG_BIT_LEN    0x4
#define ARAD_KBP_LUTWR_RESULT1_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTWR_RESULT0_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT0_IDX_OR_AD_BIT_LEN 
#define ARAD_KBP_LUT_RESULT1_IDX_OR_AD_BIT_LEN     0x1    
#define ARAD_KBP_LUTWR_RESULT1_IDX_OR_AD_BIT_START   ARAD_KBP_LUTWR_RESULT1_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT1_IDX_AD_CFG_BIT_LEN  
#define ARAD_KBP_LUT_RESULT2_IDX_AD_CFG_BIT_LEN    0x4
#define ARAD_KBP_LUTWR_RESULT2_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTWR_RESULT1_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT1_IDX_OR_AD_BIT_LEN
#define ARAD_KBP_LUT_RESULT2_IDX_OR_AD_BIT_LEN     0x1    
#define ARAD_KBP_LUTWR_RESULT2_IDX_OR_AD_BIT_START   ARAD_KBP_LUTWR_RESULT2_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT2_IDX_AD_CFG_BIT_LEN 
#define ARAD_KBP_LUT_RESULT3_IDX_AD_CFG_BIT_LEN    0x4
#define ARAD_KBP_LUTWR_RESULT3_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTWR_RESULT2_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT2_IDX_OR_AD_BIT_LEN
#define ARAD_KBP_LUT_RESULT3_IDX_OR_AD_BIT_LEN     0x1    
#define ARAD_KBP_LUTWR_RESULT3_IDX_OR_AD_BIT_START   ARAD_KBP_LUTWR_RESULT3_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT3_IDX_AD_CFG_BIT_LEN 
#define ARAD_KBP_LUT_RESULT4_IDX_AD_CFG_BIT_LEN    0x4
#define ARAD_KBP_LUTWR_RESULT4_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTWR_RESULT3_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT3_IDX_OR_AD_BIT_LEN
#define ARAD_KBP_LUT_RESULT4_IDX_OR_AD_BIT_LEN     0x1    
#define ARAD_KBP_LUTWR_RESULT4_IDX_OR_AD_BIT_START   ARAD_KBP_LUTWR_RESULT4_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT4_IDX_AD_CFG_BIT_LEN 
#define ARAD_KBP_LUT_RESULT5_IDX_AD_CFG_BIT_LEN    0x4
#define ARAD_KBP_LUTWR_RESULT5_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTWR_RESULT4_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT4_IDX_OR_AD_BIT_LEN
#define ARAD_KBP_LUT_RESULT5_IDX_OR_AD_BIT_LEN     0x1    
#define ARAD_KBP_LUTWR_RESULT5_IDX_OR_AD_BIT_START   ARAD_KBP_LUTWR_RESULT5_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT5_IDX_AD_CFG_BIT_LEN

#define ARAD_KBP_LUTRD_RESULT0_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTRD_COPY_DATA_CFG_BIT_START + ARAD_KBP_LUT_COPY_DATA_CFG_BIT_LEN  
#define ARAD_KBP_LUTRD_RESULT0_IDX_OR_AD_BIT_START   ARAD_KBP_LUTRD_RESULT0_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT0_IDX_AD_CFG_BIT_LEN
#define ARAD_KBP_LUTRD_RESULT1_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTRD_RESULT0_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT0_IDX_OR_AD_BIT_LEN    
#define ARAD_KBP_LUTRD_RESULT1_IDX_OR_AD_BIT_START   ARAD_KBP_LUTRD_RESULT1_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT1_IDX_AD_CFG_BIT_LEN  
#define ARAD_KBP_LUTRD_RESULT2_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTRD_RESULT1_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT1_IDX_OR_AD_BIT_LEN  
#define ARAD_KBP_LUTRD_RESULT2_IDX_OR_AD_BIT_START   ARAD_KBP_LUTRD_RESULT2_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT2_IDX_AD_CFG_BIT_LEN 
#define ARAD_KBP_LUTRD_RESULT3_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTRD_RESULT2_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT2_IDX_OR_AD_BIT_LEN    
#define ARAD_KBP_LUTRD_RESULT3_IDX_OR_AD_BIT_START   ARAD_KBP_LUTRD_RESULT3_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT3_IDX_AD_CFG_BIT_LEN 
#define ARAD_KBP_LUTRD_RESULT4_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTRD_RESULT3_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT3_IDX_OR_AD_BIT_LEN   
#define ARAD_KBP_LUTRD_RESULT4_IDX_OR_AD_BIT_START   ARAD_KBP_LUTRD_RESULT4_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT4_IDX_AD_CFG_BIT_LEN 
#define ARAD_KBP_LUTRD_RESULT5_IDX_AD_CFG_BIT_START  ARAD_KBP_LUTRD_RESULT4_IDX_OR_AD_BIT_START + ARAD_KBP_LUT_RESULT4_IDX_OR_AD_BIT_LEN   
#define ARAD_KBP_LUTRD_RESULT5_IDX_OR_AD_BIT_START   ARAD_KBP_LUTRD_RESULT5_IDX_AD_CFG_BIT_START + ARAD_KBP_LUT_RESULT5_IDX_AD_CFG_BIT_LEN





extern arad_kbp_frwd_ltr_db_t arad_kbp_frwd_ltr_db[ARAD_KBP_ROP_LTR_NUM_MAX];







void arad_kbp_format_long_integer(char *buf, uint32 *val, int nval)
{
    int i;

    for (i = nval - 1; i > 0; i--) {    
        if (val[i]) {
            break;
        }
    }

    if (i == 0 && val[i] < 10) {        
        sal_sprintf(buf, "%u ", val[i]);
    } else {
        sal_sprintf(buf, "0x%x ", val[i]);   
    }

    while (--i >= 0) {                  
        sal_sprintf(buf + strlen(buf), "%08x ", val[i]);
    }
}

void arad_kbp_array_print_debug(uint8 *data, int len)
{
    int i;

    for (i=0 ; i < len ; i++) {
       LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META("%02x"), *(data + i)));
        if ((i % 4) == 3) {
           LOG_VERBOSE(BSL_LS_SOC_TCAM,
                       (BSL_META(" ")));
        }
    }
   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META("\n")));
}

void arad_kbp_uint32_array_print_debug(uint32 *data, int len)
{
    int i;

    for (i = 0; i < len; i++) {
        LOG_VERBOSE(BSL_LS_SOC_TCAM, (BSL_META("%04x "), data[i]));
    }
    LOG_VERBOSE(BSL_LS_SOC_TCAM, (BSL_META("\n")));
}

#ifdef BE_HOST
STATIC void arad_kbp_buffer_endians_fix(int unit, soc_reg_above_64_val_t buffer)
{
    uint32 i;
    for(i=0 ; i< sizeof(soc_reg_above_64_val_t)/4; ++i) {
        buffer[i] = _shr_swap32(buffer[i]);
    }
}
#endif 

int soc_cmic_uc_msg_send_with_retry(int unit, int uC, mos_msg_data_t *msg,
                         sal_usecs_t timeout)
{
    int rv = SOC_E_NONE;
    int iter;

    for(iter = 0; iter < 5; iter++) {

        rv = soc_cmic_uc_msg_send(unit, uC, msg, 1000);
        if (rv == SOC_E_NONE) {
            break;
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                      "send failed, iter: %d, rv=%d\n"), iter, rv));
        }
    }

    return rv;
}

int soc_cmic_uc_msg_receive_with_retry(int unit, int uC, uint8 mclass,
                            mos_msg_data_t *msg, int timeout)
{
    int rv = SOC_E_NONE;
    int iter;

    for(iter = 0; iter < 2; iter++) {

        if (iter)
            _soc_cmic_uc_msg_process_status(unit, 0);

        rv = soc_cmic_uc_msg_receive(unit, uC, mclass, msg, timeout);

        if (rv == SOC_E_NONE) {
            break;
        } else {
              ;
        }
    }

    return rv;
}

uint32 arad_kbp_rop_dma_enable(int unit)
{
    
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_enable_dma =
                 !SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_enable_dma;
    return 0;
}

int arad_kbp_rop_dma_configure(int unit, int kbp_trans_len, int kbp_trans_zero_fill, int kbp_trans_change_endian)
{
    mos_msg_data_t send, rcv;
    int rv = SOC_E_NONE;
    uint8 *data_buffer, *buffer;
    uint32 data_buffer_len, *data_buf_print;
    int flag_rsp_received = 0;

    data_buffer_len = 200;
    data_buffer = soc_cm_salloc(unit, data_buffer_len, "DMA Status test buffer");
    if(data_buffer == NULL) {
       LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                          "No mem for allocating uC_KBP_DMA_TEST_BUFFER")));
        return (BCM_E_MEMORY);
    }
    data_buf_print = (uint32 *)data_buffer;
    printf("trying dma_configure, buff: 0x%x\n", *data_buf_print);

    buffer = data_buffer;

    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_len = kbp_trans_len;

    _SHR_PACK_U32(buffer, kbp_trans_len);
    _SHR_PACK_U32(buffer, kbp_trans_zero_fill);
    _SHR_PACK_U32(buffer, kbp_trans_change_endian);
    _SHR_PACK_U32(buffer, KBP_ROP_DMA_NUM_TRANS);

    sal_memset(&send, 0, sizeof(send));
    send.s.mclass = MOS_MSG_CLASS_KBP;
    send.s.subclass = MOS_MSG_SUBCLASS_KBP_CONFIG_REQ;
    send.s.len = bcm_htons(buffer - data_buffer);
    send.s.data = bcm_htonl(soc_cm_l2p(unit, data_buffer));

    soc_cm_sflush(unit, data_buffer, data_buffer_len);
    soc_cm_sinval(unit, data_buffer, data_buffer_len);

    if (soc_cmic_uc_msg_active_wait(unit, 0) != SOC_E_NONE) {
        
        printf("Unit: %d, is down\n", unit);
        soc_cm_sfree(unit, data_buffer);
        return (SOC_E_UNAVAIL);
    }
    
    rv = soc_cmic_uc_msg_send_with_retry(unit, 0, &send, 1000000);
    cli_out("dma_configure, configure msg sent\n");

    while(rv == SOC_E_NONE) {
        rv = soc_cmic_uc_msg_receive_with_retry(unit, 0, MOS_MSG_CLASS_KBP, &rcv, 500000);
        if (rv == SOC_E_NONE) {
            rcv.s.len  = bcm_ntohs(rcv.s.len);
            rcv.s.data = bcm_ntohl(rcv.s.data);

            if (rcv.s.subclass == MOS_MSG_SUBCLASS_KBP_CONFIG_RSP) {
                if (rcv.s.data == 1) {
                    cli_out("dma_configure, msg received with success, class: %d, subclass: %d, data %d\n", rcv.s.mclass, rcv.s.subclass, rcv.s.data);
                } else {
                    cli_out("dma_configure, msg received with fail, class: %d, subclass: %d, data %d\n", rcv.s.mclass, rcv.s.subclass, rcv.s.data);
                }
                flag_rsp_received = 1;
            } else {
                if (rcv.s.subclass == _MOS_MSG_SUBCLASS_KBP_ROP_RSP) {
                    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_ack_pending = 0;
                    cli_out("dma_configure, rop rsp received, class: %d, subclass: %d, len %d\n", rcv.s.mclass, rcv.s.subclass, rcv.s.len);
                } else {
                    cli_out("dma_configure, unknown msg received, class: %d, subclass: %d, len %d\n", rcv.s.mclass, rcv.s.subclass, rcv.s.len);
                }
            }
        }
    }

    soc_cm_sfree(unit, data_buffer);
    if(flag_rsp_received)
        return SOC_E_NONE;
    else
        return SOC_E_FAIL;
}

int arad_kbp_rop_dma_test(int unit, int debug_print)
{
    mos_msg_data_t send, rcv;
    int rv = SOC_E_NONE;
    uint8 *data_buffer, *buffer;
    uint32 data_buffer_len;
    int flag_rsp_received = 0;

    uint32 kbp_dma_total_trans_count = 0;
    uint32 kbp_dma_msg_count = 0;
    uint32 kbp_dma_trans_retry_count = 0;
    uint32 kbp_dma_trans_fail_count = 0;
    uint32 kbp_dma_cur_msg_trans_count = 0;
    uint32 kbp_dma_allocated_buffer_size = 0;
    uint32 kbp_trans_len = 0;
    uint32 kbp_trans_change_endian = 2;
    uint32 kbp_trans_zero_fill = 2;

    if (debug_print) {
        data_buffer_len = 2000;
    } else {
        data_buffer_len = 200;
    }

    data_buffer = soc_cm_salloc(unit, data_buffer_len, "DMA Status test buffer");
    if(data_buffer == NULL) {
       LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                          "No mem for allocating uC_KBP_DMA_TEST_BUFFER")));
        return (BCM_E_MEMORY);
    }
    soc_cm_sinval(unit, data_buffer, data_buffer_len);

    cli_out("trying dma_test_appl, buff: 0x%p\n", data_buffer);

    sal_memset(&send, 0, sizeof(send));
    send.s.mclass = MOS_MSG_CLASS_KBP;
    send.s.subclass = MOS_MSG_SUBCLASS_KBP_TEST_REQ;
    send.s.len = bcm_htons(debug_print);
    send.s.data = bcm_htonl(soc_cm_l2p(unit, data_buffer));

    if (soc_cmic_uc_msg_active_wait(unit, 0) != SOC_E_NONE) {
        
        cli_out("Unit: %d, is down\n", unit);
        soc_cm_sfree(unit, data_buffer);
        return (SOC_E_UNAVAIL);
    }
    
    rv = soc_cmic_uc_msg_send_with_retry(unit, 0, &send, 1000000);
    cli_out("dma_test_appl, test msg sent\n");

    while(rv == SOC_E_NONE) {
        rv = soc_cmic_uc_msg_receive_with_retry(unit, 0, MOS_MSG_CLASS_KBP, &rcv, 500000);
        if (rv == SOC_E_NONE) {
            rcv.s.len  = bcm_ntohs(rcv.s.len);
            rcv.s.data = bcm_ntohl(rcv.s.data);

            if (rcv.s.subclass == MOS_MSG_SUBCLASS_KBP_TEST_RSP) {
                flag_rsp_received = 1;
                cli_out("dma_test_appl, msg receive success, class: %d, subclass: %d, len %d\n", rcv.s.mclass, rcv.s.subclass, rcv.s.len);

                buffer = data_buffer;
                _SHR_UNPACK_U32(buffer, kbp_dma_total_trans_count);
                _SHR_UNPACK_U32(buffer, kbp_dma_msg_count);
                _SHR_UNPACK_U32(buffer, kbp_dma_trans_retry_count);
                _SHR_UNPACK_U32(buffer, kbp_dma_trans_fail_count);
                _SHR_UNPACK_U32(buffer, kbp_dma_cur_msg_trans_count);
                _SHR_UNPACK_U32(buffer, kbp_dma_allocated_buffer_size);
                _SHR_UNPACK_U32(buffer, kbp_trans_len);
                _SHR_UNPACK_U32(buffer, kbp_trans_zero_fill);
                _SHR_UNPACK_U32(buffer, kbp_trans_change_endian);
                cli_out("kbp_dma_total_trans_count: %d, kbp_dma_msg_count: %d, kbp_dma_trans_retry_count: %d\n", kbp_dma_total_trans_count, kbp_dma_msg_count, kbp_dma_trans_retry_count);
                cli_out("kbp_dma_trans_fail_count: %d, kbp_dma_cur_msg_trans_count: %d, kbp_dma_allocated_buffer_size: %d\n", kbp_dma_trans_fail_count, kbp_dma_cur_msg_trans_count, kbp_dma_allocated_buffer_size);
                cli_out("kbp_trans_len: %d, kbp_trans_zero_fill: %d, kbp_trans_change_endian: %d\n", kbp_trans_len, kbp_trans_zero_fill, kbp_trans_change_endian);
                if (rcv.s.len > buffer - data_buffer) {
                    cli_out("******\n%s\n******\n", buffer);
                }
            } else {
                if (rcv.s.subclass == _MOS_MSG_SUBCLASS_KBP_ROP_RSP) {
                    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_ack_pending = 0;
                    cli_out("dma_test_appl, rop msg received, class: %d, subclass: %d, len %d\n", rcv.s.mclass, rcv.s.subclass, rcv.s.len);
                } else {
                    cli_out("dma_test_appl, unknown msg received, class: %d, subclass: %d, len %d\n", rcv.s.mclass, rcv.s.subclass, rcv.s.len);
                }
            }
        }
    }

    soc_cm_sfree(unit, data_buffer);

    if(flag_rsp_received)
        return SOC_E_NONE;
    else
        return SOC_E_FAIL;
}

uint32 arad_kbp_rop_dma_init(int unit)
{
    int rv = SOC_E_NONE;
    if( SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[0] == NULL) {
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[0] =
                     soc_cm_salloc(unit, KBP_ROP_DMA_NUM_TRANS * 72,
                                                                                   "uC_KBP_DMA_BUFFER_0");
        printf("Allocating uC_KBP_DMA_BUFFER_0: 0x%p\n",
         SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[0]);
        if(SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[0] == NULL) {
           LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                "No mem for allocating uC_KBP_DMA_BUFFER_0")));
            return (BCM_E_MEMORY);
        }
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[1] =
                      soc_cm_salloc(unit, KBP_ROP_DMA_NUM_TRANS * 72,
                                    "uC_KBP_DMA_BUFFER_1");
        printf("Allocating uC_KBP_DMA_BUFFER_1: 0x%p\n",
         SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[1]);
        if(SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[1] == NULL) {
           LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                              "No mem for allocating uC_KBP_DMA_BUFFER_1")));
            return (BCM_E_MEMORY);
        }

        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_enable_dma = 0;
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_len = 32;
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_total_trans_count = 0;
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_msg_count = 0;
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buf_idx_in_use = 0;
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_gathered_trans_count = 0;
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_ack_pending = 0;
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_sent_in_lst_msg = 0;


        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer_curr =
             SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[
              SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buf_idx_in_use];
    } else {
        printf("DMA buffers already allocated: 0x%p, 0x%p\n",
          SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[0],
          SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[1]);
    }

    rv = soc_cmic_uc_appl_init(unit, 0, MOS_MSG_CLASS_KBP,
                                   10000000,
                                   0,
                                   0,
                                   NULL, NULL);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
        "Error in %s(): soc_cmic_uc_appl_init failed, rv = %d\n"),
        FUNCTION_NAME(), rv));
    } else {
        arad_kbp_rop_dma_configure(unit, SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_len, 0, 0);
        arad_kbp_rop_dma_enable(unit);
    }

    return rv;
}

uint32 arad_kbp_rop_send_dma_req(int unit)
{
    mos_msg_data_t send;
    int data_len;
    uint8 *data_buffer;
    int32 dma_trans_len = SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_len;
    int rv = SOC_E_NONE;

    data_len =
         SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_gathered_trans_count;
    data_buffer = SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[
               SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buf_idx_in_use];

    sal_memset(&send, 0, sizeof(send));
    send.s.mclass = MOS_MSG_CLASS_KBP;
    send.s.subclass = MOS_MSG_SUBCLASS_KBP_ROP_REQ;
    send.s.len = bcm_htons(data_len);
    send.s.data = bcm_htonl(soc_cm_l2p(unit, data_buffer));

    soc_cm_sflush(unit, data_buffer, data_len*dma_trans_len);
    soc_cm_sinval(unit, data_buffer, data_len*dma_trans_len);

    if (soc_cmic_uc_msg_active_wait(unit, 0) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
      "Error in %s(): soc_cmic_uc_msg_active_wait failed\n"),FUNCTION_NAME()));
        
        return (SOC_E_UNAVAIL);
    }

    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_ack_pending = 1;
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_sent_in_lst_msg =
         SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_gathered_trans_count;
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_msg_count++;
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buf_idx_in_use =
            !SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buf_idx_in_use;
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer_curr =
        SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[
             SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buf_idx_in_use];
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_gathered_trans_count = 0;

    rv = soc_cmic_uc_msg_send_with_retry(unit, 0, &send, 1000000);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
         "Error in %s(): soc_cmic_uc_msg_send_with_retry failed, rv = %d\n"),
         FUNCTION_NAME(), rv));
    }

    return rv;
}

uint32 arad_kbp_rop_receive_dma_rsp(int unit)
{
    int rv = SOC_E_NONE;
    mos_msg_data_t rcv;
    uint32 kbp_trans_acked_in_lst_msg = 0;
    uint8 *data_buffer, *buffer;
    uint32 kbp_dma_rop_error, kbp_dma_elk_error, kbp_dma_reply_valid;

    rv = soc_cmic_uc_msg_receive_with_retry(unit, 0, MOS_MSG_CLASS_KBP, &rcv, 20000000);
    if (rv == SOC_E_NONE) {

        if (rcv.s.subclass != _MOS_MSG_SUBCLASS_KBP_ROP_RSP) {
            LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
               "Rx'ed resp 0x%x from ARM is not ROP_RSP msg\n"),
                rcv.s.subclass));
            return SOC_E_FAIL;
        }

        kbp_trans_acked_in_lst_msg = bcm_ntohl(rcv.s.data);
        rcv.s.len  = bcm_ntohs(rcv.s.len);

        if (SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_sent_in_lst_msg != kbp_trans_acked_in_lst_msg) {

            assert(rcv.s.len == 12);
            data_buffer = SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer[
                       !(SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buf_idx_in_use)];

            buffer = data_buffer;
            _SHR_UNPACK_U32(buffer, kbp_dma_reply_valid);
            _SHR_UNPACK_U32(buffer, kbp_dma_rop_error);
            _SHR_UNPACK_U32(buffer, kbp_dma_elk_error);

            LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
              "Error in %s(): kbp_rop_dma_trans_sent_in_lst_msg: %d, kbp_trans_acked_in_lst_msg: %d, rop_reply_valid: %d, rop_error: %d, elk_error: %d\n"),
                FUNCTION_NAME(),
                SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_sent_in_lst_msg,kbp_trans_acked_in_lst_msg,
                kbp_dma_reply_valid, kbp_dma_rop_error, kbp_dma_elk_error));

            return SOC_E_FAIL;
        } else {
            assert(rcv.s.len == 0);
            SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_ack_pending = 0;
        }

    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                  "Error in %s(): rv=%d\n"),FUNCTION_NAME(), rv));
    }

    return rv;
}

int arad_kbp_rop_dma_sync(int unit)
{
    int rv = SOC_E_NONE;

    if (!SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_enable_dma) {
        return rv;
    }

    if (SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_ack_pending) {
        if (arad_kbp_rop_receive_dma_rsp(unit) != SOC_E_NONE)
            return SOC_E_FAIL;
    }
    if (SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_gathered_trans_count) {
        rv = arad_kbp_rop_send_dma_req(unit);
        if (rv != SOC_E_NONE) {
            goto err;
        }

        if (arad_kbp_rop_receive_dma_rsp(unit) != SOC_E_NONE) {
            rv = SOC_E_FAIL;
            goto err;
        }
    }

err:
    if (rv) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,"Failure in %s : error %d\n"),
                 FUNCTION_NAME(), rv));
    }

    return rv;
}

uint32 jericho_kbp_optimized_cpu_record_dma(int unit, uint32 core,
              uint32 opcode, soc_reg_above_64_val_t msb_data,
              soc_reg_above_64_val_t lsb_data, int lsb_enable,
              soc_reg_above_64_val_t read_data)
{
    int32 rv = SOC_E_NONE;
    uint8_t *msb_data_p = (uint8_t *)msb_data;
    int32 dma_trans_len = SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_trans_len;

    _SHR_PACK_U32(SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer_curr, core);
    _SHR_PACK_U32(SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer_curr, opcode);
#if defined(BE_HOST)
    arad_kbp_buffer_endians_fix(unit, msb_data);
#endif
    sal_memcpy(SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer_curr,
               msb_data_p + (64 - (dma_trans_len - 8)), (dma_trans_len - 8));
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_buffer_curr += (dma_trans_len - 8);
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_gathered_trans_count++;
    SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_total_trans_count++;

    if (SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_gathered_trans_count == KBP_ROP_DMA_NUM_TRANS) {

        if (SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_ack_pending) {
            if (arad_kbp_rop_receive_dma_rsp(unit) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                    "Failed in rop_receive_dma_rsp in %s"),
                     FUNCTION_NAME()));
                return SOC_E_FAIL;
            }
        }

        rv = arad_kbp_rop_send_dma_req(unit);
        if (rv != SOC_E_NONE) {
            goto error;
        }
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
            "Error in %s(): rv: %d, kbp_rop_dma_total_trans_count: %d, kbp_rop_dma_msg_count: %d\n"),
            FUNCTION_NAME(), rv,
            SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_total_trans_count,
            SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_dma_msg_count));
    }

error:
    return rv;
}


int
soc_dnx_schan_op_lite(int unit,
              schan_msg_t *msg,
              int dwc_write,
              int dwc_read)
{
    int i, rv = SOC_E_NONE;
    int                 cmc = SOC_PCI_CMC(unit);
    soc_timeout_t         to;
    uint32                 schanCtrl;

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    soc_schan_dump(unit, msg, dwc_write);    
#endif

    for (i = 0; i < dwc_write; i++) {
            soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i), msg->dwords[i]);
        }

    soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc),SC_CMCx_MSG_START);

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout, 100);

    while (((schanCtrl = soc_pci_read(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc))) & SC_CMCx_MSG_DONE) == 0) {
        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            break;
        }
    }

    if (rv == SOC_E_TIMEOUT) {        
#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "SchanTimeOut:soc_schan_op operation timed out\n")));
            soc_schan_dump(unit, msg, dwc_write);
#endif
            
    }


    soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc), schanCtrl & ~SC_CMCx_MSG_DONE);

                
    for (i = 0; i < dwc_read; i++) {
        msg->dwords[i] = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i));
    }

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    soc_schan_dump(unit, msg, dwc_read);
#endif
    
    return rv;
}

#ifdef JER_KBP_ROP_OPTIMIZATION
int
soc_jer_schan_op_lite(int unit,
                      int core,
                      int action,
                      schan_msg_t *msg,
                      int dwc_write,
                      int dwc_read)
{
    int i, rv = SOC_E_NONE;
    int                 cmc = SOC_PCI_CMC(unit);
    soc_timeout_t         to;
    uint32                 schanCtrl;

#ifdef ARAD_KBP_DISABLE_IHB_LOOKUP_REPLY_FOR_ROP_TRANSMIT
    static schan_msg_t schan_shadow;
#endif

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    soc_schan_dump(unit, msg, dwc_write);    
#endif
    switch (action) {
    case 0:
        soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 0), KBP_SCHAN_JER_OPCODE_32_IHB_WRITE);
        soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 1), KBP_SCHAN_JER_ADDR_ROP_OPCODE_ADDR);
        soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 2), msg->dwords[2]);
        break;
    case 1:
        soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 0), KBP_SCHAN_JER_OPCODE_64_IHB_WRITE);
        soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 1), KBP_SCHAN_JER_ADDR_IHB_MSB_WRITE_ADDR);
        for (i = 2; i < 18; i++) {
#ifdef ARAD_KBP_DISABLE_IHB_LOOKUP_REPLY_FOR_ROP_TRANSMIT
            if (schan_shadow.dwords[i] != msg->dwords[i - 2]) {
#endif
                soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i), msg->dwords[i - 2]);
#ifdef ARAD_KBP_DISABLE_IHB_LOOKUP_REPLY_FOR_ROP_TRANSMIT
                schan_shadow.dwords[i] = msg->dwords[i - 2];
            }
#endif
        }
        break;
    case 3:
        soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 0), KBP_SCHAN_JER_OPCODE_IHB_ROP_READ);
        soc_pci_write(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 1), KBP_SCHAN_JER_ADDR_ROP_REPLY_ADDR);
        break;
    }

    soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc),SC_CMCx_MSG_START);

    soc_timeout_init(&to, SOC_CONTROL(unit)->schanTimeout, 100);

    while (((schanCtrl = soc_pci_read(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc))) & SC_CMCx_MSG_DONE) == 0) {
        if (soc_timeout_check(&to)) {
            rv = SOC_E_TIMEOUT;
            break;
        }
    }

    if (rv == SOC_E_TIMEOUT) {        
#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "SchanTimeOut:soc_schan_op operation timed out\n")));
            soc_schan_dump(unit, msg, dwc_write);
#endif
            
    }

    
    soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc), schanCtrl & ~SC_CMCx_MSG_DONE);

    if (dwc_read) {
        msg->dwords[1] = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 1));
        msg->dwords[10] = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, 10));
    }

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    for (i = 0; i < dwc_read; i++) {
        msg->dwords[i] = soc_pci_read(unit, CMIC_CMCx_SCHAN_MESSAGEn(cmc, i));
    }

    soc_schan_dump(unit, msg, dwc_read);
#endif
    
    return rv;
}
#endif 


uint32 arad_kbp_cpu_lookup_reply(
    int unit,
    uint32 core,
    soc_reg_above_64_val_t data)
 {
     uint32
         res;
#ifdef ARAD_KBP_ROP_DEBUG_PRINTS         
     char
         rval_str[256];
#endif 

     SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
     if (SOC_IS_JERICHO(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_get(unit, IHB_LOOKUP_REPLYr, core, 0, data));
     } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_above_64_get(unit, IHB_LOOKUP_REPLY_DATAr, core, 0, data));
     }

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
     if (SOC_IS_JERICHO(unit)) {
        arad_kbp_format_long_integer(rval_str, data, SOC_REG_ABOVE_64_INFO(unit, IHB_LOOKUP_REPLYr).size);
     } else {
        arad_kbp_format_long_integer(rval_str, data, SOC_REG_ABOVE_64_INFO(unit, IHB_LOOKUP_REPLY_DATAr).size);
     }
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): str   data=%s\n"), FUNCTION_NAME(), rval_str));
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): array data="), FUNCTION_NAME()));
    arad_kbp_array_print_debug((uint8 *)data, sizeof(soc_reg_above_64_val_t));
#endif

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_cpu_lookup_reply()", 0, 0);
}

uint32 arad_kbp_cpu_record_send(
    int unit,
    uint32 opcode,
    soc_reg_above_64_val_t msb_data,
    soc_reg_above_64_val_t lsb_data,
    int lsb_enable)
{
    uint32
        res,
        reg_val = 0x0, 
        field_val;
    soc_timeout_t 
        to;          
#ifdef ARAD_KBP_ROP_DEBUG_PRINTS        
    char                
        rval_str[256];
#endif

 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_set(unit, IHB_CPU_RECORD_OPCODEr, REG_PORT_ANY, 0, opcode));

    

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_set(unit, IHB_CPU_RECORD_DATA_MSBr, REG_PORT_ANY, 0, msb_data)); 

    if (lsb_enable == 0x1) {
         
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg_above_64_set(unit, IHB_CPU_RECORD_DATA_LSBr, REG_PORT_ANY, 0, lsb_data));
    }

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "%s(): opcode=0x%x, "), FUNCTION_NAME(), opcode));
    arad_kbp_format_long_integer(rval_str, msb_data, SOC_REG_ABOVE_64_INFO(unit, IHB_CPU_RECORD_DATA_MSBr).size);
    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "%s(): string: msb_data=%s\n"),FUNCTION_NAME(), rval_str));
    LOG_INFO(BSL_LS_SOC_TCAM,
             (BSL_META_U(unit,
                         "%s(): byte:   msb_data= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(((uint8 *)msb_data), sizeof(soc_reg_above_64_val_t));
    if (lsb_enable == 0x1) {
        arad_kbp_format_long_integer(rval_str, lsb_data, SOC_REG_ABOVE_64_INFO(unit, IHB_CPU_RECORD_DATA_LSBr).size);
         LOG_INFO(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "lsb_data=%s\n"), rval_str));
    }
#endif
        
    

    
    soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val, CPU_RECORD_TRIGGERf, 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, REG_PORT_ANY, 0, reg_val));

    
    
    soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);
    for(;;){
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, soc_reg32_get(unit, IHB_LOOKUP_REPLYr, REG_PORT_ANY, 0, &reg_val));
        field_val = soc_reg_field_get(unit, IHB_LOOKUP_REPLYr, reg_val, LOOKUP_REPLY_VALIDf);
        
        if(field_val == 0x1) {
            break;
        }

        if (soc_timeout_check(&to)) {
          LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "Error in %s(): IHB_LOOKUP_REPLY register LOOKUP_REPLY_VALID field not asserted correctly."), FUNCTION_NAME()));
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
          break;
        }
    }

    field_val = soc_reg_field_get(unit, IHB_LOOKUP_REPLYr, reg_val, LOOKUP_REPLY_ERRORf);
    if (field_val == 0x1) {
            
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Error in %s(): register IHB_LOOKUP_REPLY field LOOKUP_REPLY_ERROR=1 (excpected 0)\n"), FUNCTION_NAME()));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
    }
     
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_cpu_record_send()", opcode, reg_val);
}


#ifdef ARAD_KBP_ROP_OPTIMIZATION
uint32 aradplus_kbp_optimized_cpu_record_send(int unit, uint32 opcode, soc_reg_above_64_val_t msb_data, soc_reg_above_64_val_t lsb_data, int lsb_enable, soc_reg_above_64_val_t read_data)
{
    int                     rv= SOC_E_NONE, i;
    schan_msg_t             msg;
    uint32                    reg_val = 0;
    soc_reg_above_64_val_t    data,error_field;
    uint32                    rop_error, elk_error;
    int                     read_data_size = 0, write_data_size = 0;

#ifndef ARAD_KBP_DISABLE_IHB_LOOKUP_REPLY_FOR_ROP_TRANSMIT
    soc_timeout_t           to;
#endif     

    SCHAN_LOCK(unit);


    if(SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[0] != opcode)
    {
        msg.dwords[0] = KBP_SCHAN_OPCODE_32_IHB_WRITE;
        msg.dwords[1] = KBP_SCHAN_ADDR_ROP_OPCODE_ADDR;
        msg.dwords[2] = opcode;

        write_data_size = 3;
        soc_dnx_schan_op_lite(unit, &msg, write_data_size , read_data_size);
        SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[0] = opcode;
    }    


    if (lsb_enable == 0) {
        if (SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[0] == 0) {
            
            reg_val = 0x0;
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_MSB_TRIGGERf, 1);
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_LSB_TRIGGERf, 0);
            rv = soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, REG_PORT_ANY, 0, reg_val);
            if (rv != 0 ) {
                SCHAN_UNLOCK(unit);
                return -1;
            }
            SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[0] = 1;
        }
    } else {
        if (SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[0] == 1) {
            
            reg_val = 0x0;
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_MSB_TRIGGERf, 0);
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_LSB_TRIGGERf, 1);
            rv = soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, REG_PORT_ANY, 0, reg_val);
            if (rv != 0 ) {
                SCHAN_UNLOCK(unit);
                return -1;
            }
            SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[0] = 0;
        }
    }


    msg.dwords[0] = KBP_SCHAN_OPCODE_64_IHB_WRITE;
    msg.dwords[1] = KBP_SCHAN_ADDR_IHB_MSB_WRITE_ADDR;
    for (i = 0; i < KBP_ROP_DATA_SIZE; i++) {
        msg.dwords[i+2] = msb_data[i];
    }

    write_data_size = 18;
    soc_dnx_schan_op_lite(unit, &msg, write_data_size, read_data_size);


    
    if (lsb_enable == 0x1) {
        schan_msg_clear(&msg);
        msg.dwords[0] = KBP_SCHAN_OPCODE_64_IHB_WRITE;
        msg.dwords[1] = KBP_SCHAN_ADDR_IHB_LSB_WRITE_ADDR;
        for (i = 0; i < KBP_ROP_DATA_SIZE; i++) {
            msg.dwords[i+2] = msb_data[i];
        }
        
        soc_dnx_schan_op_lite(unit, &msg, write_data_size, read_data_size);
    }


    
        schan_msg_clear(&msg);

        msg.dwords[0] = KBP_SCHAN_OPCODE_IHB_ROP_READ;
        msg.dwords[1] = KBP_SCHAN_ADDR_ROP_REPLY_ADDR;

        read_data_size = 11;
        write_data_size = 2;
        soc_dnx_schan_op_lite(unit, &msg, write_data_size, read_data_size);

        for(i = 0; i < 10; i++) {
            data[i] = msg.readresp.data[i];		
        }

        SHR_BITCOPY_RANGE(error_field, 0, data, 288, 1);
        rop_error = SHR_BITGET(error_field,0);

        SHR_BITCOPY_RANGE(error_field, 0, data, 289, 1);
        elk_error = SHR_BITGET(error_field,0);

        if ((rop_error == 0x1) || (elk_error == 0x1)) {
                
            SCHAN_UNLOCK(unit);
        LOG_ERROR(BSL_LS_SOC_TCAM, (BSL_META_U(unit,
                                  "Error in %s(): register IHB_LOOKUP_REPLY_DATA: LOOKUP_REPLY_ROP_ERROR=%d, LOOKUP_REPLY_ELK_ERROR=%d (excpected 0 on both)\n"), 
                                  FUNCTION_NAME() ,rop_error, elk_error));
            return -1;
        }


#ifndef ARAD_KBP_DISABLE_IHB_LOOKUP_REPLY_FOR_ROP_TRANSMIT
    schan_msg_clear(&msg);

    msg.dwords[0] = KBP_SCHAN_OPCODE_32_IHB_READ;
    msg.dwords[1] = KBP_SCHAN_ADDR_IHB_REPLY_ADDR;

    read_data_size = 2;
    write_data_size = 2;

    soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);

    for(;;){
        soc_dnx_schan_op_lite(unit, &msg, write_data_size, read_data_size);

        reg_val = msg.readresp.data[0];
        reg_val = reg_val & (1);

        if(reg_val == 0x1) {
            break;
        }

        if (soc_timeout_check(&to)) {
              LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                    "Error in %s(): IHB_CPU_LOOKUP_REPLY register LOOKUP_REPLY_VALID field not asserted correctly."),
             FUNCTION_NAME()));
              SCHAN_UNLOCK(unit);
              return -1;
        }
    }
#endif 

    SCHAN_UNLOCK(unit);

    return rv;
}

#endif 
#ifdef JER_KBP_ROP_OPTIMIZATION

uint32 jericho_kbp_optimized_cpu_record_send_normal(
     int unit, uint32 core, uint32 opcode, soc_reg_above_64_val_t msb_data,
     soc_reg_above_64_val_t lsb_data, int lsb_enable,
     soc_reg_above_64_val_t read_data)
{
    int                     rv= SOC_E_NONE;
    schan_msg_t             msg;
    uint32                    reg_val = 0;
    soc_reg_above_64_val_t    error_field;
    uint32                    rop_error, elk_error;
    int                     read_data_size = 0, write_data_size = 0, retransmit, retransmission_num = KBP_ERROR_RETRANMISSION;
    int i;

    SCHAN_LOCK(unit);

    do {   
        retransmit = FALSE;
        
        if(((core != 2 ) && (SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[core] != opcode)) ||
            ((core == 2 ) &&  ((SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[0] != opcode) ||
            (SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[1] != opcode))))
        {
            msg.dwords[2] = opcode;
            soc_jer_schan_op_lite(unit, core, 0, &msg, write_data_size , read_data_size);
            if (core == 2) {
                SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[0] = opcode;
                SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[1] = opcode;
            } else {
                SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[core] = opcode;
            }
        }

        
        if (((core != 2 ) && (SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[core] == 0)) ||
            ((core == 2 ) && ((SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[0] == 0) ||
            (SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[1] == 0))))
        {
            
            reg_val = 0x0;
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_MSB_TRIGGERf, 1);
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_LSB_TRIGGERf, 0);
            rv = soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, REG_PORT_ANY, 0, reg_val);
            if (rv != 0 ) {
                SCHAN_UNLOCK(unit);
                return -1;
            }
            if (core == 2) {
                SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[0] = 1;
                SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[1] = 1;
            } else {
                SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[core] = 1;
            }
        }

        
        soc_jer_schan_op_lite(unit, core, 1, (schan_msg_t*)msb_data, write_data_size, read_data_size);

        if (SOC_IS_JERICHO_A0(unit) || SOC_IS_QMX_A0(unit) || SOC_IS_QAX(unit)) {
            if (core == 2) {
                soc_reg32_get(unit, IHB_CPU_RECORD_CONTROLr, 0, 0, &reg_val);
                soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_TRIGGERf, 1);
                soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, 0, 0, reg_val);

                soc_reg32_get(unit, IHB_CPU_RECORD_CONTROLr, 1, 0, &reg_val);
                soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_TRIGGERf, 1);
                soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, 1, 0, reg_val);
            } else {
                soc_reg32_get(unit, IHB_CPU_RECORD_CONTROLr, core, 0, &reg_val);
                soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_TRIGGERf, 1);
                soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, core, 0, reg_val);
            }
       }

        
        read_data_size = 11;

        SOC_DPP_CORES_ITER(SOC_CORE_ALL, i) {
            if (retransmit == TRUE) {
                continue;
            }
            if ((i != core) && (core != 2)) {
                continue;
            }
            soc_jer_schan_op_lite(unit, i, 3, &msg, write_data_size, read_data_size); 

            SHR_BITCOPY_RANGE(error_field, 0, msg.readresp.data, 289, 1);
            rop_error = SHR_BITGET(error_field,0);

            SHR_BITCOPY_RANGE(error_field, 0, msg.readresp.data, 290, 1);
            elk_error = SHR_BITGET(error_field,0);

            
            if ((rop_error == 0x1) || (elk_error == 0x1)) {
                LOG_ERROR(BSL_LS_SOC_TCAM, (BSL_META_U(unit,
                                          "Error in %s(): register IHB_LOOKUP_REPLY_DATA: LOOKUP_REPLY_ROP_ERROR=%d, LOOKUP_REPLY_ELK_ERROR=%d (excpected 0 on both)\n"), 
                                          FUNCTION_NAME() ,rop_error, elk_error));
                retransmit = TRUE;
                retransmission_num--;
                continue;
            }

            
            SHR_BITCOPY_RANGE(error_field, 0, msg.readresp.data, 0, 1);
            reg_val = SHR_BITGET(error_field,0);
            if(reg_val == 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,"Error in %s(): IHB_CPU_LOOKUP_REPLY register LOOKUP_REPLY_VALID field not asserted correctly."),FUNCTION_NAME()));
                retransmit = TRUE;
                retransmission_num--;
            }
        }
    } while (retransmit && retransmission_num);

    SCHAN_UNLOCK(unit);

    if (retransmission_num == 0) {
        rv = SOC_E_INTERNAL;
    }

    return rv;
}

uint32 jericho_kbp_optimized_cpu_record_send(int unit, uint32 core, uint32 opcode, soc_reg_above_64_val_t msb_data, soc_reg_above_64_val_t lsb_data, int lsb_enable, soc_reg_above_64_val_t read_data)
{
    uint32 rv;

    if ((SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_rop_enable_dma == 0) || ARAD_KBP_IS_OP_OR_OP2) {
        rv = jericho_kbp_optimized_cpu_record_send_normal(
               unit, core, opcode, msb_data, lsb_data, lsb_enable, read_data);
    } else {
        rv = jericho_kbp_optimized_cpu_record_dma(unit,
               core, opcode, msb_data, lsb_data, lsb_enable, read_data);
    }

    return rv;
}

#endif 

#ifdef BCM_88660
uint32 aradplus_kbp_cpu_record_send(
    int unit,
    uint32 core,
    uint32 opcode,
    soc_reg_above_64_val_t msb_data,
    soc_reg_above_64_val_t lsb_data,
    int lsb_enable,
    soc_reg_above_64_val_t read_data)
{
    
    int rv;
    uint32 res, reg_val = 0x0;
#ifndef PLISIM
    int retransmission_num = KBP_ERROR_RETRANMISSION;
#endif  
#ifndef PLISIM
    soc_reg_above_64_val_t data, error_field;
    uint32 rop_error, elk_error, field_val;
    soc_timeout_t to;     
#endif

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS       
    char    rval_str[256];        
#endif
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    rv = arad_kbp_rop_dma_sync(unit);
    if (rv != SOC_E_NONE) {
        return rv;
    }

#ifndef PLISIM
retransmit:
#endif
#ifdef ARAD_KBP_DISABLE_IHB_LOOKUP_REPLY_FOR_ROP_TRANSMIT
    if (read_data != NULL){
        field_val = 1;
        soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);
        
        for(;;) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 130, exit, soc_reg32_get(unit, IHB_LOOKUP_REPLYr, core, 0, &reg_val));
            field_val = soc_reg_field_get(unit, IHB_LOOKUP_REPLYr, reg_val, LOOKUP_REPLY_VALIDf);
            if (field_val == 0) {
                break;
            }
            if (soc_timeout_check(&to)) {
              LOG_ERROR(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Error in %s(): LOOKUP_REPLY_VALID field not asserted correctly."),
                         FUNCTION_NAME()));
              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
              break;
            }
        }
    }    
#endif
    if (SOC_IS_JERICHO(unit)) {
#ifdef  JER_KBP_ROP_OPTIMIZATION

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg32_get(unit, IHB_CPU_LOOKUP_REPLY_COUNTERr, core, 0, &reg_val));        
#endif
    } else {
#ifdef  ARAD_KBP_ROP_OPTIMIZATION

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg32_get(unit, IHB_CPU_LOOKUP_REPLY_COUNTERr, core, 0, &reg_val));        
#endif
    }

    
    if (SOC_IS_JERICHO(unit)) {
        
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_set(unit, IHB_CPU_RECORD_OPCODEr, core, 0, opcode));
            SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[core] = opcode;
        
    } else {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_set(unit, IHB_CPU_RECORD_OPCODEr, core, 0, opcode));
        SOC_DPP_CONFIG(unit)->arad->init.elk.last_opcode_set[core] = opcode;
        
    }
   
    if (lsb_enable == 0) {
        
            
            reg_val = 0x0;
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_MSB_TRIGGERf, 1);
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_LSB_TRIGGERf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, core, 0, reg_val));
            SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[core] = 1;
        
    } else {
        
            
            reg_val = 0x0;
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_MSB_TRIGGERf, 0);
            soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_DATA_LSB_TRIGGERf, 1);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, core, 0, reg_val));
            SOC_DPP_CONFIG(unit)->arad->init.elk.msb_trigger_on[core] = 0;
        
    }
    reg_val = 0x0;
    
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg_above_64_set(unit, IHB_CPU_RECORD_DATA_MSBr, core, 0, msb_data)); 
    
    if (lsb_enable == 0x1) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg_above_64_set(unit, IHB_CPU_RECORD_DATA_LSBr, core, 0, lsb_data));
    }

    if (SOC_IS_JERICHO_A0(unit) || SOC_IS_QMX_A0(unit) || SOC_IS_QAX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg32_get(unit, IHB_CPU_RECORD_CONTROLr, core, 0, &reg_val));
        soc_reg_field_set(unit, IHB_CPU_RECORD_CONTROLr, &reg_val,  CPU_RECORD_TRIGGERf, 1);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg32_set(unit, IHB_CPU_RECORD_CONTROLr, core, 0, reg_val));
    }

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_INFO(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): opcode=0x%x, "), FUNCTION_NAME(), opcode));
    arad_kbp_format_long_integer(rval_str, msb_data, SOC_REG_ABOVE_64_INFO(unit, IHB_CPU_RECORD_DATA_MSBr).size);
    LOG_INFO(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"msb_data=%s\n"), rval_str));
    if (lsb_enable == 0x1) {
        arad_kbp_format_long_integer(rval_str, lsb_data, SOC_REG_ABOVE_64_INFO(unit, IHB_CPU_RECORD_DATA_LSBr).size);
        LOG_INFO(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"lsb_data=%s\n"), rval_str));   
    }
#endif
    
#ifndef PLISIM
                
    soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);
    for (;;) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg32_get(unit, IHB_CPU_LOOKUP_REPLY_COUNTERr, core, 0, &reg_val));
        if (reg_val == 1) {
            break;
        }

        if (reg_val != 0) {
            LOG_ERROR(BSL_LS_SOC_TCAM, (BSL_META_U(unit,
                                                   "Error in %s(): register IHB_CPU_LOOKUP_REPLY_COUNTERr not asserted correctly for core %u\n."), FUNCTION_NAME(), core));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 75, exit);
            break;
        }

        if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SOC_TCAM, (BSL_META_U(unit,
                                                   "Error in %s(): IHB_CPU_LOOKUP_REPLY register LOOKUP_REPLY_VALID field not asserted correctly for core %u."), FUNCTION_NAME(), core));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 80, exit);
            break;
        }
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, arad_kbp_cpu_lookup_reply(unit, core, data));

    if (SOC_IS_JERICHO(unit)) {
        soc_reg_above_64_field_get(unit, IHB_LOOKUP_REPLYr, data, LOOKUP_REPLY_ROP_ERRORf, error_field);
        rop_error = SHR_BITGET(error_field,0);

        soc_reg_above_64_field_get(unit, IHB_LOOKUP_REPLYr, data, LOOKUP_REPLY_ELK_ERRORf, error_field);
    } else {
        soc_reg_above_64_field_get(unit, IHB_LOOKUP_REPLY_DATAr, data, LOOKUP_REPLY_ROP_ERRORf, error_field);
        rop_error = SHR_BITGET(error_field,0);

        soc_reg_above_64_field_get(unit, IHB_LOOKUP_REPLY_DATAr, data, LOOKUP_REPLY_ELK_ERRORf, error_field);
    }    
    
    elk_error = SHR_BITGET(error_field,0);
    if ((rop_error == 0x1) || (elk_error == 0x1)) {
        if (retransmission_num) {
            retransmission_num --;
            goto retransmit;
        } else {
            
            LOG_ERROR(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"Error in %s(): register LOOKUP_REPLY_DATA: LOOKUP_REPLY_ROP_ERROR=%d, "
                                                       "LOOKUP_REPLY_ELK_ERROR=%d (excpected 0 on both) for core %u\n"),
                                        FUNCTION_NAME() ,rop_error, elk_error, core));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
        }
    }

    
    if (read_data != NULL){
        if (SOC_IS_JERICHO(unit)) {
            soc_reg_above_64_field_get(unit, IHB_LOOKUP_REPLYr, data, LOOKUP_REPLY_RECORDf, read_data);
        } else {
            soc_reg_above_64_field_get(unit, IHB_LOOKUP_REPLY_DATAr, data, LOOKUP_REPLY_RECORDf, read_data);
        }
    }

    	if (SOC_IS_JERICHO(unit)) {
        	field_val = soc_reg_above_64_field32_get(unit, IHB_LOOKUP_REPLYr, data, LOOKUP_REPLY_VALIDf);
    	} else {
        	SOC_SAND_SOC_IF_ERROR_RETURN(res, 130, exit, soc_reg32_get(unit, IHB_LOOKUP_REPLYr, REG_PORT_ANY, 0, &reg_val));
        	field_val = soc_reg_field_get(unit, IHB_LOOKUP_REPLYr, reg_val, LOOKUP_REPLY_VALIDf);
    	}
    
    	if(field_val != 0x1) {
        	LOG_ERROR(BSL_LS_SOC_TCAM,(BSL_META_U(unit,
                              "Error in %s(): IHB_LOOKUP_REPLY register LOOKUP_REPLY_VALIDf field not asserted correctly."), FUNCTION_NAME()));
        	SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 140, exit);
    	}
	
#endif
   
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_cpu_record_send()", opcode, reg_val);
}
#endif 



uint32 
    arad_kbp_lut_write(int unit, uint32 core, uint8 addr, arad_kbp_lut_data_t* lut_data, soc_reg_above_64_val_t lut_data_raw)
{
    uint32 local_addr,
           res;            
    soc_reg_above_64_val_t 
        msb_data,
        lsb_data;   
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
  
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

    
    local_addr = addr & 0xff;
    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "%s(): local_addr=0x%x\n"), FUNCTION_NAME(), local_addr));

    
    SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_DATA_ADDR_BIT_START , &local_addr, 0, ARAD_KBP_LUTWR_DATA_ADDR_BIT_LEN);

    if (lut_data != NULL && ARAD_KBP_IS_OP_OR_OP2) {
        uint32 ltr, mode = 0;
        uint16 first = 0, second = 0;

        if (elk->kbp_lut_write_mode == ARAD_KBP_LUT_MDIO_WRITE) {
            uint16_t value_temp;

            ltr = ((lut_data->instr & 0x3F) | (lut_data->instr & 0x8000) >> 9);

            first = addr & 0xFF;
            first |= ((lut_data->rec_size & 0x7F) << 8);
            first |= ((lut_data->rec_is_valid & 0x1) << 15);

            second |= ((ltr & 0x7F) << 1);
            second |= (1 << 8);

            arad_kbp_callback_mdio_write(&elk->kbp_user_data[core], 0, 1, 0xA005, first);
            arad_kbp_callback_mdio_write(&elk->kbp_user_data[core], 0, 1, 0xA006, second);

            if (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
                
                second = (second | (1 << 7)) + (1 << 1);
            }
            arad_kbp_callback_mdio_write(&elk->kbp_user_data[core], 0, 1, 0xB005, first);
            arad_kbp_callback_mdio_write(&elk->kbp_user_data[core], 0, 1, 0xB006, second);
            
            arad_kbp_callback_mdio_read(&elk->kbp_user_data[core], 0, 0, 0, &value_temp);
            SOC_EXIT;
        } else {
            
            ltr = ((lut_data->instr & 0x3F) | (lut_data->instr & 0x8000) >> 9) | (1 << 7);
            if ((!elk->is_master) && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT)) {

                
                ltr = (ltr | (1 << 6)) + 1;
            }
            SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_REC_SIZE_BIT_START_OP, &(lut_data->rec_size), 0, ARAD_KBP_LUT_REC_SIZE_BIT_LEN);
            SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_REC_IS_VALID_BIT_START_OP, &(lut_data->rec_is_valid), 0, ARAD_KBP_LUT_REC_IS_VALID_BIT_LEN);
            SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_MODE_BIT_START_OP, &mode, 0, ARAD_KBP_LUT_REC_TYPE_BIT_LEN);
            SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTRD_INSTR_BIT_START_OP, &ltr, 0, ARAD_KBP_LUT_INSTR_BIT_LEN);
        }
    } else if (lut_data != NULL) {  
       LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "%s(): lut_data_raw == 0"), FUNCTION_NAME()));              
        
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_REC_SIZE_BIT_START,     &(lut_data->rec_size),    0, ARAD_KBP_LUT_REC_SIZE_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_REC_TYPE_BIT_START,     &(lut_data->rec_type),    0, ARAD_KBP_LUT_REC_TYPE_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_REC_IS_VALID_BIT_START, &(lut_data->rec_is_valid),0, ARAD_KBP_LUT_REC_IS_VALID_BIT_LEN);
        
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_MODE_BIT_START,            &(lut_data->mode),            0, ARAD_KBP_LUT_MODE_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_KEY_CONFIG_BIT_START,      &(lut_data->key_config),      0, ARAD_KBP_LUT_KEY_CONFIG_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_LUT_KEY_DATA_BIT_START,    &(lut_data->lut_key_data),    0, ARAD_KBP_LUT_LUT_KEY_DATA_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_INSTR_BIT_START,           &(lut_data->instr),           0, ARAD_KBP_LUT_INSTR_BIT_LEN);
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "%s(): instr=0x%x, start=%d, len=%d\n"), FUNCTION_NAME(), lut_data->instr, ARAD_KBP_LUTWR_INSTR_BIT_START, ARAD_KBP_LUT_INSTR_BIT_LEN));
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_KEY_W_CPD_GT_80_BIT_START, &(lut_data->key_w_cpd_gt_80), 0, ARAD_KBP_LUT_KEY_W_CPD_GT_80_BIT_LEN );
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_COPY_DATA_CFG_BIT_START ,  &(lut_data->copy_data_cfg),   0, ARAD_KBP_LUT_COPY_DATA_CFG_BIT_LEN );
        
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT0_IDX_AD_CFG_BIT_START , &(lut_data->result_idx_ad_cfg[0]), 0, ARAD_KBP_LUT_RESULT0_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT0_IDX_OR_AD_BIT_START ,  &(lut_data->result_idx_or_ad[0]),  0, ARAD_KBP_LUT_RESULT0_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT1_IDX_AD_CFG_BIT_START , &(lut_data->result_idx_ad_cfg[1]), 0, ARAD_KBP_LUT_RESULT1_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT1_IDX_OR_AD_BIT_START ,  &(lut_data->result_idx_or_ad[1]),  0, ARAD_KBP_LUT_RESULT1_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT2_IDX_AD_CFG_BIT_START , &(lut_data->result_idx_ad_cfg[2]), 0, ARAD_KBP_LUT_RESULT2_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT2_IDX_OR_AD_BIT_START ,  &(lut_data->result_idx_or_ad[2]),  0, ARAD_KBP_LUT_RESULT2_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT3_IDX_AD_CFG_BIT_START , &(lut_data->result_idx_ad_cfg[3]), 0, ARAD_KBP_LUT_RESULT3_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT3_IDX_OR_AD_BIT_START ,  &(lut_data->result_idx_or_ad[3]),  0, ARAD_KBP_LUT_RESULT3_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT4_IDX_AD_CFG_BIT_START , &(lut_data->result_idx_ad_cfg[4]), 0, ARAD_KBP_LUT_RESULT4_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT4_IDX_OR_AD_BIT_START ,  &(lut_data->result_idx_or_ad[4]),  0, ARAD_KBP_LUT_RESULT4_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT5_IDX_AD_CFG_BIT_START , &(lut_data->result_idx_ad_cfg[5]), 0, ARAD_KBP_LUT_RESULT5_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_RESULT5_IDX_OR_AD_BIT_START ,  &(lut_data->result_idx_or_ad[5]),  0, ARAD_KBP_LUT_RESULT5_IDX_OR_AD_BIT_LEN);        
         
    } else if (lut_data_raw != NULL){
        if (ARAD_KBP_IS_OP_OR_OP2) {
            SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_lut_write() called with lut_raw_data for OP", 0, 0);
        }
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "%s(): lut_data_raw != 0"), FUNCTION_NAME()));   
        arad_kbp_array_print_debug((uint8 *)lut_data_raw , SOC_REG_ABOVE_64_MAX_SIZE_U32*sizeof(uint32));
        SHR_BITCOPY_RANGE(msb_data, ARAD_KBP_LUTWR_LUT_DATA_START, lut_data_raw,  0, ARAD_KBP_LUTWR_LUT_DATA_LEN);
    }

    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "%s(): msb_data="), FUNCTION_NAME()));
    arad_kbp_array_print_debug((uint8 *)msb_data , SOC_REG_ABOVE_64_MAX_SIZE_U32*sizeof(uint32));

#ifdef BCM_88660
    if (SOC_IS_JERICHO(unit)) {
#ifdef  JER_KBP_ROP_OPTIMIZATION
        res = jericho_kbp_optimized_cpu_record_send(unit, core, ARAD_KBP_CPU_WR_LUT_OPCODE, msb_data, lsb_data, 0, NULL);
#else 
        res = aradplus_kbp_cpu_record_send(unit,
                                           core,
                                           ARAD_KBP_CPU_WR_LUT_OPCODE,
                                           msb_data,
                                           lsb_data,
                                           0,
                                           NULL);
#endif
    } else if (SOC_IS_ARADPLUS(unit)) {
#ifdef  ARAD_KBP_ROP_OPTIMIZATION
        res = aradplus_kbp_optimized_cpu_record_send(unit, ARAD_KBP_CPU_WR_LUT_OPCODE, msb_data, lsb_data, 0, NULL);
#else 
        res = aradplus_kbp_cpu_record_send(unit,
                                           core,
                                           ARAD_KBP_CPU_WR_LUT_OPCODE,
                                           msb_data,
                                           lsb_data,
                                           0,
                                           NULL);
#endif
    } else
#endif 
    {
        res = arad_kbp_cpu_record_send(unit, ARAD_KBP_CPU_WR_LUT_OPCODE, msb_data, lsb_data, 0);
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_lut_write()", 0, 0);
}




uint32 
    arad_kbp_lut_read(int unit, uint32 core, uint8 addr,arad_kbp_lut_data_t* lut_data, soc_reg_above_64_val_t lut_row_data)
{
    uint32 
        res,
        *swap_addr;
    soc_reg_above_64_val_t 
        msb_data,
        lsb_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META_U(unit,
                           "%s(): addr= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(&addr, 1);

    sal_memcpy(((uint8 *)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES, 
               &addr, NLMDEV_REG_ADDR_LEN_IN_BYTES);

    
    swap_addr = &msb_data[(ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES)/4];
    *swap_addr = soc_ntohl(*swap_addr);

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        res = aradplus_kbp_cpu_record_send(
            unit,
            core,
            ARAD_KBP_CPU_RD_LUT_OPCODE,
            msb_data,
            lsb_data,
            0,
            msb_data);
    } else
#endif 
    {
        res = arad_kbp_cpu_record_send(
            unit,
            ARAD_KBP_CPU_RD_LUT_OPCODE,
            msb_data,
            lsb_data,
            0);    
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        res = arad_kbp_cpu_lookup_reply(
            unit,
            0,
            msb_data);
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    if(lut_row_data != NULL) {
        sal_memcpy(lut_row_data, ((uint8 *)msb_data), sizeof(soc_reg_above_64_val_t));
    }

    if(lut_data != NULL) {
        sal_memset(lut_data, 0, sizeof(arad_kbp_lut_data_t));
    
            
        SHR_BITCOPY_RANGE(&(lut_data->rec_size),0,msb_data, ARAD_KBP_LUTRD_REC_SIZE_BIT_START, ARAD_KBP_LUT_REC_SIZE_BIT_LEN);
        SHR_BITCOPY_RANGE( &(lut_data->rec_type),0,msb_data, ARAD_KBP_LUTRD_REC_TYPE_BIT_START, ARAD_KBP_LUT_REC_TYPE_BIT_LEN);
        SHR_BITCOPY_RANGE( &(lut_data->rec_is_valid),0,msb_data, ARAD_KBP_LUTRD_REC_IS_VALID_BIT_START, ARAD_KBP_LUT_REC_IS_VALID_BIT_LEN);
        
        SHR_BITCOPY_RANGE(&(lut_data->mode),0, msb_data, ARAD_KBP_LUTRD_MODE_BIT_START, ARAD_KBP_LUT_MODE_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->key_config),0,msb_data, ARAD_KBP_LUTRD_KEY_CONFIG_BIT_START, ARAD_KBP_LUT_KEY_CONFIG_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->lut_key_data),0,msb_data, ARAD_KBP_LUTRD_LUT_KEY_DATA_BIT_START, ARAD_KBP_LUT_LUT_KEY_DATA_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->instr),0,msb_data, ARAD_KBP_LUTRD_INSTR_BIT_START, ARAD_KBP_LUT_INSTR_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->key_w_cpd_gt_80),0,msb_data, ARAD_KBP_LUTRD_KEY_W_CPD_GT_80_BIT_START, ARAD_KBP_LUT_KEY_W_CPD_GT_80_BIT_LEN );
        SHR_BITCOPY_RANGE(&(lut_data->copy_data_cfg),0,msb_data, ARAD_KBP_LUTRD_COPY_DATA_CFG_BIT_START, ARAD_KBP_LUT_COPY_DATA_CFG_BIT_LEN );
        
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_ad_cfg[0]),0,msb_data, ARAD_KBP_LUTRD_RESULT0_IDX_AD_CFG_BIT_START, ARAD_KBP_LUT_RESULT0_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_or_ad[0]),0,msb_data, ARAD_KBP_LUTRD_RESULT0_IDX_OR_AD_BIT_START, ARAD_KBP_LUT_RESULT0_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_ad_cfg[1]),0,msb_data, ARAD_KBP_LUTRD_RESULT1_IDX_AD_CFG_BIT_START, ARAD_KBP_LUT_RESULT1_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_or_ad[1]),0,msb_data, ARAD_KBP_LUTRD_RESULT1_IDX_OR_AD_BIT_START, ARAD_KBP_LUT_RESULT1_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_ad_cfg[2]),0,msb_data, ARAD_KBP_LUTRD_RESULT2_IDX_AD_CFG_BIT_START, ARAD_KBP_LUT_RESULT2_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_or_ad[2]),0,msb_data, ARAD_KBP_LUTRD_RESULT2_IDX_OR_AD_BIT_START, ARAD_KBP_LUT_RESULT2_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_ad_cfg[3]),0,msb_data, ARAD_KBP_LUTRD_RESULT3_IDX_AD_CFG_BIT_START, ARAD_KBP_LUT_RESULT3_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_or_ad[3]),0,msb_data, ARAD_KBP_LUTRD_RESULT3_IDX_OR_AD_BIT_START, ARAD_KBP_LUT_RESULT3_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_ad_cfg[4]),0,msb_data, ARAD_KBP_LUTRD_RESULT4_IDX_AD_CFG_BIT_START, ARAD_KBP_LUT_RESULT4_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_or_ad[5]),0,msb_data, ARAD_KBP_LUTRD_RESULT4_IDX_OR_AD_BIT_START, ARAD_KBP_LUT_RESULT4_IDX_OR_AD_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_ad_cfg[5]),0,msb_data, ARAD_KBP_LUTRD_RESULT5_IDX_AD_CFG_BIT_START, ARAD_KBP_LUT_RESULT5_IDX_AD_CFG_BIT_LEN);
        SHR_BITCOPY_RANGE(&(lut_data->result_idx_or_ad[5]),0,msb_data, ARAD_KBP_LUTRD_RESULT5_IDX_OR_AD_BIT_START, ARAD_KBP_LUT_RESULT5_IDX_OR_AD_BIT_LEN); 
    
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "addr=0x%x, lut_data:\n rec_size=0x%x rec_type=0x%x rec_is_valid=0x%x mode=0x%x key_config=0x%x\n" 
                                "lut_key_data=0x%x instr=0x%x key_w_cpd_gt_80=0x%x copy_data_cfg=0x%x\n"
                                "result_idx_ad_cfg[0]=0x%x result_idx_or_ad[0]=0x%x result_idx_ad_cfg[1]=0x%x result_idx_or_ad[1]=0x%x\n"
                                "result_idx_ad_cfg[2]=0x%x result_idx_or_ad[2]=0x%x result_idx_ad_cfg[3]=0x%x result_idx_or_ad[3]=0x%x\n"
                                "result_idx_ad_cfg[4]=0x%x result_idx_or_ad[4]=0x%x result_idx_ad_cfg[5]=0x%x result_idx_or_ad[5]=0x%x\n"),
                     addr, lut_data->rec_size, lut_data->rec_type, lut_data->rec_is_valid, lut_data->mode, lut_data->key_config, 
                     lut_data->lut_key_data, lut_data->instr, lut_data->key_w_cpd_gt_80, lut_data->copy_data_cfg,
                     lut_data->result_idx_ad_cfg[0], lut_data->result_idx_or_ad[0], lut_data->result_idx_ad_cfg[1], lut_data->result_idx_or_ad[1],
                     lut_data->result_idx_ad_cfg[2], lut_data->result_idx_or_ad[2], lut_data->result_idx_ad_cfg[3], lut_data->result_idx_or_ad[3],
                     lut_data->result_idx_ad_cfg[4], lut_data->result_idx_or_ad[4], lut_data->result_idx_ad_cfg[5], lut_data->result_idx_or_ad[5]));
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_lut_read()", 0, 0);
}



uint32 arad_kbp_rop_write(
        int unit,
        uint32 core, 
        arad_kbp_rop_write_t *wr_data)
{
    uint32 res;
    soc_reg_above_64_val_t 
        msb_data,
        lsb_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifndef  JER_KBP_ROP_OPTIMIZATION
    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);
#endif

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): wr_data->addr= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(wr_data->addr, NLMDEV_REG_ADDR_LEN_IN_BYTES);
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): wr_data->data= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(wr_data->data, NLM_DATA_WIDTH_BYTES);
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): wr_data->mask= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(wr_data->mask, NLM_DATA_WIDTH_BYTES);
#endif

    

    
    ARAD_KBP_ROP_REVERSE_DATA(  wr_data->addr , (((uint8 *)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES), NLMDEV_REG_ADDR_LEN_IN_BYTES);
    ARAD_KBP_ROP_REVERSE_DATA( (wr_data->data), (((uint8 *)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES - NLM_DATA_WIDTH_BYTES), NLM_DATA_WIDTH_BYTES);
    ARAD_KBP_ROP_REVERSE_DATA( (wr_data->mask), (((uint8 *)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES - NLM_DATA_WIDTH_BYTES - NLM_DATA_WIDTH_BYTES), NLM_DATA_WIDTH_BYTES);

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_ERROR(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): before msb_data= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(((uint8 *)msb_data), sizeof(soc_reg_above_64_val_t));
#endif
    
    
#if defined(BE_HOST)
    arad_kbp_buffer_endians_fix(unit, msb_data);
#endif

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): after  msb_data= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(((uint8 *)msb_data), sizeof(soc_reg_above_64_val_t));
#endif

#ifdef ARAD_KBP_ROP_TIME_MEASUREMENTS
    soc_sand_ll_timer_set("ROP write", 1);
#endif
#ifdef BCM_88660

    if (SOC_IS_JERICHO(unit)) {
#ifdef  JER_KBP_ROP_OPTIMIZATION
        
        SOC_DPP_ALLOW_WARMBOOT_WRITE(jericho_kbp_optimized_cpu_record_send(unit, core, ARAD_KBP_CPU_PIOWR_OPCODE, msb_data, lsb_data, 0, NULL), res);
#else 
        
        SOC_DPP_ALLOW_WARMBOOT_WRITE(aradplus_kbp_cpu_record_send(
                                        unit,
                                        core,
                                        ARAD_KBP_CPU_PIOWR_OPCODE,
                                        msb_data,
                                        lsb_data,
                                        0,
                                        NULL), res);
#endif
    } else if (SOC_IS_ARADPLUS(unit)) {
#ifdef  ARAD_KBP_ROP_OPTIMIZATION
        
        SOC_DPP_ALLOW_WARMBOOT_WRITE(aradplus_kbp_optimized_cpu_record_send(unit, ARAD_KBP_CPU_PIOWR_OPCODE, msb_data, lsb_data, 0, NULL), res);
#else 
        
        SOC_DPP_ALLOW_WARMBOOT_WRITE(aradplus_kbp_cpu_record_send(
                                        unit,
                                        core,
                                        ARAD_KBP_CPU_PIOWR_OPCODE,
                                        msb_data,
                                        lsb_data,
                                        0,
                                        NULL), res);
#endif
    } else
#endif
    {
        
        SOC_DPP_ALLOW_WARMBOOT_WRITE(arad_kbp_cpu_record_send(
                                        unit,
                                        ARAD_KBP_CPU_PIOWR_OPCODE,
                                        msb_data,
                                        lsb_data,
                                        0), res);
    }

#ifdef ARAD_KBP_ROP_TIME_MEASUREMENTS
    soc_sand_ll_timer_stop(1);
#endif

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);    


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_rop_write()", 0, 0);
}


uint32 arad_kbp_rop_read(
        int unit,
        uint32 core, 
        arad_kbp_rop_read_t *rd_data
        )
{
    uint32 
        res,
        opcode,
        *addr;
    uint8 receive_data[ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN] = {0};
    soc_reg_above_64_val_t 
        msb_data,
        lsb_data,
        above_64_rd_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): rd_data->dataType=%d, rd_data->addr= "), FUNCTION_NAME(), rd_data->dataType));
    arad_kbp_array_print_debug(rd_data->addr, NLMDEV_REG_ADDR_LEN_IN_BYTES);
#endif

    if (rd_data->dataType == NLM_ARAD_READ_MODE_DATA_X) {
        opcode = ARAD_KBP_CPU_PIORDX_OPCODE;
    } else if (rd_data->dataType == NLM_ARAD_READ_MODE_DATA_Y) {
        opcode = ARAD_KBP_CPU_PIORDY_OPCODE;
    } else {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);  
    }

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): rd_data->addr= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(rd_data->addr, NLMDEV_REG_ADDR_LEN_IN_BYTES);
#endif

    sal_memcpy(((uint8 *)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES, 
               rd_data->addr, NLMDEV_REG_ADDR_LEN_IN_BYTES );

    
    addr = &msb_data[(ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLMDEV_REG_ADDR_LEN_IN_BYTES)/4];
    *addr = soc_ntohl(*addr);

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        
        SOC_DPP_ALLOW_WARMBOOT_WRITE(aradplus_kbp_cpu_record_send(
                                        unit,
                                        core,
                                        opcode,
                                        msb_data,
                                        lsb_data,
                                        0,
                                        above_64_rd_data), res);
    } else
#endif
    {
        
        SOC_DPP_ALLOW_WARMBOOT_WRITE(arad_kbp_cpu_record_send(
                                        unit,
                                        opcode,
                                        msb_data,
                                        lsb_data,
                                        0), res);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        
        SOC_DPP_ALLOW_WARMBOOT_WRITE(arad_kbp_cpu_lookup_reply(
                                        unit,
                                        core,
                                        above_64_rd_data), res); 
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS    
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): returned data= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug((uint8 *)above_64_rd_data, ARAD_KBP_CPU_LOOKUP_REPLY_DATA_REGISTER_BYTE_LEN);    
#endif 

    
#if defined(BE_HOST)
    arad_kbp_buffer_endians_fix(unit, above_64_rd_data);
#endif

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): returned data after endians fix= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug((uint8 *)above_64_rd_data, ARAD_KBP_CPU_LOOKUP_REPLY_DATA_REGISTER_BYTE_LEN);    
#endif 

    ARAD_KBP_ROP_REVERSE_DATA(((uint8 *)above_64_rd_data), receive_data, ARAD_KBP_CPU_LOOKUP_REPLY_DATA_REGISTER_BYTE_LEN);
    sal_memcpy(rd_data->data, receive_data,  NLM_DATA_WIDTH_BYTES + 1);

#ifdef ARAD_KBP_ROP_DEBUG_PRINTS
    LOG_VERBOSE(BSL_LS_SOC_TCAM,(BSL_META_U(unit,"%s(): rd_data->data= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(rd_data->data, NLM_DATA_WIDTH_BYTES + 1);
#endif 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_rop_read()", rd_data->dataType, rd_data->addr[0]);
}

uint32 arad_kbp_rop_cbw_cmp(
        int unit, 
        uint32 core,
        arad_kbp_rop_cbw_cmp_t *cbw_cmp_data
        )
{
    uint32 
        res,
        cbw_data_bytes_left,
        cmp_resp_data_len;
    int 
        lsb_enable;
    soc_reg_above_64_val_t 
        msb_data,
        lsb_data,
        above_64_cmp_data;
    uint8
        cmp_data[NLMDEV_MAX_RESP_LEN_IN_BYTES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

    sal_memset(cmp_data, 0x0, sizeof(uint8) * 64);

    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "%s(): cbw_cmp_data->cbw_data.data= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug(cbw_cmp_data->cbw_data.data, cbw_cmp_data->cbw_data.data_len);

    if ( ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN >= cbw_cmp_data->cbw_data.data_len) {
        sal_memcpy(((uint8 *)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - cbw_cmp_data->cbw_data.data_len, 
                   cbw_cmp_data->cbw_data.data, cbw_cmp_data->cbw_data.data_len);
        lsb_enable = 0;
    } else {
        cbw_data_bytes_left = cbw_cmp_data->cbw_data.data_len - ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN;
        sal_memcpy(((uint8 *)msb_data), cbw_cmp_data->cbw_data.data + cbw_data_bytes_left, ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN);
        sal_memcpy(((uint8 *)lsb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - cbw_data_bytes_left, cbw_cmp_data->cbw_data.data, cbw_data_bytes_left);

        
#if defined(BE_HOST)
        arad_kbp_buffer_endians_fix(unit, lsb_data);
#endif
        lsb_enable = 1;
    }
    
#if defined(BE_HOST)
   arad_kbp_buffer_endians_fix(unit, msb_data);
#endif

    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "%s(): msb_data= "), FUNCTION_NAME()));
    arad_kbp_array_print_debug((uint8 *)msb_data, sizeof(soc_reg_above_64_val_t));

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        res = aradplus_kbp_cpu_record_send(
            unit,
            core,
            cbw_cmp_data->opcode,
            msb_data,
            lsb_data,
            lsb_enable,
            above_64_cmp_data);
    } else
#endif
    {
        res = arad_kbp_cpu_record_send(
            unit,
            cbw_cmp_data->opcode,
            msb_data,
            lsb_data,
            lsb_enable);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        res = arad_kbp_cpu_lookup_reply(
            unit,
            0,
            above_64_cmp_data);
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if ((cbw_cmp_data->type == NLM_ARAD_CB_INST_CMP1) || 
        (cbw_cmp_data->type == NLM_ARAD_CB_INST_CMP2) ||
        (cbw_cmp_data->type == NLM_ARAD_CB_INST_CMP3)) {

        
#if defined(BE_HOST)
        arad_kbp_buffer_endians_fix(unit, above_64_cmp_data);
#endif

        cmp_resp_data_len = arad_kbp_frwd_ltr_db[cbw_cmp_data->ltr].res_total_data_len;
        sal_memcpy(cmp_data, ((uint8 *)above_64_cmp_data) + ARAD_KBP_CPU_LOOKUP_REPLY_DATA_REGISTER_BYTE_LEN - cmp_resp_data_len, cmp_resp_data_len);

           LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "%s(): cmp_data= "), FUNCTION_NAME()));
        arad_kbp_array_print_debug(cmp_data, NLM_DATA_WIDTH_BYTES + 1);

        ARAD_KBP_ROP_REVERSE_DATA(cmp_data, cbw_cmp_data->result.data_raw, cmp_resp_data_len);

        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "%s(): cbw_cmp_data->result.data_raw= "), FUNCTION_NAME()));
        arad_kbp_array_print_debug(cbw_cmp_data->result.data_raw, cmp_resp_data_len);
    } 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_rop_cbw_cmp()", cbw_cmp_data->opcode, cbw_cmp_data->cbw_data.data_len);
}

uint32 arad_kbp_rop_blk_write(
        int unit, 
        uint32 core,
        arad_kbp_rop_blk_t *wr_data)
{
    uint32 
        res;
    soc_reg_above_64_val_t 
        msb_data,
        lsb_data;
    arad_kbp_rop_write_t 
        wr_data_rev;
    uint8  data[NLM_DATA_WIDTH_BYTES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

    if ((wr_data->opcode == ARAD_KBP_CPU_BLK_CLR_OPCODE) || (wr_data->opcode == ARAD_KBP_CPU_BLK_EV_OPCODE)) {
         
        data[0] = ((wr_data->loop_cnt & 0x0ff0) >> 4);
        data[1] = ((wr_data->loop_cnt & 0x00f)  << 4) |  (wr_data->set_not_clr & 0x01);
        data[2] = (wr_data->src_addr & 0x000f0000) >> 16;
        data[3] = (wr_data->src_addr & 0x0000ff00) >> 8;
        data[4] = (wr_data->src_addr & 0x000000ff);
    }
    else { 
         
        data[0] = (wr_data->loop_cnt & 0x0ff0) >> 4;
        data[1] = ((wr_data->loop_cnt & 0x000f) << 4) | (wr_data->copy_dir & 0x01);
        data[2] = (wr_data->src_addr & 0x000f0000) >> 16;
        data[3] = (wr_data->src_addr & 0x0000ff00) >> 8;
        data[4] = (wr_data->src_addr & 0x000000ff);
        data[5] = (wr_data->dst_addr & 0x000f0000) >> 16;
        data[6] = (wr_data->dst_addr & 0x0000ff00) >> 8;
        data[7] = (wr_data->dst_addr & 0x000000ff);
    }

    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "%s(): wr_data.data="), FUNCTION_NAME()));
    arad_kbp_array_print_debug(data, wr_data->data_len);

    ARAD_KBP_ROP_REVERSE_DATA(data, wr_data_rev.data, wr_data->data_len);

    sal_memcpy(((uint8 *)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - wr_data->data_len, 
               wr_data_rev.data, wr_data->data_len);
    
#if defined(BE_HOST)
    arad_kbp_buffer_endians_fix(unit, msb_data);
#endif

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        res = aradplus_kbp_cpu_record_send(
            unit,
            core,
            wr_data->opcode,
            msb_data,
            lsb_data,
            0,
            NULL);
    } else
#endif
    {
        res = arad_kbp_cpu_record_send(
            unit,
            wr_data->opcode,
            msb_data,
            lsb_data,
            0);
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_rop_write()", 0, 0);
}

uint32 arad_kbp_rop_ad_write(
    int unit,
    uint32 core,
    uint8 *wr_data,
    uint16 length)
{
    uint32 res;
    soc_reg_above_64_val_t msb_data, lsb_data;
    uint32 opcode;
    uint8 *data = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);

    data = (((uint8*)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - NLM_DATA_WORD_LEN_IN_BYTES);
    data[0] = wr_data[7];
    data[1] = wr_data[6];
    data[2] = wr_data[3];
    data[3] = wr_data[4];
    data[4] = wr_data[5];
    data[5] = wr_data[2];
    data[6] = wr_data[1];
    data[7] = wr_data[0];
    sal_memcpy(((uint8*)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - length,
                &wr_data[NLM_DATA_WORD_LEN_IN_BYTES], (length - NLM_DATA_WORD_LEN_IN_BYTES));

    
#if defined(BE_HOST)
    arad_kbp_buffer_endians_fix(unit, msb_data);
#endif

    if (length <= ARAD_KBP_CPU_4B_AD_REC_SIZE) {
        opcode = ARAD_KBP_CPU_4B_AD_OPCODE;
    } else if (length <= ARAD_KBP_CPU_8B_AD_REC_SIZE) {
        opcode = ARAD_KBP_CPU_8B_AD_OPCODE;
    } else if (length <= ARAD_KBP_CPU_16B_AD_REC_SIZE) {
        opcode = ARAD_KBP_CPU_16B_AD_OPCODE;
    } else if (length <= ARAD_KBP_CPU_32B_AD_REC_SIZE) {
        opcode = ARAD_KBP_CPU_32B_AD_OPCODE;
    } else {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit)) {
        res = aradplus_kbp_cpu_record_send(unit, core, opcode, msb_data, lsb_data, 0, NULL);
    } else
#endif
    {
        res = arad_kbp_cpu_record_send(unit, opcode, msb_data, lsb_data, 0);
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_rop_ad_write()", 0, 0);
}

uint32_t arad_kbp_rop_uda_operation(int unit, uint32 core, uint32 *data, uint32 data_count)
{
    int res;
    uint32 opcode;
    soc_reg_above_64_val_t msb_data, lsb_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(msb_data);
    SOC_REG_ABOVE_64_CLEAR(lsb_data);
    LOG_VERBOSE(BSL_LS_SOC_TCAM, (BSL_META_U(unit, "%s(): data="), FUNCTION_NAME()));
    arad_kbp_uint32_array_print_debug(data, data_count);
    sal_memcpy(((uint8_t *)msb_data) + ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN - data_count * sizeof(data[0]), ((uint8_t *)data), data_count * sizeof(data[0]));

    opcode = (data_count > 2) ? ARAD_KBP_CPU_UDA_OPERATION1_OPCODE : ARAD_KBP_CPU_UDA_OPERATION2_OPCODE;

#ifdef BCM_88660
    if (SOC_IS_JERICHO(unit)) {
#ifdef  JER_KBP_ROP_OPTIMIZATION
        SOC_DPP_ALLOW_WARMBOOT_WRITE(jericho_kbp_optimized_cpu_record_send(unit, core, opcode, msb_data, lsb_data, 0, NULL), res);
#else
        SOC_DPP_ALLOW_WARMBOOT_WRITE(aradplus_kbp_cpu_record_send(unit, core, opcode, msb_data, lsb_data, 0, NULL), res);
#endif
    } else if (SOC_IS_ARADPLUS(unit)) {
#ifdef  ARAD_KBP_ROP_OPTIMIZATION
        SOC_DPP_ALLOW_WARMBOOT_WRITE(aradplus_kbp_optimized_cpu_record_send(unit, opcode, msb_data, lsb_data, 0, NULL), res);
#else
        SOC_DPP_ALLOW_WARMBOOT_WRITE(aradplus_kbp_cpu_record_send(unit, core, opcode, msb_data, lsb_data, 0, NULL), res);
#endif
    } else
#endif
    {
        SOC_DPP_ALLOW_WARMBOOT_WRITE( arad_kbp_cpu_record_send(unit, opcode, msb_data, lsb_data, 0), res);
    }

    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit: 
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_rop_uda_operation()", 0, 0);
} 



#endif 
