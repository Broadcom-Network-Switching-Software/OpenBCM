

#include "kaps_jr1_fibmgr.h"
#include "kaps_jr1_fib_trie.h"
#include "kaps_jr1_fib_hw.h"
#include "kaps_jr1_dbllinklist.h"
#include "kaps_jr1_algo_common.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_ad_internal.h"

static void kaps_jr1_tbl_dtor(
    kaps_jr1_fib_tbl * self);


static void
kaps_jr1_tbl_list_destroy_node(
    kaps_jr1_dbl_link_list * node,
    void *arg)
{
    kaps_jr1_fib_tbl_list *fibTblList = (kaps_jr1_fib_tbl_list *) node;
    kaps_jr1_nlm_allocator *alloc = (kaps_jr1_nlm_allocator *) arg;
    kaps_jr1_tbl_dtor(fibTblList->m_fibTbl_p);

    kaps_jr1_nlm_allocator_free(alloc, fibTblList->m_fibTbl_p);

    kaps_jr1_nlm_allocator_free(alloc, fibTblList);
}

static void
kaps_jr1_fib_tbl_list_destroy_node(
    kaps_jr1_dbl_link_list * node,
    void *arg)
{
    kaps_jr1_nlm_allocator *alloc = (kaps_jr1_nlm_allocator *) arg;
    kaps_jr1_fib_tbl_list *fibDependentTblsList = (kaps_jr1_fib_tbl_list *) node;

    kaps_jr1_nlm_allocator_free(alloc, fibDependentTblsList);
}


static void
kaps_jr1_tbl_list_destroy(
    kaps_jr1_fib_tbl_list * head,
    kaps_jr1_nlm_allocator * alloc_p,
    kaps_jr1_dbl_link_list_destroy_node_t destroyNode)
{
    kaps_jr1_dbl_link_list_destroy((kaps_jr1_dbl_link_list *) head, destroyNode, alloc_p);
}

static void
kaps_jr1_tbl_dtor(
    kaps_jr1_fib_tbl * self)
{
    if (!self)
        return;
    if (self->m_dependentTbls_p)
    {
        kaps_jr1_tbl_list_destroy((kaps_jr1_fib_tbl_list *) self->m_dependentTbls_p, self->m_alloc_p,
                              kaps_jr1_fib_tbl_list_destroy_node);
    }
}


static kaps_jr1_fib_tbl_list *
kaps_jr1_tbl_list_init(
    kaps_jr1_nlm_allocator * alloc_p)
{
    kaps_jr1_fib_tbl_list *head = kaps_jr1_nlm_allocator_calloc(alloc_p, 1,
                                                        sizeof(kaps_jr1_fib_tbl_list));

    if (!head)
        return NULL;

    kaps_jr1_dbl_link_list_init((kaps_jr1_dbl_link_list *) head);

    return head;
}


static kaps_jr1_fib_tbl_list *
kaps_jr1_tbl_list_insert(
    kaps_jr1_fib_tbl_list * head,
    kaps_jr1_fib_tbl * fibTbl_p,
    kaps_jr1_nlm_allocator * alloc_p)
{
    kaps_jr1_fib_tbl_list *node = kaps_jr1_nlm_allocator_calloc(alloc_p, 1,
                                                        sizeof(kaps_jr1_fib_tbl_list));

    if (!node)
        return NULL;

    node->m_fibTbl_p = fibTbl_p;

    kaps_jr1_dbl_link_list_insert((kaps_jr1_dbl_link_list *) head, (kaps_jr1_dbl_link_list *) node);

    return node;
}


static void
kaps_jr1_tbl_list_remove(
    kaps_jr1_fib_tbl_list * node,
    kaps_jr1_nlm_allocator * alloc_p,
    kaps_jr1_dbl_link_list_destroy_node_t destroyNode)
{
    kaps_jr1_dbl_link_list_remove((kaps_jr1_dbl_link_list *) node, destroyNode, alloc_p);
}

static kaps_jr1_fib_tbl *
kaps_jr1_tbl_search(
    kaps_jr1_fib_tbl_list * head,
    uint8_t tblId,
    uint8_t core_id)
{
    kaps_jr1_fib_tbl_list *node;

    
    node = head->m_next_p;

    while (node != head)
    {
        if (node->m_fibTbl_p->m_tblId == tblId && node->m_fibTbl_p->m_coreId == core_id)
            return node->m_fibTbl_p;
        node = node->m_next_p;
    }
    
    return NULL;
}

void
kaps_jr1_set_cur_fib_tbl_in_all_dev(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    kaps_jr1_fib_tbl * fibTbl)
{
    struct kaps_jr1_device *main_device;
    struct kaps_jr1_device *cur_device;

    main_device = fibTblMgr->m_devMgr_p->main_dev;
    if (!main_device)
        main_device = fibTblMgr->m_devMgr_p;

    if (main_device->type == KAPS_JR1_DEVICE)
    {
        fibTblMgr->m_curFibTbl = fibTbl;
        return;
    }

    fibTblMgr->m_curFibTbl = fibTbl;

    cur_device = main_device;
    while (cur_device != NULL)
    {
        cur_device->lpm_mgr->fib_tbl_mgr->m_curFibTbl = fibTbl;
        cur_device = cur_device->next_dev;
    }
}

static void
kaps_jr1_fib_tbl_mgr_ctor(
    kaps_jr1_fib_tbl_mgr * self,
    kaps_jr1_nlm_allocator * alloc_p,
    struct kaps_jr1_device *devMgr_p,
    kaps_jr1_fib_prefix_index_changed_app_cb_t indexChangedAppCb,
    struct kaps_jr1_lpm_mgr *lpm_mgr,
    NlmReasonCode * o_reason)
{
    self->m_alloc_p = alloc_p;
    self->m_devMgr_p = devMgr_p;
    self->m_fibTblCount = 0;
    self->m_fibTblList_p = NULL;
    self->m_IsConfigLocked = 0;
    self->m_indexChangeCallBackFn = indexChangedAppCb;
    self->m_lpm_mgr = lpm_mgr;

    
    if ((self->m_fibTblList_p = kaps_jr1_tbl_list_init(alloc_p)) == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return;
    }

    return;
}

