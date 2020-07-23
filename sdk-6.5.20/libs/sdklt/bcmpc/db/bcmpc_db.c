/*! \file bcmpc_db.c
 *
 * BCMPC Internal Database APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>

#include <shr/shr_error.h>
#include <bcmpc/bcmpc_db_internal.h>


/*! \brief To check whether two key arrays are identical. */
static int
db_keys_comp(size_t num_keys, uint32_t *dbe_keys, uint32_t *lookup)
{
    return (sal_memcmp(dbe_keys, lookup, sizeof(*dbe_keys) * num_keys) == 0);
}

bcmpc_db_entry_t *
bcmpc_db_entry_search(bcmpc_db_entry_t *table, size_t num_keys, uint32_t *keys,
                      bcmpc_db_entry_t **prev)
{
    bcmpc_db_entry_t *cur = table;

    if (prev) {
        *prev = NULL;
    }

    while (cur && (db_keys_comp(num_keys, cur->keys, keys) == 0)) {
        if (prev) {
            *prev = cur;
        }
        cur = cur->next;
    }

    return cur;
}

void
bcmpc_db_entry_add(bcmpc_db_entry_t **table, bcmpc_db_entry_t *new)
{
    bcmpc_db_entry_t *cur = *table;

    if (!cur) {
        *table = new;
        return;
    }

    while (cur->next) {
        cur = cur->next;
    }

    cur->next = new;
}

bcmpc_db_entry_t *
bcmpc_db_entry_del(bcmpc_db_entry_t **table, size_t num_keys, uint32_t *keys)
{
    bcmpc_db_entry_t *cur, *prev;

    cur = bcmpc_db_entry_search(*table, num_keys, keys, &prev);

    if (!cur) {
        return NULL;
    }

    if (!prev) {
        *table = cur->next;
    } else {
        prev->next = cur->next;
    }

    return cur;
}

bcmpc_db_entry_t *
bcmpc_db_entry_next(bcmpc_db_entry_t *table, bcmpc_db_entry_t *cur)
{
    if (!table) {
        return NULL;
    }

    if (!cur) {
        return table;
    }

    return cur->next;
}

int
bcmpc_db_entry_refcnt_incr(bcmpc_db_entry_t *table, size_t num_keys,
                           uint32_t *keys)
{
    bcmpc_db_entry_t *ent;

    ent = bcmpc_db_entry_search(table, num_keys, keys, NULL);

    if (!ent) {
        return -1;
    }

    ent->refcnt++;

    return 0;
}

int
bcmpc_db_entry_refcnt_decr(bcmpc_db_entry_t *table, size_t num_keys,
                           uint32_t *keys)
{
    bcmpc_db_entry_t *ent;

    ent = bcmpc_db_entry_search(table, num_keys, keys, NULL);

    if (!ent) {
        return -1;
    }

    ent->refcnt--;

    return (ent->refcnt < 0 ? ent->refcnt : 0);
}

int
bcmpc_db_entry_refcnt_get(bcmpc_db_entry_t *table, size_t num_keys,
                          uint32_t *keys)
{
    bcmpc_db_entry_t *ent;

    ent = bcmpc_db_entry_search(table, num_keys, keys, NULL);

    if (!ent) {
        return -1;
    }

    return ent->refcnt;
}

