

#include "kaps_jr1_fib_poolmgr.h"
#include "kaps_jr1_fib_triedata.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_ab.h"
#include "kaps_jr1_fib_lsnmc.h"

static NlmErrNum_t
kaps_jr1_pool_mgr_pvt_insert_rpt_node(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_trie_node * rptTrieNode_p,
    kaps_jr1_pool_info * curPoolInfo,
    uint32_t chosenRptId,
    NlmReasonCode * o_reason)
{
    
    kaps_jr1_trie_global *trie_global = rptTrieNode_p->m_trie_p->m_trie_global;
    kaps_jr1_rpts_in_pool *rptInPool_p = kaps_jr1_nlm_allocator_calloc(poolMgr->m_alloc_p, 1, sizeof(kaps_jr1_rpts_in_pool));

    if (!rptInPool_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }
    rptInPool_p->m_rptTrieNode_p = rptTrieNode_p;
    rptInPool_p->m_next_p = curPoolInfo->m_rptsInPool_p;
    curPoolInfo->m_rptsInPool_p = rptInPool_p;

    if (trie_global->use_reserved_abs_for_160b_trig)
    {
        
    }
    else
    {
        kaps_jr1_array_set_bit(curPoolInfo->m_rptIdsFree, chosenRptId, 0);
    }

    curPoolInfo->m_curNumRptEntries++;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_pool_mgr_pvt_remove_rpt_node(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_info * curPoolInfo,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t oldRptId)
{
    kaps_jr1_rpts_in_pool *prevRptInPool_p = NULL;
    kaps_jr1_rpts_in_pool *curRptInPool_p = NULL;
    int32_t found;

    found = 0;
    curRptInPool_p = curPoolInfo->m_rptsInPool_p;
    while (curRptInPool_p != NULL)
    {
        if (curRptInPool_p->m_rptTrieNode_p == rptTrieNode_p)
        {
            found = 1;
            break;
        }
        prevRptInPool_p = curRptInPool_p;
        curRptInPool_p = curRptInPool_p->m_next_p;
    }

    if (!found)
        return NLMERR_FAIL;

    if (prevRptInPool_p)
    {
        prevRptInPool_p->m_next_p = curRptInPool_p->m_next_p;
    }
    else
    {
        curPoolInfo->m_rptsInPool_p = curRptInPool_p->m_next_p;
    }
    kaps_jr1_nlm_allocator_free(poolMgr->m_alloc_p, curRptInPool_p);

    kaps_jr1_array_set_bit(curPoolInfo->m_rptIdsFree, oldRptId, 1);
    curPoolInfo->m_curNumRptEntries--;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_pool_mgr_pvt_free_rpts_in_pool(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_info * curPoolInfo)
{
    kaps_jr1_rpts_in_pool *curRptInPool_p, *delRptInPool_p;

    curRptInPool_p = curPoolInfo->m_rptsInPool_p;
    while (curRptInPool_p != NULL)
    {
        delRptInPool_p = curRptInPool_p;
        curRptInPool_p = curRptInPool_p->m_next_p;
        kaps_jr1_nlm_allocator_free(poolMgr->m_alloc_p, delRptInPool_p);
    }
    curPoolInfo->m_rptsInPool_p = NULL;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_pool_mgr_pvt_alloc_new_ipt_pool(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_info * newPoolInfo,
    uint32_t newPoolIndex,
    struct kaps_jr1_ab_info *ab,
    uint32_t ab_width_1,
    uint8_t is_warmboot,
    enum kaps_jr1_dba_trigger_compression_mode mode,
    NlmReasonCode * o_reason)
{
    enum kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_db *db = poolMgr->fibTblMgr->m_curFibTbl->m_db;
    uint32_t i, j;
    struct kaps_jr1_ab_info *cur_dup_ab;
    struct kaps_jr1_ab_info *ab_iter;
    NlmErrNum_t errNum;

    if (is_warmboot)
    {
        status = kaps_jr1_it_mgr_wb_alloc(poolMgr->m_itMgr, db, DECISION_TRIE_0, newPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE,
                                      ab);

        cur_dup_ab = ab->dup_ab;
        while (status == KAPS_JR1_OK && cur_dup_ab)
        {
            status =
                kaps_jr1_it_mgr_wb_alloc(poolMgr->m_itMgr, db, DECISION_TRIE_0, newPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE,
                                     cur_dup_ab);
            cur_dup_ab = cur_dup_ab->dup_ab;
        }

    }
    else
    {
        status = kaps_jr1_it_mgr_alloc(poolMgr->m_itMgr, db, DECISION_TRIE_0, IT_INSTANCE_0,
                                   newPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE, ab);

        cur_dup_ab = ab->dup_ab;
        while (status == KAPS_JR1_OK && cur_dup_ab)
        {
            status = kaps_jr1_it_mgr_alloc(poolMgr->m_itMgr, db, DECISION_TRIE_0, IT_INSTANCE_1,
                                       newPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE, cur_dup_ab);

            if (status != KAPS_JR1_OK)
            {
                ab_iter = ab;
                while (ab_iter && ab_iter != cur_dup_ab)
                {
                    kaps_jr1_it_mgr_free(poolMgr->m_itMgr, db, ab_iter, newPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE,
                                     DECISION_TRIE_0);
                    ab_iter = ab_iter->dup_ab;
                }
                break;
            }
            cur_dup_ab = cur_dup_ab->dup_ab;
        }
    }

    if (status != KAPS_JR1_OK)
    {
        *o_reason = NLMRSC_IT_ALLOC_FAILED;
        return NLMERR_FAIL;
    }

    newPoolInfo->m_dba_mgr = kaps_jr1_ipm_init(poolMgr->fibTblMgr, newPoolIndex,
                                           newPoolInfo->m_maxNumDbaEntries, ab, ab_width_1, mode, o_reason);

    if (!newPoolInfo->m_dba_mgr)
    {
        kaps_jr1_it_mgr_free(poolMgr->m_itMgr, db, ab, newPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE, DECISION_TRIE_0);

        ab_iter = ab->dup_ab;
        while (ab_iter)
        {
            kaps_jr1_it_mgr_free(poolMgr->m_itMgr, db, ab_iter, newPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE, DECISION_TRIE_0);
            ab_iter = ab_iter->dup_ab;
        }

        if (*o_reason != NLMRSC_LOW_MEMORY)
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;

        return NLMERR_FAIL;
    }

    for (i = 0; i < MAX_NUM_RPT_ENTRIES_IN_POOL; ++i)
    {
        errNum = kaps_jr1_ipm_init_rpt(newPoolInfo->m_dba_mgr, i, o_reason);

        if (errNum != NLMERR_OK)
        {
            for (j = 0; j < i; ++j)
            {
                kaps_jr1_ipm_free_rpt(newPoolInfo->m_dba_mgr, j);
            }

            kaps_jr1_ipm_destroy(poolMgr->m_alloc_p, newPoolInfo->m_dba_mgr);
            newPoolInfo->m_dba_mgr = NULL;
            return errNum;
        }
    }

    if (!is_warmboot)
    {
        if (newPoolIndex == poolMgr->m_numActiveIptPools)
            poolMgr->m_numActiveIptPools++;
    }

    return NLMERR_OK;

}

int
kaps_jr1_pool_mgr_get_free_pool_id(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_type poolType)
{
    uint32_t i;
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t numActivePools;

    if (poolType == KAPS_JR1_IPT_POOL)
    {
        curPoolInfo = poolMgr->m_ipmPoolInfo;
        numActivePools = poolMgr->m_numActiveIptPools;
    }
    else
    {
        curPoolInfo = poolMgr->m_apmPoolInfo;
        numActivePools = poolMgr->m_numActiveAptPools;
    }

    for (i = 0; i < numActivePools; ++i)
    {
        if (!curPoolInfo->m_isInUse)
            return i;

        curPoolInfo++;
    }

    return numActivePools;
}

void
kaps_jr1_pool_mgr_print_lpm_ab_details(
    struct NlmFibStats *fib_stats)
{
    uint32_t i, numABsProcessed;

    numABsProcessed = 0;
    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {

        if (fib_stats->isABUsed[i])
        {

            kaps_jr1_printf("\t\t\t");

            kaps_jr1_printf("AB - %3d, N_Trig = %4d, N_Cols = %3d, Width = ", i, fib_stats->numTriggersInAB[i],
                        fib_stats->numColsInAB[i]);

            if (fib_stats->widthOfAB[i] == KAPS_JR1_NORMAL_80)
            {
                kaps_jr1_printf("  80b, ");
            }
            else if (fib_stats->widthOfAB[i] == KAPS_JR1_NORMAL_160)
            {
                kaps_jr1_printf(" 160b, ");
            }

            if (fib_stats->isIPTAB[i])
            {
                kaps_jr1_printf("IPT");
            }
            else
            {
                kaps_jr1_printf("APT");
            }

            numABsProcessed++;
            kaps_jr1_printf("\n");
        }
    }
    kaps_jr1_printf("\n");
}

static NlmErrNum_t
kaps_jr1_pool_mgr_pvt_alloc_new_pool(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_type poolType,
    struct kaps_jr1_ab_info *ab,
    uint32_t numEntriesRequired,
    uint32_t ab_width_1,
    uint32_t * newPoolIndex_p,
    uint8_t is_warmboot,
    uint32_t start_offset,
    NlmReasonCode * o_reason)
{
    kaps_jr1_pool_info *newPoolInfo;
    struct kaps_jr1_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    uint32_t i, poolId, iter, columns;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t offset = 0;
    uint32_t ab_base_addr = 0, dup_ab_base_addr = 0;
    struct kaps_jr1_ab_info *cur_dup_ab;
    struct kaps_jr1_db *db = poolMgr->fibTblMgr->m_curFibTbl->m_db;
    struct kaps_jr1_ad_db *ad_db = NULL;
    uint32_t rpb_id = db->rpb_id;

    if (db->parent)
        db = db->parent;

    ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;
    (void) ad_db;

    
    if (poolType == KAPS_JR1_IPT_POOL)
    {
        if (is_warmboot)
            poolId = *newPoolIndex_p;
        else
            poolId = kaps_jr1_pool_mgr_get_free_pool_id(poolMgr, poolType);

        newPoolInfo = &poolMgr->m_ipmPoolInfo[poolId];
    }
    else
    {
        if (is_warmboot)
            poolId = *newPoolIndex_p;
        else
            poolId = kaps_jr1_pool_mgr_get_free_pool_id(poolMgr, poolType);

        newPoolInfo = &poolMgr->m_apmPoolInfo[poolId];
    }

    columns = 1;
 
    if (is_warmboot)
    {
        ab_base_addr = ab->base_addr;
        if (ab->dup_ab)
            dup_ab_base_addr = ab->dup_ab->base_addr;
    }

    for (iter = 0; iter < columns; iter++)
    {
        uint32_t pool_ix;
        enum kaps_jr1_dba_trigger_compression_mode mode = columns - 1;

        kaps_jr1_assert(newPoolInfo->m_dba_mgr == NULL, "Unable to find an empty pool\n");
        pool_ix = poolId;

        if ((start_offset == -1) || (offset == start_offset))
        {
            *newPoolIndex_p = poolId;
            start_offset = 0xFF;
        }

        newPoolInfo->m_poolType = poolType;
        newPoolInfo->m_width_1 = ab_width_1;


        {
            if (ab_width_1 == 160)
            {
                newPoolInfo->m_maxNumDbaEntries = device->hw_res->num_rows_in_rpb[rpb_id];
            }
            else
            {
                
                kaps_jr1_assert(0, "Incorrect width specified for AB \n");
                *o_reason = NLMRSC_INTERNAL_ERROR;
                return NLMERR_FAIL;
            }

        }

        cur_dup_ab = ab->dup_ab;
        while (cur_dup_ab)
        {
            cur_dup_ab->conf = ab->conf;
            cur_dup_ab = cur_dup_ab->dup_ab;
        }

        newPoolInfo->m_curNumDbaEntries = 0;
        newPoolInfo->m_maxAllowedNumDbaEntries = newPoolInfo->m_maxNumDbaEntries;

        if (newPoolInfo->m_maxAllowedNumDbaEntries < numEntriesRequired)
        {
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;
            return NLMERR_FAIL;
        }

        for (i = 0; i < MAX_NUM_RPT_ENTRIES_IN_POOL / KAPS_JR1_BITS_IN_BYTE; ++i)
        {
            newPoolInfo->m_rptIdsFree[i] = 0xFF;
        }
        newPoolInfo->m_curNumRptEntries = 0;
        newPoolInfo->m_rptsInPool_p = NULL;

        if (poolType == KAPS_JR1_IPT_POOL)
        {
            errNum = kaps_jr1_pool_mgr_pvt_alloc_new_ipt_pool(poolMgr, newPoolInfo, pool_ix,
                                                          ab, ab_width_1, is_warmboot, mode, o_reason);
        }

        if (errNum != NLMERR_OK)
        {
            if (*o_reason == NLMRSC_LOW_MEMORY)
                return errNum;
            else
                break;
        }

        if (iter == 0)
        {
            ab_base_addr = ab->base_addr;
            ab->ab_init_done = 1;
            if (ab->dup_ab)
            {
                dup_ab_base_addr = ab->dup_ab->base_addr;
                ab->dup_ab->ab_init_done = 1;
            }
        }

        newPoolInfo->m_isInUse = 1;
        newPoolInfo->m_dba_mgr->mode = mode;
        newPoolInfo->m_dba_mgr->m_start_offset_1 = offset;
        newPoolInfo->m_dba_mgr->m_num_bytes = newPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE;
        offset += newPoolInfo->m_width_1;

        
        poolId = kaps_jr1_pool_mgr_get_free_pool_id(poolMgr, poolType);
        if (poolType == KAPS_JR1_IPT_POOL)
        {
            newPoolInfo = &poolMgr->m_ipmPoolInfo[poolId];
        }
        else
        {
            newPoolInfo = &poolMgr->m_apmPoolInfo[poolId];
        }
    }

    ab->base_addr = ab_base_addr;
    if (ab->dup_ab)
        ab->dup_ab->base_addr = dup_ab_base_addr;

    
    if (errNum == NLMERR_OK)
    {

        if (poolMgr->m_dbaBlkStatus[ab->ab_num] != BLK_ALLOCATED)
        {
            poolMgr->m_dbaBlkStatus[ab->ab_num] = BLK_ALLOCATED;
            poolMgr->m_numFreeAB--;
        }
    }

    return errNum;
}

static NlmErrNum_t
kaps_jr1_pool_mgr_pvt_find_rpt_id_for_single_pool(
    kaps_jr1_pool_info * curPoolInfo,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t * found,
    uint32_t * chosenRptId)
{
    int32_t i, rptIdIter;
    kaps_jr1_trie_global *trie_global = rptTrieNode_p->m_trie_p->m_trie_global;
    kaps_jr1_rpts_in_pool *rptInPool;

    *found = 0;

    if (trie_global->use_reserved_abs_for_160b_trig)
    {

        kaps_jr1_trie_obtain_rpt_uuid(rptTrieNode_p);

        if (rptTrieNode_p->m_rpt_uuid == NLM_INVALID_RPT_UUID)
        {
            *found = 0;
            return NLMERR_OK;
        }

        rptInPool = curPoolInfo->m_rptsInPool_p;
        while (rptInPool)
        {
            if (rptInPool->m_rptTrieNode_p->m_rpt_uuid % 256 == rptTrieNode_p->m_rpt_uuid % 256)
            {
                *found = 0;
                return NLMERR_OK;
            }
            rptInPool = rptInPool->m_next_p;
        }

        *found = 1;
        *chosenRptId = rptTrieNode_p->m_rpt_uuid % 256;

        return NLMERR_OK;
    }

    for (rptIdIter = 0; rptIdIter < MAX_NUM_RPT_ENTRIES_IN_POOL / KAPS_JR1_BITS_IN_BYTE; ++rptIdIter)
    {
        if (curPoolInfo->m_rptIdsFree[rptIdIter])
        {
            for (i = 0; i < KAPS_JR1_BITS_IN_BYTE; ++i)
            {
                if (kaps_jr1_array_check_bit(&curPoolInfo->m_rptIdsFree[rptIdIter], i))
                {
                    *chosenRptId = (rptIdIter * KAPS_JR1_BITS_IN_BYTE) + i;

                    *found = 1;
                    break;
                }
            }
        }

        if (*found)
            break;
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_pool_mgr_pvt_find_one_pool(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_type poolType,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t numEntriesRequired,
    uint32_t ab_width_1,
    uint32_t lockedWidthPoolOnly,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t poolIter = 0;
    kaps_jr1_pool_info *curPoolInfo, *chosenPoolInfo;
    int32_t i = 0;
    struct kaps_jr1_ab_info *free_blk = NULL;
    uint32_t numActivePools;
    uint32_t chosenPoolIndex, chosenRptId;
    uint32_t allocateNewPool;
    uint32_t maxNumFreeSlotsInAnyPool, curNumFreeSlots;
    uint32_t allowMultipleRptInPool;
    uint32_t found;

    poolMgr->m_are_pools_updated = 1;

    if (numEntriesRequired == 0)
        numEntriesRequired = 1;

    if (poolType == KAPS_JR1_IPT_POOL)
    {
        curPoolInfo = &poolMgr->m_ipmPoolInfo[0];
        numActivePools = poolMgr->m_numActiveIptPools;
    }
    else
    {
        curPoolInfo = &poolMgr->m_apmPoolInfo[0];
        numActivePools = poolMgr->m_numActiveAptPools;
    }

    maxNumFreeSlotsInAnyPool = 0;
    chosenPoolInfo = NULL;
    chosenPoolIndex = KAPS_JR1_INVALID_POOL_INDEX;
    chosenRptId = -1;

    allowMultipleRptInPool = 1;

    if (allowMultipleRptInPool)
    {
        
        for (poolIter = 0; poolIter < numActivePools; ++poolIter, ++curPoolInfo)
        {
            if (!curPoolInfo->m_isInUse)
                continue;

            if (curPoolInfo->m_isReservedPool)
                continue;

            if (curPoolInfo->m_width_1 != ab_width_1)
                continue;

            if (curPoolInfo->m_curNumDbaEntries + numEntriesRequired > curPoolInfo->m_maxAllowedNumDbaEntries)
                continue;

            curNumFreeSlots = curPoolInfo->m_maxAllowedNumDbaEntries - curPoolInfo->m_curNumDbaEntries;
            if (curNumFreeSlots < maxNumFreeSlotsInAnyPool)
                continue;

            if (curPoolInfo->m_curNumRptEntries >= MAX_NUM_RPT_ENTRIES_IN_POOL)
                continue;

            
            if (lockedWidthPoolOnly && !curPoolInfo->m_isWidthLocked)
                continue;

            found = 0;
            kaps_jr1_pool_mgr_pvt_find_rpt_id_for_single_pool(curPoolInfo, rptTrieNode_p, &found, &chosenRptId);

            if (found)
            {
                chosenPoolIndex = poolIter;

                if (poolType == KAPS_JR1_IPT_POOL)
                    chosenPoolInfo = &poolMgr->m_ipmPoolInfo[chosenPoolIndex];
                else
                    chosenPoolInfo = &poolMgr->m_apmPoolInfo[chosenPoolIndex];

                maxNumFreeSlotsInAnyPool = curNumFreeSlots;

                break;
            }

        }
    }

    allocateNewPool = 0;
    if (chosenPoolIndex == KAPS_JR1_INVALID_POOL_INDEX)
    {
        allocateNewPool = 1;
    }

    if (allocateNewPool)
    {
        
        if (poolMgr->m_numFreeAB < 1)
        {
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;
            return NLMERR_FAIL;
        }

        free_blk = NULL;
        for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
        {
            if (poolMgr->m_dbaBlkStatus[i] == BLK_FREE)
            {
                free_blk = poolMgr->abs_allocated[i];
                break;
            }
        }

        if (free_blk)
        {

            errNum = kaps_jr1_pool_mgr_pvt_alloc_new_pool(poolMgr, poolType, free_blk, numEntriesRequired,
                                                      ab_width_1, &chosenPoolIndex, 0, -1, o_reason);

            if (errNum == NLMERR_OK)
            {
                curPoolInfo = &poolMgr->m_ipmPoolInfo[chosenPoolIndex];
                found = 0;
                kaps_jr1_pool_mgr_pvt_find_rpt_id_for_single_pool(curPoolInfo, rptTrieNode_p, &found, &chosenRptId);

                if (!found)
                {
                    *o_reason = NLMRSC_DBA_ALLOC_FAILED;
                    return NLMERR_FAIL;
                }

            }
            else
            {
                return errNum;
            }
        }
        else
        {
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;
            return NLMERR_FAIL;
        }

        if (poolType == KAPS_JR1_IPT_POOL)
            chosenPoolInfo = &poolMgr->m_ipmPoolInfo[chosenPoolIndex];
        else
            chosenPoolInfo = &poolMgr->m_apmPoolInfo[chosenPoolIndex];

    }

    errNum = kaps_jr1_pool_mgr_pvt_insert_rpt_node(poolMgr, rptTrieNode_p, chosenPoolInfo, chosenRptId, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    rptTrieNode_p->m_poolId = KAPS_JR1_CONSTRUCT_POOL_ID(chosenPoolIndex);

    rptTrieNode_p->m_rptId = chosenRptId;

    poolMgr->m_curNumRptEntriesInAllPools++;

    return errNum;

}

static NlmErrNum_t
kaps_jr1_pool_mgr_pvt_assign_pool_internal(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t numIptEntriesRequired,
    uint32_t numAptEntriesRequired,
    uint32_t numPools,
    uint32_t ab_width_1,
    uint32_t lockedWidthPoolOnly,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;

    if (numIptEntriesRequired)
    {
        errNum = kaps_jr1_pool_mgr_pvt_find_one_pool(poolMgr, KAPS_JR1_IPT_POOL, rptTrieNode_p,
                                                 numIptEntriesRequired, ab_width_1, lockedWidthPoolOnly, o_reason);
    }

    return errNum;
}

kaps_jr1_pool_mgr *
kaps_jr1_pool_mgr_create(
    kaps_jr1_fib_tbl_mgr * fibTblMgr_p,
    kaps_jr1_nlm_allocator * alloc_p,
    uint32_t is_cascaded,
    struct kaps_jr1_c_list * ab_list)
{
    uint32_t i = 0, ab_num;
    kaps_jr1_pool_mgr *poolMgr;
    struct kaps_jr1_c_list_iter ab_it;
    struct kaps_jr1_list_node *ab_el;

    poolMgr = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_pool_mgr));
    if (!poolMgr)
        return NULL;


    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {
        poolMgr->m_ipmPoolInfo[i].m_poolIndex = i;
        poolMgr->m_apmPoolInfo[i].m_poolIndex = i;
    }

    poolMgr->m_alloc_p = alloc_p;
    poolMgr->m_are_pools_updated = 1;

    
    if (ab_list)
    {
        kaps_jr1_c_list_iter_init(ab_list, &ab_it);
        while ((ab_el = kaps_jr1_c_list_iter_next(&ab_it)) != NULL)
        {
            struct kaps_jr1_ab_info *ab = KAPS_JR1_ABLIST_TO_ABINFO(ab_el);
            i = ab->ab_num % KAPS_JR1_MAX_NUM_POOLS;
            poolMgr->abs_allocated[i] = ab;
        }
    }

    for (ab_num = 0; ab_num < KAPS_JR1_MAX_NUM_POOLS; ++ab_num)
        poolMgr->m_dbaBlkStatus[ab_num] = BLK_UNAVAILABLE;

    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {
        if (poolMgr->abs_allocated[i] != NULL)
        {
            ab_num = poolMgr->abs_allocated[i]->ab_num;
            poolMgr->m_dbaBlkStatus[ab_num] = BLK_FREE;
            poolMgr->m_numFreeAB++;
            poolMgr->m_totalNumAB++;
        }
    }

    poolMgr->fibTblMgr = fibTblMgr_p;
    poolMgr->m_itMgr = (struct it_mgr *) fibTblMgr_p->m_devMgr_p->hw_res->it_mgr;
    if (is_cascaded)
        poolMgr->m_itMgr = (struct it_mgr *) fibTblMgr_p->m_devMgr_p->other_core->hw_res->it_mgr;

    return poolMgr;
}

NlmErrNum_t
kaps_jr1_pool_mgr_assign_pool_to_rpt_entry(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t numIptEntriesRequired,
    uint32_t numAptEntriesRequired,
    uint32_t numPools,
    uint32_t ab_width_1,
    int32_t * was_dba_dynamic_alloced,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;

    *was_dba_dynamic_alloced = 0;

    errNum = kaps_jr1_pool_mgr_pvt_assign_pool_internal(poolMgr, rptTrieNode_p, numIptEntriesRequired,
                                                    numAptEntriesRequired, numPools, ab_width_1, 0, o_reason);

    return errNum;
}

int
kaps_jr1_pool_mgr_check_if_ab_is_empty(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_info * pivotPoolInfo)
{
    kaps_jr1_pool_info *iterPoolInfo = NULL;
    uint32_t i, maxNumPools = 0;

    if (pivotPoolInfo->m_poolType == KAPS_JR1_IPT_POOL)
    {
        iterPoolInfo = poolMgr->m_ipmPoolInfo;
        maxNumPools = poolMgr->m_numActiveIptPools;
    }

    for (i = 0; i < maxNumPools; ++i)
    {
        if (!iterPoolInfo->m_isInUse)
        {
            iterPoolInfo++;
            continue;
        }

        if (pivotPoolInfo->m_dba_mgr->m_ab_info->ab_num == iterPoolInfo->m_dba_mgr->m_ab_info->ab_num)
        {
            if (iterPoolInfo->m_curNumRptEntries > 0)
                return 0;
        }
        ++iterPoolInfo;
    }

    return 1;
}

NlmErrNum_t
kaps_jr1_pool_mgr_release_ab(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_info * pivotPoolInfo)
{
    kaps_jr1_pool_info *iterPoolInfo = NULL;
    uint32_t i, maxNumPools = 0;
    struct kaps_jr1_ab_info *ab, *cur_dup_ab;
    enum kaps_jr1_entry_status type = DECISION_TRIE_0;
    uint32_t pool_width_8 = pivotPoolInfo->m_width_1 / KAPS_JR1_BITS_IN_BYTE;
    struct kaps_jr1_db *db = poolMgr->fibTblMgr->m_curFibTbl->m_db;

    if (db->parent)
        db = db->parent;

    ab = pivotPoolInfo->m_dba_mgr->m_ab_info;

    if (pivotPoolInfo->m_poolType == KAPS_JR1_IPT_POOL)
    {
        iterPoolInfo = poolMgr->m_ipmPoolInfo;
        maxNumPools = poolMgr->m_numActiveIptPools;
        type = DECISION_TRIE_0;
    }

    for (i = 0; i < maxNumPools; ++i)
    {
        if (!iterPoolInfo->m_isInUse)
        {
            iterPoolInfo++;
            continue;
        }

        if (ab->ab_num != iterPoolInfo->m_dba_mgr->m_ab_info->ab_num)
        {
            ++iterPoolInfo;
            continue;
        }

        if (type == DECISION_TRIE_0)
        {
            kaps_jr1_ipm_destroy(poolMgr->m_alloc_p, iterPoolInfo->m_dba_mgr);
        }

        iterPoolInfo->m_isInUse = 0;
        iterPoolInfo->m_dba_mgr = NULL;

        ++iterPoolInfo;
    }

    cur_dup_ab = ab;
    while (cur_dup_ab)
    {
        kaps_jr1_it_mgr_free(poolMgr->m_itMgr, db, cur_dup_ab, pool_width_8, type);

        cur_dup_ab->ab_init_done = 0;
        cur_dup_ab = cur_dup_ab->dup_ab;
    }

    poolMgr->m_dbaBlkStatus[ab->ab_num] = BLK_FREE;
    poolMgr->m_numFreeAB++;
    poolMgr->m_numABsReleased++;
    ab->ab_init_done = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_pool_mgr_release_pool_for_rpt_entry(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t oldPoolId,
    uint32_t oldRptId)
{
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t oldIptPoolIndex;
    kaps_jr1_pool_info *curPoolInfo;

    poolMgr->m_are_pools_updated = 1;

    oldIptPoolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(oldPoolId);
    if (oldIptPoolIndex != KAPS_JR1_INVALID_POOL_INDEX)
    {
        curPoolInfo = &poolMgr->m_ipmPoolInfo[oldIptPoolIndex];
        errNum = kaps_jr1_pool_mgr_pvt_remove_rpt_node(poolMgr, curPoolInfo, rptTrieNode_p, oldRptId);
        if (errNum != NLMERR_OK)
            return errNum;

        if (curPoolInfo->m_curNumRptEntries == 0 && !curPoolInfo->m_isWidthLocked)
        {
            if (kaps_jr1_pool_mgr_check_if_ab_is_empty(poolMgr, curPoolInfo))
            {
                kaps_jr1_pool_mgr_release_ab(poolMgr, curPoolInfo);
            }
        }
    }

    poolMgr->m_curNumRptEntriesInAllPools--;

    return errNum;
}

NlmErrNum_t
kaps_jr1_pool_mgr_incr_num_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    uint32_t incr,
    kaps_jr1_pool_type poolType)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    if (incr == 0)
        return errNum;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);
    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    curPoolInfo->m_curNumDbaEntries += incr;
    if (curPoolInfo->m_curNumDbaEntries > curPoolInfo->m_maxNumDbaEntries)
        kaps_jr1_assert(0, "Too many entries in the pool \n");

    return errNum;
}

NlmErrNum_t
kaps_jr1_pool_mgr_decr_num_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    uint32_t decr,
    kaps_jr1_pool_type poolType)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    if (decr == 0)
        return errNum;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);
    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    if (curPoolInfo->m_curNumDbaEntries < decr)
        kaps_jr1_assert(0, "Number of entries in the pool has become negative \n");

    curPoolInfo->m_curNumDbaEntries -= decr;

    return errNum;
}

