/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Provides generic routines for managing HW profile tables.
 */

#include <soc/profile_mem.h>
#include <soc/error.h>
#include <soc/drv.h>

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif

/*
 * Function:
 *      soc_profile_mem_t_init
 * Purpose:
 *      Initialize a soc_profile_mem_t structure.
 *
 * Parameters:
 *      profile_mem - (IN) Pointer to profile memory structure
 * Returns:
 *      void
 */
void
soc_profile_mem_t_init(soc_profile_mem_t *profile)
{
    if (NULL != profile) {
        profile->tables = NULL;
        profile->table_count = 0;
        profile->flags = 0;
    }
}

STATIC void
_soc_profile_mem_free(soc_profile_mem_t *profile)
{
    soc_profile_mem_table_t *table;
    int table_index;

    if (profile->tables == NULL) {
        return;
    }

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        if (table->data_mask != NULL) {
            sal_free(table->data_mask);
            table->data_mask = NULL;
        }
        if (table->entries != NULL) {
            sal_free(table->entries);
            table->entries = NULL;
        }
        if (table->cache_p != NULL) {
            sal_free(table->cache_p);
            table->cache_p = NULL;
        }
    }
    sal_free(profile->tables);
    profile->tables = NULL;
}

/*
 * Function:
 *      soc_profile_mem_index_create
 * Purpose:
 *      Create a shadow copy and refcounts of a profile table.
 *      If called during WARM BOOT, the shadow copy is populated with
 *      the HW contents, otherwise, both the shadow copy and the
 *      HW entries are cleared.
 *
 * Parameters:
 *      unit              - (IN) Unit
 *      mem_array         - (IN) Pointer to memory id array
 *      entry_words_array - (IN) Pointer to entry size array
 *      index_min_array   - (IN) (optional) Pointer to index min array
 *                               default is soc_mem_index_min
 *      index_max_array   - (IN) (optional) Pointer to index max array
 *                               default is soc_mem_index_max
 *      data_mask_array   - (IN) (optional) Pointer to data mask array
 *                               default is to compare the entire entry
 *                               (include padding)
 *      table_count       - (IN) Number of entries in memory id array
 *      profile           - (IN) Pointer to profile memory structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_index_create(int unit,
                             soc_mem_t *mem_array,
                             int *entry_words_array,
                             int *index_min_array,
                             int *index_max_array,
                             void **data_mask_array,
                             int table_count,
                             soc_profile_mem_t *profile)
{
    soc_profile_mem_table_t *table;
    int rv;
    int alloc_size;
    int num_entries, table_index, i;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int pipe, mem_per_pipe;
    uint32 pipe_map;
#endif
    soc_mem_t mem;
    uint32 *data_mask_p, *cache_p;
    void *null_entry = NULL;

    if (profile == NULL) {
        return SOC_E_INIT;
    }

    if (mem_array == NULL || entry_words_array == NULL ||
        table_count == 0) {
        return SOC_E_PARAM;
    }

    if (profile->tables != NULL) {
        _soc_profile_mem_free(profile);
    }

    alloc_size = table_count * sizeof(soc_profile_mem_table_t);
    profile->tables = sal_alloc(alloc_size, "Profile Mem Tables");
    if (profile->tables == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(profile->tables, 0, alloc_size);
    profile->table_count = table_count;

    for (table_index = 0; table_index < table_count; table_index++) {
        table = &profile->tables[table_index];
        table->mem = mem_array[table_index];
        if (index_min_array == NULL) {
            table->index_min = soc_mem_index_min(unit, table->mem);
        } else {
            table->index_min = index_min_array[table_index];
        }
        if (index_max_array == NULL) {
            table->index_max = soc_mem_index_max(unit, table->mem);
        } else {
            table->index_max = index_max_array[table_index];
        }
        if (table->index_max < table->index_min) {
            _soc_profile_mem_free(profile);
            return SOC_E_PARAM;
        }
        table->entry_words = entry_words_array[table_index];

        if (data_mask_array != NULL && data_mask_array[table_index] != NULL) {
            alloc_size = table->entry_words * sizeof(uint32);
            table->data_mask = sal_alloc(alloc_size, "Profile Mem Data Mask");
            if (table->data_mask == NULL) {
                _soc_profile_mem_free(profile);
                return SOC_E_MEMORY;
            }
            sal_memset(table->data_mask, 0, alloc_size);
            data_mask_p = data_mask_array[table_index];
            for (i = 0; i < table->entry_words; i++) {
                table->data_mask[i] = data_mask_p[i];
            }
        }

        num_entries = table->index_max - table->index_min + 1;
        alloc_size = num_entries * sizeof(soc_profile_mem_entry_t);
        table->entries = sal_alloc(alloc_size, "Profile Mem Entries");
        if (table->entries == NULL) {
            _soc_profile_mem_free(profile);
            return SOC_E_MEMORY;
        }
        sal_memset(table->entries, 0, alloc_size);

        alloc_size = num_entries * table->entry_words * sizeof(uint32);
        table->cache_p = sal_alloc(alloc_size, "Profile Mem Cache");
        if (table->cache_p == NULL) {
            _soc_profile_mem_free(profile);
            return SOC_E_MEMORY;
        }
        sal_memset(table->cache_p, 0, alloc_size);
    }

    if (SOC_WARM_BOOT(unit)) {
        for (table_index = 0; table_index < profile->table_count;
             table_index++) {
            table = &profile->tables[table_index];
            num_entries = table->index_max - table->index_min + 1;
            for (i = 0; i < num_entries; i++) {
                cache_p = &table->cache_p[table->entry_words * i];
#ifdef BCM_TOMAHAWK3_SUPPORT
                if (SOC_IS_TOMAHAWK3(unit) &&
                        SOC_MEM_UNIQUE_ACC(unit, table->mem)) {

                    mem_per_pipe =
                        (table->index_max + 1) / _TH3_PIPES_PER_DEV;
                    pipe = i / mem_per_pipe;

                    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
                    /* skip if the pipe isn't valid (half chip) */
                    if ((pipe_map & (1 << pipe)) == 0) {
                        continue;
                    }

                    mem = SOC_MEM_UNIQUE_ACC(unit, table->mem)[pipe];
                    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                  table->index_min + i%mem_per_pipe, cache_p);
                } else
#endif
                {
                    mem = table->mem;
                    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                  table->index_min + i, cache_p);
                }
                if (rv < 0) {
                    _soc_profile_mem_free(profile);
                    return rv;
                }
            }
        }
    } else {
        /* Clear HW memory */
        for (table_index = 0; table_index < profile->table_count;
             table_index++) {
            table = &profile->tables[table_index];
#ifdef BCM_RCPU_SUPPORT
            if (SOC_IS_RCPU_ONLY(unit)) {
                /* 
                 * Avoid clearing below profile tables to make remote switch
                 * able to tx/rx RCPU packets.
                 */
                if (ING_VLAN_TAG_ACTION_PROFILEm == table->mem ||
                    EGR_VLAN_TAG_ACTION_PROFILEm == table->mem) {
                    return SOC_E_NONE;
                }
            }
#endif /* BCM_RCPU_SUPPORT */

            if (SOC_HW_RESET(unit) &&
                soc_mem_clearable_on_reset(unit, table->mem, COPYNO_ALL)){
                continue;
            }

            /* If the profile is setup as a non-default index_min/max, we will
               call 'soc_mem_array_fill_range' to clear the memory, else
               call the generic 'soc_mem_clear', this is done so that the
               additional capabilities provided within the generic call like
               'h/w acceleration' can be utilized in most cases. */
            if((table->index_min != soc_mem_index_min(unit, table->mem)) ||
               (table->index_max != soc_mem_index_max(unit, table->mem))) {

                /* Allocate a null-entry to use below */
                null_entry = soc_cm_salloc(unit,
                                           WORDS2BYTES(table->entry_words),
                                           "profile create mem clear");

                /* Bail on out of memory */
                if(null_entry == NULL) {
                    _soc_profile_mem_free(profile);
                    return SOC_E_MEMORY;
                }

                sal_memcpy(null_entry,
                           soc_mem_entry_null(unit, table->mem),
                           soc_mem_entry_words(unit, table->mem) * 
                           sizeof(uint32));

                rv = soc_mem_array_fill_range(unit,
                                              table->mem,
                                              0, /* min_ar_index - unused */
                                              0, /* max_ar_index - unused */
                                              COPYNO_ALL,
                                              table->index_min,
                                              table->index_max,
                                              null_entry);

                /* Free the allocated null_ptr memory */
                soc_cm_sfree(unit, null_entry);
                null_entry = NULL;
            } else {
                rv = soc_mem_clear(unit, table->mem, COPYNO_ALL, TRUE);
            }

            if (rv < 0) {
                _soc_profile_mem_free(profile);
                return rv;
            }
        }
    }

    return SOC_E_NONE;
}

int
soc_profile_mem_create(int unit,
                       soc_mem_t *mem_array,
                       int *entry_words_array,
                       int table_count,
                       soc_profile_mem_t *profile)
{
    return soc_profile_mem_index_create(unit, mem_array, entry_words_array,
                                        NULL, NULL, NULL, table_count,
                                        profile);
}

