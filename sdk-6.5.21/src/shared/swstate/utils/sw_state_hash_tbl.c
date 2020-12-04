/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: 	sw_state_hash_tbl.c
 *
 * Overview:
 * Provides a generic hash table interface with default hash
 * and key compare functions.
 *
 * Memory is pre-allocated at hash table creation based on max number of entries allowed.
 *
 * Collisions are handled by creating a linked list per hash index and
 * a linear search is performed within the list to find an entry.  The list is
 * not sorted.
 *
 * The caller defined KEY is *copied* and stored in the variable sized hash 
 * table entry for comparison. the caller defined DATA is  also *copied* and
 * stored.
 */

#if !defined(BCM_PETRA_SUPPORT)
typedef int make_iso_compilers_happy;
#else
/* { */
#include <shared/bsl.h>

#include <shared/swstate/sw_state_hash_tbl.h>
#include <include/shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>
#include <shared/error.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/util.h>
#include <sal/core/sync.h>

#include <soc/cm.h>

/* allocate this many blocks of hash entries when the free pool is dry */

/* temporarily don't use locking */
#ifdef SS_HTBL_LOCKING
#define SS_HTBL_LOCK(_ht_indx) \
    if (sw_state_sync_db[unit].dpp.htb_locks[_ht_indx] == NULL) {\
        sw_state_sync_db[unit].dpp.htb_locks[_ht_indx] = sal_mutex_create("workaround");\
        if (sw_state_sync_db[unit].dpp.htb_locks[_ht_indx] == NULL) {\
            return _SHR_E_RESOURCE;\
        }\
    }\
    else{\
        sal_mutex_take(sw_state_sync_db[unit].dpp.htb_locks[_ht_indx], sal_mutex_FOREVER);\
    }
    

#define SS_HTBL_UNLOCK(_ht_indx) \
    sal_mutex_give(sw_state_sync_db[unit].dpp.htb_locks[_ht_indx])
#else
#define SS_HTBL_LOCK(_ht_indx)
#define SS_HTBL_UNLOCK(_ht_indx)
#endif

#define SW_STATE_SYNC_ERROR_EXIT_IF_NULL(_ptr)\
    if (!_ptr) {\
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,\
                       (BSL_META_U(unit,\
                          "sw_state: not enough memory for allocation\n")));\
    }

/* 
 *  Sw state access macro.
 */

#define HTB_ACCESS sw_state_access[unit].dpp.shr.htbls


/*
 * Function:
 *   ss_htb_default_hash_f
 * Purpose:
 *   Generic default hash function.
 * Parameters:
 *   (in) k           - key as a string of bytes 
 *   (in) length      - Length of key in bytes
 * Returns:
 *   Hash index for the given key.
 */
uint32
ss_htb_default_hash_f(uint8 *k, uint32 length)
{
    return _shr_crc32(~0, k, length);
}

/*
 * Function:
 *   ss_htb_default_key_cmp_f
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
ss_htb_default_key_cmp_f(sw_state_htb_key_t a,
                      sw_state_htb_key_t b,
                      uint32 size)
{
    return sal_memcmp(a, b, size);
}


/*
 * Function:
 *  ss_htb_default_cast_key_f
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
ss_htb_default_cast_key_f(sw_state_htb_key_t key, uint8 **key_bytes, uint32 *key_size)
{
    /* do nothing */
}

/*
 * Function:
 *  ss_htb_free_list_push
 * Purpose:
 *  Internal routine; add an hash entry index to the free stack
 * Parameters:
 *  (in) unit
 *  (in) ht_indx - hash table index to operate on
 *  (in) entry_id  - entry id to add to the list
 * Returns:
 *   _SHR_E_ error symbol
 * Notes:
 */
STATIC int
ss_htb_free_list_push(int unit, int ht_indx, int entry_id)
{
    int num_free;

    /* num_free++ */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.num_free.get(unit, ht_indx, &num_free));
    num_free++;
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.num_free.set(unit, ht_indx, num_free));

    /* set at the next free slot the new emptied entry */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.free_arr.set(unit, ht_indx, num_free-1 ,entry_id));

    return _SHR_E_NONE;
}


/*
 * Function:
 *   ss_htb_free_list_pop
 * Purpose:
 *   Internal support routine;  grab an entry from the free list. 
 * Parameters:
 *   (in) unit
 *   (in) ht_indx - hash table index to operate on
 *   (in) *entry - the function fill the data of this
 *                 variable with the poped entry
 * Returns:
 *   _SHR_E_ error symbol
 * Notes:
 */