static void
kaps_jr1_fib_tbl_mgr_dtor(
    kaps_jr1_fib_tbl_mgr * self)
{
    
    if (self->m_fibTblList_p)
    {
        kaps_jr1_tbl_list_destroy((kaps_jr1_fib_tbl_list *) self->m_fibTblList_p, self->m_alloc_p, kaps_jr1_tbl_list_destroy_node);
    }
}

kaps_jr1_fib_tbl *
kaps_jr1_tbl_ctor(
    kaps_jr1_fib_tbl * self,
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t fibTblId,
    kaps_jr1_fib_tbl_index_range * fibTblIndexRange,
    uint16_t fibMaxPrefixLength,
    NlmReasonCode * o_reason)
{
    self->m_fibTblMgr_p = fibTblMgr;
    self->m_alloc_p = fibTblMgr->m_alloc_p;
    self->m_numPrefixes = 0;
    self->m_tblIndexRange.m_indexLowValue = fibTblIndexRange->m_indexLowValue;
    self->m_startBit = 159;

    if (fibTblIndexRange->m_indexHighValue == 0 && fibTblIndexRange->m_indexLowValue == 0)
        self->m_tblIndexRange.m_indexHighValue = KAPS_JR1_FIB_MAX_INDEX_RANGE;
    else if (fibTblMgr->m_devMgr_p->type == KAPS_JR1_DEVICE)
        self->m_tblIndexRange.m_indexHighValue = self->m_db->common_info->capacity;
    else
        self->m_tblIndexRange.m_indexHighValue = fibTblIndexRange->m_indexHighValue;

    self->m_width = fibMaxPrefixLength;

    self->m_tblId = fibTblId;

    
    if ((self->m_dependentTbls_p = kaps_jr1_tbl_list_init(fibTblMgr->m_alloc_p)) == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    return self;
}

void
kaps_jr1_ftm_destroy_internal(
    kaps_jr1_fib_tbl_mgr * fibTblMgr)
{
    if (!fibTblMgr)
        return;

    
    if (fibTblMgr->m_trieGlobal)
        kaps_jr1_trie_module_destroy(fibTblMgr->m_trieGlobal);

    kaps_jr1_nlm_allocator_free(fibTblMgr->m_alloc_p, fibTblMgr->m_devWriteCallBackFns);
}

NlmErrNum_t
kaps_jr1_ftm_init_internal(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr_callback_fn_ptrs *callBackFnPtrs;
    
    if ((callBackFnPtrs =
         kaps_jr1_nlm_allocator_calloc(fibTblMgr->m_alloc_p, 1, sizeof(kaps_jr1_fib_tbl_mgr_callback_fn_ptrs))) == NULL)
    {
        kaps_jr1_ftm_destroy_internal(fibTblMgr);
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    if (fibTblMgr->m_devMgr_p->type == KAPS_JR1_DEVICE)
    {
        

        if ((fibTblMgr->m_trieGlobal =
             kaps_jr1_trie_two_level_fib_module_init(fibTblMgr->m_alloc_p, fibTblMgr, o_reason)) == NULL)
        {
            
            kaps_jr1_ftm_destroy_internal(fibTblMgr);
            return NLMERR_FAIL;
        }


        callBackFnPtrs->m_writeABData = kaps_jr1_write_ab_data;
        callBackFnPtrs->m_deleteABData = kaps_jr1_delete_ab_data;
    }

    fibTblMgr->m_devWriteCallBackFns = callBackFnPtrs;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_create_table_internal(
    kaps_jr1_fib_tbl * fibTbl,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr = fibTbl->m_fibTblMgr_p;

    
    if (fibTbl->m_width > KAPS_JR1_LPM_KEY_MAX_WIDTH_1)
    {
        *o_reason = NLMRSC_INVALID_FIB_MAX_PREFIX_LENGTH;
        return NLMERR_FAIL;
    }

    
    if ((fibTbl->m_trie = kaps_jr1_trie_create(fibTblMgr->m_trieGlobal, fibTbl, o_reason)) == NULL)
    {
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

kaps_jr1_fib_tbl_mgr *
kaps_jr1_ftm_init(
    kaps_jr1_nlm_allocator * alloc_p,
    struct kaps_jr1_device * devMgr_p,
    void *client_p,
    kaps_jr1_fib_prefix_index_changed_app_cb_t indexChangedAppCb,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *self = NULL;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    
    if (!alloc_p)
    {
        *o_reason = NLMRSC_INVALID_MEMALLOC_PTR;
        return NULL;
    }
    if (!devMgr_p)
    {
        *o_reason = NLMRSC_INVALID_DEVMGR_PTR;
        return NULL;
    }

    
    if ((self = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_fib_tbl_mgr))) == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    
    kaps_jr1_fib_tbl_mgr_ctor(self, alloc_p, devMgr_p, indexChangedAppCb, client_p, o_reason);
    if (*o_reason != NLMRSC_REASON_OK)
    {
        kaps_jr1_nlm_allocator_free(alloc_p, self);
        return NULL;
    }

    self->m_numOfDevices = 1;

    if (NLMERR_OK != kaps_jr1_ftm_init_internal(self, o_reason))
    {
        
        kaps_jr1_fib_tbl_mgr_dtor(self);
        kaps_jr1_nlm_allocator_free(alloc_p, self);
        return NULL;
    }

    self->m_arenaSizeInBytes = KAPS_JR1_DEFAULT_ARENA_SIZE_IN_BYTES;

    
    if (!self->m_alloc_p->m_arena_info)
    {
        if (kaps_jr1_arena_init(self->m_alloc_p, self->m_arenaSizeInBytes, o_reason) != NLMERR_OK)
        {
            kaps_jr1_ftm_destroy(self, o_reason);
            self = NULL;
            *o_reason = NLMRSC_LOW_MEMORY;
        }
    }

    return self;
}

NlmErrNum_t
kaps_jr1_ftm_destroy(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_list *fibTblList_p;
    kaps_jr1_fib_tbl_list *fibTblListNextNode_p;
    kaps_jr1_nlm_allocator *alloc_p;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTblMgr)
    {
        *o_reason = NLMRSC_INVALID_FIB_MGR;
        return NLMERR_NULL_PTR;
    }

    
    fibTblList_p = ((kaps_jr1_fib_tbl_list *) fibTblMgr->m_fibTblList_p)->m_next_p;
    while (fibTblList_p->m_fibTbl_p)
    {
        fibTblListNextNode_p = fibTblList_p->m_next_p;
        kaps_jr1_ftm_destroy_table(fibTblList_p->m_fibTbl_p, o_reason);
        fibTblList_p = fibTblListNextNode_p;
    }

    
    if (fibTblMgr->m_core1trieGlobal)
    {
        kaps_jr1_trie_module_destroy(fibTblMgr->m_core0trieGlobal);
        kaps_jr1_trie_module_destroy(fibTblMgr->m_core1trieGlobal);
    }
    else
    {
        if (fibTblMgr->m_trieGlobal)
            kaps_jr1_trie_module_destroy(fibTblMgr->m_trieGlobal);
    }

    kaps_jr1_nlm_allocator_free(fibTblMgr->m_alloc_p, fibTblMgr->m_devWriteCallBackFns);

    kaps_jr1_fib_tbl_mgr_dtor(fibTblMgr);
    alloc_p = fibTblMgr->m_alloc_p;

    kaps_jr1_nlm_allocator_free(alloc_p, fibTblMgr);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_destroy_table(
    kaps_jr1_fib_tbl * fibTbl,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    kaps_jr1_fib_tbl_list *node;
    kaps_jr1_fib_tbl_list *dependentTblNode;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    
    if (fibTbl->m_trie)
        kaps_jr1_trie_destroy(fibTbl->m_trie);

    
    if (fibTbl->m_dependentTbls_p)
    {
        node = ((kaps_jr1_fib_tbl_list *) fibTbl->m_dependentTbls_p)->m_next_p;
        while (node->m_fibTbl_p)
        {
            dependentTblNode = ((kaps_jr1_fib_tbl_list *) node->m_fibTbl_p->m_dependentTbls_p)->m_next_p;
            while (dependentTblNode->m_fibTbl_p)
            {
                if (dependentTblNode->m_fibTbl_p == fibTbl)
                    break;

                
                dependentTblNode = dependentTblNode->m_next_p;
            }
            if (dependentTblNode->m_fibTbl_p)
                kaps_jr1_tbl_list_remove(dependentTblNode, fibTbl->m_alloc_p, kaps_jr1_fib_tbl_list_destroy_node);

            node = node->m_next_p;
        }
    }

    
    node = ((kaps_jr1_fib_tbl_list *) fibTblMgr->m_fibTblList_p)->m_next_p;
    while (node->m_fibTbl_p)
    {
        if (node->m_fibTbl_p == fibTbl)
            break;

        
        node = node->m_next_p;
    }
    
    if (node->m_fibTbl_p)
        kaps_jr1_tbl_list_remove(node, fibTbl->m_alloc_p, kaps_jr1_tbl_list_destroy_node);
    else
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_FAIL;
    }
    fibTblMgr->m_fibTblCount--;

    return NLMERR_OK;
}

kaps_jr1_fib_tbl *
kaps_jr1_ftm_create_table(
    kaps_jr1_fib_tbl_mgr * fibTblMgr,
    uint8_t fibTblId,           
    kaps_jr1_fib_tbl_index_range * fibTblIndexRange,
    uint16_t fibMaxPrefixLength,
    struct kaps_jr1_db * db,
    uint32_t is_cascaded,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl *self;
    kaps_jr1_nlm_allocator *alloc_p;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTblMgr)
    {
        *o_reason = NLMRSC_INVALID_FIB_MGR;
        return NULL;
    }

    if (1 == fibTblMgr->m_IsConfigLocked)
    {
        *o_reason = NLMRSC_CONFIGURATION_LOCKED;
        return NULL;
    }

    
    if (NULL != kaps_jr1_tbl_search(fibTblMgr->m_fibTblList_p, fibTblId, is_cascaded))
    {
        *o_reason = NLMRSC_DUPLICATE_FIB_TBL_ID;
        return NULL;
    }

    if (fibTblIndexRange == NULL)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL_INDEX_RANGE;
        return NULL;
    }

    if (fibTblIndexRange->m_indexHighValue < fibTblIndexRange->m_indexLowValue
        || fibTblIndexRange->m_indexHighValue > KAPS_JR1_FIB_MAX_INDEX_RANGE)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL_INDEX_RANGE;
        return NULL;
    }

    alloc_p = fibTblMgr->m_alloc_p;
    if ((self = kaps_jr1_nlm_allocator_calloc(alloc_p, 1, sizeof(kaps_jr1_fib_tbl))) == NULL)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    self->m_db = db;

    self->m_cascaded_fibtbl = is_cascaded;


    
    kaps_jr1_set_cur_fib_tbl_in_all_dev(fibTblMgr, self);

    
    if ((self = kaps_jr1_tbl_ctor(self, fibTblMgr, fibTblId, fibTblIndexRange, fibMaxPrefixLength, o_reason)) == NULL)
    {
        return NULL;
    }

    if (NLMERR_OK != kaps_jr1_ftm_create_table_internal(self, o_reason))
    {
        
        kaps_jr1_tbl_dtor(self);
        kaps_jr1_nlm_allocator_free(alloc_p, self);
        return NULL;
    }

    
    if (NULL == kaps_jr1_tbl_list_insert((kaps_jr1_fib_tbl_list *) fibTblMgr->m_fibTblList_p, self, alloc_p))
    {
        kaps_jr1_tbl_dtor(self);
        kaps_jr1_nlm_allocator_free(alloc_p, self);
        *o_reason = NLMRSC_LOW_MEMORY;
        return NULL;
    }

    self->m_trie->m_maxAllowedIptEntriesForRpt = 1000;

    fibTblMgr->m_fibTblCount++;

    self->m_coreId = is_cascaded;

    return self;
}