/*
 * Function:
 *      soc_profile_mem_destroy
 * Purpose:
 *      Destroy the shadow copy and refcounts of a profile table.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile_mem - (IN) Pointer to profile memory structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_destroy(int unit,
                        soc_profile_mem_t *profile)
{
    if (profile == NULL) {
        return SOC_E_PARAM;
    }

    _soc_profile_mem_free(profile);
    return SOC_E_NONE;
}

STATIC int
_soc_profile_mem_check(int unit, soc_profile_mem_t *profile,
                       int base0)                       
{
    soc_profile_mem_table_t *table;
    int num_entries, num_sets, entries_per_set, ref_count;
    int set, table_index, i, base;

    table = &profile->tables[0];
    entries_per_set = table->entries[base0].entries_per_set;

    if (profile->table_count == 1 && entries_per_set == 1) {
        return SOC_E_NONE;
    }

    num_entries = table->index_max - table->index_min + 1;
    num_sets = num_entries / entries_per_set;
    set = base0 / entries_per_set;

    ref_count = table->entries[base0].ref_count;
    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;
        for (i = 0; i < entries_per_set; i++) {
            if (table->entries[base + i].entries_per_set != entries_per_set ||
                table->entries[base + i].ref_count != ref_count) {
                return SOC_E_INTERNAL;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_mem_sw_state_set
 * Purpose:
 *      Add a set of entries (one or more entries) to a profile table. This
 *      routine searches for a matching set in the profile table. If a matching
 *      set is found, the ref count for that entry is incremented and
 *      its base index is returned. If a matching set is not found then then
 *      entry is updated at the incoming index (index0).
 *      No write to HW is done in this routine as this sets only SW state. 
 *
 * Parameters:
 *      unit             - (IN) Unit
 *      profile          - (IN) Pointer to profile memory structure
 *      entries_array    - (IN) Array of pointer to table entries set
 *      entries_per_set0 - (IN) Number of entries in the set for table 0
 *      index0           - (IN) Base index to the entries in HW for table 0
 *                                    
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_sw_state_set(int unit,
                             soc_profile_mem_t *profile,
                             void **entries_array,
                             int entries_per_set0,
                             uint32 index0)
{
    soc_profile_mem_table_t *table;
    int index_min, num_entries, num_sets, entries_per_set;
    int set, table_index, i, j, base, base0, free_set;
    int entry_words, data_words;
    int alloc_size, rv = SOC_E_NONE;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mask[SOC_MAX_MEM_WORDS];
    uint32 *cache_p, *entry_p, *mask_p, *range_p, *ent_p;

    if (profile == NULL || entries_array == NULL || entries_per_set0 <= 0) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;

    if (num_entries % entries_per_set0) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set0;

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        if (entries_array[table_index] == NULL) {
            return SOC_E_PARAM;
        }

        if (num_entries % num_sets) {
            return SOC_E_PARAM;
        }
    }

    sal_memset(mask, 0xff, sizeof(mask));

    /*
     * Search for an existing set that has the same configuration.
     */
    free_set = -1;
    for (set = 0; set < num_sets; set++) {
        base0 = set * entries_per_set0;

        /* Skip unused entries. */
        if (profile->tables[0].entries[base0].ref_count == 0) {
            if (free_set != -1) {
                continue;
            }

            /* Preserve location of free slot. */
            free_set = set;
            if (profile->table_count == 1 && entries_per_set0 == 1) {
                continue;
            }

            for (table_index = 0; table_index < profile->table_count;
                 table_index++) {
                table = &profile->tables[table_index];
                num_entries = table->index_max - table->index_min + 1;
                entries_per_set = num_entries / num_sets;
                base = set * entries_per_set;
                for (i = 0; i < entries_per_set; i++) {
                    if (table->entries[base + i].ref_count) {
                        free_set = -1;
                        break;
                    }
                }
                if (free_set == -1) {
                    break;
                }
            }
            /* If non-shared, break out of main loop on first empty entry */
            if ((profile->flags & SOC_PROFILE_MEM_F_NO_SHARE) &&
                free_set >= 0) {
                break;
            }
            continue;
        }

        /* If non-shared, continue looking for empty entry */
        if (profile->flags & SOC_PROFILE_MEM_F_NO_SHARE) {
            continue;
        }

        /* Skip set of different size */
        if (profile->tables[0].entries[base0].entries_per_set !=
            entries_per_set0) {
            continue;
        }

        /* Compare the new set of entries against the cache */
        for (table_index = 0; table_index < profile->table_count;
             table_index++) {
            table = &profile->tables[table_index];
            num_entries = table->index_max - table->index_min + 1;
            entries_per_set = num_entries / num_sets;
            base = set * entries_per_set;
            entry_words = table->entry_words;
            data_words = soc_mem_entry_words(unit, table->mem);
            entry_p = entries_array[table_index];
            cache_p = &table->cache_p[base * entry_words];
            mask_p = table->data_mask == NULL ? mask : table->data_mask;
            for (i = 0; i < entries_per_set; i++) {
                for (j = 0; j < data_words; j++) {
                    if ((cache_p[j] ^ entry_p[j]) & mask_p[j]) {
                        break;
                    }
                }
                if (j < data_words) {
                    break;
                }
                entry_p += entry_words;
                cache_p += entry_words;
            }
            if (i != entries_per_set) {
                break;
            }
        }
        if (table_index != profile->table_count) {
            continue;
        }

        /* Do optional data integrity check */
        SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));

        /* Matched set found */
        for (table_index = 0; table_index < profile->table_count;
             table_index++) {
            table = &profile->tables[table_index];
            num_entries = table->index_max - table->index_min + 1;
            entries_per_set = num_entries / num_sets;
            base = set * entries_per_set;
            for (i = 0; i < entries_per_set; i++) {
                table->entries[base + i].ref_count++;
            }
        }
        return SOC_E_NONE;
    }

    /* set the incoming index as free index so that entry is added at same place */
    free_set = index0;

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        index_min = table->index_min;
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = free_set * entries_per_set;
        entry_words = table->entry_words;
        data_words = soc_mem_entry_words(unit, table->mem);
        entry_p = entries_array[table_index];
        cache_p = &table->cache_p[base * entry_words];

        if (entries_per_set >= SOC_PROFILE_MEM_DMA_THRESHHOLD) {
            alloc_size =  WORDS2BYTES(entry_words) * entries_per_set;
            range_p = soc_cm_salloc(unit, alloc_size, "profile update");
            if (NULL == range_p) {
                return SOC_E_MEMORY;
            }
            sal_memset((void *)range_p, 0, alloc_size);

            if (table->data_mask != NULL) {
                /* Read full entry from HW when profile only covers
                 * part of the table. */
                rv = soc_mem_read_range(unit, table->mem, MEM_BLOCK_ANY,
                                        index_min + base,
                                        index_min + base + entries_per_set - 1,
                                        range_p);
            }

            if (SOC_SUCCESS(rv)) {
                for (i = 0; i < entries_per_set; i++) {
                    ent_p =
                        soc_mem_table_idx_to_pointer(unit, table->mem,
                                                     uint32 *, range_p, i);

                    if (table->data_mask != NULL) {
                        /* Replace partial entry with new content */
                        for (j = 0; j < data_words; j++) {
                            ent_p[j] &= ~table->data_mask[j];
                            ent_p[j] |= entry_p[j] & table->data_mask[j];
                        }
                    } else {
                        /* Write entire new entry */
                        sal_memcpy(ent_p, entry_p,
                                   data_words * sizeof(uint32));
                    }
                    entry_p += entry_words;
                }
            }

            if (SOC_SUCCESS(rv)) {
                /* Copy entry into the software cache. */
                entry_p = entries_array[table_index];

                for (i = 0; i < entries_per_set; i++) {
                    sal_memcpy(cache_p, entry_p,
                               data_words * sizeof(uint32));
                    entry_p += entry_words;
                    cache_p += entry_words;

                    table->entries[base + i].ref_count++;
                    table->entries[base + i].entries_per_set =
                        entries_per_set;
                }
            }

            soc_cm_sfree(unit, range_p);
            if (SOC_FAILURE(rv)) {
                return rv;
            }            
        } else {
            for (i = 0; i < entries_per_set; i++) {
                if (table->data_mask != NULL) {
                    /* Read original entry from hardware profile table */
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, table->mem, MEM_BLOCK_ANY,
                                      index_min + base + i, entry));

                    /* Replace partial entry with new content */
                    for (j = 0; j < data_words; j++) {
                        entry[j] &= ~table->data_mask[j];
                        entry[j] |= entry_p[j] & table->data_mask[j];
                    }

                }

                /* Copy entry into the software cache. */
                sal_memcpy(cache_p, entry_p, data_words * sizeof(uint32));
                entry_p += entry_words;
                cache_p += entry_words;

                table->entries[base + i].ref_count++;
                table->entries[base + i].entries_per_set = entries_per_set;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_mem_add_unique
 * Purpose:
 *      Add a set of entries (one or more entries) to a profile table. This
 *      routine searches for a matching set in the profile table. If a matching
 *      set is found, the ref count for that entry is incremented and
 *      its base index is returned. If a matching set is not found and a free
 *      set is found, the HW table is updated, the ref count is incremented,
 *      and the base index of the set is returned. If no free set is found, an
 *      error is returned.
 *
 *      This is for when memory is split and UNIQUE per pipe
 *
 *
 * Parameters:
 *      unit             - (IN) Unit
 *      profile          - (IN) Pointer to profile memory structure
 *      entries_array    - (IN) Array of pointer to table entries set
 *      entries_per_set0 - (IN) Number of entries in the set for table 0
 *      pipe             - (IN) Pipe to affect
 *      index0           - (OUT) Base index to the entries in HW for table 0
 *
 * Returns:
 *      SOC_E_XXX
 *
 * Notes:
 * For example
 * Usually a profile structure is formed by single memory table. However if a
 * profile structure is formed by the combination of 2 tables (table1 and
 * table2 in this example). Each entry in table1 is 2 words long, and each
 * entry in table2 is 3 words long. Argument entries_per_set is uniform on all
 * tables (4 in this example).
 *   +------------------+     +--------------------------+
 *   | entries_array[0] |---> |  table1[0], 2 words long |
 *   +------------------+     +--------------------------+
 *   | entries_array[1] |-+   |  table1[1]               |
 *   +------------------+ |   +--------------------------+
 *                        |   |  table1[2]               |
 *                        |   +--------------------------+
 *                        |   |  table1[3]               |
 *                        |   +--------------------------+
 *                        |   +---------------------------------+
 *                        +-> |  table2[0], 3 words long        |
 *                            +---------------------------------+
 *                            |  table2[1]                      |
 *                            +---------------------------------+
 *                            |  table2[2]                      |
 *                            +---------------------------------+
 *                            |  table2[3]                      |
 *                            +---------------------------------+
 *
 * The code for above example may look like:
 * {
 *     void *entries[2];
 *     table1_entry_t table1[4];
 *     table2_entry_t table2[4];
 *     int entries_per_set0;
 *     uint32 index[2];
 *
 *     fill table1[0], table1[1], table1[2], table1[3]
 *     fill table2[0], table2[1], table2[2], table2[3]
 *     entries[0] = &table1;
 *     entries[1] = &table2;
 *     entries_per_set0 = 4;
 *     soc_profile_mem_add_unique(unit, profile_mem, &entries, entries_per_set0,
 *                         pipe, index);
 * }
 */