STATIC int
ss_htb_free_list_pop(int unit, int ht_indx, int *entry)
{
    int num_free;

    /* num_free get */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.num_free.get(unit, ht_indx, &num_free));

    /* alloc really shd be checking this... */
    if (num_free == 0) {
        return _SHR_E_RESOURCE;
    }

    /* num_free-- */
    num_free--;
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.num_free.set(unit, ht_indx, num_free));

    /* get the next free slot */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.free_arr.get(unit, ht_indx, num_free, entry));

    return _SHR_E_NONE;
}


/*
 * Function:
 *   ss_htb_entry_free
 * Purpose:
 *   Internal support routine; free a hash table entry
 * Parameters:
 *   (in) unit
 *   (in) ht_indx - hash table index to operate on
 *   (in) entry_id  - entry to free
 * Returns:
 *   _SHR_E_ error symbol
 * Notes:
 */
STATIC int  
ss_htb_entry_free(int unit, int ht_indx, int entry_id)
{
    int i;
    int key_size;
    int data_size;

    _SHR_E_IF_ERROR_RETURN(
       ss_htb_free_list_push(unit, ht_indx, entry_id));

    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.key_size.get(unit, ht_indx, &key_size));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.data_size.get(unit, ht_indx, &data_size));

    /* reset the next "ptr" */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.entry_arrays.next.set(unit, ht_indx, entry_id, -1));

    /* reset the data */
    for (i = 0; i < data_size; i++) {
        
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.entry_arrays.datas.set(unit, ht_indx, (entry_id * data_size) + i, 0x0));
    }

    /* reset the key */
    for (i = 0; i < key_size; i++) {
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.entry_arrays.keys.set(unit, ht_indx, (entry_id * key_size) + i, 0x0));
    }
    
    return _SHR_E_NONE;
}


/*
 * Function:
 *   ss_htb_find
 * Purpose:
 *   Internal support routine; find an entry in the table
 * Parameters:
 *   (in)  unit
 *   (in)  ht_indx - hash table index to operate on
 *   (in)  key        - key to find
 *   (out) hash_index - hash index of key found
 *   (out) entry_id      - ptr to entry id found
 *   (out) prev_id       - ptr to entry id before <entry> in chain
 * Returns:
 *   BCM_E_NONE - if key is found in ht, else
 *   BCM_E_NOT_FOUND 
 * Notes:
 *  Assumes mutex lock has been taken before called.
 */
STATIC int
ss_htb_find(int unit, int ht_indx, sw_state_htb_key_t key,
          uint32 *hash_idx, int *entry_id, int *prev_id)
{
    /* compute the hash idx, ensure the index returned is in bounds */
    int    key_size;
    uint32 key_size_32bit;
    int    max_num_entries;
    uint8 *key_bytes = key;
    uint8 *entry_key;
    int    i;
    int    next;
    int    rv;

    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.key_size.get(unit, ht_indx, &key_size));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.max_num_entries.get(unit, ht_indx, &max_num_entries));

    entry_key = sal_alloc(key_size, "htb_key");
    if (entry_key == NULL) {
        return _SHR_E_MEMORY;
    }

    key_size_32bit = key_size;
    ss_htb_default_cast_key_f(key, &key_bytes, &key_size_32bit);
    
    *hash_idx = ss_htb_default_hash_f(key_bytes, key_size);
    *hash_idx &= max_num_entries - 1;

    rv = HTB_ACCESS.hash_table.table.get(unit, ht_indx, *hash_idx, entry_id);
    if (rv != _SHR_E_NONE) {
        sal_free(entry_key);
        return rv;
    }
    *prev_id = *entry_id;
    
    /* linear search the bucket, compare the entire key, not the caller
     * casted byte string version
     */
    while (*entry_id != -1) {
        for (i = 0; i < key_size; i++) {
            rv = HTB_ACCESS.hash_table.entry_arrays.keys.get(unit, ht_indx, (*entry_id * key_size) + i, &entry_key[i]);
            if (rv != _SHR_E_NONE) {
                sal_free(entry_key);
                return rv;
            }
        }
        if (ss_htb_default_key_cmp_f(key, entry_key, key_size) == 0) {
            break;
        }
        *prev_id = *entry_id;
        rv = HTB_ACCESS.hash_table.entry_arrays.next.get(unit, ht_indx, *entry_id, &next);
        if (rv != _SHR_E_NONE) {
            sal_free(entry_key);
            return rv;
        }
        *entry_id = next;
    }

    sal_free(entry_key);

    if (*entry_id == -1) {
        return _SHR_E_NOT_FOUND;
    }
    return _SHR_E_NONE;
}


