/*! \file shr_cht.c
 *
 * Chained hash table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <shr/shr_cht.h>

uint32_t
shr_cht_hash(shr_cht_t *cht, void *entry)
{
    if (cht && cht->hash) {
        return cht->hash(cht->size, entry);
    }
    return 0;
}

void *
shr_cht_lookup(shr_cht_t *cht, void *entry)
{
    uint32_t idx, hash_val;

    /* Sanity check */
    if (cht == NULL || cht->match == NULL) {
        return NULL;
    }

    /* Get head of bucket list */
    hash_val = shr_cht_hash(cht, entry);
    idx = cht->tbl[hash_val].head;

    /* Search linked list of entries with identical hash values. */
    while (idx != 0) {
        if (cht->tbl[idx].entry && entry &&
            cht->match(cht->tbl[idx].entry, entry)) {
            return cht->tbl[idx].entry;
        }
        /* Next index in linked list */
        idx = cht->tbl[idx].list;
    }
    return NULL;
}

void *
shr_cht_insert(shr_cht_t *cht, void *entry)
{
    uint32_t hash_val;

    /* Sanity check */
    if (cht == NULL) {
        return NULL;
    }

    hash_val = shr_cht_hash(cht, entry);
    if (cht->free_idx < cht->size) {
        cht->tbl[cht->free_idx].list = cht->tbl[hash_val].head;
        cht->tbl[hash_val].head = cht->free_idx;
        cht->tbl[cht->free_idx].entry = entry;
        cht->free_idx++;
        return entry;
    }
    return NULL;
}

void
shr_cht_destroy(shr_cht_t *cht)
{
    if (cht) {
        sal_free(cht);
    }
}

shr_cht_t *
shr_cht_create(uint32_t size, shr_cht_hash_f hash, shr_cht_match_f match)
{
    shr_cht_t *cht;
    size_t cht_size;

    if (hash == NULL || match == NULL) {
        return NULL;
    }
    /*
     * The hash table uses 0 as a bucket list terminator, so we cannot
     * use table index 0. This means that the internal table must
     * contain one extra entry, and that the first usable index is 1
     * (see free_idx below).
     */
    size += 1;
    cht_size = sizeof(shr_cht_t) + size * sizeof(shr_cht_tbl_t);
    cht = sal_alloc(cht_size, "shrCht");
    if (cht == NULL) {
        return NULL;
    }
    sal_memset(cht, 0, cht_size);
    cht->size = size;
    cht->free_idx = 1;
    cht->hash = hash;
    cht->match = match;

    return cht;
}
