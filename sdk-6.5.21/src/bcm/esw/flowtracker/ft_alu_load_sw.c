/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_alu_load_sw.c
 * Purpose:     The purpose of this file is to set ALU/LOAD hash methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>

#ifdef BCM_FLOWTRACKER_SUPPORT

bcmi_ft_alu_table_hash_t *bcmi_ft_alu_table_hash[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *   bcmi_ft_alu_hash_init
 * Purpose:
 *   initialise hash table
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_xxx
 */
int
bcmi_ft_alu_hash_init(int unit)
{
    int i = 0;
    bcmi_ft_alu_table_hash_t *alu_hash = NULL;

    if (NULL == bcmi_ft_alu_table_hash[unit]) {
        bcmi_ft_alu_table_hash[unit] = (bcmi_ft_alu_table_hash_t *)
            sal_alloc(BCMI_FT_ALU_HASH_BUCKETS *
                sizeof(bcmi_ft_alu_table_hash_t), "ALU hash Alloc");

        /* return error if memory not allocated */
        if (bcmi_ft_alu_table_hash[unit] == NULL) {
            return SOC_E_MEMORY;
        }

        /* initialise head links */
        for (i = 0; i < BCMI_FT_ALU_HASH_BUCKETS; i++) {
            alu_hash = (&(bcmi_ft_alu_table_hash[unit][i]));
            alu_hash->head_link = NULL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu_hash_cleanup
 * Purpose:
 *   Clean/Free hash table
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   void
 */
void
bcmi_ft_alu_hash_cleanup(int unit)
{
    int i = 0;
    bcmi_ft_alu_table_hash_t *alu_hash = NULL;
    bcmi_ft_alu_entry_list_t *alu_hash_tmp = NULL;
    bcmi_ft_flowchecker_list_t *check_head = NULL;
    bcmi_ft_flowchecker_list_t *check_head_tmp = NULL;

    if (bcmi_ft_alu_table_hash[unit]) {

        /* Go though all hash buckets first */
        for (; i < BCMI_FT_ALU_HASH_BUCKETS; i++) {

            alu_hash =  &(bcmi_ft_alu_table_hash[unit][i]);
            alu_hash_tmp = alu_hash->head_link;

            /* Go through all hash entries in that bucket */
            while (alu_hash_tmp != NULL) {

                alu_hash->head_link = alu_hash_tmp->link;

                /* Go through all check id added in hash entry */
                check_head_tmp = alu_hash_tmp->alu_info.head;
                while (check_head_tmp != NULL) {
                    check_head = check_head_tmp->next;
                    sal_free(check_head_tmp);
                    check_head_tmp = check_head;
                }

                sal_free(alu_hash_tmp);
                alu_hash_tmp = alu_hash->head_link;
            }
        }

        sal_free(bcmi_ft_alu_table_hash[unit]);
        bcmi_ft_alu_table_hash[unit] = NULL;
    }
}

/*
 * Function:
 *   bcmi_ft_alu_hash_entry_alloc
 * Purpose:
 *   Allocates a hash table entry
 * Parameters:
 *   unit   - (IN) BCM device id
 *   entry  - (IN) hash entry
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_alu_hash_entry_alloc(int unit,
                             bcmi_ft_alu_entry_list_t **entry)
{
    bcmi_ft_alu_entry_list_t *new_entry = NULL;

    /* allocate memory for hash entry */
    new_entry = sal_alloc( sizeof(bcmi_ft_alu_entry_list_t),
                                          "hash_table_entry");
    if (new_entry == NULL) {
        return BCM_E_MEMORY;
    }

    /* initialise memory allocated */
    sal_memset((&(new_entry->alu_info)), 0, sizeof(bcmi_ft_alu_hash_info_t));

    /* set the link to NULL, adding entry at tail. */
    new_entry->alu_info.alu_load_type = bcmiFtAluLoadTypeNone;
    new_entry->link = NULL;
    *entry = new_entry;

    return BCM_E_NONE;
}

STATIC void
bcmi_ft_group_alu_load_info_set(int unit,
                   bcm_flowtracker_group_t id,
                   bcmi_ft_alu_load_type_t alu_load_type,
                   bcmi_ft_group_alu_info_t *group_alu_info,
                   int *load_indexes,
                   int alu_load_index,
                   bcmi_ft_alu_hash_info_t *hash_alu_info)
{
    int i = 0;
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    uint32 load_mem_in_vector = 0;
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
    bcmi_ft_group_alu_info_t *local = NULL;
    bcmi_ft_group_alu_info_t *group_temp = NULL;

    if ((alu_load_type == bcmiFtAluLoadTypeAlu32) ||
        (alu_load_type == bcmiFtAluLoadTypeAlu16) ||
        (alu_load_type == bcmiFtAluLoadTypeAggAlu32)) {

        /* Assign the indexes in group memory for later use. */
        group_alu_info->alu_load_index = alu_load_index;
        group_alu_info->alu_load_type = alu_load_type;

        if (hash_alu_info != NULL) {
            /* Assign other ALU params here. For loads, nothing to be assigned.
             * For loads, nothing to be assigned. Loads add multiple keys into
             * one to form one key so cant assign multiple keys to one hash
             * entry. Load the key into hash alu state. */
            hash_alu_info->key.location = group_alu_info->key1.location;
            hash_alu_info->key.is_alu   = group_alu_info->key1.is_alu;
            hash_alu_info->key.length   = group_alu_info->key1.length;
            hash_alu_info->hash_ft_type = group_alu_info->alu_ft_type;
            hash_alu_info->param        = group_alu_info->element_type1;
            hash_alu_info->custom_id    = group_alu_info->custom_id1;

            /* Store flowchecker information to match while comparing.
             * To optimize it further, we can also have a list here of
             * similar flowtrackers. That way we can have different
             * flowcheckers with similar information on same ALU. */
            bcmi_ft_flowchecker_list_add(unit, &(hash_alu_info->head),
                                        group_alu_info->flowchecker_id);

            /* update the direction */
            hash_alu_info->direction =
                            BCMI_FT_GROUP_FTFP_DATA(unit, id).direction;
        }

    } else {

        /* Assign memory chunk to local pointer. */
        group_temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

        /* Assign the id to all the sub load indexes. */
        while (load_indexes[i] != -1) {
            local = (&(group_temp[load_indexes[i]]));
            local->alu_load_index = alu_load_index;
            local->alu_load_type = alu_load_type;
            i++;
        }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
        if (hash_alu_info != NULL) {
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                bcmi_ftv2_get_load_mem_vector(unit, id, alu_load_type,
                                              load_indexes, &load_mem_in_vector);
                hash_alu_info->hash_load_mem_vector = load_mem_in_vector;
            }
        }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
    }
}

/*
 * Function:
 *   bcmi_ft_alu_hash_entry_add
 * Purpose:
 *   Allocates a hash table entry
 * Parameters:
 *   IN  :  Unit
 *   OUT :  entry pointer.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_alu_hash_entry_add(int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_alu_load_type_t alu_load_type,
                bcmi_ft_group_alu_info_t *group_alu_info,
                int *load_indexes,
                bcmi_ft_alu_table_hash_t *hash_input,
                int *alu_load_index)
{
    int rv = BCM_E_NONE;
    bcmi_ft_alu_entry_list_t *temp = NULL;
    bcmi_ft_alu_entry_list_t *new_entry = NULL;

    /* Validate hash bucket input */
    if (hash_input == NULL) {
        return BCM_E_INIT;
    }

    /* Assign pointer to first entry of hash bucket */
    temp = hash_input->head_link;


    while (temp != NULL) {
        if (BCM_SUCCESS(bcmi_ft_alu_load_compare(unit, id, group_alu_info,
                            alu_load_type, (&temp->alu_info), load_indexes))) {
            /*
             * We also need to make sure that this index is not set in
             * the group. If set, then we can not use this index in the group.
             * We will have to allocated a new alu index for the group then.
             */
            if (bcmi_ft_alu_check_free_index
                    (unit, id, alu_load_type, temp->alu_info.alu_load_index)) {

                *alu_load_index = temp->alu_info.alu_load_index;

                bcmi_ft_flowchecker_list_add(unit, &((&temp->alu_info)->head),
                                                group_alu_info->flowchecker_id);

                bcmi_ft_group_alu_load_info_set(unit, id, alu_load_type,
                                                group_alu_info, load_indexes,
                                                *alu_load_index, NULL);

                return BCM_E_EXISTS;
            }
        }

        temp = temp->link;
    }

    /* entry does not exist, add a new one */
    BCM_IF_ERROR_RETURN
        (bcmi_ft_alu_hash_entry_alloc(unit, &new_entry));

    /* if no index is provided by caller then get new index */
    if (*alu_load_index == -1) {

        if (bcmi_ft_tracking_param_in_Debug_mode(unit, id,
                            group_alu_info, alu_load_type)) {
            rv = bcmi_ft_chip_debug_alu_index_get(unit, id,
                            group_alu_info, alu_load_type, alu_load_index);

        } else {
            rv = bcmi_ft_alu_get_free_index(unit, id, alu_load_type,
                                            group_alu_info, load_indexes,
                                            alu_load_index);
        }
        if (BCM_FAILURE(rv)) {
            sal_free(new_entry);
            return rv;
        }
    }

    /* assign the indexes into hash entry */
    new_entry->alu_info.alu_load_index = *alu_load_index;
    new_entry->alu_info.alu_load_type  = alu_load_type;

    bcmi_ft_group_alu_load_info_set(unit, id, alu_load_type,
                                    group_alu_info, load_indexes,
                                    *alu_load_index, &(new_entry->alu_info));

    /* update the link table */
    temp = hash_input->head_link;
    if (hash_input->head_link == NULL) {
        hash_input->head_link = new_entry;
    } else {
        while(temp->link != NULL) {
            temp = temp->link;
        }
        temp->link = new_entry;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu_hash_entry_remove
 * Purpose:
 *   Remove hash entry from hash table.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   alu_load_type - (IN) ALU/LOAD type.
 *   group_alu_info - (IN) ALU information of group.
 *   hash_input - (IN) Bucket input to add hash entry.
 *   indexes  - (IN) Load 8 indexes in group
 *   alu_load_index - (OUT) index of ALU/LOAD memory.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_alu_hash_entry_remove(int unit,
                bcm_flowtracker_group_t id,
                bcmi_ft_alu_load_type_t alu_load_type,
                bcmi_ft_group_alu_info_t *group_alu_info,
                bcmi_ft_alu_table_hash_t *hash_input,
                int *indexes,
                int *alu_load_index)
{
    bcmi_ft_alu_entry_list_t *prev = NULL;
    bcmi_ft_alu_entry_list_t *temp = NULL;
    bcmi_ft_group_alu_info_t *temp_group_alu_info = NULL;

    if (hash_input == NULL) {
        return BCM_E_INIT;
    }

    prev = temp = hash_input->head_link;

    while (temp != NULL) {

        if (BCM_SUCCESS(bcmi_ft_alu_load_compare(unit, id, group_alu_info,
                            alu_load_type, (&temp->alu_info), indexes))) {

            if ((alu_load_type == bcmiFtAluLoadTypeAlu32) ||
                (alu_load_type == bcmiFtAluLoadTypeAlu16) ||
                (alu_load_type == bcmiFtAluLoadTypeAggAlu32)) {

                if (temp->alu_info.alu_load_index ==
                                               group_alu_info->alu_load_index) {
                    *alu_load_index = temp->alu_info.alu_load_index;

                    bcmi_ft_flowchecker_list_delete(unit,
                                (&(temp->alu_info.head)),
                                   group_alu_info->flowchecker_id);

                    if (hash_input->head_link == temp) {
                        /* If matched node is head then point head to next.*/
                        hash_input->head_link = temp->link;
                    } else {
                        /* If matched node is in middle then adjust the chain.*/
                        prev->link = temp->link;
                    }

                    sal_free(temp);
                    temp = NULL;
                    return BCM_E_NONE;
                }

            } else {

                if ((indexes != NULL) && (indexes[0] != -1)) {

                    temp_group_alu_info = &group_alu_info[indexes[0]];

                    if (temp->alu_info.alu_load_index ==
                            temp_group_alu_info->alu_load_index) {

                        *alu_load_index = temp->alu_info.alu_load_index;

                        if (hash_input->head_link == temp) {
                            /* If matched node is head then point head to next.*/
                            hash_input->head_link = temp->link;
                        } else {
                            /* If matched node is in middle then adjust the chain.*/
                            prev->link = temp->link;
                        }

                        sal_free(temp);
                        temp = NULL;
                        return BCM_E_NONE;
                    }
                }
            }
        }

        prev = temp;
        temp = temp->link;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ft_alu_load_compare
 * Purpose:
 *   Compare ALU/LOAD memories
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) FT group id.
 *   group_alu_info - (IN) ALU information of group.
 *   alu_load_type  - (IN) ALU/LOAD type.
 *   hash_input     - (IN) Bucket input to add hash entry.
 *   alu_info       - (IN) ALU hash information.
 *   indexes        - (IN) Load8/16 indexes in group
 * Notes:
 *   this will called for all entries in a particular
 *   hash index with different alu_info from all
 *   entries in hash bucket
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_load_compare(int unit,
            bcm_flowtracker_group_t id,
            bcmi_ft_group_alu_info_t *group_alu_info,
            bcmi_ft_alu_load_type_t alu_load_type,
            bcmi_ft_alu_hash_info_t *alu_info,
            int *indexes)
{
    if (alu_info == NULL) {
        return BCM_E_PARAM;
    }

    if (alu_load_type != alu_info->alu_load_type) {
        return BCM_E_NOT_FOUND;
    }

    if (bcmi_ft_method_match(alu_load_type)) {
        return ((*(bcmi_ft_method_match(alu_load_type)))
            (unit, id, group_alu_info, alu_info, indexes,
            (&(alu_info->alu_load_index))));
    } else {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "No matching function is setup for %s \n"),
            alu_type_str[alu_load_type]));

        return BCM_E_INTERNAL;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ft_hash_bucket_get
 * Purpose:
 *   Get hash bucket for the entry.
 * Parameters:
 *   unit - (IN) BCM device id
 *   alu_load_type - (IN) ALU/LOAD type.
 *   group_alu_info - (IN) ALU information of group.
 *   load_indexes  - (IN) Load 8 indexes in group
 *   idx - (OUT) sw bucket index.
 * Returns:
 *   None
 */
STATIC void
bcmi_ft_hash_bucket_get(int unit,
                bcmi_ft_alu_load_type_t alu_load_type,
                bcmi_ft_group_alu_info_t *group_alu_info,
                int *load_indexes,
                int *idx)
{
    int j = 0;
    int max_entry = 0;
    bcmi_ft_group_alu_info_t *local = NULL;
    int key[BCMI_FT_ALU_HASH_KEY_LEN_IN_WORDS];
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;

    max_entry = ((alu_load_type == bcmiFtAluLoadTypeLoad16) ?
                      TOTAL_GROUP_LOAD16_DATA_NUM : TOTAL_GROUP_LOAD8_DATA_NUM);
    sal_memset(key, 0, sizeof(key));

    if ((alu_load_type == bcmiFtAluLoadTypeLoad16) ||
        (alu_load_type == bcmiFtAluLoadTypeLoad8)) {

        for (; j < max_entry; j++) {

            /* Now based on the incoming info,
             * match the entries. */
            if (load_indexes[j] == -1) {
                break;
            }

            local = (&(group_alu_info[load_indexes[j]]));

            key[0] += local->key1.location;
            key[1] += local->key1.length;
            key[2] += local->key1.is_alu;
            key[3] += local->key2.location;
            key[4] += local->key2.length;
            key[5] += local->key2.is_alu;
            key[6] = group_alu_info->element_type1;
            key[7] = group_alu_info->custom_id1;
        }

    } else {

        key[0] = group_alu_info->key1.location;
        key[1] = group_alu_info->key1.length;
        key[2] = group_alu_info->key1.is_alu;
        key[3] = group_alu_info->key2.location;
        key[4] = group_alu_info->key2.length;
        key[5] = group_alu_info->key2.is_alu;
        key[6] = group_alu_info->element_type1;
        key[7] = group_alu_info->custom_id1;
    }

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
        "Printing Hash Key: Location= <%d><%d>, Length= <%d><%d>, "
        "is_alu= <%d><%d>, ElementType1 <%s>\n"),
        key[0], key[3], key[1], key[4], key[2], key[5], plist[key[6]]));

    *idx = _shr_crc16b(0, (uint8*)key,
            sizeof(int)* BYTES2BITS(BCMI_FT_ALU_HASH_KEY_LEN_IN_WORDS)) %
            BCMI_FT_ALU_HASH_BUCKETS;
}

