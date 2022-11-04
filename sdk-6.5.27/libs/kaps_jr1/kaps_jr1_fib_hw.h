

#ifndef INCLUDED_KAPS_JR1_FIB_HW_H
#define INCLUDED_KAPS_JR1_FIB_HW_H

#include "kaps_jr1_fibmgr.h"
#include "kaps_jr1_fibmgr_xk.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_device_alg.h"
#include "kaps_jr1_algo_hw.h"

#include "kaps_jr1_externcstart.h"



typedef struct NlmDevRPTEntry_s
{
    
    uint8_t m_data[8 * KAPS_JR1_RPB_WIDTH_8];
    uint8_t m_mask[8 * KAPS_JR1_RPB_WIDTH_8];

} kaps_jr1_dev_rpt_entry;

NlmErrNum_t kaps_jr1_iit_write(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_jr1_ab_info *start_blk,
    struct kaps_jr1_ads *write_ads,
    uint32_t addr,
    NlmReasonCode * o_reason_p);

NlmErrNum_t kaps_jr1_write_ab_data(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_jr1_ab_info *start_blk,
    uint32_t logicalLoc,
    uint8_t rptId,
    uint16_t startBit,
    uint16_t endBit,
    uint8_t * data,
    uint16_t dataLen,
    uint32_t byte_offset,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_delete_ab_data(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t devNum,
    uint16_t blkWidthInBits,
    struct kaps_jr1_ab_info *start_blk,
    uint32_t logicalLoc,
    NlmReasonCode * o_reason);







NlmErrNum_t kaps_jr1_fib_rpt_write(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    kaps_jr1_dev_rpt_entry * entry, 
    int32_t isDeleteOper,
    uint16_t addr,              
    NlmReasonCode * o_reason_p);

NlmErrNum_t kaps_jr1_fib_rit_write(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint32_t dev_num,
    struct kaps_jr1_pct *entry,
    uint16_t addr,              
    NlmReasonCode * o_reason_p);


void
kaps_jr1_write_first_half_of_fmap(
    uint8_t * write_buf,
    uint32_t buf_len_in_bytes,
    uint32_t cur_bit_pos,
    struct kaps_jr1_ads *write_ads);


void
kaps_jr1_write_second_half_of_fmap(
    uint8_t * write_buf,
    uint32_t buf_len_in_bytes,
    uint32_t cur_bit_pos,
    struct kaps_jr1_ads *write_ads);

#include "kaps_jr1_externcend.h"

#endif