int
kaps_jr1_pool_mgr_is_pool_in_use(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType)
{
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);

    if (poolIndex >= poolMgr->m_numActiveIptPools)
        return 0;

    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    return curPoolInfo->m_isInUse;
}

uint32_t
kaps_jr1_pool_mgr_get_num_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType)
{
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);
    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    return curPoolInfo->m_curNumDbaEntries;
}

uint32_t
kaps_jr1_pool_mgr_get_max_allowed_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType)
{
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);
    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    return curPoolInfo->m_maxAllowedNumDbaEntries;
}

kaps_jr1_ipm *
kaps_jr1_pool_mgr_get_ipm_for_pool(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId)
{
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);
    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    return curPoolInfo->m_dba_mgr;
}

uint32_t
kaps_jr1_pool_mgr_get_max_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType)
{
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);
    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    return curPoolInfo->m_maxNumDbaEntries;
}

uint32_t
kaps_jr1_pool_mgr_get_num_active_pools(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_pool_type poolType)
{

    return poolMgr->m_numActiveIptPools;

}

uint32_t
kaps_jr1_pool_mgr_get_num_rpt_entries_in_pool(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType)
{
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);
    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    return curPoolInfo->m_curNumRptEntries;
}

kaps_jr1_trie_node *
kaps_jr1_pool_mgr_get_rpt_with_most_entries(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t poolId,
    kaps_jr1_pool_type poolType)
{
    kaps_jr1_rpts_in_pool *rptInPool_p = NULL;
    kaps_jr1_trie_node *maxRpt_p = NULL;
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t poolIndex;

    poolIndex = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(poolId);
    curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIndex];

    rptInPool_p = curPoolInfo->m_rptsInPool_p;
    while (rptInPool_p)
    {
        if (maxRpt_p == NULL)
        {
            maxRpt_p = rptInPool_p->m_rptTrieNode_p;
        }
        else
        {
            if (poolType == KAPS_JR1_IPT_POOL)
            {
                if (rptInPool_p->m_rptTrieNode_p->m_numIptEntriesInSubtrie > maxRpt_p->m_numIptEntriesInSubtrie)
                    maxRpt_p = rptInPool_p->m_rptTrieNode_p;
            }
            else
            {
                if (rptInPool_p->m_rptTrieNode_p->m_numAptEntriesInSubtrie > maxRpt_p->m_numAptEntriesInSubtrie)
                    maxRpt_p = rptInPool_p->m_rptTrieNode_p;
            }
        }
        rptInPool_p = rptInPool_p->m_next_p;
    }
    return maxRpt_p;
}

