

#include "kaps_jr1_fib_trie.h"
#include "kaps_jr1_fib_cmn_prefix.h"
#include "kaps_jr1_fib_cmn_nstblrqt.h"
#include "kaps_jr1_fibmgr.h"
#include "kaps_jr1_fibmgr_xk.h"
#include "kaps_jr1_fib_cmn_seqgen.h"
#include "kaps_jr1_ix_mgr.h"
#include "kaps_jr1_fib_poolmgr.h"
#include "kaps_jr1_arena.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_fib_lsnmc_hw.h"
#include "kaps_jr1_fib_hw.h"
#include "kaps_jr1_fib_cmn_wb.h"
#include "kaps_jr1_algo_hw.h"
#include "kaps_jr1_ad_internal.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_handle.h"
#include "kaps_jr1_ix_mgr.h"
#include "kaps_jr1_fib_lsnmc_recover.h"



#define NLMNS_TRIE_DEFAULT_LOPOFF (8)

#define NLMNS_MAX_INDEX_BUFFER_SIZE (2 * KAPS_JR1_MAX_NUM_IPT_ENTRIES_PER_POOL)

#define NLMNSTRIE_MAX_PFX_BUNDLE_WORD (KAPS_JR1_LPM_KEY_MAX_WIDTH_1 / 32 + sizeof(kaps_jr1_pfx_bundle) / 4 + 4)

static NlmErrNum_t kaps_jr1_trie_pvt_in_place_delete(
    kaps_jr1_lpm_trie * self,
    kaps_jr1_trie_node * node,
    uint32_t isMoveToNewPoolAllowed,
    NlmReasonCode * o_reason);

static NlmErrNum_t kaps_jr1_trie_ctor(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_global * global,
    void *fibTbl_p,
    NlmReasonCode * o_reason);

static NlmErrNum_t kaps_jr1_trie_update_hardware(
    kaps_jr1_lpm_trie * self,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_trie_split_rpt_node(
    kaps_jr1_lpm_trie * trie_p,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_trie_add_ses(
    kaps_jr1_lpm_trie * self,
    kaps_jr1_trie_node * trienode,
    kaps_jr1_pfx_bundle * pfxBundle,
    NlmReasonCode * o_reason);

NlmErrNum_t kaps_jr1_trie_giveout_pending_lsns(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_lsn_mc * originalLsn,
    struct kaps_jr1_lpm_entry *entryToInsert,
    uint32_t tryOnlyLsnPush,
    NlmReasonCode * o_reason);

static void kaps_jr1_trie_pvt_cleanup_del_nodes(
    kaps_jr1_lpm_trie * trie_p);

NlmErrNum_t kaps_jr1_trie_merge_ancestor_lsns(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_node * destnode,
    NlmReasonCode * o_reason);

void kaps_jr1_trie_verify_rpt_apt_lmpsofar(
    kaps_jr1_trie_node * cur_node,
    kaps_jr1_trie_node * highestPriorityAptTrieNode);

void kaps_jr1_trie_verify_down_stream_rpt(
    kaps_jr1_trie_node * curTrieNode,
    kaps_jr1_trie_node * nearestRptAncestor);

void kaps_jr1_trie_verify_dba_reduction(
    kaps_jr1_trie_node * node_p);

void kaps_jr1_trie_verify_joined_udcs(
    kaps_jr1_trie_node * node_p);

void kaps_jr1_trie_verify_num_ipt_and_apt_in_rpt_node(
    kaps_jr1_trie_node * curNode);

void kaps_jr1_trie_verify_ab_and_pool_mapping(
    kaps_jr1_lpm_trie * trie_p);

void kaps_jr1_trie_verify_lsn(
    kaps_jr1_trie_node * curNode);

NlmErrNum_t kaps_jr1_trie_check_resources_for_new_lsn(
    kaps_jr1_trie_node * trienode,
    NlmReasonCode * o_reason);

void kaps_jr1_trie_process_merge_after_giveout(
    kaps_jr1_lpm_trie * self);

uint32_t kaps_jr1_trie_check_if_related_ipt_nodes_can_be_merged(
    kaps_jr1_trie_node * originalIptNode,
    kaps_jr1_trie_node ** trieNode2_pp,
    kaps_jr1_trie_node ** newTrieNode_pp);

NlmErrNum_t kaps_jr1_trie_merge_related_lsns(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_node * trieNodeToMerge1,
    kaps_jr1_trie_node * trieNodeToMerge2,
    kaps_jr1_trie_node * newIptTrieNode,
    NlmReasonCode * o_reason);

void kaps_jr1_trie_wb_verify(
    kaps_jr1_trie_node * trie_node);

uint32_t kaps_jr1_trie_find_terminating_path_main(
    kaps_jr1_trie_node *node);



void kaps_jr1_trie_print_brick(
    FILE *cur_fp, 
    kaps_jr1_lpm_lpu_brick *curBrick)
{
    uint32_t k;

    if (cur_fp)
        kaps_jr1_fprintf(cur_fp, "\t\t");
    else
        kaps_jr1_printf("\t\t");
    
    for (k = 0; k < curBrick->m_maxCapacity; k++) {
        kaps_jr1_pfx_bundle *pfxBundle = curBrick->m_pfxes[k];

        if (pfxBundle) 
        {
            if (cur_fp)
            {
                kaps_jr1_fprintf(cur_fp, "%d, ", pfxBundle->m_nPfxSize);
            }
            else
            {
                kaps_jr1_printf("%d, ", pfxBundle->m_nPfxSize);
            }
        } 
        else 
        {
            if (cur_fp)
            {
                kaps_jr1_fprintf(cur_fp, "0, ");
            }
            else
            {
                kaps_jr1_printf("0, ");
            }
        }
    }

    if (cur_fp)
        kaps_jr1_fprintf(cur_fp, "\n");
    else
        kaps_jr1_printf("\n");
}

void kaps_jr1_trie_print_lsn(
    FILE *cur_fp, 
    kaps_jr1_lsn_mc *curLsn)
{
    kaps_jr1_lpm_lpu_brick *curBrick;
    uint32_t j;

    if (cur_fp)
    {
        kaps_jr1_fprintf(cur_fp, " LSN Id = %d, Depth = %d, Num Pfx = %d, Num Bricks = %d\n",
            curLsn->m_lsnId, curLsn->m_nDepth, curLsn->m_nNumPrefixes,
            curLsn->m_numLpuBricks);
    
    }
    else 
    {
        kaps_jr1_printf(" LSN Id = %d, Depth = %d, Num Pfx = %d, Num Bricks = %d\n",
            curLsn->m_lsnId, curLsn->m_nDepth, curLsn->m_nNumPrefixes,
            curLsn->m_numLpuBricks);
    }
        
    curBrick = curLsn->m_lpuList;
    for (j = 0; j < curLsn->m_numLpuBricks; ++j) {
        if (cur_fp)
        {
            kaps_jr1_fprintf(cur_fp, "\n\t\tBrick Nr = %d, Gran = %d, Num Pfx = %d, Brick Capacity = %d\n",
                j, curBrick->m_gran, curBrick->m_numPfx, curBrick->m_maxCapacity);
        }
        else 
        {
            kaps_jr1_printf("\n\t\tBrick Nr = %d, Gran = %d, Num Pfx = %d, Brick Capacity = %d\n",
                j, curBrick->m_gran, curBrick->m_numPfx, curBrick->m_maxCapacity);
        }
        kaps_jr1_trie_print_brick(cur_fp, curBrick);
        
        curBrick = curBrick->m_next_p;
    }
}

void kaps_jr1_trie_print_lsn_split(
    kaps_jr1_lpm_trie *trie_p, 
    kaps_jr1_lsn_mc *originalLsn)
{
    FILE *cur_fp;
    static uint32_t g_lsnSplitCnt = 0;
    uint32_t i;
    kaps_jr1_flat_lsn_data *curGiveoutData_p;

    g_lsnSplitCnt++;

    if (g_lsnSplitCnt <= 1) {
        cur_fp = kaps_jr1_fopen("LsnSplits.txt", "w");
    } else {
        cur_fp = kaps_jr1_fopen("LsnSplits.txt", "a");
    }

    kaps_jr1_fprintf(cur_fp, "LSN_SPLIT: \n");

    kaps_jr1_fprintf(cur_fp, "TID = %d\n",
                trie_p->m_tbl_ptr->m_db->tid);

    kaps_jr1_fprintf(cur_fp, "Splitting ");

    kaps_jr1_trie_print_lsn(cur_fp, originalLsn);

    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i) {

        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        kaps_jr1_fprintf(cur_fp, "\n\tChild ");

        kaps_jr1_trie_print_lsn(cur_fp, curGiveoutData_p->lsn_cookie);

    }
    kaps_jr1_fprintf(cur_fp, "________________________________\n");

    kaps_jr1_fclose(cur_fp);

}



void kaps_jr1_trie_print_trie(
    FILE *cur_fp, 
    kaps_jr1_trie_node *cur_node)
{
    if (cur_node) {
        if (cur_node->m_node_type == NLMNSTRIENODE_LP) {
            kaps_jr1_trie_print_lsn(cur_fp, cur_node->m_lsn_p);
            kaps_jr1_fprintf(cur_fp, "________________________________________\n\n");
        }

        kaps_jr1_trie_print_trie(cur_fp, cur_node->m_child_p[0]);
        kaps_jr1_trie_print_trie(cur_fp, cur_node->m_child_p[1]);
             
    }
}

void kaps_jr1_trie_print_pfx(
    FILE *cur_fp,
    uint8_t *pfx_data,
    uint32_t pfx_len,
    char demarcater,
    uint8_t print_full_key,
    uint8_t print_new_line)
{
    uint32_t i;
    uint32_t num_pfx_bytes = (pfx_len + 7) / 8;
    
    for (i = 0; i < num_pfx_bytes; ++i)
    {
        if (i == 0)
        {
            if (cur_fp)
                kaps_jr1_fprintf(cur_fp, "%d", pfx_data[i]);
            else   
                kaps_jr1_printf("%d", pfx_data[i]);
        }
        else
        {
            if (cur_fp)
            {
                kaps_jr1_fprintf(cur_fp, "%c", demarcater);
                kaps_jr1_fprintf(cur_fp, "%d", pfx_data[i]);
            }
            else  
            {  
                kaps_jr1_printf("%c", demarcater);
                kaps_jr1_printf("%d", pfx_data[i]);
            }
        }
    }

    if (print_full_key)
    {
        for (i = num_pfx_bytes; i < 20; ++i)
        {
            if (cur_fp)
            {
                kaps_jr1_fprintf(cur_fp, "%c", demarcater);
                kaps_jr1_fprintf(cur_fp, "0");
            }
            else
            {
                kaps_jr1_printf("%c", demarcater);
                kaps_jr1_printf("0");
            }
        }
    }
    else
    {
        if (cur_fp)
            kaps_jr1_fprintf(cur_fp, "/%d", pfx_len);
        else
            kaps_jr1_printf("/%d", pfx_len);
    }
    
    if (print_new_line)
    {
        if (cur_fp)
            kaps_jr1_fprintf(cur_fp, "\n");
        else
            kaps_jr1_printf("\n");
    }
}


void
kaps_jr1_trie_print_ad(
    FILE *fp,
    struct kaps_jr1_device *device,
    struct kaps_jr1_lpm_entry *entry,
    uint8_t use_demarcater)
{

    uint32_t i;
    uint32_t ad_width_8;
    struct kaps_jr1_ad_db *ad_db = NULL;
        
    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, ad_db);
    ad_width_8 = (ad_db->db_info.width.ad_width_1 + 7)/8;
    
    for (i = 0; i < ad_width_8; ++i) 
    {
        if (i == 0) 
        {
            if (fp)
            {
                kaps_jr1_fprintf(fp, "0x%02x", entry->ad_handle->value[i]);    
            }
            else
            {
                kaps_jr1_printf("0x%02x", entry->ad_handle->value[i]);
            }
        }
        else 
        {
            if (use_demarcater)
            {
                if (fp)
                    kaps_jr1_fprintf(fp, ", 0x%02x", entry->ad_handle->value[i]);    
                else
                    kaps_jr1_printf(", 0x%02x", entry->ad_handle->value[i]);
            }
            else
            {
                if (fp)
                    kaps_jr1_fprintf(fp, "%02x", entry->ad_handle->value[i]);    
                else
                    kaps_jr1_printf("%02x", entry->ad_handle->value[i]);

            }
        }
    }

}

void
kaps_jr1_trie_log_search_key_record(
    kaps_jr1_lpm_trie * trie_p,
    struct kaps_jr1_lpm_entry *entry)
{
    struct kaps_jr1_device *device = trie_p->m_tbl_ptr->m_db->device;
    FILE *fp;
        
    if (trie_p->m_tbl_ptr->m_db->rqt_cnt == 0)
        fp = kaps_jr1_fopen("search_key_log.txt", "w");
    else
        fp = kaps_jr1_fopen("search_key_log.txt", "a");
            
    kaps_jr1_fprintf(fp, "{");
        
        
    kaps_jr1_fprintf(fp, "%d", trie_p->m_tbl_ptr->m_db->rpb_id);
        
    kaps_jr1_fprintf(fp, ", ");
        
        
    kaps_jr1_fprintf(fp, "{");
        
    kaps_jr1_trie_print_pfx(fp, entry->pfx_bundle->m_data, 
                    entry->pfx_bundle->m_nPfxSize, ',', 1, 0);
        
    kaps_jr1_fprintf(fp, "}, ");
        
        
    kaps_jr1_fprintf(fp, "{");
        
    kaps_jr1_trie_print_ad(fp, device, entry, 1);
        
    kaps_jr1_fprintf(fp, "} ");
        
    kaps_jr1_fprintf(fp, "}, \n");
        
        
    kaps_jr1_fclose(fp);
            
}

void
kaps_jr1_trie_dump_pfx_log_records(
    struct kaps_jr1_device *device)
{
    uint32_t i, j;
    FILE *fp;
    static uint32_t dump_cnt = 0;

    if (!device->pfx_log_records)
    {
        return;
    }
    
   
    if (dump_cnt == 0)
        fp = kaps_jr1_fopen("pfx_log.txt", "w");
    else
        fp = kaps_jr1_fopen("pfx_log.txt", "a");
        
    dump_cnt++;
   
    
    for (i = 0; i < device->cur_num_pfx_log_records; ++i)
    {
        if (device->pfx_log_records[i].operation == NLM_FIB_PREFIX_INSERT)
        {
            kaps_jr1_fprintf(fp, "ADD:");
        }
        else if (device->pfx_log_records[i].operation == NLM_FIB_PREFIX_DELETE)
        {
            kaps_jr1_fprintf(fp, "DEL:");
        }
        else if (device->pfx_log_records[i].operation == NLM_FIB_PREFIX_UPDATE_AD)
        {
            kaps_jr1_fprintf(fp, "UPD:");
        }
        
        kaps_jr1_fprintf(fp, "DB%d:", device->pfx_log_records[i].db_id);
        
        kaps_jr1_trie_print_pfx(fp, device->pfx_log_records[i].pfx_data,
                device->pfx_log_records[i].pfx_len, '.', 0, 0);
                
        kaps_jr1_fprintf(fp, "  ");
        
        for (j = 0; j < KAPS_JR1_MAX_AD_WIDTH_8; ++j)
        {
            kaps_jr1_fprintf(fp, "%02x", device->pfx_log_records[i].pfx_ad[j]);
        }
        
        kaps_jr1_fprintf(fp, "\n");
    }

    device->cur_num_pfx_log_records = 0;

    fflush(fp);
    
    kaps_jr1_fclose(fp);
}


void
kaps_jr1_trie_store_pfx_log_records(
    kaps_jr1_lpm_trie *trie_p,
    NlmTblRqtCode rqtCode,
    struct kaps_jr1_lpm_entry *entry)
{
    struct kaps_jr1_device *device = trie_p->m_tbl_ptr->m_db->device;
    
    
    if (device->pfx_log_records 
        && device->cur_num_pfx_log_records < device->max_num_pfx_log_records)
    {
        uint32_t i = device->cur_num_pfx_log_records;
        uint32_t num_bytes = (entry->pfx_bundle->m_nPfxSize + 7) / 8;
        
        device->pfx_log_records[i].operation = rqtCode;
        device->pfx_log_records[i].db_id = trie_p->m_tbl_ptr->m_db->tid;
        device->pfx_log_records[i].pfx_len = entry->pfx_bundle->m_nPfxSize;
        
        kaps_jr1_memcpy(device->pfx_log_records[i].pfx_data,
                    entry->pfx_bundle->m_data, 
                    num_bytes);
                    
        kaps_jr1_memcpy(device->pfx_log_records[i].pfx_ad,
                    entry->ad_handle->value,
                    KAPS_JR1_MAX_AD_WIDTH_8);
        
        device->cur_num_pfx_log_records++;
        
        if (device->cur_num_pfx_log_records == device->max_num_pfx_log_records)
        {
            kaps_jr1_trie_dump_pfx_log_records(device);
        }
    }
}




void
kaps_jr1_trie_decr_num_ipt_entries_in_subtrie(
    kaps_jr1_trie_node * node,
    uint32_t value)
{
    kaps_jr1_assert(node->m_numIptEntriesInSubtrie >= value, "Num IPT entries in subtrie is becoming negative");
    node->m_numIptEntriesInSubtrie -= value;
}

void
kaps_jr1_trie_incr_num_ipt_entries_in_subtrie(
    kaps_jr1_trie_node * node,
    uint32_t value)
{
    node->m_numIptEntriesInSubtrie += value;
}

uint16_t
kaps_jr1_trie_get_num_rpt_bits_lopped_off(
    struct kaps_jr1_device *device,
    uint32_t depth)
{
    uint16_t numRptBitsLoppedOff = 0;

    (void) device;

    
    {

        numRptBitsLoppedOff = (uint16_t) (depth / 8);

        numRptBitsLoppedOff = numRptBitsLoppedOff * 8;

        if (numRptBitsLoppedOff > MAX_RPT_LOPOFF_IN_BYTES * 8)
            numRptBitsLoppedOff = MAX_RPT_LOPOFF_IN_BYTES * 8;
    }

    return numRptBitsLoppedOff;
}

