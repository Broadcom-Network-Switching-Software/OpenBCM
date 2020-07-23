/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	hash_tbl.c
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

#include <shared/bsl.h>

#include <shared/hash_tbl.h>
#include <shared/error.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/util.h>
#include <sal/core/sync.h>

#include <soc/cm.h>

/* allocate this many blocks of hash entries when the free pool is dry */
#define HTB_DEFAULT_ALLOC_BLK_CNT   (16)

#define HTBL_LOCK(p) \
    sal_mutex_take((p)->lock, sal_mutex_FOREVER)

#define HTBL_UNLOCK(p) \
    sal_mutex_give((p)->lock)


#if 0
void _dbg_dump_list_recr(_hash_entry_t *entry)
{
    if (entry == NULL) {
        LOG_CLI((BSL_META("(NULL)\n")));
    }
    else {
        LOG_CLI((BSL_META("(0x%08x)->"), entry));
        _dbg_dump_list_recr(entry->next);
    }
}

void
_dbg_dump_list(shr_htb_hash_table_t ht, int hash_idx)
{  
    _hash_entry_t *entry = ht->table[hash_idx];
    
    LOG_CLI((BSL_META("h_tbl[%4d]="), hash_idx));
    _dbg_dump_list_recr(entry);
}
#endif


/*
 * Function:
 *   htb_default_hash_f
 * Purpose:
 *   Generic default hash function.
 * Parameters:
 *   (in) k           - key as a string of bytes 
 *   (in) length      - Length of key in bytes
 * Returns:
 *   Hash index for the given key.
 */
uint32
htb_default_hash_f(uint8 *k, uint32 length)
{
    return _shr_crc32(~0, k, length);
}

/*
 * Function:
 *   htb_default_key_cmp_f
 * Purpose:
 *   Generic default key compare function; bit for bit compare of 
 *   two keys of size key_size set at table creation time.
 * Parameters:
 *   (in) a    - Key A.
 *   (in) b    - Key B
 *   (in) size - Size of key for this hash table. 
 * Returns:
 *   0         if a==b;
 *   non-zero  if a!=b
 * Notes:
 */
int
htb_default_key_cmp_f(shr_htb_key_t a,
                      shr_htb_key_t b,
                      uint32 size)
{
    return sal_memcmp(a, b, size);
}


/*
 * Function:
 *  htb_default_cast_key_f
 * Purpose:
 *  Default routine for casting the caller defined key to a string of bytes
 * Parameters:
 *  (in)  key        - key to cast
 *  (out) key_bytes  - string of bytes representing key
 *  (out) key_size   - length of key_bytes
 * Returns:
 *   none
 * Notes:
 *   The default cast is a no-op
 */
void
htb_default_cast_key_f(shr_htb_key_t key, uint8 **key_bytes, uint32 *key_size)
{
    /* do nothing */
}

/*
 * Function:
 *  _htb_free_list_push
 * Purpose:
 *  Internal routine; add an hash entry to the free list
 * Parameters:
 *  (in) ht - hash table to operate
 *  (in) entry  - entry to add to the list
 * Returns:
 *   none
 * Notes:
 */
STATIC void
_htb_free_list_push(shr_htb_hash_table_t ht, _hash_entry_t *entry)
{
    ht->num_free++;   
    entry->next = ht->free_list;
    ht->free_list = entry;
}


/*
 * Function:
 *   _htb_free_list_pop
 * Purpose:
 *   Internal support routine;  grab an entry from the free list. 
 * Parameters:
 *   (in) ht - Hash table to operate
 * Returns:
 *   allocated hash entry
 * Notes:
 */
STATIC _hash_entry_t *
_htb_free_list_pop(shr_htb_hash_table_t ht)
{
    _hash_entry_t *entry;

    /* alloc really shd be checking this... */
    if (ht->num_free == 0) {
        return NULL;
    }

    ht->num_free--;
    entry = ht->free_list;

    ht->free_list = ht->free_list->next;

    return entry;
}


/*
 * Function:
 *   _htb_entry_free
 * Purpose:
 *   Internal support routine; free a hash table entry
 * Parameters:
 *   (in)  ht - hash table to operate
 *   (out) entry  - entry to free
 * Returns:
 *   none
 * Notes:
 *   When the free list becomes excessively large (2 * alloc_blk_cnt), 
 *   alloc_blk_cnt are removed from the free list and returned to the
 *   system.
 */