void
kaps_jr1_pool_mgr_destroy(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_nlm_allocator * alloc_p)
{
    uint32_t poolIter = 0;
    uint32_t rptId = 0;
    kaps_jr1_pool_info *curPoolInfo;

    if (poolMgr)
    {

        for (poolIter = 0; poolIter < poolMgr->m_numActiveIptPools; ++poolIter)
        {
            curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIter];

            if (!curPoolInfo->m_isInUse)
                continue;

            kaps_jr1_pool_mgr_pvt_free_rpts_in_pool(poolMgr, curPoolInfo);

            for (rptId = 0; rptId < MAX_NUM_RPT_ENTRIES_IN_POOL; ++rptId)
            {
                if (curPoolInfo->m_dba_mgr)
                    kaps_jr1_ipm_free_rpt(curPoolInfo->m_dba_mgr, rptId);
            }

            kaps_jr1_ipm_destroy(alloc_p, curPoolInfo->m_dba_mgr);
        }

        kaps_jr1_nlm_allocator_free(alloc_p, poolMgr);
    }
}

NlmErrNum_t
kaps_jr1_pool_mgr_createEntryData(
    kaps_jr1_pool_mgr * poolMgr,
    uint32_t ab_num,
    uint32_t row_nr,
    uint8_t * data,
    uint8_t * mask,
    uint8_t * is_xy,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t poolIter = 0;
    kaps_jr1_pool_info *curPoolInfo;

    for (poolIter = 0; poolIter < poolMgr->m_numActiveIptPools; ++poolIter)
    {
        curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIter];
        if (!curPoolInfo->m_isInUse)
            continue;

        if (curPoolInfo->m_dba_mgr->m_ab_info->ab_num == ab_num)
        {
            if (curPoolInfo->m_dba_mgr->mode != DBA_NO_TRIGGER_COMPRESSION)
                *is_xy = 1;
            errNum = kaps_jr1_ipm_create_entry_data(curPoolInfo->m_dba_mgr, ab_num, row_nr, data, mask, *is_xy, o_reason);
            if (errNum != NLMERR_OK)
                return errNum;
        }
    }

    return errNum;

}

