/**
 * \file multicast_imp.c
 * 
 *
 * General MULTICAST functionality for DNX.
 * Dedicated set of MULTICAST APIs are distributed between multicast_*.c files: \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif /* */
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

/*
 *  Include files. {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/multicast.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_bier_db_access.h>

#include <bcm_int/dnx/bier/bier.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include "multicast_imp.h"
#include "multicast_pp.h"
/*
 *  }
 */

/*
 *  MACROs {
 */

/*
 *  }
 */

/**
 * \brief - iterates over bitmap and performs one of the actions
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] bitmap_ptr - MCDB index to bitmap
 *   \param [in] cud - encaps id of the bitmap replications
 *   \param [in] action - see remarks
 *   \param [in] bmp - an array which holds the bitmap for core_id
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST - removes the entries specified at the rep_array.
 *                                    if entry will not be found _SHR_E_NOT_FOUND error will be returned.
 * DNX_MULTICAST_ENTRY_ITER_BMP_REMOVE_NOT_IN_LIST - update the HW bitmap with the replications which are no longer exist.
 * DNX_MULTICAST_ENTRY_ITER_BMP_ADD - Adds the bitmap to the existing HW bitmap and clears added replications from the bmp
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL - removes all the entries in the bmp link list
 * DNX_MULTICAST_ENTRY_ITER_GET - copies the content of the link list to the bmp
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_bitmap_iter(
    int unit,
    uint32 bitmap_ptr,
    uint32 cud,
    dnx_multicast_entry_iter_action_t action,
    uint32 *bmp)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 cur_entry_replaced;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Loop over the link list
     */
    while (bitmap_ptr != DNX_MULTICAST_END_OF_LIST_PTR)
    {
        cur_entry_replaced = FALSE;
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, bitmap_ptr, &entry_val));
        if ((entry_val.format != DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT) &&
            (entry_val.format != DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid format %u for this function", entry_val.format);
        }
        switch (action)
        {
            case DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST:
                entry_val.bmp_val &= ~bmp[entry_val.bmp_offset];

                /*
                 * if the entry is empty it can be removed from the link list 
                 */
                if (entry_val.bmp_val)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, bitmap_ptr, &entry_val));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, bitmap_ptr, entry_val.prev_entry, entry_val.next_entry_id, TRUE,
                                     &cur_entry_replaced));
                }
                break;
            case DNX_MULTICAST_ENTRY_ITER_BMP_REMOVE_NOT_IN_LIST:
                entry_val.bmp_val &= bmp[entry_val.bmp_offset];
                if (entry_val.bmp_val)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, bitmap_ptr, &entry_val));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, bitmap_ptr, entry_val.next_entry_id, entry_val.prev_entry, TRUE,
                                     &cur_entry_replaced));
                }
                break;
            case DNX_MULTICAST_ENTRY_ITER_BMP_ADD:
                entry_val.bmp_val |= bmp[entry_val.bmp_offset];
                bmp[entry_val.bmp_offset] = 0;
                SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, bitmap_ptr, &entry_val));
                break;
            case DNX_MULTICAST_ENTRY_ITER_GET:
                bmp[entry_val.bmp_offset] = entry_val.bmp_val;
                break;
            case DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL:
                SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, bitmap_ptr));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid action %u used for this function", action);
        }
        /*
         * If the current entry is replaced, the next entry pointer should not be moved to next member
         */
        if (cur_entry_replaced == FALSE)
        {
            bitmap_ptr = entry_val.next_entry_id;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See multicast_imp.h
 */
shr_error_e
dnx_multicast_bitmap_delete(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    dnx_multicast_entry_iter_action_t action,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    uint32 core_id, entry_id;
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 *bmp[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { NULL };
    uint32 bmp_cud[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        bmp[core_id] = sal_alloc(sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit), "bmp");

        sal_memset(bmp[core_id], 0, sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit));
    }

    if (action == DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_reps_to_bmp(unit, nof_replications, rep_array, bmp, bmp_cud));
    }
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group, TRUE, DNX_MULTICAST_IS_INGRESS(flags),
                                                  core_id, &entry_id));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, entry_id, &entry_val));

        /**
         * verify that the CUD is correct
         */
        if (!DNX_MULTICAST_IS_EMPTY_BMP_GROUP(entry_val))
        {
            if ((action == DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST) && (bmp_cud[core_id] != entry_val.cud)
                && (bmp_cud[core_id] != DNX_MULTICAST_INVALID_CUD))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "wrong cud. cud to be deleted (%u), bitmap cud (%u)",
                             bmp_cud[core_id], entry_val.cud);
            }
        }

        if (entry_val.next_entry_id != DNX_MULTICAST_END_OF_LIST_PTR)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter(unit, entry_val.bmp_ptr, entry_val.cud, action, bmp[core_id]));

            if (action == DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_entry_clear(unit, entry_id, FALSE, TRUE));
            }
        }
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            break;
        }
    }