NlmErrNum_t
kaps_jr1_trie_obtain_rpt_uuid(
    kaps_jr1_trie_node * rptTrieNode)
{
    kaps_jr1_trie_global *trie_global = rptTrieNode->m_trie_p->m_trie_global;
    uint32_t i;

    if (rptTrieNode->m_rpt_uuid == NLM_INVALID_RPT_UUID)
    {
        for (i = 0; i < KAPS_JR1_MAX_NUM_RPT_UUID; ++i)
        {
            if (trie_global->m_rpt_uuid_table[i])
            {
                rptTrieNode->m_rpt_uuid = i;
                trie_global->m_rpt_uuid_table[i] = 0;
                break;
            }
        }
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_release_rpt_uuid(
    kaps_jr1_trie_node * rptTrieNode)
{
    kaps_jr1_trie_global *trie_global = rptTrieNode->m_trie_p->m_trie_global;
    uint32_t i;

    if (rptTrieNode->m_rpt_uuid != NLM_INVALID_RPT_UUID)
    {
        i = rptTrieNode->m_rpt_uuid;
        trie_global->m_rpt_uuid_table[i] = 1;
    }

    rptTrieNode->m_rpt_uuid = NLM_INVALID_RPT_UUID;

    return NLMERR_OK;
}

uint32_t kaps_jr1_trie_is_lsn_empty(
    kaps_jr1_lpm_trie *trie_p,
    kaps_jr1_trie_node *curNode)
{
    uint32_t isLsnEmpty = 0;

    if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
    {
        
        if (curNode->m_lsn_p->m_nNumPrefixes == 0
            && (!curNode->m_iitLmpsofarPfx_p
                || curNode->m_iitLmpsofarPfx_p->m_nPfxSize != curNode->m_depth)
            )
        {
            isLsnEmpty = 1;;
        }
    }
    else 
    {
        if (curNode->m_lsn_p->m_nNumPrefixes == 0)
        {
            isLsnEmpty = 1;;
        }
    }

    return isLsnEmpty;
}




static NlmErrNum_t
kaps_jr1_trie_pvt_get_new_pool(
    kaps_jr1_pool_mgr * poolMgr,
    kaps_jr1_trie_node * rptTrieNode_p,
    uint32_t numIptEntriesRequired,
    uint32_t numAptEntriesRequired,
    uint32_t numPools,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    int32_t was_dba_dynamic_alloc = 0;


    errNum = kaps_jr1_pool_mgr_assign_pool_to_rpt_entry(poolMgr, rptTrieNode_p, numIptEntriesRequired,
                                                    numAptEntriesRequired, numPools, KAPS_JR1_RPB_WIDTH_1,
                                                    &was_dba_dynamic_alloc, o_reason);
    return errNum;

}




void
kaps_jr1_trie_propagate_rpt_attributes(
    kaps_jr1_trie_node * curNode_p,
    kaps_jr1_trie_node * rptNode_p)
{
    if (!curNode_p)
        return;

    if (curNode_p->m_isRptNode)
        return;

    curNode_p->m_rptParent_p = rptNode_p;
    if (rptNode_p)
    {
        curNode_p->m_poolId = rptNode_p->m_poolId;
        curNode_p->m_rptId = rptNode_p->m_rptId;
    }

    kaps_jr1_trie_propagate_rpt_attributes(curNode_p->m_child_p[0], rptNode_p);
    kaps_jr1_trie_propagate_rpt_attributes(curNode_p->m_child_p[1], rptNode_p);
}



void
kaps_jr1_trie_pvt_calc_num_ipt_and_apt(
    kaps_jr1_trie_node * node_p)
{
    uint32_t numIptInLeftSubtrie = 0;
    uint32_t numAptInLeftSubtrie = 0;
    uint32_t numIptInRightSubtrie = 0;
    uint32_t numAptInRightSubtrie = 0;
    uint32_t numIptEntriesInCurNode = 0;
    uint32_t numAptEntriesInCurNode = 0;
    uint32_t num160bIptInLeftSubtrie = 0;
    uint32_t num160bIptInRightSubtrie = 0;
    uint32_t num160bIptInCurNode = 0;

    if (node_p)
    {

        if (node_p->m_child_p[0] && !node_p->m_child_p[0]->m_isRptNode)
        {
            kaps_jr1_trie_pvt_calc_num_ipt_and_apt(node_p->m_child_p[0]);
            numIptInLeftSubtrie = node_p->m_child_p[0]->m_numIptEntriesInSubtrie;
            num160bIptInLeftSubtrie = node_p->m_child_p[0]->m_numReserved160bTrig;
            numAptInLeftSubtrie = node_p->m_child_p[0]->m_numAptEntriesInSubtrie;
        }

        if (node_p->m_child_p[1] && !node_p->m_child_p[1]->m_isRptNode)
        {
            kaps_jr1_trie_pvt_calc_num_ipt_and_apt(node_p->m_child_p[1]);
            numIptInRightSubtrie = node_p->m_child_p[1]->m_numIptEntriesInSubtrie;
            num160bIptInRightSubtrie = node_p->m_child_p[1]->m_numReserved160bTrig;
            numAptInRightSubtrie = node_p->m_child_p[1]->m_numAptEntriesInSubtrie;
        }

        if (node_p->m_node_type == NLMNSTRIENODE_LP && node_p->m_lp_prefix_p->m_nIndex != KAPS_JR1_LSN_NEW_INDEX)
        {
            numIptEntriesInCurNode = 1;
        }

        if (node_p->m_aptLmpsofarPfx_p)
            numAptEntriesInCurNode++;

        if (node_p->m_rptAptLmpsofarPfx)
            numAptEntriesInCurNode++;

        node_p->m_numIptEntriesInSubtrie = numIptInLeftSubtrie + numIptInRightSubtrie + numIptEntriesInCurNode;

        node_p->m_numReserved160bTrig = num160bIptInLeftSubtrie + num160bIptInRightSubtrie + num160bIptInCurNode;

        node_p->m_numAptEntriesInSubtrie = numAptInLeftSubtrie + numAptInRightSubtrie + numAptEntriesInCurNode;
    }
}



void
kaps_jr1_trie_pvt_calc_longest_size_for_ipt_and_apt(
    kaps_jr1_trie_node * node_p,
    kaps_jr1_trie_node * rpt_node_p,
    uint32_t * longest_apt_entry_depth,
    uint32_t * longest_ipt_entry_depth)
{
    if (node_p)
    {
        if (node_p->m_isRptNode && node_p != rpt_node_p)
            return;

        if (node_p->m_aptLmpsofarPfx_p)
        {
            if ((*longest_apt_entry_depth) < node_p->m_aptLmpsofarPfx_p->m_nPfxSize)
                *longest_apt_entry_depth = node_p->m_aptLmpsofarPfx_p->m_nPfxSize;
        }

        if (node_p->m_rptAptLmpsofarPfx)
        {
            if ((*longest_apt_entry_depth) < node_p->m_rptAptLmpsofarPfx->m_nPfxSize)
                *longest_apt_entry_depth = node_p->m_rptAptLmpsofarPfx->m_nPfxSize;
        }

        if (node_p->m_node_type == NLMNSTRIENODE_LP && (*longest_ipt_entry_depth) < node_p->m_depth)
            *longest_ipt_entry_depth = node_p->m_depth;


        if (node_p->m_child_p[0])
        {
            kaps_jr1_trie_pvt_calc_longest_size_for_ipt_and_apt(node_p->m_child_p[0], rpt_node_p, longest_apt_entry_depth,
                                                            longest_ipt_entry_depth);
        }

        if (node_p->m_child_p[1])
        {
            kaps_jr1_trie_pvt_calc_longest_size_for_ipt_and_apt(node_p->m_child_p[1], rpt_node_p, longest_apt_entry_depth,
                                                            longest_ipt_entry_depth);
        }
    }
}


NlmErrNum_t
kaps_jr1_trie_clear_holes_in_split_ipt_ancestor(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_node * ancestorIptNode,
    kaps_jr1_trie_node * newRptParent_p,
    uint32_t pfxLocationsToMove_p[],
    uint16_t numPfxToMove,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc *ancestorIptLsn_p = NULL;
    uint32_t i = 0, j = 0, curIndexInLsn = 0;
    uint32_t newix = 0;
    kaps_jr1_lpm_lpu_brick *curLpuBrick = NULL;
    kaps_jr1_trie_node *trieNodeForMerge;
    uint32_t isLsnEmpty;

    if (numPfxToMove == 0)
        return NLMERR_OK;

    ancestorIptLsn_p = ancestorIptNode->m_lsn_p;

    kaps_jr1_assert((ancestorIptLsn_p != NULL), "Invalid IPT LSN pointer.\n");
    curLpuBrick = ancestorIptLsn_p->m_lpuList;

    while (curLpuBrick)
    {
        for (i = 0; i < curLpuBrick->m_maxCapacity; i++)
        {
            if (curIndexInLsn == pfxLocationsToMove_p[j])
            {
                newix = kaps_jr1_lsn_mc_get_ix_mgr_index(ancestorIptLsn_p, pfxLocationsToMove_p[j]);

                NLM_STRY(kaps_jr1_lsn_mc_delete_entry_in_hw(ancestorIptLsn_p, newix, o_reason));

                curLpuBrick->m_pfxes[i] = 0;
                curLpuBrick->m_numPfx--;
                ++j;
                if (j >= numPfxToMove)
                {
                    break;
                }
            }
            ++curIndexInLsn;
        }
        if (j >= numPfxToMove)
        {
            break;
        }
        curLpuBrick = curLpuBrick->m_next_p;
    }
    ancestorIptLsn_p->m_nNumPrefixes = ancestorIptLsn_p->m_nNumPrefixes - numPfxToMove;

    if (newRptParent_p)
        newRptParent_p->m_isRptNode = 1;

    isLsnEmpty = kaps_jr1_trie_is_lsn_empty(trie_p, ancestorIptNode);
   
    if (isLsnEmpty)
    {
        kaps_jr1_trie_pvt_in_place_delete(trie_p, ancestorIptNode, 1, o_reason);

        for (i = 0; i < trie_p->m_trie_global->m_numTrieNodesForMerge; ++i)
        {
            trieNodeForMerge = trie_p->m_trie_global->m_trieNodesForRelatedMerge[i];

            if (trieNodeForMerge == ancestorIptNode)
            {
                trie_p->m_trie_global->m_trieNodesForRelatedMerge[i] = NULL;
            }
        }

    }
    else
    {
        if (trie_p->m_lsn_settings_p->m_isShrinkEnabled)
        {
            if (trie_p->m_lsn_settings_p->m_isPerLpuGran)
            {
                kaps_jr1_lsn_mc_shrink_per_lpu_gran(ancestorIptLsn_p, o_reason);
            }
            else
            {
                kaps_jr1_lsn_mc_shrink_per_lsn_gran(ancestorIptLsn_p, o_reason);
            }
        }
    }

    if (newRptParent_p)
        newRptParent_p->m_isRptNode = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_check_split_nearest_ipt_ancestor(
    kaps_jr1_lpm_trie *trie_p,
    kaps_jr1_trie_node * newRptNode_p,
    kaps_jr1_lsn_mc ** ancestorIptLsn_pp,
    uint32_t pfxLocationsToMove_p[],
    uint32_t * numPfxToMove_p,
    uint32_t *isPfxMatchingNewRptNodePresentInAncestor,
    NlmReasonCode * o_reason)
{
    kaps_jr1_trie_node *iterTrieNode_p = newRptNode_p;
    kaps_jr1_lsn_mc *ancestorIptLsn_p = NULL;
    uint32_t byteOffset = 0;
    uint32_t commonLength = 0;
    uint32_t i = 0;
    uint16_t numPfxToMove = 0;
    kaps_jr1_pfx_bundle *pfxBundle_p = NULL;
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    uint32_t indexInLsn;

    (void) o_reason;
    *numPfxToMove_p = 0;
    *isPfxMatchingNewRptNodePresentInAncestor = 0;

    
    if (newRptNode_p->m_node_type == NLMNSTRIENODE_LP)
    {
        return NLMERR_OK;
    }

    
    while (iterTrieNode_p != NULL && iterTrieNode_p->m_node_type != NLMNSTRIENODE_LP)
    {
        iterTrieNode_p = iterTrieNode_p->m_parent_p;
    }

    if (!iterTrieNode_p || iterTrieNode_p->m_node_type != NLMNSTRIENODE_LP)
    {
        return NLMERR_FAIL;
    }

    ancestorIptLsn_p = iterTrieNode_p->m_lsn_p;
    byteOffset = ancestorIptLsn_p->m_nDepth / 8;
    commonLength = byteOffset * 8;

    
    curLpuBrick = ancestorIptLsn_p->m_lpuList;
    indexInLsn = 0;
    while (curLpuBrick)
    {
        for (i = 0; i < curLpuBrick->m_maxCapacity; i++)
        {
            pfxBundle_p = curLpuBrick->m_pfxes[i];

            if (pfxBundle_p)
            {
                uint32_t isMoreSpecificOrEqual = kaps_jr1_prefix_pvt_is_more_specific_equal(&pfxBundle_p->m_data[byteOffset],
                                                                         pfxBundle_p->m_nPfxSize - commonLength,
                                                                         &newRptNode_p->
                                                                         m_lp_prefix_p->m_data[byteOffset],
                                                                         newRptNode_p->m_lp_prefix_p->m_nPfxSize -
                                                                         commonLength);
                uint32_t doMove = 0;

                if (isMoreSpecificOrEqual
                    && pfxBundle_p->m_nPfxSize == newRptNode_p->m_lp_prefix_p->m_nPfxSize)
                {
                    *isPfxMatchingNewRptNodePresentInAncestor = 1;
                }

                

                if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
                {
                    
                    if (isMoreSpecificOrEqual 
                        && pfxBundle_p->m_nPfxSize == newRptNode_p->m_lp_prefix_p->m_nPfxSize)
                    {
                        doMove = 0;
                    }
                    else
                    {
                        doMove = isMoreSpecificOrEqual;
                    }
                }
                else
                {
                    doMove = isMoreSpecificOrEqual;
                }

                
                if (doMove)
                {
                    pfxLocationsToMove_p[numPfxToMove] = indexInLsn;
                    numPfxToMove++;
                }
            }
            ++indexInLsn;
        }
        curLpuBrick = curLpuBrick->m_next_p;
    }

    *numPfxToMove_p = numPfxToMove;
    *ancestorIptLsn_pp = ancestorIptLsn_p;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_compute_ipt_ancestor_pfx_sort_data(
    kaps_jr1_lsn_mc * ancestorIptLsn_p,
    uint32_t pfxLocationsToMove_p[],
    uint16_t numPfxToMove,
    uint16_t numPfxForEachLength[],
    uint16_t whereForEachLength[])
{
    kaps_jr1_lpm_lpu_brick *ancestorLpuBrick;
    uint16_t i, j, indexInLsn;
    kaps_jr1_pfx_bundle *pfxBundle_p;
    uint16_t curLoc;
    uint32_t length;

    for (i = 0; i <= KAPS_JR1_LPM_KEY_MAX_WIDTH_1; ++i)
    {
        numPfxForEachLength[i] = 0;
    }

    ancestorLpuBrick = ancestorIptLsn_p->m_lpuList;
    j = 0;
    indexInLsn = 0;
    while (ancestorLpuBrick)
    {
        for (i = 0; i < ancestorLpuBrick->m_maxCapacity; i++)
        {
            if (indexInLsn == pfxLocationsToMove_p[j])
            {
                pfxBundle_p = ancestorLpuBrick->m_pfxes[i];
                length = pfxBundle_p->m_nPfxSize - ancestorIptLsn_p->m_nDepth;
                numPfxForEachLength[length]++;
                ++j;
                if (j >= numPfxToMove)
                {
                    break;
                }
            }
            ++indexInLsn;
        }

        if (j >= numPfxToMove)
        {
            break;
        }
        ancestorLpuBrick = ancestorLpuBrick->m_next_p;
    }

    curLoc = 0;
    for (i = 0; i <= KAPS_JR1_LPM_KEY_MAX_WIDTH_1; ++i)
    {
        whereForEachLength[i] = curLoc;
        curLoc += numPfxForEachLength[i];
    }

    return NLMERR_OK;
}


NlmErrNum_t
kaps_jr1_trie_ipt_ancestor_iit_lmpsofar(
    kaps_jr1_trie_node * newNode_p,
    kaps_jr1_lsn_mc * ancestorIptLsn_p,
    uint32_t pfxLocationsArray[],
    uint32_t *numPfxInArray)
{
    kaps_jr1_lpm_trie *trie_p = newNode_p->m_trie_p;
    kaps_jr1_lsn_mc *newLsn_p = newNode_p->m_lsn_p;
    uint32_t i;
    kaps_jr1_lpm_lpu_brick *ancestorLpuBrick;
    kaps_jr1_pfx_bundle *ancestorPfxBundle, *lmpsofarPfxBundle;
    kaps_jr1_trie_node *ancestorIptNode_p;
    void *tmp_ptr;
    uint32_t isPfxLenSuitable;
    uint32_t isLmpsofarInsideAncestorLsn = 0;
    uint32_t indexOfLmpsofarInLsn = KAPS_JR1_LSN_NEW_INDEX, indexInLsn;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = trie_p->m_lsn_settings_p->m_device;

    (void) ad_db;
    (void) device;

    if (!trie_p->m_trie_global->m_isIITLmpsofar)
        return NLMERR_OK;

    if (!ancestorIptLsn_p)
        return NLMERR_OK;

    tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(ancestorIptLsn_p->m_pParentHandle);
    kaps_jr1_memcpy(&ancestorIptNode_p, tmp_ptr, sizeof(kaps_jr1_trie_node *));

    
    lmpsofarPfxBundle = ancestorIptNode_p->m_iitLmpsofarPfx_p;

    
    ancestorLpuBrick = ancestorIptLsn_p->m_lpuList;
    indexInLsn = 0; 
    while (ancestorLpuBrick)
    {
        for (i = 0; i < ancestorLpuBrick->m_maxCapacity; ++i)
        {
            ancestorPfxBundle = ancestorLpuBrick->m_pfxes[i];
            
            if (!ancestorPfxBundle)
            {
                indexInLsn++;
                continue;
            }

            isPfxLenSuitable = 0;
            if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
            {
                
                if (ancestorPfxBundle->m_nPfxSize <= newLsn_p->m_nDepth) 
                {
                    isPfxLenSuitable = 1;
                }
            } 
            else 
            {
                
                if (ancestorPfxBundle->m_nPfxSize < newLsn_p->m_nDepth) 
                {
                    isPfxLenSuitable = 1;
                }
            }

            
            if (ancestorPfxBundle && isPfxLenSuitable &&
                !ancestorPfxBundle->m_isPfxCopy)
            {
                
                if (kaps_jr1_prefix_pvt_is_more_specific_equal
                    (newNode_p->m_lp_prefix_p->m_data, newNode_p->m_lp_prefix_p->m_nPfxSize, ancestorPfxBundle->m_data,
                     ancestorPfxBundle->m_nPfxSize))
                {
                    
                    if (!lmpsofarPfxBundle
                        || (ancestorPfxBundle->m_nPfxSize > lmpsofarPfxBundle->m_nPfxSize))
                    {
                        lmpsofarPfxBundle = ancestorPfxBundle;

                        
                        isLmpsofarInsideAncestorLsn = 1;
                        indexOfLmpsofarInLsn = indexInLsn;
                    }
                }
            }

            indexInLsn++;
        }

        ancestorLpuBrick = ancestorLpuBrick->m_next_p;
    }

    if (!lmpsofarPfxBundle)
    {
        return NLMERR_OK;
    }

    if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds
        && isLmpsofarInsideAncestorLsn
        && lmpsofarPfxBundle->m_nPfxSize == newNode_p->m_depth
        && pfxLocationsArray)
    {
        
        i = *numPfxInArray;

        while (i > 0)
        {
            
            if (pfxLocationsArray[i-1] < indexOfLmpsofarInLsn)
            {
                break;
            }
            else
            {
                pfxLocationsArray[i] = pfxLocationsArray[i-1];
            }
            --i;
        }
        
        pfxLocationsArray[i] = indexOfLmpsofarInLsn;
        (*numPfxInArray)++;
    }


    if (!lmpsofarPfxBundle->m_isLmpsofarPfx) 
    {
        lmpsofarPfxBundle->m_isLmpsofarPfx = 1;
    }


    newNode_p->m_iitLmpsofarPfx_p = lmpsofarPfxBundle;


    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_split_ipt_ancestor_and_get_pfx_for_new_lsn(
    kaps_jr1_trie_node * newRptNode_p,
    kaps_jr1_lsn_mc * ancestorIptLsn_p,
    uint32_t pfxLocationsToMove_p[],
    uint16_t numPfxToMove,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc *newLsn_p = NULL;
    uint32_t i = 0, j = 0;
    kaps_jr1_pfx_bundle *pfxBundle_p = NULL;
    uint32_t indexInLsn = 0;
    kaps_jr1_lpm_lpu_brick *ancestorLpuBrick;
    kaps_jr1_nlm_allocator *alloc_p = ancestorIptLsn_p->m_pSettings->m_pAlloc;
    kaps_jr1_flat_lsn_data *flatLsnData_p;
    uint16_t numPfxForEachLength[KAPS_JR1_LPM_KEY_MAX_WIDTH_1 + 1], whereForEachLength[KAPS_JR1_LPM_KEY_MAX_WIDTH_1 + 1];
    uint16_t curLoc;
    uint32_t length;
    uint32_t doesLsnFit;
    NlmErrNum_t errNum;

    newLsn_p = newRptNode_p->m_lsn_p;

    if (!newLsn_p)
    {
        *o_reason = NLMRSC_INTERNAL_ERROR;
        return NLMERR_FAIL;
    }

    flatLsnData_p = kaps_jr1_lsn_mc_create_flat_lsn_data(alloc_p, o_reason);
    if (!flatLsnData_p)
        return NLMERR_FAIL;

    flatLsnData_p->m_commonPfx =
        kaps_jr1_prefix_create(alloc_p, KAPS_JR1_LPM_KEY_MAX_WIDTH_1, ancestorIptLsn_p->m_nDepth,
                           ancestorIptLsn_p->m_pParentHandle->m_data);

    if (!flatLsnData_p->m_commonPfx)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (numPfxToMove)
    {
      
        kaps_jr1_trie_compute_ipt_ancestor_pfx_sort_data(ancestorIptLsn_p, pfxLocationsToMove_p, numPfxToMove,
                                                     numPfxForEachLength, whereForEachLength);

        ancestorLpuBrick = ancestorIptLsn_p->m_lpuList;
        j = 0;
        indexInLsn = 0;
        while (ancestorLpuBrick)
        {
            for (i = 0; i < ancestorLpuBrick->m_maxCapacity; i++)
            {
                if (indexInLsn == pfxLocationsToMove_p[j])
                {
                    pfxBundle_p = ancestorLpuBrick->m_pfxes[i];

                    if (newRptNode_p->m_trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds
                        && pfxBundle_p->m_nPfxSize == newRptNode_p->m_depth)
                    {
                        
                        kaps_jr1_assert(0, "Exactly matching prefix should not be present in pfxLocationsToMove_p\n");


                        
                        ++j;
                        if (j >= numPfxToMove)
                            break;

                        ++indexInLsn;
                        continue;
                    }
                    
                    length = pfxBundle_p->m_nPfxSize - ancestorIptLsn_p->m_nDepth;

                    curLoc = whereForEachLength[length];
                    flatLsnData_p->m_pfxesInLsn[curLoc] = pfxBundle_p;
                    whereForEachLength[length]++;

                    flatLsnData_p->m_numPfxInLsn++;
                    if (flatLsnData_p->m_maxPfxLenInBits < pfxBundle_p->m_nPfxSize)
                    {
                        flatLsnData_p->m_maxPfxLenInBits = pfxBundle_p->m_nPfxSize;
                    }

                    ++j;
                    if (j >= numPfxToMove)
                    {
                        break;
                    }
                }

                ++indexInLsn;
            }

            
            if (j >= numPfxToMove)
            {
                break;
            }
            
            ancestorLpuBrick = ancestorLpuBrick->m_next_p;
        }
    }

    kaps_jr1_lsn_mc_assign_flat_data_colors(newLsn_p->m_pSettings, flatLsnData_p);

    doesLsnFit = 0;
    errNum = kaps_jr1_lsn_mc_convert_flat_data_to_lsn(flatLsnData_p, newLsn_p, &doesLsnFit, o_reason);

    if (errNum != NLMERR_OK || !doesLsnFit)
    {
        kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, flatLsnData_p);
        kaps_jr1_lsn_mc_destroy(newLsn_p);

        newRptNode_p->m_lsn_p =
            kaps_jr1_lsn_mc_create(ancestorIptLsn_p->m_pSettings, newRptNode_p->m_trie_p, newRptNode_p->m_depth);

        return NLMERR_FAIL;
    }

    kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, flatLsnData_p);

    newLsn_p->m_nNumPrefixes = numPfxToMove;
    newLsn_p->m_bIsNewLsn = 1;
    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_split_nearest_ipt_ancestor(
    kaps_jr1_trie_node * newRptNode_p,
    kaps_jr1_lsn_mc * ancestorIptLsn_p,
    uint32_t pfxLocationsToMove_p[],
    uint16_t numPfxToMove,
    uint32_t pfxLocationsToClear_p[],
    uint32_t *numPfxToClear_p,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lpm_trie *trie_p = newRptNode_p->m_trie_p;
    kaps_jr1_lsn_mc *newLsn_p = NULL;
    kaps_jr1_ipm *ipm_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t numRptBitsLoppedOff = 0;
    kaps_jr1_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    kaps_jr1_lpm_lpu_brick *cur_lpu_brick;
    uint32_t curBrickIter;
    kaps_jr1_lsn_mc_settings *settings = ancestorIptLsn_p->m_pSettings;

    newLsn_p = newRptNode_p->m_lsn_p;

    if (!newLsn_p)
        return NLMERR_FAIL;

    errNum = kaps_jr1_trie_split_ipt_ancestor_and_get_pfx_for_new_lsn(newRptNode_p, ancestorIptLsn_p,
                                                              pfxLocationsToMove_p, numPfxToMove, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_jr1_lsn_mc_acquire_resources(newLsn_p, newLsn_p->m_nLsnCapacity, newLsn_p->m_numLpuBricks, o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_jr1_lsn_mc_destroy(newLsn_p);
        newRptNode_p->m_lsn_p = kaps_jr1_trie_node_pvt_create_lsn(newRptNode_p);
        return errNum;
    }

    if (settings->m_isJoinedUdc && settings->m_isPerLpuGran)
    {
        errNum = kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(newLsn_p, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    if (newLsn_p->m_pSettings->m_isPerLpuGran)
    {
        cur_lpu_brick = newLsn_p->m_lpuList;
        curBrickIter = 0;
        while (cur_lpu_brick)
        {
            uint32_t ix_rqt_size = cur_lpu_brick->m_maxCapacity;

            kaps_jr1_lsn_mc_handle_under_alloc(newLsn_p, cur_lpu_brick, curBrickIter, &ix_rqt_size);

            errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(newLsn_p, cur_lpu_brick, curBrickIter,
                                                            ix_rqt_size, cur_lpu_brick->ad_db,
                                                           o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_jr1_lsn_mc_free_resources_per_lpu(newLsn_p);
                kaps_jr1_lsn_mc_destroy(newLsn_p);
                newRptNode_p->m_lsn_p = kaps_jr1_trie_node_pvt_create_lsn(newRptNode_p);
                return errNum;
            }
            curBrickIter++;
            cur_lpu_brick = cur_lpu_brick->m_next_p;
        }
        newLsn_p->m_nNumIxAlloced = newLsn_p->m_nLsnCapacity;
    }

    kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(newLsn_p, o_reason);

    if (trie_p->m_trie_global->m_isIITLmpsofar)
    {
        kaps_jr1_trie_ipt_ancestor_iit_lmpsofar(newRptNode_p, ancestorIptLsn_p, 
                                pfxLocationsToClear_p, numPfxToClear_p);
    }

    NLM_STRY(kaps_jr1_lsn_mc_commit(newLsn_p, 0, o_reason));

    newRptNode_p->m_lsn_p = newLsn_p;
    newLsn_p->m_pParentHandle = newRptNode_p->m_lp_prefix_p;

    trie_p->m_trie_global->m_isNearestIptAncestorSplit = 1;

    newRptNode_p->m_node_type = NLMNSTRIENODE_LP;

    newRptNode_p->m_rptParent_p = newRptNode_p;

    ipm_p = kaps_jr1_pool_mgr_get_ipm_for_pool(poolMgr, newRptNode_p->m_poolId);

    numRptBitsLoppedOff = kaps_jr1_trie_get_num_rpt_bits_lopped_off(poolMgr->fibTblMgr->m_devMgr_p, newRptNode_p->m_depth);

    errNum = kaps_jr1_ipm_add_entry(ipm_p, newRptNode_p->m_lp_prefix_p, newRptNode_p->m_rptId,
                                newRptNode_p->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    kaps_jr1_ipm_delete_entry_in_hw(ipm_p, newRptNode_p->m_lp_prefix_p->m_nIndex, newRptNode_p->m_rptId,
                                newRptNode_p->m_depth - numRptBitsLoppedOff, o_reason);

    errNum = kaps_jr1_lsn_mc_commitIIT(newRptNode_p->m_lsn_p, newRptNode_p->m_lp_prefix_p->m_nIndex, o_reason);

    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_jr1_ipm_write_entry_to_hw(ipm_p, newRptNode_p->m_lp_prefix_p, newRptNode_p->m_rptId,
                                        newRptNode_p->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, o_reason);

    newLsn_p->m_bIsNewLsn = 0;

    return errNum;
}



kaps_jr1_trie_global *
kaps_jr1_trie_two_level_fib_module_init(
    kaps_jr1_nlm_allocator * alloc,
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason)
{
    kaps_jr1_trie_global *global;
    kaps_jr1_status status;
    struct kaps_jr1_lpm_mgr *lpm_mgr = NULL;
    struct kaps_jr1_db_hw_resource *db_hw_res = NULL;
    struct kaps_jr1_device *device = NULL;

    (void) o_reason;

    if (!alloc || !fibTblMgr)
    {
        *o_reason = NLMRSC_INVALID_PARAM;
        return NULL;
    }
    lpm_mgr = fibTblMgr->m_lpm_mgr;
    device = fibTblMgr->m_devMgr_p;
    if (!lpm_mgr)
    {
        *o_reason = NLMRSC_INVALID_PARAM;
        return NULL;
    }

    db_hw_res = lpm_mgr->resource_db->hw_res.db_res;

    
    global = kaps_jr1_nlm_allocator_calloc(alloc, 1, sizeof(kaps_jr1_trie_global));
    if (!global)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    global->m_alloc_p = alloc;
    global->fibtblmgr = fibTblMgr;
    global->m_areAllPoolsHealthy = 1;
    global->m_isIITLmpsofar = 1;

    status = kaps_jr1_uda_mgr_init(device, lpm_mgr->resource_db, 0, device->alloc,
                               db_hw_res->lsn_info[0].alloc_udm, &global->m_mlpmemmgr[0],
                               db_hw_res->lsn_info[0].max_lsn_size,
                               0,
                               UDM_BOTH_HALF,
                               kaps_jr1_fib_uda_sub_block_copy_callback,
                               kaps_jr1_fib_update_it_callback,
                               kaps_jr1_fib_update_lsn_size,
                               0);

    if (status != KAPS_JR1_OK)
    {
        kaps_jr1_trie_convert_status_to_err_num(status, o_reason);
        kaps_jr1_trie_module_destroy(global);
        return NULL;
    }


    if (kaps_jr1_db_is_reduced_algo_levels(lpm_mgr->resource_db))
    {
        global->poolMgr = kaps_jr1_pool_mgr_create(fibTblMgr, global->m_alloc_p, 0, &(lpm_mgr->resource_db->hw_res.db_res->rpb_ab_list));
    }
    else
    {
        global->poolMgr = kaps_jr1_pool_mgr_create(fibTblMgr, global->m_alloc_p, 0, kaps_jr1_resource_get_ab_list(lpm_mgr->resource_db));
    }
    
    if (!global->poolMgr)
    {
        kaps_jr1_trie_module_destroy(global);
        return NULL;
    }

    global->m_indexBuffer_p = kaps_jr1_nlm_allocator_calloc(global->m_alloc_p, NLMNS_MAX_INDEX_BUFFER_SIZE,
                                                        sizeof(uint32_t));

    if (!global->m_indexBuffer_p)
    {
        kaps_jr1_trie_module_destroy(global);
        return NULL;
    }

    return global;
}

void
kaps_jr1_trie_module_destroy(
    void *global)
{
    kaps_jr1_trie_global *self = (kaps_jr1_trie_global *) global;
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    kaps_jr1_nlm_allocator *alloc_p = NULL;
    uint32_t devId = 0;

    if (!self)
        return;

    fibTblMgr = self->fibtblmgr;
    for (devId = 0; devId < fibTblMgr->m_numOfDevices; devId++)
    {
        if (self->m_mlpmemmgr[devId])
        {
            if (!self->is_common_uda_mgr_used[devId])
            {
                kaps_jr1_uda_mgr_destroy(self->m_mlpmemmgr[devId]);
            }
            else
            {
                self->m_mlpmemmgr[devId] = NULL;
                self->is_common_uda_mgr_used[devId] = 0;
            }
        }
    }

    kaps_jr1_pool_mgr_destroy(self->poolMgr, self->m_alloc_p);

    if (self->m_indexBuffer_p)
        kaps_jr1_nlm_allocator_free(self->m_alloc_p, self->m_indexBuffer_p);

    if (self->m_isReserved160bTrigBuffer)
        kaps_jr1_nlm_allocator_free(self->m_alloc_p, self->m_isReserved160bTrigBuffer);

    alloc_p = self->m_alloc_p;

    kaps_jr1_nlm_allocator_free(alloc_p, self);
}

kaps_jr1_lpm_trie *
kaps_jr1_trie_create(
    void *trieGlobal,
    void *fibTbl_p,
    NlmReasonCode * o_reason)
{
    kaps_jr1_nlm_allocator *alloc_p = NULL;
    kaps_jr1_lpm_trie *trie_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_assert(trieGlobal && fibTbl_p, "Invalid Pointers passed");

    alloc_p = ((kaps_jr1_trie_global *) trieGlobal)->m_alloc_p;
    kaps_jr1_assert(alloc_p, "Invalid Alloc pointers in triesettings ");

    trie_p = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_lpm_trie));
    if (!trie_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    errNum = kaps_jr1_trie_ctor(trie_p, trieGlobal, fibTbl_p, o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_jr1_nlm_allocator_free(alloc_p, trie_p);
        return NULL;
    }

    return trie_p;
}

NlmErrNum_t
kaps_jr1_trie_commit_iits(
    kaps_jr1_lpm_trie * trie,
    NlmReasonCode * o_reason)
{
    uint32_t i = 0, nPending = trie->m_num_items_in_to_update_sit;
    kaps_jr1_trie_node *lpmnode = NULL;

    for (i = 0; i < nPending; i++)
    {
        lpmnode = trie->m_toupdate_sit[i];
        NLM_STRY(kaps_jr1_trie_node_update_iit(lpmnode, o_reason));
    }

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_trie_compute_num_joined_sets(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t * numJoinedSets,
    uint32_t * numJoinedBricksInOneSet)
{
    *numJoinedBricksInOneSet = 1;
    *numJoinedSets = 0;

    return NLMERR_OK;
}

static NlmErrNum_t
kaps_jr1_trie_ctor(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_global * trie_global,
    void *fibTbl_p,
    NlmReasonCode * o_reason)
{
    kaps_jr1_nlm_allocator *alloc_p = NULL;
    kaps_jr1_fib_tbl *fibtbl = (kaps_jr1_fib_tbl *) fibTbl_p;
    struct kaps_jr1_device *device = trie_global->fibtblmgr->m_devMgr_p;
    kaps_jr1_fib_tbl_mgr *fibTblMgr = trie_global->fibtblmgr;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_status status;
    uint32_t init_hashtbl_size = 0, i = 0;
    struct kaps_jr1_db *main_db;
    uint32_t key_width_1;
    uint32_t numJoinedSets, numJoinedBricksInOneSet;

    kaps_jr1_assert(trie_p && trie_global && fibTbl_p, "Invalid Value passed ");

    trie_p->m_tbl_ptr = fibTbl_p;
    trie_p->m_trie_global = trie_global;
    alloc_p = trie_global->m_alloc_p;

    
    main_db = fibtbl->m_db;
    if (main_db->parent)
        main_db = main_db->parent;


    numJoinedSets = 0;
    numJoinedBricksInOneSet = 1;

    if (device->type == KAPS_JR1_DEVICE)
    {

        kaps_jr1_trie_compute_num_joined_sets(device, main_db, &numJoinedSets, &numJoinedBricksInOneSet);

        trie_p->m_lsn_settings_p = kaps_jr1_lsn_mc_settings_create(trie_global->m_alloc_p,
                                                               fibtbl,
                                                               fibTblMgr->m_devMgr_p,
                                                               &trie_global->m_mlpmemmgr[0],
                                                               fibTblMgr->m_indexChangeCallBackFn,
                                                               fibTblMgr->m_lpm_mgr,
                                                               main_db->hw_res.db_res->lsn_info[0].max_lsn_size,
                                                               numJoinedSets, numJoinedBricksInOneSet);

    }

    if (!trie_p->m_lsn_settings_p)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    
    for (i = 0; i < KAPS_JR1_TRIE_NUMSTACK; i++)
    {
        trie_p->m_recurse_stack[i] =
            (kaps_jr1_trie_node **) kaps_jr1_nlm_allocator_malloc(trie_global->m_alloc_p, sizeof(kaps_jr1_trie_node *) * 320);
        trie_p->m_recurse_stack_inuse[i] = 0;
        trie_p->m_recurse_args_stack[i] = (NlmNsTrieNode__TraverseArgs *)
            kaps_jr1_nlm_allocator_malloc(trie_global->m_alloc_p, sizeof(NlmNsTrieNode__TraverseArgs) * 320);
        trie_p->m_recurse_args_stack_inuse[i] = 0;
    }

    trie_p->m_ancestors_space_inuse = 0;
    trie_p->m_ancestors_space =
        (kaps_jr1_trie_node **) kaps_jr1_nlm_allocator_malloc(trie_global->m_alloc_p, sizeof(kaps_jr1_trie_node *) * 320);

    trie_p->m_doverify = (uint8_t) 0;

    key_width_1 = fibtbl->m_db->key->width_1;

    if (device->type == KAPS_JR1_DEVICE)
    {
        struct kaps_jr1_key_field *f;
        uint32_t table_id_width_1 = 0;

        for (f = fibtbl->m_db->key->first_field; f; f = f->next)
        {
            if (f->type == KAPS_JR1_KEY_FIELD_TABLE_ID)
            {
                table_id_width_1 = f->width_1;
            }
        }

        trie_p->m_min_lopoff = 0;

        if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
        {
            if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM4
                || device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
            {
                
                if (key_width_1 > KAPS_JR1_JERICHO_PLUS_FM4_MAX_KEY_WIDTH_1)
                {
                    trie_p->m_min_lopoff = key_width_1 - KAPS_JR1_JERICHO_PLUS_FM4_MAX_KEY_WIDTH_1;
                }
            }
        }

        if (table_id_width_1 > trie_p->m_min_lopoff)
            trie_p->m_min_lopoff = table_id_width_1;
    }
    else
    {
        trie_p->m_min_lopoff = NLMNS_TRIE_DEFAULT_LOPOFF;
    }

    
    
    trie_p->m_roots_trienode_p = kaps_jr1_trie_node_create_root_node(trie_p, 0);

    
    trie_p->m_expansion_depth = trie_p->m_min_lopoff;

    
    kaps_jr1_trie_node_pvt_expand_trie(trie_p->m_roots_trienode_p, trie_p->m_expansion_depth);

    if (trie_p->m_roots_trienode_p)
        trie_p->m_roots_trienode_p->m_node_type = NLMNSTRIENODE_LP;

    trie_p->m_cache_trienode_p = NULL;

    
    init_hashtbl_size = fibtbl->m_tblIndexRange.m_indexHighValue - fibtbl->m_tblIndexRange.m_indexLowValue + 1;

    if (fibtbl->m_tblIndexRange.m_indexHighValue == 0 && fibtbl->m_tblIndexRange.m_indexLowValue == 0)
    {
        if (fibtbl->m_db->common_info->capacity)
            init_hashtbl_size = fibtbl->m_db->common_info->capacity;
        else
            init_hashtbl_size = 1 * 1024 * 1024;
    }

    if (fibtbl->m_db->common_info->ad_info.ad)
    {
        struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) fibtbl->m_db->common_info->ad_info.ad;
        uint32_t total_ad_capacity = 0;

        while (ad_db)
        {
            total_ad_capacity += ad_db->db_info.common_info->capacity;
            ad_db = ad_db->next;
        }

        if (total_ad_capacity < 1024 * 1024)
            total_ad_capacity = 1024 * 1024;

        if (fibtbl->m_db->common_info->capacity > total_ad_capacity)
            total_ad_capacity = fibtbl->m_db->common_info->capacity;

        init_hashtbl_size = total_ad_capacity;
    }

    if (init_hashtbl_size < 1024 * 1024)
        init_hashtbl_size = 1024 * 1024;

    if (!fibtbl->m_cascaded_fibtbl)
    {
        status = kaps_jr1_pfx_hash_table_create(device->alloc, init_hashtbl_size, 70, 100,
                                       fibtbl->m_width, 0, fibtbl->m_db, &trie_p->m_hashtable_p);
        if (status != KAPS_JR1_OK)
        {
            errNum = kaps_jr1_trie_convert_status_to_err_num(status, o_reason);
            return errNum;
        }
    }

    trie_p->m_isCompactionEnabled = 1;
    trie_p->m_isCompactionRequired = 0;
    trie_p->m_isOverAllocEnabled = 0;
    trie_p->m_numOfMCHoles = 16;


    if (trie_p->m_lsn_settings_p->m_areIPTHitBitsPresent)
    {
        trie_p->m_iptHitBitBuffer =
            kaps_jr1_nlm_allocator_calloc(alloc_p, KAPS_JR1_RPB_MAX_BLOCK_SIZE, sizeof(uint8_t));
    }

    if (trie_p->m_lsn_settings_p->m_areRPTHitBitsPresent)
    {
        trie_p->m_rptTcamHitBitBuffer =
            kaps_jr1_nlm_allocator_calloc(alloc_p, KAPS_JR1_RPB_MAX_BLOCK_SIZE, sizeof(uint8_t));
    }

    return errNum;
}

static void
kaps_jr1_trie_dtor(
    kaps_jr1_lpm_trie * self)
{
    kaps_jr1_nlm_allocator *alloc;
    uint32_t i = 0;

    if (!self)
        return;

    alloc = self->m_trie_global->m_alloc_p;

    kaps_jr1_trie_node_destroy(self->m_roots_trienode_p, alloc, 1);

    
    for (i = 0; i < KAPS_JR1_TRIE_NUMSTACK; i++)
    {
        kaps_jr1_nlm_allocator_free(self->m_trie_global->m_alloc_p, self->m_recurse_stack[i]);
        kaps_jr1_nlm_allocator_free(self->m_trie_global->m_alloc_p, self->m_recurse_args_stack[i]);
    }

    kaps_jr1_nlm_allocator_free(self->m_trie_global->m_alloc_p, self->m_ancestors_space);

    
    if (self->m_hashtable_p)
        kaps_jr1_pfx_hash_table_destroy(self->m_hashtable_p);

    if (self->m_iptHitBitBuffer)
    {
        kaps_jr1_nlm_allocator_free(alloc, self->m_iptHitBitBuffer);
        self->m_iptHitBitBuffer = NULL;
    }

    if (self->m_rptTcamHitBitBuffer)
    {
        kaps_jr1_nlm_allocator_free(alloc, self->m_rptTcamHitBitBuffer);
        self->m_rptTcamHitBitBuffer = NULL;
    }

    if (self->m_lsn_settings_p)
        kaps_jr1_lsn_mc_settings_destroy(self->m_lsn_settings_p);
}

void
kaps_jr1_trie_destroy(
    kaps_jr1_lpm_trie * self)
{
    if (self)
    {
        kaps_jr1_trie_dtor(self);
        kaps_jr1_nlm_allocator_free(self->m_trie_global->m_alloc_p, self);
    }
}


kaps_jr1_trie_node *
kaps_jr1_trie_pvt_is_cache_hit(
    kaps_jr1_trie_node * trienode_p,
    uint8_t * pfxData,
    uint32_t pfx_len_1)
{
    if (trienode_p && (trienode_p->m_node_type == NLMNSTRIENODE_LP) && (pfx_len_1 >= trienode_p->m_depth))
    {
        uint8_t *cachePrefix = trienode_p->m_lp_prefix_p->m_data;
        uint32_t bits = trienode_p->m_depth;
        uint32_t isMatching = 1;
        int32_t len;

        
        len = (bits >> 3) - 1;

        for (; len >= 0; len--)
        {
            if (pfxData[len] != cachePrefix[len])
            {
                isMatching = 0;
                break;
            }
        }

        if (isMatching == 1)
        {
            uint8_t mask = 0;
            if (bits % 8)
            {
                mask = (uint8_t) (0xFF << (8 - (bits % 8)));

                if ((pfxData[bits >> 3] & mask) == (cachePrefix[bits >> 3] & mask))
                {
                    return trienode_p;
                }
            }
            else
                return trienode_p;
        }
    }

    return NULL;
}




NlmErrNum_t
kaps_jr1_trie_commit_iit_lmpsofar(
    kaps_jr1_lpm_trie * trie_p,
    NlmReasonCode * o_reason)
{
    NLM_STRY(kaps_jr1_trie_commit_iits(trie_p, o_reason));

    trie_p->m_num_items_in_to_update_sit = 0;

    return NLMERR_OK;
}


kaps_jr1_pfx_bundle *
kaps_jr1_trie_handle_exact_strict_lmpsofar_for_delete(
    kaps_jr1_lpm_trie *trie_p,
    kaps_jr1_trie_node *destnode,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason)
{
    kaps_jr1_trie_node *iternode;
    kaps_jr1_pfx_bundle *lmpsofarPfxBundle_p = NULL;
    uint32_t list_size;
    uint8_t *pfxData = entry->pfx_bundle->m_data;
    uint16_t pfxLen = entry->pfx_bundle->m_nPfxSize;
    struct kaps_jr1_ad_db *ad_db = NULL;
    struct kaps_jr1_device *device = trie_p->m_lsn_settings_p->m_device;
    NlmErrNum_t errNum = NLMERR_OK;

    (void) ad_db;
    (void) device;
    
    

    iternode = destnode->m_parent_p;

    while (iternode) 
    {
        
        if (iternode->m_node_type != NLMNSTRIENODE_LP)
        {
            iternode = iternode->m_parent_p;
            continue;
        }

        
        lmpsofarPfxBundle_p = kaps_jr1_lsn_mc_locate_lpm(iternode->m_lsn_p, pfxData, pfxLen, NULL, NULL, NULL);

        if (lmpsofarPfxBundle_p)
        {
              
            if (!lmpsofarPfxBundle_p->m_isLmpsofarPfx)
            {
                lmpsofarPfxBundle_p->m_isLmpsofarPfx = 1;
            }
        }
        else
        {
            
            lmpsofarPfxBundle_p = iternode->m_iitLmpsofarPfx_p;  
        }

        break;

        
    }

    
    destnode->m_iitLmpsofarPfx_p = lmpsofarPfxBundle_p;

    errNum = kaps_jr1_lsn_mc_update_iit(destnode->m_lsn_p, o_reason);

    if (errNum != NLMERR_OK)
        kaps_jr1_assert(0, "Update IIT failed when handling exact strict lmpsofar for delete \n");

    if (destnode->m_isRptNode)
    {
        list_size = trie_p->m_num_items_in_to_update_rit;

        if (list_size >= KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY)
            kaps_jr1_assert(0, "m_toupdate_rit overflow \n");

        trie_p->m_toupdate_rit[list_size] = destnode;
        trie_p->m_num_items_in_to_update_rit++;
    }

    return lmpsofarPfxBundle_p;
}

NlmErrNum_t
kaps_jr1_trie_process_iit_lmpsofar(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_node * destnode,
    struct kaps_jr1_lpm_entry * entry,
    NlmTblRqtCode rqtCode,
    NlmReasonCode * o_reason)
{
    kaps_jr1_pfx_bundle *lmpsofarPfxBundle_p = NULL;
    kaps_jr1_trie_node *iterTrieNode;
    uint16_t depth;
    uint8_t bit;
    uint8_t *pfxData = entry->pfx_bundle->m_data;
    uint16_t pfxLen = entry->pfx_bundle->m_nPfxSize;
    kaps_jr1_trie_global *trie_global = trie_p->m_trie_global;
    struct kaps_jr1_trie_node *ancestorIptNodeWithEntry;
    uint32_t list_size;

    if (rqtCode == NLM_FIB_PREFIX_INSERT || rqtCode == NLM_FIB_PREFIX_INSERT_WITH_INDEX)
    {
        
        if (entry->pfx_bundle->m_isPfxCopy)
        {
            return NLMERR_OK;
        }
        lmpsofarPfxBundle_p = entry->pfx_bundle;

        if (destnode->m_lsn_p->m_pSettings->m_strictlyStoreLmpsofarInAds 
            && destnode->m_depth == entry->pfx_bundle->m_nPfxSize
            && destnode->m_isRptNode)
        {
            list_size = trie_p->m_num_items_in_to_update_rit;

            if (list_size >= KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY)
                kaps_jr1_assert(0, "m_toupdate_rit overflow \n");

            trie_p->m_toupdate_rit[list_size] = destnode;
            trie_p->m_num_items_in_to_update_rit++;
        }
    }
    else if (rqtCode == NLM_FIB_PREFIX_DELETE)
    {
        
        if (destnode->m_lsn_p->m_pSettings->m_strictlyStoreLmpsofarInAds 
            && destnode->m_depth == entry->pfx_bundle->m_nPfxSize)
        {
            lmpsofarPfxBundle_p = kaps_jr1_trie_handle_exact_strict_lmpsofar_for_delete(
                                        trie_p, destnode, entry, o_reason);
            
        }
        else
        {
            
            lmpsofarPfxBundle_p = kaps_jr1_lsn_mc_locate_lpm(destnode->m_lsn_p, pfxData, pfxLen, NULL, NULL, NULL);

            if (!lmpsofarPfxBundle_p || lmpsofarPfxBundle_p->m_isPfxCopy)
            {
                
                lmpsofarPfxBundle_p = destnode->m_iitLmpsofarPfx_p;
            }
        }
        
        
    }
    else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD)
    {
        lmpsofarPfxBundle_p = entry->pfx_bundle;

        if (destnode->m_lsn_p->m_pSettings->m_strictlyStoreLmpsofarInAds 
            && destnode->m_depth == entry->pfx_bundle->m_nPfxSize)
        {
            
            list_size = trie_p->m_num_items_in_to_update_sit;

            if (list_size >= KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY)
                kaps_jr1_assert(0, "m_toupdate_sit overflow \n");

            trie_p->m_toupdate_sit[list_size] = destnode;
            trie_p->m_num_items_in_to_update_sit++;

            if (destnode->m_isRptNode)
            {
                list_size = trie_p->m_num_items_in_to_update_rit;

                if (list_size >= KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY)
                    kaps_jr1_assert(0, "m_toupdate_rit overflow \n");

                trie_p->m_toupdate_rit[list_size] = destnode;
                trie_p->m_num_items_in_to_update_rit++;
            }
        }
    }
    else
    {
        return NLMERR_FAIL;
    }

    
    depth = destnode->m_depth;
    iterTrieNode = destnode;
    ancestorIptNodeWithEntry = destnode;
    while (depth < pfxLen && iterTrieNode)
    {
        bit = KAPS_JR1_PREFIX_PVT_GET_BIT(pfxData, pfxLen, depth);
        iterTrieNode = iterTrieNode->m_child_p[bit];
        depth++;

        if (iterTrieNode && iterTrieNode->m_node_type == NLMNSTRIENODE_LP)
            ancestorIptNodeWithEntry = iterTrieNode;
    }

    
    if (iterTrieNode)
    {
        kaps_jr1_trie_node_update_iitLmpsofar(iterTrieNode->m_child_p[0], ancestorIptNodeWithEntry, &lmpsofarPfxBundle_p, entry->pfx_bundle,
                                          rqtCode);
        kaps_jr1_trie_node_update_iitLmpsofar(iterTrieNode->m_child_p[1], ancestorIptNodeWithEntry, &lmpsofarPfxBundle_p, entry->pfx_bundle,
                                          rqtCode);
    }

    if (trie_global->m_isIITLmpsofar)
    {
        NLM_STRY(kaps_jr1_trie_commit_iit_lmpsofar(trie_p, o_reason));
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_get_db_details(
    struct kaps_jr1_device *device,
    uint32_t * actual_num_entries_in_dev_p,
    uint32_t * normalized_num_entries_p,
    uint32_t * num_dbs,
    uint32_t * is_public_db_present)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_db *db, *tab;
    struct kaps_jr1_lpm_db *lpm_db;
    uint32_t normalized_num_entries;

    *actual_num_entries_in_dev_p = 0;
    normalized_num_entries = 0;
    *num_dbs = 0;
    *is_public_db_present = 0;

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_JR1_DB_LPM)
            continue;
        if (db->is_bc_required && (!db->is_main_bc_db))
            continue;

        (*num_dbs)++;

        if (db->is_public)
            *is_public_db_present = 1;

        tab = db;

        while (tab)
        {
            if (tab->is_clone)
            {
                tab = tab->next_tab;
                continue;
            }

            lpm_db = (struct kaps_jr1_lpm_db *) tab;

            *actual_num_entries_in_dev_p += lpm_db->fib_tbl->m_numPrefixes;

            if (tab->key->width_1 < 88)
                normalized_num_entries += lpm_db->fib_tbl->m_numPrefixes;
            else if (tab->key->width_1 < 128)
                normalized_num_entries += 2 * lpm_db->fib_tbl->m_numPrefixes;
            else
                normalized_num_entries += 4 * lpm_db->fib_tbl->m_numPrefixes;

            tab = tab->next_tab;
        }
    }

    if (normalized_num_entries_p)
        *normalized_num_entries_p = normalized_num_entries;

    return NLMERR_OK;
}

uint32_t
kaps_jr1_trie_check_sufficient_space_for_lmpsofar_ix(
    kaps_jr1_lpm_trie * trie,
    struct kaps_jr1_db *db,
    struct kaps_jr1_lpm_entry * entry)
{
    uint32_t is_ix_space_sufficient = 1;

    return is_ix_space_sufficient;
    
}

NlmErrNum_t
kaps_jr1_trie_clear_hit_bit_for_new_entry(
    kaps_jr1_lpm_trie * trie_p,
    struct kaps_jr1_lpm_entry * lpm_entry)
{
    struct kaps_jr1_device *device = trie_p->m_tbl_ptr->m_db->device;
    uint32_t hb_index;
    struct kaps_jr1_db *db = trie_p->m_tbl_ptr->m_db;
    struct kaps_jr1_hb_db *hb_db;
    struct kaps_jr1_hb *hb = NULL;
    struct kaps_jr1_aging_entry *active_aging_table;

    if (device->type != KAPS_JR1_DEVICE)
        return NLMERR_OK;

    if (db->parent)
        db = db->parent;

    hb_db = (struct kaps_jr1_hb_db *) db->common_info->hb_info.hb;

    if (hb_db && lpm_entry->hb_user_handle != 0)
    {
        active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);

        KAPS_JR1_WB_HANDLE_READ_LOC((&hb_db->db_info), (&hb), (uintptr_t) lpm_entry->hb_user_handle);

        if (hb)
        {
            hb_index = hb->bit_no;

            active_aging_table[hb_index].entry = NULL;

            hb->bit_no = 0;
        }
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_process_first_lsn_in_path(
    kaps_jr1_lpm_trie *trie_p, 
    kaps_jr1_trie_node *destnode,
    struct kaps_jr1_lpm_entry * entry,
    NlmReasonCode *o_reason)
    
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    
    if (!destnode->m_rptParent_p)
    {
        errNum = kaps_jr1_trie_compute_rpt_parent(destnode, o_reason);
        if (errNum != NLMERR_OK)
        {
            kaps_jr1_lsn_mc_destroy(destnode->m_lsn_p);
            destnode->m_lsn_p = kaps_jr1_trie_node_pvt_create_lsn(destnode);

            if (*o_reason != NLMRSC_LOW_MEMORY)
                *o_reason = NLMRSC_DBA_ALLOC_FAILED;

            entry->pfx_bundle->m_nIndex = KAPS_JR1_LSN_NEW_INDEX;
            kaps_jr1_trie_clear_hit_bit_for_new_entry(trie_p, entry);

            return errNum;
        }
    }

    
    if (kaps_jr1_pool_mgr_get_num_entries(poolMgr, destnode->m_rptParent_p->m_poolId, KAPS_JR1_IPT_POOL)
        >= kaps_jr1_pool_mgr_get_max_entries(poolMgr, destnode->m_rptParent_p->m_poolId, KAPS_JR1_IPT_POOL))
    {

        kaps_jr1_lsn_mc_destroy(destnode->m_lsn_p);
        destnode->m_lsn_p = kaps_jr1_trie_node_pvt_create_lsn(destnode);

        entry->pfx_bundle->m_nIndex = KAPS_JR1_LSN_NEW_INDEX;
        kaps_jr1_trie_clear_hit_bit_for_new_entry(trie_p, entry);

        *o_reason = NLMRSC_DBA_ALLOC_FAILED;
        return NLMERR_FAIL;
    }

    errNum = kaps_jr1_trie_add_ses(trie_p, destnode, destnode->m_lp_prefix_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    destnode->m_lsn_p->m_bIsNewLsn = 0;

    return NLMERR_OK;
}


NlmErrNum_t
kaps_jr1_trie_submit_rqtInner(
    kaps_jr1_lpm_trie * trie_p,
    NlmTblRqtCode rqtCode,
    struct kaps_jr1_lpm_entry * entry,
    NlmReasonCode * o_reason)
{
    kaps_jr1_trie_node *destnode = NULL;
    kaps_jr1_trie_node *lastseen_node_p = NULL;
    kaps_jr1_trie_node *temp_node_p = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_jr1_lpm_entry **entrySlotInHash = NULL;
    uint8_t *pfxData = NULL;
    kaps_jr1_trie_global *trie_global;
    struct kaps_jr1_device *device = NULL;
    kaps_jr1_status status;
    kaps_jr1_pool_mgr *poolMgr;
    uint32_t i;
    uint32_t wasGiveoutPerformed = 0;
    struct kaps_jr1_db *db;
    uint32_t isLsnEmpty;

    if (!trie_p || !entry)
    {
        *o_reason = NLMRSC_INVALID_PARAM;
        return NLMERR_FAIL;
    }

    db = trie_p->m_tbl_ptr->m_db;

    trie_global = trie_p->m_trie_global;
    poolMgr = trie_global->poolMgr;

    device = trie_global->fibtblmgr->m_devMgr_p;

    (void) db;
    (void) poolMgr;

    if (device->main_dev)
        device = device->main_dev;

    trie_p->m_lsn_settings_p->m_device = device;

    trie_p->m_trie_global->m_numTrieNodesForMerge = 0;

    if (rqtCode == NLM_FIB_PREFIX_INSERT)
    {
        if (!kaps_jr1_trie_check_sufficient_space_for_lmpsofar_ix(trie_p, db, entry))
        {
            *o_reason = NLMRSC_OUT_OF_AD;
            return NLMERR_FAIL;
        }
    }

    trie_global->cur_ad_db = NULL;
    if (entry->ad_handle)
    {
        KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, trie_global->cur_ad_db);
    }


    pfxData = entry->pfx_bundle->m_data;
    trie_global->curLsnToSplit = NULL;
    for (i = 0; i < KAPS_JR1_MAX_NUM_STORED_LSN_INFO; ++i)
    {
        trie_p->m_lsn_settings_p->m_isLsnInfoValid[i] = 0;
    }

    if (rqtCode == NLM_FIB_PREFIX_DELETE)
        trie_global->m_numDeletes++;

    
    if (trie_p->m_hashtable_p)
    {
        struct kaps_jr1_entry **ht_slot;

        status = kaps_jr1_pfx_hash_table_locate(trie_p->m_hashtable_p, pfxData, entry->pfx_bundle->m_nPfxSize, &ht_slot);
        if (status != KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }

        entrySlotInHash = (struct kaps_jr1_lpm_entry **) ht_slot;

        switch (rqtCode)
        {
            case NLM_FIB_PREFIX_INSERT_WITH_INDEX:
                if (entrySlotInHash && !entry->pfx_bundle->m_isPfxCopy)
                {
                    
                    *o_reason = NLMRSC_DUPLICATE_PREFIX;
                    return NLMERR_FAIL;
                }
                break;
            case NLM_FIB_PREFIX_INSERT:
            case NLM_FIB_PREFIX_DELETE:
            case NLM_FIB_PREFIX_UPDATE_AD:
            case NLM_FIB_PREFIX_UPDATE_AD_ADDRESS:
                if (!entrySlotInHash)
                {
                    
                    *o_reason = NLMRSC_PREFIX_NOT_FOUND;
                    return NLMERR_FAIL;
                }
                break;
        }
    }

    kaps_jr1_seq_num_gen_generate_next_pfx_seq_nr();

    switch (rqtCode)
    {
        case NLM_FIB_PREFIX_INSERT:
        case NLM_FIB_PREFIX_INSERT_WITH_INDEX:

            temp_node_p =
                kaps_jr1_trie_pvt_is_cache_hit(trie_p->m_cache_trienode_p, pfxData, entry->pfx_bundle->m_nPfxSize);
            if (!temp_node_p)
                temp_node_p = trie_p->m_roots_trienode_p;

            
            destnode =
                kaps_jr1_trie_find_dest_lp_node(trie_p, temp_node_p, pfxData, entry->pfx_bundle->m_nPfxSize,
                                            &lastseen_node_p, 1);

            trie_p->m_cache_trienode_p = destnode;

            break;
        case NLM_FIB_PREFIX_DELETE:
        case NLM_FIB_PREFIX_UPDATE_AD:
        case NLM_FIB_PREFIX_UPDATE_AD_ADDRESS:
        {
            
            destnode = kaps_jr1_trie_find_dest_lp_node(trie_p, trie_p->m_roots_trienode_p, pfxData,
                                                   entry->pfx_bundle->m_nPfxSize, &lastseen_node_p, 0);
            trie_p->m_cache_trienode_p = NULL;

            break;
        }
    }

    if (!destnode)
    {
        *o_reason = NLMRSC_LOCATE_NODE_FAILED;
        return NLMERR_FAIL;
    }

    if (destnode->m_lsn_p->m_bIsNewLsn)
    {
        errNum = kaps_jr1_trie_check_resources_for_new_lsn(destnode, o_reason);
        if (errNum != NLMERR_OK)
            return errNum;
    }

    
    errNum = kaps_jr1_lsn_mc_submit_rqt(destnode->m_lsn_p, rqtCode, entry, entrySlotInHash, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    if (rqtCode == NLM_FIB_PREFIX_INSERT)
    {
        if (destnode->m_lsn_p->m_bIsNewLsn)
        {
            errNum = kaps_jr1_process_first_lsn_in_path(trie_p, destnode, entry, o_reason);
            if (errNum != NLMERR_OK)
            {
                return errNum;
            }
                
        }
        else if (destnode->m_lsn_p->m_bDoGiveout)
        {
            destnode->m_lsn_p->m_bDoGiveout = 0;

            errNum = kaps_jr1_trie_giveout_pending_lsns(trie_p, destnode->m_lsn_p, entry, 0, o_reason);
            if (errNum != NLMERR_OK)
            {
                return errNum;
            }

            wasGiveoutPerformed = 1;

            errNum = kaps_jr1_trie_update_hardware(trie_p, entry, o_reason);

            if (errNum != NLMERR_OK)
            {
                return errNum;
            }
            trie_global->curLsnToSplit = NULL;
        }

        if (trie_global->m_isIITLmpsofar)
            errNum = kaps_jr1_trie_process_iit_lmpsofar(trie_p, destnode, entry, rqtCode, o_reason);

        if (wasGiveoutPerformed)
        {
            kaps_jr1_trie_process_merge_after_giveout(trie_p);
        }

    }
    else if (rqtCode == NLM_FIB_PREFIX_DELETE)
    {
        if (trie_global->m_isIITLmpsofar)
            errNum = kaps_jr1_trie_process_iit_lmpsofar(trie_p, destnode, entry, rqtCode, o_reason);

        isLsnEmpty = kaps_jr1_trie_is_lsn_empty(trie_p, destnode);

        if (isLsnEmpty)
        {
            
            NLM_STRY(kaps_jr1_trie_pvt_in_place_delete(trie_p, destnode, 1, o_reason));
        }
        else
        {
            NLM_STRY(kaps_jr1_trie_merge_ancestor_lsns(trie_p, destnode, o_reason));
        }
    }
    else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD)
    {
        if (trie_global->m_isIITLmpsofar)
            errNum = kaps_jr1_trie_process_iit_lmpsofar(trie_p, destnode, entry, rqtCode, o_reason);

    }
    else if (rqtCode == NLM_FIB_PREFIX_INSERT_WITH_INDEX)
    {
        if (trie_global->m_isIITLmpsofar)
            errNum = kaps_jr1_trie_process_iit_lmpsofar(trie_p, destnode, entry, rqtCode, o_reason);
    }

    trie_p->m_isRecursiveSplit = 0;

    return errNum;
}


extern NlmErrNum_t
kaps_jr1_trie_submit_rqt(
    kaps_jr1_lpm_trie * trie_p,
    NlmTblRqtCode rqtCode,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t err = NLMERR_OK;
    uint32_t retry_with_rpt_split;
    uint32_t doSubmitRqt;
    uint32_t maxRetryCnt, curRetryCnt;
    uint32_t isTblFull = 0;
    struct kaps_jr1_db *db = trie_p->m_tbl_ptr->m_db;

    db->rqt_cnt++;

    

    trie_p->m_rptSplitReasonCode = NLMRSC_REASON_OK;
    trie_p->m_isRptSplitDueToIpt = 0;
    trie_p->m_isRptSplitDueToApt = 0;
    trie_p->m_tbl_ptr->m_fibTblMgr_p->m_bufferKapsABWrites = 0;
    trie_p->m_trie_global->m_move_reserved_160b_trig_to_regular_pool = 0;

    maxRetryCnt = 5;
    curRetryCnt = 0;
    doSubmitRqt = 1;
    retry_with_rpt_split = 0;

    while (doSubmitRqt)
    {
        curRetryCnt++;

        err = kaps_jr1_trie_submit_rqtInner(trie_p, rqtCode, entry, o_reason);

        if (*o_reason == NLMRSC_REASON_OK || *o_reason == NLMRSC_RETRY_WITH_RPT_SPLIT)
        {

            retry_with_rpt_split = 0;
            if (*o_reason == NLMRSC_RETRY_WITH_RPT_SPLIT)
            {
                *o_reason = NLMRSC_REASON_OK;
                retry_with_rpt_split = 1;
            }

        }
        else
        {
            break;
        }

        trie_p->m_isRptSplitDueToIpt = 0;
        trie_p->m_isRptSplitDueToApt = 0;

        doSubmitRqt = 0;
        if (retry_with_rpt_split && curRetryCnt < maxRetryCnt)
            doSubmitRqt = 1;

        trie_p->m_rpt_move_failed_during_ipt_gran_change = 0;
    }

    if (retry_with_rpt_split)
    {
        if (*o_reason == NLMRSC_REASON_OK)
        {
            *o_reason = NLMRSC_DBA_ALLOC_FAILED;
        }

        if (trie_p->m_rptSplitReasonCode != NLMRSC_REASON_OK)
        {
            *o_reason = trie_p->m_rptSplitReasonCode;
        }

        if (trie_p->m_trie_global->m_mlpmemmgr[0]->num_allocated_lpu_bricks * 100 / 
                trie_p->m_trie_global->m_mlpmemmgr[0]->total_lpu_bricks > 95) 
        {
            *o_reason = NLMRSC_UDA_ALLOC_FAILED;
        }
    }

    if (curRetryCnt > trie_p->m_tbl_ptr->m_fibStats.maxNumRptSplitsInOneOperation)
        trie_p->m_tbl_ptr->m_fibStats.maxNumRptSplitsInOneOperation = curRetryCnt;

    if (curRetryCnt > 1)
    {
        trie_p->m_tbl_ptr->m_fibStats.numRecursiveRptSplits++;
    }

    trie_p->m_rpt_move_failed_during_ipt_gran_change = 0;
    trie_p->m_numExtraIptEntriesNeededDuringGiveout = 0;

    (void) isTblFull;

    if (*o_reason == NLMRSC_PCM_ALLOC_FAILED ||
        *o_reason == NLMRSC_DBA_ALLOC_FAILED ||
        *o_reason == NLMRSC_IT_ALLOC_FAILED || 
        *o_reason == NLMRSC_UDA_ALLOC_FAILED || 
        *o_reason == NLMRSC_OUT_OF_AD)
    {
        isTblFull = 1;
        
    }

    return err;
}


 
static void
kaps_jr1_trie_pvt_cleanup_del_nodes(
    kaps_jr1_lpm_trie * trie_p)
{
    kaps_jr1_lsn_mc *lsn_p = NULL;
    uint32_t i = 0, max = 0;

    
    
    max = trie_p->m_num_items_in_to_delete_lsns;
    for (i = 0; i < max; i++)
    {
        lsn_p = trie_p->m_todelete_lsns[i];
        kaps_jr1_lsn_mc_destroy(lsn_p);
    }
    trie_p->m_num_items_in_to_delete_lsns = 0;

    
    max = trie_p->m_num_items_in_to_delete_nodes;
    for (i = 0; i < max; i++)
    {
        kaps_jr1_trie_node *node_p = trie_p->m_todelete_nodes[i];
        kaps_jr1_trie_node_destroy(node_p, trie_p->m_trie_global->m_alloc_p, 0);
    }
    trie_p->m_num_items_in_to_delete_nodes = 0;
}



kaps_jr1_trie_node *
kaps_jr1_trie_find_dest_lp_node(
    kaps_jr1_lpm_trie * self,
    kaps_jr1_trie_node * start_node,
    const uint8_t * pfxdata,
    uint32_t pfxlen,
    kaps_jr1_trie_node ** last_seen_node,
    int32_t isPfxInsertion)
{
    kaps_jr1_trie_node *node = start_node;
    kaps_jr1_trie_node *remember_lp = node;
    kaps_jr1_trie_node *last_node = node;
    uint32_t i = 0, val = 0;
    uint32_t chosen_depth;

    if (pfxlen < self->m_min_lopoff)
        kaps_jr1_assert(0, "Prefix should be >= minimum threshold\n");

    chosen_depth = self->m_expansion_depth;

    for (i = node->m_depth; (i < pfxlen); i++)
    {
        val = KAPS_JR1_PREFIX_PVT_GET_BIT(pfxdata, pfxlen, i);

        node = node->m_child_p[val];
        if (!node)
            break;

        if (node->m_node_type != NLMNSTRIENODE_REGULAR)
        {
            remember_lp = node;
        }
        else if (isPfxInsertion && node->m_isRptNode)
        {
            remember_lp = node;
        }
        else if (isPfxInsertion && remember_lp == self->m_roots_trienode_p && node->m_depth == chosen_depth)
        {
            remember_lp = node;
        }

        last_node = node;
    }

    if (last_seen_node)
        *last_seen_node = last_node;

    return remember_lp;
}


void
kaps_jr1_trie_return_stack_space(
    kaps_jr1_lpm_trie * self,
    kaps_jr1_trie_node ** stack)
{
    if (stack == self->m_recurse_stack[0])
    {
        kaps_jr1_assert(self->m_recurse_stack_inuse[0] == 1, "Stack inuse field improper");
        self->m_recurse_stack_inuse[0] = 0;
    }
    else if (stack == self->m_recurse_stack[1])
    {
        kaps_jr1_assert(self->m_recurse_stack_inuse[1] == 1, "Stack inuse field improper");
        self->m_recurse_stack_inuse[1] = 0;
    }
    else if (stack == self->m_recurse_stack[2])
    {
        kaps_jr1_assert(self->m_recurse_stack_inuse[2] == 1, "Stack inuse field improper");
        self->m_recurse_stack_inuse[2] = 0;
    }
    else
        kaps_jr1_assert(0, "Invalid stack pointer ");
}


kaps_jr1_trie_node **
kaps_jr1_trie_get_stack_space(
    kaps_jr1_lpm_trie * self)
{
    if (self->m_recurse_stack_inuse[0])
    {
        if (self->m_recurse_stack_inuse[1])
        {
            if (self->m_recurse_stack_inuse[2])
            {
                kaps_jr1_assert(0, "Stack space not available");
                return 0;
            }
            else
            {
                self->m_recurse_stack_inuse[2] = 1;
                return self->m_recurse_stack[2];
            }
        }
        else
        {
            self->m_recurse_stack_inuse[1] = 1;
            return self->m_recurse_stack[1];
        }
    }
    else
    {
        self->m_recurse_stack_inuse[0] = 1;
        return self->m_recurse_stack[0];
    }
}


NlmNsTrieNode__TraverseArgs *
kaps_jr1_trie_node_get_stack_space_args(
    kaps_jr1_lpm_trie * self)
{
    if (self->m_recurse_args_stack_inuse[0])
    {
        if (self->m_recurse_args_stack_inuse[1])
        {
            if (self->m_recurse_args_stack_inuse[2])
            {
                kaps_jr1_assert(0, "All the stacks are in use");
                return 0;
            }
            else
            {
                self->m_recurse_args_stack_inuse[2] = 1;
                return self->m_recurse_args_stack[2];
            }
        }
        else
        {
            self->m_recurse_args_stack_inuse[1] = 1;
            return self->m_recurse_args_stack[1];
        }
    }
    else
    {
        self->m_recurse_args_stack_inuse[0] = 1;
        return self->m_recurse_args_stack[0];
    }
}


void
kaps_jr1_trie_node_return_stack_space_args(
    kaps_jr1_lpm_trie * self,
    NlmNsTrieNode__TraverseArgs * stk)
{
    if (stk == self->m_recurse_args_stack[0])
    {
        kaps_jr1_assert(self->m_recurse_args_stack_inuse[0] == 1, "Must be in use to return stack space");
        self->m_recurse_args_stack_inuse[0] = 0;
    }
    else if (stk == self->m_recurse_args_stack[1])
    {
        kaps_jr1_assert(self->m_recurse_args_stack_inuse[1] == 1, "Must be in use to return stack space");
        self->m_recurse_args_stack_inuse[1] = 0;
    }
    else if (stk == self->m_recurse_args_stack[2])
    {
        kaps_jr1_assert(self->m_recurse_args_stack_inuse[2] == 1, "Must be in use to return stack space");
        self->m_recurse_args_stack_inuse[2] = 0;
    }
    else
        kaps_jr1_assert(0, "Junk stack passed");
}

static NlmErrNum_t
kaps_jr1_trie_pvt_delete_ses_in_all_dev(
    kaps_jr1_lpm_trie * self,
    kaps_jr1_pfx_bundle * pfxBundle,
    uint32_t isMoveToNewPoolAllowed,
    NlmReasonCode * o_reason)
{
    uint8_t *tempPtr = NULL;
    kaps_jr1_trie_node *trienode = NULL, *rptParent_p = NULL;
    int32_t rptId = 0, rptDepth = 0;
    uint16_t numRptBitsLoppedOff = 0;
    kaps_jr1_pool_mgr *poolMgr = self->m_trie_global->poolMgr;
    kaps_jr1_lsn_mc_settings *settings = self->m_lsn_settings_p;
    struct kaps_jr1_device *device = self->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
    kaps_jr1_ipm *ipm_p;
    uint32_t actualPoolId;

    (void) o_reason;

    tempPtr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle);
    kaps_jr1_memcpy(&trienode, tempPtr, sizeof(kaps_jr1_trie_node *));

    rptParent_p = trienode->m_rptParent_p;

    if (!rptParent_p->m_isRptNode)
        kaps_jr1_assert(0, "Could not find the Rpt node \n");

    if (trienode->m_iitLmpsofarPfx_p)
    {
        if (settings->m_areHitBitsPresent && trienode->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
        {
            struct kaps_jr1_db *db;
            struct kaps_jr1_hb *hb = NULL;

            KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, trienode->m_iitLmpsofarPfx_p->m_backPtr, db)
                KAPS_JR1_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb),
                                        (uintptr_t) trienode->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

            
            if (hb)
            {
                struct kaps_jr1_aging_entry *active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);

                active_aging_table[hb->bit_no].num_idles = KAPS_JR1_HB_SPECIAL_VALUE;
            }
        }


        trienode->m_iitLmpsofarPfx_p = NULL;

    }

    
    actualPoolId = rptParent_p->m_poolId;

    kaps_jr1_trie_decr_num_ipt_entries_in_subtrie(rptParent_p, 1);

    rptId = rptParent_p->m_rptId;
    rptDepth = rptParent_p->m_depth;
    kaps_jr1_pool_mgr_decr_num_entries(poolMgr, actualPoolId, 1, KAPS_JR1_IPT_POOL);

    trienode->m_node_type = NLMNSTRIENODE_REGULAR;
    trienode->m_isReserved160bTrig = 0;

    ipm_p = kaps_jr1_pool_mgr_get_ipm_for_pool(poolMgr, actualPoolId);

    if (pfxBundle->m_nIndex != KAPS_JR1_LSN_NEW_INDEX)
    {
        numRptBitsLoppedOff = kaps_jr1_trie_get_num_rpt_bits_lopped_off(device, rptDepth);

        NLM_STRY(kaps_jr1_ipm_delete_entry_in_hw
                 (ipm_p, pfxBundle->m_nIndex, rptId, trienode->m_depth - numRptBitsLoppedOff, o_reason));

        NLM_STRY(kaps_jr1_ipm_remove_entry
                 (ipm_p, pfxBundle->m_nIndex, rptId, trienode->m_depth - numRptBitsLoppedOff, o_reason));


        pfxBundle->m_nIndex = KAPS_JR1_LSN_NEW_INDEX;

    }
    else
    {
        kaps_jr1_assert(0, "Index value of the IPT pfx bundle to be deleted is KAPS_JR1_LSN_NEW_INDEX \n");
    }

    return NLMERR_OK;
}


static NlmErrNum_t
kaps_jr1_trie_pvt_in_place_delete(
    kaps_jr1_lpm_trie * self,
    kaps_jr1_trie_node * node,
    uint32_t isMoveToNewPoolAllowed,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc *lsn_p;
    kaps_jr1_lpm_trie *trie_p = self;

    if (node->m_node_type != NLMNSTRIENODE_LP)
        return NLMERR_OK;

    lsn_p = node->m_lsn_p;


    if (node->m_node_type == NLMNSTRIENODE_LP)
    {
        NLM_STRY(kaps_jr1_trie_pvt_delete_ses_in_all_dev(self, node->m_lp_prefix_p, isMoveToNewPoolAllowed, o_reason));

        lsn_p = node->m_lsn_p;
        kaps_jr1_lsn_mc_destroy(lsn_p);
        node->m_lsn_p = kaps_jr1_trie_node_pvt_recreate_lsn(node);
        node->m_node_type = NLMNSTRIENODE_REGULAR;

        node->m_iitLmpsofarPfx_p = NULL;
    }

    kaps_jr1_trie_node_remove_child_path(node);
    kaps_jr1_trie_pvt_cleanup_del_nodes(trie_p);


    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_check_resources_for_new_lsn(
    kaps_jr1_trie_node * trienode,
    NlmReasonCode * o_reason)
{
    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_compute_rpt_parent(
    kaps_jr1_trie_node * trienode,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_trie_global *trieGlobal = trienode->m_trie_p->m_trie_global;

    
    kaps_jr1_trie_node *root_node = trienode->m_trie_p->m_roots_trienode_p;
    trienode->m_rptParent_p = root_node;
    if (!root_node->m_isRptNode)
    {
        root_node->m_isRptNode = 1;
        root_node->m_rptParent_p = root_node;

        errNum = kaps_jr1_trie_pvt_get_new_pool(trieGlobal->poolMgr, root_node, 1, 0, 1, o_reason);

        if (errNum != NLMERR_OK)
        {
            return NLMERR_FAIL;
        }
    }
    
    return NLMERR_OK;

}

NlmErrNum_t
kaps_jr1_trie_add_ses(
    kaps_jr1_lpm_trie * self,
    kaps_jr1_trie_node * trienode,
    kaps_jr1_pfx_bundle * pfxBundle,
    NlmReasonCode * o_reason)
{
    
    kaps_jr1_trie_node *rptParent_p = NULL;
    kaps_jr1_ipm *ipm_p = NULL;
    uint16_t numRptBitsLoppedOff = 0;
    kaps_jr1_pool_mgr *poolMgr = self->m_trie_global->poolMgr;
    struct kaps_jr1_device *device = poolMgr->fibTblMgr->m_devMgr_p;
    uint32_t actualPoolId;

    


    rptParent_p = trienode->m_rptParent_p;

    numRptBitsLoppedOff = kaps_jr1_trie_get_num_rpt_bits_lopped_off(device, rptParent_p->m_depth);

    trienode->m_node_type = NLMNSTRIENODE_LP;

    trienode->m_poolId = rptParent_p->m_poolId;

    actualPoolId = rptParent_p->m_poolId;

    kaps_jr1_trie_incr_num_ipt_entries_in_subtrie(rptParent_p, 1);

    
    kaps_jr1_pool_mgr_incr_num_entries(poolMgr, actualPoolId, 1, KAPS_JR1_IPT_POOL);

    ipm_p = kaps_jr1_pool_mgr_get_ipm_for_pool(poolMgr, actualPoolId);

    NLM_STRY(kaps_jr1_ipm_add_entry(ipm_p, pfxBundle, rptParent_p->m_rptId,
                                trienode->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, o_reason));


    NLM_STRY(kaps_jr1_ipm_delete_entry_in_hw(ipm_p, pfxBundle->m_nIndex, rptParent_p->m_rptId,
                                         trienode->m_depth - numRptBitsLoppedOff, o_reason));

    NLM_STRY(kaps_jr1_lsn_mc_commitIIT(trienode->m_lsn_p, pfxBundle->m_nIndex, o_reason));

    NLM_STRY(kaps_jr1_ipm_write_entry_to_hw(ipm_p, pfxBundle, rptParent_p->m_rptId,
                                        trienode->m_depth - numRptBitsLoppedOff, numRptBitsLoppedOff, o_reason));

    return NLMERR_OK;
}

void
kaps_jr1_trie_process_merge_after_giveout(
    kaps_jr1_lpm_trie * self)
{
    uint32_t loopvar, j;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    uint32_t numTrieNodesForMerge = self->m_trie_global->m_numTrieNodesForMerge;

    for (loopvar = 0; loopvar < numTrieNodesForMerge; loopvar++)
    {
        kaps_jr1_trie_node *trieNode1, *trieNode2, *newTrieNode;

        if (self->m_lsn_settings_p->m_isRelatedLsnMergeEnabled)
        {

            trieNode1 = self->m_trie_global->m_trieNodesForRelatedMerge[loopvar];

            if (trieNode1 && kaps_jr1_trie_check_if_related_ipt_nodes_can_be_merged(trieNode1, &trieNode2, &newTrieNode))
            {

                
                kaps_jr1_trie_merge_related_lsns(self, trieNode1, trieNode2, newTrieNode, &reason);

                
                for (j = 0; j < numTrieNodesForMerge; ++j)
                {
                    if (self->m_trie_global->m_trieNodesForRelatedMerge[j] == trieNode2)
                    {
                        self->m_trie_global->m_trieNodesForRelatedMerge[j] = NULL;
                    }
                }
            }
        }

        self->m_trie_global->m_trieNodesForRelatedMerge[loopvar] = NULL;
    }

    self->m_trie_global->m_numTrieNodesForMerge = 0;
}

static NlmErrNum_t
kaps_jr1_trie_update_hardware(
    kaps_jr1_lpm_trie * self,
    struct kaps_jr1_lpm_entry *entry,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc *lsn_p = NULL;
    kaps_jr1_pfx_bundle *bundle = NULL;
    uint32_t max = 0, loopvar = 0;
    kaps_jr1_pfx_bundle *pfxBundle = NULL;
    uint8_t *tempPtr = NULL;
    kaps_jr1_trie_node *trienode = NULL;
    NlmErrNum_t errNum = NLMERR_OK;
    kaps_jr1_trie_node *delayedNode;
    kaps_jr1_lsn_mc *delayedLsn;
    uint32_t numTrieNodesForMerge;

    max = self->m_num_items_in_to_add_ses;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        pfxBundle = self->m_toadd_ses[loopvar];
        tempPtr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle);
        kaps_jr1_memcpy(&trienode, tempPtr, sizeof(kaps_jr1_trie_node *));

        if (!trienode->m_rptParent_p)
        {
            errNum = kaps_jr1_trie_compute_rpt_parent(trienode, o_reason);

            if (errNum != NLMERR_OK)
                return errNum;
        }
    }

    
    max = self->m_num_items_in_to_update_lsns;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        lsn_p = self->m_toupdate_lsns[loopvar];
        NLM_STRY(kaps_jr1_lsn_mc_commit(lsn_p, 0, o_reason));
        lsn_p->m_bIsNewLsn = 0;
    }

    self->m_num_items_in_to_update_lsns = 0;

    
    max = self->m_num_items_in_to_add_ses;
    numTrieNodesForMerge = 0;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        pfxBundle = self->m_toadd_ses[loopvar];

        tempPtr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(pfxBundle);
        kaps_jr1_memcpy(&trienode, tempPtr, sizeof(kaps_jr1_trie_node *));

        NLM_STRY(kaps_jr1_trie_add_ses(self, trienode, pfxBundle, o_reason));

        if (numTrieNodesForMerge < KAPS_JR1_MAX_NUM_TRIE_NODES_FOR_RELATED_MERGE)
        {
            self->m_trie_global->m_trieNodesForRelatedMerge[numTrieNodesForMerge] = trienode;
            numTrieNodesForMerge++;
        }

    }
    self->m_num_items_in_to_add_ses = 0;
    self->m_trie_global->m_numTrieNodesForMerge = numTrieNodesForMerge;

    
    max = self->m_num_items_in_delayed_trie_node;

    for (loopvar = 0; loopvar < max; loopvar++)
    {
        delayedNode = self->m_delayedTrieNode[loopvar];
        delayedLsn = self->m_delayedLsn[loopvar];
        delayedNode->m_lsn_p = delayedLsn;
    }
    self->m_num_items_in_delayed_trie_node = 0;
    self->m_num_items_in_delayed_lsn = 0;

    
    NLM_STRY(kaps_jr1_trie_commit_iits(self, o_reason));
    self->m_num_items_in_to_update_sit = 0;

    
    max = self->m_num_items_in_to_delete_ses;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        bundle = self->m_todelete_ses[loopvar];

        tempPtr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(bundle);
        kaps_jr1_memcpy(&trienode, tempPtr, sizeof(kaps_jr1_trie_node *));

        
        NLM_STRY(kaps_jr1_trie_pvt_delete_ses_in_all_dev(self, bundle, 0, o_reason));

        trienode->m_lsn_p = kaps_jr1_lsn_mc_create(self->m_lsn_settings_p, self, trienode->m_depth);
        trienode->m_lsn_p->m_pParentHandle = trienode->m_lp_prefix_p;
    }
    self->m_num_items_in_to_delete_ses = 0;

    
    max = self->m_num_items_in_to_delete_lsns;
    for (loopvar = 0; loopvar < max; loopvar++)
    {
        lsn_p = self->m_todelete_lsns[loopvar];
        kaps_jr1_lsn_mc_destroy(lsn_p);
    }
    self->m_num_items_in_to_delete_lsns = 0;

    return NLMERR_OK;
}


