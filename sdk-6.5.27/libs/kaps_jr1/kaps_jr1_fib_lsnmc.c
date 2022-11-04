

#include "kaps_jr1_utility.h"
#include "kaps_jr1_fib_lsnmc.h"
#include "kaps_jr1_fib_lsnmc_hw.h"
#include "kaps_jr1_fib_cmn_pfxbundle.h"
#include "kaps_jr1_fib_cmn_seqgen.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_fib_poolmgr.h"
#include "kaps_jr1_uda_mgr.h"
#include "kaps_jr1_algo_hw.h"
#include "kaps_jr1_ad_internal.h"
#include "kaps_jr1_ix_mgr.h"
#include "kaps_jr1_handle.h"
#include "kaps_jr1_fib_trie.h"

kaps_jr1_status
kaps_jr1_fib_hb_cb(
    void *client,
    struct kaps_jr1_entry *entry,
    uint32_t old_hb_index,
    uint32_t new_hb_index)
{
    struct kaps_jr1_hb_db *hb_db = client;
    struct kaps_jr1_device *device = hb_db->db_info.device;
    struct kaps_jr1_lpm_entry *lpm_entry = (struct kaps_jr1_lpm_entry *) entry;
    struct kaps_jr1_hb *hb = NULL;
    uint8_t *nv_ptr;
    struct kaps_jr1_lpm_cr_entry_info *shuffle_entry = NULL;
    struct kaps_jr1_db *db;
    struct kaps_jr1_aging_entry *active_aging_table;

    KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, entry, db);

    active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);

    nv_ptr = device->nv_ptr;

    if (nv_ptr)
    {
        uint32_t offset;

        offset = kaps_jr1_db_entry_get_nv_offset(db, entry->user_handle);

        nv_ptr += offset;
        shuffle_entry = (struct kaps_jr1_lpm_cr_entry_info *) nv_ptr;
        shuffle_entry->hb_bit_no = new_hb_index;
        if (lpm_entry->hb_user_handle)
            shuffle_entry->hb_user_handle = lpm_entry->hb_user_handle;
        else
            shuffle_entry->hb_user_handle = 0;
    }

    if (!lpm_entry->hb_user_handle)
    {
        if (old_hb_index != KAPS_JR1_LSN_NEW_INDEX)
            kaps_jr1_sassert(active_aging_table[old_hb_index].entry == NULL);

        if (new_hb_index != KAPS_JR1_LSN_NEW_INDEX)
            kaps_jr1_sassert(active_aging_table[new_hb_index].entry == NULL);

        return KAPS_JR1_OK;
    }

    KAPS_JR1_WB_HANDLE_READ_LOC((&hb_db->db_info), (&hb), (uintptr_t) lpm_entry->hb_user_handle);

    if (old_hb_index != KAPS_JR1_LSN_NEW_INDEX)
        kaps_jr1_sassert(hb->bit_no == old_hb_index);

    if (old_hb_index == new_hb_index)
    {
        kaps_jr1_sassert(old_hb_index != KAPS_JR1_LSN_NEW_INDEX);
        kaps_jr1_sassert(active_aging_table[old_hb_index].entry ==
                     KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, entry->user_handle));
        return KAPS_JR1_OK;
    }

    kaps_jr1_sassert(active_aging_table[new_hb_index].entry == NULL);

    if (old_hb_index != KAPS_JR1_LSN_NEW_INDEX)
        kaps_jr1_sassert(active_aging_table[old_hb_index].entry ==
                     KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, entry->user_handle));

    active_aging_table[new_hb_index].entry = KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, entry->user_handle);
    hb->bit_no = new_hb_index;

    if (old_hb_index != KAPS_JR1_LSN_NEW_INDEX)
    {
        active_aging_table[new_hb_index].num_idles = active_aging_table[old_hb_index].num_idles;
        active_aging_table[new_hb_index].intermediate_hb_val = active_aging_table[old_hb_index].intermediate_hb_val;

        active_aging_table[old_hb_index].entry = NULL;
        active_aging_table[old_hb_index].num_idles = 0;
        active_aging_table[old_hb_index].intermediate_hb_val = 0;
    }
    else
    {
        active_aging_table[new_hb_index].num_idles = 0;
    }

    return KAPS_JR1_OK;
}


struct kaps_jr1_ad_db *
kaps_jr1_lsn_mc_get_ad_db(
    kaps_jr1_lsn_mc *curLsn,
    kaps_jr1_lpm_lpu_brick *curBrick)
{
    struct kaps_jr1_ad_db *ad_db = NULL;

    if (curLsn->m_pSettings->m_isPerLpuGran) {
        ad_db = curBrick->m_ixInfo->ad_info->ad_db;
    } else {
        ad_db = curLsn->m_ixInfo->ad_info->ad_db;
    }

    return ad_db;
}



struct kaps_jr1_ix_mgr*
kaps_jr1_lsn_mc_get_ix_mgr_for_lsn_pfx(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_ad_db *ad_db,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum)
{
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    struct kaps_jr1_ix_mgr *ix_mgr;
    uint32_t daisy_chain_id;

    daisy_chain_id = kaps_jr1_get_daisy_chain_id(device, self, uda_chunk, brickNum);

    ix_mgr = ad_db->mgr[device->core_id][daisy_chain_id];

    return ix_mgr;
}




void
kaps_jr1_lsn_mc_incr_num_bricks_allocated_for_all_lsns(
    kaps_jr1_lsn_mc_settings * settings,
    uint32_t size)
{
    settings->m_numAllocatedBricksForAllLsns += size;
}

void
kaps_jr1_lsn_mc_decr_num_bricks_allocated_for_all_lsns(
    kaps_jr1_lsn_mc_settings * settings,
    uint32_t size)
{
    if (settings->m_numAllocatedBricksForAllLsns > size)
    {
        settings->m_numAllocatedBricksForAllLsns -= size;
    }
    else
    {
        settings->m_numAllocatedBricksForAllLsns = 0;
    }
}


NlmErrNum_t
kaps_jr1_lsn_mc_check_joined_udc_alloc(
    kaps_jr1_lsn_mc * self,
    uint32_t rqtNumUdaBricks,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    if (self->m_mlpMemInfo->size > rqtNumUdaBricks)
    {

        if (self->m_mlpMemInfo->size > settings->m_maxLpuPerLsn)
        {
            kaps_jr1_assert(0, "UDA chunk size is greater than Max LPU per LSN");
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }

        if (self->m_mlpMemInfo->size != rqtNumUdaBricks + 1)
        {
            kaps_jr1_assert(0, "Recheck if we are getting the correct UDA chunk");
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }
    }

    return NLMERR_OK;
}

static kaps_jr1_lsn_mc_settings *
kaps_jr1_lsn_mc_settings_pvt_ctor(
    kaps_jr1_lsn_mc_settings * self,
    kaps_jr1_nlm_allocator * pAlloc,
    struct kaps_jr1_uda_mgr **pMlpMemMgr)
{
    uint32_t i;

    self->m_pAlloc = pAlloc;

    self->m_pMlpMemMgr = pMlpMemMgr;

    for (i = 0; i < KAPS_JR1_MAX_NUM_STORED_LSN_INFO; ++i)
    {
        self->m_pTmpMlpMemInfoBuf[i] =
            (struct uda_mem_chunk *) kaps_jr1_nlm_allocator_calloc(pAlloc, 1, sizeof(struct uda_mem_chunk));
    }

    return self;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_settings_pvt_fill_cmp_lookup_table(
    kaps_jr1_lsn_mc_settings * self)
{
    uint32_t cmpLookupTable[] = {
        0, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  
    };

    kaps_jr1_memcpy(self->m_cmpLookupTable, cmpLookupTable, sizeof(cmpLookupTable));

    return NLMERR_OK;
}


kaps_jr1_lsn_mc_settings *
kaps_jr1_lsn_mc_settings_create(
    kaps_jr1_nlm_allocator * pAlloc,
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_device * device,
    struct kaps_jr1_uda_mgr ** pMlpMemMgr,
    kaps_jr1_fib_prefix_index_changed_app_cb_t pAppCB,
    void *pAppData,
    uint32_t numLpuPerRow,
    uint32_t numJoinedSets,
    uint32_t numJoinedBricksInOneSet)
{
    kaps_jr1_lsn_mc_settings *ret;
    uint16_t curGranIx, length;
    uint32_t i;
    uint32_t ad_width_per_pfx_1;
    uint32_t max_encoded_pfx_len;
    struct kaps_jr1_db *db = fibTbl->m_db;
    struct kaps_jr1_ad_db *ad_db;
    uint32_t num_bit_combinations = 1;
    uint32_t num_pfx_based_on_gran, num_ad_dbs = 0, alloc_max_region;
    uint32_t usable_bkt_width_1;
    uint32_t maxPfxForCurGranIx;
    uint32_t usable_bkt_width_for_in_place_1;
    uint32_t choose_inplace_for_caution_height;
    uint32_t num_20b_ad_dbs;
    uint32_t are_all_20b_ad_dbs_inplace;
    uint32_t max_num_pfx_in_480b;
    uint16_t *lpm_gran_array = NULL;

    (void) usable_bkt_width_1;
    (void) ad_width_per_pfx_1;
    (void) choose_inplace_for_caution_height;


    ret = (kaps_jr1_lsn_mc_settings *) kaps_jr1_nlm_allocator_calloc(pAlloc, 1, sizeof(kaps_jr1_lsn_mc_settings));

    kaps_jr1_lsn_mc_settings_pvt_ctor(ret, pAlloc, pMlpMemMgr);

    ret->m_lsnid = 0;

    ret->m_maxLpuPerLsn = numLpuPerRow;
    if (ret->m_maxLpuPerLsn > device->hw_res->max_lpu_per_db)
    {
        ret->m_maxLpuPerLsn = device->hw_res->max_lpu_per_db;
    }

    alloc_max_region = kaps_jr1_uda_mgr_max_region_width(ret->m_pMlpMemMgr[0]);

    if (alloc_max_region != 0)
    {
        if (alloc_max_region < ret->m_maxLpuPerLsn)
            ret->m_maxLpuPerLsn = alloc_max_region;
    }

    ret->m_initialMaxLpuPerLsn = ret->m_maxLpuPerLsn;

    ret->m_pAppCB = pAppCB;
    ret->m_pAppData = pAppData;

    ret->m_device = device;
    ret->m_fibTbl = fibTbl;

    ret->m_numJoinedSets = numJoinedSets;
    ret->m_numJoinedBricksInOneSet = numJoinedBricksInOneSet;
    ret->m_tryLsnPush = 1;

    if (db->common_info->hb_info.hb)
    {
        if (device->type == KAPS_JR1_DEVICE)
        {
            if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID
                || device->id == KAPS_JR1_QUX_DEVICE_ID
                || device->id == KAPS_JR1_QUMRAN_DEVICE_ID)
            {
                ret->m_pHitBitCB = kaps_jr1_fib_hb_cb;
                ret->m_pHitBitCBData = db->common_info->hb_info.hb;
                ret->m_areHitBitsPresent = 1;

                ret->m_areIPTHitBitsPresent = 1;
                if (device->id == KAPS_JR1_QUMRAN_DEVICE_ID)
                {
                    ret->m_areIPTHitBitsPresent = 0;
                }

            }
        }
    }

    num_20b_ad_dbs = 0;
    are_all_20b_ad_dbs_inplace = 1;
    ad_db = (struct kaps_jr1_ad_db *) db->common_info->ad_info.ad;
    while (ad_db)
    {
        num_ad_dbs++;
        if (ad_db->user_width_1 == 20)
        {
            num_20b_ad_dbs++;
            if (ad_db->db_info.hw_res.ad_res->ad_type != KAPS_JR1_AD_TYPE_INPLACE)
            {
                are_all_20b_ad_dbs_inplace = 0;
            }
        }

        ad_db = ad_db->next;
    }

    
    kaps_jr1_lsn_mc_settings_pvt_fill_cmp_lookup_table(ret);

    max_encoded_pfx_len = KAPS_JR1_LPM_KEY_MAX_WIDTH_1;
    if (device->type == KAPS_JR1_DEVICE)
    {
        max_encoded_pfx_len = KAPS_JR1_HW_LPM_MAX_GRAN;
    }


    ret->lpm_num_gran = device->hw_res->lpm_num_gran;
    lpm_gran_array = device->hw_res->lpm_gran_array;


    
    curGranIx = 0;
    for (length = 0; length <= max_encoded_pfx_len; ++length)
    {
        
        while (length > lpm_gran_array[curGranIx])
            curGranIx++;
        ret->m_lengthToGran[length] = lpm_gran_array[curGranIx];
        ret->m_lengthToGranIx[length] = curGranIx;
    }

    
    curGranIx = 0;
    if (device->hw_res->lpm_middle_level_num_gran)
    {
        for (length = 0; length <= max_encoded_pfx_len; ++length)
        {
            if (length > device->hw_res->lpm_middle_level_gran_array[curGranIx])
                curGranIx++;

            ret->m_middleLevelLengthToGran[length] = device->hw_res->lpm_middle_level_gran_array[curGranIx];
            ret->m_middleLevelLengthToGranIx[length] = curGranIx;
        }

    }

    if (device->type == KAPS_JR1_DEVICE)
    {
        ret->m_isPerLpuGran = 1;
        ret->m_isAptPresent = 0;
        ret->m_lpu_brick_width_1 = KAPS_JR1_BKT_WIDTH_1 * device->hw_res->num_bb_in_one_bb_cascade;
        ret->m_bdata_ads_size_1 = 20;

        ret->m_isHardwareMappedIx = 1;
        ret->m_isFullWidthLsn = 0;
        ret->m_isShrinkEnabled = 1;
        ret->m_isAncestorLsnMergeEnabled = 1;
        ret->m_treat_20b_ad_as_inplace = 1;

        if (!are_all_20b_ad_dbs_inplace)
            ret->m_treat_20b_ad_as_inplace = 0;

        ret->m_isMultiBrickAlloc = 0;

        if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
        {
            ret->m_isPerLpuGran = 0;
            ret->m_isHardwareMappedIx = 1;
            ret->m_isAncestorLsnMergeEnabled = 1;

            if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM0
                || device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0)
            {
                ret->m_isFullWidthLsn = 1;
                ret->m_isShrinkEnabled = 0;
            }
            else
            {
                ret->m_isMultiBrickAlloc = 1;
                ret->m_numMultiBricks = 4;
                ret->m_isShrinkEnabled = 1;
            }
        }

    }

    if (device->hw_res->no_overflow_lpm || fibTbl->m_db->hw_res.db_res->num_dba_dt == 0)
    {
        ret->m_isAptPresent = 0;
        ret->m_isUnderAllocateLpuIx = 0;
    }

    ret->m_maxNumPfxInAnyBrick = 0;


    
    for (i = 0; i < ret->lpm_num_gran; ++i)
    {
        max_num_pfx_in_480b = 16;

        usable_bkt_width_for_in_place_1 = KAPS_JR1_BKT_WIDTH_1;

        ret->m_max20bInPlacePfxInBrickForGranIx[i] = usable_bkt_width_for_in_place_1
                 / (lpm_gran_array[i] + device->hw_res->inplace_width_1);

        if (ret->m_max20bInPlacePfxInBrickForGranIx[i] > max_num_pfx_in_480b)
            ret->m_max20bInPlacePfxInBrickForGranIx[i] = max_num_pfx_in_480b;

        ret->m_max20bInPlacePfxInBrickForGranIx[i] *= device->hw_res->num_bb_in_one_bb_cascade;
    }

    if (ret->m_max20bInPlacePfxInBrickForGranIx[0] > ret->m_maxNumPfxInAnyBrick)
        ret->m_maxNumPfxInAnyBrick = ret->m_max20bInPlacePfxInBrickForGranIx[0];


    choose_inplace_for_caution_height = 1;

    if (!ret->m_treat_20b_ad_as_inplace)
        choose_inplace_for_caution_height = 0;


    
    ret->m_bottom_up_caution_height = lpm_gran_array[0];
    for (length = 0; length < lpm_gran_array[1]; ++length)
    {
        num_bit_combinations = 1 << length;
        curGranIx = ret->m_lengthToGranIx[length + 1];  

        maxPfxForCurGranIx = ret->m_max20bInPlacePfxInBrickForGranIx[curGranIx];

        num_pfx_based_on_gran = maxPfxForCurGranIx * ret->m_maxLpuPerLsn;

        if (num_bit_combinations >= num_pfx_based_on_gran)
        {
            ret->m_bottom_up_caution_height = length;
            break;
        }
    }


    if (device->type == KAPS_JR1_DEVICE)
    {
        ret->m_granIxToFormatValue[0] = 1;
        ret->m_granIxToFormatValue[1] = 2;
        ret->m_granIxToFormatValue[2] = 3;
        ret->m_granIxToFormatValue[3] = 4;
        ret->m_granIxToFormatValue[4] = 5;
        ret->m_granIxToFormatValue[5] = 6;
        ret->m_granIxToFormatValue[6] = 7;
        ret->m_granIxToFormatValue[7] = 8;
        ret->m_granIxToFormatValue[8] = 9;
        ret->m_granIxToFormatValue[9] = 10;
        ret->m_granIxToFormatValue[10] = 11;
    }

    if (device->hw_res->lpm_middle_level_num_gran)
    {
        ret->m_middleLevelGranIxToFormatValue[0] = 1;
        ret->m_middleLevelGranIxToFormatValue[1] = 2;
        ret->m_middleLevelGranIxToFormatValue[2] = 3;
        ret->m_middleLevelGranIxToFormatValue[3] = 4;
        ret->m_middleLevelGranIxToFormatValue[4] = 5;
        ret->m_middleLevelGranIxToFormatValue[5] = 6;
        ret->m_middleLevelGranIxToFormatValue[6] = 7;
        ret->m_middleLevelGranIxToFormatValue[7] = 8;
        ret->m_middleLevelGranIxToFormatValue[8] = 9;
        ret->m_middleLevelGranIxToFormatValue[9] = 10;
        ret->m_middleLevelGranIxToFormatValue[10] = 11;
    }

    
    if (ret->m_isFullWidthLsn)
        kaps_jr1_sassert(ret->m_isPerLpuGran == 0);

    
    kaps_jr1_memset(ret->zero_value_array, 0, KAPS_JR1_BKT_WIDTH_8);

    return ret;
}

static void
kaps_jr1_lsn_mc_settings_pvt_dtor(
    kaps_jr1_lsn_mc_settings * self)
{
    uint32_t i;
    for (i = 0; i < KAPS_JR1_MAX_NUM_STORED_LSN_INFO; ++i)
    {
        if (self->m_pTmpMlpMemInfoBuf[i])
        {
            kaps_jr1_nlm_allocator_free(self->m_pAlloc, self->m_pTmpMlpMemInfoBuf[i]);
        }
    }
}

void
kaps_jr1_lsn_mc_settings_destroy(
    kaps_jr1_lsn_mc_settings * self)
{
    if (self)
    {
        kaps_jr1_lsn_mc_settings_pvt_dtor(self);
        kaps_jr1_nlm_allocator_free(self->m_pAlloc, (void *) self);
    }
}

NlmErrNum_t
kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(
    kaps_jr1_lsn_mc * self,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lpm_lpu_brick *newBrick = NULL, *curBrick, *lastBrick;
    uint32_t brickIter;
    NlmErrNum_t errNum;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;
    kaps_jr1_nlm_allocator *alloc_p = settings->m_pAlloc;
    struct kaps_jr1_ad_db *ad_db = NULL;

    if (!settings->m_isJoinedUdc)
        return NLMERR_OK;

    if (self->m_mlpMemInfo->size <= self->m_numLpuBricks)
        return NLMERR_OK;

    kaps_jr1_assert(self->m_mlpMemInfo->size == self->m_numLpuBricks + 1, "UDA chunk has more than 1 extra brick");

    

    curBrick = self->m_lpuList;
    lastBrick = NULL;
    while (curBrick)
    {
        lastBrick = curBrick;
        curBrick = curBrick->m_next_p;
    }

    newBrick = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_lpm_lpu_brick));
    if (!newBrick)
    {
        return NLMERR_OK;
    }

    
    newBrick->m_gran = lastBrick->m_gran;
    newBrick->m_granIx = lastBrick->m_granIx;
    ad_db = lastBrick->ad_db;

    newBrick->m_maxCapacity =
        kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, lastBrick->meta_priority, lastBrick->m_gran);

    newBrick->m_pfxes = kaps_jr1_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_jr1_pfx_bundle *));
    if (!newBrick->m_pfxes)
    {
        kaps_jr1_nlm_allocator_free(alloc_p, newBrick);
        return NLMERR_OK;
    }

    newBrick->ad_db = ad_db;
    newBrick->meta_priority = lastBrick->meta_priority;
    newBrick->m_next_p = NULL;

    brickIter = self->m_numLpuBricks;
    if (self->m_pSettings->m_isPerLpuGran)
    {
        uint32_t ixRqtSize = newBrick->m_maxCapacity;

        kaps_jr1_lsn_mc_handle_under_alloc(self, newBrick, brickIter, &ixRqtSize);

        errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, newBrick, brickIter, ixRqtSize, ad_db, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_jr1_nlm_allocator_free(alloc_p, newBrick->m_pfxes);
            kaps_jr1_nlm_allocator_free(alloc_p, newBrick);
            return NLMERR_OK;
        }
    }

    
    lastBrick->m_next_p = newBrick;

    self->m_numLpuBricks++;

    kaps_jr1_lsn_mc_incr_num_bricks_allocated_for_all_lsns(settings, 1);

    self->m_nLsnCapacity += newBrick->m_maxCapacity;

    return NLMERR_OK;
}



void
kaps_jr1_lsn_mc_check_and_free_ix(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_ix_chunk *ixChunk,
    struct uda_mem_chunk *uda_chunk,
    uint32_t brickNum)
{
    struct kaps_jr1_ix_mgr *ix_mgr;

    if (self->m_pSettings->m_isHardwareMappedIx)
    {
        if (ixChunk->ad_info)
        {
            kaps_jr1_nlm_allocator_free(self->m_pSettings->m_pAlloc, ixChunk->ad_info);
        }
        kaps_jr1_nlm_allocator_free(self->m_pSettings->m_pAlloc, ixChunk);
        return;
    }

    ix_mgr = kaps_jr1_lsn_mc_get_ix_mgr_for_lsn_pfx(self, ixChunk->ad_info->ad_db, uda_chunk, brickNum);

    kaps_jr1_ix_mgr_free(ix_mgr, ixChunk);
}

uint16_t
kaps_jr1_lsn_mc_get_next_max_gran(
    kaps_jr1_lsn_mc_settings * settings,
    uint16_t gran,
    uint16_t next)
{
    uint16_t granIx = settings->m_lengthToGranIx[gran];

    if (gran > 128)
        return 0;

    granIx += next;

    if (granIx >= settings->lpm_num_gran)
        return 0;

    return settings->m_granIxToGran[granIx];
}

uint16_t
kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(
    kaps_jr1_lsn_mc_settings * settings,
    struct kaps_jr1_ad_db * ad_db,
    uint32_t has_reservedSlot,
    uint16_t meta_priority,
    uint16_t gran)
{
    uint16_t granIx = settings->m_lengthToGranIx[gran];
    uint16_t maxNumPfxInBrickForGran = 0;

    maxNumPfxInBrickForGran = settings->m_max20bInPlacePfxInBrickForGranIx[granIx];

    return maxNumPfxInBrickForGran;
}


uint32_t
kaps_jr1_lsn_mc_calc_max_triggers_in_brick(
    kaps_jr1_lsn_mc_settings * settings,
    uint16_t gran)
{
    uint16_t granIx = settings->m_middleLevelLengthToGranIx[gran];

    return settings->m_maxTriggersInBrickForGranIx[granIx];
}


uint32_t
kaps_jr1_lsn_mc_get_ix_mgr_index(
    kaps_jr1_lsn_mc * self,
    uint32_t relative_index)
{
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    uint32_t actual_index = 0;
    if (self->m_pSettings->m_isPerLpuGran)
    {
        curLpuBrick = self->m_lpuList;
        while (curLpuBrick)
        {
            if (relative_index < curLpuBrick->m_maxCapacity)
            {
                actual_index = curLpuBrick->m_ixInfo->start_ix + relative_index;
                break;
            }
            relative_index -= curLpuBrick->m_maxCapacity;
            curLpuBrick = curLpuBrick->m_next_p;
        }
        if (curLpuBrick)
            return actual_index;
        else
        {
            kaps_jr1_sassert(0);
            return 0;
        }
    }
    else
    {
        actual_index = self->m_nAllocBase + relative_index;
        return actual_index;
    }
}

uint32_t
kaps_jr1_lsn_mc_get_rel_index_in_lsn(
    kaps_jr1_lsn_mc * self,
    uint32_t actual_index)
{
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    uint32_t relative_index = 0;
    if (self->m_pSettings->m_isPerLpuGran)
    {
        curLpuBrick = self->m_lpuList;
        while (curLpuBrick)
        {
            uint32_t maxPfxInLpu = curLpuBrick->m_maxCapacity;
            if (curLpuBrick->m_underAllocatedIx)
                maxPfxInLpu = curLpuBrick->m_ixInfo->size;
            if (actual_index >= curLpuBrick->m_ixInfo->start_ix
                && actual_index < curLpuBrick->m_ixInfo->start_ix + maxPfxInLpu)
            {
                relative_index += actual_index - curLpuBrick->m_ixInfo->start_ix;
                break;
            }
            relative_index += curLpuBrick->m_maxCapacity;
            curLpuBrick = curLpuBrick->m_next_p;
        }
        if (curLpuBrick)
            return relative_index;
        else
        {
            kaps_jr1_sassert(0);
            return 0;
        }
    }
    else
    {
        relative_index = actual_index - self->m_nAllocBase;
        return relative_index;
    }
}

void
kaps_jr1_lsn_mc_free_single_lpu_brick(
    kaps_jr1_lpm_lpu_brick * curLpuBrick,
    kaps_jr1_nlm_allocator * alloc)
{
    kaps_jr1_nlm_allocator_free(alloc, curLpuBrick->m_pfxes);
    kaps_jr1_nlm_allocator_free(alloc, curLpuBrick);
}

void
kaps_jr1_lsn_mc_free_lpu_bricks(
    kaps_jr1_lpm_lpu_brick * curLpuBrick,
    kaps_jr1_nlm_allocator * alloc)
{
    kaps_jr1_lpm_lpu_brick *temp;

    while (curLpuBrick)
    {
        temp = curLpuBrick->m_next_p;

        kaps_jr1_lsn_mc_free_single_lpu_brick(curLpuBrick, alloc);

        curLpuBrick = temp;
    }
}

static kaps_jr1_lsn_mc *
kaps_jr1_lsn_mc_pvt_ctor(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lsn_mc_settings * settings)
{
    self->m_pSettings = settings;
    self->m_bIsNewLsn = 1;

    return self;
}

kaps_jr1_lsn_mc *
kaps_jr1_lsn_mc_create(
    kaps_jr1_lsn_mc_settings * settings,
    struct kaps_jr1_lpm_trie * pTrie,
    uint32_t depth)
{
    kaps_jr1_lsn_mc *ret = (kaps_jr1_lsn_mc *) kaps_jr1_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_jr1_lsn_mc));

    kaps_jr1_lsn_mc_pvt_ctor(ret, settings);

    ret->m_pTrie = pTrie;
    ret->m_pTbl = pTrie->m_tbl_ptr;

    ret->m_nDepth = depth;
    ret->m_nLopoff = depth;

    ret->m_lsnId = settings->m_lsnid++;

    return ret;
}

NlmErrNum_t
kaps_jr1_lsn_mc_pvt_allocate_mlp(
    kaps_jr1_lsn_mc * self,
    uint8_t deviceNum,
    uint8_t rqtNumLpu,
    struct uda_mem_chunk ** mlpMemInfo_pp,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_jr1_uda_mgr *pMlpMemMgr = self->m_pSettings->m_pMlpMemMgr[self->m_devid];
    kaps_jr1_status status;

    status = kaps_jr1_uda_mgr_alloc(pMlpMemMgr, rqtNumLpu, self->m_pTbl->m_fibTblMgr_p->m_devMgr_p, self,
                                mlpMemInfo_pp, self->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db);

    errNum = kaps_jr1_trie_convert_status_to_err_num(status, o_reason);

    if (errNum == NLMERR_OK)
    {
        kaps_jr1_lsn_mc_incr_num_bricks_allocated_for_all_lsns(self->m_pSettings, rqtNumLpu);
    }

    return errNum;
}