/*
 * Function:
 *   sw_state_htb_find
 * Purpose:
 *   Find an entry with a key in a table, remove if requested.
 * Parameters:
 *   (in)  unit
 *   (in)  ht_indx - hash table index to operate on
 *   (in)  key    - key to find
 *   (out) data   - Ponter. This procedure loads pointed memory by data stored at found key location
 *   (in)  remove - 0 - keeps entry, non-zero removes from table.
 * Returns:
 *  BCM_E_NONE - if found
 *  BCM_E_*
 * Notes:
 */
int
sw_state_htb_find(int unit, int ht_indx, sw_state_htb_key_t key, sw_state_htb_data_t data,
             int remove)
{
    uint32 hash_idx;
    int entry_id, prev_id;
    int rv;
    int i;
    int data_size;
    int id_from_table;
    int next;

    SS_HTBL_LOCK(ht_indx);

    rv = ss_htb_find(unit, ht_indx, key, &hash_idx, &entry_id, &prev_id);

    if (rv) {
        SS_HTBL_UNLOCK(ht_indx);
        return rv;
    }

    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.data_size.get(unit, ht_indx, &data_size));

    /* get the data */
    for (i = 0; i < data_size; i++) {
        
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.entry_arrays.datas.get(unit, ht_indx, (entry_id * data_size) + i, &data[i]));
    }

    if (remove) {
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.table.get(unit, ht_indx, hash_idx, &id_from_table));
        /* entry at head of list  */
        if (entry_id == id_from_table) {
            _SHR_E_IF_ERROR_RETURN(
               HTB_ACCESS.hash_table.entry_arrays.next.get(unit, ht_indx, entry_id, &next));
            _SHR_E_IF_ERROR_RETURN(
               HTB_ACCESS.hash_table.table.set(unit, ht_indx, hash_idx, next));
        } else {
            _SHR_E_IF_ERROR_RETURN(
               HTB_ACCESS.hash_table.entry_arrays.next.get(unit, ht_indx, entry_id, &next));
            _SHR_E_IF_ERROR_RETURN(
               HTB_ACCESS.hash_table.entry_arrays.next.set(unit, ht_indx, prev_id, next));
        }
        
        /* return the entry to the free list */
        ss_htb_entry_free(unit, ht_indx, entry_id);
    }

    SS_HTBL_UNLOCK(ht_indx);
    return _SHR_E_NONE;
}

/*
 * Function:
 *   sw_state_htb_insert
 * Purpose:
 *   Insert an entry into the hash table
 * Parameters:
 *   (in) unit
 *   (in) ht_indx - hash table index to operate on
 *   (in) key  - key used to insert
 *   (in) data - Pointer to the data to be stored at key's location
 * Returns:
 *   BCM_E_NONE, upon success
 *   BCM_E_*     on error
 * Notes:
 *   The same key may not be inserted twice.
 */
int
sw_state_htb_insert(int unit, int ht_indx, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    uint32 hash_idx;
    int entry, ignore;
    int rv = _SHR_E_NONE;
    int next;
    int data_size;
    int key_size;
    int table_entry;
    int i;
    
    SS_HTBL_LOCK(ht_indx);

    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.key_size.get(unit, ht_indx, &key_size));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.data_size.get(unit, ht_indx, &data_size));

    /* Check to see if the entry already exists, 
     * We'll consider this an error 
     */
    rv = ss_htb_find(unit, ht_indx, key, &hash_idx, &entry, &ignore);

    if (rv == _SHR_E_NONE) {
        rv = _SHR_E_EXISTS;
        goto exit;
    }
    rv = _SHR_E_NONE;

    /* any collisions?  Find the tail of the bucket */
    while (entry != -1) {
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.entry_arrays.next.get(unit, ht_indx, entry, &next));
        entry = next;
    }

    _SHR_E_IF_ERROR_RETURN(
       ss_htb_free_list_pop(unit, ht_indx, &entry));

    /* set the data for the entry*/
    for (i = 0; i < data_size; i++) {
        
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.entry_arrays.datas.set(unit, ht_indx, (entry * data_size) + i, data[i]));
    }

    /* set the key for the entry*/
    for (i = 0; i < key_size; i++) {
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.entry_arrays.keys.set(unit, ht_indx, (entry * key_size) + i, key[i]));
    }

    /* Add the entry to the list, if there's a collision, just add it to the 
     * head of the list - it's not sorted
     */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.table.get(unit, ht_indx, hash_idx, &table_entry));
    if(table_entry != -1) {
        /* collision */
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.entry_arrays.next.set(unit, ht_indx, entry, table_entry));
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.table.set(unit, ht_indx, hash_idx, entry));
    } else {
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.table.set(unit, ht_indx, hash_idx, entry));
    }

