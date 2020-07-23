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
#include <soc/types.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#ifdef ALPM_ENABLE
#ifdef BCM_TRIDENT2_SUPPORT

#include <shared/util.h>
#include <sal/appl/sal.h>
#include <sal/core/libc.h>
#include <sal/core/time.h>
#include <soc/esw/trie.h>
#include <soc/esw/trie_util.h>
#include <soc/esw/alpm_trie_v6.h>

#define _MAX_SKIP_LEN_  (31)
#define _MAX_KEY_LEN_   (48)

#define _MAX_KEY_WORDS_ (BITS2WORDS(_MAX_KEY_LEN_))

#define SHL(data, shift, max) \
    (((shift)>=(max))?0:((data)<<(shift)))

#define SHR(data, shift, max) \
    (((shift)>=(max))?0:((data)>>(shift)))

#define MASK(len) \
    (((len)>=32 || (len)==0)?0xFFFFFFFF:((1<<(len))-1))

#define BITMASK(len) \
    (((len)>=32)?0xFFFFFFFF:((1<<(len))-1))

#define ABS(n) ((((int)(n)) < 0) ? -(n) : (n))

#define _NUM_WORD_BITS_ (32)

#define BITS2SKIPOFF(x) (((x) + _MAX_SKIP_LEN_-1) / _MAX_SKIP_LEN_)

/* key packing expetations:
 * eg., 48 bit key
 * - 10/8 -> key[0]=0, key[1]=8
 * - 0x123456789a -> key[0] = 0x12 key[1] = 0x3456789a
 * length - represents number of valid bits from farther to lower index ie., 1->0 
 */

#define KEY_BIT2IDX(x) (((BITS2WORDS(_MAX_KEY_LEN_)*32) - (x))/32)


/* (internal) Generic operation macro on bit array _a, with bit _b */
#define	_BITOP(_a, _b, _op)	\
        ((_a) _op (1U << ((_b) % _NUM_WORD_BITS_)))

/* Specific operations */
#define	_BITGET(_a, _b)	_BITOP(_a, _b, &)
#define	_BITSET(_a, _b)	_BITOP(_a, _b, |=)
#define	_BITCLR(_a, _b)	_BITOP(_a, _b, &= ~)

/* get the bit position of the LSB set in bit 0 to bit "msb" of "data"
 * (max 32 bits), "lsb" is set to -1 if no bit is set in "data".
 */
#define BITGETLSBSET(data, msb, lsb)    \
    {                                    \
	lsb = 0;                         \
	while ((lsb)<=(msb)) {		 \
	    if ((data)&(1<<(lsb)))     { \
		break;                   \
	    } else { (lsb)++;}           \
	}                                \
	lsb = ((lsb)>(msb))?-1:(lsb);    \
    }
#define KEY_BIT2IDX(x) (((BITS2WORDS(_MAX_KEY_LEN_)*32) - (x))/32)

/********************************************************/
/* Get a chunk of bits from a key (MSB bit - on word0, lsb on word 1).. 
 */
unsigned int _key_get_bits(unsigned int *key, 
                           unsigned int pos /* 1based, msb bit position */, 
                           unsigned int len,
                           unsigned int skip_len_check)
{
    unsigned int val=0, delta=0, diff, bitpos;

    /* coverity[var_deref_op : FALSE] */
    if (!key || (pos < 1) || (pos > _MAX_KEY_LEN_) || 
        ((skip_len_check == 0) && (len > _MAX_SKIP_LEN_))) assert(0);

    bitpos = (pos-1) % _NUM_WORD_BITS_;
    bitpos++; /* 1 based */

    if (bitpos >= len) {
        diff = bitpos - len;
        /* coverity[var_deref_op : FALSE] */
        val = SHR(key[KEY_BIT2IDX(pos)], diff, _NUM_WORD_BITS_);
        val &= MASK(len);
        return val;
    } else {
        diff = len - bitpos;
        if (skip_len_check==0) assert(diff <= _MAX_SKIP_LEN_);
        /* coverity[var_deref_op : FALSE] */
        val = key[KEY_BIT2IDX(pos)] & MASK(bitpos);
        val = SHL(val, diff, _NUM_WORD_BITS_);
        /* get bits from next word */
        delta = _key_get_bits(key, pos-bitpos, diff, skip_len_check);
        return (delta | val);
    }
}		


/* 
 * Assumes the layout for 
 * 0 - most significant word
 * _MAX_KEY_WORDS_ - least significant word
 * eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
int _key_shift_left(unsigned int *key, unsigned int shift)
{
    unsigned int index=0;

    if (!key || shift > _MAX_SKIP_LEN_) return SOC_E_PARAM;

    for(index=KEY_BIT2IDX(_MAX_KEY_LEN_); index < KEY_BIT2IDX(1); index++) {
        key[index] = SHL(key[index], shift,_NUM_WORD_BITS_) | \
                     SHR(key[index+1],_NUM_WORD_BITS_-shift,_NUM_WORD_BITS_);
    }

    key[index] = SHL(key[index], shift, _NUM_WORD_BITS_);

    /* mask off snippets bit on MSW */
    key[0] &= MASK(_MAX_KEY_LEN_ % _NUM_WORD_BITS_);
    return SOC_E_NONE;
}

/* 
 * Assumes the layout for 
 * 0 - most significant word
 * _MAX_KEY_WORDS_ - least significant word
 * eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
int _key_shift_right(unsigned int *key, unsigned int shift)
{
    unsigned int index=0;

    if (!key || shift > _MAX_SKIP_LEN_) return SOC_E_PARAM;

    for(index=KEY_BIT2IDX(1); index > KEY_BIT2IDX(_MAX_KEY_LEN_); index--) {
        key[index] = SHR(key[index], shift,_NUM_WORD_BITS_) | \
                     SHL(key[index-1],_NUM_WORD_BITS_-shift,_NUM_WORD_BITS_);
    }

    key[index] = SHR(key[index], shift, _NUM_WORD_BITS_);

    /* mask off snippets bit on MSW */
    key[0] &= MASK(_MAX_KEY_LEN_ % _NUM_WORD_BITS_);
    return SOC_E_NONE;
}


/* 
 * Assumes the layout for 
 * 0 - most significant word
 * _MAX_KEY_WORDS_ - least significant word
 * eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
int _key_append(unsigned int *key, 
                unsigned int *length,
                unsigned int skip_addr,
                unsigned int skip_len)
{
    int rv=SOC_E_NONE;

    if (!key || !length || (skip_len + *length > _MAX_KEY_LEN_) || skip_len > _MAX_SKIP_LEN_ ) return SOC_E_PARAM;

    rv = _key_shift_left(key, skip_len);
    if (SOC_SUCCESS(rv)) {
        key[KEY_BIT2IDX(1)] |= skip_addr;
        *length += skip_len;
    }

    return rv;
}

int _bpm_append(unsigned int *key, 
                unsigned int *length,
                unsigned int skip_addr,
                unsigned int skip_len)
{
    int rv=SOC_E_NONE;

    if (!key || !length || (skip_len + *length > _MAX_KEY_LEN_) || skip_len > (_MAX_SKIP_LEN_+1) ) return SOC_E_PARAM;

    if (skip_len == 32) {
	key[0] = key[1];
	key[1] = skip_addr;
	*length += skip_len;
    } else {
	rv = _key_shift_left(key, skip_len);
	if (SOC_SUCCESS(rv)) {
	    key[KEY_BIT2IDX(1)] |= skip_addr;
	    *length += skip_len;
	}
    }

    return rv;
}

/*
 * Function:
 *     lcplen
 * Purpose:
 *     returns longest common prefix length provided a key & skip address
 */
unsigned int
lcplen(unsigned int *key, unsigned int len1,
       unsigned int skip_addr, unsigned int len2)
{
    unsigned int diff;
    unsigned int lcp = len1 < len2 ? len1 : len2;

    if ((len1 > _MAX_KEY_LEN_) || (len2 > _MAX_KEY_LEN_)) {
	LOG_CLI((BSL_META("len1 %d or len2 %d is larger than %d\n"),
                 len1, len2, _MAX_KEY_LEN_));
	assert(0);
    } 

    if (len1 == 0 || len2 == 0) return 0;

    diff = _key_get_bits(key, len1, lcp, 0);
    diff ^= (SHR(skip_addr, len2 - lcp, _MAX_SKIP_LEN_) & MASK(lcp));

    while (diff) {
        diff >>= 1;
        --lcp;
    }

    return lcp;
}

int _print_trie_node(trie_node_t *trie, void *datum)
{
    if (trie != NULL) {

	LOG_CLI((BSL_META("trie: %p, type %s, skip_addr 0x%x skip_len %d "
                          "count:%d bpm:0x%x Child[0]:%p Child[1]:%p\n"),
                 trie, (trie->type == PAYLOAD)?"P":"I",
                 trie->skip_addr, trie->skip_len, 
                 trie->count, trie->bpm, trie->child[0].child_node,
                 trie->child[1].child_node));
    }
    return SOC_E_NONE;
}