kaps_jr1_pfx_bundle *
kaps_jr1_trie_locate_lpm(
    kaps_jr1_lpm_trie * self,
    uint8_t * pfxData,
    uint32_t pfxLen,
    uint32_t * o_lpmHitIndex_p,
    NlmReasonCode * o_reason)
{
    kaps_jr1_trie_node *node_p = NULL;
    kaps_jr1_pfx_bundle *lsnPfxbundle_p = NULL, *high_lsnPfxbundle_p = NULL;
    kaps_jr1_pfx_bundle *pfxbundle_p = NULL;
    kaps_jr1_trie_node *lastseen_node_p = NULL;
    kaps_jr1_trie_node *nearestLpNode_p = NULL;
    kaps_jr1_fib_tbl *fibTbl = self->m_tbl_ptr;
    struct NlmFibStats *fibStats = &fibTbl->m_fibStats;
    uint32_t wasLmpsofarHit;
    uint8_t test_pfx[] = {0x40, 0x00, 0x00, 0x00, 0x21, 0x18, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00}; 

    uint32_t matchBrickIter = 0, matchPosInBrick = 0;

    (void) o_reason;
    
    if (!pfxData) 
        pfxData = test_pfx;

    wasLmpsofarHit = 0;

    nearestLpNode_p = kaps_jr1_trie_find_dest_lp_node(self, self->m_roots_trienode_p, pfxData, pfxLen, &lastseen_node_p, 0);

    node_p = nearestLpNode_p;


    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        lsnPfxbundle_p = kaps_jr1_lsn_mc_locate_lpm(node_p->m_lsn_p, pfxData, pfxLen, NULL, &matchBrickIter,
                                                &matchPosInBrick);

        if (lsnPfxbundle_p)
        {
            high_lsnPfxbundle_p = lsnPfxbundle_p;
        }

        if (!high_lsnPfxbundle_p && self->m_trie_global->m_isIITLmpsofar)
        {
            high_lsnPfxbundle_p = node_p->m_iitLmpsofarPfx_p;

            if (node_p->m_iitLmpsofarPfx_p)
            {
                wasLmpsofarHit = 1;
                
            }
        }
    }


    pfxbundle_p = high_lsnPfxbundle_p;

    if (wasLmpsofarHit)
        fibStats->numLmpsofarHitInSearch++;

    if (!pfxbundle_p)
    {
        if (o_lpmHitIndex_p)
            *o_lpmHitIndex_p = KAPS_JR1_FIB_INVALID_INDEX;

        fibStats->numMissesInSearch++;

    }
    else
    {
        if (o_lpmHitIndex_p)
            *o_lpmHitIndex_p = pfxbundle_p->m_nIndex;
    }

    if (pfxbundle_p && pfxbundle_p->m_isPfxCopy)
        fibStats->numLmpsofarHitInSearch++;

    return pfxbundle_p;
}