void
kaps_jr1_pool_mgr_verify(
    kaps_jr1_pool_mgr * poolMgr)
{
    uint32_t poolIter = 0, totalNumIptEntries = 0;
    kaps_jr1_rpts_in_pool *rptInPool_p = NULL;
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t i, num_free_ab;
    uint32_t simple_dba_num_entries;

    for (poolIter = 0; poolIter < poolMgr->m_numActiveIptPools; ++poolIter)
    {

        curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIter];
        if (!curPoolInfo->m_isInUse)
        {
            kaps_jr1_assert(curPoolInfo->m_dba_mgr == NULL, "DBA manager should be NULL \n");
            continue;
        }

        if (!curPoolInfo->m_dba_mgr)
            kaps_jr1_assert(0, "DBA Manager should not be NULL\n");

        rptInPool_p = curPoolInfo->m_rptsInPool_p;

        totalNumIptEntries = 0;
        while (rptInPool_p)
        {
            totalNumIptEntries += rptInPool_p->m_rptTrieNode_p->m_numIptEntriesInSubtrie;

            simple_dba_num_entries = kaps_jr1_ipm_verify(curPoolInfo->m_dba_mgr, rptInPool_p->m_rptTrieNode_p->m_rptId);

            if (simple_dba_num_entries != rptInPool_p->m_rptTrieNode_p->m_numIptEntriesInSubtrie)
            {
                kaps_jr1_assert(0, "Mismatch in number of IPT entries in trie node and num entries in kaps_jr1_simple_dba");
                return;
            }

            rptInPool_p = rptInPool_p->m_next_p;
        }

        if (totalNumIptEntries != curPoolInfo->m_curNumDbaEntries)
        {
            kaps_jr1_assert(0, "Mismatch in number of IPT entries in pool \n");
            return;
        }
    }

    num_free_ab = 0;
    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {
        if (poolMgr->m_dbaBlkStatus[i] == BLK_FREE)
        {
            ++num_free_ab;
        }
    }

    if (num_free_ab != poolMgr->m_numFreeAB)
    {
        kaps_jr1_assert(0, "Incorrect number of free ABs in the pool manager \n");
        return;
    }

}