STATIC int _trie_preorder_traverse(trie_node_t *trie, trie_callback_f cb, void *user_data)
{
    int rv = SOC_E_NONE;
    trie_node_t *tmp1, *tmp2;

    if (trie == NULL || !cb) {
	return SOC_E_NONE;
    } else {
        /* make the node delete safe */
        tmp1 = trie->child[0].child_node;
        tmp2 = trie->child[1].child_node;
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

STATIC int _trie_postorder_traverse(trie_node_t *trie, trie_callback_f cb, void *user_data)
{
    int rv = SOC_E_NONE;

    if (trie == NULL) {
	return SOC_E_NONE;
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_postorder_traverse(trie->child[0].child_node, cb, user_data);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_postorder_traverse(trie->child[1].child_node, cb, user_data);
    }
    if (SOC_SUCCESS(rv)) {
        rv = cb(trie, user_data);
    }
    return rv;
}

STATIC int _trie_inorder_traverse(trie_node_t *trie, trie_callback_f cb, void *user_data)
{
    int rv = SOC_E_NONE;
    trie_node_t *tmp = NULL;

    if (trie == NULL) {
	return SOC_E_NONE;
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_inorder_traverse(trie->child[0].child_node, cb, user_data);
    }

    /* make the trie pointers delete safe */
    tmp = trie->child[1].child_node;

    if (SOC_SUCCESS(rv)) {
        rv = cb(trie, user_data);
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_inorder_traverse(tmp, cb, user_data);
    }
    return rv;
}

STATIC int _trie_traverse(trie_node_t *trie, trie_callback_f cb, 
			  void *user_data,  trie_traverse_order_e_t order)
{
    int rv = SOC_E_NONE;

    switch(order) {
    case _TRIE_PREORDER_TRAVERSE:
        rv = _trie_preorder_traverse(trie, cb, user_data);
        break;
    case _TRIE_POSTORDER_TRAVERSE:
        rv = _trie_postorder_traverse(trie, cb, user_data);
        break;
    case _TRIE_INORDER_TRAVERSE:
        rv = _trie_inorder_traverse(trie, cb, user_data);
        break;
    default:
        assert(0);
    }

    return rv;
}

/*
 * Function:
 *     trie_traverse
 * Purpose:
 *     Traverse the trie & call the application callback with user data 
 */
int trie_traverse(trie_t *trie, trie_callback_f cb, 
                  void *user_data, trie_traverse_order_e_t order)
{
    if (order < _TRIE_PREORDER_TRAVERSE ||
        order >= _TRIE_TRAVERSE_MAX || !cb) return SOC_E_PARAM;

    if (trie == NULL) {
	return SOC_E_NONE;
    } else {
        return _trie_traverse(trie->trie, cb, user_data, order);
    }
}

#define TRIE_TRAVERSE_STOP(state, rv) \
    {if (state == TRIE_TRAVERSE_STATE_DONE || rv < 0) {return rv;} }

STATIC int _trie_preorder_traverse2(trie_node_t *ptrie,
                                    trie_node_t *trie,
                                    trie_traverse_states_e_t *state,
                                    trie_callback_ext_f cb,
                                    void *user_data)
{
    int rv = SOC_E_NONE;
    trie_node_t *lc, *rc;

    if (trie == NULL || !cb) {
        return SOC_E_NONE;
    } else {
        assert(!ptrie || ptrie->type == PAYLOAD);

        /* make the trie delete safe */
        lc = trie->child[0].child_node;
        rc = trie->child[1].child_node;
        if (trie->type == PAYLOAD) { /* no need to callback on internal nodes */
            rv = cb(ptrie, trie, state, user_data);
            TRIE_TRAVERSE_STOP(*state, rv);

            /* Change the ptrie as trie if applicable */
            /* make the ptrie delete safe */
            if (*state != TRIE_TRAVERSE_STATE_DELETED) {
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

STATIC int _trie_postorder_traverse2(trie_node_t *ptrie,
                                    trie_node_t *trie,
                                    trie_traverse_states_e_t *state,
                                    trie_callback_ext_f cb,
                                    void *user_data)
{
    int rv = SOC_E_NONE;
    trie_node_t *ori_ptrie = ptrie;
    trie_node_t *lc, *rc;
    node_type_e_t trie_type;
    if (trie == NULL) {
        return SOC_E_NONE;
    }

    assert(!ptrie || ptrie->type == PAYLOAD);

    /* Change the ptrie as trie if applicable */
    if (trie->type == PAYLOAD) {
        ptrie = trie;
    }

    /* During the callback, a trie node can be deleted or inserted.
     * For a deleted node, its internal parent could also be deleted, thus to
     * make it safe we should record rc.
     */
    trie_type = trie->type;
    lc = trie->child[0].child_node;
    rc = trie->child[1].child_node;

    if (SOC_SUCCESS(rv)) {
        rv = _trie_postorder_traverse2(ptrie, lc, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_postorder_traverse2(ptrie, rc, state, cb, user_data);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        if (trie_type == PAYLOAD) {
            rv = cb(ori_ptrie, trie, state, user_data);
        }
    }
    return rv;
}

STATIC int _trie_inorder_traverse2(trie_node_t *ptrie,
                                   trie_node_t *trie,
                                   trie_traverse_states_e_t *state,
                                   trie_callback_ext_f cb,
                                   void *user_data)
{
    int rv = SOC_E_NONE;
    trie_node_t *rc = NULL;
    trie_node_t *ori_ptrie = ptrie;

    if (trie == NULL) {
        return SOC_E_NONE;
    }

    assert(!ptrie || ptrie->type == PAYLOAD);

    /* Change the ptrie as trie if applicable */
    if (trie->type == PAYLOAD) {
        ptrie = trie;
    }

    rv = _trie_inorder_traverse2(ptrie, trie->child[0].child_node, state, cb, user_data);
    TRIE_TRAVERSE_STOP(*state, rv);

    /* make the trie delete safe */
    rc = trie->child[1].child_node;

    if (SOC_SUCCESS(rv)) {
        if (trie->type == PAYLOAD) {
            rv = cb(ptrie, trie, state, user_data);
            TRIE_TRAVERSE_STOP(*state, rv);
            /* make the ptrie delete safe */
            if (*state == TRIE_TRAVERSE_STATE_DELETED) {
                ptrie = ori_ptrie;
            }
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_inorder_traverse2(ptrie, rc, state, cb, user_data);
    }
    return rv;
}


STATIC int _trie_traverse2(trie_node_t *trie, trie_callback_ext_f cb,
                           void *user_data,  trie_traverse_order_e_t order,
                           trie_traverse_states_e_t *state)
{
    int rv = SOC_E_NONE;

    switch(order) {
        case _TRIE_PREORDER_TRAVERSE:
            rv = _trie_preorder_traverse2(NULL, trie, state, cb, user_data);
            break;
        case _TRIE_POSTORDER_TRAVERSE:
            rv = _trie_postorder_traverse2(NULL, trie, state, cb, user_data);
            break;
        case _TRIE_INORDER_TRAVERSE:
            rv = _trie_inorder_traverse2(NULL, trie, state, cb, user_data);
            break;
        default:
            assert(0);
    }

    return rv;
}

/*
 * Function:
 *     trie_traverse2
 * Purpose:
 *     Traverse the trie (PAYLOAD) & call the extended application callback
 *     which has current node's PAYLOAD parent node with user data.
 */
int trie_traverse2(trie_t *trie, trie_callback_ext_f cb,
                   void *user_data, trie_traverse_order_e_t order)
{
    trie_traverse_states_e_t state = TRIE_TRAVERSE_STATE_NONE;

    if (order < _TRIE_PREORDER_TRAVERSE ||
        order >= _TRIE_TRAVERSE_MAX || !cb) return SOC_E_PARAM;

    if (trie == NULL) {
        return SOC_E_NONE;
    } else {
        return _trie_traverse2(trie->trie, cb, user_data, order, &state);
    }
}



typedef struct trie_list_s {
    trie_node_t *node;
    struct trie_list_s *next;
} trie_list_t;

#ifdef REPART_DEBUG
int repart_debug = 0;
trie_t *repart_trie = (trie_t *) 0xFFFFFFFF;
#endif

STATIC int _trie_repartition(trie_node_t *ptrie,
                             trie_node_t *trie,
                             trie_traverse_states_e_t *state,
                             trie_repartition_callback_f cb,
                             void *user_data,
                             trie_list_t **ptrie_list_in_out,
                             int level,
                             int *repart_count,
                             int *merge_count)
{
    int rv = SOC_E_NONE;
    node_type_e_t   curr_trie_type;
    trie_node_t *rc = NULL;
    trie_node_t *new_ptrie = NULL;       /* new ptrie from callback, to be added to list. */
    trie_node_t *further_ptrie = NULL;   /* ptrie for recursion */
    trie_node_t *latest_ptrie = NULL;    /* ptrie for callback */
    trie_list_t *ptrie_list = NULL;      /* always the update-to-date ptrie list */
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
#ifdef REPART_DEBUG
    int lvl;
#endif
    int l_prune_count = 0, r_prune_count = 0;   /* pruning counts */
    int l_stay_count = 0, r_stay_count = 0;     /* staying counts */
    if (trie == NULL) {
        return SOC_E_NONE;
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
            while(ptrie_tail->next) {
                ptrie_tail = ptrie_tail->next;
            }
        }
    }
#ifdef REPART_DEBUG
    if (repart_debug) {
        lvl = level;
        while(lvl) {
            LOG_CLI((BSL_META("%x  "),level - lvl));
            lvl--;
        }
        LOG_CLI(("#Ptrie %p, trie %p\n", ptrie, trie));
    }
#endif

#define KEEP_OP >=
#define PRUNE_OP <

    if (curr_trie_type == PAYLOAD) {
        further_ptrie = trie;
    } else if (ptrie_list) {
        further_ptrie = ptrie_tail->node;
        /* assert(ptrie_tail->node->count <= ptrie->count); */
    } else {
        further_ptrie = ptrie;
    }

    rv = _trie_repartition(further_ptrie, trie->child[0].child_node,
                           state, cb, user_data,
                           (curr_trie_type == PAYLOAD) ? NULL : &l_ptrie_list, level+1, &lrc, &lmc);


    /* Note, the ptrie_list could be updated */
    TRIE_TRAVERSE_STOP(*state, rv);

    /* Somehow, the _trie_repartition may get us some new payload nodes,
     * these new payload nodes can be many, and they can be lower than the trie, 
     * or higher than it, but all must be lower than ptrie. The child cannot
     * decide status of each, only the current node can.
     */

    /* Prune unnecessary heads in the list (assuming the lower/longer comes first)*/
    tmc += lmc;

    l_prune_count = 0;
    l_stay_count = 0;
    l_ptrie_tail = NULL;
    ptrie_list_iter = l_ptrie_list;
#ifdef REPART_DEBUG
    if (repart_debug) {
        if (l_ptrie_list) {
            lvl = level;
            while(lvl) {
                LOG_CLI((BSL_META("%x  "),level - lvl));
                lvl--;
            }

            LOG_CLI(("After LC Pruning:"));
            while (ptrie_list_iter) {
                ptrie_list_next = ptrie_list_iter->next;
                if (ptrie_list_iter->node->count KEEP_OP (curr_trie_count - tmc - l_stay_count)) {
                    l_ptrie_tail = ptrie_list_iter;
                    l_stay_count++;
                    assert(l_prune_count == 0);
                } else {
                    l_prune_count++;
                    LOG_CLI(("%p  ", ptrie_list_iter->node));
                    sal_free(ptrie_list_iter);

                }
                ptrie_list_iter = ptrie_list_next;
            }
            if (l_ptrie_tail) {
                l_ptrie_tail->next = NULL;
            } else {
                l_ptrie_list = NULL;
            }

            LOG_CLI(("\n"));

            ptrie_list_iter = l_ptrie_list;
            lvl = level;
            while(lvl) {
                LOG_CLI((BSL_META("%x  "),level - lvl));
                lvl--;
            }
            LOG_CLI(("After LC Pruned: "));
            while(ptrie_list_iter) {
                LOG_CLI(("%p -> ", ptrie_list_iter->node));
                ptrie_list_iter = ptrie_list_iter->next;
            }
            LOG_CLI(("NULL \n"));
        }
    } else
#endif
    {
        while (ptrie_list_iter) {
            ptrie_list_next = ptrie_list_iter->next;
            if (ptrie_list_iter->node->count KEEP_OP (curr_trie_count - tmc - l_stay_count)) {
                l_ptrie_tail = ptrie_list_iter;
                l_stay_count++;
                /* assert(l_prune_count == 0); */
            } else {
                l_prune_count++;
                sal_free(ptrie_list_iter);
            }
            ptrie_list_iter = ptrie_list_next;
        }
        if (l_ptrie_tail) {
            l_ptrie_tail->next = NULL;
        } else {
            l_ptrie_list = NULL;
        }
    }

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

    if (SOC_SUCCESS(rv)) {
        rv = _trie_repartition(further_ptrie, rc, state, cb, user_data,
                               (curr_trie_type == PAYLOAD) ? NULL : &r_ptrie_list,
                               level+1, &rrc, &rmc);

        TRIE_TRAVERSE_STOP(*state, rv);
        tmc += rmc;

        /* Prune unnecessary heads in the list (assuming the lower/longer comes first)*/
        r_prune_count = 0;
        r_stay_count = 0;
        r_ptrie_tail = NULL;
        ptrie_list_iter = r_ptrie_list;
#ifdef REPART_DEBUG
        if (repart_debug) {
            if (r_ptrie_list) {
                lvl = level;
                while(lvl) {
                    LOG_CLI((BSL_META("%x  "),level - lvl));
                    lvl--;
                }

                LOG_CLI(("After RC Pruning:"));
                while (ptrie_list_iter) {
                    ptrie_list_next = ptrie_list_iter->next;
                    if (ptrie_list_iter->node->count KEEP_OP
                        (curr_trie_count - tmc - lrc - r_stay_count)) {
                        r_ptrie_tail = ptrie_list_iter;
                        r_stay_count++;
                        assert(r_prune_count == 0);
                    } else {
                        r_prune_count++;
                        LOG_CLI(("%p  ", ptrie_list_iter->node));
                        sal_free(ptrie_list_iter);
                    }
                    ptrie_list_iter = ptrie_list_next;
                }
                if (r_ptrie_tail) {
                    r_ptrie_tail->next = NULL;
                } else {
                    /* whole list cleared */
                    r_ptrie_list = NULL;
                }

                LOG_CLI(("\n"));

                ptrie_list_iter = r_ptrie_list;
                lvl = level;
                while(lvl) {
                    LOG_CLI((BSL_META("%x  "),level - lvl));
                    lvl--;
                }
                LOG_CLI(("After RC Pruned: "));
                while(ptrie_list_iter) {
                    LOG_CLI(("%p -> ", ptrie_list_iter->node));
                    ptrie_list_iter = ptrie_list_iter->next;
                }
                LOG_CLI(("NULL \n"));
            }
        } else
#endif
        {
            while (ptrie_list_iter) {
                ptrie_list_next = ptrie_list_iter->next;
                if (ptrie_list_iter->node->count KEEP_OP
                    (curr_trie_count - tmc - lrc - r_stay_count)) {
                    r_ptrie_tail = ptrie_list_iter;
                    r_stay_count++;
                    /* assert(r_prune_count == 0); */
                } else {
                    r_prune_count++;
                    sal_free(ptrie_list_iter);

                }
                ptrie_list_iter = ptrie_list_next;
            }
            if (r_ptrie_tail) {
                r_ptrie_tail->next = NULL;
            } else {
                /* whole list cleared */
                r_ptrie_list = NULL;
            }
        }

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

    }

    if (SOC_SUCCESS(rv) && curr_trie_type == PAYLOAD) {
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
            new_ptrie_list = sal_alloc(sizeof(trie_list_t), "trie list");
            sal_memset(new_ptrie_list, 0, sizeof(trie_list_t));
            new_ptrie_list->node = new_ptrie;

            if (ptrie_list) {
                ptrie_tail->next = new_ptrie_list;
                ptrie_tail = new_ptrie_list;
            } else {
                ptrie_list = new_ptrie_list;
            }

#ifdef REPART_DEBUG
            if (repart_debug) {
                lvl = level;
                while(lvl) {
                    LOG_CLI((BSL_META("%x  "),level - lvl));
                    lvl--;
                }

                LOG_CLI(("New Head %p. Now: ", new_ptrie));
                ptrie_list_iter = ptrie_list;
                while(ptrie_list_iter) {
                    LOG_CLI(("%p -> ", ptrie_list_iter->node));
                    ptrie_list_iter = ptrie_list_iter->next;
                }
                LOG_CLI(("NULL. Updated trie:\n"));
                trie_dump(repart_trie, 0, 0);
            }
#endif
            trc++;
        } else if (*state == TRIE_TRAVERSE_STATE_DELETED) {
#ifdef REPART_DEBUG
            if (repart_debug) {
                lvl = level;
                while(lvl) {
                    LOG_CLI((BSL_META("%x  "),level - lvl));
                    lvl--;
                }

                LOG_CLI(("Node %p merged (deleted).  Updated trie:\n", trie));
                trie_dump(repart_trie, 0, 0);
            }
#endif
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
#ifdef REPART_DEBUG
            if (repart_debug) {
                lvl = level;
                while(lvl) {
                    LOG_CLI((BSL_META("%x  "),level - lvl));
                    lvl--;
                }
                LOG_CLI((" Clear Prune %p . trc %d. \n",
                    ptrie_list_iter->node, trc));

                if (ptrie_list_iter->node->count != (curr_trie_count - tmc - tmp)) {
                    LOG_CLI((" Count %d . Curr %d tmc %d tmp %d. \n",
                        ptrie_list_iter->node->count, curr_trie_count, tmc, tmp));
                    assert(0);
                }
            }
#endif
            sal_free(ptrie_list_iter);
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
    return rv;
}
/*
 * Function:
 *     trie_repartition
 * Purpose:
 *     Traverse the trie (PAYLOAD) & call the extended application callback
 *     which has current node's PAYLOAD parent node with user data.
 */
int trie_repartition(trie_t *trie, trie_repartition_callback_f cb,
                     void *user_data, trie_traverse_order_e_t order)
{
    trie_traverse_states_e_t state = TRIE_TRAVERSE_STATE_NONE;

    if (order < _TRIE_PREORDER_TRAVERSE ||
        order >= _TRIE_TRAVERSE_MAX || !cb) return SOC_E_PARAM;

#ifdef REPART_DEBUG
    if (repart_debug) {
        repart_trie = trie;
        LOG_CLI(("=====================\n"));
        trie_dump(repart_trie, 0, 0);
    }
#endif
    if (trie == NULL) {
        return SOC_E_NONE;
    } else {
        return _trie_repartition(NULL, trie->trie, &state, cb, user_data, NULL, 0, NULL, NULL);
    }
}



STATIC int _trie_preorder_iter_get_first(trie_node_t *node, trie_node_t **payload)
{
    int rv = SOC_E_NONE;

    if (!payload) return SOC_E_PARAM;

    if (*payload != NULL) return SOC_E_NONE;

    if (node == NULL) {
	return SOC_E_NONE;
    } else {
        if (node->type == PAYLOAD) {
            *payload = node;
            return rv;
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_preorder_iter_get_first(node->child[0].child_node, payload);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_preorder_iter_get_first(node->child[1].child_node, payload);
    }
    return rv;
}

/*
 * Function:
 *     trie_iter_get_first
 * Purpose:
 *     Traverse the trie & return pointer to first payload node
 */
int trie_iter_get_first(trie_t *trie, trie_node_t **payload)
{
    int rv = SOC_E_EMPTY;

    if (!trie || !payload) return SOC_E_PARAM;

    if (trie && trie->trie) {
        *payload = NULL;
        return _trie_preorder_iter_get_first(trie->trie, payload);
    }

    return rv;
}

STATIC int _trie_dump(trie_node_t *trie, trie_callback_f cb, 
		      void *user_data, unsigned int level)
{
    if (trie == NULL) {
	return SOC_E_NONE;
    } else {
        unsigned int lvl = level;
	while(lvl) {
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
            _print_trie_node(trie, NULL);
        }
    }

    _trie_dump(trie->child[0].child_node, cb, user_data, level+1);
    _trie_dump(trie->child[1].child_node, cb, user_data, level+1);
    return SOC_E_NONE;
}

/*
 * Function:
 *     trie_dump
 * Purpose:
 *     Dumps the trie pre-order [root|left|child]
 */
int trie_dump(trie_t *trie, trie_callback_f cb, void *user_data)
{
    if (trie->trie) {
	return _trie_dump(trie->trie, cb, user_data, 0);
    } else {
        return SOC_E_PARAM;
    }
}

STATIC int _trie_search(trie_node_t *trie,
			unsigned int *key,
			unsigned int length,
			trie_node_t **payload,
			unsigned int *result_key,
			unsigned int *result_len,
			unsigned int dump,
			unsigned int find_pivot)
{
    unsigned int lcp=0;
    int bit=0, rv=SOC_E_NONE;

    if (!trie || (length && trie->skip_len && !key)) return SOC_E_PARAM;
    if ((result_key && !result_len) || (!result_key && result_len)) return SOC_E_PARAM;

    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    if (dump) {
        _print_trie_node(trie, (unsigned int *)1);
    }

    if (length > trie->skip_len) {
        if (lcp == trie->skip_len) {
            bit = (key[KEY_BIT2IDX(length - lcp)] & \
                   (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;
            if (dump) {
                LOG_CLI((BSL_META(" Length: %d Next-Bit[%d] \n"), length, bit));
            }

            if (result_key) {
                rv = _key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
                if (SOC_FAILURE(rv)) return rv;
            }

            /* based on next bit branch left or right */
            if (trie->child[bit].child_node) {

                if (result_key) {
                    rv = _key_append(result_key, result_len, bit, 1);
                    if (SOC_FAILURE(rv)) return rv;
                }

                return _trie_search(trie->child[bit].child_node, key, 
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
            if (dump) {
                LOG_CLI((BSL_META(": MATCH \n")));
            }
            *payload = trie;
	    if (trie->type != PAYLOAD && !find_pivot) {
		/* no assert here, possible during dbucket search
		 * due to 1* and 0* bucket search
		 */
		return SOC_E_NOT_FOUND;
	    }
            if (result_key) {
                rv = _key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
                if (SOC_FAILURE(rv)) return rv;
            }
            return SOC_E_NONE;
        }
        else return SOC_E_NOT_FOUND;
    } else {
        if (lcp == length && find_pivot) {
            *payload = trie;
            if (result_key) {
                rv = _key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
                if (SOC_FAILURE(rv)) return rv;
            }
            return SOC_E_NONE;
        }
        return SOC_E_NOT_FOUND; /* not found */
    }
}

/*
 * Function:
 *     trie_search
 * Purpose:
 *     Search the given trie for exact match of provided prefix/length
 *     If dump is set to 1 it traces the path as it traverses the trie 
 */
int trie_search(trie_t *trie, 
                unsigned int *key, 
                unsigned int length,
                trie_node_t **payload)
{
    if (trie->trie) {
	if (trie->v6_key) {
	    return _trie_v6_search(trie->trie, key, length, payload, NULL, NULL, 0, 0);	    
	} else {
	    return _trie_search(trie->trie, key, length, payload, NULL, NULL, 0, 0);
	}
    } else {
        return SOC_E_NOT_FOUND;
    }
}

/*
 * Function:
 *     trie_search_verbose
 * Purpose:
 *     Search the given trie for provided prefix/length
 *     If dump is set to 1 it traces the path as it traverses the trie 
 */
int trie_search_verbose(trie_t *trie, 
                        unsigned int *key, 
                        unsigned int length,
                        trie_node_t **payload,
                        unsigned int *result_key, 
                        unsigned int *result_len)
{
    if (trie->trie) {
	if (trie->v6_key) {
	    return _trie_v6_search(trie->trie, key, length, payload, result_key, result_len, 0, 0);
	} else {
	    return _trie_search(trie->trie, key, length, payload, result_key, result_len, 0, 0);
	}
    } else {
        return SOC_E_NOT_FOUND;
    }
}

/*
 * Internal function for LPM match searching.
 * callback on all payload nodes if cb != NULL.
 */
STATIC int _trie_find_lpm(trie_node_t *trie,
			  unsigned int *key,
			  unsigned int length,
			  trie_node_t **payload,
			  trie_callback_f cb,
			  void *user_data,
			  unsigned int exclude_self)
{
    unsigned int lcp=0;
    int bit=0, rv=SOC_E_NONE;

    if (!trie || (length && trie->skip_len && !key)) return SOC_E_PARAM;

    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    if ((length > trie->skip_len) && (lcp == trie->skip_len)) {
        if (trie->type == PAYLOAD) {
	    /* lpm cases */
	    if (payload != NULL) {
		/* update lpm result */
		*payload = trie;
	    }

	    if (cb != NULL) {
		/* callback with any nodes which is shorter and matches the prefix */
		rv = cb(trie, user_data);
		if (SOC_FAILURE(rv)) {
		    /* early bailout if there is error in callback handling */
		    return rv;
		}
	    }
	}

        bit = (key[KEY_BIT2IDX(length - lcp)] & \
               (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

        /* based on next bit branch left or right */
        if (trie->child[bit].child_node) {
            return _trie_find_lpm(trie->child[bit].child_node, key, length - lcp - 1,
				  payload, cb, user_data, exclude_self);
        } 
    } else if ((length == trie->skip_len) && (lcp == length)) {
        if (trie->type == PAYLOAD) {
	    /* exact match case */
	    if (payload != NULL && !exclude_self) {
		/* lpm is exact match */
		*payload = trie;
	    }

	    if (cb != NULL) {
		/* callback with the exact match node */
		rv = cb(trie, user_data);
		if (SOC_FAILURE(rv)) {
		    /* early bailout if there is error in callback handling */
		    return rv;
		}		
	    }
        }
    }
    return rv;
}

/*
 * Function:
 *     trie_find_lpm
 * Purpose:
 *     Find the longest prefix matched with given prefix 
 */
int trie_find_lpm(trie_t *trie, 
                  unsigned int *key, 
                  unsigned int length,
                  trie_node_t **payload)
{
    int rv = SOC_E_NONE;

    *payload = NULL;

    if (trie->trie) {
        if (trie->v6_key) {
            rv = _trie_v6_find_lpm(trie->trie, key, length, payload,
                                   NULL, NULL, 0);
        } else {
            rv = _trie_find_lpm(trie->trie, key, length, payload,
                                NULL, NULL, 0);
        }
        if (*payload || (rv != SOC_E_NONE)) {
            return rv;
        }
    }

    return SOC_E_NOT_FOUND;
}



/*
 * Function:
 *     trie_find_lpm2
 * Purpose:
 *     Find the longest prefix matched with given prefix
 */
int trie_find_lpm2(trie_t *trie,
                   unsigned int *key,
                   unsigned int length,
                   trie_node_t **payload)
{
    int rv = SOC_E_NONE;

    *payload = NULL;

    if (trie->trie) {
        if (trie->v6_key) {
            rv = _trie_v6_find_lpm(trie->trie, key, length, payload,
                                   NULL, NULL, 1);
        } else {
            rv = _trie_find_lpm(trie->trie, key, length, payload,
                                NULL, NULL, 1);
        }
        if (*payload || (rv != SOC_E_NONE)) {
            return rv;
        }
    }

    return SOC_E_NOT_FOUND;
}


/*
 * Function:
 *     trie_find_pm
 * Purpose:
 *     Find the prefix matched nodes with given prefix and callback
 *     with specified callback funtion and user data
 */
int trie_find_pm(trie_t *trie, 
		 unsigned int *key, 
		 unsigned int length,
		 trie_callback_f cb,
		 void *user_data)
{

    if (trie->trie) {
	if (trie->v6_key) {
	    return _trie_v6_find_lpm(trie->trie, key, length, NULL, cb, user_data, 0);	
	} else {
	    return _trie_find_lpm(trie->trie, key, length, NULL, cb, user_data, 0);
	}
    }

    return SOC_E_NONE;
}

/* trie->bpm format:
 * bit 0 is for the pivot itself (longest)
 * bit skip_len is for the trie branch leading to the pivot node (shortest)
 * bits (0-skip_len) is for the routes in the parent node's bucket
 */
int _trie_find_bpm(trie_node_t *trie,
                   unsigned int *key,
                   unsigned int length,
		   int *bpm_length)
{
    unsigned int lcp=0, local_bpm_mask=0;
    int bit=0, rv=SOC_E_NONE, local_bpm=0;

    if (!trie || (length && trie->skip_len && !key) ||
        (length > _MAX_KEY_LEN_)) return SOC_E_PARAM;

    /* calculate number of matching msb bits */
    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    if (length > trie->skip_len) {
	if (lcp == trie->skip_len) {
	    /* fully matched and more bits to check, go down the trie */
	    bit = (key[KEY_BIT2IDX(length - lcp)] &			\
		   (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;
	    
	    if (trie->child[bit].child_node) {
		rv = _trie_find_bpm(trie->child[bit].child_node, key, length - lcp - 1, bpm_length);
		/* on the way back, start bpm_length accumulation when encounter first non-0 bpm */
		if (*bpm_length >= 0) {
		    /* child node has non-zero bpm, just need to accumulate skip_len and branch bit */
		    *bpm_length += (trie->skip_len+1);
		    return rv;
		} else if (trie->bpm & BITMASK(trie->skip_len+1)) {
		    /* first non-zero bmp on the way back */
		    BITGETLSBSET(trie->bpm, trie->skip_len, local_bpm);
		    if (local_bpm >= 0) {
                        *bpm_length = trie->skip_len - local_bpm;
		    }
		}
		/* on the way back, and so far all bpm are 0 */
		return rv;
	    }
	}
    }

    /* no need to go further, we find whatever bits matched and 
     * check that part of the bpm mask
     */
    /* coverity[large_shift : FALSE] */
    local_bpm_mask = trie->bpm & (~(BITMASK(trie->skip_len-lcp)));
    if (local_bpm_mask & BITMASK(trie->skip_len+1)) {
	/* first non-zero bmp on the way back */
	BITGETLSBSET(local_bpm_mask, trie->skip_len, local_bpm);
	if (local_bpm >= 0) {
	    *bpm_length = trie->skip_len - local_bpm;
	}
    }

    return rv;
}

/*
 * Function:
 *     trie_find_prefix_bpm
 * Purpose:
 *    Given a key/length return the Best prefix match length
 *    key/bpm_pfx_len will be the BPM for the key/length
 *    using the bpm info in the trie database
 */
int trie_find_prefix_bpm(trie_t *trie, 
                         unsigned int *key, 
                         unsigned int length,
                         unsigned int *bpm_pfx_len)
{
    /* Return: SOC_E_EMPTY is not bpm bit is found */
    int rv = SOC_E_EMPTY, bpm=0;

    if (!trie || !key || !bpm_pfx_len ) {
	return SOC_E_PARAM;
    }

    bpm = -1;
    if (trie->trie) {
	if (trie->v6_key) {
	    rv = _trie_v6_find_bpm(trie->trie, key, length, &bpm);
	} else {
	    rv = _trie_find_bpm(trie->trie, key, length, &bpm);
	}

	if (SOC_SUCCESS(rv)) {
            /* all bpm bits are 0 */
            *bpm_pfx_len = (bpm < 0)? 0:(unsigned int)bpm;
        }
    }

    return rv;
}

STATIC int _trie_bpm_mask_get(trie_node_t *trie,
                    unsigned int *key,
                    unsigned int length,
                    unsigned int *bpm_mask)
{
    unsigned int lcp=0, scratch=0;
    int bit=0, rv=SOC_E_NONE;

    if (!trie || (length > _MAX_KEY_LEN_)) return SOC_E_PARAM;

    /* calculate number of matching msb bits */
    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    if (length > trie->skip_len) {
        if (lcp == trie->skip_len) {
            /* fully matched and more bits to check, go down the trie */
            bit = (key[KEY_BIT2IDX(length - lcp)] & \
                   (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

            if (trie->child[bit].child_node) {
                _bpm_append(bpm_mask, &scratch, trie->bpm, trie->skip_len + 1);
                rv = _trie_bpm_mask_get(trie->child[bit].child_node, key, length - lcp - 1, bpm_mask);
                return rv;
            }
        }
    }

    _bpm_append(bpm_mask, &scratch, trie->bpm, trie->skip_len + 1);
    return rv;
}

/*
 * Function:
 *     trie_bpm_mask_get
 * Purpose:
 *     Get the bpm mask of target key. This key is already in the trie.
 */
int trie_bpm_mask_get(trie_t *trie,
                  unsigned int *key,
                  unsigned int length,
                  unsigned int *bpm_mask)
{
    int rv = SOC_E_NONE;

    if (!trie || !key || !bpm_mask ) {
        return SOC_E_PARAM;
    }

    if (trie->trie) {
        if (trie->v6_key) {
            rv = _trie_v6_bpm_mask_get(trie->trie, key, length, bpm_mask);
        } else {
            rv = _trie_bpm_mask_get(trie->trie, key, length, bpm_mask);
        }
    }
    return rv;
}

/*
 * Function:
 *   _trie_skip_node_free
 * Purpose:
 *   Destroy a chain of trie_node_t that has the target node at the end.
 *   The target node is not necessarily PAYLOAD type, but all nodes
 *   on the chain except for the end must have only one branch.
 * Input:
 *   key      --  target key
 *   length   --  target key length
 *   free_end --  free
 */
STATIC int _trie_skip_node_free(trie_node_t *trie,
                                unsigned int *key,
                                unsigned int length)
{
    unsigned int lcp=0;
    int bit=0, rv=SOC_E_NONE;

    if (!trie || (length && trie->skip_len && !key)) return SOC_E_PARAM;

    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);


    if (length > trie->skip_len) {

        if (lcp == trie->skip_len) {
            bit = (key[KEY_BIT2IDX(length - lcp)] & \
                    (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

            /* There should be only one branch on the chain until the end node */
            if (!trie->child[0].child_node == !trie->child[1].child_node) {
                return SOC_E_PARAM;
            }

            /* based on next bit branch left or right */
            if (trie->child[bit].child_node) {
                rv = _trie_skip_node_free(trie->child[bit].child_node, key,
                        length - lcp - 1);
                if (SOC_SUCCESS(rv)) {
                    assert(trie->type == INTERNAL);
                    sal_free(trie);
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
 *   create a chain of trie_node_t that has the payload at the end.
 *   each node in the chain can skip upto _MAX_SKIP_LEN number of bits,
 *   while the child pointer in the chain represent 1 bit. So totally
 *   each node can absorb (_MAX_SKIP_LEN+1) bits.
 * Input:
 *   key      --  
 *   bpm      --  
 *   msb      --  
 *   skip_len --  skip_len of the whole chain
 *   payload  --  payload node we want to insert
 *   count    --  child count
 * Output:
 *   node     -- return pointer of the starting node of the chain.
 */
STATIC int _trie_skip_node_alloc(trie_node_t **node, 
				 unsigned int *key, 
				 /* bpm bit map if bpm management is required, passing null skips bpm management */
				 unsigned int *bpm, 
				 unsigned int msb, /* NOTE: valid msb position 1 based, 0 means skip0/0 node */
				 unsigned int skip_len,
				 trie_node_t *payload,
				 unsigned int count) /* payload count underneath - mostly 1 except some tricky cases */
{
    int lsb=0, msbpos=0, lsbpos=0, bit=0, index;
    trie_node_t *child = NULL, *skip_node = NULL;

    /* calculate lsb bit position, also 1 based */
    lsb = ((msb)? msb + 1 - skip_len : msb);

    assert(((int)msb >= 0) && (lsb >= 0));

    if (!node || !key || !payload || msb > _MAX_KEY_LEN_ || msb < skip_len) return SOC_E_PARAM;

    if (msb) {
        for (index = BITS2SKIPOFF(lsb), lsbpos = lsb - 1; index <= BITS2SKIPOFF(msb); index++) {
	    /* each loop process _MAX_SKIP_LEN number of bits?? */
            if (lsbpos == lsb-1) {
		/* (lsbpos == lsb-1) is only true for first node (loop) here */
                skip_node = payload;
            } else {
		/* other nodes need to be created */
                skip_node = sal_alloc(sizeof(trie_node_t), "trie_node");
            }

	    /* init memory */
            sal_memset(skip_node, 0, sizeof(trie_node_t));

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
                skip_node->skip_addr = key[KEY_BIT2IDX(msbpos+1)] & MASK((msbpos+1) % _NUM_WORD_BITS_); 
                skip_node->skip_addr = SHL(skip_node->skip_addr, 
                                           skip_node->skip_len - ((msbpos+1) % _NUM_WORD_BITS_),
                                           _NUM_WORD_BITS_);
                skip_node->skip_addr |= SHR(key[KEY_BIT2IDX(lsbpos+1)],(lsbpos % _NUM_WORD_BITS_),_NUM_WORD_BITS_);
            } else {
                skip_node->skip_addr = SHR(key[KEY_BIT2IDX(msbpos+1)], (lsbpos % _NUM_WORD_BITS_),_NUM_WORD_BITS_);
            }

	    /* set up the chain of child pointer, first node has no child since "child" was inited to NULL */
            if (child) {
                skip_node->child[bit].child_node = child;
            }

	    /* calculate child pointer for next loop. NOTE: skip_addr has not been masked
	     * so we still have the child bit in the skip_addr here.
	     */
            bit = (skip_node->skip_addr & SHL(1, skip_node->skip_len - 1,_MAX_SKIP_LEN_)) ? 1:0;

	    /* calculate node type */
            if (lsbpos == lsb-1) {
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
		if (lsbpos == _MAX_KEY_LEN_) {
		    /* parent node is 0/0, so there is no branch bit here */
		    skip_node->bpm = _key_get_bits(bpm, lsbpos, skip_node->skip_len, 1);
		} else {
		    skip_node->bpm = _key_get_bits(bpm, lsbpos+1, skip_node->skip_len+1, 1);
		}
            }
            
            /* for all child nodes 0/1 is implicitly obsorbed on parent */
            if (msbpos != msb-1) {
		/* msbpos == (msb-1) is only true for the first node */
		skip_node->skip_len--;
	    }
            skip_node->bpm &= MASK(skip_node->skip_len + 1);
            skip_node->skip_addr &= MASK(skip_node->skip_len);
            child = skip_node;
        } 
    } else {
	/* skip_len == 0 case, create a payload node with skip_len = 0 and bpm should be 1 bits only
	 * bit 0 and bit "skip_len" are same bit (bit 0).
	 */
        skip_node = payload;
        sal_memset(skip_node, 0, sizeof(trie_node_t));  
        skip_node->type = PAYLOAD;   
        skip_node->count = count;
        if (bpm) {
            skip_node->bpm =  _key_get_bits(bpm,1,1,0);
        }
    }

    *node = skip_node;
    return SOC_E_NONE;
}

STATIC int _trie_insert(trie_node_t *trie, 
			unsigned int *key, 
			/* bpm bit map if bpm management is required, passing null skips bpm management */
			unsigned int *bpm, 
			unsigned int length,
			trie_node_t *payload, /* payload node */
            trie_node_t **child, /* child pointer if the child is modified */
            int child_count)
{
    unsigned int lcp;
    int rv=SOC_E_NONE, bit=0;
    trie_node_t *node = NULL;

    if (!trie || (length && trie->skip_len && !key) ||
        !payload || !child || (length > _MAX_KEY_LEN_))
        return SOC_E_PARAM;

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
            if (trie->type != INTERNAL) {
                /* duplicate */ 
                return SOC_E_EXISTS;
            } else { 
                /* change the internal node to payload node */
                _CLONE_TRIE_NODE_(payload,trie);
                sal_free(trie);
                payload->type = PAYLOAD;
                payload->count += child_count;
                *child = payload;

                if (bpm) {
                    /* bpm at this internal mode must be same as the inserted pivot */
                    payload->bpm |= _key_get_bits(bpm, lcp+1, lcp+1, 1);
                    /* implicity preserve the previous bpm & set bit 0 -myself bit */
                } 
                return SOC_E_NONE;
            }
        } else { /* skip length can never be less than lcp implcitly here */
            /* this node is new parent for the old trie node */
            /* lcp is the new skip length */
            _CLONE_TRIE_NODE_(payload,trie);
            *child = payload;

            bit = (trie->skip_addr & SHL(1,trie->skip_len - length - 1,_MAX_SKIP_LEN_)) ? 1 : 0;
            trie->skip_addr &= MASK(trie->skip_len - length - 1);
            trie->skip_len  -= (length + 1);   
 
            if (bpm) {
                trie->bpm &= MASK(trie->skip_len+1);   
            }

            payload->skip_addr = (length > 0) ? key[KEY_BIT2IDX(length)] : 0;
            payload->skip_addr &= MASK(length);
            payload->skip_len  = length;
            payload->child[bit].child_node = trie;
            payload->child[!bit].child_node = NULL;
            payload->type = PAYLOAD;
            payload->count += child_count;

            if (bpm) {
                payload->bpm = SHR(payload->bpm, trie->skip_len + 1,_NUM_WORD_BITS_);
                payload->bpm |= _key_get_bits(bpm, payload->skip_len+1, payload->skip_len+1, 1);
            }
        }
    } else if (lcp == trie->skip_len) {
        /* key length is implictly greater than lcp here */
        /* decide based on key's next applicable bit */
        bit = (key[KEY_BIT2IDX(length-lcp)] & 
               (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

        if (!trie->child[bit].child_node) {
            /* the key is going to be one of the child of existing node */
            /* should be the child */
            rv = _trie_skip_node_alloc(&node, key, bpm,
				       length-lcp-1, /* 0 based msbit position */
				       length-lcp-1,
                       payload, child_count);
            if (SOC_SUCCESS(rv)) {
                trie->child[bit].child_node = node;
                trie->count += child_count;
            } else {
                LOG_CLI((BSL_META("\n Error on trie skip node allocaiton [%d]!!!!\n"),
                         rv));
            }
        } else { 
            rv = _trie_insert(trie->child[bit].child_node, 
                              key, bpm, length - lcp - 1, 
                              payload, child, child_count);
            if (SOC_SUCCESS(rv)) {
                trie->count += child_count;
                if (*child != NULL) { /* chande the old child pointer to new child */
                    trie->child[bit].child_node = *child;
                    *child = NULL;
                }
            }
        }
    } else {
        trie_node_t *newchild = NULL;

        /* need to introduce internal nodes */
        node = sal_alloc(sizeof(trie_node_t), "trie-node");
        _CLONE_TRIE_NODE_(node, trie);

        rv = _trie_skip_node_alloc(&newchild, key, bpm,
				   ((lcp)?length-lcp-1:length-1),
				   length - lcp - 1,
                   payload, child_count);
        if (SOC_SUCCESS(rv)) {
            bit = (key[KEY_BIT2IDX(length-lcp)] & 
                   (1 << ((length - lcp - 1) % _NUM_WORD_BITS_))) ? 1: 0;

            node->child[!bit].child_node = trie;
            node->child[bit].child_node = newchild;
            node->type = INTERNAL;
            node->skip_addr = SHR(trie->skip_addr,trie->skip_len - lcp,_MAX_SKIP_LEN_);
            node->skip_len = lcp;
            node->count += child_count;
            if (bpm) {
                node->bpm = SHR(node->bpm, trie->skip_len - lcp, _MAX_SKIP_LEN_);
            }
            *child = node;
            
            trie->skip_addr &= MASK(trie->skip_len - lcp - 1);
            trie->skip_len  -= (lcp + 1); 
            if (bpm) {
                trie->bpm &= MASK(trie->skip_len+1);      
            }
        } else {
            LOG_CLI((BSL_META("\n Error on trie skip node allocaiton [%d]!!!!\n"), rv));
	    sal_free(node);
        }
    }

    return rv;
}

/*
 * Function:
 *     trie_insert
 * Purpose:
 *     Inserts provided prefix/length in to the trie
 */
int trie_insert(trie_t *trie, 
                unsigned int *key, 
                unsigned int *bpm,
                unsigned int length, 
                trie_node_t *payload)
{
    int rv = SOC_E_NONE;
    trie_node_t *child=NULL;

    if (!trie) return SOC_E_PARAM;

    if (trie->trie == NULL) {
        if (trie->v6_key) {
	        rv = _trie_v6_skip_node_alloc(&trie->trie, key, bpm, length, length, payload, 1);
	    } else {
           rv = _trie_skip_node_alloc(&trie->trie, key, bpm, length, length, payload, 1);
	    }
    } else {
	   if (trie->v6_key) {
           rv = _trie_v6_insert(trie->trie, key, bpm, length, payload, &child, 1);
	   } else {
           rv = _trie_insert(trie->trie, key, bpm, length, payload, &child, 1);
	   }
       if (child) { /* chande the old child pointer to new child */
           trie->trie = child;
       }
    }

    return rv;
}

int _trie_fuse_child(trie_node_t *trie, int bit)
{
    trie_node_t *child = NULL;
    int rv = SOC_E_NONE;

    if (trie->child[0].child_node && trie->child[1].child_node) {
        return SOC_E_PARAM;
    } 

    bit = (bit > 0)?1:0;
    child = trie->child[bit].child_node;

    if (child == NULL) {
        return SOC_E_PARAM;
    } else {
        if (trie->skip_len + child->skip_len + 1 <= _MAX_SKIP_LEN_) {

            if (trie->skip_len == 0) trie->skip_addr = 0; 

            if (child->skip_len < _MAX_SKIP_LEN_) {
                trie->skip_addr = SHL(trie->skip_addr,child->skip_len + 1,_MAX_SKIP_LEN_);
            }

            trie->skip_addr  |= SHL(bit,child->skip_len,_MAX_SKIP_LEN_);
            child->skip_addr |= trie->skip_addr;
            child->bpm       |= SHL(trie->bpm,child->skip_len+1,_MAX_SKIP_LEN_); 
            child->skip_len  += trie->skip_len + 1;

            /* do not free payload nodes as they are user managed */
            if (trie->type == INTERNAL) {
                sal_free(trie);
            }
        }
    }

    return rv;
}

STATIC int _trie_delete(trie_node_t *trie, 
			unsigned int *key,
			unsigned int length,
			trie_node_t **payload,
			trie_node_t **child)
{
    unsigned int lcp;
    int rv=SOC_E_NONE, bit=0;
    trie_node_t *node = NULL;

    /* our algorithm should return before the length < 0, so this means
     * something wrong with the trie structure. Internal error?
     */
    if (!trie || (length && trie->skip_len && !key) ||
        !payload || !child || (length > _MAX_KEY_LEN_)) {
	return SOC_E_PARAM;
    }

    *child = NULL;

    /* check a section of key, return the number of matched bits and value of next bit */
    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    if (length > trie->skip_len) {

        if (lcp == trie->skip_len) {

            bit = (key[KEY_BIT2IDX(length-lcp)] & 
                   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;

            /* based on next bit branch left or right */
            if (trie->child[bit].child_node) {

	        /* has child node, keep searching */
                rv = _trie_delete(trie->child[bit].child_node, key, length - lcp - 1, payload, child);

	        if (rv == SOC_E_BUSY) {

                    trie->child[bit].child_node = NULL; /* sal_free the child */
                    rv = SOC_E_NONE;
                    trie->count--;

                    if (trie->type == INTERNAL) {

                        bit = (bit==0)?1:0;

                        if (trie->child[bit].child_node == NULL) {
                            /* parent and child connected, sal_free the middle-node itself */
                            sal_free(trie);
                            rv = SOC_E_BUSY;
                        } else {
                            /* fuse the parent & child */
                            if (trie->skip_len + trie->child[bit].child_node->skip_len + 1 <= 
                                _MAX_SKIP_LEN_) {
                                *child = trie->child[bit].child_node;
                                rv = _trie_fuse_child(trie, bit);
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
                        trie->child[bit].child_node = *child;
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
        if (!((lcp == length) && (trie->type == PAYLOAD))) {
	    rv = SOC_E_NOT_FOUND;
	} else {
            /* payload node can be deleted */
            /* if this node has 2 children update it to internal node */
            rv = SOC_E_NONE;

            if (trie->child[0].child_node && trie->child[1].child_node ) {
		/* the node has 2 children, update it to internal node */
                _BITCLR(trie->bpm, 0);
                node = sal_alloc(sizeof(trie_node_t), "trie_node");
                _CLONE_TRIE_NODE_(node, trie);
                node->type = INTERNAL;
                node->count--;
                *child = node;
            } else if (trie->child[0].child_node || trie->child[1].child_node ) {
                /* if this node has 1 children fuse the children with this node */
                bit = (trie->child[0].child_node) ? 0:1;
                trie->count--;
                if (trie->skip_len + trie->child[bit].child_node->skip_len + 1 <= _MAX_SKIP_LEN_) {
                    /* we need to clear the bpm bit of itself before fusing with child */
                    _BITCLR(trie->bpm, 0);

		    /* able to fuse the node with its child node */
                    *child = trie->child[bit].child_node;
                    rv = _trie_fuse_child(trie, bit);
                    if (rv != SOC_E_NONE) {
                        *child = NULL;
                    }
                } else {
		    /* convert it to internal node, we need to alloc new memory for internal nodes
		     * since the old payload node memory will be freed by caller
		     */
                    /* we need to clear the bpm bit of itself before converting */
                    _BITCLR(trie->bpm, 0);

                    node = sal_alloc(sizeof(trie_node_t), "trie_node");
                    _CLONE_TRIE_NODE_(node, trie);
                    node->type = INTERNAL;
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
 *     trie_delete
 * Purpose:
 *     Deletes provided prefix/length in to the trie
 */
int trie_delete(trie_t *trie,
                unsigned int *key,
                unsigned int length,
                trie_node_t **payload)
{
    int rv = SOC_E_NONE;
    trie_node_t *child = NULL;

    if (trie->trie) {
	if (trie->v6_key) {
	    rv = _trie_v6_delete(trie->trie, key, length, payload, &child);
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

STATIC INLINE int
_trie_splitable(trie_node_t *trie, trie_node_t *child, int max_count, int max_split_count)
{
/*
    * NOTE:
    *  ABS(trie->count * 2 - max_count) actually means
    *  ABS(trie->count - (max_count - trie->count))
    * which means the count's distance to half depth of the bucket
*/
    int do_split = 0;
    int half_count = (max_count + 1) >> 1;

    if (trie->count <= max_split_count && trie->count != max_count) {
        if (child == NULL) {
            do_split = 1;
        } else if (trie->count >= half_count && child->count < half_count) {
            do_split = 1;
        } else if (trie->count == half_count && child->count == half_count) {
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
 *     trie_split
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
STATIC int _trie_split(trie_node_t  *trie,
		       unsigned int *pivot,
		       unsigned int *length,
		       unsigned int *split_count,
		       trie_node_t **split_node,
		       trie_node_t **child,
		       const unsigned int max_count,
		       const unsigned int max_split_len,
		       const int split_to_pair,
		       unsigned int *bpm,
		       trie_split_states_e_t *state,
               const int max_split_count)
{
    int bit=0, rv=SOC_E_NONE;

    if (!trie || !pivot || !length || !split_node || max_count == 0 || !state || max_split_count == 0) return SOC_E_PARAM;

    if (trie->child[0].child_node && trie->child[1].child_node) {
        bit = (trie->child[0].child_node->count > 
               trie->child[1].child_node->count) ? 0:1;
    } else {
        bit = (trie->child[0].child_node)?0:1;
    }

    /* start building the pivot */
    rv = _key_append(pivot, length, trie->skip_addr, trie->skip_len);
    if (SOC_FAILURE(rv)) return rv;

    if (bpm) {
        unsigned int scratch=0;
        rv = _bpm_append(bpm, &scratch, trie->bpm, trie->skip_len+1);
        if (SOC_FAILURE(rv)) return rv;        
    }

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
	    if ((TRIE_SPLIT_STATE_PAYLOAD_SPLIT == *state) && 
		(trie->type == INTERNAL)) {
		*state = TRIE_SPLIT_STATE_PAYLOAD_SPLIT_DONE;
	    } else {
		if (((*length - max_split_len) > trie->skip_len) && (trie->skip_len == 0)) {
		    /* the length is longer than max_split_len, and the trie->skip_len is 0,
		     * so the best we can do is use the node as the split point
		     */
		    *split_node = trie;
		    *split_count = trie->count;
		    
		    *state = TRIE_SPLIT_STATE_PRUNE_NODES;
		    return rv;
		}
		
		/* we need to insert a node and use it as split point */
		*split_node = sal_alloc(sizeof(trie_node_t), "trie_node");
		sal_memset((*split_node), 0, sizeof(trie_node_t));
		(*split_node)->type = INTERNAL;
		(*split_node)->count = trie->count;
		
		if ((*length - max_split_len) > trie->skip_len) {
		    /* the length is longer than the max_split_len, and the trie->skip_len is
		     * shorter than the difference (max_split_len pivot is not covered by this 
		     * node but covered by its parent, the best we can do is split at the branch
		     * lead to this node. we insert a skip_len=0 node and use it as split point
		     */
		    (*split_node)->skip_len = 0;
		    (*split_node)->skip_addr = 0;
		    (*split_node)->bpm = (trie->bpm >> trie->skip_len);
		    
		    if (_BITGET(trie->skip_addr, (trie->skip_len-1))) {
			(*split_node)->child[1].child_node = trie;
		    } else {
			(*split_node)->child[0].child_node = trie;
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
		    (*split_node)->bpm = (trie->bpm >> (*length - max_split_len));
		    
		    if (_BITGET(trie->skip_addr, (*length-max_split_len-1))) {
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
		    rv = _key_shift_right(bpm, trie->skip_len+1);
        }
		if (SOC_SUCCESS(rv)) {
		    rv = _key_shift_right(pivot, trie->skip_len+1);
		}
		*state = TRIE_SPLIT_STATE_PRUNE_NODES;
		return rv;
	    }
	} else if ( ((*length == max_split_len) && (trie->count != max_count) && trie->count <= max_split_count) ||
                 _trie_splitable(trie, trie->child[bit].child_node, max_count, max_split_count)) {
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
	    
	    if ((TRIE_SPLIT_STATE_PAYLOAD_SPLIT == *state) && 
		(trie->type == INTERNAL)) {
		*state = TRIE_SPLIT_STATE_PAYLOAD_SPLIT_DONE;
	    } else {
		*state = TRIE_SPLIT_STATE_PRUNE_NODES;
		return rv;
	    }
	} else {
	    /* we can not split at this node, keep searching, it's better to 
	     * split at longer pivot
	     */
	    rv = _key_append(pivot, length, bit, 1);
	    if (SOC_FAILURE(rv)) return rv;
	    
	    rv = _trie_split(trie->child[bit].child_node, 
			     pivot, length,
			     split_count, split_node,
			     child, max_count, max_split_len,
                 split_to_pair, bpm, state, max_split_count);
	}
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
            rv = _key_shift_right(pivot, trie->skip_len+1);
            assert(*length >= trie->skip_len + 1);
            *length -= (trie->skip_len + 1);
        }
        break;

    case TRIE_SPLIT_STATE_PRUNE_NODES:
        if (trie->count == *split_count) {
            /* if the split point has associate internal nodes they have to
             * be cleaned up */
            assert(trie->type == INTERNAL);
            assert(!(trie->child[0].child_node && trie->child[1].child_node));
            sal_free(trie);
        } else {
            assert(*child == NULL);
            /* fuse with child if possible */
            trie->child[bit].child_node = NULL;
            bit = (bit==0)?1:0;
            trie->count -= *split_count;

            /* optimize more */
            if ((trie->type == INTERNAL) &&
                (trie->skip_len +
                 trie->child[bit].child_node->skip_len + 1 <= _MAX_SKIP_LEN_)) {
                *child = trie->child[bit].child_node;
                rv = _trie_fuse_child(trie, bit);
                if (rv != SOC_E_NONE) {
                    *child = NULL;
                }
            }
            *state = TRIE_SPLIT_STATE_DONE;
        }
        break;

    case TRIE_SPLIT_STATE_DONE:
        /* adjust parent's count */
        assert(*split_count > 0);
        assert(trie->count >= *split_count);

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

    return rv;
}

/*
 * Function:
 *     trie_split
 * Purpose:
 *     Split the trie into 2 based on optimum pivot
 * Note:
 *     we need to make sure the length is shorter than
 *     the max_split_len (for capacity optimization) if
 *     possible. We should ignore the max_split_len
 *     if that will result into trie not spliting
 */
int trie_split(trie_t *trie,
               const unsigned int max_split_len,
               const int split_to_pair,
               unsigned int *pivot,
               unsigned int *length,
               trie_node_t **split_trie_root,
               unsigned int *bpm,
               uint8 payload_node_split,
               const int max_split_count)
{
    int rv = SOC_E_NONE;
    unsigned int split_count=0, max_count=0;
    trie_node_t *child = NULL, *node=NULL, clone;
    trie_split_states_e_t state = TRIE_SPLIT_STATE_NONE;

    if (!trie || !pivot || !length || !split_trie_root) return SOC_E_PARAM;

    *length = 0;

    if (trie->trie) {

        if (payload_node_split) state = TRIE_SPLIT_STATE_PAYLOAD_SPLIT;

	max_count = trie->trie->count;

	if (trie->v6_key) {	    
	    sal_memset(pivot, 0, sizeof(unsigned int) * BITS2WORDS(_MAX_KEY_LEN_144_));
	    if (bpm) {
		sal_memset(bpm, 0, sizeof(unsigned int) * BITS2WORDS(_MAX_KEY_LEN_144_));
	    }
	    rv = _trie_v6_split(trie->trie, pivot, length, &split_count, split_trie_root,
                 &child, max_count, max_split_len, split_to_pair, bpm, &state, max_split_count);
	} else {
	    sal_memset(pivot, 0, sizeof(unsigned int) * BITS2WORDS(_MAX_KEY_LEN_48_));
	    if (bpm) {
		sal_memset(bpm, 0, sizeof(unsigned int) * BITS2WORDS(_MAX_KEY_LEN_48_));
	    }

	    rv = _trie_split(trie->trie, pivot, length, &split_count, split_trie_root,
                 &child, max_count, max_split_len, split_to_pair, bpm, &state, max_split_count);
	}
        if (SOC_SUCCESS(rv) && (TRIE_SPLIT_STATE_DONE == state)) {
            /* adjust parent's count */
            assert(split_count > 0);
            if (trie->trie == NULL) {
                trie_t *c1, *c2;
                trie_init(48, &c1);
                trie_init(48, &c2);
                c1->trie = child;
                c2->trie = *split_trie_root;
                LOG_ERROR(BSL_LS_SOC_ALPM,
                          (BSL_META("dumping the 2 child trees\n")));
                trie_dump(c1, 0, 0);
                trie_dump(c2, 0, 0);
            }
            /* update the child pointer if child was pruned */
            if (child != NULL) {
                trie->trie = child;
            }
            assert(trie->trie->count >= split_count || (*split_trie_root)->count >= split_count);

            sal_memcpy(&clone, *split_trie_root, sizeof(trie_node_t));
            child = *split_trie_root;

            /* take advantage of thie function by passing in internal or payload node whatever
             * is the new root. If internal the function assumed it as payload node & changes type.
             * But this method is efficient to reuse the last internal or payload node possible to
             * implant the new pivot */
	    if (trie->v6_key) {	    
		rv = _trie_v6_skip_node_alloc(&node, pivot, NULL,
					      *length, *length,
					      child, child->count);
	    } else {
		rv = _trie_skip_node_alloc(&node, pivot, NULL,
					   *length, *length,
					   child, child->count);
	    }

            if (SOC_SUCCESS(rv)) {
                if (clone.type == INTERNAL) {
                    child->type = INTERNAL; /* since skip alloc would have reset it to payload */
                }
                child->child[0].child_node = clone.child[0].child_node;
                child->child[1].child_node = clone.child[1].child_node;
                *split_trie_root = node;
            }
        } else {
            LOG_CLI((BSL_META("!!!! Failed to split the trie error:%d state: %d trie_count %d!!!\n"),
                     rv, state, max_count));
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
_trie_merge(trie_node_t *parent_trie,
            trie_node_t *child_trie,
            unsigned int *pivot,
            unsigned int length,
            trie_node_t **new_parent)
{
    int rv, child_count;
    trie_node_t *child = NULL, clone;
    unsigned int bpm[TAPS_MAX_KEY_SIZE_WORDS] = {0};
    unsigned int child_pivot[BITS2WORDS(_MAX_KEY_LEN_)] = {0};
    unsigned int child_length = 0;

    if (!parent_trie || length == 0 || !pivot || !new_parent || (length > _MAX_KEY_LEN_))
        return SOC_E_PARAM;

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
     * but for some cases, where the pivot is an INTERNAL node,
     * and it is fused with its child, then the pivot can no longer
     * be found, but we can still search a head. The head can be
     * payload (if this is the only payload head), or internal (if
     * two payload head coexist).
     */
    if (child == NULL) {
        return SOC_E_PARAM;
    }

    _CLONE_TRIE_NODE_(&clone, child);

    if (child->type == PAYLOAD && child->bpm) {
        _TAPS_SET_KEY_BIT(bpm, 0, TAPS_IPV4_KEY_SIZE);
    }

    if (child != child_trie) {
        rv = _trie_skip_node_free(child_trie, child_pivot, child_length);
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

    /* merge happens on bucket trie, which usually does not need bpm */
    rv = _trie_insert(parent_trie, child_pivot, bpm, child_length, child,
                      new_parent, child_count);
    if (rv < 0) {
        return SOC_E_PARAM;
    }

    /*
     * child node, the inserted node, will be modified during insert,
     * and it must be a leaf node of the parent trie without any child.
     * The child node could be either payload or internal.
     */
    if (child->child[0].child_node || child->child[1].child_node) {
        return SOC_E_PARAM;
    }
    if (clone.type == INTERNAL) {
        child->type = INTERNAL;
    }
    child->child[0].child_node = clone.child[0].child_node;
    child->child[1].child_node = clone.child[1].child_node;

    return SOC_E_NONE;
}


/*
 * Function:
 *     trie_merge
 * Purpose:
 *     merge or fuse the child trie with parent trie.
 */
int trie_merge(trie_t *parent_trie,
               trie_node_t *child_trie,
               unsigned int *child_pivot,
               unsigned int length)
{
    int rv=SOC_E_NONE;
    trie_node_t *child=NULL;

    if (!parent_trie) {
        return SOC_E_PARAM;
    }

    if (!child_trie) {
        return SOC_E_NONE;
    }

    if (parent_trie->trie == NULL) {
        parent_trie->trie = child_trie;
    } else {
        if (parent_trie->v6_key) {
            rv = _trie_v6_merge(parent_trie->trie, child_trie, child_pivot, length, &child);
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
 *     trie_split
 * Purpose:
 *     Split the trie into 2 such that the new sub trie covers given prefix/length.
 * NOTE:
 *     key, key_len    -- The given prefix/length
 *     max_split_count -- The sub trie's max allowed count.
 */
int
_trie_split2(trie_node_t *trie,
             unsigned int *key,
             unsigned int key_len,
             unsigned int *pivot,
             unsigned int *pivot_len,
             unsigned int *split_count,
             trie_node_t **split_node,
             trie_node_t **child,
             trie_split2_states_e_t *state,
             const int max_split_count,
             const int exact_same)
{
    unsigned int lcp=0;
    int bit=0, rv=SOC_E_NONE;

    if (!trie || !pivot || !pivot_len || !split_node || !state || max_split_count == 0) return SOC_E_PARAM;
    /* start building the pivot */
    rv = _key_append(pivot, pivot_len, trie->skip_addr, trie->skip_len);
    if (SOC_FAILURE(rv)) return rv;


    lcp = lcplen(key, key_len, trie->skip_addr, trie->skip_len);

    if (lcp == trie->skip_len) {
        if (trie->count <= max_split_count &&
            (!exact_same || (key_len - lcp) == 0)) {
            *split_node = trie;
            *split_count = trie->count;
            if (trie->count < max_split_count) {
                *state = TRIE_SPLIT2_STATE_PRUNE_NODES;
            }
            return SOC_E_NONE;
        }
        if (key_len > lcp) {
            bit = (key[KEY_BIT2IDX(key_len - lcp)] & \
                    (1 << ((key_len - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

            /* based on next bit branch left or right */
            if (trie->child[bit].child_node) {
                /* we can not split at this node, keep searching, it's better to
                 * split at longer pivot
                 */
                rv = _key_append(pivot, pivot_len, bit, 1);
                if (SOC_FAILURE(rv)) return rv;

                rv = _trie_split2(trie->child[bit].child_node,
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
        case TRIE_SPLIT2_STATE_NONE: /* fail to split */
            break;

        case TRIE_SPLIT2_STATE_PRUNE_NODES:
            if (trie->count == *split_count) {
                /* if the split point has associate internal nodes they have to
                 * be cleaned up */
                assert(trie->type == INTERNAL);
                /* at most one child */
                assert(!(trie->child[0].child_node && trie->child[1].child_node));
                /* at least one child */
                assert(trie->child[0].child_node || trie->child[1].child_node);
                sal_free(trie);
            } else {
                assert(*child == NULL);
                /* fuse with child if possible */
                trie->child[bit].child_node = NULL;
                bit = (bit==0)?1:0;
                trie->count -= *split_count;

                /* optimize more */
                if ((trie->type == INTERNAL) &&
                        (trie->skip_len +
                         trie->child[bit].child_node->skip_len + 1 <= _MAX_SKIP_LEN_)) {
                    *child = trie->child[bit].child_node;
                    rv = _trie_fuse_child(trie, bit);
                    if (rv != SOC_E_NONE) {
                        *child = NULL;
                    }
                }
                *state = TRIE_SPLIT2_STATE_DONE;
            }
            break;

        case TRIE_SPLIT2_STATE_DONE:
            /* adjust parent's count */
            assert(*split_count > 0);
            assert(trie->count >= *split_count);

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

    return rv;
}



/*
 * Function:
 *     trie_split2
 * Purpose:
 *     Split the trie such that the new sub trie covers given prefix/length.
 *     Basically this is a reverse of trie_merge.
 */

int trie_split2(trie_t *trie,
                unsigned int *key,
                unsigned int key_len,
                unsigned int *pivot,
                unsigned int *pivot_len,
                trie_node_t **split_trie_root,
                const int max_split_count,
                const int exact_same)
{
    int rv = SOC_E_NONE;
    int msc = max_split_count;
    unsigned int split_count=0;
    trie_node_t *child = NULL, *node=NULL, clone;
    trie_split2_states_e_t state = TRIE_SPLIT2_STATE_NONE;

    if (!trie || (key_len && !key) || !pivot || !pivot_len ||
        !split_trie_root || max_split_count == 0) {
        return SOC_E_PARAM;
    }

    *split_trie_root = NULL;
    *pivot_len = 0;

    if (trie->trie) {
        if (max_split_count == 0xfffffff) {
            trie_node_t *child2 = NULL;
            trie_node_t *payload;
            payload = sal_alloc(sizeof(trie_node_t), "trie_node");
            if (payload == NULL) {
                return SOC_E_MEMORY;
            }

            if (trie->v6_key) {
                rv = _trie_v6_insert(trie->trie, key, NULL, key_len, payload, &child2, 0);
            } else {
                rv = _trie_insert(trie->trie, key, NULL, key_len, payload, &child2, 0);
            }
            if (child2) { /* change the old child pointer to new child */
                trie->trie = child2;
            }

            if (SOC_SUCCESS(rv)) {
                payload->type = INTERNAL;
            } else {
                sal_free(payload);
                if (rv != SOC_E_EXISTS) {
                    return rv;
                }
            }

            msc = trie->trie->count;
        }
        if (trie->v6_key) {
            sal_memset(pivot, 0, sizeof(unsigned int) * BITS2WORDS(_MAX_KEY_LEN_144_));
            rv = _trie_v6_split2(trie->trie, key, key_len, pivot, pivot_len,
                    &split_count, split_trie_root, &child, &state,
                    msc, exact_same);
        } else {
            sal_memset(pivot, 0, sizeof(unsigned int) * BITS2WORDS(_MAX_KEY_LEN_48_));
            rv = _trie_split2(trie->trie, key, key_len, pivot, pivot_len,
                    &split_count, split_trie_root, &child, &state,
                    msc, exact_same);
        }

        if (SOC_SUCCESS(rv) && (TRIE_SPLIT2_STATE_DONE == state)) {
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

            sal_memcpy(&clone, *split_trie_root, sizeof(trie_node_t));
            child = *split_trie_root;

            /* take advantage of thie function by passing in internal or payload node whatever
             * is the new root. If internal the function assumed it as payload node & changes type.
             * But this method is efficient to reuse the last internal or payload node possible to
             * implant the new pivot */
            if (trie->v6_key) {
                rv = _trie_v6_skip_node_alloc(&node, pivot, NULL,
                                              *pivot_len, *pivot_len,
                                              child, child->count);
            } else {
                rv = _trie_skip_node_alloc(&node, pivot, NULL,
                                           *pivot_len, *pivot_len,
                                           child, child->count);
            }

            if (SOC_SUCCESS(rv)) {
                if (clone.type == INTERNAL) {
                    child->type = INTERNAL; /* since skip alloc would have reset it to payload */
                }
                child->child[0].child_node = clone.child[0].child_node;
                child->child[1].child_node = clone.child[1].child_node;
                *split_trie_root = node;
            }
        } else if (SOC_SUCCESS(rv) && (max_split_count == 0xfffffff) &&
                   (split_count == trie->trie->count)) {
            /* take all */
            *split_trie_root = trie->trie;
            trie->trie = NULL;
        } else { /* split2 is not like split which can always succeed */
            LOG_INFO(BSL_LS_SOC_ALPM,
                      (BSL_META("Failed to split the trie error:%d state: %d "\
                           "split_trie_root: %p !!!\n"),
                        rv, state, *split_trie_root));
            rv = SOC_E_NOT_FOUND;
        }
    } else {
        rv = SOC_E_PARAM;
    }

    return rv;
}


/*
 * Function:
 *     _trie_traverse_propagate_prefix
 * Purpose:
 *     calls back applicable payload object is affected by prefix updates 
 * NOTE:
 *     propagation stops once any callback funciton return something otherthan SOC_E_NONE
 *     tcam propagation code should return !SOC_E_NONE so that callback only happen once.
 * 
 *     other propagation code should always return SOC_E_NONE so that callback will
 *     happen on all pivot.
 */
int _trie_traverse_propagate_prefix(trie_node_t *trie,
                                    trie_propagate_cb_f cb,
                                    trie_bpm_cb_info_t *cb_info,
                                    unsigned int mask)
{
    int rv = SOC_E_NONE, index=0;

    if (!trie || !cb || !cb_info) return SOC_E_PARAM;

    if ((trie->bpm & mask) == 0) {
        /* call back the payload object if applicable */
        if (PAYLOAD == trie->type) {
            rv = cb(trie, cb_info);
	    if (SOC_FAILURE(rv)) {
		/* callback stops once any callback function not returning SOC_E_NONE */
		return rv;
	    }
        }

        for (index=0; index < _MAX_CHILD_ && SOC_SUCCESS(rv); index++) {
            if (trie->child[index].child_node && trie->child[index].child_node->bpm == 0) {
                /* coverity[large_shift : FALSE] */
                rv = _trie_traverse_propagate_prefix(trie->child[index].child_node,
                                                     cb, cb_info, MASK(32));
            }

	    if (SOC_FAILURE(rv)) {
		/* callback stops once any callback function not returning SOC_E_NONE */
		return rv;
	    }
        }
    }

    return rv;
}

/*
 * Function:
 *     _trie_propagate_prefix
 * Purpose:
 *  Propogate prefix BPM. If the propogation starts from intermediate pivot on
 *  the trie, then the prefix length has to be appropriately adjusted or else 
 *  it will end up with ill updates. 
 *  Assumption: the prefix length is adjusted as per trie node on which is starts from.
 *  If node == head node then adjust is none
 *     node == pivot, then prefix length = org len - pivot len          
 */
STATIC int _trie_propagate_prefix(trie_node_t *trie,
				  unsigned int *pfx,
				  unsigned int len,
				  unsigned int add, /* 0-del/1-add */
				  trie_propagate_cb_f cb,
				  trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE; /*, index;*/
    unsigned int bit=0, lcp=0;

    if (!trie || (len && trie->skip_len && !pfx) ||
        (len > _MAX_KEY_LEN_) || !cb || !cb_info) return SOC_E_PARAM;

    if (len > 0) {
        /* BPM bit maps has to be updated before propagation */
        lcp = lcplen(pfx, len, trie->skip_addr, trie->skip_len);            
        /* if the lcp is less than prefix length the prefix is not applicable
         * for any propagation */
        if (lcp < ((len>trie->skip_len)?trie->skip_len:len)) {
            return SOC_E_NONE; 
        } else { 
            if (len > trie->skip_len) {
                /* fully matched and more bits to check, go down the trie */
                bit = _key_get_bits(pfx, len-lcp, 1, 0);
                if (!trie->child[bit].child_node) return SOC_E_NONE;
                rv = _trie_propagate_prefix(trie->child[bit].child_node,
                                            pfx, len-lcp-1, add, cb, cb_info);
            } else {
                /* given pfx exactly matched or covers trie node, this is the
                 * point to propagate.
                 */
                /* pfx is <= trie skip len */
                if (!add) { /* delete */
                    _BITCLR(trie->bpm, trie->skip_len - len);
                }
                
                /* update bit map and propagate if applicable:
                 * there is no longer bpm than this new prefix
                 */
                if ((trie->bpm & BITMASK(trie->skip_len - len)) == 0) {
                    rv = _trie_traverse_propagate_prefix(trie, cb, 
                                                         cb_info, 
                                                         BITMASK(trie->skip_len - len));
                    if (SOC_E_LIMIT == rv) rv = SOC_E_NONE;
                } else if (add && _BITGET(trie->bpm, trie->skip_len - len)) {
		    /* if adding, and bpm of this node is the specified prefix
		     * also propagate. (this is really update case)
		     */
                    rv = _trie_traverse_propagate_prefix(trie, cb, 
                                                         cb_info, 
                                                         BITMASK(trie->skip_len - len));
                    if (SOC_E_LIMIT == rv) rv = SOC_E_NONE;
		}
                
                if (add && SOC_SUCCESS(rv)) {
                    /* this is the case where child bit is the new prefix */
                    _BITSET(trie->bpm, trie->skip_len - len);
                }
            }
        }
    } else {
        if (!add) { /* delete */
            _BITCLR(trie->bpm, trie->skip_len);
        }

        if ((trie->bpm == 0) || 
	    (add && ((trie->bpm & BITMASK(trie->skip_len)) == 0))) {
	    /* if adding, and bpm of this node is the specified prefix
	     * also propagate. (this is really update case)
	     */
            rv = _trie_traverse_propagate_prefix(trie, cb, cb_info, BITMASK(trie->skip_len));
            if (SOC_E_LIMIT == rv) rv = SOC_E_NONE;
        }
        
        if (add && SOC_SUCCESS(rv)) { /* add */
            /* this is the case where child bit is the new prefix */
            _BITSET(trie->bpm, trie->skip_len);
        }
    }

    return rv;
}

/*
 * Function:
 *     _trie_propagate_prefix_validate
 * Purpose:
 *  validate that the provided prefix is valid for propagation.
 *  The added prefix which was member of a shorter pivot's domain 
 *  must never be more specific than another pivot encounter if any
 *  in the path
 */
STATIC int _trie_propagate_prefix_validate(trie_node_t *trie,
					   unsigned int *pfx,
					   unsigned int len)
{
    unsigned int lcp=0, bit=0;

    if (!trie || (len && trie->skip_len && !pfx)) return SOC_E_PARAM;

    if (len == 0) return SOC_E_NONE;

    lcp = lcplen(pfx, len, trie->skip_addr, trie->skip_len);

    if (lcp == trie->skip_len) {
        if (PAYLOAD == trie->type) return SOC_E_PARAM;
	if (len == lcp) return SOC_E_NONE;
        bit = _key_get_bits(pfx, len-lcp, 1, 0);
        if (!trie->child[bit].child_node) return SOC_E_NONE;
        return _trie_propagate_prefix_validate(trie->child[bit].child_node,
                                               pfx, len-1-lcp);
    }

    return SOC_E_NONE;
}

int _trie_init_propagate_info(unsigned int *pfx,
			      unsigned int len,
			      trie_propagate_cb_f cb,
			      trie_bpm_cb_info_t *cb_info)
{
    cb_info->pfx = pfx;
    cb_info->len = len;
    return SOC_E_NONE;
}

/*
 * Function:
 *     trie_pivot_propagate_prefix
 * Purpose:
 *  Propogate prefix BPM from a given pivot.      
 */
int trie_pivot_propagate_prefix(trie_node_t *pivot,
                                unsigned int pivot_len,
                                unsigned int *pfx,
                                unsigned int len,
                                unsigned int add, /* 0-del/1-add */
                                trie_propagate_cb_f cb,
                                trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE;

    if (!pfx || !pivot || (len > _MAX_KEY_LEN_) ||
        (pivot_len >  _MAX_KEY_LEN_) || (len < pivot_len) ||
        (pivot->type != PAYLOAD) || !cb || !cb_info ||
        !cb_info->pfx) {
	return SOC_E_PARAM;
    }

    _trie_init_propagate_info(pfx,len,cb,cb_info);
    len -= pivot_len;

    if (len > 0) {
        unsigned int bit =  _key_get_bits(pfx, len, 1, 0);

        if (pivot->child[bit].child_node) {
            /* validate if the pivot provided is correct */
            rv = _trie_propagate_prefix_validate(pivot->child[bit].child_node,
						 pfx, len-1);
            if (SOC_SUCCESS(rv)) {
                rv = _trie_propagate_prefix(pivot->child[bit].child_node,
                                            pfx, len-1,
                                            add, cb, cb_info);
            }
        } /* else nop, nothing to propagate on this path end */
    } else {
        /* pivot == prefix */
        rv = _trie_propagate_prefix(pivot, pfx, pivot->skip_len,
                                    add, cb, cb_info);
    }

    return rv;
}

/*
 * Function:
 *     _pvt_trie_traverse_propagate_prefix
 * Purpose:
 *     calls back applicable payload object is affected by prefix updates
 * NOTE:
 *     other propagation code should always return SOC_E_NONE so that
 *     callback will happen on all pivot.
 */
int _pvt_trie_traverse_propagate_prefix(trie_node_t *trie,
                                        trie_propagate_cb_f cb,
                                        trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE, index=0;
    int rv1 = SOC_E_NONE;

    if (!trie || !cb || !cb_info) {
        return SOC_E_PARAM;
    }

    /* call back the payload object if applicable */
    if (PAYLOAD == trie->type) {
        rv = cb(trie, cb_info);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }

    for (index=0; index < _MAX_CHILD_; index++) {
        if (trie->child[index].child_node) {
            rv = _pvt_trie_traverse_propagate_prefix(
                    trie->child[index].child_node, cb, cb_info);
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
 *   _pvt_trie_propagate_prefix
 * Purpose:
 *   If the propogation starts from intermediate pivot on
 *   the trie, then the prefix length has to be appropriately adjusted or else
 *   it will end up with ill updates.
 *   Assumption: the prefix length is adjusted as per trie node on which
 *               is starts from.
 *   If node == head node then adjust is none
 *      node == pivot, then prefix length = org len - pivot len
 */
STATIC int _pvt_trie_propagate_prefix(trie_node_t *trie,
                    unsigned int *pfx,
                    unsigned int len,
                    trie_propagate_cb_f cb,
                    trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE; /*, index;*/
    unsigned int bit = 0, lcp = 0;

    if (!trie || (len && trie->skip_len && !pfx) ||
        (len > _MAX_KEY_LEN_) || !cb || !cb_info) {
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
                bit = _key_get_bits(pfx, len-lcp, 1, 0);
                if (!trie->child[bit].child_node) {
                    return SOC_E_NONE;
                }
                rv = _pvt_trie_propagate_prefix(
                        trie->child[bit].child_node,
                        pfx, len-lcp-1, cb, cb_info);
            } else {
                /* pfx is <= trie skip len */
                /* propagate if applicable */
                rv = _pvt_trie_traverse_propagate_prefix(trie, cb,
                                                         cb_info);
                if (SOC_E_LIMIT == rv) {
                    rv = SOC_E_NONE;
                }
            }
        }
    } else {
        rv = _pvt_trie_traverse_propagate_prefix(trie, cb, cb_info);
        if (SOC_E_LIMIT == rv) {
            rv = SOC_E_NONE;
        }
    }

    return rv;
}

/*
 * Function:
 *      pvt_trie_propagate_prefix
 * Purpose:
 *      Propogate prefix from a given pivot.
 *      Callback function to decide INSERT/DELETE propagation,
 *               and decide to update bpm_len or not.
 */
int pvt_trie_propagate_prefix(trie_node_t *pivot,
                              unsigned int pivot_len,
                              unsigned int *pfx,
                              unsigned int len,
                              trie_propagate_cb_f cb,
                              trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE;

    if (!pfx || !pivot || (len > _MAX_KEY_LEN_) ||
        (pivot_len > _MAX_KEY_LEN_) || (len < pivot_len) ||
        (pivot->type != PAYLOAD) || !cb || !cb_info ||
        !cb_info->pfx) {
        return SOC_E_PARAM;
    }

    len -= pivot_len;

    if (len > 0) {
        unsigned int bit = _key_get_bits(pfx, len, 1, 0);
        if (pivot->child[bit].child_node) {
            /* validate if the pivot provided is correct */
            rv = _trie_propagate_prefix_validate(pivot->child[bit].child_node,
                                                 pfx, len-1);
            if (SOC_SUCCESS(rv)) {
                rv = _pvt_trie_propagate_prefix(pivot->child[bit].child_node,
                                                pfx, len-1,
                                                cb, cb_info);
            }
        } /* else nop, nothing to propagate on this path end */
    } else {
        /* pivot == prefix */
        rv = _pvt_trie_propagate_prefix(pivot, pfx, pivot->skip_len,
                                        cb, cb_info);
    }

    return rv;
}

int trie_ppg_prefix(trie_t *trie, unsigned int pvt_len,
                    unsigned int *pfx,
                    unsigned int len,
                    trie_propagate_cb_f cb,
                    trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE, rv2 = SOC_E_NONE;
    trie_node_t *payload;

    if (!pfx || !trie || !trie->trie || !cb || !cb_info) {
        return SOC_E_PARAM;
    }

    payload = sal_alloc(sizeof(trie_node_t), "trie_node");
    if (payload == NULL) {
        return SOC_E_MEMORY;
    }
    rv2 = trie_insert(trie, pfx, NULL, len, payload);
    if (SOC_FAILURE(rv2)) {
        sal_free(payload);
        if (rv2 != SOC_E_EXISTS) {
            return rv2;
        }
        rv = trie_find_lpm(trie, pfx, len, &payload);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    } else {
        payload->bpm = -1;
    }

    if (trie->v6_key) {
        rv = pvt_trie_v6_propagate_prefix(payload, len, pfx, len,
                                          cb, cb_info);
    } else {
        rv = pvt_trie_propagate_prefix(payload, len, pfx, len,
                                       cb, cb_info);
    }

    if (SOC_SUCCESS(rv2)) {
        trie_delete(trie, pfx, len, &payload);
        sal_free(payload);
    }

    return rv;
}

/*
 * Function:
 *     trie_propagate_prefix
 * Purpose:
 *  Propogate prefix BPM on a given trie.      
 */
int trie_propagate_prefix(trie_t *trie,
                          unsigned int *pfx,
                          unsigned int len,
                          unsigned int add, /* 0-del/1-add */
                          trie_propagate_cb_f cb,
                          trie_bpm_cb_info_t *cb_info)
{
    int rv=SOC_E_NONE;

    if (!pfx || !trie || !trie->trie || !cb || !cb_info || !cb_info->pfx) {
	return SOC_E_PARAM;
    }

    _trie_init_propagate_info(pfx,len,cb,cb_info);

    if (SOC_SUCCESS(rv)) {
	if (trie->v6_key) {	    
	    rv = _trie_v6_propagate_prefix(trie->trie, pfx, len, add, 
					   cb, cb_info);
	} else {
	    rv = _trie_propagate_prefix(trie->trie, pfx, len, add, 
					cb, cb_info);
	}
    }

    return rv;
}


/*
 * Function:
 *     trie_init
 * Purpose:
 *     allocates a trie & initializes it
 */
int trie_init(unsigned int max_key_len, trie_t **ptrie)
{
    trie_t *trie = sal_alloc(sizeof(trie_t), "trie-node");
    sal_memset(trie, 0, sizeof(trie_t));

    if (max_key_len == _MAX_KEY_LEN_48_) {
        trie->v6_key = FALSE;
    } else if (max_key_len == _MAX_KEY_LEN_144_) {
        trie->v6_key = TRUE;
    } else {
        sal_free(trie);
        return SOC_E_PARAM;
    }

    trie->trie = NULL; /* means nothing is on teie */
    *ptrie = trie;
    return SOC_E_NONE;
}


/*
 * Function:
 *     trie_delete_node_cb
 * Purpose:
 *     Call back to delete a node
 */
int
trie_delete_node_cb(trie_node_t *node, void *info)
{
    if (node != NULL) {
        sal_free(node);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     trie_destroy2
 * Purpose:
 *     destroys a trie and its body
 */
int trie_destroy2(trie_t *trie)
{
    int rv;
    rv = trie_traverse(trie, trie_delete_node_cb, NULL,
                       _TRIE_POSTORDER_TRAVERSE);
    if (SOC_FAILURE(rv)) {
        return rv;
    }
    sal_free(trie);
    return SOC_E_NONE;
}


/*
 * Function:
 *     trie_destroy
 * Purpose:
 *     destroys a trie
 */
int trie_destroy(trie_t *trie)
{
    if (trie != NULL) {
        sal_free(trie);
    }
    return SOC_E_NONE;
}


STATIC int _trie_clone(trie_node_t *ptrie,
                       trie_node_t *trie,
                       int bit,
                       trie_traverse_states_e_t *state,
                       trie_node_t *pnode,
                       trie_t      *clone_trie)
{
    int rv = SOC_E_NONE;
    trie_node_t *lc, *rc;

    if (trie == NULL) {
        return SOC_E_NONE;
    } else {
        /* make the trie delete safe */
        lc = trie->child[0].child_node;
        rc = trie->child[1].child_node;
        {
            trie_node_t *clone_node = NULL;

            /* assert(trie); */
            clone_node = sal_alloc(sizeof(trie_node_t), "clone trie");
            if (clone_node == NULL) {
                return SOC_E_MEMORY;
            }
            sal_memcpy(clone_node, trie, sizeof(trie_node_t));

            if (ptrie == NULL) { /* clone head */
                /* assert(clone_trie); */
                clone_trie->trie = clone_node;
            } else { /* clone body */
                /* assert(pnode); */
                pnode->child[bit].child_node = clone_node;
            }

            pnode = clone_node;
        }
        TRIE_TRAVERSE_STOP(*state, rv);
        /* make the ptrie delete safe */
        if (*state != TRIE_TRAVERSE_STATE_DELETED) {
            ptrie = trie;
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_clone(ptrie, lc, 0, state, pnode, NULL);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_clone(ptrie, rc, 1, state, pnode, NULL);
    }
    return rv;
}


int trie_clone(trie_t *trie_src, trie_t **trie_dst)
{
    int rv;
    trie_traverse_states_e_t state = TRIE_TRAVERSE_STATE_NONE;

    assert(trie_src && trie_dst);

    rv = trie_init(trie_src->v6_key ? _MAX_KEY_LEN_144_ : _MAX_KEY_LEN_48_,
                   trie_dst);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    rv = _trie_clone(NULL, trie_src->trie, 0, &state, NULL, *trie_dst);
    if (SOC_FAILURE(rv)) {
        trie_destroy2(*trie_dst);
        *trie_dst = NULL;
    }
    return rv;
}



STATIC int _trie_compare(trie_node_t *ptrie,
                         trie_node_t *trie,
                         int bit,
                         trie_traverse_states_e_t *state,
                         trie_node_t *ptrie_cmp,
                         trie_node_t *trie_cmp,
                         int *cmp_result)
{
    int rv = SOC_E_NONE;
    trie_node_t *lc, *rc;

    if (trie == NULL) {
        return SOC_E_NONE;
    } else {
        /* make the trie delete safe */
        lc = trie->child[0].child_node;
        rc = trie->child[1].child_node;
        {

            assert(trie && state);

            if (ptrie == NULL) { /* compare head */
                trie_cmp = trie_cmp;
            } else { /* compare body */
                trie_cmp = ptrie_cmp->child[bit].child_node;
            }

            /* When traverse against one trie, there is a possibility the other is
             * a super set of this one. To rule out that possibility, compare the child
             * pointer as well.
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
                *state = TRIE_TRAVERSE_STATE_DONE;
            }

            /* In case of v6, the skip len not equal does not mean the two
             * tries are different, thus we print it and let user be aware
             * and then continue.
             */
            if (trie_cmp->skip_len != trie->skip_len) {
                LOG_CLI(("AWARE:len %d - %d   addr 0x%x - 0x%x\n",
                         trie_cmp->skip_len, trie->skip_len,
                         trie_cmp->skip_addr, trie->skip_addr));
            }
            ptrie_cmp = trie_cmp;
        }
        TRIE_TRAVERSE_STOP(*state, rv);
        /* make the ptrie delete safe */
        if (*state != TRIE_TRAVERSE_STATE_DELETED) {
            ptrie = trie;
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = _trie_compare(ptrie, lc, 0, state, ptrie_cmp, NULL, cmp_result);
        TRIE_TRAVERSE_STOP(*state, rv);
    }
    if (SOC_SUCCESS(rv)) {
        rv = _trie_compare(ptrie, rc, 1, state, ptrie_cmp, NULL, cmp_result);
    }
    return rv;
}


int trie_compare(trie_t *trie_src, trie_t *trie_dst, int *equal)
{
    trie_traverse_states_e_t state = TRIE_TRAVERSE_STATE_NONE;

    assert(trie_src && trie_dst && equal);

    *equal = 0;

    return _trie_compare(NULL, trie_src->trie, 0, &state, NULL, trie_dst->trie, equal);
}

#if 0 
/****************/
/** unit tests **/
/****************/
#define _NUM_KEY_ (4 * 1024)
#define _VRF_LEN_ 16
/*#define VERBOSE 
  #define LOG*/
/* use the followign diag shell command to run this test:
 * tr c3sw test=tmu_trie_ut
 */
typedef struct _payload_s {
    trie_node_t node; /*trie node */
    dq_t        listnode; /* list node */
    union {
        trie_t      *trie;
        trie_node_t pfx_trie_node;
    } info;
    unsigned int key[BITS2WORDS(_MAX_KEY_LEN_)];
    unsigned int len;
} payload_t;

int ut_print_payload_node(trie_node_t *payload, void *datum)
{
    payload_t *pyld;

    if (payload && payload->type == PAYLOAD) {
        pyld = TRIE_ELEMENT_GET(payload_t*, payload, node);
        LOG_CLI((BSL_META(" key[0x%08x:0x%08x] Length:%d \n"),
                 pyld->key[0], pyld->key[1], pyld->len));
    }
    return SOC_E_NONE;
}

int ut_print_prefix_payload_node(trie_node_t *payload, void *datum)
{
    payload_t *pyld;

    if (payload && payload->type == PAYLOAD) {
        pyld = TRIE_ELEMENT_GET(payload_t*, payload, info.pfx_trie_node);
        LOG_CLI((BSL_META(" key[0x%08x:0x%08x] Length:%d \n"),
                 pyld->key[0], pyld->key[1], pyld->len));
    }
    return SOC_E_NONE;
}

int ut_check_duplicate(payload_t *pyld, int pyld_vector_size)
{
    int i=0;

    assert(pyld);

    for (i=0; i < pyld_vector_size; i++) {
        if (pyld[i].len == pyld[pyld_vector_size].len &&
            pyld[i].key[0] == pyld[pyld_vector_size].key[0] && 
            pyld[i].key[1] == pyld[pyld_vector_size].key[1]) {
            break;
        }
    }

    return ((i == pyld_vector_size)?0:1);
}


int tmu_taps_util_get_bpm_pfx_ut(void) 
{
    int rv;
    unsigned int pfx_len=0;
    unsigned int bpm[] = { 0x80, 0x00101010 }; /* 40th bit msb */

    /* v4 test */
    rv = taps_get_bpm_pfx(&bpm[0], 48, _MAX_KEY_LEN_48_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 48-4) return SOC_E_FAIL;

    bpm[0] = 0;
    bpm[1] = 0x00010000;
    rv = taps_get_bpm_pfx(&bpm[0], 48, _MAX_KEY_LEN_48_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 48-16) return SOC_E_FAIL;

    bpm[0] = 0;
    bpm[1] = 0x80000000;
    rv = taps_get_bpm_pfx(&bpm[0], 48, _MAX_KEY_LEN_48_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 48-31) return SOC_E_FAIL;

    bpm[0] = 0x1;
    bpm[1] = 0x80000000;
    rv = taps_get_bpm_pfx(&bpm[0], 48, _MAX_KEY_LEN_48_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 48-31) return SOC_E_FAIL;

    bpm[0] = 0;
    bpm[1] = 0;
    rv = taps_get_bpm_pfx(&bpm[0], 48, _MAX_KEY_LEN_48_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 0) return SOC_E_FAIL;

    /* v6 test */
    bpm[0] = 0x80;
    bpm[1] = 0x00101010;
    rv = taps_get_bpm_pfx(&bpm[0], 144, _MAX_KEY_LEN_144_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 144-4) return SOC_E_FAIL;

    bpm[0] = 0;
    bpm[1] = 0x00010000;
    rv = taps_get_bpm_pfx(&bpm[0], 144, _MAX_KEY_LEN_144_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 144-16) return SOC_E_FAIL;

    bpm[0] = 0;
    bpm[1] = 0x80000000;
    rv = taps_get_bpm_pfx(&bpm[0], 144, _MAX_KEY_LEN_144_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 144-31) return SOC_E_FAIL;

    bpm[0] = 0x1;
    bpm[1] = 0x80000000;
    rv = taps_get_bpm_pfx(&bpm[0], 144, _MAX_KEY_LEN_144_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 144-31) return SOC_E_FAIL;

    bpm[0] = 0;
    bpm[1] = 0;
    rv = taps_get_bpm_pfx(&bpm[0], 144, _MAX_KEY_LEN_144_, &pfx_len);
    if (SOC_FAILURE(rv)) return SOC_E_FAIL;
    if (pfx_len != 0) return SOC_E_FAIL;

    return SOC_E_NONE;
}

int tmu_taps_kshift_ut(void) 
{
    unsigned int key[] = { 0x1234, 0x12345678 }, length=0;

    /* v4 tests */
    _key_shift_left(key, 8);
    if (key[0] != 0x3412 && key[1] != 0x34567800) {
        return SOC_E_FAIL;
    }

    key[0] = 0;
    key[1] = 0x12345678;
    _key_shift_left(key, 15);    

    if (key[0] != (0x12345678 >> 17) && key[1] != (0x12345678 << 15)) {
        return SOC_E_FAIL;
    }

    key[0] = 0x1234;
    key[1] = 0xdeadbeef;
    _key_shift_left(key, 0);    
    if (key[0] != 0x1234 && key[1] != 0xdeadbeef) {
        return SOC_E_FAIL;
    }

    key[0] = 0;
    key[1] = 0;
    _key_append(key, &length, 0xba5e, 16);
    if (key[0] != 0 && key[1] != 0xba5e) {
        return SOC_E_FAIL;
    }

    _key_append(key, &length, 0x3a5eba11, 31);
    if (key[0] != 0xba5e && key[1] != 0x3a5eba11) {
        return SOC_E_FAIL;
    }

    /* v6 tests */

    return SOC_E_NONE;
}

int tmu_trie_split_ut(unsigned int seed) 
{
    int index, rv = SOC_E_NONE, numkey=0, id=0;
    trie_t *trie, *newtrie;
    trie_node_t *newroot;
    payload_t *pyld = sal_alloc(_NUM_KEY_ * sizeof(payload_t), "unit-test");
    trie_node_t *pyldptr = NULL;
    unsigned int pivot[_MAX_KEY_WORDS_], length;

    for (id=0; id < 4; id++) {
        switch(id) {
        case 0:  /* 1:1 split */
            pyld[0].key[0] = 0; pyld[0].key[1] = 0x10; pyld[0].len = _VRF_LEN_ + 8;  /* v=0 p=0x10000000/8  */
            pyld[1].key[0] = 0; pyld[1].key[1] = 0x1000; pyld[1].len = _VRF_LEN_ + 16; /* v=0 p=0x10000000/16 */
            pyld[2].key[0] = 0; pyld[2].key[1] = 0x100000; pyld[2].len = _VRF_LEN_ + 24; /* v=0 p=0x10000000/24 */
            pyld[3].key[0] = 0; pyld[3].key[1] = 0x10000000; pyld[3].len = _VRF_LEN_ + 32; /* v=0 p=0x10000000/48 */
            numkey = 4;
            break;
        case 1: /* 1:1 split */
            pyld[0].key[0] = 0; pyld[0].key[1] = 0x10000000; pyld[0].len = _VRF_LEN_ + 32; /* v=0 p=0x10000000/32 */
            pyld[1].key[0] = 0; pyld[1].key[1] = 0x10000001; pyld[1].len = _VRF_LEN_ + 32; /* v=0 p=0x10000001/32 */
            pyld[2].key[0] = 0; pyld[2].key[1] = 0x10000002; pyld[2].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
            pyld[3].key[0] = 0; pyld[3].key[1] = 0x10000003; pyld[3].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
            pyld[4].key[0] = 0; pyld[4].key[1] = 0x10000004; pyld[4].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
            pyld[5].key[0] = 0; pyld[5].key[1] = 0x10000005; pyld[5].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
            numkey = 6;
            break;
        case 2: /* 2:5 split */
            pyld[0].key[0] = 0; pyld[0].key[1] = 0x100; pyld[0].len = _VRF_LEN_ + 12;
            pyld[1].key[0] = 0; pyld[1].key[1] = 0x1011; pyld[1].len = _VRF_LEN_ + 16;
            pyld[2].key[0] = 0; pyld[2].key[1] = 0x100000; pyld[2].len = _VRF_LEN_ + 24; 
            pyld[3].key[0] = 0; pyld[3].key[1] = 0x1000000; pyld[3].len = _VRF_LEN_ + 28;
            pyld[4].key[0] = 0; pyld[4].key[1] = 0x1001; pyld[4].len = _VRF_LEN_ + 16;
            pyld[5].key[0] = 0; pyld[5].key[1] = 0x10011; pyld[5].len = _VRF_LEN_ + 20;
            numkey = 6;
            break;

        case 3:
        {
            int dup;

            if (seed == 0) {
                seed = sal_time();
                sal_srand(seed);
            }

            index = 0;
            LOG_CLI((BSL_META("Random test: %d Seed: 0x%x \n"), id, seed));
            do {
                do {
                    pyld[index].key[1] = (unsigned int) sal_rand();
                    pyld[index].len = (unsigned int)sal_rand() % 32;
                    pyld[index].len += _VRF_LEN_;

                    if (pyld[index].len <= 32) {
                        pyld[index].key[0] = 0;
                        pyld[index].key[1] &= MASK(pyld[index].len);
                    }

                    if (pyld[index].len > 32) {
                        pyld[index].key[0] = (unsigned int)sal_rand() % 16;                        
                        pyld[index].key[0] &= MASK(pyld[index].len-32); 
                    }

                    dup = ut_check_duplicate(pyld, index);
                    if (dup) {                    
                        LOG_CLI((BSL_META("\n Duplicate at index[%d]:"
                                          "key[0x%08x:0x%08x] Retry!!!\n"), 
                                 index, pyld[index].key[0],
                                 pyld[index].key[1]));
                    }
                } while(dup > 0);
            } while(++index < _NUM_KEY_);

            numkey = index;
       }
       break;

        default:
            return SOC_E_PARAM;
        }

        trie_init(_MAX_KEY_LEN_, &trie);
        trie_init(_MAX_KEY_LEN_, &newtrie);

        for(index=0; index < numkey && rv == SOC_E_NONE; index++) {
            rv = trie_insert(trie, &pyld[index].key[0], NULL, pyld[index].len, &pyld[index].node);
        }

        rv = trie_split(trie, _MAX_KEY_LEN_, FALSE, pivot, &length, &newroot, NULL, FALSE);
        if (SOC_SUCCESS(rv)) {
            LOG_CLI((BSL_META("\n Split Trie Pivot: 0x%08x 0x%08x "
                              "Length: %d Root: %p \n"),
                     pivot[0], pivot[1], length, newroot));
            LOG_CLI((BSL_META(" $Payload Count Old Trie:%d New Trie:%d \n"),
                     trie->trie->count, newroot->count));

            /* set new trie */
            newtrie->trie = newroot;
            newtrie->v6_key = trie->v6_key;
#ifdef VERBOSE
            LOG_CLI((BSL_META("\n OLD Trie Dump ############: \n")));
            trie_dump(trie, NULL, NULL);
            LOG_CLI((BSL_META("\n SPLIT Trie Dump ############: \n")));
            trie_dump(newtrie, NULL, NULL);
#endif
            
            for(index=0; index < numkey && rv == SOC_E_NONE; index++) {
                rv = trie_search(trie, &pyld[index].key[0], pyld[index].len, &pyldptr);
                if (rv != SOC_E_NONE) {
                    rv = trie_search(newtrie, &pyld[index].key[0], pyld[index].len, &pyldptr);
                    if (rv != SOC_E_NONE) {
                        LOG_CLI((BSL_META("SEARCH: Key=0x%x 0x%x len %d SEARCH "
                                          "idx:%d failed on both trie!!!!\n"), 
                                 pyld[index].key[0], pyld[index].key[1],
                                 pyld[index].len, index));
                    } else {
                        assert(pyldptr == &pyld[index].node);
                    }
                } 
            }
            
        }
    }

    trie_destroy(trie);
    trie_destroy(newtrie);
    sal_free(pyld);
    return rv;
}

int tmu_taps_trie_ut(int id, unsigned int seed)
{
    int index, rv = SOC_E_NONE, numkey=0, num_deleted=0;
    trie_t *trie;
    payload_t *pyld = sal_alloc(_NUM_KEY_ * sizeof(payload_t), "unit-test");
    trie_node_t *pyldptr = NULL;
    unsigned int result_len=0, result_key[_MAX_KEY_WORDS_];

    /* keys packed right to left (ie) most significant word starts at index 0*/

    switch(id) {
    case 0:
        pyld[0].key[0] = 0; pyld[0].key[1] = 0x10; pyld[0].len = _VRF_LEN_ + 8;  /* v=0 p=0x10000000/8  */
        pyld[1].key[0] = 0; pyld[1].key[1] = 0x1000; pyld[1].len = _VRF_LEN_ + 16; /* v=0 p=0x10000000/16 */
        pyld[2].key[0] = 0; pyld[2].key[1] = 0x100000; pyld[2].len = _VRF_LEN_ + 24; /* v=0 p=0x10000000/24 */
        pyld[3].key[0] = 0; pyld[3].key[1] = 0x10000000; pyld[3].len = _VRF_LEN_ + 32; /* v=0 p=0x10000000/48 */
        numkey = 4;
        break;

    case 1:
        pyld[0].key[0] = 0; pyld[0].key[1] = 0x123456; pyld[0].len  = _VRF_LEN_ + 24; /* v=0 p=0x12345678/24 */
        pyld[1].key[0] = 0; pyld[1].key[1] = 0x246; pyld[1].len = _VRF_LEN_ + 13; /* v=0 p=0x12345678/13 */
        pyld[2].key[0] = 0; pyld[2].key[1] = 0x24; pyld[2].len = _VRF_LEN_ + 9; /* v=0 p=0x12345678/9 */
        numkey = 3;
        break;

    case 2: /* dup routes on another vrf */
        pyld[0].key[0] = 0; pyld[0].key[1] = 0x1123456; pyld[0].len = _VRF_LEN_ + 24; /* v=1 p=0x12345678/24 */
        pyld[1].key[0] = 0; pyld[1].key[1] = 0x2246; pyld[1].len = _VRF_LEN_ + 13; /* v=1 p=0x12345678/13 */
        pyld[2].key[0] = 0; pyld[2].key[1] = 0x224; pyld[2].len = _VRF_LEN_ + 9; /* v=1 p=0x12345678/9 */
        numkey = 3;
        break;

    case 3:
        pyld[0].key[0] = 0; pyld[0].key[1] = 0x10000000; pyld[0].len = _VRF_LEN_ + 32; /* v=0 p=0x10000000/32 */
        pyld[1].key[0] = 0; pyld[1].key[1] = 0x10000001; pyld[1].len = _VRF_LEN_ + 32; /* v=0 p=0x10000001/32 */
        pyld[2].key[0] = 0; pyld[2].key[1] = 0x10000002; pyld[2].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
        numkey = 3;
        break;

    case 4:
        pyld[0].key[0] = 0; pyld[0].key[1] = 0x12345670; pyld[0].len = _VRF_LEN_ + 32; /* v=0 p=0x12345670/32 */
        pyld[1].key[0] = 0; pyld[1].key[1] = 0x12345671; pyld[1].len = _VRF_LEN_ + 32; /* v=0 p=0x12345671/32 */
        pyld[2].key[0] = 0; pyld[2].key[1] = 0x91a2b38;  pyld[2].len = _VRF_LEN_ + 31; /* v=0 p=0x12345670/31 */
        numkey = 3;
        break;

    case 5:
        pyld[0].key[0] = 0; pyld[0].key[1] = 0x20; pyld[0].len = _VRF_LEN_ + 8; /* v=0 p=0x20000000/8 */
        pyld[1].key[0] = 0; pyld[1].key[1] = 0x8000; pyld[1].len = _VRF_LEN_ + 16; /* v=0 p=0x80000000/16 */
        pyld[2].key[0] = 0; pyld[2].key[1] = 0; pyld[2].len = _VRF_LEN_ + 0; /* v=0 p=0/0 */
        numkey = 3;
        break;

    case 6:
        {
            int dup;

            if (seed == 0) {
                seed = sal_time();
                sal_srand(seed);
            }
            index = 0;
            LOG_CLI((BSL_META("Random test: %d Seed: 0x%x \n"), id, seed));
            do {
                do {
                    pyld[index].key[1] = (unsigned int) sal_rand();
                    pyld[index].len = (unsigned int)sal_rand() % 32;
                    pyld[index].len += _VRF_LEN_;

                    if (pyld[index].len <= 32) {
                        pyld[index].key[0] = 0;
                        pyld[index].key[1] &= MASK(pyld[index].len);
                    }

                    if (pyld[index].len > 32) {
                        pyld[index].key[0] = (unsigned int)sal_rand() % 16;                        
                        pyld[index].key[0] &= MASK(pyld[index].len-32); 
                    }

                    dup = ut_check_duplicate(pyld, index);
                    if (dup) {
                            LOG_CLI((BSL_META("\n Duplicate at index[%d]:"
                                              "key[0x%08x:0x%08x] Retry!!!\n"), 
                                     index, pyld[index].key[0],
                                     pyld[index].key[1]));
                    }
                } while(dup > 0);
            } while(++index < _NUM_KEY_);

            numkey = index;
        }
        break;

    default:
        sal_free(pyld);      
        return -1;
    }

    trie_init(_MAX_KEY_LEN_, &trie);
    LOG_CLI((BSL_META("\n Num keys to test= %d \n"), numkey));

    for(index=0; index < numkey && rv == SOC_E_NONE; index++) {
        unsigned int vrf=0, i;
        vrf = (pyld[index].len - _VRF_LEN_ == 32) ? 0:pyld[index].key[1] >> (pyld[index].len - _VRF_LEN_);
        vrf |= pyld[index].key[0] << (32 - (pyld[index].len - _VRF_LEN_));

#ifdef LOG
        LOG_CLI((BSL_META("+ Inserted Key=0x%x 0x%x vpn=0x%x pfx=0x%x "
                          "Len=%d idx:%d\n"), 
                 pyld[index].key[0], pyld[index].key[1], vrf,
                 pyld[index].key[1] & MASK(pyld[index].len - _VRF_LEN_), 
                 pyld[index].len, index));
#endif
        rv = trie_insert(trie, &pyld[index].key[0], NULL, pyld[index].len, &pyld[index].node);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META("FAILED to Insert Key=0x%x 0x%x vpn=0x%x "
                              "pfx=0x%x Len=%d idx:%d\n"), 
                     pyld[index].key[0], pyld[index].key[1], vrf,
                     pyld[index].key[1] & MASK(pyld[index].len - _VRF_LEN_), 
                     pyld[index].len, index));
        }
#define _VERBOSE_SEARCH_
        /* search all keys & figure out breakage right away */
        for (i=0; i <= index && rv == SOC_E_NONE; i++) {
#ifdef _VERBOSE_SEARCH_
            result_key[0] = 0;
            result_key[1] = 0;
            result_len    = 0;
            rv = trie_search_verbose(trie, &pyld[index].key[0], pyld[index].len, 
                                     &pyldptr, &result_key[0], &result_len);
#else
            rv = trie_search(trie, &pyld[index].key[0], pyld[index].len, &pyldptr);
#endif
            if (rv != SOC_E_NONE) {
                LOG_CLI((BSL_META("SEARCH: Key=0x%x 0x%x len %d SEARCH "
                                  "idx:%d failed!!!!\n"), 
                         pyld[index].key[0], pyld[index].key[1],
                         pyld[index].len, index));
                break;
            } else {
                assert(pyldptr == &pyld[index].node);
#ifdef _VERBOSE_SEARCH_
                if (pyld[index].key[0] != result_key[0] ||
                    pyld[index].key[1] != result_key[1] ||
                    pyld[index].len != result_len) {
                    LOG_CLI((BSL_META(" Found key mismatches with the "
                                      "expected Key !!!! \n")));
                    rv = SOC_E_FAIL;
                }
#ifdef VERBOSE
                LOG_CLI((BSL_META("Lkup[%d] key/len: 0x%x 0x%x/%d "
                                  "Found Key/len: 0x%x 0x%x/%d \n"),
                         index ,pyld[index].key[0], pyld[index].key[1],
                         pyld[index].len,
                         result_key[0], result_key[1], result_len));
#endif
#endif
            }
        }
    }

#ifdef VERBOSE
    LOG_CLI((BSL_META("\n============== TRIE DUMP ================\n")));
    trie_dump(trie, NULL, NULL);
    LOG_CLI((BSL_META("\n=========================================\n")));
#endif

    /* randomly pickup prefix & delete */
    while(num_deleted < numkey && rv == SOC_E_NONE) {
        index = sal_rand() % numkey;
        if (pyld[index].len != 0xFFFFFFFF) {
            rv = trie_search(trie, &pyld[index].key[0], pyld[index].len, &pyldptr);
            if (rv == SOC_E_NONE) {
                assert(pyldptr == &pyld[index].node);
                rv = trie_delete(trie, &pyld[index].key[0], pyld[index].len, &pyldptr);

#ifdef VERBOSE
                LOG_CLI((BSL_META("\n============== TRIE DUMP ================\n")));
                trie_dump(trie, NULL, NULL);
#endif
                if (rv == SOC_E_NONE) {
#ifdef LOG
                    LOG_CLI((BSL_META("Deleted Key=0x%x 0x%x Len=%d idx:%d "
                                      "Num-Key:%d\n"), 
                             pyld[index].key[0], pyld[index].key[1], 
                             pyld[index].len, index, num_deleted));
#endif
                    pyld[index].len = 0xFFFFFFFF;
                    num_deleted++;

                    /* search all keys & figure out breakage right away */
                    for (index=0; index < numkey; index++) {
                        if (pyld[index].len == 0xFFFFFFFF) continue;

                        rv = trie_search(trie, &pyld[index].key[0], pyld[index].len, &pyldptr);
                        if (rv != SOC_E_NONE) {
                            LOG_CLI((BSL_META("ALL SEARCH after delete: "
                                              "Key=0x%x 0x%x len %d SEARCH "
                                              "idx:%d failed!!!!\n"), 
                                     pyld[index].key[0], pyld[index].key[1],
                                     pyld[index].len, index));
                            break;
                        } else {
                            assert(pyldptr == &pyld[index].node);
                        }
                    }
                } else {
                    LOG_CLI((BSL_META("Deleted Key=0x%x 0x%x Len=%d idx:%d "
                                      "FAILED!!!\n"), 
                             pyld[index].key[0], pyld[index].key[1], 
                             pyld[index].len, index));
                    break;
                }
            } else {
                LOG_CLI((BSL_META("SEARCH: Key=0x%x 0x%x len %d SEARCH "
                                  "idx:%d failed!!!!\n"), 
                         pyld[index].key[0], pyld[index].key[1],
                         pyld[index].len, index));
                break;
            }
        }
    }

    if (rv == SOC_E_NONE) {
        LOG_CLI((BSL_META("\n TEST ID %d passed \n"), id));
    }
    else {  
        LOG_CLI((BSL_META("\n TEST ID %d Failed Num Delete:%d !!!!!!!!\n"),
                 id, num_deleted));
    }

    sal_free(pyld);
    trie_destroy(trie);
    return rv;
}

/**********************************************/
/* BPM unit tests */
/* test cases:
 * 1 - insert pivot's with bpm bit masks
 * 2 - propagate updated prefix bpm (add/del)
 * 3 - fuse node bpm verification
 * 4 - split bpm - nop
 * 5 - */

typedef struct _expect_datum_s {
    dq_t list;
    payload_t *pfx; 
    trie_t *pfx_trie;
} expect_datum_t;

int ut_bpm_build_expect_list(trie_node_t *payload, void *user_data)
{
    int rv=SOC_E_NONE;

    if (payload && payload->type == PAYLOAD) {
        trie_node_t *pyldptr;
        payload_t *pivot;
        expect_datum_t *datum = (expect_datum_t*)user_data;

        pivot = TRIE_ELEMENT_GET(payload_t*, payload, node);
        /* if the inserted prefix is a best prefix, add the pivot to expected list */
        rv = trie_find_lpm(datum->pfx_trie, &pivot->key[0], pivot->len, &pyldptr); 
        assert(rv == SOC_E_NONE);
        if (pyldptr == &datum->pfx->info.pfx_trie_node) {
            /* if pivot is not equal to prefix add to expect list */
            if (!(pivot->key[0] == datum->pfx->key[0] && 
                  pivot->key[1] == datum->pfx->key[1] &&
                  pivot->len    == datum->pfx->len)) {
                DQ_INSERT_HEAD(&datum->list, &pivot->listnode);
            }
        }
    }

    return SOC_E_NONE;
}

int ut_bpm_propagate_cb(trie_node_t *payload, trie_bpm_cb_info_t *cbinfo)
{
    if (payload && cbinfo && payload->type == PAYLOAD) {
        payload_t *pivot;
        dq_p_t elem;
        expect_datum_t *datum = (expect_datum_t*)cbinfo->user_data;

        pivot = TRIE_ELEMENT_GET(payload_t*, payload, node);
        DQ_TRAVERSE(&datum->list, elem) {
            payload_t *velem = DQ_ELEMENT_GET(payload_t*, elem, listnode); 
            if (velem == pivot) {
                DQ_REMOVE(&pivot->listnode);
                break;
            }
        } DQ_TRAVERSE_END(&datum->list, elem);
    }

    return SOC_E_NONE;
}

int ut_bpm_propagate_empty_cb(trie_node_t *payload, trie_bpm_cb_info_t *cbinfo)
{
    /* do nothing */
    return SOC_E_NONE;
}

void ut_bpm_dump_expect_list(expect_datum_t *datum, char *str)
{
    dq_p_t elem;
    if (datum) {
        /* dump expected list */
        LOG_CLI((BSL_META("%s \n"), str));
        DQ_TRAVERSE(&datum->list, elem) {
            payload_t *velem = DQ_ELEMENT_GET(payload_t*, elem, listnode); 
            LOG_CLI((BSL_META(" Pivot: 0x%x 0x%x Len: %d \n"), 
                     velem->key[0], velem->key[1], velem->len));
        } DQ_TRAVERSE_END(&datum->list, elem);
    }
}

#define _MAX_TEST_PIVOTS_ (10)
#define _MAX_BKT_PFX_ (20)
#define _MAX_NUM_PICK (30)

int tmu_taps_bpm_trie_ut(int id, unsigned int seed)
{
    int rv = SOC_E_NONE, pivot=0, pfx=0, index=0, dup=0, domain=0;
    trie_t *pfx_trie, *trie;
    payload_t *pyld = sal_alloc(_MAX_BKT_PFX_ * _MAX_TEST_PIVOTS_ * sizeof(payload_t), "bpm-unit-test");
    payload_t *pivot_pyld = sal_alloc(_MAX_TEST_PIVOTS_ * sizeof(payload_t), "bpm-unit-test");
    trie_node_t *pyldptr = NULL, *newroot;
    unsigned int bpm[BITS2WORDS(_MAX_KEY_LEN_)];
    expect_datum_t datum;
    trie_bpm_cb_info_t cbinfo;
    int num_pick, bpm_pfx_len;

    sal_memset(pyld, 0, _MAX_BKT_PFX_ * _MAX_TEST_PIVOTS_ * sizeof(payload_t));
    sal_memset(pivot_pyld, 0, _MAX_TEST_PIVOTS_ * sizeof(payload_t));

    if (seed == 0) {
        seed = sal_time();
        sal_srand(seed);
    }    

    trie_init(_MAX_KEY_LEN_, &trie);
    trie_init(_MAX_KEY_LEN_, &pfx_trie);

    /* populate a random pivot / prefix trie */
    LOG_CLI((BSL_META("Random test: %d Seed: 0x%x \n"), id, seed));

    /* insert a vrf=0,* pivot */
    pivot = 0;
    pfx = 0;
    pivot_pyld[pivot].key[1] = 0;
    pivot_pyld[pivot].key[0] = 0;
    pivot_pyld[pivot].len    = 0;
    trie_init(_MAX_KEY_LEN_, &pivot_pyld[pivot].info.trie);
    sal_memset(&bpm[0], 0,  BITS2WORDS(_MAX_KEY_LEN_) * sizeof(unsigned int));
            
    do {
        rv = trie_insert(trie, &pivot_pyld[pivot].key[0], &bpm[0], 
                         pivot_pyld[pivot].len, &pivot_pyld[pivot].node);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META("FAILED to Insert PIVOT Key=0x%x 0x%x Len=%d idx:%d\n"), 
                     pivot_pyld[pivot].key[0], pivot_pyld[pivot].key[1], 
                     pivot_pyld[pivot].len, pivot));
        } else {
            if (pivot > 0) {
                /* choose a random pivot bucket to fill & split */
                domain = ((unsigned int) sal_rand()) % pivot;
            } else {
                domain = 0;
            }
            
            index = 0;
            sal_memset(&bpm[0], 0,  BITS2WORDS(_MAX_KEY_LEN_) * sizeof(unsigned int));
    
            do {
                do {
                    /* add prefix such that lpm of the prefix is the pivot to ensure
                     * it goes into specific pivot domain */
                    pyld[pfx+index].key[1] = (unsigned int) sal_rand();
                    pyld[pfx+index].len = (unsigned int)sal_rand() % 32;
                    pyld[pfx+index].len += _VRF_LEN_;

                    if (pyld[pfx+index].len <= 32) {
                        pyld[pfx+index].key[0] = 0;
                        pyld[pfx+index].key[1] &= MASK(pyld[pfx+index].len);
                    }

                    if (pyld[pfx+index].len > 32) {
                        pyld[pfx+index].key[0] = (unsigned int)sal_rand() % 16;                        
                        pyld[pfx+index].key[0] &= MASK(pyld[pfx+index].len-32); 
                    }

                    dup = ut_check_duplicate(pyld, pfx+index);
                    if (!dup) {
                        rv = trie_find_lpm(trie, &pyld[pfx+index].key[0], pyld[pfx+index].len, &pyldptr); 
                        if (SOC_FAILURE(rv)) {
                            LOG_CLI((BSL_META("\n !! Failed to find LPM pivot for "
                                              "index[%d]:key[0x%08x:0x%08x] !!!!\n"),
                                     pfx, pyld[pfx+index].key[0],
                                     pyld[pfx+index].key[1]));
                        } 
                    }
                } while ((dup || (pyldptr != &pivot_pyld[domain].node)) && SOC_SUCCESS(rv));

                if (SOC_SUCCESS(rv)) {
                    rv =  trie_insert(pivot_pyld[domain].info.trie,
                                      &pyld[pfx+index].key[0], NULL, 
                                      pyld[pfx+index].len, &pyld[pfx+index].node);
                    if (SOC_FAILURE(rv)) {
                        LOG_CLI((BSL_META("\n !! Failed insert prefix into pivot trie"
                                          " index[%d]:key[0x%08x:0x%08x] !!!!\n"),
                                 pfx+index, pyld[pfx+index].key[0],
                                 pyld[pfx+index].key[1]));
                    } else {
                        rv =  trie_insert(pfx_trie,
                                          &pyld[pfx+index].key[0], NULL, 
                                          pyld[pfx+index].len, &pyld[pfx+index].info.pfx_trie_node);     
                        if (SOC_FAILURE(rv)) {
                            LOG_CLI((BSL_META("\n !! Failed insert prefix into "
                                              "prefix trie"
                                              " index[%d]:key[0x%08x:0x%08x] !!!!\n"),
                                     pfx+index, pyld[pfx+index].key[0],
                                     pyld[pfx+index].key[1]));
                        } else {
                            index++;
                        }                      
                    }
                }

            } while(index < (_MAX_BKT_PFX_/2 - 1) && SOC_SUCCESS(rv));

            /* try to populate prefix where p == v */
            if (pivot > 0) {
                /* 25% probability */
                if (((unsigned int) sal_rand() % 4) == 0) {
                }
            }

#ifdef VERBOSE
            LOG_CLI((BSL_META("### Split Domain ID: %d \n"), domain));
            for (i=0; i <= pivot; i++) {
                LOG_CLI((BSL_META("\n --- TRIE domain dump: Pivot: 0x%x 0x%x "
                                  "len=%d ----- \n"),
                         pivot_pyld[i].key[0], pivot_pyld[i].key[1],
                         pivot_pyld[i].len));
                trie_dump(pivot_pyld[i].info.trie, ut_print_payload_node, NULL);
            }
#endif

            if (SOC_SUCCESS(rv) && ++pivot < _MAX_TEST_PIVOTS_) {
                pfx += index;
                trie_init(_MAX_KEY_LEN_, &pivot_pyld[pivot].info.trie);
                /* split the domain & insert a new pivot */
                rv = trie_split(pivot_pyld[domain].info.trie,
				_MAX_KEY_LEN_, FALSE,
                                &pivot_pyld[pivot].key[0], 
                                &pivot_pyld[pivot].len, &newroot, 
                                &bpm[0], FALSE);
                if (SOC_SUCCESS(rv)) {
                    pivot_pyld[pivot].info.trie->trie = newroot;
                    pivot_pyld[pivot].info.trie->v6_key = pivot_pyld[domain].info.trie->v6_key;
                    LOG_CLI((BSL_META("BPM for split pivot: 0x%x 0x%x / "
                                      "%d = [0x%x 0x%x] \n"),
                             pivot_pyld[pivot].key[0],
                             pivot_pyld[pivot].key[1],
                             pivot_pyld[pivot].len, bpm[0], bpm[1]));
                } else {
                    LOG_CLI((BSL_META("\n !!! Failed to split domain trie for "
                                      "domain: %d !!!\n"), domain));
                }
            }
        }
    } while(pivot < _MAX_TEST_PIVOTS_ && SOC_SUCCESS(rv));

    /* pick up the root node on pivot trie & add a prefix shorter than the nearest child.
     * This is ripple & create huge propagation */
    /* insert *\/1 into the * bucket so huge propagation kicks in */
    pyld[pfx].key[1] = (unsigned int) sal_rand() % 1;
    pyld[pfx].key[0] = 0;
    pyld[pfx].len    = 1;
    do {
        dup = ut_check_duplicate(pyld, pfx);
        if (!dup) {
            rv = trie_find_lpm(trie, &pyld[pfx].key[0], pyld[pfx].len, &pyldptr); 
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META("\n !! Failed to find LPM pivot for "
                                  "index[%d]:key[0x%08x:0x%08x] !!!!\n"),
                         pfx, pyld[pfx].key[0], pyld[pfx].key[1]));
            } 
        } else {
            pyld[pfx].len++;
        }
    } while(dup && SOC_SUCCESS(rv));

    if (SOC_SUCCESS(rv)) {
        rv =  trie_insert(pfx_trie,
                          &pyld[pfx].key[0], NULL, 
                          pyld[pfx].len, &pyld[pfx].info.pfx_trie_node);
        if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META("\n !! Failed insert prefix into pivot trie"
                              " index[%d]:key[0x%08x:0x%08x] !!!!\n"),
                     pfx, pyld[pfx].key[0], pyld[pfx].key[1]));
        } else {
            DQ_INIT(&datum.list);
            datum.pfx = &pyld[pfx];
            datum.pfx_trie = pfx_trie;
            /* create expected list of pivot to be propagated */
            trie_traverse(trie, ut_bpm_build_expect_list, &datum, _TRIE_PREORDER_TRAVERSE);

            /* dump expected list */
            ut_bpm_dump_expect_list(&datum, "-- Expected Propagation List --");
        }
    }

    sal_memset(&cbinfo, 0, sizeof(trie_bpm_cb_info_t));
    cbinfo.user_data = &datum;
    cbinfo.pfx = &pyld[pfx].key[0];
    cbinfo.len = pyld[pfx].len;
    if (pyldptr == NULL) {
        assert(0); /* check here for coverity */
    }
    rv = trie_pivot_propagate_prefix(pyldptr,
                               (TRIE_ELEMENT_GET(payload_t*, pyldptr, node))->len,
                               &pyld[pfx].key[0], pyld[pfx].len,
                               1, ut_bpm_propagate_cb, &cbinfo);
    if (DQ_EMPTY(&datum.list)) {
        LOG_CLI((BSL_META("++ Propagation Test Passed \n")));
    } else {
        LOG_CLI((BSL_META("!!!!! Propagation Test FAILED !!!!!\n")));
        rv = SOC_E_FAIL;
        ut_bpm_dump_expect_list(&datum, "!! Zombies on Propagation List !!");
        assert(0);
    }

    /* propagate a shorter prefix of an existing pivot 
     * we should find the bpm
     */
    pfx++;
    num_pick = 0;
    do {
	/* randomly pick a pivot */
	index = ((unsigned int) sal_rand()) % pivot;
	
	/* create a prefix shorter */
	pyld[pfx].len    = ((unsigned int) sal_rand()) % pivot_pyld[index].len;
	pyld[pfx].key[1] = pivot_pyld[index].key[1]>>(pivot_pyld[index].len - pyld[pfx].len);
	pyld[pfx].key[0] = 0;

	if (pyld[pfx].len >= 1) {
	    /* propagate add len=0 */
	    rv = trie_pivot_propagate_prefix(trie->trie,
				       (TRIE_ELEMENT_GET(payload_t*, trie->trie, node))->len,
				       &pyld[pfx].key[0], 0,
				       1, ut_bpm_propagate_empty_cb, 
				       &cbinfo);

	    if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META("!!!!! BPM search Test FAILED to propagate "
                              "add len=0!!!!!\n")));
		assert(0);
	    }

	    /* propagate add */
	    rv = trie_pivot_propagate_prefix(trie->trie,
				       (TRIE_ELEMENT_GET(payload_t*, trie->trie, node))->len,
				       &pyld[pfx].key[0], pyld[pfx].len,
				       1, ut_bpm_propagate_empty_cb, 
				       &cbinfo);
	    if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META("!!!!! BPM search Test FAILED to propagate add \n"
                              " index[%d]:key[0x%08x:0x%08x] len=%d!!!!\n"),
                     pfx, pyld[pfx].key[0], pyld[pfx].key[1], pyld[pfx].len));
		assert(0);
	    }

	    /* perform bpm lookup on the pivot, we should find the pyld[pfx].len */
	    rv = trie_find_prefix_bpm(trie, (unsigned int *)&(pivot_pyld[index].key[0]),
				      pivot_pyld[index].len, (unsigned int *)&bpm_pfx_len);
	    if (SOC_FAILURE(rv) || (bpm_pfx_len != pyld[pfx].len)) {
            LOG_CLI((BSL_META("!!!!! BPM search Test FAILDED after propagate "
                              "add !!!!!\n")));
		assert(0);		
	    }

	    /* propagate delete */
	    rv = trie_pivot_propagate_prefix(trie->trie,
				       (TRIE_ELEMENT_GET(payload_t*, trie->trie, node))->len,
				       &pyld[pfx].key[0], pyld[pfx].len,
				       0, ut_bpm_propagate_empty_cb, 
				       &cbinfo);
	    
	    if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META("!!!!! BPM search Test FAILED to propagate add \n"
                              " index[%d]:key[0x%08x:0x%08x] len=%d!!!!\n"),
                     pfx, pyld[pfx].key[0], pyld[pfx].key[1], pyld[pfx].len));
		assert(0);
	    }

	    /* perform bpm lookup on the pivot, we should find the len==0 */
	    rv = trie_find_prefix_bpm(trie, (unsigned int *)&(pivot_pyld[index].key[0]),
				      pivot_pyld[index].len, (unsigned int *)&bpm_pfx_len);
	    if (SOC_FAILURE(rv) || (bpm_pfx_len != 0)) {
            LOG_CLI((BSL_META("!!!!! BPM search Test FAILDED after propagate "
                              "delete !!!!!\n")));
		assert(0);		
	    }

	    num_pick = _MAX_NUM_PICK+1;
	}
	num_pick++;
    } while(num_pick<_MAX_NUM_PICK);

    if (num_pick <= _MAX_NUM_PICK) {
        LOG_CLI((BSL_META("!!!!! BPM search Test 2 Skipped after "
                          "tried %d times!!!!!\n"), _MAX_NUM_PICK));	
    } else {
        LOG_CLI((BSL_META("!!!!! BPM search Test 2 Passed!!!!!\n")));	
    }

#ifdef VERBOSE
    LOG_CLI((BSL_META("\n ----- Prefix Trie dump ----- \n")));
    trie_dump(pfx_trie, ut_print_prefix_payload_node, NULL);
#endif

    LOG_CLI((BSL_META("\n ++++++++ Trie dump ++++++++ \n")));
    trie_dump(trie, ut_print_payload_node, NULL);

    /* clean up */
    for (index=0; index < pivot; index++) {
#ifdef VERBOSE
        LOG_CLI((BSL_META("\n ddddddd dump dddddddd \n")));
        trie_dump(pivot_pyld[index].info.trie, ut_print_payload_node, NULL);
#endif
        trie_destroy(pivot_pyld[index].info.trie);
    }

    sal_free(pyld);
    sal_free(pivot_pyld);
    trie_destroy(trie);
    trie_destroy(pfx_trie);
    return rv;
}

/**********************************************/
#endif 

#endif /* BCM_TRIDENT2_SUPPORT */
#endif /* ALPM_ENABLE */