void
kaps_jr1_lsn_mc_free_resources(
    kaps_jr1_lsn_mc * self)
{
    if (self->m_bAllocedResource)
    {
        if (self->m_pSettings->m_isPerLpuGran)
            kaps_jr1_lsn_mc_free_resources_per_lpu(self);
        else
        {
            kaps_jr1_lsn_mc_check_and_free_ix(self, self->m_ixInfo, self->m_mlpMemInfo, 0);
            self->m_ixInfo = NULL;
        }

        kaps_jr1_lsn_mc_decr_num_bricks_allocated_for_all_lsns(self->m_pSettings, self->m_mlpMemInfo->size);
        kaps_jr1_uda_mgr_free(self->m_pSettings->m_pMlpMemMgr[self->m_devid], self->m_mlpMemInfo);
        self->m_mlpMemInfo = NULL;

        self->m_bAllocedResource = 0;
        self->m_nAllocBase = 0;
    }
}

void
kaps_jr1_lsn_mc_free_resources_per_lpu(
    kaps_jr1_lsn_mc * self)
{
    kaps_jr1_lpm_lpu_brick *cur_lpu_brick = self->m_lpuList;
    uint32_t brickNum;

    if (self->m_bAllocedResource)
    {
        brickNum = 0;
        while (cur_lpu_brick)
        {
            if (cur_lpu_brick->m_ixInfo)
            {
                kaps_jr1_lsn_mc_check_and_free_ix(self, cur_lpu_brick->m_ixInfo, self->m_mlpMemInfo, brickNum);
                cur_lpu_brick->m_ixInfo = NULL;
            }

            brickNum++;
            cur_lpu_brick = cur_lpu_brick->m_next_p;
        }

        self->m_nAllocBase = 0;
    }
}

void
kaps_jr1_lsn_mc_free_mlp_resources(
    kaps_jr1_lsn_mc * self,
    struct uda_mem_chunk *mlpMemInfo_p,
    NlmReasonCode * o_reason)
{
    if (self->m_bAllocedResource)
    {
        kaps_jr1_lsn_mc_decr_num_bricks_allocated_for_all_lsns(self->m_pSettings, mlpMemInfo_p->size);
        kaps_jr1_uda_mgr_free(self->m_pSettings->m_pMlpMemMgr[self->m_devid], mlpMemInfo_p);
    }
}

static void
kaps_jr1_lsn_mc_pvt_dtor(
    kaps_jr1_lsn_mc * self)
{
    kaps_jr1_lsn_mc_free_lpu_bricks(self->m_lpuList, self->m_pSettings->m_pAlloc);
    self->m_lpuList = NULL;
    self->m_nNumPrefixes = 0;
}

void
kaps_jr1_lsn_mc_destroy(
    kaps_jr1_lsn_mc * self)
{
    if (self)
    {
        kaps_jr1_nlm_allocator *alloc = self->m_pSettings->m_pAlloc;

        
        kaps_jr1_lsn_mc_free_resources(self);

        kaps_jr1_lsn_mc_pvt_dtor(self);
        kaps_jr1_nlm_allocator_free(alloc, (void *) self);
    }
}

void
kaps_jr1_lsn_mc_update_prefix(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_pfx_bundle * b,
    uint32_t oldix,
    uint32_t nextix)
{
    kaps_jr1_fib_tbl *tbl;
    kaps_jr1_fib_prefix_index_changed_app_cb_t appCB;
    void *appData;
    struct kaps_jr1_db *db = self->m_pTrie->m_tbl_ptr->m_db;

    tbl = self->m_pTbl;

    
    appCB = self->m_pSettings->m_pAppCB;
    appData = self->m_pSettings->m_pAppData;

    b->m_nIndex = nextix;
    if (oldix == KAPS_JR1_LSN_NEW_INDEX)
    {
        
        if (oldix != nextix)
        {

            if (appCB)
                appCB(appData, tbl, b, oldix, nextix);

            if (db->device->nv_ptr && db->device->issu_status == KAPS_JR1_ISSU_INIT && nextix != KAPS_JR1_LSN_NEW_INDEX)
                db->fn_table->cr_entry_shuffle(db, (struct kaps_jr1_entry *) b->m_backPtr, 1);
        }

    }
    else
    {
        
        if (oldix != nextix)
        {

            if (appCB)
                appCB(appData, tbl, b, oldix, nextix);

            if (db->device->nv_ptr && db->device->issu_status == KAPS_JR1_ISSU_INIT && nextix != KAPS_JR1_LSN_NEW_INDEX)
                db->fn_table->cr_entry_shuffle(db, (struct kaps_jr1_entry *) b->m_backPtr, 1);
        }
    }
}

uint16_t
kaps_jr1_lsn_mc_get_first_lpu_granIx(
    kaps_jr1_lsn_mc * self)
{
    kaps_jr1_lpm_lpu_brick *curLpuBrick = self->m_lpuList;

    return curLpuBrick->m_granIx;
}

uint16_t
kaps_jr1_lsn_mc_get_first_lpu_gran(
    kaps_jr1_lsn_mc * self)
{
    kaps_jr1_lpm_lpu_brick *curLpuBrick = self->m_lpuList;

    return curLpuBrick->m_gran;
}

uint16_t
kaps_jr1_lsn_mc_compute_gran(
    kaps_jr1_lsn_mc_settings * settings,
    uint32_t lengthAfterLopoff,
    uint16_t * o_granIx)
{
    uint16_t gran;

    
    ++lengthAfterLopoff;

    gran = settings->m_lengthToGran[lengthAfterLopoff];
    if (o_granIx)
    {
        *o_granIx = settings->m_lengthToGranIx[lengthAfterLopoff];
    }

    return gran;
}

void
kaps_jr1_lsn_mc_free_prefixes_safe(
    kaps_jr1_lsn_mc * self)
{
    kaps_jr1_nlm_allocator *alloc = self->m_pSettings->m_pAlloc;
    kaps_jr1_lpm_lpu_brick *curLpuBrick, *temp;

    kaps_jr1_lsn_mc_free_resources(self);

    curLpuBrick = self->m_lpuList;
    while (curLpuBrick)
    {
        temp = curLpuBrick->m_next_p;
        kaps_jr1_nlm_allocator_free(alloc, curLpuBrick->m_pfxes);
        kaps_jr1_nlm_allocator_free(alloc, curLpuBrick);

        curLpuBrick = temp;
    }
    self->m_lpuList = NULL;
    self->m_nNumPrefixes = 0;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_update_indexes(
    kaps_jr1_lsn_mc * self)
{
    uint32_t i;
    uint32_t oldix, newix;
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    uint32_t maxNumPfxPerLpuBrick;

    if (self->m_pTbl->m_fibTblMgr_p->m_devMgr_p->type == KAPS_JR1_DEVICE)
    {
        
        kaps_jr1_assert(0, "Update Indexes should not be called for this device type\n");
    }

    
    curLpuBrick = self->m_lpuList;
    newix = self->m_nAllocBase;

    while (curLpuBrick)
    {
        maxNumPfxPerLpuBrick = curLpuBrick->m_maxCapacity;
        if (self->m_pSettings->m_isPerLpuGran)
            newix = curLpuBrick->m_ixInfo->start_ix;

        if (curLpuBrick->m_numPfx == 0)
        {
            newix += maxNumPfxPerLpuBrick;
            curLpuBrick = curLpuBrick->m_next_p;
            continue;
        }

        for (i = 0; i < maxNumPfxPerLpuBrick; i++)
        {
            kaps_jr1_pfx_bundle *b = curLpuBrick->m_pfxes[i];

            if (b)
            {
                oldix = KAPS_JR1_PFX_BUNDLE_GET_INDEX(b);
                b->m_nIndex = newix;
                kaps_jr1_lsn_mc_update_prefix(self, b, oldix, newix);
            }
            ++newix;
        }

        curLpuBrick = curLpuBrick->m_next_p;
    }
    return NLMERR_OK;
}

static void
kaps_jr1_lsn_mc_remove_entry_helper(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry **entrySlotInHash)
{
    void *tmp_ptr;
    kaps_jr1_trie_node *trienode = NULL;
    uint32_t canFreeLsnResource;

    tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);
    kaps_jr1_memcpy(&trienode, tmp_ptr, sizeof(kaps_jr1_trie_node *));

    canFreeLsnResource = 1;

    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds)
    {
        
        if (trienode->m_iitLmpsofarPfx_p &&
            trienode->m_iitLmpsofarPfx_p->m_nPfxSize == trienode->m_depth)
        {
            canFreeLsnResource = 0;
        }
    }

    if (!self->m_nNumPrefixes && canFreeLsnResource)
    {
        kaps_jr1_lsn_mc_free_resources(self);
        self->m_pTbl = 0;
    }
}

static NlmErrNum_t
kaps_jr1_lsn_mc_pvt_remove_entry_normal(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * curBrick,
    kaps_jr1_pfx_bundle * found,
    uint32_t foundix,
    struct kaps_jr1_lpm_entry **entrySlotInHash,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl *tbl;
    kaps_jr1_fib_prefix_index_changed_app_cb_t appCB;
    kaps_jr1_pfx_bundle *pfx;
    void *appData;

    uint32_t oldIndex = 0;

    tbl = self->m_pTbl;

    
    appCB = self->m_pSettings->m_pAppCB;
    appData = self->m_pSettings->m_pAppData;

    pfx = curBrick->m_pfxes[foundix];

    curBrick->m_pfxes[foundix] = NULL;

    curBrick->m_numPfx--;

    self->m_nNumPrefixes--;

    oldIndex = KAPS_JR1_PFX_BUNDLE_GET_INDEX(found);

    if (appCB)
    {
        appCB(appData, tbl, pfx, oldIndex, KAPS_JR1_LSN_NEW_INDEX);
    }

    NLM_STRY(kaps_jr1_lsn_mc_delete_entry_in_hw(self, KAPS_JR1_PFX_BUNDLE_GET_INDEX(found), o_reason));

    kaps_jr1_lsn_mc_remove_entry_helper(self, entrySlotInHash);

    return NLMERR_OK;
}

struct uda_mem_chunk *
kaps_jr1_lsn_mc_get_uda_chunk(
    void *self)
{
    kaps_jr1_lsn_mc *lsn = (kaps_jr1_lsn_mc *) self;

    return lsn->m_mlpMemInfo;
}


static NlmErrNum_t
kaps_jr1_lsn_mc_try_fast_insert(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    int32_t found;
    kaps_jr1_pfx_bundle *newPfxBundle;
    uint32_t indexInLsn, whereInLpu;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device;
    kaps_jr1_pfx_bundle *oldPfxBundle;
    kaps_jr1_fib_prefix_index_changed_app_cb_t appCB = self->m_pSettings->m_pAppCB;
    void *appData = self->m_pSettings->m_pAppData;
    NlmErrNum_t errNum;
    uint32_t doSearch = 0;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    (void) settings;

    *newPfx_pp = NULL;

    device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;

    ad_db = NULL;
    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds
        && self->m_nDepth == entryToInsert->pfx_bundle->m_nPfxSize)
    {
        kaps_jr1_trie_node *trienode = NULL;

        void *tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);
        kaps_jr1_memcpy(&trienode, tmp_ptr, sizeof(kaps_jr1_trie_node *));

        entryToInsert->pfx_bundle->m_nIndex = KAPS_JR1_STRICT_LMPSOFAR_INDEX;

        entryToInsert->pfx_bundle->m_isLmpsofarPfx = 1;

        
        trienode->m_iitLmpsofarPfx_p = entryToInsert->pfx_bundle;

        
        errNum = kaps_jr1_lsn_mc_update_iit(self, o_reason);

        *newPfx_pp = entryToInsert->pfx_bundle;

        return errNum;
    }

    
    curLpuBrick = self->m_lpuList;
    found = 0;
    indexInLsn = 0;


    while (curLpuBrick)
    {
        
        if ((curLpuBrick->m_gran + self->m_nLopoff) >= (uint32_t) (entryToInsert->pfx_bundle->m_nPfxSize + 1))
        {
            if (curLpuBrick->m_numPfx < curLpuBrick->m_maxCapacity)
            {

                if (curLpuBrick->meta_priority == entryToInsert->meta_priority)
                {

                    if (entryToInsert->ad_handle && curLpuBrick->ad_db
                        && curLpuBrick->ad_db->db_info.seq_num != KAPS_JR1_GET_AD_SEQ_NUM(entryToInsert->ad_handle))
                    {
                        indexInLsn += curLpuBrick->m_maxCapacity;
                        curLpuBrick = curLpuBrick->m_next_p;
                        continue;
                    }

                    if (curLpuBrick->m_hasReservedSlot)
                    {
                        
                        if (curLpuBrick->m_pfxes[curLpuBrick->m_maxCapacity - 1] == NULL
                            && (curLpuBrick->m_numPfx + 1 == curLpuBrick->m_maxCapacity))
                        {
                            indexInLsn += curLpuBrick->m_maxCapacity;
                            curLpuBrick = curLpuBrick->m_next_p;
                            continue;
                        }
                    }

                    found = 1;
                    break;
                }
            }
        }

        indexInLsn += curLpuBrick->m_maxCapacity;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (!found)
    {
        return NLMERR_OK;
    }


    whereInLpu = 0;
    doSearch = 1;
    if (curLpuBrick->m_hasReservedSlot)
    {
        if (entryToInsert->pfx_bundle->m_nPfxSize == self->m_nDepth && !self->m_pSettings->m_onlyPfxCopyInReservedSlot)
        {
            whereInLpu = curLpuBrick->m_maxCapacity - 1;
            doSearch = 0;
            oldPfxBundle = curLpuBrick->m_pfxes[whereInLpu];

            if (oldPfxBundle)
            {

                if (appCB)
                    appCB(appData, self->m_pTbl, oldPfxBundle, oldPfxBundle->m_nIndex, KAPS_JR1_LSN_NEW_INDEX);

                
                kaps_jr1_pfx_bundle_destroy(oldPfxBundle, self->m_pSettings->m_pAlloc);
                curLpuBrick->m_pfxes[whereInLpu] = NULL;
                curLpuBrick->m_numPfx--;
                self->m_nNumPrefixes--;
            }
        }
    }

    if (doSearch)
    {
        for (; whereInLpu < curLpuBrick->m_maxCapacity; whereInLpu++)
        {
            if (!curLpuBrick->m_pfxes[whereInLpu])
            {
                break;
            }
        }
    }

    if (curLpuBrick->m_hasReservedSlot && whereInLpu == curLpuBrick->m_maxCapacity - 1)
    {
        if (self->m_pSettings->m_onlyPfxCopyInReservedSlot)
        {
            kaps_jr1_assert(0, "Incorrect prefix bundle in the reserved slot\n");
        }
        else
        {
            kaps_jr1_assert(entryToInsert->pfx_bundle->m_nPfxSize == self->m_nDepth,
                        "Incorrect prefix bundle in the reserved slot\n");
        }
    }

    if (curLpuBrick->m_pfxes[whereInLpu])
    {
        kaps_jr1_assert(0, "LPU Brick should be having a hole, but unable to find it\n");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    newPfxBundle->m_nIndex = kaps_jr1_lsn_mc_get_ix_mgr_index(self, indexInLsn + whereInLpu);

    curLpuBrick->m_pfxes[whereInLpu] = newPfxBundle;

    curLpuBrick->m_numPfx++;

    self->m_nNumPrefixes++;


    if (!curLpuBrick->ad_db)
        curLpuBrick->ad_db = ad_db;


    kaps_jr1_sassert(curLpuBrick->ad_db == ad_db);
    kaps_jr1_sassert(curLpuBrick->meta_priority == entryToInsert->meta_priority);


    errNum = kaps_jr1_lsn_mc_write_entry_to_hw(self, curLpuBrick->m_gran, newPfxBundle, KAPS_JR1_LSN_NEW_INDEX,
                                           KAPS_JR1_PFX_BUNDLE_GET_INDEX(newPfxBundle), 0, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static int32_t
kaps_jr1_lsn_mc_pvt_grow(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    uint16_t newGran,
    int32_t * isGrowUp,
    int32_t * isNewIndexRange_p)
{
    struct kaps_jr1_uda_mgr *pMlpMemMgr = self->m_pSettings->m_pMlpMemMgr[self->m_devid];
    struct uda_mem_chunk *mlpMemInfo_p = self->m_mlpMemInfo;
    struct kaps_jr1_ix_mgr *pIxMgr;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_jr1_lsn_mc_settings *lsn_settings = self->m_pSettings;
    uint32_t ixGrowDownSize = 0, ixGrowUpSize = 0;
    uint32_t mlpLpuGrowDownSize = 0, mlpLpuGrowUpSize = 0;
    uint16_t maxNumPfxInNewLpuBrick = 0;
    int32_t isGrowComplete = 0, isGrowUpAllowed;
    uint8_t ix_chunk_available = 0;
    struct kaps_jr1_ad_db *ad_db = NULL;
    kaps_jr1_status status = KAPS_JR1_OK;
    uint32_t actualMlpGrowSize, actualIxGrowSize;
    uint32_t can_ix_grow, can_mlp_grow;
    uint32_t udc_no;
    struct kaps_jr1_db *db = self->m_pTbl->m_db;

    (void) db;

    *isNewIndexRange_p = 0;

    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    maxNumPfxInNewLpuBrick =
        kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority, newGran);

    if (self->m_pSettings->m_isUnderAllocateLpuIx)
    {
        uint32_t nextHighGran = kaps_jr1_lsn_mc_get_next_max_gran(lsn_settings, newGran, 1);
        if (nextHighGran
            && (self->m_numLpuBricks + 1 >= lsn_settings->m_maxLpuPerLsn - lsn_settings->m_numUnderallocateLpus))
            maxNumPfxInNewLpuBrick =
                kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority,
                                                      nextHighGran);
    }

    pIxMgr = kaps_jr1_lsn_mc_get_ix_mgr_for_lsn_pfx(self, ad_db, self->m_mlpMemInfo, 0);

    if (!self->m_pSettings->m_isPerLpuGran)
        kaps_jr1_ix_mgr_check_grow_down(pIxMgr, self->m_ixInfo, &ixGrowDownSize);

    kaps_jr1_uda_mgr_check_grow_down(pMlpMemMgr, mlpMemInfo_p, &mlpLpuGrowDownSize);

    can_ix_grow = 0;
    actualIxGrowSize = maxNumPfxInNewLpuBrick;
    if (ixGrowDownSize >= maxNumPfxInNewLpuBrick || ix_chunk_available)
    {
        can_ix_grow = 1;
    }

    can_mlp_grow = 0;
    actualMlpGrowSize = 1;
    if (mlpLpuGrowDownSize >= 1)
    {
        can_mlp_grow = 1;
    }

    if (lsn_settings->m_isJoinedUdc && mlpLpuGrowDownSize >= 2)
    {
        udc_no = kaps_jr1_uda_mgr_compute_brick_udc(pMlpMemMgr, mlpMemInfo_p, mlpMemInfo_p->size);

        if (pMlpMemMgr->config.joined_udcs[udc_no + 1])
        {
            if (ixGrowDownSize >= 2 * maxNumPfxInNewLpuBrick)
            {
                can_ix_grow = 1;
                actualIxGrowSize = 2 * maxNumPfxInNewLpuBrick;
                actualMlpGrowSize = 2;
            }
            else
            {
                can_ix_grow = 0;
            }
        }
    }

    
    if (can_ix_grow && can_mlp_grow)
    {
        if (!self->m_pSettings->m_isPerLpuGran)
            status = kaps_jr1_ix_mgr_grow_down(pIxMgr, self->m_ixInfo, actualIxGrowSize);

        if (status != KAPS_JR1_OK)
        {
            kaps_jr1_assert(0, "Ix Grow Down Failed \n");
            return 0;
        }

        
        status = kaps_jr1_uda_mgr_grow_down(pMlpMemMgr, mlpMemInfo_p, actualMlpGrowSize);
        if (status != KAPS_JR1_OK)
            return 0;

        *isGrowUp = 0;
        isGrowComplete = 1;
    }

    isGrowUpAllowed = 1;

    if (lsn_settings->m_isJoinedUdc)
        isGrowUpAllowed = 0;

    
    if (!isGrowComplete && isGrowUpAllowed)
    {
        if (!self->m_pSettings->m_isPerLpuGran)
            kaps_jr1_ix_mgr_check_grow_up(pIxMgr, self->m_ixInfo, &ixGrowUpSize);

        kaps_jr1_uda_mgr_check_grow_up(pMlpMemMgr, mlpMemInfo_p, &mlpLpuGrowUpSize);

        if ((ixGrowUpSize >= maxNumPfxInNewLpuBrick || ix_chunk_available) && mlpLpuGrowUpSize >= 1)
        {
            if (!self->m_pSettings->m_isPerLpuGran)
                status = kaps_jr1_ix_mgr_grow_up(pIxMgr, self->m_ixInfo, maxNumPfxInNewLpuBrick);

            if (status != KAPS_JR1_OK)
            {
                kaps_jr1_assert(0, "Ix Grow Up Failed \n");
                return 0;
            }

            
            status = kaps_jr1_uda_mgr_grow_up(pMlpMemMgr, mlpMemInfo_p, 1);

            if (status != KAPS_JR1_OK)
                return 0;

            *isGrowUp = 1;
            isGrowComplete = 1;
        }
    }

    if (!isGrowComplete)
    {
        return 0;
    }

    
    if (!self->m_pSettings->m_isPerLpuGran)
    {
        self->m_nAllocBase = self->m_ixInfo->start_ix;
        self->m_nNumIxAlloced = (uint16_t) self->m_ixInfo->size;
    }

    self->m_bAllocedResource = 1;

    return 1;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_pvt_commit_grow(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    uint16_t newPfxGran,
    uint16_t newPfxGranIx,
    int32_t isGrowUp,
    int32_t isNewIndexRange,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_jr1_lpm_lpu_brick *newBrick;
    kaps_jr1_lpm_lpu_brick *iterLpuBrick;
    kaps_jr1_pfx_bundle *newPfxBundle;
    kaps_jr1_lsn_mc_settings *lsn_settings = self->m_pSettings;
    uint32_t indexInLsn, numPfxs = 0;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_jr1_status status = KAPS_JR1_OK;
    NlmErrNum_t errNum;
    uint32_t oldNumBricks, newNumBricks;

    (void) numPfxs;
    (void) status;

    *newPfx_pp = NULL;

    newBrick = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_lpm_lpu_brick));
    if (!newBrick)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    newBrick->m_gran = newPfxGran;
    newBrick->m_granIx = newPfxGranIx;
    newBrick->meta_priority = entryToInsert->meta_priority;
    newBrick->m_maxCapacity = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings,
                                                                    ad_db, 0, entryToInsert->meta_priority, newPfxGran);
    numPfxs = newBrick->m_maxCapacity;
    newBrick->m_pfxes = kaps_jr1_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_jr1_pfx_bundle *));
    if (!newBrick->m_pfxes)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (self->m_pSettings->m_isUnderAllocateLpuIx)
    {
        uint32_t nextHighGran = kaps_jr1_lsn_mc_get_next_max_gran(lsn_settings, newPfxGran, 1);
        if (nextHighGran
            && (self->m_numLpuBricks + 1 >= lsn_settings->m_maxLpuPerLsn - lsn_settings->m_numUnderallocateLpus))
        {
            numPfxs =
                kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority,
                                                      nextHighGran);
            newBrick->m_underAllocatedIx = 1;
        }
    }

    newBrick->m_numPfx = 1;

    newBrick->ad_db = ad_db;

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    indexInLsn = 0;
    if (isGrowUp)
    {
        
        newBrick->m_next_p = self->m_lpuList;
        self->m_lpuList = newBrick;
    }
    else
    {
        
        iterLpuBrick = self->m_lpuList;

        while (iterLpuBrick->m_next_p)
        {
            indexInLsn += iterLpuBrick->m_maxCapacity;
            iterLpuBrick = iterLpuBrick->m_next_p;
        }

        indexInLsn += iterLpuBrick->m_maxCapacity;
        iterLpuBrick->m_next_p = newBrick;
        newBrick->m_next_p = NULL;
    }

    oldNumBricks = self->m_numLpuBricks;
    self->m_numLpuBricks++;

    newPfxBundle->m_nIndex = kaps_jr1_lsn_mc_get_ix_mgr_index(self, indexInLsn);

    newBrick->m_pfxes[0] = newPfxBundle;
    self->m_nNumPrefixes++;
    self->m_nLsnCapacity += newBrick->m_maxCapacity;

    kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(self, o_reason);

    newNumBricks = self->m_numLpuBricks;

    NLM_STRY(kaps_jr1_lsn_mc_clear_lpu_for_grow(self, isGrowUp, newNumBricks - oldNumBricks, o_reason));

    
    errNum = kaps_jr1_lsn_mc_write_entry_to_hw(self, newBrick->m_gran, newPfxBundle, KAPS_JR1_LSN_NEW_INDEX,
                                           KAPS_JR1_PFX_BUNDLE_GET_INDEX(newPfxBundle), 0, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    if (isNewIndexRange)
    {
        
        kaps_jr1_lsn_mc_update_indexes(self);
    }

    
    NLM_STRY(kaps_jr1_lsn_mc_update_iit(self, o_reason));

    *newPfx_pp = newPfxBundle;

    (self->m_pSettings->m_numAllocatedBricksForAllLsns)++;

    return NLMERR_OK;
}

static int32_t
kaps_jr1_lsn_mc_pvt_realloc_per_lsn_gran(
    kaps_jr1_lsn_mc * self,
    uint16_t gran,
    uint32_t * numLpuBricksRequired_p,
    NlmReasonCode * o_reason)
{
    uint32_t maxPfxPerLpuBrick, totalNumPfx, numLpuBricksRequired;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;
    uint32_t granIx, numHoles;
    kaps_jr1_lpm_lpu_brick *iterBrick;
    struct kaps_jr1_ad_db * ad_db = (struct kaps_jr1_ad_db *) settings->m_fibTbl->m_db->common_info->ad_info.ad;

    maxPfxPerLpuBrick = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, 0xF, gran);
    granIx = settings->m_lengthToGranIx[gran];

    numHoles = settings->m_numHolesForGranIx[granIx];
    if (numHoles)
        totalNumPfx = self->m_nNumPrefixes + numHoles;
    else
        totalNumPfx = self->m_nNumPrefixes + 1;

    
    iterBrick = self->m_lpuList;
    while (iterBrick)
    {
        if (iterBrick->m_hasReservedSlot)
        {
            if (!iterBrick->m_pfxes[iterBrick->m_maxCapacity - 1])
            {
                totalNumPfx++;
            }
            break;
        }
        iterBrick = iterBrick->m_next_p;
    }

    numLpuBricksRequired = (totalNumPfx + maxPfxPerLpuBrick - 1) / maxPfxPerLpuBrick;
    if (numLpuBricksRequired > settings->m_maxLpuPerLsn)
    {
        totalNumPfx = self->m_nNumPrefixes + 1;
        numLpuBricksRequired = (totalNumPfx + maxPfxPerLpuBrick - 1) / maxPfxPerLpuBrick;
        if (numLpuBricksRequired > settings->m_maxLpuPerLsn)
        {
            return 0;
        }
    }

    if (self->m_pSettings->dynamic_alloc_enable)
    {
        if (numLpuBricksRequired <= settings->m_maxLpuPerLsn / 2)
            numLpuBricksRequired = settings->m_maxLpuPerLsn / 2;
        else
            numLpuBricksRequired = settings->m_maxLpuPerLsn;
    }

    if (self->m_pSettings->m_isFullWidthLsn)
        numLpuBricksRequired = settings->m_maxLpuPerLsn;

    if (settings->m_isMultiBrickAlloc)
    {
        numLpuBricksRequired = (numLpuBricksRequired + settings->m_numMultiBricks - 1) / settings->m_numMultiBricks;
        numLpuBricksRequired = numLpuBricksRequired * settings->m_numMultiBricks;
        if (numLpuBricksRequired > settings->m_maxLpuPerLsn)
            numLpuBricksRequired = settings->m_maxLpuPerLsn;
    }

    *numLpuBricksRequired_p = numLpuBricksRequired;
    return 1;
}

