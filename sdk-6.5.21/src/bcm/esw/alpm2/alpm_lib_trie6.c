/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    trie_v6.c
 * Purpose: Custom Trie Data structure
 * Requires:
 */

#ifdef ALPM_ENABLE

#include <bcm_int/esw/alpm_lib_trie.h>

extern void * alpm_util_alloc(unsigned int sz, char *s);
extern void alpm_util_free(void *addr);

/*
 *
 * Function:
 *     taps_key_shift
 * Input:
 *     max_key_size  -- max number of bits in the key
 *                      ipv4 == 48
 *                      ipv4 == 144
 *     key   -- uint32 array head. Only "length" number of bits
 *              is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length-- number of valid bits in key array. This would be
 *              valid MSB bits of the route. For example,
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 *     shift -- positive means right shift, negative means left shift
 *              routine will check if the shifted key is out of
 *              max_key_size boundary.
 */
static int taps_key_shift(uint32 *key, uint32 length, int32 shift)
{
    int wi, lsb;

    if (shift > 0) {
        /* right shift */
        for (lsb = shift, wi=BITS2WORDS(_MAX_KEY_LEN_144_)-1; wi >=0;
             lsb+=32, wi--) {
            if (lsb < length) {
                key[wi] = _TAPS_GET_KEY_BITS(key, lsb, ((length-lsb)>=32)?32:(length-lsb));
            } else {
                key[wi] = 0;
            }
        }
    } else if (shift < 0) {
        /* left shift */
        shift = 0 - shift;

        /* whole words shifting first */
        for (wi = 0; ((shift/32)!=0) && (wi < BITS2WORDS(_MAX_KEY_LEN_144_)); wi++) {
            key[wi] = ((wi + (shift/32)) >= BITS2WORDS(_MAX_KEY_LEN_144_)) ? 0 : key[wi + (shift/32)];
        }

        /* shifting remaining bits */
        for (wi = 0; ((shift%32)!=0) && (wi < BITS2WORDS(_MAX_KEY_LEN_144_)); wi++) {
            if (wi == TP_BITS2IDX(0)) {
                /* at bit 0 word, next word doesn't exist */
                key[wi] = _SHL(key[wi], (shift%_NUM_WORD_BITS_));
            } else {
                key[wi] = _SHL(key[wi], (shift%_NUM_WORD_BITS_)) | \
                    TRIE_SHR(key[wi+1], _NUM_WORD_BITS_-(shift%_NUM_WORD_BITS_), _NUM_WORD_BITS_);
            }
        }

        /* mask off bits higher than max_key_size */
        key[0] &= BITMASK(_MAX_KEY_LEN_144_%32);
    }

    return SOC_E_NONE;
}


/********************************************************/
/* Get a chunk of bits from a key (MSB bit - on word0, lsb on word 1)..
 */
static uint32 _alpm_lib_key_get_bits(uint32 *key,
                  uint32 pos /* 1based, msb bit position */,
                  uint32 len)
{
    /* use Macro, convert to what's required by Macro */
    return _TAPS_GET_KEY_BITS(key, pos-len, len);
}

/*
 * Assumes the layout for
 * 0 - most significant word
 * MAX_KEY_WORDS - least significant word
 * eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
int _v6_key_append(uint32 *key,
               uint32 *length,
               uint32 skip_addr,
               uint32 skip_len)
{
    int rv=SOC_E_NONE;

    rv = taps_key_shift(key, *length, 0-(int)skip_len);
    if (SOC_SUCCESS(rv)) {
        key[KEY144_BIT2IDX(1)] |= skip_addr;
        *length += skip_len;
    }

    return rv;
}

/*
 * Function:
 *     lcplen6
 * Purpose:
 *     returns longest common prefix length provided a key & skip address
 */
