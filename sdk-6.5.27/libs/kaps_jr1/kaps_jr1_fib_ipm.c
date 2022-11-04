

#include "kaps_jr1_fib_ipm.h"
#include "kaps_jr1_fib_cmn_pfxbundle.h"

#include "kaps_jr1_fib_trienode.h"
#include "kaps_jr1_fib_lsnmc.h"
#include "kaps_jr1_fib_lsnmc_hw.h"
#include "kaps_jr1_fib_hw.h"

#include "kaps_jr1_device_internal.h"

static uint32_t
kaps_jr1_ab_get_blk_width(
    struct kaps_jr1_ab_info *ab)
{
    uint32_t blk_width_1 = 80;

    switch (ab->conf)
    {
        case KAPS_JR1_NORMAL_80:
            blk_width_1 = 80;
            break;
        case KAPS_JR1_NORMAL_160:
            blk_width_1 = 160;
            break;
        default:
            kaps_jr1_sassert(0);
            break;
    }

    return blk_width_1;
}

int32_t
kaps_jr1_get_pfx_bundle_prio_length(
    void *pfxBundle)
{
    kaps_jr1_pfx_bundle *pfxBundle_p = (kaps_jr1_pfx_bundle *) pfxBundle;
    return pfxBundle_p->m_nPfxSize;
}

uint32_t
kaps_jr1_ipm_recompute_len_in_simple_dba(
    kaps_jr1_ipm * ipm,
    uint32_t length)
{
    uint32_t recomputed_len = length;

    return recomputed_len;
}

void
kaps_jr1_ipm_ipt_shuffle_callback(
    void *ipm_p,
    void *bundle_p,
    uint32_t newIptLocation,
    uint32_t length)
{
    kaps_jr1_pfx_bundle *pfxBundle_p = (kaps_jr1_pfx_bundle *) bundle_p;
    uint32_t oldIptLocation = pfxBundle_p->m_nIndex;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_ipm *self_p = (kaps_jr1_ipm *) ipm_p;
    kaps_jr1_fib_tbl_mgr *fibTblMgr = self_p->m_fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    int32_t isDirShiftUp;
    uint8_t *tempPtr;
    kaps_jr1_trie_node *iptNode_p, *rptNode_p;
    uint32_t numRptBitsLoppedOff;
    kaps_jr1_lsn_mc_settings *settings;
    struct kaps_jr1_device *device;
    uint8_t hb_data[KAPS_JR1_HB_ROW_WIDTH_8];
    kaps_jr1_status status;
    struct kaps_jr1_db *db = fibTblMgr->m_curFibTbl->m_db;

    (void) length;

    pfxBundle_p->m_nIndex = newIptLocation;

    isDirShiftUp = 1;
    if (oldIptLocation < newIptLocation)
    {
        
        isDirShiftUp = 0;
    }

    tempPtr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle_p);
    kaps_jr1_memcpy(&iptNode_p, tempPtr, sizeof(kaps_jr1_trie_node *));

    settings = iptNode_p->m_lsn_p->m_pSettings;
    device = fibTblMgr->m_devMgr_p;

    rptNode_p = iptNode_p->m_rptParent_p;

    if (isDirShiftUp)
    {
        errNum = kaps_jr1_lsn_mc_update_iit(iptNode_p->m_lsn_p, &reason);

        if (errNum != NLMERR_OK)
        {
            self_p->cb_result = errNum;
            self_p->cb_reason = reason;
            return;
        }

    }

    numRptBitsLoppedOff = kaps_jr1_trie_get_num_rpt_bits_lopped_off(device, rptNode_p->m_depth);

    errNum = kaps_jr1_ipm_delete_entry_in_hw(self_p, pfxBundle_p->m_nIndex, rptNode_p->m_rptId,
                                         iptNode_p->m_depth - numRptBitsLoppedOff, &reason);

    if (errNum != NLMERR_OK)
    {
        self_p->cb_result = errNum;
        self_p->cb_reason = reason;
        return;
    }

    errNum = kaps_jr1_ipm_write_entry_to_hw(self_p, pfxBundle_p, rptNode_p->m_rptId,
                                        iptNode_p->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, &reason);
    if (errNum != NLMERR_OK)
    {
        self_p->cb_result = errNum;
        self_p->cb_reason = reason;
        return;
    }

    if (!isDirShiftUp)
    {
        errNum = kaps_jr1_lsn_mc_update_iit(iptNode_p->m_lsn_p, &reason);

        if (errNum != NLMERR_OK)
        {
            self_p->cb_result = errNum;
            self_p->cb_reason = reason;
            return;
        }
    }

    if (settings->m_areIPTHitBitsPresent)
    {
        if (iptNode_p->m_iitLmpsofarPfx_p && iptNode_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
        {
            struct kaps_jr1_ab_info *ab = self_p->m_ab_info;

            {

                while (ab)
                {
                    
                    status = kaps_jr1_dm_hb_read(device, db, device->dba_offset + ab->ab_num, oldIptLocation, hb_data);
                    if (status != KAPS_JR1_OK)
                    {
                        self_p->cb_result = NLMERR_FAIL;
                        self_p->cb_reason = NLMRSC_XPT_FAILED;
                    }

                    
                    status = kaps_jr1_dm_hb_write(device, db, device->dba_offset + ab->ab_num, newIptLocation, hb_data);
                    if (status != KAPS_JR1_OK)
                    {
                        self_p->cb_result = NLMERR_FAIL;
                        self_p->cb_reason = NLMRSC_XPT_FAILED;
                    }

                    ab = ab->dup_ab;
                }
            }
        }
    }

    fibStats->numOfIPTShuffle++;

}

