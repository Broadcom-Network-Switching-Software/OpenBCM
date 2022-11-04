
#include <kaps_jr1_fib_cmn_pfxbundle.h>
#include <kaps_jr1_fib_trienode.h>
#include <kaps_jr1_fib_trie.h>
#include "kaps_jr1_fib_lsnmc_hw.h"
#include "kaps_jr1_utility.h"
#include "kaps_jr1_handle.h"



static void kaps_jr1_trie_node_ctor(
    kaps_jr1_trie_node * self,
    kaps_jr1_trie_node * parent,
    kaps_jr1_lpm_trie * trie);


static void kaps_jr1_trie_node_traverse_dfs_for_remove(
    kaps_jr1_trie_node * node,
    kaps_jr1_trie_node_traverse_dfs_visitor_for_remove_t func,
    NlmNsTrieNode__TraverseArgs * args);


static kaps_jr1_trie_node *kaps_jr1_trie_node_insert(
    kaps_jr1_trie_node * self,
    kaps_jr1_lpm_trie * trie,
    uint8_t child);


static void
kaps_jr1_trie_node_ctor(
    kaps_jr1_trie_node * self,
    kaps_jr1_trie_node * parent,
    kaps_jr1_lpm_trie * trie_p)
{
    static uint32_t trienodeId = 1;

    kaps_jr1_assert(self != NULL, "Trienode Pointer is NULL");

    kaps_jr1_memset(self, 0, sizeof(kaps_jr1_trie_node));
    self->m_parent_p = parent;
    self->m_trie_p = trie_p;
    self->m_node_type = NLMNSTRIENODE_REGULAR;
    if (parent)
    {
        self->m_poolId = parent->m_poolId;
        self->m_rptId = parent->m_rptId;
    }
    self->m_rpt_uuid = NLM_INVALID_RPT_UUID;

    self->m_trienodeId = trienodeId++;

    if (trie_p->m_lsn_settings_p)
        self->m_lsn_p = kaps_jr1_lsn_mc_create(trie_p->m_lsn_settings_p, trie_p, parent ? parent->m_depth + 1 : 0);

    if (parent)
    {
        self->m_rptParent_p = parent->m_rptParent_p;
        self->m_poolId = parent->m_poolId;
    }

}


void
kaps_jr1_trie_node_set_pfx_bundle(
    kaps_jr1_trie_node * self,
    kaps_jr1_pfx_bundle * pfx)
{
    self->m_lp_prefix_p = pfx;
    if (self->m_lsn_p)
        self->m_lsn_p->m_pParentHandle = pfx;
    kaps_jr1_memcpy(KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(pfx), &self, sizeof(void *));

}

static int32_t
kaps_jr1_trie_node_destroyHelper(
    kaps_jr1_trie_node * self,
    NlmNsTrieNode__TraverseArgs * args)
{
    kaps_jr1_nlm_allocator *alloc = (kaps_jr1_nlm_allocator *) args->arg1;
    int32_t delPfxes = (int32_t) (uintptr_t) args->arg2;
    NlmNsDownStreamRptNodes *curNode, *nextNode;

    if (delPfxes && self->m_lsn_p)
        kaps_jr1_lsn_mc_free_prefixes_safe(self->m_lsn_p);

    
    if (self == self->m_trie_p->m_cache_trienode_p)
        self->m_trie_p->m_cache_trienode_p = NULL;

    if (self->m_lsn_p)
        kaps_jr1_lsn_mc_destroy(self->m_lsn_p);

    kaps_jr1_pfx_bundle_destroy(self->m_lp_prefix_p, alloc);

    if (self->m_rpt_prefix_p)
        kaps_jr1_pfx_bundle_destroy(self->m_rpt_prefix_p, alloc);

    if (self->m_rptAptLmpsofarPfx)
        kaps_jr1_pfx_bundle_destroy(self->m_rptAptLmpsofarPfx, alloc);

    curNode = self->m_downstreamRptNodes;
    while (curNode)
    {
        nextNode = curNode->next;
        kaps_jr1_nlm_allocator_free(alloc, curNode);
        curNode = nextNode;
    }

    kaps_jr1_nlm_allocator_free(alloc, self);

    return 1;
}

