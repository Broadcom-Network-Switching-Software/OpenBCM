/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cpudb.c
 * Purpose:     CPU database; 
 * Requires:
 *
 * See the document cpu_database.txt for more information.
 *
 * The sysid is an application specific number between 0 and number
 * of CPUs permitted.  It is independent of the hard index used
 * by cpudb.
 */

#include <assert.h>

#include <sal/core/sync.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>
#include <bcm/error.h>

#include <appl/cpudb/cpudb.h>

#define CPUDB_CHECK(db_ref, rv) if (!DB_REF_VALID(db_ref)) return (rv)
#define CPUDB_MAGIC 0xfeedface
#define DB_REF_VALID(db_ref)    \
    (((db_ref) != NULL) && ((db_ref)->magic == CPUDB_MAGIC))
#define CPUDB_SET_MAGIC(db_ref) (db_ref)->magic = CPUDB_MAGIC
#define CPUDB_CLEAR_MAGIC(db_ref) (db_ref)->magic = 0

/* Declare broadcast and neighbor keys */
CPUDB_BCAST_KEY_DECLARATION;
CPUDB_NEIGHBOR_KEY_DECLARATION;

static sal_mutex_t cpudb_lock;
#define _CPUDB_LOCK sal_mutex_take(cpudb_lock, sal_mutex_FOREVER)
#define _CPUDB_UNLOCK sal_mutex_give(cpudb_lock)
#define INIT_DONE (cpudb_lock != NULL)

STATIC int
_cpudb_init(void)
{
    if (cpudb_lock == NULL) {
        cpudb_lock = sal_mutex_create("cpudb-int-lock");
        if (cpudb_lock == NULL) {
            return BCM_E_MEMORY;
        }
    }

    return BCM_E_NONE;
}

STATIC cpudb_entry_t *_cpudb_entry_create(cpudb_t *db_ref,
                                          const cpudb_key_t key);

/****************************************************************
 *
 * DB selection functions
 *      cpudb_create            Create a DB
 *      cpudb_destroy           De-init a DB
 *      cpudb_valid             Check if DB reference is valid
 *      cpudb_clear             Remove all info from a DB
 */


/*
 * Function:
 *      cpudb_create
 * Purpose:
 *      Create a database
 * Parameters:
 * Returns:
 *      Small integer reference >= 0 if successful; used in cpudb calls.
 *      BCM_E_XXX < 0 otherwise.
 * Notes:
 */

cpudb_ref_t
cpudb_create(void)
{
    cpudb_t *db;

    if (!INIT_DONE) {
        if (_cpudb_init() < 0) {
            return NULL;
        }
    }

    db = sal_alloc(sizeof(cpudb_t), "cpudb_create");
    if (db == NULL) {
        return NULL;
    }

    sal_memset(db, 0, sizeof(cpudb_t));
    CPUDB_SET_MAGIC(db);
    db->old_db = CPUDB_REF_NULL;

    return db;
}


