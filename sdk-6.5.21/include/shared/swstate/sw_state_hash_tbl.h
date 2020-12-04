/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	shr_hash.h
 * Purpose: 	Defines a generic hash table for key/value pairs.
 *
 * Overview:
 * Provides a generic hash table interface with configurable and default hash
 * and key compare functions.  The interface allows for complex key types, but
 * the caller must implement a _cast_ method to transform the complex type into
 * a string of bytes for the hash function.  Alternatively, the caller may
 * replace the hash function itself, in which case, the default _cast_ method
 * may be used to return the complex key and complex key size. 
 *
 * Memory is allocated on-demand in blocks for hash entries, and freed in 
 * blocks when the free pool becomes large.
 *
 * Collisions are handled simply by creating a linked list per hash index and
 * a linear search is performed within the list to find an entry.  The list is
 * not sorted.  (future upgrade?)
 *
 * The caller defined KEY is *copied* and stored in the variable sized hash 
 * table entry for comparison.  A *pointer* to the caller defined DATA is 
 * stored.  The caller is reponsible for managing the memory where DATA points
 * if any.  Callbacks are provided upon hash destruction to free any allocated
 * memory.  The hash table module itself does not explitly free any DATA
 * pointer at any time.
 */

#ifndef _SW_STATE_HASH_TBL_H_
#define _SW_STATE_HASH_TBL_H_

#include <sal/types.h>
#include <sal/core/sync.h>
#include <shared/swstate/sw_state.h>

typedef uint8*  sw_state_htb_key_t;
typedef uint8*  sw_state_htb_data_t;

typedef uint32 (*sw_state_htb_hash_f)(uint8* key_bytes, uint32 length);
typedef void (*sw_state_htb_cast_key_f)(sw_state_htb_key_t key,
                                   uint8  **key_bytes, 
                                   uint32  *key_size);
typedef int (*sw_state_htb_key_cmp_f)(sw_state_htb_key_t a,
                                 sw_state_htb_key_t b,
                                 uint32 size);
typedef void (*sw_state_htb_data_free_f)(int unit, sw_state_htb_data_t data);

typedef struct ss_hash_entries_s {
    PARSER_HINT_ARR  int                    *next;
    PARSER_HINT_ARR  uint8                  *datas;  /* data0/data1/data2/..../datan */
    PARSER_HINT_ARR  uint8                  *keys;   /* key0/key1/key2/..../keyn */
} ss_hash_entries_t;

typedef struct sw_state_hash_table_s {
                          int                  max_num_entries;
                          int                  key_size;       /* size of variable length key 
                                                                * in sw_state_htb_key_t */
                          int                  data_size;
                          int                  num_free;
    PARSER_HINT_ARR       int                 *free_arr;
                          ss_hash_entries_t    entry_arrays /*entry_list*/;
    PARSER_HINT_ARR       int                 *table;

} sw_state_hash_table_t;

/*
 * The following definition replaces:
 *   typedef sw_state_hash_table_t *sw_state_htb_hash_table_t;
 * So that the new sw state could use a handle rather than a pointer.
 */
typedef int sw_state_htb_hash_table_t ;

typedef struct sw_state_hash_table_db_s {
    PARSER_HINT_ARR_PTR   sw_state_hash_table_t    **hash_table;
                          int                        nof_htbl_used;
                          int                        max_nof_htbls;
} sw_state_hash_table_db_t;



int
sw_state_htb_create(int unit, int *ht_indx, int max_num_entries, int key_size, int data_size,
               char* tbl_name);

int
sw_state_htb_destroy(int unit, int ht_indx, sw_state_htb_data_free_f cb);

int
sw_state_htb_find(int unit, int ht_indx, sw_state_htb_key_t key, sw_state_htb_data_t data,
             int remove);

int
sw_state_htb_insert(int unit, int ht_indx, sw_state_htb_key_t key, sw_state_htb_data_t data);


typedef int (*sw_state_htb_cb_t)(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data);

int
sw_state_htb_iterate(int unit, int ht_indx, sw_state_htb_cb_t restore_cb);

int
sw_state_htb_init(int unit, int max_nof_htbls);

int
sw_state_htb_deinit(int unit);

#endif /* _SW_STATE_HASH_TBL_H_ */