int
soc_profile_mem_add_unique(int unit,
                    soc_profile_mem_t *profile,
                    void **entries_array,
                    int entries_per_set0,
                    int pipe,
                    uint32 *index0)
{
    soc_profile_mem_table_t *table;
    int index_min, num_entries, num_sets, entries_per_set;
    int set, table_index, i, j, base, base0, free_set;
    int entry_words, data_words;
    int alloc_size, rv = SOC_E_NONE;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mask[SOC_MAX_MEM_WORDS];
    uint32 *cache_p, *entry_p, *mask_p, *range_p, *ent_p;
    soc_mem_t mem = INVALIDm;
    int mod, entries_per_pipe;

    int set_start, set_end;

    /* COVERITY: Intentional, stack use of 5192 bytes */
    /* coverity[stack_use_callee_max : FALSE] */
    /* coverity[stack_use_overflow : FALSE] */
    /* coverity[stack_use_return : FALSE] */

    if (profile == NULL || entries_array == NULL || entries_per_set0 <= 0 ||
        index0 == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;
    
    if (NULL == SOC_MEM_UNIQUE_ACC(unit, table->mem)) {
        return SOC_E_PARAM;
    }

    if (num_entries % entries_per_set0) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set0;

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        if (entries_array[table_index] == NULL) {
            return SOC_E_PARAM;
        }

        if (num_entries % num_sets) {
            return SOC_E_PARAM;
        }
    }

    sal_memset(mask, 0xff, sizeof(mask));

    /*
     * Search for an existing set that has the same configuration.
     */

    /* Start searching at the entries at the start of the pipe.
     * Example: If pipe 1 was passed in, start the search at the
     * the pipe 1 section of the profile table, which follows
     * the pipe 0 section of the profile table.
     *
     *       UNIQUE MEM          DUPLICATE MEM
     *      -----------          ----------
     *     0|PIPE0 MEM|         0|MEM     |
     *      |PIPE1 MEM|          |        |
     *      |PIPE2 MEM|          |        |
     *      |...      |          |...     |
     *   159|PIPE7 MEM|       159|        |
     *      -----------          ----------
 */
    entries_per_pipe = SOC_MEM_SIZE(unit, profile->tables[0].mem);
    set_start = pipe * entries_per_pipe;
    set_end =   (pipe + 1) * entries_per_pipe;

    free_set = -1;
    for (set = set_start; set < set_end; set++) {
        base0 = set * entries_per_set0;

        /* Skip unused entries. */
        if (profile->tables[0].entries[base0].ref_count == 0) {
            if (free_set != -1) {
                continue;
            }

            /* Preserve location of free slot. */
            free_set = set;
            if (profile->table_count == 1 && entries_per_set0 == 1) {
                continue;
            }

            for (table_index = 0; table_index < profile->table_count;
                 table_index++) {
                table = &profile->tables[table_index];
                num_entries = table->index_max - table->index_min + 1;
                entries_per_set = num_entries / num_sets;
                base = set * entries_per_set;
                for (i = 0; i < entries_per_set; i++) {
                    if (table->entries[base + i].ref_count) {
                        free_set = -1;
                        break;
                    }
                }
                if (free_set == -1) {
                    break;
                }
            }
            /* If non-shared, break out of main loop on first empty entry */
            if ((profile->flags & SOC_PROFILE_MEM_F_NO_SHARE) &&
                free_set >= 0) {
                break;
            }
            continue;
        }

        /* If non-shared, continue looking for empty entry */
        if (profile->flags & SOC_PROFILE_MEM_F_NO_SHARE) {
            continue;
        }

        /* Skip set of different size */
        if (profile->tables[0].entries[base0].entries_per_set !=
            entries_per_set0) {
            continue;
        }

        /* Compare the new set of entries against the cache */
        for (table_index = 0; table_index < profile->table_count;
             table_index++) {
            table = &profile->tables[table_index];
            num_entries = table->index_max - table->index_min + 1;
            entries_per_set = num_entries / num_sets;
            base = set * entries_per_set;
            entry_words = table->entry_words;
            data_words = soc_mem_entry_words(unit, table->mem);
            entry_p = entries_array[table_index];
            cache_p = &table->cache_p[base * entry_words];
            mask_p = table->data_mask == NULL ? mask : table->data_mask;
            for (i = 0; i < entries_per_set; i++) {
                for (j = 0; j < data_words; j++) {
                    if ((cache_p[j] ^ entry_p[j]) & mask_p[j]) {
                        break;
                    }
                }
                if (j < data_words) {
                    break;
                }
                entry_p += entry_words;
                cache_p += entry_words;
            }
            if (i != entries_per_set) {
                break;
            }
        }
        if (table_index != profile->table_count) {
            continue;
        }

        /* Do optional data integrity check */
        SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));

        /* Matched set found */
        for (table_index = 0; table_index < profile->table_count;
             table_index++) {
            table = &profile->tables[table_index];
            num_entries = table->index_max - table->index_min + 1;
            entries_per_set = num_entries / num_sets;
            base = set * entries_per_set;
            for (i = 0; i < entries_per_set; i++) {
                table->entries[base + i].ref_count++;
            }
        }

        /* Return the index based on the pipe, not on the profile table */
        *index0 = (base0%entries_per_pipe) + profile->tables[0].index_min;

        return SOC_E_NONE;
    }

    if (free_set == -1) {
        return SOC_E_RESOURCE;
    }

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        index_min = table->index_min;
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = free_set * entries_per_set;
        entry_words = table->entry_words;
        data_words = soc_mem_entry_words(unit, table->mem);
        entry_p = entries_array[table_index];
        cache_p = &table->cache_p[base * entry_words];

        if (entries_per_set >= SOC_PROFILE_MEM_DMA_THRESHHOLD) {
            alloc_size =  WORDS2BYTES(entry_words) * entries_per_set;
            range_p = soc_cm_salloc(unit, alloc_size, "profile update");
            if (NULL == range_p) {
                return SOC_E_MEMORY;
            }

            sal_memset((void *)range_p, 0, alloc_size);

            mem = table->mem;
            entries_per_set = num_entries / num_sets;
            base = free_set * entries_per_set;

            /* Unique Memories in the profile memory table need to
             * be written to hardware per pipe, while non
             * unique memories do not */
            if (SOC_MEM_UNIQUE_ACC(unit, mem)) {
                mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
                mod = entries_per_pipe;
            } else {
                mod = 1;
            }

            if (table->data_mask != NULL) {
                /* Read full entry from HW when profile only covers
                 * part of the table. */

                /* This reads from one pipe at a time */
                rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                        index_min + base%mod,
                                        index_min + base%mod + entries_per_set - 1,
                                        range_p);
            }

            if (SOC_SUCCESS(rv)) {
                for (i = 0; i < entries_per_set; i++) {
                    ent_p =
                        soc_mem_table_idx_to_pointer(unit, mem,
                                                     uint32 *, range_p, i);

                    if (table->data_mask != NULL) {
                        /* Replace partial entry with new content */
                        for (j = 0; j < data_words; j++) {
                            ent_p[j] &= ~table->data_mask[j];
                            ent_p[j] |= entry_p[j] & table->data_mask[j];
                        }
                    } else {
                        /* Write entire new entry */
                        sal_memcpy(ent_p, entry_p,
                                   data_words * sizeof(uint32));
                    }
                    entry_p += entry_words;
                }
            }

            if (SOC_SUCCESS(rv)) {
                /* Write back the modified entries */
                /* This writes to one pipe at a time */
                rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                                         index_min + base%mod,
                                         index_min + base%mod + entries_per_set - 1,
                                         range_p);
            }

            if (SOC_SUCCESS(rv)) {
                /* Copy entry into the software cache. */
                entry_p = entries_array[table_index];

                /* Restore base for writing into contiguous mem */
                base = free_set * entries_per_set;

                for (i = 0; i < entries_per_set; i++) {
                    sal_memcpy(cache_p, entry_p,
                               data_words * sizeof(uint32));
                    entry_p += entry_words;
                    cache_p += entry_words;

                    table->entries[base + i].ref_count++;
                    table->entries[base + i].entries_per_set =
                        entries_per_set;
                }
            }

            soc_cm_sfree(unit, range_p);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
        } else {
            for (i = 0; i < entries_per_set; i++) {
                mem = table->mem;
                base = free_set * entries_per_set;
                if (SOC_MEM_UNIQUE_ACC(unit, mem)) {
                    mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
                    mod = entries_per_pipe;
                } else {
                    mod = 1;
                }
                if (table->data_mask != NULL) {
                    /* Read original entry from hardware profile table */
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                      index_min + base%mod + i, entry));

                    /* Replace partial entry with new content */
                    for (j = 0; j < data_words; j++) {
                        entry[j] &= ~table->data_mask[j];
                        entry[j] |= entry_p[j] & table->data_mask[j];
                    }

                    /* Write modified entry into hardware profile table */
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                       index_min + base%mod + i, entry));
                } else {
                    /* Write entire new entry into hardware profile table */
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                       index_min + base%mod + i, entry_p));
                }

                /* Copy entry into the software cache. */
                sal_memcpy(cache_p, entry_p, data_words * sizeof(uint32));
                entry_p += entry_words;
                cache_p += entry_words;

                table->entries[base + i].ref_count++;
                table->entries[base + i].entries_per_set = entries_per_set;
            }
        }
    }

    /* Return the index based on the pipe, not on the profile table */
    *index0 = (free_set%entries_per_pipe) * entries_per_set0 + profile->tables[0].index_min;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_mem_add
 * Purpose:
 *      Add a set of entries (one or more entries) to a profile table. This
 *      routine searches for a matching set in the profile table. If a matching
 *      set is found, the ref count for that entry is incremented and
 *      its base index is returned. If a matching set is not found and a free
 *      set is found, the HW table is updated, the ref count is incremented,
 *      and the base index of the set is returned. If no free set is found, an
 *      error is returned
 *
 * Parameters:
 *      unit             - (IN) Unit
 *      profile          - (IN) Pointer to profile memory structure
 *      entries_array    - (IN) Array of pointer to table entries set
 *      entries_per_set0 - (IN) Number of entries in the set for table 0
 *      index0           - (OUT) Base index to the entries in HW for table 0
 *                                    
 * Returns:
 *      SOC_E_XXX
 *
 * Notes:
 * For example
 * Usually a profile structure is formed by single memory table. However if a
 * profile structure is formed by the combination of 2 tables (table1 and
 * table2 in this example). Each entry in table1 is 2 words long, and each
 * entry in table2 is 3 words long. Argument entries_per_set is uniform on all
 * tables (4 in this example).
 *   +------------------+     +--------------------------+
 *   | entries_array[0] |---> |  table1[0], 2 words long |
 *   +------------------+     +--------------------------+
 *   | entries_array[1] |-+   |  table1[1]               |
 *   +------------------+ |   +--------------------------+
 *                        |   |  table1[2]               |
 *                        |   +--------------------------+
 *                        |   |  table1[3]               |
 *                        |   +--------------------------+
 *                        |   +---------------------------------+
 *                        +-> |  table2[0], 3 words long        |
 *                            +---------------------------------+
 *                            |  table2[1]                      |
 *                            +---------------------------------+
 *                            |  table2[2]                      |
 *                            +---------------------------------+
 *                            |  table2[3]                      |
 *                            +---------------------------------+
 *
 * The code for above example may look like:
 * {
 *     void *entries[2];
 *     table1_entry_t table1[4];
 *     table2_entry_t table2[4];
 *     int entries_per_set0;
 *     uint32 index[2];
 *
 *     fill table1[0], table1[1], table1[2], table1[3]
 *     fill table2[0], table2[1], table2[2], table2[3]
 *     entries[0] = &table1;
 *     entries[1] = &table2;
 *     entries_per_set0 = 4;
 *     soc_profile_mem_add(unit, profile_mem, &entries, entries_per_set0,
 *                         index);
 * }
 */