void
kaps_jr1_pool_mgr_get_dba_stats(
    kaps_jr1_pool_mgr * poolMgr,
    struct kaps_jr1_db *parent_db,
    uint32_t avg_num_pfx_per_brick,
    uint32_t cur_num_entries_in_db,
    uint32_t * numOfABsUsed,
    uint32_t * is_dba_brimmed,
    uint32_t * num_abs_released,
    uint32_t * extra_entry_estimate,
    uint32_t log_info)
{
    uint32_t i = 0, numABUsed = 0, poolIter;
    struct kaps_jr1_ab_info *ab;
    struct kaps_jr1_ab_info *cur_dup_ab;
    uint32_t num_used_ipt_ab = 0, num_free_ipt_ab = 0, num_total_ipt_ab = 0;
    uint32_t cur_used_ipt_slots = 0, cur_total_ipt_slots = 0;
    uint32_t projected_total_ipt_slots = 0, projected_free_ipt_slots = 0;
    uint32_t ipt_extra = 0;
    uint32_t totalNumFreeAB;

    for (poolIter = 0; poolIter < poolMgr->m_numActiveAptPools; ++poolIter)
    {
        if (!poolMgr->m_apmPoolInfo[poolIter].m_isInUse)
            continue;
    }
    *is_dba_brimmed = 1;

    for (poolIter = 0; poolIter < poolMgr->m_numActiveIptPools; ++poolIter)
    {
        if (!poolMgr->m_ipmPoolInfo[poolIter].m_isInUse)
            continue;

        ++num_used_ipt_ab;
        cur_used_ipt_slots += poolMgr->m_ipmPoolInfo[poolIter].m_curNumDbaEntries;
        cur_total_ipt_slots += poolMgr->m_ipmPoolInfo[poolIter].m_maxAllowedNumDbaEntries;

        if (poolMgr->m_ipmPoolInfo[poolIter].m_curNumDbaEntries
            < (poolMgr->m_ipmPoolInfo[poolIter].m_maxNumDbaEntries * 0.95))
            *is_dba_brimmed = 0;

        
    }


    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {
        if ((poolMgr->abs_allocated[i] != NULL) && (poolMgr->m_dbaBlkStatus[i] == BLK_ALLOCATED))
        {
            numABUsed++;

            ab = poolMgr->abs_allocated[i];
            cur_dup_ab = ab->dup_ab;
            while (cur_dup_ab)
            {
                numABUsed++;
                cur_dup_ab = cur_dup_ab->dup_ab;
            }
        }
        else if (poolMgr->m_dbaBlkStatus[i] == BLK_UNAVAILABLE)
        {
            continue;
        }
        else
        {
            *is_dba_brimmed = 0;
        }
    }


    *numOfABsUsed = numABUsed;


    *num_abs_released = poolMgr->m_numABsReleased;

    totalNumFreeAB = poolMgr->m_numFreeAB;


    num_free_ipt_ab = totalNumFreeAB;

    num_total_ipt_ab = num_free_ipt_ab + num_used_ipt_ab;


    if (num_used_ipt_ab) {
        projected_total_ipt_slots = (1.0 * num_total_ipt_ab * cur_total_ipt_slots) / num_used_ipt_ab;
        projected_free_ipt_slots = projected_total_ipt_slots - cur_used_ipt_slots;
    }

    if (log_info) {
        kaps_jr1_printf("\n");
        kaps_jr1_printf("DBA ESTIMATE: Num used IPT AB = %d\n", num_used_ipt_ab);
        kaps_jr1_printf("DBA ESTIMATE: Number of slots available in Used IPT Blocks = %d\n", cur_total_ipt_slots);
        kaps_jr1_printf("DBA ESTIMATE: Total number of IPT AB = %d\n", num_total_ipt_ab);
        kaps_jr1_printf("DBA ESTIMATE: Number of slots in All AB = %d\n", projected_total_ipt_slots);
        kaps_jr1_printf("DBA ESTIMATE: Number of Free IPT Slots = %d\n", projected_free_ipt_slots);
    }

    if (cur_used_ipt_slots <= 10 || num_used_ipt_ab == 0) {
        
        ipt_extra = 64 * 1024 * 1024;

        if (log_info) {
            kaps_jr1_printf("DBA ESTIMATE: cur_used_ipt_slots = %d, num_used_ipt_ab = %d \n",
                        cur_used_ipt_slots, num_used_ipt_ab);
            kaps_jr1_printf("DBA ESTIMATE: Num Pfx that can be stored due to extra triggers = %d\n\n", ipt_extra);
        }
    }
    else {
        ipt_extra = (1.0 * projected_free_ipt_slots / cur_used_ipt_slots) * cur_num_entries_in_db;

        if (log_info) {
            kaps_jr1_printf("DBA ESTIMATE: (projected_free_ipt_slots / cur_used_ipt_slots) * cur_num_entries_in_db \n");
            kaps_jr1_printf("DBA ESTIMATE: (%d / %d) * %d \n", projected_free_ipt_slots, cur_used_ipt_slots,
                        cur_num_entries_in_db);
            kaps_jr1_printf("DBA ESTIMATE: Num Pfx that can be stored due to extra triggers = %d\n\n", ipt_extra);
        }
    }


    if (extra_entry_estimate)
    {
        *extra_entry_estimate = ipt_extra;
    }
}




