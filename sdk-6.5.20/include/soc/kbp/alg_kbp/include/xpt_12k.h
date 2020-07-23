/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __XPT_H
#define __XPT_H

#include "kbp_legacy.h"
#include "kbp_portable.h"

#ifdef __cplusplus
extern "C" {
#endif



#define     XPT_MAGIC_NUMBER        (0x12344)
#define     NLM_XPT_OPCODE_CNT      (2)



#define NLM_OPCODE_NOP_BITS_8_6             (0x0)
#define NLM_OPCODE_NOP_BITS_5_0             (0x0)

#define NLM_OPCODE_REG_WRITE_BITS_8_6       (0x0)
#define NLM_OPCODE_REG_WRITE_BITS_5_0       (0x1)

#define NLM_OPCODE_DBA_WRITE_BITS_8_6       (0x0)
#define NLM_OPCODE_DBA_WRITE_BITS_5_0       (0x1)

#define NLM_OPCODE_UDA_WRITE_BITS_8_6       (0x0)
#define NLM_OPCODE_UDA_WRITE_BITS_5_0       (0x1)

#define NLM_OPCODE_REG_READ_BITS_8_6        (0x0)
#define NLM_OPCODE_REG_READ_BITS_5_0        (0x2)

#define NLM_OPCODE_UDA_READ_BITS_8_6        (0x0)
#define NLM_OPCODE_UDA_READ_BITS_5_0        (0x2)

#define NLM_OPCODE_DBA_READ_X_BITS_8_6      (0x0)
#define NLM_OPCODE_DBA_READ_X_BITS_5_0      (0x2)

#define NLM_OPCODE_DBA_READ_Y_BITS_8_6      (0x0)
#define NLM_OPCODE_DBA_READ_Y_BITS_5_0      (0x3)

#define NLM_OPCODE_EM_WRA_BITS_8_6          (0x0)
#define NLM_OPCODE_EM_WRA_BITS_5_0          (0x4)

#define NLM_OPCODE_EM_WRB_BITS_8_6          (0x0)
#define NLM_OPCODE_EM_WRB_BITS_5_0          (0x5)

#define NLM_OPCODE_EM_RDA_BITS_8_6          (0x0)
#define NLM_OPCODE_EM_RDA_BITS_5_0          (0x6)

#define NLM_OPCODE_EM_RDB_BITS_8_6          (0x0)
#define NLM_OPCODE_EM_RDB_BITS_5_0          (0x7)

#define NLM_OPCODE_BLOCK_COPY_BITS_8_6      (0x0)
#define NLM_OPCODE_BLOCK_COPY_BITS_5_0      (0x8)

#define NLM_OPCODE_BLOCK_MOVE_BITS_8_6      (0x0)
#define NLM_OPCODE_BLOCK_MOVE_BITS_5_0      (0x9)

#define NLM_OPCODE_BLOCK_CLEAR_BITS_8_6     (0x0)
#define NLM_OPCODE_BLOCK_CLEAR_BITS_5_0     (0xA)

#define NLM_OPCODE_BLOCK_EV_BITS_8_6        (0x0)
#define NLM_OPCODE_BLOCK_EV_BITS_5_0        (0xB)

#define NLM_OPCODE_LPMTYPE1_BITS_8_6            (0x0)
#define NLM_OPCODE_LPMTYPE1_BITS_5_0            (0xC)

#define NLM_OPCODE_LPMTYPE2_BITS_8_6            (0x0)
#define NLM_OPCODE_LPMTYPE2_BITS_5_0            (0xD)

#define NLM_OPCODE_LPMTYPE3_BITS_8_6            (0x0)
#define NLM_OPCODE_LPMTYPE3_BITS_5_0            (0xE)

#define NLM_OPCODE_CBWRITE_CMP1_BITS_8_6        (0x1)
#define NLM_OPCODE_CBWRITE_CMP2_BITS_8_6        (0x2)
#define NLM_OPCODE_CBWRITE_CMP3_BITS_8_6        (0x3)
#define NLM_OPCODE_CBWRITE_BITS_8_6             (0x4)
#define NLM_OPCODE_CBWLPM_BITS_8_6              (0x5)


#define NLM_DATA_WORD_LEN_IN_BYTES              (8)
#define NLM_MAX_DATA_WORDS                      (5)
#define NLM_LPMTYPE1_NUM_DATA_WORDS             (5)
#define NLM_LPMTYPE2_NUM_DATA_WORDS             (2)
#define NLM_LPMTYPE3_NUM_DATA_WORDS             (2)




typedef struct kbp_xpt_rqt_t {
    
    uint8_t m_opcode[NLM_XPT_OPCODE_CNT];
    uint32_t m_data_len;
    uint16_t m_ctx_addr;

    uint32_t m_address;
    uint8_t *m_data_p;
    uint8_t *m_mask_p;

    
    uint8_t m_opcode1[NLM_XPT_OPCODE_CNT];
    uint32_t m_data_len1;
    uint16_t m_ctx_addr1;


    uint8_t *m_data1_p;

    
    int32_t m_2smt;

    uint8_t m_smt_num;
    uint8_t m_port_num;
    uint8_t m_response_req;
    uint8_t m_format_data;
    int32_t m_ADorIdx;          

    void *m_result;
} kbp_xpt_rqt;



typedef struct kbp_xpt_rqt_id_t {
    uint32_t m_giga_count;
    uint32_t m_count;
} kbp_xpt_rqt_id;

struct kbp_xpt_operations_t;


typedef struct kbp_xpt_t {
    uint32_t device_type;   
    struct kbp_xpt_operations_t *m_oprs;
    void *m_derived;
    NlmCmAllocator *m_alloc;

    uint32_t m_xpt_chanel_id;

    int32_t m_locked;
    uint32_t xpt_magic;

    uint8_t m_port_mode;
    uint8_t m_smt_mode;

    uint32_t bc_bitmap;  

    uint8_t m_payload_size;
#ifdef NPX_NPS
    uint32_t m_xpt_side;
#endif

} kbp_xpt;



typedef struct kbp_xpt_operations_t {
    

    void *(*kbp_xpt_get_result) (void *self, kbp_xpt_rqt *rqt, NlmReasonCode *o_reason);

    

    void (*kbp_xpt_discard_result) (void *self, kbp_xpt_rqt *rqt, NlmReasonCode *o_reason);

    

    kbp_xpt_rqt *(*kbp_xpt_get_request) (void *self, NlmReasonCode *o_reason);

   

   kbp_xpt_rqt *(*kbp_xpt_get_request_smt) (void *self, uint32_t smt_no, NlmReasonCode *o_reason);

    

    void (*kbp_xpt_reset_requests) (void *self, NlmReasonCode *o_reason);

    

    void (*kbp_xpt_service_requests) (void *self, NlmReasonCode *o_reason);

    
    void (*kbp_xpt_service_requests_smt) (void *self, uint32_t smt_no, NlmReasonCode *o_reason);

    

    NlmErrNum_t(*kbp_xpt_reset_device) (void *self, NlmReasonCode *o_reason);

    
    NlmErrNum_t(*kbp_xpt_lock_config) (void *, uint32_t, NlmReasonCode *);
} kbp_xpt_operations;



#ifdef __cplusplus
}
#endif

#endif                          


