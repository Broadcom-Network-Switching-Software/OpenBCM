/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    trie_v6.c
 * Purpose: Custom Trie Data structure
 * Requires:
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
#include <soc/esw/sbDq.h>

#define _MAX_KEY_LEN_   (_MAX_KEY_LEN_144_)
#define _MAX_KEY_WORDS_ (BITS2WORDS(_MAX_KEY_LEN_))

/* key packing expetations:
 * eg., 144 bit key
 * - 0x10/8 -> key[0]=0, key[1]=0, key[2]=0, key[3]=0, key[0]=0x10
 * - 0x123456789a/48 -> key[0]=0, key[1]=0, key[2]=0, key[3] = 0x12 key[4] = 0x3456789a
 * length - represents number of valid bits from farther to lower index ie., 1->0 
 */
#define KEY_BIT2IDX(x) (((BITS2WORDS(_MAX_KEY_LEN_)*32) - (x))/32)


/********************************************************/
/* Get a chunk of bits from a key (MSB bit - on word0, lsb on word 1).. 
 */
static unsigned int _key_get_bits(unsigned int *key, 
				  unsigned int pos /* 1based, msb bit position */, 
				  unsigned int len,
				  unsigned int skip_len_check)
{
    /* coverity[var_deref_op : FALSE] */
    if (!key || (pos < len) || (pos > _MAX_KEY_LEN_) ||
	((skip_len_check == TRUE) && (len > _MAX_SKIP_LEN_))) {
	assert(0);
    }

    /* use Macro, convert to what's required by Macro */
    return _TAPS_GET_KEY_BITS(key, pos-len, len, _MAX_KEY_LEN_);
}		

/* 
 * Assumes the layout for 
 * 0 - most significant word
 * _MAX_KEY_WORDS_ - least significant word
 * eg., for key size of 48, word0-[bits 48-32] word1-[bits31-0]
 */
static int _key_append(unsigned int *key, 
		       unsigned int *length,
		       unsigned int skip_addr,
		       unsigned int skip_len)
{
    int rv=SOC_E_NONE;

    if (!key || !length || ((skip_len + *length) > _MAX_KEY_LEN_) ||
	(skip_len > _MAX_SKIP_LEN_) ) {
	return SOC_E_PARAM;
    }

    rv = taps_key_shift(_MAX_KEY_LEN_, key, *length, 0-(int)skip_len);
    if (SOC_SUCCESS(rv)) {
        key[KEY_BIT2IDX(1)] |= skip_addr;
        *length += skip_len;
    }

    return rv;
}