/*
 * Function:
 *   bcmi_ft_alu_hash_insert
 * Purpose:
 *   Add ALU/LOAD hash entry into hash table.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) FT group id.
 *   alu_load_type  - (IN) ALU/LOAD type.
 *   group_alu_info - (IN) ALU information of group.
 *   load_indexes   - (IN) Load8/16 indexes in group.
 *   alu_load_index - (OUT) sw index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_hash_insert(int unit,
                    bcm_flowtracker_group_t id,
                    bcmi_ft_alu_load_type_t alu_load_type,
                    bcmi_ft_group_alu_info_t *group_alu_info,
                    int *load_indexes,
                    int *alu_load_index)
{
    int rv = BCM_E_NONE;
    int hash_idx = 0;
    bcmi_ft_alu_table_hash_t *hash_input = NULL;

    /* First get the index of bucket.*/
    bcmi_ft_hash_bucket_get(unit, alu_load_type,
                            group_alu_info, load_indexes,
                            &hash_idx);

    /* Get the bucket from the hash table. */
    hash_input = &(bcmi_ft_alu_table_hash[unit][hash_idx]);

    /* Now add the index and type into the alu hash table. */
    rv = bcmi_ft_alu_hash_entry_add(unit, id, alu_load_type, group_alu_info,
                                    load_indexes, hash_input, alu_load_index);

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit,
        "Hash_idx = %d, alu_load_index = %d, type = %s\n"),
        hash_idx, *alu_load_index, alu_type_str[alu_load_type]));

    return rv;
}