int
soc_profile_mem_add(int unit,
                    soc_profile_mem_t *profile,
                    void **entries_array,
                    int entries_per_set0,
                    uint32 *index0)
{
    soc_profile_mem_table_t *table;
    int index_min, num_entries, num_sets, entries_per_set;
    int set, table_index, i, j, base, base0, free_set;
    int entry_words, data_words;
    int alloc_size, rv = SOC_E_NONE;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 mask[SOC_MAX_MEM_WORDS];
    uint32 *cache_p, *entry_p, *mask_p, *range_p, *ent_p;

    /* COVERITY: Intentional, stack use of 5192 bytes */
    /* coverity[stack_use_callee_max : FALSE] */
    /* coverity[stack_use_overflow : FALSE] */
    /* coverity[stack_use_return : FALSE] */

    if (profile == NULL || entries_array == NULL || entries_per_set0 <= 0 ||
        index0 == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;

    if (num_entries % entries_per_set0) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set0;

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        if (entries_array[table_index] == NULL) {
            return SOC_E_PARAM;
        }

        if (num_entries % num_sets) {
            return SOC_E_PARAM;
        }
    }

    sal_memset(mask, 0xff, sizeof(mask));

    /*
     * Search for an existing set that has the same configuration.
     */
    free_set = -1;
    for (set = 0; set < num_sets; set++) {
        base0 = set * entries_per_set0;

        /* Skip unused entries. */
        if (profile->tables[0].entries[base0].ref_count == 0) {
            if (free_set != -1) {
                continue;
            }

            /* Preserve location of free slot. */
            free_set = set;
            if (profile->table_count == 1 && entries_per_set0 == 1) {
                continue;
            }

            for (table_index = 0; table_index < profile->table_count;
                 table_index++) {
                table = &profile->tables[table_index];
                num_entries = table->index_max - table->index_min + 1;
                entries_per_set = num_entries / num_sets;
                base = set * entries_per_set;
                for (i = 0; i < entries_per_set; i++) {
                    if (table->entries[base + i].ref_count) {
                        free_set = -1;
                        break;
                    }
                }
                if (free_set == -1) {
                    break;
                }
            }
            /* If non-shared, break out of main loop on first empty entry */
            if ((profile->flags & SOC_PROFILE_MEM_F_NO_SHARE) &&
                free_set >= 0) {
                break;
            }
            continue;
        }

        /* If non-shared, continue looking for empty entry */
        if (profile->flags & SOC_PROFILE_MEM_F_NO_SHARE) {
            continue;
        }

        /* Skip set of different size */
        if (profile->tables[0].entries[base0].entries_per_set !=
            entries_per_set0) {
            continue;
        }

        /* Compare the new set of entries against the cache */
        for (table_index = 0; table_index < profile->table_count;
             table_index++) {
            table = &profile->tables[table_index];
            num_entries = table->index_max - table->index_min + 1;
            entries_per_set = num_entries / num_sets;
            base = set * entries_per_set;
            entry_words = table->entry_words;
            data_words = soc_mem_entry_words(unit, table->mem);
            entry_p = entries_array[table_index];
            cache_p = &table->cache_p[base * entry_words];
            mask_p = table->data_mask == NULL ? mask : table->data_mask;
            for (i = 0; i < entries_per_set; i++) {
                for (j = 0; j < data_words; j++) {
                    if ((cache_p[j] ^ entry_p[j]) & mask_p[j]) {
                        break;
                    }
                }
                if (j < data_words) {
                    break;
                }
                entry_p += entry_words;
                cache_p += entry_words;
            }
            if (i != entries_per_set) {
                break;
            }
        }
        if (table_index != profile->table_count) {
            continue;
        }

        /* Do optional data integrity check */
        SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));

        /* Matched set found */
        for (table_index = 0; table_index < profile->table_count;
             table_index++) {
            table = &profile->tables[table_index];
            num_entries = table->index_max - table->index_min + 1;
            entries_per_set = num_entries / num_sets;
            base = set * entries_per_set;
            for (i = 0; i < entries_per_set; i++) {
                table->entries[base + i].ref_count++;
            }
        }
        *index0 = base0 + profile->tables[0].index_min;

        return SOC_E_NONE;
    }

    if (free_set == -1) {
        return SOC_E_RESOURCE;
    }

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        index_min = table->index_min;
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = free_set * entries_per_set;
        entry_words = table->entry_words;
        data_words = soc_mem_entry_words(unit, table->mem);
        entry_p = entries_array[table_index];
        cache_p = &table->cache_p[base * entry_words];

        if (entries_per_set >= SOC_PROFILE_MEM_DMA_THRESHHOLD) {
            alloc_size =  WORDS2BYTES(entry_words) * entries_per_set;
            range_p = soc_cm_salloc(unit, alloc_size, "profile update");
            if (NULL == range_p) {
                return SOC_E_MEMORY;
            }
            sal_memset((void *)range_p, 0, alloc_size);

            if (table->data_mask != NULL) {
                /* Read full entry from HW when profile only covers
                 * part of the table. */
                rv = soc_mem_read_range(unit, table->mem, MEM_BLOCK_ANY,
                                        index_min + base,
                                        index_min + base + entries_per_set - 1,
                                        range_p);
            }

            if (SOC_SUCCESS(rv)) {
                for (i = 0; i < entries_per_set; i++) {
                    ent_p =
                        soc_mem_table_idx_to_pointer(unit, table->mem,
                                                     uint32 *, range_p, i);

                    if (table->data_mask != NULL) {
                        /* Replace partial entry with new content */
                        for (j = 0; j < data_words; j++) {
                            ent_p[j] &= ~table->data_mask[j];
                            ent_p[j] |= entry_p[j] & table->data_mask[j];
                        }
                    } else {
                        /* Write entire new entry */
                        sal_memcpy(ent_p, entry_p,
                                   data_words * sizeof(uint32));
                    }
                    entry_p += entry_words;
                }
            }

            if (SOC_SUCCESS(rv)) {
                /* Write back the modified entries */
                rv = soc_mem_write_range(unit, table->mem, MEM_BLOCK_ALL,
                                         index_min + base,
                                         index_min + base + entries_per_set - 1,
                                         range_p);
            }

            if (SOC_SUCCESS(rv)) {
                /* Copy entry into the software cache. */
                entry_p = entries_array[table_index];

                for (i = 0; i < entries_per_set; i++) {
                    sal_memcpy(cache_p, entry_p,
                               data_words * sizeof(uint32));
                    entry_p += entry_words;
                    cache_p += entry_words;

                    table->entries[base + i].ref_count++;
                    table->entries[base + i].entries_per_set =
                        entries_per_set;
                }
            }

            soc_cm_sfree(unit, range_p);
            if (SOC_FAILURE(rv)) {
                return rv;
            }            
        } else {
            for (i = 0; i < entries_per_set; i++) {
                if (table->data_mask != NULL) {
                    /* Read original entry from hardware profile table */
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, table->mem, MEM_BLOCK_ANY,
                                      index_min + base + i, entry));

                    /* Replace partial entry with new content */
                    for (j = 0; j < data_words; j++) {
                        entry[j] &= ~table->data_mask[j];
                        entry[j] |= entry_p[j] & table->data_mask[j];
                    }

                    /* Write modified entry into hardware profile table */
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, table->mem, MEM_BLOCK_ALL,
                                       index_min + base + i, entry));
                } else {
                    /* Write entire new entry into hardware profile table */
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, table->mem, MEM_BLOCK_ALL,
                                       index_min + base + i, entry_p));
                }

                /* Copy entry into the software cache. */
                sal_memcpy(cache_p, entry_p, data_words * sizeof(uint32));
                entry_p += entry_words;
                cache_p += entry_words;

                table->entries[base + i].ref_count++;
                table->entries[base + i].entries_per_set = entries_per_set;
            }
        }
    }
    *index0 = free_set * entries_per_set0 + profile->tables[0].index_min;

    return SOC_E_NONE;
}