void
kaps_jr1_pool_mgr_cr_save_pool_info(
    kaps_jr1_pool_mgr * pool_mgr,
    uint16_t pool_ix,
    kaps_jr1_pool_type pool_type,
    struct kaps_jr1_wb_cb_functions *wb_fun)
{
    kaps_jr1_pool_info *pool_info;
    struct kaps_jr1_wb_pool_info *pool_info_wb;

    pool_info_wb = (struct kaps_jr1_wb_pool_info *) wb_fun->nv_ptr;
    wb_fun->nv_ptr = wb_fun->nv_ptr + sizeof(*pool_info_wb);
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*pool_info_wb);

    pool_info_wb->pool_type = pool_type;
    pool_info_wb->pool_ix = pool_ix;

    if (pool_type == KAPS_JR1_IPT_POOL)
    {
        pool_info = &pool_mgr->m_ipmPoolInfo[pool_ix];
    }
    else
    {
        pool_info = &pool_mgr->m_apmPoolInfo[pool_ix];
    }

    pool_info_wb->blk_num = pool_info->m_dba_mgr->m_ab_info->ab_num;
    pool_info_wb->m_width_1 = pool_info->m_width_1;
    pool_info_wb->m_start_offset_1 = pool_info->m_dba_mgr->m_start_offset_1;

}

