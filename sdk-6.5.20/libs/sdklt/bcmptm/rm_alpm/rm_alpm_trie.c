/*! \file rm_alpm_trie.c
 *
 * Interface functions for trie management
 *
 * This file contains the implementation for trie management
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* Implementation notes:
 * Trie is a prefix based data strucutre.
 * It is based on modification to digital search trie.
 * This implementation is not a Path compressed Binary Trie (or)
 * a Patricia Trie.
 * It is a custom version which represents prefix on a digital search trie
 * as following.
 * A given node on the trie could be a Payload node or a Internal node.
 * Each node is represented by <skip address, skip length> pair.
 * Each node represents the given prefix it represents when the prefix is viewed
 * from Right to Left. ie., Most significant bits to Least significant bits.
 * Each node has a Right & Left child which branches based on bit on that
 * position.
 */


/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_dq.h>
#include <bcmdrd_config.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>

#include "rm_alpm_trie.h"
#include "rm_alpm_trie_util.h"


/*******************************************************************************
 * Defines
 */
#define TRIE_TRAVERSE_STOP(state, rv) \
    {if (state == TRAVERSE_STATE_DONE || rv < 0) {SHR_ERR_EXIT(rv);} }

#define TRIE_LOG_DEBUG if (repart_debug) LOG_CLI
#define FULL_MATCH_AND_MORE 0
#define EXACT_MATCH         1
#define OUT_MATCH           2
#define PARTIAL_MATCH       3
#define MIN(a, b)           ((a) < (b) ? (a) : (b))

#define SPLIT_FURTHER        0
#define SPLIT_GOOD           1
#define SPLIT_LEN_EXCEED     2
#define SPLIT_ABORT          3

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPMTRIE

#define TRIE_ASSERT(x)   if (!(x)) \
        LOG_ERROR(BSL_LOG_MODULE, ( BSL_META("Trie Assert Fail\n")));
#define TRIE_ASSERT_RETURN(x)   if (!(x)) \
       {LOG_ERROR(BSL_LOG_MODULE, ( BSL_META("Trie Assert Fail\n")));\
        SHR_ERR_EXIT(SHR_E_INTERNAL);}

#define INTERNAL_NODES_ALLOC_CHUNK_SIZE  512
#define BFS_NODES_ALLOC_CHUNK_SIZE       64
#define TRIES_ALLOC_CHUNK_SIZE           16
#define CLONE_CHUNK_SIZE                 256
/*******************************************************************************
 * Typedefs
 */
typedef enum trie_split_states_e_s {
    TRIE_SPLIT_STATE_NONE,
    TRIE_SPLIT_STATE_PAYLOAD_SPLIT,
    TRIE_SPLIT_STATE_PAYLOAD_SPLIT_DONE,
    TRIE_SPLIT_STATE_PRUNE_NODES,
    TRIE_SPLIT_STATE_DONE,
    TRIE_SPLIT_STATE_MAX
} trie_split_states_e_t;

typedef enum trie_split2_states_e_s {
    TRIE_SPLIT2_STATE_NONE,
    TRIE_SPLIT2_STATE_PRUNE_NODES,
    TRIE_SPLIT2_STATE_DONE,
    TRIE_SPLIT2_STATE_MAX
} trie_split2_states_e_t;

typedef struct trie_list_s {
    rm_alpm_trie_node_t *node;
    struct trie_list_s *next;
} trie_list_t;

typedef struct tre_bfs_node_s {
    rm_alpm_trie_node_t *node;
    shr_dq_t listnode;
    struct tre_bfs_node_s *next;
} trie_bfs_node_t;

/*******************************************************************************
 * Private variables
 */
static const int repart_debug = 0;
static const int trie_validate = 0;
static const int propagate_debug = 1;
static rm_alpm_trie_t *repart_trie = NULL; /* internal usage only */