int 
soc_profile_mem_search (int unit,
                         soc_profile_mem_t *profile,
                         void ** entries_array,
                         int entries_per_set0,
                          uint32 *index0)
{
    soc_profile_mem_table_t *table;
    int num_entries, num_sets, entries_per_set;
    int set, table_index, i, j, base, base0;
    int entry_words, data_words;
    int rv = SOC_E_NONE;
    uint32 mask[SOC_MAX_MEM_WORDS];
    uint32 *cache_p, *entry_p, *mask_p;

    if (profile == NULL || entries_array == NULL || entries_per_set0 <= 0 ||
            index0 == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    sal_memset(mask, 0xff, sizeof(mask));
    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;

    if (num_entries % entries_per_set0) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set0;
    for (set = 0; set < num_sets; set++) {
        base0 = set * entries_per_set0;

        if (profile->tables[0].entries[base0].ref_count == 0) {
            continue;
        }
        for (table_index = 0; table_index < profile->table_count;
                table_index++) {
            table = &profile->tables[table_index];
            num_entries = table->index_max - table->index_min + 1;
            entries_per_set = num_entries / num_sets;
            base = set * entries_per_set;
            entry_words = table->entry_words;
            data_words = soc_mem_entry_words(unit, table->mem);
            entry_p = entries_array[table_index];
            cache_p = &table->cache_p[base * entry_words];
            mask_p = table->data_mask == NULL ? mask : table->data_mask;
            for (i = 0; i < entries_per_set; i++) {
                for (j = 0; j < data_words; j++) {
                    if ((cache_p[j] ^ entry_p[j]) & mask_p[j]) {
                        break;
                    }
                }
                if (j < data_words) {
                    break;
                }
                entry_p += entry_words;
                cache_p += entry_words;
            }
            if (i != entries_per_set) {
                break;
            }

        }
        if (table_index != profile->table_count) {
             continue;
        }

        *index0 = (base0 + profile->tables[0].index_min)/entries_per_set0;
        return SOC_E_EXISTS;
    }
    return rv;
}

int
soc_profile_mem_single_table_add(int unit,
                                 soc_profile_mem_t *profile,
                                 void *entries,
                                 int entries_per_set,
                                 int *index)
{
    void *entries_array[1];

    entries_array[0] = entries;
    return soc_profile_mem_add(unit, profile, entries_array,
                               entries_per_set, (uint32 *)index);
}

/*
 * Function:
 *      soc_profile_mem_delete_unique
 * Purpose:
 *      Delete the reference to the set of entries (one or more entries) at
 *      the specified base index of the specified pipe
 *
 *      This is for when memory is split and UNIQUE per pipe
 *
 * Parameters:
 *      unit    - (IN) Unit
 *      profile - (IN) Pointer to profile memory structure
 *      index0  - (IN) Base index to the entries in HW for table 0
 *      pipe    - (IN) Pipe to affect
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_delete_unique(int unit,
                       soc_profile_mem_t *profile,
                       uint32 index0, int pipe)
{
    soc_profile_mem_table_t *table;
    int index_min, num_entries, num_sets, entries_per_set;
    int set, table_index, i, j, base, base0;
    int alloc_size, rv = SOC_E_NONE;
    int entry_words, data_words;
    uint32 *range_p, *ent_p;
    void *null_entry;
    soc_mem_t mem = INVALIDm;
    int mod, entries_per_pipe;

    if (profile == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    if (NULL == SOC_MEM_UNIQUE_ACC(unit, table->mem)) {
        return SOC_E_PARAM;
    }

    entries_per_pipe = SOC_MEM_SIZE(unit, table->mem);

    base0 = index0 - table->index_min;
    /* Offset the base to the appropriate pipe */
    base0 += pipe*entries_per_pipe;

    if (table->entries[base0].ref_count == 0) {
        return SOC_E_NOT_FOUND;
    }

    entries_per_set = table->entries[base0].entries_per_set;

    if (base0 % entries_per_set) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set;
    set = base0 / entries_per_set;

    /* Do optional data integrity check */
    SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;
        for (i = 0; i < entries_per_set; i++) {
            table->entries[base + i].ref_count--;
        }
    }

    if (profile->tables[0].entries[base0].ref_count != 0) {
        return SOC_E_NONE;
    }

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        index_min = table->index_min;
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;

        if (entries_per_set >= SOC_PROFILE_MEM_DMA_THRESHHOLD) {
            entry_words = table->entry_words;
            alloc_size =  WORDS2BYTES(entry_words) * entries_per_set;
            range_p = soc_cm_salloc(unit, alloc_size, "profile update");
            if (NULL == range_p) {
                return SOC_E_MEMORY;
            }

            sal_memset((void *)range_p, 0, alloc_size);
            mem = table->mem;
            entries_per_set = num_entries / num_sets;
            base = set * entries_per_set;

            /* If the memory is unique, need to write to its proper index
             * by removing the pipe offset */
            if (SOC_MEM_UNIQUE_ACC(unit, mem)) {
                mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
                mod = entries_per_pipe;
            } else {
                mod = 1;
            }

            if (table->data_mask != NULL) {
                /* Read full entry from HW when profile only covers
                 * part of the table. */
                rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                        index_min + base%mod,
                                        index_min + base%mod + entries_per_set - 1,
                                        range_p);
            }

            if (SOC_SUCCESS(rv)) {
                null_entry = soc_mem_entry_null(unit, mem);
                data_words = soc_mem_entry_words(unit, mem);
                for (i = 0; i < entries_per_set; i++) {
                    ent_p =
                        soc_mem_table_idx_to_pointer(unit, mem,
                                                     uint32 *, range_p, i);

                    if (table->data_mask != NULL) {
                        /* Clear partial entry */
                        for (j = 0; j < data_words; j++) {
                            ent_p[j] &= ~table->data_mask[j];
                        }
                    } else {
                        /* Write entire new entry */
                        sal_memcpy(ent_p, null_entry,
                                   data_words * sizeof(uint32));
                    }
                }
            }

            if (SOC_SUCCESS(rv)) {
                /* Write back the modified entries */
                rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY,
                                         index_min + base%mod,
                                         index_min + base%mod + entries_per_set - 1,
                                         range_p);
            }

            soc_cm_sfree(unit, range_p);
            if (SOC_FAILURE(rv)) {
                return rv;
            }
        } else {
            /* FIXME:  We don't check for the data_mask != 0 case here
             * because the only extant example is handled above.  If
             * we have new cases that fall into this section, additional
             * code must be added.
             */
            mem = table->mem;
            if (SOC_MEM_UNIQUE_ACC(unit, mem)) {
                mem = SOC_MEM_UNIQUE_ACC(unit, mem)[pipe];
                mod = entries_per_pipe;
            } else {
                mod = 1;
            }

            for (i = 0; i < entries_per_set; i++) {
                /* Insert the new entries into profile table */
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, table->mem, MEM_BLOCK_ANY,
                                   index_min + base%mod + i,
                                   soc_mem_entry_null(unit, table->mem)));
            }
        }
    }

    return SOC_E_NONE;
}
/*
 * Function:
 *      soc_profile_mem_delete
 * Purpose:
 *      Delete the reference to the set of entries (one or more entries) at
 *      the specified base index.
 *
 * Parameters:
 *      unit    - (IN) Unit
 *      profile - (IN) Pointer to profile memory structure
 *      index0  - (IN) Base index to the entries in HW for table 0
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_delete(int unit,
                       soc_profile_mem_t *profile,
                       uint32 index0)
{
    soc_profile_mem_table_t *table;
    int index_min, num_entries, num_sets, entries_per_set;
    int set, table_index, i, j, base, base0;
    int alloc_size, rv = SOC_E_NONE;
    int entry_words, data_words;
    uint32 *range_p, *ent_p;
    void *null_entry;

    if (profile == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    base0 = index0 - table->index_min;

    if (table->entries[base0].ref_count == 0) {
        return SOC_E_NOT_FOUND;
    }

    entries_per_set = table->entries[base0].entries_per_set;

    if (base0 % entries_per_set) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set;
    set = base0 / entries_per_set;

    /* Do optional data integrity check */
    SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;
        for (i = 0; i < entries_per_set; i++) {
            table->entries[base + i].ref_count--;
        }
    }

    if (profile->tables[0].entries[base0].ref_count != 0) {
        return SOC_E_NONE;
    }

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        index_min = table->index_min;
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;

        if (entries_per_set >= SOC_PROFILE_MEM_DMA_THRESHHOLD) {
            entry_words = table->entry_words;
            alloc_size =  WORDS2BYTES(entry_words) * entries_per_set;
            range_p = soc_cm_salloc(unit, alloc_size, "profile update");
            if (NULL == range_p) {
                return SOC_E_MEMORY;
            }
            sal_memset((void *)range_p, 0, alloc_size);

            if (table->data_mask != NULL) {
                /* Read full entry from HW when profile only covers
                 * part of the table. */
                rv = soc_mem_read_range(unit, table->mem, MEM_BLOCK_ANY,
                                        index_min + base,
                                        index_min + base + entries_per_set - 1,
                                        range_p);
            }

            if (SOC_SUCCESS(rv)) {
                null_entry = soc_mem_entry_null(unit, table->mem);
                data_words = soc_mem_entry_words(unit, table->mem);
                for (i = 0; i < entries_per_set; i++) {
                    ent_p =
                        soc_mem_table_idx_to_pointer(unit, table->mem,
                                                     uint32 *, range_p, i);

                    if (table->data_mask != NULL) {
                        /* Clear partial entry */
                        for (j = 0; j < data_words; j++) {
                            ent_p[j] &= ~table->data_mask[j];
                        }
                    } else {
                        /* Write entire new entry */
                        sal_memcpy(ent_p, null_entry,
                                   data_words * sizeof(uint32));
                    }
                }
            }

            if (SOC_SUCCESS(rv)) {
                /* Write back the modified entries */
                rv = soc_mem_write_range(unit, table->mem, MEM_BLOCK_ANY,
                                         index_min + base,
                                         index_min + base + entries_per_set - 1,
                                         range_p);
            }

            soc_cm_sfree(unit, range_p);
            if (SOC_FAILURE(rv)) {
                return rv;
            }            
        } else {
            /* FIXME:  We don't check for the data_mask != 0 case here
             * because the only extant example is handled above.  If
             * we have new cases that fall into this section, additional
             * code must be added.
             */
            for (i = 0; i < entries_per_set; i++) {
                /* Insert the new entries into profile table */
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, table->mem, MEM_BLOCK_ANY,
                                   index_min + base + i,
                                   soc_mem_entry_null(unit, table->mem)));
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_mem_set
 * Purpose:
 *      Update existing non-shared entry.
 *
 * Parameters:
 *      unit            - (IN) Unit
 *      profile         - (IN) Pointer to profile memory structure
 *      entries_array   - (IN) Array of pointer to table entries set
 *      index0          - (IN) Base index to update
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_set(int unit,
                    soc_profile_mem_t *profile,
                    void **entries_array,
                    uint32 index0)
{
    soc_profile_mem_table_t *table;
    int index_min, num_entries, num_sets, entries_per_set;
    int set, table_index, i, base, base0;
    int entry_words, data_words;
    int alloc_size, rv = SOC_E_NONE;
    uint32 *cache_p, *entry_p, *range_p, *ent_p;

    if (profile == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    base0 = index0 - table->index_min;

    if (table->entries[base0].ref_count == 0) {
        return SOC_E_NOT_FOUND;
    }

    entries_per_set = table->entries[base0].entries_per_set;

    if (base0 % entries_per_set) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set;
    set = base0 / entries_per_set;

    /* Do optional data integrity check */
    SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        index_min = table->index_min;
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;
        entry_words = table->entry_words;
        data_words = soc_mem_entry_words(unit, table->mem);
        entry_p = entries_array[table_index];
        cache_p = &table->cache_p[base * entry_words];

        if (entries_per_set >= SOC_PROFILE_MEM_DMA_THRESHHOLD) {
            alloc_size =  WORDS2BYTES(entry_words) * entries_per_set;
            range_p = soc_cm_salloc(unit, alloc_size, "profile update");

            /* FIXME: No check for data_mask != 0 here because no
             * current case needs such support.  It may need
             * to be added in the future. */

            if (NULL == range_p) {
                return SOC_E_MEMORY;
            }
            sal_memset((void *)range_p, 0, alloc_size);

            if (SOC_SUCCESS(rv)) {
                for (i = 0; i < entries_per_set; i++) {
                    ent_p =
                        soc_mem_table_idx_to_pointer(unit, table->mem,
                                                     uint32 *, range_p, i);
                    /* Write entire new entry */
                    sal_memcpy(ent_p, entry_p,
                               data_words * sizeof(uint32));
                    entry_p += entry_words;
                }
            }

            if (SOC_SUCCESS(rv)) {
                /* Write back the modified entries */
                rv = soc_mem_write_range(unit, table->mem, MEM_BLOCK_ANY,
                                         index_min + base,
                                         index_min + base + entries_per_set - 1,
                                         range_p);

                if (SOC_SUCCESS(rv)) {
                    /* Copy entry into the software cache. */
                    entry_p = entries_array[table_index];

                    for (i = 0; i < entries_per_set; i++) {
                        sal_memcpy(cache_p, entry_p,
                                   data_words * sizeof(uint32));
                        entry_p += entry_words;
                        cache_p += entry_words;

                        table->entries[base + i].ref_count++;
                        table->entries[base + i].entries_per_set =
                            entries_per_set;
                    }
                }
            }

            soc_cm_sfree(unit, range_p);
            if (SOC_FAILURE(rv)) {
                return rv;
            }            
        } else {
            for (i = 0; i < entries_per_set; i++) {
                /* Write the new entries into profile table */
                SOC_IF_ERROR_RETURN(soc_mem_write(unit, table->mem, MEM_BLOCK_ANY,
                                                  index_min + base + i, entry_p));

                /* Copy entry into the software cache. */
                sal_memcpy(cache_p, entry_p, data_words * sizeof(uint32));
                entry_p += entry_words;
                cache_p += entry_words;

            }
        }
    }

    return SOC_E_NONE;
}