NlmErrNum_t
kaps_jr1_lsn_mc_undo_realloc(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * newBrickList,
    uint32_t startBrickNum)
{
    kaps_jr1_lpm_lpu_brick *curBrick, *nextBrick;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;
    kaps_jr1_nlm_allocator *alloc = settings->m_pAlloc;
    uint32_t curBrickNum = startBrickNum;

    curBrick = newBrickList;
    while (curBrick)
    {
        nextBrick = curBrick->m_next_p;

        if (settings->m_isPerLpuGran && curBrick->m_ixInfo)
        {
            kaps_jr1_lsn_mc_check_and_free_ix(self, curBrick->m_ixInfo,
                                    self->m_mlpMemInfo, curBrickNum);
        }

        if (curBrick->m_pfxes)
            kaps_jr1_nlm_allocator_free(alloc, curBrick->m_pfxes);

        kaps_jr1_nlm_allocator_free(alloc, curBrick);

        curBrickNum++;
        curBrick = nextBrick;
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_pvt_commit_realloc_normal(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    uint16_t curGran,
    uint16_t curGranIx,
    uint32_t numLpuBricksRequired,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_jr1_lpm_lpu_brick *newBrick = NULL, *newListHead;
    kaps_jr1_lpm_lpu_brick *iterLpuBrick, *lastLpuBrick;
    kaps_jr1_pfx_bundle *newPfxBundle;
    kaps_jr1_lsn_mc_settings *lsn_settings = self->m_pSettings;
    uint32_t i, numAdditionalLpuBricks;
    uint16_t additionalLsnCapacity;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t brickIter;
    NlmErrNum_t errNum;
    uint32_t ixRqtSize, maxPfxPerLpuBrick;
    struct kaps_jr1_ix_chunk *oldIxChunk;
    uint32_t isSbcPossible = 1;
    uint32_t originalNumBricks;

    *newPfx_pp = NULL;

    if (numLpuBricksRequired <= self->m_numLpuBricks)
    {
        kaps_jr1_assert(0, "reallocing to a smaller size which is incorrect");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }


    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    maxPfxPerLpuBrick =
        kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(lsn_settings, ad_db, 0, entryToInsert->meta_priority, curGran);
    ixRqtSize = numLpuBricksRequired * maxPfxPerLpuBrick;

    errNum = kaps_jr1_lsn_mc_acquire_resources(self, ixRqtSize, (uint8_t) numLpuBricksRequired, o_reason);
    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    if (self->m_mlpMemInfo->size > numLpuBricksRequired)
    {
        
        numLpuBricksRequired = self->m_mlpMemInfo->size;
    }

    numAdditionalLpuBricks = numLpuBricksRequired - self->m_numLpuBricks;
    newListHead = NULL;
    additionalLsnCapacity = 0;
    brickIter = self->m_numLpuBricks;
    originalNumBricks = self->m_numLpuBricks;
    for (i = 0; i < numAdditionalLpuBricks; ++i)
    {
        uint32_t numPfxs = 0;

        newBrick = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!newBrick)
        {
            kaps_jr1_lsn_mc_undo_realloc(self, newListHead, originalNumBricks);
            kaps_jr1_lsn_mc_undo_acquire_resources(self);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_next_p = newListHead;
        newListHead = newBrick;

        newBrick->m_gran = curGran;
        newBrick->m_granIx = curGranIx;
        newBrick->m_maxCapacity =
            kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority, curGran);
        numPfxs = newBrick->m_maxCapacity;

        newBrick->m_pfxes = kaps_jr1_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_jr1_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            kaps_jr1_lsn_mc_undo_realloc(self, newListHead, originalNumBricks);
            kaps_jr1_lsn_mc_undo_acquire_resources(self);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        if (self->m_pSettings->m_isUnderAllocateLpuIx
            && !newBrick->m_hasReservedSlot)
        {
            uint32_t nextHighGran = kaps_jr1_lsn_mc_get_next_max_gran(lsn_settings, curGran, 1);
            if (nextHighGran
                && (self->m_numLpuBricks + i >= lsn_settings->m_maxLpuPerLsn - lsn_settings->m_numUnderallocateLpus))
            {
                numPfxs =
                    kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority,
                                                          nextHighGran);
                newBrick->m_underAllocatedIx = 1;
            }
        }

        additionalLsnCapacity += newBrick->m_maxCapacity;

        if (self->m_pSettings->m_isPerLpuGran)
        {
            errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, newBrick, brickIter, numPfxs, ad_db, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_jr1_lsn_mc_undo_realloc(self, newListHead, originalNumBricks);
                kaps_jr1_lsn_mc_undo_acquire_resources(self);
                return errNum;
            }
        }

        newBrick->ad_db = ad_db;
        newBrick->meta_priority = entryToInsert->meta_priority;
        brickIter++;
    }

    newBrick->m_numPfx = 1;

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    
    iterLpuBrick = self->m_lpuList;
    brickIter = 0;
    lastLpuBrick = NULL;
    while (iterLpuBrick)
    {
        if (self->m_pSettings->m_isPerLpuGran && self->m_pSettings->m_isHardwareMappedIx)
        {
            oldIxChunk = iterLpuBrick->m_ixInfo;
            errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, iterLpuBrick, brickIter, iterLpuBrick->m_maxCapacity,
                                                           iterLpuBrick->ad_db, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_jr1_lsn_mc_undo_acquire_resources_per_lpu(self);
                kaps_jr1_lsn_mc_undo_realloc(self, newListHead, originalNumBricks);
                kaps_jr1_lsn_mc_undo_acquire_resources(self);
                return errNum;
            }

            kaps_jr1_lsn_mc_backup_resources_per_lpu(self, iterLpuBrick, oldIxChunk);
        }

        brickIter++;
        lastLpuBrick = iterLpuBrick;
        iterLpuBrick = iterLpuBrick->m_next_p;
    }

    if (self->m_pSettings->m_isPerLpuGran && self->m_pSettings->m_isHardwareMappedIx)
    {
        kaps_jr1_lsn_mc_free_old_resources_per_lpu(self);
    }

    kaps_jr1_lsn_mc_free_old_resources(self);

    
    lastLpuBrick->m_next_p = newListHead;

    self->m_numLpuBricks = (uint8_t) numLpuBricksRequired;

    newBrick->m_pfxes[0] = newPfxBundle;
    self->m_nNumPrefixes++;
    self->m_nLsnCapacity += additionalLsnCapacity;

    isSbcPossible = 1;

    NLM_STRY(kaps_jr1_lsn_mc_commit(self, isSbcPossible, o_reason));

    NLM_STRY(kaps_jr1_lsn_mc_update_iit(self, o_reason));

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_pvt_commit_realloc_with_gran_change(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    uint16_t newPfxGran,
    uint16_t newPfxGranIx,
    uint32_t numLpuBricksRequired,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;
    kaps_jr1_nlm_allocator *alloc_p = settings->m_pAlloc;
    kaps_jr1_lpm_lpu_brick *newBrick, *headBrick, *oldBrick, *iterBrick;
    kaps_jr1_pfx_bundle *newPfxBundle;
    uint32_t lpuIter;
    uint32_t oldLoc, newLoc;
    uint16_t originalNumPfx, newLsnCapacity;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t ixRqtSize, maxPfxPerLpuBrick;
    NlmErrNum_t errNum;

    *newPfx_pp = NULL;


    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    maxPfxPerLpuBrick =
        kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, entryToInsert->meta_priority, newPfxGran);
    ixRqtSize = numLpuBricksRequired * maxPfxPerLpuBrick;

    errNum = kaps_jr1_lsn_mc_acquire_resources(self, ixRqtSize, (uint8_t) numLpuBricksRequired, o_reason);
    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    if (self->m_mlpMemInfo->size > numLpuBricksRequired)
    {
        
        numLpuBricksRequired = self->m_mlpMemInfo->size;
    }

    headBrick = NULL;
    newLsnCapacity = 0;
    for (lpuIter = 0; lpuIter < numLpuBricksRequired; ++lpuIter)
    {
        newBrick = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!newBrick)
        {
            kaps_jr1_lsn_mc_undo_realloc(self, headBrick, 0);
            kaps_jr1_lsn_mc_undo_acquire_resources(self);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_next_p = headBrick;
        headBrick = newBrick;

        newBrick->m_gran = newPfxGran;
        newBrick->m_granIx = newPfxGranIx;
        newBrick->m_maxCapacity =
            kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, ad_db, 0, entryToInsert->meta_priority,
                                                  newPfxGran);
        newLsnCapacity += newBrick->m_maxCapacity;

        newBrick->m_pfxes = kaps_jr1_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_jr1_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            kaps_jr1_lsn_mc_undo_realloc(self, headBrick, 0);
            kaps_jr1_lsn_mc_undo_acquire_resources(self);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }
        newBrick->ad_db = ad_db;
    }

    kaps_jr1_lsn_mc_free_old_resources(self);

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    
    oldLoc = 0;
    newLoc = 0;
    oldBrick = self->m_lpuList;
    newBrick = headBrick;

    
    iterBrick = oldBrick;
    while (iterBrick)
    {
        if (iterBrick->m_hasReservedSlot)
        {
            newBrick->m_hasReservedSlot = 1;

            newBrick->m_pfxes[newBrick->m_maxCapacity - 1] = iterBrick->m_pfxes[iterBrick->m_maxCapacity - 1];

            if (newBrick->m_pfxes[newBrick->m_maxCapacity - 1])
                newBrick->m_numPfx++;

            break;
        }

        iterBrick = iterBrick->m_next_p;
    }

    while (oldBrick && newBrick)
    {

        
        if (oldBrick->m_hasReservedSlot && oldLoc == oldBrick->m_maxCapacity - 1)
        {
            oldLoc = 0;
            oldBrick = oldBrick->m_next_p;
            continue;
        }

        if (newBrick->m_hasReservedSlot && newLoc == newBrick->m_maxCapacity - 1)
        {
            newLoc = 0;
            newBrick = newBrick->m_next_p;
            continue;
        }

        if (oldBrick->m_pfxes[oldLoc])
        {
            newBrick->m_pfxes[newLoc] = oldBrick->m_pfxes[oldLoc];
            newBrick->m_numPfx++;
            newLoc++;
        }

        oldLoc++;
        if (oldLoc >= oldBrick->m_maxCapacity)
        {
            oldLoc = 0;
            oldBrick = oldBrick->m_next_p;
        }

        if (newLoc >= newBrick->m_maxCapacity)
        {
            newLoc = 0;
            newBrick = newBrick->m_next_p;
        }
    }

    
    if (newBrick)
    {
        newBrick->m_pfxes[newLoc] = newPfxBundle;
        newBrick->m_numPfx++;
    }

    
    originalNumPfx = self->m_nNumPrefixes;
    kaps_jr1_lsn_mc_free_lpu_bricks(self->m_lpuList, alloc_p);

    self->m_lpuList = headBrick;
    self->m_numLpuBricks = (uint8_t) numLpuBricksRequired;
    self->m_nNumPrefixes = originalNumPfx + 1;
    self->m_nLsnCapacity = newLsnCapacity;

    NLM_STRY(kaps_jr1_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_jr1_lsn_mc_update_iit(self, o_reason));

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_resize_per_lsn_gran(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    uint16_t newPfxGran, curGran = 0, newPfxGranIx = 0, curGranIx;
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    int32_t doGrow, doRealloc, isGrowUp, isNewIndexRange;
    uint32_t lengthAfterLopoff;
    uint32_t numLpuBricksRequired = 0;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;
    NlmErrNum_t errNum;
    struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) settings->m_fibTbl->m_db->common_info->ad_info.ad;

    *newPfx_pp = NULL;

    lengthAfterLopoff = entryToInsert->pfx_bundle->m_nPfxSize - self->m_nLopoff;
    newPfxGran = kaps_jr1_lsn_mc_compute_gran(settings, lengthAfterLopoff, &newPfxGranIx);

    curLpuBrick = self->m_lpuList;
    curGran = curLpuBrick->m_gran;
    curGranIx = curLpuBrick->m_granIx;

    if (newPfxGran <= curGran)
    {
        if (self->m_numLpuBricks >= settings->m_maxLpuPerLsn)
            return NLMERR_OK;

        doGrow = kaps_jr1_lsn_mc_pvt_grow(self, entryToInsert, curGran, &isGrowUp, &isNewIndexRange);
        if (doGrow)
        {
            errNum = kaps_jr1_lsn_mc_pvt_commit_grow(self, entryToInsert, curGran, curGranIx,
                                                 isGrowUp, isNewIndexRange, newPfx_pp, o_reason);
            return errNum;
        }
        
        doRealloc = kaps_jr1_lsn_mc_pvt_realloc_per_lsn_gran(self, curGran, &numLpuBricksRequired, o_reason);
        if (doRealloc)
        {
            NLM_STRY(kaps_jr1_lsn_mc_pvt_commit_realloc_normal(self, entryToInsert, curGran, curGranIx,
                                                           numLpuBricksRequired, newPfx_pp, o_reason));
        }
    }
    else
    {

        uint32_t maxNumHighGranPfxInLsn = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, 0xF, newPfxGran)
            * settings->m_maxLpuPerLsn;
        if (maxNumHighGranPfxInLsn < (uint32_t) (self->m_nNumPrefixes + 1))
        {
            return NLMERR_OK;
        }

        kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(self);

        doRealloc = kaps_jr1_lsn_mc_pvt_realloc_per_lsn_gran(self, newPfxGran, &numLpuBricksRequired, o_reason);
        if (doRealloc)
        {
            NLM_STRY(kaps_jr1_lsn_mc_pvt_commit_realloc_with_gran_change(self, entryToInsert, newPfxGran,
                                                                     newPfxGranIx, numLpuBricksRequired, newPfx_pp,
                                                                     o_reason));
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_pvt_realloc_modifying_chosen_lpu(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * chosenLpuBrick,
    uint32_t chosenBrickIter,
    uint16_t maxGran,
    uint16_t maxGranIx,
    struct kaps_jr1_lpm_entry *entryToInsert,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;
    kaps_jr1_pfx_bundle *newPfxBundle;
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_jr1_pfx_bundle **newPfxesInLpu, **oldPfxesInLpu;
    uint32_t ixRqtSize;
    uint16_t newMaxLpuCapacity, i, j;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    kaps_jr1_lpm_lpu_brick *curBrick;
    uint32_t curBrickCapacity;
    struct kaps_jr1_ix_chunk *oldIxChunk;

    *newPfx_pp = NULL;

    newMaxLpuCapacity = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, chosenLpuBrick->ad_db,
                                                              chosenLpuBrick->m_hasReservedSlot,
                                                              chosenLpuBrick->meta_priority, maxGran);
    newPfxesInLpu = kaps_jr1_nlm_allocator_calloc(alloc_p, newMaxLpuCapacity, sizeof(kaps_jr1_pfx_bundle *));

    if (!newPfxesInLpu)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (entryToInsert->ad_handle
        && chosenLpuBrick->ad_db->db_info.seq_num != KAPS_JR1_GET_AD_SEQ_NUM(entryToInsert->ad_handle))
    {
        kaps_jr1_nlm_allocator_free(alloc_p, newPfxesInLpu);
        return NLMERR_OK;
    }

    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(self);
    kaps_jr1_lsn_mc_store_old_lsn_info(self, 0);

    ixRqtSize = self->m_nLsnCapacity - chosenLpuBrick->m_maxCapacity + newMaxLpuCapacity;
    errNum = kaps_jr1_lsn_mc_acquire_resources(self, ixRqtSize, self->m_numLpuBricks, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_jr1_nlm_allocator_free(alloc_p, newPfxesInLpu);
        return errNum;
    }

    if (self->m_pSettings->m_isPerLpuGran)
    {

        if (self->m_pSettings->m_isHardwareMappedIx)
        {
            i = 0;
            curBrick = self->m_lpuList;
            while (curBrick)
            {
                curBrickCapacity = curBrick->m_maxCapacity;
                if (i == chosenBrickIter)
                    curBrickCapacity = newMaxLpuCapacity;

                oldIxChunk = curBrick->m_ixInfo;

                errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, curBrick, i, curBrickCapacity,
                                                               curBrick->ad_db, o_reason);
                if (errNum != NLMERR_OK)
                {
                    kaps_jr1_lsn_mc_undo_acquire_resources_per_lpu(self);
                    kaps_jr1_lsn_mc_undo_acquire_resources(self);
                    kaps_jr1_nlm_allocator_free(alloc_p, newPfxesInLpu);
                    return errNum;
                }

                kaps_jr1_lsn_mc_backup_resources_per_lpu(self, curBrick, oldIxChunk);

                ++i;
                curBrick = curBrick->m_next_p;
            }

        }
        else
        {
            KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

            oldIxChunk = chosenLpuBrick->m_ixInfo;

            errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, chosenLpuBrick, chosenBrickIter,
                                                           newMaxLpuCapacity, ad_db, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_jr1_lsn_mc_undo_acquire_resources(self);
                kaps_jr1_nlm_allocator_free(alloc_p, newPfxesInLpu);
                return errNum;
            }

            kaps_jr1_lsn_mc_backup_resources_per_lpu(self, chosenLpuBrick, oldIxChunk);
        }

        self->m_nNumIxAlloced = ixRqtSize;
    }

    kaps_jr1_lsn_mc_free_old_resources_per_lpu(self);
    kaps_jr1_lsn_mc_free_old_resources(self);

    oldPfxesInLpu = chosenLpuBrick->m_pfxes;

    j = 0;
    for (i = 0; i < chosenLpuBrick->m_maxCapacity; ++i)
    {
        if (oldPfxesInLpu[i])
        {
            if (chosenLpuBrick->m_hasReservedSlot && i == chosenLpuBrick->m_maxCapacity - 1)
            {
                newPfxesInLpu[newMaxLpuCapacity - 1] = oldPfxesInLpu[i];
            }
            else
            {
                newPfxesInLpu[j] = oldPfxesInLpu[i];
                ++j;
            }
        }
    }

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    newPfxesInLpu[j] = newPfxBundle;

    chosenLpuBrick->m_gran = maxGran;
    chosenLpuBrick->m_granIx = maxGranIx;
    chosenLpuBrick->m_maxCapacity = newMaxLpuCapacity;
    chosenLpuBrick->m_pfxes = newPfxesInLpu;
    chosenLpuBrick->m_numPfx++;


    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);


    chosenLpuBrick->ad_db = ad_db;

    kaps_jr1_sassert(chosenLpuBrick->meta_priority == entryToInsert->meta_priority);

    self->m_nNumPrefixes++;
    self->m_nLsnCapacity = (uint16_t) ixRqtSize;

    kaps_jr1_nlm_allocator_free(alloc_p, oldPfxesInLpu);

    NLM_STRY(kaps_jr1_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_jr1_lsn_mc_update_iit(self, o_reason));

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static int32_t
kaps_jr1_lsn_mc_pvt_realloc_per_lpu_gran(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    uint16_t gran,
    uint32_t * numLpuBricksRequired_p,
    NlmReasonCode * o_reason)
{
    uint32_t maxPfxPerLpuBrick, numAdditionalLpuBricks, numLpuBricksRequired;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;
    uint32_t granIx, numHoles;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;

    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    maxPfxPerLpuBrick = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, entryToInsert->meta_priority, gran);
    granIx = settings->m_lengthToGranIx[gran];

    numHoles = settings->m_numHolesForGranIx[granIx];
    if (numHoles == 0)
        numHoles = 1;

    numAdditionalLpuBricks = (numHoles + maxPfxPerLpuBrick - 1) / maxPfxPerLpuBrick;

    if (self->m_numLpuBricks + numAdditionalLpuBricks > settings->m_maxLpuPerLsn)
    {
        numAdditionalLpuBricks = 1;
        if (self->m_numLpuBricks + numAdditionalLpuBricks > settings->m_maxLpuPerLsn)
        {
            return 0;
        }
    }

    numLpuBricksRequired = self->m_numLpuBricks + numAdditionalLpuBricks;

    *numLpuBricksRequired_p = numLpuBricksRequired;
    return 1;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_resize_per_lpu_gran(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    uint16_t newPfxGran, maxGran = 0, newPfxGranIx = 0, maxGranIx;
    kaps_jr1_lpm_lpu_brick *curLpuBrick, *chosenLpuBrick;
    int32_t doGrow, isGrowUp, isNewIndexRange, doRealloc;
    uint32_t lengthAfterLopoff, numLpuBricksRequired;
    int32_t isLsnFull;
    uint16_t chosenGran = 0, localMaxGran = 0;
    kaps_jr1_lsn_mc_settings *lsnSettings = self->m_pSettings;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t targetNumPfx;
    uint32_t curBrickIter, chosenBrickIter;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t sort_and_resize = 0;

    if (device->type == KAPS_JR1_DEVICE)
        sort_and_resize = 1;

    *newPfx_pp = NULL;

    lengthAfterLopoff = entryToInsert->pfx_bundle->m_nPfxSize - self->m_nLopoff;
    newPfxGran = kaps_jr1_lsn_mc_compute_gran(lsnSettings, lengthAfterLopoff, &newPfxGranIx);

    isLsnFull = 1;
    if (self->m_nLsnCapacity > self->m_nNumPrefixes)
    {
        isLsnFull = 0;
    }

    curLpuBrick = self->m_lpuList;
    chosenLpuBrick = NULL;
    curBrickIter = 0;
    chosenBrickIter = 0;
    maxGran = newPfxGran;
    maxGranIx = newPfxGranIx;
    while (curLpuBrick)
    {

        if (curLpuBrick->m_numPfx == 0)
        {
            chosenLpuBrick = curLpuBrick;
            chosenBrickIter = curBrickIter;
            chosenGran = newPfxGran;
            break;
        }

        if (maxGran < curLpuBrick->m_gran)
        {
            maxGran = curLpuBrick->m_gran;
            maxGranIx = curLpuBrick->m_granIx;
        }
        localMaxGran = newPfxGran > curLpuBrick->m_gran ? newPfxGran : curLpuBrick->m_gran;

        if (entryToInsert->ad_handle
            && curLpuBrick->ad_db
            && curLpuBrick->ad_db->db_info.seq_num != KAPS_JR1_GET_AD_SEQ_NUM(entryToInsert->ad_handle))
        {
            curLpuBrick = curLpuBrick->m_next_p;
            curBrickIter++;
            continue;
        }

        if (entryToInsert->meta_priority != curLpuBrick->meta_priority)
        {
            curLpuBrick = curLpuBrick->m_next_p;
            curBrickIter++;
            continue;
        }

        
        targetNumPfx = curLpuBrick->m_numPfx + 1;

        
        if (curLpuBrick->m_hasReservedSlot && !curLpuBrick->m_pfxes[curLpuBrick->m_maxCapacity - 1])
            targetNumPfx++;

        if (kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick
            (lsnSettings, curLpuBrick->ad_db, curLpuBrick->m_hasReservedSlot, curLpuBrick->meta_priority,
             localMaxGran) >= targetNumPfx)
        {
            if (!chosenLpuBrick)
            {
                chosenLpuBrick = curLpuBrick;
                chosenBrickIter = curBrickIter;
                chosenGran = localMaxGran;
                break;
            }
        }
        ++curBrickIter;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (chosenLpuBrick)
    {
        if (lsnSettings->m_isJoinedUdc)
        {
            errNum = kaps_jr1_lsn_mc_sort_and_resize_per_lpu_gran(self, entryToInsert, newPfx_pp, o_reason);
            return errNum;
        }
        else
        {
            struct kaps_jr1_ad_db *old_ad_db = chosenLpuBrick->ad_db;
            uint32_t old_meta_priority = chosenLpuBrick->meta_priority;
            uint32_t changed_ad_priority = 0;

            if (chosenLpuBrick->m_numPfx == 0)
            {
                changed_ad_priority = 1;

                KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, chosenLpuBrick->ad_db);

                chosenLpuBrick->meta_priority = entryToInsert->meta_priority;
            }

            errNum = kaps_jr1_lsn_mc_pvt_realloc_modifying_chosen_lpu(self, chosenLpuBrick, chosenBrickIter, chosenGran,
                                                                  lsnSettings->m_lengthToGranIx[chosenGran],
                                                                  entryToInsert, newPfx_pp, o_reason);

            if (errNum != NLMERR_OK && changed_ad_priority)
            {
                chosenLpuBrick->ad_db = old_ad_db;
                chosenLpuBrick->meta_priority = old_meta_priority;
            }

            return errNum;
        }
    }

    if (self->m_numLpuBricks >= lsnSettings->m_maxLpuPerLsn)
        return NLMERR_OK;

    if (sort_and_resize)
    {
        errNum = kaps_jr1_lsn_mc_sort_and_resize_per_lpu_gran(self, entryToInsert, newPfx_pp, o_reason);
    }
    else
    {

        if (isLsnFull)
        {
            doGrow = kaps_jr1_lsn_mc_pvt_grow(self, entryToInsert, maxGran, &isGrowUp, &isNewIndexRange);
            if (doGrow)
            {
                errNum = kaps_jr1_lsn_mc_pvt_commit_grow(self, entryToInsert, maxGran, maxGranIx,
                                                     isGrowUp, isNewIndexRange, newPfx_pp, o_reason);
                return errNum;
            }
        }

        doRealloc = kaps_jr1_lsn_mc_pvt_realloc_per_lpu_gran(self, entryToInsert, maxGran, &numLpuBricksRequired, o_reason);
        if (doRealloc)
        {
            errNum = kaps_jr1_lsn_mc_pvt_commit_realloc_normal(self, entryToInsert, maxGran,
                                                           maxGranIx, numLpuBricksRequired, newPfx_pp, o_reason);
        }
    }

    return errNum;
}

void
kaps_jr1_lsn_mc_undo_sort_and_resize(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * oldBrickList,
    uint32_t oldNumLpuBricks,
    uint32_t oldNumPrefix,
    uint32_t oldLsnCapacity,
    struct uda_mem_chunk *oldMlpMemInfo)
{
    kaps_jr1_lpm_lpu_brick *newBrickList, *iterLpuBrick, *nextBrick;
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    uint32_t i;

    newBrickList = self->m_lpuList;

    self->m_lpuList = oldBrickList;
    self->m_numLpuBricks = oldNumLpuBricks;
    self->m_nNumPrefixes = oldNumPrefix;
    self->m_nLsnCapacity = oldLsnCapacity;
    self->m_mlpMemInfo = oldMlpMemInfo;

    iterLpuBrick = newBrickList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        kaps_jr1_nlm_allocator_free(alloc_p, iterLpuBrick->m_pfxes);
        kaps_jr1_nlm_allocator_free(alloc_p, iterLpuBrick);
        ++i;
        iterLpuBrick = nextBrick;
    }

    self->m_bAllocedResource = 1;
}

void
kaps_jr1_lsn_mc_undo_chunk_move(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * oldBrickList,
    uint32_t oldNumLpuBricks,
    uint32_t oldNumPrefix,
    uint32_t oldLsnCapacity,
    struct uda_mem_chunk *oldMlpMemInfo,
    uint32_t oldOffset,
    uint32_t oldSize)
{
    kaps_jr1_lpm_lpu_brick *newBrickList, *iterLpuBrick, *nextBrick;
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    uint32_t i;

    newBrickList = self->m_lpuList;

    self->m_lpuList = oldBrickList;
    self->m_numLpuBricks = oldNumLpuBricks;
    self->m_nNumPrefixes = oldNumPrefix;
    self->m_nLsnCapacity = oldLsnCapacity;
    self->m_mlpMemInfo = oldMlpMemInfo;

    self->m_mlpMemInfo->offset = oldOffset;
    self->m_mlpMemInfo->size = oldSize;

    iterLpuBrick = newBrickList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        kaps_jr1_nlm_allocator_free(alloc_p, iterLpuBrick->m_pfxes);
        kaps_jr1_nlm_allocator_free(alloc_p, iterLpuBrick);
        ++i;
        iterLpuBrick = nextBrick;
    }

    self->m_bAllocedResource = 1;
}