static kaps_jr1_trie_node *
kaps_jr1_trie_node_create(
    kaps_jr1_nlm_allocator * alloc,
    kaps_jr1_trie_node * parent,
    kaps_jr1_lpm_trie * trie)
{
    kaps_jr1_trie_node *self;

    kaps_jr1_assert(alloc, "No Allocator pointer");

    self = (kaps_jr1_trie_node *) kaps_jr1_nlm_allocator_calloc(alloc, 1, sizeof(kaps_jr1_trie_node));
    kaps_jr1_trie_node_ctor(self, parent, trie);

    return self;
}


void
kaps_jr1_trie_node_destroy(
    kaps_jr1_trie_node * self,
    kaps_jr1_nlm_allocator * alloc,
    int32_t delPfxes)
{
    if (self)
    {
        NlmNsTrieNode__TraverseArgs args;
        args.arg1 = alloc;
        args.arg2 = KAPS_JR1_CAST_U32_TO_PTR(delPfxes);
        kaps_jr1_trie_node_traverse_dfs_for_remove(self, kaps_jr1_trie_node_destroyHelper, &args);
    }
}


kaps_jr1_trie_node *
kaps_jr1_trie_node_create_root_node(
    kaps_jr1_lpm_trie * trie,
    uint32_t child)
{
    kaps_jr1_trie_node *newnode_p;
    kaps_jr1_pfx_bundle *bundle;

    newnode_p = kaps_jr1_trie_node_create(trie->m_trie_global->m_alloc_p, NULL, trie);

    newnode_p->m_child_id = child;

    bundle =
        kaps_jr1_pfx_bundle_create_from_string(trie->m_trie_global->m_alloc_p, NULL, 0, KAPS_JR1_LSN_NEW_INDEX, sizeof(void *),
                                           0);

    kaps_jr1_trie_node_set_pfx_bundle(newnode_p, bundle);

    newnode_p->m_depth = 0;
    if (newnode_p->m_lsn_p)
    {
        newnode_p->m_lsn_p->m_nDepth = 0;
        newnode_p->m_lsn_p->m_nLopoff = 0;
    }

    return newnode_p;
}

kaps_jr1_trie_node *
kaps_jr1_trie_node_add_child(
    kaps_jr1_trie_node * self,
    kaps_jr1_lpm_trie * trie,
    uint8_t child)
{
    kaps_jr1_trie_node *newnode_p;
    kaps_jr1_pfx_bundle *bundle;
    uint8_t tmpbuf[320 / KAPS_JR1_BITS_IN_BYTE + 1];
    uint8_t *tmpptr;

    newnode_p = kaps_jr1_trie_node_create(trie->m_trie_global->m_alloc_p, self, trie);

    self->m_child_p[child] = newnode_p;

    newnode_p->m_child_id = child;
    newnode_p->m_depth = (uint16_t) (self->m_depth + 1);
    if (newnode_p->m_lsn_p)
        newnode_p->m_lsn_p->m_nDepth = newnode_p->m_depth;

    if ((self->m_depth & 7) == 0)
    {
        uint32_t ptrLen = KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(self->m_depth);

        kaps_jr1_memcpy(tmpbuf, KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(self->m_lp_prefix_p), ptrLen);
        tmpptr = tmpbuf;
        tmpptr[ptrLen] = 0;
    }
    else
        tmpptr = KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(self->m_lp_prefix_p);

    bundle =
        kaps_jr1_pfx_bundle_create_from_string(trie->m_trie_global->m_alloc_p, tmpptr, newnode_p->m_depth,
                                           KAPS_JR1_LSN_NEW_INDEX, sizeof(void *), 0);

    kaps_jr1_pfx_bundle_set_bit(bundle, self->m_depth, child);

    kaps_jr1_trie_node_set_pfx_bundle(newnode_p, bundle);

    kaps_jr1_assert(trie == newnode_p->m_trie_p, "kaps_jr1_trie_node_add_child");

    if (trie->m_maxDepth < newnode_p->m_depth)
        trie->m_maxDepth = newnode_p->m_depth;

    return newnode_p;
}

