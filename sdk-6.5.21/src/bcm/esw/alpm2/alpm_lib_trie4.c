/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    trie.c
 * Purpose: Custom Trie Data structure
 * Requires:
 */

/* Implementation notes:
 * Trie is a prefix based data strucutre. It is based on modification to digital search trie.
 * This implementation is not a Path compressed Binary Trie (or) a Patricia Trie.
 * It is a custom version which represents prefix on a digital search trie as following.
 * A given node on the trie could be a Payload node or a Internal node. Each node is represented
 * by <skip address, skip length> pair. Each node represents the given prefix it represents when
 * the prefix is viewed from Right to Left. ie., Most significant bits to Least significant bits.
 * Each node has a Right & Left child which branches based on bit on that position.
 * There can be empty split node i.e, <0,0> just to host two of its children.
 */
#ifdef ALPM_ENABLE

#include <bcm_int/esw/alpm_lib_trie.h>

extern void * alpm_util_alloc(unsigned int sz, char *s);
extern void alpm_util_free(void *addr);
extern uint32 lcplen6(uint32 *key, uint32 len1, uint32 skip_addr, uint32 len2);

/********************************************************/
/* Get a chunk of bits from a key (MSB bit - on word0, lsb on word 1), pos is 1 based(msb bit position)..
 */
static uint32 _alpm_lib_key_get_bits(uint32 *key, uint32 pos, uint32 len)
{
    uint32 val=0, diff, bitpos;

    bitpos = (pos-1) % _NUM_WORD_BITS_;
    bitpos++; /* 1 based */

    if (bitpos >= len) {
        diff = bitpos - len;
        /* coverity[var_deref_op : FALSE] */
        val = TRIE_SHR(key[KEY48_BIT2IDX(pos)], diff, _NUM_WORD_BITS_);
        val &= TRIE_MASK(len);
        return val;
    } else {
        diff = len - bitpos;
        /* coverity[var_deref_op : FALSE] */
        val = TRIE_SHL(key[KEY48_BIT2IDX(pos)] & TRIE_MASK(bitpos), diff, _NUM_WORD_BITS_);
        /* get bits from next word */
        return (_alpm_lib_key_get_bits(key, pos-bitpos, diff) | val);
    }
}

/*
 * Assumes the layout for
 * 0 - most significant word
 * MAX_KEY_WORDS - least significant word
 * eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
static void _key_shift_left(uint32 *key, uint32 shift)
{
    uint32 index=0;
    for (index=KEY48_BIT2IDX(_MAX_KEY_LEN_48_); index < KEY48_BIT2IDX(1); index++) {
        key[index] = TRIE_SHL(key[index], shift,_NUM_WORD_BITS_) | \
                     TRIE_SHR(key[index+1],_NUM_WORD_BITS_-shift,_NUM_WORD_BITS_);
    }

    key[index] = TRIE_SHL(key[index], shift, _NUM_WORD_BITS_);

    /* mask off snippets bit on MSW */
    key[0] &= _MASK(_MAX_KEY_LEN_48_ % _NUM_WORD_BITS_);
    return ;
}

/*
 * Assumes the layout for
 * 0 - most significant word
 * MAX_KEY_WORDS - least significant word
 * eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
static int _key_shift_right(uint32 *key, uint32 shift)
{
    uint32 index=0;
    for(index=KEY48_BIT2IDX(1); index > KEY48_BIT2IDX(_MAX_KEY_LEN_48_); index--) {
        key[index] = TRIE_SHR(key[index], shift,_NUM_WORD_BITS_) | \
                     TRIE_SHL(key[index-1],_NUM_WORD_BITS_-shift,_NUM_WORD_BITS_);
    }

    key[index] = TRIE_SHR(key[index], shift, _NUM_WORD_BITS_);

    /* mask off snippets bit on MSW */
    key[0] &= _MASK(_MAX_KEY_LEN_48_ % _NUM_WORD_BITS_);
    return SOC_E_NONE;
}


/*
 * Assumes the layout for
 * 0 - most significant word
 * MAX_KEY_WORDS - least significant word
 * eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
static void _key_append(uint32 *key,
                uint32 *length,
                uint32 skip_addr,
                uint32 skip_len)
{
    _key_shift_left(key, skip_len);
    key[KEY48_BIT2IDX(1)] |= skip_addr;
    *length += skip_len;

    return ;
}

/*
 * v4 and v6 _key_append
 */
static void _alpm_lib_key_append(uint32 v6,
                                 uint32 *key,
                                 uint32 *key_len,
                                 uint32 skip_addr,
                                 uint32 skip_len)
{
    if (v6) {
        (void)_v6_key_append(key, key_len, skip_addr, skip_len);
    } else {
        _key_append(key, key_len, skip_addr, skip_len);
    }

    return ;
}

/*
 * Function:
 *     lcplen
 * Purpose:
 *     returns longest common prefix length provided a key & skip address
 */
static uint32
lcplen(uint32 *key, uint32 len1,
       uint32 skip_addr, uint32 len2)
{
    uint32 diff;
    uint32 lcp = len1 < len2 ? len1 : len2;

    if (len1 == 0 || len2 == 0) return 0;

    diff = _alpm_lib_key_get_bits(key, len1, lcp);
    diff ^= (TRIE_SHR(skip_addr, len2 - lcp, _MAX_SKIP_LEN_) & TRIE_MASK(lcp));

    while (diff) {
        diff >>= 1;
        --lcp;
    }

    return lcp;
}

int _alpm_lib_print_trie_node(alpm_lib_trie_node_t *trie, void *datum)
{
    if (trie != NULL) {

    cli_out("trie: %p, type %s, skip_addr 0x%x skip_len %d "
            "count:%d Child[0]:%p Child[1]:%p\n",
            trie, (trie->type == trieNodeTypePayload)?"P":"I",
            trie->skip_addr, trie->skip_len,
            trie->count, trie->child[0],
            trie->child[1]);
    }
    return SOC_E_NONE;
}

#if 0 /* Not used */
static int _trie_preorder_traverse(alpm_lib_trie_node_t *trie, alpm_lib_trie_callback_f cb, void *user_data)
{
    int rv = SOC_E_NONE;
    alpm_lib_trie_node_t *tmp1, *tmp2;

    if (trie == NULL || !cb) {
        return SOC_E_NONE;
    } else {
        /* make the node delete safe */
        tmp1 = trie->child[0];
        tmp2 = trie->child[1];
        rv = cb(trie, user_data);
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_preorder_traverse(tmp1, cb, user_data);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_preorder_traverse(tmp2, cb, user_data);
    }
    return rv;
}

static int _trie_postorder_traverse(alpm_lib_trie_node_t *trie, alpm_lib_trie_callback_f cb, void *user_data)
{
    int rv = SOC_E_NONE;

    if (trie == NULL) {
        return SOC_E_NONE;
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_postorder_traverse(trie->child[0], cb, user_data);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_postorder_traverse(trie->child[1], cb, user_data);
    }
    if (SOC_SUCCESS(rv)) {
        rv = cb(trie, user_data);
    }
    return rv;
}
#endif