/*
 * Function:
 *   bcmi_ft_alu_hash_remove
 * Purpose:
 *   Remove ALU/LOAD hash entry from hash table.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) FT group id.
 *   alu_load_type  - (IN) ALU/LOAD type.
 *   group_alu_info - (IN) ALU information of group.
 *   load_indexes   - (IN) Load8/16 indexes in group
 *   alu_load_index - (OUT) sw index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_hash_remove(int unit,
                    bcm_flowtracker_group_t id,
                    bcmi_ft_alu_load_type_t alu_load_type,
                    bcmi_ft_group_alu_info_t *group_alu_info,
                    int *load_indexes, int *alu_load_index)
{
    int rv = BCM_E_NONE;
    int hash_idx = 0;
    bcmi_ft_alu_table_hash_t *hash_input = NULL;

    /* First get the index of bucket.*/
    bcmi_ft_hash_bucket_get(unit, alu_load_type,
                            group_alu_info, load_indexes,
                            &hash_idx);

    /* Get the bucket from the hash table. */
    hash_input = &(bcmi_ft_alu_table_hash[unit][hash_idx]);

    /* We have got the hash bucket. Now remove the ALU from this list. */
    rv = bcmi_ft_alu_hash_entry_remove(unit, id, alu_load_type, group_alu_info,
                                      hash_input, load_indexes, alu_load_index);

    return rv;
}

