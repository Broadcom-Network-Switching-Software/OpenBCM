

#ifndef INCLUDED_KAPS_JR1_IPM_H
#define INCLUDED_KAPS_JR1_IPM_H

#include "kaps_jr1_fib_cmn_pfxbundle.h"
#include "kaps_jr1_bitmap.h"
#include "kaps_jr1_fibmgr.h"
#include "kaps_jr1_fibmgr_xk.h"
#include "kaps_jr1_fib_dba.h"

#ifdef __cplusplus
extern "C"
{
#endif

    kaps_jr1_ipm *kaps_jr1_ipm_init(
    kaps_jr1_fib_tbl_mgr * fibTblMgr_p,
    uint32_t poolIndex,
    uint32_t pool_size,
    struct kaps_jr1_ab_info *startBlk,
    uint32_t blkWidthInBits,
    enum kaps_jr1_dba_trigger_compression_mode mode,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_jr1_ipm_add_entry(
    kaps_jr1_ipm * ipm,
    kaps_jr1_pfx_bundle * pfxBundle,
    uint32_t rptId,
    uint32_t length,
    uint32_t numRptBitsLoppedOff,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_jr1_ipm_write_entry_to_hw(
    kaps_jr1_ipm * ipm_p,
    kaps_jr1_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t numRptBitsLoppedOff,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_jr1_ipm_remove_entry(
    kaps_jr1_ipm * ipm,
    uint32_t pfxIndex,
    uint32_t rptId,
    uint32_t length,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_jr1_ipm_delete_entry_in_hw(
    kaps_jr1_ipm * ipm_p,
    uint32_t pfxIndex,
    uint32_t rptId,
    uint32_t length,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_jr1_ipm_fix_entry(
    kaps_jr1_ipm * ipm_p,
    uint32_t ab_num,
    uint32_t row_nr,
    NlmReasonCode * reason);

    NlmErrNum_t kaps_jr1_ipm_create_entry_data(
    kaps_jr1_ipm * ipm_p,
    uint32_t ab_num,
    uint32_t row_nr,
    uint8_t * data,
    uint8_t * mask,
    uint8_t is_xy,
    NlmReasonCode * reason);

    uint32_t kaps_jr1_ipm_recompute_len_in_simple_dba(
    kaps_jr1_ipm * ipm,
    uint32_t length);

    NlmErrNum_t kaps_jr1_ipm_init_rpt(
    kaps_jr1_ipm * ipm_p,
    uint32_t rptId,
    NlmReasonCode * o_reason);

    NlmErrNum_t kaps_jr1_ipm_free_rpt(
    kaps_jr1_ipm * ipm_p,
    uint32_t rptId);

    void kaps_jr1_ipm_destroy(
    kaps_jr1_nlm_allocator * alloc_p,
    kaps_jr1_ipm * ipm);

    uint32_t kaps_jr1_ipm_verify(
    kaps_jr1_ipm * ipm_p,
    uint32_t rptId);


    NlmErrNum_t kaps_jr1_ipm_wb_add_entry(
    kaps_jr1_ipm * ipm_p,
    kaps_jr1_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t pos);


    int32_t kaps_jr1_ipm_wb_store_ipt_entries(
    kaps_jr1_ipm * ipm_p,
    uint32_t rpt_id,
    uint32_t * nv_offset,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle);

#ifdef __cplusplus
}
#endif

#endif