int32_t
kaps_jr1_pool_mgr_wb_save_pool_info(
    kaps_jr1_pool_mgr * pool_mgr,
    uint16_t pool_ix,
    kaps_jr1_pool_type pool_type,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset)
{
    kaps_jr1_pool_info *pool_info;
    struct kaps_jr1_wb_pool_info pool_info_wb;
    uint32_t size;

    pool_info_wb.pool_type = pool_type;
    pool_info_wb.pool_ix = pool_ix;

    if (pool_type == KAPS_JR1_IPT_POOL)
    {
        pool_info = &pool_mgr->m_ipmPoolInfo[pool_ix];
    }
    else
    {
        pool_info = &pool_mgr->m_apmPoolInfo[pool_ix];
    }

    pool_info_wb.blk_num = pool_info->m_dba_mgr->m_ab_info->ab_num;
    pool_info_wb.m_width_1 = pool_info->m_width_1;
    pool_info_wb.m_start_offset_1 = pool_info->m_dba_mgr->m_start_offset_1;
    pool_info_wb.m_isReservedPool = pool_info->m_isReservedPool;
    pool_info_wb.m_isWidthLocked = pool_info->m_isWidthLocked;

    size = sizeof(struct kaps_jr1_wb_pool_info);
    *nv_offset += size;

    return write_fn(handle, (uint8_t *) & pool_info_wb, size, *nv_offset - size);
}

void
kaps_jr1_pool_mgr_cr_save_pools(
    kaps_jr1_pool_mgr * pool_mgr,
    struct kaps_jr1_wb_cb_functions *wb_fun)
{
    uint32_t pool_ix;
    uint32_t final_nv_offset;
    uint8_t *final_nv_ptr;

    final_nv_offset = *wb_fun->nv_offset + (2 * KAPS_JR1_MAX_NUM_POOLS * sizeof(struct kaps_jr1_wb_pool_info));
    final_nv_ptr = wb_fun->nv_ptr + (2 * KAPS_JR1_MAX_NUM_POOLS * sizeof(struct kaps_jr1_wb_pool_info));

    
    for (pool_ix = 0; pool_ix < KAPS_JR1_MAX_NUM_POOLS; pool_ix++)
    {
        if (pool_mgr->m_ipmPoolInfo[pool_ix].m_dba_mgr)
        {
            kaps_jr1_pool_mgr_cr_save_pool_info(pool_mgr, pool_ix, KAPS_JR1_IPT_POOL, wb_fun);
        }
    }

    wb_fun->nv_ptr = final_nv_ptr;
    *wb_fun->nv_offset = final_nv_offset;
}