exit:

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_FREE(bmp[core_id]);
    }

    SHR_FUNC_EXIT;
}

/**
 * See multicast_imp.h
 */
shr_error_e
dnx_multicast_bitmap_get(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int max_replications,
    bcm_multicast_replication_t * rep_array,
    int *rep_count)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 entry_id;
    int core_rep_count = 0, core_id;

    uint32 *bmp = NULL;

    SHR_FUNC_INIT_VARS(unit);

    *rep_count = 0;
    bmp = sal_alloc(sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit), "bmp");
    sal_memset(bmp, 0, sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                        (unit, group, TRUE, DNX_MULTICAST_IS_INGRESS(flags), core_id, &entry_id));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, entry_id, &entry_val));
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter
                        (unit, entry_val.next_entry_id, entry_val.cud, DNX_MULTICAST_ENTRY_ITER_GET, bmp));

        if (!DNX_MULTICAST_IS_EMPTY_BMP_GROUP(entry_val))
        {
            if (max_replications != 0)
            {
                /** get actual replications to rep_array, it is expected that rep_array will have sufficient storage for all reps */
                SHR_IF_ERR_EXIT(dnx_multicast_bitmap_bmp_to_reps(unit, entry_val.cud, core_id, bmp,
                                                                 max_replications - core_rep_count,
                                                                 rep_array + core_rep_count, &core_rep_count));
            }
            else
            {
                /** only count replications and return their number */
                SHR_BITCOUNT_RANGE(bmp, core_rep_count, 0, DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit));
            }
            *rep_count += core_rep_count;
        }

        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            break;
        }
    }

exit:
    SHR_FREE(bmp);
    SHR_FUNC_EXIT;
}

/**
 * See multicast_imp.h
 */
shr_error_e
dnx_multicast_bitmap_create(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 is_set,
    int rep_count,
    bcm_multicast_replication_t * rep_array)
{
    uint32 allocated_entry;
    uint32 next_entry_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 nof_new_entries_needed = 0, bmp_entry_id, cur_entry_id, core_id;
    uint32 *bmp_new_reps[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { NULL };
    uint32 *bmp_existing_reps[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { NULL };
    uint32 bitmap_first_entry_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 bmp_cud[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    dnx_multicast_mcdb_dbal_field_t entry_val;
    int nof_free_elements;
    int ext_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        bmp_new_reps[core_id] = sal_alloc(sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit), "bmp");
        sal_memset(bmp_new_reps[core_id], 0, sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit));

        bmp_existing_reps[core_id] = sal_alloc(sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit), "bmp");
        sal_memset(bmp_existing_reps[core_id], 0, sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit));
    }
    /*
     * Convert destination to bitmap
     */
    SHR_IF_ERR_EXIT(dnx_multicast_bitmap_reps_to_bmp(unit, rep_count, rep_array, bmp_new_reps, bmp_cud));
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        /*
         * Get the existing bitmap
         */
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                        (unit, group, TRUE, DNX_MULTICAST_IS_INGRESS(flags), core_id, &bitmap_first_entry_id[core_id]));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, bitmap_first_entry_id[core_id], &entry_val));
        next_entry_id[core_id] = entry_val.next_entry_id;
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter(unit, entry_val.bmp_ptr, entry_val.cud,
                                                  DNX_MULTICAST_ENTRY_ITER_GET, bmp_existing_reps[core_id]));

        /*
         * Calculate the number of new entries are which needed by comparing the new bitmap
         * with the existing bitmap. Count only entries that are found in the new bitmap but not in the existing bitmap.
         */
        for (bmp_entry_id = 0; bmp_entry_id < DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit); bmp_entry_id++)
        {
            if ((bmp_new_reps[core_id][bmp_entry_id] != 0) && (bmp_existing_reps[core_id][bmp_entry_id] == 0))
            {
                nof_new_entries_needed++;
            }
        }

        /**
         * check whether the new replication CUD and the old one is the same. relevant for add API.
         */
        if (!DNX_MULTICAST_IS_EMPTY_BMP_GROUP(entry_val))
        {
            if ((is_set == FALSE) && (bmp_cud[core_id] != entry_val.cud)
                && (bmp_cud[core_id] != DNX_MULTICAST_INVALID_CUD))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "core replications with different cuds can't be used "
                             "for bitmap group. cud1 (%u), cud2 (%u)", bmp_cud[core_id], entry_val.cud);
            }
        }
    }

    /*
     * check whether the entries can be allocated
     */
    SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_elements));
    if (nof_free_elements < nof_new_entries_needed)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "there are not enough resources. available (%u), needed (%u)",
                     nof_free_elements, nof_new_entries_needed);
    }

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, bitmap_first_entry_id[core_id], &entry_val));
        /*
         * remove the entries user wish to remove. relevant for set API only 
         */
        if (is_set)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter(unit, entry_val.bmp_ptr, entry_val.cud,
                                                      DNX_MULTICAST_ENTRY_ITER_BMP_REMOVE_NOT_IN_LIST,
                                                      bmp_new_reps[core_id]));
        }

        /*
         * update the existing HW bitmap with the original bitmap 
         */
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter(unit, entry_val.bmp_ptr, entry_val.cud,
                                                  DNX_MULTICAST_ENTRY_ITER_BMP_ADD, bmp_new_reps[core_id]));

        /*
         * loop over the new entries needed, allocate and add them to existing link list 
         */
        for (bmp_entry_id = 0; bmp_entry_id < DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit); bmp_entry_id++)
        {
            if (bmp_new_reps[core_id][bmp_entry_id])
            {
                SHR_IF_ERR_EXIT(dnx_multicast_entry_allocate(unit, flags, core_id, FALSE, &allocated_entry));
                cur_entry_id = allocated_entry;
                entry_val.next_entry_id = next_entry_id[core_id];
                entry_val.bmp_val = bmp_new_reps[core_id][bmp_entry_id];
                entry_val.bmp_offset = bmp_entry_id;
                entry_val.format =
                    (ext_mode ? DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT : DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT);

                /*
                 * write entry to HW 
                 */
                SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, cur_entry_id, &entry_val));

                /*
                 * next entry prev_pointer set to cur entry 
                 */
                if (next_entry_id[core_id] != DNX_MULTICAST_END_OF_LIST_PTR)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, next_entry_id[core_id], cur_entry_id));
                }

                /*
                 * next entry added will use next_entry_id to set the next pointer 
                 */
                next_entry_id[core_id] = cur_entry_id;
            }
        }

        /*
         * the first entry (bitmap pointer) is written to HW
         */

        entry_val.format = DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT;
        entry_val.next_entry_id = next_entry_id[core_id];
        entry_val.bmp_ptr = next_entry_id[core_id];
        entry_val.cud = (bmp_cud[core_id] == DNX_MULTICAST_INVALID_CUD) ? entry_val.cud : bmp_cud[core_id];
        SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, bitmap_first_entry_id[core_id], &entry_val));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, next_entry_id[core_id], bitmap_first_entry_id[core_id]));
    }