static void
kaps_jr1_ipm_pvt_fill_ab_info(
    kaps_jr1_fib_tbl_mgr * fibTblMgr_p,
    struct kaps_jr1_ab_info *ab,
    uint32_t blkWidthInBits)
{
    struct kaps_jr1_db *db = fibTblMgr_p->m_curFibTbl->m_db;
    uint32_t rpb_id = db->rpb_id;
    struct kaps_jr1_device *device = db->device;

    if (db->parent)
        db = db->parent;

    ab->conf = KAPS_JR1_NORMAL_160;
    ab->num_slots = device->hw_res->num_rows_in_rpb[rpb_id]; 
}

kaps_jr1_ipm *
kaps_jr1_ipm_init(
    kaps_jr1_fib_tbl_mgr * fibTblMgr_p,
    uint32_t poolIndex,
    uint32_t pool_size,
    struct kaps_jr1_ab_info *startBlk,
    uint32_t blkWidthInBits,
    enum kaps_jr1_dba_trigger_compression_mode mode,
    NlmReasonCode * o_reason)
{
    kaps_jr1_ipm *ipm_p = NULL;
    struct kaps_jr1_ab_info *cur_dup_ab;

    ipm_p = (kaps_jr1_ipm *) kaps_jr1_simple_dba_init(fibTblMgr_p->m_devMgr_p, 0, pool_size, 0, 0);


    if (ipm_p == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    ipm_p->pool_index = poolIndex;
    ipm_p->m_fib_tbl_mgr = fibTblMgr_p;
    ipm_p->m_ab_info = startBlk;

    if (startBlk->ab_init_done)
        return ipm_p;

    kaps_jr1_ipm_pvt_fill_ab_info(fibTblMgr_p, startBlk, blkWidthInBits);

    cur_dup_ab = startBlk->dup_ab;
    while (cur_dup_ab)
    {
        kaps_jr1_ipm_pvt_fill_ab_info(fibTblMgr_p, cur_dup_ab, blkWidthInBits);
        cur_dup_ab = cur_dup_ab->dup_ab;
    }

    return ipm_p;
}

NlmErrNum_t
kaps_jr1_ipm_add_entry_to_brick(
    kaps_jr1_ipm * ipm_p,
    kaps_jr1_pfx_bundle * pfxBundle_p,
    uint32_t length,
    NlmReasonCode * o_reason)
{
    struct kaps_jr1_device *device = ipm_p->kaps_jr1_simple_dba.device;
    uint32_t cur_brick_nr;
    struct kaps_jr1_ab_info *ab = ipm_p->m_ab_info;
    uint32_t i, gran, numPfxInBkt, found = 0;
    struct kaps_jr1_simple_dba_range_for_pref_len *r;

    

    
    for (cur_brick_nr = 0; cur_brick_nr < device->kaps_jr1_shadow->ab_to_small_bb[ab->ab_num].num_bricks; ++cur_brick_nr)
    {
        gran = device->kaps_jr1_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].sub_ab_gran;

        if (length < gran)
        {
            
            numPfxInBkt = device->kaps_jr1_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].upper_index
                - device->kaps_jr1_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].lower_index + 1;

            found = 0;
            for (i = 0; i < numPfxInBkt; ++i)
            {
                if (kaps_jr1_array_check_bit
                    (device->kaps_jr1_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].free_slot_bmp, i))
                {
                    
                    pfxBundle_p->m_nIndex =
                        device->kaps_jr1_shadow->ab_to_small_bb[ab->ab_num].sub_ab_bricks[cur_brick_nr].lower_index + i;

                    
                    kaps_jr1_array_set_bit(device->kaps_jr1_shadow->ab_to_small_bb[ab->ab_num].
                                       sub_ab_bricks[cur_brick_nr].free_slot_bmp, i, 0);

                    
                    ipm_p->kaps_jr1_simple_dba.pfx_bundles[pfxBundle_p->m_nIndex] = pfxBundle_p;

                    r = ipm_p->kaps_jr1_simple_dba.nodes[0].chunks;

                    if (!r)
                    {
                        ipm_p->kaps_jr1_simple_dba.nodes[0].chunks =
                            (struct kaps_jr1_simple_dba_range_for_pref_len *) device->alloc->xcalloc(device->alloc->cookie,
                                                                                                 1,
                                                                                                 sizeof(struct
                                                                                                        kaps_jr1_simple_dba_range_for_pref_len));
                        if (!ipm_p->kaps_jr1_simple_dba.nodes[0].chunks)
                            return KAPS_JR1_OUT_OF_MEMORY;

                        r = ipm_p->kaps_jr1_simple_dba.nodes[0].chunks;
                        r->prefix_len = kaps_jr1_ipm_recompute_len_in_simple_dba(ipm_p, length);

                        r->first_row = pfxBundle_p->m_nIndex;
                    }

                    if (r->first_row > pfxBundle_p->m_nIndex)
                        r->first_row = pfxBundle_p->m_nIndex;

                    if (r->last_row < pfxBundle_p->m_nIndex)
                        r->last_row = pfxBundle_p->m_nIndex;

                    kaps_jr1_reset_bit(&ipm_p->kaps_jr1_simple_dba.free, pfxBundle_p->m_nIndex);
                    kaps_jr1_set_bit(&ipm_p->kaps_jr1_simple_dba.nodes[0].used, pfxBundle_p->m_nIndex);

                    ipm_p->kaps_jr1_simple_dba.free_entries--;

                    found = 1;

                    break;
                }
            }

            if (found)
                break;
        }
    }

    
    if (!found)
    {
        kaps_jr1_assert(0, "Unable to insert IPT entry in the pool \n");
        *o_reason = NLMRSC_DBA_ALLOC_FAILED;
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ipm_add_entry(
    kaps_jr1_ipm * ipm_p,
    kaps_jr1_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t numRptBitsLoppedOff,
    NlmReasonCode * o_reason)
{
    kaps_jr1_status status;

    (void) numRptBitsLoppedOff;

    ipm_p->cb_result = NLMERR_OK;
    ipm_p->cb_reason = NLMRSC_REASON_OK;


    status = kaps_jr1_simple_dba_find_place_for_entry(&ipm_p->kaps_jr1_simple_dba, pfxBundle_p, rptId,
                                                  length, (int32_t *) & pfxBundle_p->m_nIndex);

    if (status != KAPS_JR1_OK)
    {
        if (status == KAPS_JR1_OUT_OF_MEMORY)
            *o_reason = NLMRSC_LOW_MEMORY;

        return NLMERR_FAIL;
    }


    if (pfxBundle_p->m_nIndex == KAPS_JR1_LSN_NEW_INDEX)
    {
        kaps_jr1_assert(0, "Unable to insert IPT entry in the pool \n");
        return NLMERR_FAIL;
    }

    *o_reason = ipm_p->cb_reason;

    return ipm_p->cb_result;

}


NlmErrNum_t
kaps_jr1_ipm_wb_add_entry(
    kaps_jr1_ipm * ipm_p,
    kaps_jr1_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t pos)
{
    uint32_t recomputed_len;

    pfxBundle_p->m_nIndex = pos;
    recomputed_len = kaps_jr1_ipm_recompute_len_in_simple_dba(ipm_p, length);
    kaps_jr1_simple_dba_place_entry_at_loc(&ipm_p->kaps_jr1_simple_dba, pfxBundle_p, rptId, recomputed_len, pos);


    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ipm_write_entry_to_hw(
    kaps_jr1_ipm * ipm_p,
    kaps_jr1_pfx_bundle * pfxBundle_p,
    uint32_t rptId,
    uint32_t length,
    uint32_t numRptBitsLoppedOff,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_jr1_fib_tbl_mgr_callback_fn_ptrs *fnptrs = (kaps_jr1_fib_tbl_mgr_callback_fn_ptrs *) fibTblMgr->m_devWriteCallBackFns;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t blockWidthInBits;
    uint8_t *trig_data;

    if (!fnptrs->m_writeABData)
        return errNum;

    blockWidthInBits = kaps_jr1_ab_get_blk_width(ipm_p->m_ab_info);

    ipm_p->m_is_cur_active = 1;

    trig_data = &pfxBundle_p->m_data[numRptBitsLoppedOff / 8];

    errNum = fnptrs->m_writeABData(fibTblMgr, 0, blockWidthInBits, ipm_p->m_ab_info,
                                   pfxBundle_p->m_nIndex, (uint8_t) rptId,
                                   (uint16_t) (blockWidthInBits - 1), 0,
                                   trig_data, (uint16_t) length, ipm_p->m_start_offset_1 / KAPS_JR1_BITS_IN_BYTE, o_reason);

    ipm_p->m_is_cur_active = 0;


    fibStats->numIPTWrites++;


    return errNum;

}

NlmErrNum_t
kaps_jr1_ipm_remove_entry(
    kaps_jr1_ipm * ipm_p,
    uint32_t pfxIndex,
    uint32_t rptId,
    uint32_t length,
    NlmReasonCode * o_reason)
{
    uint32_t recomputed_len;
    (void) o_reason;

    recomputed_len = kaps_jr1_ipm_recompute_len_in_simple_dba(ipm_p, length);
    kaps_jr1_simple_dba_free_entry(&ipm_p->kaps_jr1_simple_dba, rptId, recomputed_len, pfxIndex);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ipm_delete_entry_in_hw(
    kaps_jr1_ipm * ipm_p,
    uint32_t pfxIndex,
    uint32_t rptId,
    uint32_t length,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_jr1_fib_tbl_mgr_callback_fn_ptrs *fnptrs = (kaps_jr1_fib_tbl_mgr_callback_fn_ptrs *) fibTblMgr->m_devWriteCallBackFns;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t blockWidthInBits;
    uint8_t invalid_data[KAPS_JR1_LPM_KEY_MAX_WIDTH_1 / KAPS_JR1_BITS_IN_BYTE];

    (void) rptId;
    (void) length;

    if (!fnptrs->m_deleteABData)
        return errNum;

    blockWidthInBits = kaps_jr1_ab_get_blk_width(ipm_p->m_ab_info);

    ipm_p->m_is_cur_active = 1;

    if (ipm_p->mode != DBA_NO_TRIGGER_COMPRESSION)
    {
        ipm_p->m_is_delete_op = 1;

        kaps_jr1_memset(invalid_data, 0, KAPS_JR1_LPM_KEY_MAX_WIDTH_1 / KAPS_JR1_BITS_IN_BYTE);

        errNum = fnptrs->m_writeABData(fibTblMgr, 0, blockWidthInBits, ipm_p->m_ab_info,
                                       pfxIndex, (uint8_t) rptId,
                                       (uint16_t) (blockWidthInBits - 1), 0,
                                       invalid_data, (uint16_t) length, ipm_p->m_start_offset_1 / KAPS_JR1_BITS_IN_BYTE,
                                       o_reason);

        ipm_p->m_is_delete_op = 0;
    }
    else
    {

        errNum = fnptrs->m_deleteABData(fibTblMgr, 0, blockWidthInBits, ipm_p->m_ab_info, pfxIndex, o_reason);

    }
    ipm_p->m_is_cur_active = 0;


    fibStats->numIPTWrites++;


    return errNum;

}

void
kaps_jr1_ipm_delete_entry_at_location(
    void *self,
    uint32_t entry_nr)
{
    kaps_jr1_ipm *ipm_p = (kaps_jr1_ipm *) self;
    kaps_jr1_fib_tbl_mgr *fibTblMgr = (kaps_jr1_fib_tbl_mgr *) ipm_p->m_fib_tbl_mgr;
    struct NlmFibStats *fibStats = &fibTblMgr->m_curFibTbl->m_fibStats;
    kaps_jr1_fib_tbl_mgr_callback_fn_ptrs *fnptrs = (kaps_jr1_fib_tbl_mgr_callback_fn_ptrs *) fibTblMgr->m_devWriteCallBackFns;
    NlmReasonCode reason;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t blkWidthInBits;

    if (ipm_p->mode == DBA_NO_TRIGGER_COMPRESSION)
    {

        blkWidthInBits = kaps_jr1_ab_get_blk_width(ipm_p->m_ab_info);

        ipm_p->m_is_cur_active = 1;

        errNum = fnptrs->m_deleteABData(fibTblMgr, 0, blkWidthInBits, ipm_p->m_ab_info, entry_nr, &reason);

        ipm_p->m_is_cur_active = 0;

        fibStats->numAPTWrites++;

        if (errNum != NLMERR_OK)
            kaps_jr1_assert(0, "Error in DeleteEntryAtLocation while deleting IPT entry\n");
    }

}

NlmErrNum_t
kaps_jr1_ipm_fix_entry(
    kaps_jr1_ipm * ipm_p,
    uint32_t ab_num,
    uint32_t row_nr,
    NlmReasonCode * reason)
{
    kaps_jr1_pfx_bundle *pfxBundle_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    uint8_t *tempPtr;
    kaps_jr1_trie_node *iptNode_p, *rptNode_p;
    uint32_t numRptBitsLoppedOff;
    uint32_t location, blockWidthInBits;
    kaps_jr1_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;

    if (ab_num != ipm_p->m_ab_info->ab_num)
    {
        *reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    blockWidthInBits = kaps_jr1_ab_get_blk_width(ipm_p->m_ab_info);
    location = row_nr / (blockWidthInBits / KAPS_JR1_HW_MIN_DBA_WIDTH_1);
    pfxBundle_p = kaps_jr1_simple_dba_get_entry(&ipm_p->kaps_jr1_simple_dba, location);

    if (!pfxBundle_p)
    {
        kaps_jr1_ipm_delete_entry_at_location(ipm_p, location);
        return NLMERR_OK;
    }

    tempPtr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle_p);
    kaps_jr1_memcpy(&iptNode_p, tempPtr, sizeof(kaps_jr1_trie_node *));

    rptNode_p = iptNode_p->m_rptParent_p;

    numRptBitsLoppedOff = kaps_jr1_trie_get_num_rpt_bits_lopped_off(fibTblMgr->m_devMgr_p, rptNode_p->m_depth);

    errNum = kaps_jr1_ipm_write_entry_to_hw(ipm_p, pfxBundle_p, rptNode_p->m_rptId,
                                        iptNode_p->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, reason);

    return errNum;
}

NlmErrNum_t
kaps_jr1_ipm_create_entry_data(
    kaps_jr1_ipm * ipm_p,
    uint32_t ab_num,
    uint32_t row_nr,
    uint8_t * data,
    uint8_t * mask,
    uint8_t is_xy,
    NlmReasonCode * reason)
{
    kaps_jr1_pfx_bundle *pfxBundle_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    uint8_t *tempPtr;
    kaps_jr1_trie_node *iptNode_p, *rptNode_p;
    uint32_t numRptBitsLoppedOff;
    uint32_t location, blockWidthInBits;
    uint32_t byte_offset, start_bit;
    uint8_t num_bytes;
    uint32_t data_len;
    kaps_jr1_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;
    uint32_t rptId;

    if (ab_num != ipm_p->m_ab_info->ab_num)
    {
        *reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    blockWidthInBits = kaps_jr1_ab_get_blk_width(ipm_p->m_ab_info);
    location = row_nr / (blockWidthInBits / KAPS_JR1_HW_MIN_DBA_WIDTH_1);
    pfxBundle_p = kaps_jr1_simple_dba_get_entry(&ipm_p->kaps_jr1_simple_dba, location);

    if ((!pfxBundle_p && !ipm_p->m_is_cur_active) || ipm_p->m_is_delete_op)
    {
        if (is_xy)
        {
            byte_offset = ipm_p->m_start_offset_1 / KAPS_JR1_BITS_IN_BYTE;
            kaps_jr1_memset(&data[byte_offset], 0xFF, ipm_p->m_num_bytes);
            kaps_jr1_memset(&mask[byte_offset], 0xFF, ipm_p->m_num_bytes);
        }
        return NLMERR_OK;
    }

    if (!ipm_p->m_is_cur_active)
    {

        tempPtr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle_p);
        kaps_jr1_memcpy(&iptNode_p, tempPtr, sizeof(kaps_jr1_trie_node *));

        rptNode_p = iptNode_p->m_rptParent_p;
        rptId = rptNode_p->m_rptId;

        
        if (rptNode_p->m_trie_p->m_isCopyIptAndAptToNewPoolInProgress && !rptNode_p->m_isRptNode)
        {

            if (rptNode_p->m_trie_p->m_trie_global->m_rptOp == KAPS_JR1_RPT_MOVE)
            {
                rptId = rptNode_p->m_trie_p->m_trie_global->m_oldRptId;
            }
            else
            {

                
                rptNode_p = rptNode_p->m_parent_p;
                rptId = rptNode_p->m_rptId;
            }
        }

        numRptBitsLoppedOff = kaps_jr1_trie_get_num_rpt_bits_lopped_off(fibTblMgr->m_devMgr_p, rptNode_p->m_depth);

        byte_offset = ipm_p->m_start_offset_1 / KAPS_JR1_BITS_IN_BYTE;
        data_len = iptNode_p->m_depth - numRptBitsLoppedOff;
        num_bytes = (uint8_t) ((data_len + 7) >> 3);

        data[byte_offset] = rptId;
        kaps_jr1_memcpy(&data[byte_offset + 1], &pfxBundle_p->m_data[numRptBitsLoppedOff / 8], num_bytes);
        start_bit = blockWidthInBits - 1 - ipm_p->m_start_offset_1;

        kaps_jr1_FillZeroes(mask, blockWidthInBits / KAPS_JR1_BITS_IN_BYTE, start_bit, (start_bit - (data_len - 1) - 8));
    }

    if (is_xy)
    {
        uint8_t data_d[KAPS_JR1_LPM_KEY_MAX_WIDTH_1 / KAPS_JR1_BITS_IN_BYTE];
        uint8_t data_m[KAPS_JR1_LPM_KEY_MAX_WIDTH_1 / KAPS_JR1_BITS_IN_BYTE];

        byte_offset = ipm_p->m_start_offset_1 / KAPS_JR1_BITS_IN_BYTE;

        kaps_jr1_memcpy(data_d, &data[byte_offset], ipm_p->m_num_bytes);
        kaps_jr1_memcpy(data_m, &mask[byte_offset], ipm_p->m_num_bytes);

        kaps_jr1_convert_dm_to_xy(data_d, data_m, &data[byte_offset], &mask[byte_offset], ipm_p->m_num_bytes);
    }
    return errNum;
}

NlmErrNum_t
kaps_jr1_ipm_init_rpt(
    kaps_jr1_ipm * ipm_p,
    uint32_t rptId,
    NlmReasonCode * o_reason)
{
    kaps_jr1_status status = kaps_jr1_simple_dba_init_node(&ipm_p->kaps_jr1_simple_dba, rptId,
                                                        kaps_jr1_ipm_ipt_shuffle_callback);

    if (status != KAPS_JR1_OK)
    {
        if (status == KAPS_JR1_OUT_OF_MEMORY)
            *o_reason = NLMRSC_LOW_MEMORY;

        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ipm_free_rpt(
    kaps_jr1_ipm * ipm_p,
    uint32_t rptId)
{
    kaps_jr1_simple_dba_free_node(&ipm_p->kaps_jr1_simple_dba, rptId);

    return NLMERR_OK;
}

void
kaps_jr1_ipm_destroy(
    kaps_jr1_nlm_allocator * alloc_p,
    kaps_jr1_ipm * ipm)
{
    kaps_jr1_simple_dba_destroy(&ipm->kaps_jr1_simple_dba);
}

uint32_t
kaps_jr1_ipm_verify(
    kaps_jr1_ipm * ipm_p,
    uint32_t rptId)
{
    uint32_t num_entries;

    num_entries = kaps_jr1_simple_dba_verify(&ipm_p->kaps_jr1_simple_dba, rptId, kaps_jr1_get_pfx_bundle_prio_length);

    return num_entries;
}



int32_t
kaps_jr1_ipm_wb_store_ipt_entries(
    kaps_jr1_ipm * ipm_p,
    uint32_t rpt_id,
    uint32_t * nv_offset,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle)
{
    struct kaps_jr1_simple_dba_range_for_pref_len *r;
    struct kaps_jr1_wb_ipt_entry_info ipt_entry;
    kaps_jr1_pfx_bundle *pfx_bundle;
    uint32_t len_in_bytes = 0, pos;
    uint8_t *pfx_data;
    void *tempPtr;
    kaps_jr1_trie_node *trienode;
    int32_t num_ipt_entries = 0;
    uint32_t rpt_lopoff_1;
    kaps_jr1_fib_tbl_mgr *fibTblMgr = ipm_p->m_fib_tbl_mgr;

    r = ipm_p->kaps_jr1_simple_dba.nodes[rpt_id].chunks;

    kaps_jr1_ipm_verify(ipm_p, rpt_id);

    while (r)
    {
        for (pos = r->first_row; pos <= r->last_row; pos++)
        {
            pfx_bundle = ipm_p->kaps_jr1_simple_dba.pfx_bundles[pos];
            if (pfx_bundle == NULL)     
                continue;

            tempPtr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(pfx_bundle);
            kaps_jr1_memcpy(&trienode, tempPtr, sizeof(kaps_jr1_trie_node *));

            if (trienode->m_rptParent_p->m_rptId != rpt_id)
                continue;

            pfx_data = KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(pfx_bundle);
            len_in_bytes = KAPS_JR1_PFX_BUNDLE_GET_PFX_SIZE(pfx_bundle);
            len_in_bytes = KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(len_in_bytes);
            kaps_jr1_memcpy(ipt_entry.ipt_entry, pfx_data, len_in_bytes);

            rpt_lopoff_1 =
                kaps_jr1_trie_get_num_rpt_bits_lopped_off(fibTblMgr->m_devMgr_p, trienode->m_rptParent_p->m_depth);
            ipt_entry.ipt_entry_len_1 = pfx_bundle->m_nPfxSize - rpt_lopoff_1;
            ipt_entry.is_reserved_160b_trig = trienode->m_isReserved160bTrig;

            ipt_entry.blk_num = ipm_p->m_ab_info->ab_num;
            ipt_entry.addr = pos;

            if (NLMERR_OK != kaps_jr1_lsn_mc_wb_prepare_lsn_info(trienode->m_lsn_p, &ipt_entry.lsn_info))
                return -1;

            len_in_bytes = sizeof(ipt_entry);
            if (0 != write_fn(handle, (uint8_t *) & ipt_entry, len_in_bytes, *nv_offset))
                return -1;
            num_ipt_entries++;

            *nv_offset += len_in_bytes;
            if (NLMERR_OK != kaps_jr1_lsn_mc_wb_store_prefixes(trienode->m_lsn_p, nv_offset, write_fn, handle))
                return -1;

        }

        r = r->next;    
    }

    return num_ipt_entries;
}