uint32_t
kaps_jr1_trie_locate_exact(
    kaps_jr1_lpm_trie * self,
    uint8_t * pfxData_p,
    uint16_t inUse,
    kaps_jr1_pfx_bundle ** o_pfxBundle_pp,
    NlmReasonCode * o_reason)
{
    kaps_jr1_trie_node *node_p = NULL;
    kaps_jr1_pfx_bundle *pfxBundle = NULL, *pfxBundleInHash = NULL;
    kaps_jr1_pfx_bundle *aptPfxBundle = NULL;
    struct kaps_jr1_lpm_entry **entrySlotInHash = NULL;
    kaps_jr1_trie_node *lastseen_node_p = NULL;
    uint32_t locateExactIndex = 0;
    struct kaps_jr1_entry **ht_slot;
    struct kaps_jr1_lpm_entry *entry;
    kaps_jr1_status status;
    uint32_t match_brick_num = 0;

    (void) o_reason;
    (void) aptPfxBundle;

    node_p = kaps_jr1_trie_find_dest_lp_node(self, self->m_roots_trienode_p, pfxData_p, inUse, &lastseen_node_p, 0);
    (void) node_p;

    if (!pfxBundle)
    {
        pfxBundle = kaps_jr1_lsn_mc_locate_exact(node_p->m_lsn_p, pfxData_p, inUse, &match_brick_num);
    }

    node_p = kaps_jr1_trie_find_dest_lp_node(self, self->m_roots_trienode_p, pfxData_p, inUse, &lastseen_node_p, 1);
    (void) node_p;

    if (self->m_hashtable_p)
    {
        status = kaps_jr1_pfx_hash_table_locate(self->m_hashtable_p, pfxData_p, inUse, &ht_slot);
        if (status != KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_INTERNAL_ERROR;
            return NLMERR_FAIL;
        }

        entrySlotInHash = (struct kaps_jr1_lpm_entry **) ht_slot;
        if (entrySlotInHash)
            pfxBundleInHash = (*entrySlotInHash)->pfx_bundle;
    }

    if (pfxBundle != pfxBundleInHash)
    {
        kaps_jr1_assert(0, "Prefix bundle in data structures does not match the hash prefix bundle \n");
    }

    if (!pfxBundle)
    {
        locateExactIndex = KAPS_JR1_FIB_INVALID_INDEX;
    }
    else
    {
        locateExactIndex = pfxBundle->m_nIndex;
    }

    if (pfxBundle)
    {
        entry = pfxBundle->m_backPtr;

        if (entry->ad_handle)
        {
            struct kaps_jr1_device *device = self->m_trie_global->fibtblmgr->m_devMgr_p;
            struct kaps_jr1_ad_db *ad_db;
            enum kaps_jr1_ad_type ad_type;

            KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, entry->ad_handle, ad_db)
                ad_type = kaps_jr1_resource_get_ad_type(&ad_db->db_info);

            if (ad_db->user_width_1)
            {
                if (ad_type == KAPS_JR1_AD_TYPE_INPLACE)
                {
                    locateExactIndex = entry->ad_handle->value[0] << 16;
                    locateExactIndex |= entry->ad_handle->value[1] << 8;
                    locateExactIndex |= entry->ad_handle->value[2];
                }
            }
        }
    }

    if (o_pfxBundle_pp)
    {
        *o_pfxBundle_pp = pfxBundle;
    }

    return locateExactIndex;
}

NlmErrNum_t
kaps_jr1_trie_get_prefix_location(
    kaps_jr1_lpm_trie * trie_p,
    struct kaps_jr1_lpm_entry * lpm_entry,
    uint32_t * abs_udc,
    uint32_t * abs_row)
{
    uint32_t match_brick_num = 0;
    kaps_jr1_trie_node *node_p, *lastseen_node_p = NULL;
    kaps_jr1_pfx_bundle *pfxBundle = NULL;

    node_p = kaps_jr1_trie_find_dest_lp_node(trie_p, trie_p->m_roots_trienode_p, lpm_entry->pfx_bundle->m_data,
                                         lpm_entry->pfx_bundle->m_nPfxSize, &lastseen_node_p, 0);

    pfxBundle =
        kaps_jr1_lsn_mc_locate_exact(node_p->m_lsn_p, lpm_entry->pfx_bundle->m_data, lpm_entry->pfx_bundle->m_nPfxSize,
                                 &match_brick_num);

    if (pfxBundle)
    {
        kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(trie_p->m_trie_global->m_mlpmemmgr[0], node_p->m_lsn_p->m_mlpMemInfo,
                                                   match_brick_num, abs_udc, abs_row);
    }

    return NLMERR_OK;
}



NlmErrNum_t
kaps_jr1_trie_undo_giveout(
    kaps_jr1_lpm_trie * trie_p)
{
    uint32_t i;
    kaps_jr1_flat_lsn_data *curGiveoutData_p;
    kaps_jr1_nlm_allocator *alloc_p = trie_p->m_trie_global->m_alloc_p;

    for (i = 0; i < trie_p->m_num_items_in_pending_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_pendingGiveouts[i];
        kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, curGiveoutData_p);
    }
    trie_p->m_num_items_in_pending_giveouts = 0;

    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        if (curGiveoutData_p->lsn_cookie)
        {
            kaps_jr1_lsn_mc_destroy(curGiveoutData_p->lsn_cookie);
        }

        if (curGiveoutData_p->temp_uda_chunk)
        {
            kaps_jr1_uda_mgr_free(trie_p->m_trie_global->m_mlpmemmgr[0], curGiveoutData_p->temp_uda_chunk);
            curGiveoutData_p->temp_uda_chunk = NULL;
        }

        kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, curGiveoutData_p);
    }

    trie_p->m_num_items_in_completed_giveouts = 0;

    trie_p->m_num_items_in_to_update_lsns = 0;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_add_sorted_prefix_to_addses_list(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_pfx_bundle * item)
{
    kaps_jr1_pfx_bundle *curBundle_p = NULL;
    kaps_jr1_pfx_bundle *itemBundle = (kaps_jr1_pfx_bundle *) item;
    int32_t i = 0, j = 0;

    if (trie_p->m_num_items_in_to_add_ses >= KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY)
        kaps_jr1_assert(0, "m_toadd_ses overflow \n");

    for (i = 0; i < (int32_t) trie_p->m_num_items_in_to_add_ses; ++i)
    {
        curBundle_p = trie_p->m_toadd_ses[i];

        if (curBundle_p->m_nPfxSize < itemBundle->m_nPfxSize)
        {
            for (j = trie_p->m_num_items_in_to_add_ses; j > i; --j)
            {
                trie_p->m_toadd_ses[j] = trie_p->m_toadd_ses[j - 1];
            }
            break;
        }
    }

    trie_p->m_toadd_ses[i] = item;

    ++trie_p->m_num_items_in_to_add_ses;

    return NLMERR_OK;

}

NlmErrNum_t
kaps_jr1_trie_giveout_pending_lsns(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_lsn_mc * originalLsn,
    struct kaps_jr1_lpm_entry * entryToInsert,
    uint32_t tryOnlyLsnPush,
    NlmReasonCode * o_reason)
{
    kaps_jr1_flat_lsn_data *curGiveoutData_p, *keepData_p, *giveData_p;
    kaps_jr1_lsn_mc_settings *lsnSettings_p = originalLsn->m_pSettings;
    NlmErrNum_t errNum = NLMERR_OK;
    uint32_t numGiveouts = 0, i;
    kaps_jr1_lsn_mc *newLsn = NULL;
    kaps_jr1_trie_global *trie_global = trie_p->m_trie_global;
    kaps_jr1_nlm_allocator *alloc_p = trie_p->m_trie_global->m_alloc_p;
    int32_t deleteOriginalSes;
    void *tmp_ptr;
    kaps_jr1_trie_node *originalLsnTrienode_p, *curLsnTrieNode_p = NULL;
    uint32_t curBrickIter;
    kaps_jr1_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    struct kaps_jr1_device *device = trie_p->m_trie_global->fibtblmgr->m_devMgr_p;
    struct kaps_jr1_fib_tbl *fibTbl = trie_p->m_trie_global->fibtblmgr->m_curFibTbl;
    struct kaps_jr1_db *db = fibTbl->m_db;
    uint32_t rpb_id = db->rpb_id;
    uint32_t originalPoolId;
    uint32_t numIptEntriesInPool;
    uint32_t numIptEntriesToAdd;
    uint32_t isLsnFitPossible = 0;
    uint32_t num_new_160b_triggers;
    uint32_t lsn_depth = 0, trigger_length_1 = 0, numRptBitsLoppedOff;
    kaps_jr1_pfx_bundle *pfxToInsert, **pfxToInsert_pp;
    uint32_t numExtraPfx;
    uint32_t list_size;
    uint32_t isKeepLsnEmpty, isGiveLsnEmpty, isPfxExactlyMatchingLsnPresent;

    pfxToInsert = NULL;
    pfxToInsert_pp = NULL;
    numExtraPfx = 0;
    if (entryToInsert)
    {
        pfxToInsert = entryToInsert->pfx_bundle;
        pfxToInsert_pp = &pfxToInsert;
        kaps_jr1_seq_num_gen_set_current_pfx_seq_nr(pfxToInsert);
        numExtraPfx = 1;
    }

    trie_p->m_numExtraIptEntriesNeededDuringGiveout = 0;


    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(originalLsn);

    kaps_jr1_lsn_mc_store_old_lsn_info(originalLsn, 0);

    tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(originalLsn->m_pParentHandle);
    kaps_jr1_memcpy(&originalLsnTrienode_p, tmp_ptr, sizeof(kaps_jr1_trie_node *));

    originalPoolId = originalLsnTrienode_p->m_rptParent_p->m_poolId;

    numRptBitsLoppedOff = kaps_jr1_trie_get_num_rpt_bits_lopped_off(device, originalLsnTrienode_p->m_rptParent_p->m_depth);

    curGiveoutData_p = kaps_jr1_lsn_mc_create_flat_lsn_data(alloc_p, o_reason);
    if (!curGiveoutData_p)
        return NLMERR_FAIL;

    errNum = kaps_jr1_lsn_mc_convert_lsn_to_flat_data(originalLsn, pfxToInsert_pp, numExtraPfx, curGiveoutData_p, o_reason);
    if (errNum != NLMERR_OK)
    {
        return errNum;
    }

    curGiveoutData_p->m_iitLmpsofarPfx = originalLsnTrienode_p->m_iitLmpsofarPfx_p;

    list_size = trie_p->m_num_items_in_pending_giveouts;
    trie_p->m_pendingGiveouts[list_size] = curGiveoutData_p;
    trie_p->m_num_items_in_pending_giveouts++;

    while (trie_p->m_num_items_in_pending_giveouts)
    {
        curGiveoutData_p = trie_p->m_pendingGiveouts[0];

        keepData_p = kaps_jr1_lsn_mc_create_flat_lsn_data(alloc_p, o_reason);
        if (!keepData_p)
        {
            kaps_jr1_trie_undo_giveout(trie_p);
            return NLMERR_FAIL;
        }
        keepData_p->m_iitLmpsofarPfx = curGiveoutData_p->m_iitLmpsofarPfx;

        giveData_p = kaps_jr1_lsn_mc_create_flat_lsn_data(alloc_p, o_reason);
        if (!giveData_p)
        {
            kaps_jr1_trie_undo_giveout(trie_p);
            return NLMERR_FAIL;
        }
        giveData_p->isGiveLsn = 1;
        giveData_p->m_iitLmpsofarPfx = curGiveoutData_p->m_iitLmpsofarPfx;

        errNum = kaps_jr1_lsn_mc_giveout(originalLsn, lsnSettings_p, fibTbl, curGiveoutData_p, keepData_p, giveData_p,
                                     pfxToInsert, alloc_p, tryOnlyLsnPush, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_jr1_trie_undo_giveout(trie_p);
            kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, keepData_p);
            kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);

            if (*o_reason != NLMRSC_LOW_MEMORY)
                *o_reason = NLMRSC_UDA_ALLOC_FAILED;

            return errNum;
        }

        isKeepLsnEmpty = 0;
        if (lsnSettings_p->m_strictlyStoreLmpsofarInAds)
        {
            isPfxExactlyMatchingLsnPresent = 0;
            if (keepData_p->m_iitLmpsofarPfx 
                && keepData_p->m_iitLmpsofarPfx->m_nPfxSize == keepData_p->m_commonPfx->m_inuse)
            {
                isPfxExactlyMatchingLsnPresent = 1;
            }

            if (keepData_p->m_numPfxInLsn == 0 && !isPfxExactlyMatchingLsnPresent)
                isKeepLsnEmpty = 1;
        }
        else
        {
            if (keepData_p->m_numPfxInLsn == 0)
                isKeepLsnEmpty = 1;
        }

        
        if (!isKeepLsnEmpty)
        {

            newLsn = kaps_jr1_lsn_mc_create(trie_p->m_lsn_settings_p, trie_p, keepData_p->m_commonPfx->m_inuse);

            if (!newLsn)
            {
                kaps_jr1_trie_undo_giveout(trie_p);
                kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, keepData_p);
                kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
                *o_reason = NLMRSC_LOW_MEMORY;
                return NLMERR_FAIL;
            }

            isLsnFitPossible = 0;

            errNum = kaps_jr1_lsn_mc_convert_flat_data_to_lsn(keepData_p, newLsn, &isLsnFitPossible, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_jr1_trie_undo_giveout(trie_p);
                kaps_jr1_lsn_mc_destroy(newLsn);
                kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, keepData_p);
                kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
                return errNum;
            }

            if (isLsnFitPossible)
            {

                
                if (lsnSettings_p->m_splitIfLowUda)
                {
                    kaps_jr1_uda_mgr_alloc(trie_global->m_mlpmemmgr[0], newLsn->m_numLpuBricks, device, newLsn,
                                       &keepData_p->temp_uda_chunk, trie_global->m_mlpmemmgr[0]->db);
                }

                
                if (!lsnSettings_p->m_splitIfLowUda || keepData_p->temp_uda_chunk || newLsn->m_numLpuBricks == 1)
                {
                    keepData_p->lsn_cookie = newLsn;

                    list_size = trie_p->m_num_items_in_completed_giveouts;
                    trie_p->m_completedGiveouts[list_size] = keepData_p;
                    trie_p->m_num_items_in_completed_giveouts++;

                }
                else
                {
                    if (keepData_p->temp_uda_chunk)
                    {
                        kaps_jr1_uda_mgr_free(trie_global->m_mlpmemmgr[0], keepData_p->temp_uda_chunk);
                        keepData_p->temp_uda_chunk = NULL;
                    }

                    kaps_jr1_lsn_mc_destroy(newLsn);

                    list_size = trie_p->m_num_items_in_pending_giveouts;
                    trie_p->m_pendingGiveouts[list_size] = keepData_p;
                    trie_p->m_num_items_in_pending_giveouts++;
                }

            }
            else
            {
                kaps_jr1_lsn_mc_destroy(newLsn);

                list_size = trie_p->m_num_items_in_pending_giveouts;
                trie_p->m_pendingGiveouts[list_size] = keepData_p;
                trie_p->m_num_items_in_pending_giveouts++;

            }
        }
        else
        {
            kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, keepData_p);
            keepData_p = NULL;
        }

        isGiveLsnEmpty = 0;
        if (lsnSettings_p->m_strictlyStoreLmpsofarInAds)
        {
            isPfxExactlyMatchingLsnPresent = 0;
            if (giveData_p->m_iitLmpsofarPfx 
                && giveData_p->m_iitLmpsofarPfx->m_nPfxSize == giveData_p->m_commonPfx->m_inuse)
            {
                isPfxExactlyMatchingLsnPresent = 1;
            }

            if (giveData_p->m_numPfxInLsn == 0 && !isPfxExactlyMatchingLsnPresent)
                isGiveLsnEmpty = 1;
        }
        else
        {
            if (giveData_p->m_numPfxInLsn == 0)
                isGiveLsnEmpty = 1;
        }

        
        if (!isGiveLsnEmpty)
        {

            newLsn = kaps_jr1_lsn_mc_create(trie_p->m_lsn_settings_p, trie_p, giveData_p->m_commonPfx->m_inuse);

            if (!newLsn)
            {
                kaps_jr1_trie_undo_giveout(trie_p);
                kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
                *o_reason = NLMRSC_LOW_MEMORY;
                return NLMERR_FAIL;
            }

            isLsnFitPossible = 0;
            errNum = kaps_jr1_lsn_mc_convert_flat_data_to_lsn(giveData_p, newLsn, &isLsnFitPossible, o_reason);

            if (errNum != NLMERR_OK)
            {
                kaps_jr1_trie_undo_giveout(trie_p);
                kaps_jr1_lsn_mc_destroy(newLsn);
                kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
                return errNum;
            }

            if (isLsnFitPossible)
            {
                
                if (lsnSettings_p->m_splitIfLowUda)
                {
                    kaps_jr1_uda_mgr_alloc(trie_global->m_mlpmemmgr[0], newLsn->m_numLpuBricks, device, newLsn,
                                       &giveData_p->temp_uda_chunk, trie_global->m_mlpmemmgr[0]->db);
                }

                
                if (!lsnSettings_p->m_splitIfLowUda || giveData_p->temp_uda_chunk || newLsn->m_numLpuBricks == 1)
                {
                    giveData_p->lsn_cookie = newLsn;

                    list_size = trie_p->m_num_items_in_completed_giveouts;
                    trie_p->m_completedGiveouts[list_size] = giveData_p;
                    trie_p->m_num_items_in_completed_giveouts++;

                }
                else
                {
                    if (giveData_p->temp_uda_chunk)
                    {
                        kaps_jr1_uda_mgr_free(trie_global->m_mlpmemmgr[0], giveData_p->temp_uda_chunk);
                        giveData_p->temp_uda_chunk = NULL;
                    }

                    kaps_jr1_lsn_mc_destroy(newLsn);

                    list_size = trie_p->m_num_items_in_pending_giveouts;
                    trie_p->m_pendingGiveouts[list_size] = giveData_p;
                    trie_p->m_num_items_in_pending_giveouts++;
                }

            }
            else
            {
                kaps_jr1_lsn_mc_destroy(newLsn);

                list_size = trie_p->m_num_items_in_pending_giveouts;
                trie_p->m_pendingGiveouts[list_size] = giveData_p;
                trie_p->m_num_items_in_pending_giveouts++;
            }
        }
        else
        {
            kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, giveData_p);
            giveData_p = NULL;
        }

        
        list_size = trie_p->m_num_items_in_pending_giveouts;

        for (i = 1; i < list_size; ++i)
        {
            trie_p->m_pendingGiveouts[i - 1] = trie_p->m_pendingGiveouts[i];
        }

        trie_p->m_num_items_in_pending_giveouts--;

        kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, curGiveoutData_p);

        ++numGiveouts;
        if (numGiveouts > 1)
        {
            trie_p->m_isRecursiveSplit = 1;
        }
    }

    
    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        if (curGiveoutData_p->temp_uda_chunk)
        {
            kaps_jr1_uda_mgr_free(trie_p->m_trie_global->m_mlpmemmgr[0], curGiveoutData_p->temp_uda_chunk);
            curGiveoutData_p->temp_uda_chunk = NULL;
        }
    }

    
    
    num_new_160b_triggers = 0;
    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];
        lsn_depth = curGiveoutData_p->m_commonPfx->m_inuse;
        trigger_length_1 = lsn_depth - numRptBitsLoppedOff;

        trigger_length_1 += 1;      


        if (trigger_length_1 > 80)
        {
            num_new_160b_triggers++;
        }
    }

    numIptEntriesToAdd = trie_p->m_num_items_in_completed_giveouts;

    deleteOriginalSes = 1;

    numIptEntriesInPool = kaps_jr1_pool_mgr_get_num_entries(poolMgr, originalPoolId, KAPS_JR1_IPT_POOL);
    if (numIptEntriesInPool + numIptEntriesToAdd > device->hw_res->num_rows_in_rpb[rpb_id])
    {
        kaps_jr1_trie_undo_giveout(trie_p);
        *o_reason = NLMRSC_DBA_ALLOC_FAILED;
        return NLMERR_FAIL;
    }


    
    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];

        if (curGiveoutData_p->m_commonPfx->m_inuse == originalLsn->m_nDepth)
        {
            deleteOriginalSes = 0;
        }

        newLsn = curGiveoutData_p->lsn_cookie;

        errNum = kaps_jr1_lsn_mc_acquire_resources(newLsn, newLsn->m_nLsnCapacity, newLsn->m_numLpuBricks, o_reason);

        if (errNum != NLMERR_OK)
        {
            kaps_jr1_trie_undo_giveout(trie_p);
            return errNum;
        }

        if (newLsn->m_pSettings->m_isJoinedUdc && newLsn->m_pSettings->m_isPerLpuGran)
        {
            errNum = kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(newLsn, o_reason);

            if (errNum != NLMERR_OK)
                return errNum;
        }

        if (newLsn->m_pSettings->m_isPerLpuGran)
        {
            kaps_jr1_lpm_lpu_brick *cur_lpu_brick;
            cur_lpu_brick = newLsn->m_lpuList;
            curBrickIter = 0;
            while (cur_lpu_brick)
            {
                uint32_t ixRqtSize = cur_lpu_brick->m_maxCapacity;

                kaps_jr1_lsn_mc_handle_under_alloc(newLsn, cur_lpu_brick, curBrickIter, &ixRqtSize);

                errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(newLsn, cur_lpu_brick, curBrickIter,
                                                               ixRqtSize, cur_lpu_brick->ad_db, o_reason);

                if (errNum != NLMERR_OK)
                {
                    kaps_jr1_trie_undo_giveout(trie_p);
                    return errNum;
                }

                curBrickIter++;
                cur_lpu_brick = cur_lpu_brick->m_next_p;
            }
            newLsn->m_nNumIxAlloced = newLsn->m_nLsnCapacity;
        }

        kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(newLsn, o_reason);

        list_size = trie_p->m_num_items_in_to_update_lsns;

        if (list_size >= KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY)
            kaps_jr1_assert(0, "m_toupdate_lsns overflow \n");

        trie_p->m_toupdate_lsns[list_size] = newLsn;
        trie_p->m_num_items_in_to_update_lsns++;
    }

    for (i = 0; i < trie_p->m_num_items_in_completed_giveouts; ++i)
    {
        curGiveoutData_p = trie_p->m_completedGiveouts[i];
        newLsn = trie_p->m_toupdate_lsns[i];

        kaps_jr1_assert(curGiveoutData_p->m_commonPfx->m_inuse >= originalLsn->m_nDepth,
                    "The common prefix is shorter than original LSN during giveout \n");

        if (curGiveoutData_p->m_commonPfx->m_inuse == originalLsn->m_nDepth)
        {
            
            list_size = trie_p->m_num_items_in_delayed_trie_node;
            trie_p->m_delayedTrieNode[list_size] = originalLsnTrienode_p;
            trie_p->m_num_items_in_delayed_trie_node++;

            list_size = trie_p->m_num_items_in_delayed_lsn;
            trie_p->m_delayedLsn[list_size] = newLsn;
            trie_p->m_num_items_in_delayed_lsn++;

            newLsn->m_pParentHandle = originalLsnTrienode_p->m_lp_prefix_p;
            curLsnTrieNode_p = originalLsnTrienode_p;
        }
        else
        {
            curLsnTrieNode_p = kaps_jr1_trie_node_insertPathFromPrefix(originalLsnTrienode_p,
                                                                   curGiveoutData_p->m_commonPfx, originalLsn->m_nDepth,
                                                                   curGiveoutData_p->m_commonPfx->m_inuse - 1);

            kaps_jr1_trie_add_sorted_prefix_to_addses_list(trie_p, curLsnTrieNode_p->m_lp_prefix_p);

            kaps_jr1_lsn_mc_destroy(curLsnTrieNode_p->m_lsn_p);
            curLsnTrieNode_p->m_lsn_p = newLsn;
            newLsn->m_pParentHandle = curLsnTrieNode_p->m_lp_prefix_p;
            curLsnTrieNode_p->m_node_type = NLMNSTRIENODE_LP;
        }

        if (trie_p->m_trie_global->m_isIITLmpsofar)
        {
            curLsnTrieNode_p->m_iitLmpsofarPfx_p = curGiveoutData_p->m_iitLmpsofarPfx;
        }
        

        kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc_p, curGiveoutData_p);
    }

    trie_p->m_num_items_in_completed_giveouts = 0;

    list_size = trie_p->m_num_items_in_to_delete_lsns;
    if (list_size >= KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY)
        kaps_jr1_assert(0, "m_todelete_lsns overflow \n");

    trie_p->m_todelete_lsns[list_size] = originalLsn;
    trie_p->m_num_items_in_to_delete_lsns++;

    trie_p->m_trie_global->curLsnToSplit = originalLsn;

    if (deleteOriginalSes)
    {
        list_size = trie_p->m_num_items_in_to_delete_ses;
        if (list_size >= KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY)
            kaps_jr1_assert(0, "m_todelete_ses overflow \n");

        trie_p->m_todelete_ses[list_size] = originalLsnTrienode_p->m_lp_prefix_p;
        trie_p->m_num_items_in_to_delete_ses++;
    }
    else
    {
        list_size = trie_p->m_num_items_in_to_update_sit;
        if (list_size >= KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY)
            kaps_jr1_assert(0, "m_toupdate_sit overflow \n");

        trie_p->m_toupdate_sit[list_size] = originalLsnTrienode_p;
        trie_p->m_num_items_in_to_update_sit++;
    }

    trie_p->m_tbl_ptr->m_fibStats.numLsnGiveOuts++;

    if (numGiveouts > trie_p->m_tbl_ptr->m_fibStats.maxNumIptSplitsInOneOperation)
    {
        trie_p->m_tbl_ptr->m_fibStats.maxNumIptSplitsInOneOperation = numGiveouts;
    }

    if (numGiveouts > 1)
    {
        trie_p->m_tbl_ptr->m_fibStats.numRecursiveIptSplits++;
    }

    return NLMERR_OK;
}


