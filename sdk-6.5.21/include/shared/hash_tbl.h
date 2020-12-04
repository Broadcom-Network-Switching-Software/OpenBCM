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

#ifndef _HASH_TBL_H_
#define _HASH_TBL_H_

#include <sal/types.h>
#include <sal/core/sync.h>

typedef void*  shr_htb_key_t;
typedef void*  shr_htb_data_t;

typedef uint32 (*shr_htb_hash_f)(uint8* key_bytes, uint32 length);
typedef void (*shr_htb_cast_key_f)(shr_htb_key_t key,
                                   uint8  **key_bytes, 
                                   uint32  *key_size);
typedef int (*shr_htb_key_cmp_f)(shr_htb_key_t a,
                                 shr_htb_key_t b,
                                 uint32 size);
typedef void (*shr_htb_data_free_f)(shr_htb_data_t data);

typedef struct _hash_entry_s {
    struct _hash_entry_s   *next;
    shr_htb_data_t         *data;  /* caller managed memory */
    shr_htb_key_t           key;   /* NOTE: Variable size field. Struct
                                    * is extented to caller supplied key_size 
                                    */
} _hash_entry_t;

typedef struct hash_table_s {
    int                  max_num_entries;
    int                  key_size;       /* size of variable length key 
                                          * in shr_htb_key_t */
    int                  alloc_blk_cnt;  /* number of entries to allocate 
                                          * when the free list runs dry */
    int                  num_free;
    _hash_entry_t       *free_list;
    _hash_entry_t      **table;
    sal_mutex_t          lock;

    shr_htb_hash_f       hash_f;
    shr_htb_key_cmp_f    key_cmp_f;
    shr_htb_cast_key_f   cast_key_f;

} hash_table_t;

typedef hash_table_t* shr_htb_hash_table_t;

int
shr_htb_create(shr_htb_hash_table_t *ht, int max_num_entries, int key_size,
               char* tbl_name);

int
shr_htb_destroy(shr_htb_hash_table_t *ht, shr_htb_data_free_f cb);

int
shr_htb_find(shr_htb_hash_table_t ht, shr_htb_key_t key, shr_htb_data_t *data,
             int remove);

int
shr_htb_insert(shr_htb_hash_table_t ht, shr_htb_key_t key, shr_htb_data_t data);


typedef int (*shr_htb_cb_t)(int unit, shr_htb_key_t key, shr_htb_data_t data);

int
shr_htb_iterate(int unit, shr_htb_hash_table_t ht, shr_htb_cb_t restore_cb);


/* Configuration routines */
void
shr_htb_cast_key_func_set(shr_htb_hash_table_t ht, 
                          shr_htb_cast_key_f func);
void 
shr_htb_hash_func_set(shr_htb_hash_table_t ht, 
                      shr_htb_hash_f func);

void 
shr_htb_key_cmp_func_set(shr_htb_hash_table_t ht,
                         shr_htb_key_cmp_f func);



#endif /* _HASH_TBL_H_ */