exit:

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_FREE(bmp_new_reps[core_id]);
        SHR_FREE(bmp_existing_reps[core_id]);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - checks whether an MCDB entry includes one of the replications and removes it from the link list.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] entry_val - pointer to structure filled with the entry values
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] entry_id - MCDB index
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the replications array
 *   \param [out] cur_entry_replaced - indicates whether current entry id is replaced with other entry
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * next_entry_id need to updated only the current entry is removed and replaced by another entry.
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_rep_remove(
    int unit,
    uint32 flags,
    dnx_multicast_mcdb_dbal_field_t * entry_val,
    uint32 core_id,
    uint32 entry_id,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    uint32 *cur_entry_replaced)
{
    uint32 rep_id, is_belong_to_core;
    int profile_found, egress_trunk_id;
    dnx_multicast_mcdb_dbal_field_t rep_entry_val;
    uint32 destination_removed = FALSE;
    uint32 entry_removed = FALSE;
    uint32 egr_invalid_destination;
    uint8 double_destination_removed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));

    for (rep_id = 0; rep_id < rep_count_in; rep_id++)
    {
        /*
         * for egress trunk if the trunk gport does not created, gport_to_hw function will fail.
         */
        if (DNX_MULTICAST_IS_EGRESS(flags) && BCM_GPORT_IS_TRUNK(rep_array[rep_id].port))
        {
            SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get
                            (unit, BCM_GPORT_TRUNK_GET(rep_array[rep_id].port), &egress_trunk_id, &profile_found));
            if (!profile_found)
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dnx_multicast_convert_gport_to_hw(unit, rep_array[rep_id].port, rep_array[rep_id].encap1,
                                                          flags, core_id, &rep_entry_val, &is_belong_to_core,
                                                          DNX_MULTICAST_TRUNK_ACTION_DESTROY));
        if (is_belong_to_core == FALSE)
        {
            continue;
        }

        if (rep_entry_val.format == DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT &&
            entry_val->format != DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT)
        {
            /** in case of bitmap pointer - the formats must match */
            continue;
        }
        if (rep_entry_val.format == DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT &&
            entry_val->format != DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT)
        {
            /** in case of link list - the formats must match */
            continue;
        }
        switch (entry_val->format)
        {
            case DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT:
                if (rep_entry_val.dest[0] == entry_val->dest[0] && rep_entry_val.cud == entry_val->cud)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, entry_id, entry_val->prev_entry, entry_val->next_entry_id, FALSE,
                                     cur_entry_replaced));
                    destination_removed = TRUE;
                    entry_removed = TRUE;
                }
                break;
            case DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT:
                if (rep_entry_val.dest[0] == entry_val->dest[0] && rep_entry_val.cud == entry_val->cud)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, entry_id, entry_val->prev_entry, entry_val->next_entry_id, FALSE,
                                     cur_entry_replaced));
                    destination_removed = TRUE;
                    entry_removed = TRUE;
                }
                break;
            case DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT:
                /** first replication match, the second invalid */
                if ((rep_entry_val.dest[0] == entry_val->dest[0]) &&
                    (egr_invalid_destination == entry_val->dest[1]) && (rep_entry_val.cud == entry_val->cud))
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, entry_id, entry_val->prev_entry, entry_val->next_entry_id, FALSE,
                                     cur_entry_replaced));
                    destination_removed = TRUE;
                    entry_removed = TRUE;
                }
                /** second replication match, the first invalid */
                else if ((rep_entry_val.dest[0] == entry_val->dest[1]) &&
                         (egr_invalid_destination == entry_val->dest[0]) && (rep_entry_val.cud == entry_val->cud))
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, entry_id, entry_val->prev_entry, entry_val->next_entry_id, FALSE,
                                     cur_entry_replaced));
                    destination_removed = TRUE;
                    entry_removed = TRUE;
                }
                /** first and second match */
                else if ((rep_entry_val.dest[0] == entry_val->dest[0]) &&
                         (rep_entry_val.dest[0] == entry_val->dest[1]) && (rep_entry_val.cud == entry_val->cud))
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, entry_id, entry_val->prev_entry, entry_val->next_entry_id, FALSE,
                                     cur_entry_replaced));
                    destination_removed = TRUE;
                    double_destination_removed = TRUE;
                    entry_removed = TRUE;
                }
                /** one match and one valid unmatch replication */
                else if ((rep_entry_val.dest[0] == entry_val->dest[0] || rep_entry_val.dest[0] == entry_val->dest[1])
                         && rep_entry_val.cud == entry_val->cud)
                {
                    if (rep_entry_val.dest[0] == entry_val->dest[0])
                    {
                        entry_val->dest[0] = egr_invalid_destination;
                    }
                    else
                    {
                        entry_val->dest[1] = egr_invalid_destination;
                    }
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, entry_id, entry_val));
                    destination_removed = TRUE;
                }
                break;
            case DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT:
                if (rep_entry_val.bmp_ptr == entry_val->bmp_ptr && rep_entry_val.cud == entry_val->cud)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, entry_id, entry_val->prev_entry, entry_val->next_entry_id, FALSE,
                                     cur_entry_replaced));
                    destination_removed = FALSE;
                    entry_removed = TRUE;
                }
                break;
            case DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT:
                if (rep_entry_val.link_list_ptr == entry_val->link_list_ptr)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                    (unit, entry_id, entry_val->prev_entry, entry_val->next_entry_id, FALSE,
                                     cur_entry_replaced));
                    destination_removed = FALSE;
                    entry_removed = TRUE;
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_CONFIG, "unexpected format %u found", entry_val->format);
        }
        if (destination_removed)
        {
            /** When removing a replication which is TRUNK the egress profile must be removed*/
            if (BCM_GPORT_IS_TRUNK(rep_array[rep_id].port) && DNX_MULTICAST_IS_EGRESS_GROUP(flags))
            {
                SHR_IF_ERR_EXIT(dnx_trunk_egress_destroy(unit, BCM_GPORT_TRUNK_GET(rep_array[rep_id].port)));
                if (double_destination_removed)
                {
                    SHR_IF_ERR_EXIT(dnx_trunk_egress_destroy(unit, BCM_GPORT_TRUNK_GET(rep_array[rep_id].port)));
                }
            }
        }
        if (entry_removed)
        {
            /*
             * If the current entry is replaced, the entry need to be read again in order to get the updated next pointer
             */
            if (cur_entry_replaced)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, entry_id, entry_val));
                cur_entry_replaced = FALSE;
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - iterates over group link list or a link list started at entry_id and performs one of the actions
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group id
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] entry_id - MCDB index
 *   \param [in] action - see remarks
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the replications array
 *   \param [in] rep_count_out - pointer to number of replications returned by this function for "get" action
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST - removes the entries specified at the rep_array.
 *                                    if an entry will not be found _SHR_E_NOT_FOUND error will be returned
 * DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL - removes all the entries in the link list
 * DNX_MULTICAST_ENTRY_ITER_GET - copies the content of the link list to the rep_array
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_linklist_iter(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 *entry_id,
    dnx_multicast_entry_iter_action_t action,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    int *rep_count_out)
{
    uint32 core_id, rep_id = 0, cur_entry_id;
    dnx_multicast_mcdb_dbal_field_t entry_val;
    bcm_gport_t dest1, dest2;
    bcm_if_t cud;
    uint8 count_only;
    uint32 cur_entry_replaced;
    SHR_FUNC_INIT_VARS(unit);

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        if (entry_id != NULL)
        {
            cur_entry_id = entry_id[core_id];
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group, FALSE, DNX_MULTICAST_IS_INGRESS(flags),
                                                      core_id, &cur_entry_id));
        }

        /*
         * loop over the link list 
         */
        while (cur_entry_id != DNX_MULTICAST_END_OF_LIST_PTR)
        {
            cur_entry_replaced = FALSE;
            SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, cur_entry_id, &entry_val));
            switch (action)
            {
                case DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST:
                    SHR_IF_ERR_EXIT(dnx_multicast_rep_remove
                                    (unit, flags, &entry_val, core_id, cur_entry_id, rep_count_in, rep_array,
                                     &cur_entry_replaced));
                    break;

                case DNX_MULTICAST_ENTRY_ITER_GET:
                    /** when rep_count_in is 0, only count entries without getting them */
                    count_only = !rep_count_in;
                    SHR_IF_ERR_EXIT(dnx_multicast_convert_hw_to_gport(unit, entry_val, core_id, &dest1, &dest2, &cud));
                    if (core_id && entry_val.format == DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT)
                    {
                        /** Trunk replications are always created on both cores.
                         * No need to return the same replication twice */
                        break;
                    }

                    if (core_id && entry_val.format == DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT)
                    {
                        /** Trunk replications are always created on both cores.
                         * No need to return the same replication twice */
                        break;
                    }

                    if (core_id && BCM_GPORT_IS_TRUNK(dest1))
                    {
                        /** Trunk replications are always created on both cores.
                         * No need to return the same replication twice */
                        break;
                    }

                    if (dest1 != BCM_GPORT_INVALID)
                    {
                        if (!count_only)
                        {
                            if (rep_id == rep_count_in)
                            {
                                *rep_count_out = rep_id;
                                SHR_ERR_EXIT(_SHR_E_PARAM,
                                             "Caller supplied too few replication buffers (%u) for output for this multicast group (%d)\n",
                                             rep_count_in, group);
                            }
                            rep_array[rep_id].port = dest1;
                            rep_array[rep_id].encap1 = cud;
                            rep_array[rep_id].encap2 = BCM_IF_INVALID;
                        }
                        rep_id++;
                    }
                    if ((entry_val.format == DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT) && (dest2 != BCM_GPORT_INVALID))
                    {
                        if (!count_only)
                        {
                            if (rep_id == rep_count_in)
                            {
                                *rep_count_out = rep_id;
                                SHR_ERR_EXIT(_SHR_E_PARAM, "There are not enough buffers (%u) for replications\n",
                                             rep_count_in);
                            }
                            rep_array[rep_id].port = dest2;
                            rep_array[rep_id].encap1 = cud;
                            rep_array[rep_id].encap2 = BCM_IF_INVALID;
                        }
                        rep_id++;
                    }
                    break;

                case DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL:

                    /*
                     * free SW data base if this is not the first entry of the group, otherwise clear the entry
                     */
                    if (entry_val.prev_entry != DNX_MULTICAST_END_OF_LIST_PTR)
                    {
                        /** first entry would be removed in destroy */
                        SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, cur_entry_id));
                    }
                    else
                    {
                        /** the first entry is not removed, so it need to be cleared */
                        SHR_IF_ERR_EXIT(dnx_multicast_entry_clear(unit, cur_entry_id, FALSE, FALSE));
                    }
                    break;

                default:
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid action %u used for this function", action);
            }
            /*
             * If the current entry is replaced, the next entry pointer should not be moved to next member
             */
            if (cur_entry_replaced == FALSE)
            {
                cur_entry_id = entry_val.next_entry_id;
            }
        }
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            break;
        }
    }
    *rep_count_out = rep_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - creates link list with the rep_array and connects it to existing link list and remove the old link list if neccessary.
 *                                      first_group_entry<>new_link_list<>old_link_list
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group id
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] is_set - whether called from bcm_multicast_set/add
 *   \param [in] rep_count - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the replications array
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 * error _SHR_E_FULL returned if there isn't enough space to create the new link list.
 * This functions needs to know if previous link list is empty.
 * In a case which called from bcm_multicast_set API (is_set == 1) previous group considered as empty and removed at the end of the sequence.
 * In a case which called from bcm_multicast_add API (is_set == 0), HW link list need to be checked to determine whether the link list is empty.
 * When the link list is empty, first entry of the group is used to store replications.
 * For egress replication, the rep_array divided between cores. for each core different link list created.
 * For the 2 egress destination with the same cud, one replication entry is used.
 * This optimization is taking place only when the replications with the same CUD are consecutive
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_linklist_create(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    uint32 is_set,
    int rep_count,
    bcm_multicast_replication_t * rep_array)
{
    uint32 core_id, is_belong_to_core, rep_id;
    uint32 cur_entry_id, allocated_entry, nof_reps_at_entry;
    int nof_free_elements;
    uint32 core_rep_count[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 link_list_is_empty[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 group_first_entry_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 next_entry_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 old_link_list[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };/** the old link list to be removed at the end */
    dnx_multicast_mcdb_dbal_field_t entry_val, sec_rep_entry_val;
    int rep_count_out;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * check whether the group is empty and save the old link list if not 
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group, FALSE, DNX_MULTICAST_IS_INGRESS(flags),
                                                  core_id, &group_first_entry_id[core_id]));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, group_first_entry_id[core_id], &entry_val));
        link_list_is_empty[core_id] = is_set ? TRUE : DNX_MULTICAST_IS_EMPTY_GROUP(unit, entry_val);

        /*
         * for set API the existing link list need to be saved to be freed at the end
         */
        if (is_set)
        {
            old_link_list[core_id] = entry_val.next_entry_id;
            next_entry_id[core_id] = DNX_MULTICAST_END_OF_LIST_PTR;
        }
        else
        {
            old_link_list[core_id] = DNX_MULTICAST_END_OF_LIST_PTR;
            next_entry_id[core_id] = entry_val.next_entry_id;
        }
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            core_rep_count[0] = rep_count;
            core_rep_count[1] = 0;
            break;
        }
        SHR_IF_ERR_EXIT(dnx_multicast_nof_egress_reps_per_core_get
                        (unit, core_id, rep_count, rep_array, &core_rep_count[core_id]));
        /*
         * enable group replication per core
         */
        if (is_set || core_rep_count[core_id])
        {
            SHR_IF_ERR_EXIT(dnx_multicast_egr_core_enable_set(unit, group, core_id, core_rep_count[core_id] ? 1 : 0));
        }
    }

    /*
     * check whether there is enough entries to make the new link list 
     */
    SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_elements));
    if (nof_free_elements < core_rep_count[0] + core_rep_count[1])
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "there are not enough resources. available (%u), needed (%u)", nof_free_elements,
                     core_rep_count[0] + core_rep_count[1]);
    }

    /*
     * iterate over cores for replication.
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        /*
         * loop over the replications
         */
        for (rep_id = 0; rep_id < rep_count; rep_id++)
        {
            nof_reps_at_entry = 1;

            SHR_IF_ERR_EXIT(dnx_multicast_convert_gport_to_hw(unit, rep_array[rep_id].port, rep_array[rep_id].encap1,
                                                              flags, core_id, &entry_val, &is_belong_to_core,
                                                              DNX_MULTICAST_TRUNK_ACTION_CREATE));
            if (is_belong_to_core == FALSE)
            {
                continue;
            }
            entry_val.next_entry_id = next_entry_id[core_id];

            /*
             * check for double format replication whether current destination and the next destination having the same
             * CUD and format in order to put them at the same entry
             */
            if ((entry_val.format == DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT) && (rep_id < rep_count - 1)
                && (rep_array[rep_id].encap1 == rep_array[rep_id + 1].encap1))
            {
                SHR_IF_ERR_EXIT(dnx_multicast_convert_gport_to_hw(unit, rep_array[rep_id + 1].port,
                                                                  rep_array[rep_id].encap1, flags, core_id,
                                                                  &sec_rep_entry_val, &is_belong_to_core,
                                                                  DNX_MULTICAST_TRUNK_ACTION_CREATE));
                if ((sec_rep_entry_val.format == entry_val.format) && (is_belong_to_core))
                {
                    entry_val.dest[1] = sec_rep_entry_val.dest[0];
                    nof_reps_at_entry = 2;
                }
            }

            /*
             * if the link list was empty, the last replication needs to be written to the group first empty entry 
             */
            if (link_list_is_empty[core_id] && nof_reps_at_entry == core_rep_count[core_id])
            {
                cur_entry_id = group_first_entry_id[core_id];
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_multicast_entry_allocate(unit, flags, core_id, FALSE, &allocated_entry));
                cur_entry_id = allocated_entry;
            }

            /*
             * write entry to HW 
             */
            SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, cur_entry_id, &entry_val));

            /*
             * next entry prev_pointer set to cur entry 
             */
            if (next_entry_id[core_id] != DNX_MULTICAST_END_OF_LIST_PTR)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, next_entry_id[core_id], cur_entry_id));
            }

            if (nof_reps_at_entry == 2)
            {
                rep_id++;
            }
            core_rep_count[core_id] -= nof_reps_at_entry;

            /*
             * next entry added will use next_entry_id to set the next pointer 
             */
            next_entry_id[core_id] = cur_entry_id;
        }
    }

    /*
     * connect the first entry to the new entry if the link list was not empty
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        if (link_list_is_empty[core_id] == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_entry_next_ptr_set(unit, group_first_entry_id[core_id],
                                                             next_entry_id[core_id]));
            SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, next_entry_id[core_id],
                                                             group_first_entry_id[core_id]));
        }
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            break;
        }
    }

    /*
     * free the old link list(Free all entry IDs from SW and clear the firt entry from HW)
     */
    SHR_IF_ERR_EXIT(dnx_multicast_linklist_iter
                    (unit, group, flags, old_link_list, DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL, 0, NULL, &rep_count_out));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the content (replications) of a TDM multicast group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_tdm_set(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 core_id;
    bcm_core_t dest_core_id;
    int rep_idx;
    uint32 cur_entry_id;
    int ext_mode;
    bcm_port_t dest;
    uint32 dest_idx;
    uint32 cud;
    uint32 egr_invalid_destination;
    int nof_supported_dests;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));
    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));
    entry_val.dest[0] = entry_val.dest[1] = entry_val.dest[2] = entry_val.dest[3] = egr_invalid_destination;
    nof_supported_dests = ext_mode ? 3 : 4;

    /*
     * iterate over cores for replication.
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group, FALSE, FALSE, core_id, &cur_entry_id));
        cud = rep_array[0].encap1;
        dest_idx = 0;
        for (rep_idx = 0; rep_idx < nof_replications; rep_idx++)
        {
            /*
             * Check if gport is actually local port that is not encoded as GPORT
             */
            if (BCM_GPORT_IS_LOCAL(rep_array[dest_idx].port)
                || ((rep_array[dest_idx].port >= 0) && (rep_array[dest_idx].port < SOC_MAX_NUM_PORTS)))
            {
                /** Verify input */
                dest =
                    (BCM_GPORT_IS_LOCAL(rep_array[dest_idx].port)) ? BCM_GPORT_LOCAL_GET(rep_array[dest_idx].port) :
                    rep_array[dest_idx].port;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PORT, "Invalid Port");
            }

            /*
             * CUD validation
             */
            if (rep_idx)
            {
                if (rep_array[rep_idx].encap1 != cud)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "All CUDs per core (%d) need to be the same\n", core_id);
                }
            }
            else
            {
                rep_array[rep_idx].encap1 = cud;
            }

            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, dest, &dest_core_id));
            if (core_id == dest_core_id)
            {
                if (dest_idx >= nof_supported_dests)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Too many replications per core_id (%d)\n", core_id);
                }
                else
                {
                    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                                    (unit, ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST, dest,
                                     &entry_val.dest[rep_idx]));
                    dest_idx++;
                }
            }
        }
        /*
         * write entry to HW
         */
        entry_val.cud = (cud == BCM_IF_INVALID) ? DNX_MULTICAST_INVALID_CUD : cud;
        entry_val.format = (ext_mode ? DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT : DBAL_RESULT_TYPE_MCDB_TDM_FORMAT);
        SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, cur_entry_id, &entry_val));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the content (replications) of a TDM multicast group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] max_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *   \param [in] rep_count - pointer to number of replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e