static int _bpm_append(unsigned int *key, 
		       unsigned int *length,
		       unsigned int skip_addr,
		       unsigned int skip_len)
{
    int rv=SOC_E_NONE;

    if (!key || !length || ((skip_len + *length) > _MAX_KEY_LEN_) ||
	(skip_len > (_MAX_SKIP_LEN_+1)) ) {
	return SOC_E_PARAM;
    }

    rv = taps_key_shift(_MAX_KEY_LEN_, key, *length, 0-(int)skip_len);
    if (SOC_SUCCESS(rv)) {
	key[KEY_BIT2IDX(1)] |= skip_addr;
	*length += skip_len;
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
static lcplen(unsigned int *key, unsigned int len1,
	      unsigned int skip_addr, unsigned int len2)
{
    unsigned int diff;
    unsigned int lcp = len1 < len2 ? len1 : len2;

    if ((len1 > _MAX_KEY_LEN_) || (len2 > _MAX_KEY_LEN_)) {
        LOG_CLI((BSL_META("len1 %d or len2 %d is larger than %d\n"),
                 len1, len2, _MAX_KEY_LEN_));
	assert(0);
    } 

    if ((len1 == 0) || (len2 == 0)) {
	return 0;
    }

    diff = _key_get_bits(key, len1, lcp, TRUE);
    diff ^= (SHR(skip_addr, len2 - lcp, _MAX_SKIP_LEN_) & MASK(lcp));

    while (diff) {
        diff >>= 1;
        --lcp;
    }

    return lcp;
}

int _trie_v6_search(trie_node_t *trie,
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
                   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;
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

                return _trie_v6_search(trie->child[bit].child_node, key, 
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
        }
        return SOC_E_NOT_FOUND; /* not found */
    }
}

/*
 * Internal function for LPM match searching.
 * callback on all payload nodes if cb != NULL.
 */
int _trie_v6_find_lpm(trie_node_t *trie,
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
               (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;

        /* based on next bit branch left or right */
        if (trie->child[bit].child_node) {
            return _trie_v6_find_lpm(trie->child[bit].child_node, key, length - lcp - 1,
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

/* trie->bpm format:
 * bit 0 is for the pivot itself (longest)
 * bit skip_len is for the trie branch leading to the pivot node (shortest)
 * bits (0-skip_len) is for the routes in the parent node's bucket
 */
int _trie_v6_find_bpm(trie_node_t *trie,
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
		   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;
	    
	    if (trie->child[bit].child_node) {
		rv = _trie_v6_find_bpm(trie->child[bit].child_node, key, length - lcp - 1, bpm_length);
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

int _trie_v6_bpm_mask_get(trie_node_t *trie,
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
                   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;

            if (trie->child[bit].child_node) {
                _bpm_append(bpm_mask, &scratch, trie->bpm, trie->skip_len + 1);
                rv = _trie_v6_bpm_mask_get(trie->child[bit].child_node, key, length - lcp - 1, bpm_mask);
                return rv;
            }
        }
    }

    _bpm_append(bpm_mask, &scratch, trie->bpm, trie->skip_len + 1);

    return rv;
}

/*
 * Function:
 *   _trie_v6_skip_node_free
 * Purpose:
 *   Destroy a chain of trie_node_t that has the target node at the end.
 *   The target node is not necessarily PAYLOAD type, but all nodes
 *   on the chain except for the end must have only one branch.
 * Input:
 *   key      --  target key
 *   length   --  target key length
 *   free_end --  free
 */
int _trie_v6_skip_node_free(trie_node_t *trie,
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
                rv = _trie_v6_skip_node_free(trie->child[bit].child_node, key,
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

            return SOC_E_NONE;
        }
        else return SOC_E_NOT_FOUND;
    } else {
        return SOC_E_NOT_FOUND; /* not found */
    }
}


/*
 * Function:
 *   _trie_v6_skip_node_alloc
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
int _trie_v6_skip_node_alloc(trie_node_t **node, 
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
		    skip_node->bpm = _key_get_bits(bpm, lsbpos, skip_node->skip_len, FALSE);
		} else {
		    skip_node->bpm = _key_get_bits(bpm, lsbpos+1, skip_node->skip_len+1, FALSE);
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
            skip_node->bpm =  _key_get_bits(bpm,1,1,TRUE);
        }
    }

    *node = skip_node;
    return SOC_E_NONE;
}

int _trie_v6_insert(trie_node_t *trie, 
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
        !payload || !child || (length > _MAX_KEY_LEN_)) return SOC_E_PARAM;

    *child = NULL;

    lcp = lcplen(key, length, trie->skip_addr, trie->skip_len);

    /* insert cases:
     * 1 - new key could be the parent of existing node
     * 2 - new node could become the child of a existing node
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
                    payload->bpm |= _key_get_bits(bpm, lcp+1, lcp+1, FALSE);
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
                payload->bpm |= _key_get_bits(bpm, payload->skip_len+1, payload->skip_len+1, FALSE);
            }
        }
    } else if (lcp == trie->skip_len) {
        /* key length is implictly greater than lcp here */
        /* decide based on key's next applicable bit */
        bit = (key[KEY_BIT2IDX(length-lcp)] & 
               (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1:0;

        if (!trie->child[bit].child_node) {
            /* the key is going to be one of the child of existing node */
            /* should be the child */
            rv = _trie_v6_skip_node_alloc(&node, key, bpm,
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
            rv = _trie_v6_insert(trie->child[bit].child_node, 
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

        rv = _trie_v6_skip_node_alloc(&newchild, key, bpm,
				      ((lcp)?length-lcp-1:length-1),
				      length - lcp - 1,
                      payload, child_count);
        if (SOC_SUCCESS(rv)) {
            bit = (key[KEY_BIT2IDX(length-lcp)] & 
                   (1 << ((length - lcp -1) % _NUM_WORD_BITS_))) ? 1: 0;

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

int _trie_v6_delete(trie_node_t *trie, 
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
                rv = _trie_v6_delete(trie->child[bit].child_node, key, length - lcp - 1, payload, child);

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

STATIC INLINE int
_trie_v6_splitable(trie_node_t *trie, trie_node_t *child, int max_count, int max_split_count)
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
int _trie_v6_split(trie_node_t  *trie,
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
           int max_split_count)
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
    		rv = taps_key_shift(_MAX_KEY_LEN_, bpm, _MAX_KEY_LEN_, trie->skip_len+1);
        }
		
		if (SOC_SUCCESS(rv)) {
		    rv = taps_key_shift(_MAX_KEY_LEN_, pivot, *length+trie->skip_len+1, trie->skip_len+1);
		}
		*state = TRIE_SPLIT_STATE_PRUNE_NODES;
		return rv;
	    }
	} else if ( ((*length == max_split_len) && (trie->count != max_count) && (trie->count <= max_split_count)) ||
                _trie_v6_splitable(trie, trie->child[bit].child_node, max_count, max_split_count)) {
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
	    
	    rv = _trie_v6_split(trie->child[bit].child_node, 
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
	    rv = taps_key_shift(_MAX_KEY_LEN_, pivot, *length, trie->skip_len+1);
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
 *     _trie_v6_merge
 * Purpose:
 *     merge or fuse the child trie with parent trie
 */
int
_trie_v6_merge(trie_node_t *parent_trie,
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
    _trie_v6_search(child_trie, pivot, length, &child, child_pivot, &child_length, 0, 1);
    if (child == NULL) {
        return SOC_E_PARAM;
    }

    _CLONE_TRIE_NODE_(&clone, child);

    if (child->type == PAYLOAD && child->bpm) {
        _TAPS_SET_KEY_BIT(bpm, 0, TAPS_IPV6_KEY_SIZE);
    }

    if (child != child_trie) {
        rv = _trie_v6_skip_node_free(child_trie, child_pivot, child_length);
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
    rv = _trie_v6_insert(parent_trie, child_pivot, bpm, child_length, child,
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
 *     trie_split
 * Purpose:
 *     Split the trie into 2 such that the new sub trie covers given prefix/length.
 * NOTE:
 *     key, key_len    -- The given prefix/length
 *     max_split_count -- The sub trie's max allowed count.
 */
int
_trie_v6_split2(trie_node_t *trie,
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

                rv = _trie_v6_split2(trie->child[bit].child_node,
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
 *     _trie_v6_propagate_prefix
 * Purpose:
 *  Propogate prefix BPM. If the propogation starts from intermediate pivot on
 *  the trie, then the prefix length has to be appropriately adjusted or else 
 *  it will end up with ill updates. 
 *  Assumption: the prefix length is adjusted as per trie node on which is starts from.
 *  If node == head node then adjust is none
 *     node == pivot, then prefix length = org len - pivot len          
 */
int _trie_v6_propagate_prefix(trie_node_t *trie,
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
                bit = _key_get_bits(pfx, len-lcp, 1, TRUE);
                if (!trie->child[bit].child_node) return SOC_E_NONE;
                rv = _trie_v6_propagate_prefix(trie->child[bit].child_node,
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
 *     _trie_v6_propagate_prefix_validate
 * Purpose:
 *  validate that the provided prefix is valid for propagation.
 *  The added prefix which was member of a shorter pivot's domain 
 *  must never be more specific than another pivot encounter if any
 *  in the path
 */
STATIC int _trie_v6_propagate_prefix_validate(trie_node_t *trie,
					      unsigned int *pfx,
					      unsigned int len)
{
    unsigned int lcp=0, bit=0;

    if (!trie || (len && trie->skip_len && !pfx)) return SOC_E_PARAM;

    if (len == 0) return SOC_E_NONE;

    lcp = lcplen(pfx, len, trie->skip_addr, trie->skip_len);

    if (lcp == trie->skip_len) {
        if (PAYLOAD == trie->type) {
	    return SOC_E_PARAM;
	}

	if (len == lcp) {
	    return SOC_E_NONE;
	}

        bit = _key_get_bits(pfx, len-lcp, 1, TRUE);
        if (!trie->child[bit].child_node) {
	    return SOC_E_NONE;
	}

        return _trie_v6_propagate_prefix_validate(trie->child[bit].child_node,
                                                  pfx, len-1-lcp);
    }

    return SOC_E_NONE;
}

int trie_v6_pivot_propagate_prefix(trie_node_t *pivot,
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
            rv = _trie_v6_propagate_prefix_validate(pivot->child[bit].child_node,
						 pfx, len-1);
            if (SOC_SUCCESS(rv)) {
                rv = _trie_v6_propagate_prefix(pivot->child[bit].child_node,
                                            pfx, len-1,
                                            add, cb, cb_info);
            }
        } /* else nop, nothing to propagate on this path end */
    } else {
        /* pivot == prefix */
        rv = _trie_v6_propagate_prefix(pivot, pfx, pivot->skip_len,
                                    add, cb, cb_info);
    }

    return rv;
}

/*
 * Function:
 *   _pvt_trie_v6_propagate_prefix
 * Purpose:
 *   If the propogation starts from intermediate pivot on
 *   the trie, then the prefix length has to be appropriately adjusted or else
 *   it will end up with ill updates.
 *   Assumption: the prefix length is adjusted as per trie node on
 *             which is starts from.
 *   If node == head node then adjust is none
 *      node == pivot, then prefix length = org len - pivot len
 */
int _pvt_trie_v6_propagate_prefix(trie_node_t *trie,
                                  unsigned int *pfx,
                                  unsigned int len,
                                  trie_propagate_cb_f cb,
                                  trie_bpm_cb_info_t *cb_info)
{
    int rv = SOC_E_NONE;
    unsigned int bit=0, lcp=0;

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
                bit = _key_get_bits(pfx, len-lcp, 1, TRUE);
                if (!trie->child[bit].child_node) {
                    return SOC_E_NONE;
                }
                rv = _pvt_trie_v6_propagate_prefix(trie->child[bit].child_node,
                            pfx, len-lcp-1, cb, cb_info);
            } else {
                /* pfx is <= trie skip len */
                rv = _pvt_trie_traverse_propagate_prefix(trie, cb, cb_info);
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
 *      pvt_trie_v6_propagate_prefix
 * Purpose:
 *      Propogate prefix from a given pivot.
 *      Callback function to decide INSERT/DELETE propagation,
 *               and decide to update bpm_len or not.
 */
int pvt_trie_v6_propagate_prefix(trie_node_t *pivot,
                    unsigned int pivot_len,
                    unsigned int *pfx,
                    unsigned int len,
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

    len -= pivot_len;

    if (len > 0) {
        unsigned int bit =  _key_get_bits(pfx, len, 1, 0);

        if (pivot->child[bit].child_node) {
            /* validate if the pivot provided is correct */
            rv = _trie_v6_propagate_prefix_validate(
                    pivot->child[bit].child_node, pfx, len-1);
            if (SOC_SUCCESS(rv)) {
                rv = _pvt_trie_v6_propagate_prefix(
                        pivot->child[bit].child_node,
                        pfx, len-1, cb, cb_info);
            }
        } /* else nop, nothing to propagate on this path end */
    } else {
        /* pivot == prefix */
        rv = _pvt_trie_v6_propagate_prefix(
                pivot, pfx, pivot->skip_len, cb, cb_info);
    }

    return rv;
}

/****************/
/** unit tests **/
/****************/
#define _NUM_KEY_ (4 * 1024)
#define _VRF_LEN_ 16
/*#define VERBOSE 
  #define LOG*/
/* use the followign diag shell command to run this test:
 * tr c3sw test=tmu_trie_v6_ut
 */
typedef struct _v6_payload_s {
    trie_node_t node; /*trie node */
    dq_t        listnode; /* list node */
    union {
        trie_t      *trie;
        trie_node_t pfx_trie_node;
    } info;
    unsigned int key[BITS2WORDS(_MAX_KEY_LEN_)];
    unsigned int len;
} v6_payload_t;

STATIC int ut_print_payload_node(trie_node_t *payload, void *datum)
{
    v6_payload_t *pyld;

    if (payload && payload->type == PAYLOAD) {
        pyld = TRIE_ELEMENT_GET(v6_payload_t*, payload, node);
        LOG_CLI((BSL_META(" key[0x%08x:0x%08x] Length:%d \n"),
                 pyld->key[0], pyld->key[1], pyld->len));
    }
    return SOC_E_NONE;
}

STATIC int ut_print_prefix_payload_node(trie_node_t *payload, void *datum)
{
    v6_payload_t *pyld;

    if (payload && payload->type == PAYLOAD) {
        pyld = TRIE_ELEMENT_GET(v6_payload_t*, payload, info.pfx_trie_node);
        taps_show_prefix(_MAX_KEY_LEN_, pyld->key, pyld->len);
    }
    return SOC_E_NONE;
}

STATIC int ut_check_duplicate(v6_payload_t *pyld, int pyld_vector_size)
{
    int i=0;

    assert(pyld);

    for (i=0; i < pyld_vector_size; i++) {
        if (pyld[i].len == pyld[pyld_vector_size].len &&
            pyld[i].key[0] == pyld[pyld_vector_size].key[0] && 
            pyld[i].key[1] == pyld[pyld_vector_size].key[1] && 
            pyld[i].key[2] == pyld[pyld_vector_size].key[2] && 
            pyld[i].key[3] == pyld[pyld_vector_size].key[3] && 
            pyld[i].key[4] == pyld[pyld_vector_size].key[4]) {
            break;
        }
    }

    return ((i == pyld_vector_size)?0:1);
}

int tmu_trie_v6_split_ut(unsigned int seed) 
{
    int index, rv = SOC_E_NONE, numkey=0, id=0;
    trie_t *trie, *newtrie;
    trie_node_t *newroot;
    v6_payload_t *pyld = sal_alloc(_NUM_KEY_ * sizeof(v6_payload_t), "unit-test");
    trie_node_t *pyldptr = NULL;
    unsigned int pivot[_MAX_KEY_WORDS_], length;

    sal_memset(pyld, 0, _NUM_KEY_ * sizeof(v6_payload_t));
    for (id=0; id < 4; id++) {
        switch(id) {
	    case 0:  /* 1:1 split */
		pyld[0].key[3] = 0; pyld[0].key[4] = 0x10; pyld[0].len = _VRF_LEN_ + 8;  /* v=0 p=0x10000000/8  */
		pyld[1].key[3] = 0; pyld[1].key[4] = 0x1000; pyld[1].len = _VRF_LEN_ + 16; /* v=0 p=0x10000000/16 */
		pyld[2].key[3] = 0; pyld[2].key[4] = 0x100000; pyld[2].len = _VRF_LEN_ + 24; /* v=0 p=0x10000000/24 */
		pyld[3].key[3] = 0; pyld[3].key[4] = 0x10000000; pyld[3].len = _VRF_LEN_ + 32; /* v=0 p=0x10000000/48 */
		numkey = 4;
		break;
	    case 1: /* 1:1 split */
		pyld[0].key[3] = 0; pyld[0].key[4] = 0x10000000; pyld[0].len = _VRF_LEN_ + 32; /* v=0 p=0x10000000/32 */
		pyld[1].key[3] = 0; pyld[1].key[4] = 0x10000001; pyld[1].len = _VRF_LEN_ + 32; /* v=0 p=0x10000001/32 */
		pyld[2].key[3] = 0; pyld[2].key[4] = 0x10000002; pyld[2].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
		pyld[3].key[3] = 0; pyld[3].key[4] = 0x10000003; pyld[3].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
		pyld[4].key[3] = 0; pyld[4].key[4] = 0x10000004; pyld[4].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
		pyld[5].key[3] = 0; pyld[5].key[4] = 0x10000005; pyld[5].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
		numkey = 6;
		break;
	    case 2: /* 2:5 split */
		pyld[0].key[3] = 0; pyld[0].key[4] = 0x100; pyld[0].len = _VRF_LEN_ + 12;
		pyld[1].key[3] = 0; pyld[1].key[4] = 0x1011; pyld[1].len = _VRF_LEN_ + 16;
		pyld[2].key[3] = 0; pyld[2].key[4] = 0x100000; pyld[2].len = _VRF_LEN_ + 24; 
		pyld[3].key[3] = 0; pyld[3].key[4] = 0x1000000; pyld[3].len = _VRF_LEN_ + 28;
		pyld[4].key[3] = 0; pyld[4].key[4] = 0x1001; pyld[4].len = _VRF_LEN_ + 16;
		pyld[5].key[3] = 0; pyld[5].key[4] = 0x10011; pyld[5].len = _VRF_LEN_ + 20;
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
			pyld[index].len = (unsigned int)sal_rand() % 128;
			pyld[index].len += _VRF_LEN_;
			
			pyld[index].key[0] = 0;
			pyld[index].key[1] = 0;
			pyld[index].key[2] = 0;
			pyld[index].key[3] = 0;
			if (pyld[index].len > 128) {
			    pyld[index].key[0] &= MASK(pyld[index].len-128);
			    pyld[index].key[1] = (unsigned int) sal_rand();
			    pyld[index].key[2] = (unsigned int) sal_rand();
			    pyld[index].key[3] = (unsigned int) sal_rand();
			    pyld[index].key[4] = (unsigned int) sal_rand();
			} else if (pyld[index].len > 96) {
			    pyld[index].key[1] &= MASK(pyld[index].len-96);
			    pyld[index].key[2] = (unsigned int) sal_rand();
			    pyld[index].key[3] = (unsigned int) sal_rand();
			    pyld[index].key[4] = (unsigned int) sal_rand();
			} else if (pyld[index].len > 64) {
			    pyld[index].key[2] &= MASK(pyld[index].len-64);
			    pyld[index].key[3] = (unsigned int) sal_rand();
			    pyld[index].key[4] = (unsigned int) sal_rand();
			} else if (pyld[index].len > 32) {
			    pyld[index].key[3] &= MASK(pyld[index].len-32);
			    pyld[index].key[4] = (unsigned int) sal_rand();
			} else {
			    pyld[index].key[4] &= MASK(pyld[index].len);
			}
			
			dup = ut_check_duplicate(pyld, index);
			if (dup) {                    
			    LOG_CLI((BSL_META("\n Duplicate at index[%d]:"
                                              "key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x] "
                                              "Retry!!!\n"), 
                                     index, pyld[index].key[0],
                                     pyld[index].key[1], pyld[index].key[2],
                                     pyld[index].key[3], pyld[index].key[4]));
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

        rv = trie_split(trie, _MAX_KEY_LEN_, FALSE, pivot, &length, &newroot, NULL, FALSE, 1024);
        if (SOC_SUCCESS(rv)) {
            LOG_CLI((BSL_META("\n Split Trie Pivot: 0x%08x 0x%08x "
                              "Length: %d Root: %p \n"),
                     pivot[0], pivot[1], length, newroot));
            LOG_CLI((BSL_META(" $Payload Count Old Trie:%d New Trie:%d \n"),
                     trie->trie->count, newroot->count));

            /* set new trie */
            newtrie->trie = newroot;
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
                        LOG_CLI((BSL_META("SEARCH: Key=0x%x 0x%x 0x%x 0x%x  0x%x "
                                          "len %d SEARCH idx:%d failed on "
                                          "both trie!!!!\n"), 
                                 pyld[index].key[0], pyld[index].key[1],
                                 pyld[index].key[2], pyld[index].key[3],
                                 pyld[index].key[4], pyld[index].len, index));
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

int tmu_taps_trie_v6_ut(int id, unsigned int seed)
{
    int index, rv = SOC_E_NONE, numkey=0, num_deleted=0;
    trie_t *trie;
    v6_payload_t *pyld = sal_alloc(_NUM_KEY_ * sizeof(v6_payload_t), "unit-test");
    trie_node_t *pyldptr = NULL;
    unsigned int result_len=0, result_key[_MAX_KEY_WORDS_];

    /* keys packed right to left (ie) most significant word starts at index 0*/
    sal_memset(pyld, 0, _NUM_KEY_ * sizeof(v6_payload_t));
    switch(id) {
    case 0:
        pyld[0].key[3] = 0; pyld[0].key[4] = 0x10; pyld[0].len = _VRF_LEN_ + 8;  /* v=0 p=0x10000000/8  */
        pyld[1].key[3] = 0; pyld[1].key[4] = 0x1000; pyld[1].len = _VRF_LEN_ + 16; /* v=0 p=0x10000000/16 */
        pyld[2].key[3] = 0; pyld[2].key[4] = 0x100000; pyld[2].len = _VRF_LEN_ + 24; /* v=0 p=0x10000000/24 */
        pyld[3].key[3] = 0; pyld[3].key[4] = 0x10000000; pyld[3].len = _VRF_LEN_ + 32; /* v=0 p=0x10000000/48 */
        numkey = 4;
        break;

    case 1:
        pyld[0].key[3] = 0; pyld[0].key[4] = 0x123456; pyld[0].len  = _VRF_LEN_ + 24; /* v=0 p=0x12345678/24 */
        pyld[1].key[3] = 0; pyld[1].key[4] = 0x246; pyld[1].len = _VRF_LEN_ + 13; /* v=0 p=0x12345678/13 */
        pyld[2].key[3] = 0; pyld[2].key[4] = 0x24; pyld[2].len = _VRF_LEN_ + 9; /* v=0 p=0x12345678/9 */
        numkey = 3;
        break;

    case 2: /* dup routes on another vrf */
        pyld[0].key[3] = 0; pyld[0].key[4] = 0x1123456; pyld[0].len = _VRF_LEN_ + 24; /* v=1 p=0x12345678/24 */
        pyld[1].key[3] = 0; pyld[1].key[4] = 0x2246; pyld[1].len = _VRF_LEN_ + 13; /* v=1 p=0x12345678/13 */
        pyld[2].key[3] = 0; pyld[2].key[4] = 0x224; pyld[2].len = _VRF_LEN_ + 9; /* v=1 p=0x12345678/9 */
        numkey = 3;
        break;

    case 3:
        pyld[0].key[3] = 0; pyld[0].key[4] = 0x10000000; pyld[0].len = _VRF_LEN_ + 32; /* v=0 p=0x10000000/32 */
        pyld[1].key[3] = 0; pyld[1].key[4] = 0x10000001; pyld[1].len = _VRF_LEN_ + 32; /* v=0 p=0x10000001/32 */
        pyld[2].key[3] = 0; pyld[2].key[4] = 0x10000002; pyld[2].len = _VRF_LEN_ + 32; /* v=0 p=0x10000002/32 */
        numkey = 3;
        break;

    case 4:
        pyld[0].key[3] = 0; pyld[0].key[4] = 0x12345670; pyld[0].len = _VRF_LEN_ + 32; /* v=0 p=0x12345670/32 */
        pyld[1].key[3] = 0; pyld[1].key[4] = 0x12345671; pyld[1].len = _VRF_LEN_ + 32; /* v=0 p=0x12345671/32 */
        pyld[2].key[3] = 0; pyld[2].key[4] = 0x91a2b38;  pyld[2].len = _VRF_LEN_ + 31; /* v=0 p=0x12345670/31 */
        numkey = 3;
        break;

    case 5:
        pyld[0].key[3] = 0; pyld[0].key[4] = 0x20; pyld[0].len = _VRF_LEN_ + 8; /* v=0 p=0x20000000/8 */
        pyld[1].key[3] = 0; pyld[1].key[4] = 0x8000; pyld[1].len = _VRF_LEN_ + 16; /* v=0 p=0x80000000/16 */
        pyld[2].key[3] = 0; pyld[2].key[4] = 0; pyld[2].len = _VRF_LEN_ + 0; /* v=0 p=0/0 */
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
		    pyld[index].len = (unsigned int)sal_rand() % 128;
		    pyld[index].len += _VRF_LEN_;
		    
		    pyld[index].key[0] = 0;
		    pyld[index].key[1] = 0;
		    pyld[index].key[2] = 0;
		    pyld[index].key[3] = 0;
		    if (pyld[index].len > 128) {
			pyld[index].key[0] &= MASK(pyld[index].len-128);
			pyld[index].key[1] = (unsigned int) sal_rand();
			pyld[index].key[2] = (unsigned int) sal_rand();
			pyld[index].key[3] = (unsigned int) sal_rand();
			pyld[index].key[4] = (unsigned int) sal_rand();
		    } else if (pyld[index].len > 96) {
			pyld[index].key[1] &= MASK(pyld[index].len-96);
			pyld[index].key[2] = (unsigned int) sal_rand();
			pyld[index].key[3] = (unsigned int) sal_rand();
			pyld[index].key[4] = (unsigned int) sal_rand();
		    } else if (pyld[index].len > 64) {
			pyld[index].key[2] &= MASK(pyld[index].len-64);
			pyld[index].key[3] = (unsigned int) sal_rand();
			pyld[index].key[4] = (unsigned int) sal_rand();
		    } else if (pyld[index].len > 32) {
			pyld[index].key[3] &= MASK(pyld[index].len-32);
			pyld[index].key[4] = (unsigned int) sal_rand();
		    } else {
			pyld[index].key[4] &= MASK(pyld[index].len);
		    }
		    
		    dup = ut_check_duplicate(pyld, index);
		    if (dup) {                    
			LOG_CLI((BSL_META("\n Duplicate at index[%d]:"
                                          "key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x] Retry!!!\n"), 
                                 index, pyld[index].key[0],
                                 pyld[index].key[1], pyld[index].key[2],
                                 pyld[index].key[3], pyld[index].key[4]));
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
        vrf = _key_get_bits(pyld[index].key, pyld[index].len, _VRF_LEN_, FALSE);

#ifdef LOG
        LOG_CLI((BSL_META("+ Inserted Key=0x%x 0x%x 0x%x 0x%x 0x%x 0x%x "
                          "vpn=0x%x Len=%d idx:%d\n"), 
                 pyld[index].key[0], pyld[index].key[1],
                 pyld[index].key[2], pyld[index].key[3],
                 pyld[index].key[4], vrf, pyld[index].len, index));
#endif
        rv = trie_insert(trie, &pyld[index].key[0], NULL, pyld[index].len, &pyld[index].node);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META("FAILED to Insert Key=0x%x 0x%x 0x%x 0x%x 0x%x "
                              "vpn=0x%x Len=%d idx:%d\n"), 
                     pyld[index].key[0], pyld[index].key[1],
                     pyld[index].key[2], pyld[index].key[3],
                     pyld[index].key[4], vrf, pyld[index].len, index));
        }
#define _VERBOSE_SEARCH_
        /* search all keys & figure out breakage right away */
        for (i=0; i <= index && rv == SOC_E_NONE; i++) {
#ifdef _VERBOSE_SEARCH_
            result_key[0] = 0;
            result_key[1] = 0;
            result_key[2] = 0;
            result_key[3] = 0;
            result_key[4] = 0;
            result_len    = 0;
            rv = trie_search_verbose(trie, &pyld[index].key[0], pyld[index].len, 
                                     &pyldptr, &result_key[0], &result_len);
#else
            rv = trie_search(trie, &pyld[index].key[0], pyld[index].len, &pyldptr);
#endif
            if (rv != SOC_E_NONE) {
                LOG_CLI((BSL_META("SEARCH: Key=0x%x 0x%x 0x%x 0x%x 0x%x "
                                  "len %d SEARCH idx:%d failed!!!!\n"), 
                         pyld[index].key[0], pyld[index].key[1],
                         pyld[index].key[2], pyld[index].key[3],
                         pyld[index].key[4], pyld[index].len, index));
                break;
            } else {
                assert(pyldptr == &pyld[index].node);
#ifdef _VERBOSE_SEARCH_
                if (pyld[index].key[0] != result_key[0] ||
                    pyld[index].key[1] != result_key[1] ||
                    pyld[index].key[2] != result_key[2] ||
                    pyld[index].key[3] != result_key[3] ||
                    pyld[index].key[4] != result_key[4] ||
                    pyld[index].len != result_len) {
                    LOG_CLI((BSL_META(" Found key mismatches with the "
                                      "expected Key !!!! \n")));
                    rv = SOC_E_FAIL;
                }
#ifdef VERBOSE
                LOG_CLI((BSL_META("Lkup[%d] key/len: 0x%x 0x%x 0x%x 0x%x 0x%x/%d "
                                  "Found Key/len: 0x%x 0x%x 0x%x 0x%x 0x%x/%d \n"),
                         index, pyld[index].key[0],
                         pyld[index].key[1], pyld[index].key[2],
                         pyld[index].key[3], pyld[index].key[4],
                         pyld[index].len, result_key[0],
                         result_key[1], result_key[2], result_key[3],
                         result_key[4], result_len));
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
                    LOG_CLI((BSL_META("Deleted Key=0x%x 0x%x 0x%x 0x%x 0x%x "
                                      "Len=%d idx:%d Num-Key:%d\n"), 
                             pyld[index].key[0], pyld[index].key[1],
                             pyld[index].key[2], pyld[index].key[3],
                             pyld[index].key[4], pyld[index].len,
                             index, num_deleted));
#endif
                    pyld[index].len = 0xFFFFFFFF;
                    num_deleted++;

                    /* search all keys & figure out breakage right away */
                    for (index=0; index < numkey; index++) {
                        if (pyld[index].len == 0xFFFFFFFF) continue;

                        rv = trie_search(trie, &pyld[index].key[0], pyld[index].len, &pyldptr);
                        if (rv != SOC_E_NONE) {
                            LOG_CLI((BSL_META("ALL SEARCH after delete: "
                                              "Key= 0x%x 0x%x 0x%x 0x%x 0x%x "
                                              "len %d SEARCH idx:%d failed!!!!\n"), 
                                     pyld[index].key[0], pyld[index].key[1],
                                     pyld[index].key[2], pyld[index].key[3],
                                     pyld[index].key[4],
                                     pyld[index].len, index));
                            break;
                        } else {
                            assert(pyldptr == &pyld[index].node);
                        }
                    }
                } else {
                    LOG_CLI((BSL_META("Deleted Key=0x%x 0x%x 0x%x 0x%x 0x%x "
                                      "Len=%d idx:%d FAILED!!!\n"), 
                             pyld[index].key[0], pyld[index].key[1],
                             pyld[index].key[2], pyld[index].key[3],
                             pyld[index].key[4], 
                             pyld[index].len, index));
                    break;
                }
            } else {
                LOG_CLI((BSL_META("SEARCH: Key=0x%x 0x%x 0x%x 0x%x 0x%x "
                                  "len %d SEARCH idx:%d failed!!!!\n"), 
                         pyld[index].key[0], pyld[index].key[1],
                         pyld[index].key[2], pyld[index].key[3],
                         pyld[index].key[4],
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
    v6_payload_t *pfx; 
    trie_t *pfx_trie;
} expect_datum_t;

STATIC int ut_bpm_build_expect_list(trie_node_t *payload, void *user_data)
{
    int rv=SOC_E_NONE;

    if (payload && payload->type == PAYLOAD) {
        trie_node_t *pyldptr;
        v6_payload_t *pivot;
        expect_datum_t *datum = (expect_datum_t*)user_data;

        pivot = TRIE_ELEMENT_GET(v6_payload_t*, payload, node);
        /* if the inserted prefix is a best prefix, add the pivot to expected list */
        rv = trie_find_lpm(datum->pfx_trie, &pivot->key[0], pivot->len, &pyldptr); 
        assert(rv == SOC_E_NONE);
        if (pyldptr == &datum->pfx->info.pfx_trie_node) {
            /* if pivot is not equal to prefix add to expect list */
            if (!(pivot->key[0] == datum->pfx->key[0] && 
                  pivot->key[1] == datum->pfx->key[1] &&
                  pivot->key[2] == datum->pfx->key[2] &&
                  pivot->key[3] == datum->pfx->key[3] &&
                  pivot->key[4] == datum->pfx->key[4] &&
                  pivot->len    == datum->pfx->len)) {
                DQ_INSERT_HEAD(&datum->list, &pivot->listnode);
            }
        }
    }

    return SOC_E_NONE;
}

STATIC int ut_bpm_propagate_cb(trie_node_t *payload, trie_bpm_cb_info_t *cbinfo)
{
    if (payload && cbinfo && payload->type == PAYLOAD) {
        v6_payload_t *pivot;
        dq_p_t elem;
        expect_datum_t *datum = (expect_datum_t*)cbinfo->user_data;

        pivot = TRIE_ELEMENT_GET(v6_payload_t*, payload, node);
        DQ_TRAVERSE(&datum->list, elem) {
            v6_payload_t *velem = DQ_ELEMENT_GET(v6_payload_t*, elem, listnode); 
            if (velem == pivot) {
                DQ_REMOVE(&pivot->listnode);
                break;
            }
        } DQ_TRAVERSE_END(&datum->list, elem);
    }

    return SOC_E_NONE;
}

STATIC int ut_bpm_propagate_empty_cb(trie_node_t *payload, trie_bpm_cb_info_t *cbinfo)
{
    /* do nothing */
    return SOC_E_NONE;
}

STATIC void ut_bpm_dump_expect_list(expect_datum_t *datum, char *str)
{
    dq_p_t elem;
    if (datum) {
        /* dump expected list */
        LOG_CLI((BSL_META("%s \n"), str));
        DQ_TRAVERSE(&datum->list, elem) {
            v6_payload_t *velem = DQ_ELEMENT_GET(v6_payload_t*, elem, listnode); 
            LOG_CLI((BSL_META(" Pivot: 0x%x 0x%x 0x%x 0x%x 0x%x Len: %d \n"), 
                     velem->key[0], velem->key[1], velem->key[2],
                     velem->key[3], 
                     velem->key[4], velem->len));
        } DQ_TRAVERSE_END(&datum->list, elem);
    }
}

#define _MAX_TEST_PIVOTS_ (10)
#define _MAX_BKT_PFX_ (20)
#define _MAX_NUM_PICK (30)

int tmu_taps_bpm_trie_v6_ut(int id, unsigned int seed)
{
    int rv = SOC_E_NONE, pivot=0, pfx=0, index=0, dup=0, domain=0;
    trie_t *pfx_trie, *trie;
    v6_payload_t *pyld = sal_alloc(_MAX_BKT_PFX_ * _MAX_TEST_PIVOTS_ * sizeof(v6_payload_t), "bpm-unit-test");
    v6_payload_t *pivot_pyld = sal_alloc(_MAX_TEST_PIVOTS_ * sizeof(v6_payload_t), "bpm-unit-test");
    trie_node_t *pyldptr = NULL, *newroot;
    unsigned int bpm[BITS2WORDS(_MAX_KEY_LEN_)];
    expect_datum_t datum;
    trie_bpm_cb_info_t cbinfo;
    int num_pick, bpm_pfx_len;

    sal_memset(pyld, 0, _MAX_BKT_PFX_ * _MAX_TEST_PIVOTS_ * sizeof(v6_payload_t));
    sal_memset(pivot_pyld, 0, _MAX_TEST_PIVOTS_ * sizeof(v6_payload_t));

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
    pivot_pyld[pivot].key[0] = 0;
    pivot_pyld[pivot].key[1] = 0;
    pivot_pyld[pivot].key[2] = 0;
    pivot_pyld[pivot].key[3] = 0;
    pivot_pyld[pivot].key[4] = 0;
    pivot_pyld[pivot].len    = 0;
    trie_init(_MAX_KEY_LEN_, &pivot_pyld[pivot].info.trie);
    sal_memset(&bpm[0], 0,  BITS2WORDS(_MAX_KEY_LEN_) * sizeof(unsigned int));
            
    do {
        rv = trie_insert(trie, &pivot_pyld[pivot].key[3], &bpm[0], 
                         pivot_pyld[pivot].len, &pivot_pyld[pivot].node);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META("FAILED to Insert PIVOT "
                              "Key=0x%x 0x%x 0x%x 0x%x 0x%x Len=%d idx:%d\n"), 
                     pivot_pyld[pivot].key[0], pivot_pyld[pivot].key[1],
                     pivot_pyld[pivot].key[2], pivot_pyld[pivot].key[3],
                     pivot_pyld[pivot].key[4], 
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
		    pyld[index].len = (unsigned int)sal_rand() % 128;
		    pyld[index].len += _VRF_LEN_;
		    
		    pyld[index].key[0] = 0;
		    pyld[index].key[1] = 0;
		    pyld[index].key[2] = 0;
		    pyld[index].key[3] = 0;
		    if (pyld[index].len > 128) {
			pyld[index].key[0] &= MASK(pyld[index].len-128);
			pyld[index].key[1] = (unsigned int) sal_rand();
			pyld[index].key[2] = (unsigned int) sal_rand();
			pyld[index].key[3] = (unsigned int) sal_rand();
			pyld[index].key[4] = (unsigned int) sal_rand();
		    } else if (pyld[index].len > 96) {
			pyld[index].key[1] &= MASK(pyld[index].len-96);
			pyld[index].key[2] = (unsigned int) sal_rand();
			pyld[index].key[3] = (unsigned int) sal_rand();
			pyld[index].key[4] = (unsigned int) sal_rand();
		    } else if (pyld[index].len > 64) {
			pyld[index].key[2] &= MASK(pyld[index].len-64);
			pyld[index].key[3] = (unsigned int) sal_rand();
			pyld[index].key[4] = (unsigned int) sal_rand();
		    } else if (pyld[index].len > 32) {
			pyld[index].key[3] &= MASK(pyld[index].len-32);
			pyld[index].key[4] = (unsigned int) sal_rand();
		    } else {
			pyld[index].key[4] &= MASK(pyld[index].len);
		    }
		    
                    dup = ut_check_duplicate(pyld, pfx+index);
                    if (!dup) {
                        rv = trie_find_lpm(trie, &pyld[pfx+index].key[0], pyld[pfx+index].len, &pyldptr); 
                        if (SOC_FAILURE(rv)) {
                            LOG_CLI((BSL_META("\n !! Failed to find LPM pivot "
                                              "for index[%d]:"
                                              "key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x]"
                                              "!!!!\n"),
                                     pfx, pyld[pfx+index].key[0],
                                     pyld[pfx+index].key[1],
                                     pyld[pfx+index].key[2],
                                     pyld[pfx+index].key[3],
                                     pyld[pfx+index].key[4]));
                        } 
                    }
                } while ((dup || (pyldptr != &pivot_pyld[domain].node)) && SOC_SUCCESS(rv));

                if (SOC_SUCCESS(rv)) {
                    rv =  trie_insert(pivot_pyld[domain].info.trie,
				      &pyld[pfx+index].key[0], NULL, 
				      pyld[pfx+index].len, &pyld[pfx+index].node);
                    if (SOC_FAILURE(rv)) {
                        LOG_CLI((BSL_META("\n !! Failed insert prefix into pivot trie"
                                          " index[%d]:"
                                          "key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x] "
                                          "!!!!\n"),
                                 pfx+index, pyld[pfx+index].key[0],
                                 pyld[pfx+index].key[1],
                                 pyld[pfx+index].key[2],
                                 pyld[pfx+index].key[3],
                                 pyld[pfx+index].key[4]));
                    } else {
                        rv =  trie_insert(pfx_trie,
					  &pyld[pfx+index].key[0], NULL, 
					  pyld[pfx+index].len, &pyld[pfx+index].info.pfx_trie_node);     
                        if (SOC_FAILURE(rv)) {
                            LOG_CLI((BSL_META("\n !! Failed insert prefix into "
                                              "prefix trie index[%d]:"
                                              "key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x]"
                                              " !!!!\n"),
                                     pfx+index, pyld[pfx+index].key[0],
                                     pyld[pfx+index].key[1],
                                     pyld[pfx+index].key[2],
                                     pyld[pfx+index].key[3],
                                     pyld[pfx+index].key[4]));
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
                LOG_CLI((BSL_META("\n --- TRIE domain dump: Pivot: "
                                  "0x%x 0x%x 0x%x 0x%x 0x%x len=%d ----- \n"),
                         pivot_pyld[i].key[0], pivot_pyld[i].key[1],
                         pivot_pyld[i].key[2],
                         pivot_pyld[i].key[3], pivot_pyld[i].key[4],
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
                                &bpm[0], FALSE, 1024);
                if (SOC_SUCCESS(rv)) {
                    pivot_pyld[pivot].info.trie->trie = newroot;
                    LOG_CLI((BSL_META("BPM for split pivot: 0x%x 0x%x 0x%x 0x%x 0x%x "
                                      "/ %d = [0x%x 0x%x 0x%x 0x%x 0x%x] \n"),
                             pivot_pyld[pivot].key[0],
                             pivot_pyld[pivot].key[1],
                             pivot_pyld[pivot].key[2],
                             pivot_pyld[pivot].key[3],
                             pivot_pyld[pivot].key[4],
                             pivot_pyld[pivot].len,
                             bpm[0], bpm[1], bpm[2], bpm[3], bpm[4]));
                } else {
                    LOG_CLI((BSL_META("\n !!! Failed to split domain trie "
                                      "for domain: %d !!!\n"), domain));
                }
            }
        }
    } while(pivot < _MAX_TEST_PIVOTS_ && SOC_SUCCESS(rv));

    /* pick up the root node on pivot trie & add a prefix shorter than the nearest child.
     * This is ripple & create huge propagation */
    /* insert *\/1 into the * bucket so huge propagation kicks in */
    pyld[pfx].key[4] = (unsigned int) sal_rand() % 1;
    pyld[pfx].key[3] = 0;
    pyld[pfx].key[2] = 0;
    pyld[pfx].key[1] = 0;
    pyld[pfx].key[0] = 0;
    pyld[pfx].len    = 1;
    do {
        dup = ut_check_duplicate(pyld, pfx);
        if (!dup) {
            rv = trie_find_lpm(trie, &pyld[pfx].key[0], pyld[pfx].len, &pyldptr); 
            if (SOC_FAILURE(rv)) {
                LOG_CLI((BSL_META("\n !! Failed to find LPM pivot for index[%d]:"
                                  "key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x] !!!!\n"),
                         pfx, pyld[pfx].key[0], pyld[pfx].key[1],
                         pyld[pfx].key[2],pyld[pfx].key[3],pyld[pfx].key[4]));
            } 
        } else {
            pyld[pfx].len++;
        }
    } while(dup && SOC_SUCCESS(rv));

    if (SOC_SUCCESS(rv)) {
        rv =  trie_insert(pfx_trie, &pyld[pfx].key[0], NULL, 
			  pyld[pfx].len, &pyld[pfx].info.pfx_trie_node);
        if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META("\n !! Failed insert prefix into pivot trie"
                              " index[%d]:key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x] "
                              "!!!!\n"),
                     pfx, pyld[pfx].key[0], pyld[pfx].key[1],
                     pyld[pfx].key[2], pyld[pfx].key[3], pyld[pfx].key[4]));
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
    rv = trie_v6_pivot_propagate_prefix(pyldptr,
					(TRIE_ELEMENT_GET(v6_payload_t*, pyldptr, node))->len,
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
	sal_memcpy(pyld[pfx].key, pivot_pyld[index].key, _MAX_KEY_WORDS_*sizeof(uint32));
	rv = taps_key_shift(_MAX_KEY_LEN_, pyld[pfx].key, pivot_pyld[index].len,
			    (pivot_pyld[index].len-pyld[pfx].len));

	if (pyld[pfx].len >= 1) {
	    /* propagate add len=0 */
	    rv = trie_v6_pivot_propagate_prefix(trie->trie,
				       (TRIE_ELEMENT_GET(v6_payload_t*, trie->trie, node))->len,
				       &pyld[pfx].key[0], 0,
				       1, ut_bpm_propagate_empty_cb, 
				       &cbinfo);

	    if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META("!!!!! BPM search Test FAILED to propagate "
                              "add len=0!!!!!\n")));
		assert(0);
	    }

	    /* propagate add */
	    rv = trie_v6_pivot_propagate_prefix(trie->trie,
				       (TRIE_ELEMENT_GET(v6_payload_t*, trie->trie, node))->len,
				       &pyld[pfx].key[0], pyld[pfx].len,
				       1, ut_bpm_propagate_empty_cb, 
				       &cbinfo);
	    if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META("!!!!! BPM search Test FAILED to propagate add \n"
                              " index[%d]:key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x] "
                              "len=%d!!!!\n"),
                     pfx, pyld[pfx].key[0], pyld[pfx].key[1],
                     pyld[pfx].key[2], pyld[pfx].key[3],
                     pyld[pfx].key[4], pyld[pfx].len));
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
	    rv = trie_v6_pivot_propagate_prefix(trie->trie,
				       (TRIE_ELEMENT_GET(v6_payload_t*, trie->trie, node))->len,
				       &pyld[pfx].key[0], pyld[pfx].len,
				       0, ut_bpm_propagate_empty_cb, 
				       &cbinfo);
	    
	    if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META("!!!!! BPM search Test FAILED to propagate add \n"
                              " index[%d]:key[0x%08x:0x%08x:0x%08x:0x%08x:0x%08x] "
                              "len=%d!!!!\n"),
                     pfx, pyld[pfx].key[0], pyld[pfx].key[1],
                     pyld[pfx].key[2], pyld[pfx].key[3],
                     pyld[pfx].key[4], pyld[pfx].len));
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

    LOG_CLI((BSL_META("\n ----- Prefix Trie dump ----- \n")));
    trie_dump(pfx_trie, ut_print_prefix_payload_node, NULL);

    LOG_CLI((BSL_META("\n ++++++++ Trie dump ++++++++ \n")));
    trie_dump(trie, ut_print_payload_node, NULL);

    /* clean up */
    for (index=0; index < pivot; index++) {
        LOG_CLI((BSL_META("\n ddddddd dump dddddddd \n")));
        trie_dump(pivot_pyld[index].info.trie, ut_print_payload_node, NULL);
        trie_destroy(pivot_pyld[index].info.trie);
    }

    sal_free(pyld);
    sal_free(pivot_pyld);
    trie_destroy(trie);
    trie_destroy(pfx_trie);
    return rv;
}

/**********************************************/

#endif /* BCM_TRIDENT2_SUPPORT */
#endif /* ALPM_ENABLE */
