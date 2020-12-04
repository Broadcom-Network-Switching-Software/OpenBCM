/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * trie data structure
 *
 *-----------------------------------------------------------------------------*/
#ifndef _ALPM_LIB_TRIE_H_
#define _ALPM_LIB_TRIE_H_

#ifdef ALPM_ENABLE

#ifdef ALPMSIM
#include <bcm_int/esw/alpm_stub.h>
#else
#include <sal/types.h>
#include <soc/error.h>
#include <assert.h>
#include <shared/bsl.h>
#endif /* !ALPMSIM */

typedef struct alpm_lib_trie_node_s alpm_lib_trie_node_t;

typedef enum alpm_lib_trie_node_type_e {
    trieNodeTypeInternal,
    trieNodeTypePayload,
    trieNodeTypeInternalPpg,
    trieNodeTypeMax
} alpm_lib_trie_node_type_t;

struct alpm_lib_trie_node_s {
    alpm_lib_trie_node_t *trie_node;
    alpm_lib_trie_node_t *child[2];
    uint32 skip_len;
    uint32 skip_addr;
    alpm_lib_trie_node_type_t type;
    uint32 count;   /* number of payload node counts */
};

typedef struct alpm_lib_trie_s {
    alpm_lib_trie_node_t *trie;  /* trie root pointer */
    uint32 v6_key;      /* support 144 bits key, otherwise expect 48 bits key */
} alpm_lib_trie_t;

typedef int (*alpm_lib_trie_callback_f)(alpm_lib_trie_node_t *trie, void *datum);

typedef int (*alpm_lib_trie_callback1_f)(alpm_lib_trie_node_t *trie,
                                         uint32 *key, uint32 key_len, void *datum);

typedef struct alpm_lib_trie_bpm_cb_info_s {
    uint32  *pfx;   /* prefix buffer pointer from caller space */
    uint32  len;    /* prefix length */
    void    *user_data;
} alpm_lib_trie_bpm_cb_info_t;

typedef int (*alpm_lib_trie_ppg_cb_f)(alpm_lib_trie_node_t *trie, alpm_lib_trie_bpm_cb_info_t *info);

typedef struct trie_list_s {
    alpm_lib_trie_node_t *node;
    struct trie_list_s *next;
} trie_list_t;

typedef enum alpm_lib_trie_traverse_order_e {
    trieTraverseOrderPre,  /* root, left, right */
    trieTraverseOrderIn,   /* left, root, right */
    trieTraverseOrderPost, /* left, right, root */
    trieTraverseOrderMax
} alpm_lib_trie_traverse_order_t;

typedef enum alpm_lib_trie_traverse_state_e {
    trieTraverseStateNone,
    trieTraverseStateDel,
    trieTraverseStateDone,
    trieTraverseStateMax
} alpm_lib_trie_traverse_state_t;

#define TRIE_TRAVERSE_STOP(state, rv) \
    {if (state == trieTraverseStateDone || rv < 0) {return rv;} }

typedef int (*alpm_lib_trie_callback_ext_f)(alpm_lib_trie_node_t *ptrie, alpm_lib_trie_node_t *trie,
                                   alpm_lib_trie_traverse_state_t *state, void *info);

typedef int (*alpm_lib_trie_callback_ext3_f)(alpm_lib_trie_node_t *pptrie,
                                             alpm_lib_trie_node_t *ptrie, alpm_lib_trie_node_t *trie,
                                             uint32 *prt_key,
                                             uint32 prt_key_len,
                                   alpm_lib_trie_traverse_state_t *state, void *info);

typedef enum alpm_lib_trie_split_state_e {
    trieSplitStateNone,
    trieSplitStatePayloadSplit,
    trieSplitStatePayloadSplitDone,
    trieSplitStatePruneNodes,
    trieSplitStateDone,
    trieSplitStateMax
} alpm_lib_trie_split_state_t;

typedef enum alpm_lib_trie_split2_state_e {
    trieSplit2StateNone,
    trieSplit2StatePruneNodes,
    trieSplit2StateDone,
    trieSplit2StateMax
} alpm_lib_trie_split2_state_t;

/*
 * This macro is a tidy way of performing subtraction to move from a
 * pointer within an object to a pointer to the object.
 *
 * Arguments are:
 *    type of object to recover
 *    pointer to object from which to recover element pointer
 *    pointer to an object of type t
 *    name of the trie node field in t through which the object is linked on trie
 * Returns:
 *    a pointer to the object, of type t
 */