uint32_t kaps_jr1_trie_get_uda_estimate(
    struct kaps_jr1_device *device,
    kaps_jr1_fib_tbl *fibTbl,
    uint32_t total_num_pfx,
    kaps_jr1_lsn_mc_settings *settings,
    struct uda_mgr_stats *uda_stats,
    uint32_t *avg_num_pfx_per_brick,
    uint32_t log_info)
{
    uint32_t num_uda_pfx_bits_per_entry;
    uint32_t num_unassigned_lpu_bricks_in_device;
    uint32_t num_assigned_bricks;
    int32_t total_unassigned_bits;
    uint32_t uda_based_estimate;
    uint32_t num_pfx_in_free_bricks_in_uda_mgr;
    struct kaps_jr1_db *parent_db;

    parent_db = fibTbl->m_db;
    if (parent_db->parent)
        parent_db = parent_db->parent;


    if (log_info) 
    {
        kaps_jr1_printf("UDA ESTIMATE: Num Pfx in all tables = %d\n", total_num_pfx);
        kaps_jr1_printf("UDA ESTIMATE: Num allocated bricks for all LSNs = %d\n", settings->m_numAllocatedBricksForAllLsns);
        kaps_jr1_printf("UDA ESTIMATE: Num allocated bricks in UDA Mgr = %d\n", uda_stats->total_num_allocated_lpu_bricks);
    }


    
    if (fibTbl->m_numPrefixes >= 10) 
    {
        num_assigned_bricks = uda_stats->total_num_allocated_lpu_bricks;

        num_uda_pfx_bits_per_entry = (num_assigned_bricks * settings->m_lpu_brick_width_1) /  total_num_pfx;
        num_uda_pfx_bits_per_entry++; 

    } 
    else 
    {
        
        if (fibTbl->m_width <= 56) {
            num_uda_pfx_bits_per_entry = 32;
        } else {
            num_uda_pfx_bits_per_entry = 96;
        }
    }

    if (log_info) 
    {
        kaps_jr1_printf("UDA ESTIMATE: Num UDA Bits / Pfx = %d\n", num_uda_pfx_bits_per_entry);
    }


    
    num_unassigned_lpu_bricks_in_device = 0;

    total_unassigned_bits = num_unassigned_lpu_bricks_in_device * settings->m_lpu_brick_width_1;


    
    num_pfx_in_free_bricks_in_uda_mgr = (uda_stats->total_num_free_lpu_bricks * settings->m_lpu_brick_width_1) / num_uda_pfx_bits_per_entry;

    if (log_info) 
    {
        kaps_jr1_printf("UDA ESTIMATE: Number of Free Bricks in UDA Manager = %d\n", uda_stats->total_num_free_lpu_bricks);
        kaps_jr1_printf("UDA ESTIMATE: Number of prefix in Free Bricks in UDA Manager = %d\n", num_pfx_in_free_bricks_in_uda_mgr);
    }


    
    uda_based_estimate = total_num_pfx;

    uda_based_estimate += num_pfx_in_free_bricks_in_uda_mgr;
    uda_based_estimate += total_unassigned_bits / num_uda_pfx_bits_per_entry;


    *avg_num_pfx_per_brick = settings->m_lpu_brick_width_1 / num_uda_pfx_bits_per_entry;

    if (log_info) 
    {
        kaps_jr1_printf("UDA ESTIMATE: avg_num_pfx_per_brick = %d\n", *avg_num_pfx_per_brick);
        kaps_jr1_printf("UDA ESTIMATE: uda_based_estimate = %d\n", uda_based_estimate);
    }

    return uda_based_estimate;

}



void kaps_jr1_trie_get_resource_usage(
    kaps_jr1_fib_tbl *fibTbl,
    kaps_jr1_fib_resource_usage * rxcUsed_p,
    uint32_t log_info)
{
    kaps_jr1_trie_global *trieGlobal_p = fibTbl->m_fibTblMgr_p->m_trieGlobal;
    struct kaps_jr1_device *device = fibTbl->m_fibTblMgr_p->m_devMgr_p;
    struct kaps_jr1_uda_mgr *mlpMemMgr_p = trieGlobal_p->m_mlpmemmgr[0];
    struct uda_mgr_stats uda_stats;
    uint32_t udaUsedInBits = 0;
    uint32_t numOfABsUsed = 0;
    uint32_t is_dba_brimmed = 0;
    uint32_t numABsReleased = 0;
    struct kaps_jr1_db *parent_db;
    struct kaps_jr1_db *tab;
    struct kaps_jr1_lpm_db *lpm_db;
    uint32_t total_entries_in_this_db;
    uint32_t uda_based_estimate;
    uint32_t dba_based_estimate, dba_extra_entry_estimate = 0;
    struct kaps_jr1_lsn_mc_settings *settings = fibTbl->m_trie->m_lsn_settings_p;
    uint32_t avg_num_pfx_per_brick = 1;

    parent_db = fibTbl->m_db;
    if (parent_db->parent)
        parent_db = parent_db->parent;

    
    total_entries_in_this_db = 0;
    tab = parent_db;

    while (tab) {
        if (tab->is_clone) {
            tab = tab->next_tab;
            continue;
        }

        lpm_db = (struct kaps_jr1_lpm_db*) tab;

        total_entries_in_this_db += lpm_db->fib_tbl->m_numPrefixes;
        tab = tab->next_tab;
    }
    
    if (total_entries_in_this_db == 0)
        total_entries_in_this_db = 1;

    kaps_jr1_uda_mgr_calc_stats(mlpMemMgr_p, &uda_stats);

    udaUsedInBits = uda_stats.total_num_allocated_lpu_bricks * fibTbl->m_trie->m_lsn_settings_p->m_lpu_brick_width_1;
    rxcUsed_p->m_udaUsedInKb = (udaUsedInBits + 1024 - 1) >> 10;

    uda_based_estimate = kaps_jr1_trie_get_uda_estimate(device, fibTbl, total_entries_in_this_db, 
                                settings, &uda_stats, &avg_num_pfx_per_brick, log_info);

    kaps_jr1_pool_mgr_get_dba_stats(trieGlobal_p->poolMgr, parent_db, avg_num_pfx_per_brick, total_entries_in_this_db, &numOfABsUsed, 
                            &is_dba_brimmed, &numABsReleased, &dba_extra_entry_estimate, log_info);

    rxcUsed_p->m_numOfABsUsed = numOfABsUsed;

    rxcUsed_p->m_numABsReleased = numABsReleased;


     
    dba_based_estimate = dba_extra_entry_estimate + fibTbl->m_numPrefixes;

    if (log_info) 
    {
        kaps_jr1_printf("DBA Estimate: dba_extra_entry_estimate + fibTbl->m_numPrefixes \n");
        kaps_jr1_printf("DBA Estimate: dba_based_estimate = %d\n", dba_based_estimate);
    }

    rxcUsed_p->m_estCapacity = uda_based_estimate;

    if (rxcUsed_p->m_estCapacity > dba_based_estimate)
        rxcUsed_p->m_estCapacity = dba_based_estimate;


    if (log_info)
    {
        kaps_jr1_printf("\nFINAL DB STATS ESTIMATE = %d \n", rxcUsed_p->m_estCapacity);
    }

}


void
kaps_jr1_trie_verify_ab_and_pool_mapping(
    kaps_jr1_lpm_trie * trie_p)
{
    uint32_t i, j, k;
    struct kaps_jr1_device *device = trie_p->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
    kaps_jr1_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    uint32_t num_free_slots_in_bmp, numFreeSlotsInPoolMgr;
    uint32_t poolId, ab_num;

    for (poolId = 0; poolId < KAPS_JR1_MAX_NUM_POOLS; ++poolId)
    {

        if (!poolMgr->m_ipmPoolInfo[poolId].m_isInUse)
            continue;

        ab_num = poolMgr->m_ipmPoolInfo[poolId].m_dba_mgr->m_ab_info->ab_num;

        num_free_slots_in_bmp = 0;
        for (i = 0; i < device->kaps_jr1_shadow->ab_to_small_bb[ab_num].num_bricks; ++i)
        {
            for (j = 0; j < 2; ++j)
            {
                for (k = 0; k < 8; ++k)
                {
                    if (device->kaps_jr1_shadow->ab_to_small_bb[ab_num].sub_ab_bricks[i].free_slot_bmp[j] & (1u << k))
                    {
                        num_free_slots_in_bmp++;
                    }
                }
            }
        }

        numFreeSlotsInPoolMgr = poolMgr->m_ipmPoolInfo[poolId].m_maxNumDbaEntries -
            poolMgr->m_ipmPoolInfo[poolId].m_curNumDbaEntries;

        if (num_free_slots_in_bmp != numFreeSlotsInPoolMgr)
        {
            kaps_jr1_assert(0, "Incorrect number of entries in bitmap and pool\n");
        }
    }
}

void
kaps_jr1_trie_verify_num_ipt_and_apt_in_rpt_node(
    kaps_jr1_trie_node * curNode)
{
    uint32_t oldNumIptEntries, oldNumAptEntries;

    if (curNode->m_isRptNode)
    {
        oldNumIptEntries = curNode->m_numIptEntriesInSubtrie;
        oldNumAptEntries = curNode->m_numAptEntriesInSubtrie;

        kaps_jr1_trie_pvt_calc_num_ipt_and_apt(curNode);

        kaps_jr1_assert(oldNumIptEntries == curNode->m_numIptEntriesInSubtrie,
                    "Mismatch in number of IPT entries in the subtrie \n");

        kaps_jr1_assert(oldNumAptEntries == curNode->m_numAptEntriesInSubtrie,
                    "Mismatch in number of APT entries in the subtrie \n");
    }

    if (curNode->m_child_p[0])
    {
        kaps_jr1_trie_verify_num_ipt_and_apt_in_rpt_node(curNode->m_child_p[0]);
    }

    if (curNode->m_child_p[1])
    {
        kaps_jr1_trie_verify_num_ipt_and_apt_in_rpt_node(curNode->m_child_p[1]);
    }
}

void
kaps_jr1_trie_verify_lsn(
    kaps_jr1_trie_node * curNode)
{
    if (curNode->m_node_type == NLMNSTRIENODE_LP)
    {
        kaps_jr1_lsn_mc_verify(curNode->m_lsn_p);
    }

    if (curNode->m_child_p[0])
    {
        kaps_jr1_trie_verify_lsn(curNode->m_child_p[0]);
    }

    if (curNode->m_child_p[1])
    {
        kaps_jr1_trie_verify_lsn(curNode->m_child_p[1]);
    }
}


void
kaps_jr1_trie_verify_iit_lmpsofar(
    kaps_jr1_trie_node * node_p,
    kaps_jr1_trie_node * ancestorIptNode_p)
{
    kaps_jr1_pfx_bundle *lmp_in_prev_or_cur_lsn, *lmp_in_prev_lsn, *lmp_in_cur_lsn;
    kaps_jr1_lpm_trie *trie_p = NULL;
    uint32_t is_candidate_node;
    uint32_t matchBrickIter = 0, matchBrickPosInBrick = 0;

    if (!node_p)
        return;

    trie_p = node_p->m_trie_p;


    is_candidate_node = 0;
    if (node_p->m_node_type == NLMNSTRIENODE_LP)
        is_candidate_node = 1;


    if (is_candidate_node)
    {

        if (ancestorIptNode_p)
        {
            
            lmp_in_prev_lsn = kaps_jr1_lsn_mc_locate_lpm(ancestorIptNode_p->m_lsn_p, node_p->m_lp_prefix_p->m_data,
                                                     node_p->m_lp_prefix_p->m_nPfxSize, NULL, 
                                                     &matchBrickIter, &matchBrickPosInBrick);

            lmp_in_prev_or_cur_lsn = lmp_in_prev_lsn;

            if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
            {
                
                if (node_p->m_iitLmpsofarPfx_p 
                    && node_p->m_iitLmpsofarPfx_p->m_nPfxSize == node_p->m_depth)
                {
                    lmp_in_cur_lsn = node_p->m_iitLmpsofarPfx_p;
                    lmp_in_prev_or_cur_lsn = lmp_in_cur_lsn;

                    if (lmp_in_prev_lsn && lmp_in_prev_lsn->m_nPfxSize >= lmp_in_cur_lsn->m_nPfxSize)
                        kaps_jr1_assert(0, "Previous LSN has the prefix that exactly matches Current LSN\n");
                }
            }

            
            
            if (lmp_in_prev_or_cur_lsn )
            {
                
                if (node_p->m_iitLmpsofarPfx_p != lmp_in_prev_or_cur_lsn)
                {
                    kaps_jr1_assert(0, "Lmpsofar is not the longest possible\n");
                }
            }
            else
            {
                
                if (node_p->m_iitLmpsofarPfx_p != ancestorIptNode_p->m_iitLmpsofarPfx_p)
                    kaps_jr1_assert(0, "The previous LP node and current LP node should have same IIT Lmpssofar\n");
            }
        }
        else
        {
            if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
            {
                
                if (node_p->m_iitLmpsofarPfx_p &&
                    node_p->m_iitLmpsofarPfx_p->m_nPfxSize != node_p->m_depth)
                {
                    kaps_jr1_assert(0, "The IIT lmpsofar of the current node should be NULL\n");       
                }
            }
            else 
            {
                
                if (node_p->m_iitLmpsofarPfx_p != NULL)
                    kaps_jr1_assert(0, "The IIT lmpsofar of the current node should be NULL\n");
            }
        }


        if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
        {
            
            uint32_t matchBrickNum = 0;
            kaps_jr1_pfx_bundle *exactMatchPfxBundle;
            
            exactMatchPfxBundle = kaps_jr1_lsn_mc_locate_exact(node_p->m_lsn_p, node_p->m_lp_prefix_p->m_data,
                             node_p->m_lp_prefix_p->m_nPfxSize, &matchBrickNum);

            if (exactMatchPfxBundle)
                kaps_jr1_assert(0, "Prefix exactly matching the LSN should not be present in LSN \n");
        }

        
        if (node_p->m_iitLmpsofarPfx_p && !node_p->m_iitLmpsofarPfx_p->m_isLmpsofarPfx)
            kaps_jr1_assert(0, "IIT lmpsofar m_isLmpsofarPfx is 0 \n");



        if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
        {
            
            if (node_p->m_iitLmpsofarPfx_p && node_p->m_iitLmpsofarPfx_p->m_nPfxSize > node_p->m_depth)
                kaps_jr1_assert(0, "IIT lmpsofar should be less than the node depth\n");
        }
        else 
        {
            
            if (node_p->m_iitLmpsofarPfx_p && node_p->m_iitLmpsofarPfx_p->m_nPfxSize >= node_p->m_depth)
                kaps_jr1_assert(0, "IIT lmpsofar should be less than the node depth\n");
        }


        
        if (node_p->m_iitLmpsofarPfx_p &&
            node_p->m_iitLmpsofarPfx_p->m_backPtr->pfx_bundle != node_p->m_iitLmpsofarPfx_p)
        {
            kaps_jr1_assert(0, "IIT lmpsofar backPtr is incorrect\n");
        }
        

        
        if (node_p->m_iitLmpsofarPfx_p && node_p->m_iitLmpsofarPfx_p->m_isPfxCopy)
            kaps_jr1_assert(0, "IIT lmpsofar should not be a prefix copy\n");


        
        if (node_p->m_iitLmpsofarPfx_p && !node_p->m_iitLmpsofarPfx_p->m_backPtr)
            kaps_jr1_assert(0, "IIT lmpsofar entry handle should not be NULL\n");


        ancestorIptNode_p = node_p;
    }
    else
    {
        if (node_p->m_iitLmpsofarPfx_p != NULL)
            kaps_jr1_assert(0, "Non IPT node has iitLmpsofar set\n");
    }

    if (node_p->m_child_p[0])
        kaps_jr1_trie_verify_iit_lmpsofar(node_p->m_child_p[0], ancestorIptNode_p);

    if (node_p->m_child_p[1])
        kaps_jr1_trie_verify_iit_lmpsofar(node_p->m_child_p[1], ancestorIptNode_p);
}

void
kaps_jr1_trie_verify_rpt_apt_lmpsofar(
    kaps_jr1_trie_node * cur_node,
    kaps_jr1_trie_node * highestPriorityAptTrieNode)
{
    if (!cur_node)
        return;

    if (!cur_node->m_isRptNode && cur_node->m_rptAptLmpsofarPfx)
        kaps_jr1_assert(0, "Non RPT node has m_rptAptLmpsofarPfx set \n");

    if (cur_node->m_aptLmpsofarPfx_p && cur_node->m_aptLmpsofarPfx_p->m_isPfxCopy)
        kaps_jr1_assert(0, "APT LmpsofarPfx can't be prefix copy \n");

    if (cur_node->m_rptAptLmpsofarPfx && !cur_node->m_rptAptLmpsofarPfx->m_isPfxCopy)
        kaps_jr1_assert(0, "RPT APT lmpsofar should be prefix copy \n");

    if (highestPriorityAptTrieNode)
    {
        if (cur_node->m_aptLmpsofarPfx_p
            && cur_node->m_aptLmpsofarPfx_p->m_backPtr->meta_priority <=
            highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr->meta_priority)
        {
            if (cur_node->m_rptAptLmpsofarPfx)
            {
                kaps_jr1_assert(0,
                            "RPT APT Lmpsofar should not be present since trie node APT lmpsofar itself is of highest priority\n");
            }
        }
        else
        {
            if (cur_node->m_isRptNode &&
                (!cur_node->m_rptAptLmpsofarPfx
                 || cur_node->m_rptAptLmpsofarPfx->m_backPtr !=
                 highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr))
            {
                kaps_jr1_assert(0, "RPT APT lmpsofar is not the highest priority in the trie path \n");
            }
        }
    }
    else
    {
        
        if (cur_node->m_rptAptLmpsofarPfx != NULL)
        {
            kaps_jr1_assert(0, "RPT APT Lmpsofar should be NULL");
        }
    }

    
    if (highestPriorityAptTrieNode)
    {
        if (cur_node->m_aptLmpsofarPfx_p
            && cur_node->m_aptLmpsofarPfx_p->m_backPtr->meta_priority <=
            highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr->meta_priority)
        {
            highestPriorityAptTrieNode = cur_node;
        }
    }
    else
    {
        if (cur_node->m_aptLmpsofarPfx_p)
        {
            highestPriorityAptTrieNode = cur_node;
        }
    }

    if (cur_node->m_child_p[0])
        kaps_jr1_trie_verify_rpt_apt_lmpsofar(cur_node->m_child_p[0], highestPriorityAptTrieNode);

    if (cur_node->m_child_p[1])
        kaps_jr1_trie_verify_rpt_apt_lmpsofar(cur_node->m_child_p[1], highestPriorityAptTrieNode);
}

void
kaps_jr1_trie_verify_down_stream_rpt(
    kaps_jr1_trie_node * curTrieNode,
    kaps_jr1_trie_node * upstreamRptNode)
{
    NlmNsDownStreamRptNodes *listNode;
    uint32_t found;

    if (!curTrieNode)
        return;

    if (!curTrieNode->m_isRptNode && curTrieNode->m_downstreamRptNodes)
        kaps_jr1_assert(0, "Non RPT node has down stream RPT nodes \n");

    if (curTrieNode->m_isRptNode)
    {
        listNode = curTrieNode->m_downstreamRptNodes;

        
        while (listNode)
        {
            if (listNode->rptNode->m_depth <= curTrieNode->m_depth)
            {
                kaps_jr1_assert(0, "upstream RPT node is present in downstream RPT list \n");
            }

            if (!listNode->rptNode->m_isRptNode)
            {
                kaps_jr1_assert(0, "Non RPT node is present in the downstream RPT list \n");
            }

            if (listNode->rptNode == curTrieNode)
            {
                kaps_jr1_assert(0, "Circular reference in the downstream RPT list \n");
            }

            if (!kaps_jr1_prefix_pvt_is_more_specific
                (listNode->rptNode->m_lp_prefix_p->m_data, listNode->rptNode->m_lp_prefix_p->m_nPfxSize,
                 curTrieNode->m_lp_prefix_p->m_data, curTrieNode->m_lp_prefix_p->m_nPfxSize))
            {
                kaps_jr1_assert(0, "In correct RPT node linked in the list of down stream RPT nodes \n");
            }

            listNode = listNode->next;
        }

        
        if (upstreamRptNode)
        {
            listNode = upstreamRptNode->m_downstreamRptNodes;
            found = 0;
            while (listNode)
            {
                if (listNode->rptNode == curTrieNode)
                {
                    found = 1;
                    break;
                }
                listNode = listNode->next;
            }

            if (!found)
            {
                kaps_jr1_assert(0, "Unable to find the current trie node in the upstream RPT node");
            }
        }

        upstreamRptNode = curTrieNode;
    }

    if (curTrieNode->m_child_p[0])
        kaps_jr1_trie_verify_down_stream_rpt(curTrieNode->m_child_p[0], upstreamRptNode);

    if (curTrieNode->m_child_p[1])
        kaps_jr1_trie_verify_down_stream_rpt(curTrieNode->m_child_p[1], upstreamRptNode);
}

void
kaps_jr1_trie_verify_dba_reduction(
    kaps_jr1_trie_node * node_p)
{
    if (!node_p->m_trie_p->m_trie_global->use_reserved_abs_for_160b_trig)
        return;

    if (node_p->m_isRptNode)
    {
        kaps_jr1_ipm *ipm = kaps_jr1_pool_mgr_get_ipm_for_pool(node_p->m_trie_p->m_trie_global->poolMgr, node_p->m_poolId);

        if (node_p->m_numReserved160bTrig)
        {

            if (ipm->m_ab_info->ab_num > 1 && ipm->m_ab_info->conf == 1)
            {
                kaps_jr1_assert(0, "The RPT with reserved entries is in non reserved pool\n");
            }
        }

        if (node_p->m_numIptEntriesInSubtrie)
        {
            if (ipm->m_ab_info->ab_num <= 1)
            {
                kaps_jr1_assert(0, "Normal IPT entries are stored in Reserved ABs\n");
            }
        }
    }

    if (node_p->m_child_p[0])
    {
        kaps_jr1_trie_verify_dba_reduction(node_p->m_child_p[0]);
    }

    if (node_p->m_child_p[1])
    {
        kaps_jr1_trie_verify_dba_reduction(node_p->m_child_p[1]);
    }
}

void
kaps_jr1_trie_verify_joined_udcs(
    kaps_jr1_trie_node * node_p)
{
    uint32_t lpu_nr, row_nr;

    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        struct kaps_jr1_uda_mgr *mgr = node_p->m_lsn_p->m_pSettings->m_pMlpMemMgr[0];

        kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(mgr, node_p->m_lsn_p->m_mlpMemInfo,
                                                   node_p->m_lsn_p->m_numLpuBricks - 1, &lpu_nr, &row_nr);

        if (lpu_nr % 2 == 0 && mgr->config.joined_udcs[lpu_nr + 1])
        {
            kaps_jr1_assert(0, "Last Brick should not be joined \n");
        }
    }

    if (node_p->m_child_p[0])
    {
        kaps_jr1_trie_verify_joined_udcs(node_p->m_child_p[0]);
    }

    if (node_p->m_child_p[1])
    {
        kaps_jr1_trie_verify_joined_udcs(node_p->m_child_p[1]);
    }
}