NlmErrNum_t
kaps_jr1_pool_mgr_wb_save_pools(
    kaps_jr1_pool_mgr * pool_mgr,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset)
{
    uint32_t pool_ix;

    
    for (pool_ix = 0; pool_ix < KAPS_JR1_MAX_NUM_POOLS; pool_ix++)
    {
        if (pool_mgr->m_ipmPoolInfo[pool_ix].m_dba_mgr)
        {
            if (0 != kaps_jr1_pool_mgr_wb_save_pool_info(pool_mgr, pool_ix, KAPS_JR1_IPT_POOL, write_fn, handle, nv_offset))
                return NLMERR_FAIL;
        }
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_pool_mgr_wb_restore_pool_info(
    kaps_jr1_pool_mgr * pool_mgr,
    struct kaps_jr1_wb_pool_info * pool_info,
    uint32_t * reserved_pool_iter)
{
    NlmErrNum_t err_num = NLMERR_OK;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    uint8_t is_warmboot = 1;
    struct kaps_jr1_ab_info *ab = pool_mgr->abs_allocated[pool_info->blk_num];
    uint32_t blkWidthInBits = pool_info->m_width_1;

    err_num = kaps_jr1_pool_mgr_pvt_alloc_new_pool(pool_mgr, pool_info->pool_type, ab, 0,
                                               blkWidthInBits, (uint32_t *) & pool_info->pool_ix,
                                               is_warmboot, pool_info->m_start_offset_1, &reason);
    if (pool_info->pool_type == KAPS_JR1_IPT_POOL)
    {
        if (pool_mgr->m_numActiveIptPools <= pool_info->pool_ix)
        {
            pool_mgr->m_numActiveIptPools = pool_info->pool_ix + 1;
        }

        if (pool_info->m_isReservedPool)
        {
            pool_mgr->m_ipmPoolInfo[pool_info->pool_ix].m_isReservedPool = 1;
            pool_mgr->fibTblMgr->m_trieGlobal->poolIdsForReserved160bTrig[*reserved_pool_iter] = pool_info->pool_ix;
            (*reserved_pool_iter)++;
        }

        pool_mgr->m_ipmPoolInfo[pool_info->pool_ix].m_isWidthLocked = pool_info->m_isWidthLocked;
    }

    return err_num;
}

NlmErrNum_t
kaps_jr1_pool_mgr_wb_restore_pools(
    kaps_jr1_pool_mgr * pool_mgr,
    kaps_jr1_device_issu_read_fn read_fn,
    void *handle,
    uint32_t * nv_offset)
{
    struct kaps_jr1_wb_pool_info pool_info;
    uint32_t num_active_pools, iter;
    uint32_t reserved_pool_iter = 0;

    num_active_pools = pool_mgr->m_numActiveIptPools + pool_mgr->m_numActiveAptPools;

    
    for (iter = 0; iter < num_active_pools; iter++)
    {
        if (0 != read_fn(handle, (uint8_t *) & pool_info, sizeof(pool_info), *nv_offset))
            return NLMERR_FAIL;
        *nv_offset += sizeof(pool_info);

        kaps_jr1_pool_mgr_wb_restore_pool_info(pool_mgr, &pool_info, &reserved_pool_iter);
    }

    return NLMERR_OK;
}

void
kaps_jr1_pool_mgr_cr_restore_pools(
    kaps_jr1_pool_mgr * pool_mgr,
    struct kaps_jr1_wb_cb_functions *wb_fun)
{
    uint32_t final_nv_offset;
    uint8_t *final_nv_ptr;
    struct kaps_jr1_wb_pool_info *pool_info_wb;
    uint32_t num_active_pools, iter;
    uint32_t reserved_pool_iter = 0;

    final_nv_offset = *wb_fun->nv_offset + (2 * KAPS_JR1_MAX_NUM_POOLS * sizeof(struct kaps_jr1_wb_pool_info));
    final_nv_ptr = wb_fun->nv_ptr + (2 * KAPS_JR1_MAX_NUM_POOLS * sizeof(struct kaps_jr1_wb_pool_info));

    num_active_pools = pool_mgr->m_numActiveIptPools + pool_mgr->m_numActiveAptPools;

    
    for (iter = 0; iter < num_active_pools; iter++)
    {
        pool_info_wb = (struct kaps_jr1_wb_pool_info *) wb_fun->nv_ptr;
        wb_fun->nv_ptr = wb_fun->nv_ptr + sizeof(*pool_info_wb);
        kaps_jr1_pool_mgr_wb_restore_pool_info(pool_mgr, pool_info_wb, &reserved_pool_iter);
    }

    wb_fun->nv_ptr = final_nv_ptr;
    *wb_fun->nv_offset = final_nv_offset;
}


NlmErrNum_t
kaps_jr1_pool_mgr_wb_add_rpt_entry_to_pool(
    kaps_jr1_pool_mgr * pool_mgr,
    kaps_jr1_trie_node * rpt_node)
{
    kaps_jr1_pool_info *pool_info;
    NlmErrNum_t err_num = NLMERR_OK;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    uint32_t pool_ix;

    pool_ix = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(rpt_node->m_poolId);
    if (pool_ix != KAPS_JR1_INVALID_POOL_INDEX)
    {
        pool_info = &pool_mgr->m_ipmPoolInfo[pool_ix];

        err_num = kaps_jr1_pool_mgr_pvt_insert_rpt_node(pool_mgr, rpt_node, pool_info, rpt_node->m_rptId, &reason);
        if (err_num != NLMERR_OK)
            return err_num;
    }

    return err_num;
}

void
kaps_jr1_pool_mgr_get_pool_stats(
    kaps_jr1_pool_mgr * poolMgr,
    struct NlmFibStats *fibStats)
{
    uint32_t poolIter;
    struct kaps_jr1_ab_info *ab;
    kaps_jr1_pool_info *curPoolInfo;
    uint32_t i;
    uint32_t num_copies = 0;

    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {
        if (poolMgr->m_dbaBlkStatus[i] == BLK_FREE || poolMgr->m_dbaBlkStatus[i] == BLK_ALLOCATED)
        {
            fibStats->rxcTotalNumABAssigned++;
        }
    }

    for (poolIter = 0; poolIter < poolMgr->m_numActiveIptPools; ++poolIter)
    {

        curPoolInfo = &poolMgr->m_ipmPoolInfo[poolIter];

        if (!curPoolInfo->m_isInUse)
            continue;

        ab = curPoolInfo->m_dba_mgr->m_ab_info;
        fibStats->numTriggersInAB[ab->ab_num] += curPoolInfo->m_curNumDbaEntries;       

        fibStats->numColsInAB[ab->ab_num]++;

        num_copies = 0;
        while (ab)
        {
            fibStats->isABUsed[ab->ab_num] = 1;
            fibStats->isIPTAB[ab->ab_num] = 1;
            fibStats->widthOfAB[ab->ab_num] = ab->conf;

            num_copies++;

            ab = ab->dup_ab;
        }

    }

    fibStats->rxcTotalNumABAssigned *= num_copies;

    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {
        if (fibStats->isABUsed[i])
        {
            fibStats->numTotalIptAB++;

            if (fibStats->widthOfAB[i] == KAPS_JR1_NORMAL_80)
            {
                fibStats->numTotal80bIptAB++;
            }
            else if (fibStats->widthOfAB[i] == KAPS_JR1_NORMAL_160)
            {
                fibStats->numTotal160bIptAB++;
            }
        }
    }

    if (num_copies)
        fibStats->numDupIptAB = fibStats->numTotalIptAB - (fibStats->numTotalIptAB / num_copies);

    for (poolIter = 0; poolIter < poolMgr->m_numActiveAptPools; ++poolIter)
    {

        curPoolInfo = &poolMgr->m_apmPoolInfo[poolIter];

        if (!curPoolInfo->m_isInUse)
            continue;

        ab = curPoolInfo->m_dba_mgr->m_ab_info;

        
        fibStats->numTriggersInAB[ab->ab_num] += curPoolInfo->m_curNumDbaEntries;       

        fibStats->numColsInAB[ab->ab_num]++;

        while (ab)
        {
            fibStats->isABUsed[ab->ab_num] = 1;
            fibStats->isIPTAB[ab->ab_num] = 0;
            fibStats->widthOfAB[ab->ab_num] = ab->conf;

            ab = ab->dup_ab;
        }

    }

    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {
        
        if (fibStats->isABUsed[i] && !fibStats->isIPTAB[i])
        {
            fibStats->numTotalAptAB++;

            if (fibStats->widthOfAB[i] == KAPS_JR1_NORMAL_80)
            {
                fibStats->numTotal80bAptAB++;
            }
            else if (fibStats->widthOfAB[i] == KAPS_JR1_NORMAL_160)
            {
                fibStats->numTotal160bAptAB++;
            }
        }
    }

    if (num_copies)
        fibStats->numDupAptAB = fibStats->numTotalAptAB - (fibStats->numTotalAptAB / num_copies);
}