NlmErrNum_t
kaps_jr1_ftm_compact_ix_space(struct kaps_jr1_db *db, NlmReasonCode * o_reason)
{
    struct kaps_jr1_ad_db *ad_db;
    int daisy_chain_id, thresold_percent_for_ad_wastage = 4;
    kaps_jr1_status status = KAPS_JR1_OK;

    ad_db = (struct kaps_jr1_ad_db *)db->common_info->ad_info.ad;

    
    if (ad_db->next)
        thresold_percent_for_ad_wastage = 9;

    while (ad_db)
    {
        uint32_t total_size = 0, ad_free_space = 0;

        if (ad_db->db_info.hw_res.ad_res->ad_type != KAPS_JR1_AD_TYPE_INDEX)
            continue;

        for (daisy_chain_id = 0; daisy_chain_id < db->device->hw_res->num_daisy_chains; ++daisy_chain_id)
        {
            if (ad_db->mgr[db->device->core_id][daisy_chain_id])
                total_size += ad_db->mgr[db->device->core_id][daisy_chain_id]->size;
        }

        ad_free_space = ((total_size - ad_db->ad_list.count) / (total_size * 1.0)) * 100;

        if (ad_free_space > thresold_percent_for_ad_wastage)
        {
            int was_compaction_done;

            for (daisy_chain_id = 0; daisy_chain_id < db->device->hw_res->num_daisy_chains; ++daisy_chain_id)
            {
                if (ad_db->mgr[db->device->core_id][daisy_chain_id])
                {
                    status = kaps_jr1_ix_mgr_compact(ad_db->mgr[db->device->core_id][daisy_chain_id],
                            0, &was_compaction_done);
                    if (status != KAPS_JR1_OK)
                    {
                        *o_reason = NLMRSC_INTERNAL_ERROR;
                        return NLMERR_FAIL;
                    }
                }
            }
        }
        ad_db = ad_db->next;
    }
    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_add_prefix_internal(
    kaps_jr1_fib_tbl * fibTbl,      
    struct kaps_jr1_lpm_entry * entry,      
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;
    struct kaps_jr1_device *device = NULL;
    uint32_t i = 0, num_bytes;
    struct kaps_jr1_db *db; 

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }

    db = fibTbl->m_db;

    device = fibTbl->m_fibTblMgr_p->m_devMgr_p;

    if (!device)
    {
        *o_reason = NLMRSC_INVALID_DEV_PTR;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_PREFIX;
        return NLMERR_NULL_PTR;
    }
    if (entry->pfx_bundle->m_nPfxSize > fibTbl->m_width)
    {
        *o_reason = NLMRSC_INVALID_PREFIX;
        return NLMERR_FAIL;
    }

    if (device->main_dev)
        device = device->main_dev;

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_jr1_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    
    errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);

    
    if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
    {
        struct kaps_jr1_uda_mgr *mgr = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0];
        kaps_jr1_status status = KAPS_JR1_OUT_OF_UDA;

        status = kaps_jr1_uda_mgr_compact_all_regions(mgr);
        if (status == KAPS_JR1_OK)
        {
            fibTbl->m_trie->m_tbl_ptr->m_fibStats.numUDACompactionCalls++;
        }

        if (status == KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_REASON_OK;

            errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);
        }
    }

    
    if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
    {
        struct kaps_jr1_uda_mgr *mgr = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0];
        kaps_jr1_status status = KAPS_JR1_OUT_OF_UDA;

        status = kaps_jr1_uda_mgr_dynamic_uda_alloc_n_lpu(mgr, fibTbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn);
        if (status == KAPS_JR1_OK)
        {
            fibTbl->m_trie->m_tbl_ptr->m_fibStats.numUDADynamicAllocCalls++;
        }

        if (status == KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_REASON_OK;

            errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);
        }
    }

    
    if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
    {
        struct kaps_jr1_uda_mgr *mgr = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0];
        kaps_jr1_status status = KAPS_JR1_OUT_OF_UDA;

        status = kaps_jr1_resource_expand_uda_mgr_regions(mgr);
        if (status == KAPS_JR1_OK)
        {
            fibTbl->m_trie->m_tbl_ptr->m_fibStats.numUDAExpansionCalls++;
        }

        if (status == KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_REASON_OK;

            errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);
        }
    }

    
    if (*o_reason == NLMRSC_UDA_ALLOC_FAILED && db->common_info->enable_dynamic_allocation)
    {
        struct kaps_jr1_uda_mgr *mgr = fibTbl->m_trie->m_trie_global->m_mlpmemmgr[0];
        kaps_jr1_status status = KAPS_JR1_OUT_OF_UDA;
        int32_t num_udms_required;

        num_udms_required = fibTbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn - 1;
        while (num_udms_required)
        {
            status = kaps_jr1_uda_mgr_dynamic_uda_alloc_n_lpu(mgr, num_udms_required);
            if (status == KAPS_JR1_OK)
                break;
            num_udms_required--;
        }

        if (status == KAPS_JR1_OK)
        {
            fibTbl->m_trie->m_tbl_ptr->m_fibStats.numUDADynamicAllocCalls++;
        }

        if (status == KAPS_JR1_OK)
        {
            *o_reason = NLMRSC_REASON_OK;

            errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);

            if (*o_reason == NLMRSC_UDA_ALLOC_FAILED)
            {
                status = kaps_jr1_uda_mgr_release_last_allocated_regions(mgr);
                kaps_jr1_sassert(status == KAPS_JR1_OK);
            }
        }
    }

    if (*o_reason == NLMRSC_IDX_RANGE_FULL)
    {

        errNum = kaps_jr1_ftm_compact_ix_space(db, o_reason);
        if (errNum == NLMERR_OK)
        {
            *o_reason = NLMRSC_REASON_OK;
            errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT, entry, o_reason);
        }
    }

    kaps_jr1_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    fibTbl->m_numPrefixes++;

    num_bytes = (entry->pfx_bundle->m_nPfxSize + 7) / 8;
    for (i = 0; i < num_bytes; ++i)
    {
        device->hw_res->running_sum += entry->pfx_bundle->m_data[i];
    }

    return errNum;
}