kaps_jr1_trie_node *
kaps_jr1_trie_check_sub_tree_for_ipt_merge(
    kaps_jr1_trie_node * originalIptNode,
    kaps_jr1_trie_node * nodeToCheckForMerge,
    kaps_jr1_trie_node * commonAncestor,
    kaps_jr1_trie_node * skipNode)
{
    kaps_jr1_trie_node *trieNodeToMergeWith = NULL;

    
    if (nodeToCheckForMerge == originalIptNode || nodeToCheckForMerge == skipNode)
        return NULL;

    
    if (nodeToCheckForMerge->m_isRptNode && nodeToCheckForMerge != originalIptNode->m_rptParent_p)
    {
        return NULL;
    }

    if (nodeToCheckForMerge->m_node_type == NLMNSTRIENODE_LP)
    {
        kaps_jr1_lsn_mc_settings *settings = originalIptNode->m_lsn_p->m_pSettings;
        kaps_jr1_lpm_lpu_brick *curBrick;
        uint16_t maxLen1, maxLen2, maxLen; 
        uint16_t maxGranIx;
        uint32_t numPfxThatCanBeFitWithMaxGran, actualNumPfx;

        if (originalIptNode->m_lsn_p->m_numLpuBricks + nodeToCheckForMerge->m_lsn_p->m_numLpuBricks + 1 >
            settings->m_maxLpuPerLsn)
        {
            return NULL;
        }

        

        maxLen1 = 0;

        curBrick = originalIptNode->m_lsn_p->m_lpuList;
        while (curBrick)
        {
            if (curBrick->m_gran > maxLen1)
            {
                maxLen1 = originalIptNode->m_depth + curBrick->m_gran;
            }

            curBrick = curBrick->m_next_p;
        }

        maxLen2 = 0;

        curBrick = nodeToCheckForMerge->m_lsn_p->m_lpuList;
        while (curBrick)
        {
            if (curBrick->m_gran > maxLen2)
            {
                maxLen2 = nodeToCheckForMerge->m_depth + curBrick->m_gran;
            }

            curBrick = curBrick->m_next_p;
        }

        if (maxLen1 > maxLen2)
        {
            maxLen = maxLen1 - commonAncestor->m_depth;
        }
        else
        {
            maxLen = maxLen2 - commonAncestor->m_depth;
        }

        
        if (maxLen >= KAPS_JR1_HW_LPM_MAX_GRAN)
            maxLen = KAPS_JR1_HW_LPM_MAX_GRAN - 1;

        kaps_jr1_lsn_mc_compute_gran(settings, maxLen, &maxGranIx);

        
        
        numPfxThatCanBeFitWithMaxGran = settings->m_max20bInPlacePfxInBrickForGranIx[maxGranIx] * (settings->m_maxLpuPerLsn - 1);

        actualNumPfx = originalIptNode->m_lsn_p->m_nNumPrefixes + nodeToCheckForMerge->m_lsn_p->m_nNumPrefixes;

        if (actualNumPfx > numPfxThatCanBeFitWithMaxGran)
        {
            return NULL;
        }

        return nodeToCheckForMerge;
    }

    if (nodeToCheckForMerge->m_child_p[0])
    {
        trieNodeToMergeWith = kaps_jr1_trie_check_sub_tree_for_ipt_merge(originalIptNode,
                                              nodeToCheckForMerge->m_child_p[0], 
                                              commonAncestor,
                                              skipNode);

        if (trieNodeToMergeWith)
        {
            return trieNodeToMergeWith;
        }
    }

    if (nodeToCheckForMerge->m_child_p[1])
    {
        trieNodeToMergeWith = kaps_jr1_trie_check_sub_tree_for_ipt_merge(originalIptNode,
                                              nodeToCheckForMerge->m_child_p[1], 
                                              commonAncestor,
                                              skipNode);

        if (trieNodeToMergeWith)
        {
            return trieNodeToMergeWith;
        }
    }

    return NULL;
}

uint32_t
kaps_jr1_trie_check_if_related_ipt_nodes_can_be_merged(
    kaps_jr1_trie_node * originalIptNode,
    kaps_jr1_trie_node ** trieNode2_pp,
    kaps_jr1_trie_node ** newTrieNode_pp)
{
    uint32_t canIptNodeBeMerged = 0;
    kaps_jr1_trie_node *curNode;
    kaps_jr1_trie_node *prevNode;
    kaps_jr1_trie_node *trieNodeToMergeWith = NULL;

    *trieNode2_pp = NULL;
    *newTrieNode_pp = NULL;

    
    if (originalIptNode->m_isRptNode)
        return 0;

    
    if (originalIptNode->m_node_type != NLMNSTRIENODE_LP)
    {
        return 0;
    }

    curNode = originalIptNode->m_parent_p;
    prevNode = originalIptNode;
    canIptNodeBeMerged = 0;
    
    while (curNode)
    {
        if (curNode->m_node_type == NLMNSTRIENODE_LP)
        {
            break;
        }

        
        trieNodeToMergeWith = kaps_jr1_trie_check_sub_tree_for_ipt_merge(originalIptNode, curNode, curNode, prevNode);

        if (trieNodeToMergeWith)
        {
            *trieNode2_pp = trieNodeToMergeWith;
            *newTrieNode_pp = curNode;
            canIptNodeBeMerged = 1;
            break;
        }

        if (curNode->m_isRptNode)
        {
            break;
        }

        prevNode = curNode;
        curNode = curNode->m_parent_p;

    }

    return canIptNodeBeMerged;
}

NlmErrNum_t
kaps_jr1_trie_pvt_undo_related_lsn_merge(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_lsn_mc * tempMergedLsn,
    kaps_jr1_lsn_mc * newMergedLsn,
    kaps_jr1_flat_lsn_data * mergedFlatLsnData)
{
    kaps_jr1_nlm_allocator *alloc = trie_p->m_trie_global->m_alloc_p;

    if (tempMergedLsn)
    {
        kaps_jr1_nlm_allocator_free(alloc, (void *) tempMergedLsn);
    }

    if (newMergedLsn)
    {
        kaps_jr1_lsn_mc_destroy(newMergedLsn);
    }

    if (mergedFlatLsnData)
    {
        kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc, mergedFlatLsnData);
    }

    return NLMERR_OK;
}



NlmErrNum_t
kaps_jr1_trie_merge_related_lsns(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_node * trieNodeToMerge1,
    kaps_jr1_trie_node * trieNodeToMerge2,
    kaps_jr1_trie_node * newIptTrieNode,
    NlmReasonCode * o_reason)
{
    kaps_jr1_lsn_mc_settings *settings_p = trie_p->m_lsn_settings_p;
    kaps_jr1_flat_lsn_data *mergedFlatLsnData = NULL;
    kaps_jr1_lsn_mc *tempMergedLsn = NULL, *newMergedLsn = NULL;
    NlmErrNum_t errNum;
    NlmReasonCode reason = NLMRSC_REASON_OK;
    kaps_jr1_lsn_mc *oldLsn1, *oldLsn2;
    kaps_jr1_lpm_lpu_brick *iterBrick, *lastBrickOfLsn1;
    uint32_t i, j, doesLsnFit, brickNum;
    kaps_jr1_pfx_bundle *curPfxBundle, *toDeletePfxCopy = NULL;
    kaps_jr1_fib_prefix_index_changed_app_cb_t appCB = settings_p->m_pAppCB;
    kaps_jr1_fib_tbl *tbl;
    uint32_t numIptEntriesInPool, maxAllowedIptEntriesInPool;
    kaps_jr1_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    uint32_t numExtraPfxFromIptParent, totalNumExtraPfx;
    kaps_jr1_trie_node *iptAncestorNode, *iterNode;
    struct kaps_jr1_db *db = trie_p->m_tbl_ptr->m_db;
    uint32_t numHolesToClear, excludeExactLengthMatch;
    uint32_t isExactMatchLenPfxPresent;
    uint32_t shouldSplitNearestIptAncestor;

    (void) o_reason;

    if (!settings_p->m_isRelatedLsnMergeEnabled)
        return NLMERR_OK;

    if (db->is_destroy_in_progress)
        return NLMERR_OK;

    if (trieNodeToMerge1->m_rptParent_p != trieNodeToMerge2->m_rptParent_p)
    {
        kaps_jr1_assert(0, "Incorrect trie nodes trying to be merged \n");
        return NLMERR_OK;
    }

    if (trieNodeToMerge1->m_node_type != NLMNSTRIENODE_LP || trieNodeToMerge2->m_node_type != NLMNSTRIENODE_LP)
        return NLMERR_OK;

    
    numIptEntriesInPool =
        kaps_jr1_pool_mgr_get_num_entries(poolMgr, trieNodeToMerge1->m_rptParent_p->m_poolId, KAPS_JR1_IPT_POOL);
    maxAllowedIptEntriesInPool =
        kaps_jr1_pool_mgr_get_max_allowed_entries(poolMgr, trieNodeToMerge1->m_rptParent_p->m_poolId, KAPS_JR1_IPT_POOL);

    if (numIptEntriesInPool >= maxAllowedIptEntriesInPool)
        return NLMERR_OK;


    iptAncestorNode = NULL;
    iterNode = newIptTrieNode->m_parent_p;
    while (iterNode)
    {
        if (iterNode->m_node_type == NLMNSTRIENODE_LP)
        {
            iptAncestorNode = iterNode;
            break;
        }

        iterNode = iterNode->m_parent_p;
    }

    oldLsn1 = trieNodeToMerge1->m_lsn_p;
    oldLsn2 = trieNodeToMerge2->m_lsn_p;

    tbl = oldLsn1->m_pTbl;

    mergedFlatLsnData = kaps_jr1_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, &reason);
    if (!mergedFlatLsnData)
    {
        return NLMERR_OK;
    }

    
    tempMergedLsn = kaps_jr1_lsn_mc_create(settings_p, trie_p, newIptTrieNode->m_depth);
    if (!tempMergedLsn)
    {
        kaps_jr1_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    
    lastBrickOfLsn1 = NULL;
    iterBrick = oldLsn1->m_lpuList;
    while (iterBrick)
    {
        lastBrickOfLsn1 = iterBrick;
        iterBrick = iterBrick->m_next_p;
    }

    
    lastBrickOfLsn1->m_next_p = oldLsn2->m_lpuList;

    
    tempMergedLsn->m_lpuList = oldLsn1->m_lpuList;
    tempMergedLsn->m_numLpuBricks = oldLsn1->m_numLpuBricks + oldLsn2->m_numLpuBricks;
    tempMergedLsn->m_nLsnCapacity = oldLsn1->m_nLsnCapacity + oldLsn2->m_nLsnCapacity;
    tempMergedLsn->m_nNumPrefixes = oldLsn1->m_nNumPrefixes + oldLsn2->m_nNumPrefixes;
    tempMergedLsn->m_pParentHandle = newIptTrieNode->m_lp_prefix_p;

    

    numExtraPfxFromIptParent = 0;
    isExactMatchLenPfxPresent = 0;
    if (iptAncestorNode)
    {
        
        excludeExactLengthMatch = 0;
        if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
            excludeExactLengthMatch = 1;

        kaps_jr1_lsn_mc_find_prefixes_in_path(iptAncestorNode->m_lsn_p,
                                          newIptTrieNode->m_lp_prefix_p->m_data,
                                          newIptTrieNode->m_lp_prefix_p->m_nPfxSize,
                                          excludeExactLengthMatch,
                                          trie_p->m_trie_global->m_extraPfxArray,
                                          trie_p->m_trie_global->m_pfxLocationsToMove, 
                                          &numExtraPfxFromIptParent,
                                          &isExactMatchLenPfxPresent);
    }

    shouldSplitNearestIptAncestor = 0;
    if (numExtraPfxFromIptParent)
        shouldSplitNearestIptAncestor = 1;

    if (settings_p->m_strictlyStoreLmpsofarInAds
        && isExactMatchLenPfxPresent)
    {
        shouldSplitNearestIptAncestor = 1;
    }

    
    numHolesToClear = numExtraPfxFromIptParent;
    for (i = 0; i < numExtraPfxFromIptParent; ++i)
    {
        trie_p->m_trie_global->m_pfxLocationsToClear[i] = 
                        trie_p->m_trie_global->m_pfxLocationsToMove[i];
    }
    
    totalNumExtraPfx = numExtraPfxFromIptParent;
    
    if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
    {
        
        if (trieNodeToMerge1->m_iitLmpsofarPfx_p &&
            trieNodeToMerge1->m_depth == trieNodeToMerge1->m_iitLmpsofarPfx_p->m_nPfxSize)
        {
            trie_p->m_trie_global->m_extraPfxArray[totalNumExtraPfx] = trieNodeToMerge1->m_iitLmpsofarPfx_p;
            totalNumExtraPfx++;
        }

        if (trieNodeToMerge2->m_iitLmpsofarPfx_p &&
            trieNodeToMerge2->m_depth == trieNodeToMerge2->m_iitLmpsofarPfx_p->m_nPfxSize)
        {
            trie_p->m_trie_global->m_extraPfxArray[totalNumExtraPfx] = trieNodeToMerge2->m_iitLmpsofarPfx_p;
            totalNumExtraPfx++;
        }
    }

    
    errNum =
        kaps_jr1_lsn_mc_convert_lsn_to_flat_data(tempMergedLsn, trie_p->m_trie_global->m_extraPfxArray,
                                             totalNumExtraPfx, mergedFlatLsnData, &reason);

    if (errNum != NLMERR_OK)
    {
        kaps_jr1_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        lastBrickOfLsn1->m_next_p = NULL;
        return NLMERR_OK;
    }

    
    lastBrickOfLsn1->m_next_p = NULL;

    
    newMergedLsn = kaps_jr1_lsn_mc_create(settings_p, trie_p, newIptTrieNode->m_depth);
    if (!newMergedLsn)
    {
        kaps_jr1_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    newMergedLsn->m_pParentHandle = newIptTrieNode->m_lp_prefix_p;

    
    for (i = 0; i < mergedFlatLsnData->m_numPfxInLsn; i++)
    {
        curPfxBundle = mergedFlatLsnData->m_pfxesInLsn[i];

        if (curPfxBundle->m_isPfxCopy && curPfxBundle->m_nPfxSize > newMergedLsn->m_nDepth)
        {
            toDeletePfxCopy = curPfxBundle;
            mergedFlatLsnData->m_pfxesInLsn[i] = NULL;
            for (j = i + 1; j < mergedFlatLsnData->m_numPfxInLsn; j++, i++)
            {
                mergedFlatLsnData->m_pfxesInLsn[i] = mergedFlatLsnData->m_pfxesInLsn[j];
                mergedFlatLsnData->pfx_color[i] = mergedFlatLsnData->pfx_color[j];
            }
            mergedFlatLsnData->m_numPfxInLsn--;
            break;
        }
    }
      

    
    errNum = kaps_jr1_lsn_mc_convert_flat_data_to_lsn(mergedFlatLsnData, newMergedLsn, &doesLsnFit, &reason);
    if (errNum != NLMERR_OK || !doesLsnFit)
    {
        kaps_jr1_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    
    kaps_jr1_nlm_allocator_free(settings_p->m_pAlloc, (void *) tempMergedLsn);
    tempMergedLsn = NULL;

    kaps_jr1_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, mergedFlatLsnData);
    mergedFlatLsnData = NULL;

    
    errNum =
        kaps_jr1_lsn_mc_acquire_resources(newMergedLsn, newMergedLsn->m_nLsnCapacity, newMergedLsn->m_numLpuBricks,
                                      &reason);
    if (errNum != NLMERR_OK)
    {
        kaps_jr1_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(newMergedLsn, &reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    
    if (settings_p->m_isPerLpuGran)
    {
        iterBrick = newMergedLsn->m_lpuList;
        brickNum = 0;

        while (iterBrick)
        {
            errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(newMergedLsn, iterBrick, brickNum, iterBrick->m_maxCapacity,
                                                           iterBrick->ad_db, &reason);
            if (errNum != NLMERR_OK)
            {
                kaps_jr1_lsn_mc_free_resources(newMergedLsn);
                kaps_jr1_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
                return NLMERR_OK;
            }

            brickNum++;
            iterBrick = iterBrick->m_next_p;
        }

        newMergedLsn->m_nNumIxAlloced = newMergedLsn->m_nLsnCapacity;
    }

    kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(newMergedLsn, &reason);

    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(oldLsn1);
    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(oldLsn2);

    kaps_jr1_lsn_mc_store_old_lsn_info(oldLsn1, 0);
    kaps_jr1_lsn_mc_store_old_lsn_info(oldLsn2, 1);

    if (shouldSplitNearestIptAncestor)
    {
        kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(iptAncestorNode->m_lsn_p);
        kaps_jr1_lsn_mc_store_old_lsn_info(iptAncestorNode->m_lsn_p, 2);
    }

    if (trie_p->m_trie_global->m_isIITLmpsofar)
    {
        if (iptAncestorNode)
        {
            
            kaps_jr1_trie_ipt_ancestor_iit_lmpsofar(newIptTrieNode, 
                            iptAncestorNode->m_lsn_p,
                            trie_p->m_trie_global->m_pfxLocationsToClear,
                            &numHolesToClear);
        }
        else
        {
            
            newIptTrieNode->m_iitLmpsofarPfx_p = NULL;
        }
    }

    
    errNum = kaps_jr1_lsn_mc_commit(newMergedLsn, 0, &reason);
    if (errNum != NLMERR_OK)
    {
        newIptTrieNode->m_iitLmpsofarPfx_p = NULL;
        kaps_jr1_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    kaps_jr1_lsn_mc_destroy(newIptTrieNode->m_lsn_p);
    newIptTrieNode->m_lsn_p = newMergedLsn;

    if (!newIptTrieNode->m_rptParent_p)
        newIptTrieNode->m_rptParent_p = trieNodeToMerge1->m_rptParent_p;

    
    errNum = kaps_jr1_trie_add_ses(trie_p, newIptTrieNode, newIptTrieNode->m_lp_prefix_p, &reason);
    if (errNum != NLMERR_OK)
    {
        kaps_jr1_trie_pvt_undo_related_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    if (toDeletePfxCopy)
    {
        if (appCB)
        {
            appCB(settings_p->m_pAppData, tbl, toDeletePfxCopy, toDeletePfxCopy->m_nIndex, KAPS_JR1_LSN_NEW_INDEX);
        }
        kaps_jr1_pfx_bundle_destroy(toDeletePfxCopy, trie_p->m_trie_global->m_alloc_p);
    }

    newMergedLsn->m_bIsNewLsn = 0;

    
    kaps_jr1_trie_pvt_in_place_delete(trie_p, trieNodeToMerge1, 0, &reason);
    kaps_jr1_trie_pvt_in_place_delete(trie_p, trieNodeToMerge2, 0, &reason);

    
    
    if (iptAncestorNode && numHolesToClear)
    {
        kaps_jr1_trie_clear_holes_in_split_ipt_ancestor(trie_p, iptAncestorNode, NULL,
                                                    trie_p->m_trie_global->m_pfxLocationsToClear,
                                                    numHolesToClear, &reason);
    }

    trie_p->m_tbl_ptr->m_fibStats.numRelatedLsnMerges++;

    return NLMERR_OK;
}


uint32_t
kaps_jr1_trie_find_terminating_path_worker(
    kaps_jr1_trie_node *rpt_node,
    kaps_jr1_trie_node *cur_node)
{
    uint32_t ret_val = 0;
    
    if (cur_node == rpt_node)
    {
        
    }
    else
    {
        if (cur_node->m_isRptNode)
            return 0;
    }
   
    if (cur_node->m_node_type == 1)
        return 0;


    if (!cur_node->m_child_p[0]) 
    {
        kaps_jr1_printf("Found, left child is NULL \n");

        kaps_jr1_trie_print_pfx(NULL, cur_node->m_lp_prefix_p->m_data, 
                            cur_node->m_lp_prefix_p->m_nPfxSize, '.', 0, 1);
            
        return 1;
    }


    if (!cur_node->m_child_p[1])
    {
        kaps_jr1_printf("Found, right child is NULL \n");

        kaps_jr1_trie_print_pfx(NULL, cur_node->m_lp_prefix_p->m_data, 
                            cur_node->m_lp_prefix_p->m_nPfxSize, '.', 0, 1);
        
        return 1;
    }

    if (cur_node->m_child_p[0])
    {
        ret_val = kaps_jr1_trie_find_terminating_path_worker(rpt_node, cur_node->m_child_p[0]);
        if (ret_val)
        {
            return ret_val;
        }
    }

    if (cur_node->m_child_p[1])
    {
        ret_val = kaps_jr1_trie_find_terminating_path_worker(rpt_node, cur_node->m_child_p[1]);
        if (ret_val)
        {
            return ret_val;
        }
    }

    return 0;
}
    

uint32_t 
kaps_jr1_trie_find_terminating_path_main(
    kaps_jr1_trie_node * node_p)
{
    uint32_t is_terminating_path_found = 0;
    
    if (!node_p)
        return 0;

    if (node_p->m_isRptNode && node_p->m_depth != 0)
    {
        is_terminating_path_found = kaps_jr1_trie_find_terminating_path_worker(node_p, node_p);

        if (is_terminating_path_found)
        {
            return 1;
        }
    }

    if (node_p->m_child_p[0])
    {
        is_terminating_path_found = kaps_jr1_trie_find_terminating_path_main(node_p->m_child_p[0]);

        if (is_terminating_path_found)
        {
            return 1;
        }
    }

    if (node_p->m_child_p[1])
    {
        is_terminating_path_found = kaps_jr1_trie_find_terminating_path_main(node_p->m_child_p[1]);

        if (is_terminating_path_found)
        {
            return 1;
        }

    }

    return 0;
}



void
kaps_jr1_trie_calc_trie_lsn_stats(
    kaps_jr1_trie_node * node_p,
    uint32_t find_terminating_search_path, 
    struct NlmFibStats *stats_p)
{
    kaps_jr1_lsn_mc *lsn_p = NULL;
    kaps_jr1_lpm_lpu_brick *curLpuBrick;
    uint32_t numRptBitsLoppedOff;
    uint32_t trigger_len;
    kaps_jr1_trie_node *rptParent;
    struct kaps_jr1_device *device;
    uint32_t brickIter, numHolesInCurBrick;
    kaps_jr1_pfx_bundle *pfxBundle;
    uint32_t i, foundPfxThatExactlyMatchesLsn;

    if (!node_p)
        return;

    device = node_p->m_trie_p->m_trie_global->fibtblmgr->m_devMgr_p;

    stats_p->numTrieNodes++;

    if (node_p->m_isRptNode)
    {
        stats_p->numRPTEntries++;
        numRptBitsLoppedOff = kaps_jr1_trie_get_num_rpt_bits_lopped_off(device, node_p->m_depth);
        stats_p->totalRptBytesLoppedOff += numRptBitsLoppedOff / 8;
        stats_p->rpt_lopoff_info[numRptBitsLoppedOff / 8]++;

        
        if (find_terminating_search_path && node_p->m_node_type == 0)
        {
            kaps_jr1_printf("RPT Node Id = %d: Node at depth = %d is not an IPT node and it has %d IPT nodes under it \n", 
               node_p->m_trienodeId,
               node_p->m_depth,
               node_p->m_numIptEntriesInSubtrie);

            kaps_jr1_trie_find_terminating_path_worker(node_p, node_p);

        }
        
    }

    if (node_p->m_aptLmpsofarPfx_p)
    {
        stats_p->numLmpsofarPfx++;
        stats_p->numAPTLmpsofarEntries++;
    }

    if (node_p->m_rptAptLmpsofarPfx)
    {
        stats_p->numLmpsofarPfx++;
        stats_p->numAPTLmpsofarEntries++;
    }

    if (node_p->m_numRptSplits > stats_p->maxNumRptSplitsInANode)
    {
        stats_p->maxNumRptSplitsInANode = node_p->m_numRptSplits;
        stats_p->depthofNodeWithMaxRptSplits = node_p->m_depth;
        stats_p->idOfNodeWithMaxRptSplits = node_p->m_trienodeId;
    }

    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        stats_p->numIPTEntries++;

        rptParent = node_p->m_rptParent_p;

        if (rptParent)
        {
            trigger_len = node_p->m_depth - kaps_jr1_trie_get_num_rpt_bits_lopped_off(device, rptParent->m_depth);


            if (trigger_len <= 40)
            {
                stats_p->num40bTriggers++;
            }
            else if (trigger_len <= 80)
            {
                stats_p->num80bTriggers++;
            }
            else
            {
                stats_p->num160bTriggers++;
            }

            stats_p->avgTrigLen += trigger_len;
        }

        lsn_p = node_p->m_lsn_p;
        curLpuBrick = lsn_p->m_lpuList;
        if (lsn_p->m_numLpuBricks)
            stats_p->numLsnForEachSize[lsn_p->m_numLpuBricks - 1]++;

        foundPfxThatExactlyMatchesLsn = 0;
        brickIter = 0;
        while (curLpuBrick)
        {
            stats_p->numBricksForEachGran[curLpuBrick->m_granIx]++;
            stats_p->numHolesForEachGran[curLpuBrick->m_granIx] += curLpuBrick->m_maxCapacity - curLpuBrick->m_numPfx;
            if (lsn_p->m_numLpuBricks)
                stats_p->numHolesForEachLsnSize[lsn_p->m_numLpuBricks - 1] +=
                    curLpuBrick->m_maxCapacity - curLpuBrick->m_numPfx;
            stats_p->numPfxForEachGran[curLpuBrick->m_granIx] += curLpuBrick->m_numPfx;

            numHolesInCurBrick = curLpuBrick->m_maxCapacity - curLpuBrick->m_numPfx;
            stats_p->totalNumHolesInLsns += curLpuBrick->m_maxCapacity - curLpuBrick->m_numPfx;
            stats_p->numUsedBricks++;

            if (brickIter == lsn_p->m_numLpuBricks - 1) 
            {
                stats_p->numHolesInLastBrick += numHolesInCurBrick;
            } 
            else if (brickIter == lsn_p->m_numLpuBricks - 2 
                && kaps_jr1_lsn_mc_find_if_next_brick_is_joined(lsn_p, brickIter)) 
            {
                stats_p->numHolesInLastBrick += numHolesInCurBrick; 
            } 
            else 
            {
                stats_p->numHolesInNonLastBricks += numHolesInCurBrick; 
            }

            if (curLpuBrick->m_numPfx == 0) {
                stats_p->numUsedButEmptyBricks++;
                stats_p->numEmptyBricksForEachGran[curLpuBrick->m_granIx]++;

                if (curLpuBrick->m_next_p) 
                {
                    stats_p->numNonLastBricksThatAreEmpty++;
                } 
                else 
                {
                    stats_p->numLastBricksThatAreEmpty++;
                }
            }

            if (curLpuBrick->m_numPfx > 0 && curLpuBrick->m_numPfx < curLpuBrick->m_maxCapacity)
            {
                stats_p->numPartiallyOccupiedBricks++;
            }

            for (i = 0; i < curLpuBrick->m_maxCapacity; ++i)
            {
                pfxBundle = curLpuBrick->m_pfxes[i];

                if (pfxBundle && pfxBundle->m_nPfxSize == lsn_p->m_nDepth) 
                {
                    foundPfxThatExactlyMatchesLsn = 1;
                }
            }

            brickIter++;
            curLpuBrick = curLpuBrick->m_next_p;
        }

        if (foundPfxThatExactlyMatchesLsn) 
        {
            stats_p->numLsnsWithPrefixMatchingDepth++;
        }
        else 
        {
            stats_p->numLsnsThatCanbePushedDeeper++;
        }

        
        if (node_p->m_iitLmpsofarPfx_p)
            stats_p->numLmpsofarPfx++;
    }

    if (node_p->m_child_p[0])
        kaps_jr1_trie_calc_trie_lsn_stats(node_p->m_child_p[0], find_terminating_search_path, 
                                stats_p);

    if (node_p->m_child_p[1])
        kaps_jr1_trie_calc_trie_lsn_stats(node_p->m_child_p[1], find_terminating_search_path,
                                stats_p);
}


NlmErrNum_t
kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_lsn_mc * tempMergedLsn,
    kaps_jr1_lsn_mc * newMergedLsn,
    kaps_jr1_flat_lsn_data * mergedFlatLsnData)
{
    kaps_jr1_nlm_allocator *alloc = trie_p->m_trie_global->m_alloc_p;

    if (tempMergedLsn)
    {
        kaps_jr1_nlm_allocator_free(alloc, (void *) tempMergedLsn);
    }

    if (newMergedLsn)
    {
        kaps_jr1_lsn_mc_destroy(newMergedLsn);
    }

    if (mergedFlatLsnData)
    {
        kaps_jr1_lsn_mc_destroy_flat_lsn_data(alloc, mergedFlatLsnData);
    }

    return NLMERR_OK;
}

void
kaps_jr1_trie_pvt_find_longest_pfx_len(
    kaps_jr1_lsn_mc * curLsn,
    uint32_t * longestPfxLen_p)
{
    uint32_t i;
    kaps_jr1_lpm_lpu_brick *curBrick = curLsn->m_lpuList;

    while (curBrick)
    {
        for (i = 0; i < curBrick->m_maxCapacity; ++i)
        {
            kaps_jr1_pfx_bundle *curPfx = curBrick->m_pfxes[i];
            if (curPfx && curPfx->m_nPfxSize > *longestPfxLen_p)
                *longestPfxLen_p = curPfx->m_nPfxSize;
        }
        curBrick = curBrick->m_next_p;
    }

    if (curLsn->m_pSettings->m_strictlyStoreLmpsofarInAds
        && *longestPfxLen_p < curLsn->m_nDepth)
    {
        kaps_jr1_trie_node *trienode = NULL;
        void * tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(curLsn->m_pParentHandle);
        kaps_jr1_memcpy(&trienode , tmp_ptr, sizeof(kaps_jr1_trie_node *));

        if (trienode->m_iitLmpsofarPfx_p && 
            trienode->m_iitLmpsofarPfx_p->m_nPfxSize == trienode->m_depth)
        {
            *longestPfxLen_p = trienode->m_iitLmpsofarPfx_p->m_nPfxSize; 
        }
    }
}

NlmErrNum_t
kaps_jr1_trie_merge_ancestor_lsns(
    kaps_jr1_lpm_trie * trie_p,
    kaps_jr1_trie_node * downStreamNode_p,
    NlmReasonCode * o_reason)
{
    kaps_jr1_trie_node *iterNode_p, *upStreamNode_p;
    kaps_jr1_lsn_mc *upStreamLsn, *downStreamLsn;
    kaps_jr1_lsn_mc_settings *settings_p = trie_p->m_lsn_settings_p;
    kaps_jr1_fib_prefix_index_changed_app_cb_t appCB = settings_p->m_pAppCB;
    kaps_jr1_lpm_lpu_brick *iterBrick, *lastUpStreamBrick;
    kaps_jr1_flat_lsn_data *mergedFlatLsnData = NULL;
    kaps_jr1_lsn_mc *tempMergedLsn = NULL, *newMergedLsn = NULL;
    NlmErrNum_t errNum;
    uint32_t i, j, brickNum;
    kaps_jr1_pfx_bundle *toDeletePfxCopy = NULL, *curPfxBundle;
    uint32_t doesLsnFit;
    kaps_jr1_fib_tbl *tbl;
    uint32_t totalNumExtraPfx;

    if (!settings_p->m_isAncestorLsnMergeEnabled)
        return NLMERR_OK;

    if (downStreamNode_p->m_isRptNode)
        return NLMERR_OK;

    if (trie_p->m_tbl_ptr->m_db->is_destroy_in_progress)
        return NLMERR_OK;

    iterNode_p = downStreamNode_p->m_parent_p;
    upStreamNode_p = NULL;
    while (iterNode_p)
    {
        if (iterNode_p->m_node_type == NLMNSTRIENODE_LP)
        {
            upStreamNode_p = iterNode_p;
            break;
        }

        if (iterNode_p->m_isRptNode)
            break;

        iterNode_p = iterNode_p->m_parent_p;
    }

    if (!upStreamNode_p)
        return NLMERR_OK;

    upStreamLsn = upStreamNode_p->m_lsn_p;
    tbl = upStreamLsn->m_pTbl;

    if (!upStreamLsn->m_lpuList)
        return NLMERR_OK;

    
    if (upStreamLsn->m_nDepth < trie_p->m_expansion_depth)
        return NLMERR_OK;

    downStreamLsn = downStreamNode_p->m_lsn_p;

    if (!downStreamLsn->m_lpuList)
        return NLMERR_OK;

    if (!settings_p->m_isPerLpuGran)
    {
        uint32_t totalNumPfx = downStreamLsn->m_nNumPrefixes + upStreamLsn->m_nNumPrefixes;
        uint32_t longestPfxLen, longestPfxGran, maxPfxPerBrick, numBricksNeeded;
        struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) settings_p->m_fibTbl->m_db->common_info->ad_info.ad;

        longestPfxLen = 0;
        kaps_jr1_trie_pvt_find_longest_pfx_len(downStreamLsn, &longestPfxLen);
        kaps_jr1_trie_pvt_find_longest_pfx_len(upStreamLsn, &longestPfxLen);

        longestPfxLen -= upStreamLsn->m_nDepth;

        longestPfxGran = kaps_jr1_lsn_mc_compute_gran(settings_p, longestPfxLen, NULL);
        maxPfxPerBrick = kaps_jr1_lsn_mc_calc_max_pfx_in_lpu_brick(settings_p, ad_db, 0, 0xf, longestPfxGran);
        numBricksNeeded = (totalNumPfx + maxPfxPerBrick - 1) / maxPfxPerBrick;

        if (numBricksNeeded >= settings_p->m_maxLpuPerLsn / 2)
            return NLMERR_OK;

    }
    else
    {
        if (downStreamLsn->m_numLpuBricks + upStreamLsn->m_numLpuBricks >= (settings_p->m_maxLpuPerLsn + 1) / 2)
            return NLMERR_OK;
    }

    mergedFlatLsnData = kaps_jr1_lsn_mc_create_flat_lsn_data(settings_p->m_pAlloc, o_reason);
    if (!mergedFlatLsnData)
    {
        *o_reason = NLMRSC_REASON_OK;
        return NLMERR_OK;
    }

    
    tempMergedLsn = kaps_jr1_lsn_mc_create(settings_p, trie_p, upStreamLsn->m_nDepth);
    if (!tempMergedLsn)
    {
        kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    
    lastUpStreamBrick = NULL;
    iterBrick = upStreamLsn->m_lpuList;
    while (iterBrick)
    {
        lastUpStreamBrick = iterBrick;
        iterBrick = iterBrick->m_next_p;
    }

    
    lastUpStreamBrick->m_next_p = downStreamLsn->m_lpuList;

    
    tempMergedLsn->m_lpuList = upStreamLsn->m_lpuList;
    tempMergedLsn->m_numLpuBricks = upStreamLsn->m_numLpuBricks + downStreamLsn->m_numLpuBricks;
    tempMergedLsn->m_nLsnCapacity = upStreamLsn->m_nLsnCapacity + downStreamLsn->m_nLsnCapacity;
    tempMergedLsn->m_nNumPrefixes = upStreamLsn->m_nNumPrefixes + downStreamLsn->m_nNumPrefixes;
    tempMergedLsn->m_pParentHandle = upStreamLsn->m_pParentHandle;

    totalNumExtraPfx = 0;
    
    if (trie_p->m_lsn_settings_p->m_strictlyStoreLmpsofarInAds)
    {
        
        if (downStreamNode_p->m_iitLmpsofarPfx_p &&
            downStreamNode_p->m_depth == downStreamNode_p->m_iitLmpsofarPfx_p->m_nPfxSize)
        {
            trie_p->m_trie_global->m_extraPfxArray[totalNumExtraPfx] = downStreamNode_p->m_iitLmpsofarPfx_p;
            totalNumExtraPfx++;
        }
    }

    
    errNum = kaps_jr1_lsn_mc_convert_lsn_to_flat_data(tempMergedLsn, 
                    trie_p->m_trie_global->m_extraPfxArray, totalNumExtraPfx, 
                    mergedFlatLsnData, o_reason);

    if (errNum != NLMERR_OK)
    {
        kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        lastUpStreamBrick->m_next_p = NULL;
        return NLMERR_OK;
    }

    
    lastUpStreamBrick->m_next_p = NULL;

    
    newMergedLsn = kaps_jr1_lsn_mc_create(settings_p, trie_p, upStreamLsn->m_nDepth);
    if (!newMergedLsn)
    {
        kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    newMergedLsn->m_pParentHandle = upStreamLsn->m_pParentHandle;

    
    for (i = 0; i < mergedFlatLsnData->m_numPfxInLsn; i++)
    {
        curPfxBundle = mergedFlatLsnData->m_pfxesInLsn[i];

        if (curPfxBundle->m_isPfxCopy && curPfxBundle->m_nPfxSize > newMergedLsn->m_nDepth)
        {
            toDeletePfxCopy = curPfxBundle;
            mergedFlatLsnData->m_pfxesInLsn[i] = NULL;
            for (j = i + 1; j < mergedFlatLsnData->m_numPfxInLsn; j++, i++)
            {
                mergedFlatLsnData->m_pfxesInLsn[i] = mergedFlatLsnData->m_pfxesInLsn[j];
                mergedFlatLsnData->pfx_color[i] = mergedFlatLsnData->pfx_color[j];
            }
            mergedFlatLsnData->m_numPfxInLsn--;
            break;
        }
    }

    
    
    errNum = kaps_jr1_lsn_mc_convert_flat_data_to_lsn(mergedFlatLsnData, newMergedLsn, &doesLsnFit, o_reason);
    if (errNum != NLMERR_OK || !doesLsnFit)
    {
        kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    
    kaps_jr1_nlm_allocator_free(settings_p->m_pAlloc, (void *) tempMergedLsn);
    tempMergedLsn = NULL;

    kaps_jr1_lsn_mc_destroy_flat_lsn_data(settings_p->m_pAlloc, mergedFlatLsnData);
    mergedFlatLsnData = NULL;

    
    if (!settings_p->m_isFullWidthLsn)
    {
        if (newMergedLsn->m_numLpuBricks >= (settings_p->m_maxLpuPerLsn + 1) / 2)
        {
            kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
            return NLMERR_OK;
        }
    }

    
    errNum =
        kaps_jr1_lsn_mc_acquire_resources(newMergedLsn, newMergedLsn->m_nLsnCapacity, newMergedLsn->m_numLpuBricks,
                                      o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    if (settings_p->m_isJoinedUdc && settings_p->m_isPerLpuGran)
    {
        errNum = kaps_jr1_lsn_mc_rearrange_prefixes_for_joined_udcs(newMergedLsn, o_reason);

        if (errNum != NLMERR_OK)
            return errNum;
    }

    
    if (settings_p->m_isPerLpuGran)
    {
        iterBrick = newMergedLsn->m_lpuList;
        brickNum = 0;

        while (iterBrick)
        {
            errNum = kaps_jr1_lsn_mc_acquire_resources_per_lpu(newMergedLsn, iterBrick, brickNum, iterBrick->m_maxCapacity,
                                                           iterBrick->ad_db, o_reason);
            if (errNum != NLMERR_OK)
            {
                kaps_jr1_lsn_mc_free_resources(newMergedLsn);
                kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
                return NLMERR_OK;
            }

            brickNum++;
            iterBrick = iterBrick->m_next_p;
        }

        newMergedLsn->m_nNumIxAlloced = newMergedLsn->m_nLsnCapacity;
    }

    kaps_jr1_lsn_mc_add_extra_brick_for_joined_udcs(newMergedLsn, o_reason);

    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(upStreamLsn);
    kaps_jr1_lsn_mc_get_hit_bits_for_lsn_from_hw(downStreamLsn);

    kaps_jr1_lsn_mc_store_old_lsn_info(upStreamLsn, 0);
    kaps_jr1_lsn_mc_store_old_lsn_info(downStreamLsn, 1);

    
    errNum = kaps_jr1_lsn_mc_commit(newMergedLsn, 0, o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    
    errNum = kaps_jr1_lsn_mc_update_iit(newMergedLsn, o_reason);
    if (errNum != NLMERR_OK)
    {
        kaps_jr1_trie_pvt_undo_ancestor_lsn_merge(trie_p, tempMergedLsn, newMergedLsn, mergedFlatLsnData);
        return NLMERR_OK;
    }

    if (toDeletePfxCopy)
    {
        if (appCB)
        {
            appCB(settings_p->m_pAppData, tbl, toDeletePfxCopy, toDeletePfxCopy->m_nIndex, KAPS_JR1_LSN_NEW_INDEX);
        }
        kaps_jr1_pfx_bundle_destroy(toDeletePfxCopy, trie_p->m_trie_global->m_alloc_p);
    }

    newMergedLsn->m_bIsNewLsn = 0;

    kaps_jr1_lsn_mc_destroy(upStreamLsn);

    upStreamNode_p->m_lsn_p = newMergedLsn;

    kaps_jr1_trie_pvt_in_place_delete(trie_p, downStreamNode_p, 1, o_reason);

    trie_p->m_tbl_ptr->m_fibStats.numLsnMerges++;

    return NLMERR_OK;
}

void
kaps_jr1_trie_update_hit_bits_iit_lmpsofar(
    kaps_jr1_trie_node * node_p)
{
    if (!node_p)
        return;

    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        if (node_p->m_iitLmpsofarPfx_p)
        {
            struct kaps_jr1_device *device = node_p->m_trie_p->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
            struct kaps_jr1_hb *hb_entry = NULL;

            if (node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
            {
                kaps_jr1_lpm_trie *trie_p = node_p->m_trie_p;
                struct kaps_jr1_db *db;
                struct kaps_jr1_aging_entry *active_aging_table;

                KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, node_p->m_iitLmpsofarPfx_p->m_backPtr, db)
                KAPS_JR1_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb_entry),
                                            (uintptr_t) node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);
                if (hb_entry == NULL)
                {
                    return;
                }
                active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);

                if (trie_p->m_iptHitBitBuffer)
                {
                    if (trie_p->m_iptHitBitBuffer[node_p->m_lp_prefix_p->m_nIndex])
                    {
                        active_aging_table[hb_entry->bit_no].num_idles = 0;
                    }

                }
            }
        }
    }

    if (node_p->m_isRptNode)
    {
        if (node_p->m_iitLmpsofarPfx_p)
        {
            struct kaps_jr1_device *device = node_p->m_trie_p->m_tbl_ptr->m_fibTblMgr_p->m_devMgr_p;
            struct kaps_jr1_hb *hb_entry = NULL;

            if (node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
            {
                kaps_jr1_lpm_trie *trie_p = node_p->m_trie_p;
                struct kaps_jr1_db *db;
                struct kaps_jr1_aging_entry *active_aging_table;

                KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, node_p->m_iitLmpsofarPfx_p->m_backPtr, db)
                    KAPS_JR1_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb_entry),
                                            (uintptr_t) node_p->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);
                if (hb_entry == NULL)
                {
                    return;
                }
                active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);

                if (trie_p->m_rptTcamHitBitBuffer)
                {
                    if (trie_p->m_rptTcamHitBitBuffer[node_p->m_rpt_prefix_p->m_nIndex])
                    {
                        active_aging_table[hb_entry->bit_no].num_idles = 0;
                    }
                }
            }
        }
    }

    if (node_p->m_child_p[0])
        kaps_jr1_trie_update_hit_bits_iit_lmpsofar(node_p->m_child_p[0]);

    if (node_p->m_child_p[1])
        kaps_jr1_trie_update_hit_bits_iit_lmpsofar(node_p->m_child_p[1]);
}