uint32
lcplen6(uint32 *key, uint32 len1, uint32 skip_addr, uint32 len2)
{
    uint32 diff;
    uint32 lcp = len1 < len2 ? len1 : len2;

    if ((len1 == 0) || (len2 == 0)) {
        return 0;
    }

    diff = _alpm_lib_key_get_bits(key, len1, lcp);
    diff ^= (TRIE_SHR(skip_addr, len2 - lcp, _MAX_SKIP_LEN_) & TRIE_MASK(lcp));

    while (diff) {
        diff >>= 1;
        --lcp;
    }

    return lcp;
}

int _alpm_lib_trie_v6_search(alpm_lib_trie_node_t *trie,
            uint32 *key,
            uint32 length,
            alpm_lib_trie_node_t **payload,
            uint32 *result_key,
            uint32 *result_len,
            uint32 dump,
            uint32 find_pivot)
{
    uint32 lcp=0;
    int bit=0, rv=SOC_E_NONE;

    lcp = lcplen6(key, length, trie->skip_addr, trie->skip_len);

#if 0
    if (dump) {
        _alpm_lib_print_trie_node(trie, (uint32 *)1);
    }
#endif

    if (length > trie->skip_len) {
        if (lcp == trie->skip_len) {
            bit = (key[KEY144_BIT2IDX(length - lcp)] & \
                   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;
#if 0
            if (dump) {
                cli_out(" Length: %d Next-Bit[%d] \n", length, bit);
            }
#endif

            if (result_key) {
                rv = _v6_key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
                if (SOC_FAILURE(rv)) return rv;
            }

            /* based on next bit branch left or right */
            if (trie->child[bit]) {
                if (result_key) {
                    rv = _v6_key_append(result_key, result_len, bit, 1);
                    if (SOC_FAILURE(rv)) return rv;
                }

                return _alpm_lib_trie_v6_search(trie->child[bit], key,
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
#if 0
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
                rv = _v6_key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
                if (SOC_FAILURE(rv)) return rv;
            }
            return SOC_E_NONE;
        }
        else return SOC_E_NOT_FOUND;
    } else {
        if (lcp == length && find_pivot) {
            *payload = trie;
            if (result_key) {
                rv = _v6_key_append(result_key, result_len, trie->skip_addr, trie->skip_len);
                if (SOC_FAILURE(rv)) return rv;
            }
        }
        return SOC_E_NOT_FOUND; /* not found */
    }
}

/*
 * Internal function for LPM match searching.
 * callback on all payload nodes if cb != NULL.
 */
int _alpm_lib_trie_v6_find_lpm(alpm_lib_trie_node_t *trie,
              uint32 *key,
              uint32 length,
              alpm_lib_trie_node_t **payload,
              uint32 exclude_self)
{
    uint32 lcp=0;
    int bit=0, rv=SOC_E_NONE;

    lcp = lcplen6(key, length, trie->skip_addr, trie->skip_len);

    if ((length > trie->skip_len) && (lcp == trie->skip_len)) {
        if (trie->type == trieNodeTypePayload) {
        /* lpm cases */
        if (payload != NULL) {
        /* update lpm result */
        *payload = trie;
        }
    }

        bit = (key[KEY144_BIT2IDX(length - lcp)] & \
               (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;

        /* based on next bit branch left or right */
        if (trie->child[bit]) {
            return _alpm_lib_trie_v6_find_lpm(trie->child[bit], key, length - lcp - 1,
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
 *   _alpm_lib_trie_v6_skip_node_alloc
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
int _alpm_lib_trie_v6_skip_node_alloc(alpm_lib_trie_node_t **node,
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
                skip_node->skip_addr = key[KEY144_BIT2IDX(msbpos+1)] & TRIE_MASK((msbpos+1) % _NUM_WORD_BITS_);
                skip_node->skip_addr = _SHL(skip_node->skip_addr, skip_node->skip_len - ((msbpos+1) % _NUM_WORD_BITS_));
                skip_node->skip_addr |= _SHR(key[KEY144_BIT2IDX(lsbpos+1)],(lsbpos % _NUM_WORD_BITS_));
            } else {
                skip_node->skip_addr = _SHR(key[KEY144_BIT2IDX(msbpos+1)], (lsbpos % _NUM_WORD_BITS_));
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

int _alpm_lib_trie_v6_insert(alpm_lib_trie_node_t *trie, uint32 *key, uint32 length,
            alpm_lib_trie_node_t *payload, /* payload node */
            alpm_lib_trie_node_t **child, /* child pointer if the child is modified */
            int child_count)
{
    uint32 lcp;
    int rv=SOC_E_NONE, bit=0;
    alpm_lib_trie_node_t *node = NULL;

    *child = NULL;

    lcp = lcplen6(key, length, trie->skip_addr, trie->skip_len);

    /* insert cases:
     * 1 - new key could be the parent of existing node
     * 2 - new node could become the child of a existing node
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

            bit = (trie->skip_addr & _SHL(1,trie->skip_len - length - 1)) ? 1 : 0;
            trie->skip_addr &= TRIE_MASK(trie->skip_len - length - 1);
            trie->skip_len  -= (length + 1);

            payload->skip_addr = (length > 0) ? key[KEY144_BIT2IDX(length)] : 0;
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
        bit = (key[KEY144_BIT2IDX(length-lcp)] &
               (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;

        if (!trie->child[bit]) {
            /* the key is going to be one of the child of existing node */
            /* should be the child */
            rv = _alpm_lib_trie_v6_skip_node_alloc(&node, key,
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
            rv = _alpm_lib_trie_v6_insert(trie->child[bit],
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

        rv = _alpm_lib_trie_v6_skip_node_alloc(&newchild, key,
                      ((lcp)?length-lcp-1:length-1),
                      length - lcp - 1,
                      payload, child_count);
        if (SOC_SUCCESS(rv)) {
            bit = (key[KEY144_BIT2IDX(length-lcp)] &
                   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1: 0;

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

int
_alpm_lib_trie_v6_delete(alpm_lib_trie_node_t *trie, uint32 *key, uint32 length, alpm_lib_trie_node_t **payload, alpm_lib_trie_node_t **child)
{
    uint32 lcp;
    int rv=SOC_E_NONE, bit=0;
    alpm_lib_trie_node_t *node = NULL;

    *child = NULL;

    /* check a section of key, return the number of matched bits and value of next bit */
    lcp = lcplen6(key, length, trie->skip_addr, trie->skip_len);

    if (length > trie->skip_len) {

        if (lcp == trie->skip_len) {

            bit = (key[KEY144_BIT2IDX(length-lcp)] &
                   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;

            /* based on next bit branch left or right */
            if (trie->child[bit]) {
                /* has child node, keep searching */
                rv = _alpm_lib_trie_v6_delete(trie->child[bit], key, length - lcp - 1, payload, child);

                if (rv == SOC_E_BUSY) {
                    trie->child[bit] = NULL; /* alpm_util_free the child */
                    rv = SOC_E_NONE;
                    trie->count--;

                    if (trie->type == trieNodeTypeInternal) {

                        bit = (bit==0)?1:0;

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

STATIC INLINE int
_alpm_lib_trie_v6_splitable(alpm_lib_trie_node_t *trie, alpm_lib_trie_node_t *child,
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
 *     trie_v6_split
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
int _alpm_lib_trie_v6_split(alpm_lib_trie_node_t  *trie,
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
           int max_split_count)
{
    int bit=0, rv=SOC_E_NONE;

    if (trie->child[0] && trie->child[1]) {
        bit = (trie->child[0]->count >
               trie->child[1]->count) ? 0:1;
    } else {
        bit = (trie->child[0])?0:1;
    }

    /* start building the pivot */
    rv = _v6_key_append(pivot, length, trie->skip_addr, trie->skip_len);
    if (SOC_FAILURE(rv)) return rv;

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
                rv = taps_key_shift(pivot, *length+trie->skip_len+1, trie->skip_len+1);
            }
            *state = trieSplitStatePruneNodes;
            return rv;
        }
    } else if ( ((*length == max_split_len) && (trie->count != max_count) && (trie->count <= max_split_count)) ||
                _alpm_lib_trie_v6_splitable(trie, trie->child[bit], cb, user_data, max_count, max_split_count)) {
        /*
         * (1) when the node is at the max_split_len and if used as spliting point
         * the resulted trie will not have all pivots (FULL). we should split
         * at this node.
         * (2) when the node is at the max_split_len and if the resulted trie
         * will have all pivots (FULL), we fall through to keep searching
         * (3) when the node is shorter than the max_split_len and the node
             * has a more even pivot distribution compare to it's cc, we
             * can split at this node. The split count must be less than or
             * equal to max_split_count.
             * (4) when the node's count is only 1, we must split at this point.
             *
             * NOTE:
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
        rv = _v6_key_append(pivot, length, bit, 1);
        if (SOC_FAILURE(rv)) return rv;

        rv = _alpm_lib_trie_v6_split(trie->child[bit],
                 pivot, length,
                 split_count, split_node,
                 child, max_count, max_split_len,
                 state, cb, user_data, max_split_count);
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
            rv = taps_key_shift(pivot, *length, trie->skip_len+1);
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
 *     _alpm_lib_trie_v6_merge
 * Purpose:
 *     merge or fuse the child trie with parent trie
 */
int
_alpm_lib_trie_v6_merge(alpm_lib_trie_node_t *parent_trie,
               alpm_lib_trie_node_t *child_trie,
               uint32 *pivot,
               uint32 length,
               alpm_lib_trie_node_t **new_parent)
{
    int rv, child_count;
    alpm_lib_trie_node_t *child = NULL, clone;
    uint32 child_pivot[BITS2WORDS(_MAX_KEY_LEN_144_)] = {0};
    uint32 child_length = 0;

    /*
     * to do merge, there is one and only one condition:
     * parent must cover the child
     */

    /*
     * child pivot could be an internal node, i.e., NOT_FOUND on search
     * so check the out child instead of rv.
     */
    _alpm_lib_trie_v6_search(child_trie, pivot, length, &child, child_pivot, &child_length, 0, 1);
    if (child == NULL) {
        return SOC_E_PARAM;
    }

    _TRIE_NODE_CLONE_(&clone, child);

    if (child != child_trie) {
        rv = _alpm_lib_trie_skip_node_free(child_trie, _MAX_KEY_LEN_144_, child_pivot, child_length);
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
    /* merge happens on bucket trie */
    rv = _alpm_lib_trie_v6_insert(parent_trie, child_pivot, child_length, child,
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
 *     _alpm_lib_trie_v6_split2
 * Purpose:
 *     Split the trie into 2 such that the new sub trie covers given prefix/length.
 * NOTE:
 *     key, key_len    -- The given prefix/length
 *     max_split_count -- The sub trie's max allowed count.
 */
int
_alpm_lib_trie_v6_split2(alpm_lib_trie_node_t *trie,
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
    rv = _v6_key_append(pivot, pivot_len, trie->skip_addr, trie->skip_len);
    if (SOC_FAILURE(rv)) return rv;


    lcp = lcplen6(key, key_len, trie->skip_addr, trie->skip_len);

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
            bit = (key[KEY144_BIT2IDX(key_len - lcp)] & \
                    (1 << ((key_len - lcp - 1) % _NUM_WORD_BITS_))) ? 1:0;

            /* based on next bit branch left or right */
            if (trie->child[bit]) {
                /* we can not split at this node, keep searching, it's better to
                 * split at longer pivot
                 */
                rv = _v6_key_append(pivot, pivot_len, bit, 1);
                if (SOC_FAILURE(rv)) return rv;

                rv = _alpm_lib_trie_v6_split2(trie->child[bit],
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
 *     _alpm_lib_trie_v6_ppg_prefix_validate
 * Purpose:
 *  validate that the provided prefix is valid for propagation.
 *  The added prefix which was member of a shorter pivot's domain
 *  must never be more specific than another pivot encounter if any
 *  in the path
 */
STATIC int _alpm_lib_trie_v6_ppg_prefix_validate(alpm_lib_trie_node_t *trie,
                          uint32 *pfx,
                          uint32 len)
{
    uint32 lcp=0, bit=0;

    if (len == 0) return SOC_E_NONE;

    lcp = lcplen6(pfx, len, trie->skip_addr, trie->skip_len);

    if (lcp == trie->skip_len) {
        if (trieNodeTypePayload == trie->type) {
            return SOC_E_PARAM;
        }

        if (len == lcp) {
            return SOC_E_NONE;
        }

        bit = _alpm_lib_key_get_bits(pfx, len-lcp, 1);
        if (!trie->child[bit]) {
            return SOC_E_NONE;
        }

        return _alpm_lib_trie_v6_ppg_prefix_validate(trie->child[bit],
                                                  pfx, len-1-lcp);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *   _alpm_lib_trie_v6_ppg_prefix_walk
 * Purpose:
 *   If the propogation starts from intermediate pivot on
 *   the trie, then the prefix length has to be appropriately adjusted or else
 *   it will end up with ill updates.
 *   Assumption: the prefix length is adjusted as per trie node on
 *             which is starts from.
 *   If node == head node then adjust is none
 *      node == pivot, then prefix length = org len - pivot len
 */
int _alpm_lib_trie_v6_ppg_prefix_walk(alpm_lib_trie_node_t *trie,
                      uint32 *pfx,
                      uint32 len,
                      alpm_lib_trie_ppg_cb_f cb,
                      alpm_lib_trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE;
    uint32 bit=0, lcp=0;

    if (!trie || (len && trie->skip_len && !pfx) ||
        (len > _MAX_KEY_LEN_144_) || !cb || !cb_info) {
        return SOC_E_PARAM;
    }

    if (len > 0) {
        lcp = lcplen6(pfx, len, trie->skip_addr, trie->skip_len);
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
                rv = _alpm_lib_trie_v6_ppg_prefix_walk(trie->child[bit],
                            pfx, len-lcp-1, cb, cb_info);
            } else {
                /* pfx is <= trie skip len */
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
 *      _alpm_lib_trie_v6_ppg_prefix
 * Purpose:
 *      Propogate prefix from a given pivot.
 *      Callback function to decide INSERT/DELETE propagation,
 *               and decide to update bpm_len or not.
 */
int _alpm_lib_trie_v6_ppg_prefix(alpm_lib_trie_node_t *pivot,
                    uint32 pivot_len,
                    uint32 *pfx,
                    uint32 len,
                    alpm_lib_trie_ppg_cb_f cb,
                    alpm_lib_trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE;

    len -= pivot_len;

    if (len > 0) {
        uint32 bit =  _alpm_lib_key_get_bits(pfx, len, 1);

        if (pivot->child[bit]) {
            /* validate if the pivot provided is correct */
            rv = _alpm_lib_trie_v6_ppg_prefix_validate(
                    pivot->child[bit], pfx, len-1);
            if (SOC_SUCCESS(rv)) {
                rv = _alpm_lib_trie_v6_ppg_prefix_walk(
                        pivot->child[bit],
                        pfx, len-1, cb, cb_info);
            }
        } /* else nop, nothing to propagate on this path end */
    } else {
        /* pivot == prefix */
        rv = _alpm_lib_trie_v6_ppg_prefix_walk(
                pivot, pfx, pivot->skip_len, cb, cb_info);
    }

    return rv;
}

#else
typedef int bcm_esw_alpm2_alpm_lib_trie6_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