int
soc_profile_mem_index_get(int unit,
                          soc_profile_mem_t *profile,
                          soc_mem_t mem,
                          int *index)
{
    int i;
    soc_profile_mem_table_t *table;
    if (profile == NULL) {
        return SOC_E_PARAM;
    }
    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    for (i = 0; i < profile->table_count; i++) {
        table = &profile->tables[i];
        if (table->mem == mem) {
            *index = i;
            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

/*
 * Function:
 *      soc_profile_mem_get
 * Purpose:
 *      Get a set of entries (one or more entries) at the specified index.
 *
 * Parameters:
 *      unit          - (IN) Unit
 *      profile       - (IN) Pointer to profile memory structure
 *      index0        - (IN) Base index to the entries in HW for table 0
 *      count         - (IN) Array of number of entries to retrieve
 *      entries_array - (OUT) Array of pointer to table entries set
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_get(int unit,
                    soc_profile_mem_t *profile,
                    int index0,
                    int count,
                    void **entries_array)
{
    soc_profile_mem_table_t *table;
    int num_entries, num_sets, entries_per_set;
    int set, table_index, i, j, base, base0;
    int entry_words, data_words;
    uint32 *cache_p, *entry_p;

    if (profile == NULL || count <= 0) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    base0 = index0 - table->index_min;

    if (table->entries[base0].ref_count == 0) {
        return SOC_E_NOT_FOUND;
    }

    entries_per_set = table->entries[base0].entries_per_set;

    if (base0 % entries_per_set) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set;
    set = base0 / entries_per_set;

    /* Do optional data integrity check */
    SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;
        entry_words = table->entry_words;
        data_words = soc_mem_entry_words(unit, table->mem);
        entry_p = entries_array[table_index];
        cache_p = &table->cache_p[base * entry_words];

        for (i = 0; i < entries_per_set; i++) {
            if (i >= count) {
                break;
            }
            /* Copy entry from the software cache. (include pad) */
            if (table->data_mask != NULL) {
                for (j = 0; j < data_words; j++) {
                    entry_p[j] = cache_p[j] & table->data_mask[j];
                }
            } else {
                sal_memcpy(entry_p, cache_p, entry_words * sizeof(uint32));
            }
            entry_p += entry_words;
            cache_p += entry_words;
        }
    }

    return SOC_E_NONE;
}

int
soc_profile_mem_single_table_get(int unit,
                                 soc_profile_mem_t *profile,
                                 int index,
                                 int count,
                                 void *entries)
{
    void *entries_array[1];

    entries_array[0] = entries;
    return soc_profile_mem_get(unit, profile, index, count, entries_array);
}

/*
 * Function:
 *      soc_profile_mem_reference_unique
 * Purpose:
 *      Add the reference to the set of entries (one or more entries) at
 *      the specified base index.
 *
 * Parameters:
 *      unit             - (IN) Unit
 *      profile          - (IN) Pointer to profile memory structure
 *      index0           - (IN) Base index to the entries in HW for table 0
 *      entries_per_set0 - (IN) Number of entries in the set for table 0
 *                              (for WARM BOOT only)
 *      pipe             - (IN) Pipe to affect
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_reference_unique(int unit,
                          soc_profile_mem_t *profile,
                          int index0,
                          int entries_per_set0,
                          int pipe)
{
    soc_profile_mem_table_t *table;
    int num_entries, num_sets, entries_per_set;
    int set, table_index, i, base, base0, entries_per_pipe;

    if (profile == NULL) {
        return SOC_E_PARAM;
    }

    if (SOC_WARM_BOOT(unit)) {
        if (entries_per_set0 <= 0) {
            return SOC_E_PARAM;
        }
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    if (NULL == SOC_MEM_UNIQUE_ACC(unit, table->mem)) {
        return SOC_E_PARAM;
    }

    entries_per_pipe = SOC_MEM_SIZE(unit, table->mem);

    base0 = index0 - table->index_min;
    /* Offset the base to the appropriate pipe */
    base0 += pipe*entries_per_pipe;

    if (SOC_WARM_BOOT(unit)) {
        if (num_entries % entries_per_set0) {
            return SOC_E_PARAM;
        }
        entries_per_set = entries_per_set0;
    } else {
        if (table->entries[base0].ref_count == 0) {
            return SOC_E_NOT_FOUND;
        }

        entries_per_set = table->entries[base0].entries_per_set;
    }

    if (base0 % entries_per_set) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set;
    set = base0 / entries_per_set;

#if 0
    /* Do optional data integrity check */
    SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));