NlmErrNum_t
kaps_jr1_ftm_add_prefix(
    struct kaps_jr1_lpm_db * lpm_db,        
    struct kaps_jr1_lpm_entry * entry,      
    NlmReasonCode * o_reason)
{
    NlmErrNum_t err_num = NLMERR_OK;

    err_num = kaps_jr1_ftm_add_prefix_internal(lpm_db->fib_tbl, entry, o_reason);

    return err_num;
}

NlmErrNum_t
kaps_jr1_ftm_wb_add_prefix(
    kaps_jr1_fib_tbl * fibTbl,      
    struct kaps_jr1_lpm_entry * entry,      
    uint32_t index,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_PREFIX;
        return NLMERR_NULL_PTR;
    }
    if (entry->pfx_bundle->m_nPfxSize > fibTbl->m_width)
    {
        *o_reason = NLMRSC_INVALID_PREFIX;
        return NLMERR_FAIL;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_jr1_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    entry->pfx_bundle->m_nIndex = index;

    
    errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_INSERT_WITH_INDEX, entry, o_reason);
    kaps_jr1_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    if (!entry->pfx_bundle->m_isPfxCopy)
        fibTbl->m_numPrefixes++;

    return errNum;
}

NlmErrNum_t
kaps_jr1_ftm_delete_prefix(
    kaps_jr1_fib_tbl * fibTbl,      
    struct kaps_jr1_lpm_entry * entry,      
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;
    uint32_t i, num_bytes;
    struct kaps_jr1_device *device;

    if (!o_reason)
        o_reason = &dummyVar;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }

    *o_reason = NLMRSC_REASON_OK;

    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_jr1_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    
    errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_DELETE, entry, o_reason);
    kaps_jr1_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    fibTbl->m_numPrefixes--;

    device = fibTbl->m_fibTblMgr_p->m_devMgr_p;
    if (device->main_dev)
        device = device->main_dev;

    num_bytes = (entry->pfx_bundle->m_nPfxSize + 7) / 8;
    for (i = 0; i < num_bytes; ++i)
    {
        device->hw_res->running_sum -= entry->pfx_bundle->m_data[i];
    }

    return errNum;
}