NlmErrNum_t
kaps_jr1_lsn_mc_sort_and_resize_per_lpu_gran(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entryToInsert,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_lsn_mc_settings *settings_p = self->m_pSettings;
    kaps_jr1_flat_lsn_data *flatData = NULL;
    kaps_jr1_lpm_lpu_brick *oldBrickList, *oldLpuBrick, *cur_lpu_brick, *nextBrick;
    uint32_t oldNumLpuBricks, oldNumPrefix, oldLsnCapacity;
    uint32_t i;
    struct uda_mem_chunk *oldMlpMemInfo;
    struct uda_mem_chunk *newMlpMemInfo;
    uint32_t doesLsnFit;

    
    flatData = kaps_jr1_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, o_reason);
    if (!flatData)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    errNum = kaps_jr1_lsn_mc_convert_lsn_to_flat_data(self, &(entryToInsert->pfx_bundle), 1, flatData, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(self);
    kaps_jr1_lsn_mc_store_old_lsn_info(self, 0);

    oldBrickList = self->m_lpuList;
    oldNumLpuBricks = self->m_numLpuBricks;
    oldNumPrefix = self->m_nNumPrefixes;
    oldLsnCapacity = self->m_nLsnCapacity;
    oldMlpMemInfo = self->m_mlpMemInfo;

    doesLsnFit = 0;
    kaps_jr1_lsn_mc_convert_flat_data_to_lsn(flatData, self, &doesLsnFit, o_reason);

    kaps_jr1_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, flatData);

    if (!doesLsnFit)
    {
        kaps_jr1_lsn_mc_undo_sort_and_resize(self, oldBrickList, oldNumLpuBricks, oldNumPrefix,
                                         oldLsnCapacity, self->m_mlpMemInfo);
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    errNum =
        kaps_jr1_lsn_mc_pvt_allocate_mlp(self, (uint8_t) self->m_devid, self->m_numLpuBricks, &newMlpMemInfo, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_jr1_lsn_mc_undo_sort_and_resize(self, oldBrickList, oldNumLpuBricks, oldNumPrefix,
                                         oldLsnCapacity, self->m_mlpMemInfo);
        return errNum;
    }

    self->m_mlpMemInfo = newMlpMemInfo;

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(self, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    cur_lpu_brick = self->m_lpuList;
    i = 0;
    while (cur_lpu_brick)
    {
        uint32_t ix_rqt_size = cur_lpu_brick->m_maxCapacity;

        kaps_jr1_lsn_mc_handle_under_alloc(self, cur_lpu_brick, i, &ix_rqt_size);

        errNum =
            kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, cur_lpu_brick, i, ix_rqt_size,
                                                  cur_lpu_brick->ad_db, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_jr1_lsn_mc_free_resources(self);
            kaps_jr1_lsn_mc_undo_sort_and_resize(self, oldBrickList, oldNumLpuBricks, oldNumPrefix,
                                             oldLsnCapacity, oldMlpMemInfo);

            return errNum;

        }

        ++i;
        cur_lpu_brick = cur_lpu_brick->m_next_p;
    }

    self->m_nNumIxAlloced = self->m_nLsnCapacity;

    kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(self, o_reason);




    oldLpuBrick = oldBrickList;
    i = 0;
    while (oldLpuBrick)
    {
        nextBrick = oldLpuBrick->m_next_p;
        if (oldLpuBrick->m_ixInfo)
            kaps_jr1_lsn_mc_check_and_free_ix(self, oldLpuBrick->m_ixInfo, oldMlpMemInfo, i);

        kaps_jr1_nlm_allocator_free(settings_p->m_pAlloc, oldLpuBrick->m_pfxes);
        kaps_jr1_nlm_allocator_free(settings_p->m_pAlloc, oldLpuBrick);
        ++i;
        oldLpuBrick = nextBrick;
    }

    kaps_jr1_lsn_mc_free_mlp_resources(self, oldMlpMemInfo, o_reason);

    NLM_STRY(kaps_jr1_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_jr1_lsn_mc_update_iit(self, o_reason));

    kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(entryToInsert->pfx_bundle);

    *newPfx_pp = entryToInsert->pfx_bundle;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_chunk_move_per_lpu_gran(
    struct uda_mem_chunk *uda_chunk,
    int32_t to_region_id,
    int32_t to_offset,
    int32_t *size)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_lsn_mc *self = (kaps_jr1_lsn_mc *) (uda_chunk->handle);
    kaps_jr1_lsn_mc_settings *settings_p = self->m_pSettings;
    struct kaps_jr1_uda_mgr * mgr = *(settings_p->m_pMlpMemMgr);
    kaps_jr1_flat_lsn_data *flatData = NULL;
    kaps_jr1_lpm_lpu_brick *oldBrickList, *oldLpuBrick, *cur_lpu_brick, *nextBrick;
    uint32_t oldNumLpuBricks, oldNumPrefix, oldLsnCapacity, oldOffset, oldSize;
    uint32_t i;
    struct uda_mem_chunk *oldMlpMemInfo;
    uint32_t doesLsnFit;
    NlmReasonCode o_reason;
    

    flatData = kaps_jr1_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, &o_reason);
    if (!flatData)
    {
        o_reason = NLMRSC_CHUNK_MOVE_FAILED;
        return NLMERR_FAIL;
    }

    errNum = kaps_jr1_lsn_mc_convert_lsn_to_flat_data(self, NULL, 0, flatData, &o_reason);
    if (errNum != NLMERR_OK)
        return errNum;


    oldBrickList = self->m_lpuList;
    oldNumLpuBricks = self->m_numLpuBricks;
    oldNumPrefix = self->m_nNumPrefixes;
    oldLsnCapacity = self->m_nLsnCapacity;
    oldMlpMemInfo = self->m_mlpMemInfo;
    oldOffset = oldMlpMemInfo->offset;
    oldSize = oldMlpMemInfo->size;


    doesLsnFit = 0;
    kaps_jr1_lsn_mc_convert_flat_data_to_lsn(flatData, self, &doesLsnFit, &o_reason);

    kaps_jr1_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, flatData);

    if (!doesLsnFit)
    {
        kaps_jr1_lsn_mc_undo_chunk_move(self, oldBrickList, oldNumLpuBricks, oldNumPrefix,
                                         oldLsnCapacity, self->m_mlpMemInfo, oldOffset, oldSize);
        o_reason = NLMRSC_CHUNK_MOVE_FAILED;
        return NLMERR_FAIL;
    }

    self->m_mlpMemInfo->offset = to_offset;

    i = 0;
    while (i < self->m_numLpuBricks)
    {

        int32_t udc_no1 = kaps_jr1_uda_mgr_compute_brick_udc(mgr, self->m_mlpMemInfo, i + 1);
        if (mgr->config.joined_udcs[udc_no1])
        {
            i+=2;
        }
        else
        {
            i++;
        }
    }
    self->m_mlpMemInfo->size = i;

    if (to_offset)
    {
        uint32_t udc_no1, row_no1;
        uint32_t udc_no2, row_no2;

        kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row_from_offset(mgr, to_region_id, to_offset - 1, &udc_no1, &row_no1);
        kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row_from_offset(mgr, to_region_id, to_offset, &udc_no2, &row_no2);
        if ((udc_no1 % 2 == 0) && (udc_no1 + 1 == udc_no2) && (row_no1 == row_no2))
        {
            kaps_jr1_sassert (0);
        }

    }

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(self, &o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    cur_lpu_brick = self->m_lpuList;
    i = 0;
    while (cur_lpu_brick)
    {
        errNum =
            kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, cur_lpu_brick, i, cur_lpu_brick->m_maxCapacity,
                                                  cur_lpu_brick->ad_db, &o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_jr1_lsn_mc_free_resources_per_lpu(self);
            kaps_jr1_lsn_mc_undo_chunk_move(self, oldBrickList, oldNumLpuBricks, oldNumPrefix,
                                             oldLsnCapacity, oldMlpMemInfo, oldOffset, oldSize);

            return errNum;

        }

        ++i;
        cur_lpu_brick = cur_lpu_brick->m_next_p;
    }


    self->m_nNumIxAlloced = self->m_nLsnCapacity;

    kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(self, &o_reason);

    *size = uda_chunk->size;

    
    self->m_mlpMemInfo->offset = oldOffset;
    self->m_mlpMemInfo->size = oldSize;

    oldLpuBrick = oldBrickList;
    i = 0;
    while (oldLpuBrick)
    {
        nextBrick = oldLpuBrick->m_next_p;
        if (oldLpuBrick->m_ixInfo)
            kaps_jr1_lsn_mc_check_and_free_ix(self, oldLpuBrick->m_ixInfo, oldMlpMemInfo, i);

        kaps_jr1_nlm_allocator_free(settings_p->m_pAlloc, oldLpuBrick->m_pfxes);
        kaps_jr1_nlm_allocator_free(settings_p->m_pAlloc, oldLpuBrick);
        ++i;
        oldLpuBrick = nextBrick;
    }

    
    self->m_mlpMemInfo->offset = to_offset;
    self->m_mlpMemInfo->size = *size;

    NLM_STRY(kaps_jr1_lsn_mc_commit(self, 0, &o_reason));

    NLM_STRY(kaps_jr1_lsn_mc_update_iit(self, &o_reason));

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_move_holes_lsn_gran_to_one_side(
    kaps_jr1_lsn_mc * self,
    uint32_t perform_writes,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lpm_lpu_brick *brickList[KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB];
    kaps_jr1_lpm_lpu_brick *curBrick;
    int32_t holeBrick, pfxBrick, holePos, pfxPos, holeIx, pfxIx;
    int32_t i, totalNumBricks, foundHole, foundPrefix, done;
    kaps_jr1_pfx_bundle *pfxBundle;

    curBrick = self->m_lpuList;
    i = 0;
    while (curBrick)
    {
        brickList[i] = curBrick;
        ++i;
        curBrick = curBrick->m_next_p;
    }
    totalNumBricks = i;

    holeBrick = 0;
    holePos = 0;
    holeIx = self->m_nAllocBase;

    pfxBrick = totalNumBricks - 1;
    pfxPos = brickList[pfxBrick]->m_maxCapacity - 1;
    pfxIx = self->m_nAllocBase + self->m_nLsnCapacity - 1;

    done = 0;
    while (!done)
    {
        
        foundHole = 0;
        while (!foundHole && holeBrick < totalNumBricks)
        {
            if (brickList[holeBrick]->m_pfxes[holePos] == NULL)
            {

                
                if (brickList[holeBrick]->m_hasReservedSlot && holePos == brickList[holeBrick]->m_maxCapacity - 1)
                {
                    foundHole = 0;
                }
                else
                {
                    foundHole = 1;
                    break;
                }
            }

            holePos++;
            if (holePos >= brickList[holeBrick]->m_maxCapacity)
            {
                holeBrick++;
                holePos = 0;
            }
            holeIx++;
        }

        if (!foundHole)
        {
            done = 1;
            break;
        }

        
        foundPrefix = 0;
        while (!foundPrefix && pfxBrick >= 0)
        {
            if (brickList[pfxBrick]->m_pfxes[pfxPos])
            {
                
                if (brickList[pfxBrick]->m_hasReservedSlot && pfxPos == brickList[pfxBrick]->m_maxCapacity - 1)
                {
                    foundPrefix = 0;
                }
                else
                {
                    foundPrefix = 1;
                    break;
                }
            }

            --pfxPos;
            if (pfxPos < 0)
            {
                pfxBrick--;
                if (pfxBrick >= 0)
                    pfxPos = brickList[pfxBrick]->m_maxCapacity - 1;
            }
            --pfxIx;
        }

        if (!foundPrefix)
        {
            done = 1;
            break;
        }

        if ((holeBrick < pfxBrick) || (holeBrick == pfxBrick && holePos < pfxPos))
        {
            
            pfxBundle = brickList[pfxBrick]->m_pfxes[pfxPos];
            brickList[holeBrick]->m_pfxes[holePos] = pfxBundle;
            brickList[pfxBrick]->m_pfxes[pfxPos] = NULL;
            brickList[holeBrick]->m_numPfx++;
            brickList[pfxBrick]->m_numPfx--;

            if (perform_writes)
            {
                uint32_t oldix = pfxIx, newix = holeIx;

                NLM_STRY(kaps_jr1_lsn_mc_write_entry_to_hw(self, brickList[holeBrick]->m_gran,
                                                       pfxBundle, oldix, newix, 0, o_reason));

                NLM_STRY(kaps_jr1_lsn_mc_delete_entry_in_hw(self, oldix, o_reason));
            }

            holePos++;
            if (holePos >= brickList[holeBrick]->m_maxCapacity)
            {
                holeBrick++;
                holePos = 0;
            }
            ++holeIx;

            --pfxPos;
            if (pfxPos < 0)
            {
                pfxBrick--;
                if (pfxBrick >= 0)
                    pfxPos = brickList[pfxBrick]->m_maxCapacity - 1;
            }
            --pfxIx;

        }
        else
        {
            done = 1;
            break;
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_pvt_commit_shrink(
    kaps_jr1_lsn_mc * self,
    int32_t newNumBricks,
    NlmReasonCode * o_reason)
{
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_jr1_lpm_lpu_brick *iterLpuBrick, *nextBrick;
    int32_t i;

    
    iterLpuBrick = self->m_lpuList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        if (i + 1 == newNumBricks)
        {
            iterLpuBrick->m_next_p = NULL;
        }
        else if (i + 1 > newNumBricks)
        {
            kaps_jr1_nlm_allocator_free(alloc_p, iterLpuBrick->m_pfxes);
            kaps_jr1_nlm_allocator_free(alloc_p, iterLpuBrick);
        }
        ++i;
        iterLpuBrick = nextBrick;
    }

    self->m_numLpuBricks = (uint8_t) newNumBricks;

    self->m_nLsnCapacity = newNumBricks * self->m_lpuList->m_maxCapacity;

    NLM_STRY(kaps_jr1_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_jr1_lsn_mc_update_iit(self, o_reason));

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_shrink_per_lsn_gran(
    kaps_jr1_lsn_mc * self,
    NlmReasonCode * o_reason)
{
    uint16_t numHolesPresent = 0;
    uint16_t totalNumBricks = 0, newNumBricks, numCanFreeBricks;
    NlmErrNum_t errNum;
    kaps_jr1_lpm_lpu_brick *curBrick;
    uint32_t numMultiBricksBeforeShrink, numMultiBricksAfterShrink;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    curBrick = self->m_lpuList;
    while (curBrick)
    {
        numHolesPresent += (curBrick->m_maxCapacity - curBrick->m_numPfx);

        
        if (curBrick->m_hasReservedSlot && curBrick->m_pfxes[curBrick->m_maxCapacity - 1] == NULL)
        {
            numHolesPresent--;
        }

        totalNumBricks++;
        curBrick = curBrick->m_next_p;
    }

    curBrick = self->m_lpuList;

    numCanFreeBricks = numHolesPresent / curBrick->m_maxCapacity;

    if (numCanFreeBricks < 1)
        return NLMERR_OK;

    if (self->m_pSettings->m_isJoinedUdc && numCanFreeBricks < 2)
        return NLMERR_OK;

    newNumBricks = totalNumBricks - numCanFreeBricks;

    if (newNumBricks == 0)
        return NLMERR_OK;

    if (self->m_pSettings->m_isMultiBrickAlloc) {
        numMultiBricksBeforeShrink = (totalNumBricks + settings->m_numMultiBricks - 1)  / settings->m_numMultiBricks;
        numMultiBricksAfterShrink = (newNumBricks + settings->m_numMultiBricks - 1)  / settings->m_numMultiBricks;

        if (numMultiBricksAfterShrink >= numMultiBricksBeforeShrink) {
            return NLMERR_OK;
        }

        newNumBricks = numMultiBricksAfterShrink * settings->m_numMultiBricks;
    }

    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(self);

    errNum = kaps_jr1_lsn_mc_acquire_resources(self, newNumBricks * curBrick->m_maxCapacity, newNumBricks, o_reason);

    if (errNum != NLMERR_OK)
    {
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    errNum = kaps_jr1_lsn_mc_move_holes_lsn_gran_to_one_side(self, 0, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    if (self->m_mlpMemInfo->size > newNumBricks)
    {
        newNumBricks = self->m_mlpMemInfo->size;
    }

    kaps_jr1_lsn_mc_free_old_resources(self);

    errNum = kaps_jr1_lsn_mc_pvt_commit_shrink(self, newNumBricks, o_reason);

    self->m_pTrie->m_tbl_ptr->m_fibStats.numLsnShrinks++;

    return errNum;
}

void
kaps_jr1_lsn_mc_undo_shrink(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * oldBrickList,
    uint32_t oldNumLpuBricks,
    uint32_t oldLsnCapacity,
    struct uda_mem_chunk *oldMlpMemInfo)
{
    kaps_jr1_lpm_lpu_brick *newBrickList, *iterLpuBrick, *nextBrick;
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    uint32_t i;

    newBrickList = self->m_lpuList;

    self->m_lpuList = oldBrickList;
    self->m_numLpuBricks = oldNumLpuBricks;
    self->m_nLsnCapacity = oldLsnCapacity;
    self->m_mlpMemInfo = oldMlpMemInfo;

    iterLpuBrick = newBrickList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        kaps_jr1_nlm_allocator_free(alloc_p, iterLpuBrick->m_pfxes);
        kaps_jr1_nlm_allocator_free(alloc_p, iterLpuBrick);
        ++i;
        iterLpuBrick = nextBrick;
    }

    self->m_bAllocedResource = 1;
}

NlmErrNum_t
kaps_jr1_lsn_mc_shrink_per_lpu_gran(
    kaps_jr1_lsn_mc * self,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_lsn_mc_settings *settings_p = self->m_pSettings;
    kaps_jr1_flat_lsn_data *flatData = NULL;
    kaps_jr1_lpm_lpu_brick *oldBrickList, *iterLpuBrick, *cur_lpu_brick, *nextBrick;
    uint32_t oldNumLpuBricks, oldLsnCapacity;
    uint32_t i, shouldTryShrink;
    struct uda_mem_chunk *oldMlpMemInfo;
    struct uda_mem_chunk *newMlpMemInfo;
    uint32_t doesLsnFit, undoShrink;

    if (!self->m_lpuList || !self->m_lpuList->m_next_p || self->m_nNumPrefixes == 0)
        return NLMERR_OK;

    shouldTryShrink = 0;

    iterLpuBrick = self->m_lpuList;
    while (iterLpuBrick)
    {
        if (iterLpuBrick->m_numPfx == 0)
        {
            shouldTryShrink = 1;
            break;
        }
        iterLpuBrick = iterLpuBrick->m_next_p;
    }

    
    if (self->m_numDeletes % 10 == 0)
        shouldTryShrink = 1;

    if (!shouldTryShrink)
        return NLMERR_OK;

    
    flatData = kaps_jr1_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, o_reason);
    if (!flatData)
    {
        return NLMERR_OK;
    }

    errNum = kaps_jr1_lsn_mc_convert_lsn_to_flat_data(self, NULL, 0, flatData, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(self);
    kaps_jr1_lsn_mc_store_old_lsn_info(self, 0);

    oldBrickList = self->m_lpuList;
    oldNumLpuBricks = self->m_numLpuBricks;
    oldLsnCapacity = self->m_nLsnCapacity;
    oldMlpMemInfo = self->m_mlpMemInfo;

    doesLsnFit = 0;
    kaps_jr1_lsn_mc_convert_flat_data_to_lsn(flatData, self, &doesLsnFit, o_reason);

    kaps_jr1_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, flatData);

    undoShrink = 0;

    if (!doesLsnFit)
        undoShrink = 1;

    if (settings_p->m_isJoinedUdc)
    {
        if (self->m_numLpuBricks + 1 >= oldNumLpuBricks)
        {
            undoShrink = 1;
        }
    }
    else
    {
        if (self->m_numLpuBricks >= oldNumLpuBricks)
        {
            undoShrink = 1;
        }
    }

    if (undoShrink)
    {
        kaps_jr1_lsn_mc_undo_shrink(self, oldBrickList, oldNumLpuBricks, oldLsnCapacity, self->m_mlpMemInfo);
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    errNum =
        kaps_jr1_lsn_mc_pvt_allocate_mlp(self, (uint8_t) self->m_devid, self->m_numLpuBricks, &newMlpMemInfo, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_jr1_lsn_mc_undo_shrink(self, oldBrickList, oldNumLpuBricks, oldLsnCapacity, self->m_mlpMemInfo);
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    self->m_mlpMemInfo = newMlpMemInfo;

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(self, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    cur_lpu_brick = self->m_lpuList;
    i = 0;
    while (cur_lpu_brick)
    {
        errNum =
            kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, cur_lpu_brick, i, cur_lpu_brick->m_maxCapacity,
                                                  cur_lpu_brick->ad_db, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_jr1_lsn_mc_free_resources(self);
            kaps_jr1_lsn_mc_undo_shrink(self, oldBrickList, oldNumLpuBricks, oldLsnCapacity, oldMlpMemInfo);

            *o_reason = NLMRSC_REASON_OK;
            return NLMERR_OK;

        }

        ++i;
        cur_lpu_brick = cur_lpu_brick->m_next_p;
    }

    self->m_nNumIxAlloced = self->m_nLsnCapacity;

    kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(self, o_reason);


    iterLpuBrick = oldBrickList;
    i = 0;
    while (iterLpuBrick)
    {
        nextBrick = iterLpuBrick->m_next_p;
        if (iterLpuBrick->m_ixInfo)
            kaps_jr1_lsn_mc_check_and_free_ix(self, iterLpuBrick->m_ixInfo, oldMlpMemInfo, i);

        kaps_jr1_nlm_allocator_free(settings_p->m_pAlloc, iterLpuBrick->m_pfxes);
        kaps_jr1_nlm_allocator_free(settings_p->m_pAlloc, iterLpuBrick);
        ++i;
        iterLpuBrick = nextBrick;
    }

    kaps_jr1_lsn_mc_free_mlp_resources(self, oldMlpMemInfo, o_reason);

    NLM_STRY(kaps_jr1_lsn_mc_commit(self, 0, o_reason));

    NLM_STRY(kaps_jr1_lsn_mc_update_iit(self, o_reason));

    self->m_pTrie->m_tbl_ptr->m_fibStats.numLsnShrinks++;

    return NLMERR_OK;
}


NlmErrNum_t
kaps_jr1_lsn_mc_store_old_lsn_info(
    kaps_jr1_lsn_mc * curLsn,
    uint32_t lsnInfoIndex)
{
    kaps_jr1_lsn_mc_settings *settings = curLsn->m_pSettings;
    kaps_jr1_lpm_lpu_brick *curBrick;
    uint32_t i;
    uint32_t *oldLsnStartIx, *oldLsnNumBricks;
    uint32_t *oldLsnMaxCapacity;

    kaps_jr1_memcpy(settings->m_pTmpMlpMemInfoBuf[lsnInfoIndex], curLsn->m_mlpMemInfo, sizeof(*curLsn->m_mlpMemInfo));
    oldLsnStartIx = &settings->m_startIxOfStoredLsnInfo[lsnInfoIndex];
    oldLsnNumBricks = &settings->m_numBricksInStoredLsnInfo[lsnInfoIndex];
    oldLsnMaxCapacity = settings->m_maxCapacityOfStoredLsnInfo[lsnInfoIndex];
    settings->m_isLsnInfoValid[lsnInfoIndex] = 1;

    if (settings->m_isPerLpuGran)
    {
        *oldLsnStartIx = curLsn->m_lpuList->m_ixInfo->start_ix;
    }
    else
    {
        *oldLsnStartIx = curLsn->m_ixInfo->start_ix;
    }

    *oldLsnNumBricks = curLsn->m_numLpuBricks;

    i = 0;
    curBrick = curLsn->m_lpuList;
    while (curBrick)
    {
        oldLsnMaxCapacity[i] = curBrick->m_maxCapacity;
        curBrick = curBrick->m_next_p;
        ++i;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_acquire_resources(
    kaps_jr1_lsn_mc * self,
    uint32_t ixRqtSize,
    uint8_t rqtNumLpuBricks,
    NlmReasonCode * o_reason)
{
    struct uda_mem_chunk *mlpMemInfo_p = NULL;
    struct kaps_jr1_ix_chunk *ixInfo_p = NULL;
    kaps_jr1_status status;
    NlmErrNum_t errNum = NLMERR_OK;

    errNum = kaps_jr1_lsn_mc_pvt_allocate_mlp(self, (uint8_t) self->m_devid, rqtNumLpuBricks, &mlpMemInfo_p, o_reason);

    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    
    if (mlpMemInfo_p->size > rqtNumLpuBricks)
    {
        ixRqtSize = (ixRqtSize / rqtNumLpuBricks) * mlpMemInfo_p->size;
    }

    if (!self->m_pSettings->m_isPerLpuGran)
    {
        if (self->m_pSettings->m_isHardwareMappedIx)
        {
            struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) self->m_pTrie->m_tbl_ptr->m_db->common_info->ad_info.ad;

            errNum = kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lsn(self, mlpMemInfo_p, ad_db, &ixInfo_p, o_reason);
            if (errNum != NLMERR_OK)
            {
                kaps_jr1_lsn_mc_decr_num_bricks_allocated_for_all_lsns(self->m_pSettings, mlpMemInfo_p->size);
                kaps_jr1_uda_mgr_free(self->m_pSettings->m_pMlpMemMgr[self->m_devid], mlpMemInfo_p);
                return errNum;
            }

        }
        else
        {
            struct kaps_jr1_ad_db *ad_db = kaps_jr1_lsn_mc_get_ad_db(self, self->m_lpuList);
            struct kaps_jr1_ix_mgr *pIxMgr = kaps_jr1_lsn_mc_get_ix_mgr_for_lsn_pfx(self, ad_db,
                                                                    mlpMemInfo_p, 0);

            status = kaps_jr1_ix_mgr_alloc(pIxMgr, NULL, ixRqtSize, self, IX_USER_LSN, &ixInfo_p);
            if (status != KAPS_JR1_OK)
            {
                kaps_jr1_lsn_mc_decr_num_bricks_allocated_for_all_lsns(self->m_pSettings, mlpMemInfo_p->size);
                kaps_jr1_uda_mgr_free(self->m_pSettings->m_pMlpMemMgr[self->m_devid], mlpMemInfo_p);
                errNum = kaps_jr1_trie_convert_status_to_err_num(status, o_reason);
                return errNum;
            }
        }
    }

    if (self->m_bAllocedResource)
    {
        kaps_jr1_lsn_mc_store_old_lsn_info(self, 0);

        self->m_pSettings->m_oldMlpMemInfoPtr = self->m_mlpMemInfo;
        self->m_pSettings->m_oldIxChunkForLsn = self->m_ixInfo;
    }

    self->m_mlpMemInfo = mlpMemInfo_p;

    if (!self->m_pSettings->m_isPerLpuGran)
    {
        self->m_ixInfo = ixInfo_p;

        self->m_nAllocBase = ixInfo_p->start_ix;
        self->m_nNumIxAlloced = (uint16_t) ixInfo_p->size;
    }

    self->m_bAllocedResource = 1;

    kaps_jr1_lsn_mc_check_joined_udc_alloc(self, rqtNumLpuBricks, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_jr1_lsn_mc_undo_acquire_resources(
    kaps_jr1_lsn_mc * self)
{
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    if (self->m_ixInfo)
    {
        kaps_jr1_lsn_mc_check_and_free_ix(self, self->m_ixInfo, self->m_mlpMemInfo, 0);
    }

    if (self->m_mlpMemInfo)
    {
        kaps_jr1_lsn_mc_decr_num_bricks_allocated_for_all_lsns(settings, self->m_mlpMemInfo->size);
        kaps_jr1_uda_mgr_free(settings->m_pMlpMemMgr[self->m_devid], self->m_mlpMemInfo);
    }


    self->m_mlpMemInfo = settings->m_oldMlpMemInfoPtr;
    self->m_ixInfo = settings->m_oldIxChunkForLsn;

    if (self->m_ixInfo)
    {
        self->m_nAllocBase = self->m_ixInfo->start_ix;
        self->m_nNumIxAlloced = self->m_ixInfo->size;
    }

    settings->m_oldMlpMemInfoPtr = NULL;
    settings->m_oldIxChunkForLsn = NULL;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_free_old_resources(
    kaps_jr1_lsn_mc * self)
{
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    if (settings->m_oldIxChunkForLsn)
    {
        kaps_jr1_lsn_mc_check_and_free_ix(self, settings->m_oldIxChunkForLsn,
                        settings->m_oldMlpMemInfoPtr, 0);
    }

    if (settings->m_oldMlpMemInfoPtr)
    {
        kaps_jr1_lsn_mc_decr_num_bricks_allocated_for_all_lsns(settings, settings->m_oldMlpMemInfoPtr->size);
        kaps_jr1_uda_mgr_free(settings->m_pMlpMemMgr[self->m_devid], settings->m_oldMlpMemInfoPtr);
    }

    settings->m_oldMlpMemInfoPtr = NULL;
    settings->m_oldIxChunkForLsn = NULL;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lpu(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * brick,
    uint32_t brickIter,
    struct kaps_jr1_ad_db * ad_db,
    struct kaps_jr1_ix_chunk ** ixInfo_pp,
    NlmReasonCode * o_reason)
{
    struct kaps_jr1_ix_chunk *ixInfo_p = NULL;
    uint32_t cur_lpu, cur_row;
    uint32_t maxNumPfxInAnyBrick;
    kaps_jr1_lsn_mc_settings *settings_p = self->m_pSettings;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t total_num_bbs_in_final_level = kaps_jr1_device_get_num_final_level_bbs(device, self->m_pTbl->m_db);

    kaps_jr1_sassert(self->m_pSettings->m_isHardwareMappedIx);

    ixInfo_p = kaps_jr1_nlm_allocator_calloc(self->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_jr1_ix_chunk));
    if (!ixInfo_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    ixInfo_p->ad_info = kaps_jr1_nlm_allocator_calloc(self->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_jr1_ad_chunk));
    if (!ixInfo_p->ad_info)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    maxNumPfxInAnyBrick = settings_p->m_maxNumPfxInAnyBrick;

    kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), self->m_mlpMemInfo, brickIter, &cur_lpu,
                                               &cur_row);

    ixInfo_p->start_ix = (cur_row * total_num_bbs_in_final_level  * maxNumPfxInAnyBrick) + (cur_lpu * maxNumPfxInAnyBrick);

    ixInfo_p->size = maxNumPfxInAnyBrick;
    ixInfo_p->type = IX_ALLOCATED_CHUNK;
    ixInfo_p->user_type = IX_USER_LSN;
    ixInfo_p->lsn_ptr = self;
    ixInfo_p->ad_info->ad_db = ad_db;
    ixInfo_p->ad_info->device = device;

    *ixInfo_pp = ixInfo_p;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lsn(
    kaps_jr1_lsn_mc * self,
    struct uda_mem_chunk * mlpMemInfo,
    struct kaps_jr1_ad_db * ad_db,
    struct kaps_jr1_ix_chunk ** ixInfo_pp,
    NlmReasonCode * o_reason)
{
    struct kaps_jr1_ix_chunk *ixInfo_p = NULL;
    uint32_t cur_lpu, cur_row;
    uint32_t maxNumPfxInAnyBrick;
    kaps_jr1_lsn_mc_settings *settings_p = self->m_pSettings;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t total_num_bbs_in_final_level = kaps_jr1_device_get_num_final_level_bbs(device, self->m_pTbl->m_db);

    kaps_jr1_sassert(self->m_pSettings->m_isHardwareMappedIx);

    ixInfo_p = kaps_jr1_nlm_allocator_calloc(self->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_jr1_ix_chunk));
    if (!ixInfo_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    ixInfo_p->ad_info = kaps_jr1_nlm_allocator_calloc(self->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_jr1_ad_chunk));
    if (!ixInfo_p->ad_info)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), mlpMemInfo, 0, &cur_lpu, &cur_row);

    maxNumPfxInAnyBrick = settings_p->m_maxNumPfxInAnyBrick;

    ixInfo_p->start_ix = (cur_row * total_num_bbs_in_final_level * maxNumPfxInAnyBrick) + (cur_lpu * maxNumPfxInAnyBrick);

    ixInfo_p->size = maxNumPfxInAnyBrick * mlpMemInfo->size;
    ixInfo_p->type = IX_ALLOCATED_CHUNK;
    ixInfo_p->user_type = IX_USER_LSN;
    ixInfo_p->lsn_ptr = self;
    ixInfo_p->ad_info->ad_db = ad_db;
    ixInfo_p->ad_info->device = device;

    *ixInfo_pp = ixInfo_p;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_acquire_resources_per_lpu(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * brick,
    uint32_t brickIter,
    uint32_t ixRqtSize,
    struct kaps_jr1_ad_db * ad_db,
    NlmReasonCode * o_reason)
{
    struct kaps_jr1_ix_chunk *ixInfo_p = NULL;
    struct kaps_jr1_ix_mgr *pIxMgr;
    kaps_jr1_status status;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    if (settings->m_isHardwareMappedIx)
    {
        errNum = kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lpu(self, brick, brickIter, ad_db, &ixInfo_p, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;

    }
    else
    {
        pIxMgr = kaps_jr1_lsn_mc_get_ix_mgr_for_lsn_pfx(self, ad_db, self->m_mlpMemInfo, brickIter);
        status = kaps_jr1_ix_mgr_alloc(pIxMgr, ad_db, ixRqtSize, self, IX_USER_LSN, &ixInfo_p);
        if (status != KAPS_JR1_OK)
        {
            errNum = kaps_jr1_trie_convert_status_to_err_num(status, o_reason);
            return errNum;
        }
    }

    brick->m_ixInfo = ixInfo_p;

    return errNum;
}

NlmErrNum_t
kaps_jr1_lsn_mc_backup_resources_per_lpu(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_lpm_lpu_brick * brick,
    struct kaps_jr1_ix_chunk * oldIxInfo)
{
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    if (settings->m_numBackupLpuBrickResources < KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB)
    {
        settings->m_oldLpuBrick[settings->m_numBackupLpuBrickResources] = brick;
        settings->m_oldLpuIxChunk[settings->m_numBackupLpuBrickResources] = oldIxInfo;
        settings->m_numBackupLpuBrickResources++;
    }
    else
    {
        kaps_jr1_assert(0, "Too many brick resources to back up");
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_undo_acquire_resources_per_lpu(
    kaps_jr1_lsn_mc * self)
{
    uint32_t i;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;
    kaps_jr1_lpm_lpu_brick *curBrick;

    for (i = 0; i < settings->m_numBackupLpuBrickResources; ++i)
    {
        curBrick = settings->m_oldLpuBrick[i];

        if (curBrick->m_ixInfo != settings->m_oldLpuIxChunk[i])
        {
            kaps_jr1_lsn_mc_check_and_free_ix(self, curBrick->m_ixInfo,
                                    self->m_mlpMemInfo, i);
        }

        curBrick->m_ixInfo = settings->m_oldLpuIxChunk[i];

        settings->m_oldLpuBrick[i] = NULL;
        settings->m_oldLpuIxChunk[i] = NULL;
    }

    settings->m_numBackupLpuBrickResources = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_free_old_resources_per_lpu(
    kaps_jr1_lsn_mc * self)
{
    uint32_t i;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    for (i = 0; i < settings->m_numBackupLpuBrickResources; ++i)
    {
        if (settings->m_oldLpuIxChunk[i])
        {
            kaps_jr1_lsn_mc_check_and_free_ix(self, settings->m_oldLpuIxChunk[i],
                                    settings->m_oldMlpMemInfoPtr, i);

            settings->m_oldLpuIxChunk[i] = NULL;
        }
        settings->m_oldLpuBrick[i] = NULL;
    }

    settings->m_numBackupLpuBrickResources = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_add_pfx_to_new_lsn(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry * entryToInsert,
    kaps_jr1_pfx_bundle ** newPfx_pp,
    NlmReasonCode * o_reason)
{
    kaps_jr1_pfx_bundle *newPfxBundle;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_jr1_lpm_lpu_brick *newBrick, *prevBrick, *curBrick, *nextBrick;
    uint16_t newPfxGran, newPfxGranIx, lengthAfterLopoff;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    uint32_t freeSlotLoc;
    uint32_t i, numBricks;

    *newPfx_pp = NULL;

    lengthAfterLopoff = (uint16_t) (entryToInsert->pfx_bundle->m_nPfxSize - self->m_nDepth);
    newPfxGran = kaps_jr1_lsn_mc_compute_gran(self->m_pSettings, lengthAfterLopoff, &newPfxGranIx);

    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entryToInsert->ad_handle, ad_db);

    numBricks = 1;
    if (settings->m_isFullWidthLsn)
    {
        numBricks = settings->m_maxLpuPerLsn;
    }

    if (settings->m_isMultiBrickAlloc)
    {
        numBricks = settings->m_numMultiBricks;
    }

    prevBrick = NULL;
    newBrick = NULL;
    for (i = 0; i < numBricks; ++i)
    {
        newBrick = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!newBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }


        newBrick->ad_db = ad_db;
        newBrick->m_gran = newPfxGran;
        newBrick->m_granIx = newPfxGranIx;
        newBrick->meta_priority = entryToInsert->meta_priority;
        newBrick->m_maxCapacity = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(self->m_pSettings, newBrick->ad_db,
                                                                        newBrick->m_hasReservedSlot,
                                                                        newBrick->meta_priority, newPfxGran);

        newBrick->m_pfxes = kaps_jr1_nlm_allocator_calloc(alloc_p, newBrick->m_maxCapacity, sizeof(kaps_jr1_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            kaps_jr1_nlm_allocator_free(alloc_p, newBrick);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_next_p = prevBrick;
        prevBrick = newBrick;
    }

    if (newBrick == NULL)
    {
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }


    errNum = kaps_jr1_lsn_mc_acquire_resources(self, newBrick->m_maxCapacity * numBricks, numBricks, o_reason);

    if (errNum != NLMERR_OK)
    {
        curBrick = newBrick;
        while (curBrick)
        {
            nextBrick = curBrick->m_next_p;
            kaps_jr1_nlm_allocator_free(alloc_p, curBrick->m_pfxes);
            kaps_jr1_nlm_allocator_free(alloc_p, curBrick);
            curBrick = nextBrick;
        }
        return errNum;
    }

    if (self->m_pSettings->m_isPerLpuGran)
    {
        errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(self, newBrick, 0, newBrick->m_maxCapacity, ad_db, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_jr1_lsn_mc_undo_acquire_resources(self);
            curBrick = newBrick;
            while (curBrick)
            {
                nextBrick = curBrick->m_next_p;
                kaps_jr1_nlm_allocator_free(alloc_p, curBrick->m_pfxes);
                kaps_jr1_nlm_allocator_free(alloc_p, curBrick);
                curBrick = nextBrick;
            }
            self->m_mlpMemInfo = NULL;
            self->m_bAllocedResource = 0;
            return errNum;
        }

        self->m_nNumIxAlloced = newBrick->m_maxCapacity;
    }

    kaps_jr1_lsn_mc_free_old_resources(self);

    newPfxBundle = entryToInsert->pfx_bundle;

    kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(newPfxBundle);

    freeSlotLoc = 0;

    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds
        && self->m_nDepth == newPfxBundle->m_nPfxSize)
    {
        kaps_jr1_trie_node *trienode;
        void *tmp_ptr;

        tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);
        kaps_jr1_memcpy(&trienode, tmp_ptr, sizeof(kaps_jr1_trie_node *));

        newPfxBundle->m_nIndex = KAPS_JR1_STRICT_LMPSOFAR_INDEX;

        trienode->m_iitLmpsofarPfx_p = newPfxBundle;

        newPfxBundle->m_isLmpsofarPfx = 1;


    }
    else
    {
        newBrick->m_pfxes[freeSlotLoc] = newPfxBundle;
        self->m_nNumPrefixes = 1;
        newBrick->m_numPfx = 1;
    }


    self->m_lpuList = newBrick;
    self->m_numLpuBricks = numBricks;

    self->m_nLsnCapacity = newBrick->m_maxCapacity * numBricks;

    kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(self, o_reason);

    NLM_STRY(kaps_jr1_lsn_mc_commit(self, 0, o_reason));

    *newPfx_pp = newPfxBundle;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_insert_pfx(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason)
{
    kaps_jr1_pfx_bundle *newpfx = NULL;
    struct kaps_jr1_db *db = self->m_pTbl->m_db;
    uint32_t rpb_id = db->rpb_id;
    struct kaps_jr1_device *device = db->device;

    kaps_jr1_assert(entry->pfx_bundle->m_nPfxSize >= self->m_nDepth, "prefix length is < LSN depth");

    if (self->m_bIsNewLsn)
    {

        kaps_jr1_pool_mgr *poolMgr = self->m_pTrie->m_trie_global->poolMgr;
        if (kaps_jr1_pool_mgr_get_num_entries(poolMgr, 0, KAPS_JR1_IPT_POOL) >=
            device->hw_res->num_rows_in_rpb[rpb_id])
        {
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;
            return NLMERR_FAIL;
        }

        NLM_STRY(kaps_jr1_lsn_mc_add_pfx_to_new_lsn(self, entry, &newpfx, o_reason));

    }
    else
    {

        
        NLM_STRY(kaps_jr1_lsn_mc_try_fast_insert(self, entry, &newpfx, o_reason));

        if (!newpfx)
        {
            if (self->m_pSettings->m_isPerLpuGran)
            {
                kaps_jr1_lsn_mc_resize_per_lpu_gran(self, entry, &newpfx, o_reason);
            }
            else
            {
                kaps_jr1_lsn_mc_resize_per_lsn_gran(self, entry, &newpfx, o_reason);
            }
        }

        if (!newpfx)
        {
            if (*o_reason == NLMRSC_REASON_OK || *o_reason == NLMRSC_UDA_ALLOC_FAILED)
            {
                
                if (!self->m_pTrie->m_trie_global->m_areAllPoolsHealthy)
                {
                    *o_reason = NLMRSC_DBA_ALLOC_FAILED;
                    return NLMERR_FAIL;
                }

                
                self->m_bDoGiveout = 1;
                return NLMERR_OK;

            }
            else
            {
                return NLMERR_FAIL;
            }
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_delete_pfx(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry **entrySlotInHash,
    NlmReasonCode * o_reason)
{
    int32_t found_in_lsn = 0;
    kaps_jr1_pfx_bundle *pfxBundleInHash = NULL;
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    struct kaps_jr1_db *db;
    struct kaps_jr1_lpm_entry *e = NULL;

    kaps_jr1_sassert(entrySlotInHash != NULL);

    e = (*entrySlotInHash);
    KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(self->m_pSettings->m_device, e, db);

    self->m_numDeletes++;
    pfxBundleInHash = (*entrySlotInHash)->pfx_bundle;


    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds
        && self->m_nDepth == pfxBundleInHash->m_nPfxSize)
    {
        
        kaps_jr1_trie_node *trienode;
        void *tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(self->m_pParentHandle);

        kaps_jr1_memcpy(&trienode, tmp_ptr, sizeof(kaps_jr1_trie_node *));

        trienode->m_iitLmpsofarPfx_p = NULL;

        return NLMERR_OK;
    }



    {
        uint32_t curIx = self->m_nAllocBase;
        uint32_t indexInLpu;

        curLpuBrick = self->m_lpuList;

        while (curLpuBrick)
        {
            kaps_jr1_pfx_bundle *item;
            uint32_t maxNumPfxInLpuBrick = curLpuBrick->m_maxCapacity;
            if (self->m_pSettings->m_isPerLpuGran)
            {
                curIx = curLpuBrick->m_ixInfo->start_ix;
                maxNumPfxInLpuBrick = curLpuBrick->m_ixInfo->size;
                if (maxNumPfxInLpuBrick < curLpuBrick->m_ixInfo->size)
                    kaps_jr1_sassert(curLpuBrick->m_underAllocatedIx == 1);
            }

            if (curIx <= pfxBundleInHash->m_nIndex && pfxBundleInHash->m_nIndex < curIx + maxNumPfxInLpuBrick)
            {

                indexInLpu = pfxBundleInHash->m_nIndex - curIx;
                item = curLpuBrick->m_pfxes[indexInLpu];
                if (item)
                {
                    if (self->m_pTrie->m_hashtable_p && pfxBundleInHash == item)
                    {
                        found_in_lsn = 1;

                        NLM_STRY(kaps_jr1_lsn_mc_pvt_remove_entry_normal
                                 (self, curLpuBrick, item, indexInLpu, entrySlotInHash, o_reason));
                    }
                }
                break;
            }

            curIx += maxNumPfxInLpuBrick;
            curLpuBrick = curLpuBrick->m_next_p;
        }
    }

    if (!found_in_lsn)
    {
        kaps_jr1_assert(0, "Inconsistency between hash table and lsn bucket ");
        *o_reason = NLMRSC_PREFIX_NOT_FOUND;
        return NLMERR_FAIL;
    }

    {

        if (e->hb_user_handle)
        {
            struct kaps_jr1_hb *hb = NULL;
            struct kaps_jr1_hb_db *hb_db;
            struct kaps_jr1_lpm_db *lpm_db = (struct kaps_jr1_lpm_db *) db;
            uint32_t hit_bit_value = 0;
            uint8_t clear_on_read;
            struct kaps_jr1_aging_entry *active_aging_table;

            (void) hit_bit_value;
            (void) clear_on_read;

            KAPS_JR1_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb), (uintptr_t) e->hb_user_handle);

            
            clear_on_read = 1;
            hb_db = (struct kaps_jr1_hb_db *) db->common_info->hb_info.hb;
            lpm_db->is_entry_delete_in_progress = 1;
            kaps_jr1_hb_entry_get_bit_value(hb_db, KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_hb, e->hb_user_handle),
                                        &hit_bit_value, clear_on_read);
            lpm_db->is_entry_delete_in_progress = 0;

            active_aging_table = kaps_jr1_device_get_active_aging_table(db->device, db);

            active_aging_table[hb->bit_no].entry = NULL;
            active_aging_table[hb->bit_no].num_idles = 0;
        }
    }

    if (self->m_pSettings->m_isShrinkEnabled && !db->is_destroy_in_progress)
    {
        if (self->m_pSettings->m_isPerLpuGran)
        {
            NLM_STRY(kaps_jr1_lsn_mc_shrink_per_lpu_gran(self, o_reason));
        }
        else
        {
            NLM_STRY(kaps_jr1_lsn_mc_shrink_per_lsn_gran(self, o_reason));
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_update_ad(
    kaps_jr1_lsn_mc * self,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason,
    uint32_t update_type)
{
    kaps_jr1_pfx_bundle *curPfxBundle = entry->pfx_bundle;
    kaps_jr1_lpm_lpu_brick *curLpuBrick = self->m_lpuList;
    uint32_t curBrickStartIx = 0;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = self->m_pTbl->m_fibTblMgr_p->m_devMgr_p;
    NlmErrNum_t errNum;

    if (self->m_pSettings->m_strictlyStoreLmpsofarInAds
        && self->m_nDepth == entry->pfx_bundle->m_nPfxSize)
    {
        
        return NLMERR_OK;
    }

    if (!self->m_pSettings->m_isPerLpuGran)
        curBrickStartIx = self->m_ixInfo->start_ix;

    while (curLpuBrick)
    {
        uint32_t maxPfxInLpu = curLpuBrick->m_maxCapacity;
        if (self->m_pSettings->m_isPerLpuGran)
        {
            curBrickStartIx = curLpuBrick->m_ixInfo->start_ix;
            if (curLpuBrick->m_underAllocatedIx)
                maxPfxInLpu = curLpuBrick->m_ixInfo->size;
        }
        if (curBrickStartIx <= curPfxBundle->m_nIndex && curPfxBundle->m_nIndex < (curBrickStartIx + maxPfxInLpu))
            break;
        curBrickStartIx += curLpuBrick->m_maxCapacity;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (!curLpuBrick)
    {
        kaps_jr1_assert(0, "Unable to find the prefix in the LSN\n");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }


    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, ad_db);

    if (ad_db && ad_db->db_info.hw_res.ad_res->ad_type == KAPS_JR1_AD_TYPE_INDIRECTION_WITH_DUPLICATION)
    {
        uint32_t whereInLpu = 0;

        for (whereInLpu = 0; whereInLpu < curLpuBrick->m_maxCapacity; whereInLpu++)
        {
            if (curLpuBrick->m_pfxes[whereInLpu] == curPfxBundle)
                break;
        }
        kaps_jr1_sassert(whereInLpu < curLpuBrick->m_maxCapacity);

        if (update_type == 2)
            return NLMERR_OK;
    }


    errNum = kaps_jr1_lsn_mc_write_entry_to_hw(self, curLpuBrick->m_gran, curPfxBundle, curPfxBundle->m_nIndex,
                                           curPfxBundle->m_nIndex, update_type, o_reason);

    return errNum;
}

NlmErrNum_t
kaps_jr1_lsn_mc_submit_rqt(
    kaps_jr1_lsn_mc * self,
    NlmTblRqtCode rqtCode,
    struct kaps_jr1_lpm_entry * entry,
    struct kaps_jr1_lpm_entry ** entrySlotInHash,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t retVal = 0;
    kaps_jr1_lsn_mc_settings *settings = self->m_pSettings;

    if (rqtCode == NLM_FIB_PREFIX_INSERT)
    {
        retVal = kaps_jr1_lsn_mc_insert_pfx(self, entry, o_reason);

    }
    else if (rqtCode == NLM_FIB_PREFIX_INSERT_WITH_INDEX)
    {
        retVal = kaps_jr1_lsn_mc_wb_insert_pfx(self, entry->pfx_bundle->m_nIndex, entry);

    }
    else if (rqtCode == NLM_FIB_PREFIX_DELETE)
    {
        retVal = kaps_jr1_lsn_mc_delete_pfx(self, entrySlotInHash, o_reason);

    }
    else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD)
    {
        retVal = kaps_jr1_lsn_mc_update_ad(self, entry, o_reason, 1);

    }
    else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD_ADDRESS)
    {
        retVal = kaps_jr1_lsn_mc_update_ad(self, entry, o_reason, 2);
    }
    else
    {
        kaps_jr1_assert(0, "We only handle inserts, deletes and update AD");
        *o_reason = NLMRSC_INTERNAL_ERROR;
        retVal = NLMERR_FAIL;
    }

    kaps_jr1_assert(settings->m_oldMlpMemInfoPtr == NULL, "old UDA resources have to be cleared");
    kaps_jr1_assert(settings->m_oldIxChunkForLsn == NULL, "old IX resources have to be cleared");
    kaps_jr1_assert(settings->m_numBackupLpuBrickResources == 0, "old Lpu Brick Resources have to be cleared");

    return retVal;
}

static kaps_jr1_pfx_bundle *
kaps_jr1_lsn_mc_pvt__locate_pfx_exact(
    kaps_jr1_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    uint32_t * retix,
    uint32_t * match_brick_num)
{
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    uint32_t cur_brick_num = 0;

    curLpuBrick = self->m_lpuList;
    while (curLpuBrick)
    {
        uint32_t i;
        uint32_t len = (pfxlen + 7) >> 3;

        
        int32_t lenN1 = len - 1;
        uint32_t ch1 = 0;

        if (lenN1 >= 0)
            ch1 = pfxdata[lenN1];

        for (i = 0; i < curLpuBrick->m_maxCapacity; i++)
        {
            kaps_jr1_pfx_bundle *item = curLpuBrick->m_pfxes[i];

            if (item)
            {
                uint8_t *data2 ;
                const uint8_t *pfxp;

                
                if (item->m_nPfxSize != pfxlen)
                    continue;

                if (item->m_nPfxSize == 0 && pfxlen == 0)
                {
                    
                    *retix = item->m_nIndex;
                    *match_brick_num = cur_brick_num;
                    return item;
                }


                data2 = KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(item) + lenN1;

                
                if (item->m_isPfxCopy)
                    continue;

                
                if (ch1 != *data2)
                    continue;

                pfxp = pfxdata + lenN1;
                for (;;)
                {
                    if (pfxp-- == pfxdata)
                    {
                        *retix = item->m_nIndex;
                        *match_brick_num = cur_brick_num;
                        return item;
                    }
                    data2--;
                    if (*pfxp != *data2)
                        break;
                }
            }
        }
        curLpuBrick = curLpuBrick->m_next_p;
        cur_brick_num++;
    }

    return 0;
}

kaps_jr1_pfx_bundle *
kaps_jr1_lsn_mc_locate_exact(
    kaps_jr1_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    uint32_t * match_brick_num)
{
    uint32_t ix;

    return kaps_jr1_lsn_mc_pvt__locate_pfx_exact(self, pfxdata, pfxlen, &ix, match_brick_num);
}


static kaps_jr1_pfx_bundle *
kaps_jr1_lsn_mc_pvt_do_lpm(
    kaps_jr1_pfx_bundle ** bundles,
    uint32_t nBundles,
    uint32_t len,
    const uint8_t * pfxdata)
{
    uint32_t ch1, memcmplen;

    kaps_jr1_assert(len > 0, "prefix length is 0");

    memcmplen = len >> 3;

    len &= 0x7;
    if (len)
    {   
        ch1 = pfxdata[memcmplen];       
        ch1 &= 0xff << (8 - len);
    }
    else
    {
        ch1 = pfxdata[--memcmplen];
    }

    while (nBundles-- > 0)
    {
        kaps_jr1_pfx_bundle *item = *bundles++;
        if (item)
        {
            uint8_t *data2 = KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(item) + memcmplen;
            const uint8_t *pfxp;
            

            if (ch1 != *data2)
                continue;       

            if (!memcmplen)
                return item;
            data2--;
            pfxp = pfxdata + memcmplen - 1;
            for (;;)
            {
                if (*pfxp != *data2)
                    break;
                if (pfxp == pfxdata)
                    return item;
                pfxp--;
                data2--;
            }
        }
    }
    return 0;
}

static kaps_jr1_pfx_bundle *
kaps_jr1_lsn_mc_pvt_do_lpm_zero_len(
    kaps_jr1_pfx_bundle ** bundles,
    uint32_t nBundles)
{
    while (nBundles-- > 0)
    {
        kaps_jr1_pfx_bundle *item = *bundles++;
        if (item)
            return item;
    }

    return 0;
}

kaps_jr1_pfx_bundle *
kaps_jr1_lsn_mc_locate_lpm(
    kaps_jr1_lsn_mc * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    struct kaps_jr1_lpm_entry * skipEntry,
    uint32_t * matchBrickIter,
    uint32_t * matchPosInBrick)
{
    kaps_jr1_pfx_bundle *item, *found, *lpmMatch = NULL;
    uint32_t len, i, brickIter;
    kaps_jr1_lpm_lpu_brick *curLpuBrick;

    kaps_jr1_assert(pfxlen < 0x10000, "Invalid prefix length");

    curLpuBrick = self->m_lpuList;
    brickIter = 0;
    while (curLpuBrick)
    {
        for (i = 0; i < curLpuBrick->m_maxCapacity; i++)
        {
            item = curLpuBrick->m_pfxes[i];
            if (!item)
                continue;

            len = KAPS_JR1_PFX_BUNDLE_GET_PFX_SIZE(item);

            if (len > pfxlen)
                continue;

            if (len)
            {
                found = kaps_jr1_lsn_mc_pvt_do_lpm(&item, 1, len, pfxdata);
            }
            else
            {
                
                found = kaps_jr1_lsn_mc_pvt_do_lpm_zero_len(&item, 1);
            }

            if (found && skipEntry)
            {
                if (found && found->m_backPtr == skipEntry)
                {
                    found = NULL;
                }
            }

            
            if (found)
            {
                if (lpmMatch == NULL)
                {
                    lpmMatch = found;

                    if (matchBrickIter)
                        *matchBrickIter = brickIter;

                    if (matchPosInBrick)
                        *matchPosInBrick = i;
                }
                else
                {
                    if (lpmMatch->m_backPtr->meta_priority > found->m_backPtr->meta_priority
                        || (lpmMatch->m_backPtr->meta_priority == found->m_backPtr->meta_priority
                            && lpmMatch->m_nPfxSize < found->m_nPfxSize)
                        || (lpmMatch->m_backPtr->meta_priority == found->m_backPtr->meta_priority
                            && lpmMatch->m_nPfxSize == found->m_nPfxSize && lpmMatch->m_isPfxCopy))
                    {
                        lpmMatch = found;

                        if (matchBrickIter)
                            *matchBrickIter = brickIter;

                        if (matchPosInBrick)
                            *matchPosInBrick = i;
                    }
                }
            }
        }
        ++brickIter;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    return lpmMatch;
}

kaps_jr1_flat_lsn_data *
kaps_jr1_lsn_mc_create_flat_lsn_data(
    kaps_jr1_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason)
{
    kaps_jr1_flat_lsn_data *ret = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_flat_lsn_data));

    if (!ret)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
    }

    return ret;
}

NlmErrNum_t
kaps_jr1_lsn_mc_destroy_flat_lsn_data(
    kaps_jr1_nlm_allocator * alloc_p,
    kaps_jr1_flat_lsn_data * flatLsnData_p)
{
    kaps_jr1_prefix_destroy(flatLsnData_p->m_commonPfx, alloc_p);
    kaps_jr1_nlm_allocator_free(alloc_p, flatLsnData_p);
    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_pvt_compute_pfx_sort_data(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_pfx_bundle * extraPfxArray[],
    uint32_t numExtraPfx,
    uint16_t numPfxForEachLength[],
    uint16_t whereForEachLength[])
{
    uint16_t i, numPfxInLsn, curLoc;
    kaps_jr1_lpm_lpu_brick *curLpuBrick = self->m_lpuList;
    uint32_t length;
    kaps_jr1_pfx_bundle *newPfxBundle;

    for (i = 0; i <= KAPS_JR1_LPM_KEY_MAX_WIDTH_1; ++i)
    {
        numPfxForEachLength[i] = 0;
    }

    numPfxInLsn = 0;

    while (curLpuBrick)
    {
        if (curLpuBrick->m_numPfx == 0)
        {
            curLpuBrick = curLpuBrick->m_next_p;
            continue;
        }

        for (i = 0; i < curLpuBrick->m_maxCapacity; ++i)
        {
            kaps_jr1_pfx_bundle *bundle = curLpuBrick->m_pfxes[i];
            if (bundle)
            {
                length = bundle->m_nPfxSize;
                numPfxForEachLength[length]++;
                ++numPfxInLsn;
            }
        }

        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (extraPfxArray)
    {
        for (i = 0; i < numExtraPfx; ++i)
        {
            newPfxBundle = extraPfxArray[i];

            length = newPfxBundle->m_nPfxSize;
            numPfxForEachLength[length]++;
            ++numPfxInLsn;
        }
    }

    curLoc = 0;
    for (i = 0; i <= KAPS_JR1_LPM_KEY_MAX_WIDTH_1; ++i)
    {
        whereForEachLength[i] = curLoc;
        curLoc += numPfxForEachLength[i];
    }

    return NLMERR_OK;
}

void
kaps_jr1_lsn_mc_assign_flat_data_colors(
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_flat_lsn_data * flatLsnData_p)
{
    flatLsnData_p->num_colors = 1;
    return;
}

NlmErrNum_t
kaps_jr1_lsn_mc_convert_lsn_to_flat_data(
    kaps_jr1_lsn_mc * self,
    kaps_jr1_pfx_bundle * extraPfxArray[],
    uint32_t numExtraPfx,
    kaps_jr1_flat_lsn_data * flatLsnData_p,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lpm_lpu_brick *curLpuBrick = self->m_lpuList;
    uint32_t i;
    uint16_t numPfxInLsn;
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    kaps_jr1_pfx_bundle *newPfxBundle = NULL;
    uint16_t numPfxForEachLength[KAPS_JR1_LPM_KEY_MAX_WIDTH_1 + 1], whereForEachLength[KAPS_JR1_LPM_KEY_MAX_WIDTH_1 + 1];
    uint32_t length;
    uint16_t curLoc;
    uint32_t sortData;

    flatLsnData_p->m_commonPfx = kaps_jr1_prefix_create(alloc_p, KAPS_JR1_LPM_KEY_MAX_WIDTH_1, self->m_nDepth,
                                                    self->m_pParentHandle->m_data);

    if (!flatLsnData_p->m_commonPfx)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    
    sortData = 0;
    if (self->m_pSettings->m_isPerLpuGran)
        sortData = 1;

    if (sortData)
    {
        kaps_jr1_lsn_mc_pvt_compute_pfx_sort_data(self, extraPfxArray, numExtraPfx, numPfxForEachLength,
                                              whereForEachLength);
    }

    flatLsnData_p->m_maxPfxLenInBits = 0;
    numPfxInLsn = 0;
    curLoc = 0;
    while (curLpuBrick)
    {
        if (curLpuBrick->m_numPfx == 0)
        {
            curLpuBrick = curLpuBrick->m_next_p;
            continue;
        }

        for (i = 0; i < curLpuBrick->m_maxCapacity; ++i)
        {
            kaps_jr1_pfx_bundle *bundle = curLpuBrick->m_pfxes[i];
            if (bundle)
            {
                if (sortData)
                {
                    length = bundle->m_nPfxSize;
                    curLoc = whereForEachLength[length];
                    whereForEachLength[length]++;
                }

                flatLsnData_p->m_pfxesInLsn[curLoc] = bundle;

                if (!sortData)
                {
                    ++curLoc;
                }

                ++numPfxInLsn;
                if (flatLsnData_p->m_maxPfxLenInBits < bundle->m_nPfxSize)
                {
                    flatLsnData_p->m_maxPfxLenInBits = bundle->m_nPfxSize;
                }
            }
        }

        curLpuBrick = curLpuBrick->m_next_p;
    }

    if (extraPfxArray)
    {
        for (i = 0; i < numExtraPfx; ++i)
        {
            newPfxBundle = extraPfxArray[i];

            if (sortData)
            {
                length = newPfxBundle->m_nPfxSize;
                curLoc = whereForEachLength[length];
                whereForEachLength[length]++;
            }

            flatLsnData_p->m_pfxesInLsn[curLoc] = newPfxBundle;

            if (!sortData)
            {
                ++curLoc;
            }

            ++numPfxInLsn;
            if (flatLsnData_p->m_maxPfxLenInBits < newPfxBundle->m_nPfxSize)
            {
                flatLsnData_p->m_maxPfxLenInBits = newPfxBundle->m_nPfxSize;
            }
        }
    }

    flatLsnData_p->m_numPfxInLsn = numPfxInLsn;

    kaps_jr1_lsn_mc_assign_flat_data_colors(self->m_pSettings, flatLsnData_p);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_add_empty_bricks(
    kaps_jr1_lsn_mc * self,
    uint32_t pfxGran,
    uint32_t pfxGranIx,
    uint32_t totalNumBricks,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc_settings *lsnSettings = self->m_pSettings;
    kaps_jr1_lpm_lpu_brick *curBrick = self->m_lpuList;
    kaps_jr1_lpm_lpu_brick *newLpuBrick, *lastBrick = NULL;
    kaps_jr1_nlm_allocator *alloc_p = self->m_pSettings->m_pAlloc;
    struct kaps_jr1_db *db = lsnSettings->m_fibTbl->m_db;
    struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;

    while (curBrick)
    {
        lastBrick = curBrick;
        curBrick = curBrick->m_next_p;
    }

    while (self->m_numLpuBricks < totalNumBricks)
    {
        
        newLpuBrick = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!newLpuBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        
        newLpuBrick->m_pfxes =
            kaps_jr1_nlm_allocator_calloc(alloc_p, lsnSettings->m_maxNumPfxInAnyBrick, sizeof(kaps_jr1_pfx_bundle *));
        if (!newLpuBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        self->m_numLpuBricks++;

        newLpuBrick->m_maxCapacity = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(lsnSettings, ad_db, 0, 0xf, pfxGran);
        newLpuBrick->m_granIx = pfxGranIx;
        newLpuBrick->m_gran = pfxGran;
        newLpuBrick->ad_db = ad_db;

        self->m_nLsnCapacity += newLpuBrick->m_maxCapacity;

        if (lastBrick)
        {
            lastBrick->m_next_p = newLpuBrick;
        }
        else
        {
            self->m_lpuList = newLpuBrick;
        }

        lastBrick = newLpuBrick;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_fill_up_brick_helper(
    kaps_jr1_lsn_mc * curLsn,
    kaps_jr1_lpm_lpu_brick * newBrick,
    kaps_jr1_lpm_lpu_brick ** lastBrick_pp,
    uint32_t * brick_iter_p,
    kaps_jr1_pfx_bundle ** tempPfxBrick,
    uint32_t startPosInTempBrick,
    uint32_t numPfxToFill,
    uint32_t gran,
    struct kaps_jr1_ad_db * ad_db)
{
    kaps_jr1_lsn_mc_settings *settings_p = curLsn->m_pSettings;
    uint32_t i, j;

    newBrick->m_numPfx = numPfxToFill;
    newBrick->m_gran = gran;
    newBrick->m_granIx = settings_p->m_lengthToGranIx[gran];
    newBrick->m_maxCapacity = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(settings_p, ad_db, 0, 0, gran);
    newBrick->ad_db = ad_db;

    j = startPosInTempBrick;
    for (i = 0; i < numPfxToFill; ++i)
    {
        newBrick->m_pfxes[i] = tempPfxBrick[j];
        ++j;
    }

    if (*lastBrick_pp)
    {
        (*lastBrick_pp)->m_next_p = newBrick;
    }
    else
    {
        curLsn->m_lpuList = newBrick;
    }

    *lastBrick_pp = newBrick;
    (*brick_iter_p)++;

    return NLMERR_OK;

}

NlmErrNum_t
kaps_jr1_lsn_mc_copy_from_temp_pfx_brick_to_current_brick(
    kaps_jr1_lsn_mc * curLsn,
    kaps_jr1_lpm_lpu_brick ** lastBrick_pp,
    uint32_t * brick_iter_p,
    kaps_jr1_pfx_bundle ** tempPfxBrick,
    uint32_t numPfxInTempPfxBrick,
    uint32_t gran,
    uint32_t areBricksJoined,
    uint32_t * lsnCapacity,
    struct kaps_jr1_ad_db * ad_db,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc_settings *settings = curLsn->m_pSettings;
    kaps_jr1_lpm_lpu_brick *newBrick;

    
    if (areBricksJoined)
    {
        uint32_t numPfxInFirstBrick, numPfxInSecondBrick;

        numPfxInFirstBrick = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(settings, ad_db, 0, 0, gran);
        if (numPfxInFirstBrick > numPfxInTempPfxBrick)
        {
            numPfxInFirstBrick = numPfxInTempPfxBrick;
        }

        numPfxInSecondBrick = numPfxInTempPfxBrick - numPfxInFirstBrick;

        
        newBrick = kaps_jr1_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!newBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_pfxes = kaps_jr1_nlm_allocator_calloc(settings->m_pAlloc, settings->m_maxNumPfxInAnyBrick,
                                                      sizeof(kaps_jr1_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        kaps_jr1_lsn_mc_fill_up_brick_helper(curLsn, newBrick, lastBrick_pp, brick_iter_p,
                                         tempPfxBrick, 0, numPfxInFirstBrick, gran, ad_db);

        *lsnCapacity += newBrick->m_maxCapacity;

        
        newBrick = kaps_jr1_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!newBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_pfxes = kaps_jr1_nlm_allocator_calloc(settings->m_pAlloc, settings->m_maxNumPfxInAnyBrick,
                                                      sizeof(kaps_jr1_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        kaps_jr1_lsn_mc_fill_up_brick_helper(curLsn, newBrick, lastBrick_pp, brick_iter_p,
                                         tempPfxBrick, numPfxInFirstBrick, numPfxInSecondBrick, gran, ad_db);

        *lsnCapacity += newBrick->m_maxCapacity;

    }
    else
    {
        newBrick = kaps_jr1_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!newBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newBrick->m_pfxes = kaps_jr1_nlm_allocator_calloc(settings->m_pAlloc, settings->m_maxNumPfxInAnyBrick,
                                                      sizeof(kaps_jr1_pfx_bundle *));
        if (!newBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        kaps_jr1_lsn_mc_fill_up_brick_helper(curLsn, newBrick, lastBrick_pp, brick_iter_p,
                                         tempPfxBrick, 0, numPfxInTempPfxBrick, gran, ad_db);

        *lsnCapacity += newBrick->m_maxCapacity;

    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_convert_flat_data_to_lsn_for_one_color(
    kaps_jr1_flat_lsn_data * flatLsnData_p,
    kaps_jr1_lsn_mc * curLsn,
    uint32_t curColor,
    uint32_t longestPfxGran,
    uint32_t * doesLsnFit_p,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc_settings *settings_p = curLsn->m_pSettings;
    uint32_t brickIter;
    kaps_jr1_pfx_bundle *pfxBundle;
    uint32_t buffer_loc;
    uint32_t lengthAfterLopoff, maxPossibleEntriesInBrick;
    uint16_t curPfxGran, curPfxGranIx;
    uint32_t prevGran;
    uint32_t numPfxInTempPfxBrick;
    kaps_jr1_pfx_bundle **tempPfxBrick = curLsn->m_pTrie->m_trie_global->m_tempPfxBrick;
    kaps_jr1_lpm_lpu_brick *lastBrick, *curBrick;
    NlmErrNum_t errNum = NLMCMOK;
    uint32_t lsnCapacity;
    struct kaps_jr1_ad_db *ad_db = NULL, *ad_db_for_cur_color = NULL;
    uint32_t numPfxProcessedForThisColor;

    lsnCapacity = 0;
    brickIter = 0;
    lastBrick = NULL;

    *doesLsnFit_p = 0;

    curBrick = curLsn->m_lpuList;

    while (curBrick)
    {
        lsnCapacity += curBrick->m_maxCapacity;
        brickIter++;
        lastBrick = curBrick;
        curBrick = curBrick->m_next_p;
    }

    buffer_loc = 0;
    curPfxGran = longestPfxGran;
    prevGran = 0;
    numPfxInTempPfxBrick = 0;
    numPfxProcessedForThisColor = 0;
    ad_db_for_cur_color = NULL;

    
    while (buffer_loc < flatLsnData_p->m_numPfxInLsn)
    {
        pfxBundle = flatLsnData_p->m_pfxesInLsn[buffer_loc];

        KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(settings_p->m_device, pfxBundle->m_backPtr->ad_handle, ad_db);

        if (flatLsnData_p->pfx_color[buffer_loc] == curColor)
        {
            ad_db_for_cur_color = ad_db;
        }

        
        if (flatLsnData_p->pfx_color[buffer_loc] != curColor)
        {
            
            buffer_loc++;

            
            if (buffer_loc == flatLsnData_p->m_numPfxInLsn)
            {

                if (numPfxProcessedForThisColor)
                {
                    errNum =
                        kaps_jr1_lsn_mc_copy_from_temp_pfx_brick_to_current_brick(curLsn, &lastBrick, &brickIter, tempPfxBrick,
                                                                                numPfxInTempPfxBrick, prevGran,
                                                                                settings_p->m_numJoinedSetsProcessed <
                                                                                settings_p->m_numJoinedSets, &lsnCapacity,
                                                                                ad_db_for_cur_color, o_reason);
                }
                break;
            }
            else
            {
                continue;
            }
        }


        lengthAfterLopoff = pfxBundle->m_nPfxSize - curLsn->m_nDepth;

        if (settings_p->m_isPerLpuGran)
        {
            curPfxGran = kaps_jr1_lsn_mc_compute_gran(settings_p, lengthAfterLopoff, &curPfxGranIx);
        }

        maxPossibleEntriesInBrick =
            kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(settings_p, ad_db_for_cur_color, 0, pfxBundle->m_backPtr->meta_priority, curPfxGran);

        if (settings_p->m_numJoinedSetsProcessed < settings_p->m_numJoinedSets)
        {
            maxPossibleEntriesInBrick *= settings_p->m_numJoinedBricksInOneSet;
        }

        
        if (numPfxInTempPfxBrick + 1 > maxPossibleEntriesInBrick)
        {

            errNum =
                kaps_jr1_lsn_mc_copy_from_temp_pfx_brick_to_current_brick(curLsn, &lastBrick, &brickIter, tempPfxBrick,
                                                                        numPfxInTempPfxBrick, prevGran,
                                                                        settings_p->m_numJoinedSetsProcessed <
                                                                        settings_p->m_numJoinedSets, &lsnCapacity,
                                                                        ad_db_for_cur_color, o_reason);

            if (errNum != NLMERR_OK)
                return errNum;

            numPfxInTempPfxBrick = 0;

            if (settings_p->m_numJoinedSetsProcessed < settings_p->m_numJoinedSets)
            {
                settings_p->m_numJoinedSetsProcessed++;
            }

            continue;
        }

        prevGran = curPfxGran;

        
        tempPfxBrick[numPfxInTempPfxBrick] = pfxBundle;

        buffer_loc++;
        numPfxInTempPfxBrick++;
        numPfxProcessedForThisColor++;

        if (buffer_loc == flatLsnData_p->m_numPfxInLsn)
        {
            errNum =
                kaps_jr1_lsn_mc_copy_from_temp_pfx_brick_to_current_brick(curLsn, &lastBrick, &brickIter, tempPfxBrick,
                                                                        numPfxInTempPfxBrick, prevGran,
                                                                        settings_p->m_numJoinedSetsProcessed <
                                                                        settings_p->m_numJoinedSets, &lsnCapacity,
                                                                        ad_db_for_cur_color, o_reason);
            break;
        }

    }


    
    if (buffer_loc == flatLsnData_p->m_numPfxInLsn && brickIter <= settings_p->m_maxLpuPerLsn)
    {
        *doesLsnFit_p = 1;
    }

    curLsn->m_nNumPrefixes += numPfxProcessedForThisColor;
    curLsn->m_nLsnCapacity = lsnCapacity;

    curLsn->m_numLpuBricks = brickIter;

    return errNum;

}


NlmErrNum_t
kaps_jr1_lsn_mc_convert_flat_data_to_lsn(
    kaps_jr1_flat_lsn_data * flatLsnData_p,
    kaps_jr1_lsn_mc * self,
    uint32_t * doesLsnFit_p,
    NlmReasonCode * o_reason)
{
    uint16_t  longestPfxGran = 0, longestPfxGranIx = 0;
    uint32_t lengthAfterLopoff;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_lsn_mc_settings *lsnSettings = self->m_pSettings;
    uint16_t color = 0;
    kaps_jr1_lpm_lpu_brick *newLpuBrick;

    *doesLsnFit_p = 0;

    self->m_lpuList = NULL;
    self->m_nNumPrefixes = 0;


    
    if (flatLsnData_p->m_numPfxInLsn == 0)
    {
        newLpuBrick = kaps_jr1_nlm_allocator_calloc(lsnSettings->m_pAlloc, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!newLpuBrick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newLpuBrick->m_pfxes =
            kaps_jr1_nlm_allocator_calloc(lsnSettings->m_pAlloc, lsnSettings->m_maxNumPfxInAnyBrick, sizeof(kaps_jr1_pfx_bundle *));
        if (!newLpuBrick->m_pfxes)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        newLpuBrick->m_gran = lsnSettings->m_lengthToGran[0];
        newLpuBrick->m_granIx = 0;

        
        newLpuBrick->ad_db = (struct kaps_jr1_ad_db*) self->m_pTrie->m_tbl_ptr->m_db->common_info->ad_info.ad;

        newLpuBrick->m_maxCapacity = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(lsnSettings, newLpuBrick->ad_db,
                                                0, 0, newLpuBrick->m_gran);

        self->m_lpuList = newLpuBrick;

        *doesLsnFit_p = 1;

        self->m_nNumPrefixes = 0;
        self->m_nLsnCapacity = newLpuBrick->m_maxCapacity;
        self->m_numLpuBricks = 1;

        return errNum;

    }


    lengthAfterLopoff = flatLsnData_p->m_maxPfxLenInBits - flatLsnData_p->m_commonPfx->m_inuse;
    longestPfxGran = kaps_jr1_lsn_mc_compute_gran(lsnSettings, lengthAfterLopoff, &longestPfxGranIx);


    lsnSettings->m_numJoinedSetsProcessed = 0;
    for (color = 0; color < flatLsnData_p->num_colors; ++color)
    {
        errNum =
            kaps_jr1_lsn_mc_convert_flat_data_to_lsn_for_one_color(flatLsnData_p,
                                            self, color, longestPfxGran, doesLsnFit_p, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;

        if (*doesLsnFit_p == 0)
            return NLMERR_OK;
    }


    if (lsnSettings->m_isFullWidthLsn)
    {
        errNum =
            kaps_jr1_lsn_mc_add_empty_bricks(self, longestPfxGran, longestPfxGranIx, lsnSettings->m_maxLpuPerLsn, o_reason);
    }

    if (lsnSettings->m_isMultiBrickAlloc)
    {
        uint32_t totalNumBricks =
            ((self->m_numLpuBricks + lsnSettings->m_numMultiBricks - 1) / lsnSettings->m_numMultiBricks);
        totalNumBricks = totalNumBricks * lsnSettings->m_numMultiBricks;

        errNum = kaps_jr1_lsn_mc_add_empty_bricks(self, longestPfxGran, longestPfxGranIx, totalNumBricks, o_reason);
    }

    return errNum;
}



uint32_t
kaps_jr1_lsn_mc_find_if_next_brick_is_joined(
    kaps_jr1_lsn_mc * curLsn,
    uint32_t brick_iter)
{
    uint32_t isNextBrickJoined = 0;
    uint32_t lpu_nr_1 = 0, row_nr_1 = 0;
    uint32_t lpu_nr_2 = 0, row_nr_2 = 0;
    kaps_jr1_lsn_mc_settings *settings_p = curLsn->m_pSettings;

    kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), curLsn->m_mlpMemInfo, brick_iter, &lpu_nr_1,
                                               &row_nr_1);

    if (brick_iter + 1 < curLsn->m_numLpuBricks)
    {
        kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(*(settings_p->m_pMlpMemMgr), curLsn->m_mlpMemInfo, brick_iter + 1,
                                                   &lpu_nr_2, &row_nr_2);

        if ((lpu_nr_1 % 2 == 0) && (lpu_nr_1 + 1 == lpu_nr_2) && (row_nr_1 == row_nr_2))
        {
            isNextBrickJoined = 1;
        }
    }

    return isNextBrickJoined;
}

uint32_t
kaps_jr1_lsn_mc_find_longest_prefix_length(
    kaps_jr1_lsn_mc * curLsn)
{
    kaps_jr1_lpm_lpu_brick *curBrick;
    uint32_t maxPfxLength;
    kaps_jr1_pfx_bundle *pfxBundle;
    uint32_t i;

    maxPfxLength = 0;
    curBrick = curLsn->m_lpuList;
    while (curBrick)
    {
        for (i = 0; i < curBrick->m_maxCapacity; ++i)
        {
            pfxBundle = curBrick->m_pfxes[i];

            if (pfxBundle && pfxBundle->m_nPfxSize > maxPfxLength)
                maxPfxLength = pfxBundle->m_nPfxSize;
        }

        curBrick = curBrick->m_next_p;
    }

    return maxPfxLength;
}

NlmErrNum_t
kaps_jr1_lsn_mc_find_prefixes_in_path(
    kaps_jr1_lsn_mc * curLsn,
    uint8_t * pathDataToCheck,
    uint32_t pathLengthToCheck,
    uint32_t excludeExactPathLengthMatch,
    kaps_jr1_pfx_bundle * pfxArray[],
    uint32_t pfxLocationsToMove_p[],
    uint32_t * numPfxInPath,
    uint32_t *isExactMatchLenPfxPresent)
{
    kaps_jr1_lpm_lpu_brick *curBrick;
    kaps_jr1_pfx_bundle *pfxBundle;
    uint32_t i, indexInLsn;

    *numPfxInPath = 0;
    indexInLsn = 0;
    *isExactMatchLenPfxPresent = 0;
    curBrick = curLsn->m_lpuList;
    while (curBrick)
    {
        for (i = 0; i < curBrick->m_maxCapacity; ++i)
        {
            pfxBundle = curBrick->m_pfxes[i];

            if (pfxBundle)
            {

                if (kaps_jr1_prefix_pvt_is_more_specific_equal(pfxBundle->m_data, pfxBundle->m_nPfxSize,
                                                           pathDataToCheck, pathLengthToCheck))
                {

                    if (pfxBundle->m_nPfxSize == pathLengthToCheck)
                        *isExactMatchLenPfxPresent = 1;

                    if (excludeExactPathLengthMatch && pfxBundle->m_nPfxSize == pathLengthToCheck)
                    {
                        
                    }
                    else
                    {
                        pfxArray[*numPfxInPath] = pfxBundle;
                        pfxLocationsToMove_p[*numPfxInPath] = indexInLsn;
                        (*numPfxInPath)++;
                    }

                }
            }

            ++indexInLsn;
        }

        curBrick = curBrick->m_next_p;
    }

    return NLMERR_OK;
}

void
kaps_jr1_lsn_mc_append_to_brick_list(
    kaps_jr1_lsn_mc * curLsn,
    kaps_jr1_lpm_lpu_brick ** lastBrick_pp,
    kaps_jr1_lpm_lpu_brick * newBrick)
{
    if (*lastBrick_pp)
    {
        (*lastBrick_pp)->m_next_p = newBrick;
    }
    else
    {
        curLsn->m_lpuList = newBrick;
    }

    *lastBrick_pp = newBrick;
}

NlmErrNum_t
kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(
    kaps_jr1_lsn_mc * curLsn,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lpm_lpu_brick *initialArrayOfBricks[KAPS_JR1_HW_MAX_LPUS_PER_DB];
    kaps_jr1_lpm_lpu_brick *finalArrayOfBricks[KAPS_JR1_HW_MAX_LPUS_PER_DB];
    kaps_jr1_lpm_lpu_brick *curBrick, *nextBrick, *prevBrick;
    uint32_t i, brickIter, found;

    kaps_jr1_memset(initialArrayOfBricks, 0, KAPS_JR1_HW_MAX_LPUS_PER_DB * sizeof(kaps_jr1_lpm_lpu_brick *));
    kaps_jr1_memset(finalArrayOfBricks, 0, KAPS_JR1_HW_MAX_LPUS_PER_DB * sizeof(kaps_jr1_lpm_lpu_brick *));

    curBrick = curLsn->m_lpuList;
    i = 0;

    while (curBrick)
    {
        nextBrick = curBrick->m_next_p;
        initialArrayOfBricks[i] = curBrick;
        curBrick->m_next_p = NULL;
        curBrick = nextBrick;
        ++i;
    }

    kaps_jr1_assert(i == curLsn->m_numLpuBricks, "Incorrect number of bricks in the LSN \n");

    
    brickIter = 0;
    while (brickIter < curLsn->m_numLpuBricks)
    {
        if (kaps_jr1_lsn_mc_find_if_next_brick_is_joined(curLsn, brickIter))
        {

            found = 0;

            
            for (i = 0; i < KAPS_JR1_HW_MAX_LPUS_PER_DB - 1; i++)
            {
                
                curBrick = initialArrayOfBricks[i];
                nextBrick = initialArrayOfBricks[i + 1];
                if (curBrick && nextBrick && curBrick->m_gran == nextBrick->m_gran)
                {
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                kaps_jr1_assert(0, "Should have found joined brick pair, but couldn't find\n");
                *o_reason = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }

            finalArrayOfBricks[brickIter] = curBrick;
            finalArrayOfBricks[brickIter + 1] = nextBrick;

            initialArrayOfBricks[i] = NULL;
            initialArrayOfBricks[i + 1] = NULL;

            brickIter += 2;

        }
        else
        {
            brickIter++;
        }

    }

    
    brickIter = 0;
    while (brickIter < curLsn->m_numLpuBricks)
    {

        if (kaps_jr1_lsn_mc_find_if_next_brick_is_joined(curLsn, brickIter))
        {
            brickIter += 2;
        }
        else
        {

            found = 0;
            for (i = 0; i < KAPS_JR1_HW_MAX_LPUS_PER_DB; i++)
            {
                curBrick = initialArrayOfBricks[i];

                if (curBrick)
                {
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                kaps_jr1_assert(0, "Should have found brick, but couldn't find\n");
                *o_reason = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }

            finalArrayOfBricks[brickIter] = curBrick;

            initialArrayOfBricks[i] = NULL;

            brickIter++;
        }
    }

    
    brickIter = 0;
    prevBrick = NULL;
    while (brickIter < curLsn->m_numLpuBricks)
    {
        if (brickIter == 0)
        {
            curLsn->m_lpuList = finalArrayOfBricks[brickIter];
        }
        else
        {
            prevBrick->m_next_p = finalArrayOfBricks[brickIter];
        }

        prevBrick = finalArrayOfBricks[brickIter];
        brickIter++;
    }

    return NLMERR_OK;
}

uint8_t *
kaps_jr1_lsn_mc_calc_common_bits(
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_flat_lsn_data * flatLsnData_p,
    uint32_t * retlen)
{
    uint32_t i, j, startbyte;
    uint32_t maxmatch = 0, maxbit = 0;
    uint8_t *tmpData = 0;

    
    startbyte = flatLsnData_p->m_commonPfx->m_inuse / 8;

    

    maxbit = KAPS_JR1_LPM_KEY_MAX_WIDTH_1;
    maxmatch = KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(maxbit);

    for (i = 0; i < flatLsnData_p->m_numPfxInLsn; i++)
    {
        kaps_jr1_pfx_bundle *bundle = flatLsnData_p->m_pfxesInLsn[i];

        if (bundle)
        {       
            uint8_t *data = KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(bundle);

            if (maxbit > bundle->m_nPfxSize)
            {
                maxbit = bundle->m_nPfxSize;
                maxmatch = KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(maxbit);
            }

            if (!tmpData)
            {
                tmpData = data;
                continue;
            }

            for (j = startbyte; j < maxmatch; j++)
            {
                uint32_t cmp = tmpData[j] ^ data[j];
                if (cmp)
                {
                    uint32_t tmp = (j << 3) + settings->m_cmpLookupTable[cmp];

                    if (maxbit > tmp)
                    {
                        maxbit = tmp;
                        maxmatch = j + 1;
                    }
                    break;
                }
            }
        }
    }

    *retlen = maxbit;
    return tmpData;
}

static NlmErrNum_t
kaps_jr1_lsn_mc_pct_find_give_out_prefix(
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_flat_lsn_data * flatLsnData_p,
    kaps_jr1_prefix * giveout_prefix,
    uint8_t give_or_keep[],
    uint16_t tbl_width)
{
    uint32_t norig, ngive_upper_thresh, ngive_lower_thresh, found_giveout, ngive = 0, nkeep = 0, giveix, keepix;
    uint32_t flipped = 0;
    uint8_t zero_byte = 0;
    uint32_t offset_8, commonLength;
    uint32_t i = 0;
    uint8_t is_root_being_split;
    uint32_t longest_give_pfx_len;
    uint32_t caution_height = settings->m_bottom_up_caution_height;

    is_root_being_split = 0;
    if (giveout_prefix->m_inuse == 0)
    {
        is_root_being_split = 1;
    }

    norig = flatLsnData_p->m_numPfxInLsn;
    ngive_upper_thresh = (2 * norig) / 3;
    ngive_lower_thresh = norig / 3;

    if (ngive_upper_thresh == 0)
        ngive_upper_thresh = 1;

    if (ngive_lower_thresh == 0)
        ngive_lower_thresh = 1;

    offset_8 = giveout_prefix->m_inuse / 8;
    commonLength = offset_8 * 8;

    found_giveout = 0;

    while (!found_giveout)
    {
        if (flipped == 0)
        {
            kaps_jr1_prefix_append(giveout_prefix, 1, &zero_byte);
        }

        flipped = 0;

        giveix = 0;
        keepix = 0;

        longest_give_pfx_len = 0;

        for (i = 0; i < flatLsnData_p->m_numPfxInLsn; i++)
        {
            
            kaps_jr1_pfx_bundle *bundle = flatLsnData_p->m_pfxesInLsn[i];
            if (KAPS_JR1_PFX_BUNDLE_GET_PFX_SIZE(bundle) < giveout_prefix->m_inuse)
            {
                
                give_or_keep[i] = KAPS_JR1_KEEP;
                keepix++;
                continue;
            }

            if (kaps_jr1_prefix_pvt_is_more_specific_equal(&bundle->m_data[offset_8],
                                                       bundle->m_nPfxSize - commonLength,
                                                       &giveout_prefix->m_data[offset_8],
                                                       giveout_prefix->m_inuse - commonLength))
            {
                
                give_or_keep[i] = KAPS_JR1_GIVE;
                giveix++;

                if (bundle->m_nPfxSize >= longest_give_pfx_len)
                    longest_give_pfx_len = bundle->m_nPfxSize;

            }
            else
            {
                give_or_keep[i] = KAPS_JR1_KEEP;
                keepix++;
            }
        }

        ngive = giveix;
        nkeep = keepix;

        if ((ngive <= ngive_upper_thresh) && (ngive >= ngive_lower_thresh))
        {
            found_giveout = 1;
        }
        else if ((longest_give_pfx_len - giveout_prefix->m_inuse) == caution_height && ngive >= ngive_upper_thresh
                 && nkeep)
        {
            
            found_giveout = 1;
        }
        else if (is_root_being_split && giveout_prefix->m_inuse >= 8 && ngive >= 1)
        {
            
            found_giveout = 1;
        }
        else if (ngive < ngive_lower_thresh)
        {
            
            KAPS_JR1_PREFIX_SET_BIT(giveout_prefix, giveout_prefix->m_inuse - 1, 1);
            flipped = 1;
        }

        
        if (giveout_prefix->m_inuse > flatLsnData_p->m_maxPfxLenInBits)
        {
            found_giveout = 1;
            break;
        }
    }

    return NLMERR_OK;
}

void
kaps_jr1_lsn_mc_flat_data_update_color(
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_flat_lsn_data * flatData_p)
{
    uint32_t i, j;
    uint16_t color = 0;
    uint16_t old_color;
    const uint16_t flag_bit = (1 << 15);        
    const uint16_t flag_mask = (1 << 15) - 1;
    struct kaps_jr1_device *device = settings->m_fibTbl->m_fibTblMgr_p->m_devMgr_p;

    for (i = 0; i < flatData_p->m_numPfxInLsn; i++)
    {
        old_color = flatData_p->pfx_color[i];
        if (old_color & flag_bit)
        {
            flatData_p->pfx_color[i] &= flag_mask;
            continue;
        }
        flatData_p->pfx_color[i] = color;
        for (j = i + 1; j < flatData_p->m_numPfxInLsn; j++)
        {
            if (flatData_p->pfx_color[j] == old_color)
            {
                flatData_p->pfx_color[j] = color | flag_bit;
                if (flatData_p->m_pfxesInLsn[j]->m_backPtr->ad_handle)
                {
                    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, flatData_p->m_pfxesInLsn[j]->m_backPtr->ad_handle,
                                                      flatData_p->ad_colors[color]);
                }
            }
        }
        color++;
    }

    flatData_p->num_colors = color;
}

NlmErrNum_t
kaps_jr1_lsn_mc_undo_lsn_push(
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_flat_lsn_data * giveData_p,
    kaps_jr1_flat_lsn_data * keepData_p,
    struct kaps_jr1_lsn_mc *newLsn)
{
    kaps_jr1_pfx_bundle *iitLmpsofar;

    if (giveData_p->m_commonPfx)
    {
        kaps_jr1_prefix_destroy(giveData_p->m_commonPfx, settings->m_pAlloc);
        giveData_p->m_commonPfx = NULL;

    }

    if (keepData_p->m_commonPfx)
    {
        kaps_jr1_prefix_destroy(keepData_p->m_commonPfx, settings->m_pAlloc);
        keepData_p->m_commonPfx = NULL;
    }

    
    iitLmpsofar = giveData_p->m_iitLmpsofarPfx;
    kaps_jr1_memset(giveData_p, 0, sizeof(kaps_jr1_flat_lsn_data));
    giveData_p->m_iitLmpsofarPfx = iitLmpsofar;

    if (newLsn)
    {
        kaps_jr1_lsn_mc_destroy(newLsn);
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_try_in_place_lsn_push(
    kaps_jr1_lsn_mc * originalLsn,
    kaps_jr1_lsn_mc_settings * settings,
    uint8_t * commonBits,
    uint32_t commonBitsLen,
    kaps_jr1_flat_lsn_data * curGiveoutData_p,
    kaps_jr1_flat_lsn_data * giveData_p,
    kaps_jr1_flat_lsn_data * keepData_p,
    kaps_jr1_pfx_bundle * pfxBundleToInsert,
    uint32_t * isInPlacePushSuccessful,
    NlmReasonCode * o_reason)
{
    uint32_t originalLsnDepth = curGiveoutData_p->m_commonPfx->m_inuse;
    uint32_t maxPfxLen = curGiveoutData_p->m_maxPfxLenInBits;
    uint32_t i, j;
    struct kaps_jr1_lsn_mc *newLsn = NULL;
    uint32_t isLsnFitPossible;
    NlmErrNum_t errNum;
    struct kaps_jr1_uda_mgr *pMlpMemMgr = settings->m_pMlpMemMgr[0];
    struct uda_mem_chunk *uda_chunk = NULL;
    kaps_jr1_status status;
    uint32_t chosenGiveSlot = 0;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = settings->m_device;
    uint32_t caution_height = settings->m_bottom_up_caution_height;

    (void) chosenGiveSlot;
    (void) ad_db;
    (void) device;

    
    if (commonBitsLen <= originalLsnDepth)
    {
        *isInPlacePushSuccessful = 0;
        return NLMERR_OK;
    }

    if (settings->m_strictlyStoreLmpsofarInAds)
    {
        kaps_jr1_trie_node *originalLsnTrieNode;

        void *tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(originalLsn->m_pParentHandle);

        kaps_jr1_memcpy(&originalLsnTrieNode, tmp_ptr, sizeof(kaps_jr1_trie_node *));

        
        if (originalLsnTrieNode->m_iitLmpsofarPfx_p &&
            originalLsnTrieNode->m_iitLmpsofarPfx_p->m_nPfxSize == originalLsnTrieNode->m_depth)
        {
            *isInPlacePushSuccessful = 0;
            return NLMERR_OK;
        }
    }

    
    if (maxPfxLen >= caution_height)
    {
        
        if (commonBitsLen > (uint32_t) (maxPfxLen - caution_height)
            && (uint32_t) (maxPfxLen - caution_height) > originalLsnDepth)
        {
            commonBitsLen = maxPfxLen - caution_height;
        }
    }


    
    if (commonBitsLen <= originalLsnDepth)
    {
        *isInPlacePushSuccessful = 0;
        return NLMERR_OK;
    }

    giveData_p->m_commonPfx =
        kaps_jr1_prefix_create(settings->m_pAlloc, KAPS_JR1_LPM_KEY_MAX_WIDTH_1, commonBitsLen, commonBits);

    j = 0;
    for (i = 0; i < curGiveoutData_p->m_numPfxInLsn; ++i)
    {
        kaps_jr1_pfx_bundle *bundle = curGiveoutData_p->m_pfxesInLsn[i];

        if (settings->m_strictlyStoreLmpsofarInAds
                && giveData_p->m_commonPfx->m_inuse == bundle->m_nPfxSize)
        {
            
            giveData_p->m_iitLmpsofarPfx = bundle;
            chosenGiveSlot = i;
        }
        else
        {
            giveData_p->m_pfxesInLsn[j] = bundle;
            giveData_p->pfx_color[j] = curGiveoutData_p->pfx_color[i];
            if (giveData_p->m_maxPfxLenInBits < bundle->m_nPfxSize)
            {
                giveData_p->m_maxPfxLenInBits = bundle->m_nPfxSize;
            }
            ++j;
        }
    }

    giveData_p->m_numPfxInLsn = j;
    giveData_p->num_colors = curGiveoutData_p->num_colors;

    if (giveData_p->m_iitLmpsofarPfx && !giveData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx)
    {
        giveData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx = 1;
    }


    newLsn = kaps_jr1_lsn_mc_create(settings, settings->m_fibTbl->m_trie, giveData_p->m_commonPfx->m_inuse);

    if (!newLsn)
    {
        kaps_jr1_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    isLsnFitPossible = 0;
    errNum = kaps_jr1_lsn_mc_convert_flat_data_to_lsn(giveData_p, newLsn, &isLsnFitPossible, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_jr1_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        return errNum;
    }

    if (!isLsnFitPossible)
    {
        kaps_jr1_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        return NLMERR_OK;
    }

    keepData_p->m_commonPfx = kaps_jr1_prefix_create(settings->m_pAlloc, KAPS_JR1_LPM_KEY_MAX_WIDTH_1,
                                                 curGiveoutData_p->m_commonPfx->m_inuse,
                                                 curGiveoutData_p->m_commonPfx->m_data);

    if (!keepData_p->m_commonPfx)
    {
        kaps_jr1_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    status = kaps_jr1_uda_mgr_alloc(pMlpMemMgr, newLsn->m_numLpuBricks, newLsn->m_pTbl->m_fibTblMgr_p->m_devMgr_p, newLsn,
                                &uda_chunk, newLsn->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db);

    if (status != KAPS_JR1_OK || !uda_chunk)
    {
        kaps_jr1_lsn_mc_undo_lsn_push(settings, giveData_p, keepData_p, newLsn);
        *isInPlacePushSuccessful = 0;
        return NLMERR_OK;
    }

    kaps_jr1_uda_mgr_free(pMlpMemMgr, uda_chunk);

    
    if (newLsn)
    {
        kaps_jr1_lsn_mc_destroy(newLsn);
    }

    kaps_jr1_lsn_mc_flat_data_update_color(settings, giveData_p);

    *isInPlacePushSuccessful = 1;
    settings->m_fibTbl->m_fibStats.numLsnPush++;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_giveout(
    kaps_jr1_lsn_mc * originalLsn,
    kaps_jr1_lsn_mc_settings * settings,
    kaps_jr1_fib_tbl *fibTbl,
    kaps_jr1_flat_lsn_data * curGiveoutData_p,
    kaps_jr1_flat_lsn_data * keepData_p,
    kaps_jr1_flat_lsn_data * giveData_p,
    kaps_jr1_pfx_bundle * pfxBundleToInsert,
    kaps_jr1_nlm_allocator * alloc_p,
    uint32_t forceOnlyLsnPush,
    NlmReasonCode * o_reason)
{
    uint8_t *commonBits = NULL;
    uint32_t commonBitsLen;
    uint16_t lsnDepth = curGiveoutData_p->m_commonPfx->m_inuse;
    kaps_jr1_prefix *giveout_prefix = NULL;
    uint16_t giveix, keepix;
    uint32_t byteOffset, commonLength, i;
    uint16_t maxDepth = curGiveoutData_p->m_maxPfxLenInBits;
    uint32_t caution_height = settings->m_bottom_up_caution_height;
    uint32_t isCurPfxBundleLmpsofarOfGiveLsn;
    uint32_t isInPlacePushSuccessful;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_jr1_db *db = fibTbl->m_db;
    struct kaps_jr1_ad_db *ad_db = NULL;
    uint32_t areGiveAndKeepLmpsofarIdentical = 0;

    (void) db;
    (void) ad_db;
    (void) areGiveAndKeepLmpsofarIdentical;


    
    commonBits = kaps_jr1_lsn_mc_calc_common_bits(settings, curGiveoutData_p, &commonBitsLen);

    kaps_jr1_memset(settings->common_bits_array, 0, KAPS_JR1_LPM_KEY_MAX_WIDTH_8);
    kaps_jr1_memcpy(settings->common_bits_array, commonBits, (commonBitsLen + 7) / 8);

    if (settings->m_strictlyStoreLmpsofarInAds)
    {
        

        if (curGiveoutData_p->m_iitLmpsofarPfx &&
            curGiveoutData_p->m_iitLmpsofarPfx->m_nPfxSize == curGiveoutData_p->m_commonPfx->m_inuse)
        {
            commonBitsLen = curGiveoutData_p->m_commonPfx->m_inuse;
        }
    }



    
    isInPlacePushSuccessful = 0;
    if (settings->m_tryLsnPush || forceOnlyLsnPush)
    {
        errNum =
            kaps_jr1_lsn_mc_try_in_place_lsn_push(originalLsn, settings, settings->common_bits_array, commonBitsLen, curGiveoutData_p, giveData_p,
                                              keepData_p, pfxBundleToInsert, &isInPlacePushSuccessful, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;

        if (isInPlacePushSuccessful)
        {
            return NLMERR_OK;
        }

        if (forceOnlyLsnPush && !isInPlacePushSuccessful)
        {
            return NLMERR_FAIL;
        }
    }

    
    if (curGiveoutData_p->num_colors == 1)
    {   
        if (curGiveoutData_p->m_numPfxInLsn < 3)
        {
            return NLMERR_FAIL;
        }
    }

    
    if (curGiveoutData_p->num_colors <= settings->m_maxLpuPerLsn)
    {
        if (maxDepth >= caution_height)
        {
            
            if (commonBitsLen > (uint32_t) (maxDepth - caution_height)
                && (uint32_t) (maxDepth - caution_height) > lsnDepth)
            {
                commonBitsLen = maxDepth - caution_height;
            }
        }
    }

    giveout_prefix = kaps_jr1_prefix_create(alloc_p, KAPS_JR1_LPM_KEY_MAX_WIDTH_1, commonBitsLen, settings->common_bits_array);

    kaps_jr1_lsn_mc_pct_find_give_out_prefix(settings, curGiveoutData_p, giveout_prefix, settings->m_give_or_keep,
                                         settings->m_fibTbl->m_width);


    keepData_p->m_commonPfx = kaps_jr1_prefix_create(alloc_p, KAPS_JR1_LPM_KEY_MAX_WIDTH_1, commonBitsLen, settings->common_bits_array);


    giveData_p->m_commonPfx = giveout_prefix;

    byteOffset = commonBitsLen / 8;
    commonLength = byteOffset * 8;

    keepix = 0;
    giveix = 0;
    for (i = 0; i < curGiveoutData_p->m_numPfxInLsn; ++i)
    {
        kaps_jr1_pfx_bundle *bundle = curGiveoutData_p->m_pfxesInLsn[i];

        if (settings->m_give_or_keep[i] == KAPS_JR1_KEEP)
        {
            
            int32_t isInGiveoutPath = kaps_jr1_prefix_pvt_is_more_specific_equal(&giveout_prefix->m_data[byteOffset],
                                                                             giveout_prefix->m_inuse - commonLength,
                                                                             &bundle->m_data[byteOffset],
                                                                             bundle->m_nPfxSize - commonLength);
            
            isCurPfxBundleLmpsofarOfGiveLsn = 0;

            if (isInGiveoutPath)
            {

                if (!giveData_p->m_iitLmpsofarPfx)
                {
                    isCurPfxBundleLmpsofarOfGiveLsn = 1;
                }
                else
                {
                    if (bundle->m_nPfxSize > giveData_p->m_iitLmpsofarPfx->m_nPfxSize)
                    {
                        isCurPfxBundleLmpsofarOfGiveLsn = 1;
                    }
                    else if (bundle->m_nPfxSize == giveData_p->m_iitLmpsofarPfx->m_nPfxSize && !bundle->m_isPfxCopy)
                    {
                        isCurPfxBundleLmpsofarOfGiveLsn = 1;
                    }
                }

                if (isCurPfxBundleLmpsofarOfGiveLsn)
                {
                    giveData_p->m_iitLmpsofarPfx = bundle;
                }

            }

            if (settings->m_strictlyStoreLmpsofarInAds
                && keepData_p->m_commonPfx->m_inuse == bundle->m_nPfxSize)
            {
                
                keepData_p->m_iitLmpsofarPfx = bundle;
            }
            else
            {
                keepData_p->m_pfxesInLsn[keepix] = bundle;
                keepData_p->pfx_color[keepix] = curGiveoutData_p->pfx_color[i];
                if (keepData_p->m_maxPfxLenInBits < bundle->m_nPfxSize)
                    keepData_p->m_maxPfxLenInBits = bundle->m_nPfxSize;

                ++keepix;
            }



        }
        else if (settings->m_give_or_keep[i] == KAPS_JR1_GIVE)
        {
            if (settings->m_strictlyStoreLmpsofarInAds
                && giveData_p->m_commonPfx->m_inuse == bundle->m_nPfxSize)
            {
                
                giveData_p->m_iitLmpsofarPfx = bundle;

            }
            else
            {
                giveData_p->m_pfxesInLsn[giveix] = bundle;
                giveData_p->pfx_color[giveix] = curGiveoutData_p->pfx_color[i];
                if (giveData_p->m_maxPfxLenInBits < bundle->m_nPfxSize)
                {
                    giveData_p->m_maxPfxLenInBits = bundle->m_nPfxSize;
                }
                ++giveix;
            }

        }
        else
        {
            kaps_jr1_assert(0, "Incorrect value in give_or_keep array\n");
        }
    }

    areGiveAndKeepLmpsofarIdentical = 0;
    if (giveData_p->m_iitLmpsofarPfx == keepData_p->m_iitLmpsofarPfx)
        areGiveAndKeepLmpsofarIdentical = 1;

    if (giveData_p->m_iitLmpsofarPfx && !giveData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx)
    {
        giveData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx = 1;
    }



    if (keepData_p->m_iitLmpsofarPfx && !keepData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx)
    {
        keepData_p->m_iitLmpsofarPfx->m_isLmpsofarPfx = 1;
    }


    giveData_p->m_numPfxInLsn = giveix;
    keepData_p->m_numPfxInLsn = keepix;

    kaps_jr1_lsn_mc_flat_data_update_color(settings, giveData_p);
    kaps_jr1_lsn_mc_flat_data_update_color(settings, keepData_p);

    return NLMERR_OK;
}

int32_t
kaps_jr1_lsn_mc_get_num_prefixes(
    void *lsn,
    struct kaps_jr1_ix_chunk * cur_chunk)
{
    kaps_jr1_lsn_mc *self = (kaps_jr1_lsn_mc *) lsn;
    kaps_jr1_lpm_lpu_brick *brickWithReservedSlot = NULL;
    (void) brickWithReservedSlot;

    if (self->m_pSettings->m_isPerLpuGran)
    {
        kaps_jr1_lpm_lpu_brick *ixBrick = NULL;

        kaps_jr1_assert(self->m_ixInfo == NULL, "Invalid Ix info in the LSN ");

        ixBrick = self->m_lpuList;
        while (ixBrick)
        {
            if (ixBrick->m_hasReservedSlot)
                brickWithReservedSlot = ixBrick;

            if (ixBrick->m_ixInfo == cur_chunk)
                break;
            ixBrick = ixBrick->m_next_p;
        }


        kaps_jr1_sassert(ixBrick);
        return ixBrick->m_numPfx;
    }
    else
    {
        return self->m_nNumPrefixes;
    }
}

NlmErrNum_t
kaps_jr1_lsn_mc_verify(
    kaps_jr1_lsn_mc * self)
{
    kaps_jr1_lpm_lpu_brick *curLpuBrick_p;
    uint16_t numPfxInLsn, numPfxInLpu;
    uint32_t i = 0;
    kaps_jr1_pfx_bundle *pfx;
    struct kaps_jr1_device *device = self->m_pTbl->m_db->device;
    struct kaps_jr1_ad_db *ad_db = NULL;
    uint32_t brickIter;

    curLpuBrick_p = self->m_lpuList;
    numPfxInLsn = 0;
    brickIter = 0;
    while (curLpuBrick_p)
    {
        
        if (curLpuBrick_p->m_numPfx && !curLpuBrick_p->ad_db)
        {
            kaps_jr1_assert(0, "AD DB is not set in the Brick\n");
            return NLMERR_FAIL;
        }

        
        numPfxInLpu = 0;
        for (i = 0; i < curLpuBrick_p->m_maxCapacity; ++i)
        {
            pfx = curLpuBrick_p->m_pfxes[i];
            if (pfx)
            {
                ++numPfxInLpu;

                KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, pfx->m_backPtr->ad_handle, ad_db);

                
                if (ad_db != curLpuBrick_p->ad_db)
                {
                    kaps_jr1_assert(0, " AD DB of Brick and Prefix don't match \n");
                    return NLMERR_FAIL;
                }
            }
        }

        
        if (numPfxInLpu != curLpuBrick_p->m_numPfx)
        {
            kaps_jr1_assert(0, "Incorrect number of pfxes per LPU \n");
            return NLMERR_FAIL;
        }

        numPfxInLsn += numPfxInLpu;
        brickIter++;
        curLpuBrick_p = curLpuBrick_p->m_next_p;
    }

    if (numPfxInLsn != self->m_nNumPrefixes)
    {
        kaps_jr1_assert(0, "Incorrect number of pfxes per LSN \n");
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

void
kaps_jr1_lsn_mc_print(
    kaps_jr1_lsn_mc * self,
    FILE * fp)
{
    (void) self;
    (void) fp;
}

void
kaps_jr1_lsn_mc_printPrefix(
    kaps_jr1_lsn_mc * self,
    FILE * fp)
{
    (void) self;
    (void) fp;
}


NlmErrNum_t
kaps_jr1_lsn_mc_wb_create_lsn(
    kaps_jr1_lsn_mc * lsn_p,
    struct kaps_jr1_wb_lsn_info *lsn_info,
    NlmReasonCode * o_reason)
{
    kaps_jr1_nlm_allocator *alloc_p = NULL;
    kaps_jr1_lpm_lpu_brick *new_brick = NULL, *head = NULL, *tail = NULL;
    kaps_jr1_lsn_mc_settings *lsn_settings = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    uint8_t alloc_udm[KAPS_JR1_MAX_NUM_CORES][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS];
    uint8_t max_lpus_in_a_chunk;
    struct kaps_jr1_db *db;
    struct kaps_jr1_ix_mgr *ix_mgr;

    uint32_t iter = 0;

    if (lsn_p == NULL)
    {
        *o_reason = NLMRSC_INVALID_POINTER;
        return NLMERR_FAIL;
    }

    if (lsn_p->m_pSettings == NULL || lsn_p->m_pTbl->m_alloc_p == NULL ||
         lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0] == NULL)
    {
        *o_reason = NLMRSC_INVALID_POINTER;
        return NLMERR_FAIL;
    }

    alloc_p = lsn_p->m_pTbl->m_alloc_p;
    lsn_settings = lsn_p->m_pSettings;

    lsn_p->m_bAllocedResource = 1;
    lsn_p->m_bDoGiveout = 0;
    lsn_p->m_bIsCompacted = 0;
    lsn_p->m_bIsNewLsn = 0;

    
    lsn_p->m_nAllocBase = lsn_info->base_address;
    lsn_p->m_nNumIxAlloced = lsn_info->ix_allocated;

    lsn_p->m_nNumPrefixes = 0;  
    lsn_p->m_lsnId = lsn_settings->m_lsnid++;
    lsn_p->m_nLsnCapacity = 0;  

    
    if (!lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->chunk_map[0]
        && lsn_p->m_pTbl->m_db->device->type == KAPS_JR1_DEVICE)
    {
        uint16_t no_of_regions;
        struct region region_info[MAX_UDA_REGIONS];

        kaps_jr1_memcpy(alloc_udm, lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->alloc_udm, sizeof(alloc_udm));
        db = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db;
        max_lpus_in_a_chunk = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->max_lpus_in_a_chunk;
        no_of_regions = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->no_of_regions;
        kaps_jr1_memcpy(region_info, lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->region_info, sizeof(region_info));

        kaps_jr1_uda_mgr_destroy(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]);
        kaps_jr1_uda_mgr_init(db->device, db, 0, lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p->alloc,
                          alloc_udm,
                          &lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0], max_lpus_in_a_chunk,
                          1, UDM_BOTH_HALF,
                          kaps_jr1_fib_uda_sub_block_copy_callback, kaps_jr1_fib_update_it_callback,
                          kaps_jr1_fib_update_lsn_size, 0);

        lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->no_of_regions = no_of_regions;
        kaps_jr1_memcpy(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->region_info, region_info, sizeof(region_info));
        KAPS_JR1_STRY(kaps_jr1_uda_mgr_wb_restore_regions
                  (lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0], lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p->alloc,
                   no_of_regions, region_info));
    }

    if (lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->chunk_map[0] == NULL)
        KAPS_JR1_STRY(kaps_jr1_uda_mgr_wb_restore_regions(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0],
                                                  lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p->alloc,
                                                  lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->no_of_regions,
                                                  lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->region_info));

    errNum = kaps_jr1_uda_mgr_wb_alloc(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0], lsn_p, (lsn_info->num_lpus),
                                   lsn_info->region_id, lsn_info->offset, lsn_p->m_lsnId, &lsn_p->m_mlpMemInfo);
    if (errNum != NLMERR_OK)
        return errNum;
    lsn_p->m_mlpMemInfo->db = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db;

    if (!lsn_p->m_pSettings->m_isPerLpuGran)
    {
        struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) lsn_p->m_pTrie->m_tbl_ptr->m_db->common_info->ad_info.ad;

        ix_mgr = kaps_jr1_lsn_mc_get_ix_mgr_for_lsn_pfx(lsn_p, ad_db, lsn_p->m_mlpMemInfo, 0);

        if (lsn_p->m_pSettings->m_isHardwareMappedIx)
        {
            errNum =
                kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lsn(lsn_p, lsn_p->m_mlpMemInfo, ad_db, &lsn_p->m_ixInfo, o_reason);
            lsn_p->m_nAllocBase = lsn_p->m_ixInfo->start_ix;
            lsn_p->m_nNumIxAlloced = (uint16_t) lsn_p->m_ixInfo->size;
        }
        else
        {
            errNum = kaps_jr1_ix_mgr_wb_alloc(ix_mgr, ad_db, lsn_p->m_nNumIxAlloced, lsn_p->m_nAllocBase,
                                          lsn_p, IX_USER_LSN, &lsn_p->m_ixInfo);
        }
    }
    if (errNum != NLMERR_OK)
        return errNum;


    
    for (iter = 0; iter < lsn_info->num_lpus; iter++)
    {
        new_brick = (kaps_jr1_lpm_lpu_brick *) kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        if (!new_brick)
        {
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        if (lsn_info->brick_info[iter].ad_db)
        {
            KAPS_JR1_STRY(kaps_jr1_ad_db_refresh_handle
                      (lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p, lsn_info->brick_info[iter].ad_db, &new_brick->ad_db));
        }

        new_brick->m_next_p = NULL;
        new_brick->m_gran = lsn_info->brick_info[iter].gran;
        new_brick->m_granIx = lsn_info->brick_info[iter].gran_ix;
        new_brick->meta_priority = lsn_info->brick_info[iter].meta_priority;
        new_brick->m_hasReservedSlot = lsn_info->brick_info[iter].m_hasReservedSlot;
        new_brick->m_underAllocatedIx = lsn_info->brick_info[iter].m_underAllocatedIx;
        new_brick->m_maxCapacity = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(lsn_p->m_pSettings, new_brick->ad_db,
                                                                         new_brick->m_hasReservedSlot,
                                                                         new_brick->meta_priority,
                                                                         lsn_info->brick_info[iter].gran);
        new_brick->m_numPfx = 0;

        lsn_p->m_nLsnCapacity += new_brick->m_maxCapacity;      

        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            if (lsn_p->m_pSettings->m_isHardwareMappedIx)
            {
                kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lpu(lsn_p, new_brick, iter, new_brick->ad_db, &new_brick->m_ixInfo,
                                                        o_reason);
            }
            else
            {
                ix_mgr = kaps_jr1_lsn_mc_get_ix_mgr_for_lsn_pfx(lsn_p, new_brick->ad_db,
                                                lsn_p->m_mlpMemInfo, iter);

                errNum =
                    kaps_jr1_ix_mgr_wb_alloc(ix_mgr, new_brick->ad_db,
                                         lsn_info->brick_info[iter].brick_ix_allocated,
                                         lsn_info->brick_info[iter].base_address, lsn_p, IX_USER_LSN,
                                         &new_brick->m_ixInfo);
            }
        }

        new_brick->m_pfxes = (kaps_jr1_pfx_bundle **) kaps_jr1_nlm_allocator_calloc(alloc_p, new_brick->m_maxCapacity,
                                                                            sizeof(kaps_jr1_pfx_bundle *));
        if (!new_brick->m_pfxes)
        {
            kaps_jr1_nlm_allocator_free(alloc_p, new_brick);
            *o_reason = NLMRSC_LOW_MEMORY;
            return NLMERR_FAIL;
        }

        if (iter)
        {
            tail->m_next_p = new_brick;
            tail = new_brick;
        }
        else
        {
            head = new_brick;
            tail = new_brick;
        }
    }

    lsn_p->m_lpuList = head;
    lsn_p->m_numLpuBricks = lsn_info->num_lpus;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_wb_insert_pfx(
    kaps_jr1_lsn_mc * lsn_p,
    uint32_t insert_index,
    struct kaps_jr1_lpm_entry * entry)
{
    uint16_t max_pfx_in_lpu_brk = 0;
    uint32_t  which_brick = 0;
    uint32_t end_ix;

    
    kaps_jr1_lpm_lpu_brick *cur_lpu_brk;
    int32_t i = 0;
    uint32_t newix;
    uint32_t lpu_offset = 0;

    
    kaps_jr1_assert((lsn_p != NULL), "Not valid lsn ptr");
    kaps_jr1_assert(lsn_p->m_bAllocedResource, "Resources not allocated");

    kaps_jr1_assert((lsn_p->m_lpuList != NULL), " LSN LPU list is NULL");
    kaps_jr1_assert((lsn_p->m_mlpMemInfo != NULL), " LSN MLP info is NULL");
    if (!lsn_p->m_pSettings->m_isPerLpuGran)
    {
        kaps_jr1_assert((insert_index >= lsn_p->m_nAllocBase), " insert_index < base");
        kaps_jr1_assert((insert_index >= lsn_p->m_nAllocBase), " Index Location is out of range");
        kaps_jr1_assert((insert_index < lsn_p->m_nAllocBase + lsn_p->m_nNumIxAlloced), " Index Location is out of range");
    }

    cur_lpu_brk = lsn_p->m_lpuList;
    

    
    kaps_jr1_assert(cur_lpu_brk, "Can't Insert Prefix at Location!!\n");
    newix = lsn_p->m_nAllocBase;
    end_ix = newix + cur_lpu_brk->m_maxCapacity - 1;
    if (lsn_p->m_pSettings->m_isPerLpuGran)
    {
        newix = cur_lpu_brk->m_ixInfo->start_ix;
        end_ix = newix + cur_lpu_brk->m_ixInfo->size - 1;
    }

    while (insert_index < newix || insert_index > end_ix)
    {
        cur_lpu_brk = cur_lpu_brk->m_next_p;
        kaps_jr1_assert(cur_lpu_brk, "Can't Insert Prefix at Location!!\n");
        newix += cur_lpu_brk->m_maxCapacity;
        end_ix += cur_lpu_brk->m_maxCapacity;
        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            newix = cur_lpu_brk->m_ixInfo->start_ix;
            end_ix = newix + cur_lpu_brk->m_ixInfo->size - 1;
        }
        which_brick++;
    }
    cur_lpu_brk = lsn_p->m_lpuList;

    while (which_brick)
    {
        if (!cur_lpu_brk)
        {
            kaps_jr1_assert(0, "Can't Insert Prefix at Location!!\n");
            return NLMERR_FAIL;
        }
        lpu_offset += cur_lpu_brk->m_maxCapacity;
        cur_lpu_brk = cur_lpu_brk->m_next_p;
        kaps_jr1_assert((cur_lpu_brk != NULL), "Not valid lbrick ");
        which_brick--;
    }

    if (cur_lpu_brk)
    {
        max_pfx_in_lpu_brk = cur_lpu_brk->m_maxCapacity;
        if (lsn_p->m_pSettings->m_isPerLpuGran)
            i = insert_index - cur_lpu_brk->m_ixInfo->start_ix;
        else
            i = (insert_index - lsn_p->m_nAllocBase - lpu_offset) % max_pfx_in_lpu_brk;

        if (lsn_p->m_pSettings->m_isPerLpuGran)
            newix = cur_lpu_brk->m_ixInfo->start_ix + i;
        else
            newix = lsn_p->m_nAllocBase + lpu_offset + i;
        if ((insert_index == newix) && (cur_lpu_brk->m_pfxes[i] == NULL))
        {
            cur_lpu_brk->m_pfxes[i] = entry->pfx_bundle;
            entry->pfx_bundle->m_nIndex = newix;
            entry->pfx_bundle->m_isLmpsofarPfx = 0;

            
            if (lsn_p->m_pTrie->m_hashtable_p && !entry->pfx_bundle->m_isPfxCopy)
                kaps_jr1_pfx_hash_table_insert(lsn_p->m_pTrie->m_hashtable_p, (struct kaps_jr1_entry *) entry);

        }
        else
        {
            kaps_jr1_assert(0, "Prefix already inserted!!\n");
            return NLMERR_FAIL;
        }
        cur_lpu_brk->m_numPfx++;        
    }
    else
    {
        kaps_jr1_assert(0, "Can't Insert Prefix at Location!!\n");
        return NLMERR_FAIL;
    }

    lsn_p->m_nNumPrefixes++;    

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_wb_prepare_lsn_info(
    kaps_jr1_lsn_mc * lsn_p,
    struct kaps_jr1_wb_lsn_info * lsn_info)
{
    uint8_t i;
    kaps_jr1_lpm_lpu_brick *brick = NULL;

    lsn_info->lsn_id = lsn_p->m_lsnId;
    lsn_info->num_lpus = lsn_p->m_numLpuBricks;
    lsn_info->region_id = lsn_p->m_mlpMemInfo->region_id;
    lsn_info->offset = lsn_p->m_mlpMemInfo->offset;
    lsn_info->base_address = lsn_p->m_nAllocBase;
    lsn_info->ix_allocated = lsn_p->m_nNumIxAlloced;

    brick = lsn_p->m_lpuList;
    i = 0;

    while (brick)
    {
        lsn_info->brick_info[i].gran = brick->m_gran;
        lsn_info->brick_info[i].gran_ix = brick->m_granIx;
        lsn_info->brick_info[i].meta_priority = brick->meta_priority;
        lsn_info->brick_info[i].m_hasReservedSlot = brick->m_hasReservedSlot;


        lsn_info->brick_info[i].m_underAllocatedIx = brick->m_underAllocatedIx;
        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            lsn_info->brick_info[i].base_address = brick->m_ixInfo->start_ix;
            lsn_info->brick_info[i].ad_db = brick->ad_db;
            lsn_info->brick_info[i].brick_ix_allocated = brick->m_ixInfo->size;
        }
        else
        {
            lsn_info->brick_info[i].ad_db = brick->ad_db;
        }

        i++;
        brick = brick->m_next_p;
    }
    lsn_info->special_entry_info.brick_ix_allocated = 0;


    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_wb_store_prefixes(
    kaps_jr1_lsn_mc * lsn_p,
    uint32_t * nv_offset,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle)
{
    kaps_jr1_lpm_lpu_brick *cur_lpu_brk;
    struct kaps_jr1_entry *entry;
    struct kaps_jr1_device *device;
    uint16_t iter = 0;
    uint32_t lopoff_8;
    uint16_t entries_in_brick = 0;
    uint32_t temp_offset = 0;
    uint32_t total_lsn_data_length_8 = 0;
    uint32_t offset = 0;
    uint32_t max_prefixes_in_brick = 0;
    struct kaps_jr1_wb_prefix_info prefix_info;
    uint32_t total_num_prefixes = 0;

    if (0 != write_fn(handle, (uint8_t *) & (lsn_p->m_nNumPrefixes), sizeof(lsn_p->m_nNumPrefixes), *nv_offset))
        return NLMERR_FAIL;
    *nv_offset += sizeof(lsn_p->m_nNumPrefixes);

    if (!lsn_p->m_nNumPrefixes)
        return NLMERR_OK;

    
    lopoff_8 = lsn_p->m_nLopoff / 8;

    device = lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p;

    cur_lpu_brk = lsn_p->m_lpuList;
    while (cur_lpu_brk)
    {
        max_prefixes_in_brick = cur_lpu_brk->m_maxCapacity;
        entries_in_brick = 0;

        
        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            if (cur_lpu_brk->m_underAllocatedIx)
                max_prefixes_in_brick = cur_lpu_brk->m_ixInfo->size;
            temp_offset = *nv_offset;
            *nv_offset += sizeof(entries_in_brick);
        }

        for (iter = 0; iter < max_prefixes_in_brick; iter++)
        {
            kaps_jr1_pfx_bundle *b = cur_lpu_brk->m_pfxes[iter];
            uint32_t index, len_8;
            uint16_t pfx_len, relative_index;

            if (b)
            {
                pfx_len = KAPS_JR1_PFX_BUNDLE_GET_PFX_SIZE(b);
                prefix_info.pfx_len = pfx_len;

                index = KAPS_JR1_PFX_BUNDLE_GET_INDEX(b);
                if (lsn_p->m_pSettings->m_isPerLpuGran)
                    relative_index = index - cur_lpu_brk->m_ixInfo->start_ix;
                else
                    relative_index = index - lsn_p->m_nAllocBase;

                prefix_info.index = relative_index;
                prefix_info.meta_priority = b->m_backPtr->meta_priority;
                prefix_info.pfx_copy = b->m_isPfxCopy;

                entry = (struct kaps_jr1_entry *) b->m_backPtr;

                prefix_info.user_handle = entry->user_handle;

                if (entry->ad_handle)
                {
                    prefix_info.ad_seq_num = KAPS_JR1_GET_AD_SEQ_NUM(entry->ad_handle);
                }

                if (0 != write_fn(handle, (uint8_t *) & (prefix_info), sizeof(prefix_info), *nv_offset))
                    return NLMERR_FAIL;
                *nv_offset += sizeof(prefix_info);

                len_8 = (pfx_len + 7) / 8;
                len_8 -= lopoff_8;      

                total_lsn_data_length_8 += len_8;

                if (entry->ad_handle)
                {
                    struct kaps_jr1_ad_db *ad_db;
                    uint32_t ad_user_hdl = entry->ad_handle->user_handle;

                    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, ad_db)
                        if (0 != write_fn(handle, (uint8_t *) & (ad_user_hdl), sizeof(ad_user_hdl), *nv_offset))
                        return NLMERR_FAIL;
                    *nv_offset += sizeof(ad_user_hdl);


                    if (0 !=
                        write_fn(handle, (uint8_t *) & (entry->ad_handle->value[ad_db->byte_offset]),
                                 sizeof(uint8_t) * ad_db->user_bytes, *nv_offset))
                        return NLMERR_FAIL;
                    *nv_offset += (sizeof(uint8_t) * ad_db->user_bytes);

                    {
                        uintptr_t hb_user_handle = 0;
                        if (b->m_backPtr->hb_user_handle)
                            hb_user_handle = b->m_backPtr->hb_user_handle;

                        if (0 != write_fn(handle, (uint8_t *) & (hb_user_handle), sizeof(hb_user_handle), *nv_offset))
                            return NLMERR_FAIL;
                        *nv_offset += sizeof(hb_user_handle);
                    }
                }
                entries_in_brick++;
            }
        }

        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            if (0 != write_fn(handle, (uint8_t *) & (entries_in_brick), sizeof(entries_in_brick), temp_offset))
                return NLMERR_FAIL;
        }
        total_num_prefixes += entries_in_brick;
        cur_lpu_brk = cur_lpu_brk->m_next_p;
    }

    cur_lpu_brk = lsn_p->m_lpuList;
    while (cur_lpu_brk)
    {
        max_prefixes_in_brick = cur_lpu_brk->m_maxCapacity;
        if (lsn_p->m_pSettings->m_isPerLpuGran && cur_lpu_brk->m_underAllocatedIx)
            max_prefixes_in_brick = cur_lpu_brk->m_ixInfo->size;

        for (iter = 0; iter < max_prefixes_in_brick; iter++)
        {
            kaps_jr1_pfx_bundle *b = cur_lpu_brk->m_pfxes[iter];
            uint8_t *pfx_data = NULL;
            uint32_t len_8;
            uint16_t pfx_len;

            if (b)
            {
                pfx_len = KAPS_JR1_PFX_BUNDLE_GET_PFX_SIZE(b);

                len_8 = (pfx_len + 7) / 8;
                len_8 -= lopoff_8;      

                pfx_data = KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(b);
                kaps_jr1_memcpy(&lsn_p->m_pSettings->lsn_data_buffer[offset], &pfx_data[lopoff_8], len_8);
                offset += len_8;
            }
        }
        cur_lpu_brk = cur_lpu_brk->m_next_p;
    }

    
    if (0 != write_fn(handle, (uint8_t *) & (lsn_p->m_pSettings->lsn_data_buffer), total_lsn_data_length_8, *nv_offset))
        return NLMERR_FAIL;
    *nv_offset += total_lsn_data_length_8;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_wb_restore_pfx_data(
    kaps_jr1_lsn_mc * lsn_p,
    uint32_t * nv_offset,
    kaps_jr1_device_issu_read_fn read_fn,
    void *handle)
{
    struct kaps_jr1_wb_prefix_data **prefix = NULL;
    kaps_jr1_lpm_lpu_brick *cur_lpu_brk;
    kaps_jr1_pfx_bundle *pfx_bundle = NULL;
    uint16_t i, iter, num_prefixes = 0;
    uint32_t lopoff_8, len_8;
    uint16_t entries_in_brick = 0;
    uint32_t total_lsn_data_length_8 = 0;
    uint32_t offset = 0;
    struct kaps_jr1_ad_db *ad_db;
    uintptr_t hb_user_handle;
    uint32_t num_bytes_to_alloc;

    if (0 != read_fn(handle, (uint8_t *) & (num_prefixes), sizeof(num_prefixes), *nv_offset))
        return NLMERR_FAIL;
    *nv_offset += sizeof(num_prefixes);

    if (!num_prefixes)
        return NLMERR_OK;

    
    lopoff_8 = lsn_p->m_nLopoff / 8;
    pfx_bundle = lsn_p->m_pParentHandle;

    cur_lpu_brk = lsn_p->m_lpuList;
    if (lsn_p->m_pSettings->m_isPerLpuGran && num_prefixes)
    {
        if (0 != read_fn(handle, (uint8_t *) & (entries_in_brick), sizeof(entries_in_brick), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(entries_in_brick);
    }

    prefix =
        kaps_jr1_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, num_prefixes, sizeof(struct kaps_jr1_wb_prefix_data *));
    if (!prefix)
        return NLMERR_FAIL;

    for (iter = 0; iter < num_prefixes; iter++)
    {
        prefix[iter] = kaps_jr1_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, 1, sizeof(struct kaps_jr1_wb_prefix_data));

        if (!prefix[iter])
        {
            for (i = 0; i < iter; ++i)
            {
                kaps_jr1_nlm_allocator_free(lsn_p->m_pSettings->m_pAlloc, prefix[i]);
            }

            kaps_jr1_nlm_allocator_free(lsn_p->m_pSettings->m_pAlloc, prefix);
            return NLMERR_FAIL;
        }
    }

    for (iter = 0; iter < num_prefixes; iter++)
    {
        if (lsn_p->m_pSettings->m_isPerLpuGran)
        {
            while (!entries_in_brick)
            {
                if (0 != read_fn(handle, (uint8_t *) & (entries_in_brick), sizeof(entries_in_brick), *nv_offset))
                    return NLMERR_FAIL;
                *nv_offset += sizeof(entries_in_brick);
                cur_lpu_brk = cur_lpu_brk->m_next_p;
            }
        }

        if (0 != read_fn(handle, (uint8_t *) & prefix[iter]->info, sizeof(prefix[iter]->info), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(prefix[iter]->info);

        len_8 = (prefix[iter]->info.pfx_len + 7) / 8;

        num_bytes_to_alloc = len_8;
        if (num_bytes_to_alloc == 0)
            num_bytes_to_alloc = 1;

        prefix[iter]->data = kaps_jr1_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, num_bytes_to_alloc,
                                                       sizeof(uint8_t));

        if (!prefix[iter]->data)
            return NLMERR_FAIL;

        
        len_8 -= lopoff_8;      
        total_lsn_data_length_8 += len_8;
        kaps_jr1_memcpy(prefix[iter]->data, pfx_bundle->m_data, lopoff_8);  

        if (lsn_p->m_pSettings->m_isPerLpuGran)
            prefix[iter]->info.index += cur_lpu_brk->m_ixInfo->start_ix;
        else
            prefix[iter]->info.index += lsn_p->m_nAllocBase;

        if (lsn_p->m_pSettings->m_isPerLpuGran)
            ad_db = cur_lpu_brk->ad_db;
        else
            ad_db = (struct kaps_jr1_ad_db *) lsn_p->m_pTbl->m_db->common_info->ad_info.ad;

        if (ad_db)
        {
            uint32_t ad_bytes = ad_db->user_bytes;
            uint32_t ad_db_seq_num = prefix[iter]->info.ad_seq_num;
            struct kaps_jr1_device *device = lsn_p->m_pTbl->m_fibTblMgr_p->m_devMgr_p;

            kaps_jr1_sassert(ad_db_seq_num);
            kaps_jr1_sassert(ad_db_seq_num <= device->seq_num_to_ptr->num_ad_db);
            kaps_jr1_sassert(device->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num]);
            ad_db = device->seq_num_to_ptr->ad_db_ptr[ad_db_seq_num];
            ad_bytes = ad_db->user_bytes;

            if (0 !=
                read_fn(handle, (uint8_t *) & (prefix[iter]->ad_user_handle), sizeof(prefix[iter]->ad_user_handle),
                        *nv_offset))
                return NLMERR_FAIL;
            *nv_offset += sizeof(prefix[iter]->ad_user_handle);


            if (ad_bytes)
            {
                prefix[iter]->ad = kaps_jr1_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, ad_bytes, sizeof(uint8_t));

                if (!prefix[iter]->ad)
                    return NLMERR_FAIL;

                if (0 != read_fn(handle, (uint8_t *) prefix[iter]->ad, sizeof(uint8_t) * ad_bytes, *nv_offset))
                    return NLMERR_FAIL;
                *nv_offset += (sizeof(uint8_t) * ad_bytes);
            }

            if (0 != read_fn(handle, (uint8_t *) & (hb_user_handle), sizeof(hb_user_handle), *nv_offset))
                return NLMERR_FAIL;
            *nv_offset += sizeof(hb_user_handle);

            if (device->type == KAPS_JR1_DEVICE && lsn_p->m_pSettings->m_areHitBitsPresent)
            {

                prefix[iter]->hb_user_handle = hb_user_handle;

                prefix[iter]->hb_no = kaps_jr1_get_old_pfx_hb_index(lsn_p, prefix[iter]->info.index);
            }

        }

        if (!prefix[iter]->info.pfx_copy)
            kaps_jr1_c_list_add_tail(&lsn_p->m_pTbl->wb_prefixes, &prefix[iter]->kaps_jr1_list_node);
        else
            kaps_jr1_c_list_add_tail(&lsn_p->m_pTbl->wb_apt_pfx_copies, &prefix[iter]->kaps_jr1_list_node);
        if (lsn_p->m_pSettings->m_isPerLpuGran)
            entries_in_brick--;
    }

    if (lsn_p->m_pSettings->m_isPerLpuGran && num_prefixes)
    {
        while (cur_lpu_brk && cur_lpu_brk->m_next_p)
        {
            if (0 != read_fn(handle, (uint8_t *) & (entries_in_brick), sizeof(entries_in_brick), *nv_offset))
                return NLMERR_FAIL;
            *nv_offset += sizeof(entries_in_brick);
            kaps_jr1_sassert(entries_in_brick == 0);
            cur_lpu_brk = cur_lpu_brk->m_next_p;
        }
    }
    
    if (0 != read_fn(handle, (uint8_t *) & (lsn_p->m_pSettings->lsn_data_buffer), total_lsn_data_length_8, *nv_offset))
        return NLMERR_FAIL;
    *nv_offset += total_lsn_data_length_8;

    for (iter = 0; iter < num_prefixes; iter++)
    {
        len_8 = (prefix[iter]->info.pfx_len + 7) / 8;
        len_8 -= lopoff_8;

        
        kaps_jr1_memcpy(&prefix[iter]->data[lopoff_8], &lsn_p->m_pSettings->lsn_data_buffer[offset], len_8);
        offset += len_8;
    }

    kaps_jr1_nlm_allocator_free(lsn_p->m_pSettings->m_pAlloc, prefix);

    return NLMERR_OK;
}

NlmErrNum_t kaps_jr1_lsn_mc_handle_under_alloc(kaps_jr1_lsn_mc *newLsn, kaps_jr1_lpm_lpu_brick *cur_lpu_brick,
                                           uint32_t curBrickIter, uint32_t *ixRqtSize)
{
    uint32_t ix_size_to_allocate = 0;

    if (cur_lpu_brick->ad_db && cur_lpu_brick->ad_db->db_info.hw_res.ad_res->ad_type != KAPS_JR1_AD_TYPE_INDEX)
        return NLMERR_OK;

    if (newLsn->m_pSettings->m_isUnderAllocateLpuIx
        && !cur_lpu_brick->m_hasReservedSlot
        && cur_lpu_brick->ad_db) {

        uint32_t next_1Gran = kaps_jr1_lsn_mc_get_next_max_gran(newLsn->m_pSettings, cur_lpu_brick->m_gran, 3);
        uint32_t next_2Gran = kaps_jr1_lsn_mc_get_next_max_gran(newLsn->m_pSettings, cur_lpu_brick->m_gran, 4);


        if (curBrickIter >= newLsn->m_pSettings->m_maxLpuPerLsn - newLsn->m_pSettings->m_numUnderallocateLpus) {

            if (cur_lpu_brick->m_numPfx == 0) {
                ix_size_to_allocate = 2;
            } else if (cur_lpu_brick->m_numPfx + cur_lpu_brick->ad_db->ix_max_num_holes_allowed < *ixRqtSize) {
                ix_size_to_allocate = cur_lpu_brick->m_numPfx + cur_lpu_brick->ad_db->ix_max_num_holes_allowed;
            } else if (next_2Gran) {
                ix_size_to_allocate = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(newLsn->m_pSettings,
                                                                                    cur_lpu_brick->ad_db,
                                                                                    cur_lpu_brick->m_hasReservedSlot,
                                                                                    cur_lpu_brick->meta_priority,
                                                                                    next_2Gran);

            } else if (next_1Gran) {
                ix_size_to_allocate = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(newLsn->m_pSettings,
                                                                                cur_lpu_brick->ad_db,
                                                                                cur_lpu_brick->m_hasReservedSlot,
                                                                                cur_lpu_brick->meta_priority,
                                                                                next_1Gran);
            }
        }

        if (cur_lpu_brick->m_numPfx < ix_size_to_allocate) {
            *ixRqtSize = ix_size_to_allocate;
            cur_lpu_brick->m_underAllocatedIx = 1;
        }
    }

    return NLMERR_OK;
}

static NlmErrNum_t kaps_jr1_lsn_mc_move_lpu_gran_holes_to_one_side(kaps_jr1_lsn_mc *self,
                                                         kaps_jr1_lpm_lpu_brick *curLpuBrick,
                                                         NlmReasonCode  *o_reason)
{
    int32_t holePos, pfxPos, holeIx, pfxIx;
    int32_t foundHole, foundPrefix, done;
    int32_t i;
    kaps_jr1_pfx_bundle *pfxBundle;
    kaps_jr1_lpm_lpu_brick *tmpBrick = self->m_lpuList;
    int32_t max_pfxs = curLpuBrick->m_maxCapacity;
    NlmErrNum_t errNum = NLMERR_OK;

    while (tmpBrick) {

        if (tmpBrick->m_ixInfo->size < tmpBrick->m_maxCapacity)
            tmpBrick->m_underAllocatedIx = 1;

        if (tmpBrick == curLpuBrick)
            break;
        tmpBrick = tmpBrick->m_next_p;
    }

    kaps_jr1_sassert (tmpBrick != NULL);

    kaps_jr1_sassert(tmpBrick->m_hasReservedSlot == 0);

    while (tmpBrick) {
        if (tmpBrick->m_ixInfo->size < tmpBrick->m_maxCapacity)
            tmpBrick->m_underAllocatedIx = 1;

        tmpBrick = tmpBrick->m_next_p;
    }

    holePos= 0;
    holeIx = curLpuBrick->m_ixInfo->start_ix;

    if (max_pfxs < curLpuBrick->m_ixInfo->size)
        max_pfxs = curLpuBrick->m_ixInfo->size;

    for (i = curLpuBrick->m_maxCapacity - 1; i >= 0; i--) {
        if (curLpuBrick->m_pfxes[i])
            break;
    }

    pfxPos = i;
    pfxIx  = curLpuBrick->m_ixInfo->start_ix + i;


    done = 0;
    while (!done) {
        
        foundHole = 0;
        while ((!foundHole) && (holePos <= max_pfxs)) {
            if (curLpuBrick->m_pfxes[holePos] == NULL) {
                foundHole = 1;
                break;
            }
            holePos++;
            holeIx++;
        }

        if (!foundHole) {
            done = 1;
            break;
        }

        
        foundPrefix = 0;
        while (!foundPrefix && pfxPos >= 0) {
            if (curLpuBrick->m_pfxes[pfxPos]) {
                foundPrefix = 1;
                break;
            }

            --pfxPos;
            --pfxIx;
        }

        if (!foundPrefix) {
            done = 1;
            break;
        }

        if (holePos < pfxPos) {

            
            pfxBundle = curLpuBrick->m_pfxes[pfxPos];
            curLpuBrick->m_pfxes[holePos] = pfxBundle;
            curLpuBrick->m_pfxes[holePos] = pfxBundle;
            curLpuBrick->m_pfxes[pfxPos] = NULL;

            pfxBundle->m_nIndex = holeIx;

            ++holePos;
            ++holeIx;

            --pfxPos;
            --pfxIx;
        } else {
            done = 1;
            break;
        }
    }

    return errNum;
}

kaps_jr1_status kaps_jr1_lsn_mc_ad_chunk_move(struct kaps_jr1_ix_mgr *mgr,
                                        struct kaps_jr1_ix_compaction_chunk *cur_playback_chunk)
{
    kaps_jr1_lsn_mc *self;
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    struct kaps_jr1_ix_chunk *ix_chunk;
    kaps_jr1_pfx_bundle *lmpsofar;
    NlmReasonCode o_reason;
    NlmErrNum_t errNum = NLMERR_OK;
    int32_t brick_idx = 0;

    ix_chunk = cur_playback_chunk->kaps_jr1_ix_chunk;

    ix_chunk->start_ix = cur_playback_chunk->to_start_ix;
    ix_chunk->size = cur_playback_chunk->to_size;
    ix_chunk->ad_info->ad_blk_no = cur_playback_chunk->to_ad_blk_no;
    ix_chunk->ad_info->offset_in_ad_blk = cur_playback_chunk->to_offset_in_ad_blk;

    if (ix_chunk->user_type == IX_LMPSOFAR)
    {
        struct kaps_jr1_db *db;
        struct kaps_jr1_lpm_db *lpm_db;
        kaps_jr1_trie_node *destnode;
        struct kaps_jr1_lsn_mc_settings *lsn_settings;

        lmpsofar = (kaps_jr1_pfx_bundle *)ix_chunk->lsn_ptr;

        if (lmpsofar->m_isLmpsofarPfx == 0)
            return NLMERR_OK;

        KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(mgr->device, lmpsofar->m_backPtr, db);
        lpm_db = (struct kaps_jr1_lpm_db *)db;

        destnode = kaps_jr1_trie_find_dest_lp_node(lpm_db->fib_tbl->m_trie,
                lpm_db->fib_tbl->m_trie->m_roots_trienode_p,
                lmpsofar->m_data, lmpsofar->m_nPfxSize, NULL, 0);

        lsn_settings = lpm_db->fib_tbl->m_trie->m_lsn_settings_p;
        
        if (lsn_settings->m_strictlyStoreLmpsofarInAds &&
            destnode->m_depth == lmpsofar->m_nPfxSize)
        {
            errNum = kaps_jr1_lsn_mc_update_iit(destnode->m_lsn_p, &o_reason);

            if (errNum != NLMERR_OK)
                return KAPS_JR1_INTERNAL_ERROR;
        }

        errNum = kaps_jr1_trie_process_iit_lmpsofar(lpm_db->fib_tbl->m_trie,
                destnode, lmpsofar->m_backPtr, NLM_FIB_PREFIX_INSERT, &o_reason);

        if (errNum != NLMERR_OK)
            return KAPS_JR1_INTERNAL_ERROR;
        
        kaps_jr1_trie_verify_iit_lmpsofar(NULL, lpm_db->fib_tbl->m_trie->m_roots_trienode_p);

        return errNum;
    }

    self = (kaps_jr1_lsn_mc *)ix_chunk->lsn_ptr;
    curLpuBrick = self->m_lpuList;

    while (curLpuBrick)
    {
        if (curLpuBrick->m_ixInfo == ix_chunk)
            break;
        brick_idx++;
        curLpuBrick = curLpuBrick->m_next_p;
    }

    kaps_jr1_sassert (curLpuBrick);
    if (cur_playback_chunk->to_size < cur_playback_chunk->from_size)
    {
        errNum = kaps_jr1_lsn_mc_move_lpu_gran_holes_to_one_side(self, curLpuBrick, &o_reason);

        if (errNum != NLMERR_OK)
            return KAPS_JR1_INTERNAL_ERROR;
        curLpuBrick->m_underAllocatedIx = 1;
    }

    errNum = kaps_jr1_lsn_mc_commit_brick(self, brick_idx, &o_reason);

    if (errNum != NLMERR_OK)
        return KAPS_JR1_INTERNAL_ERROR;

    return errNum;
}