#define TRIE_ELEMENT(t, p, ep, f) \
  ((t) (((char *) (p)) - (((char *) &((ep)->f)) - ((char *) (ep)))))

/*
 * TRIE_ELEMENT_GET performs the same function as TRIE_ELEMENT, but does not
 * require a pointer of type (t).  This form is preferred as TRIE_ELEMENT
 * typically generate Coverity errors, and the (ep) argument is unnecessary.
 *
 * Arguments are:
 *    type of object to recover
 *    pointer to object from which to recover element pointer
 *    name of the trie node field in t through which the object is linked on trie
 * Returns:
 *    a pointer to the object, of type t
 */
#define TRIE_ELEMENT_GET(t, p, f) \
  ((t) (((char *) (p)) - (((char *) &(((t)(0))->f)))))


#define _TRIE_NODE_CLONE_(dest,src) \
    sal_memcpy((dest),(src),sizeof(alpm_lib_trie_node_t))

/* allocates a trie & initializes it */
extern int alpm_lib_trie_init(uint32 max_key_len, alpm_lib_trie_t **ptrie);

/* destroys a trie */
extern int alpm_lib_trie_destroy(alpm_lib_trie_t *trie);

/* Inserts provided prefix/length in to the trie */
extern int alpm_lib_trie_insert(alpm_lib_trie_t *trie, uint32 *key, uint32 length, alpm_lib_trie_node_t *payload);

/* Deletes provided prefix/length in to the trie */
extern int alpm_lib_trie_delete(alpm_lib_trie_t *trie, uint32 *key, uint32 length, alpm_lib_trie_node_t **payload);

/* Search the given trie for provided prefix/length */
extern int alpm_lib_trie_search(alpm_lib_trie_t *trie, uint32 *key, uint32 length, alpm_lib_trie_node_t **payload);

/* Dumps the trie pre-order [root|left|child] */
extern int alpm_lib_trie_dump(alpm_lib_trie_t *trie, alpm_lib_trie_callback_f cb, void *user_data);

/* Dumps the trie pre-order [root|left|child] including internal node key and length */
extern int alpm_lib_trie_dump1(alpm_lib_trie_t *trie, alpm_lib_trie_callback1_f cb, void *user_data);

/* Find the longest prefix matched with given prefix */
extern int alpm_lib_trie_find_lpm(alpm_lib_trie_t *trie, uint32 *key, uint32 length, alpm_lib_trie_node_t **payload);

/* Split the trie into 2 based on optimum pivot */
extern int alpm_lib_trie_split(alpm_lib_trie_t *trie, const uint32 max_split_len,
                               uint32 *pivot, uint32 *length,
                               alpm_lib_trie_node_t **split_trie_root,
                               /* if set split will strictly split only on payload nodes
                                * if not set splits at optimal point on the trie */
                               uint8 payload_node_split,
                               alpm_lib_trie_callback_ext_f cb,
                               void *user_data,
                               int max_split_count);

/* unsplit or fuse the child trie with parent trie */
extern int alpm_lib_trie_merge(alpm_lib_trie_t *parent_trie, alpm_lib_trie_node_t *child_trie, uint32 *child_pivot, uint32 length);

/* Split the trie such that the new sub trie covers given prefix/length. Basically this is a reverse of alpm_lib_trie_merge. */
extern int alpm_lib_trie_split2(alpm_lib_trie_t *trie, uint32 *key, uint32 key_len, uint32 *pivot, uint32 *pivot_len, alpm_lib_trie_node_t **split_trie_root, const int max_split_count, const int exact_same);

/* Traverse the trie & call the application callback with user data */
extern int alpm_lib_trie_traverse(alpm_lib_trie_t *trie, alpm_lib_trie_callback_f cb, void *user_data, alpm_lib_trie_traverse_order_t order, int only_payload);

/* Traverse the trie (trieNodeTypePayload) & call the extended application callback which has current node's trieNodeTypePayload parent node with user data. */
extern int alpm_lib_trie_traverse2(alpm_lib_trie_t *trie, alpm_lib_trie_callback_ext_f cb, void *user_data, alpm_lib_trie_traverse_order_t order);