/*
 * Function:
 *   bcmi_ft_alu_hash_entry_index_get
 * Purpose:
 *   Get hash entry index.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) FT group id.
 *   alu_load_type  - (IN) ALU/LOAD type.
 *   group_alu_info - (IN) ALU information of group.
 *   hash_input     - (IN) Bucket input to add hash entry.
 *   indexes        - (IN) Load8/16 indexes in group
 *   alu_load_index - (OUT) sw index.
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_alu_hash_entry_index_get(int unit,
                    bcm_flowtracker_group_t id,
                    bcmi_ft_alu_load_type_t alu_load_type,
                    bcmi_ft_group_alu_info_t *group_alu_info,
                    bcmi_ft_alu_table_hash_t *hash_input,
                    int *indexes, int *alu_load_index)
{
    bcmi_ft_alu_entry_list_t *temp = NULL;

    if (hash_input == NULL) {
        return BCM_E_INIT;
    }

    temp = hash_input->head_link;

    while (temp != NULL) {
        if (BCM_SUCCESS(bcmi_ft_alu_load_compare
                (unit, id, group_alu_info, alu_load_type,
                (&temp->alu_info), indexes))) {

            *alu_load_index = temp->alu_info.alu_load_index;

            return BCM_E_NONE;
        }
        temp = temp->link;
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *   bcmi_ft_alu_hash_mem_index_get
 * Purpose:
 *   Get hash index for this group with matching alu info.
 * Parameters:
 *   unit           - (IN) BCM device id
 *   id             - (IN) FT group id.
 *   alu_load_type  - (IN) ALU/LOAD type.
 *   group_alu_info - (IN) ALU information of group.
 *   load_indexes   - (IN) Load 8 indexes in group
 *   alu_load_index - (OUT) sw index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_hash_mem_index_get(int unit,
                    bcm_flowtracker_group_t id,
                    bcmi_ft_alu_load_type_t alu_load_type,
                    bcmi_ft_group_alu_info_t *group_alu_info,
                    int *load_indexes,
                    int *alu_load_index)
{
    int rv = BCM_E_NONE;
    int hash_idx = 0;
    bcmi_ft_alu_table_hash_t *hash_input = NULL;

    /* First get the index of bucket.*/
    bcmi_ft_hash_bucket_get(unit, alu_load_type,
                            group_alu_info, load_indexes,
                            &hash_idx);

    /* Get the bucket from the hash table. */
    hash_input = &(bcmi_ft_alu_table_hash[unit][hash_idx]);

    /* We have got the hash bucket. Now remove the ALU from this list. */
    rv = bcmi_ft_alu_hash_entry_index_get(unit, id, alu_load_type,
                                          group_alu_info, hash_input,
                                          load_indexes, alu_load_index);

    LOG_INFO(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit,
        "Hash_idx = %d, alu_load_index = %d, type = %s\n"),
        hash_idx, *alu_load_index, alu_type_str[alu_load_type]));

    return rv;
}