NlmErrNum_t
kaps_jr1_trie_process_hit_bits_iit_lmpsofar(
    kaps_jr1_lpm_trie * trie_p)
{
    kaps_jr1_lsn_mc_settings *settings = trie_p->m_lsn_settings_p;
    struct kaps_jr1_device *device = settings->m_device;
    kaps_jr1_pool_mgr *poolMgr = trie_p->m_trie_global->poolMgr;
    struct kaps_jr1_db *db = trie_p->m_tbl_ptr->m_db;
    uint32_t rpb_id = db->rpb_id;
    uint32_t num_rows_to_read = device->hw_res->num_rows_in_rpb[rpb_id] / KAPS_JR1_HB_ROW_WIDTH_1;
    struct kaps_jr1_ab_info *ab;
    uint32_t i, j, pos, offset;
    
    uint16_t value;

    if (settings->m_areIPTHitBitsPresent)
    {
        {

            kaps_jr1_ipm *ipm = kaps_jr1_pool_mgr_get_ipm_for_pool(poolMgr, trie_p->m_roots_trienode_p->m_poolId);

            if (trie_p->m_iptHitBitBuffer)
                kaps_jr1_memset(trie_p->m_iptHitBitBuffer, 0, device->hw_res->num_rows_in_rpb[rpb_id]);

            ab = ipm->m_ab_info;
            while (ab)
            {
                kaps_jr1_dm_hb_dump(device, db, device->dba_offset + ab->ab_num, 0,
                                     device->dba_offset + ab->ab_num, num_rows_to_read - 1, 1, device->hb_buffer);

                if (trie_p->m_iptHitBitBuffer)
                {
                    pos = 0;
                    offset = 0;
                    for (i = 0; i < num_rows_to_read; ++i)
                    {
                        value = KapsJr1ReadBitsInArrray(&device->hb_buffer[offset], KAPS_JR1_HB_ROW_WIDTH_8,
                                                 KAPS_JR1_HB_ROW_WIDTH_1 - 1, 0);

                        for (j = 0; j < KAPS_JR1_HB_ROW_WIDTH_1; ++j)
                        {
                            if (value & (1u << j))
                            {
                                trie_p->m_iptHitBitBuffer[pos] = 1;
                            }
                            ++pos;
                        }
                        offset += KAPS_JR1_HB_ROW_WIDTH_8;
                    }
                }

                ab = ab->dup_ab;
            }
        }
    }

    if (settings->m_areRPTHitBitsPresent)
    {
        uint64_t rptMap;
        uint32_t rptNr;

        num_rows_to_read = device->hw_res->num_rows_in_rpb[rpb_id] / KAPS_JR1_HB_ROW_WIDTH_1;

        if (trie_p->m_rptTcamHitBitBuffer)
            kaps_jr1_memset(trie_p->m_rptTcamHitBitBuffer, 0, device->hw_res->num_rows_in_rpb[rpb_id]);

        rptMap = db->hw_res.db_res->rpt_map[0];

        for (rptNr = 0; rptNr < HW_MAX_PCM_BLOCKS; ++rptNr)
        {
            if (rptMap & (1ULL << rptNr))
            {
                kaps_jr1_dm_hb_dump(device, db, device->dba_offset + rptNr, 0,
                                     device->dba_offset + rptNr, num_rows_to_read - 1, 1, device->hb_buffer);
            }

            if (trie_p->m_rptTcamHitBitBuffer)
            {
                pos = 0;
                offset = 0;

                for (i = 0; i < num_rows_to_read; ++i)
                {
                    value = KapsJr1ReadBitsInArrray(&device->hb_buffer[offset], KAPS_JR1_HB_ROW_WIDTH_8,
                                             KAPS_JR1_HB_ROW_WIDTH_1 - 1, 0);

                    for (j = 0; j < KAPS_JR1_HB_ROW_WIDTH_1; ++j)
                    {
                        if (value & (1u << j))
                        {

                            if (pos >= device->hw_res->num_rows_in_rpb[rpb_id])
                                kaps_jr1_assert(0,
                                            "Incorrect RPB hit bit while copying hardware hit bits into software buffer\n");

                            trie_p->m_rptTcamHitBitBuffer[pos] = 1;
                        }
                        ++pos;
                    }

                    offset += KAPS_JR1_HB_ROW_WIDTH_8;
                }
            }
        }
    }

    if (settings->m_areIPTHitBitsPresent || settings->m_areRPTHitBitsPresent)
    {
        kaps_jr1_trie_update_hit_bits_iit_lmpsofar(trie_p->m_roots_trienode_p);
    }

    return NLMERR_OK;
}

void
kaps_jr1_trie_get_algo_hit_bit_from_hw(
    kaps_jr1_lpm_trie * trie,
    kaps_jr1_trie_node * curNode,
    uint32_t clear_on_read,
    uint32_t * bit_value)
{
    kaps_jr1_pool_mgr *poolMgr = trie->m_trie_global->poolMgr;
    struct kaps_jr1_device *device = trie->m_lsn_settings_p->m_device;
    struct kaps_jr1_ab_info *ab;
    kaps_jr1_ipm *ipm;
    kaps_jr1_status status;
    struct kaps_jr1_db *db = trie->m_tbl_ptr->m_db;


    {
        uint8_t hb_data[KAPS_JR1_HB_ROW_WIDTH_8];

        ipm = kaps_jr1_pool_mgr_get_ipm_for_pool(poolMgr, trie->m_roots_trienode_p->m_poolId);
        ab = ipm->m_ab_info;

        status = kaps_jr1_dm_hb_read(device, db, device->dba_offset + ab->ab_num, curNode->m_lp_prefix_p->m_nIndex, hb_data);

        if (status != KAPS_JR1_OK)
        	return;

        if (hb_data[0] & (1u << 7))
            *bit_value = 1;

        if (clear_on_read)
        {
            kaps_jr1_memset(hb_data, 0, KAPS_JR1_HB_ROW_WIDTH_8);

            kaps_jr1_dm_hb_write(device, db, device->dba_offset + ab->ab_num, curNode->m_lp_prefix_p->m_nIndex, hb_data);
        }

    }
}

void
kaps_jr1_trie_explore_trie_for_algo_hit_bit(
    kaps_jr1_lpm_trie * trie,
    kaps_jr1_pfx_bundle * pfxBundle,
    kaps_jr1_trie_node * curNode,
    uint32_t clear_on_read,
    uint32_t * bit_value)
{
    if (curNode->m_node_type == NLMNSTRIENODE_LP)
    {
        if (curNode->m_iitLmpsofarPfx_p != pfxBundle)
        {
            return;
        }
        else
        {
            kaps_jr1_trie_get_algo_hit_bit_from_hw(trie, curNode, clear_on_read, bit_value);

            
            if (*bit_value && !clear_on_read)
            {
                return;
            }
        }
    }

    if (curNode->m_child_p[0])
        kaps_jr1_trie_explore_trie_for_algo_hit_bit(trie, pfxBundle, curNode->m_child_p[0], clear_on_read, bit_value);

    
    if (*bit_value && !clear_on_read)
    {
        return;
    }

    if (curNode->m_child_p[1])
        kaps_jr1_trie_explore_trie_for_algo_hit_bit(trie, pfxBundle, curNode->m_child_p[1], clear_on_read, bit_value);

}

NlmErrNum_t
kaps_jr1_trie_get_algo_hit_bit_value(
    kaps_jr1_lpm_trie * trie,
    struct kaps_jr1_lpm_entry *entry,
    uint8_t clear_on_read,
    uint32_t * bit_value)
{
    uint32_t i, val;
    kaps_jr1_pfx_bundle *pfxBundle = entry->pfx_bundle;
    kaps_jr1_trie_node *node = trie->m_roots_trienode_p;
    struct kaps_jr1_lpm_db *lpm_db = (struct kaps_jr1_lpm_db *) trie->m_tbl_ptr->m_db;

    if (!trie->m_lsn_settings_p->m_areIPTHitBitsPresent)
    {
        return NLMERR_OK;
    }

    
    if (lpm_db->is_entry_delete_in_progress)
        clear_on_read = 0;

    
    if (!node)
        return NLMERR_OK;

    for (i = node->m_depth; i < pfxBundle->m_nPfxSize; i++)
    {
        val = KAPS_JR1_PREFIX_PVT_GET_BIT(pfxBundle->m_data, pfxBundle->m_nPfxSize, i);

        node = node->m_child_p[val];
        if (!node)
            break;
    }

    
    if (!node)
        return NLMERR_OK;

    
    if (node->m_child_p[0])
        kaps_jr1_trie_explore_trie_for_algo_hit_bit(trie, pfxBundle, node->m_child_p[0], clear_on_read, bit_value);

    if (node->m_child_p[1])
        kaps_jr1_trie_explore_trie_for_algo_hit_bit(trie, pfxBundle, node->m_child_p[1], clear_on_read, bit_value);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_trie_convert_status_to_err_num(
    kaps_jr1_status status,
    NlmReasonCode * o_reason_p)
{
    NlmErrNum_t errNum = NLMERR_OK;
    switch (status)
    {
        case KAPS_JR1_OK:
            errNum = NLMERR_OK;
            *o_reason_p = NLMRSC_REASON_OK;
            break;

        case KAPS_JR1_INVALID_ARGUMENT:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_INVALID_PARAM;
            break;

        case KAPS_JR1_OUT_OF_MEMORY:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_LOW_MEMORY;
            break;

        case KAPS_JR1_INTERNAL_ERROR:
            errNum = NLMRSC_INTERNAL_ERROR;
            *o_reason_p = NLMRSC_INTERNAL_ERROR;
            break;

        case KAPS_JR1_OUT_OF_UDA:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_UDA_ALLOC_FAILED;
            break;

        case KAPS_JR1_OUT_OF_DBA:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_DBA_ALLOC_FAILED;
            break;

        case KAPS_JR1_OUT_OF_INDEX:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_IDX_RANGE_FULL;
            break;

        case KAPS_JR1_OUT_OF_UIT:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_IT_ALLOC_FAILED;
            break;

        case KAPS_JR1_EXHAUSTED_PCM_RESOURCE:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_PCM_ALLOC_FAILED;
            break;

        case KAPS_JR1_OUT_OF_AD:
            errNum = NLMERR_FAIL;
            *o_reason_p = NLMRSC_OUT_OF_AD;
            break;

        default:
            errNum = NLMRSC_INTERNAL_ERROR;
            *o_reason_p = NLMRSC_INTERNAL_ERROR;
            break;
    }

    return errNum;
}



NlmErrNum_t
kaps_jr1_trie_wb_save_pools(
    kaps_jr1_trie_global * trie_g,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    uint32_t num_active_pools;
    int32_t i;

    
    num_active_pools = 0;
    for (i = 0; i < trie_g->poolMgr->m_numActiveIptPools; i++)
    {
        if (trie_g->poolMgr->m_ipmPoolInfo[i].m_isInUse)
            num_active_pools++;
    }
    if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & num_active_pools,
                              sizeof(num_active_pools), *wb_fun->nv_offset))
        return NLMERR_FAIL;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_active_pools);

    num_active_pools = 0;
    for (i = 0; i < trie_g->poolMgr->m_numActiveAptPools; i++)
    {
        if (trie_g->poolMgr->m_apmPoolInfo[i].m_isInUse)
            num_active_pools++;
    }

    if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & num_active_pools,
                              sizeof(num_active_pools), *wb_fun->nv_offset))
        return NLMERR_FAIL;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_active_pools);

    return kaps_jr1_pool_mgr_wb_save_pools(trie_g->poolMgr, wb_fun->write_fn, wb_fun->handle, wb_fun->nv_offset);
}

void
kaps_jr1_trie_cr_save_pools(
    kaps_jr1_trie_global * trie_g,
    struct kaps_jr1_wb_cb_functions *wb_fun)
{
    uint32_t *num_active_pools, num_pools;
    int32_t i;

    if (!trie_g->poolMgr->m_are_pools_updated)
    {
        return;
    }

    trie_g->poolMgr->m_are_pools_updated = 0;

    num_active_pools = (uint32_t *) wb_fun->nv_ptr;
    wb_fun->nv_ptr += sizeof(*num_active_pools);

    
    num_pools = 0;
    for (i = 0; i < trie_g->poolMgr->m_numActiveIptPools; i++)
    {
        if (trie_g->poolMgr->m_ipmPoolInfo[i].m_isInUse)
            num_pools++;
    }
    *num_active_pools = num_pools;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*num_active_pools);

    num_active_pools = (uint32_t *) wb_fun->nv_ptr;
    wb_fun->nv_ptr += sizeof(*num_active_pools);

    num_pools = 0;
    for (i = 0; i < trie_g->poolMgr->m_numActiveAptPools; i++)
    {
        if (trie_g->poolMgr->m_apmPoolInfo[i].m_isInUse)
            num_pools++;
    }
    *num_active_pools = num_pools;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*num_active_pools);

    kaps_jr1_pool_mgr_cr_save_pools(trie_g->poolMgr, wb_fun);
}