STATIC int
_cpudb_clear(cpudb_ref_t db_ref, int keep_local)
{
    int i;
    cpudb_entry_t *entry, *next_entry;
    cpudb_entry_t save_local_entry;
    int saved = FALSE;

    if (keep_local) {
        if (db_ref->local_entry != NULL) {
            sal_memcpy(&save_local_entry, db_ref->local_entry,
                       sizeof(cpudb_entry_t));
            saved = TRUE;
        }
    }

    for (entry = db_ref->entries; entry != NULL; entry = next_entry) {
        next_entry = entry->next;
        sal_free(entry);
    }

    db_ref->entries = NULL;
    db_ref->num_cpus = 0;

    /* Clear key hashes */
    for (i = 0; i < CPUDB_HASH_ENTRY_COUNT; i++) {
        db_ref->key_hash[i] = NULL;
    }

    db_ref->master_entry = NULL;

    if (saved) {
        entry = _cpudb_entry_create(db_ref, save_local_entry.base.key);
        if (entry == NULL) {
            return BCM_E_FAIL;
        }
        sal_memcpy(&entry->base, &save_local_entry.base,
                   sizeof(cpudb_base_t));
        entry->flags = CPUDB_F_IS_LOCAL | CPUDB_F_BASE_INIT_DONE;
        db_ref->local_entry = entry;
        /* Clear the stack info for the local entry */
        for (i = 0; i < entry->base.num_stk_ports; i++) {
            /* Clear all flags except no link and ETH flag */
            entry->sp_info[i].flags =
                save_local_entry.sp_info[i].flags &
                (CPUDB_SPF_NO_LINK|CPUDB_SPF_ETHERNET);
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      cpudb_clear
 * Purpose:
 *      Clear (initialized) the given DB.
 * Parameters:
 *      db_ref     - which DB to clear
 *      keep_local - Boolean indicating if local entry should be kept
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Only the base info and user_cookie of the local entry is kept.
 *      The flags are cleared except for "is_local"
 */

int
cpudb_clear(cpudb_ref_t db_ref, int keep_local)
{
    int rv;

    CPUDB_CHECK(db_ref, BCM_E_PARAM);
    _CPUDB_LOCK;
    rv = _cpudb_clear(db_ref, keep_local);
    _CPUDB_UNLOCK;

    return rv;
}


/*
 * Function:
 *      cpudb_destroy
 * Purpose:
 *      De-init a database
 * Parameters:
 *      db_ref        - Reference to DB to destroy
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      There should be no calls pending on the db_lock of this DB.
 *      Will not destroy db_ref->old_db even if non-NULL.
 */

int
cpudb_destroy(cpudb_ref_t db_ref)
{
    int rv;

    CPUDB_CHECK(db_ref, BCM_E_PARAM);
    _CPUDB_LOCK;
    CPUDB_CLEAR_MAGIC(db_ref);

    rv = _cpudb_clear(db_ref, FALSE);
    if (rv == BCM_E_NONE) {
        sal_free(db_ref);
    } else { /* Shouldn't happen with keep_local == FALSE */
        assert(!"CPUDB failed to clear DB");
    }
    _CPUDB_UNLOCK;

    return rv;
}


/*
 * Function:
 *      cpudb_valid
 * Purpose:
 *      Is a CPU DB reference currently valid?
 * Parameters:
 *      db_ref         - The database reference to check
 * Returns:
 *      Boolean:  True means currently exists
 */

int
cpudb_valid(cpudb_ref_t db_ref)
{
    return DB_REF_VALID(db_ref);
}


/****************************************************************
 *
 * Create functions
 *      cpudb_entry_create      Add an entry
 *      cpudb_entry_count_get   How many CPU entries in DB
 *
 * Set functions
 *      cpudb_sysid_set         Set system id by key
 *
 * Lookup functions; see also macros in cpudb.h
 *      cpudb_mac_lookup        Get entry pointer by MAC
 *      cpudb_sysid_lookup      Get entry pointer by system id
 *
 * Remove functions
 *      cpudb_entry_remove      Remove entry with given key
 */

STATIC void _cpudb_unlink_entry(cpudb_ref_t db_ref,
                                cpudb_entry_t *entry);

/*
 * Function:
 *      cpudb_entry_create
 * Purpose:
 *      Create an entry for the given key with locking
 * Parameters:
 *      db_ref         - The database reference
 *      key            - The key to identify new entry
 * Returns:
 *      Pointer to entry created
 * Notes:
 *      Returns pointer to the entry or NULL if fails
 */

cpudb_entry_t *
cpudb_entry_create(cpudb_ref_t db_ref, const cpudb_key_t key, int is_local)
{
    cpudb_entry_t *entry;

    CPUDB_CHECK(db_ref, NULL);

    _CPUDB_LOCK;
    entry =  _cpudb_entry_create(db_ref, key);
    if ((entry != NULL) && (is_local)) {
        db_ref->local_entry = entry;
        entry->flags |= CPUDB_F_IS_LOCAL;
    }
    _CPUDB_UNLOCK;

    return entry;
}


/*
 * Function:
 *      cpudb_local_base_info_set
 * Purpose:
 *      Set up local entry in db_ref according to local_entry data
 * Parameters:
 *      db_ref          - DB to update
 *      local_entry     - Pointer to "local" entry data
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Creates local entry if one doesn't already exist in the DB.
 *      Error if local entry exists in db_ref with a different key.
 *      Sets the "base init done" flag for the entry.
 */

int
cpudb_local_base_info_set(cpudb_ref_t db_ref, cpudb_base_t *local_base)
{
    cpudb_entry_t *entry;

    CPUDB_CHECK(db_ref, BCM_E_PARAM);
    if (db_ref->local_entry == NULL) {
        entry = cpudb_entry_create(db_ref, local_base->key, TRUE);
        if (entry == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        if (CPUDB_KEY_COMPARE(local_base->key,
                              db_ref->local_entry->base.key) != 0) {
            return BCM_E_EXISTS;
        }
        entry = db_ref->local_entry;
    }

    sal_memcpy(&entry->base, local_base, sizeof(cpudb_base_t));
    entry->flags |= CPUDB_F_BASE_INIT_DONE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      cpudb_master_set
 * Purpose:
 *      Set the entry to be the master
 * Parameters:
 *      db_ref       -- DB being updated
 *      key          -- Key of the master CPU
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If key is not in the DB, returns NOT_FOUND
 */

int
cpudb_master_set(cpudb_ref_t db_ref, const cpudb_key_t key)
{
    cpudb_entry_t *entry;

    CPUDB_CHECK(db_ref, BCM_E_PARAM);
    CPUDB_KEY_SEARCH(db_ref, key, entry);
    if (entry == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if (db_ref->master_entry != NULL) {
        db_ref->master_entry->flags &= ~CPUDB_F_IS_MASTER;
    }

    db_ref->master_entry = entry;
    entry->flags |= CPUDB_F_IS_MASTER;

    return BCM_E_NONE;
}


/*
 * Function:
 *      cpudb_master_get
 * Purpose:
 *      Get pointer to the master entry, if set
 * Parameters:
 *      db_ref     -- The DB to examine
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Returns NULL if the master is not set.
 */

cpudb_entry_t *
cpudb_master_get(cpudb_ref_t db_ref)
{
    CPUDB_CHECK(db_ref, NULL);
    
    return db_ref->master_entry;
}


/*
 * Function:
 *      cpudb_entry_count_get
 * Purpose:
 *      Returns the number of entries in the DB referenced.
 * Parameters:
 *      db_ref             - DB to update
 * Returns:
 *      BCM_E_XXX < 0 if not valid; otherwise, number of entries in this DB
 * Notes:
 */

int
cpudb_entry_count_get(cpudb_ref_t db_ref)
{
    CPUDB_CHECK(db_ref, BCM_E_PARAM);

    return db_ref->num_cpus;
}


/*
 * Function:
 *      cpudb_sysid_set
 * Purpose:
 *      Set the sysid of a CPUDB entry
 * Parameters:
 *      db_ref     - Reference to CPUDB
 *      key        - Key of entry to update
 *      sysid      - Value to set sysid to.
 *      overwrite  - If TRUE, will overwrite existing sysid; otherwise
 *                   returns exists.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Checks whether the sysid is already assigned to a different entry
 */

int
cpudb_sysid_set(cpudb_ref_t db_ref, cpudb_key_t key, void *sysid,
                int overwrite)
{
    cpudb_entry_t *entry;
    int rv = BCM_E_NONE;

    CPUDB_CHECK(db_ref, BCM_E_PARAM);

    entry = cpudb_sysid_lookup(db_ref, sysid);
    if (entry != NULL) {   /* Sys ID in DB already; see if key matches */
        if (CPUDB_KEY_COMPARE(entry->base.key, key)) {   /* memcmp semantics */
            return BCM_E_EXISTS;  /* Key mismatch */
        }

        return BCM_E_NONE;   /* Exists, matches */
    }

    _CPUDB_LOCK;

    CPUDB_KEY_SEARCH(db_ref, key, entry);
    if (entry == NULL) {
        _CPUDB_UNLOCK;
        return BCM_E_NOT_FOUND;
    }

    if (entry->flags & CPUDB_F_SYSID_KNOWN) {
        if (overwrite) {
            entry->sysid = sysid;
        } else {
            rv = BCM_E_EXISTS;
        }
        /* We know it doesn't match b/c the above search failed */
        _CPUDB_UNLOCK;
        return rv;
    }

    entry->sysid = sysid;
    entry->flags |= CPUDB_F_SYSID_KNOWN;

    _CPUDB_UNLOCK;
    return BCM_E_NONE;
}


/*
 * Function:
 *      cpudb_sysid_lookup
 * Purpose:
 *      Search DB for sysid (not efficient)
 * Parameters:
 *      db_ref    - DB to search
 *      sysid     - sysid to search for
 * Returns:
 *      Pointer to entry if found; otherwise NULL
 */

cpudb_entry_t *
cpudb_sysid_lookup(cpudb_ref_t db_ref, void *sysid)
{
    cpudb_entry_t *cur;

    CPUDB_CHECK(db_ref, NULL);
    _CPUDB_LOCK;
    CPUDB_FOREACH_ENTRY(db_ref, cur) {
        if (cur->flags & CPUDB_F_SYSID_KNOWN && cur->sysid == sysid) {
            _CPUDB_UNLOCK;
            return cur;  /* Found it */
        }
    }

    _CPUDB_UNLOCK;
    return NULL;
}

/*
 * Function:
 *      cpudb_mac_lookup
 * Purpose:
 *      Search DB for mac (not efficient)
 * Parameters:
 *      db_ref    - DB to search
 *      mac       - mac to search for
 * Returns:
 *      Pointer to entry if found; otherwise NULL
 * Notes:
 *      In the future, for systems with many CPUs supported, we
 *      might want to add a hash lookup for the MAC address.
 */

cpudb_entry_t *
cpudb_mac_lookup(cpudb_ref_t db_ref, const bcm_mac_t mac)
{
    cpudb_entry_t *cur;

    CPUDB_CHECK(db_ref, NULL);
    _CPUDB_LOCK;
    CPUDB_FOREACH_ENTRY(db_ref, cur) {
        if (!sal_memcmp(cur->base.mac, mac, sizeof(bcm_mac_t))) {
            _CPUDB_UNLOCK;
            return cur;  /* Found it */
        }
    }

    _CPUDB_UNLOCK;
    return NULL;
}


/*
 * Function:
 *      cpudb_entry_remove
 * Purpose:
 *      Remove an entry from the CPU database by key
 * Parameters:
 *      db_ref             - DB to update
 *      key                - The key to search for and remove
 * Returns:
 *      BCM_E_XXX
 */

int
cpudb_entry_remove(cpudb_ref_t db_ref, const cpudb_key_t key)
{
    cpudb_entry_t *entry;

    CPUDB_CHECK(db_ref, BCM_E_PARAM);
    _CPUDB_LOCK;

    /* Check if removing entry marked local or master */
    if (db_ref->local_entry != NULL &&
        CPUDB_KEY_EQUAL(db_ref->local_entry->base.key, key)) {
        db_ref->local_entry = NULL;
    }
    if (db_ref->master_entry != NULL &&
        CPUDB_KEY_EQUAL(db_ref->master_entry->base.key, key)) {
        db_ref->master_entry = NULL;
    }

    CPUDB_KEY_SEARCH(db_ref, key, entry);
    if (entry != NULL) {
        _cpudb_unlink_entry(db_ref, entry);
        sal_free(entry);
    }

    _CPUDB_UNLOCK;
    return BCM_E_NONE;
}


static char	cpudb_hex[] = "0123456789abcdef";

/*
 * Format a key into a string.
 * This version assumes that a key is a MAC address.
 */
int
cpudb_key_format(cpudb_key_t key, char *buf, int len)
{
    int	i, n;

    /* 00:11:22:33:44:55:66 */
    if (len < CPUDB_KEY_STRING_LEN) {
	return BCM_E_FAIL;
    }
    for (i = 0; i < 6; i++) {
	n = key.key[i];
	if (n > 0xf) {
	    *buf++ = cpudb_hex[(n>>4) & 0xf];
	}
	*buf++ = cpudb_hex[n & 0xf];
	*buf++ = ':';
    }
    *--buf = '\0';
    return BCM_E_NONE;
}

/*
 * Parse a string into a key.
 * This version assumes that a key is a MAC address.
 */
int
cpudb_key_parse(char *buf, cpudb_key_t *keyp)
{
    int		i, c1, c2;
    char	*s;

    keyp->key[0] = keyp->key[1] = keyp->key[2] = 0;
    keyp->key[3] = keyp->key[4] = keyp->key[5] = 0;

    if (buf == NULL) {
	return BCM_E_FAIL;
    }

    /* skip leading 0x if plain hex format */
    if (buf[0] == '0' && (buf[1] == 'x' || buf[1] == 'X')) {
	buf += 2;
    }

    /* start at end of string and work backwards */
    for (s = buf; *s; s++) {
	;
    }

    for (i = 5; i >= 0 && s >= buf; i--) {
	c1 = c2 = 0;
	if (--s >= buf) {
	    if (*s >= '0' && *s <= '9') {
		c2 = *s - '0';
	    } else if (*s >= 'a' && *s <= 'f') {
		c2 = *s - 'a' + 10;
	    } else if (*s >= 'A' && *s <= 'F') {
		c2 = *s - 'A' + 10;
	    } else if (*s == ':') {
		;
	    } else {
		return BCM_E_FAIL;
	    }
	}
	if (*s != ':' && --s >= buf) {
	    if (*s >= '0' && *s <= '9') {
		c1 = *s - '0';
	    } else if (*s >= 'a' && *s <= 'f') {
		c1 = *s - 'a' + 10;
	    } else if (*s >= 'A' && *s <= 'F') {
		c1 = *s - 'A' + 10;
	    } else if (*s == ':') {
		;
	    } else {
		return BCM_E_FAIL;
	    }
	}
	if (s > buf && s[-1] == ':') {
	    --s;
	}
	keyp->key[i] = c1 << 4 | c2;
    }
    return BCM_E_NONE;
}

/****************************************************************
 *
 * Internal functions
 */

/* Create an entry; assumes lock is held; */
STATIC cpudb_entry_t *
_cpudb_entry_create(cpudb_t *db_ref, const cpudb_key_t key)
{
    cpudb_entry_t *entry, *next, *prev;
    int h_idx;

    CPUDB_KEY_SEARCH(db_ref, key, entry);

    if (entry != NULL) {
        return entry;
    }

    entry = sal_alloc(sizeof(cpudb_entry_t), "cpudb_entry");
    if (entry == NULL) {
        return NULL;
    }

    sal_memset(entry, 0, sizeof(cpudb_entry_t));
    CPUDB_KEY_COPY(entry->base.key, key);

    /* Link into normal (sorted) list */
    prev = NULL;
    CPUDB_FOREACH_ENTRY(db_ref, next) {
	if (CPUDB_KEY_COMPARE(next->base.key, key) > 0) {
	    break;
	}
	prev = next;
    }
    entry->prev = prev;
    entry->next = next;
    if (entry->prev != NULL) {
	entry->prev->next = entry;
    } else {
	db_ref->entries = entry;
    }
    if (entry->next != NULL) {
	entry->next->prev = entry;
    }
	
    /* Link into hash list for key */
    h_idx = CPUDB_KEY_HASH(key);
    entry->h_prev = NULL;
    entry->h_next = db_ref->key_hash[h_idx];
    if (db_ref->key_hash[h_idx] != NULL) {
        db_ref->key_hash[h_idx]->h_prev = entry;
    }
    db_ref->key_hash[h_idx] = entry;

    entry->db_ref = db_ref;

    db_ref->num_cpus++;

    return entry;
}

/*
 * Unlink an entry from (doubly) linked lists (including key hash list)
 */

STATIC void
_cpudb_unlink_entry(cpudb_ref_t db_ref, cpudb_entry_t *entry)
{
    int h_idx;

    /* Unlink from hash entry list */
    h_idx = CPUDB_KEY_HASH(entry->base.key);
    if (entry->h_prev == NULL) {
        db_ref->key_hash[h_idx] = entry->h_next;
    } else {
        entry->h_prev->h_next = entry->h_next;
    }
    if (entry->h_next != NULL) {
        entry->h_next->h_prev = entry->h_prev;
    }

    /* Unlink entry from main list */
    if (entry->prev != NULL) {
        entry->prev->next = entry->next;
    } else {
        db_ref->entries = entry->next;
    }
    if (entry->next != NULL) {
        entry->next->prev = entry->prev;
    }
    db_ref->num_cpus--;
}

/* Create a duplicate of src in db_ref */
STATIC cpudb_entry_t *
_cpudb_entry_duplicate(cpudb_ref_t db_ref, const cpudb_entry_t *src,
                       int is_local, int is_master)
{
    cpudb_entry_t *entry;

    entry = _cpudb_entry_create(db_ref, src->base.key);
    if (entry == NULL) {
        return NULL;
    }

    if (is_local) {
        db_ref->local_entry = entry;
    }
    if (is_master) {
        db_ref->master_entry = entry;
    }

    sal_memcpy(&entry->base, &src->base, sizeof(cpudb_base_t));
    sal_memcpy(&entry->sp_info, &src->sp_info, sizeof(cpudb_sp_list_t));
    sal_memcpy(&entry->mod_ids, &src->mod_ids, sizeof(cpudb_mod_list_t));
    entry->flags = src->flags;
    entry->sysid = src->sysid;
    entry->tx_unit = src->tx_unit;
    entry->tx_port = src->tx_port;
    entry->dest_mod = src->dest_mod;
    entry->dest_port = src->dest_port;
    entry->user_cookie = src->user_cookie;
    entry->trans_ptr = src->trans_ptr;
    entry->topo_idx = src->topo_idx;

    return entry;
}

/*
 * Function:
 *      cpudb_copy
 * Purpose:
 *      Create a copy of a CPUDB and return a reference to it.
 * Parameters:
 *      db_ref     - which DB to copy
 * Returns:
 *      NULL if fails;
 *      Pointer to new DB clone otherwise
 *      Note that the TOPO COOKIE is NOT duplicated.  It stays
 *      NULL in the new DB since CPUDB doesn't know enough to
 *      duplicate it.
 */

cpudb_ref_t
cpudb_copy(const cpudb_ref_t src_db)
{
    cpudb_ref_t new_db;
    int error = FALSE;
    cpudb_entry_t *current, *entry;

    if (!DB_REF_VALID(src_db)) {
        return CPUDB_REF_NULL;
    }

    new_db = cpudb_create();
    if (new_db == CPUDB_REF_NULL) {
        return CPUDB_REF_NULL;
    }

    _CPUDB_LOCK;
    CPUDB_FOREACH_ENTRY(src_db, current) {
        entry = _cpudb_entry_duplicate(new_db, current,
                                       src_db->local_entry == current,
                                       src_db->master_entry == current);
        if (entry == NULL) {
            error = TRUE;
            break;
        }
    }
    _CPUDB_UNLOCK;

    if (error) {
        if (new_db != CPUDB_REF_NULL) {
            cpudb_destroy(new_db);
        }
        return CPUDB_REF_NULL;
    }

    return new_db;
}


/*
 * Function:
 *      cpudb_entry_copy
 * Purpose:
 *      Copy data from one CPUDB entry to another
 * Parameters:
 *      dest    -- Pointer to destination entry
 *      src     -- Pointer to source entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Copies the user cookie directly, so be careful if this is a
 *      pointer to a resource that could go away due to the original
 *      being destroyed.
 */

int
cpudb_entry_copy(cpudb_entry_t *dest, const cpudb_entry_t *src)
{
    if (dest == NULL || src == NULL) {
        return BCM_E_PARAM;
    }

    sal_memcpy(&dest->base, &src->base, sizeof(cpudb_base_t));
    sal_memcpy(&dest->sp_info, &src->sp_info, sizeof(cpudb_sp_list_t));
    sal_memcpy(&dest->mod_ids, &src->mod_ids, sizeof(cpudb_mod_list_t));
    dest->flags = src->flags;
    dest->sysid = src->sysid;
    dest->tx_unit = src->tx_unit;
    dest->tx_port = src->tx_port;
    dest->dest_mod = src->dest_mod;
    dest->dest_port = src->dest_port;
    dest->user_cookie = src->user_cookie;
    dest->trans_ptr = src->trans_ptr;
    dest->topo_idx = src->topo_idx;

    return BCM_E_NONE;
}

/*
 * Function:
 *      cpudb_sp_idx_to_slot
 * Purpose:
 *      Map a stack port index to the slot number and DB entry
 *      it connects to.
 * Parameters:
 *      db_ref  -- DB reference
 *      entry   -- source entry
 *      sp_idx  -- stack port index from source entry
 *      out_entry -- (OUT) Destination entry
 * Returns:
 *      The slot ID of the destination if found
 * Notes:
 *      Returns -1 if not found.
 *      Works for CFM and LM sources
 *      out_entry may be NULL
 *      Assumes sp_idx is valid for entry.
 */

int
cpudb_sp_idx_to_slot(const cpudb_ref_t db_ref, const cpudb_entry_t *entry,
                     int sp_idx, cpudb_entry_t **out_entry)
{
    cpudb_entry_t *db_ent;

    /* LM Source:  Search DB for CFM to which the SP connects */
    CPUDB_KEY_SEARCH(db_ref, entry->sp_info[sp_idx].tx_cpu_key, db_ent);
    if (db_ent == NULL) {
        return -1;
    }

    if (out_entry != NULL) {
        *out_entry = db_ent;
    }

    return db_ent->base.slot_id;
}

/*
 * Function:
 *      cpudb_key_lookup
 * Purpose:
 *      Search DB for key
 * Parameters:
 *      db_ref    - DB to search
 *      key       - key to search for
 * Returns:
 *      Pointer to entry if found; otherwise NULL
 */

cpudb_entry_t *
cpudb_key_lookup(cpudb_ref_t db_ref, const cpudb_key_t key)
{
    cpudb_entry_t *cur;

    CPUDB_CHECK(db_ref, NULL);
    _CPUDB_LOCK;
    CPUDB_KEY_SEARCH(db_ref, key, cur);
    _CPUDB_UNLOCK;
    return cur;
}