#endif

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;
        for (i = 0; i < entries_per_set; i++) {
            table->entries[base + i].ref_count++;
            table->entries[base + i].entries_per_set = entries_per_set;
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_profile_mem_reference
 * Purpose:
 *      Add the reference to the set of entries (one or more entries) at
 *      the specified base index.
 *
 * Parameters:
 *      unit             - (IN) Unit
 *      profile          - (IN) Pointer to profile memory structure
 *      index0           - (IN) Base index to the entries in HW for table 0
 *      entries_per_set0 - (IN) Number of entries in the set for table 0
 *                              (for WARM BOOT only)
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_reference(int unit,
                          soc_profile_mem_t *profile,
                          int index0,
                          int entries_per_set0)
{
    soc_profile_mem_table_t *table;
    int num_entries, num_sets, entries_per_set;
    int set, table_index, i, base, base0;

    if (profile == NULL) {
        return SOC_E_PARAM;
    }

    if (SOC_WARM_BOOT(unit)) {
        if (entries_per_set0 <= 0) {
            return SOC_E_PARAM;
        }
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    num_entries = table->index_max - table->index_min + 1;
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    base0 = index0 - table->index_min;

    if (SOC_WARM_BOOT(unit)) {
        if (num_entries % entries_per_set0) {
            return SOC_E_PARAM;
        }
        entries_per_set = entries_per_set0;
    } else {
        if (table->entries[base0].ref_count == 0) {
            return SOC_E_NOT_FOUND;
        }

        entries_per_set = table->entries[base0].entries_per_set;
    }

    if (base0 % entries_per_set) {
        return SOC_E_PARAM;
    }

    num_sets = num_entries / entries_per_set;
    set = base0 / entries_per_set;

#if 0
    /* Do optional data integrity check */
    SOC_IF_ERROR_RETURN(_soc_profile_mem_check(unit, profile, base0));
#endif

    for (table_index = 0; table_index < profile->table_count; table_index++) {
        table = &profile->tables[table_index];
        num_entries = table->index_max - table->index_min + 1;
        entries_per_set = num_entries / num_sets;
        base = set * entries_per_set;
        for (i = 0; i < entries_per_set; i++) {
            table->entries[base + i].ref_count++;
            table->entries[base + i].entries_per_set = entries_per_set;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_mem_ref_count_get
 * Purpose:
 *      Get the reference count of the cached entry at the specified index.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile     - (IN) Pointer to profile memory structure
 *      index0      - (IN) Base index to the entries in HW for table 0
 *      ref_count   - (OUT) Reference count
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_ref_count_get(int unit,
                              soc_profile_mem_t *profile,
                              int index0,
                              int *ref_count)
{
    soc_profile_mem_table_t *table;
    int base0;

    if (profile == NULL || ref_count == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    base0 = index0 - table->index_min;

    *ref_count = table->entries[base0].ref_count;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_mem_fields32_modify_unique
 * Purpose:
 *      Modify the specified fields (of maximum size 32-bits)
 *      for all valid (non-zero reference count) entries of a
 *      profile table in the profile set.
 *      For simplicity, only one table in a profile
 *      set may be modified at a time.
 *
 *      This is for when memory is split and UNIQUE per pipe. It will
 *      modify ALL memories, not just a single pipe
 *
 * Parameters:
 *      unit             - (IN) Unit
 *      profile          - (IN) Pointer to profile memory structure
 *      table_id         - (IN) Profile set index of table to modify
 *      field_count      - (IN) Number of field/value pairs to modify
 *      fields           - (IN) Array of table fields to modify
 *      values           - (IN) Array of table values to modify
 *                                    
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_fields32_modify_unique(int unit,
                                soc_profile_mem_t *profile,
                                int table_id,
                                int field_count,
                                soc_field_t *fields,
                                uint32 *values)
{
    soc_profile_mem_table_t *table;
    int index_min, modify_entries;
    int offset, fix;
    int index, start_index, end_index;
    int entry_words, data_words, word;
    int alloc_size, rv = SOC_E_NONE;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 *cache_p, *mask_p, *range_p, *ent_p;
    int i, entries_per_pipe;
    soc_mem_t mem;

    /* COVERITY: Intentional, stack use of 5192 bytes */
    /* coverity[stack_use_callee_max : FALSE] */
    /* coverity[stack_use_overflow : FALSE] */
    /* coverity[stack_use_return : FALSE] */

    if ((profile == NULL) ||
        (field_count <= 0) || (fields == NULL) || (values == 0)){
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    if ((table_id < 0) || (table_id >= profile->table_count)) {
        return SOC_E_PARAM;
    }


    table = &profile->tables[table_id];

    if (NULL == (SOC_MEM_UNIQUE_ACC(unit, table->mem))) {
        return SOC_E_PARAM;
    }

    entries_per_pipe = SOC_MEM_SIZE(unit, table->mem);

    index_min = table->index_min;

    for (i = 0; i < SOC_INFO(unit).num_pipe; i++) {
        /* Record range of modified entries in table */
        start_index = end_index = -1;
        for (index = i*entries_per_pipe;
             index < (i+1)*entries_per_pipe; index++) {
             /* Skip unused entries. */
            if (table->entries[index].ref_count == 0) {
                continue;
            }

            /* Update range of modified entries */
            if (start_index < 0) {
                start_index = index;
            }
            end_index = index;
        }

        mem = SOC_MEM_UNIQUE_ACC(unit, table->mem)[i];

        /* Knowing the range, we can create the modification buffer */
        modify_entries = end_index - start_index + 1;
        entry_words = table->entry_words;
        data_words = soc_mem_entry_words(unit, mem);
        cache_p = &table->cache_p[start_index * entry_words];

        /* Verify fields fit within data_mask of table, if any */
        if (table->data_mask != NULL) {
            mask_p = table->data_mask;
            sal_memset(entry, 0, sizeof(entry));
            for (fix = 0; fix < field_count; fix++) {
                soc_mem_field32_set(unit, mem, entry,
                                    fields[fix], values[fix]);
            }
            for (word = 0; word < data_words; word++) {
                if ((entry[word] & mask_p[word]) != entry[word]) {
                    break;
                }
                /* coverity[dead_error_condition] */
                if (word < data_words) {
                    return SOC_E_PARAM;
                }
            }
        }

        /*
         * Use DMA to update regardless of range since this is expected
         * to be a bulk operation.
         */
        alloc_size =  WORDS2BYTES(entry_words) * modify_entries;
        range_p = soc_cm_salloc(unit, alloc_size, "profile update");
        if (NULL == range_p) {
            return SOC_E_MEMORY;
        }
        sal_memset((void *)range_p, 0, alloc_size);

        /* Read full entry from "HW" */
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                index_min + start_index%entries_per_pipe,
                                index_min + end_index%entries_per_pipe,
                                range_p);

        if (SOC_SUCCESS(rv)) {
            for (offset = 0; offset < modify_entries; offset++) {
                ent_p =
                    soc_mem_table_idx_to_pointer(unit, mem,
                                                 uint32 *, range_p, offset);

                /* Update entries in working buffer */
                for (fix = 0; fix < field_count; fix++) {
                    soc_mem_field32_set(unit, mem, ent_p,
                                        fields[fix], values[fix]);
                }
            }
        }

        if (SOC_SUCCESS(rv)) {
            /* Write back the modified entries */
            rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL,
                                     index_min + start_index%entries_per_pipe,
                                     index_min + end_index%entries_per_pipe,
                                     range_p);
        }

        if (SOC_SUCCESS(rv)) {
            /* Now that the write to HW succeeded, commit changes to
             * profile cache */
            for (offset = 0; offset < modify_entries; offset++) {
                ent_p =
                    soc_mem_table_idx_to_pointer(unit, mem,
                                                 uint32 *, range_p, offset);

                /* Update cache */
                sal_memcpy(cache_p, ent_p,
                           data_words * sizeof(uint32));
                cache_p += entry_words;
            }
        }
        soc_cm_sfree(unit, range_p);
    }

    return rv;
}


/*
 * Function:
 *      soc_profile_mem_fields32_modify
 * Purpose:
 *      Modify the specified fields (of maximum size 32-bits)
 *      for all valid (non-zero reference count) entries of a
 *      profile table in the profile set.
 *      For simplicity, only one table in a profile
 *      set may be modified at a time.
 *
 * Parameters:
 *      unit             - (IN) Unit
 *      profile          - (IN) Pointer to profile memory structure
 *      table_id         - (IN) Profile set index of table to modify
 *      field_count      - (IN) Number of field/value pairs to modify
 *      fields           - (IN) Array of table fields to modify
 *      values           - (IN) Array of table values to modify
 *
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_fields32_modify(int unit,
                                soc_profile_mem_t *profile,
                                int table_id,
                                int field_count,
                                soc_field_t *fields,
                                uint32 *values)
{
    soc_profile_mem_table_t *table;
    int index_min, num_entries, modify_entries;
    int offset, fix;
    int index, start_index, end_index;
    int entry_words, data_words, word;
    int alloc_size, rv = SOC_E_NONE;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 *cache_p, *mask_p, *range_p, *ent_p;

    /* COVERITY: Intentional, stack use of 5192 bytes */
    /* coverity[stack_use_callee_max : FALSE] */
    /* coverity[stack_use_overflow : FALSE] */
    /* coverity[stack_use_return : FALSE] */

    if ((profile == NULL) ||
        (field_count <= 0) || (fields == NULL) || (values == 0)){
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    if ((table_id < 0) || (table_id >= profile->table_count)) {
        return SOC_E_PARAM;
    }

    table = &profile->tables[table_id];
    num_entries = table->index_max - table->index_min + 1;
    index_min = table->index_min;

    /* Record range of modified entries in table */
    start_index = end_index = -1;
    for (index = 0; index < num_entries; index++) {
         /* Skip unused entries. */
        if (table->entries[index].ref_count == 0) {
            continue;
        }

        /* Update range of modified entries */
        if (start_index < 0) {
            start_index = index;
        }
        end_index = index;
    }

    /* Knowing the range, we can create the modification buffer */
    modify_entries = end_index - start_index + 1;
    entry_words = table->entry_words;
    data_words = soc_mem_entry_words(unit, table->mem);
    cache_p = &table->cache_p[start_index * entry_words];

    /* Verify fields fit within data_mask of table, if any */
    if (table->data_mask != NULL) {
        mask_p = table->data_mask;
        sal_memset(entry, 0, sizeof(entry));
        for (fix = 0; fix < field_count; fix++) {
            soc_mem_field32_set(unit, table->mem, entry,
                                fields[fix], values[fix]);
        }
        for (word = 0; word < data_words; word++) {
            if ((entry[word] & mask_p[word]) != entry[word]) {
                break;
            }
	    /* coverity[dead_error_condition] */
            if (word < data_words) {
                return SOC_E_PARAM;
            }
        }
    }

    /*
     * Use DMA to update regardless of range since this is expected
     * to be a bulk operation.
     */
    alloc_size =  WORDS2BYTES(entry_words) * modify_entries;
    range_p = soc_cm_salloc(unit, alloc_size, "profile update");
    if (NULL == range_p) {
        return SOC_E_MEMORY;
    }
    sal_memset((void *)range_p, 0, alloc_size);

    /* Read full entry from "HW" */
    rv = soc_mem_read_range(unit, table->mem, MEM_BLOCK_ANY,
                            index_min + start_index,
                            index_min + end_index,
                            range_p);

    if (SOC_SUCCESS(rv)) {
        for (offset = 0; offset < modify_entries; offset++) {
            ent_p =
                soc_mem_table_idx_to_pointer(unit, table->mem,
                                             uint32 *, range_p, offset);

            /* Update entries in working buffer */
            for (fix = 0; fix < field_count; fix++) {
                soc_mem_field32_set(unit, table->mem, ent_p,
                                    fields[fix], values[fix]);
            }
        }
    }

    if (SOC_SUCCESS(rv)) {
        /* Write back the modified entries */
        rv = soc_mem_write_range(unit, table->mem, MEM_BLOCK_ALL,
                                 index_min + start_index,
                                 index_min + end_index,
                                 range_p);
    }

    if (SOC_SUCCESS(rv)) {
        /* Now that the write to HW succeeded, commit changes to
         * profile cache */
        for (offset = 0; offset < modify_entries; offset++) {
            ent_p =
                soc_mem_table_idx_to_pointer(unit, table->mem,
                                             uint32 *, range_p, offset);

            /* Update cache */
            sal_memcpy(cache_p, ent_p,
                       data_words * sizeof(uint32));
            cache_p += entry_words;
        }
    }

    soc_cm_sfree(unit, range_p);

    return rv;
}

/*
 * Function:
 *      soc_profile_mem_fields32_modify
 * Purpose:
 *      Modify the specified field (of maximum size 32-bits)
 *      for all valid (non-zero reference count) entries of the
 *      indicated profile table of the profile set.
 *
 * Parameters:
 *      unit             - (IN) Unit
 *      profile          - (IN) Pointer to profile memory structure
 *      table_id         - (IN) Profile set index of table to modify
 *      field            - (IN) Table field to modify
 *      value            - (IN) Table value to modify
 *                                    
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_field32_modify(int unit,
                               soc_profile_mem_t *profile,
                               int table_id,
                               soc_field_t field,
                               uint32 value)
{
    return soc_profile_mem_fields32_modify(unit, profile, table_id,
                                           1, &field, &value);
}

/*
 * Function:
 *      soc_profile_reg_t_init
 * Purpose:
 *      Initialize a soc_profile_reg_t structure.
 *
 * Parameters:
 *      profile_reg - (IN) Pointer to profile register structure
 * Returns:
 *      void
 */
void
soc_profile_reg_t_init(soc_profile_reg_t *profile_reg)
{
    if (NULL != profile_reg) {
        sal_memset(profile_reg, 0, sizeof(soc_profile_reg_t));
    }
}

/*
 * Function:
 *      soc_profile_reg_create
 * Purpose:
 *      Create a shadow copy and refcounts of a profile table.
 *      If called during WARM BOOT, the shadow copy is populated with
 *      the HW contents, otherwise, both the shadow copy and the
 *      HW entries are cleared.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      regs        - (IN) Pointer to register id array
 *      regs_count  - (IN) Number of entries in register id array
 *      profile_reg - (IN) Pointer to profile register structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_create(int unit,
                       soc_reg_t *regs,
                       int regs_count,
                       soc_profile_reg_t *profile_reg)
{
    soc_reg_t reg;
    int rv;
    int num_entries, reg_idx, i;
    int alloc_size;
    uint64 *cache_p;
    uint64 rval;

    if (profile_reg == NULL) {
        return SOC_E_INIT;
    }

    if (regs == NULL || !regs_count) {
        return SOC_E_PARAM;
    }

    num_entries = SOC_REG_NUMELS(unit, regs[0]);
    if (num_entries <= 0) {
        return SOC_E_BADID;
    }
    for (reg_idx = 1; reg_idx < regs_count; reg_idx++) {
       /*  Not needed, as is overwritten later. Coverity complains also.
        reg = regs[reg_idx];
        */
        /* Make sure index mechanism is consistent across all registers */
        if (SOC_REG_NUMELS(unit, regs[0]) != num_entries) {
            return SOC_E_PARAM;
        }
    }

    alloc_size = num_entries * \
        (sizeof(soc_profile_reg_entry_t) + regs_count * sizeof(uint64)) +
         regs_count * sizeof(soc_reg_t);

    /* If profile_reg->regs is NULL, init the profile_reg for the first
     * time, otherwise simply check for null pointers */
    if (profile_reg->regs != NULL) {
        if (profile_reg->entries == NULL) {
            return SOC_E_INTERNAL;
        }
    } else {
        profile_reg->entries = sal_alloc(alloc_size,  "Profile Reg Entries");
        if (profile_reg->entries == NULL) {
            return SOC_E_MEMORY;
        }
    }
    sal_memset(profile_reg->entries, 0, alloc_size);

    cache_p = (uint64 *)&profile_reg->entries[num_entries];
    for (i = 0; i < num_entries; i++) {
        profile_reg->entries[i].cache_p = cache_p;
        cache_p += regs_count;
    }
    profile_reg->regs = (soc_reg_t *)cache_p;
    for (reg_idx = 0; reg_idx < regs_count; reg_idx++) {
        profile_reg->regs[reg_idx] = regs[reg_idx];
    }
    profile_reg->regs_count = regs_count;

    if (SOC_WARM_BOOT(unit)) {
        for (i = 0; i < num_entries; i++) {
            cache_p = profile_reg->entries[i].cache_p;
            for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
                reg = profile_reg->regs[reg_idx];
                rv = soc_reg_get(unit, reg, REG_PORT_ANY, i, cache_p);
                if (rv < 0) {
                    sal_free(profile_reg->entries);
                    profile_reg->regs = NULL;
                    profile_reg->entries = NULL;
                    return rv;
                }
                cache_p++;
            }
        }
    } else {
        /* Clear HW memory */
        COMPILER_64_ZERO(rval);
        for (i = 0; i < num_entries; i++) {
            for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
                reg = profile_reg->regs[reg_idx];
                rv = soc_reg_set(unit, reg, REG_PORT_ANY, i, rval);
                if (rv < 0) {
                    sal_free(profile_reg->entries);
                    profile_reg->regs = NULL;
                    profile_reg->entries = NULL;
                    return rv;
                }
            }
        }
    }

    return SOC_E_NONE;
}

int
soc_profile_reg_single_reg_create(int unit,
                                  soc_reg_t reg,
                                  soc_profile_reg_t *profile_reg)
{
    return soc_profile_reg_create(unit, &reg, 1, profile_reg);
}

/*
 * Function:
 *      soc_profile_reg_destroy
 * Purpose:
 *      Destroy the shadow copy and refcounts of a profile table.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile_reg - (IN) Pointer to profile register structure
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_destroy(int unit,
                        soc_profile_reg_t *profile_reg)
{
    if (profile_reg != NULL) {
        if (profile_reg->entries != NULL) {
            sal_free(profile_reg->entries);
        }
        profile_reg->regs = NULL;
        profile_reg->entries = NULL;
        return SOC_E_NONE;
    }
    return SOC_E_PARAM;
}

/*
 * Function:
 *      soc_profile_reg_add
 * Purpose:
 *      Add a set of entries (one or more entries) to a profile table. This
 *      routine searches for a matching set in the profile table. If a matching
 *      set is found, the ref count for that entry is incremented and
 *      its base index is returned. If a matching set is not found and a free
 *      set is found, the HW table is updated, the ref count is incremented,
 *      and the base index of the set is returned. If no free set is found, an
 *      error is returned
 *
 * Parameters:
 *      unit            - (IN) Unit
 *      profile_reg     - (IN) Pointer to profile register structure
 *      entries         - (IN) Array of pointer to register entries set
 *      entries_per_set - (IN) Number of entries in the set
 *      index           - (OUT) Base index for the entries allocated in HW
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_add(int unit,
                    soc_profile_reg_t *profile_reg,
                    uint64 **entries,
                    int entries_per_set,
                    uint32 *index)
{
    soc_reg_t reg;
    int base, free_index;
    int num_entries, reg_idx, i;
    uint64 *cache_p, *entry_p;

    if (profile_reg == NULL || profile_reg->regs == NULL ||
        profile_reg->entries == NULL) {
        return SOC_E_INIT;
    }

    num_entries = SOC_REG_NUMELS(unit, profile_reg->regs[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if (entries == NULL || index == NULL ||
        entries_per_set < 1 || entries_per_set > num_entries) {
        return SOC_E_PARAM;
    }

    /*
     * Search for an existing entry that has the same configuration.
     */
    free_index = -1;
    for (base = 0; base < num_entries; base += entries_per_set) {
        /* Skip unused entries. */
        if (profile_reg->entries[base].ref_count == 0) {
            if (free_index == -1) {
                /* Preserve location of free slot. */
                free_index = base;
                for (i = 1; i < entries_per_set; i++) {
                    if (profile_reg->entries[base + i].ref_count) {
                        free_index = -1;
                        break;
                    }
                }
            }
            continue;
        }

        for (i = 0; i < entries_per_set; i++) {
            if (profile_reg->entries[base + i].entries_per_set !=
                entries_per_set) {
                break;
            }
            for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
                reg = profile_reg->regs[reg_idx];
                entry_p = &entries[reg_idx][i];
                cache_p = &profile_reg->entries[base + i].cache_p[reg_idx];
                if (COMPILER_64_NE(*cache_p, *entry_p)) {
                    break;
                }
            }
            if (reg_idx != profile_reg->regs_count) {
                break;
            }
        }
        if (i == entries_per_set) {
            for (i = 0; i < entries_per_set; i++) {
                profile_reg->entries[base + i].ref_count++;
            }
            *index = base;
            return SOC_E_NONE;
        }
        if (profile_reg->entries[base].entries_per_set > entries_per_set) {
            base += profile_reg->entries[base].entries_per_set -
                    entries_per_set;
        }
    }

    if (free_index == -1) {
        return SOC_E_RESOURCE;
    }

    for (i = 0; i < entries_per_set; i++) {
        for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
            reg = profile_reg->regs[reg_idx];
            entry_p = &entries[reg_idx][i];
            cache_p = &profile_reg->entries[free_index + i].cache_p[reg_idx];

            if (profile_reg->profile_flag & SOC_PROFILE_FLAG_XPE_SINGLE_ACC) {
                int xpe;
                for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                    int inst = xpe | SOC_REG_ADDR_INSTANCE_MASK;
                    /* Insert the new entries into profile table */
                    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst,
                                                    free_index + i, *entry_p));
                }
            } else {
                /* Insert the new entries into profile table */
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY,
                                                free_index + i, *entry_p));
            }

            /* Copy entry into the software cache. */
            *cache_p = *entry_p;
        }
        profile_reg->entries[free_index + i].ref_count++;
        profile_reg->entries[free_index + i].entries_per_set = entries_per_set;
    }
    *index = free_index;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_reg_delete
 * Purpose:
 *      Delete the reference to the set of entries (one or more entries) at
 *      the specified index.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile_reg - (IN) Pointer to profile register structure
 *      index       - (IN) Base index for the entries allocated in HW
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_delete(int unit,
                       soc_profile_reg_t *profile_reg,
                       uint32 index)
{
    soc_reg_t reg;
    int num_entries, entries_per_set, reg_idx, i;
    uint64 rval;

    if (profile_reg == NULL || profile_reg->regs == NULL ||
        profile_reg->entries == NULL) {
        return SOC_E_INIT;
    }

    num_entries = SOC_REG_NUMELS(unit, profile_reg->regs[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if ((int)index >= num_entries) {
        return SOC_E_PARAM;
    }

    if (profile_reg->entries[index].ref_count == 0) {
        return SOC_E_NOT_FOUND;
    }

    entries_per_set = profile_reg->entries[index].entries_per_set;
    if (index % entries_per_set) {
        return SOC_E_BADID;
    }

    profile_reg->entries[index].ref_count--;
    for (i = 1; i < entries_per_set; i++) {
        profile_reg->entries[index + i].ref_count--;
        if (profile_reg->entries[index + i].ref_count !=
            profile_reg->entries[index].ref_count ||
            profile_reg->entries[index + i].entries_per_set !=
            profile_reg->entries[index].entries_per_set) {
            return SOC_E_INTERNAL;
        }
    }

    if (profile_reg->entries[index].ref_count != 0) {
        return SOC_E_NONE;
    }

    COMPILER_64_ZERO(rval);
    for (i = 0; i < entries_per_set; i++) {
        for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
            /* Clear the entry in the HW */
            reg = profile_reg->regs[reg_idx];
            if (profile_reg->profile_flag & SOC_PROFILE_FLAG_XPE_SINGLE_ACC) {
                int xpe;
                for (xpe = 0; xpe < NUM_XPE(unit); xpe++) {
                    int inst = xpe | SOC_REG_ADDR_INSTANCE_MASK;
                    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, inst,
                                                    index + i, rval));
                }
            } else {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY,
                                                index + i, rval));
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_reg_get
 * Purpose:
 *      Get a set of entries (one or more entries) at the specified index.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile_reg - (IN) Pointer to profile register structure
 *      index       - (IN) Base index for the entries allocated in HW
 *      count       - (IN) Number of entries to retreive
 *      entries     - (OUT) Array of pointer to register entries set
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_get(int unit,
                    soc_profile_reg_t *profile_reg,
                    uint32 index,
                    int count,
                    uint64 **entries)
{
    int num_entries, reg_idx, i;
    uint64 *cache_p, *entry_p;

    if (profile_reg == NULL || profile_reg->regs == NULL ||
        profile_reg->entries == NULL) {
        return SOC_E_INIT;
    }

    num_entries = SOC_REG_NUMELS(unit, profile_reg->regs[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if (entries == NULL || (int)index >= num_entries || count <= 0) {
        return SOC_E_PARAM;
    }

    if (profile_reg->entries[index].ref_count == 0) {
        return SOC_E_NOT_FOUND;
    }

    if (count > profile_reg->entries[index].entries_per_set) {
        return SOC_E_PARAM;
    }

    if (index % profile_reg->entries[index].entries_per_set) {
        return SOC_E_BADID;
    }

    for (i = 1; i < count; i++) {
        /* Do some optional data integrity check */
        if (profile_reg->entries[index + i].ref_count !=
            profile_reg->entries[index].ref_count ||
            profile_reg->entries[index + i].entries_per_set !=
            profile_reg->entries[index].entries_per_set) {
            return SOC_E_INTERNAL;
        }
    }

    for (i = 0; i < count; i++) {
        for (reg_idx = 0; reg_idx < profile_reg->regs_count; reg_idx++) {
            entry_p = &entries[reg_idx][i];
            cache_p = &profile_reg->entries[index + i].cache_p[reg_idx];

            /* Copy entry from the software cache. */
            *entry_p = *cache_p;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_reg_reference
 * Purpose:
 *      Add the reference to the set of entries (one or more entries) at
 *      the specified base index.
 *
 * Parameters:
 *      unit            - (IN) Unit
 *      profile_reg     - (IN) Pointer to profile register structure
 *      index           - (IN) Base index for the entries allocated in HW
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_reference(int unit,
                          soc_profile_reg_t *profile_reg,
                          uint32 index,
                          int entries_per_set_override)
{
    int num_entries, entries_per_set, i;

    if (profile_reg == NULL || profile_reg->regs == NULL ||
        profile_reg->entries == NULL) {
        return SOC_E_INIT;
    }

    num_entries = SOC_REG_NUMELS(unit, profile_reg->regs[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if ((int)index >= num_entries) {
        return SOC_E_PARAM;
    }

    if ((profile_reg->entries[index].ref_count == 0) && 
        (!SOC_WARM_BOOT(unit))) {
        return SOC_E_NOT_FOUND;
    }

    if (SOC_WARM_BOOT(unit)) {
        /* During WB, use the passed in entries_per_set_override value */
        if ((entries_per_set_override < 1) || 
            (entries_per_set_override > num_entries)) {
            return SOC_E_PARAM;
        }
        profile_reg->entries[index].entries_per_set = 
            entries_per_set_override;
    }
    entries_per_set = profile_reg->entries[index].entries_per_set;
    if (index % entries_per_set) {
        return SOC_E_BADID;
    }

    profile_reg->entries[index].ref_count++;
    for (i = 1; i < entries_per_set; i++) {
        profile_reg->entries[index + i].ref_count++;
        if (SOC_WARM_BOOT(unit)) {
            /* During WB, use the passed in entries_per_set_override value */
            profile_reg->entries[index + i].entries_per_set = 
                entries_per_set_override;
        }
        /* Do some optional data integrity check */
        if (profile_reg->entries[index + i].ref_count !=
            profile_reg->entries[index].ref_count ||
            profile_reg->entries[index + i].entries_per_set !=
            profile_reg->entries[index].entries_per_set) {
            return SOC_E_INTERNAL;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_profile_reg_ref_count_get
 * Purpose:
 *      Get the reference count of the cached entry at the specified index.
 *
 * Parameters:
 *      unit        - (IN) Unit
 *      profile_reg - (IN) Pointer to profile register structure
 *      index       - (IN) Base index for the entries allocated in HW
 *      ref_count   - (OUT) Reference count
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_reg_ref_count_get(int unit,
                              soc_profile_reg_t *profile_reg,
                              uint32 index, int *ref_count)
{
    int num_entries, entries_per_set;

    if (profile_reg == NULL || profile_reg->regs == NULL ||
        profile_reg->entries == NULL || ref_count == NULL) {
        return SOC_E_INIT;
    }

    num_entries = SOC_REG_NUMELS(unit, profile_reg->regs[0]);
    if (num_entries <= 0) {
        return SOC_E_INTERNAL;
    }

    if ((int)index >= num_entries) {
        return SOC_E_PARAM;
    }

    if (profile_reg->entries[index].ref_count == 0) {
        *ref_count = 0;
        return SOC_E_NONE;
    }

    entries_per_set = profile_reg->entries[index].entries_per_set;
    if (index % entries_per_set) {
        return SOC_E_BADID;
    }

    *ref_count = profile_reg->entries[index].ref_count;

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_profile_mem_write_mode_get
 * Purpose:
 *      Get dma or pio write operation mode
 *      for a profile table mem.
 *
 * Parameters:
 *      unit            - (IN) Unit
 *      profile         - (IN) Pointer to profile memory structure
 *      index0          - (IN) Base index to get mode
 *      mode            - (OUT) table write mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_profile_mem_write_mode_get(int unit,
                    soc_profile_mem_t *profile,
                    uint32 index0, int *mode)
{
    soc_profile_mem_table_t *table;
    int entries_per_set;
    int base0;

    if (profile == NULL) {
        return SOC_E_PARAM;
    }

    if (profile->tables == NULL || profile->table_count == 0) {
        return SOC_E_INIT;
    }

    table = &profile->tables[0];
    if (index0 < table->index_min || index0 > table->index_max) {
        return SOC_E_PARAM;
    }

    base0 = index0 - table->index_min;

    if (table->entries[base0].ref_count == 0) {
        return SOC_E_NOT_FOUND;
    }

    entries_per_set = table->entries[base0].entries_per_set;

    if (base0 % entries_per_set) {
        return SOC_E_PARAM;
    }

    if (entries_per_set >= SOC_PROFILE_MEM_DMA_THRESHHOLD) {
        *mode = SOC_PROFILE_MEM_WRITE_DMA_MODE;
    } else {
        *mode = SOC_PROFILE_MEM_WRITE_PIO_MODE;
    }

    return SOC_E_NONE;
}
