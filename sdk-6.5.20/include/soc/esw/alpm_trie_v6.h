/* 
 * $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 * trie IPv6 data structure
 */

#ifndef _ESW_TRIDENT2_ALPM_TRIE_V6_H_
#define _ESW_TRIDENT2_ALPM_TRIE_V6_H_

#ifdef ALPM_ENABLE
#ifndef ALPM_IPV6_128_SUPPORT
extern int _trie_v6_search(trie_node_t *trie,
			   unsigned int *key,
			   unsigned int length,
			   trie_node_t **payload,
			   unsigned int *result_key,
			   unsigned int *result_len,
               unsigned int dump,
               unsigned int find_pivot);

extern int _trie_v6_find_lpm(trie_node_t *trie,
			     unsigned int *key,
			     unsigned int length,
			     trie_node_t **payload,
			     trie_callback_f cb,
			     void *user_data,
			     unsigned int exclude_self);

extern int _trie_v6_find_bpm(trie_node_t *trie,
			     unsigned int *key,
			     unsigned int length,
			     int *bpm_length);

extern int _trie_v6_bpm_mask_get(trie_node_t *trie,
                unsigned int *key,
                unsigned int length,
                unsigned int *bpm_mask);

extern int _trie_v6_skip_node_free(trie_node_t *trie,
                            unsigned int *key,
                            unsigned int length);

extern int _trie_v6_skip_node_alloc(trie_node_t **node,
				    unsigned int *key,
				    /* bpm bit map if bpm management is required, passing null skips bpm management */
				    unsigned int *bpm, 
				    unsigned int msb, /* NOTE: valid msb position 1 based, 0 means skip0/0 node */
				    unsigned int skip_len,
				    trie_node_t *payload,
				    unsigned int count);

extern int _trie_v6_insert(trie_node_t *trie, 
			   unsigned int *key, 
			   /* bpm bit map if bpm management is required, passing null skips bpm management */
			   unsigned int *bpm, 
			   unsigned int length,
			   trie_node_t *payload, /* payload node */
               trie_node_t **child, /* child pointer if the child is modified */
               int child_count);

extern int _trie_v6_delete(trie_node_t *trie, 
			   unsigned int *key,
			   unsigned int length,
			   trie_node_t **payload,
			   trie_node_t **child);

extern int _trie_v6_split(trie_node_t  *trie,
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
			  int max_split_count);

extern int _trie_v6_merge(trie_node_t *parent_trie,
                          trie_node_t *child_trie,
                          unsigned int *child_pivot,
                          unsigned int length,
                          trie_node_t **new_parent);

extern int _trie_v6_split2(trie_node_t *trie,
                           unsigned int *key,
                           unsigned int key_len,
                           unsigned int *pivot,
                           unsigned int *pivot_len,
                           unsigned int *split_count,
                           trie_node_t **split_node,
                           trie_node_t **child,
                           trie_split2_states_e_t *state,
                           const int max_split_count,
                           const int exact_same);

extern int _trie_v6_propagate_prefix(trie_node_t *trie,
				     unsigned int *pfx,
				     unsigned int len,
				     unsigned int add, /* 0-del/1-add */
				     trie_propagate_cb_f cb,
				     trie_bpm_cb_info_t *cb_info);

extern int trie_v6_pivot_propagate_prefix(trie_node_t *pivot,
					  unsigned int pivot_len,
					  unsigned int *pfx,
					  unsigned int len,
					  unsigned int add, /* 0-del/1-add */
					  trie_propagate_cb_f cb,
					  trie_bpm_cb_info_t *cb_info);

extern int pvt_trie_v6_propagate_prefix(trie_node_t *pivot,
                    unsigned int pivot_len,
                    unsigned int *pfx,
                    unsigned int len,
                    trie_propagate_cb_f cb,
                    trie_bpm_cb_info_t *cb_info);

extern int tmu_trie_v6_split_ut(unsigned int seed);

extern int tmu_taps_trie_v6_ut(int id, unsigned int seed);

extern int tmu_taps_bpm_trie_v6_ut(int id, unsigned int seed);

#endif /* ALPM_IPV6_128_SUPPORT */
#endif /* ALPM_ENABLE */
#endif /* _ESW_TRIDENT2_ALPM_TRIE_V6_H_ */