/* Traverse the trie (trieNodeTypePayload) & call the extended application callback which has current node's payload or internal parent node with user data. */
extern int alpm_lib_trie_traverse3(alpm_lib_trie_t *trie, alpm_lib_trie_callback_ext3_f cb, void *user_data, alpm_lib_trie_traverse_order_t order);

extern int _alpm_lib_trie_ppg_prefix(alpm_lib_trie_node_t *pivot, uint32 pivot_len, uint32 *pfx, uint32 len, alpm_lib_trie_ppg_cb_f cb, alpm_lib_trie_bpm_cb_info_t *cb_info);

extern int alpm_lib_trie_ppg(alpm_lib_trie_t *trie, uint32 pivot_len, uint32 *pfx, uint32 len, alpm_lib_trie_ppg_cb_f cb, alpm_lib_trie_bpm_cb_info_t *cb_info);

/* ====================== v6 ======================== */

/* get bit at bit_position from uint32 key array of maximum length of max_len
 * assuming the big-endian word order. bit_pos is 0 based. for example, assuming
 * the max_len is 48 bits, then key[0] has bits 47-32, and key[1] has bits 31-0.
 * use _TAPS_GET_KEY_BIT(key, 0, 48) to get bit 0.
 */
#define TP_BITS2IDX(x) ((BITS2WORDS(_MAX_KEY_LEN_144_) - 1) - (x)/32)

/* Get "len" number of bits start with lsb bit postion from an uint32 array
 * the array is assumed to be with format described above in _TAPS_GET_KEY_BIT
 * NOTE: len must be <= 32 bits
 */
#define _TAPS_GET_KEY_BITS(key, lsb, len)            \
  ((TRIE_SHR((key)[TP_BITS2IDX(lsb)], ((lsb)%_NUM_WORD_BITS_), _NUM_WORD_BITS_) |  \
    ((TP_BITS2IDX(lsb)<1)?0:(TRIE_SHL((key)[TP_BITS2IDX(lsb)-1], _NUM_WORD_BITS_-((lsb)%_NUM_WORD_BITS_), _NUM_WORD_BITS_)))) & \
   (BITMASK((len))))

extern int _alpm_lib_trie_v6_search(alpm_lib_trie_node_t *trie,
               uint32 *key,
               uint32 length,
               alpm_lib_trie_node_t **payload,
               uint32 *result_key,
               uint32 *result_len,
               uint32 dump,
               uint32 find_pivot);

extern int _alpm_lib_trie_v6_find_lpm(alpm_lib_trie_node_t *trie,
                 uint32 *key,
                 uint32 length,
                 alpm_lib_trie_node_t **payload,
                 uint32 exclude_self);

extern int _alpm_lib_trie_skip_node_free(alpm_lib_trie_node_t *trie,
                                         int max_key_len, uint32 *key, uint32 length);

extern int _alpm_lib_trie_v6_skip_node_alloc(alpm_lib_trie_node_t **node,
                    uint32 *key,
                    uint32 msb, /* NOTE: valid msb position 1 based, 0 means skip0/0 node */
                    uint32 skip_len,
                    alpm_lib_trie_node_t *payload,
                    uint32 count);

extern int _alpm_lib_trie_v6_insert(alpm_lib_trie_node_t *trie,
               uint32 *key,
               uint32 length,
               alpm_lib_trie_node_t *payload, /* payload node */
               alpm_lib_trie_node_t **child, /* child pointer if the child is modified */
               int child_count);

extern int _alpm_lib_trie_v6_delete(alpm_lib_trie_node_t *trie,
               uint32 *key,
               uint32 length,
               alpm_lib_trie_node_t **payload,
               alpm_lib_trie_node_t **child);

extern int _alpm_lib_trie_v6_split(alpm_lib_trie_node_t  *trie,
              uint32 *pivot,
              uint32 *length,
              uint32 *split_count,
              alpm_lib_trie_node_t **split_node,
              alpm_lib_trie_node_t **child,
              const uint32 max_count,
              const uint32 max_split_len,
              alpm_lib_trie_split_state_t *state,
              alpm_lib_trie_callback_ext_f cb,
              void *user_data,
              int max_split_count);

extern int _alpm_lib_trie_v6_merge(alpm_lib_trie_node_t *parent_trie,
                          alpm_lib_trie_node_t *child_trie,
                          uint32 *child_pivot,
                          uint32 length,
                          alpm_lib_trie_node_t **new_parent);