void
kaps_jr1_trie_node_traverse_dfs_preorder(
    kaps_jr1_trie_node * node,
    kaps_jr1_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args)
{
    if (node)
    {
        int32_t ret;
        kaps_jr1_lpm_trie *trie = node->m_trie_p;
        kaps_jr1_trie_node **ary = kaps_jr1_trie_get_stack_space(trie);
        uint32_t n = 0;
        kaps_jr1_trie_node *left, *right;

        while (node)
        {
again:
            ret = func(node, args);
            if (!ret)
            {
                if (!n)
                    break;
                node = ary[--n];
                goto again;
            }
            left = node->m_child_p[NLMNSTRIE_LCHILD];
            right = node->m_child_p[NLMNSTRIE_RCHILD];
            if (right)
            {
                kaps_jr1_assert(n < KAPS_JR1_LPM_KEY_MAX_WIDTH_1, "kaps_jr1_trie_node_traverse_dfs_preorder");
                ary[n++] = right;
            }

            node = left ? left : (n ? ary[--n] : 0);
        }

        kaps_jr1_trie_return_stack_space(trie, ary);
    }
}



        

void
kaps_jr1_trie_node_traverse_dfs_postorder(
    kaps_jr1_trie_node * start_node,
    kaps_jr1_trie_node_traverse_dfs_visitor_t func,
    NlmNsTrieNode__TraverseArgs * args)
{
    if (start_node)
    {
        kaps_jr1_recursion_status ret;
        kaps_jr1_lpm_trie *trie = start_node->m_trie_p;
        kaps_jr1_trie_node **ary = kaps_jr1_trie_get_stack_space(trie);
        uint32_t n = 0;
        kaps_jr1_trie_node *left, *right, *cur_node;
        uint32_t *processing_status_stack = trie->m_node_processing_stack;
        uint32_t *processing_status;

        kaps_jr1_assert(args != 0, "No args ");

        ary[n] = start_node;
        processing_status_stack[n] = KAPS_JR1_UNTRAVERSED_NODE;
        n++;

        while (n)
        {
            cur_node = ary[n-1];
            if (!cur_node)
            {
                --n;
                continue;
            }
                
            processing_status = &processing_status_stack[n-1];

            if (cur_node->m_isRptNode) 
            {
                kaps_jr1_trie_node *rptParentOfNodes = args->arg1;
                if (cur_node != rptParentOfNodes) 
                {
                    --n;
                    continue;
                }
                
            }

            if (*processing_status == KAPS_JR1_UNTRAVERSED_NODE)
            {
                left = cur_node->m_child_p[NLMNSTRIE_LCHILD];
                *processing_status = KAPS_JR1_LEFT_CHILD_EXPANDED;

                ary[n] = left;
                processing_status_stack[n] = KAPS_JR1_UNTRAVERSED_NODE;
                ++n;
                continue;

            }

            if (*processing_status == KAPS_JR1_LEFT_CHILD_EXPANDED)
            {
                right = cur_node->m_child_p[NLMNSTRIE_RCHILD];
                *processing_status = KAPS_JR1_RIGHT_CHILD_EXPANDED;

                ary[n] = right;
                processing_status_stack[n] = KAPS_JR1_UNTRAVERSED_NODE;
                ++n;
                continue;

            }

            if (*processing_status == KAPS_JR1_RIGHT_CHILD_EXPANDED) 
            {
                ret = func(cur_node, args);
                n--;

                if (ret == KAPS_JR1_RECURSION_ERROR)
                    break;
            }

           
        }

        kaps_jr1_trie_return_stack_space(trie, ary);
    }
}