/*
 * Function:
 *   bcmi_ft_alu_check16_mapping_get
 * Purpose:
 *   Get flowcheck mapping in ALU16 memory.
 * Parameters:
 *   unit      - (IN) BCM device id
 *   id        - (IN) Flowtracker Group Id
 *   check_map - (OUT) Flowtracker Check in ALU16.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_check16_mapping_get(int unit,
                    bcm_flowtracker_group_t id,
                    bcm_flowtracker_check_t *check_map)
{
    int iter = 0, a_idx = 0;
    bcmi_ft_alu_load_type_t alu_load_type;
    bcm_flowtracker_check_t flowchecker_id;
    int alu_load_index = 0, num_data_info = 0;
    bcmi_ft_group_alu_info_t *ext_data_info = NULL;

    /* If FT Group is not validated, fail */
    if (!BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
        return BCM_E_CONFIG;
    }

    ext_data_info = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);
    num_data_info = BCMI_FT_GROUP_EXT_INFO(unit, id).num_data_info;

    for (iter = 0; iter < num_data_info; iter++) {

        alu_load_type = ext_data_info[iter].alu_load_type;

        if (alu_load_type != bcmiFtAluLoadTypeAlu16) {
            continue;
        }

        alu_load_index = ext_data_info[iter].alu_load_index;
        flowchecker_id = ext_data_info[iter].flowchecker_id;

        bcmi_ft_alu_load_mem_index_get(unit, alu_load_index,
                            alu_load_type, NULL, NULL, &a_idx);
        check_map[a_idx] = flowchecker_id;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu_load_hash_dump
 * Purpose:
 *   Dump ALU/LOAD hash allocation and reference counts.
 * Parameters:
 *   unit - (IN) BCM device id
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu_load_hash_dump(int unit)
{
    int i = 0, j = 0;
    int total_indexes_per_mem = 0;
    bcmi_ft_alu_table_hash_t *alu_hash = NULL;
    bcmi_ft_alu_entry_list_t *alu_entry = NULL;
    const char *tlist[] = BCMI_FT_TYPE_STRINGS;
    char *strlist[] = {"LOAD8", "LOAD16", "ALU16", "ALU32", "AGG_ALU32"};

    LOG_CLI((BSL_META_U(unit, "\nALU/LOAD Hash info\n")));
    if (NULL != bcmi_ft_alu_table_hash[unit]) {
        for (i = 0; i < BCMI_FT_ALU_HASH_BUCKETS; i++) {
            alu_hash = (&(bcmi_ft_alu_table_hash[unit][i]));

            if (alu_hash->head_link != NULL) {
                LOG_CLI((BSL_META_U(unit, "Hash Index = %d\n"), i));
                alu_entry = alu_hash->head_link;
                while(alu_entry != NULL) {
                    LOG_CLI((BSL_META_U(unit, "LoadType: %s, Stage %s, Idx: %d,"
                       " isAlu %d, Key: (%d - %d / %d(%d)), load_vector 0x%08x\n"),
                                strlist[alu_entry->alu_info.alu_load_type],
                                tlist[alu_entry->alu_info.hash_ft_type],
                                alu_entry->alu_info.alu_load_index,
                                alu_entry->alu_info.key.is_alu,
                                alu_entry->alu_info.key.location,
                                alu_entry->alu_info.key.length,
                                alu_entry->alu_info.param,
                                alu_entry->alu_info.custom_id,
                                alu_entry->alu_info.hash_load_mem_vector));
                    alu_entry = alu_entry->link;
                }
            }
        }
    }

    LOG_CLI((BSL_META_U(unit, "\nALU/LOAD Reference Count info\n")));
    for (i = bcmiFtAluLoadTypeLoad8; i < bcmiFtAluLoadTypeNone; i++) {

        total_indexes_per_mem = BCMI_FT_ALU_LOAD_MEM_TOTAL_INDEX(unit, i);

        LOG_CLI((BSL_META_U(unit, "%-6s : "), strlist[i]));
        for (j = 0; j < total_indexes_per_mem; j++) {
            if (BCMI_FT_ALU_LOAD_REFCOUNT(unit, i, j) != 0) {
                LOG_CLI((BSL_META_U(unit, "%d(%d), "), j,
                            BCMI_FT_ALU_LOAD_REFCOUNT(unit, i, j)));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    return BCM_E_NONE;
}

#else /* BCM_FLOWTRACKER_SUPPORT*/
typedef int bcmi_ft_alu_load_sw_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT */
