/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    rh.c
 * Purpose: Tomahawk / Trident3 Resilient Hashing function implementations
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#if defined(INCLUDE_L3) && defined(BCM_TOMAHAWK_SUPPORT) 

#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <soc/tomahawk.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */

#define BSL_LSS_SCACHE (BSL_LS_SHARED_SCACHE | BSL_VERBOSE)

/* RH info per ecmp group */
typedef struct _th_ecmp_rh_group_s {
    bcm_if_t *rh_intf_arr; /* ECMP members */
    uint16 data_hash;      /* data hash of sorted ECMP member list */
    int rh_intf_count;     /* Number of ECMP members */
    uint16 max_paths;     /* Number of ECMP members */
    uint8 enable;
} _ecmp_rh_group_t;

/* Bookkeeping info for ECMP resilient hashing */
typedef struct _th_ecmp_rh_info_s {
    uint32 ecmp_rh_rand_seed; /* The seed for pseudo-random number generator */
    _ecmp_rh_group_t *rhg;
} _opt_ecmp_rh_info_t;

STATIC _opt_ecmp_rh_info_t *_opt_ecmp_rh_info[BCM_MAX_NUM_UNITS];

#if defined(BCM_TOMAHAWK_SUPPORT)
extern int ecmp_mode_hierarchical;
#endif

/* RH member */
typedef struct _ecmp_rh_member_s {
    int nh_index; /* Next hop index of the member */
    int member_id; /* Member ID */
    int num_replica; /* Number of members with the same next hop index as
                        this member. Valid only for the first replica. */
    int replica_id; /* Index among members with the same next hop index as
                       this member. Valid for every replica. */
    int next_replica_id; /* Index of the next replica to be assigned to
                            a rh set entry containing the next hop index
                            shared by replicas. Valid only for the first
                            replica. */
} _ecmp_rh_member_t;

/*----- STATIC FUNCS ----- */
STATIC int
_bcm_opt_rh_ecmp_grp_hash_calc(int unit, void *buf, uint16 *hash)
{

    if ((NULL == buf) || (NULL == hash)) {
        return (BCM_E_PARAM);
    }

    /* Calculate hash of next hop indexes, which are members in the group. */
    *hash = _shr_crc16(0, (uint8 *) buf,
                       BCM_XGS3_L3_ECMP_MAX_PATHS(unit) * sizeof(int));

    return (BCM_E_NONE);
}

/*
* Compare function used to sort ECMP members
*/
static INLINE int
_opt_rh_cmp_int(void *a, void *b)
{
    int first;                  /* First compared integer. */
    int second;                 /* Second compared integer. */

    first = *(int *)a;
    second = *(int *)b;

    if (first < second) {
        return (BCM_L3_CMP_LESS);
    } else if (first > second) {
        return (BCM_L3_CMP_GREATER);
    }
    return (BCM_L3_CMP_EQUAL);
}

/*
 * Function:
 *      bcm_opt_ecmp_rh_deinit
 * Purpose:
 *      Deallocate ECMP resilient hashing internal data structures
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_opt_ecmp_rh_deinit(int unit)
{
    int i;
    bcm_if_t **rh_intf_arr_ptr;
    if (_opt_ecmp_rh_info[unit]) {
        if (_opt_ecmp_rh_info[unit]->rhg) {
            for (i = 0; i < BCM_XGS3_L3_ECMP_MAX_GROUPS(unit); i++) {
                /* Free ECMP member array */
                rh_intf_arr_ptr = &(_opt_ecmp_rh_info[unit]->rhg[i].rh_intf_arr);
                if (*rh_intf_arr_ptr) {
                    sal_free(*rh_intf_arr_ptr);
                    *rh_intf_arr_ptr = NULL;
                }
            }
            sal_free(_opt_ecmp_rh_info[unit]->rhg);
        }
        sal_free(_opt_ecmp_rh_info[unit]);
        _opt_ecmp_rh_info[unit] = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_opt_ecmp_rh_init
 * Purpose:
 *      Initialize ECMP resilient hashing internal data structures
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_opt_ecmp_rh_init(int unit)
{
    int rv = BCM_E_NONE;

    bcm_opt_ecmp_rh_deinit(unit);

    _opt_ecmp_rh_info[unit] = sal_alloc(sizeof(_opt_ecmp_rh_info_t),
            "_opt_ecmp_rh_info");
    if (_opt_ecmp_rh_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_opt_ecmp_rh_info[unit], 0, sizeof(_opt_ecmp_rh_info_t));

    _opt_ecmp_rh_info[unit]->rhg = sal_alloc(
            sizeof(_ecmp_rh_group_t) * BCM_XGS3_L3_ECMP_MAX_GROUPS(unit),
            "_opt_ecmp_rh_info");
    if (_opt_ecmp_rh_info[unit]->rhg == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_opt_ecmp_rh_info[unit]->rhg, 0,
              sizeof(_ecmp_rh_group_t) * BCM_XGS3_L3_ECMP_MAX_GROUPS(unit));

    /* Set the seed for the pseudo-random number generator */
    _opt_ecmp_rh_info[unit]->ecmp_rh_rand_seed = sal_time_usecs();

    return rv;
}

/*
 * Function:
 *     bcm_opt_ecmp_rh_set_intf_arr 
 * Purpose:
 *     Store ECMP members in the rh info data structure 
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      intf_count - (IN) ECMP member count.
 *      intf_array - (IN) ECMP member array.
 *      ecmp_group_idx - (IN) ECMP group ID.
 *      group_size - (IN) RH ECMP group size.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_opt_ecmp_rh_set_intf_arr(int unit,
                            int intf_count,
                            bcm_if_t *intf_array,
                            int ecmp_group_idx,
                            int group_size,
                            int max_paths)
{
    bcm_if_t **rh_intf_arr_ptr = NULL;
    uint16 hash;
    bcm_if_t *hash_intf_array = NULL;
    int alloc_size;

    if (intf_array == NULL) {
        return(BCM_E_INTERNAL);
    }

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_group_idx +
                                                 BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit))) {
        rh_intf_arr_ptr = &(_opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].rh_intf_arr);
        if (*rh_intf_arr_ptr) {
            sal_free(*rh_intf_arr_ptr);
            *rh_intf_arr_ptr = NULL;
        }
        *rh_intf_arr_ptr = sal_alloc(sizeof(bcm_if_t) * intf_count,
            "ECMP RH entry count array");
        if (NULL == *rh_intf_arr_ptr) {
            return(BCM_E_MEMORY);
        }
        sal_memset(*rh_intf_arr_ptr, 0, (sizeof(bcm_if_t) * intf_count));
        /* Copy ECMP member count */
        _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].rh_intf_count = intf_count;
        _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].max_paths = max_paths;

        alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
        hash_intf_array = sal_alloc(alloc_size, "RH intf array");
        if (NULL == hash_intf_array) {
            return(BCM_E_MEMORY);
        }
        sal_memset(hash_intf_array, 0, sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit));
        sal_memcpy(hash_intf_array, intf_array, sizeof(bcm_if_t) * intf_count);
        /* Sort ECMP member array */
        _shr_sort(hash_intf_array, intf_count, sizeof(int), _opt_rh_cmp_int);
        /* Copy sorted ECMP member array */
        sal_memcpy(*rh_intf_arr_ptr, hash_intf_array, 
                            (sizeof(bcm_if_t) * intf_count));     
        /* Compute member hash */
        _bcm_opt_rh_ecmp_grp_hash_calc(unit, hash_intf_array, &hash);
        _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].data_hash = hash;
        sal_free(hash_intf_array);
    }
    return (BCM_E_NONE); 
}

/*
 * Function:
 *     bcm_th_ecmp_group_rh_set
 * Purpose:
 *     Set/reset the given ECMP group as a RH group
 * Parameters:
 *     unit - (IN) SOC unit number.
 *     ecmp_group_idx - (IN) ECMP group ID.
 *     enable - (IN) Enable
 * Returns:
 *     TRUE or FALSE
 */
void
bcm_th_ecmp_group_rh_set(int unit, int ecmp_group_idx, int enable)
{
    if (ecmp_group_idx > BCM_XGS3_L3_ECMP_MAX_GROUPS(unit)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                   (BSL_META_U(unit,
                   "Group index %d out of bound\n"), ecmp_group_idx));
        return;
    }

    /* coverity[overrun-local : FALSE] */
    _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].enable = enable ? 1 : 0;
    return;
}

/*
 * Function:
 *     bcm_opt_ecmp_group_is_rh
 * Purpose:
 *    Check whether the given ECMP group is a RH group
 * Parameters:
 *     unit - (IN) SOC unit number.
 *     ecmp_group_idx - (IN) ECMP group ID.
 * Returns:
 *     TRUE or FALSE
 */
int
bcm_opt_ecmp_group_is_rh(int unit, int ecmp_group_idx)
{
    if (!soc_feature(unit, soc_feature_ecmp_resilient_hash_optimized)) {
        return FALSE;
    }
    if (ecmp_group_idx > BCM_XGS3_L3_ECMP_MAX_GROUPS(unit)) {
        LOG_ERROR(BSL_LS_BCM_L3,
                   (BSL_META_U(unit,
                   "Group index %d out of bound\n"), ecmp_group_idx));
        return FALSE;
    }

    /* coverity[overrun-local : FALSE] */
    return _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].enable ? TRUE : FALSE;
}

/*
 * Function:
 *     bcm_opt_ecmp_rh_multipath_get 
 * Purpose:
 *     Get ECMP members and member count for the group
 * Parameters:
 *     unit - (IN) SOC unit number.
 *     ecmp_group_idx - (IN) ECMP group ID.
 *     intf_arr - (OUT) ECMP member array.
 *     intf_count - (OUT) ECMP member count.
 *     max_paths - (OUT) ECMP max paths.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_opt_ecmp_rh_multipath_get(int unit, int ecmp_group_idx, int intf_size,
                             bcm_if_t *intf_arr, int *intf_count, int *max_paths)
{
    bcm_if_t *intf_arr_ptr;

    if (intf_size && intf_count == NULL) {
        return BCM_E_PARAM;
    }

    intf_arr_ptr = _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].rh_intf_arr;
    if (intf_arr_ptr) {
        if (intf_count) {
            *intf_count = _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].rh_intf_count;
        }
        if (max_paths) {
            *max_paths = _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].max_paths;
        }
        if (intf_size == 0) {
            return (BCM_E_NONE);
        } else if (intf_size < *intf_count) {
            *intf_count = intf_size;
        }
        if (intf_arr) {
            sal_memcpy(intf_arr, intf_arr_ptr, 
                                 (sizeof(bcm_if_t) * (*intf_count)));     
        }
    } else {
        return (BCM_E_INTERNAL);
    }
    
    return (BCM_E_NONE); 
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_free_resource
 * Purpose:
 *      Free resources for an ECMP resilient hashing group.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      ecmp_group_idx - (IN) ECMP group ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_opt_ecmp_rh_free_resource(int unit, int ecmp_group_idx)
{
    ecmp_count_entry_t ecmp_count_entry;
    initial_l3_ecmp_group_entry_t initial_l3_ecmp_group_entry;
    bcm_if_t **intf_arr_ptr;
    soc_mem_t mem = L3_ECMP_COUNTm;

    if (!BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp_group_idx +
                                                 BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit))) {
            return(BCM_E_PARAM);
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_l3_ecmp_hier_tbl) &&
        (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
             ecmp_mode_hierarchical) &&
        (ecmp_group_idx < (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) / 2))) {
        mem = ECMP_GROUP_HIERARCHICALm;
    }
#endif

    /* Read group entry from hw to check if load balancing mode is RH */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                ecmp_group_idx, &ecmp_count_entry));
    if (BCM_TH_L3_ECMP_LB_MODE_RH != soc_mem_field32_get(unit, mem,
            &ecmp_count_entry, LB_MODEf)) {
        /* Resilient hashing is not enabled on this ECMP group. */
        return BCM_E_NONE;
    }
    /* Clear resilient hashing lb mode field */
    soc_mem_field32_set(unit, mem, &ecmp_count_entry,
            LB_MODEf, 0);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                ecmp_group_idx, &ecmp_count_entry));

    if (SOC_MEM_IS_VALID(unit, INITIAL_L3_ECMP_GROUPm)) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, INITIAL_L3_ECMP_GROUPm, MEM_BLOCK_ANY,
            ecmp_group_idx, &initial_l3_ecmp_group_entry));
        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
            &initial_l3_ecmp_group_entry, LB_MODEf, 0);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_L3_ECMP_GROUPm,
            MEM_BLOCK_ALL, ecmp_group_idx, &initial_l3_ecmp_group_entry));
    }

    /* Free ECMP member array */
    intf_arr_ptr = &(_opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].rh_intf_arr);
    if (*intf_arr_ptr) {
      sal_free(*intf_arr_ptr);
      *intf_arr_ptr = NULL;
    }
    _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].data_hash = 0;
    _opt_ecmp_rh_info[unit]->rhg[ecmp_group_idx].rh_intf_count = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_rand_get
 * Purpose:
 *      Get a random number between 0 and the given max value.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      rand_max - (IN) Maximum random number.
 *      rand_num - (OUT) Random number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      This procedure uses the pseudo-random number generator algorithm
 *      suggested by the C standard.
 */