extern int _alpm_lib_trie_v6_split2(alpm_lib_trie_node_t *trie,
                           uint32 *key,
                           uint32 key_len,
                           uint32 *pivot,
                           uint32 *pivot_len,
                           uint32 *split_count,
                           alpm_lib_trie_node_t **split_node,
                           alpm_lib_trie_node_t **child,
                           alpm_lib_trie_split2_state_t *state,
                           const int max_split_count,
                           const int exact_same);

extern int _alpm_lib_trie_v6_ppg_prefix(alpm_lib_trie_node_t *pivot,
                    uint32 pivot_len,
                    uint32 *pfx,
                    uint32 len,
                    alpm_lib_trie_ppg_cb_f cb,
                    alpm_lib_trie_bpm_cb_info_t *cb_info);

/*=================================
 * Used by internal functions only
 *================================*/
#define _MAX_KEY_LEN_48_  (48)
#define _MAX_KEY_LEN_144_ (144)
/* key packing expetations:
 * eg., 48 bit key
 * - 10/8 -> key[0]=0, key[1]=8
 * - 0x123456789a -> key[0] = 0x12 key[1] = 0x3456789a
 * length - represents number of valid bits from farther to lower index ie., 1->0
 * eg., 144 bit key
 * - 0x10/8 -> key[0]=0, key[1]=0, key[2]=0, key[3]=0, key[0]=0x10
 * - 0x123456789a/48 -> key[0]=0, key[1]=0, key[2]=0, key[3] = 0x12 key[4] = 0x3456789a
 * length - represents number of valid bits from farther to lower index ie., 1->0
 */
#define KEY48_BIT2IDX(x) (((BITS2WORDS(_MAX_KEY_LEN_48_)*32) - (x))/32)
#define KEY144_BIT2IDX(x) (((BITS2WORDS(_MAX_KEY_LEN_144_)*32) - (x))/32)
#define KEY_BIT2IDX(maxkl, x) (((BITS2WORDS((maxkl))*32) - (x))/32)

#define _MAX_SKIP_LEN_  (31)

#define _SHL(data, shift)        ((data) << (shift))
#define _SHR(data, shift)        ((data) >> (shift))
#define _MASK(len)               ((1 << (len)) - 1)

#define TRIE_SHL(data, shift, max) \
    (((shift)>=(max))?0:(_SHL(data, shift)))

#define TRIE_SHR(data, shift, max) \
    (((shift)>=(max))?0:(_SHR(data, shift)))

#define TRIE_MASK(len) \
    (((len)>=32 || (len)==0) ? 0xFFFFFFFF :(_MASK(len)))

#define BITMASK(len) \
    (((len)>=32)?0xFFFFFFFF:((1<<(len))-1))

#define ABS(n) ((((int)(n)) < 0) ? -(n) : (n))

#define _NUM_WORD_BITS_ (32)

/*
 * bit 0                                  - 0
 * bit [1, _MAX_SKIP_LEN]                 - 1
 * bit [_MAX_SKIP_LEN+1, 2*_MAX_SKIP_LEN] - 2...
 */
#define BITS2SKIPOFF(x) (((x) + _MAX_SKIP_LEN_-1) / _MAX_SKIP_LEN_)

/* (internal) Generic operation macro on bit array _a, with bit _b */
#define _BITOP(_a, _b, _op) \
        ((_a) _op (1U << ((_b) % _NUM_WORD_BITS_)))

/* Specific operations */
#define _BITGET(_a, _b) _BITOP(_a, _b, &)
#define _BITSET(_a, _b) _BITOP(_a, _b, |=)
#define _BITCLR(_a, _b) _BITOP(_a, _b, &= ~)

extern int _alpm_lib_trie_fuse_child(alpm_lib_trie_node_t *trie, int bit);
extern int _alpm_lib_print_trie_node(alpm_lib_trie_node_t *trie, void *datum);
extern int _trie_traverse_ppg_prefix(alpm_lib_trie_node_t *trie, alpm_lib_trie_ppg_cb_f cb, alpm_lib_trie_bpm_cb_info_t *cb_info);

extern int _v6_key_append(uint32 *key, uint32 *key_len, uint32 skip_addr, uint32 skip_len);

#endif /* ALPM_ENABLE */

#endif /* _ALPM_LIB_TRIE_H_ */