static shr_lmm_hdl_t trie_internal_nodes[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static shr_lmm_hdl_t bfs_traverse_nodes[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static shr_lmm_hdl_t bfs_traverse_next_nodes[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static shr_lmm_hdl_t all_tries[BCMDRD_CONFIG_MAX_UNITS][ALPMS];

/*******************************************************************************
 * Private Functions
 */
static int
trie_split2_insert(uint32_t            mkl,
                   rm_alpm_trie_node_t *trie,
                   uint32_t            *key,
                   uint32_t            *bpm,
                   uint32_t            key_len,
                   rm_alpm_trie_node_t *payload,
                   rm_alpm_trie_node_t **child,
                   int                 child_count);
static int
trie_insert(int u, int m,
            uint32_t            mkl,
            rm_alpm_trie_node_t *trie,
            uint32_t            *key,
            uint32_t            *bpm,
            uint32_t            key_len,
            rm_alpm_trie_node_t *payload,
            rm_alpm_trie_node_t **child,
            int                 child_count);
static int
trie_delete(int u, int m,
            uint32_t            mkl,
            rm_alpm_trie_node_t *trie,
            uint32_t            *key,
            uint32_t            key_len,
            rm_alpm_trie_node_t **payload,
            rm_alpm_trie_node_t **child);

/*
 * Function:
 *     trie_match_type
 * Purpose:
 *     Calculate match type based on lcp, key_len, skip_len
 */
static inline int
trie_match_type(int key_len, int lcp, int skip_len)
{
    int type;
    if (skip_len == lcp && lcp < key_len) {
        type = FULL_MATCH_AND_MORE;
    } else if (skip_len == lcp && lcp == key_len) {
        type = EXACT_MATCH;
    } else if (skip_len > lcp && lcp == key_len) {
        type = OUT_MATCH;
    } else { /* lcp < key_len && lcp < skip_len */
        type = PARTIAL_MATCH;
    }
    return type;

}


/*
 * Function:
 *     trie_node_print
 * Purpose:
 *     Print detail for a trie node
 */
static int
trie_node_print(rm_alpm_trie_node_t *trie,
                uint32_t            *key,
                uint32_t            len,
                uint32_t            mkl)
{
    int i, word_size;
    if (trie != NULL) {

        if (key == NULL) {
            LOG_CLI((BSL_META("trie: %p, type %s, skip_addr 0x%x skip_len %d "
                              "count:%d bpm:0x%x Child[0]:%p Child[1]:%p "),
                     (void*)trie, (trie->type == PAYLOAD) ? "P" : "I",
                     trie->skip_addr, trie->skip_len,
                     trie->count, trie->bpm, (void*)trie->child[0].child_node,
                     (void*)trie->child[1].child_node));
        } else {
            LOG_CLI((BSL_META("trie: %p, type %s, skip_addr 0x%x skip_len %d "
                              "count:%d bpm:0x%x Child[0]:%p Child[1]:%p "
                              "mkl=%d len=%d "),
                     (void*)trie, (trie->type == PAYLOAD) ? "P" : "I",
                     trie->skip_addr, trie->skip_len,
                     trie->count, trie->bpm, (void*)trie->child[0].child_node,
                     (void*)trie->child[1].child_node,
                     mkl, len));
            LOG_CLI((BSL_META("key[")));
            word_size = ((mkl + 31) / 32);
            for (i = 0; i < word_size; i++) {
                if ((i + 1)  < word_size) {
                    LOG_CLI((BSL_META("0x%08x:"), key[i]));
                } else {
                    LOG_CLI((BSL_META("0x%08x"), key[i]));
                }
            }
            LOG_CLI((BSL_META("]\n")));
        }
    }
    return SHR_E_NONE;
}


/*
 * Function:
 *     trie_preorder_traverse
 * Purpose:
 *     Recursive routine to traverse using preorder
 */
static int
trie_preorder_traverse(rm_alpm_trie_node_t     *trie,
                       rm_alpm_trie_callback_f cb,
                       void                    *user_data)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *tmp1, *tmp2;

    if (trie == NULL || !cb) {
        return SHR_E_NONE;
    } else {
        /* make the node delete safe */
        tmp1 = trie->child[0].child_node;
        tmp2 = trie->child[1].child_node;
        rv = cb(trie, user_data);
    }

    if (SHR_SUCCESS(rv)) {
        rv = trie_preorder_traverse(tmp1, cb, user_data);
    }
    if (SHR_SUCCESS(rv)) {
        rv = trie_preorder_traverse(tmp2, cb, user_data);
    }
    return rv;
}

/*
 * Function:
 *     trie_preorder_traverse
 * Purpose:
 *     Recursive routine to traverse using postorder
 */
static int
trie_postorder_traverse(rm_alpm_trie_node_t     *trie,
                        rm_alpm_trie_callback_f cb,
                        void                    *user_data)
{
    int rv = SHR_E_NONE;

    if (trie == NULL) {
        return SHR_E_NONE;
    }

    rv = trie_postorder_traverse(trie->child[0].child_node, cb, user_data);
    if (SHR_SUCCESS(rv)) {
        rv = trie_postorder_traverse(trie->child[1].child_node, cb, user_data);
    }
    if (SHR_SUCCESS(rv)) {
        rv = cb(trie, user_data);
    }
    return rv;
}

/*
 * Function:
 *     trie_preorder_traverse
 * Purpose:
 *     Recursive routine to traverse using inorder
 */
static int
trie_inorder_traverse(rm_alpm_trie_node_t     *trie,
                      rm_alpm_trie_callback_f cb,
                      void                    *user_data)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *tmp = NULL;

    if (trie == NULL) {
        return SHR_E_NONE;
    }

    if (SHR_SUCCESS(rv)) {
        rv = trie_inorder_traverse(trie->child[0].child_node, cb, user_data);
    }

    /* make the trie pointers delete safe */
    tmp = trie->child[1].child_node;

    if (SHR_SUCCESS(rv)) {
        rv = cb(trie, user_data);
    }

    if (SHR_SUCCESS(rv)) {
        rv = trie_inorder_traverse(tmp, cb, user_data);
    }
    return rv;
}

static int
trie_bfs_traverse(int u, int m,
                  rm_alpm_trie_node_t *trie,
                  rm_alpm_trie_callback_f cb,
                  void *user_data)
{
    int rv = SHR_E_NONE;
    shr_dq_t queue;
    shr_dq_t *elem;
    trie_bfs_node_t *enq_node;
    trie_bfs_node_t *deq_node;
    rm_alpm_trie_node_t *trie_node = NULL;

    if (trie == NULL) {
        return SHR_E_NONE;
    }

    SHR_DQ_INIT(&queue);
    enq_node = shr_lmm_alloc(bfs_traverse_nodes[u][m]);
    if (enq_node == NULL) {
        return SHR_E_MEMORY;
    }
    enq_node->node = trie;
    SHR_DQ_INSERT_HEAD(&queue, &enq_node->listnode);

    while (!SHR_DQ_EMPTY(&queue)) {
        SHR_DQ_REMOVE_TAIL(&queue, elem);
        deq_node = SHR_DQ_ELEMENT_GET(trie_bfs_node_t *, elem, listnode);
        trie_node = deq_node->node;
        if (trie_node->child[0].child_node) {
            enq_node = shr_lmm_alloc(bfs_traverse_nodes[u][m]);
            if (enq_node == NULL) {
                return SHR_E_MEMORY;
            }
            enq_node->node = trie_node->child[0].child_node;
            SHR_DQ_INSERT_HEAD(&queue, &enq_node->listnode);
        }
        if (trie_node->child[1].child_node) {
            enq_node = shr_lmm_alloc(bfs_traverse_nodes[u][m]);
            if (enq_node == NULL) {
                return SHR_E_MEMORY;
            }
            enq_node->node = trie_node->child[1].child_node;
            SHR_DQ_INSERT_HEAD(&queue, &enq_node->listnode);
        }
        /* Callback on payloads only */
        if (trie_node->type == PAYLOAD) {
            rv = cb(trie_node, user_data);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
        shr_lmm_free(bfs_traverse_nodes[u][m], deq_node);
    }
    return rv;
}




static int
trie_bfs_traverse_get_next(int u, int m, bool get_first,
                           rm_alpm_trie_node_t *trie,
                           shr_dq_t *queue,
                           rm_alpm_trie_callback_f cb,
                           void *user_data,
                           void **payload
                           )
{
    int rv = SHR_E_NONE;
    shr_dq_t *elem;
    trie_bfs_node_t *enq_node;
    trie_bfs_node_t *deq_node;
    rm_alpm_trie_node_t *trie_node = NULL;
    int next_visited = 0;

    if (get_first) {
        if (trie == NULL) {
            return SHR_E_NONE;
        }

        SHR_DQ_INIT(queue);
        enq_node = shr_lmm_alloc(bfs_traverse_next_nodes[u][m]);
        if (enq_node == NULL) {
            return SHR_E_MEMORY;
        }
        enq_node->node = trie;
        SHR_DQ_INSERT_HEAD(queue, &enq_node->listnode);
    }

    while (!SHR_DQ_EMPTY(queue) && !next_visited) {
        SHR_DQ_REMOVE_TAIL(queue, elem);
        deq_node = SHR_DQ_ELEMENT_GET(trie_bfs_node_t *, elem, listnode);
        trie_node = deq_node->node;
        if (trie_node->child[0].child_node) {
            enq_node = shr_lmm_alloc(bfs_traverse_next_nodes[u][m]);
            if (enq_node == NULL) {
                return SHR_E_MEMORY;
            }
            enq_node->node = trie_node->child[0].child_node;
            SHR_DQ_INSERT_HEAD(queue, &enq_node->listnode);
        }
        if (trie_node->child[1].child_node) {
            enq_node = shr_lmm_alloc(bfs_traverse_next_nodes[u][m]);
            if (enq_node == NULL) {
                return SHR_E_MEMORY;
            }
            enq_node->node = trie_node->child[1].child_node;
            SHR_DQ_INSERT_HEAD(queue, &enq_node->listnode);
        }
        /* Callback on payloads only */
        if (trie_node->type == PAYLOAD) {
            if (cb) {
                rv = cb(trie_node, user_data);
                if (rv == SHR_E_BUSY) {
                    next_visited = 1;
                    *payload = trie_node;
                    rv = SHR_E_NONE;
                } else if (SHR_FAILURE(rv)) {
                    return rv;
                }
            } else {
                next_visited = 1;
                *payload = trie_node;
            }
        }
        shr_lmm_free(bfs_traverse_next_nodes[u][m], deq_node);
    }
    if (next_visited == 0) {
        *payload = NULL;
    }
    return rv;
}


/* Traverse wrapper */
static int
trie_traverse(int u, int m,
              rm_alpm_trie_node_t             *trie,
              rm_alpm_trie_callback_f         cb,
              void                            *user_data,
              rm_alpm_trie_traverse_order_e_t order)
{
    int rv = SHR_E_NONE;

    switch (order) {
    case TRAVERSE_PREORDER:
        rv = trie_preorder_traverse(trie, cb, user_data);
        break;
    case TRAVERSE_POSTORDER:
        rv = trie_postorder_traverse(trie, cb, user_data);
        break;
    case TRAVERSE_INORDER:
        rv = trie_inorder_traverse(trie, cb, user_data);
        break;
    case TRAVERSE_BFS:
        rv = trie_bfs_traverse(u, m, trie, cb, user_data);
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *     trie_preorder_traverse2
 * Purpose:
 *     Recursive routine to traverse using preorder
 */
static int
trie_preorder_traverse2(rm_alpm_trie_node_t              *ptrie,
                        rm_alpm_trie_node_t              *trie,
                        rm_alpm_trie_traverse_states_e_t *state,
                        rm_alpm_trie_callback_ext_f      cb,
                        void                             *user_data)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *lc, *rc;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (trie == NULL || !cb) {
        SHR_EXIT();
    } else {
        TRIE_ASSERT_RETURN(!ptrie || ptrie->type == PAYLOAD);

        /* make the trie delete safe */
        lc = trie->child[0].child_node;
        rc = trie->child[1].child_node;
        if (trie->type == PAYLOAD) { /* no need to callback on internal nodes */
            rv = cb(ptrie, trie, state, user_data);
            TRIE_TRAVERSE_STOP(*state, rv);

            /* Change the ptrie as trie if applicable */
            /* make the ptrie delete safe */
            if (*state != TRAVERSE_STATE_DELETED) {
                ptrie = trie;
            }
        }
    }

    if (SHR_SUCCESS(rv)) {
        rv = trie_preorder_traverse2(ptrie, lc, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SHR_SUCCESS(rv)) {
        rv = trie_preorder_traverse2(ptrie, rc, state, cb, user_data);
    }
    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     trie_postorder_traverse2
 * Purpose:
 *     Recursive routine to traverse using postorder
 */
static int
trie_postorder_traverse2(rm_alpm_trie_node_t              *ptrie,
                         rm_alpm_trie_node_t              *trie,
                         rm_alpm_trie_traverse_states_e_t *state,
                         rm_alpm_trie_callback_ext_f      cb,
                         void                             *user_data)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *ori_ptrie = ptrie;
    rm_alpm_trie_node_t *rc;
    rm_alpm_node_type_e_t trie_type;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (trie == NULL) {
        SHR_EXIT();
    }

    TRIE_ASSERT_RETURN(!ptrie || ptrie->type == PAYLOAD);

    /* Change the ptrie as trie if applicable */
    if (trie->type == PAYLOAD) {
        ptrie = trie;
    }

    /* During the callback, a trie node can be deleted or inserted.
     * For a deleted node, its internal parent could also be deleted, thus to
     * make it safe we should record rc.
     */
    trie_type = trie->type;
    rc = trie->child[1].child_node;

    if (SHR_SUCCESS(rv)) {
        rv = trie_postorder_traverse2(ptrie, trie->child[0].child_node, state,
                                      cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SHR_SUCCESS(rv)) {
        rv = trie_postorder_traverse2(ptrie, rc, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SHR_SUCCESS(rv)) {
        if (trie_type == PAYLOAD) {
            rv = cb(ori_ptrie, trie, state, user_data);
        }
    }
    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     trie_inorder_traverse2
 * Purpose:
 *     Recursive routine to traverse using inorder
 */
static int
trie_inorder_traverse2(rm_alpm_trie_node_t              *ptrie,
                       rm_alpm_trie_node_t              *trie,
                       rm_alpm_trie_traverse_states_e_t *state,
                       rm_alpm_trie_callback_ext_f      cb,
                       void                             *user_data)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *rc = NULL;
    rm_alpm_trie_node_t *ori_ptrie = ptrie;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (trie == NULL) {
        SHR_EXIT();
    }

    TRIE_ASSERT_RETURN(!ptrie || ptrie->type == PAYLOAD);

    /* Change the ptrie as trie if applicable */
    if (trie->type == PAYLOAD) {
        ptrie = trie;
    }

    rv = trie_inorder_traverse2(ptrie, trie->child[0].child_node, state, cb,
                                user_data);
    TRIE_TRAVERSE_STOP(*state, rv);

    /* make the trie delete safe */
    rc = trie->child[1].child_node;

    if (SHR_SUCCESS(rv)) {
        if (trie->type == PAYLOAD) {
            rv = cb(ptrie, trie, state, user_data);
            TRIE_TRAVERSE_STOP(*state, rv);
            /* make the ptrie delete safe */
            if (*state == TRAVERSE_STATE_DELETED) {
                ptrie = ori_ptrie;
            }
        }
    }

    if (SHR_SUCCESS(rv)) {
        rv = trie_inorder_traverse2(ptrie, rc, state, cb, user_data);
    }
    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}


/* Traverse wrapper with visibility for both ptrie and trie */
static int
trie_traverse2(rm_alpm_trie_node_t              *trie,
               rm_alpm_trie_callback_ext_f      cb,
               void                             *user_data,
               rm_alpm_trie_traverse_order_e_t  order,
               rm_alpm_trie_traverse_states_e_t *state)
{
    int rv = SHR_E_NONE;

    switch (order) {
    case TRAVERSE_PREORDER:
        rv = trie_preorder_traverse2(NULL, trie, state, cb, user_data);
        break;
    case TRAVERSE_POSTORDER:
        rv = trie_postorder_traverse2(NULL, trie, state, cb, user_data);
        break;
    case TRAVERSE_INORDER:
        rv = trie_inorder_traverse2(NULL, trie, state, cb, user_data);
        break;
    default:
        return SHR_E_UNAVAIL;
    }

    return rv;
}

/*
 * Function:
 *     trie_repartition
 * Purpose:
 *     Prune  in the list (assuming the lower/longer comes first)
 */
static void
trie_repartition_list_prune(int         level,
                            int         trie_count,
                            int         merge_count,
                            int         repart_count,
                            int         left,
                            int         *prune_count,
                            trie_list_t **ptrie_list, /* [in|out] */
                            trie_list_t **ptrie_tail)
{
    trie_list_t *ptrie_list_iter = NULL;
    int stay_count = 0;     /* staying counts */
    int lvl;
    trie_list_t *ptrie_list_next = NULL; /* for delete safe */

    *prune_count = 0;
    *ptrie_tail = NULL;
    ptrie_list_iter = *ptrie_list;

    repart_count = left ? 0 : repart_count;
    if (ptrie_list_iter) {
        if (repart_debug) {
            lvl = level;
            while (lvl) {
                TRIE_LOG_DEBUG((BSL_META("%x  "), level - lvl));
                lvl--;
            }
            TRIE_LOG_DEBUG(("After %cC Pruning:", left ? 'L' : 'R'));
        }
        while (ptrie_list_iter) {
            ptrie_list_next = ptrie_list_iter->next;
            if ((int)ptrie_list_iter->node->count >=
                (trie_count - merge_count - repart_count - stay_count)) {
                *ptrie_tail = ptrie_list_iter;
                stay_count++;
                TRIE_ASSERT(*prune_count == 0);
            } else {
                (*prune_count)++;
                TRIE_LOG_DEBUG(("%p  ", (void*)ptrie_list_iter->node));
                SHR_FREE(ptrie_list_iter);
            }
            ptrie_list_iter = ptrie_list_next;
        }

        /* part of the list pruned, break the list */
        if (*ptrie_tail) {
            (*ptrie_tail)->next = NULL;
        } else { /* whole list pruned */
            *ptrie_list = NULL;
        }
        if (repart_debug) {
            TRIE_LOG_DEBUG(("\n"));

            ptrie_list_iter = *ptrie_list;
            lvl = level;
            while (lvl) {
                TRIE_LOG_DEBUG((BSL_META("%x  "), level - lvl));
                lvl--;
            }
            TRIE_LOG_DEBUG(("After %cC Pruned: ", left ? 'L' : 'R'));
            while (ptrie_list_iter) {
                TRIE_LOG_DEBUG(("%p -> ", (void*)ptrie_list_iter->node));
                ptrie_list_iter = ptrie_list_iter->next;
            }
            TRIE_LOG_DEBUG(("NULL \n"));
        }
    }
}


/*
 * Function:
 *     trie_repartition
 * Purpose:
 *     Recursive routine to do repartition
 */

static int
trie_repartition(rm_alpm_trie_node_t              *ptrie,
                 rm_alpm_trie_node_t              *trie,
                 rm_alpm_trie_traverse_states_e_t *state,
                 rm_alpm_trie_repart_callback_f   cb,
                 void                             *user_data,
                 trie_list_t                      **ptrie_list_in_out,
                 int                              level,
                 int                              *repart_count,
                 int                              *merge_count)
{
    int rv = SHR_E_NONE;
    rm_alpm_node_type_e_t curr_trie_type;
    rm_alpm_trie_node_t *rc = NULL;
    rm_alpm_trie_node_t *new_ptrie = NULL;       /* new ptrie from callback, to
                                                    be added to list. */
    rm_alpm_trie_node_t *further_ptrie = NULL;   /* ptrie for recursion */
    rm_alpm_trie_node_t *latest_ptrie = NULL;    /* ptrie for callback */
    trie_list_t *ptrie_list = NULL;      /* always update-to-date ptrie list */
    trie_list_t *ptrie_list_next = NULL; /* for delete safe */
    trie_list_t *new_ptrie_list = NULL;
    trie_list_t *l_ptrie_list = NULL;    /* ptrie list from left child */
    trie_list_t *r_ptrie_list = NULL;    /* ptrie list from right child */
    trie_list_t *ptrie_tail = NULL;      /* ptrie list tail */
    trie_list_t *r_ptrie_tail = NULL;    /* right ptrie list tail */
    trie_list_t *l_ptrie_tail = NULL;    /* left ptrie list tail */
    trie_list_t *ptrie_list_iter = NULL;
    int curr_trie_count;
    int trc = 0, lrc = 0, rrc = 0;      /* repartition counts */
    int tmc = 0, lmc = 0, rmc = 0;      /* merge counts */
    int tmp = 0;
    int lvl;

    int l_prune_count = 0, r_prune_count = 0;   /* pruning counts */

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (trie == NULL) {
        SHR_EXIT();
    }

    /* assert(!ptrie || ptrie->type == PAYLOAD); */

    /* During the callback, a trie node can be deleted or inserted.
     * For a deleted node, its internal parent could also be deleted, thus to
     * make it safe we should record rc.
     */
    curr_trie_type  = trie->type;
    curr_trie_count = trie->count;
    rc = trie->child[1].child_node;

    if (ptrie_list_in_out) {
        ptrie_list = *ptrie_list_in_out;

        /* Set tail */
        ptrie_tail = ptrie_list;
        if (ptrie_tail) {
            while (ptrie_tail->next) {
                ptrie_tail = ptrie_tail->next;
            }
        }
    }
    if (repart_debug) {
        lvl = level;
        while (lvl) {
            LOG_CLI((BSL_META("%x  "), level - lvl));
            lvl--;
        }
        LOG_CLI((BSL_META("#Ptrie %p, trie %p\n"), (void*)ptrie, (void*)trie));
    }

    if (curr_trie_type == PAYLOAD) {
        further_ptrie = trie;
    } else if (ptrie_list) {
        further_ptrie = ptrie_tail->node;
        /* assert(ptrie_tail->node->count <= ptrie->count); */
    } else {
        further_ptrie = ptrie;
    }

    rv = trie_repartition(further_ptrie, trie->child[0].child_node,
                          state, cb, user_data,
                          (curr_trie_type == PAYLOAD) ? NULL : &l_ptrie_list,
                          level + 1, &lrc, &lmc);

    /* Note, the ptrie_list could be updated */
    TRIE_TRAVERSE_STOP(*state, rv);

    /* Somehow, the trie_repartition may get us some new payload nodes,
     * these new payload nodes can be many, and they can be lower than the trie,
     * or higher than it, but all must be lower than ptrie. The child cannot
     * decide status of each, only the current node can.
     */

    tmc += lmc;
    trie_repartition_list_prune(level, curr_trie_count, tmc, lrc, 1,
                                &l_prune_count, &l_ptrie_list, &l_ptrie_tail);
    lrc = lrc - l_prune_count;
    /* assert(lrc == l_stay_count); */

    trc += lrc;

    if (l_ptrie_list) {
        if (ptrie_list) {
            ptrie_tail->next = l_ptrie_list;
        } else {
            ptrie_list = l_ptrie_list;
        }

        /* Reset tail */
        ptrie_tail = l_ptrie_tail;
    }

    if (curr_trie_type == PAYLOAD) {
        further_ptrie = trie;
    } else if (ptrie_list) {
        further_ptrie = ptrie_tail->node;
        /* assert(ptrie_tail->node->count <= ptrie->count); */
    } else {
        further_ptrie = ptrie;
    }

    /* One important thing to notice is that after done with left child trie,
     * the current node (trie) could be deleted, but, the right child trie won't
     * be deleted. It's because that the internal node are allcoated because of
     * its children, not because of its parent. Despite of that, the skip length
     * and skip addr of the right child could be modified.
     */

    rv = trie_repartition(further_ptrie, rc, state, cb, user_data,
                          (curr_trie_type == PAYLOAD) ? NULL : &r_ptrie_list,
                          level + 1, &rrc, &rmc);

    TRIE_TRAVERSE_STOP(*state, rv);
    tmc += rmc;
    trie_repartition_list_prune(level, curr_trie_count, tmc, lrc, 1,
                                &r_prune_count, &r_ptrie_list, &r_ptrie_tail);
    rrc = rrc - r_prune_count;
    /* assert(rrc == r_stay_count); */

    trc += rrc;

    if (r_ptrie_list) {
        if (ptrie_list) {
            ptrie_tail->next = r_ptrie_list;
        } else {
            ptrie_list = r_ptrie_list;
        }
        /* Reset tail */
        ptrie_tail = r_ptrie_tail;
    }

    if (curr_trie_type == PAYLOAD) {
        /* Get the latest ptrie, could be from the list or the orig ptre */
        if (ptrie_list) {
            latest_ptrie = ptrie_tail->node;
            /* assert(ptrie_tail->node->count <= ptrie->count); */
        } else {
            latest_ptrie = ptrie;
        }

        /* If curr_trie_type is payload, then trie must not be deleted */
        rv = cb(latest_ptrie, trie, state, user_data, &new_ptrie);

        /* a new ptrie generated, add to the list as tail */
        if (new_ptrie) {
            SHR_ALLOC(new_ptrie_list, sizeof(trie_list_t), "bcmptmRmalpmTrielist");
            SHR_NULL_CHECK(new_ptrie_list, SHR_E_MEMORY);
            sal_memset(new_ptrie_list, 0, sizeof(trie_list_t));
            new_ptrie_list->node = new_ptrie;

            if (ptrie_list) {
                ptrie_tail->next = new_ptrie_list;
                ptrie_tail = new_ptrie_list;
            } else {
                ptrie_list = new_ptrie_list;
            }

            if (repart_debug) {
                lvl = level;
                while (lvl) {
                    TRIE_LOG_DEBUG((BSL_META("%x  "), level - lvl));
                    lvl--;
                }

                TRIE_LOG_DEBUG(("New Head %p. Now: ", (void*)new_ptrie));
                ptrie_list_iter = ptrie_list;
                while (ptrie_list_iter) {
                    TRIE_LOG_DEBUG(("%p -> ", (void*)ptrie_list_iter->node));
                    ptrie_list_iter = ptrie_list_iter->next;
                }
                TRIE_LOG_DEBUG(("NULL. Updated trie:\n"));
                if (repart_trie) {
                    bcmptm_rm_alpm_trie_dump(repart_trie, 0, 0);
                }
            }
            trc++;
        } else if (*state == TRAVERSE_STATE_DELETED) {
            if (repart_debug) {
                lvl = level;
                while (lvl) {
                    TRIE_LOG_DEBUG((BSL_META("%x  "), level - lvl));
                    lvl--;
                }

                TRIE_LOG_DEBUG(("Node %p merged (deleted).  Updated trie:\n",
                                (void*)trie));
                if (repart_trie) {
                    bcmptm_rm_alpm_trie_dump(repart_trie, 0, 0);
                }
            }
            tmc++;
        }
    }

    if (ptrie_list_in_out) {
        *ptrie_list_in_out = ptrie_list;
    } else {
        /* Done with repartition, clear list if any */
        tmp = 0;
        ptrie_list_iter = ptrie_list;
        while (ptrie_list_iter) {
            ptrie_list_next = ptrie_list_iter->next;
            if (repart_debug) {
                lvl = level;
                while (lvl) {
                    TRIE_LOG_DEBUG((BSL_META("%x  "), level - lvl));
                    lvl--;
                }
                TRIE_LOG_DEBUG((" Clear Prune %p . trc %d. \n",
                                (void*)ptrie_list_iter->node, trc));

                if ((int)ptrie_list_iter->node->count !=
                    (curr_trie_count - tmc - tmp)) {
                    TRIE_LOG_DEBUG((
                                    "Error: Count %d . Curr %d tmc %d tmp %d\n",
                                    ptrie_list_iter->node->count,
                                    curr_trie_count, tmc, tmp));
                }
            }
            SHR_FREE(ptrie_list_iter);
            ptrie_list_iter = ptrie_list_next;
            tmp++;
        }
        /* assert(trc == tmp); */
        trc = 0;
    }

    /* assert(trc >= 0 && tmc >= 0); */
    if (repart_count) {
        *repart_count += trc;
    }
    if (merge_count) {
        *merge_count += tmc;
    }
    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *     trie_preorder_iter_get_first
 * Purpose:
 *     Recursive routine to get first payload for a given root
 */
static int
trie_preorder_iter_get_first(rm_alpm_trie_node_t *node,
                             rm_alpm_trie_node_t **payload)
{
    int rv = SHR_E_NONE;

    if (!payload) {
        return SHR_E_PARAM;
    }

    if (*payload != NULL) {
        return SHR_E_NONE;
    }

    if (node == NULL) {
        return SHR_E_NONE;
    } else {
        if (node->type == PAYLOAD) {
            *payload = node;
            return rv;
        }
    }

    if (SHR_SUCCESS(rv)) {
        rv = trie_preorder_iter_get_first(node->child[0].child_node, payload);
    }
    if (SHR_SUCCESS(rv)) {
        rv = trie_preorder_iter_get_first(node->child[1].child_node, payload);
    }
    return rv;
}

/*
 * Function:
 *     trie_preorder_iter_get_first
 * Purpose:
 *     Recursive routine to get first payload for a given root
 */
static int
trie_preorder_iter_get_first2(rm_alpm_trie_node_t *node,
                             rm_alpm_trie_node_t **payload)
{
    if (!payload) {
        return SHR_E_PARAM;
    }

    if (*payload != NULL) {
        return SHR_E_NONE;
    }

    if (node == NULL) {
        return SHR_E_NONE;
    } else {
        if (node->type == PAYLOAD ||
            (node->child[0].child_node && node->child[1].child_node)) {
            *payload = node;
            return SHR_E_NONE;
        }
    }

    return trie_preorder_iter_get_first2(node->child[0].child_node ?
                                         node->child[0].child_node :
                                         node->child[1].child_node , payload);
}


/*
 * Function:
 *     trie_dump
 * Purpose:
 *     Recursive routine to dump all nodes (including internal) with full
 *     ip/length
 */
static int
trie_dump(rm_alpm_trie_node_t     *trie,
          rm_alpm_trie_callback_f cb,
          void                    *user_data,
          uint32_t                level,
          uint32_t                *key,
          uint32_t                len,
          uint32_t                mkl)
{
    uint32_t key0[ALPM_MAX_KEY_SIZE_WORDS] = {0};
    uint32_t key1[ALPM_MAX_KEY_SIZE_WORDS] = {0};
    uint32_t len0 = 0;
    uint32_t len1 = 0;

    if (trie == NULL) {
        return SHR_E_NONE;
    } else {
        uint32_t lvl = level;

        if (key) {
            bcmptm_rm_alpm_key_append(mkl, key, &len,
                                      trie->skip_addr, trie->skip_len);
        }
        while (lvl) {
            if (lvl == 1) {
                LOG_CLI((BSL_META("|-")));
            } else {
                LOG_CLI((BSL_META("| ")));
            }
            lvl--;
        }

        if (cb) {
            cb(trie, user_data);
        } else {
            trie_node_print(trie, key, len, mkl);
        }
    }

    if (key) {
        sal_memcpy(key0, key, sizeof(key0));
        sal_memcpy(key1, key, sizeof(key1));
        len0 = len;
        len1 = len;

        bcmptm_rm_alpm_key_append(mkl, key0, &len0, 0, 1);
        bcmptm_rm_alpm_key_append(mkl, key1, &len1, 1, 1);
    }
    trie_dump(trie->child[0].child_node, cb, user_data, level + 1, key0, len0,
              mkl);
    trie_dump(trie->child[1].child_node, cb, user_data, level + 1, key1, len1,
              mkl);
    return SHR_E_NONE;
}


static int
trie_fast_search(uint32_t            mkl,
                 rm_alpm_trie_node_t *trie,
                 uint32_t            *key,
                 uint32_t            key_len,
                 rm_alpm_trie_node_t **payload)
{
    uint32_t lcp;
    int bit;
    while (1) {
        LCP_LEN(lcp, mkl, key, key_len, trie->skip_addr, trie->skip_len);
        switch (trie_match_type(key_len, lcp, trie->skip_len)) {
        case FULL_MATCH_AND_MORE:
            bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);
            if (trie->child[bit].child_node) {
                trie = trie->child[bit].child_node;
                key_len = key_len - lcp - 1;
                continue;
            }
            break;
        case EXACT_MATCH:
            if (trie->type == PAYLOAD) {
                *payload = trie;
                /* no assert here, possible during dbucket search
                 * due to 1* and 0* bucket search
                 */
                return SHR_E_NONE;
            }
            break;
        case OUT_MATCH:
        default:
            break;
        }
        break;
    }
    return SHR_E_NOT_FOUND;
}


/*
 * Function:
 *     trie_search
 * Purpose:
 *     Recursive routine to exact match a given key
 */
static int
trie_search(uint32_t            mkl,
            rm_alpm_trie_node_t *trie,
            uint32_t            *key,
            uint32_t            key_len,
            rm_alpm_trie_node_t **payload,
            uint32_t            *result_key,
            uint32_t            *result_len,
            uint32_t            dump,
            uint32_t            find_pivot)
{
    uint32_t lcp = 0;
    int bit = 0, rv = SHR_E_NONE;

    if (!trie || (key_len && trie->skip_len && !key)) {
        return SHR_E_PARAM;
    }
    if ((result_key && !result_len) ||
        (!result_key && result_len)) {
        return SHR_E_PARAM;
    }

    LCP_LEN(lcp, mkl, key, key_len, trie->skip_addr, trie->skip_len);

    if (dump) {
        trie_node_print(trie, NULL, 0, 0);
    }

    switch (trie_match_type(key_len, lcp, trie->skip_len)) {
    case FULL_MATCH_AND_MORE:
        bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);
        if (dump) {
            LOG_CLI((BSL_META(" Length: %d Next-Bit[%d] \n"), key_len, bit));
        }

        if (result_key) {
            rv = bcmptm_rm_alpm_key_append(mkl,
                                           result_key, result_len,
                                           trie->skip_addr, trie->skip_len);
            if (SHR_FAILURE(rv)) return rv;
        }

        /* based on next bit branch left or right */
        if (trie->child[bit].child_node) {

            if (result_key) {
                rv = bcmptm_rm_alpm_key_append(mkl,
                                               result_key, result_len, bit, 1);
                if (SHR_FAILURE(rv)) return rv;
            }

            return trie_search(mkl, trie->child[bit].child_node, key,
                               key_len - lcp - 1, payload,
                               result_key, result_len, dump, find_pivot);
        } else {
            return SHR_E_NOT_FOUND; /* not found */
        }

        break;
    case EXACT_MATCH:
        if (dump) {
            LOG_CLI((BSL_META(": MATCH \n")));
        }
        *payload = trie;
        if (trie->type != PAYLOAD && !find_pivot) {
            /* no assert here, possible during dbucket search
             * due to 1* and 0* bucket search
             */
            return SHR_E_NOT_FOUND;
        }
        if (result_key) {
            rv = bcmptm_rm_alpm_key_append(mkl,
                                           result_key, result_len,
                                           trie->skip_addr, trie->skip_len);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
        break;
    case OUT_MATCH:
        if (!find_pivot) {
            return SHR_E_NOT_FOUND;
        }
        *payload = trie;
        if (result_key) {
            rv = bcmptm_rm_alpm_key_append(mkl,
                                           result_key, result_len,
                                           trie->skip_addr, trie->skip_len);
            if (SHR_FAILURE(rv)) {
                return rv;
            }
        }
        break;
    default:
        rv = SHR_E_NOT_FOUND;
        break;
    }
    return rv;
}


/*
 * Function:
 *     trie_find_lpm
 * Purpose:
 *     Recursive function for LPM match searching.
 *     callback on all payload nodes if cb != NULL.
 */
static int
trie_find_lpm(uint32_t                mkl,
              rm_alpm_trie_node_t     *trie,
              uint32_t                *key,
              uint32_t                key_len,
              rm_alpm_trie_node_t     **payload,
              rm_alpm_trie_callback_f cb,
              void                    *user_data,
              uint32_t                exclude_self,
              uint32_t                *result_key,
              uint32_t                *result_len,
              uint32_t                *cut)
{
    uint32_t lcp = 0;
    int bit = 0, rv = SHR_E_NONE;
    int match_type;

    if (!trie || (key_len && trie->skip_len && !key)) {
        return SHR_E_PARAM;
    }
    if ((result_key && !result_len) ||
        (!result_key && result_len)) {
        return SHR_E_PARAM;
    }

    lcp = bcmptm_rm_alpm_lcplen(mkl,
                                key, key_len, trie->skip_addr, trie->skip_len);
    match_type = trie_match_type(key_len, lcp, trie->skip_len);

    switch (match_type) {
    case FULL_MATCH_AND_MORE:
    case EXACT_MATCH:
        if (trie->type == PAYLOAD) {
            /* exact match case */
            if (payload != NULL &&
                (match_type != EXACT_MATCH || !exclude_self)) {
                /* lpm is exact match */
                *payload = trie;
            }
            if (result_key) {
                rv = bcmptm_rm_alpm_key_append(mkl,
                                               result_key, result_len,
                                               trie->skip_addr, trie->skip_len);
                *cut = 0;
                if (SHR_FAILURE(rv)) {
                    return rv;
                }
            }
            if (cb != NULL) {
                /* callback with the exact match node */
                rv = cb(trie, user_data);
                if (SHR_FAILURE(rv)) {
                    /* early bailout if there is error in callback handling */
                    return rv;
                }
            }
        }

        if (match_type == EXACT_MATCH) {
            break;
        }


        bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

        /* based on next bit branch left or right */
        if (trie->child[bit].child_node) {
            if (result_key) {
                rv = bcmptm_rm_alpm_key_append(mkl,
                                               result_key, result_len,
                                               bit, 1);
                (*cut)++;
                if (SHR_FAILURE(rv)) {
                    return rv;
                }
            }

            return trie_find_lpm(mkl, trie->child[bit].child_node,
                                 key, key_len - lcp - 1,
                                 payload, cb, user_data, exclude_self,
                                 result_key, result_len, cut);
        }
        break;
    default:
        break;
    }

    return rv;
}


/*
 * Function:
 *     trie_find_bpm
 * Purpose:
 *     Recursive function to find bpm_len.
 * Note:
 *     trie->bpm format:
 *     bit 0 is for the pivot itself (longest)
 *     bit skip_len is for the trie branch leading to the pivot node (shortest)
 *     bits (0-skip_len) is for the routes in the parent node's bucket
 */
static int
trie_find_bpm(uint32_t            mkl,
              rm_alpm_trie_node_t *trie,
              uint32_t            *key,
              uint32_t            key_len,
              int                 *bpm_length)
{
    uint32_t lcp = 0, local_bpm_mask = 0;
    int bit = 0, rv = SHR_E_NONE;
    uint32_t lsb = 0;

    if (!trie || (key_len && trie->skip_len && !key) ||
        (key_len > mkl)) {
        return SHR_E_PARAM;
    }

    /* calculate number of matching msb bits */
    lcp = bcmptm_rm_alpm_lcplen(mkl,
                                key, key_len, trie->skip_addr, trie->skip_len);

    if (FULL_MATCH_AND_MORE == trie_match_type(key_len, lcp, trie->skip_len)) {
        /* fully matched and more bits to check, go down the trie */
        bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

        if (trie->child[bit].child_node) {
            rv = trie_find_bpm(mkl,
                               trie->child[bit].child_node,
                               key, key_len - lcp - 1, bpm_length);
            /* on the way back, start bpm_length accumulation
             * when encounter first non-0 bpm
             */
            if (*bpm_length >= 0) {
                /* child node has non-zero bpm, just need to
                 * accumulate skip_len and branch bit
                 */
                *bpm_length += (trie->skip_len + 1);
            } else if (trie->bpm & BITMASK(trie->skip_len + 1)) {
                /* first non-zero bmp on the way back */
                BITGETLSBSET(trie->bpm, trie->skip_len, lsb);
                if ((int)lsb >= 0) {
                    *bpm_length = trie->skip_len - lsb;
                }
            }
            /* on the way back, and so far all bpm are 0 */
            return rv;
        }
    }

    /* no need to go further, we find whatever bits matched and
     * check that part of the bpm mask
     */
    /* coverity[large_shift : FALSE] */
    local_bpm_mask = trie->bpm & (~(BITMASK(trie->skip_len - lcp)));
    if (local_bpm_mask & BITMASK(trie->skip_len + 1)) {
        /* first non-zero bmp on the way back */
        BITGETLSBSET(local_bpm_mask, trie->skip_len, lsb);
        if ((int)lsb >= 0) {
            *bpm_length = trie->skip_len - lsb;
        }
    }

    return rv;
}

/*
 * Function:
 *     trie_bpm_mask_get
 * Purpose:
 *     Recursive routine to get the bpm mask of target key.
 * Note:
 *     The bpm_mask returned can have multiple bits set, all bits
 *     are ancestors for the deepest reached node. However, that
 *     deepest reached node can be exactly matched, or partially matched,
 *     or out matched. That means some farest bits may not be the bpm
 *     for the target key. That is ok as long as we do not use this
 *     bpm_mask blindly, i.e, the selected bits must be checked first to
 *     apply to a trie node.
 */
static int
trie_bpm_mask_get(uint32_t            mkl,
                  rm_alpm_trie_node_t *trie,
                  uint32_t            *key,
                  uint32_t            key_len,
                  uint32_t            *bpm_mask)
{
    uint32_t lcp = 0, scratch = 0;
    int bit = 0, rv = SHR_E_NONE;

    if (!trie || (key_len > mkl)) {
        return SHR_E_PARAM;
    }

    /* calculate number of matching msb bits */
    lcp = bcmptm_rm_alpm_lcplen(mkl,
                                key, key_len, trie->skip_addr, trie->skip_len);

    if (FULL_MATCH_AND_MORE == trie_match_type(key_len, lcp, trie->skip_len)) {
        /* fully matched and more bits to check, go down the trie */
        bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

        if (trie->child[bit].child_node) {
            bcmptm_rm_alpm_bpm_append(mkl,
                                      bpm_mask, &scratch,
                                      trie->bpm, trie->skip_len + 1);
            rv = trie_bpm_mask_get(mkl,
                                   trie->child[bit].child_node,
                                   key, key_len - lcp - 1, bpm_mask);
            return rv;
        }
    }

    rv = bcmptm_rm_alpm_bpm_append(mkl, bpm_mask, &scratch, trie->bpm,
                                   trie->skip_len + 1);
    return rv;
}

/*
 * Function:
 *   trie_skip_node_free
 * Purpose:
 *   Destroy a chain of rm_alpm_trie_node_t that has the target node at the end.
 *   The target node is not necessarily PAYLOAD type, but all nodes
 *   on the chain except for the end must have only one branch.
 * Input:
 *   key      --  target key
 *   length   --  target key length
 *   free_end --  free
 */
static int
trie_skip_node_free(int u, int m,
                    uint32_t            mkl,
                    rm_alpm_trie_node_t *trie,
                    uint32_t            *key,
                    uint32_t            key_len)
{
    uint32_t lcp = 0;
    int bit = 0, rv = SHR_E_NONE;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!trie || (key_len && trie->skip_len && !key)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    lcp = bcmptm_rm_alpm_lcplen(mkl,
                                key, key_len, trie->skip_addr, trie->skip_len);


    switch (trie_match_type(key_len, lcp, trie->skip_len)) {
    case FULL_MATCH_AND_MORE:
        bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

        /* There should be only one branch on the chain until the end node */
        if (!trie->child[0].child_node == !trie->child[1].child_node) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* based on next bit branch left or right */
        if (trie->child[bit].child_node) {
            rv = trie_skip_node_free(u, m, mkl, trie->child[bit].child_node, key,
                                     key_len - lcp - 1);
            if (SHR_SUCCESS(rv)) {
                TRIE_ASSERT_RETURN(trie->type == INTERNAL);
                shr_lmm_free(trie_internal_nodes[u][m], trie);
            }
        } else {
            rv = SHR_E_NOT_FOUND;     /* not found */
        }
        break;
    case EXACT_MATCH:
        /* the end node is not necessarily type payload. */
        /* Do not free the end */

        rv = SHR_E_NONE;
        break;
    default:
        rv = SHR_E_NOT_FOUND;
        break;
    }

    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}



/*
 * Function:
 *   trie_skip_node_alloc
 * Purpose:
 *   create a chain of rm_alpm_trie_node_t that has the payload at the end.
 *   each node in the chain can skip upto _MAX_SKIP_LEN number of bits,
 *   while the child pointer in the chain represent 1 bit. So totally
 *   each node can absorb (MAX_SKIP_LEN+1) bits.
 * Input:
 *   key      --  inserting key
 *   bpm      --  bpm of the key, passing null skips bpm management
 *   msb      --  NOTE: valid msb position 1 based, 0 means skip 0/0 node
 *   skip_len --  skip_len of the whole chain
 *   payload  --  payload node we want to insert
 *   count    --  payload count underneath - mostly 1 except some tricky cases
 * Output:
 *   node     -- return pointer of the starting node of the chain.
 */
static int
trie_skip_node_alloc(int u, int m,
                     uint32_t            mkl,
                     rm_alpm_trie_node_t **node,
                     uint32_t            *key,
                     uint32_t            *bpm,
                     uint32_t            msb,
                     uint32_t            skip_len,
                     rm_alpm_trie_node_t *payload,
                     uint32_t            count)
{
    uint32_t lsb = 0, msbpos = 0, lsbpos = 0, bit = 0, index;
    rm_alpm_trie_node_t *child = NULL, *skip_node = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* calculate lsb bit position, also 1 based */
    lsb = ((msb) ? msb + 1 - skip_len : msb);

    TRIE_ASSERT_RETURN(((int)msb >= 0) && ((int)lsb >= 0));

    if (!node || !key || !payload || msb > mkl || msb < skip_len) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (msb == 0) {
        /* skip_len == 0 case, create a payload node with skip_len = 0
         * and bpm should be 1 bits only
         * bit 0 and bit "skip_len" are same bit (bit 0).
         */
        skip_node = payload;
        sal_memset(skip_node, 0, sizeof(rm_alpm_trie_node_t));
        skip_node->type = PAYLOAD;
        skip_node->count = count;
        if (bpm) {
            skip_node->bpm = bcmptm_rm_alpm_key_bits_get(mkl, bpm, 1, 1, 0);
        }
        *node = skip_node;
        SHR_EXIT();
    }

    TRIE_ASSERT_RETURN(((int)msb >= 1) && ((int)lsb >= 1));

    lsbpos = lsb - 1;
    for (index = BITS2SKIPOFF(lsb); index <= BITS2SKIPOFF(msb); index++) {
        /* each loop process MAX_SKIP_LEN number of bits?? */
        if (lsbpos == lsb - 1) {
            /* (lsbpos == lsb-1) is only true for first node (loop) here */
            skip_node = payload;
        } else {
            /* other nodes need to be created */
            skip_node = shr_lmm_alloc(trie_internal_nodes[u][m]);
            SHR_NULL_CHECK(skip_node, SHR_E_MEMORY);
        }

        /* init memory */
        sal_memset(skip_node, 0, sizeof(rm_alpm_trie_node_t));

        /* calculate msb bit position of current chunk of bits we are processing
         */
        msbpos = MIN(index * MAX_SKIP_LEN - 1, msb - 1);

        /* calculate the skip_len of the created node */
        if ((msbpos - lsbpos + 1) > MAX_SKIP_LEN) {
            TRIE_ASSERT(0);
            if (skip_node != payload) {
                shr_lmm_free(trie_internal_nodes[u][m], skip_node);
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
        skip_node->skip_len = msbpos - lsbpos + 1;

        /* calculate the skip_addr (skip_length number of bits).
         * skip might be skipping bits on 2 different words
         * if msb & lsb spawns 2 word boundary in worst case
         */
        if (BITS2WORDS(msbpos + 1) != BITS2WORDS(lsbpos + 1)) {
            /* pull snippets from the different words & fuse */
            uint32_t mword, lword;
            /* coverity[result_independent_of_operands] */
            mword = key[KEY_BIT2IDX(mkl, msbpos + 1)] &
                    MASK((msbpos + 1) % NUM_WORD_BITS);
            lword = key[KEY_BIT2IDX(mkl, lsbpos + 1)];

            skip_node->skip_addr =  SHL(mword,
                                        (skip_node->skip_len -
                                         ((msbpos + 1) % NUM_WORD_BITS)),
                                        NUM_WORD_BITS);
            /* coverity[result_independent_of_operands] */
            skip_node->skip_addr |= SHR(lword,
                                        (lsbpos % NUM_WORD_BITS),
                                        NUM_WORD_BITS);
        } else {
            /* coverity[result_independent_of_operands] */
            skip_node->skip_addr = SHR(key[KEY_BIT2IDX(mkl, msbpos + 1)],
                                       (lsbpos % NUM_WORD_BITS), NUM_WORD_BITS);
        }

        /* set up the chain of child pointer, first node has no child since
           "child" was inited to NULL */
        if (child) {
            skip_node->child[bit].child_node = child;
        }

        /* calculate child pointer for next loop. NOTE: skip_addr has not been
         * masked so we still have the child bit in the skip_addr here.
         */
        bit = (skip_node->skip_addr &
               SHL(1, skip_node->skip_len - 1, MAX_SKIP_LEN)) ? 1 : 0;

        /* calculate node type */
        if (lsbpos == lsb - 1) {
            /* first node is payload */
            skip_node->type = PAYLOAD;
        } else {
            /* other nodes are internal nodes */
            skip_node->type = INTERNAL;
        }

        /* all internal nodes will have the same "count" as the payload node */
        skip_node->count = count;

        /* advance lsb to next word */
        lsbpos += skip_node->skip_len;

        /* calculate bpm of the skip_node */
        if (bpm) {
            if ((uint32_t)lsbpos == mkl) {
                /* this node is root, so there is no branch bit here */
                skip_node->bpm =
                    bcmptm_rm_alpm_key_bits_get(mkl, bpm,
                                                lsbpos,
                                                skip_node->skip_len,
                                                1);
            } else {
                /* for last node: lsbpos now points to the 0based branch bit,
                 *                plus 1 to become 1based
                 * for other nodes: lsbpos points to 1based branch bit.
                 */
                skip_node->bpm =
                    bcmptm_rm_alpm_key_bits_get(mkl, bpm,
                                                lsbpos + 1,
                                                skip_node->skip_len + 1,
                                                1);
            }
        }

        /* for all child nodes 0/1 is implicitly obsorbed on parent(branch) */
        if (msbpos != msb - 1) {
            /* msbpos == (msb-1) is only true for the last node
             * branch bit of last node is absorbed by caller of this routine
             */

            skip_node->skip_len--;
        }
        /* Include the branch bit for both all nodes (last & non-last) */
        skip_node->bpm &= MASK(skip_node->skip_len + 1);
        skip_node->skip_addr &= MASK(skip_node->skip_len);
        child = skip_node;
    }

    *node = skip_node;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     trie_insert_em
 * Purpose:
 *     Insert helper function for exact match case
 */
static int
trie_insert_em(int u, int m,
               uint32_t            mkl,
               rm_alpm_trie_node_t *trie,
               uint32_t            *key,
               uint32_t            *bpm,
               uint32_t            key_len,
               rm_alpm_trie_node_t *payload,    /* payload node */
               rm_alpm_trie_node_t **child,
               int                 child_count,
               int                 lcp)
{
    if (trie->type != INTERNAL) {
        /* duplicate */
        return SHR_E_EXISTS;
    } else {
        /* change the internal node to payload node */
        CLONE_TRIE_NODE(payload, trie);
        shr_lmm_free(trie_internal_nodes[u][m], trie);
        payload->type = PAYLOAD;
        payload->count += child_count;
        *child = payload;

        if (bpm) {
            /* bpm at this internal mode must be same as the inserted pivot */
            payload->bpm |=
                bcmptm_rm_alpm_key_bits_get(mkl, bpm, lcp + 1, lcp + 1, 1);
            /* implicity preserve the previous bpm & set bit 0 -myself bit */
        }
        return SHR_E_NONE;
    }
}

/*
 * Function:
 *     trie_insert_om
 * Purpose:
 *     Insert helper function for out match case
 */
static int
trie_insert_om(uint32_t            mkl,
               rm_alpm_trie_node_t *trie,
               uint32_t            *key,
               uint32_t            *bpm,
               uint32_t            key_len,
               rm_alpm_trie_node_t *payload,     /* payload node */
               rm_alpm_trie_node_t **child,
               int                 child_count,
               int                 lcp)
{
    int bit;
    /* this node is new parent for the old trie node */
    /* lcp is the new skip key_len */
    CLONE_TRIE_NODE(payload, trie);
    *child = payload;


    bit = (trie->skip_addr &
           SHL(1, trie->skip_len - key_len - 1, MAX_SKIP_LEN)) ? 1 : 0;
    trie->skip_addr &= MASK(trie->skip_len - key_len - 1);
    trie->skip_len  -= (key_len + 1);

    if (bpm) {
        trie->bpm &= MASK(trie->skip_len + 1);
    }

    payload->skip_addr = (key_len > 0) ? key[KEY_BIT2IDX(mkl, key_len)] : 0;
    payload->skip_addr &= MASK(key_len);
    payload->skip_len  = key_len;
    payload->child[bit].child_node = trie;
    payload->child[!bit].child_node = NULL;
    payload->type = PAYLOAD;
    payload->count += child_count;

    if (bpm) {
        payload->bpm = SHR(payload->bpm, trie->skip_len + 1, NUM_WORD_BITS);
        payload->bpm |= bcmptm_rm_alpm_key_bits_get(mkl,
                                                    bpm,
                                                    payload->skip_len + 1,
                                                    payload->skip_len + 1,
                                                    1);
    }
    return SHR_E_NONE;
}

/*
 * Function:
 *     trie_insert_fm
 * Purpose:
 *     Insert helper function for full match and more case
 */
static int
trie_insert_fm(int u, int m,
               uint32_t            mkl,
               rm_alpm_trie_node_t *trie,
               uint32_t            *key,
               uint32_t            *bpm,
               uint32_t            key_len,
               rm_alpm_trie_node_t *payload,     /* payload node */
               rm_alpm_trie_node_t **child,
               int                 child_count,
               int                 lcp)
{
    int bit, rv = SHR_E_NONE;
    rm_alpm_trie_node_t *node = NULL;

    bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

    if (!trie->child[bit].child_node) {
        /* the key is going to be one of the child of existing node */
        /* should be the child */
        rv = trie_skip_node_alloc(u, m, mkl, &node, key, bpm,
                                  /* exclude branch bit (-1) */
                                  key_len - lcp - 1,
                                  key_len - lcp - 1,
                                  payload, child_count);
        if (SHR_SUCCESS(rv)) {
            trie->child[bit].child_node = node;
            trie->count += child_count;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,(
                          BSL_META(
                              "\n Error on trie skip node allocation [%d]!!\n"),
                          rv));
        }
    } else {
        rv = trie_insert(u, m, mkl, trie->child[bit].child_node,
                         key, bpm, key_len - lcp - 1,
                         payload, child, child_count);
        if (SHR_SUCCESS(rv)) {
            trie->count += child_count;
            if (*child != NULL) {
                /* change the old child pointer to new child */
                trie->child[bit].child_node = *child;
                *child = NULL;
            }
        }
    }
    return rv;
}

/*
 * Function:
 *     trie_insert_pm
 * Purpose:
 *     Insert helper function for partial match case
 */
static int
trie_insert_pm(int u, int m,
               uint32_t            mkl,
               rm_alpm_trie_node_t *trie,
               uint32_t            *key,
               uint32_t            *bpm,
               uint32_t            key_len,
               rm_alpm_trie_node_t *payload,     /* payload node */
               rm_alpm_trie_node_t **child,
               int                 child_count,
               int                 lcp)
{
    int rv, bit;
    rm_alpm_trie_node_t *newchild = NULL;
    rm_alpm_trie_node_t *node = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* need to introduce internal nodes */
    node = shr_lmm_alloc(trie_internal_nodes[u][m]);
    SHR_NULL_CHECK(node, SHR_E_MEMORY);

    CLONE_TRIE_NODE(node, trie);

    rv = trie_skip_node_alloc(u, m, mkl, &newchild, key, bpm,
                              key_len - lcp - 1,
                              key_len - lcp - 1,
                              payload, child_count);
    if (SHR_SUCCESS(rv)) {
        bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

        node->child[!bit].child_node = trie;
        node->child[bit].child_node = newchild;
        node->type = INTERNAL;
        node->skip_addr = SHR(trie->skip_addr, trie->skip_len - lcp,
                              MAX_SKIP_LEN);
        node->skip_len = lcp;
        node->count += child_count;
        if (bpm) {
            node->bpm = SHR(node->bpm, trie->skip_len - lcp, MAX_SKIP_LEN);
        }
        *child = node;

        trie->skip_addr &= MASK(trie->skip_len - lcp - 1);
        trie->skip_len  -= (lcp + 1);
        if (bpm) {
            trie->bpm &= MASK(trie->skip_len + 1);
        }
    } else {
        LOG_CLI((BSL_META("\n Error on trie skip node allocation [%d]!!!!\n"),
                 rv));
        shr_lmm_free(trie_internal_nodes[u][m], node);
    }
    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *     trie_insert
 * Purpose:
 *     Recursive routine to insert a key into the trie
 * Input:
 *   trie     --  comparing node
 *   key      --  inserting key
 *   bpm      --  bpm of the key, passing null skips bpm management
 *   key_len  --  inserting key length
 *   payload  --  inserting payload node
 *   child_count -- child count
 * Output:
 *   child    -- child pointer if the child is modified
 */
static int
trie_insert(int u, int m,
            uint32_t            mkl,
            rm_alpm_trie_node_t *trie,
            uint32_t            *key,
            uint32_t            *bpm,
            uint32_t            key_len,
            rm_alpm_trie_node_t *payload,
            rm_alpm_trie_node_t **child,
            int                 child_count)
{
    uint32_t lcp;
    int rv = SHR_E_NONE;

    if (!trie || (key_len && trie->skip_len && !key) ||
        !payload || !child || (key_len > mkl)) {
        return SHR_E_PARAM;
    }
    *child = NULL;

    LCP_LEN(lcp, mkl, key, key_len, trie->skip_addr, trie->skip_len);
    /* insert cases:
     * 1 - new key could be the parent of existing node
     * 2 - new key could become the child of a existing node
     * 3 - internal node could be inserted and the key becomes one of child
     * 4 - internal node is converted to a payload node */

    switch (trie_match_type(key_len, lcp, trie->skip_len)) {
    case EXACT_MATCH:
        rv = trie_insert_em(u, m, mkl, trie, key, bpm, key_len,
                            payload, child, child_count, lcp);
        break;
    case FULL_MATCH_AND_MORE:
        rv = trie_insert_fm(u, m, mkl, trie, key, bpm, key_len,
                            payload, child, child_count, lcp);
        break;
    case OUT_MATCH:
        rv = trie_insert_om(mkl, trie, key, bpm, key_len,
                            payload, child, child_count, lcp);
        break;
    case PARTIAL_MATCH:
    default:
        rv = trie_insert_pm(u, m, mkl, trie, key, bpm, key_len,
                            payload, child, child_count, lcp);
        break;
    }

    return rv;
}
/*
 * Function:
 *     trie_split2_om
 * Purpose:
 *     Insert helper function for out match case
 */
static int
trie_split2_om(uint32_t            mkl,
               rm_alpm_trie_node_t *trie,
               uint32_t            *key,
               uint32_t            *bpm,
               uint32_t            key_len,
               rm_alpm_trie_node_t *payload,     /* payload node */
               rm_alpm_trie_node_t **child,
               int                 child_count,
               int                 lcp)
{
    int bit;
    /* this node is new parent for the old trie node */
    /* lcp is the new skip key_len */
    CLONE_TRIE_NODE(payload, trie);
    *child = payload;


    bit = (trie->skip_addr &
           SHL(1, trie->skip_len - key_len - 1, MAX_SKIP_LEN)) ? 1 : 0;
    trie->skip_addr &= MASK(trie->skip_len - key_len - 1);
    trie->skip_len  -= (key_len + 1);

    if (bpm) {
        trie->bpm &= MASK(trie->skip_len + 1);
    }

    payload->skip_addr = (key_len > 0) ? key[KEY_BIT2IDX(mkl, key_len)] : 0;
    payload->skip_addr &= MASK(key_len);
    payload->skip_len  = key_len;
    payload->child[bit].child_node = trie;
    payload->child[!bit].child_node = NULL;
    payload->type = PAYLOAD;
    payload->count += child_count;

    if (bpm) {
        payload->bpm = SHR(payload->bpm, trie->skip_len + 1, NUM_WORD_BITS);
        payload->bpm |= bcmptm_rm_alpm_key_bits_get(mkl,
                                                    bpm,
                                                    payload->skip_len + 1,
                                                    payload->skip_len + 1,
                                                    1);
    }
    return SHR_E_NONE;
}

/*
 * Function:
 *     trie_split2_fm
 * Purpose:
 *     Insert helper function for full match and more case
 */
static int
trie_split2_fm(uint32_t            mkl,
               rm_alpm_trie_node_t *trie,
               uint32_t            *key,
               uint32_t            *bpm,
               uint32_t            key_len,
               rm_alpm_trie_node_t *payload,     /* payload node */
               rm_alpm_trie_node_t **child,
               int                 child_count,
               int                 lcp)
{
    int bit, rv = SHR_E_NONE;

    bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

    if (!trie->child[bit].child_node) {
        rv = SHR_E_NOT_FOUND; /* we don't need to split this trie at all */
    } else {
        rv = trie_split2_insert(mkl, trie->child[bit].child_node,
                                key, bpm, key_len - lcp - 1,
                                payload, child, child_count);
        if (SHR_SUCCESS(rv)) {
            trie->count += child_count;
            if (*child != NULL) {
                /* change the old child pointer to new child */
                trie->child[bit].child_node = *child;
                *child = NULL;
            }
        }
    }
    return rv;
}



/*
 * Function:
 *     trie_split2
 * Purpose:
 *     Recursive routine to split2 a key into the trie
 * Input:
 *   trie     --  comparing node
 *   key      --  split2ing key
 *   bpm      --  bpm of the key, passing null skips bpm management
 *   key_len  --  split2ing key length
 *   payload  --  split2ing payload node
 *   child_count -- child count
 * Output:
 *   child    -- child pointer if the child is modified
 */
static int
trie_split2_insert(uint32_t            mkl,
                   rm_alpm_trie_node_t *trie,
                   uint32_t            *key,
                   uint32_t            *bpm,
                   uint32_t            key_len,
                   rm_alpm_trie_node_t *payload,
                   rm_alpm_trie_node_t **child,
                   int                 child_count)
{
    uint32_t lcp;
    int rv = SHR_E_NONE;

    if (!trie || (key_len && trie->skip_len && !key) ||
        !payload || !child || (key_len > mkl)) {
        return SHR_E_PARAM;
    }
    *child = NULL;

    LCP_LEN(lcp, mkl, key, key_len, trie->skip_addr, trie->skip_len);
    /* split2 cases:
     * 1 - new key could be the parent of existing node
     * 2 - new key could become the child of a existing node
     * 3 - internal node could be split2ed and the key becomes one of child
     * 4 - internal node is converted to a payload node */

    switch (trie_match_type(key_len, lcp, trie->skip_len)) {
    case EXACT_MATCH:
        return SHR_E_EXISTS;
        break;
    case FULL_MATCH_AND_MORE:
        rv = trie_split2_fm(mkl, trie, key, bpm, key_len,
                            payload, child, child_count, lcp);
        break;
    case OUT_MATCH:
        rv = trie_split2_om(mkl, trie, key, bpm, key_len,
                            payload, child, child_count, lcp);
        break;
    case PARTIAL_MATCH:
    default:
        rv = SHR_E_NOT_FOUND;
        break;
    }

    return rv;
}
/*
 * Function:
 *     trie_fuse_child
 * Purpose:
 *     Fuse a trie node with its child, trie node will be freed if internal
 */
static int
trie_fuse_child(int u, int m, rm_alpm_trie_node_t *trie, int bit)
{
    rm_alpm_trie_node_t *child = NULL;
    int rv = SHR_E_NONE;

    if (trie->child[0].child_node && trie->child[1].child_node) {
        return SHR_E_PARAM;
    }

    bit = (bit > 0) ? 1 : 0;
    child = trie->child[bit].child_node;

    if (child == NULL) {
        return SHR_E_PARAM;
    } else {
        if (trie->skip_len + child->skip_len + 1 <= MAX_SKIP_LEN) {

            if (trie->skip_len == 0) trie->skip_addr = 0;

            if (child->skip_len < MAX_SKIP_LEN) {
                trie->skip_addr = SHL(trie->skip_addr, child->skip_len + 1,
                                      MAX_SKIP_LEN);
            }

            trie->skip_addr  |= SHL(bit, child->skip_len, MAX_SKIP_LEN);
            child->skip_addr |= trie->skip_addr;
            child->bpm       |=
                SHL(trie->bpm, child->skip_len + 1, MAX_SKIP_LEN);
            child->skip_len  += trie->skip_len + 1;

            /* do not free payload nodes as they are user managed */
            if (trie->type == INTERNAL) {
                shr_lmm_free(trie_internal_nodes[u][m], trie);
            }
        }
    }

    return rv;
}

/*
 * Function:
 *     trie_delete_em
 * Purpose:
 *     Delete helper function for exact match case
 */
static int
trie_delete_em(int u, int m,
               uint32_t            mkl,
               rm_alpm_trie_node_t *trie,
               uint32_t            *key,
               uint32_t            key_len,
               rm_alpm_trie_node_t **payload,
               rm_alpm_trie_node_t **child,
               int                 lcp)
{
    int rv = SHR_E_NONE;
    int bit;
    rm_alpm_trie_node_t *node = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* internal node cann't be deleted */
    if (trie->type != PAYLOAD) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (trie->child[0].child_node && trie->child[1].child_node ) {
        /* the node has 2 children, update it to internal node */
        BITCLR(trie->bpm, 0);
        node = shr_lmm_alloc(trie_internal_nodes[u][m]);
        SHR_NULL_CHECK(node, SHR_E_MEMORY);

        CLONE_TRIE_NODE(node, trie);
        node->type = INTERNAL;
        node->count--;
        *child = node;
    } else if (trie->child[0].child_node || trie->child[1].child_node ) {
        /* if this node has 1 children fuse the children with this node */
        bit = (trie->child[0].child_node) ? 0 : 1;
        trie->count--;
        if (trie->skip_len + trie->child[bit].child_node->skip_len + 1 <=
            MAX_SKIP_LEN) {
            /* we need to clear the bpm bit of itself before fusing with child
             */
            BITCLR(trie->bpm, 0);

            /* able to fuse the node with its child node */
            *child = trie->child[bit].child_node;
            rv = trie_fuse_child(u, m, trie, bit);
            if (rv != SHR_E_NONE) {
                *child = NULL;
            }
        } else {
            /* convert it to internal node, we need to alloc new memory for
             * internal nodes, since the old payload node memory will
             * be freed by caller
             */
            /* we need to clear the bpm bit of itself before converting */
            BITCLR(trie->bpm, 0);

            node = shr_lmm_alloc(trie_internal_nodes[u][m]);
            SHR_NULL_CHECK(node, SHR_E_MEMORY);

            CLONE_TRIE_NODE(node, trie);
            node->type = INTERNAL;
            *child = node;
        }
    } else { /* leaf node */
        rv = SHR_E_BUSY;
    }

    *payload = trie;
    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     trie_delete_fm
 * Purpose:
 *     Delete helper function for full match and more case
 */
static int
trie_delete_fm(int u, int m,
               uint32_t            mkl,
               rm_alpm_trie_node_t *trie,
               uint32_t            *key,
               uint32_t            key_len,
               rm_alpm_trie_node_t **payload,
               rm_alpm_trie_node_t **child,
               int                 lcp)
{
    int bit, rv;

    bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

    if (!trie->child[bit].child_node) {
        /* no child node: not found */
        return SHR_E_NOT_FOUND;
    }

    /* has child node, keep searching */
    rv = trie_delete(u, m, mkl, trie->child[bit].child_node,
                     key, key_len - lcp - 1, payload, child);

    if (rv == SHR_E_BUSY) {

        trie->child[bit].child_node = NULL; /* sal_free the child */
        rv = SHR_E_NONE;
        trie->count--;

        if (trie->type == INTERNAL) {

            bit = !bit;

            if (trie->child[bit].child_node == NULL) { /* still leaf node */
                /* parent and child connected,
                 * sal_free the middle-node itself
                 */
                shr_lmm_free(trie_internal_nodes[u][m], trie);
                rv = SHR_E_BUSY;
            } else {
                /*
                 * lost a payload child, but still has another child
                 * fuse the parent & child
                 */
                if (trie->skip_len +
                    trie->child[bit].child_node->skip_len + 1 <=
                    MAX_SKIP_LEN) {
                    *child = trie->child[bit].child_node;
                    rv = trie_fuse_child(u, m, trie, bit);
                    if (rv != SHR_E_NONE) {
                        *child = NULL;
                    }
                }
            }
        }
    } else if (SHR_SUCCESS(rv)) {
        trie->count--;
        /* update child pointer if applicable */
        if (*child != NULL) {
            trie->child[bit].child_node = *child;
            *child = NULL;
        }
    }
    return rv;
}

/*
 * Function:
 *     trie_delete
 * Purpose:
 *     Recursive routine to delete a given key
 */
static int
trie_delete(int u, int m,
            uint32_t            mkl,
            rm_alpm_trie_node_t *trie,
            uint32_t            *key,
            uint32_t            key_len,
            rm_alpm_trie_node_t **payload,
            rm_alpm_trie_node_t **child)
{
    uint32_t lcp;
    int rv = SHR_E_NONE;

    /* our algorithm should return before the key_len < 0, so this means
     * something wrong with the trie structure. Internal error?
     */
    if (!trie || (key_len && trie->skip_len && !key) ||
        !payload || !child || (key_len > mkl)) {
        return SHR_E_PARAM;
    }

    *child = NULL;

    /* check a section of key, return the number of matched bits and value of
       next bit */
    LCP_LEN(lcp, mkl, key, key_len, trie->skip_addr, trie->skip_len);

    switch (trie_match_type(key_len, lcp, trie->skip_len)) {
    case FULL_MATCH_AND_MORE:
        rv = trie_delete_fm(u, m, mkl, trie, key, key_len, payload, child, lcp);
        break;
    case EXACT_MATCH:
        rv = trie_delete_em(u, m, mkl, trie, key, key_len, payload, child, lcp);
        break;
    default:
        rv = SHR_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *     trie_split_type
 * Purpose:
 *     Determine if this is a split point, and the split_type if so
 */
static inline int
trie_split_type(rm_alpm_trie_node_t *trie,
                rm_alpm_trie_node_t *child,
                uint32_t            length,
                uint32_t            max_count,
                uint32_t            max_split_count,
                uint32_t            max_split_len,
                trie_split_states_e_t state)
{
/*
 * NOTE:
 *  ABS(trie->count * 2 - max_count) actually means
 *  ABS(trie->count - (max_count - trie->count))
 * which means the count's distance to half depth of the bucket
 */
    uint32_t split_type;

    TRIE_ASSERT(TRIE_SPLIT_STATE_NONE == state ||
                TRIE_SPLIT_STATE_PAYLOAD_SPLIT == state);
    if (trie->count != max_count && trie->count <= max_split_count) {
        if (length > max_split_len) {
            /* the pivot is getting too long, we better split at this node for
             * better bucket capacity efficiency if we can. We can split if
             * the trie node has a count != max_count, which means the
             * resulted new trie will not have all pivots (FULL)
             */

            split_type = SPLIT_LEN_EXCEED;
        } else if (length == max_split_len) {
            /*
             * (1) when the node is at the max_split_len and if used as spliting
             * point the resulted trie will not have all pivots (FULL). we
             * should split at this node.
             * (2) when the node is at the max_split_len and if the resulted
             * trie will have all pivots (FULL), we fall through to keep
             * searching
             * (3) when the node is shorter than the max_split_len and the node
             * has a more even pivot distribution compare to it's child, we
             * can split at this node. The split count must be less than or
             * equal to max_split_count.
             * (4) when the node's count is only 1, we must split at this point.
             *
             * NOTE :
             *  when trie->count == max_count, the above check will be FALSE
             *  so here it guarrantees *length < max_split_len. We don't
             *  need to further split this node.
             */

            split_type = SPLIT_GOOD;
        } else {
            uint32_t safe_count = (max_count + 1) / 2;
             if (TRIE_SPLIT_STATE_PAYLOAD_SPLIT == state) {
                if (child == NULL) {
                    split_type = SPLIT_GOOD;
                } else if (trie->type == PAYLOAD) {
                    if (ABS(child->count * 2 - max_count) >
                        ABS(trie->count * 2 - max_count)) {
                        if (child->count >= safe_count) {
                            split_type = SPLIT_FURTHER;
                        } else {
                            split_type = SPLIT_GOOD;
                        }
                    } else {
                        split_type = SPLIT_FURTHER;
                    }
                } else {
                    split_type = SPLIT_FURTHER;
                }
            } else {
                if (child == NULL) {
                    split_type = SPLIT_GOOD;
                } else if (ABS(child->count * 2 - max_count) >
                           ABS(trie->count * 2 - max_count)) {
                    if (child->count >= safe_count) {
                        split_type = SPLIT_FURTHER;
                    } else {
                        split_type = SPLIT_GOOD;
                    }
                } else {
                    split_type = SPLIT_FURTHER;
                }
            }
        }
    } else {
        split_type = SPLIT_FURTHER;
    }

    return split_type;
}


/*
 * Function:
 *     trie_split_middle_point
 * Purpose:
 *     Introduce a new middle point to reduce the new pivot len to meet
 *     the "<=max_split_len" rule as much as possible.
 */
static int
trie_split_middle_point(int u, int m,
                        uint32_t            mkl,
                        rm_alpm_trie_node_t *trie,
                        uint32_t            *pivot,
                        uint32_t            *length,
                        uint32_t            *split_count,
                        rm_alpm_trie_node_t **split_node,
                        const uint32_t      max_split_len,
                        uint32_t            *bpm)
{
    int rv = SHR_E_NONE;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Allocate a new middle node and use it as split point */
    *split_node = shr_lmm_alloc(trie_internal_nodes[u][m]);
    SHR_NULL_CHECK(*split_node, SHR_E_MEMORY);

    sal_memset((*split_node), 0, sizeof(rm_alpm_trie_node_t));
    (*split_node)->type = INTERNAL;
    (*split_node)->count = trie->count;

    if ((*length - max_split_len) > trie->skip_len) {
        /* the length is longer than the max_split_len, and the trie->skip_len
         * is shorter than the difference (max_split_len pivot is not covered by
         * this node but covered by its parent, the best we can do is split at
         * the branch lead to this node. we insert a skip_len=0 node and use it
         * as split point
         */
        (*split_node)->skip_len = 0;
        (*split_node)->skip_addr = 0;
        (*split_node)->bpm = (trie->bpm >> trie->skip_len);

        if (BITGET(trie->skip_addr, (trie->skip_len - 1))) {
            (*split_node)->child[1].child_node = trie;
        } else {
            (*split_node)->child[0].child_node = trie;
        }

        /* the split point is with length max_split_len */
        *length -= trie->skip_len;

        /* update the current node to reflect the node inserted */
        trie->skip_len = trie->skip_len - 1;
    } else {
        /* the length is longer than the max_split_len, and the trie->skip_len
         * is longer than the difference (max_split_len pivot is covered by this
         * node, we insert a node with length = max_split_len and use it as
         * split point
         */
        (*split_node)->skip_len = trie->skip_len - (*length - max_split_len);
        (*split_node)->skip_addr =
            (trie->skip_addr >> (*length - max_split_len));
        (*split_node)->bpm = (trie->bpm >> (*length - max_split_len));

        if (BITGET(trie->skip_addr, (*length - max_split_len - 1))) {
            (*split_node)->child[1].child_node = trie;
        } else {
            (*split_node)->child[0].child_node = trie;
        }

        /* update the current node to reflect the node inserted */
        trie->skip_len = *length - max_split_len - 1;

        /* the split point is with length max_split_len */
        *length = max_split_len;
    }

    trie->skip_addr = trie->skip_addr & BITMASK(trie->skip_len);
    trie->bpm = trie->bpm & BITMASK(trie->skip_len + 1);

    /* there is no need to update the parent node's child_node pointer
     * to the "trie" node since we will split here and the parent node's
     * child_node pointer will be set to NULL later
     */
    *split_count = trie->count;
    if (bpm) {
        rv = bcmptm_rm_alpm_key_shift(mkl,
                                      bpm, mkl, trie->skip_len + 1);
    }

    if (SHR_SUCCESS(rv)) {
        rv = bcmptm_rm_alpm_key_shift(mkl, pivot,
                                      *length + trie->skip_len + 1,
                                      trie->skip_len + 1);
    }
    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     trie_split
 * Purpose:
 *     Split the trie into 2 based on optimum pivot
 * Input:
 *     mkl  -- max_key_len
 *     trie -- recursing node
 *     max_split_len -- split will make sure the split point
 *                has a length shorter or equal to the max_split_len
 *                unless this will cause a no-split (all prefixs
 *                stays below the split point)
 *     max_count    -- parent trie count
 *     bpm          --
 *     state        --
 *     max_split_count -- maximum split count
 * Output:
 *     pivot       -- split pivot
 *     pivot       -- split pivot length
 *     split_node  -- split node
 *     child       -- split node child
 * Hard split rules:
 *     - The split point has a count different than original's (or no split)
 *     - The split point has a count less or equal to max_split_count.
 *     - The split point must be a payload if required so.
 * Soft split rules:
 *     - The split point has a length shorter or equal to max_split_len.
 *     - Even split.
 */
static int
trie_split(int u, int m,
           uint32_t              mkl,
           rm_alpm_trie_node_t   *trie,
           uint32_t              *pivot,
           uint32_t              *length,
           uint32_t              *split_count,
           rm_alpm_trie_node_t   **split_node,
           rm_alpm_trie_node_t   **child,
           const uint32_t        max_count,
           const uint32_t        max_split_len,
           uint32_t              *bpm,
           trie_split_states_e_t *state,
           const int             max_split_count)
{
    int bit = 0, rv = SHR_E_NONE;
    int split_type;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!trie || !pivot || !length || !split_node || max_count == 0 ||
        !state || max_split_count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (trie->child[0].child_node && trie->child[1].child_node) {
        bit = (trie->child[0].child_node->count >
               trie->child[1].child_node->count) ? 0 : 1;
    } else {
        bit = (trie->child[0].child_node) ? 0 : 1;
    }

    /* start building the pivot */
    rv = bcmptm_rm_alpm_key_append(mkl, pivot, length, trie->skip_addr,
                                   trie->skip_len);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    if (bpm) {
        uint32_t scratch = 0;
        rv = bcmptm_rm_alpm_bpm_append(mkl, bpm, &scratch, trie->bpm,
                                       trie->skip_len + 1);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }
    }
    split_type = trie_split_type(trie, trie->child[bit].child_node, *length,
                                 max_count, max_split_count, max_split_len,
                                 *state);

    if (split_type == SPLIT_FURTHER) {
        rv = bcmptm_rm_alpm_key_append(mkl, pivot, length, bit, 1);
        if (SHR_FAILURE(rv)) {
            SHR_ERR_EXIT(rv);
        }

        rv = trie_split(u, m, mkl, trie->child[bit].child_node,
                        pivot, length,
                        split_count, split_node,
                        child, max_count, max_split_len,
                        bpm, state, max_split_count);
    } else if (split_type == SPLIT_ABORT) {
        /* Simply do nothing, and return all the way to recursion entrance. */
    } else if ((TRIE_SPLIT_STATE_PAYLOAD_SPLIT == *state) &&
               (trie->type == INTERNAL)) {

        *state = TRIE_SPLIT_STATE_PAYLOAD_SPLIT_DONE;
    } else if (split_type == SPLIT_GOOD) {

        *split_node = trie;
        *split_count = trie->count;

        *state = TRIE_SPLIT_STATE_PRUNE_NODES;
        SHR_ERR_EXIT(rv);
    } else if (split_type == SPLIT_LEN_EXCEED) {

        if (trie->skip_len == 0) {
            /* trie->skip_len is 0, must split at this node */
            *split_node = trie;
            *split_count = trie->count;

            *state = TRIE_SPLIT_STATE_PRUNE_NODES;
        } else {
            /* split above this node, split the node in middle */
            rv = trie_split_middle_point(u, m, mkl, trie,
                                         pivot, length,
                                         split_count, split_node,
                                         max_split_len, bpm);
            *state = TRIE_SPLIT_STATE_PRUNE_NODES;
        }
        SHR_ERR_EXIT(rv);
    }

    /* free up internal nodes if applicable */
    switch(*state) {
    case TRIE_SPLIT_STATE_PAYLOAD_SPLIT_DONE:
        if (trie->type == PAYLOAD) {
            *state = TRIE_SPLIT_STATE_PRUNE_NODES;
            *split_node = trie;
            *split_count = trie->count;
        } else {
            /* shift the pivot to right to ignore this internal node */
            rv = bcmptm_rm_alpm_key_shift(mkl,
                                          pivot, *length, trie->skip_len + 1);
            TRIE_ASSERT_RETURN(*length >= (uint32_t)trie->skip_len + 1);
            *length -= (trie->skip_len + 1);
        }
        break;

    case TRIE_SPLIT_STATE_PRUNE_NODES:
        if (trie->count == *split_count) {
            /* if the split point has associate internal nodes they have to
             * be cleaned up */
            TRIE_ASSERT_RETURN(trie->type == INTERNAL);
            /* at most one child */
            TRIE_ASSERT_RETURN(
                !(trie->child[0].child_node && trie->child[1].child_node));
            shr_lmm_free(trie_internal_nodes[u][m], trie);
        } else {
            /* Child should not be modified so far */
            TRIE_ASSERT_RETURN(*child == NULL);
            trie->child[bit].child_node = NULL;
            trie->count -= *split_count;

            /* optimize more */
            bit = !bit; /* fuse with the other child if possible */
            if ((trie->type == INTERNAL) &&
                (trie->skip_len +
                 trie->child[bit].child_node->skip_len + 1 <= MAX_SKIP_LEN)) {
                *child = trie->child[bit].child_node;
                rv = trie_fuse_child(u, m, trie, bit);
                if (rv != SHR_E_NONE) {
                    *child = NULL;
                }
            }
            *state = TRIE_SPLIT_STATE_DONE;
        }
        break;

    case TRIE_SPLIT_STATE_DONE:
        /* adjust parent's count */
        TRIE_ASSERT_RETURN(*split_count > 0);
        TRIE_ASSERT_RETURN(trie->count >= *split_count);

        /* update the child pointer if child was pruned */
        if (*child != NULL) {
            trie->child[bit].child_node = *child;
            *child = NULL;
        }
        trie->count -= *split_count;
        break;

    default:
        break;
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     trie_merge
 * Purpose:
 *     merge or fuse the child trie with parent trie
 */
static int trie_merge(int u, int m,
                      uint32_t            mkl,
                      rm_alpm_trie_node_t *parent_trie,
                      rm_alpm_trie_node_t *child_trie,
                      uint32_t            *pivot,
                      uint32_t            length,
                      rm_alpm_trie_node_t **new_parent)
{
    int rv, child_count;
    rm_alpm_trie_node_t *child = NULL, clone;
    uint32_t bpm[ALPM_MAX_KEY_SIZE_WORDS] = {0};
    uint32_t child_pivot[ALPM_MAX_KEY_SIZE_WORDS] = {0};
    uint32_t child_length = 0;

    if (!parent_trie || length == 0 || !pivot
        || !new_parent || (length > mkl)) {
        return SHR_E_PARAM;
    }

    /*
     * to do merge, there is one and only one condition:
     * parent must cover the child
     */

    /*
     * child pivot could be an internal node, i.e., NOT_FOUND on search
     * so check the out child instead of rv.
     */
    trie_search(mkl,
                child_trie, pivot, length,
                &child, child_pivot, &child_length, 0, 1);

    /* The head of a bucket usually is the pivot of the bucket,
     * but for some cases, where the pivot is an INTERNAL node,
     * and it is fused with its child, then the pivot can no longer
     * be found, but we can still search a head. The head can be
     * payload (if this is the only payload head), or internal (if
     * two payload head coexist).
     */
    if (child == NULL) {
        return SHR_E_PARAM;
    }

    CLONE_TRIE_NODE(&clone, child);

    if (child->type == PAYLOAD && child->bpm) {
        KEY_BIT_SET(bpm, 1, mkl);
    }

    if (child != child_trie) {
        rv = trie_skip_node_free(u, m, mkl,
                                 child_trie, child_pivot, child_length);
        if (rv < 0) {
            return SHR_E_PARAM;
        }
    }

    /* Record the child count before being cleared */
    child_count = child->count;

    /* Clear the info before insert, mainly it is to prevent previous non-zero
     * count being erroneously included to calculation.
     */
    sal_memset(child, 0, sizeof(*child));

    /* merge happens on bucket trie, which usually does not need bpm */
    rv = trie_insert(u, m, mkl,
                     parent_trie, child_pivot, bpm, child_length, child,
                     new_parent, child_count);
    if (rv < 0) {
        return SHR_E_PARAM;
    }

    /*
     * child node, the inserted node, will be modified during insert,
     * and it must be a leaf node of the parent trie without any child.
     * The child node could be either payload or internal.
     */
    if (child->child[0].child_node || child->child[1].child_node) {
        return SHR_E_PARAM;
    }
    if (clone.type == INTERNAL) {
        child->type = INTERNAL;
    }
    child->child[0].child_node = clone.child[0].child_node;
    child->child[1].child_node = clone.child[1].child_node;

    return SHR_E_NONE;
}


/*
 * Function:
 *     trie_split2
 * Purpose:
 *     Split the trie into 2 at given split point
 *     prefix/length.
 * NOTE:
 *     key, key_len    -- The given prefix/length
 *     max_split_count -- The sub trie's max allowed count.
 */
static int
trie_split2(int u, int m,
            uint32_t               mkl,
            rm_alpm_trie_node_t    *trie,
            uint32_t               *key,
            uint32_t               key_len,
            uint32_t               *pivot,
            uint32_t               *pivot_len,
            uint32_t               *split_count,
            rm_alpm_trie_node_t    **split_node,
            rm_alpm_trie_node_t    **child,
            trie_split2_states_e_t *state,
            uint32_t               max_split_count)
{
    uint32_t lcp = 0;
    int bit = 0, rv = SHR_E_NONE;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!trie || !pivot || !pivot_len || !split_node
        || !state || max_split_count == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    /* start building the pivot */
    rv = bcmptm_rm_alpm_key_append(mkl,
                                   pivot, pivot_len,
                                   trie->skip_addr, trie->skip_len);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }

    LCP_LEN(lcp, mkl, key, key_len, trie->skip_addr, trie->skip_len);

    if (lcp == trie->skip_len) {
        if (key_len == lcp)  {
            if (trie->count < max_split_count) {
                *split_node = trie;
                *split_count = trie->count;
                *state = TRIE_SPLIT2_STATE_PRUNE_NODES;
                SHR_EXIT();
            } else {
                *split_node = trie;
                *split_count = trie->count;
                /* *state = TRIE_SPLIT2_STATE_PRUNE_NODES; */
                SHR_EXIT();
            }
        }
        if (key_len > lcp) {
            bit = KEY_BRANCH_BIT(key, mkl, key_len - lcp);

            /* based on next bit branch left or right */
            if (trie->child[bit].child_node) {
                /* we can not split at this node, keep searching, it's better to
                 * split at longer pivot
                 */
                rv = bcmptm_rm_alpm_key_append(mkl, pivot, pivot_len, bit, 1);
                if (SHR_FAILURE(rv)) {
                    SHR_ERR_EXIT(rv);
                }
                rv = trie_split2(u, m, mkl, trie->child[bit].child_node,
                                 key, key_len - lcp - 1,
                                 pivot, pivot_len, split_count,
                                 split_node, child, state,
                                 max_split_count);
                if (SHR_FAILURE(rv)) {
                    SHR_ERR_EXIT(rv);
                }
            }
        }
    }

    /* free up internal nodes if applicable */
    switch(*state) {
    case TRIE_SPLIT2_STATE_NONE:     /* fail to split */
        break;

    case TRIE_SPLIT2_STATE_PRUNE_NODES:
        if (trie->count == *split_count) {
            /* if the split point has associate internal nodes they have to
             * be cleaned up */
            TRIE_ASSERT_RETURN(trie->type == INTERNAL);
            /* at most one child */
            TRIE_ASSERT_RETURN(
                !(trie->child[0].child_node && trie->child[1].child_node));
            /* at least one child */
            TRIE_ASSERT_RETURN(trie->child[0].child_node || trie->child[1].child_node);
            shr_lmm_free(trie_internal_nodes[u][m], trie);
        } else {
            /* Child should not be modified so far. */
            TRIE_ASSERT_RETURN(*child == NULL);
            trie->child[bit].child_node = NULL;
            trie->count -= *split_count;

            /* optimize more */
            bit = !bit;     /* fuse with the other child if possible */
            if ((trie->type == INTERNAL) &&
                (trie->skip_len +
                 trie->child[bit].child_node->skip_len + 1 <= MAX_SKIP_LEN)) {

                *child = trie->child[bit].child_node;
                rv = trie_fuse_child(u, m, trie, bit);
                if (rv != SHR_E_NONE) {
                    *child = NULL;
                }
            }
            *state = TRIE_SPLIT2_STATE_DONE;
        }
        break;

    case TRIE_SPLIT2_STATE_DONE:
        /* adjust parent's count */
        TRIE_ASSERT_RETURN(*split_count > 0);
        TRIE_ASSERT_RETURN(trie->count >= *split_count);

        /* update the child pointer if child was pruned */
        if (*child != NULL) {
            trie->child[bit].child_node = *child;
            *child = NULL;
        }
        trie->count -= *split_count;
        break;

    default:
        break;
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *     trie_traverse_propagate_prefix
 * Purpose:
 *     calls back applicable payload object is affected by prefix updates
 * NOTE:
 *     propagation stops once any callback funciton return something other than
 *     SHR_E_NONE
 *     tcam propagation code should return !SHR_E_NONE so that callback only
 *     happen once.
 *     other propagation code should always return SHR_E_NONE so that callback
 *     will happen on all pivot.
 */
static int
trie_traverse_propagate_prefix(rm_alpm_trie_node_t         *trie,
                               rm_alpm_trie_propagate_cb_f cb,
                               rm_alpm_trie_bpm_cb_info_t  *cb_info,
                               uint32_t                    mask)
{
    int rv = SHR_E_NONE, index = 0;

    if (!trie || !cb || !cb_info) {
        return SHR_E_PARAM;
    }

    /* Base condition: could be removed for performance */
    if (trie->bpm & mask) {
        return SHR_E_NONE;
    }

    /* call back the payload object if applicable */
    if (trie->type == PAYLOAD) {
        rv = cb(trie, cb_info);
        if (SHR_FAILURE(rv)) {
            /* callback stops once any callback function not returning
               SHR_E_NONE */
            return rv;
        }
    }

    /* Traverse stops once recursion routine not returning
       SHR_E_NONE */
    for (index = 0; index < MAX_CHILD && SHR_SUCCESS(rv); index++) {
        if (trie->child[index].child_node
            && trie->child[index].child_node->bpm == 0) {
            /* coverity[large_shift : FALSE] */
            rv = trie_traverse_propagate_prefix(trie->child[index].child_node,
                                                cb, cb_info, MASK(32));
        }
    }

    return rv;
}


/*
 * A simpler version for bpm-unaware trie.
 * This approach leaves the bpm info management to users, instead of maitaining
 * it in the trie algorithm itself.
 * This has an advantage if user maintains the BPM anyway.
 */
static int
trie_propagate_prefix2(uint32_t                    mkl,
                       rm_alpm_trie_node_t         *trie,
                       uint32_t                    *pfx,
                       uint32_t                    len,
                       trie_propagate_op_t         op,
                       rm_alpm_trie_propagate_cb_f cb,
                       rm_alpm_trie_bpm_cb_info_t  *cb_info)
{
    int rv = SHR_E_NONE;
    uint32_t bit = 0, lcp = 0;

    if (!trie || (len && trie->skip_len && !pfx) ||
        (len > mkl) || !cb || !cb_info) {
        return SHR_E_PARAM;
    }

    lcp = bcmptm_rm_alpm_lcplen(mkl, pfx, len, trie->skip_addr, trie->skip_len);
    switch (trie_match_type(len, lcp, trie->skip_len)) {
    case FULL_MATCH_AND_MORE:
        /* Fully matched and more bits to check, go down the trie */
        bit = bcmptm_rm_alpm_key_bits_get(mkl, pfx, len - lcp, 1, 0);
        if (trie->child[bit].child_node) {
            rv = trie_propagate_prefix2(mkl, trie->child[bit].child_node,
                                        pfx, len - lcp - 1, op, cb, cb_info);
        }
        break;
    case EXACT_MATCH:
    case OUT_MATCH:
        /*
         * Given pfx exactly matched or covers trie node, this is the
         * point to propagate.
         */
        rv = trie_traverse_propagate_prefix(trie, cb,
                                            cb_info, 0);
        if (SHR_E_LIMIT == rv) {/* A special err code to return early */
            rv = SHR_E_NONE;
        }
        break;
    case PARTIAL_MATCH:
    default:
        /* Not applicable for any propagation */
        break;
    }

    return rv;
}


/*
 * Function:
 *     trie_propagate_prefix
 * Purpose:
 *  Propogate prefix BPM. If the propogation starts from intermediate pivot on
 *  the trie, then the prefix length has to be appropriately adjusted or else
 *  it will end up with ill updates.
 *  Assumption: the prefix length is adjusted as per trie node on which
 *              starts from.
 *  If node == head node then adjust is none
 *     node == pivot, then prefix length = org len - pivot len
 */
static int
trie_propagate_prefix(uint32_t                    mkl,
                      rm_alpm_trie_node_t         *trie,
                      uint32_t                    *pfx,
                      uint32_t                    len,
                      trie_propagate_op_t         op,
                      rm_alpm_trie_propagate_cb_f cb,
                      rm_alpm_trie_bpm_cb_info_t  *cb_info)
{
    int rv = SHR_E_NONE;
    uint32_t bit = 0, lcp = 0;

    if (!trie || (len && trie->skip_len && !pfx) ||
        (len > mkl) || !cb || !cb_info) {
        return SHR_E_PARAM;
    }

    /* BPM bit maps has to be updated before propagation */
    lcp = bcmptm_rm_alpm_lcplen(mkl, pfx, len, trie->skip_addr, trie->skip_len);

    switch (trie_match_type(len, lcp, trie->skip_len)) {
    case FULL_MATCH_AND_MORE:
        /* fully matched and more bits to check, go down the trie */
        bit = bcmptm_rm_alpm_key_bits_get(mkl, pfx, len - lcp, 1, 0);
        if (trie->child[bit].child_node) {
            rv = trie_propagate_prefix(mkl, trie->child[bit].child_node,
                                       pfx, len - lcp - 1, op, cb, cb_info);
        }
        break;

    case EXACT_MATCH:
    case OUT_MATCH:
        /* given pfx exactly matched or covers trie node, this is the
         * point to propagate.
         */
        /* pfx is <= trie skip len */
        if (propagate_debug || LOG_CHECK_INFO(BSL_LOG_MODULE)) {
            switch (op) {
            case PROPOGATE_INSERT:
                TRIE_ASSERT(!BITGET(trie->bpm, trie->skip_len - len));
                break;
            case PROPOGATE_UPDATE:
            case PROPOGATE_DELETE:
            default:
                TRIE_ASSERT(BITGET(trie->bpm, trie->skip_len - len));
                break;
            }
        }
        if (op == PROPOGATE_DELETE) {
            BITCLR(trie->bpm, trie->skip_len - len);
        }

        /* Update bit map and propagate if applicable:
         * there is no longer bpm than this new prefix.
 */
        if ((trie->bpm & BITMASK(trie->skip_len - len)) == 0) {
            rv = trie_traverse_propagate_prefix(trie, cb,
                                                cb_info,
                                                BITMASK(trie->skip_len - len));
            if (SHR_E_LIMIT == rv) {
                rv = SHR_E_NONE;
            }
        }

        if (op == PROPOGATE_INSERT && SHR_SUCCESS(rv)) {
            /* this is the case where child bit is the new prefix */
            BITSET(trie->bpm, trie->skip_len - len);
        }
        break;
    case PARTIAL_MATCH:
    default:
        /* not applicable for any propagation */
        break;
    }

    return rv;
}

/*
 * Function:
 *     trie_propagate_prefix_validate
 * Purpose:
 *  Validate that the provided prefix is valid for propagation.
 *  The added prefix which was member of a shorter pivot's domain
 *  must never be more specific than another pivot encounter if any
 *  in the path.
 */
static int
trie_propagate_prefix_validate(uint32_t            mkl,
                               rm_alpm_trie_node_t *trie,
                               uint32_t            *pfx,
                               uint32_t            len)
{
    uint32_t lcp = 0, bit = 0;

    if (!trie_validate) {
        return SHR_E_NONE;
    }
    if (!trie || (len && trie->skip_len && !pfx)) {
        return SHR_E_PARAM;
    }

    if (len == 0) {
        return SHR_E_NONE;
    }

    lcp = bcmptm_rm_alpm_lcplen(mkl,
                                pfx, len, trie->skip_addr, trie->skip_len);

    if (lcp == trie->skip_len) {
        if (PAYLOAD == trie->type) {
            return SHR_E_PARAM;
        }

        if (len == lcp) {
            return SHR_E_NONE;
        }

        bit = bcmptm_rm_alpm_key_bits_get(mkl, pfx, len - lcp, 1, 0);
        if (!trie->child[bit].child_node) {
            return SHR_E_NONE;
        }

        return trie_propagate_prefix_validate(mkl,
                                              trie->child[bit].child_node,
                                              pfx, len - 1 - lcp);
    }

    return SHR_E_NONE;
}

/*
 * Function:
 *     trie_init_propagate_info
 * Purpose:
 *     Initialize rm_alpm_trie_bpm_cb_info_t.
 */
static int
trie_init_propagate_info(uint32_t                    *pfx,
                         uint32_t                    len,
                         rm_alpm_trie_propagate_cb_f cb,
                         rm_alpm_trie_bpm_cb_info_t  *cb_info)
{
    cb_info->pfx = pfx;
    cb_info->len = len;
    return SHR_E_NONE;
}


/*
 * Function:
 *     trie_clone
 * Purpose:
 *     Recursive routine to clone a trie
 */
static int
trie_clone(rm_alpm_trie_node_t              *trie,
           shr_lmm_hdl_t                    clone_hdl,
           int                              bit,
           rm_alpm_trie_clone_callback_f    cb,
           void                             *user_data,
           rm_alpm_trie_node_t              *pnode,
           rm_alpm_trie_t                   *clone_trie
           )
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *lc, *rc;
    rm_alpm_trie_node_t *clone_node = NULL;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    if (trie == NULL) {
        SHR_EXIT();
    } else {
        lc = trie->child[0].child_node;
        rc = trie->child[1].child_node;

        /* assert(trie); */

        if (trie->type == PAYLOAD) {
            rv = cb(trie, &clone_node, user_data);
            SHR_IF_ERR_EXIT(rv);
        } else {
            clone_node = shr_lmm_alloc(clone_hdl);
            SHR_NULL_CHECK(clone_node, SHR_E_MEMORY);
        }

        sal_memcpy(clone_node, trie, sizeof(rm_alpm_trie_node_t));

        if (clone_trie) { /* clone head */
            /* assert(clone_trie); */
            clone_trie->trie = clone_node;
        } else { /* clone body */
            /* assert(pnode); */
            pnode->child[bit].child_node = clone_node;
        }

        pnode = clone_node;
    }


    if (SHR_SUCCESS(rv)) {
        rv = trie_clone(lc, clone_hdl, 0, cb, user_data, pnode, NULL);
    }
    if (SHR_SUCCESS(rv)) {
        rv = trie_clone(rc, clone_hdl, 1, cb, user_data, pnode, NULL);
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     trie_destroy
 * Purpose:
 *     Destroys an entire trie.
 */
static void
trie_destroy(int u, int m, rm_alpm_trie_t *trie)
{
    shr_lmm_free(all_tries[u][m], trie);
}


/* Compare whether two tries are identical */
static int
trie_compare(rm_alpm_trie_node_t              *ptrie,
             rm_alpm_trie_node_t              *trie,
             int                              bit,
             rm_alpm_trie_traverse_states_e_t *state,
             rm_alpm_trie_node_t              *ptrie_cmp,
             rm_alpm_trie_node_t              *trie_cmp,
             int                              *cmp_result)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *lc, *rc;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (trie == NULL) {
        SHR_EXIT();
    } else {
        /* make the trie delete safe */
        lc = trie->child[0].child_node;
        rc = trie->child[1].child_node;

        TRIE_ASSERT_RETURN(trie && state);

        if (ptrie == NULL) { /* compare head */
            trie_cmp = trie_cmp;
        } else { /* compare body */
            trie_cmp = ptrie_cmp->child[bit].child_node;
        }

        /* When traverse against one trie, there is a possibility the other
         * is a super set of this one. To rule out that possibility, compare
         * the child pointer as well.
         */
        if (/* trie_cmp->skip_len != trie->skip_len ||
               (BITMASK(trie->skip_len) & trie_cmp->skip_addr) !=
               (BITMASK(trie->skip_len) & trie->skip_addr) || */
            trie_cmp->type != trie->type ||
            trie_cmp->count != trie->count ||
            trie_cmp->bpm != trie->bpm ||
            !trie_cmp->child[0].child_node != !trie->child[0].child_node ||
            !trie_cmp->child[1].child_node != !trie->child[1].child_node) {
            *cmp_result = 1;
            /* stop traverse on unequal */
            *state = TRAVERSE_STATE_DONE;
        }

        /* In case of v6, the skip len not equal does not mean the two
         * tries are different, thus we print it and let user be aware
         * and then continue.
         */
        if (trie_cmp->skip_len != trie->skip_len) {
            LOG_CLI((BSL_META("AWARE:len %d - %d   addr 0x%x - 0x%x\n"),
                     trie_cmp->skip_len, trie->skip_len,
                     trie_cmp->skip_addr, trie->skip_addr));
        }
        ptrie_cmp = trie_cmp;

        TRIE_TRAVERSE_STOP(*state, rv);
        /* make the ptrie delete safe */
        if (*state != TRAVERSE_STATE_DELETED) {
            ptrie = trie;
        }
    }

    if (SHR_SUCCESS(rv)) {
        rv = trie_compare(ptrie, lc, 0, state, ptrie_cmp, NULL, cmp_result);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SHR_SUCCESS(rv)) {
        rv = trie_compare(ptrie, rc, 1, state, ptrie_cmp, NULL, cmp_result);
    }
    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

/*
 * Function:
 *     bcmptm_rm_alpm_trie_traverse
 * Purpose:
 *     Traverse the trie & call the application callback with user data
 */
int bcmptm_rm_alpm_trie_traverse(int                             u, int m,
                                 rm_alpm_trie_node_t             *trie,
                                 rm_alpm_trie_callback_f         cb,
                                 void                            *user_data,
                                 rm_alpm_trie_traverse_order_e_t order)
{
    if (order < TRAVERSE_PREORDER ||
        order >= TRAVERSE_ORDER_MAX || !cb) {
        return SHR_E_PARAM;
    }


    return trie_traverse(u, m, trie, cb, user_data, order);
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_traverse
 * Purpose:
 *     Traverse the trie & call the application callback with user data
 */
int bcmptm_rm_alpm_trie_traverse_get_next(int u, int m, bool get_first,
                                          rm_alpm_trie_node_t *trie,
                                          shr_dq_t *queue,
                                          rm_alpm_trie_callback_f cb,
                                          void *user_data,
                                          void **payload)
{
    return trie_bfs_traverse_get_next(u, m, get_first, trie, queue, cb, user_data, payload);
}
/*
 * Function:
 *     bcmptm_rm_alpm_trie_traverse2
 * Purpose:
 *     Traverse the trie (PAYLOAD) & call the extended application callback
 *     which has current node's PAYLOAD parent node with user data.
 */
int bcmptm_rm_alpm_trie_traverse2(rm_alpm_trie_node_t             *trie,
                                  rm_alpm_trie_callback_ext_f     cb,
                                  void                            *user_data,
                                  rm_alpm_trie_traverse_order_e_t order)
{
    rm_alpm_trie_traverse_states_e_t state = TRAVERSE_STATE_NONE;

    if (order < TRAVERSE_PREORDER ||
        order >= TRAVERSE_ORDER_MAX || !cb) {
        return SHR_E_PARAM;
    }
    return trie_traverse2(trie, cb, user_data, order, &state);
}
/*
 * Function:
 *     bcmptm_rm_alpm_trie_repartition
 * Purpose:
 *     Traverse the trie (PAYLOAD) & call the extended application callback
 *     which has current node's PAYLOAD parent node with user data.
 */
int bcmptm_rm_alpm_trie_repartition(rm_alpm_trie_t                  *trie,
                                    rm_alpm_trie_repart_callback_f  cb,
                                    void                            *user_data,
                                    rm_alpm_trie_traverse_order_e_t order)
{
    rm_alpm_trie_traverse_states_e_t state = TRAVERSE_STATE_NONE;

    if (order < TRAVERSE_PREORDER ||
        order >= TRAVERSE_ORDER_MAX || !cb) {
        return SHR_E_PARAM;
    }

    if (repart_debug) {
        repart_trie = trie;
        LOG_CLI((BSL_META("=====================\n")));
        bcmptm_rm_alpm_trie_dump(repart_trie, 0, 0);
    }
    if (trie == NULL) {
        return SHR_E_NONE;
    } else {
        return trie_repartition(NULL, trie->trie, &state,
                                cb, user_data, NULL, 0, NULL, NULL);
    }
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_iter_get_first
 * Purpose:
 *     Traverse the trie & return pointer to first payload node
 */
int bcmptm_rm_alpm_trie_iter_get_first(rm_alpm_trie_t      *trie,
                                       rm_alpm_trie_node_t **payload)
{
    int rv = SHR_E_EMPTY;

    if (!trie || !payload) {
        return SHR_E_PARAM;
    }

    if (trie && trie->trie) {
        *payload = NULL;
        return trie_preorder_iter_get_first(trie->trie, payload);
    }

    return rv;
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_iter_get_first2
 * Purpose:
 *     Traverse the trie & return pointer to first non-skip node
 */
int bcmptm_rm_alpm_trie_iter_get_first2(rm_alpm_trie_node_t *trie,
                                         rm_alpm_trie_node_t **node)
{

    if (!trie || !node) {
        return SHR_E_PARAM;
    }

    *node = NULL;
    return trie_preorder_iter_get_first2(trie, node);

}

int
bcmptm_rm_alpm_trie_swap(rm_alpm_trie_t *trie_dst,
                         rm_alpm_trie_t *trie_src)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(trie_src, SHR_E_PARAM);
    SHR_NULL_CHECK(trie_dst, SHR_E_PARAM);
    trie_dst->trie = trie_src->trie;
    trie_src->trie = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_dump
 * Purpose:
 *     Dumps the trie pre-order [root|left|child]
 */
int bcmptm_rm_alpm_trie_dump(rm_alpm_trie_t          *trie,
                             rm_alpm_trie_callback_f cb,
                             void                    *user_data)
{
    uint32_t key[ALPM_MAX_KEY_SIZE_WORDS] = {0};
    uint32_t len = 0;

    if (trie->trie) {
        return trie_dump(trie->trie, cb, user_data, 0, key, len, trie->mkl);
    } else {
        return SHR_E_PARAM;
    }
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_dump
 * Purpose:
 *     Dumps the trie pre-order [root|left|child]
 */
int bcmptm_rm_alpm_trie_dump2(rm_alpm_trie_node_t *trie,
                              rm_alpm_trie_callback_f cb,
                              void *user_data,
                              uint32_t mkl)
{
    uint32_t key[ALPM_MAX_KEY_SIZE_WORDS] = {0};
    uint32_t len = 0;


    if (trie) {
        return trie_dump(trie, cb, user_data, 0, key, len, mkl);
    } else {
        return SHR_E_PARAM;
    }
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_search
 * Purpose:
 *     Search the given trie for exact match of provided prefix/length
 *     If dump is set to 1 it traces the path as it traverses the trie
 */
int bcmptm_rm_alpm_trie_search(rm_alpm_trie_t      *trie,
                               uint32_t            *key,
                               uint32_t            length,
                               rm_alpm_trie_node_t **payload)
{
    if (trie->trie) {
        return trie_fast_search(trie->mkl,
                                 trie->trie, key, length, payload);
    } else {
        return SHR_E_NOT_FOUND;
    }
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_search_verbose
 * Purpose:
 *     Search the given trie for provided prefix/length
 *     If dump is set to 1 it traces the path as it traverses the trie
 */
int bcmptm_rm_alpm_trie_search_verbose(rm_alpm_trie_t      *trie,
                                       uint32_t            *key,
                                       uint32_t            length,
                                       rm_alpm_trie_node_t **payload,
                                       uint32_t            *result_key,
                                       uint32_t            *result_len)
{
    if (trie->trie) {
        return trie_search(trie->mkl,
                           trie->trie, key, length, payload,
                           result_key, result_len, 0, 0);
    } else {
        return SHR_E_NOT_FOUND;
    }
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_lpm
 * Purpose:
 *     Find the longest prefix matched with given prefix
 */
int bcmptm_rm_alpm_trie_find_lpm(rm_alpm_trie_t      *trie,
                                 uint32_t            *key,
                                 uint32_t            length,
                                 rm_alpm_trie_node_t **payload,
                                 uint32_t            *lpm_pfx,
                                 uint32_t            *lpm_len)
{
    int rv = SHR_E_NONE;
    uint32_t cut = 0;

    *payload = NULL;

    if (trie->trie) {
        if (lpm_len) {
            *lpm_len = 0;
        }
        if (lpm_pfx) {
            sal_memset(lpm_pfx, 0,
                       sizeof(uint32_t) * ALPM_MAX_KEY_SIZE_WORDS);
        }
        rv = trie_find_lpm(trie->mkl,
                           trie->trie, key, length, payload,
                           NULL, NULL, 0, lpm_pfx, lpm_len, &cut);
        if (*payload || (rv != SHR_E_NONE)) {
            if (cut && lpm_pfx && lpm_len && (rv == SHR_E_NONE)) {
                rv = bcmptm_rm_alpm_key_shift(trie->mkl,
                                              lpm_pfx, *lpm_len, cut);
                *lpm_len -= cut;
            }
            return rv;
        }
    }

    return SHR_E_NOT_FOUND;
}



/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_lpm2
 * Purpose:
 *     Find the longest prefix matched with given prefix
 */
int bcmptm_rm_alpm_trie_find_lpm2(rm_alpm_trie_t      *trie,
                                  uint32_t            *key,
                                  uint32_t            length,
                                  rm_alpm_trie_node_t **payload)
{
    int rv = SHR_E_NONE;

    *payload = NULL;

    if (trie->trie) {
        rv = trie_find_lpm(trie->mkl,
                           trie->trie, key, length, payload,
                           NULL, NULL, 1, NULL, NULL, NULL);
        if (*payload || (rv != SHR_E_NONE)) {
            return rv;
        }
    }

    return SHR_E_NOT_FOUND;
}


/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_pm
 * Purpose:
 *     Find the prefix matched nodes with given prefix and callback
 *     with specified callback funtion and user data
 */
int bcmptm_rm_alpm_trie_find_pm(rm_alpm_trie_t          *trie,
                                uint32_t                *key,
                                uint32_t                length,
                                rm_alpm_trie_callback_f cb,
                                void                    *user_data)
{

    if (trie->trie) {
        return trie_find_lpm(trie->mkl,
                             trie->trie, key, length, NULL,
                             cb, user_data, 0, NULL, NULL, NULL);
    }

    return SHR_E_NONE;
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_find_prefix_bpm
 * Purpose:
 *    Given a key/length return the Best prefix match length
 *    key/bpm_pfx_len will be the BPM for the key/length
 *    using the bpm info in the trie database
 */
int bcmptm_rm_alpm_trie_find_prefix_bpm(rm_alpm_trie_t *trie,
                                        uint32_t       *key,
                                        uint32_t       length,
                                        uint32_t       *bpm_pfx_len)
{
    /* Return: SHR_E_EMPTY is not bpm bit is found */
    int rv = SHR_E_EMPTY, bpm = 0;

    if (!trie || !key || !bpm_pfx_len ) {
        return SHR_E_PARAM;
    }

    bpm = -1;  /* negative as a flag indicating no bpm set yet */
    if (trie->trie) {
        rv = trie_find_bpm(trie->mkl, trie->trie, key, length, &bpm);

        if (SHR_SUCCESS(rv)) {
            /* all bpm bits are 0 */
            *bpm_pfx_len = (bpm < 0) ? 0 : (uint32_t)bpm;
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_bpm_mask_get
 * Purpose:
 *     Get the bpm mask of target key. This key is already in the trie.
 */
int bcmptm_rm_alpm_trie_bpm_mask_get(rm_alpm_trie_t *trie,
                                     uint32_t       *key,
                                     uint32_t       length,
                                     uint32_t       *bpm_mask)
{
    int rv = SHR_E_NONE;

    if (!trie || !key || !bpm_mask ) {
        return SHR_E_PARAM;
    }

    if (trie->trie) {
        rv = trie_bpm_mask_get(trie->mkl, trie->trie, key, length, bpm_mask);
    }
    return rv;
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_insert
 * Purpose:
 *     Inserts provided prefix/length in to the trie
 */
int bcmptm_rm_alpm_trie_insert(int u, int m,
                               rm_alpm_trie_t      *trie,
                               uint32_t            *key,
                               uint32_t            *bpm,
                               uint32_t            length,
                               rm_alpm_trie_node_t *payload)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *child = NULL;

    if (!trie) {
        return SHR_E_PARAM;
    }

    if (trie->trie == NULL) {
        rv = trie_skip_node_alloc(u, m, trie->mkl,
                                  &trie->trie, key, bpm, length, length,
                                  payload, 1);
    } else {
        rv = trie_insert(u, m, trie->mkl,
                         trie->trie, key, bpm, length, payload, &child, 1);
        if (child) { /* chande the old child pointer to new child */
            trie->trie = child;
        }
    }

    return rv;
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_delete
 * Purpose:
 *     Deletes provided prefix/length in to the trie
 */
int bcmptm_rm_alpm_trie_delete(int u, int m,
                               rm_alpm_trie_t      *trie,
                               uint32_t            *key,
                               uint32_t            length,
                               rm_alpm_trie_node_t **payload)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *child = NULL;

    if (trie->trie) {
        rv = trie_delete(u, m, trie->mkl,
                         trie->trie, key, length, payload, &child);
        if (rv == SHR_E_BUSY) {
            /* the head node of trie was deleted, reset trie pointer to null */
            trie->trie = NULL;
            rv = SHR_E_NONE;
        } else if (rv == SHR_E_NONE && child != NULL) {
            trie->trie = child;
        }
    } else {
        rv = SHR_E_NOT_FOUND;
    }
    return rv;
}

/*
 * Function:
 *     bcmptm_rm_alpm_trie_split
 * Purpose:
 *     Split the trie into 2 based on optimum pivot
 * Note:
 *     we need to make sure the length is shorter than
 *     the max_split_len (for capacity optimization) if
 *     possible. We should ignore the max_split_len
 *     if that will result into trie not spliting
 */
int bcmptm_rm_alpm_trie_split(int u, int m,
                              rm_alpm_trie_t      *trie,
                              const uint32_t      max_split_len,
                              uint32_t            *pivot,
                              uint32_t            *length,
                              rm_alpm_trie_node_t **split_trie_root,
                              rm_alpm_trie_node_t **split_trie_node,
                              uint32_t            *bpm,
                              uint32_t            payload_node_split,
                              const int           max_split_count)
{
    int rv = SHR_E_NONE;
    uint32_t split_count = 0, max_count = 0;
    rm_alpm_trie_node_t *child = NULL, *node = NULL, clone;
    trie_split_states_e_t state = TRIE_SPLIT_STATE_NONE;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!trie || !pivot || !length || !split_trie_root) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *length = 0;

    if (trie->trie) {

        if (payload_node_split) {
            state = TRIE_SPLIT_STATE_PAYLOAD_SPLIT;
        }

        max_count = trie->trie->count;

        sal_memset(pivot, 0, sizeof(uint32_t) * BITS2WORDS(trie->mkl));
        if (bpm) {
            sal_memset(bpm, 0, sizeof(uint32_t) * BITS2WORDS(trie->mkl));
        }
        rv = trie_split(u, m, trie->mkl,
                        trie->trie, pivot, length, &split_count,
                        split_trie_root, &child,
                        max_count, max_split_len,
                        bpm, &state, max_split_count);

        if (SHR_SUCCESS(rv) && (TRIE_SPLIT_STATE_DONE == state)) {
            /* adjust parent's count */
            TRIE_ASSERT_RETURN(split_count > 0);
            if (trie->trie == NULL) {
                rm_alpm_trie_t *c1, *c2; /* dummy head */
                bcmptm_rm_alpm_trie_create(u, m, trie->mkl, &c1);
                bcmptm_rm_alpm_trie_create(u, m, trie->mkl, &c2);
                c1->trie = child;
                c2->trie = *split_trie_root;
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("dumping the 2 child trees\n")));
                bcmptm_rm_alpm_trie_dump(c1, 0, 0);
                bcmptm_rm_alpm_trie_dump(c2, 0, 0);
            }
            /* update the child pointer if child was pruned */
            if (child != NULL) {
                trie->trie = child;
            }
            TRIE_ASSERT_RETURN(trie->trie->count >= split_count
                        || (*split_trie_root)->count >= split_count);

            sal_memcpy(&clone, *split_trie_root, sizeof(rm_alpm_trie_node_t));
            child = *split_trie_root;

            /* take advantage of thie function by passing in internal or payload
             * node whatever is the new root. If internal the function assumed
             * it as payload node & changes type.
             * But this method is efficient to reuse the last internal or
             * payload node possible to implant the new pivot
             */
            rv = trie_skip_node_alloc(u, m, trie->mkl, &node, pivot, NULL,
                                      *length, *length,
                                      child, child->count);

            if (SHR_SUCCESS(rv)) {
                if (clone.type == INTERNAL) {
                    child->type = INTERNAL; /* since skip alloc would have reset
                                               it to payload */
                }
                child->child[0].child_node = clone.child[0].child_node;
                child->child[1].child_node = clone.child[1].child_node;
                *split_trie_root = node;
                if (split_trie_node) {
                    *split_trie_node = child;
                }
            }
        } else if (payload_node_split) {
            LOG_INFO(BSL_LOG_MODULE,
                      (BSL_META("!! Failed to split the trie error:%d"\
                                " state: %d !!\n"), rv, state));
            rv = SHR_E_NOT_FOUND;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("!! Failed to split the trie error:%d"\
                                " state: %d !!\n"), rv, state));
        }
    } else {
        rv = SHR_E_PARAM;
    }

    SHR_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();
}


/*
 * Function:
 *     bcmptm_rm_alpm_trie_merge
 * Purpose:
 *     merge or fuse the child trie with parent trie.
 */
int bcmptm_rm_alpm_trie_merge(int u, int m,
                              rm_alpm_trie_t      *parent_trie,
                              rm_alpm_trie_node_t *child_trie,
                              uint32_t            *child_pivot,
                              uint32_t            length)
{
    int rv = SHR_E_NONE;
    rm_alpm_trie_node_t *child = NULL;

    if (!parent_trie) {
        return SHR_E_PARAM;
    }

    if (!child_trie) {
        return SHR_E_NONE;
    }

    if (parent_trie->trie == NULL) {
        parent_trie->trie = child_trie;
    } else {
        rv = trie_merge(u, m, parent_trie->mkl,
                        parent_trie->trie, child_trie, child_pivot, length,
                        &child);
        if (child) {
            /* The parent head can be changed if the new payload generates a
             * new internal node, which then becomes the new head.
             */
            parent_trie->trie = child;
        }
    }

    return rv;
}



/*
 * Function:
 *     bcmptm_rm_alpm_trie_split2
 * Purpose:
 *     Split the trie at given prefix/length.
 *     Basically this is a reverse of bcmptm_rm_alpm_trie_merge.
 */

int bcmptm_rm_alpm_trie_split2(int u, int m,
                               rm_alpm_trie_t      *trie,
                               uint32_t            *key,
                               uint32_t            key_len,
                               uint32_t            *pivot,
                               uint32_t            *pivot_len,
                               rm_alpm_trie_node_t **split_trie_root)
{
    int rv = SHR_E_NONE;
    uint32_t split_count = 0;
    rm_alpm_trie_node_t *child = NULL, *node = NULL, clone;
    trie_split2_states_e_t state = TRIE_SPLIT2_STATE_NONE;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (!trie || (key_len && !key) || !pivot || !pivot_len || !split_trie_root) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *split_trie_root = NULL;
    *pivot_len = 0;

    if (trie->trie) {
        rm_alpm_trie_node_t *payload;
        sal_memset(pivot, 0, sizeof(uint32_t) * BITS2WORDS(trie->mkl));
        payload = shr_lmm_alloc(trie_internal_nodes[u][m]);
        /* Insert an internal node if the exact key does not exist */
        rv = trie_split2_insert(trie->mkl, trie->trie, key, NULL,
                                key_len, payload, &child, 0);
        if (SHR_SUCCESS(rv)) {
            payload->type = INTERNAL;
        } else {
            shr_lmm_free(trie_internal_nodes[u][m], payload);
            if (rv == SHR_E_NOT_FOUND) { /* no error msg */
                SHR_ERR_EXIT(rv);
            } else if (rv != SHR_E_EXISTS && rv != SHR_E_NONE) {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        if (child) { /* chande the old child pointer to new child */
            trie->trie = child;
        }
        child = NULL;
        rv = trie_split2(u, m, trie->mkl, trie->trie, key, key_len, pivot, pivot_len,
                         &split_count, split_trie_root, &child, &state,
                         trie->trie->count);

        if (SHR_SUCCESS(rv) && (TRIE_SPLIT2_STATE_DONE == state)) {
            TRIE_ASSERT_RETURN(split_count > 0);
            TRIE_ASSERT_RETURN(*split_trie_root);
            TRIE_ASSERT_RETURN(*pivot_len == key_len);

            /* update the child pointer if child was pruned */
            if (child != NULL) {
                trie->trie = child;
            }

            sal_memcpy(&clone, *split_trie_root, sizeof(rm_alpm_trie_node_t));
            child = *split_trie_root;

            /* take advantage of thie function by passing in internal or payload
             * node whatever is the new root. If internal the function assumed
             * it as payload node & changes type.
             * But this method is efficient to reuse the last internal or
             * payload node possible to implant the new pivot
             */
            rv = trie_skip_node_alloc(u, m, trie->mkl, &node, pivot, NULL,
                                      *pivot_len, *pivot_len,
                                      child, child->count);

            if (SHR_SUCCESS(rv)) {
                if (clone.type == INTERNAL) {
                    child->type = INTERNAL; /* since skip alloc would have reset
                                               it to payload */
                }
                child->child[0].child_node = clone.child[0].child_node;
                child->child[1].child_node = clone.child[1].child_node;
                *split_trie_root = node;
            }
            TRIE_ASSERT(*split_trie_root);
        } else if (SHR_SUCCESS(rv) && (split_count == trie->trie->count)) {
            /* take all */
            *split_trie_root = trie->trie;
            trie->trie = NULL;
        } else {
            LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META("Failed to split2 the trie error:%d state: %d " \
                               "split_trie_root: %p !!!\n"),
                      rv, state, (void*)*split_trie_root));
        }
    } else {
        rv = SHR_E_PARAM;
    }

    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}



/*
 * Function:
 *     bcmptm_rm_alpm_trie_pivot_propagate_prefix
 * Purpose:
 *     Propogate prefix BPM from a given pivot.
 * Input:
 *     trie -- usually pivot trie
 *     add  -- 0-del/1-add
 */
int bcmptm_rm_alpm_trie_pivot_propagate(uint32_t mkl,
                                        rm_alpm_trie_node_t *pivot,
                                        uint32_t pivot_len,
                                        uint32_t *pfx,
                                        uint32_t len,
                                        trie_propagate_op_t op,
                                        rm_alpm_trie_propagate_cb_f cb,
                                        rm_alpm_trie_bpm_cb_info_t *cb_info,
                                        bool bpm_aware)
{
    int rv = SHR_E_NONE;

    if (!pfx || !pivot || (len > mkl) ||
        (pivot_len > mkl) || (len < pivot_len) ||
        (pivot->type != PAYLOAD) || !cb || !cb_info) {
        return SHR_E_PARAM;
    }

    trie_init_propagate_info(pfx, len, cb, cb_info);
    len -= pivot_len;

    if (len > 0) {
        uint32_t bit = bcmptm_rm_alpm_key_bits_get(mkl, pfx, len, 1, 0);

        if (pivot->child[bit].child_node) {
            /* validate if the pivot provided is correct */
            rv = trie_propagate_prefix_validate(mkl,
                                                pivot->child[bit].child_node,
                                                pfx, len - 1);
            if (SHR_SUCCESS(rv)) {
                if (bpm_aware) {
                    rv = trie_propagate_prefix(mkl,
                                               pivot->child[bit].child_node,
                                               pfx, len - 1,
                                               op, cb, cb_info);
                } else {
                    rv = trie_propagate_prefix2(mkl,
                                                pivot->child[bit].child_node,
                                                pfx, len - 1,
                                                op, cb, cb_info);
                }
            }
        } /* else nop, nothing to propagate on this path end */
    } else {
        /* pivot == prefix */
        if (bpm_aware) {
            rv = trie_propagate_prefix(mkl, pivot, pfx,
                                       pivot->skip_len,/* Not len (0) !*/
                                       op, cb, cb_info);
        } else {
            rv = trie_propagate_prefix2(mkl, pivot, pfx,
                                        pivot->skip_len,/* Not len (0) !*/
                                        op, cb, cb_info);
        }
    }

    return rv;
}

int bcmptm_rm_alpm_trie_propagate(rm_alpm_trie_t              *trie,
                                  uint32_t                    *pfx,
                                  uint32_t                    len,
                                  trie_propagate_op_t         op,
                                  rm_alpm_trie_propagate_cb_f cb,
                                  rm_alpm_trie_bpm_cb_info_t  *cb_info,
                                  bool                        bpm_aware)
{
    if (!trie || !pfx  || !cb || !cb_info) {
        return SHR_E_PARAM;
    }

    trie_init_propagate_info(pfx, len, cb, cb_info);

    /*
     * It automatically finds the pivot for the given pfx & len, and further
     * finds the propagation point, and traverse the pivot trie to do propa.
     */
    if (bpm_aware) {
        return trie_propagate_prefix(trie->mkl, trie->trie,
                                     pfx, len, op, cb, cb_info);
    } else {
        return trie_propagate_prefix2(trie->mkl, trie->trie,
                                      pfx, len, op, cb, cb_info);
    }

}


/*
 * Function:
 *     bcmptm_rm_alpm_trie_create
 * Purpose:
 *     allocates a trie & initializes it
 */
int bcmptm_rm_alpm_trie_create(int u, int m, uint32_t mkl, rm_alpm_trie_t **ptrie)
{
    rm_alpm_trie_t *trie = NULL;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    trie = shr_lmm_alloc(all_tries[u][m]);
    SHR_NULL_CHECK(trie, SHR_E_MEMORY);

    sal_memset(trie, 0, sizeof(rm_alpm_trie_t));

    if (mkl % 32 == 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    trie->mkl = mkl;
    trie->trie = NULL; /* means nothing is on trie */
    *ptrie = trie;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}




/*
 * Function:
 *     bcmptm_rm_alpm_trie_destroy
 * Purpose:
 *     Destroys a trie's head only
 */
int bcmptm_rm_alpm_trie_destroy(int u, int m, rm_alpm_trie_t *trie)
{
    trie_destroy(u, m, trie);
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_trie_count(rm_alpm_trie_t *trie)
{

    if (trie && trie->trie) {
        return trie->trie->count;
    }

    return 0;
}


int
bcmptm_rm_alpm_trie_clone_create(int u, int m,
                                 rm_alpm_trie_t *trie_src,
                                 rm_alpm_trie_clone_callback_f clone_cb,
                                 void *user_data,
                                 rm_alpm_trie_t **trie_dst,
                                 shr_lmm_hdl_t *clone_hdl)
{
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(trie_src, SHR_E_PARAM);
    SHR_NULL_CHECK(trie_dst, SHR_E_PARAM);
    SHR_NULL_CHECK(clone_hdl, SHR_E_PARAM);

    rv = bcmptm_rm_alpm_trie_create(u, m, trie_src->mkl, trie_dst);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(rv);
    }
    if (trie_src->trie == NULL) {
        SHR_EXIT();
    }

    if (!*clone_hdl) {
        ALPM_LMM_INIT
            (rm_alpm_trie_node_t, *clone_hdl, CLONE_CHUNK_SIZE, false, rv);

        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_EXIT(SHR_E_MEMORY);
        }
    }

    rv = trie_clone(trie_src->trie,         /* trie */
                    *clone_hdl,
                    0,                      /* bit */
                    clone_cb,
                    user_data,
                    NULL,                   /* parent node */
                    *trie_dst);

    if (SHR_FAILURE(rv)) {
        bcmptm_rm_alpm_trie_clone_destroy(u, m, *trie_dst, *clone_hdl);
        *trie_dst = NULL;
    }
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_trie_clone_destroy(int u, int m, rm_alpm_trie_t *trie_dst,
                                  shr_lmm_hdl_t clone_hdl)
{

    if (trie_dst && clone_hdl) {
        shr_lmm_delete(clone_hdl);
        trie_destroy(u, m, trie_dst);
    }
}


/*
 * Function:
 *     bcmptm_rm_alpm_trie_compare
 * Purpose:
 *     Compare a trie.
 * Note:
 *     Two tries are considered identical if all the payloads' key & length
 *     of first trie is exactly same as the ones of second trie. The internal
 *     nodes' contents and structure does not make a difference.
 */
int bcmptm_rm_alpm_trie_compare(rm_alpm_trie_t *trie_src,
                                rm_alpm_trie_t *trie_dst,
                                int *equal)
{
    int rv;
    rm_alpm_trie_traverse_states_e_t state = TRAVERSE_STATE_NONE;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    TRIE_ASSERT_RETURN(trie_src && trie_dst && equal);

    *equal = 0;

    rv = trie_compare(NULL, trie_src->trie, 0, &state, NULL, trie_dst->trie,
                      equal);
    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

int bcmptm_rm_alpm_trie_init(int u, int m, bool multi_thread)
{
    int rv;

    ALPM_LMM_INIT(rm_alpm_trie_node_t,
                  trie_internal_nodes[u][m],
                  INTERNAL_NODES_ALLOC_CHUNK_SIZE,
                  multi_thread,
                  rv);
    if (rv != SHR_E_NONE) {
        return SHR_E_MEMORY;
    }

    ALPM_LMM_INIT(trie_bfs_node_t,
                  bfs_traverse_next_nodes[u][m],
                  BFS_NODES_ALLOC_CHUNK_SIZE,
                  multi_thread,
                  rv);
    if (rv != SHR_E_NONE) {
        return SHR_E_MEMORY;
    }


    ALPM_LMM_INIT(trie_bfs_node_t,
                  bfs_traverse_nodes[u][m],
                  BFS_NODES_ALLOC_CHUNK_SIZE,
                  multi_thread,
                  rv);
    if (rv != SHR_E_NONE) {
        return SHR_E_MEMORY;
    }

    ALPM_LMM_INIT(rm_alpm_trie_t,
                  all_tries[u][m],
                  TRIES_ALLOC_CHUNK_SIZE,
                  multi_thread,
                  rv);
    if (rv != SHR_E_NONE) {
        return SHR_E_MEMORY;
    }
    return rv;
}

void
bcmptm_rm_alpm_trie_traverse_cache_cleanup(int u, int m)
{
    if (bfs_traverse_next_nodes[u][m]) {
        shr_lmm_free_all(bfs_traverse_next_nodes[u][m]);
    }
}


void
bcmptm_rm_alpm_trie_cleanup(int u, int m)
{
    if (trie_internal_nodes[u][m]) {
        shr_lmm_delete(trie_internal_nodes[u][m]);
        trie_internal_nodes[u][m] = NULL;
    }
    if (bfs_traverse_nodes[u][m]) {
        shr_lmm_delete(bfs_traverse_nodes[u][m]);
        bfs_traverse_nodes[u][m] = NULL;
    }
    if (bfs_traverse_next_nodes[u][m]) {
        shr_lmm_delete(bfs_traverse_next_nodes[u][m]);
        bfs_traverse_next_nodes[u][m] = NULL;
    }
    if (all_tries[u][m]) {
        shr_lmm_delete(all_tries[u][m]);
        all_tries[u][m] = NULL;
    }
}