NlmErrNum_t
kaps_jr1_ftm_get_index(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry * entry,
    int32_t * nindices,
    int32_t ** indices,
    NlmReasonCode * o_reason)
{
    int32_t *ret_indices = NULL;
    int32_t num_indices;
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    NlmReasonCode dummyVar;
    struct kaps_jr1_allocator *alloc;
    uint32_t mainPfxIndex;
    kaps_jr1_pfx_bundle *mainPfxBundle = NULL;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    if (entry->pfx_bundle->m_nPfxSize > fibTbl->m_width)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_FAIL;
    }

    alloc = fibTbl->m_fibTblMgr_p->m_devMgr_p->alloc;
    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    num_indices = 1;
    if (entry->pfx_bundle->m_status == 0)
        mainPfxIndex = KAPS_JR1_FIB_INVALID_INDEX;
    else
        mainPfxIndex = kaps_jr1_trie_locate_exact(fibTbl->m_trie, entry->pfx_bundle->m_data,
                                              entry->pfx_bundle->m_nPfxSize, &mainPfxBundle, o_reason);

    ret_indices = alloc->xcalloc(alloc->cookie, 1, (num_indices * sizeof(int32_t)));
    if (!ret_indices)
    {
        *o_reason = NLMRSC_LOW_MEMORY;
        return NLMERR_FAIL;
    }

    ret_indices[0] = mainPfxIndex;

    *indices = ret_indices;
    *nindices = num_indices;

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_locate_lpm(
    kaps_jr1_fib_tbl * fibTbl,
    uint8_t * key,
    struct kaps_jr1_lpm_entry ** entry_pp,
    uint32_t * index,
    uint32_t * length,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    kaps_jr1_pfx_bundle *pfxBundle;
    NlmReasonCode dummyVar;
    struct kaps_jr1_device *device;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!key)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    device = fibTbl->m_fibTblMgr_p->m_devMgr_p;
    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    pfxBundle = kaps_jr1_trie_locate_lpm(fibTbl->m_trie, key, fibTbl->m_width, index, o_reason);

    if (pfxBundle)
    {
        *entry_pp = pfxBundle->m_backPtr;
        *length = pfxBundle->m_nPfxSize;

        if (pfxBundle->m_isPfxCopy)
            *length = pfxBundle->m_backPtr->pfx_bundle->m_nPfxSize;

        if (pfxBundle->m_backPtr->ad_handle && device->type != KAPS_JR1_DEVICE)
        {
            struct kaps_jr1_ad_db *ad_db;
            enum kaps_jr1_ad_type ad_type;

            KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, pfxBundle->m_backPtr->ad_handle, ad_db);
            ad_type = kaps_jr1_resource_get_ad_type(&ad_db->db_info);

            if (ad_db->user_width_1 != 0)
            {
                if (ad_type == KAPS_JR1_AD_TYPE_INPLACE)
                {
                    *index = pfxBundle->m_backPtr->ad_handle->value[0] << 16;
                    *index |= pfxBundle->m_backPtr->ad_handle->value[1] << 8;
                    *index |= pfxBundle->m_backPtr->ad_handle->value[2];
                }
            }
        }
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_update_ad(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry * entry,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_jr1_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_UPDATE_AD, entry, o_reason);

    kaps_jr1_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    return errNum;
}