static void
kaps_jr1_trie_node_traverse_dfs_for_remove(
    kaps_jr1_trie_node * node,
    kaps_jr1_trie_node_traverse_dfs_visitor_for_remove_t func,
    NlmNsTrieNode__TraverseArgs * args)
{
    if (node)
    {
        kaps_jr1_lpm_trie *trie = node->m_trie_p;
        kaps_jr1_trie_node **ary = kaps_jr1_trie_get_stack_space(trie);
        uint32_t n = 0;
        kaps_jr1_trie_node *left, *right;

        kaps_jr1_assert(args != 0, "No args ");

        while (node)
        {
            kaps_jr1_trie_node *cur = node;
            left = node->m_child_p[NLMNSTRIE_LCHILD];
            right = node->m_child_p[NLMNSTRIE_RCHILD];
            if (right)
            {
                kaps_jr1_assert(n < 320, "Depth of node is invalid ");
                ary[n++] = right;
            }

            node = left ? left : (n ? ary[--n] : 0);
            func(cur, args);
        }

        kaps_jr1_trie_return_stack_space(trie, ary);
    }
}

int32_t
kaps_jr1_trie_node_pvt_is_lp_node(
    kaps_jr1_trie_node * self)
{
    if (self)
    {
        switch (self->m_node_type)
        {
            case NLMNSTRIENODE_REGULAR:
                break;
            default:
                return 1;
        }
    }
    return 0;
}


kaps_jr1_lsn_mc *
kaps_jr1_trie_node_pvt_create_lsn(
    kaps_jr1_trie_node * self)
{
    kaps_jr1_lsn_mc *lsn_p;
    lsn_p = kaps_jr1_lsn_mc_create(self->m_trie_p->m_lsn_settings_p, self->m_trie_p, self->m_depth);
    lsn_p->m_pParentHandle = self->m_lp_prefix_p;
    return lsn_p;
}


kaps_jr1_lsn_mc *
kaps_jr1_trie_node_pvt_recreate_lsn(
    kaps_jr1_trie_node * self)
{
    

    return (kaps_jr1_trie_node_pvt_create_lsn(self));
}


NlmErrNum_t
kaps_jr1_trie_node_update_iit(
    kaps_jr1_trie_node * self,
    NlmReasonCode * o_reason)
{
    NlmErrNum_t errNum;

    errNum = kaps_jr1_lsn_mc_update_iit(self->m_lsn_p, o_reason);

    return errNum;
}


static kaps_jr1_trie_node *
kaps_jr1_trie_node_insert(
    kaps_jr1_trie_node * self,
    kaps_jr1_lpm_trie * trie,
    uint8_t child)
{
    kaps_jr1_trie_node *newnode_p;
    kaps_jr1_trie_node *nextnode_p;

    nextnode_p = self->m_child_p[child];

    newnode_p = kaps_jr1_trie_node_add_child(self, trie, child);

    newnode_p->m_child_p[child] = nextnode_p;
    if (nextnode_p)
        nextnode_p->m_parent_p = newnode_p;

    return newnode_p;
}

kaps_jr1_trie_node *
kaps_jr1_trie_node_insertPathFromPrefix(
    kaps_jr1_trie_node * self,
    kaps_jr1_prefix * prefix,
    uint32_t startBitPos,
    uint32_t endBitPos)
{
    uint8_t bit;
    uint32_t curBitPos = startBitPos;

    while (curBitPos <= endBitPos)
    {
        bit = KAPS_JR1_PREFIX_GET_BIT(prefix, curBitPos);
        if (self->m_child_p[bit] == NULL)
            kaps_jr1_trie_node_insert(self, self->m_trie_p, bit);
        self = self->m_child_p[bit];
        curBitPos++;
    }

    return self;
}