dnx_multicast_tdm_get(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int max_replications,
    bcm_multicast_replication_t * rep_array,
    int *rep_count)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 core_id;
    int rep_idx;
    uint32 cur_entry_id;
    int ext_mode;
    bcm_port_t dest;
    uint32 cud;
    uint32 egr_invalid_destination;
    uint32 format;
    int nof_supported_dests;

    SHR_FUNC_INIT_VARS(unit);

    *rep_count = 0;
    SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));
    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));
    nof_supported_dests = ext_mode ? 3 : 4;
    /*
     * iterate over cores for replication.
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group, FALSE, FALSE, core_id, &cur_entry_id));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, cur_entry_id, &entry_val));
        format = (ext_mode ? DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT : DBAL_RESULT_TYPE_MCDB_TDM_FORMAT);
        if (entry_val.format != format)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "incorrect MC format: %u", entry_val.format);
        }
        cud = (entry_val.cud == DNX_MULTICAST_INVALID_CUD) ? BCM_IF_INVALID : entry_val.cud;
        for (rep_idx = 0; rep_idx < nof_supported_dests; rep_idx++)
        {
            if (entry_val.dest[rep_idx] != egr_invalid_destination)
            {
                if (*rep_count >= max_replications)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Caller supplied too few replication buffers (%u) for output for this multicast group (%d)\n",
                                 max_replications, group);
                }
                SHR_IF_ERR_EXIT(dnx_multicast_gport_from_encoded_destination_field
                                (unit, core_id, TRUE /** is_egress */ ,
                                 entry_val.dest[rep_idx], &dest));

                rep_array[*rep_count].port = dest;
                rep_array[*rep_count].encap1 = cud;
                (*rep_count)++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - add the content (replications) to an existing replication of a TDM multicast group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group/bitmap id
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] nof_replications - specifies the rep_array size
 *   \param [in] rep_array - contains the group/bitmap replications
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   int
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_tdm_add(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    bcm_multicast_replication_t temp_rep_array[8];
    int rep_count;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_tdm_get(unit, group, flags, 8, temp_rep_array, &rep_count));
    if (rep_count + nof_replications > 8)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Too many replications for the group %u", group);
    }
    sal_memcpy(&temp_rep_array[rep_count], rep_array, sizeof(bcm_multicast_replication_t) * nof_replications);
    SHR_IF_ERR_EXIT(dnx_multicast_tdm_set(unit, group, flags, rep_count + nof_replications, temp_rep_array));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See multicast_imp.h
 */
shr_error_e
dnx_multicast_tdm_delete(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int nof_replications,
    bcm_multicast_replication_t * rep_array)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 core_id;
    bcm_core_t dest_core_id;
    int rep_idx;
    uint32 cur_entry_id;
    bcm_port_t dest;
    uint32 hw_dest;
    uint32 egr_invalid_destination;
    int i;
    int nof_supported_dests;
    int ext_mode;
    uint32 cud;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));
    SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));
    entry_val.dest[0] = entry_val.dest[1] = entry_val.dest[2] = entry_val.dest[3] = egr_invalid_destination;
    nof_supported_dests = ext_mode ? 3 : 4;

    /*
     * iterate over cores for replication.
     */
    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group, FALSE, FALSE, core_id, &cur_entry_id));
        if (flags & BCM_MULTICAST_REMOVE_ALL)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_entry_clear(unit, cur_entry_id, TRUE, FALSE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, cur_entry_id, &entry_val));
            for (rep_idx = 0; rep_idx < nof_replications; rep_idx++)
            {
                /*
                 * Check if gport is actually local port that is not encoded as GPORT
                 */
                if (BCM_GPORT_IS_SET(rep_array[rep_idx].port))
                {
                    dest = BCM_GPORT_LOCAL_GET(rep_array[rep_idx].port);
                }
                else
                {
                    dest = rep_array[rep_idx].port;
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, dest, &dest_core_id));
                if (core_id == dest_core_id)
                {
                    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                                    (unit, ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST, dest, &hw_dest));
                    cud =
                        (rep_array[rep_idx].encap1 ==
                         BCM_IF_INVALID) ? DNX_MULTICAST_INVALID_CUD : rep_array[rep_idx].encap1;
                    if (cud == entry_val.cud)
                    {
                        for (i = 0; i < nof_supported_dests; i++)
                        {
                            if (entry_val.dest[i] == hw_dest)
                            {
                                entry_val.dest[i] = egr_invalid_destination;
                            }
                        }
                    }
                }
            }
            /*
             * write entry to HW
             */
            SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, cur_entry_id, &entry_val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See multicast.h
 */