kaps_jr1_status
kaps_jr1_trie_wb_save_rpt_ipt_and_apt_entries(
    kaps_jr1_trie_node * trie_node,
    uint32_t * num_rpt_entries,
    uint32_t * nv_offset,
    kaps_jr1_pool_mgr * pool_mgr,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle)
{
    kaps_jr1_pool_info *pool_info = NULL;
    uint32_t pool_ix, num_entries;

    if (trie_node == NULL)
        return 0;

    if (trie_node->m_isRptNode)
    {
        if (0 != kaps_jr1_trienode_wb_save_rpt(trie_node, nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
        (*num_rpt_entries)++;

        pool_ix = KAPS_JR1_CONVERT_POOL_ID_TO_IPT_POOL_INDEX(trie_node->m_poolId);
        if (pool_ix != KAPS_JR1_INVALID_POOL_INDEX)
        {
            pool_info = &pool_mgr->m_ipmPoolInfo[pool_ix];

            num_entries =
                kaps_jr1_ipm_wb_store_ipt_entries(pool_info->m_dba_mgr, trie_node->m_rptId, nv_offset, write_fn, handle);
            if (num_entries == -1)
                return KAPS_JR1_NV_READ_WRITE_FAILED;
            kaps_jr1_assert((num_entries == trie_node->m_numIptEntriesInSubtrie), "Num IPT entries mismatch");
        }

    }

    if (trie_node->m_child_p[0])
        KAPS_JR1_STRY(kaps_jr1_trie_wb_save_rpt_ipt_and_apt_entries(trie_node->m_child_p[0], num_rpt_entries, nv_offset,
                                                                 pool_mgr, write_fn, handle));

    if (trie_node->m_child_p[1])
        KAPS_JR1_STRY(kaps_jr1_trie_wb_save_rpt_ipt_and_apt_entries(trie_node->m_child_p[1], num_rpt_entries, nv_offset,
                                                                 pool_mgr, write_fn, handle));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_trie_wb_save(
    kaps_jr1_lpm_trie * trie_p,
    uint32_t * nv_offset)
{
    kaps_jr1_trie_global *trie_g = trie_p->m_trie_global;
    uint32_t num_rpts_offset, num_rpt_entries = 0;
    uint16_t no_of_regions;
    struct region region_info[MAX_UDA_REGIONS];
    struct kaps_jr1_ad_db *ad_db;
    struct kaps_jr1_ix_mgr *ix_mgr;
    uint32_t i;
    struct kaps_jr1_device *device = trie_g->fibtblmgr->m_devMgr_p;
    struct kaps_jr1_db *db= trie_p->m_tbl_ptr->m_db;

    if (0 != trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_g->m_are_reserved_160b_abs_initialized,
                                      sizeof(trie_g->m_are_reserved_160b_abs_initialized), *nv_offset))
    {
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    }
    *nv_offset += sizeof(trie_g->m_are_reserved_160b_abs_initialized);

    if (0 !=
        trie_g->wb_fun->write_fn(trie_g->wb_fun->handle,
                                 (uint8_t *) & trie_p->m_lsn_settings_p->m_numAllocatedBricksForAllLsns,
                                 sizeof(trie_p->m_lsn_settings_p->m_numAllocatedBricksForAllLsns), *nv_offset))
    {
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    }
    *nv_offset += sizeof(trie_p->m_lsn_settings_p->m_numAllocatedBricksForAllLsns);

    if (0 != trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_p->m_lsn_settings_p->m_maxLpuPerLsn,
                                      sizeof(trie_p->m_lsn_settings_p->m_maxLpuPerLsn), *nv_offset))
    {
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    }
    *nv_offset += sizeof(trie_p->m_lsn_settings_p->m_maxLpuPerLsn);

    
    no_of_regions = trie_g->m_mlpmemmgr[0]->no_of_regions;
    if (0 !=
        trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & no_of_regions, sizeof(no_of_regions),
                                 *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(no_of_regions);

    
    kaps_jr1_memcpy(region_info, trie_g->m_mlpmemmgr[0]->region_info, sizeof(region_info));
    if (0 !=
        trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & region_info, sizeof(region_info), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(region_info);

    
    num_rpts_offset = *nv_offset;
    *nv_offset += sizeof(num_rpt_entries);

    ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;

    while (ad_db) 
    {
        for (i = 0; i < device->hw_res->num_daisy_chains; ++i) {
            ix_mgr = ad_db->mgr[0][i];
            KAPS_JR1_STRY(kaps_jr1_ix_mgr_wb_save_ix_translate
                  (ix_mgr, trie_g->wb_fun->write_fn, trie_g->wb_fun->handle, nv_offset));
        }

        ad_db = ad_db->next;
    }

    
    KAPS_JR1_STRY(kaps_jr1_trie_wb_save_rpt_ipt_and_apt_entries(trie_p->m_roots_trienode_p, &num_rpt_entries, nv_offset,
                                                             trie_g->poolMgr, trie_g->wb_fun->write_fn,
                                                             trie_g->wb_fun->handle));

    if (0 !=
        trie_g->wb_fun->write_fn(trie_g->wb_fun->handle, (uint8_t *) & num_rpt_entries, sizeof(num_rpt_entries),
                                 num_rpts_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;

    return KAPS_JR1_OK;
}

NlmErrNum_t
kaps_jr1_trie_wb_cr_post_process(
    kaps_jr1_trie_node * rpt_parent,
    kaps_jr1_trie_node * curr_node,
    kaps_jr1_trie_node * highestPriorityAptTrieNode,
    uint8_t is_crash_recovery,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;

    if (curr_node == NULL)
        return NLMERR_OK;

    if (curr_node->m_isRptNode)
    {
        rpt_parent = curr_node;

        if (is_crash_recovery && highestPriorityAptTrieNode)
        {
            if (curr_node->m_rptAptLmpsofarPfx)
            {
                curr_node->m_rptAptLmpsofarPfx->m_backPtr = highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr;
            }
        }
    }

    
    if (is_crash_recovery)
    {
        if (highestPriorityAptTrieNode)
        {
            if (curr_node->m_aptLmpsofarPfx_p
                && curr_node->m_aptLmpsofarPfx_p->m_backPtr->meta_priority <=
                highestPriorityAptTrieNode->m_aptLmpsofarPfx_p->m_backPtr->meta_priority)
            {
                highestPriorityAptTrieNode = curr_node;
            }
        }
        else
        {
            if (curr_node->m_aptLmpsofarPfx_p)
            {
                highestPriorityAptTrieNode = curr_node;
            }
        }
    }

    curr_node->m_rptParent_p = rpt_parent;
    if (rpt_parent)
        curr_node->m_poolId = rpt_parent->m_poolId;

    errNum =
        kaps_jr1_trie_wb_cr_post_process(rpt_parent, curr_node->m_child_p[0], highestPriorityAptTrieNode, is_crash_recovery,
                                     o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    errNum =
        kaps_jr1_trie_wb_cr_post_process(rpt_parent, curr_node->m_child_p[1], highestPriorityAptTrieNode, is_crash_recovery,
                                     o_reason);

    return errNum;
}

NlmErrNum_t
kaps_jr1_trie_wb_restore_ipt_entry(
    kaps_jr1_lpm_trie * trie,
    kaps_jr1_trie_node * rpt_parent,
    struct kaps_jr1_wb_ipt_entry_info * ipt_node,
    uint32_t * nv_offset)
{
    kaps_jr1_nlm_allocator *alloc = NULL;
    kaps_jr1_ipm *ipm = NULL;
    kaps_jr1_trie_node *ipt_trienode = NULL;
    kaps_jr1_prefix *pfx = NULL;
    NlmErrNum_t err_num = NLMERR_OK;
    uint32_t ipt_depth, ipt_length_1, rpt_lopoff_1;
    NlmReasonCode o_reason;

    if (trie == NULL || ipt_node == NULL)
        return NLMERR_FAIL;

    alloc = trie->m_trie_global->m_alloc_p;

    
    ipt_length_1 = ipt_node->ipt_entry_len_1;
    rpt_lopoff_1 =
        kaps_jr1_trie_get_num_rpt_bits_lopped_off(trie->m_trie_global->fibtblmgr->m_devMgr_p, rpt_parent->m_depth);
    ipt_depth = ipt_length_1 + rpt_lopoff_1;

    pfx = kaps_jr1_prefix_create(alloc, ipt_depth, ipt_depth, ipt_node->ipt_entry);
    if (!pfx)
        return NLMERR_FAIL;

    
    if (ipt_depth)
        ipt_trienode = kaps_jr1_trie_node_insertPathFromPrefix(trie->m_roots_trienode_p, pfx,
                                                           trie->m_roots_trienode_p->m_depth, (ipt_depth - 1));
    else
        ipt_trienode = rpt_parent;

    if (ipt_trienode == NULL)
        return NLMERR_FAIL;

    ipt_trienode->m_node_type = NLMNSTRIENODE_LP;

    ipt_trienode->m_rptParent_p = rpt_parent;

    ipt_trienode->m_isReserved160bTrig = ipt_node->is_reserved_160b_trig;

    ipm = kaps_jr1_pool_mgr_get_ipm_for_pool(trie->m_trie_global->poolMgr, rpt_parent->m_poolId);
    if (ipm == NULL)
        return NLMERR_FAIL;

    
    err_num = kaps_jr1_ipm_wb_add_entry(ipm, ipt_trienode->m_lp_prefix_p, rpt_parent->m_rptId,
                                    ipt_length_1, ipt_node->addr);
    if (err_num != NLMERR_OK)
        return err_num;

    
    err_num = kaps_jr1_ipm_write_entry_to_hw(ipm, ipt_trienode->m_lp_prefix_p, rpt_parent->m_rptId,
                                         ipt_length_1, rpt_lopoff_1, &o_reason);
    if (err_num != NLMERR_OK)
        return err_num;

    err_num = kaps_jr1_lsn_mc_wb_create_lsn(ipt_trienode->m_lsn_p, &ipt_node->lsn_info, &o_reason);
    if (err_num != NLMERR_OK)
        return err_num;

    err_num = kaps_jr1_lsn_mc_commitIIT(ipt_trienode->m_lsn_p, ipt_trienode->m_lp_prefix_p->m_nIndex, &o_reason);
    if (err_num != NLMERR_OK)
        return err_num;

    err_num =
        kaps_jr1_lsn_mc_wb_restore_pfx_data(ipt_trienode->m_lsn_p, nv_offset, trie->m_trie_global->wb_fun->read_fn,
                                        trie->m_trie_global->wb_fun->handle);
    if (err_num != NLMERR_OK)
        return err_num;

    kaps_jr1_prefix_destroy(pfx, alloc);

    kaps_jr1_ipm_verify(ipm, rpt_parent->m_rptId);

    return err_num;
}

NlmErrNum_t
kaps_jr1_trie_cr_restore_ipt_entry(
    kaps_jr1_lpm_trie * trie,
    kaps_jr1_trie_node * rpt_parent,
    struct kaps_jr1_wb_ipt_entry_info * ipt_node)
{
    kaps_jr1_nlm_allocator *alloc = NULL;
    kaps_jr1_ipm *ipm = NULL;
    kaps_jr1_trie_node *ipt_trienode = NULL;
    kaps_jr1_prefix *pfx = NULL;
    NlmErrNum_t err_num = NLMERR_OK;
    uint32_t ipt_depth_1, ipt_length_1, rpt_lopoff_1;
    NlmReasonCode o_reason;

    if (trie == NULL || ipt_node == NULL)
        return NLMERR_FAIL;

    alloc = trie->m_trie_global->m_alloc_p;

    
    ipt_length_1 = ipt_node->ipt_entry_len_1;
    rpt_lopoff_1 =
        kaps_jr1_trie_get_num_rpt_bits_lopped_off(trie->m_trie_global->fibtblmgr->m_devMgr_p, rpt_parent->m_depth);
    ipt_depth_1 = ipt_length_1 + rpt_lopoff_1;

    pfx = kaps_jr1_prefix_create(alloc, ipt_depth_1, ipt_depth_1, ipt_node->ipt_entry);
    if (!pfx)
        return NLMERR_FAIL;

    
    if (ipt_depth_1)
        ipt_trienode = kaps_jr1_trie_node_insertPathFromPrefix(trie->m_roots_trienode_p, pfx,
                                                           trie->m_roots_trienode_p->m_depth, (ipt_depth_1 - 1));
    else
        ipt_trienode = rpt_parent;

    if (ipt_trienode == NULL)
        return NLMERR_FAIL;

    ipt_trienode->m_node_type = NLMNSTRIENODE_LP;

    ipt_trienode->m_rptParent_p = rpt_parent;

    ipm = kaps_jr1_pool_mgr_get_ipm_for_pool(trie->m_trie_global->poolMgr, rpt_parent->m_poolId);
    if (ipm == NULL)
        return NLMERR_FAIL;

    
    err_num = kaps_jr1_ipm_wb_add_entry(ipm, ipt_trienode->m_lp_prefix_p, rpt_parent->m_rptId,
                                    ipt_length_1, ipt_node->addr);
    if (err_num != NLMERR_OK)
        return err_num;

    err_num = kaps_jr1_lsn_mc_recover_rebuild_lsn(&ipt_node->lsn_info, ipt_trienode->m_lsn_p, pfx, &o_reason);
    if (err_num != NLMERR_OK)
        return err_num;

    kaps_jr1_prefix_destroy(pfx, alloc);

    kaps_jr1_ipm_verify(ipm, rpt_parent->m_rptId);

    return err_num;
}

static uint32_t
kaps_jr1_get_length_of_prefix_from_mask(
    uint8_t * data,
    uint8_t num_bytes)
{
    uint32_t len = 0;
    uint32_t i = 0;

    for (i = 0; i < num_bytes; i++)
    {
        if (data[i] == 0)
            len += 8;
        else
            break;
    }

    if (i < num_bytes)
    {
        uint8_t byte = data[i];
        uint8_t num_of_ones = 0;

        while (byte)
        {
            num_of_ones++;
            byte >>= 1;
        }

        len += (8 - num_of_ones);
    }

    return len;
}

NlmErrNum_t
kaps_jr1_trie_cr_restore_rpt_entry(
    kaps_jr1_lpm_trie * trie,
    kaps_jr1_trie_node * rpt_trienode,
    struct kaps_jr1_wb_rpt_entry_info * rpt_entry_info)
{
    kaps_jr1_trie_global *trie_g = trie->m_trie_global;
    uint32_t ab_num, row_num = 0, lpu_no, num_lpu, brick_no, region_id, chunk_offset;
    struct kaps_jr1_device *device_p = trie_g->fibtblmgr->m_devMgr_p;
    struct kaps_jr1_wb_ipt_entry_info ipt_node = {0};
    NlmErrNum_t err_num = NLMERR_OK;
    kaps_jr1_ipm *ipm_p;

    ipm_p = kaps_jr1_pool_mgr_get_ipm_for_pool(trie->m_trie_global->poolMgr, rpt_trienode->m_poolId);
    if (ipm_p)
    {
        ab_num = ipm_p->m_ab_info->ab_num;
        for (row_num = 0; row_num < ipm_p->m_ab_info->num_slots; row_num++)
        {
            if (device_p->type == KAPS_JR1_DEVICE)
            {
                struct kaps_jr1_dba_entry data, mask;
                struct kaps_jr1_ads ads_info;
                uint8_t ads_read_buf[16];

                KAPS_JR1_STRY(kaps_jr1_dm_rpb_read(device_p, ab_num, row_num, &data, &mask));
                if (device_p->kaps_jr1_shadow->rpb_blks[ab_num].rpb_rows[row_num].rpb_tbl_id != trie->m_tbl_ptr->m_tblId)
                    continue;
                if (data.is_valid && mask.is_valid)
                {
                    ipt_node.addr = row_num;
                    ipt_node.blk_num = ab_num;
                    ipt_node.ipt_entry_len_1 = kaps_jr1_get_length_of_prefix_from_mask(mask.key, KAPS_JR1_LPM_KEY_MAX_WIDTH_8);
                    kaps_jr1_memcpy(ipt_node.ipt_entry, data.key, KAPS_JR1_LPM_KEY_MAX_WIDTH_8);

                    KAPS_JR1_STRY(kaps_jr1_dm_iit_read
                              (trie_g->fibtblmgr->m_devMgr_p, ab_num, row_num, sizeof(ads_info), ads_read_buf));
                    ads_info.bpm_ad = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 19, 0);
                    ads_info.bpm_len = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 27, 20);
                    ads_info.row_offset = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 31, 28);

                    ads_info.format_map_00 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 35, 32);
                    ads_info.format_map_01 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 39, 36);
                    ads_info.format_map_02 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 43, 40);
                    ads_info.format_map_03 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 47, 44);
                    ads_info.format_map_04 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 51, 48);
                    ads_info.format_map_05 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 55, 52);
                    ads_info.format_map_06 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 59, 56);
                    ads_info.format_map_07 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 63, 60);
                    ads_info.format_map_08 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 67, 64);
                    ads_info.format_map_09 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 71, 68);
                    ads_info.format_map_10 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 75, 72);
                    ads_info.format_map_11 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 79, 76);
                    ads_info.format_map_12 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 83, 80);
                    ads_info.format_map_13 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 87, 84);
                    ads_info.format_map_14 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 91, 88);
                    ads_info.format_map_15 = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 95, 92);

                    ads_info.bkt_row = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 108, 96);
                    ads_info.reserved = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 111, 109);
                    ads_info.key_shift = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 119, 112);
                    ads_info.ecc = KapsJr1ReadBitsInArrray(ads_read_buf, 16, 127, 120);

                    KAPS_JR1_STRY(kaps_jr1_uda_mgr_get_relative_params_from_abs((*(trie->m_lsn_settings_p->m_pMlpMemMgr)),
                                                                        ads_info.row_offset, ads_info.bkt_row,
                                                                        &region_id, &chunk_offset));
                    ipt_node.lsn_info.region_id = region_id;
                    ipt_node.lsn_info.offset = chunk_offset;

                    num_lpu = (*(trie->m_lsn_settings_p->m_pMlpMemMgr))->region_info[0].num_lpu;
                    brick_no = 0;
                    ipt_node.lsn_info.num_lpus = 0;

                    while (num_lpu)
                    {
                        uint32_t gran_ix;
                        lpu_no =
                            kaps_jr1_uda_mgr_compute_brick_udc_from_region_id((*
                                                                                (trie->m_lsn_settings_p->m_pMlpMemMgr)),
                                                                               ipt_node.lsn_info.region_id,
                                                                               ipt_node.lsn_info.offset, brick_no);
                        err_num = kaps_jr1_format_map_get_gran(trie->m_lsn_settings_p, &ads_info, lpu_no, &gran_ix);
                        if (err_num != NLMERR_OK)
                            return err_num;
                        if (gran_ix != -1)
                        {
                            ipt_node.lsn_info.brick_info[ipt_node.lsn_info.num_lpus].gran_ix = gran_ix;
                            ipt_node.lsn_info.brick_info[ipt_node.lsn_info.num_lpus].gran
                                = trie->m_lsn_settings_p->m_granIxToGran[gran_ix];
                            ipt_node.lsn_info.brick_info[ipt_node.lsn_info.num_lpus].base_address = 0;
                            ipt_node.lsn_info.brick_info[ipt_node.lsn_info.num_lpus].ad_db
                                = (struct kaps_jr1_ad_db *) trie->m_tbl_ptr->m_db->common_info->ad_info.ad;
                            ipt_node.lsn_info.num_lpus++;
                        }
                        num_lpu--;
                        brick_no++;
                    }

                    err_num = kaps_jr1_trie_cr_restore_ipt_entry(trie, rpt_trienode, &ipt_node);
                    if (err_num != NLMERR_OK)
                        return err_num;
                    kaps_jr1_trie_incr_num_ipt_entries_in_subtrie(rpt_trienode, 1);
                    kaps_jr1_pool_mgr_incr_num_entries(trie->m_trie_global->poolMgr, rpt_trienode->m_poolId,
                                                   1, KAPS_JR1_IPT_POOL);
                }
            }

        }
    }

    return KAPS_JR1_OK;
}


NlmErrNum_t
kaps_jr1_trie_wb_cr_restore_rpt_entry(
    kaps_jr1_lpm_trie * trie,
    struct kaps_jr1_wb_rpt_entry_info * rpt_entry_info,
    uint32_t * nv_offset)
{
    kaps_jr1_trie_global *trie_g = NULL;
    struct kaps_jr1_wb_ipt_entry_info ipt_node;
    kaps_jr1_trie_node *rpt_trienode;
    NlmErrNum_t err_num = NLMERR_OK;
    uint32_t num_entries = 0, num_bytes_read, iter;

    trie_g = trie->m_trie_global;

    rpt_trienode = trie->m_roots_trienode_p;

    rpt_trienode->m_isRptNode = 1;

    rpt_trienode->m_rptId = rpt_entry_info->rpt_id;
    rpt_trienode->m_rpt_uuid = rpt_entry_info->rpt_uuid;
    if (rpt_trienode->m_rpt_uuid != NLM_INVALID_RPT_UUID)
        trie->m_trie_global->m_rpt_uuid_table[rpt_entry_info->rpt_uuid] = 0;

    rpt_trienode->m_poolId = rpt_entry_info->pool_id;
    rpt_trienode->m_rptParent_p = rpt_trienode;

    
    err_num = kaps_jr1_pool_mgr_wb_add_rpt_entry_to_pool(trie->m_trie_global->poolMgr, rpt_trienode);
    if (err_num != NLMERR_OK)
        return err_num;


    
    if (trie_g->fibtblmgr->m_devMgr_p->nv_ptr)
    {
        return kaps_jr1_trie_cr_restore_rpt_entry(trie, rpt_trienode, rpt_entry_info);
    }

    
    num_entries = rpt_entry_info->num_ipt_entries;

    if (num_entries)
    {
        for (iter = 0; iter < num_entries; iter++)
        {
            if (0 !=
                trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & ipt_node,
                                        sizeof(struct kaps_jr1_wb_ipt_entry_info), *nv_offset))
                return NLMERR_FAIL;
            num_bytes_read = sizeof(struct kaps_jr1_wb_ipt_entry_info);
            *nv_offset += num_bytes_read;

            err_num = kaps_jr1_trie_wb_restore_ipt_entry(trie, rpt_trienode, &ipt_node, nv_offset);
            if (err_num != NLMERR_OK)
                return err_num;
        }

        kaps_jr1_trie_incr_num_ipt_entries_in_subtrie(rpt_trienode, num_entries);
        kaps_jr1_pool_mgr_incr_num_entries(trie->m_trie_global->poolMgr, rpt_trienode->m_poolId,
                                       num_entries, KAPS_JR1_IPT_POOL);
    }

    return err_num;
}

NlmErrNum_t
kaps_jr1_trie_wb_restore_pools(
    kaps_jr1_trie_global * trie_g,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    uint32_t num_active_pools;

    
    if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & num_active_pools,
                             sizeof(num_active_pools), *wb_fun->nv_offset))
        return NLMERR_FAIL;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_active_pools);

    trie_g->poolMgr->m_numActiveIptPools = num_active_pools;

    if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & num_active_pools,
                             sizeof(num_active_pools), *wb_fun->nv_offset))
        return NLMERR_FAIL;
    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_active_pools);

    trie_g->poolMgr->m_numActiveAptPools = num_active_pools;

    return kaps_jr1_pool_mgr_wb_restore_pools(trie_g->poolMgr, wb_fun->read_fn, wb_fun->handle, wb_fun->nv_offset);
}

NlmErrNum_t
kaps_jr1_trie_cr_restore_pools(
    kaps_jr1_lpm_trie * trie,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    kaps_jr1_trie_global *trie_g = trie->m_trie_global;
    struct kaps_jr1_device *device = trie_g->fibtblmgr->m_devMgr_p;
    NlmErrNum_t err_num = NLMERR_OK;
    NlmReasonCode o_reason;

    if (device->type == KAPS_JR1_DEVICE)
    {
        
        kaps_jr1_trie_node *root_node = trie->m_roots_trienode_p;

        if (!root_node->m_isRptNode)
        {
            root_node->m_isRptNode = 1;
            root_node->m_rptParent_p = root_node;

            err_num = kaps_jr1_trie_pvt_get_new_pool(trie_g->poolMgr, root_node, 1, 0, 1, &o_reason);

            if (err_num != NLMERR_OK)
            {
                return NLMERR_FAIL;
            }
        }
        return NLMERR_OK;
    }
    else
    {
        uint32_t *num_active_pools;

        num_active_pools = (uint32_t *) wb_fun->nv_ptr;
        wb_fun->nv_ptr += sizeof(*num_active_pools);

        
        trie_g->poolMgr->m_numActiveIptPools = *num_active_pools;
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*num_active_pools);

        num_active_pools = (uint32_t *) wb_fun->nv_ptr;
        wb_fun->nv_ptr += sizeof(*num_active_pools);

        trie_g->poolMgr->m_numActiveAptPools = *num_active_pools;
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*num_active_pools);

        kaps_jr1_pool_mgr_cr_restore_pools(trie_g->poolMgr, wb_fun);
    }

    return NLMERR_OK;
}

kaps_jr1_status
kaps_jr1_trie_wb_restore(
    kaps_jr1_lpm_trie * trie,
    uint32_t * nv_offset)
{
    kaps_jr1_trie_global *trie_g;
    struct kaps_jr1_wb_rpt_entry_info rpt_info;
    NlmErrNum_t err_num;
    uint32_t num_rpt_entries, iter;
    kaps_jr1_fib_tbl *fibTbl = trie->m_tbl_ptr;
    uint16_t no_of_regions;
    struct region region_info[MAX_UDA_REGIONS];
    NlmReasonCode reason = NLMRSC_REASON_OK;
    struct kaps_jr1_db *db = fibTbl->m_db;
    struct kaps_jr1_ad_db *ad_db;
    struct kaps_jr1_ix_mgr *ix_mgr;
    uint32_t i;
    struct kaps_jr1_device *device;

    trie_g = trie->m_trie_global;
    device = trie_g->fibtblmgr->m_devMgr_p;

    if (0 != trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_g->m_are_reserved_160b_abs_initialized,
                                     sizeof(trie_g->m_are_reserved_160b_abs_initialized), *nv_offset))
    {
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    }

    *nv_offset += sizeof(trie_g->m_are_reserved_160b_abs_initialized);

    if (0 !=
        trie_g->wb_fun->read_fn(trie_g->wb_fun->handle,
                                (uint8_t *) & trie->m_lsn_settings_p->m_numAllocatedBricksForAllLsns,
                                sizeof(trie->m_lsn_settings_p->m_numAllocatedBricksForAllLsns), *nv_offset))
    {
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    }

    *nv_offset += sizeof(trie->m_lsn_settings_p->m_numAllocatedBricksForAllLsns);

    if (0 != trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & trie->m_lsn_settings_p->m_maxLpuPerLsn,
                                     sizeof(trie->m_lsn_settings_p->m_maxLpuPerLsn), *nv_offset))
    {
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    }

    *nv_offset += sizeof(trie->m_lsn_settings_p->m_maxLpuPerLsn);

    
    if (0 !=
        trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_g->m_mlpmemmgr[0]->no_of_regions,
                                sizeof(no_of_regions), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(no_of_regions);

    
    if (0 !=
        trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & trie_g->m_mlpmemmgr[0]->region_info,
                                sizeof(region_info), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(region_info);

    no_of_regions = trie_g->m_mlpmemmgr[0]->no_of_regions;
    kaps_jr1_memcpy(region_info, trie_g->m_mlpmemmgr[0]->region_info, sizeof(region_info));

    if (0 !=
        trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & num_rpt_entries, sizeof(num_rpt_entries),
                                *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;

    *nv_offset += sizeof(num_rpt_entries);

    ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;

    while (ad_db) 
    {
        for (i = 0; i < device->hw_res->num_daisy_chains; ++i) 
        {
            ix_mgr = ad_db->mgr[0][i];

            KAPS_JR1_STRY(kaps_jr1_ix_mgr_wb_restore_ix_translate
                (ix_mgr, trie_g->wb_fun->read_fn, trie_g->wb_fun->handle, nv_offset));
        }

        ad_db = ad_db->next;
    }

    for (iter = 0; iter < num_rpt_entries; iter++)
    {
        if (0 !=
            trie_g->wb_fun->read_fn(trie_g->wb_fun->handle, (uint8_t *) & rpt_info,
                                    sizeof(struct kaps_jr1_wb_rpt_entry_info), *nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
        *nv_offset += sizeof(struct kaps_jr1_wb_rpt_entry_info);

        err_num = kaps_jr1_trie_wb_cr_restore_rpt_entry(trie, &rpt_info, nv_offset);
        if (err_num != NLMERR_OK)
            return err_num;
    }


    
    err_num = kaps_jr1_trie_wb_cr_post_process(NULL, trie->m_roots_trienode_p, NULL, 0, &reason);
    if (err_num != NLMERR_OK)
        return err_num;

    if (trie_g->fibtblmgr->m_devMgr_p->type != KAPS_JR1_DEVICE)
    {
        kaps_jr1_trie_wb_verify(trie->m_roots_trienode_p);
    }

    if (!trie_g->m_mlpmemmgr[0]->chunk_map[0])
    {
        trie_g->m_mlpmemmgr[0]->no_of_regions = no_of_regions;
        kaps_jr1_memcpy(trie_g->m_mlpmemmgr[0]->region_info, region_info, sizeof(region_info));
        KAPS_JR1_STRY(kaps_jr1_uda_mgr_wb_restore_regions
                  (trie_g->m_mlpmemmgr[0], fibTbl->m_fibTblMgr_p->m_devMgr_p->alloc, no_of_regions, region_info));
    }

    return KAPS_JR1_OK;
}

void
kaps_jr1_trie_wb_compute_iit_lmpsofar(
    kaps_jr1_trie_node * node_p,
    kaps_jr1_trie_node * ancestorIptNode_p)
{
    kaps_jr1_pfx_bundle *lmp_in_prev_lsn;

    if (ancestorIptNode_p)
    {
        lmp_in_prev_lsn = kaps_jr1_lsn_mc_locate_lpm(ancestorIptNode_p->m_lsn_p, node_p->m_lp_prefix_p->m_data,
                                                 node_p->m_lp_prefix_p->m_nPfxSize, NULL, NULL, NULL);
        
        if (lmp_in_prev_lsn && !lmp_in_prev_lsn->m_isPfxCopy)
        {
            node_p->m_iitLmpsofarPfx_p = lmp_in_prev_lsn;
            node_p->m_iitLmpsofarPfx_p->m_isLmpsofarPfx = 1;
        }
        else
        {
            
            node_p->m_iitLmpsofarPfx_p = ancestorIptNode_p->m_iitLmpsofarPfx_p;
        }
    }
}

void
kaps_jr1_trie_wb_restore_iit_lmpsofar(
    kaps_jr1_trie_node * node_p,
    kaps_jr1_trie_node * ancestorIptNode_p)
{
    if (!node_p)
        return;

    node_p->m_iitLmpsofarPfx_p = NULL;

    if (node_p->m_node_type == NLMNSTRIENODE_LP)
    {
        kaps_jr1_trie_wb_compute_iit_lmpsofar(node_p, ancestorIptNode_p);

        ancestorIptNode_p = node_p;
    }

    if (node_p->m_child_p[0])
        kaps_jr1_trie_wb_restore_iit_lmpsofar(node_p->m_child_p[0], ancestorIptNode_p);

    if (node_p->m_child_p[1])
        kaps_jr1_trie_wb_restore_iit_lmpsofar(node_p->m_child_p[1], ancestorIptNode_p);
}

kaps_jr1_status
kaps_jr1_trie_cr_restore(
    kaps_jr1_lpm_trie * trie,
    uint32_t * nv_offset)
{
    struct kaps_jr1_wb_rpt_entry_info rpt_info;
    NlmErrNum_t err_num;
    NlmReasonCode reason = NLMRSC_REASON_OK;

    kaps_jr1_memset(&rpt_info, 0, sizeof(struct kaps_jr1_wb_rpt_entry_info));
    if (trie->m_lsn_settings_p->m_device->type == KAPS_JR1_DEVICE)
    {
        rpt_info.rpt_id = trie->m_roots_trienode_p->m_rptId;

        err_num = kaps_jr1_trie_wb_cr_restore_rpt_entry(trie, &rpt_info, nv_offset);
        if (err_num != NLMERR_OK)
            return err_num;
    }

    
    err_num = kaps_jr1_trie_wb_cr_post_process(NULL, trie->m_roots_trienode_p, NULL, 1, &reason);
    if (err_num != NLMERR_OK)
        return err_num;

    return KAPS_JR1_OK;
}



void
kaps_jr1_trie_wb_traverse_and_print_trienodes(
    kaps_jr1_trie_node * trienode,
    FILE * fp)
{
    
    kaps_jr1_fprintf(fp, "%u %u ", trienode->m_rptId, trienode->m_numAptEntriesInSubtrie);
    kaps_jr1_fprintf(fp, "%u %u ", trienode->m_child_id, trienode->m_isRptNode);

    
    if (trienode->m_rptParent_p)
        kaps_jr1_fprintf(fp, "1 ");

    
    if (trienode->m_child_p[0])
        kaps_jr1_fprintf(fp, "1 ");
    if (trienode->m_child_p[1])
        kaps_jr1_fprintf(fp, "1 ");

    

    

    if (trienode->m_child_p[0])
        kaps_jr1_trie_wb_traverse_and_print_trienodes(trienode->m_child_p[0], fp);

    if (trienode->m_child_p[1])
        kaps_jr1_trie_wb_traverse_and_print_trienodes(trienode->m_child_p[1], fp);
}

kaps_jr1_status
kaps_jr1_trie_wb_traverse_and_print_trie(
    kaps_jr1_lpm_trie * trie,
    FILE * fp)
{
    

    kaps_jr1_trie_wb_traverse_and_print_trienodes(trie->m_roots_trienode_p, fp);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_trie_wb_traverse_and_verify_trienodes(
    kaps_jr1_trie_node * trienode,
    FILE * fp)
{
    uint32_t uint1, uint2, uint3, uint4, uint5;
    uint16_t ushort1;
    uint8_t uchar1;

    (void) kaps_jr1_fscanf(fp, "%u %hd %c %u %u ", &uint1, &ushort1, &uchar1, &uint2, &uint3);

    KAPS_JR1_WB_IS_EQUAL(trienode->m_node_type, uint1, "node_type mismatch ");
    KAPS_JR1_WB_IS_EQUAL(trienode->m_depth, ushort1, "depth mismatch ");
    KAPS_JR1_WB_IS_EQUAL(trienode->m_numIptEntriesInSubtrie, uint2, "numIPTEntries mismatch ");
    KAPS_JR1_WB_IS_EQUAL(trienode->m_poolId, uint1, "poolId mismatch ");

    (void) kaps_jr1_fscanf(fp, "%u %u %u ", &uint1, &uint2, &uint3);

    KAPS_JR1_WB_IS_EQUAL(trienode->m_rptId, uint1, "rptId mismatch ");
    KAPS_JR1_WB_IS_EQUAL(trienode->m_numAptEntriesInSubtrie, uint2, "numAPTEntries mismatch ");

    (void) kaps_jr1_fscanf(fp, "%u %u %u %u %u ", &uint1, &uint2, &uint3, &uint4, &uint5);

    KAPS_JR1_WB_IS_EQUAL(trienode->m_child_id, uint1, "child_id mismatch ");
    KAPS_JR1_WB_IS_EQUAL(trienode->m_isRptNode, uint4, "isRptNode mismatch ");

    if (trienode->m_rptParent_p)
    {
        (void) kaps_jr1_fscanf(fp, "%u ", &uint1);

        KAPS_JR1_WB_IS_EQUAL(1, uint1, "rptParent missing ");
    }

    if (trienode->m_child_p[0])
    {
        (void) kaps_jr1_fscanf(fp, "%u ", &uint1);

        KAPS_JR1_WB_IS_EQUAL(1, uint1, "left child missing ");
    }

    if (trienode->m_child_p[1])
    {
        (void) kaps_jr1_fscanf(fp, "%u ", &uint1);

        KAPS_JR1_WB_IS_EQUAL(1, uint1, "right child missing ");
    }

    if (trienode->m_child_p[0])
        kaps_jr1_trie_wb_traverse_and_print_trienodes(trienode->m_child_p[0], fp);

    if (trienode->m_child_p[1])
        kaps_jr1_trie_wb_traverse_and_print_trienodes(trienode->m_child_p[1], fp);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_trie_wb_verify_trie(
    kaps_jr1_lpm_trie * trie,
    FILE * fp)
{
    kaps_jr1_trie_wb_traverse_and_verify_trienodes(trie->m_roots_trienode_p, fp);

    return KAPS_JR1_OK;
}

void
kaps_jr1_trie_wb_verify(
    kaps_jr1_trie_node * trie_node)
{
    uint32_t old_num_ipt_entries, old_num_apt_entries;

    if (trie_node == NULL)
        return;

    if (trie_node->m_isRptNode)
    {
        old_num_ipt_entries = trie_node->m_numIptEntriesInSubtrie;
        old_num_apt_entries = trie_node->m_numAptEntriesInSubtrie;

        trie_node->m_trie_p->m_trie_global->m_rptNodeForCalcIptApt = trie_node;
        trie_node->m_trie_p->m_trie_global->useTrigLenForCalcIptApt = 0;
        kaps_jr1_trie_pvt_calc_num_ipt_and_apt(trie_node);

        kaps_jr1_assert(old_num_ipt_entries == trie_node->m_numIptEntriesInSubtrie,
                    "Mismatch in number of IPT entries in the subtrie \n");

        kaps_jr1_assert(old_num_apt_entries == trie_node->m_numAptEntriesInSubtrie,
                    "Mismatch in number of APT entries in the subtrie \n");
    }

    kaps_jr1_trie_wb_verify(trie_node->m_child_p[0]);
    kaps_jr1_trie_wb_verify(trie_node->m_child_p[1]);
}