kaps_jr1_trie_node *
kaps_jr1_trie_node_insertPathFromPrefixBundle(
    kaps_jr1_trie_node * self,
    kaps_jr1_pfx_bundle * pfxBundle)
{
    uint8_t bit;
    uint32_t curBitPos = 0;

    while (curBitPos < pfxBundle->m_nPfxSize)
    {
        bit = KAPS_JR1_PREFIX_PVT_GET_BIT(pfxBundle->m_data, pfxBundle->m_nSize, curBitPos);

        if (self->m_child_p[bit] == NULL)
            kaps_jr1_trie_node_insert(self, self->m_trie_p, bit);

        self = self->m_child_p[bit];
        curBitPos++;
    }

    return self;
}


void
kaps_jr1_trie_node_remove_child_path(
    kaps_jr1_trie_node * self)
{
    kaps_jr1_trie_node *cur_node_p = self;
    kaps_jr1_trie_node *parent_node_p;
    kaps_jr1_lpm_trie *trie_p = cur_node_p->m_trie_p;
    uint32_t list_size;

    
    while (cur_node_p)
    {
        if (kaps_jr1_lsn_mc_get_prefix_count(cur_node_p->m_lsn_p) || cur_node_p->m_child_p[NLMNSTRIE_LCHILD] != NULL
            || cur_node_p->m_child_p[NLMNSTRIE_RCHILD] != NULL)
            return;

        if (cur_node_p->m_depth <= trie_p->m_expansion_depth)
            return;

        parent_node_p = cur_node_p->m_parent_p;
        if (cur_node_p->m_node_type != NLMNSTRIENODE_LP && !cur_node_p->m_isRptNode && !cur_node_p->m_aptLmpsofarPfx_p)
        {
            if (cur_node_p->m_rptAptLmpsofarPfx)
                kaps_jr1_assert(0, "RPT APT Lmpsofar should be NULL \n");

            parent_node_p->m_child_p[cur_node_p->m_child_id] = NULL;

            list_size = trie_p->m_num_items_in_to_delete_nodes;
            if (list_size >= KAPS_JR1_MAX_SIZE_OF_TRIE_ARRAY)
                kaps_jr1_assert(0, "m_todelete_nodes overflow \n");

            trie_p->m_todelete_nodes[list_size] = cur_node_p;
            trie_p->m_num_items_in_to_delete_nodes++;

            cur_node_p = cur_node_p->m_parent_p;
        }
        else
        {
            break;
        }
    }
}



static int32_t
kaps_jr1_trie_node_pvt_expand_trieHelper(
    kaps_jr1_trie_node * self,
    NlmNsTrieNode__TraverseArgs * args)
{
    uint32_t d = self->m_depth;

    if (d == KAPS_JR1_CAST_PTR_TO_U32(args->arg1))
    {
        return 0;
    }
    if (self->m_child_p[NLMNSTRIE_LCHILD] == NULL)
    {
        kaps_jr1_trie_node_add_child(self, self->m_trie_p, 0);
    }
    if (self->m_child_p[NLMNSTRIE_RCHILD] == NULL)
    {
        kaps_jr1_trie_node_add_child(self, self->m_trie_p, 1);
    }

    return 1;
}



void
kaps_jr1_trie_node_pvt_expand_trie(
    kaps_jr1_trie_node * self,
    uint32_t depth)
{
    NlmNsTrieNode__TraverseArgs args;
    args.arg1 = KAPS_JR1_CAST_U32_TO_PTR(depth);
    kaps_jr1_trie_node_traverse_dfs_preorder(self, kaps_jr1_trie_node_pvt_expand_trieHelper, &args);
}

static int32_t
kaps_jr1_trie_node_printHelper(
    kaps_jr1_trie_node * self,
    NlmNsTrieNode__TraverseArgs * args)
{
    FILE *fp = args->arg1;

    if (kaps_jr1_trie_node_pvt_is_lp_node(self) && (kaps_jr1_lsn_mc_get_prefix_count(self->m_lsn_p)))
    {
        kaps_jr1_lsn_mc_print(self->m_lsn_p, fp);
    }
    return 1;
}