shr_error_e
dnx_multicast_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint32 entry_handle_id;
    uint32 bier_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Init the multicast sw state db.
     */
    SHR_IF_ERR_EXIT(multicast_db.init(unit));

    bier_size = dnx_bier_mcdb_size_get(unit);
    /*
     * check whether multicast group numbers are not exceeding the limitation
     */
    if (dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
        dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit) +
        dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1 +
                                                       dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit)) >
        dnx_data_multicast.params.nof_mcdb_entries_get(unit) - bier_size - 1)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "nof_ingress (%u) + nof_ing_bmp (%u) + (nof_egress (%u) + nof_egr_bmp (%u)) * nof_cores (%u) > nof_entries (%u) - bier entries (%u)",
                     dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1,
                     dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit),
                     dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1,
                     dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit),
                     dnx_data_device.general.nof_cores_get(unit),
                     dnx_data_multicast.params.nof_mcdb_entries_get(unit) - 1, bier_size);
    }

    /*
     * creating resource manager data base. Last mcdb entry should be reserved for discard replication.
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    data.nof_elements = dnx_data_multicast.params.nof_mcdb_entries_get(unit) - 1;
    data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_MULTICAST_MCDB;
    sal_strncpy(data.name, DNX_MULTICAST_ALGO_RES_MCDB_STR, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(multicast_db.mcdb.create(unit, &data, NULL));

    /*
     * creating resource manager data base. Last mcdb entry should be reserved for discard replication.
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.flags = 0;
    data.nof_elements = dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1;
    sal_strncpy(data.name, DNX_MULTICAST_ALGO_RES_IS_TDM_STR, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.create(unit, &data, NULL));

    /*
     * static egress bitmap table mapping 
     */

    /*
     * groups offset configuration 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MULTICAST_OFFSETS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_CORE_0_OFFSET, INST_SINGLE, bier_size);
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_CORE_1_OFFSET, INST_SINGLE, bier_size);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_CORE_0_OFFSET, INST_SINGLE,
                                 dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 + bier_size);
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_CORE_1_OFFSET, INST_SINGLE,
                                     dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
                                     dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1 + bier_size);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dnx_multicast_pp_init(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Deinitialize dnx multicast module
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sw state resources will be deinit together.
     */
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