NlmErrNum_t
kaps_jr1_ftm_update_ad_address(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry * entry,
    NlmReasonCode * o_reason)
{
    kaps_jr1_fib_tbl_mgr *fibTblMgr;
    NlmErrNum_t errNum = NLMERR_OK;
    NlmReasonCode dummyVar;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_NULL_PTR;
    }
    if (!entry)
    {
        *o_reason = NLMRSC_INVALID_INPUT;
        return NLMERR_NULL_PTR;
    }

    fibTblMgr = fibTbl->m_fibTblMgr_p;
    fibTblMgr->m_curFibTbl = fibTbl;

    errNum = kaps_jr1_arena_activate_arena(fibTbl->m_alloc_p, o_reason);
    if (errNum != NLMERR_OK)
        return errNum;

    errNum = kaps_jr1_trie_submit_rqt(fibTbl->m_trie, NLM_FIB_PREFIX_UPDATE_AD_ADDRESS, entry, o_reason);

    kaps_jr1_arena_deactivate_arena(fibTbl->m_alloc_p);

    if (errNum != NLMERR_OK)
        return errNum;

    return errNum;
}

NlmErrNum_t
kaps_jr1_ftm_get_resource_usage(
    kaps_jr1_fib_tbl * fibTbl,
    kaps_jr1_fib_resource_usage * rxcUsage,
    NlmReasonCode * o_reason)
{
    NlmReasonCode dummyVar;
    uint32_t log_info = 0;

    if (!o_reason)
        o_reason = &dummyVar;

    *o_reason = NLMRSC_REASON_OK;

    if (!fibTbl)
    {
        *o_reason = NLMRSC_INVALID_FIB_TBL;
        return NLMERR_FAIL;
    }

    if (!rxcUsage)
    {
        *o_reason = NLMRSC_INVALID_POINTER;
        return NLMERR_FAIL;
    }

    kaps_jr1_trie_get_resource_usage(fibTbl, rxcUsage, log_info);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_get_prefix_handle(
    kaps_jr1_fib_tbl * fibTbl,
    uint8_t * data,
    uint32_t length,
    struct kaps_jr1_lpm_entry ** entry)
{
    struct pfx_hash_table *hash_table = fibTbl->m_trie->m_hashtable_p;
    struct kaps_jr1_entry **ht_slot = NULL;
    kaps_jr1_status status;

    status = kaps_jr1_pfx_hash_table_locate(hash_table, data, length, &ht_slot);

    if (status != KAPS_JR1_OK)
    {
        *entry = NULL;
        return NLMERR_FAIL;
    }

    if (!ht_slot)
    {
        *entry = NULL;
        return NLMERR_OK;
    }

    *entry = *((struct kaps_jr1_lpm_entry **) ht_slot);
    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_update_hash_table(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry * entry,
    uint32_t is_del)
{
    struct pfx_hash_table *hash_table = fibTbl->m_trie->m_hashtable_p;
    kaps_jr1_status status;
    struct kaps_jr1_entry **ht_slot;

    if (!entry)
        return NLMERR_FAIL;

    status =
        kaps_jr1_pfx_hash_table_locate(fibTbl->m_trie->m_hashtable_p, entry->pfx_bundle->m_data, entry->pfx_bundle->m_nPfxSize,
                              &ht_slot);
    if (status != KAPS_JR1_OK)
    {
        return NLMERR_FAIL;
    }

    if (is_del)
    {
        if (!ht_slot)
            return NLMERR_FAIL;
        status = kaps_jr1_pfx_hash_table_delete(hash_table, ht_slot);
    }
    else
    {
        struct kaps_jr1_entry *ht_entry = (struct kaps_jr1_entry *) entry;
        if (ht_slot)
            return NLMERR_FAIL;

        status = kaps_jr1_pfx_hash_table_insert(hash_table, ht_entry);
    }

    if (status != KAPS_JR1_OK)
    {
        return NLMERR_FAIL;
    }

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_get_prefix_location(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_lpm_entry * lpm_entry,
    uint32_t * abs_udc,
    uint32_t * abs_row,
    NlmReasonCode * o_reason)
{
    *o_reason = NLMRSC_REASON_OK;

    kaps_jr1_trie_get_prefix_location(fibTbl->m_trie, lpm_entry, abs_udc, abs_row);

    return NLMERR_OK;
}

NlmErrNum_t
kaps_jr1_ftm_process_hit_bits(
    kaps_jr1_fib_tbl * fibTbl,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_jr1_device *device = fibTbl->m_fibTblMgr_p->m_devMgr_p;

    *o_reason = NLMRSC_REASON_OK;

    if (device->type == KAPS_JR1_DEVICE)
    {
        errNum = kaps_jr1_trie_process_hit_bits_iit_lmpsofar(fibTbl->m_trie);
    }

    return errNum;
}

NlmErrNum_t
kaps_jr1_ftm_get_algo_hit_bit_value(
    kaps_jr1_fib_tbl * fibTbl,
    struct kaps_jr1_entry * entry,
    uint8_t clear_on_read,
    uint32_t * bit_value,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum = NLMERR_OK;
    struct kaps_jr1_device *device = fibTbl->m_fibTblMgr_p->m_devMgr_p;

    if (device->type == KAPS_JR1_DEVICE)
    {
        errNum =
            kaps_jr1_trie_get_algo_hit_bit_value(fibTbl->m_trie, (struct kaps_jr1_lpm_entry *) entry, clear_on_read, bit_value);
    }

    return errNum;
}

void
kaps_jr1_print_rpb_stats_html(
    FILE * f,
    struct kaps_jr1_device *dev,
    struct kaps_jr1_lpm_db *db,
    struct NlmFibStats *fib_stats)
{


    kaps_jr1_fprintf(f, "<h3>RPB Stats</h3>\n");
    kaps_jr1_fprintf(f, "<table style= \"float: left; margin-right:50px;\">\n");
    kaps_jr1_fprintf(f, "<tbody>\n");
    kaps_jr1_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_jr1_fprintf(f, "  <th>Total</th>\n");
    kaps_jr1_fprintf(f, "  <th>Used</th>\n");
    kaps_jr1_fprintf(f, "  <th>Avg Width (Bytes)</th>\n");
    kaps_jr1_fprintf(f, "</tr>\n");

    if (fib_stats->numRPTEntries)
    {
        kaps_jr1_fprintf(f, "<tr>\n");
        kaps_jr1_fprintf(f, "  <td>%d</td>\n", fib_stats->numRPTEntries);
        kaps_jr1_fprintf(f, "  <td>%.02f</td>\n",
                     (float) ((1.0 * fib_stats->totalRptBytesLoppedOff) / fib_stats->numRPTEntries));
        kaps_jr1_fprintf(f, "</tr>\n");
    }

    kaps_jr1_fprintf(f, "</tbody>\n");
    kaps_jr1_fprintf(f, "</table>\n");

    kaps_jr1_fprintf(f, "<br>\n");

#if 0
    kaps_jr1_fprintf(f, "<table>\n");
    kaps_jr1_fprintf(f, "<tbody>\n");
    kaps_jr1_fprintf(f, "<tr class=\"broadcom2\"> \n");
    kaps_jr1_fprintf(f, "  <th>Length</th>\n");
    kaps_jr1_fprintf(f, "  <th>Num Entries</th>\n");
    kaps_jr1_fprintf(f, "  <th>Avg Width</th>\n");
    kaps_jr1_fprintf(f, "</tr>\n");

    for (i = 0; i <= KAPS_JR1_LPM_KEY_MAX_WIDTH_8; ++i)
    {
        if (fib_stats.rpt_lopoff_info[i])
        {
            kaps_jr1_printf("                Length = %d bytes, Num entries = %d\n", i, fib_stats.rpt_lopoff_info[i]);
        }
    }

    kaps_jr1_fprintf(f, "</tbody>\n");
    kaps_jr1_fprintf(f, "</table>\n");

    kaps_jr1_fprintf(f, "<br>\n");
#endif
}


void
kaps_jr1_print_detailed_stats_html(
    FILE * f,
    struct kaps_jr1_device *dev,
    struct kaps_jr1_db *db)
{
    struct kaps_jr1_db_stats stats;
    struct NlmFibStats *fib_stats_ptr;
    struct kaps_jr1_lpm_db *lpm_db;

    (void) kaps_jr1_db_stats(db, &stats);

    if (stats.num_entries == 0)
        return;

    lpm_db = (struct kaps_jr1_lpm_db *) db;

    fib_stats_ptr =
        lpm_db->db_info.device->alloc->xmalloc(lpm_db->db_info.device->alloc->cookie, sizeof(struct NlmFibStats));
    if (fib_stats_ptr == NULL)
        return;

    (void) kaps_jr1_lpm_db_get_fib_stats(lpm_db, fib_stats_ptr);
#if 0
    kaps_jr1_fprintf(f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n");
    kaps_jr1_fprintf(f, "\"http://www.w3.org/TR/html4/strict.dtd\">\n");
    kaps_jr1_fprintf(f, "<HTML lang=\"en\">\n");
    kaps_jr1_fprintf(f, "<HEAD>\n");
    kaps_jr1_fprintf(f, "<STYLE type=\"text/css\">\n");
    kaps_jr1_fprintf(f, "td,th {padding-bottom:4pt;padding-top:4pt;padding-left:4pt;padding-right:4pt}\n");
    kaps_jr1_fprintf(f, "table {border-collapse:collapse}\n");
    kaps_jr1_fprintf(f, "td {text-align:center;font-family:Courier New;font-weight:bold;font-size:100%%}\n");
    kaps_jr1_fprintf(f, "table,td,th {border:2px solid #adafb2}\n");
    kaps_jr1_fprintf(f, "tr.broadcom1 {background:#e31837;color:#ffffff}\n");
    kaps_jr1_fprintf(f, "tr.broadcom2 {background:#005568;color:#ffffff}\n");
    kaps_jr1_fprintf(f, "tr.broadcom3 {background:#ffd457;color:#000000}\n");
    kaps_jr1_fprintf(f, "body {margin:20px 20px 20px 20px}\n");
    kaps_jr1_fprintf(f, "</STYLE>\n");
    kaps_jr1_fprintf(f, "</HEAD>\n");

    kaps_jr1_fprintf(f, "<BODY>\n");
#endif

    kaps_jr1_fprintf(f, "<h2>RPB %d Capacity %d </h2>\n", db->rpb_id, stats.num_entries);

    

#if 0
    kaps_jr1_fprintf(f, "</BODY>\n");
    kaps_jr1_fprintf(f, "</HTML>\n");
#endif
    lpm_db->db_info.device->alloc->xfree(lpm_db->db_info.device->alloc->cookie, fib_stats_ptr);
}

NlmErrNum_t
kaps_jr1_ftm_calc_stats(
    kaps_jr1_fib_tbl * fibTbl)
{
    kaps_jr1_trie_global *trieGlobal_p = fibTbl->m_fibTblMgr_p->m_trieGlobal;
    struct NlmFibStats *fibStats = &fibTbl->m_fibStats;
    struct uda_mgr_stats uda_stats;
    uint32_t i;
    struct kaps_jr1_ad_db *ad_db;

    for (i = 0; i < KAPS_JR1_HW_MAX_NUM_LPU_GRAN; ++i)
    {
        fibStats->numBricksForEachGran[i] = 0;
        fibStats->numEmptyBricksForEachGran[i] = 0;
        fibStats->numPfxForEachGran[i] = 0;
        fibStats->numHolesForEachGran[i] = 0;
    }

    for (i = 0; i < KAPS_JR1_HW_MAX_LPUS_PER_LPM_DB; ++i)
    {
        fibStats->numLsnForEachSize[i] = 0;
        fibStats->numHolesForEachLsnSize[i] = 0;
    }

    for (i = 0; i <= KAPS_JR1_LPM_KEY_MAX_WIDTH_8; ++i)
    {
        fibStats->rpt_lopoff_info[i] = 0;
    }

    for (i = 0; i < KAPS_JR1_MAX_NUM_POOLS; ++i)
    {
        fibStats->isABUsed[i] = 0;
        fibStats->widthOfAB[i] = 0;
        fibStats->numTriggersInAB[i] = 0;
        fibStats->numColsInAB[i] = 0;
    }

    fibStats->rxcTotalNumABAssigned = 0;
    fibStats->numTotalIptAB = 0;
    fibStats->numTotalAptAB = 0;

    fibStats->numTotal80bIptAB = 0;
    fibStats->numTotal160bIptAB = 0;
    fibStats->numTotal80bSmallIptAB = 0;
    fibStats->numTotal160bSmallIptAB = 0;
    fibStats->numDupIptAB = 0;

    fibStats->numTotal80bAptAB = 0;
    fibStats->numTotal160bAptAB = 0;
    fibStats->numTotal80bSmallAptAB = 0;
    fibStats->numTotal160bSmallAptAB = 0;
    fibStats->numDupAptAB = 0;

    fibStats->num40bTriggers = 0;
    fibStats->num80bTriggers = 0;
    fibStats->num160bTriggers = 0;
    fibStats->avgTrigLen = 0;

    fibStats->totalNumHolesInLsns = 0;
    fibStats->numIPTEntries = 0;
    fibStats->numRPTEntries = 0;
    fibStats->numUsedBricks = 0;
    fibStats->numUsedButEmptyBricks = 0;
    fibStats->numPartiallyOccupiedBricks = 0;
    fibStats->numTrieNodes = 0;
    fibStats->numLmpsofarPfx = 0;
    fibStats->numAptLongPfx = 0;
    fibStats->numAPTLmpsofarEntries = 0;
    fibStats->numUsedUitBanks = 0;
    fibStats->totalRptBytesLoppedOff = 0;
    fibStats->numADWrites = 0;

    fibStats->maxNumRptSplitsInANode = 0;
    fibStats->depthofNodeWithMaxRptSplits = 0;
    fibStats->idOfNodeWithMaxRptSplits = 0;

    fibStats->numHolesInNonLastBricks = 0;
    fibStats->numHolesInLastBrick = 0;

    fibStats->numNonLastBricksThatAreEmpty = 0;
    fibStats->numLastBricksThatAreEmpty = 0;

    fibStats->numLsnsWithPrefixMatchingDepth = 0;
    fibStats->numLsnsThatCanbePushedDeeper = 0;

    fibStats->numUsedAds2Slots = 0;
    fibStats->numFreeAds2Slots = 0;

    kaps_jr1_memset(&fibStats->kaps_jr1_small_bb_stats, 0, sizeof(fibStats->kaps_jr1_small_bb_stats));

    fibStats->maxLsnWidth = fibTbl->m_trie->m_lsn_settings_p->m_maxLpuPerLsn;
    fibStats->initialLsnWidth = fibTbl->m_trie->m_lsn_settings_p->m_initialMaxLpuPerLsn;

    kaps_jr1_trie_calc_trie_lsn_stats(fibTbl->m_trie->m_roots_trienode_p, 0, fibStats);

    if (fibStats->numIPTEntries)
        fibStats->avgTrigLen = fibStats->avgTrigLen / fibStats->numIPTEntries;


    kaps_jr1_pool_mgr_get_pool_stats(fibTbl->m_trie->m_trie_global->poolMgr, fibStats);

    kaps_jr1_uda_mgr_calc_stats(trieGlobal_p->m_mlpmemmgr[0], &uda_stats);
    
    fibStats->numFreeBricks = uda_stats.total_num_free_lpu_bricks;
    fibStats->numRegions = trieGlobal_p->m_mlpmemmgr[0]->no_of_regions;

    ad_db = (struct kaps_jr1_ad_db *) fibTbl->m_db->common_info->ad_info.ad;
    while (ad_db)
    {
        fibStats->numADWrites += ad_db->num_writes;
        ad_db = ad_db->next;
    }

    return NLMERR_OK;
}

kaps_jr1_status
kaps_jr1_ftm_wb_process_rpt(
    kaps_jr1_fib_tbl * fib_tbl)
{
    
    fib_tbl->m_fibTblMgr_p->m_curFibTbl = fib_tbl;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ftm_wb_save(
    kaps_jr1_fib_tbl * fib_tbl,
    uint32_t * nv_offset)
{
    if (KAPS_JR1_OK != kaps_jr1_trie_wb_save(fib_tbl->m_trie, nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ftm_wb_restore(
    kaps_jr1_fib_tbl * fib_tbl,
    uint32_t * nv_offset)
{
    return kaps_jr1_trie_wb_restore(fib_tbl->m_trie, nv_offset);
}

kaps_jr1_status
kaps_jr1_ftm_cr_restore(
    kaps_jr1_fib_tbl * fib_tbl,
    struct kaps_jr1_wb_cb_functions * cb_fun)
{
    return kaps_jr1_trie_cr_restore(fib_tbl->m_trie, cb_fun->nv_offset);
}