exit:
    SS_HTBL_UNLOCK(ht_indx);

    return rv;
}


/*
 * Function:
 *   sw_state_htb_create
 * Purpose:
 *   Create and initialize a hash table.
 * Parameters:
 *   (in)  unit
 *   (in)  ht_indx - hash table index to operate on
 *   (in)  max_num_entries - maximum number of hash table entries, power of 2
 *   (in)  key_size        - size of keys used in hash function
 *   (in)  data_size       - size of data used in hash function
 *   (in)  tbl_name        - name of table
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 * Notes:
 */
int
sw_state_htb_create(int unit, int *ht_indx, int max_num_entries, int key_size, int data_size,
               char* tbl_name)
{
    int rv = _SHR_E_NONE;
    int nof_htbl_used;
    int i;
    int max_nof_htbls;

    /* table size must be a power of 2 */
    if((max_num_entries & (max_num_entries - 1)) != 0) {
        return _SHR_E_PARAM;
    }

    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.nof_htbl_used.get(unit, &nof_htbl_used));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.max_nof_htbls.get(unit, &max_nof_htbls));

    /* check if there is enough memory for a new htb */
    if (nof_htbl_used >= max_nof_htbls) {
        return _SHR_E_MEMORY;
    }

    *ht_indx = nof_htbl_used;

    /* alloc new hash table at next free slot */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.alloc(unit, *ht_indx));

    /* nof_htbl_used++ */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.nof_htbl_used.set(unit, nof_htbl_used +1));

    /* mutex create */
    sw_state_sync_db[unit].dpp.htb_locks[*ht_indx] = sal_mutex_create(tbl_name);
    if (sw_state_sync_db[unit].dpp.htb_locks[*ht_indx] == NULL) {
        HTB_ACCESS.hash_table.free(unit, *ht_indx);
        return _SHR_E_RESOURCE;
    }

    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.max_num_entries.set(unit, *ht_indx, max_num_entries));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.key_size.set(unit, *ht_indx, key_size));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.data_size.set(unit, *ht_indx, data_size));

    /* hash table is an array of integers (ints are indexes in the entries arrays) */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.table.alloc(unit, *ht_indx, max_num_entries));

    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.entry_arrays.keys.alloc(unit, *ht_indx, key_size * max_num_entries));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.entry_arrays.datas.alloc(unit, *ht_indx, data_size * max_num_entries));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.entry_arrays.next.alloc(unit, *ht_indx, max_num_entries));

    /* mark all next and table entries as null (-1) */
    for (i=0; i < max_num_entries; i++) {
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.table.set(unit, *ht_indx, i, -1));
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.entry_arrays.next.set(unit, *ht_indx, i, -1));
    }

    /* create the free list (actually a stack) which is now full */
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.free_arr.alloc(unit, *ht_indx, max_num_entries));
    for (i=0; i < max_num_entries; i++) {
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.free_arr.set(unit, *ht_indx, i, max_num_entries-i-1));
    }
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.num_free.set(unit, *ht_indx, max_num_entries));

    return rv;
}


/*
 * Function:
 *   sw_state_htb_destroy
 * Purpose:
 *   destroy a hash table and all associated entries.
 * Parameters:
 *   (in)  unit
 *   (in)  ht_indx - hash table index to operate on
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
sw_state_htb_destroy(int unit, int ht_indx, sw_state_htb_data_free_f cb)
{
    int rv = _SHR_E_NONE;
    
    /* currently not important, will be free at sw state deinit */
    /*
     * Currently, we ignore the callback procedure. If it is to be revived then
     * it should be called for exch entry of the hash table - This releases the
     * corresponding linked list.
     */
    

    return rv;
}

