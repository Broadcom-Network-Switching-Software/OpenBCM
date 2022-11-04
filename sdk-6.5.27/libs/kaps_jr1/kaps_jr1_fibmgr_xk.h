

#ifndef INCLUDED_KAPS_JR1_XK_FIBTBLMGR_H
#define INCLUDED_KAPS_JR1_XK_FIBTBLMGR_H

#include "kaps_jr1_ab.h"
#include "kaps_jr1_simple_dba.h"

#include "kaps_jr1_externcstart.h"


typedef NlmErrNum_t(
    *kaps_jr1_fib_tbl_mgr_write_ab_data_cb_t) (
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_jr1_ab_info * startBlkNum,
    uint32_t logicalLoc,
    uint8_t rptId,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t byte_offset,
    NlmReasonCode * o_reason);

typedef NlmErrNum_t(
    *kaps_jr1_fib_tbl_mgr_delete_ab_data_cb_t) (
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_jr1_ab_info * startBlkNum,
    uint32_t logicalLoc,
    NlmReasonCode * o_reason);


typedef NlmErrNum_t(
    *kaps_jr1_fib_tbl_mgr_write_uda_data_cb_t) (
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint8_t sbNum,
    uint16_t address,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t udaDataType,
    NlmReasonCode * o_reason);

typedef NlmErrNum_t(
    *kaps_jr1_fib_tbl_mgr_copy_uda_data_cb_t) (
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t srcDevNum,
    uint8_t srcSBNum,
    uint16_t srcAddress,
    uint8_t numOfSB,            
    uint8_t dstDevNum,
    uint8_t dstSBNum,
    uint16_t dstAddress,
    NlmReasonCode * o_reason);

typedef NlmErrNum_t(
    *kaps_jr1_fib_tbl_mgr_uda_sbc_cb_t) (
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t dev_num,
    uint16_t src_rowNum,
    uint8_t src_LpuNum,
    uint16_t dst_rowNum,
    uint8_t dst_LpuNum,
    uint8_t numLpus_copy,
    uint8_t numLpus_clear,
    int8_t start_lpu,
    int8_t end_lpu,
    NlmReasonCode * o_reason_p);

typedef NlmErrNum_t(
    *kaps_jr1_fib_tbl_mgr_lsn_clear_cb_t) (
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t dev_num,
    uint16_t row_num,
    uint8_t lpu_num,
    uint8_t num_lpus,
    int8_t start_lpu,
    int8_t end_lpu,
    NlmReasonCode * o_reason_p);

typedef struct kaps_jr1_fib_tbl_mgr_callback_fn_ptrs
{
    kaps_jr1_fib_tbl_mgr_write_ab_data_cb_t m_writeABData;
    kaps_jr1_fib_tbl_mgr_delete_ab_data_cb_t m_deleteABData;
    kaps_jr1_fib_tbl_mgr_uda_sbc_cb_t m_UdaSBC;
    kaps_jr1_fib_tbl_mgr_lsn_clear_cb_t m_LsnClear;
} kaps_jr1_fib_tbl_mgr_callback_fn_ptrs;

#include "kaps_jr1_externcend.h"
#endif 
