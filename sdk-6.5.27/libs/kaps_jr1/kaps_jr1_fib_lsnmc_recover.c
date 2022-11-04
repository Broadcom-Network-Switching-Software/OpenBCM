

#include "kaps_jr1_fib_lsnmc_recover.h"
#include "kaps_jr1_fib_lsnmc_hw.h"
#include "kaps_jr1_fib_trie.h"
#include "kaps_jr1_ad_internal.h"

NlmErrNum_t
kaps_jr1_lsn_mc_recover_construct_pfx_bundle(
    kaps_jr1_lsn_mc_settings * pSettings,
    uint8_t * commonBits,
    uint32_t commonLength_1,
    uint8_t * suffix,
    int32_t suffixLength_1,
    uint32_t ix,
    kaps_jr1_pfx_bundle ** pfxBundle_pp,
    NlmReasonCode * o_reason)
{
    kaps_jr1_pfx_bundle *pfxBundle;
    uint32_t commonLength_8 = (commonLength_1 + 7) / KAPS_JR1_BITS_IN_BYTE;
    uint32_t suffixLength_8 = (suffixLength_1 + 7) / KAPS_JR1_BITS_IN_BYTE;
    uint32_t fullLength_1 = commonLength_1 + suffixLength_1;
    uint32_t fullLength_8 = (fullLength_1 + 7) / KAPS_JR1_BITS_IN_BYTE;
    kaps_jr1_nlm_allocator *alloc_p = pSettings->m_pAlloc;
    uint8_t fullPrefixData[KAPS_JR1_LPM_KEY_MAX_WIDTH_8];
    int32_t remainingLength_1, sourceBitPos, targetBitPos, numBitsToProcess;
    uint32_t value;

    *pfxBundle_pp = NULL;

    if (suffixLength_1 == -1)
        return NLMERR_OK;

    kaps_jr1_memset(fullPrefixData, 0, KAPS_JR1_LPM_KEY_MAX_WIDTH_8);

    kaps_jr1_memcpy(fullPrefixData, commonBits, commonLength_8);

    remainingLength_1 = suffixLength_1;
    sourceBitPos = (suffixLength_8 * KAPS_JR1_BITS_IN_BYTE) - 1;
    targetBitPos = (fullLength_8 * KAPS_JR1_BITS_IN_BYTE) - 1 - commonLength_1;

    while (remainingLength_1 > 0)
    {

        numBitsToProcess = remainingLength_1;
        if (numBitsToProcess > 32)
            numBitsToProcess = 32;

        value = KapsJr1ReadBitsInArrray(suffix, suffixLength_8, sourceBitPos, sourceBitPos - numBitsToProcess + 1);

        KapsJr1WriteBitsInArray(fullPrefixData, fullLength_8, targetBitPos, targetBitPos - numBitsToProcess + 1, value);

        remainingLength_1 -= numBitsToProcess;
        sourceBitPos -= numBitsToProcess;
        targetBitPos -= numBitsToProcess;
    }

    pfxBundle = kaps_jr1_pfx_bundle_create_from_string(alloc_p, fullPrefixData, fullLength_1, ix, 0, 0);

    if (!pfxBundle)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    *pfxBundle_pp = pfxBundle;
    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_recover_parse_pfx(
    uint8_t * brickData,
    uint32_t brickLength_1,
    uint32_t pfxMSBitPosInBrick,
    uint32_t gran,
    uint8_t * suffix,
    int32_t * suffixLength_1,
    NlmReasonCode * o_reason)
{
    int32_t remainingLength_1, numBitsToProcess;
    int32_t sourceBitPos, targetBitPos;
    uint32_t brickLength_8 = (brickLength_1 + 7) / KAPS_JR1_BITS_IN_BYTE;
    uint32_t targetLength_8 = (gran + 7) / KAPS_JR1_BITS_IN_BYTE;
    uint32_t value;
    uint32_t numUnusedBits, bitIter, bitValue;

    *suffixLength_1 = -1;

    remainingLength_1 = gran;
    sourceBitPos = pfxMSBitPosInBrick;
    targetBitPos = targetLength_8 * KAPS_JR1_BITS_IN_BYTE - 1;

    while (remainingLength_1 > 0)
    {
        numBitsToProcess = remainingLength_1;

        while (numBitsToProcess > 32)
            numBitsToProcess = 32;

        value = KapsJr1ReadBitsInArrray(brickData, brickLength_8, sourceBitPos, sourceBitPos - numBitsToProcess + 1);

        KapsJr1WriteBitsInArray(suffix, targetLength_8, targetBitPos, targetBitPos - numBitsToProcess + 1, value);

        remainingLength_1 -= numBitsToProcess;
        sourceBitPos -= numBitsToProcess;
        targetBitPos -= numBitsToProcess;
    }

    
    numUnusedBits = 8 - (gran % 8);

    numUnusedBits = numUnusedBits % 8;

    for (bitIter = 0; bitIter < gran; ++bitIter)
    {
        bitValue = (uint8_t) KapsJr1ReadBitsInArrray(suffix, targetLength_8, numUnusedBits + bitIter, numUnusedBits + bitIter);
        if (bitValue != 0)
        {
            *suffixLength_1 = gran - (bitIter + 1);
            break;
        }
    }

    return NLMERR_OK;

}

NlmErrNum_t
kaps_jr1_lsn_mc_recover_rebuild_brick(
    kaps_jr1_lsn_mc * lsn_p,
    kaps_jr1_lpm_lpu_brick * curBrick,
    kaps_jr1_prefix * commonPfx,
    uint32_t curBrickIter,
    uint32_t curLpu,
    uint32_t curRow,
    NlmReasonCode * o_reason)
{
    struct kaps_jr1_device *device = lsn_p->m_pSettings->m_device;
    kaps_jr1_lsn_mc_settings *settings = lsn_p->m_pSettings;
    struct kaps_jr1_shadow_device *kaps_jr1_shadow = device->kaps_jr1_shadow;
    struct kaps_jr1_shadow_bkt *curBB;
    uint32_t numBktsInCascade = device->hw_res->num_bb_in_one_bb_cascade;
    uint32_t curGranIx = curBrick->m_granIx;
    uint32_t curGran = device->hw_res->lpm_gran_array[curGranIx];
    struct kaps_jr1_ad_db *ad_db;
    NlmErrNum_t errNum;
    uint32_t i, pfxSlotInBB, maxNumPfxPerBB;
    uint32_t curPfxBitPos, pfxSlotInBrick;
    uint8_t suffix[KAPS_JR1_LPM_KEY_MAX_WIDTH_8];
    int32_t suffixLength_1 = -1;
    kaps_jr1_pfx_bundle *newPfxBundle;
    uint32_t curIndex, is_pending;
    kaps_jr1_status status;

    ad_db = (struct kaps_jr1_ad_db *) lsn_p->m_pTrie->m_tbl_ptr->m_db->common_info->ad_info.ad;

    curBrick->m_gran = curGran;
    curBrick->ad_db = ad_db;
    curBrick->m_maxCapacity = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(lsn_p->m_pSettings, ad_db, 0, 0xf, curGran);

    curBrick->m_pfxes =
        kaps_jr1_nlm_allocator_calloc(lsn_p->m_pSettings->m_pAlloc, curBrick->m_maxCapacity, sizeof(kaps_jr1_pfx_bundle *));
    if (!curBrick->m_pfxes)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (settings->m_isPerLpuGran)
    {
        errNum =
            kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lpu(lsn_p, curBrick, curBrickIter, ad_db, &curBrick->m_ixInfo,
                                                    o_reason);
        if (errNum != NLMERR_OK)
            return errNum;

        curIndex = curBrick->m_ixInfo->start_ix;
    }
    else
    {
        curIndex = (curBrickIter * curBrick->m_maxCapacity) + lsn_p->m_ixInfo->start_ix;
    }

    maxNumPfxPerBB = curBrick->m_maxCapacity / numBktsInCascade;
    pfxSlotInBrick = 0;

    for (i = 0; i < numBktsInCascade; ++i)
    {

        curBB = &kaps_jr1_shadow->bkt_blks[(numBktsInCascade * curLpu) + i];
        curPfxBitPos = KAPS_JR1_BKT_WIDTH_1 - 1;

        for (pfxSlotInBB = 0; pfxSlotInBB < maxNumPfxPerBB; ++pfxSlotInBB)
        {

            kaps_jr1_memset(suffix, 0, KAPS_JR1_LPM_KEY_MAX_WIDTH_8);

            kaps_jr1_lsn_mc_recover_parse_pfx(curBB->bkt_rows[curRow].data, KAPS_JR1_BKT_WIDTH_1, curPfxBitPos, curGran,
                                          suffix, &suffixLength_1, o_reason);

            newPfxBundle = NULL;
            errNum = kaps_jr1_lsn_mc_recover_construct_pfx_bundle(lsn_p->m_pSettings, commonPfx->m_data, commonPfx->m_inuse,
                                                              suffix, suffixLength_1, curIndex, &newPfxBundle,
                                                              o_reason);

            if (errNum != NLMERR_OK)
                return errNum;

            if (newPfxBundle)
            {
                status = kaps_jr1_lpm_cr_remap_prefix(lsn_p->m_pTrie->m_tbl_ptr, newPfxBundle, &is_pending);
                if (status != KAPS_JR1_OK)
                {
                    *o_reason = NLMRSC_INTERNAL_ERROR;
                    return NLMERR_FAIL;
                }

                if (is_pending)
                {
                    
                    errNum = kaps_jr1_lsn_mc_delete_entry_in_hw(lsn_p, curIndex, o_reason);
                    if (errNum != NLMERR_OK)
                        return errNum;
                    kaps_jr1_pfx_bundle_destroy(newPfxBundle, lsn_p->m_pSettings->m_pAlloc);
                    newPfxBundle = NULL;
                }
                if (newPfxBundle && lsn_p->m_pTrie->m_hashtable_p)
                {
                    kaps_jr1_pfx_hash_table_insert(lsn_p->m_pTrie->m_hashtable_p, (struct kaps_jr1_entry *) newPfxBundle->m_backPtr);
                    lsn_p->m_pTrie->m_tbl_ptr->m_numPrefixes++;
                }

            }

            curBrick->m_pfxes[pfxSlotInBrick] = newPfxBundle;

            if (newPfxBundle)
            {
                curBrick->m_numPfx++;
            }

            curPfxBitPos -= curGran;

            ++pfxSlotInBrick;
            ++curIndex;
        }

    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_lsn_mc_recover_rebuild_lsn(
    struct kaps_jr1_wb_lsn_info * lsn_info,
    kaps_jr1_lsn_mc * lsn_p,
    kaps_jr1_prefix * commonPfx,
    NlmReasonCode * o_reason)
{
    uint32_t i, curLpu, curRow;
    kaps_jr1_lpm_lpu_brick *curBrick, *prevBrick;
    NlmErrNum_t errNum = NLMERR_OK;
    uint8_t alloc_udm[KAPS_JR1_MAX_NUM_CORES][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS];
    uint8_t max_lpus_in_a_chunk;
    kaps_jr1_lsn_mc_settings *settings;
    struct kaps_jr1_db *db;
    struct kaps_jr1_ad_db *ad_db;

    if (!lsn_p)
    {
        *o_reason = NLMRSC_INVALID_POINTER;
        return NLMERR_FAIL;
    }

    settings = lsn_p->m_pSettings;

    lsn_p->m_bIsNewLsn = 0;

    
    if (!lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->chunk_map[0])
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

    errNum = kaps_jr1_uda_mgr_wb_alloc(lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0], lsn_p, (lsn_info->num_lpus),
                                   lsn_info->region_id, lsn_info->offset, lsn_p->m_lsnId, &lsn_p->m_mlpMemInfo);
    if (errNum != NLMERR_OK)
        return errNum;

    settings->m_numAllocatedBricksForAllLsns += lsn_info->num_lpus;

    lsn_p->m_mlpMemInfo->db = lsn_p->m_pTrie->m_trie_global->m_mlpmemmgr[0]->db;

    prevBrick = NULL;

    lsn_p->m_numLpuBricks = lsn_p->m_mlpMemInfo->size;

    ad_db = (struct kaps_jr1_ad_db *) lsn_p->m_pTrie->m_tbl_ptr->m_db->common_info->ad_info.ad;
        

    if (settings->m_isHardwareMappedIx && !settings->m_isPerLpuGran)
    {
        errNum = kaps_jr1_lsn_mc_assign_hw_mapped_ix_per_lsn(lsn_p, lsn_p->m_mlpMemInfo, ad_db, &lsn_p->m_ixInfo, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;
        lsn_p->m_nAllocBase = lsn_p->m_ixInfo->start_ix;
        lsn_p->m_nNumIxAlloced = (uint16_t) lsn_p->m_ixInfo->size;
    }

    for (i = 0; i < lsn_p->m_numLpuBricks; ++i)
    {

        kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(*(settings->m_pMlpMemMgr), lsn_p->m_mlpMemInfo, i, &curLpu, &curRow);

        curBrick = kaps_jr1_nlm_allocator_calloc(settings->m_pAlloc, 1, sizeof(kaps_jr1_lpm_lpu_brick));
        curBrick->m_granIx = lsn_info->brick_info[i].gran_ix;
        if (!settings->m_isPerLpuGran)
            curBrick->m_granIx = lsn_info->brick_info[0].gran_ix;

        if (prevBrick)
            prevBrick->m_next_p = curBrick;
        else
            lsn_p->m_lpuList = curBrick;

        switch (settings->m_device->type)
        {

            case KAPS_JR1_DEVICE:
                errNum =
                    kaps_jr1_lsn_mc_recover_rebuild_brick(lsn_p, curBrick, commonPfx, i, curLpu, curRow, o_reason);
                break;

            default:
                kaps_jr1_assert(0, "Incorrect device type while rebuilding LSN\n");
                break;
        }

        if (errNum != NLMERR_OK)
            return errNum;

        lsn_p->m_nNumPrefixes += curBrick->m_numPfx;
        lsn_p->m_nLsnCapacity += curBrick->m_maxCapacity;

        lsn_p->m_pTrie->m_tbl_ptr->m_numPrefixes += curBrick->m_numPfx;
        lsn_p->m_pTrie->m_tbl_ptr->m_db->common_info->cur_capacity += curBrick->m_numPfx;
        ad_db->num_entries_in_ad_db[0] += curBrick->m_numPfx;

        prevBrick = curBrick;

    }

    lsn_p->m_bAllocedResource = 1;
    return NLMERR_OK;
}