int
sw_state_htb_iterate(int unit, int ht_indx, sw_state_htb_cb_t restore_cb)
{
    int      rv = _SHR_E_NONE;
    int      entry;
    int      data_size;
    int      key_size;
    int      max_num_entries;
    int      i,j;
    int      next;
    uint8   *entry_key;
    uint8   *entry_data;


    SS_HTBL_LOCK(ht_indx);

    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.data_size.get(unit, ht_indx, &data_size));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.key_size.get(unit, ht_indx, &key_size));
    _SHR_E_IF_ERROR_RETURN(
       HTB_ACCESS.hash_table.max_num_entries.get(unit, ht_indx, &max_num_entries));

    entry_key = sal_alloc(key_size, "htb_key");
    if (entry_key == NULL) {
        return _SHR_E_MEMORY;
    }
    entry_data = sal_alloc(data_size, "htb_data");
    if (entry_data == NULL) {
        sal_free(entry_key);
        return _SHR_E_MEMORY;
    }

    for (j = 0; j < max_num_entries; j++) {
        rv = HTB_ACCESS.hash_table.table.get(unit, ht_indx, j, &entry);
        if (rv != _SHR_E_NONE) {
            sal_free(entry_key);
            sal_free(entry_data);
            return rv;
        }
        while (entry != -1) {
            if (restore_cb) {
                /* get the data for the entry*/
                for (i = 0; i < data_size; i++) {
                    
                    rv = HTB_ACCESS.hash_table.entry_arrays.datas.get(unit, ht_indx, (entry * data_size) + i, &entry_data[i]);
                    if (rv != _SHR_E_NONE) {
                        sal_free(entry_key);
                        sal_free(entry_data);
                        return rv;
                    }
                }

                /* get the key for the entry*/
                for (i = 0; i < key_size; i++) {
                    rv = HTB_ACCESS.hash_table.entry_arrays.keys.get(unit, ht_indx, (entry * key_size) + i, &entry_key[i]);
                    if (rv != _SHR_E_NONE) {
                        sal_free(entry_key);
                        sal_free(entry_data);
                        return rv;
                    }
                }

                /* apply the cb */
                _SHR_E_IF_ERROR_RETURN(restore_cb(unit, entry_key, entry_data));

                /* set the data for the entry*/
                for (i = 0; i < data_size; i++) {
                    
                    rv = HTB_ACCESS.hash_table.entry_arrays.datas.set(unit, ht_indx, (entry * data_size) + i, entry_data[i]);
                    if (rv != _SHR_E_NONE) {
                        sal_free(entry_key);
                        sal_free(entry_data);
                        return rv;
                    }
                }

                /* set the key for the entry*/
                for (i = 0; i < key_size; i++) {
                    rv = HTB_ACCESS.hash_table.entry_arrays.keys.set(unit, ht_indx, (entry * key_size) + i, entry_key[i]);
                    if (rv != _SHR_E_NONE) {
                        sal_free(entry_key);
                        sal_free(entry_data);
                        return rv;
                    }
                }
            }
            rv = HTB_ACCESS.hash_table.entry_arrays.next.get(unit, ht_indx, entry, &next);
            if (rv != _SHR_E_NONE) {
                sal_free(entry_key);
                sal_free(entry_data);
                return rv;
            }
            entry = next;
        }
    }

    sal_free(entry_key);
    sal_free(entry_data);

    SS_HTBL_UNLOCK(ht_indx);

    return(rv);
}

/*
 * Function:
 *   sw_state_htb_init
 * Purpose:
 *   init required structure to manage sw state hash tables.
 * Parameters:
 *   (in)  unit
 *   (in)  max_nof_htbls - max number of hash tables allowed
 *
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 * Notes:
 *   this function must be called before creating any hash tables.
 */
int
sw_state_htb_init(int unit, int max_nof_htbls)
{

    sw_state_sync_db[unit].dpp.htb_locks = sal_alloc(sizeof(sal_mutex_t) * SW_STATE_MAX_NOF_HASH_TABLES, "hash table mutexes");
    if (sw_state_sync_db[unit].dpp.htb_locks == NULL) {
        return _SHR_E_MEMORY;
    }
    
    if (!SOC_WARM_BOOT(unit)) {
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.nof_htbl_used.set(unit, 0x0));
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.max_nof_htbls.set(unit, max_nof_htbls));
        _SHR_E_IF_ERROR_RETURN(
           HTB_ACCESS.hash_table.ptr_alloc(unit, max_nof_htbls));
    }

    return _SHR_E_NONE;
}

/*
 * Function:
 *   sw_state_htb_deinit
 * Parameters:
 *   (in)  unit
 *
 * Returns:
 *   BCM_E_NONE upon success
 *   BCM_E_*    on failure
 */
int
sw_state_htb_deinit(int unit)
{
    sal_free(sw_state_sync_db[unit].dpp.htb_locks);
    return _SHR_E_NONE;
}
/* } */
#endif