void
kaps_jr1_trie_node_print(
    kaps_jr1_trie_node * self,
    FILE * fp)
{
    if (self && fp)
    {
        NlmNsTrieNode__TraverseArgs args;
        args.arg1 = (void *) fp;
        args.arg2 = NULL;
        kaps_jr1_trie_node_traverse_dfs_preorder(self, kaps_jr1_trie_node_printHelper, &args);
    }
}

kaps_jr1_trie_node *
kaps_jr1_trie_node_get_path(
    kaps_jr1_trie_node * self,
    const uint8_t * pfxdata,
    uint32_t pfxlen)
{
    uint8_t bit;
    uint16_t depth;

    depth = self->m_depth;
    while (depth < pfxlen && self)
    {
        bit = KAPS_JR1_PREFIX_PVT_GET_BIT(pfxdata, pfxlen, depth);
        self = self->m_child_p[bit];
        depth++;
    }
    return self;
}

int32_t
kaps_jr1_trienode_wb_save_rpt(
    kaps_jr1_trie_node * rpt_node,
    uint32_t * nv_offset)
{
    kaps_jr1_trie_global *trie_global = rpt_node->m_trie_p->m_trie_global;
    struct kaps_jr1_wb_rpt_entry_info rpt_entry;
    uint32_t len_in_bytes;

    rpt_entry.rpt_entry_len_1 = rpt_node->m_depth;

    rpt_entry.rpt_id = rpt_node->m_rptId;
    rpt_entry.rpt_uuid = rpt_node->m_rpt_uuid;
    rpt_entry.pool_id = rpt_node->m_poolId;
    rpt_entry.num_ipt_entries = rpt_node->m_numIptEntriesInSubtrie;
    rpt_entry.num_apt_entries = rpt_node->m_numAptEntriesInSubtrie;
    rpt_entry.num_reserved_160b_trig = rpt_node->m_numReserved160bTrig;

    len_in_bytes = sizeof(struct kaps_jr1_wb_rpt_entry_info);
    *nv_offset += len_in_bytes;
    return trie_global->wb_fun->write_fn(trie_global->wb_fun->handle,
                                         (uint8_t *) & rpt_entry, len_in_bytes, *nv_offset - len_in_bytes);

}