STATIC int
_bcm_opt_ecmp_rh_rand_get(int unit, int rand_max, int *rand_num)
{
    int modulus;
    int rand_seed_shift;

    if (rand_max < 0) {
        return BCM_E_PARAM;
    }

    if (NULL == rand_num) {
        return BCM_E_PARAM;
    }

    /* Make sure the modulus does not exceed limit. For instance,
     * if the 32-bit rand_seed is shifted to the right by 16 bits before
     * the modulo operation, the modulus should not exceed 1 << 16.
     */
    modulus = rand_max + 1;
    rand_seed_shift = 16;
    if (modulus > (1 << (32 - rand_seed_shift))) {
        return BCM_E_PARAM;
    }

    _opt_ecmp_rh_info[unit]->ecmp_rh_rand_seed =
        _opt_ecmp_rh_info[unit]->ecmp_rh_rand_seed * 1103515245 + 12345;

    *rand_num = (_opt_ecmp_rh_info[unit]->ecmp_rh_rand_seed >> rand_seed_shift) %
                modulus;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_opt_l3_egress_rh_ecmp_find 
 * Purpose:
 *      Find resilient hashing group given its members.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      intf_count - (IN) ECMP member count.
 *      intf_array - (IN) ECMP member array.
 *      mpintf - (OUT) ECMP multipath group ID.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_opt_l3_egress_rh_ecmp_find(int unit, int intf_count,
                              bcm_if_t *intf_array, bcm_if_t *mpintf)
{
    uint16 hash;
    int rv = BCM_E_NONE;
    bcm_if_t *hash_intf_array = NULL;
    int alloc_size;
    int i;

    /* Allocate  member array */
    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
    hash_intf_array = sal_alloc(alloc_size, "RH intf array");
    if (NULL == hash_intf_array) {
        return(BCM_E_MEMORY);
    }
    sal_memset(hash_intf_array, 0, sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit));
    /* Copy ECMP members */
    sal_memcpy(hash_intf_array, intf_array, sizeof(bcm_if_t) * intf_count);
    /* Sort ECMP members in the member array */
    _shr_sort(hash_intf_array, intf_count, sizeof(int), _opt_rh_cmp_int);
    /* Compute hash on the sorted member array */
    _bcm_opt_rh_ecmp_grp_hash_calc(unit, hash_intf_array, &hash);
    i = 0;
    if (_opt_ecmp_rh_info[unit]) {
        for (i = 0; i < BCM_XGS3_L3_ECMP_MAX_GROUPS(unit); i++) {
            if (_opt_ecmp_rh_info[unit]->rhg[i].rh_intf_arr) {
                if ((hash == _opt_ecmp_rh_info[unit]->rhg[i].data_hash) &&
                     (intf_count == _opt_ecmp_rh_info[unit]->rhg[i].rh_intf_count)) {
                    /* Compare ecmp groups. */
                    if (sal_memcmp(_opt_ecmp_rh_info[unit]->rhg[i].rh_intf_arr,
                                   hash_intf_array, (intf_count * sizeof(int))) == 0) {
                        *mpintf = i + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                        break;
                    }
                }
            }
        }
    }
    sal_free(hash_intf_array);
    if (i == BCM_XGS3_L3_ECMP_MAX_GROUPS(unit)) {
        rv = BCM_E_NOT_FOUND;
    }
    return rv;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_member_choose
 * Purpose:
 *      Choose a member of the ECMP group.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      num_members - (IN) Number of members to choose from.
 *      entry_count_arr   - (IN/OUT) An array keeping track of how many
 *                                   rh set entries have been assigned
 *                                   to each member.
 *      max_entry_count   - (IN/OUT) The maximum number of rh set entries
 *                                   that can be assigned to a member. 
 *      chosen_index      - (OUT) The index of the chosen member.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_opt_ecmp_rh_member_choose(int unit, int num_members, int *entry_count_arr,
                              int *max_entry_count, int *chosen_index)
{
    int member_index;
    int next_index;

    *chosen_index = 0;

    /* Choose a random member index */
    BCM_IF_ERROR_RETURN(_bcm_opt_ecmp_rh_rand_get(unit, num_members - 1,
                &member_index));

    if (entry_count_arr[member_index] < *max_entry_count) {
        entry_count_arr[member_index]++;
        *chosen_index = member_index;
    } else {
        /* The randomly chosen member has reached the maximum
         * rh set entry count. Choose the next member that
         * has not reached the maximum entry count.
         */
        next_index = (member_index + 1) % num_members;
        while (next_index != member_index) {
            if (entry_count_arr[next_index] < *max_entry_count) {
                entry_count_arr[next_index]++;
                *chosen_index = next_index;
                break;
            } else {
                next_index = (next_index + 1) % num_members;
            }
        }
        if (next_index == member_index) {
            /* All members have reached the maximum rh set entry
             * count. This scenario occurs when dividing the number of
             * rh set entries by the number of members results
             * in a non-zero remainder. The remainder rh set entries
             * will be distributed among members, at most 1 remainder
             * entry per member.
             */
            (*max_entry_count)++;
            if (entry_count_arr[member_index] < *max_entry_count) {
                entry_count_arr[member_index]++;
                *chosen_index = member_index;
            } else {
                /* It's possible that the member's entry count equals
                 * to the incremented value of max_entry_count, when
                 * this procedure is invoked by
                 * _bcm_opt_ecmp_rh_populate_empty_entries.
                 */
                next_index = (member_index + 1) % num_members;
                while (next_index != member_index) {
                    if (entry_count_arr[next_index] < *max_entry_count) {
                        entry_count_arr[next_index]++;
                        *chosen_index = next_index;
                        break;
                    } else {
                        next_index = (next_index + 1) % num_members;
                    }
                }
                if (next_index == member_index) {
                    return BCM_E_INTERNAL;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_member_replica_find
 * Purpose:
 *      For each member in the member_array, find others members
 *      who share the same next hop index.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_members - (IN) Number of members.
 *      member_array - (IN/OUT) Array of members.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_opt_ecmp_rh_member_replica_find(int unit, int num_members,
                                    _ecmp_rh_member_t *member_array)
{
    int i, k;

    if (num_members < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == member_array) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < num_members - 1; i++) {
        if (member_array[i].replica_id == 0) {
            /* replica_id == 0 indicates first replica.
             * Search for other replicas in subsequent members.
             */
            for (k = i + 1; k < num_members; k++) {
                if (member_array[k].nh_index == member_array[i].nh_index) {
                    /* Replica found. Assign replica id. */
                    member_array[k].replica_id = member_array[i].num_replica;

                    /* For non-first replicas, set non-applicable fields to -1. */
                    member_array[k].num_replica = -1;
                    member_array[k].next_replica_id = -1;

                    /* Increment the num_replica field of the first replica */
                    member_array[i].num_replica++;
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_add_rebalance
 * Purpose:
 *      Re-balance rh set entries from existing members to the new member.
 *      For example, if the number of rh set entries is 64, and the number of
 *      existing members is 6, then each existing member has between 10 and 11
 *      entries. The entries should be re-assigned from the 6 existing members
 *      to the new member such that each member will end up with between 9 and
 *      10 entries.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of rh set entries in buffer.
 *      rh_intf_array - (IN/OUT) intf rh set array entry buffer.
 *      member_id_buf - (IN/OUT) Member ID buffer.
 *      num_members - (IN) Number of members in member_array.
 *      member_array - (IN) Array of existing members.
 *      entry_count_array - (IN/OUT) Array of entry counts of existing members.
 *      new_member - (IN) The member to be added.
 *      entry_count - (OUT) Entry count of the new member.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_opt_ecmp_rh_add_rebalance(int unit,
                              int num_entries, int overlay, bcm_if_t *rh_intf_array,
                              int *member_id_buf, int num_members,
                              _ecmp_rh_member_t *member_array,
                              int *entry_count_array,
                              _ecmp_rh_member_t *new_member,
                              int *entry_count)
{
    int lower_bound, upper_bound;
    int threshold;
    int entry_index, next_entry_index;
    int member_id;
    int is_new_member;
    int member_index = 0;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == member_id_buf) {
        return BCM_E_PARAM;
    }
    if (num_members < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == member_array) {
        return BCM_E_PARAM;
    }
    if (NULL == entry_count_array) {
        return BCM_E_PARAM;
    }
    if (NULL == new_member) {
        return BCM_E_PARAM;
    }
    if (NULL == entry_count) {
        return BCM_E_PARAM;
    }
    
    lower_bound = num_entries / (num_members + 1);
    upper_bound = (num_entries % (num_members + 1)) ?
                  (lower_bound + 1) : lower_bound;
    threshold = upper_bound;
    *entry_count = 0;
    while (*entry_count < lower_bound) {
        /* Pick a random entry in the rh set entry buffer */
        BCM_IF_ERROR_RETURN
            (_bcm_opt_ecmp_rh_rand_get(unit, num_entries - 1, &entry_index));
        member_id = member_id_buf[entry_index];

        /* Determine if the randomly picked entry contains the new member.
         * If not, determine the existing member index.
         */
        if (member_id == new_member->member_id) {
            is_new_member = TRUE;
        } else {
            is_new_member = FALSE;

            /* In the member_array, it's assumed that each array element's
             * member_id matches the element's array index.
             */
            member_index = member_id;
        }
        if (!is_new_member && (entry_count_array[member_index] > threshold)) {
            if (overlay) {
                rh_intf_array[entry_index] = new_member->nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
            } else {
                rh_intf_array[entry_index] = new_member->nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
            }
            member_id_buf[entry_index] = new_member->member_id;
            entry_count_array[member_index]--;
            (*entry_count)++;
        } else {
            /* Either the member of the randomly chosen entry is
             * the same as the new member, or the member is an existing
             * member and its entry count has decreased to threshold.
             * In both cases, find the next entry that contains a
             * member that's not the new member and whose entry count
             * has not decreased to threshold.
             */
            next_entry_index = (entry_index + 1) % num_entries;
            while (next_entry_index != entry_index) {
                member_id = member_id_buf[next_entry_index];

                /* Determine if the entry contains the new member.
                 * If not, determine the existing member index.
                 */
                if (member_id == new_member->member_id) {
                    is_new_member = TRUE;
                } else {
                    is_new_member = FALSE;

                    /* In the member_array, it's assumed that each array element's
                     * member_id matches the array index of the element.
                     */
                    member_index = member_id;
                }

                if (!is_new_member &&
                        (entry_count_array[member_index] > threshold)) {
                    if (overlay) {
                        rh_intf_array[next_entry_index] = new_member->nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                    } else {
                        rh_intf_array[next_entry_index] = new_member->nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
                    }
                    member_id_buf[next_entry_index] = new_member->member_id;
                    entry_count_array[member_index]--;
                    (*entry_count)++;
                    break;
                } else {
                    next_entry_index = (next_entry_index + 1) % num_entries;
                }
            }
            if (next_entry_index == entry_index) {
                /* The entry count of all existing members has decreased
                 * to threshold. The entry count of the new member has
                 * not yet increased to lower_bound. Lower the threshold.
                 */
                threshold--;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_member_id_buf_assign
 * Purpose:
 *      For each entry in the rh set entry buffer, match its next hop
 *      index against the next hop index of a member, then assign the
 *      matching member's member ID to the entry.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_members - (IN) Number of members.
 *      member_array - (IN/OUT) Array of members.
 *      num_entries - (IN) Number of rh set entries in buffer.
 *      rh_intf_array - (IN) intf rh set array entry buffer.
 *      member_id_buf - (OUT) Member ID buffer.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_opt_ecmp_rh_member_id_buf_assign(int unit, int num_members,
                                     _ecmp_rh_member_t *member_array,
                                     int num_entries,
                                     bcm_if_t *rh_intf_array,
                                     int *member_id_buf)
{
    int i, k, m;
    int next_hop_index;
    int offset;

    for (i = 0; i < num_entries; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, rh_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                 BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                    rh_intf_array[i])) {
            offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif
        else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, rh_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            return(BCM_E_INTERNAL);
        }
        next_hop_index = rh_intf_array[i] - offset;
        /* Search the member_array for matching next hop index */
        for (k = 0; k < num_members; k++) {
            if (member_array[k].nh_index == next_hop_index) {
                if (member_array[k].num_replica == 1) {
                    /* The next hop index is not shared by other members. */
                    member_id_buf[i] = member_array[k].member_id;
                    break;
                }

                /* Else there are more than one members sharing the next
                 * hop index. Assign the member ID of the replica
                 * indicated by next_replica_id.
                 */
                for (m = k; m < num_members; m++) {
                    if ((member_array[m].nh_index == next_hop_index) &&
                        (member_array[m].replica_id == member_array[k].next_replica_id)) {
                        member_id_buf[i] = member_array[m].member_id;
                        break;
                    }
                }
                if (m == num_members) {
                    /* Cannot find a matching replica */
                    return BCM_E_INTERNAL;
                }

                /* Increment the next_replica_id field of the first replica */
                member_array[k].next_replica_id = (member_array[k].next_replica_id + 1) %
                    member_array[k].num_replica;

                break;
            }
        }
        if (k == num_members) {
            /* Cannot find a member with matching next hop index */
            return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_set
 * Purpose:
 *      Configure an ECMP resilient hashing group.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      new_intf_array - (OUT) intf rh set array entry buffer.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_opt_ecmp_rh_set(int unit,
                    bcm_l3_egress_ecmp_t *ecmp,
                    int intf_count,
                    bcm_if_t *intf_array,
                    bcm_if_t *new_intf_array)
{
    int rv = BCM_E_NONE;
    int max_entry_count;
    int chosen_index;
    int *entry_count_arr = NULL;
    int i;

    if (ecmp == NULL ||
            ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        return BCM_E_PARAM;
    }

    if (intf_count > 0 && intf_array == NULL) {
        return BCM_E_PARAM;
    }

    if (intf_count == 0) {
        return BCM_E_NONE;
    }

    entry_count_arr = sal_alloc(sizeof(int) * intf_count,
            "ECMP RH entry count array");
    if (NULL == entry_count_arr) {
        return BCM_E_MEMORY;
    }
    sal_memset(entry_count_arr, 0, sizeof(int) * intf_count);
    max_entry_count = ecmp->dynamic_size / intf_count;

    for (i = 0; i < ecmp->dynamic_size; i++) {
        /* Choose a member of the ECMP */
        rv = _bcm_opt_ecmp_rh_member_choose(unit, intf_count,
                entry_count_arr, &max_entry_count, &chosen_index);
        if (BCM_FAILURE(rv)) {
            sal_free(entry_count_arr);
            return rv;
        }
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit,
                    intf_array[chosen_index])) {
            new_intf_array[i] = intf_array[chosen_index];
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                 (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                    intf_array[chosen_index]))) {
            new_intf_array[i] = intf_array[chosen_index];
        }
#endif
         else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit,
                    intf_array[chosen_index])) {
            new_intf_array[i] = intf_array[chosen_index];
        } else {
            sal_free(entry_count_arr);
            return BCM_E_PARAM;
        }
    }
    sal_free(entry_count_arr);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_add
 * Purpose:
 *      Add a member to an ECMP resilient hashing group.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of resilient hashing eligible members,
 *                        including the member to be added.
 *      new_intf   - (IN) New member to be added.
 *      rh_intf_array - (IN/OUT) intf rh set array entry buffer.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_opt_ecmp_rh_add(int unit,
                    bcm_l3_egress_ecmp_t *ecmp,
                    int intf_count,
                    bcm_if_t *intf_array,
                    bcm_if_t new_intf,
                    bcm_if_t *rh_intf_array)
{
    int rv = BCM_E_NONE;
    int offset;
    int new_next_hop_index;
    int num_existing_members;
    int alloc_size;
    int num_entries;
    _ecmp_rh_member_t *existing_member_arr = NULL;
    int i;
    int *member_id_buf = NULL;
    int *entry_count_arr = NULL;
    int member_id;
    _ecmp_rh_member_t new_member;
    int new_member_entry_count;
    int overlay = 0;

    if (ecmp == NULL ||
            ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        return BCM_E_PARAM;
    }

    if (intf_count == 0 || intf_array == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, new_intf)) {
        offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
              BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                new_intf)) {
        offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    }
#endif
    else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, new_intf)) {
        offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }
    new_next_hop_index = new_intf - offset;

    /* Check that the new member is the last element of the array of
     * resilient hashing eligible members.
     */
    if (new_intf != intf_array[intf_count - 1]) {
        return BCM_E_PARAM;
    }
    num_existing_members = intf_count - 1;

    if (intf_count == 1) {
        /* Adding the first member is the same as setting one member */
        return _bcm_opt_ecmp_rh_set(unit, ecmp, intf_count, intf_array,
                                   rh_intf_array);
    }
    num_entries = ecmp->dynamic_size; 
    /* Construct an array of existing members */
    alloc_size = num_existing_members * sizeof(_ecmp_rh_member_t);
    existing_member_arr = sal_alloc(alloc_size, "ECMP RH member array");
    if (NULL == existing_member_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(existing_member_arr, 0, alloc_size);
    for (i = 0; i < num_existing_members; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                 BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                                        intf_array[i])) {
            offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif
        else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        existing_member_arr[i].nh_index = intf_array[i] - offset;
        existing_member_arr[i].member_id = i;
        existing_member_arr[i].num_replica = 1;
        existing_member_arr[i].replica_id = 0;
        existing_member_arr[i].next_replica_id = 0;
    }
    /* Find members that share the same next hop index, and
     * update the replica information in existing_member_arr.
     */
    rv = _bcm_opt_ecmp_rh_member_replica_find(unit, num_existing_members,
            existing_member_arr);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Derive a buffer of member IDs by assigning a member ID to each
     * entry of the rh set entry buffer.
     */
    alloc_size = num_entries * sizeof(int);
    member_id_buf = sal_alloc(alloc_size, "ECMP RH member ID buffer");
    if (NULL == member_id_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_id_buf, 0, alloc_size);
    rv = _bcm_opt_ecmp_rh_member_id_buf_assign(unit, num_existing_members,
            existing_member_arr, num_entries, rh_intf_array, member_id_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Compute the number of entries currently assigned to each
     * existing member.
     */
    alloc_size = intf_count * sizeof(int);
    entry_count_arr = sal_alloc(alloc_size, "ECMP RH entry count array");
    if (NULL == entry_count_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(entry_count_arr, 0, alloc_size);
    for (i = 0; i < num_entries; i++) {
        member_id = member_id_buf[i];

        /* In this procedure, the member_id happens to be the same as
         * the index into the existing_member_arr.
         */
        entry_count_arr[member_id]++;
    }

    /* Check that the distribution of rh set entries among existing members
     * is balanced. For instance, if the number of rh set entries is 64, and
     * the number of existing members is 6, then every member should have
     * between 10 and 11 entries.
     */

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) {
        overlay = (ecmp->ecmp_group_flags & BCM_L3_ECMP_OVERLAY);
    }
#endif

    /* Re-balance rh set entries from existing members to the new member */
    sal_memset(&new_member, 0, sizeof(_ecmp_rh_member_t));
    new_member.nh_index = new_next_hop_index;
    new_member.member_id = num_existing_members;
    rv = _bcm_opt_ecmp_rh_add_rebalance(unit,
            num_entries, overlay, rh_intf_array,
            member_id_buf, num_existing_members, existing_member_arr,
            entry_count_arr, &new_member, &new_member_entry_count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

cleanup:
    if (member_id_buf) {
        sal_free(member_id_buf);
    }
    if (existing_member_arr) {
        sal_free(existing_member_arr);
    }
    if (entry_count_arr) {
        sal_free(entry_count_arr);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_delete
 * Purpose:
 *      Delete a member from an ECMP resilient hashing group.
 * Parameters:
 *      unit         - (IN) SOC unit number.
 *      ecmp         - (IN) ECMP group info.
 *      intf_count   - (IN) Number of elements in intf_array.
 *      intf_array   - (IN) Array of resilient hashing eligible members,
 *                        except the member to be deleted.
 *      leaving_intf - (IN) Member to delete.
 *      rh_intf_array - (IN/OUT) intf rh set array entry buffer.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_opt_ecmp_rh_delete(int unit,
                       bcm_l3_egress_ecmp_t *ecmp,
                       int intf_count,
                       bcm_if_t *intf_array,
                       bcm_if_t leaving_intf,
                       bcm_if_t *rh_intf_array)
{
    int rv = BCM_E_NONE;
    int ecmp_group;
    int offset;
    int leaving_next_hop_index;
    int num_entries;
    int alloc_size;
    int num_existing_members;
    _ecmp_rh_member_t *existing_member_arr = NULL;
    int i;
    int *member_id_buf = NULL;
    int *entry_count_arr = NULL;
    int lower_bound, upper_bound;
    int num_remaining_members;
    int threshold;
    int leaving_member_id;
    int member_id;
    int chosen_index;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int overlay_group = 0;
#endif

    if (ecmp == NULL ||
            ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        return BCM_E_PARAM;
    }
    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    if (intf_count > 0 && intf_array == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, leaving_intf)) {
        offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
    }
#ifdef BCM_TOMAHAWK3_SUPPORT
    else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
              BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                leaving_intf)) {
        offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        overlay_group = 1;
    }
#endif
    else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, leaving_intf)) {
        offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }
    leaving_next_hop_index = leaving_intf - offset;
    num_entries = ecmp->dynamic_size;

    if (intf_count == 0) {
        /* Deleting the last member is the same as freeing all resources */
        BCM_IF_ERROR_RETURN(_bcm_opt_ecmp_rh_free_resource(unit, ecmp_group));

        return BCM_E_NONE;
    }

    /* Construct an array of existing members */
    num_existing_members = intf_count + 1;
    alloc_size = num_existing_members * sizeof(_ecmp_rh_member_t);
    existing_member_arr = sal_alloc(alloc_size, "ECMP RH member array");
    if (NULL == existing_member_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(existing_member_arr, 0, alloc_size);
    for (i = 0; i < intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                  BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                    intf_array[i])) {
            offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif
        else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        existing_member_arr[i].nh_index = intf_array[i] - offset;
        existing_member_arr[i].member_id = i;
        existing_member_arr[i].num_replica = 1;
        existing_member_arr[i].replica_id = 0;
        existing_member_arr[i].next_replica_id = 0;
    }
    existing_member_arr[intf_count].nh_index = leaving_next_hop_index;
    existing_member_arr[intf_count].member_id = intf_count;
    existing_member_arr[intf_count].num_replica = 1;
    existing_member_arr[intf_count].replica_id = 0;
    existing_member_arr[intf_count].next_replica_id = 0;

    /* Find members that share the same next hop index, and
     * update the replica information in existing_member_arr.
     */
    rv = _bcm_opt_ecmp_rh_member_replica_find(unit, num_existing_members,
            existing_member_arr);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Derive a buffer of member IDs by assigning a member ID to each
     * entry of the rh set entry buffer.
     */
    alloc_size = num_entries * sizeof(int);
    member_id_buf = sal_alloc(alloc_size, "ECMP RH member ID buffer");
    if (NULL == member_id_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_id_buf, 0, alloc_size);
    rv = _bcm_opt_ecmp_rh_member_id_buf_assign(unit, num_existing_members,
            existing_member_arr, num_entries, rh_intf_array, member_id_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Compute the number of entries currently assigned to each
     * existing member.
     */
    alloc_size = num_existing_members * sizeof(int);
    entry_count_arr = sal_alloc(alloc_size, "ECMP RH entry count array");
    if (NULL == entry_count_arr) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(entry_count_arr, 0, alloc_size);
    for (i = 0; i < num_entries; i++) {
        member_id = member_id_buf[i];

        /* In the existing_member_arr, each element's member_id is the
         * same as its array index.
         */
        entry_count_arr[member_id]++;
    }

    /* Check that the distribution of rh set entries among all members
     * is balanced. For instance, if the number of rh set entries is 64, and
     * the number of members is 6, then every member should have
     * between 10 and 11 entries.
     */
    lower_bound = num_entries / num_existing_members;
    upper_bound = (num_entries % num_existing_members) ?
                  (lower_bound + 1) : lower_bound;
    for (i = 0; i < num_existing_members; i++) {
        if (entry_count_arr[i] < lower_bound ||
                entry_count_arr[i] > upper_bound) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }
    }

    /* Re-balance rh set entries from the leaving member to the remaining
     * members. For example, if the number of rh set entries is 64, and
     * the number of members is 6, then each member has between 10 and 11
     * entries. The entries should be re-assigned from the leaving member to
     * the remaining 5 members such that each remaining member will end up
     * with between 12 and 13 entries.
     */
    num_remaining_members = num_existing_members - 1;
    lower_bound = num_entries / num_remaining_members;
    threshold = lower_bound;
    leaving_member_id = existing_member_arr[intf_count].member_id;
    for (i = 0; i < num_entries; i++) {
        member_id = member_id_buf[i];
        if (member_id != leaving_member_id) {
            continue;
        }

        /* Randomly choose a member among the remaining members */
        rv = _bcm_opt_ecmp_rh_member_choose(unit, num_remaining_members,
                entry_count_arr, &threshold, &chosen_index);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (overlay_group) {
            rh_intf_array[i] = existing_member_arr[chosen_index].nh_index +
                                       BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        } else
#endif
        {
            rh_intf_array[i] = existing_member_arr[chosen_index].nh_index +
                                       BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
        member_id_buf[i] = existing_member_arr[chosen_index].member_id;
    }

cleanup:
    if (member_id_buf) {
        sal_free(member_id_buf);
    }
    if (existing_member_arr) {
        sal_free(existing_member_arr);
    }
    if (entry_count_arr) {
        sal_free(entry_count_arr);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_clear_by_member_id_th
 * Purpose:
 *      In the given rh set entry buffer, clear entries whose
 *      member ID is greater than or equal to the given member ID threshold.
 *      Also, keep track of entry count for each member ID smaller
 *      than the given member ID threshold.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of rh set entries in buffer.
 *      rh_intf_array - (IN/OUT) intf array entry buffer.
 *      member_id_buf - (IN/OUT) Member ID buffer.
 *      member_id_th - (IN) Member ID threshold.
 *      array_size - (IN) Number of elements in entry_count_array.
 *      entry_count_array - (OUT) Array of entry counts for member IDs not cleared.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_opt_ecmp_rh_clear_by_member_id_th(int unit,
                                      int num_entries, bcm_if_t *rh_intf_aray,
                                      int *member_id_buf,
                                      int member_id_th, int array_size,
                                      int *entry_count_array)
{
    int i;
    int member_id;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == member_id_buf) {
        return BCM_E_PARAM;
    }
    if (member_id_th != array_size) {
        return BCM_E_PARAM;
    }
    if (NULL == entry_count_array) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < num_entries; i++) {
        member_id = member_id_buf[i];

        if (member_id >= member_id_th) {

            /* Do not clear the next hop index, which is needed later by
             * _bcm_opt_ecmp_rh_populate_empty_entries.
             */

            /* Clear member ID */
            member_id_buf[i] = -1;
        } else {
            entry_count_array[member_id]++;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_populate_empty_entries
 * Purpose:
 *      In the given rh set entry buffer, populate entries not
 *      containing a member, such that maximal balance is achieve
 *      among members in member_array.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of rh set entries in buffer.
 *      rh_intf_array - (IN/OUT) intf array entry buffer.
 *      member_id_buf - (IN/OUT) Member ID buffer.
 *      num_members - (IN) Number of elements in member_array and
 *                         entry_count_array.
 *      member_array - (IN) Array of members.
 *      entry_count_array - (IN/OUT) Array of entry counts.
 *      num_shared_members - (IN) Number of members in member_array that
 *                                are shared by the old and the new ECMP
 *                                groups.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_opt_ecmp_rh_populate_empty_entries(int unit,
                                       int num_entries,
                                       int overlay,
                                       bcm_if_t *rh_intf_array,
                                       int *member_id_buf,
                                       int num_members,
                                       _ecmp_rh_member_t *member_array,
                                       int *entry_count_array,
                                       int num_shared_members)
{
    int max_entry_count;
    int i, k;
    int next_hop_index;
    int chosen_index;
    int offset;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (NULL == member_id_buf) {
        return BCM_E_PARAM;
    }
    if (0 == num_members) {
        return BCM_E_PARAM;
    }
    if (NULL == member_array) {
        return BCM_E_PARAM;
    }
    if (NULL == entry_count_array) {
        return BCM_E_PARAM;
    }
    if (num_shared_members > num_members) {
        return BCM_E_PARAM;
    }

    max_entry_count = num_entries / num_members;
    for (i = 0; i < num_entries; i++) {
       if (member_id_buf[i] != -1) {
            /* Skip valid entries */
            continue;
        }

        /* First, try to find a shared member whose next hop index
         * matches the current entry and whose entry count has not reached
         * max_entry_count. Doing so minimizes flow-to-member reassignments.
         */
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, rh_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                  BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                    rh_intf_array[i])) {
            offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif
        else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, rh_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            return BCM_E_PARAM;
        }
        next_hop_index = rh_intf_array[i] - offset;
        for (k = 0; k < num_shared_members; k++) {
            if (member_array[k].nh_index == next_hop_index) {
                if (entry_count_array[k] < max_entry_count) {
                    entry_count_array[k]++;
                    chosen_index = k;
                    break;
                }
            }
        }
        if (k == num_shared_members) {
            /* Cannot find a shared member with matching next hop index and
             * with entry count < max_entry_count. Randomly choose a member.
             */
            BCM_IF_ERROR_RETURN(_bcm_opt_ecmp_rh_member_choose(unit, num_members,
                        entry_count_array, &max_entry_count, &chosen_index));
        }
        /* Set rh set entry */
        member_id_buf[i] = member_array[chosen_index].member_id;
        rh_intf_array[i] = member_array[chosen_index].nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (overlay) {
            rh_intf_array[i] = member_array[chosen_index].nh_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif

    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_opt_ecmp_rh_balance_with_min_shared_mod
 * Purpose:
 *      Modify the given rh set entry buffer to achieve balance among
 *      the shared members and members exclusive to the new ECMP group,
 *      while keeping modification of entries containing the
 *      shared members to a minimum.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of rh set entries in buffer.
 *      rh_intf_array - (IN/OUT) intf array entry buffer. Initially, this buffer
 *                             contains the old ECMP group's rh set entries.
 *      shared_intf_count - (IN) Number of members shared by old and new ECMP
 *                               groups.
 *      shared_intf_array - (IN) Array of members shared by old and new ECMP
 *                               groups.
 *      ex_old_intf_count - (IN) Number of members exclusive to old ECMP group.
 *      ex_old_intf_array - (IN) Array of members exclusive to old ECMP group.
 *      ex_new_intf_count - (IN) Number of members exclusive to new ECMP group.
 *      ex_new_intf_array - (IN) Array of members exclusive to new ECMP group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_opt_ecmp_rh_balance_with_min_shared_mod(int unit,
                                            int num_entries,
                                            int overlay,
                                            bcm_if_t *rh_intf_array,
                                            int shared_intf_count,
                                            bcm_if_t *shared_intf_array,
                                            int ex_old_intf_count,
                                            bcm_if_t *ex_old_intf_array,
                                            int ex_new_intf_count,
                                            bcm_if_t *ex_new_intf_array)
{
    int rv = BCM_E_NONE;
    int old_intf_count;
    int new_intf_count;
    int num_members;
    int alloc_size;
    _ecmp_rh_member_t *member_array = NULL;
    int *entry_count_array = NULL;
    int i;
    int ex_old_index, ex_new_index;
    int *member_id_buf = NULL;
    int intf_count;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (0 == shared_intf_count) {
        return BCM_E_PARAM;
    }
    if (NULL == shared_intf_array) {
        return BCM_E_PARAM;
    }
    if ((ex_old_intf_count > 0) && (NULL == ex_old_intf_array)) {
        return BCM_E_PARAM;
    }
    if ((ex_new_intf_count > 0) && (NULL == ex_new_intf_array)) {
        return BCM_E_PARAM;
    }

    /* Allocate an array of members and an array of entry counts */
    old_intf_count = shared_intf_count + ex_old_intf_count;
    new_intf_count = shared_intf_count + ex_new_intf_count;
    num_members = (old_intf_count > new_intf_count) ? old_intf_count : new_intf_count;
    alloc_size = num_members * sizeof(_ecmp_rh_member_t);
    member_array = sal_alloc(alloc_size, "ECMP RH member array");
    if (NULL == member_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_array, 0, alloc_size);

    alloc_size = num_members * sizeof(int);
    entry_count_array = sal_alloc(alloc_size, "ECMP RH entry count array");
    if (NULL == entry_count_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(entry_count_array, 0, alloc_size);

    /* Initialize the member_array with shared members, followed by members
     * exclusive to the old ECMP group.
     */
    for (i = 0; i < shared_intf_count; i++) {
        member_array[i].nh_index = shared_intf_array[i];
        member_array[i].member_id = i;
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }
    for (i = shared_intf_count; i < old_intf_count; i++) {
        ex_old_index = i - shared_intf_count;
        member_array[i].nh_index = ex_old_intf_array[ex_old_index];
        member_array[i].member_id = i;
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }
    /* Find members that share the same next hop index, and
     * update the replica information in member_array.
     */
    rv = _bcm_opt_ecmp_rh_member_replica_find(unit, old_intf_count,
            member_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Derive a buffer of member IDs by assigning a member ID to each
     * entry of the rh set entry buffer.
     */
    alloc_size = num_entries * sizeof(int);
    member_id_buf = sal_alloc(alloc_size, "ECMP RH member ID buffer");
    if (NULL == member_id_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_id_buf, 0, alloc_size);
    rv = _bcm_opt_ecmp_rh_member_id_buf_assign(unit, old_intf_count,
            member_array, num_entries, rh_intf_array, member_id_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Traverse the rh set entry buffer to clear entries with member
     * IDs greater than the member IDs of the shared members. Also keep
     * track of each shared member's entry count.
     */
    rv = _bcm_opt_ecmp_rh_clear_by_member_id_th(unit, num_entries, rh_intf_array,
            member_id_buf, shared_intf_count, shared_intf_count,
            entry_count_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Clear from the member_array members exclusive to the old ECMP group.
     * Also need to reset share members' replica info.
     */
    for (i = shared_intf_count; i < old_intf_count; i++) {
        sal_memset(&member_array[i], 0, sizeof(_ecmp_rh_member_t));
    }
    for (i = 0; i < shared_intf_count; i++) {
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }

    /* Add to member_array members exclusive to the new ECMP group */
    for (i = shared_intf_count; i < new_intf_count; i++) {
        ex_new_index = i - shared_intf_count;
        member_array[i].nh_index = ex_new_intf_array[ex_new_index];
        member_array[i].member_id = i;
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }

    /* Find members that share the same next hop index, and
     * update the replica information in member_array.
     */
    rv = _bcm_opt_ecmp_rh_member_replica_find(unit, new_intf_count,
            member_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    /* Populate the cleared entries in the rh set entry buffer.
     * There're 2 cases:
     *
     * Case 1:
     * If the number of members in the old ECMP group is greater than or
     * equal to the number in the new ECMP group, populate the cleared
     * entries with shared members and members exclusive to the new ECMP
     * group, such that maximal balance is achieved among them.
     *
     * Case 2:
     * If the old ECMP group has fewer members than the new ECMP group,
     * populate the cleared entries with N members that are exclusive
     * to the new ECMP group, such that N + number of shared members equals
     * to the number of members in old ECMP group.
     *
     * In both cases, the rh set entries containing the shared members
     * are unmodified.
     */
    if (old_intf_count >= new_intf_count) {
        intf_count = new_intf_count;
    } else {
        intf_count = old_intf_count;
    }
    rv = _bcm_opt_ecmp_rh_populate_empty_entries(unit, num_entries, overlay, rh_intf_array,
            member_id_buf, intf_count, member_array, entry_count_array,
            shared_intf_count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* For any remaining unused members in member_array, move some
     * rh set entries from existing members to these remaining
     * members, such that maximal balance is achieved.
     */
    if (new_intf_count > intf_count) {
        for (i = intf_count; i < new_intf_count; i++) {
            rv = _bcm_opt_ecmp_rh_add_rebalance(unit, num_entries, overlay, rh_intf_array,
                    member_id_buf, i, member_array, entry_count_array,
                    &member_array[i], &entry_count_array[i]);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    }

cleanup:
    if (member_array) {
        sal_free(member_array);
    }
    if (entry_count_array) {
        sal_free(entry_count_array);
    }
    if (member_id_buf) {
        sal_free(member_id_buf);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_arrange_with_no_shared_entries
 * Purpose:
 *      Arrange the given rh set entry buffer to achieve Maximization of the shared members,
 *      while entries containing none of shared members.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      num_entries - (IN) Number of rh set entries in buffer.
 *      rh_intf_array - (IN/OUT) intf array entry buffer. Initially, this buffer
 *                             contains the old ECMP group's rh set entries.
 *      old_intf_count - (IN) Number of members exclusive to old ECMP group.
 *      old_intf_array - (IN) Array of members exclusive to old ECMP group.
 *      new_intf_count - (IN) Number of members exclusive to new ECMP group.
 *      new_intf_array - (IN) Array of members exclusive to new ECMP group.
 *      shared_intf_count - (OUT) Number of members shared by old and new ECMP
 *                               groups.
 *      shared_intf_array - (OUT) Array of members shared by old and new ECMP
 *                               groups.
 *      ex_old_intf_count - (OUT) Number of members exclusive to old ECMP group.
 *      ex_old_intf_array - (OUT) Array of members exclusive to old ECMP group.
 *      ex_new_intf_count - (OUT) Number of members exclusive to new ECMP group.
 *      ex_new_intf_array - (OUT) Array of members exclusive to new ECMP group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_opt_ecmp_rh_arrange_with_no_shared_entries(int unit,
                                               int num_entries,
                                               bcm_if_t *rh_intf_array,
                                               int old_intf_count,
                                               bcm_if_t *old_intf_array,
                                               int new_intf_count,
                                               bcm_if_t *new_intf_array,
                                               int *shared_intf_count,
                                               bcm_if_t *shared_intf_array,
                                               int *ex_old_intf_count,
                                               bcm_if_t *ex_old_intf_array,
                                               int *ex_new_intf_count,
                                               bcm_if_t *ex_new_intf_array)
{
    int rv = BCM_E_NONE;
    int num_members;
    int alloc_size;
    _ecmp_rh_member_t *member_array = NULL;
    int i;
    int *member_id_buf = NULL;
    int member_id;
    int max_shared;

    if (num_entries < 1) {
        return BCM_E_PARAM;
    }
    if (old_intf_count > 0 && old_intf_array == NULL) {
        return BCM_E_PARAM;
    }
    if (new_intf_count > 0 && new_intf_array == NULL) {
        return BCM_E_PARAM;
    }
    if (NULL == shared_intf_array) {
        return BCM_E_PARAM;
    }
    if (NULL == ex_old_intf_array) {
        return BCM_E_PARAM;
    }
    if (NULL == ex_new_intf_array) {
        return BCM_E_PARAM;
    }

    /* Allocate an array of members and an array of entry counts */
    num_members = old_intf_count;
    alloc_size = num_members * sizeof(_ecmp_rh_member_t);
    member_array = sal_alloc(alloc_size, "ECMP RH member array");
    if (NULL == member_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_array, 0, alloc_size);

    /* Initialize the member_array with shared members, followed by members
     * exclusive to the old ECMP group.
     */
    for (i = 0; i < num_members; i++) {
        member_array[i].nh_index = old_intf_array[i];
        member_array[i].member_id = i;
        member_array[i].num_replica = 1;
        member_array[i].replica_id = 0;
        member_array[i].next_replica_id = 0;
    }

    /* Find members that share the same next hop index, and
     * update the replica information in member_array.
     */
    rv = _bcm_opt_ecmp_rh_member_replica_find(unit, old_intf_count,
            member_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Derive a buffer of member IDs by assigning a member ID to each
     * entry of the rh set entry buffer.
     */
    alloc_size = num_entries * sizeof(int);
    member_id_buf = sal_alloc(alloc_size, "ECMP RH member ID buffer");
    if (NULL == member_id_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(member_id_buf, 0, alloc_size);
    rv = _bcm_opt_ecmp_rh_member_id_buf_assign(unit, num_members,
            member_array, num_entries, rh_intf_array, member_id_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Determine members shared by old and new ECMP groups, and
     * members exclusive to the old and the new ECMP groups.
     */
    max_shared = (old_intf_count > new_intf_count) ? new_intf_count :
        old_intf_count;

    for (i = 0; i < num_entries; i++) {
        member_id = member_id_buf[i];
        if (member_id >= max_shared) {
            continue;
        }
        rh_intf_array[i] = new_intf_array[member_id] + BCM_XGS3_EGRESS_IDX_MIN(unit);
    }

    *shared_intf_count = max_shared;
    sal_memcpy(shared_intf_array, new_intf_array,
                                *shared_intf_count * sizeof(bcm_if_t));

    if (old_intf_count > new_intf_count) {
        *ex_new_intf_count = 0;
        *ex_old_intf_count = old_intf_count - *shared_intf_count;
        sal_memcpy(ex_old_intf_array, &old_intf_array[*shared_intf_count],
                                    *ex_old_intf_count * sizeof(bcm_if_t));
    } else {
        *ex_old_intf_count = 0;
        *ex_new_intf_count = new_intf_count - *shared_intf_count;
        sal_memcpy(ex_new_intf_array, &new_intf_array[*shared_intf_count],
                                    *ex_new_intf_count * sizeof(bcm_if_t));
    }

cleanup:
    if (member_array) {
        sal_free(member_array);
    }
    if (member_id_buf) {
        sal_free(member_id_buf);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_opt_ecmp_rh_replace
 * Purpose:
 *      Replace ECMP resilient hashing group members without rh set shuffle.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      old_intf_count - (IN) Number of elements in old_intf_array.
 *      old_intf_array - (IN) Array of Egress forwarding objects before replacing.
 *      rh_intf_array - (IN/OUT) intf array entry buffer. Initially, this buffer
 *                             contains the old ECMP group's rh set entries.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_opt_ecmp_rh_replace(int unit,
                        bcm_l3_egress_ecmp_t *ecmp,
                        int intf_count,
                        bcm_if_t *intf_array,
                        int old_intf_count,
                        bcm_if_t *old_intf_array,
                        bcm_if_t *rh_intf_array)
{
    int rv = BCM_E_NONE;
    int ecmp_group;
    int offset;
    int num_entries;
    int alloc_size;
    int max_shared;
    int i, j;
    int shared_intf_count;
    int ex_old_intf_count;
    int ex_new_intf_count;
    bcm_if_t *shared_intf_array = NULL;
    bcm_if_t *ex_old_intf_array = NULL;
    bcm_if_t *ex_new_intf_array = NULL;
    bcm_if_t *temp_old_intf_array = NULL;
    bcm_if_t *temp_new_intf_array = NULL;
    int overlay = 0;

    if (ecmp == NULL ||
            ecmp->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        return BCM_E_PARAM;
    }

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    num_entries = ecmp->dynamic_size;

    if (intf_count > 0 && intf_array == NULL) {
        return BCM_E_PARAM;
    }

    if (intf_count == 0) {
        /* Replacing the last member is the same as freeing all resources */
        BCM_IF_ERROR_RETURN(_bcm_opt_ecmp_rh_free_resource(unit, ecmp_group));

        return BCM_E_NONE;
    }

    if (old_intf_count == 0) {
        /* Replacing the first member is the same as setting one member */
        return _bcm_opt_ecmp_rh_set(unit, ecmp, intf_count, intf_array, rh_intf_array);
    }

    /* Determine members shared by old and new ECMP groups, and
     * members exclusive to the old and the new ECMP groups.
     */
    max_shared = (old_intf_count > intf_count) ? intf_count :
        old_intf_count;

    alloc_size = max_shared * sizeof(bcm_if_t);
    shared_intf_array = sal_alloc(alloc_size, "shared ecmp member array");
    if (NULL == shared_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(shared_intf_array, 0, alloc_size);

    alloc_size = old_intf_count * sizeof(bcm_if_t);
    ex_old_intf_array = sal_alloc(alloc_size,
                                "array of members exclusive to old ecmp group");
    if (NULL == ex_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ex_old_intf_array, 0, alloc_size);

    alloc_size = intf_count * sizeof(bcm_if_t);
    ex_new_intf_array = sal_alloc(alloc_size,
                                "array of members exclusive to new ecmp group");
    if (NULL == ex_new_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ex_new_intf_array, 0, alloc_size);

    alloc_size = old_intf_count * sizeof(bcm_if_t);
    temp_old_intf_array = sal_alloc(alloc_size, "copy of old_intf_array");
    if (NULL == temp_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memcpy(temp_old_intf_array, old_intf_array, alloc_size);
    for (i = 0; i < old_intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, temp_old_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                  BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                    temp_old_intf_array[i])) {
            offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif
        else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, temp_old_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        temp_old_intf_array[i] -= offset;
    }
    alloc_size = intf_count * sizeof(bcm_if_t);
    temp_new_intf_array = sal_alloc(alloc_size, "copy of intf_array");
    if (NULL == temp_new_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memcpy(temp_new_intf_array, intf_array, alloc_size);
    for (i = 0; i < intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, temp_new_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                  BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                    temp_new_intf_array[i])) {
            offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif
        else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, temp_new_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        temp_new_intf_array[i] -= offset;
    }

    /* Replace arrangement */
    shared_intf_count = 0;
    ex_new_intf_count = 0;
    for (i = 0; i < intf_count; i++) {
        for (j = 0; j < old_intf_count; j++) {
            if (temp_new_intf_array[i] == temp_old_intf_array[j]) {
                shared_intf_array[shared_intf_count++] = temp_new_intf_array[i];
                /* Mark the matched element invalid */
                temp_old_intf_array[j] = BCM_XGS3_L3_INVALID_INDEX;
                break;
            }
        }
        if (j == old_intf_count) {
            ex_new_intf_array[ex_new_intf_count++] = temp_new_intf_array[i];
        }
    }
    ex_old_intf_count = 0;
    for (i = 0; i < old_intf_count; i++) {
        if (temp_old_intf_array[i] != BCM_XGS3_L3_INVALID_INDEX) {
            ex_old_intf_array[ex_old_intf_count++] = temp_old_intf_array[i];
        }
    }

    if (shared_intf_count == 0) {
        /* The old and the new ECMP groups don't share any members.
         * Replacing rh set entries with max_shared members so as to
         * set shared_intf_count = max_shared.
         */
        rv = _bcm_opt_ecmp_rh_arrange_with_no_shared_entries(unit,
                num_entries, rh_intf_array,
                old_intf_count, temp_old_intf_array,
                intf_count, temp_new_intf_array,
                &shared_intf_count, shared_intf_array,
                &ex_old_intf_count, ex_old_intf_array,
                &ex_new_intf_count, ex_new_intf_array);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    /* Modify the rh set entry buffer to achieve balance among
     * shared members and members exclusive to the new ECMP group,
     * while keeping modifications of entries containing the
     * shared members to a minimum.
     */

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_l3_ecmp_hier_tbl) &&
       (ecmp->ecmp_group_flags & BCM_L3_ECMP_OVERLAY)) {
        overlay = 1;
    }
#endif

    rv = _bcm_opt_ecmp_rh_balance_with_min_shared_mod(unit,
            num_entries, overlay, rh_intf_array,
            shared_intf_count, shared_intf_array,
            ex_old_intf_count, ex_old_intf_array,
            ex_new_intf_count, ex_new_intf_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

cleanup:
    if (shared_intf_array) {
        sal_free(shared_intf_array);
    }
    if (ex_old_intf_array) {
        sal_free(ex_old_intf_array);
    }
    if (ex_new_intf_array) {
        sal_free(ex_new_intf_array);
    }
    if (temp_old_intf_array) {
        sal_free(temp_old_intf_array);
    }
    if (temp_new_intf_array) {
        sal_free(temp_new_intf_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_opt_l3_egress_ecmp_rh_create 
 * Purpose:
 *      Create or modify an ECMP resilient hashing group.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      op         - (IN) Member operation: SET, ADD, DELETE, or REPLACE.
 *      count      - (IN) Number of elements in intf.
 *      intf       - (IN) Egress forwarding objects to add, delete, or replace.
 *      rh_intf_array - (IN/OUT) intf array entry buffer. For new group, the rh set
                         is output in this array. For modifications, this buffer
 *                       contains the old ECMP group's rh set entries.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_opt_l3_egress_ecmp_rh_create(int unit, bcm_l3_egress_ecmp_t *ecmp,
                                int intf_count,
                                bcm_if_t *intf_array, int op, int count,
                                bcm_if_t *intf, 
                                bcm_if_t *rh_intf_array)
{
    int rh_enable;
    int ecmp_group;

    if (ecmp->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        /* Verify ecmp->dynamic_size */
        rh_enable = 1;
    } else {
        rh_enable = 0;
    }

    if (op == BCM_L3_ECMP_MEMBER_OP_SET) {
        /* Free resilient hashing resources associated with this ecmp group */
        if (ecmp->flags & BCM_L3_WITH_ID) {
            ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
            BCM_IF_ERROR_RETURN(_bcm_opt_ecmp_rh_free_resource(unit, ecmp_group));
        }

        if (rh_enable) {
            /* Set resilient hashing members for this ecmp group */
            BCM_IF_ERROR_RETURN
                (_bcm_opt_ecmp_rh_set(unit, ecmp, intf_count, intf_array, rh_intf_array));
        }
    } else if (op == BCM_L3_ECMP_MEMBER_OP_ADD) {
        if (rh_enable) {
            /* Add new resilient hashing member to ecmp group */
            BCM_IF_ERROR_RETURN
                (_bcm_opt_ecmp_rh_add(unit, ecmp, intf_count, intf_array, *intf,
                         rh_intf_array));
        }
    } else if (op == BCM_L3_ECMP_MEMBER_OP_DELETE) {
        if (rh_enable) {
            /* Delete resilient hashing member from ecmp group */
            BCM_IF_ERROR_RETURN
                (_bcm_opt_ecmp_rh_delete(unit, ecmp, intf_count, intf_array, 
                             *intf, rh_intf_array));
        }
    } else if (op == BCM_L3_ECMP_MEMBER_OP_REPLACE) {
        if (rh_enable) {
            /* Replace resilient hashing member for ecmp group */
            BCM_IF_ERROR_RETURN
                (_bcm_opt_ecmp_rh_replace(unit, ecmp, intf_count, intf_array, 
                                count, intf, rh_intf_array));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_opt_l3_egress_ecmp_rh_destroy
 * Purpose:
 *      Destroy an ECMP resilient hashing group.
 * Parameters:
 *      unit    - (IN) bcm device.
 *      mpintf  - (IN) L3 interface id pointing to Egress multipath object.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_opt_l3_egress_ecmp_rh_destroy(int unit, bcm_if_t mpintf) 
{
    int ecmp_group;

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, mpintf)) {
        ecmp_group = mpintf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_opt_ecmp_rh_free_resource(unit, ecmp_group));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_opt_l3_egress_ecmp_lb_get
 * Purpose:
 *      Get load balancing info for the ECMP group.
 * Parameters:
 *      unit - (IN) bcm device.
 *      ecmp - (INOUT) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_opt_l3_egress_ecmp_lb_get(int unit, bcm_l3_egress_ecmp_t *ecmp)
{
    int ecmp_group;
    int lb_mode;
    ecmp_count_entry_t ecmp_count_entry;
    uint32 count;
    int min_count = 6;
    int max_count = 14;
    soc_mem_t mem = L3_ECMP_COUNTm;

    if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, ecmp->ecmp_intf)) {
        ecmp_group = ecmp->ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
        return BCM_E_PARAM;
    }

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_l3_ecmp_hier_tbl) &&
        (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
             ecmp_mode_hierarchical) &&
        (ecmp_group < (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) / 2))) {
        ecmp->ecmp_group_flags |= BCM_L3_ECMP_OVERLAY;
        mem = ECMP_GROUP_HIERARCHICALm;
    }
#endif

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                ecmp_group, &ecmp_count_entry));
    lb_mode = soc_mem_field32_get(unit, mem,
            &ecmp_count_entry, LB_MODEf);

    if (lb_mode == BCM_TH_L3_ECMP_LB_MODE_RH) {
        ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        count = soc_mem_field32_get(unit, mem,
                   &ecmp_count_entry, COUNTf);
        if ((SOC_IS_TOMAHAWK2(unit)) ||
            (SOC_IS_TOMAHAWK3(unit))) {
            max_count = 15;
        }
        if ((count < min_count) || (count > max_count)) {
            return BCM_E_INTERNAL;
        } else {
            ecmp->dynamic_size = (1 << count);
        }
    } else if (lb_mode == BCM_TH_L3_ECMP_LB_MODE_RANDOM) {
        ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RANDOM;
    } else if (!(soc_feature(unit, soc_feature_l3_ecmp_weighted)) &&
               (lb_mode == BCM_TH_L3_ECMP_LB_MODE_RR)) {
        ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_ROUND_ROBIN;
    } else {
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (soc_feature(unit, soc_feature_l3_ecmp_weighted)) {
            switch (lb_mode) {
                case BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_256:
                case BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_512:
                case BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_1024:
                case BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_2048:
                case BCM_TH3_L3_ECMP_LB_MODE_WEIGHTED_4096:
                    ecmp->dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_DISABLED;
                    ecmp->ecmp_group_flags |= BCM_L3_ECMP_WEIGHTED;
                        break;
                default:
                    break;
            }
        }
#endif
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_opt_ecmp_lb_mode_set
 * Purpose:
 *     Set load balancing mode for the given ECMP group
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      ecmp_group_idx - (IN) ECMP group ID.
 *      lb_mode - (IN) ECMP group loadbalancing mode
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_opt_ecmp_lb_mode_set(int unit,
                        int ecmp_group_idx,
                        uint8 lb_mode)
{
    ecmp_count_entry_t ecmp_count_entry;
    ecmp_rrlb_cnt_entry_t rrlb_cnt_entry;
    initial_l3_ecmp_group_entry_t initial_l3_ecmp_group_entry;
    soc_mem_t mem = L3_ECMP_COUNTm;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_l3_ecmp_hier_tbl) &&
        (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
             ecmp_mode_hierarchical) &&
        (ecmp_group_idx < (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) / 2))) {
        mem = ECMP_GROUP_HIERARCHICALm;
    }
#endif
    /* Update ECMP group table LB field*/
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, ecmp_group_idx,
                &ecmp_count_entry));
    soc_mem_field32_set(unit, mem, &ecmp_count_entry,
            LB_MODEf, lb_mode);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, ecmp_group_idx,
                &ecmp_count_entry));

    if (SOC_MEM_IS_VALID(unit, INITIAL_L3_ECMP_GROUPm)) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, INITIAL_L3_ECMP_GROUPm,
            MEM_BLOCK_ANY, ecmp_group_idx, &initial_l3_ecmp_group_entry));
        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
            &initial_l3_ecmp_group_entry, LB_MODEf, lb_mode);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_L3_ECMP_GROUPm,
            MEM_BLOCK_ALL, ecmp_group_idx, &initial_l3_ecmp_group_entry));
    }

    /* Reset round robin counter for the group */
    if ((!(SOC_IS_TOMAHAWK3(unit))) && (lb_mode == BCM_TH_L3_ECMP_LB_MODE_RR)) {
        BCM_IF_ERROR_RETURN(READ_L3_ECMP_RRLB_CNTm(unit, MEM_BLOCK_ANY,
                ecmp_group_idx, &rrlb_cnt_entry));
        soc_L3_ECMP_RRLB_CNTm_field32_set(unit, &rrlb_cnt_entry,
                        RRLB_CNTf, 0);
        BCM_IF_ERROR_RETURN(WRITE_L3_ECMP_RRLB_CNTm(unit, MEM_BLOCK_ALL,
                        ecmp_group_idx, &rrlb_cnt_entry));
    }
    if (!SOC_IS_TRIDENT3X(unit)) {
        if (lb_mode == BCM_TH_L3_ECMP_LB_MODE_RH) {
            bcm_th_ecmp_group_rh_set(unit, ecmp_group_idx, 1);
        } else {
            bcm_th_ecmp_group_rh_set(unit, ecmp_group_idx, 0);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_opt_ecmp_lb_mode_reset
 * Purpose:
 *     Reset load balancing mode for the given ECMP group
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      mpintf - (IN) ECMP group index.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_opt_ecmp_lb_mode_reset(int unit,
                        bcm_if_t mpintf)
{
    ecmp_count_entry_t ecmp_count_entry;
    int ecmp_group_idx;
    initial_l3_ecmp_group_entry_t initial_l3_ecmp_group_entry;
    soc_mem_t mem = L3_ECMP_COUNTm;

    ecmp_group_idx = mpintf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_l3_ecmp_hier_tbl) &&
        (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
             ecmp_mode_hierarchical) &&
        (ecmp_group_idx < (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) / 2))) {
        mem = ECMP_GROUP_HIERARCHICALm;
    }
#endif

    /* Update ECMP group table LB field*/
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, ecmp_group_idx,
                &ecmp_count_entry));
    soc_mem_field32_set(unit, mem, &ecmp_count_entry,
            LB_MODEf, 0);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, ecmp_group_idx,
                &ecmp_count_entry));

    if (SOC_MEM_IS_VALID(unit, INITIAL_L3_ECMP_GROUPm)) {
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, INITIAL_L3_ECMP_GROUPm, MEM_BLOCK_ANY,
            ecmp_group_idx, &initial_l3_ecmp_group_entry));
        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
            &initial_l3_ecmp_group_entry, LB_MODEf, 0);
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, INITIAL_L3_ECMP_GROUPm,
            MEM_BLOCK_ALL, ecmp_group_idx, &initial_l3_ecmp_group_entry));
    }

    if (!SOC_IS_TRIDENT3X(unit)) {
        bcm_th_ecmp_group_rh_set(unit, ecmp_group_idx, 0);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_opt_l3_egress_ecmp_rh_shared_copy
 * Purpose:
 *      For members shared by the old and the new ECMP groups,
 *      copy the resilient hash flow set entries containing these
 *      members from the old to the new ECMP group, in order to minimize
 *      flow-to-member reassignments when the old ECMP group in a route
 *      entry is replaced by the new one.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      old_ecmp_group - (IN) Old ECMP group.
 *      new_ecmp_group - (IN) New ECMP group.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_opt_l3_egress_ecmp_rh_shared_copy(int unit, int old_ecmp_group,
        int new_ecmp_group)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t old_ecmp, new_ecmp, replace_ecmp;
    int old_intf_count, new_intf_count;
    bcm_if_t *old_intf_array = NULL;
    bcm_if_t *rh_old_intf_array = NULL;
    bcm_if_t *new_intf_array = NULL;
    int offset;
    int count;
    int max_shared; 
    bcm_if_t *shared_intf_array = NULL;
    bcm_if_t *ex_old_intf_array = NULL;
    bcm_if_t *ex_new_intf_array = NULL;
    bcm_if_t *temp_old_intf_array = NULL;
    int shared_intf_count, ex_new_intf_count, ex_old_intf_count;
    int i, j;
    int num_entries;
    int overlay = 0;

    /* Check if there are any routing entries pointing to the
     * new ECMP group. If so, the new ECMP group's flow set
     * entries should not be modified.
     */
    if (BCM_XGS3_L3_ENT_REF_CNT(BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp),
                new_ecmp_group) > 2) {
        /* When the new ECMP group was created, its reference count
         * was set to 1. Then, in bcm_xgs3_defip_add, its reference count
         * was incremented before this procedure is invoked. Hence,
         * if there are any routing entry already pointing at the new
         * ECMP group, the reference count would be greater than 2.
         */
        return BCM_E_NONE;
    }

    /* Check if the old ECMP group is resilient hash enabled. */
    bcm_l3_egress_ecmp_t_init(&old_ecmp);
    old_ecmp.ecmp_intf = old_ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    BCM_IF_ERROR_RETURN(bcm_esw_l3_egress_ecmp_get(unit, &old_ecmp,
                0, NULL, &old_intf_count));
    if (old_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        /* The old ECMP group was not resilient hash enabled. Copying
         * flow set entries is not applicable.
         */
        return BCM_E_NONE;
    }

    /* Check if the new ECMP group is resilient hash enabled. */
    bcm_l3_egress_ecmp_t_init(&new_ecmp);
    new_ecmp.ecmp_intf = new_ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    BCM_IF_ERROR_RETURN(bcm_esw_l3_egress_ecmp_get(unit, &new_ecmp,
                0, NULL, &new_intf_count));
    if (new_ecmp.dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        /* The new ECMP group is not resilient hash enabled. Copying
         * flow set entries is not applicable.
         */
        return BCM_E_NONE;
    }

    if (old_ecmp.dynamic_size != new_ecmp.dynamic_size) {
        /* The number of flow set entries in old ECMP group is not the
         * same as the number in new ECMP group. Copying
         * flow set entries is not applicable.
         */
        return BCM_E_NONE;
    }

    /* Get old ECMP group members */
    old_intf_array = sal_alloc(old_ecmp.dynamic_size * sizeof(bcm_if_t),
            "old ecmp member array");
    if (NULL == old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(old_intf_array, 0, old_ecmp.dynamic_size * sizeof(bcm_if_t));
    rv = bcm_esw_l3_egress_ecmp_get(unit, &old_ecmp, old_intf_count,
            old_intf_array, &count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    for (i = 0; i < old_intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, old_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                 BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                    old_intf_array[i])) {
            offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif
        else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, old_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        old_intf_array[i] -= offset;
    }

    /* Get new ECMP group members */
    new_intf_array = sal_alloc(new_ecmp.dynamic_size * sizeof(bcm_if_t),
            "new ecmp member array");
    if (NULL == new_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(new_intf_array, 0, new_ecmp.dynamic_size * sizeof(bcm_if_t));
    rv = bcm_esw_l3_egress_ecmp_get(unit, &new_ecmp, new_intf_count,
            new_intf_array, &count);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    for (i = 0; i < new_intf_count; i++) {
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, new_intf_array[i])) {
            offset = BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        else if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
                 BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                    new_intf_array[i])) {
            offset = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        }
#endif
        else if (BCM_XGS3_DVP_EGRESS_IDX_VALID(unit, new_intf_array[i])) {
            offset = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        } else {
            rv = BCM_E_PARAM;
            goto cleanup;
        }
        new_intf_array[i] -= offset;
    }

    /* Determine members shared by old and new ECMP groups, and
     * members exclusive to the old and the new ECMP groups.
     */
    max_shared = (old_intf_count > new_intf_count) ? new_intf_count :
        old_intf_count;
    shared_intf_array = sal_alloc(max_shared * sizeof(bcm_if_t),
            "shared ecmp member array");
    if (NULL == shared_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(shared_intf_array, 0, max_shared * sizeof(bcm_if_t));

    ex_old_intf_array = sal_alloc(old_intf_count * sizeof(bcm_if_t),
            "array of members exclusive to old ecmp group");
    if (NULL == ex_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ex_old_intf_array, 0, old_intf_count * sizeof(bcm_if_t));

    ex_new_intf_array = sal_alloc(new_intf_count * sizeof(bcm_if_t),
            "array of members exclusive to new ecmp group");
    if (NULL == ex_new_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(ex_new_intf_array, 0, new_intf_count * sizeof(bcm_if_t));

    temp_old_intf_array = sal_alloc(old_intf_count * sizeof(bcm_if_t),
            "copy of old_intf_array");
    if (NULL == temp_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memcpy(temp_old_intf_array, old_intf_array, old_intf_count * sizeof(bcm_if_t));

    shared_intf_count = 0;
    ex_new_intf_count = 0;
    for (i = 0; i < new_intf_count; i++) {
        for (j = 0; j < old_intf_count; j++) {
            if (new_intf_array[i] == temp_old_intf_array[j]) {
                shared_intf_array[shared_intf_count++] = new_intf_array[i];
                /* Mark the matched element invalid */
                temp_old_intf_array[j] = BCM_XGS3_L3_INVALID_INDEX;
                break;
            }
        }
        if (j == old_intf_count) {
            ex_new_intf_array[ex_new_intf_count++] = new_intf_array[i];
        }
    }

    ex_old_intf_count = 0;
    for (i = 0; i < old_intf_count; i++) {
        if (temp_old_intf_array[i] != BCM_XGS3_L3_INVALID_INDEX) {
            ex_old_intf_array[ex_old_intf_count++] = temp_old_intf_array[i];
        }
    }

    if (shared_intf_count == 0) {
        /* The old and the new ECMP groups don't share any members.
         * Copying flow set entries is not applicable.
         */
        rv = BCM_E_NONE;
        goto cleanup;
    }

    /* Get old ECMP RH Set */
    rh_old_intf_array = sal_alloc(old_ecmp.dynamic_size * sizeof(bcm_if_t),
            "new ecmp member array");
    if (NULL == rh_old_intf_array) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    sal_memset(rh_old_intf_array, 0, old_ecmp.dynamic_size * sizeof(bcm_if_t));
    rv = bcm_xgs3_l3_egress_multipath_get(unit,
                                          old_ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit),
                                          BCM_XGS3_L3_ECMP_MAX(unit),
                                          rh_old_intf_array, &num_entries);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    /* Modify the flow set entry buffer to achieve balance among
     * shared members and members exclusive to the new ECMP group,
     * while keeping modifications of entries containing the
     * shared members to a minimum.
     */

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (soc_feature(unit, soc_feature_l3_ecmp_hier_tbl) &&
       (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
             ecmp_mode_hierarchical) &&
        (new_ecmp_group < (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) / 2))) {
        overlay = 1;
    }
#endif

    rv = _bcm_opt_ecmp_rh_balance_with_min_shared_mod(unit,
                                 num_entries, overlay, rh_old_intf_array,
                                 shared_intf_count, shared_intf_array,
                                 ex_old_intf_count, ex_old_intf_array,
                                 ex_new_intf_count, ex_new_intf_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Replace the new ECMP group's RH set entries with the buffer */
    bcm_l3_egress_ecmp_t_init(&replace_ecmp);
    replace_ecmp.ecmp_intf = new_ecmp_group + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    replace_ecmp.flags = BCM_L3_REPLACE | BCM_L3_WITH_ID;
    /* Set no sorting flag so that RH set is not sorted */
    replace_ecmp.ecmp_group_flags = BCM_L3_ECMP_PATH_NO_SORTING;
    /* Set RH flag so that RH length checks apply */
    replace_ecmp.ecmp_group_flags |= BCM_L3_ECMP_RH_OPT;
    replace_ecmp.max_paths = num_entries;
    rv = bcm_esw_l3_egress_ecmp_create(unit, &replace_ecmp, num_entries,
                                       rh_old_intf_array);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    BCM_XGS3_L3_ECMP_GROUP_FLAGS_RESET(unit,
                                    (replace_ecmp.ecmp_intf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit)),
                                    BCM_L3_ECMP_PATH_NO_SORTING);

    /* set back lb mode. no need to set intf array, since they are same */
    rv = bcm_opt_ecmp_lb_mode_set(unit, new_ecmp_group, 
                                 BCM_TH_L3_ECMP_LB_MODE_RH);

cleanup:
    if (old_intf_array) {
        sal_free(old_intf_array);
    }
    if (new_intf_array) {
        sal_free(new_intf_array);
    }
    if (shared_intf_array) {
        sal_free(shared_intf_array);
    }
    if (ex_old_intf_array) {
        sal_free(ex_old_intf_array);
    }
    if (ex_new_intf_array) {
        sal_free(ex_new_intf_array);
    }
    if (temp_old_intf_array) {
        sal_free(temp_old_intf_array);
    }
    if (rh_old_intf_array) {
        sal_free(rh_old_intf_array);
    }

    return rv;
}


#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcm_opt_l3_ecmp_rh_member_sync 
 * Purpose:
 *      Store ECMP members in the scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_opt_l3_ecmp_rh_member_sync(int unit, uint8 **scache_ptr, int offset)
{
    int16 i;
    int j;
    bcm_if_t *rh_intf_arr_ptr = NULL;
    int intf_count;
    uint8 *end_scache_ptr;
    int16 *grp_ptr;
    bcm_if_t *mem_ptr;
    SOC_SCACHE_DUMP_DECL;

    if ((scache_ptr == NULL) || (*scache_ptr == NULL)) {
        return BCM_E_PARAM;
    }
    SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L3),
                          *scache_ptr - offset);
    /* ECMP RH max paths info */
    sal_memcpy((*scache_ptr), &BCM_XGS3_L3_ECMP_RH_MAX_PATHS(unit), 
                sizeof(int));
    SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_rh_max_paths");
    SOC_SCACHE_DUMP_DATA_HEAD((*scache_ptr), sizeof(int));
    SOC_SCACHE_DUMP_DATA_Vd((*scache_ptr), int);
    SOC_SCACHE_DUMP_DATA_TAIL();
    SOC_SCACHE_DUMP_DATA_END();
    (*scache_ptr) += sizeof(int);

    SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_rh_group_idx;max_paths");
    /* ECMP RH per group max_paths info */
    for (i = 0; i < BCM_XGS3_L3_ECMP_MAX_GROUPS(unit); i++) {
        rh_intf_arr_ptr = (_opt_ecmp_rh_info[unit]->rhg[i].rh_intf_arr);
        if (rh_intf_arr_ptr) {
            sal_memcpy((*scache_ptr),                     
                       &(_opt_ecmp_rh_info[unit]->rhg[i].max_paths),
                       sizeof(uint16));
        }
        SOC_SCACHE_DUMP_DATA_UINT16d_TV((*scache_ptr), i);
        (*scache_ptr) += sizeof(uint16);
    }
    SOC_SCACHE_DUMP_DATA_END();

    /*
     * Scache will have size as large as ECMP Member table.
     * Each entry will be of (ecmp group index, ecmp member).
     * Multiple members for a group will take up multiple consecutive entries
     */
    end_scache_ptr = *scache_ptr + 
           (RH_ECMP_MEMBER_TBL_MAX(unit) * (sizeof(int16) + sizeof(bcm_if_t)));
    /* Copy (group index, member) tuple for each group into the member table */
    SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_rh_group_id;rh_member_intf");
    for (i = 0; i < BCM_XGS3_L3_ECMP_MAX_GROUPS(unit); i++) {
        rh_intf_arr_ptr = (_opt_ecmp_rh_info[unit]->rhg[i].rh_intf_arr);
        if (rh_intf_arr_ptr) {
            intf_count = _opt_ecmp_rh_info[unit]->rhg[i].rh_intf_count;

            /* Store groups with empty members */
            if (intf_count == 0) {
                intf_count = 1;
            }
            for (j = 0; j < intf_count; j++) {
                SOC_SCACHE_DUMP_DATA_HEAD((*scache_ptr),
                                          sizeof(int16) + sizeof(bcm_if_t));
                /* Increment group index before storing so that 0 can be used to
                 * check for an invalid entry
                 */
                grp_ptr = (int16 *)(*scache_ptr);
                *grp_ptr = i + 1;
                SOC_SCACHE_DUMP_DATA_Vx((*scache_ptr), int16);
                *scache_ptr += sizeof(int16);
                mem_ptr = (bcm_if_t *)(*scache_ptr);
                *mem_ptr = rh_intf_arr_ptr[j];
                SOC_SCACHE_DUMP_DATA_Vx((*scache_ptr), bcm_if_t);
                *scache_ptr += sizeof(bcm_if_t);
                SOC_SCACHE_DUMP_DATA_TAIL();
            }

        }
    }
    SOC_SCACHE_DUMP_DATA_END();

    *scache_ptr = end_scache_ptr;

    SOC_SCACHE_DUMP_STOP();

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_opt_l3_ecmp_rh_member_recover
 * Purpose:
 *      Recover RH ECMP members from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_opt_l3_ecmp_rh_member_recover(int unit, uint8 **scache_ptr,
                                  int ecmp_max_paths, int offset)
{
    bcm_if_t **rh_intf_arr_ptr;
    int stable_size = 0;
    int intf_count;
    int j;
    uint8 *end_scache_ptr;
    int16 *grp_ptr;
    int16 ecmp_group_index;
    bcm_if_t *mem_ptr;
    uint8 *start_grp_ptr;
    bcm_if_t *hash_intf_array = NULL;
    int alloc_size;
    uint16 hash;
    int grp_size;
    ecmp_count_entry_t entry;
    int rv = BCM_E_NONE;
    SOC_SCACHE_DUMP_DECL;

    if ((scache_ptr == NULL) || (*scache_ptr == NULL)) {
        return BCM_E_PARAM;
    }

    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

        if (stable_size == 0) { /* level 1 */
            /* Nothing to recover */
        } else {  /* Level 2 */
            SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L3),
                                  *scache_ptr - offset);
            /* Set max_paths here for hash calculation */
            BCM_XGS3_L3_ECMP_MAX_PATHS(unit) = ecmp_max_paths;
            /* recover from scache into book-keeping structs */
            /* Copy RH max paths */
            sal_memcpy(&BCM_XGS3_L3_ECMP_RH_MAX_PATHS(unit), (*scache_ptr), 
                       sizeof(int));
            SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_rh_max_paths");
            SOC_SCACHE_DUMP_DATA_HEAD((*scache_ptr), sizeof(int));
            SOC_SCACHE_DUMP_DATA_Vd((*scache_ptr), int);
            SOC_SCACHE_DUMP_DATA_TAIL();
            SOC_SCACHE_DUMP_DATA_END();
            (*scache_ptr) += sizeof(int);

            SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_rh_group_idx;max_paths");
            /* Retrieve ECMP RH per group max_paths info */
            for (j = 0; j < BCM_XGS3_L3_ECMP_MAX_GROUPS(unit); j++) {
                sal_memcpy(&(_opt_ecmp_rh_info[unit]->rhg[j].max_paths),
                           (*scache_ptr),
                           sizeof(uint16));
                SOC_SCACHE_DUMP_DATA_UINT16d_TV((*scache_ptr), j);
                (*scache_ptr) += sizeof(uint16);
            }
            SOC_SCACHE_DUMP_DATA_END();

            end_scache_ptr = *scache_ptr + 
                              (RH_ECMP_MEMBER_TBL_MAX(unit) * (sizeof(int16) + sizeof(bcm_if_t)));
            SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_rh_group_id;rh_member_intf");
            while ((*scache_ptr) < end_scache_ptr) {
                start_grp_ptr = (*scache_ptr);
                ecmp_group_index = *((int16 *)start_grp_ptr);
                /* Look for the first member of a valid group */
                if (ecmp_group_index != 0) {
                    intf_count = 0;
                    grp_size = 0;
                    /* Find member count */
                    do {
                        intf_count++;
                        start_grp_ptr += (sizeof(int16) + sizeof(bcm_if_t));
                        grp_ptr = (int16 *)(start_grp_ptr);
                    } while (ecmp_group_index == *grp_ptr);
                    grp_size = intf_count;
                    /* Decrement group index to get the original value */
                    ecmp_group_index--;
                    _opt_ecmp_rh_info[unit]->rhg[ecmp_group_index].rh_intf_count =
                               intf_count; 
                    rh_intf_arr_ptr =
                         &(_opt_ecmp_rh_info[unit]->rhg[ecmp_group_index].rh_intf_arr);
                    *rh_intf_arr_ptr = sal_alloc(sizeof(bcm_if_t) * intf_count,
                                              "ECMP RH entry count array");
                    if (NULL == *rh_intf_arr_ptr) {
                        SOC_SCACHE_DUMP_STOP();
                        return(BCM_E_MEMORY);
                    }
                    sal_memset(*rh_intf_arr_ptr, 0, (sizeof(bcm_if_t) *
                                     intf_count));
                    /* Go back to the beginning of the group and copy ECMP member array */
                    start_grp_ptr = (*scache_ptr);
                    for (j = 0; j < intf_count; j++) {
                        SOC_SCACHE_DUMP_DATA_HEAD(start_grp_ptr,
                            sizeof(int16) + sizeof(bcm_if_t));
                        SOC_SCACHE_DUMP_DATA_Vx(start_grp_ptr, int16);
                        start_grp_ptr += sizeof(int16);
                        SOC_SCACHE_DUMP_DATA_Vx(start_grp_ptr, bcm_if_t);
                        mem_ptr = (bcm_if_t *)(start_grp_ptr);
                        *(*rh_intf_arr_ptr + j) = *mem_ptr;
                        start_grp_ptr += sizeof(bcm_if_t);
                        SOC_SCACHE_DUMP_DATA_TAIL();
                    }

                    /* Group with empty members will have invalid interface */
                    if (!(BCM_XGS3_L3_EGRESS_IDX_VALID(unit, *(*rh_intf_arr_ptr)))
                        && !(BCM_XGS3_DVP_EGRESS_IDX_VALID(unit,
                        *(*rh_intf_arr_ptr)))) {
                        _opt_ecmp_rh_info[unit]->rhg[ecmp_group_index].
                                rh_intf_count = 0;
                        intf_count = 0;
                    }

                    /*
                     * Sort ECMP member array. This should already be in sorted order.
                     * Sort again before computing data hash to be safe
                     */
                    alloc_size = sizeof(bcm_if_t) * BCM_XGS3_L3_ECMP_MAX(unit);
                    hash_intf_array = sal_alloc(alloc_size, "RH intf array");
                    if (NULL == hash_intf_array) {
                        SOC_SCACHE_DUMP_STOP();
                        return(BCM_E_MEMORY);
                    }
                    sal_memset(hash_intf_array, 0, alloc_size);
                    sal_memcpy(hash_intf_array, *rh_intf_arr_ptr, 
                                     sizeof(bcm_if_t) * intf_count);
                    _shr_sort(hash_intf_array, intf_count, sizeof(int), _opt_rh_cmp_int);
                    sal_memcpy(*rh_intf_arr_ptr, hash_intf_array, 
                                         (sizeof(bcm_if_t) * intf_count));     
                    /* Compute member hash */
                    _bcm_opt_rh_ecmp_grp_hash_calc(unit, hash_intf_array, &hash);
                    _opt_ecmp_rh_info[unit]->rhg[ecmp_group_index].data_hash = hash;
                    sal_free(hash_intf_array);
                    hash_intf_array = NULL;

                    rv = READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                                             ecmp_group_index, &entry);
                    if (BCM_FAILURE(rv)) {
                        SOC_SCACHE_DUMP_STOP();
                        return rv;
                    }
                    if (BCM_TH_L3_ECMP_LB_MODE_RH ==
                        soc_L3_ECMP_COUNTm_field32_get(unit, &entry, LB_MODEf)) {
                        bcm_th_ecmp_group_rh_set(unit, ecmp_group_index, 1);
                    }

                    /* Advance scache pointer to the beginning of next distinct group id */
                    *scache_ptr += ((sizeof(int16) + sizeof(bcm_if_t)) * grp_size);
                } else {
                    *scache_ptr += (sizeof(int16) + sizeof(bcm_if_t));
                }
            }
            SOC_SCACHE_DUMP_DATA_END();

            SOC_SCACHE_DUMP_STOP();

            *scache_ptr = end_scache_ptr;
        }
    }

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE

/*
 * Function:
 *     bcm_opt_ecmp_rh_sw_dump
 * Purpose:
 *     Displays ECMP resilient hashing state maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void bcm_opt_ecmp_rh_sw_dump(int unit)
{
    int i, j;
    bcm_if_t *rh_intf_arr_ptr;

    LOG_CLI((BSL_META_U(unit,
                        "  ECMP Resilient Hashing Info -\n")));

    /* Print ECMP RH member entries*/
    if (_opt_ecmp_rh_info[unit]) {
        for (i = 0; i < BCM_XGS3_L3_ECMP_MAX_GROUPS(unit); i++) {
             /* Get ECMP member array */
             rh_intf_arr_ptr = (_opt_ecmp_rh_info[unit]->rhg[i].rh_intf_arr);
             if (rh_intf_arr_ptr) {
                 LOG_CLI((BSL_META_U(unit,
                                     "RH Group  %4d, Member count %4d, Max paths %4d: "),
                                     i, _opt_ecmp_rh_info[unit]->rhg[i].rh_intf_count,
                                     _opt_ecmp_rh_info[unit]->rhg[i].max_paths));
                 for (j = 0; j < _opt_ecmp_rh_info[unit]->rhg[i].rh_intf_count; j++)
                     {
                         LOG_CLI((BSL_META_U(unit,
                                             " %4d"), rh_intf_arr_ptr[j]));
                     }
                 LOG_CLI((BSL_META_U(unit,
                                     "\n")));
             }
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    return;
}

#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* BCM_TOMAHAWK_SUPPORT && INCLIDE_L3 */