STATIC void  
_htb_entry_free(shr_htb_hash_table_t ht, _hash_entry_t **entry)
{
    _htb_free_list_push(ht, *entry);
    *entry = NULL;

    if (ht->num_free > (2 * ht->alloc_blk_cnt)) {
        int i;
        _hash_entry_t *free_entry;

        for (i=0; i<ht->alloc_blk_cnt; i++) {
            free_entry = _htb_free_list_pop(ht);
            sal_free(free_entry);
        }
    }
}


/*
 * Function:
 *   _htb_entry_alloc
 * Purpose:
 *   Internal support routine; allocate a hash table entry
 * Parameters:
 *   (in) ht - hash table to operate
 * Returns:
 *   NULL if out of memory, else ptr to hash table entry
 * Notes:
 *   If the free list runs dry, this function will allocate
 *   another block of entries to the pool
 *   Assumes mutex lock has been obtained before called
 */
STATIC _hash_entry_t*
_htb_entry_alloc(shr_htb_hash_table_t ht)
{
    _hash_entry_t *entry = NULL;
    int entry_size = (sizeof(_hash_entry_t) - 
                      sizeof(shr_htb_key_t) + ht->key_size);

    /* if the free list is empty, allocate a block of entries to the pool */
    if (ht->num_free == 0) {        

        int i;
        for (i=0; i < ht->alloc_blk_cnt; i++) {
            entry = sal_alloc(entry_size, "htb_entry");
            if (entry == NULL) {
                return NULL;
            }

            _htb_free_list_push(ht, entry);
        }
    }

    /* free list must be non-zero here.  Pop a free entry & return */
    entry = _htb_free_list_pop(ht);

    if (entry) {
        sal_memset(entry, 0, entry_size);
    }

    return entry;
}

/*
 * Function:
 *   _htb_find
 * Purpose:
 *   Internal support routine; find an entry in the table
 * Parameters:
 *   (in)  ht     - hash table to operate
 *   (in)  key        - key to find
 *   (out) hash_index - hash index of key found
 *   (out) entry      - ptr to entry found
 *   (out) prev       - ptr to entry before <entry> in chain
 * Returns:
 *   BCM_E_NONE - if key is found in ht, else
 *   BCM_E_NOT_FOUND 
 * Notes:
 *  Assumes mutex lock has been taken before called.
 */
STATIC int
_htb_find(shr_htb_hash_table_t ht, shr_htb_key_t key,
          uint32 *hash_idx, _hash_entry_t **entry, _hash_entry_t **prev)
{
    /* compute the hash idx, ensure the index returned is in bounds */
    uint32 key_size = ht->key_size;
    uint8 *key_bytes = key;

    ht->cast_key_f(key, &key_bytes, &key_size);
    
    *hash_idx = ht->hash_f(key_bytes, key_size);
    *hash_idx &= ht->max_num_entries - 1;

    *prev = *entry = ht->table[*hash_idx];
    
    /* linear search the bucket, compare the entire key, not the caller
     * casted byte string version
     */
    while (*entry) {
        if (ht->key_cmp_f(key, &(*entry)->key, key_size) == 0) {
            break;
        }
        *prev = *entry;
        *entry = (*entry)->next;
    }

    if (*entry == NULL) {
        return _SHR_E_NOT_FOUND;
    }
    return _SHR_E_NONE;
}


/*
 * Function:
 *   shr_htb_find
 * Purpose:
 *   Find an entry with a key in a table, remove if requested.
 * Parameters:
 *   (in)  ht     - hash table to operate
 *   (in)  key    - key to find
 *   (out) data   - data stored at found key location
 *   (in)  remove - 0 - keeps entry, non-zero removes from table.
 * Returns:
 *  BCM_E_NONE - if found
 *  BCM_E_*
 * Notes:
 */
int
shr_htb_find(shr_htb_hash_table_t ht, shr_htb_key_t key, shr_htb_data_t *data,
             int remove)
{
    uint32 hash_idx;
    _hash_entry_t *entry, *prev;
    int rv;

    HTBL_LOCK(ht);

    rv = _htb_find(ht, key, &hash_idx, &entry, &prev);

    if (rv) {
        HTBL_UNLOCK(ht);
        return rv;
    }

    *data = entry->data;

    if (remove) {
        /* entry at head of list  */
        if (entry == ht->table[hash_idx]) {
            ht->table[hash_idx] = entry->next;
            
        } else {
            prev->next = entry->next;
        }
        
        /* return the entry to the free list */
        _htb_entry_free(ht, &entry);
    }

    HTBL_UNLOCK(ht);
    return _SHR_E_NONE;
}

/*
 * Function:
 *   shr_htb_insert
 * Purpose:
 *   Insert an entry into the hash table
 * Parameters:
 *   (in) ht   - hash table to operate
 *   (in) key  - key used to insert
 *   (in) data - data stored at key's location
 * Returns:
 *   BCM_E_NONE, upon success
 *   BCM_E_*     on error
 * Notes:
 *   The same key may not be inserted twice.
 */
int
shr_htb_insert(shr_htb_hash_table_t ht, shr_htb_key_t key, shr_htb_data_t data)
{
    uint32 hash_idx;
    _hash_entry_t *entry, *ignore;
    int rv = _SHR_E_NONE;
    
    HTBL_LOCK(ht);

    /* Check to see if the entry already exists, 
     * We'll consider this an error 
     */
    rv = _htb_find(ht, key, &hash_idx, &entry, &ignore);

    if (rv == _SHR_E_NONE) {
        rv = _SHR_E_EXISTS;
        goto exit;
    }
    rv = _SHR_E_NONE;

    /* any collisions?  Find the tail of the bucket */
    while (entry) {
        entry = entry->next;
    }

    /* alloc the entry and save the key & data */
    entry = _htb_entry_alloc(ht);
    if (entry == NULL) {
        rv = _SHR_E_MEMORY;
        goto exit;
    }

    entry->data = data;
    sal_memcpy(&entry->key, key, ht->key_size);

    /* Add the entry to the list, if there's a collision, just add it to the 
     * head of the list - it's not sorted
     */
    if(ht->table[hash_idx]) {
        /* collision */
        entry->next         = ht->table[hash_idx];
        ht->table[hash_idx] = entry;
    } else {
        ht->table[hash_idx] = entry;
    }

#if 0
    _dbg_dump_list(ht, hash_idx);
#endif

exit:
    HTBL_UNLOCK(ht);

    return rv;
}


/*
 * Function:
 *   shr_htb_create
 * Purpose:
 *   Create and initialize a hash table.
 * Parameters:
 *   (out) ht              - hash table to operate
 *   (in)  max_num_entries - maximum number of hash table entries, power of 2
 *   (in)  key_size        - size of keys used in hash function
 *   (in)  tbl_name        - name of table
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 * Notes:
 */
int
shr_htb_create(shr_htb_hash_table_t *ht, int max_num_entries, int key_size,
               char* tbl_name) 
{
    int rv = _SHR_E_NONE;
    int table_mem_size;
    shr_htb_hash_table_t prv_ht;

    /* table size must be a power of 2 */
    if((max_num_entries & (max_num_entries - 1)) != 0) {
        return _SHR_E_PARAM;
    }

    prv_ht = sal_alloc(sizeof(struct hash_table_s), "_hash_tbl_");
    if (prv_ht == NULL) {
        return _SHR_E_MEMORY;
    }
    sal_memset(prv_ht, 0, sizeof(struct hash_table_s));

    prv_ht->lock = sal_mutex_create(tbl_name);
    if (prv_ht->lock == NULL) {
        sal_free(prv_ht);
        return _SHR_E_RESOURCE;
    }

    prv_ht->max_num_entries = max_num_entries;
    prv_ht->key_size        = key_size;
    prv_ht->alloc_blk_cnt   = HTB_DEFAULT_ALLOC_BLK_CNT;
    prv_ht->hash_f          = htb_default_hash_f;
    prv_ht->key_cmp_f       = htb_default_key_cmp_f;
    prv_ht->cast_key_f      = htb_default_cast_key_f;

    /* hash table is an array of pointers */
    table_mem_size = max_num_entries * sizeof(_hash_entry_t*);
    prv_ht->table = sal_alloc(table_mem_size, tbl_name);
    if (prv_ht->table == NULL) {
        sal_mutex_destroy(prv_ht->lock);
        sal_free(prv_ht);
        return _SHR_E_MEMORY;
    }

    sal_memset(prv_ht->table, 0, table_mem_size);

    *ht = prv_ht;
    return rv;
}


/*
 * Function:
 *   shr_htb_destroy
 * Purpose:
 *   destroy a hash table and all associated entries.
 * Parameters:
 *   (in/out) ht  - hash table to operate
 *   (in)     cb  - callback function for each valid hash entry freed
 *                  may be NULL.
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 * Notes:
 *  The CALLER is responsible for freeing all memory associated with data by
 *  supplying a callback function.  The callback is called for all entries 
 *  found in the hash table.
 */
int
shr_htb_destroy(shr_htb_hash_table_t *ht, shr_htb_data_free_f cb)
{
    int i;
    int rv = _SHR_E_NONE;
    _hash_entry_t *entry, *next;
    shr_htb_hash_table_t prv_ht = *ht;

    /* for each entry in the table, 
     *   for each entry in the collision chain,
     *     call supplied callback
     *     return the entry to the free list
     */
    HTBL_LOCK(prv_ht);
    for (i=0; i < prv_ht->max_num_entries; i++) {
        entry = prv_ht->table[i];
        while (entry) {
            if (cb) {
                cb(entry->data);
            }
            next = entry->next;
            _htb_entry_free(prv_ht, &entry);   
            entry = next;
        }
    }

    /* free the free list */
    while(prv_ht->num_free > 0) {
        entry = _htb_free_list_pop(prv_ht);
        
        /* shouldn't happen.  free list accounting error if does */
        if (entry == NULL) {
            rv = _SHR_E_INTERNAL;
        } else {
            sal_free(entry);
        }
    }

    HTBL_UNLOCK(prv_ht);
    
    sal_mutex_destroy(prv_ht->lock);
    sal_free(prv_ht->table);
    sal_free(prv_ht);
    *ht = NULL;
    
    return rv;
}

int
shr_htb_iterate(int unit, shr_htb_hash_table_t ht, shr_htb_cb_t restore_cb)
{
    int                 rv = _SHR_E_NONE;
    _hash_entry_t      *entry, *next;
    int                 i;


    HTBL_LOCK(ht);

    for (i = 0; i < ht->max_num_entries; i++) {
        entry = ht->table[i];
        while (entry) {
            if (restore_cb) {
                restore_cb(unit, &(entry->key), entry->data);
            }
            next = entry->next;
            entry = next;
        }
    }

    HTBL_UNLOCK(ht);

    return(rv);
}

/*
 * Function:
 *   shr_htb_hash_func_set
 * Purpose:
 *   Change the default hash function
 * Parameters:
 *   (in) ht   - hash table to operate
 *   (in) func - replacement hash function
 * Returns:
 *   none
 * Notes:
 */
void 
shr_htb_hash_func_set(shr_htb_hash_table_t ht, shr_htb_hash_f func)
{
    ht->hash_f = func;
}

/*
 * Function:
 *   shr_htb_key_cmp_func_set
 * Purpose:
 *   Change the default key compare function
 * Parameters:
 *   (in) ht   - hash table to operate
 *   (in) func - replacement key compare func
 * Returns:
 *  none
 * Notes:
 */
void 
shr_htb_key_cmp_func_set(shr_htb_hash_table_t ht, shr_htb_key_cmp_f func)
{
    ht->key_cmp_f = func;
}


/*
 * Function:
 *   shr_htb_cast_key_func_set
 * Purpose:
 *   Change the default cast key function
 * Parameters:
 *   (in) ht   - hash table to operate
 *   (in) func - replacement cast key func
 * Returns:
 *  none
 * Notes:
 *  The cast_key method is provided to allow for complex key data types.  The
 *  cast implementation should return a pointer to a string of bytes that is
 *  the unique key for the hash entry, and the number of bytes in the string.
 *  The intent is to allow the most flexabilty for the caller to define a key
 *  structure, while keeping the hash function generic enough for most use 
 *  cases.
 */
void
shr_htb_cast_key_func_set(shr_htb_hash_table_t ht, shr_htb_cast_key_f func)
{
    ht->cast_key_f = func;
}