static int _trie_inorder_traverse(alpm_lib_trie_node_t *trie, alpm_lib_trie_callback_f cb, void *user_data, int only_payload)
{
    int rv = SOC_E_NONE;
    alpm_lib_trie_node_t *tmp;

    if (trie == NULL) {
        return SOC_E_NONE;
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_inorder_traverse(trie->child[0], cb, user_data, only_payload);
    }

    /* make the trie pointers delete safe */
    tmp = trie->child[1];

    if (SOC_SUCCESS(rv)) {
        if (!only_payload || trie->type == trieNodeTypePayload) {
            rv = cb(trie, user_data);
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_inorder_traverse(tmp, cb, user_data, only_payload);
    }
    return rv;
}

static int _trie_traverse(alpm_lib_trie_node_t *trie, alpm_lib_trie_callback_f cb,
              void *user_data,  alpm_lib_trie_traverse_order_t order, int only_payload)
{
    int rv = SOC_E_PARAM;

    switch(order) {
#if 0 /* Not used */
    case trieTraverseOrderPre:
        rv = _trie_preorder_traverse(trie, cb, user_data);
        break;
    case trieTraverseOrderPost:
        rv = _trie_postorder_traverse(trie, cb, user_data);
        break;
#endif
    case trieTraverseOrderIn:
        rv = _trie_inorder_traverse(trie, cb, user_data, only_payload);
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *     alpm_lib_trie_traverse
 * Purpose:
 *     Traverse the trie & call the application callback with user data
 */
int alpm_lib_trie_traverse(alpm_lib_trie_t *trie, alpm_lib_trie_callback_f cb,
                  void *user_data, alpm_lib_trie_traverse_order_t order, int only_payload)
{
    if (trie == NULL) {
        return SOC_E_NONE;
    } else {
        return _trie_traverse(trie->trie, cb, user_data, order, only_payload);
    }
}

#if 0 /* Not used */
static int _trie_preorder_traverse2(alpm_lib_trie_node_t *ptrie,
                                    alpm_lib_trie_node_t *trie,
                                    alpm_lib_trie_traverse_state_t *state,
                                    alpm_lib_trie_callback_ext_f cb,
                                    void *user_data)
{
    int rv = SOC_E_NONE;
    alpm_lib_trie_node_t *lc, *rc;

    if (trie == NULL || !cb) {
        return SOC_E_NONE;
    } else {
        assert(!ptrie || ptrie->type == trieNodeTypePayload);

        /* make the trie delete safe */
        lc = trie->child[0];
        rc = trie->child[1];
        if (trie->type == trieNodeTypePayload) { /* no need to callback on internal nodes */
            rv = cb(ptrie, trie, state, user_data);
            TRIE_TRAVERSE_STOP(*state, rv);

            /* Change the ptrie as trie if applicable */
            /* make the ptrie delete safe */
            if (*state != trieTraverseStateDel) {
                ptrie = trie;
            }
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_preorder_traverse2(ptrie, lc, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_preorder_traverse2(ptrie, rc, state, cb, user_data);
    }
    return rv;
}
#endif

static int _trie_postorder_traverse2(alpm_lib_trie_node_t *ptrie,
                                    alpm_lib_trie_node_t *trie,
                                    alpm_lib_trie_traverse_state_t *state,
                                    alpm_lib_trie_callback_ext_f cb,
                                    void *user_data)
{
    int rv = SOC_E_NONE;
    alpm_lib_trie_node_t *ori_ptrie = ptrie;
    alpm_lib_trie_node_t *lc, *rc;
    alpm_lib_trie_node_type_t trie_type;
    if (trie == NULL) {
        return SOC_E_NONE;
    }

    assert(!ptrie || ptrie->type == trieNodeTypePayload);

    /* Change the ptrie as trie if applicable */
    if (trie->type == trieNodeTypePayload) {
        ptrie = trie;
    }

    /* During the callback, a trie node can be deleted or inserted.
     * For a deleted node, its internal parent could also be deleted, thus to
     * make it safe we should record rc.
     */
    trie_type = trie->type;
    lc = trie->child[0];
    rc = trie->child[1];

    if (SOC_SUCCESS(rv)) {
        rv = _trie_postorder_traverse2(ptrie, lc, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_postorder_traverse2(ptrie, rc, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        if (trie_type == trieNodeTypePayload) {
            rv = cb(ori_ptrie, trie, state, user_data);
        }
    }
    return rv;
}

#if 0 /* Not used */
static int _trie_inorder_traverse2(alpm_lib_trie_node_t *ptrie,
                                   alpm_lib_trie_node_t *trie,
                                   alpm_lib_trie_traverse_state_t *state,
                                   alpm_lib_trie_callback_ext_f cb,
                                   void *user_data)
{
    int rv = SOC_E_NONE;
    alpm_lib_trie_node_t *rc = NULL;
    alpm_lib_trie_node_t *ori_ptrie = ptrie;

    if (trie == NULL) {
        return SOC_E_NONE;
    }

    assert(!ptrie || ptrie->type == trieNodeTypePayload);

    /* Change the ptrie as trie if applicable */
    if (trie->type == trieNodeTypePayload) {
        ptrie = trie;
    }

    rv = _trie_inorder_traverse2(ptrie, trie->child[0], state, cb, user_data);
    TRIE_TRAVERSE_STOP(*state, rv);

    /* make the trie delete safe */
    rc = trie->child[1];

    if (SOC_SUCCESS(rv)) {
        if (trie->type == trieNodeTypePayload) {
            rv = cb(ptrie, trie, state, user_data);
            TRIE_TRAVERSE_STOP(*state, rv);
            /* make the ptrie delete safe */
            if (*state == trieTraverseStateDel) {
                ptrie = ori_ptrie;
            }
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_inorder_traverse2(ptrie, rc, state, cb, user_data);
    }
    return rv;
}
#endif

static int _trie_traverse2(alpm_lib_trie_node_t *trie, alpm_lib_trie_callback_ext_f cb,
                           void *user_data,  alpm_lib_trie_traverse_order_t order,
                           alpm_lib_trie_traverse_state_t *state)
{
    int rv = SOC_E_PARAM;

    switch (order) {
#if 0 /* Not used */
        case trieTraverseOrderPre:
            rv = _trie_preorder_traverse2(NULL, trie, state, cb, user_data);
            break;
        case trieTraverseOrderIn:
            rv = _trie_inorder_traverse2(NULL, trie, state, cb, user_data);
            break;
#endif
        case trieTraverseOrderPost:
            rv = _trie_postorder_traverse2(NULL, trie, state, cb, user_data);
            break;
        default:
            break;
    }

    return rv;
}

/*
 * Function:
 *     alpm_lib_trie_traverse2
 * Purpose:
 *     Traverse the trie (trieNodeTypePayload) & call the extended application callback
 *     which has current node's trieNodeTypePayload parent node with user data.
 */
int alpm_lib_trie_traverse2(alpm_lib_trie_t *trie, alpm_lib_trie_callback_ext_f cb,
                   void *user_data, alpm_lib_trie_traverse_order_t order)
{
    alpm_lib_trie_traverse_state_t state = trieTraverseStateNone;

    if (order < trieTraverseOrderPre ||
        order >= trieTraverseOrderMax || !cb) return SOC_E_PARAM;

    if (trie == NULL) {
        return SOC_E_NONE;
    } else {
        return _trie_traverse2(trie->trie, cb, user_data, order, &state);
    }
}

/* Merge traverse for: P1 + P2 => P3 (internal) or P1 + P2 => P2 (payload)
    - v6: IPv6 (1) or IPv4 (0).
    - pptrie: immediate payload parent node
    - ptrie: immediate parent node (payload or internal)
    - trie: current node
    - prt_key/prt_key_len: last parent node key and length.
   Note: if (pptrie == ptrie), then P1 + P2 => P2 (payload) case */
static int _trie_postorder_traverse3(uint32 v6,
                                     alpm_lib_trie_node_t *pptrie,
                                     alpm_lib_trie_node_t *ptrie,
                                     alpm_lib_trie_node_t *trie,
                                     uint32 *prt_key,
                                     uint32 prt_key_len,
                                     alpm_lib_trie_traverse_state_t *state,
                                     alpm_lib_trie_callback_ext3_f cb,
                                     void *user_data)
{
    int rv = SOC_E_NONE;
    alpm_lib_trie_node_t *ori_pptrie = pptrie;
    alpm_lib_trie_node_t *ori_ptrie = ptrie;
    alpm_lib_trie_node_t *lc, *rc;
    alpm_lib_trie_node_type_t trie_type;
    uint32 key[5];
    uint32 key_len;
    uint8 bit = 0; /* coming from previous ptrie left */

    if (trie == NULL) {
        return SOC_E_NONE;
    }

    if (ptrie == NULL) {
        bit = 2; /* invalid bit */
    } else if (trie == ptrie->child[1]) {
        bit = 1;
    }

    ptrie = trie; /* immediate parent node (payload or internal) */

    if (trie->type == trieNodeTypePayload) {
        pptrie = trie; /* immediate payload parent node */
    }

    /* During the callback, a trie node can be deleted or inserted.
     * For a deleted node, its internal parent could also be deleted, thus to
     * make it safe we should record rc.
     */
    trie_type = trie->type;
    lc = trie->child[0];
    rc = trie->child[1];

    if (SOC_SUCCESS(rv)) {
        sal_memcpy(key, prt_key, sizeof(key));
        key_len = prt_key_len;
        if (bit < 2) { /* bypass for initial default node */
            _alpm_lib_key_append(v6, key, &key_len, bit, 1);
        }
        _alpm_lib_key_append(v6, key, &key_len, trie->skip_addr, trie->skip_len);
        rv = _trie_postorder_traverse3(v6, pptrie, ptrie,
                                       lc, key, key_len, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        sal_memcpy(key, prt_key, sizeof(key));
        key_len = prt_key_len;
        if (bit < 2) { /* bypass for initial default node */
            _alpm_lib_key_append(v6, key, &key_len, bit, 1);
        }
        _alpm_lib_key_append(v6, key, &key_len, trie->skip_addr, trie->skip_len);
        rv = _trie_postorder_traverse3(v6, pptrie, ptrie,
                                       rc, key, key_len, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        if (trie_type == trieNodeTypePayload) {
            rv = cb(ori_pptrie, ori_ptrie, trie,
                    prt_key, prt_key_len, state, user_data);
        }
    }
    return rv;
}

static int _trie_traverse3(uint32 v6,
                           alpm_lib_trie_node_t *trie,
                           alpm_lib_trie_callback_ext3_f cb,
                           void *user_data,
                           alpm_lib_trie_traverse_order_t order,
                           alpm_lib_trie_traverse_state_t *state)
{
    int rv = SOC_E_PARAM;
    uint32 prt_key[5] = {0};
    uint32 prt_key_len = 0;

    switch (order) {
#if 0 /* Not used */
        case trieTraverseOrderPre:
            rv = _trie_preorder_traverse3(NULL, trie, state, cb, user_data);
            break;
        case trieTraverseOrderIn:
            rv = _trie_inorder_traverse3(NULL, trie, state, cb, user_data);
            break;
#endif
        case trieTraverseOrderPost:
            rv = _trie_postorder_traverse3(v6, NULL, NULL, trie, prt_key,
                                           prt_key_len, state, cb, user_data);
            break;
        default:
            break;
    }

    return rv;
}

/*
 * Function:
 *     alpm_lib_trie_traverse3
 * Purpose:
 *     Traverse the trie (trieNodeTypePayload) & call the extended application callback
 *     which has current node's any count = 2 parent node with user data.
 */
int alpm_lib_trie_traverse3(alpm_lib_trie_t *trie, alpm_lib_trie_callback_ext3_f cb,
                            void *user_data, alpm_lib_trie_traverse_order_t order)
{
    alpm_lib_trie_traverse_state_t state = trieTraverseStateNone;

    if (order < trieTraverseOrderPre ||
        order >= trieTraverseOrderMax || !cb) return SOC_E_PARAM;

    if (trie == NULL) {
        return SOC_E_NONE;
    } else {
        return _trie_traverse3(trie->v6_key, trie->trie, cb, user_data, order, &state);
    }
}

static int _trie_dump(alpm_lib_trie_node_t *trie, alpm_lib_trie_callback_f cb,
              void *user_data, uint32 level)
{
    if (trie == NULL) {
        return SOC_E_NONE;
    } else {
        uint32 lvl = level;
        while(lvl) {
            if (lvl == 1) {
                cli_out("|-");
            } else {
                cli_out("| ");
            }
            lvl--;
        }

        if (cb) {
            cb(trie, user_data);
        } else {
            _alpm_lib_print_trie_node(trie, NULL);
        }
    }

    _trie_dump(trie->child[0], cb, user_data, level+1);
    _trie_dump(trie->child[1], cb, user_data, level+1);
    return SOC_E_NONE;
}

/* Dump also internal node key and key_len */
static int _trie_dump1(uint32 v6,
                       alpm_lib_trie_node_t *trie,
                       uint32 *key,
                       uint32 key_len,
                       alpm_lib_trie_callback1_f cb,
                       void *user_data,
                       uint32 level)
{
    uint32 key1[5];
    uint32 key1_len;

    if (trie == NULL || key == NULL) {
        return SOC_E_NONE;
    } else {
        uint32 lvl = level;
        while(lvl) {
            if (lvl == 1) {
                cli_out("|-");
            } else {
                cli_out("| ");
            }
            lvl--;
        }

        _alpm_lib_key_append(v6, key, &key_len, trie->skip_addr, trie->skip_len);

        /* save current node key and key_len for right child */
        sal_memcpy(key1, key, sizeof(key1));
        key1_len = key_len;

        if (cb) {
            cb(trie, key1, key1_len, user_data);
        } else {
            _alpm_lib_print_trie_node(trie, NULL);
        }
    }

    _alpm_lib_key_append(v6, key, &key_len, 0, 1); /* bit = 0 for left child */
    _trie_dump1(v6, trie->child[0], key, key_len, cb, user_data, level+1);

    _alpm_lib_key_append(v6, key1, &key1_len, 1, 1); /* bit = 1 for right child */
    _trie_dump1(v6, trie->child[1], key1, key1_len, cb, user_data, level+1);
    return SOC_E_NONE;
}

/*
 * Function:
 *     alpm_lib_trie_dump
 * Purpose:
 *     Dumps the trie pre-order [root|left|child]
 */
int alpm_lib_trie_dump(alpm_lib_trie_t *trie, alpm_lib_trie_callback_f cb, void *user_data)
{
    if (trie->trie) {
        return _trie_dump(trie->trie, cb, user_data, 0);
    } else {
        return SOC_E_PARAM;
    }
}

/*
 * Function:
 *     alpm_lib_trie_dump1
 * Purpose:
 *     Dumps the trie pre-order [root|left|child]
 */
int alpm_lib_trie_dump1(alpm_lib_trie_t *trie, alpm_lib_trie_callback1_f cb, void *user_data)
{
    uint32 key[5] = {0};
    uint32 key_len = 0;

    if (trie->trie) {
        return _trie_dump1(trie->v6_key, trie->trie, key, key_len, cb, user_data, 0);
    } else {
        return SOC_E_PARAM;
    }
}

static int _trie_search(alpm_lib_trie_node_t *trie,
            uint32 *key,
            uint32 length,
            alpm_lib_trie_node_t **payload,
            uint32 *result_key,
            uint32 *result_len,
            uint32 dump,
            uint32 find_pivot)
{
    uint32 lcp=0;
    int bit=0;

    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

#if 0 /* Not used */
    if (dump) {
        _alpm_lib_print_trie_node(trie, (uint32 *)1);
    }
#endif

    if (length > trie->skip_len) {
        if (lcp == trie->skip_len) {
            bit = (key[KEY48_BIT2IDX(length - lcp)] & \
                   (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;
#if 0 /* Not used */
            if (dump) {
                cli_out(" Length: %d Next-Bit[%d] \n", length, bit);
            }
#endif

            if (result_key) {
                _key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
            }

            /* based on next bit branch left or right */
            if (trie->child[bit]) {

                if (result_key) {
                    _key_append(result_key, result_len, bit, 1);
                }

                return _trie_search(trie->child[bit], key,
                                    length - lcp - 1, payload,
                                    result_key, result_len, dump, find_pivot);
            } else {
                return SOC_E_NOT_FOUND; /* not found */
            }
        } else {
            return SOC_E_NOT_FOUND; /* not found */
        }
    } else if (length == trie->skip_len) {
        if (lcp == length) {
#if 0 /* Not used */
            if (dump) {
                cli_out(": MATCH \n");
            }
#endif
            *payload = trie;
            if (trie->type != trieNodeTypePayload && !find_pivot) {
                /* no assert here, possible during dbucket search
                 * due to 1* and 0* bucket search
                 */
                return SOC_E_NOT_FOUND;
            }
            if (result_key) {
                _key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
            }
            return SOC_E_NONE;
        }
        else return SOC_E_NOT_FOUND;
    } else {
        if (lcp == length && find_pivot) {
            *payload = trie;
            if (result_key) {
                _key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
            }
            return SOC_E_NONE;
        }
        return SOC_E_NOT_FOUND; /* not found */
    }
}

/*
 * Function:
 *     alpm_lib_trie_search
 * Purpose:
 *     Search the given trie for exact match of provided prefix/length
 *     If dump is set to 1 it traces the path as it traverses the trie
 */
int alpm_lib_trie_search(alpm_lib_trie_t *trie,
                uint32 *key,
                uint32 length,
                alpm_lib_trie_node_t **payload)
{
    if (trie->trie) {
        if (trie->v6_key) {
            return _alpm_lib_trie_v6_search(trie->trie, key, length, payload, NULL, NULL, 0, 0);
        } else {
            return _trie_search(trie->trie, key, length, payload, NULL, NULL, 0, 0);
        }
    } else {
        return SOC_E_NOT_FOUND;
    }
}

/*
 * Internal function for LPM match searching.
 * callback on all payload nodes if cb != NULL.
 */
static int _trie_find_lpm(alpm_lib_trie_node_t *trie,
              uint32 *key,
              uint32 length,
              alpm_lib_trie_node_t **payload,
              uint32 exclude_self)
{
    uint32 lcp=0;
    int bit=0, rv=SOC_E_NONE;

    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    if ((length > trie->skip_len) && (lcp == trie->skip_len)) {
        if (trie->type == trieNodeTypePayload) {
            /* lpm cases */
            if (payload != NULL) {
                /* update lpm result */
                *payload = trie;
            }
        }

        bit = (key[KEY48_BIT2IDX(length - lcp)] & \
               (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

        /* based on next bit branch left or right */
        if (trie->child[bit]) {
            return _trie_find_lpm(trie->child[bit], key, length - lcp - 1,
                  payload, exclude_self);
        }
    } else if ((length == trie->skip_len) && (lcp == length)) {
        if (trie->type == trieNodeTypePayload) {
            /* exact match case */
            if (payload != NULL && !exclude_self) {
                /* lpm is exact match */
                *payload = trie;
            }
        }
    }
    return rv;
}

/*
 * Function:
 *     alpm_lib_trie_find_lpm
 * Purpose:
 *     Find the longest prefix matched with given prefix
 */
int alpm_lib_trie_find_lpm(alpm_lib_trie_t *trie, uint32 *key, uint32 length, alpm_lib_trie_node_t **payload)
{
    int rv = SOC_E_NONE;

    *payload = NULL;

    if (trie->trie) {
        if (trie->v6_key) {
            rv = _alpm_lib_trie_v6_find_lpm(trie->trie, key, length, payload, 0);
        } else {
            rv = _trie_find_lpm(trie->trie, key, length, payload, 0);
        }
        if (*payload || (rv != SOC_E_NONE)) {
            return rv;
        }
    }

    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *   _alpm_lib_trie_skip_node_free
 * Purpose:
 *   Destroy a chain of alpm_lib_trie_node_t that has the target node at the end.
 *   The target node is not necessarily trieNodeTypePayload type, but all nodes
 *   on the chain except for the end must have only one branch.
 * Input:
 *   key      --  target key
 *   length   --  target key length
 *   free_end --  free
 */
int
_alpm_lib_trie_skip_node_free(alpm_lib_trie_node_t *trie, int max_key_len, uint32 *key, uint32 length)
{
    uint32 lcp=0;
    int bit=0, rv=SOC_E_NONE;

    if (max_key_len == _MAX_KEY_LEN_144_) {
        lcp = lcplen6(key, length, trie->skip_addr, trie->skip_len);
    } else {
        lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);
    }

    if (length > trie->skip_len) {
        if (lcp == trie->skip_len) {
            bit = (key[KEY_BIT2IDX(max_key_len, length - lcp)] & \
                    (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

            /* There should be only one branch on the chain until the end node */
            if (!trie->child[0] == !trie->child[1]) {
                return SOC_E_PARAM;
            }

            /* based on next bit branch left or right */
            if (trie->child[bit]) {
                rv = _alpm_lib_trie_skip_node_free(trie->child[bit], max_key_len, key,
                        length - lcp - 1);
                if (SOC_SUCCESS(rv)) {
                    assert(trie->type == trieNodeTypeInternal);
                    alpm_util_free(trie);
                }
                return rv;
            } else {
                return SOC_E_NOT_FOUND; /* not found */
            }
        } else {
            return SOC_E_NOT_FOUND; /* not found */
        }
    } else if (length == trie->skip_len) {
        if (lcp == length) {
            /* the end node is not necessarily type payload. */
            /* Do not free the end */

            return SOC_E_NONE;
        }
        else return SOC_E_NOT_FOUND;
    } else {
        return SOC_E_NOT_FOUND; /* not found */
    }
}



/*
 * Function:
 *   _trie_skip_node_alloc
 * Purpose:
 *   create a chain of alpm_lib_trie_node_t that has the payload at the end.
 *   each node in the chain can skip upto _MAX_SKIP_LEN number of bits,
 *   while the child pointer in the chain represent 1 bit. So totally
 *   each node can absorb (_MAX_SKIP_LEN+1) bits.
 * Input:
 *   key      --
 *   msb      --
 *   skip_len --  skip_len of the whole chain
 *   payload  --  payload node we want to insert
 *   count    --  child count
 * Output:
 *   node     -- return pointer of the starting node of the chain.
 */
static int _trie_skip_node_alloc(alpm_lib_trie_node_t **node,
                 uint32 *key,
                 uint32 msb, /* NOTE: valid msb position 1 based, 0 means skip0/0 node */
                 uint32 skip_len,
                 alpm_lib_trie_node_t *payload,
                 uint32 count) /* payload count underneath - mostly 1 except some tricky cases */
{
    int lsb=0, msbpos=0, lsbpos=0, bit=0, index;
    alpm_lib_trie_node_t *child = NULL, *skip_node = NULL;

    /* calculate lsb bit position, also 1 based */
    lsb = ((msb)? msb + 1 - skip_len : msb);

    if (msb) {
        for (index = BITS2SKIPOFF(lsb), lsbpos = lsb - 1; index <= BITS2SKIPOFF(msb); index++) {
            /* each loop process _MAX_SKIP_LEN number of bits?? */
            if (lsbpos == lsb-1) {
                /* (lsbpos == lsb-1) is only true for first node (loop) here */
                skip_node = payload;
            } else {
                /* other nodes need to be created */
                skip_node = alpm_util_alloc(sizeof(alpm_lib_trie_node_t), "trie_node");
            }

            /* init memory */
            sal_memset(skip_node, 0, sizeof(alpm_lib_trie_node_t));

            /* calculate msb bit position of current chunk of bits we are processing */
            msbpos = index * _MAX_SKIP_LEN_ - 1;
            if (msbpos > msb-1) msbpos = msb-1;

            /* calculate the skip_len of the created node */
            if (msbpos - lsbpos < _MAX_SKIP_LEN_) {
                skip_node->skip_len = msbpos - lsbpos + 1;
            } else {
                skip_node->skip_len = _MAX_SKIP_LEN_;
            }

            /* calculate the skip_addr (skip_length number of bits).
             * skip might be skipping bits on 2 different words
             * if msb & lsb spawns 2 word boundary in worst case
             */
            if (BITS2WORDS(msbpos+1) != BITS2WORDS(lsbpos+1)) {
                /* pull snippets from the different words & fuse */
                skip_node->skip_addr = key[KEY48_BIT2IDX(msbpos+1)] & TRIE_MASK((msbpos+1) % _NUM_WORD_BITS_);
                skip_node->skip_addr = _SHL(skip_node->skip_addr, skip_node->skip_len - ((msbpos+1) % _NUM_WORD_BITS_));
                skip_node->skip_addr |= _SHR(key[KEY48_BIT2IDX(lsbpos+1)],(lsbpos % _NUM_WORD_BITS_));
            } else {
                skip_node->skip_addr = _SHR(key[KEY48_BIT2IDX(msbpos+1)], (lsbpos % _NUM_WORD_BITS_));
            }

            /* set up the chain of child pointer, first node has no child since "child" was inited to NULL */
            if (child) {
                skip_node->child[bit] = child;
            }

            /* calculate child pointer for next loop. NOTE: skip_addr has not been masked
             * so we still have the child bit in the skip_addr here.
             */
            bit = (skip_node->skip_addr & _SHL(1, skip_node->skip_len - 1)) ? 1:0;

            /* calculate node type */
            if (lsbpos == lsb-1) {
                /* first node is payload */
                skip_node->type = trieNodeTypePayload;
            } else {
                /* other nodes are internal nodes */
                skip_node->type = trieNodeTypeInternal;
            }

            /* all internal nodes will have the same "count" as the payload node */
            skip_node->count = count;

            /* advance lsb to next word */
            lsbpos += skip_node->skip_len;

            /* for all child nodes 0/1 is implicitly obsorbed on parent */
            if (msbpos != msb-1) {
                /* msbpos == (msb-1) is only true for the first node */
                skip_node->skip_len--;
            }
            skip_node->skip_addr &= TRIE_MASK(skip_node->skip_len);
            child = skip_node;
        }
    } else {
        /* skip_len == 0 case, create a payload node with skip_len = 0
         * bit 0 and bit "skip_len" are same bit (bit 0).
         */
        skip_node = payload;
        sal_memset(skip_node, 0, sizeof(alpm_lib_trie_node_t));
        skip_node->type = trieNodeTypePayload;
        skip_node->count = count;
    }

    *node = skip_node;
    return SOC_E_NONE;
}

static int _trie_insert(alpm_lib_trie_node_t *trie,
            uint32 *key,
            uint32 length,
            alpm_lib_trie_node_t *payload, /* payload node */
            alpm_lib_trie_node_t **child, /* child pointer if the child is modified */
            int child_count)
{
    uint32 lcp;
    int rv=SOC_E_NONE, bit=0;
    alpm_lib_trie_node_t *node = NULL;

    *child = NULL;

    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    /* insert cases:
     * 1 - new key could be the parent of existing node
     * 2 - new key could become the child of a existing node
     * 3 - internal node could be inserted and the key becomes one of child
     * 4 - internal node is converted to a payload node */

    /* if the new key qualifies as new root do the inserts here */
    if (lcp == length) { /* guaranteed: length < _MAX_SKIP_LEN_ */
        if (trie->skip_len == lcp) {
            if (trie->type != trieNodeTypeInternal) {
                /* duplicate */
                return SOC_E_EXISTS;
            } else {
                /* change the internal node to payload node */
                _TRIE_NODE_CLONE_(payload,trie);
                alpm_util_free(trie);
                payload->type = trieNodeTypePayload;
                payload->count += child_count;
                *child = payload;
                return SOC_E_NONE;
            }
        } else { /* skip length can never be less than lcp implcitly here */
            /* this node is new parent for the old trie node */
            /* lcp is the new skip length */
            _TRIE_NODE_CLONE_(payload,trie);
            *child = payload;

            bit = (trie->skip_addr & _SHL(1, trie->skip_len - length - 1)) ? 1 : 0;
            trie->skip_addr &= TRIE_MASK(trie->skip_len - length - 1);
            trie->skip_len  -= (length + 1);

            payload->skip_addr = (length > 0) ? key[KEY48_BIT2IDX(length)] : 0;
            payload->skip_addr &= TRIE_MASK(length);
            payload->skip_len  = length;
            payload->child[bit] = trie;
            payload->child[!bit] = NULL;
            payload->type = trieNodeTypePayload;
            payload->count += child_count;
        }
    } else if (lcp == trie->skip_len) {
        /* key length is implictly greater than lcp here */
        /* decide based on key's next applicable bit */
        bit = (key[KEY48_BIT2IDX(length-lcp)] &
               (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

        if (!trie->child[bit]) {
            /* the key is going to be one of the child of existing node */
            /* should be the child */
            rv = _trie_skip_node_alloc(&node, key,
                       length-lcp-1, /* 0 based msbit position */
                       length-lcp-1,
                       payload, child_count);
            if (SOC_SUCCESS(rv)) {
                trie->child[bit] = node;
                trie->count += child_count;
            } else {
                cli_out("\n Error on trie skip node allocaiton [%d]!!!!\n", rv);
            }
        } else {
            rv = _trie_insert(trie->child[bit],
                              key, length - lcp - 1,
                              payload, child, child_count);
            if (SOC_SUCCESS(rv)) {
                trie->count += child_count;
                if (*child != NULL) { /* chande the old child pointer to new child */
                    trie->child[bit] = *child;
                    *child = NULL;
                }
            }
        }
    } else {
        alpm_lib_trie_node_t *newchild = NULL;

        /* need to introduce internal nodes */
        node = alpm_util_alloc(sizeof(alpm_lib_trie_node_t), "trie-node");
        _TRIE_NODE_CLONE_(node, trie);

        rv = _trie_skip_node_alloc(&newchild, key,
                   ((lcp)?length-lcp-1:length-1),
                   length - lcp - 1,
                   payload, child_count);
        if (SOC_SUCCESS(rv)) {
            bit = (key[KEY48_BIT2IDX(length-lcp)] &
                   (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1: 0;

            node->child[!bit] = trie;
            node->child[bit] = newchild;
            node->type = trieNodeTypeInternal;
            node->skip_addr = _SHR(trie->skip_addr,trie->skip_len - lcp);
            node->skip_len = lcp;
            node->count += child_count;

            *child = node;

            trie->skip_addr &= TRIE_MASK(trie->skip_len - lcp - 1);
            trie->skip_len  -= (lcp + 1);
        } else {
            cli_out("\n Error on trie skip node allocaiton [%d]!!!!\n", rv);
            alpm_util_free(node);
        }
    }

    return rv;
}

/*
 * Function:
 *     alpm_lib_trie_insert
 * Purpose:
 *     Inserts provided prefix/length in to the trie
 */
int alpm_lib_trie_insert(alpm_lib_trie_t *trie,
                         uint32 *key,
                         uint32 length,
                         alpm_lib_trie_node_t *payload)
{
    int rv = SOC_E_NONE;
    alpm_lib_trie_node_t *child=NULL;

    if (!trie) return SOC_E_PARAM;

    if (trie->trie == NULL) {
        if (trie->v6_key) {
            rv = _alpm_lib_trie_v6_skip_node_alloc(&trie->trie, key, length, length, payload, 1);
        } else {
           rv = _trie_skip_node_alloc(&trie->trie, key, length, length, payload, 1);
        }
    } else {
       if (trie->v6_key) {
           rv = _alpm_lib_trie_v6_insert(trie->trie, key, length, payload, &child, 1);
       } else {
           rv = _trie_insert(trie->trie, key, length, payload, &child, 1);
       }
       if (child) { /* chande the old child pointer to new child */
           trie->trie = child;
       }
    }

    return rv;
}

int _alpm_lib_trie_fuse_child(alpm_lib_trie_node_t *trie, int bit)
{
    alpm_lib_trie_node_t *child = NULL;
    int rv = SOC_E_NONE;

    if (trie->child[0] && trie->child[1]) {
        return SOC_E_PARAM;
    }

    bit = (bit > 0)?1:0;
    child = trie->child[bit];

    if (child == NULL) {
        return SOC_E_PARAM;
    } else {
        if (trie->skip_len + child->skip_len + 1 <= _MAX_SKIP_LEN_) {

            if (trie->skip_len == 0) trie->skip_addr = 0;

            if (child->skip_len < _MAX_SKIP_LEN_) {
                trie->skip_addr = TRIE_SHL(trie->skip_addr,child->skip_len + 1,_MAX_SKIP_LEN_);
            }

            trie->skip_addr  |= _SHL(bit, child->skip_len);
            child->skip_addr |= trie->skip_addr;
            child->skip_len  += trie->skip_len + 1;

            /* do not free payload nodes as they are user managed */
            if (trie->type == trieNodeTypeInternal) {
                alpm_util_free(trie);
            }
        }
    }

    return rv;
}

static int _trie_delete(alpm_lib_trie_node_t *trie,
            uint32 *key,
            uint32 length,
            alpm_lib_trie_node_t **payload,
            alpm_lib_trie_node_t **child)
{
    uint32 lcp;
    int rv=SOC_E_NONE, bit=0;
    alpm_lib_trie_node_t *node = NULL;

    /* our algorithm should return before the length < 0, so this means
     * something wrong with the trie structure. Internal error?
     */
    if (!trie || (length && trie->skip_len && !key) ||
        !payload || !child || (length > _MAX_KEY_LEN_48_)) {
    return SOC_E_PARAM;
    }

    *child = NULL;

    /* check a section of key, return the number of matched bits and value of next bit */
    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    if (length > trie->skip_len) {
        if (lcp == trie->skip_len) {
            bit = (key[KEY48_BIT2IDX(length-lcp)] &
                   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;

            /* based on next bit branch left or right */
            if (trie->child[bit]) {
                /* has child node, keep searching */
                rv = _trie_delete(trie->child[bit], key, length - lcp - 1, payload, child);

                if (rv == SOC_E_BUSY) {
                    trie->child[bit] = NULL; /* alpm_util_free the child */
                    rv = SOC_E_NONE;
                    trie->count--;

                    if (trie->type == trieNodeTypeInternal) {
                        bit = (bit == 0) ? 1 : 0;
                        if (trie->child[bit] == NULL) {
                            /* parent and child connected, alpm_util_free the middle-node itself */
                            alpm_util_free(trie);
                            rv = SOC_E_BUSY;
                        } else {
                            /* fuse the parent & child */
                            if (trie->skip_len + trie->child[bit]->skip_len + 1 <=
                                _MAX_SKIP_LEN_) {
                                *child = trie->child[bit];
                                rv = _alpm_lib_trie_fuse_child(trie, bit);
                                if (rv != SOC_E_NONE) {
                                    *child = NULL;
                                }
                            }
                        }
                    }
                } else if (SOC_SUCCESS(rv)) {
                    trie->count--;
                    /* update child pointer if applicable */
                    if (*child != NULL) {
                        trie->child[bit] = *child;
                        *child = NULL;
                    }
                }
            } else {
                /* no child node case 0: not found */
                rv = SOC_E_NOT_FOUND;
            }

        } else {
            /* some bits are not matching, case 0: not found */
            rv = SOC_E_NOT_FOUND;
        }
    } else if (length == trie->skip_len) {
        /* when length equal to skip_len, unless this is a payload node
         * and it's an exact match (lcp == length), we can not found a match
         */
        if (!((lcp == length) && (trie->type == trieNodeTypePayload))) {
            rv = SOC_E_NOT_FOUND;
        } else {
            /* payload node can be deleted */
            /* if this node has 2 children update it to internal node */
            rv = SOC_E_NONE;

            if (trie->child[0] && trie->child[1] ) {
                /* the node has 2 children, update it to internal node */
                node = alpm_util_alloc(sizeof(alpm_lib_trie_node_t), "trie_node");
                _TRIE_NODE_CLONE_(node, trie);
                node->type = trieNodeTypeInternal;
                node->count--;
                *child = node;
            } else if (trie->child[0] || trie->child[1] ) {
                /* if this node has 1 children fuse the children with this node */
                bit = (trie->child[0]) ? 0:1;
                trie->count--;
                if (trie->skip_len + trie->child[bit]->skip_len + 1 <= _MAX_SKIP_LEN_) {
                    /* able to fuse the node with its child node */
                    *child = trie->child[bit];
                    rv = _alpm_lib_trie_fuse_child(trie, bit);
                    if (rv != SOC_E_NONE) {
                        *child = NULL;
                    }
                } else {
                    /* convert it to internal node, we need to alloc new memory for internal nodes
                     * since the old payload node memory will be freed by caller
                     */
                    node = alpm_util_alloc(sizeof(alpm_lib_trie_node_t), "trie_node");
                    _TRIE_NODE_CLONE_(node, trie);
                    node->type = trieNodeTypeInternal;
                    *child = node;
                }
            } else {
                rv = SOC_E_BUSY;
            }

            *payload = trie;
        }
    } else {
        /* key length is shorter, no match if it's internal node,
         * will not exact match even if this is a payload node
         */
        rv = SOC_E_NOT_FOUND; /* case 0: not found */
    }

    return rv;
}

/*
 * Function:
 *     alpm_lib_trie_delete
 * Purpose:
 *     Deletes provided prefix/length in to the trie
 */
int alpm_lib_trie_delete(alpm_lib_trie_t *trie,
                uint32 *key,
                uint32 length,
                alpm_lib_trie_node_t **payload)
{
    int rv = SOC_E_NONE;
    alpm_lib_trie_node_t *child = NULL;

    if (trie->trie) {
        if (trie->v6_key) {
            rv = _alpm_lib_trie_v6_delete(trie->trie, key, length, payload, &child);
        } else {
            rv = _trie_delete(trie->trie, key, length, payload, &child);
        }
        if (rv == SOC_E_BUSY) {
            /* the head node of trie was deleted, reset trie pointer to null */
            trie->trie = NULL;
            rv = SOC_E_NONE;
        } else if (rv == SOC_E_NONE && child != NULL) {
            trie->trie = child;
        }
    } else {
        rv = SOC_E_NOT_FOUND;
    }
    return rv;
}

static INLINE int
_trie_splitable(alpm_lib_trie_node_t *trie, alpm_lib_trie_node_t *child,
                alpm_lib_trie_callback_ext_f cb, void *user_data,
                int max_count, int max_split_count)
{
    /*
     * NOTE:
     *  ABS(trie->count * 2 - max_count) actually means
     *  ABS(trie->count - (max_count - trie->count))
     * which means the count's distance to half depth of the bucket
     */
    int do_split = 0;
    int half_count = (max_count + 1) >> 1;

    if (cb && cb(trie, child, NULL, user_data)) {
        do_split = 1;
    } else if (trie->count <= max_split_count && trie->count != max_count) {
        if (child == NULL) {
            do_split = 1;
        } else if (trie->count >= half_count && child->count <= half_count) {
            do_split = 1;
        } else if (ABS(child->count * 2 - max_count) >
                   ABS(trie->count * 2 - max_count)) {
            do_split = 1;
        }
    }

    return do_split;
}

/*
 * Function:
 *     _trie_split
 * Purpose:
 *     Split the trie into 2 based on optimum pivot
 * NOTE:
 *     max_split_len -- split will make sure the split point
 *                has a length shorter or equal to the max_split_len
 *                unless this will cause a no-split (all prefixs
 *                stays below the split point)
 *     split_to_pair -- used only when the split point will be
 *                used to create a pair of tries later (i.e: dbucket
 *                pair. we assume the split point itself will always be
 *                put into 0* trie if itself is a payload/prefix)
 */
static int _trie_split(alpm_lib_trie_node_t  *trie, uint32 *pivot,
               uint32 *length, uint32 *split_count,
               alpm_lib_trie_node_t **split_node, alpm_lib_trie_node_t **child,
               const uint32 max_count, const uint32 max_split_len,
               alpm_lib_trie_split_state_t *state, alpm_lib_trie_callback_ext_f cb,
               void *user_data,
               const int max_split_count)
{
    int bit=0, rv=SOC_E_NONE;

    if (trie->child[0] && trie->child[1]) {
        bit = (trie->child[0]->count >
               trie->child[1]->count) ? 0:1;
    } else {
        bit = (trie->child[0])?0:1;
    }

    /* start building the pivot */
    _key_append(pivot, length, trie->skip_addr, trie->skip_len);

    {
    /*
     * split logic to make sure the split length is shorter than the
     * requested max_split_len, unless we don't actully split the
     * tree if we stop here.
     * if (*length > max_split_len) && (trie->count != max_count) {
     *    need to split at or above this node. might need to split the node in middle
     * } else if ((ABS(child count*2 - max_count) > ABS(count*2 - max_count)) ||
     *            ((*length == max_split_len) && (trie->count != max_count))) {
     *    (the check above imply trie->count != max_count, so also imply *length < max_split_len)
     *    need to split at this node.
     * } else {
     *    keep searching, will be better split at longer pivot.
     * }
     */
    if ((*length > max_split_len) && (trie->count != max_count)) {
        /* the pivot is getting too long, we better split at this node for
         * better bucket capacity efficiency if we can. We can split if
         * the trie node has a count != max_count, which means the
         * resulted new trie will not have all pivots (FULL)
         */
        if ((trieSplitStatePayloadSplit == *state) &&
            (trie->type == trieNodeTypeInternal)) {
            *state = trieSplitStatePayloadSplitDone;
        } else {
            if (((*length - max_split_len) > trie->skip_len) && (trie->skip_len == 0)) {
                /* the length is longer than max_split_len, and the trie->skip_len is 0,
                 * so the best we can do is use the node as the split point
                 */
                *split_node = trie;
                *split_count = trie->count;

                *state = trieSplitStatePruneNodes;
                return rv;
            }

            /* we need to insert a node and use it as split point */
            *split_node = alpm_util_alloc(sizeof(alpm_lib_trie_node_t), "trie_node");
            sal_memset((*split_node), 0, sizeof(alpm_lib_trie_node_t));
            (*split_node)->type = trieNodeTypeInternal;
            (*split_node)->count = trie->count;

            if ((*length - max_split_len) > trie->skip_len) {
                /* the length is longer than the max_split_len, and the trie->skip_len is
                 * shorter than the difference (max_split_len pivot is not covered by this
                 * node but covered by its parent, the best we can do is split at the branch
                 * lead to this node. we insert a skip_len=0 node and use it as split point
                 */
                (*split_node)->skip_len = 0;
                (*split_node)->skip_addr = 0;

                if (_BITGET(trie->skip_addr, (trie->skip_len-1))) {
                    (*split_node)->child[1] = trie;
                } else {
                    (*split_node)->child[0] = trie;
                }

                /* the split point is with length max_split_len */
                *length -= trie->skip_len;

                /* update the current node to reflect the node inserted */
                trie->skip_len = trie->skip_len - 1;
            } else {
                /* the length is longer than the max_split_len, and the trie->skip_len is
                 * longer than the difference (max_split_len pivot is covered by this
                 * node, we insert a node with length = max_split_len and use it as split point
                 */
                (*split_node)->skip_len = trie->skip_len - (*length - max_split_len);
                (*split_node)->skip_addr = (trie->skip_addr >> (*length - max_split_len));

                if (_BITGET(trie->skip_addr, (*length-max_split_len-1))) {
                    (*split_node)->child[1] = trie;
                } else {
                    (*split_node)->child[0] = trie;
                }

                /* update the current node to reflect the node inserted */
                trie->skip_len = *length - max_split_len - 1;

                /* the split point is with length max_split_len */
                *length = max_split_len;
            }

            trie->skip_addr = trie->skip_addr & BITMASK(trie->skip_len);

            /* there is no need to update the parent node's child pointer
             * to the "trie" node since we will split here and the parent node's
             * child pointer will be set to NULL later
             */
            *split_count = trie->count;

            if (SOC_SUCCESS(rv)) {
                rv = _key_shift_right(pivot, trie->skip_len+1);
            }
            *state = trieSplitStatePruneNodes;
            return rv;
        }
    } else if ( ((*length == max_split_len) && (trie->count != max_count) && trie->count <= max_split_count) ||
                 _trie_splitable(trie, trie->child[bit], cb, user_data, max_count, max_split_count)) {
        /*
         * (1) when the node is at the max_split_len and if used as spliting point
         * the resulted trie will not have all pivots (FULL). we should split
         * at this node.
         * (2) when the node is at the max_split_len and if the resulted trie
         * will have all pivots (FULL), we fall through to keep searching
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
        *split_node = trie;
        *split_count = trie->count;

        if ((trieSplitStatePayloadSplit == *state) &&
            (trie->type == trieNodeTypeInternal)) {
            *state = trieSplitStatePayloadSplitDone;
        } else {
            *state = trieSplitStatePruneNodes;
            return rv;
        }
    } else {
        /* we can not split at this node, keep searching, it's better to
         * split at longer pivot
         */
        _key_append(pivot, length, bit, 1);

        rv = _trie_split(trie->child[bit],
                 pivot, length,
                 split_count, split_node,
                 child, max_count, max_split_len,
                 state, cb, user_data, max_split_count);
    }
    }

    /* free up internal nodes if applicable */
    switch(*state) {
    case trieSplitStatePayloadSplitDone:
        if (trie->type == trieNodeTypePayload) {
            *state = trieSplitStatePruneNodes;
            *split_node = trie;
            *split_count = trie->count;
        } else {
            /* shift the pivot to right to ignore this internal node */
            rv = _key_shift_right(pivot, trie->skip_len+1);
            assert(*length >= trie->skip_len + 1);
            *length -= (trie->skip_len + 1);
        }
        break;

    case trieSplitStatePruneNodes:
        if (trie->count == *split_count) {
            /* if the split point has associate internal nodes they have to
             * be cleaned up */
            assert(trie->type == trieNodeTypeInternal);
            assert(!(trie->child[0] && trie->child[1]));
            alpm_util_free(trie);
        } else {
            assert(*child == NULL);
            /* fuse with child if possible */
            trie->child[bit] = NULL;
            bit = (bit==0)?1:0;
            trie->count -= *split_count;

            /* optimize more */
            if ((trie->type == trieNodeTypeInternal) &&
                (trie->skip_len +
                 trie->child[bit]->skip_len + 1 <= _MAX_SKIP_LEN_)) {
                *child = trie->child[bit];
                rv = _alpm_lib_trie_fuse_child(trie, bit);
                if (rv != SOC_E_NONE) {
                    *child = NULL;
                }
            }
            *state = trieSplitStateDone;
        }
        break;

    case trieSplitStateDone:
        /* adjust parent's count */
        assert(*split_count > 0);
        assert(trie->count >= *split_count);

        /* update the child pointer if child was pruned */
        if (*child != NULL) {
            trie->child[bit] = *child;
            *child = NULL;
        }
        trie->count -= *split_count;
        break;

    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *     alpm_lib_trie_split
 * Purpose:
 *     Split the trie into 2 based on optimum pivot
 * Note:
 *     we need to make sure the length is shorter than
 *     the max_split_len (for capacity optimization) if
 *     possible. We should ignore the max_split_len
 *     if that will result into trie not spliting
 */
int alpm_lib_trie_split(alpm_lib_trie_t *trie,
                        const uint32 max_split_len,
                        uint32 *pivot,
                        uint32 *length,
                        alpm_lib_trie_node_t **split_trie_root,
                        uint8 payload_node_split,
                        alpm_lib_trie_callback_ext_f cb,
                        void *user_data,
                        const int max_split_count)
{
    int rv = SOC_E_NONE;
    uint32 split_count=0, max_count=0;
    alpm_lib_trie_node_t *child = NULL, *node=NULL, clone;
    alpm_lib_trie_split_state_t state = trieSplitStateNone;

    if (!trie || !pivot || !length || !split_trie_root) return SOC_E_PARAM;
    *length = 0;

    if (trie->trie) {
        if (payload_node_split) state = trieSplitStatePayloadSplit;
        max_count = trie->trie->count;

        if (trie->v6_key) {
            sal_memset(pivot, 0, sizeof(uint32) * BITS2WORDS(_MAX_KEY_LEN_144_));
            rv = _alpm_lib_trie_v6_split(trie->trie, pivot, length, &split_count, split_trie_root,
                     &child, max_count, max_split_len, &state, cb, user_data, max_split_count);
        } else {
            sal_memset(pivot, 0, sizeof(uint32) * BITS2WORDS(_MAX_KEY_LEN_48_));

            rv = _trie_split(trie->trie, pivot, length, &split_count, split_trie_root,
                     &child, max_count, max_split_len, &state, cb, user_data, max_split_count);
        }
        if (SOC_SUCCESS(rv) && (trieSplitStateDone == state)) {
            /* adjust parent's count */
            assert(split_count > 0);
            if (trie->trie == NULL) {
                cli_out("parent tree became NULL after split\n");
            }
            /* update the child pointer if child was pruned */
            if (child != NULL) {
                trie->trie = child;
            }
            assert(trie->trie->count >= split_count || (*split_trie_root)->count >= split_count);

            sal_memcpy(&clone, *split_trie_root, sizeof(alpm_lib_trie_node_t));
            child = *split_trie_root;

            /* take advantage of thie function by passing in internal or payload node whatever
             * is the new root. If internal the function assumed it as payload node & changes type.
             * But this method is efficient to reuse the last internal or payload node possible to
             * implant the new pivot */
            if (trie->v6_key) {
                rv = _alpm_lib_trie_v6_skip_node_alloc(&node, pivot,
                              *length, *length,
                              child, child->count);
            } else {
                rv = _trie_skip_node_alloc(&node, pivot,
                           *length, *length,
                           child, child->count);
            }

            if (SOC_SUCCESS(rv)) {
                if (clone.type == trieNodeTypeInternal) {
                    child->type = trieNodeTypeInternal; /* since skip alloc would have reset it to payload */
                }
                child->child[0] = clone.child[0];
                child->child[1] = clone.child[1];
                *split_trie_root = node;
            }
        } else {
            cli_out("!!!! Failed to split the trie error:%d state: %d trie_count %d!!!\n",
                     rv, state, max_count);
        }
    } else {
        rv = SOC_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *     _trie_merge
 * Purpose:
 *     merge or fuse the child trie with parent trie
 */
static int
_trie_merge(alpm_lib_trie_node_t *parent_trie,
            alpm_lib_trie_node_t *child_trie,
            uint32 *pivot,
            uint32 length,
            alpm_lib_trie_node_t **new_parent)
{
    int rv, child_count;
    alpm_lib_trie_node_t *child = NULL, clone;
    uint32 child_pivot[BITS2WORDS(_MAX_KEY_LEN_48_)] = {0};
    uint32 child_length = 0;

    if (length == 0 || !pivot || (length > _MAX_KEY_LEN_48_)) { return SOC_E_PARAM; }

    /*
     * to do merge, there is one and only one condition:
     * parent must cover the child
     */

    /*
     * child pivot could be an internal node, i.e., NOT_FOUND on search
     * so check the out child instead of rv.
     */
    _trie_search(child_trie, pivot, length, &child, child_pivot, &child_length, 0, 1);

    /* The head of a bucket usually is the pivot of the bucket,
     * but for some cases, where the pivot is an trieNodeTypeInternal node,
     * and it is fused with its child, then the pivot can no longer
     * be found, but we can still search a head. The head can be
     * payload (if this is the only payload head), or internal (if
     * two payload head coexist).
     */
    if (child == NULL) {
        return SOC_E_PARAM;
    }

    _TRIE_NODE_CLONE_(&clone, child);

    if (child != child_trie) {
        rv = _alpm_lib_trie_skip_node_free(child_trie, _MAX_KEY_LEN_48_, child_pivot, child_length);
        if (rv < 0) {
            return SOC_E_PARAM;
        }
    }

    /* Record the child count before being cleared */
    child_count = child->count;

    /* Clear the info before insert, mainly it is to prevent previous non-zero
     * count being erroneously included to calculation.
     */
    sal_memset(child, 0, sizeof(*child));

    rv = _trie_insert(parent_trie, child_pivot, child_length, child,
                      new_parent, child_count);
    if (rv < 0) {
        return SOC_E_PARAM;
    }

    /*
     * child node, the inserted node, will be modified during insert,
     * and it must be a leaf node of the parent trie without any child.
     * The child node could be either payload or internal.
     */
    if (child->child[0] || child->child[1]) {
        return SOC_E_PARAM;
    }
    if (clone.type == trieNodeTypeInternal) {
        child->type = trieNodeTypeInternal;
    }
    child->child[0] = clone.child[0];
    child->child[1] = clone.child[1];

    return SOC_E_NONE;
}


/*
 * Function:
 *     alpm_lib_trie_merge
 * Purpose:
 *     merge or fuse the child trie with parent trie.
 */
int alpm_lib_trie_merge(alpm_lib_trie_t *parent_trie,
                        alpm_lib_trie_node_t *child_trie,
                        uint32 *child_pivot,
                        uint32 length)
{
    int rv=SOC_E_NONE;
    alpm_lib_trie_node_t *child=NULL;

    if (!parent_trie) { return SOC_E_PARAM; }
    if (!child_trie) { return SOC_E_NONE; }

    if (parent_trie->trie == NULL) {
        parent_trie->trie = child_trie;
    } else {
        if (parent_trie->v6_key) {
            rv = _alpm_lib_trie_v6_merge(parent_trie->trie, child_trie, child_pivot, length, &child);
        } else {
            rv = _trie_merge(parent_trie->trie, child_trie, child_pivot, length, &child);
        }
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
 *     _trie_split2
 * Purpose:
 *     Split the trie into 2 such that the new sub trie covers given prefix/length.
 * NOTE:
 *     key, key_len    -- The given prefix/length
 *     max_split_count -- The sub trie's max allowed count.
 */
static int
_trie_split2(alpm_lib_trie_node_t *trie,
             uint32 *key,
             uint32 key_len,
             uint32 *pivot,
             uint32 *pivot_len,
             uint32 *split_count,
             alpm_lib_trie_node_t **split_node,
             alpm_lib_trie_node_t **child,
             alpm_lib_trie_split2_state_t *state,
             const int max_split_count,
             const int exact_same)
{
    uint32 lcp=0;
    int bit=0, rv=SOC_E_NONE;

    /* start building the pivot */
    _key_append(pivot, pivot_len, trie->skip_addr, trie->skip_len);


    lcp = lcplen(key, key_len, trie->skip_addr, trie->skip_len);

    if (lcp == trie->skip_len) {
        if (trie->count <= max_split_count &&
            (!exact_same || (key_len - lcp) == 0)) {
            *split_node = trie;
            *split_count = trie->count;
            if (trie->count < max_split_count) {
                *state = trieSplit2StatePruneNodes;
            }
            return SOC_E_NONE;
        }
        if (key_len > lcp) {
            bit = (key[KEY48_BIT2IDX(key_len - lcp)] & \
                    (1 << ((key_len - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

            /* based on next bit branch left or right */
            if (trie->child[bit]) {
                /* we can not split at this node, keep searching, it's better to
                 * split at longer pivot
                 */
                _key_append(pivot, pivot_len, bit, 1);

                rv = _trie_split2(trie->child[bit],
                                  key, key_len - lcp - 1,
                                  pivot, pivot_len, split_count,
                                  split_node, child, state,
                                  max_split_count, exact_same);
                if (SOC_FAILURE(rv)) return rv;
            }
        }
    }

    /* free up internal nodes if applicable */
    switch(*state) {
        case trieSplit2StateNone: /* fail to split */
            break;

        case trieSplit2StatePruneNodes:
            if (trie->count == *split_count) {
                /* if the split point has associate internal nodes they have to
                 * be cleaned up */
                assert(trie->type == trieNodeTypeInternal);
                /* at most one child */
                assert(!(trie->child[0] && trie->child[1]));
                /* at least one child */
                assert(trie->child[0] || trie->child[1]);
                alpm_util_free(trie);
            } else {
                assert(*child == NULL);
                /* fuse with child if possible */
                trie->child[bit] = NULL;
                bit = (bit==0)?1:0;
                trie->count -= *split_count;

                /* optimize more */
                if ((trie->type == trieNodeTypeInternal) &&
                        (trie->skip_len +
                         trie->child[bit]->skip_len + 1 <= _MAX_SKIP_LEN_)) {
                    *child = trie->child[bit];
                    rv = _alpm_lib_trie_fuse_child(trie, bit);
                    if (rv != SOC_E_NONE) {
                        *child = NULL;
                    }
                }
                *state = trieSplit2StateDone;
            }
            break;

        case trieSplit2StateDone:
            /* adjust parent's count */
            assert(*split_count > 0);
            assert(trie->count >= *split_count);

            /* update the child pointer if child was pruned */
            if (*child != NULL) {
                trie->child[bit] = *child;
                *child = NULL;
            }
            trie->count -= *split_count;
            break;

        default:
            break;
    }

    return rv;
}



/*
 * Function:
 *     alpm_lib_trie_split2
 * Purpose:
 *     Split the trie such that the new sub trie covers given prefix/length.
 *     Basically this is a reverse of alpm_lib_trie_merge.
 */

int alpm_lib_trie_split2(alpm_lib_trie_t *trie,
                         uint32 *key,
                         uint32 key_len,
                         uint32 *pivot,
                         uint32 *pivot_len,
                         alpm_lib_trie_node_t **split_trie_root,
                         const int max_split_count,
                         const int exact_same)
{
    int rv = SOC_E_NONE;
    int msc = max_split_count;
    uint32 split_count=0;
    alpm_lib_trie_node_t *child = NULL, *node=NULL, clone;
    alpm_lib_trie_split2_state_t state = trieSplit2StateNone;

    if (!trie || (key_len && !key) || !pivot || !pivot_len ||
        !split_trie_root || max_split_count == 0) {
        return SOC_E_PARAM;
    }

    *split_trie_root = NULL;
    *pivot_len = 0;

    if (trie->trie) {
        if (max_split_count == 0xfffffff) {
            alpm_lib_trie_node_t *child2 = NULL;
            alpm_lib_trie_node_t *payload;
            payload = alpm_util_alloc(sizeof(alpm_lib_trie_node_t), "trie_node");
            if (payload == NULL) {
                return SOC_E_MEMORY;
            }

            if (trie->v6_key) {
                rv = _alpm_lib_trie_v6_insert(trie->trie, key, key_len, payload, &child2, 0);
            } else {
                rv = _trie_insert(trie->trie, key, key_len, payload, &child2, 0);
            }
            if (child2) { /* change the old child pointer to new child */
                trie->trie = child2;
            }

            if (SOC_SUCCESS(rv)) {
                payload->type = trieNodeTypeInternal;
            } else {
                alpm_util_free(payload);
                if (rv != SOC_E_EXISTS) {
                    return rv;
                }
            }

            msc = trie->trie->count;
        }
        if (trie->v6_key) {
            sal_memset(pivot, 0, sizeof(uint32) * BITS2WORDS(_MAX_KEY_LEN_144_));
            rv = _alpm_lib_trie_v6_split2(trie->trie, key, key_len, pivot, pivot_len,
                    &split_count, split_trie_root, &child, &state,
                    msc, exact_same);
        } else {
            sal_memset(pivot, 0, sizeof(uint32) * BITS2WORDS(_MAX_KEY_LEN_48_));
            rv = _trie_split2(trie->trie, key, key_len, pivot, pivot_len,
                    &split_count, split_trie_root, &child, &state,
                    msc, exact_same);
        }

        if (SOC_SUCCESS(rv) && (trieSplit2StateDone == state)) {
            assert(split_count > 0);
            assert(*split_trie_root);
            if (max_split_count == 0xfffffff) {
                assert(*pivot_len == key_len);
            } else {
                assert(*pivot_len < key_len);
            }

            /* update the child pointer if child was pruned */
            if (child != NULL) {
                trie->trie = child;
            }

            sal_memcpy(&clone, *split_trie_root, sizeof(alpm_lib_trie_node_t));
            child = *split_trie_root;

            /* take advantage of thie function by passing in internal or payload node whatever
             * is the new root. If internal the function assumed it as payload node & changes type.
             * But this method is efficient to reuse the last internal or payload node possible to
             * implant the new pivot */
            if (trie->v6_key) {
                rv = _alpm_lib_trie_v6_skip_node_alloc(&node, pivot,
                                              *pivot_len, *pivot_len,
                                              child, child->count);
            } else {
                rv = _trie_skip_node_alloc(&node, pivot,
                                           *pivot_len, *pivot_len,
                                           child, child->count);
            }

            if (SOC_SUCCESS(rv)) {
                if (clone.type == trieNodeTypeInternal) {
                    child->type = trieNodeTypeInternal; /* since skip alloc would have reset it to payload */
                }
                child->child[0] = clone.child[0];
                child->child[1] = clone.child[1];
                *split_trie_root = node;
            }
        } else if (SOC_SUCCESS(rv) && (max_split_count == 0xfffffff) &&
                   (split_count == trie->trie->count)) {
            /* take all */
            *split_trie_root = trie->trie;
            trie->trie = NULL;
        } else { /* split2 is not like split which can always succeed */
            cli_out("Failed to split the trie error:%d state: %d "\
                    "split_trie_root: %p !!!\n",
                    rv, state, *split_trie_root);
            rv = SOC_E_NOT_FOUND;
        }
    } else {
        rv = SOC_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *     _trie_ppg_prefix_validate
 * Purpose:
 *  validate that the provided prefix is valid for propagation.
 *  The added prefix which was member of a shorter pivot's domain
 *  must never be more specific than another pivot encounter if any
 *  in the path
 */
static int _trie_ppg_prefix_validate(alpm_lib_trie_node_t *trie,
                       uint32 *pfx,
                       uint32 len)
{
    uint32 lcp=0, bit=0;

    if (!trie || (len && trie->skip_len && !pfx)) return SOC_E_PARAM;

    if (len == 0) return SOC_E_NONE;

    lcp = lcplen(pfx, len, trie->skip_addr, trie->skip_len);

    if (lcp == trie->skip_len) {
        if (trieNodeTypePayload == trie->type) return SOC_E_PARAM;
    if (len == lcp) return SOC_E_NONE;
        bit = _alpm_lib_key_get_bits(pfx, len-lcp, 1);
        if (!trie->child[bit]) return SOC_E_NONE;
        return _trie_ppg_prefix_validate(trie->child[bit],
                                               pfx, len-1-lcp);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     _trie_traverse_ppg_prefix
 * Purpose:
 *     calls back applicable payload object is affected by prefix updates
 * NOTE:
 *     other propagation code should always return SOC_E_NONE so that
 *     callback will happen on all pivot.
 */
int _trie_traverse_ppg_prefix(alpm_lib_trie_node_t *trie,
                            alpm_lib_trie_ppg_cb_f cb,
                            alpm_lib_trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE, index=0;
    int rv1 = SOC_E_NONE;

    if (!trie || !cb || !cb_info) {
        return SOC_E_PARAM;
    }

    /* call back the payload object if applicable */
    if (trieNodeTypePayload == trie->type) {
        rv = cb(trie, cb_info);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }

    for (index=0; index < 2; index++) {
        if (trie->child[index]) {
            rv = _trie_traverse_ppg_prefix(
                    trie->child[index], cb, cb_info);
            /* Save first error, second error can overwrite if it's more severe
               than the first. SOC_E_LIMIT is considered as no severe error */
            if (SOC_FAILURE(rv)) {
                if (rv1 == SOC_E_NONE || rv1 == SOC_E_LIMIT) {
                    rv1 = rv;
                }
            }
        }
    }

    return rv1;
}

/*
 * Function:
 *   _trie_ppg_prefix
 * Purpose:
 *   If the propogation starts from intermediate pivot on
 *   the trie, then the prefix length has to be appropriately adjusted or else
 *   it will end up with ill updates.
 *   Assumption: the prefix length is adjusted as per trie node on which
 *               is starts from.
 *   If node == head node then adjust is none
 *      node == pivot, then prefix length = org len - pivot len
 */
static int _trie_ppg_prefix(alpm_lib_trie_node_t *trie,
                    uint32 *pfx,
                    uint32 len,
                    alpm_lib_trie_ppg_cb_f cb,
                    alpm_lib_trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE; /*, index;*/
    uint32 bit = 0, lcp = 0;

    if (!trie || (len && trie->skip_len && !pfx) ||
        (len > _MAX_KEY_LEN_48_) || !cb || !cb_info) {
        return SOC_E_PARAM;
    }

    if (len > 0) {
        lcp = lcplen(pfx, len, trie->skip_addr, trie->skip_len);
        /* if the lcp is less than prefix length the prefix is not applicable
         * for any propagation */
        if (lcp < ((len>trie->skip_len) ? trie->skip_len : len)) {
            return SOC_E_NONE;
        } else {
            if (len > trie->skip_len) {
                bit = _alpm_lib_key_get_bits(pfx, len-lcp, 1);
                if (!trie->child[bit]) {
                    return SOC_E_NONE;
                }
                rv = _trie_ppg_prefix(
                        trie->child[bit],
                        pfx, len-lcp-1, cb, cb_info);
            } else {
                /* pfx is <= trie skip len */
                /* propagate if applicable */
                rv = _trie_traverse_ppg_prefix(trie, cb, cb_info);
                if (SOC_E_LIMIT == rv) {
                    rv = SOC_E_NONE;
                }
            }
        }
    } else {
        rv = _trie_traverse_ppg_prefix(trie, cb, cb_info);
        if (SOC_E_LIMIT == rv) {
            rv = SOC_E_NONE;
        }
    }

    return rv;
}

/*
 * Function:
 *      _alpm_lib_trie_ppg_prefix
 * Purpose:
 *      Propogate prefix from a given pivot.
 *      Callback function to decide INSERT/DELETE propagation,
 *               and decide to update bpm_len or not.
 */
int _alpm_lib_trie_ppg_prefix(alpm_lib_trie_node_t *pivot,
                              uint32 pivot_len,
                              uint32 *pfx,
                              uint32 len,
                              alpm_lib_trie_ppg_cb_f cb,
                              alpm_lib_trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE;

    len -= pivot_len;

    if (len > 0) {
        uint32 bit = _alpm_lib_key_get_bits(pfx, len, 1);
        if (pivot->child[bit]) {
            /* validate if the pivot provided is correct */
            rv = _trie_ppg_prefix_validate(pivot->child[bit],
                                                 pfx, len-1);
            if (SOC_SUCCESS(rv)) {
                rv = _trie_ppg_prefix(pivot->child[bit],
                                                pfx, len-1,
                                                cb, cb_info);
            }
        } /* else nop, nothing to propagate on this path end */
    } else {
        /* pivot == prefix */
        rv = _trie_ppg_prefix(pivot, pfx, pivot->skip_len,
                                        cb, cb_info);
    }

    return rv;
}

int alpm_lib_trie_ppg(alpm_lib_trie_t *trie, uint32 pvt_len,
                    uint32 *pfx,
                    uint32 len,
                    alpm_lib_trie_ppg_cb_f cb,
                    alpm_lib_trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE, rv2 = SOC_E_NONE;
    alpm_lib_trie_node_t *payload;
    alpm_lib_trie_node_type_t payload_type = trieNodeTypeMax;

    if (!pfx || !trie || !trie->trie || !cb || !cb_info) {
        return SOC_E_PARAM;
    }

    payload = alpm_util_alloc(sizeof(alpm_lib_trie_node_t), "trie_node");
    if (payload == NULL) {
        return SOC_E_MEMORY;
    }
    rv2 = alpm_lib_trie_insert(trie, pfx, len, payload);
    if (SOC_FAILURE(rv2)) {
        alpm_util_free(payload);
        if (rv2 != SOC_E_EXISTS) {
            return rv2;
        }
        rv = alpm_lib_trie_find_lpm(trie, pfx, len, &payload);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    } else {
        payload_type = payload->type;
        payload->type = trieNodeTypeInternalPpg;
    }

    if (trie->v6_key) {
        rv = _alpm_lib_trie_v6_ppg_prefix(payload, len, pfx, len,
                                          cb, cb_info);
    } else {
        rv = _alpm_lib_trie_ppg_prefix(payload, len, pfx, len,
                                       cb, cb_info);
    }

    if (payload_type != trieNodeTypeMax) {
        payload->type = payload_type;
    }

    if (SOC_SUCCESS(rv2)) {
        alpm_lib_trie_delete(trie, pfx, len, &payload);
        alpm_util_free(payload);
    }

    return rv;
}

/*
 * Function:
 *     alpm_lib_trie_init
 * Purpose:
 *     allocates a trie & initializes it
 */
int alpm_lib_trie_init(uint32 max_key_len, alpm_lib_trie_t **ptrie)
{
    alpm_lib_trie_t *trie = alpm_util_alloc(sizeof(alpm_lib_trie_t), "trie-node");
    sal_memset(trie, 0, sizeof(alpm_lib_trie_t));

    trie->v6_key = (max_key_len == _MAX_KEY_LEN_144_);
    trie->trie = NULL; /* means nothing is on teie */
    *ptrie = trie;
    return SOC_E_NONE;
}

/*
 * Function:
 *     alpm_lib_trie_destroy
 * Purpose:
 *     destroys a trie
 */
int alpm_lib_trie_destroy(alpm_lib_trie_t *trie)
{
    if (trie != NULL) {
        alpm_util_free(trie);
    }
    return SOC_E_NONE;
}

#else
typedef int bcm_esw_alpm2_alpm_lib_trie4_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