void
kaps_jr1_trie_node_update_iitLmpsofar(
    kaps_jr1_trie_node * self,
    kaps_jr1_trie_node * ancestorIptNodeWithPotentialLmpPfx,
    kaps_jr1_pfx_bundle ** potentialLmpPfx_pp,
    kaps_jr1_pfx_bundle * rqtPfx,
    NlmTblRqtCode rqtCode)
{
    struct kaps_jr1_lsn_mc_settings *settings;
    struct kaps_jr1_device *device;
    struct kaps_jr1_hb *hb_entry;
    uint32_t is_candidate_node;
    uint32_t list_size;
    kaps_jr1_pfx_bundle *potentialLmpPfx;
    struct kaps_jr1_ad_db *ad_db;
    

    if (!self)
        return;

    (void) ancestorIptNodeWithPotentialLmpPfx;
    (void) ad_db;

    potentialLmpPfx = *potentialLmpPfx_pp;

    settings = self->m_trie_p->m_lsn_settings_p;
    device = settings->m_device;

    is_candidate_node = 0;
    if (self->m_node_type == NLMNSTRIENODE_LP)
        is_candidate_node = 1;


    if (is_candidate_node)
    {

        if (rqtCode == NLM_FIB_PREFIX_INSERT || rqtCode == NLM_FIB_PREFIX_INSERT_WITH_INDEX)
        {
            
            if (self->m_iitLmpsofarPfx_p)
            {
                if (self->m_iitLmpsofarPfx_p->m_nPfxSize > potentialLmpPfx->m_nPfxSize)
                {
                    return;
                }
            }

            if (self->m_iitLmpsofarPfx_p)
            {
                if (settings->m_areHitBitsPresent)
                {
                    hb_entry = NULL;
                    if (self->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
                    {
                        struct kaps_jr1_db *db;
                        struct kaps_jr1_aging_entry *active_aging_table;

                        KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, self->m_iitLmpsofarPfx_p->m_backPtr, db);

                        KAPS_JR1_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb_entry),
                                                (uintptr_t) self->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

                        active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);
                        if (hb_entry == NULL)
                        {
                            return; 
                        }
                        active_aging_table[hb_entry->bit_no].num_idles = KAPS_JR1_HB_SPECIAL_VALUE;
                    }
                }
            }

            if (!potentialLmpPfx->m_isLmpsofarPfx) 
            {
                potentialLmpPfx->m_isLmpsofarPfx = 1;
            }

            self->m_iitLmpsofarPfx_p = potentialLmpPfx;
            
        }
        else if (rqtCode == NLM_FIB_PREFIX_DELETE)
        {
            
            if (self->m_iitLmpsofarPfx_p != rqtPfx)
                return;

            if (self->m_iitLmpsofarPfx_p)
            {
                if (settings->m_areHitBitsPresent)
                {
                    hb_entry = NULL;
                    if (self->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle)
                    {
                        struct kaps_jr1_db *db;
                        struct kaps_jr1_aging_entry *active_aging_table;

                        KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, self->m_iitLmpsofarPfx_p->m_backPtr, db)
                            KAPS_JR1_WB_HANDLE_READ_LOC((db->common_info->hb_info.hb), (&hb_entry),
                                                    (uintptr_t) self->m_iitLmpsofarPfx_p->m_backPtr->hb_user_handle);

                        active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);
                        if (hb_entry == NULL)
                        {
                            return; 
                        }
                        active_aging_table[hb_entry->bit_no].num_idles = KAPS_JR1_HB_SPECIAL_VALUE;
                    }
                }
            }

            
            if (potentialLmpPfx && !potentialLmpPfx->m_isLmpsofarPfx) 
            {
                potentialLmpPfx->m_isLmpsofarPfx = 1;
            }

            self->m_iitLmpsofarPfx_p = potentialLmpPfx;

        }
        else if (rqtCode == NLM_FIB_PREFIX_UPDATE_AD)
        {
            
            if (self->m_iitLmpsofarPfx_p != rqtPfx)
                return;
        }
        else
        {
            return;
        }

        if (self->m_node_type == NLMNSTRIENODE_LP)
        {
            list_size = self->m_trie_p->m_num_items_in_to_update_sit;

            if (list_size >= KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_SIT_ARRAY)
                kaps_jr1_assert(0, "m_toupdate_sit overflow \n");

            self->m_trie_p->m_toupdate_sit[list_size] = self;
            self->m_trie_p->m_num_items_in_to_update_sit++;
        }

        if (self->m_isRptNode)
        {
            list_size = self->m_trie_p->m_num_items_in_to_update_rit;

            if (list_size >= KAPS_JR1_MAX_SIZE_OF_TO_UPDATE_RIT_ARRAY)
                kaps_jr1_assert(0, "m_toupdate_rit overflow \n");

            self->m_trie_p->m_toupdate_rit[list_size] = self;
            self->m_trie_p->m_num_items_in_to_update_rit++;

        }

       
    }

    if (self->m_child_p[0])
        kaps_jr1_trie_node_update_iitLmpsofar(self->m_child_p[0], ancestorIptNodeWithPotentialLmpPfx, 
                        potentialLmpPfx_pp, rqtPfx, rqtCode);

    if (self->m_child_p[1])
        kaps_jr1_trie_node_update_iitLmpsofar(self->m_child_p[1], ancestorIptNodeWithPotentialLmpPfx, 
                        potentialLmpPfx_pp, rqtPfx, rqtCode);
    
}


