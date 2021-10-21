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
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
#include <bcm/mcast.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_bier_db_access.h>

#include <bcm_int/dnx/bier/bier.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "multicast_imp.h"
#include "multicast_pp.h"
/*
 *  }
 */

/*
 *  Defines
 *  {
 */
/** Maximum reference number supported per unique replication in a MC group */
#define DNX_MULTICAST_MAX_NOF_REF_PER_REP 32

/** Destination types for hash key encoding */
#define DNX_MULTICAST_HASH_KEY_DEST_PORT 0x0
#define DNX_MULTICAST_HASH_KEY_DEST_TRUNK 0x1
#define DNX_MULTICAST_HASH_KEY_DEST_GROUP_ID 0x2
#define DNX_MULTICAST_HASH_KEY_DEST_BITMAP_ID 0x3
/*
 *  }
 */

/*
 *  Static functions
 *  {
 */
/**
 * \brief - Determine the hash table index and construct the hash key for
 * the hash tables mapping unique group and destination to a MCDB entry ID
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group_id - ID of the MC group
 *   \param [in] is_ingress - indicating if the group is Ingress or Egress
 *   \param [in] core_id - Core ID
 *   \param [in] dest_gport - Destination Gport
 *   \param [in] dest - Destination encoded for HW
 *   \param [in] cud - Destination CUD
 *   \param [in] ref_num - Reference number, in case the destination is repeating
 *   \param [out] hash_key - Constructed hash key
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
static shr_error_e
dnx_multicast_hash_key_construct(
    int unit,
    bcm_multicast_t group_id,
    uint8 is_ingress,
    uint32 core_id,
    bcm_gport_t dest_gport,
    uint32 dest,
    uint32 cud,
    int ref_num,
    mcdb_hash_table_key_t * hash_key)
{
    uint32 destination = 0;
    uint8 destination_type = 0;
    uint32 key_temp[3] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(hash_key, 0, sizeof(*hash_key));

    /** Determine the destination and type */
    if (BCM_MCAST_GPORT_IS_GROUP_ID(dest_gport))
    {
        destination = BCM_MCAST_GPORT_GROUP_ID_GET(dest_gport);
        destination_type = DNX_MULTICAST_HASH_KEY_DEST_GROUP_ID;
    }
    else if (BCM_MCAST_GPORT_IS_BITMAP_ID(dest_gport))
    {
        destination = BCM_MCAST_GPORT_BITMAP_ID_GET(dest_gport);
        destination_type = DNX_MULTICAST_HASH_KEY_DEST_BITMAP_ID;
    }
    else if (BCM_GPORT_IS_TRUNK(dest_gport))
    {
        destination = BCM_GPORT_TRUNK_GET(dest_gport);
        destination_type = DNX_MULTICAST_HASH_KEY_DEST_TRUNK;
    }
    else
    {
        destination = dest;
        destination_type = DNX_MULTICAST_HASH_KEY_DEST_PORT;
    }

    /** Construct the key */
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (key_temp, DNX_MULTICAST_HASH_KEY_DESTINATION_START_BIT,
                     DNX_MULTICAST_HASH_KEY_DESTINATION_NOF_BITS, destination));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (key_temp, DNX_MULTICAST_HASH_KEY_REFERENCE_NUM_START_BIT,
                     DNX_MULTICAST_HASH_KEY_REFERENCE_NUM_NOF_BITS, (uint32) ref_num));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (key_temp, DNX_MULTICAST_HASH_KEY_DESTINATION_TYPE_START_BIT,
                     DNX_MULTICAST_HASH_KEY_DESTINATION_TYPE_NOF_BITS, destination_type));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (key_temp, DNX_MULTICAST_HASH_KEY_ENCAPSULATION_ID_START_BIT,
                     DNX_MULTICAST_HASH_KEY_ENCAPSULATION_ID_NOF_BITS, cud));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (key_temp, DNX_MULTICAST_HASH_KEY_GROUP_ID_START_BIT, DNX_MULTICAST_HASH_KEY_GROUP_ID_NOF_BITS,
                     (uint32) group_id));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (key_temp, DNX_MULTICAST_HASH_KEY_IS_INGRESS_START_BIT, DNX_MULTICAST_HASH_KEY_IS_INGRESS_NOF_BITS,
                     is_ingress));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                    (key_temp, DNX_MULTICAST_HASH_KEY_CORE_ID_START_BIT, DNX_MULTICAST_HASH_KEY_CORE_ID_NOF_BITS,
                     core_id));

    /** Convert the constructed key to the appropriate format for the hash table */
    SHR_IF_ERR_EXIT(utilex_U32_to_U8
                    (key_temp, dnx_data_multicast.mcdb_hash_table.max_key_size_in_words_get(unit), hash_key->key));

exit:
    SHR_FUNC_EXIT;
}

 /**
  * \brief - Construct the hash key for last used or first free reference of the specified destination
  * in the hash tables mapping unique group and destination to a MCDB entry ID
  *
  * \par DIRECT_INPUT:
  *   \param [in] unit -  Unit-ID
  *   \param [in] group_id - ID of the MC group
  *   \param [in] is_ingress - indicating if the group is Ingress or Egress
  *   \param [in] core_id - Core ID
  *   \param [in] dest_gport - Destination Gport
  *   \param [in] dest - Destination encoded for HW
  *   \param [in] cud - Destination CUD
  *   \param [out] last_reference - the last reference to the rplication that is currently used
  *
  * \par INDIRECT INPUT:
  *   * None
  * \par DIRECT OUTPUT:
  *   shr_error_e
  * \par INDIRECT OUTPUT
  *   * None
  * \remark
  *   * last_reference = -1 indicates no used references
  * \see
  *   * None
  */
static shr_error_e
dnx_multicast_hash_last_reference_get(
    int unit,
    bcm_multicast_t group_id,
    uint8 is_ingress,
    uint32 core_id,
    bcm_gport_t dest_gport,
    uint32 dest,
    uint32 cud,
    int *last_reference)
{
    uint8 found = 0;
    uint32 ref_num = 0;
    uint32 entry_id_temp = 0;
    mcdb_hash_table_key_t hash_key_temp;
    SHR_FUNC_INIT_VARS(unit);

    /** Initializing variables */
    *last_reference = 0;
    sal_memset(&hash_key_temp, 0, sizeof(hash_key_temp));

    /** Go over the references to find the first free one */
    for (ref_num = 0; ref_num < DNX_MULTICAST_MAX_NOF_REF_PER_REP; ref_num++)
    {
        found = 0;
        SHR_IF_ERR_EXIT(dnx_multicast_hash_key_construct
                        (unit, group_id, is_ingress, core_id, dest_gport, dest, cud, ref_num, &hash_key_temp));
        SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.find(unit, &hash_key_temp, &entry_id_temp, &found));
        if (!found)
        {
            /** Entry for the current reference is not found.
             *  This means the previous reference was the last one used. */
            *last_reference = ref_num - 1;
            break;
        }

        if (ref_num == (DNX_MULTICAST_MAX_NOF_REF_PER_REP - 1))
        {
            /** We are at the last possible reference and it was used */
            *last_reference = ref_num;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Determine the hash table index and construct the hash key for
 * the reference of the specified destination that is matching a specific MCDB entry ID
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group_id - ID of the MC group
 *   \param [in] is_ingress - indicating if the group is Ingress or Egress
 *   \param [in] core_id - Core ID
 *   \param [in] dest_gport - Destination Gport
 *   \param [in] dest - Destination encoded for HW
 *   \param [in] cud - Destination CUD
 *   \param [in] entry_id - MCDB entry ID to match
 *   \param [out] hash_key - Constructed hash key
 *   \param [out] found - indicates if the MCDB entry was found
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
static shr_error_e
dnx_multicast_hash_mcdb_hash_key_get(
    int unit,
    bcm_multicast_t group_id,
    uint8 is_ingress,
    uint32 core_id,
    bcm_gport_t dest_gport,
    uint32 dest,
    uint32 cud,
    uint32 entry_id,
    mcdb_hash_table_key_t * hash_key,
    uint8 *found)
{
    uint32 ref_num = 0;
    uint8 found_temp = 0;
    uint32 entry_id_temp = 0;
    mcdb_hash_table_key_t hash_key_temp;
    SHR_FUNC_INIT_VARS(unit);

    *found = FALSE;
    sal_memset(hash_key, 0, sizeof(*hash_key));
    /** Go over the references to find the reference that is containing the provided MCDB entry ID */
    for (ref_num = 0; ref_num < DNX_MULTICAST_MAX_NOF_REF_PER_REP; ref_num++)
    {
        found_temp = 0;
        SHR_IF_ERR_EXIT(dnx_multicast_hash_key_construct
                        (unit, group_id, is_ingress, core_id, dest_gport, dest, cud, ref_num, &hash_key_temp));
        SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.find(unit, &hash_key_temp, &entry_id_temp, &found_temp));
        if ((found_temp) && (entry_id_temp == entry_id))
        {
            /** Entry is found and the MCDB entry ID is matching */
            *found = TRUE;
            break;
        }

        if (ref_num == (DNX_MULTICAST_MAX_NOF_REF_PER_REP - 1))
        {
            /** We are at the last possible reference. If we reach this point, it means that
             *  it is the last iteration and the requested MCDB entry was not found in the hash table */
            *found = FALSE;
        }
    }

    /** Return the hash key of the reference that contains the provided MCDB entry ID */
    sal_memcpy(hash_key, &hash_key_temp, sizeof(*hash_key));

exit:
    SHR_FUNC_EXIT;
}

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

    /** Loop over the link list */
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

                /** if the entry is empty it can be removed from the link list  */
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
                                    (unit, bitmap_ptr, entry_val.prev_entry, entry_val.next_entry_id, TRUE,
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
                SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, bitmap_ptr, NULL));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid action %u used for this function", action);
        }

        /** If the current entry is replaced, the next entry pointer should not be moved to next member */
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
    uint32 entry_id;
    bcm_core_t core_id;
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 *bmp[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { NULL };
    uint32 bmp_cud[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        bmp[core_id] = sal_alloc(sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit), "bmp");

        sal_memset(bmp[core_id], 0, sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit));
    }

    if (action == DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_reps_to_bmp(unit, nof_replications, rep_array, bmp, bmp_cud));
    }
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                        (unit, group, TRUE, DNX_MULTICAST_IS_INGRESS(flags), core_id, &entry_id));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, entry_id, &entry_val));

        /** verify that the CUD is correct */
        if (!DNX_MULTICAST_IS_EMPTY_BMP_GROUP(entry_val))
        {
            if ((action == DNX_MULTICAST_ENTRY_ITER_REMOVE_LIST) && (bmp_cud[core_id] != entry_val.cud)
                && (bmp_cud[core_id] != DNX_MULTICAST_INVALID_CUD) && (entry_val.cud != DNX_MULTICAST_INVALID_CUD))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "wrong cud. cud to be deleted (%u), bitmap cud (%u)", bmp_cud[core_id],
                             entry_val.cud);
            }
        }

        if (entry_val.next_entry_id != DNX_MULTICAST_END_OF_LIST_PTR)
        {
            /** iterate and delete relevant entries */
            SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter(unit, entry_val.bmp_ptr, entry_val.cud, action, bmp[core_id]));

            /** if all entries were deleted need to re-allocate an empty bitmap and attach it to bitmap ptr */
            SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, entry_id, &entry_val));
            if (entry_val.next_entry_id == DNX_MULTICAST_END_OF_LIST_PTR)
            {
                int ext_mode;
                uint32 allocated_entry;
                dnx_multicast_mcdb_dbal_field_t empty_bitmap_entry = { 0 };

                /** add an empty bitmap */
                SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));
                SHR_IF_ERR_EXIT(dnx_multicast_entry_allocate(unit, flags, core_id, FALSE, &allocated_entry));
                empty_bitmap_entry.format =
                    (ext_mode ? DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT : DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT);
                empty_bitmap_entry.next_entry_id = DNX_MULTICAST_END_OF_LIST_PTR;
                empty_bitmap_entry.bmp_val = 0;
                empty_bitmap_entry.bmp_offset = 0;
                empty_bitmap_entry.prev_entry = entry_id;

                /** write empty bitmap entry to HW */
                SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, allocated_entry, &empty_bitmap_entry));

                /** update bitmap ptr entry with empty bitmap */
                entry_val.bmp_ptr = allocated_entry;
                entry_val.next_entry_id = allocated_entry;
                SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, entry_id, &entry_val));
            }
        }
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            break;
        }
    }

exit:

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
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

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
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
    uint32 nof_new_entries_needed = 0, bmp_entry_id, cur_entry_id;
    bcm_core_t core_id;
    uint32 *bmp_new_reps[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { NULL };
    uint32 *bmp_existing_reps[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { NULL };
    uint32 bitmap_first_entry_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 bmp_cud[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    dnx_multicast_mcdb_dbal_field_t entry_val;
    int nof_free_elements;
    int ext_mode;
    int add_empty_bitmap = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        bmp_new_reps[core_id] = sal_alloc(sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit), "bmp");
        sal_memset(bmp_new_reps[core_id], 0, sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit));

        bmp_existing_reps[core_id] = sal_alloc(sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit), "bmp");
        sal_memset(bmp_existing_reps[core_id], 0, sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit));
    }

    /** Convert destination to bitmap */
    SHR_IF_ERR_EXIT(dnx_multicast_bitmap_reps_to_bmp(unit, rep_count, rep_array, bmp_new_reps, bmp_cud));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        /** Get the existing bitmap */
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                        (unit, group, TRUE, DNX_MULTICAST_IS_INGRESS(flags), core_id, &bitmap_first_entry_id[core_id]));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, bitmap_first_entry_id[core_id], &entry_val));
        next_entry_id[core_id] = entry_val.next_entry_id;
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter
                        (unit, entry_val.bmp_ptr, entry_val.cud, DNX_MULTICAST_ENTRY_ITER_GET,
                         bmp_existing_reps[core_id]));

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

        /** check whether the new replication CUD and the old one is the same. relevant for add API. */
        if (!DNX_MULTICAST_IS_EMPTY_BMP_GROUP(entry_val))
        {
            if ((is_set == FALSE) && (bmp_cud[core_id] != entry_val.cud)
                && (bmp_cud[core_id] != DNX_MULTICAST_INVALID_CUD) && (entry_val.cud != DNX_MULTICAST_INVALID_CUD))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "core replications with different cuds can't be used "
                             "for bitmap group. cud1 (%u), cud2 (%u)", bmp_cud[core_id], entry_val.cud);
            }
        }
    }

    /** check whether the entries can be allocated */
    SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_elements));
    if (nof_free_elements < nof_new_entries_needed)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "there are not enough resources. available (%u), needed (%u)",
                     nof_free_elements, nof_new_entries_needed);
    }

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        int core_rep_count = 0;
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, bitmap_first_entry_id[core_id], &entry_val));

        /** remove the entries user wish to remove. relevant for set API only */
        if (is_set)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter(unit, entry_val.bmp_ptr, entry_val.cud,
                                                      DNX_MULTICAST_ENTRY_ITER_BMP_REMOVE_NOT_IN_LIST,
                                                      bmp_new_reps[core_id]));
        }

        /** update next entry id, as it could have been overrun when the iterator removed entries from the list */
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, bitmap_first_entry_id[core_id], &entry_val));
        next_entry_id[core_id] = entry_val.next_entry_id;

        /** when setting a core to 0, need to add an empty bitmap, this is because a bitmap PTR has to point to an existing bitmap */
        SHR_BITCOUNT_RANGE(bmp_new_reps[core_id], core_rep_count, 0,
                           (sizeof(uint32) * DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit)));
        if ((is_set) && (core_rep_count == 0))
        {
            add_empty_bitmap = TRUE;
        }

        /** update the existing HW bitmap with the original bitmap  */
        /** This iter function also implicitly removes entries in existing HW bitmap from the new reps bitmap */
        SHR_IF_ERR_EXIT(dnx_multicast_bitmap_iter
                        (unit, entry_val.bmp_ptr, entry_val.cud, DNX_MULTICAST_ENTRY_ITER_BMP_ADD,
                         bmp_new_reps[core_id]));

        /** loop over the new entries needed, allocate and add them to existing link list  */
        for (bmp_entry_id = 0; bmp_entry_id < DNX_MULTICAST_BMP_NOF_ENTRIES_PER_DEV(unit); bmp_entry_id++)
        {
            if (bmp_new_reps[core_id][bmp_entry_id] || add_empty_bitmap)
            {
                add_empty_bitmap = FALSE;
                SHR_IF_ERR_EXIT(dnx_multicast_entry_allocate(unit, flags, core_id, FALSE, &allocated_entry));
                cur_entry_id = allocated_entry;
                entry_val.next_entry_id = next_entry_id[core_id];
                entry_val.bmp_val = bmp_new_reps[core_id][bmp_entry_id];
                entry_val.bmp_offset = bmp_entry_id;
                entry_val.format =
                    (ext_mode ? DBAL_RESULT_TYPE_MCDB_BITMAP_EXT_FORMAT : DBAL_RESULT_TYPE_MCDB_BITMAP_FORMAT);

                /** write entry to HW  */
                SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, cur_entry_id, &entry_val));

                /** next entry prev_pointer set to cur entry  */
                if (next_entry_id[core_id] != DNX_MULTICAST_END_OF_LIST_PTR)
                {
                    SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, next_entry_id[core_id], cur_entry_id));
                }

                /** next entry added will use next_entry_id to set the next pointer  */
                next_entry_id[core_id] = cur_entry_id;
            }
        }

        /** the first entry (bitmap pointer) is written to HW */
        entry_val.format = DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT;
        entry_val.next_entry_id = next_entry_id[core_id];
        entry_val.bmp_ptr = next_entry_id[core_id];
        entry_val.cud = (bmp_cud[core_id] == DNX_MULTICAST_INVALID_CUD) ? entry_val.cud : bmp_cud[core_id];
        SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, bitmap_first_entry_id[core_id], &entry_val));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, next_entry_id[core_id], bitmap_first_entry_id[core_id]));
    }
exit:

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_FREE(bmp_new_reps[core_id]);
        SHR_FREE(bmp_existing_reps[core_id]);
    }

    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_multicast_hash_table_entry_relocate(
    int unit,
    uint32 old_entry_id,
    uint32 new_entry_id)
{
    uint32 temp_entry_id = 0;
    uint32 prev_entry_id = 0;
    uint32 dest_core_id = 0;
    dnx_multicast_mcdb_dbal_field_t entry_val;
    dnx_multicast_group_region_t region;
    bcm_gport_t dest1 = BCM_GPORT_INVALID, dest2 = BCM_GPORT_INVALID;
    bcm_if_t cud;
    mcdb_hash_table_key_t hash_key;
    uint8 found = 0, success = 0;
    bcm_multicast_t group_id = 0;
    uint32 is_open = 0;
    uint8 is_ingress = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the new entry, assume it is already set in HW */
    SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, new_entry_id, &entry_val));

    if (entry_val.prev_entry == DNX_MULTICAST_END_OF_LIST_PTR)
    {
        /** Current entry is the first entry of the group */
        temp_entry_id = new_entry_id;
    }
    else
    {
        /** Loop over all entries of the link list to find the first entry of the group */
        prev_entry_id = entry_val.prev_entry;
        while (prev_entry_id != DNX_MULTICAST_END_OF_LIST_PTR)
        {
            /** Get the current entry */
            temp_entry_id = prev_entry_id;
            SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_get(unit, temp_entry_id, &prev_entry_id));
        }
    }

    /** Using the first Entry of the group, find the region and ID in order to determine
     * the correct core for resolving the destination */
    SHR_IF_ERR_EXIT(dnx_multicast_entry_to_id(unit, temp_entry_id, (uint32 *) &group_id, &region));
    switch (region)
    {
        case DNX_MULTICAST_GROUP_REGION_INGRESS:
            is_ingress = 1;
            dest_core_id = 0;
            break;
        case DNX_MULTICAST_GROUP_REGION_EG_CORE0:
            is_ingress = 0;
            dest_core_id = 0;
            break;
        case DNX_MULTICAST_GROUP_REGION_EG_CORE1:
            is_ingress = 0;
            dest_core_id = 1;
            break;
        default:
            break;
    }

    /** Convert the destinations. */
    dest2 = BCM_GPORT_INVALID;
    SHR_IF_ERR_EXIT(dnx_multicast_convert_hw_to_gport(unit, entry_val, dest_core_id, &dest1, &dest2, &cud));
    /** If CUD is invalid, convert it back to Multicast invalid value */
    cud = (cud == BCM_IF_INVALID) ? DNX_MULTICAST_INVALID_CUD : cud;

    /** Find the old entry in the hash table mapping unique destinations to MCDB Index */
    SHR_IF_ERR_EXIT(dnx_multicast_group_open_get
                    (unit, group_id, ((is_ingress == 0) ? BCM_MULTICAST_EGRESS_GROUP : BCM_MULTICAST_INGRESS_GROUP),
                     &is_open));
    if (is_open)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_hash_mcdb_hash_key_get
                        (unit, group_id, is_ingress, dest_core_id, dest1, entry_val.dest[0], cud, old_entry_id,
                         &hash_key, &found));
    }

    if (found == FALSE)
    {
        if (entry_val.format == DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT)
        {
            /** The entry is the first entry of a Bitmap group, no need to update hash table */
            SHR_EXIT();
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "MCDB entry %d not found in hash table! \n", old_entry_id);
        }
    }

    /** Remove the old entry from the hash table */
    SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.delete(unit, &hash_key));

    /** Re-map it with the mew MCDB Index */
    SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.insert(unit, &hash_key, &new_entry_id, &success));

    if (dest2 != BCM_GPORT_INVALID)
    {
        /** Remove also the second entry from the hash table */
        SHR_IF_ERR_EXIT(dnx_multicast_hash_mcdb_hash_key_get
                        (unit, group_id, is_ingress, dest_core_id, dest2, entry_val.dest[1], cud, old_entry_id,
                         &hash_key, &found));
        if (found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "MCDB entry %d not found in hash table! \n", old_entry_id);
        }
        SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.delete(unit, &hash_key));

        /** Re-map it with the mew MCDB Index */
        SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.insert(unit, &hash_key, &new_entry_id, &success));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Removes replications from a Linked list group
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] group - group id
 *   \param [in] flags - specifies whether bitmap id is ingress/egress
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the replications array
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
dnx_multicast_linklist_rep_remove(
    int unit,
    bcm_multicast_t group,
    uint32 flags,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array)
{
    uint32 core_id, rep_id = 0, cur_entry_id;
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 cur_entry_replaced;
    mcdb_hash_table_key_t hash_key;
    uint32 is_belong_to_core;
    dnx_multicast_mcdb_dbal_field_t rep_entry_val;
    uint8 found = 0;
#ifdef BCM_DNX2_SUPPORT
    int egress_trunk_id = 0;
    int profile_found = 0;
#endif /* BCM_DNX2_SUPPORT */
    uint32 egr_invalid_destination;
    int last_reference = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        /** Get the invalid destination value */
        if (DNX_MULTICAST_IS_EGRESS(flags))
        {
            SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));
        }
        for (rep_id = 0; rep_id < rep_count_in; rep_id++)
        {
            /** Reset variable for next iteration */
            cur_entry_replaced = FALSE;
#ifdef BCM_DNX2_SUPPORT
            /** For Egress groups with Trunk replication, check first if the trunk has an Egress profile. */
            if (DNX_MULTICAST_IS_EGRESS(flags) && BCM_GPORT_IS_TRUNK(rep_array[rep_id].port))
            {
                /** If the Trunk profile is not created - skip. */
                SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get
                                (unit, BCM_GPORT_TRUNK_GET(rep_array[rep_id].port), &egress_trunk_id, &profile_found));
                if (!profile_found)
                {
                    continue;
                }
            }
#endif /* BCM_DNX2_SUPPORT */
            /** Convert replication to HW */
            SHR_IF_ERR_EXIT(dnx_multicast_convert_gport_to_hw
                            (unit, rep_array[rep_id].port, rep_array[rep_id].encap1, flags, core_id,
                             &rep_entry_val, &is_belong_to_core, DNX_MULTICAST_TRUNK_ACTION_DESTROY));

            if (is_belong_to_core == FALSE)
            {
                continue;
            }
#ifdef BCM_DNX2_SUPPORT
            /** For Egress groups with Trunk replication, the Egress Trunk profile also must be handled. */
            if (DNX_MULTICAST_IS_EGRESS(flags) && BCM_GPORT_IS_TRUNK(rep_array[rep_id].port))
            {
                /** Clear Egress Trunk profile */
                SHR_IF_ERR_EXIT(dnx_trunk_egress_destroy(unit, BCM_GPORT_TRUNK_GET(rep_array[rep_id].port)));
            }
#endif /* BCM_DNX2_SUPPORT */
            /** Get the entry from the hash table mapping unique destinations to MCDB Index */
            SHR_IF_ERR_EXIT(dnx_multicast_hash_last_reference_get
                            (unit, group, DNX_MULTICAST_IS_INGRESS_GROUP(flags), core_id, rep_array[rep_id].port,
                             rep_entry_val.dest[0], rep_entry_val.cud, &last_reference));
            /** Return the hash key and table index of the last used reference */
            SHR_IF_ERR_EXIT(dnx_multicast_hash_key_construct
                            (unit, group, DNX_MULTICAST_IS_INGRESS_GROUP(flags), core_id, rep_array[rep_id].port,
                             rep_entry_val.dest[0], rep_entry_val.cud, last_reference, &hash_key));
            SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.find(unit, &hash_key, &cur_entry_id, &found));

            /** Entry for this replication does not exist */
            if (found == FALSE)
            {
                continue;
            }

            /** Get the current entry from HW */
            SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, cur_entry_id, &entry_val));

            /** If the format id double, but one of the destinations is invalid, or if the format is not double - remove the entry */
            if (((entry_val.format == DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT)
                 && ((egr_invalid_destination == entry_val.dest[0])
                     || (egr_invalid_destination == entry_val.dest[1])))
                || (entry_val.format != DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT))
            {
                /** Remove the entry, or if it is the first entry - replace it */
                SHR_IF_ERR_EXIT(dnx_multicast_entry_remove
                                (unit, cur_entry_id, entry_val.prev_entry, entry_val.next_entry_id, FALSE,
                                 &cur_entry_replaced));
            }
            else
            {
                /** If the format is double and both destinations are valid, remove only the one that is relevant */
                if (rep_entry_val.dest[0] == entry_val.dest[0])
                {
                    entry_val.dest[0] = egr_invalid_destination;
                }
                else
                {
                    entry_val.dest[1] = egr_invalid_destination;
                }
                SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, cur_entry_id, &entry_val));
            }

            /** Remove the entry from the hash table mapping unique destinations to MCDB Index */
            SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.delete(unit, &hash_key));

            /** If the current entry is replaced, the new values need to be updated in the hash table */
            if (cur_entry_replaced == TRUE)
            {
                /** Re-allocate the current entry in the hash table */
                SHR_IF_ERR_EXIT(dnx_multicast_hash_table_entry_relocate(unit, entry_val.next_entry_id, cur_entry_id));
            }
        }

        /** Ingress is valid only for the first core */
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
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
    bcm_gport_t dest1 = BCM_GPORT_INVALID, dest2 = BCM_GPORT_INVALID;
    bcm_if_t cud;
    uint8 count_only;
    mcdb_hash_table_key_t hash_key;
    uint8 found = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
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

        /** Loop over all entries of the link list */
        while (cur_entry_id != DNX_MULTICAST_END_OF_LIST_PTR)
        {
            /** Get the current entry */
            SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, cur_entry_id, &entry_val));
            switch (action)
            {
                case DNX_MULTICAST_ENTRY_ITER_GET:
                    /** When rep_count_in is 0, only count entries without getting them */
                    count_only = !rep_count_in;
                    SHR_IF_ERR_EXIT(dnx_multicast_convert_hw_to_gport(unit, entry_val, core_id, &dest1, &dest2, &cud));
                    if (core_id && entry_val.format == DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT)
                    {
                        /** Linked list replications are always created on both cores.
                         * No need to return the same replication twice , continue to the next entry */
                        break;
                    }

                    if (core_id && entry_val.format == DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT)
                    {
                        /** Bitmap pointer replications are always created on both cores.
                         * No need to return the same replication twice, continue to the next entry */
                        break;
                    }

                    if (core_id && BCM_GPORT_IS_TRUNK(dest1))
                    {
                        /** Trunk replications are always created on both cores.
                         * No need to return the same replication twice , continue to the next entry */
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
                    /** Convert the destinations. */
                    dest2 = BCM_GPORT_INVALID;
                    SHR_IF_ERR_EXIT(dnx_multicast_convert_hw_to_gport(unit, entry_val, core_id, &dest1, &dest2, &cud));
                    /** If CUD is invalud, convert it back to Multicast invalid value */
                    cud = (cud == BCM_IF_INVALID) ? DNX_MULTICAST_INVALID_CUD : cud;
                    /** Remove the entry from the hash table mapping unique destinations to MCDB Index */
                    SHR_IF_ERR_EXIT(dnx_multicast_hash_mcdb_hash_key_get
                                    (unit, group, DNX_MULTICAST_IS_INGRESS_GROUP(flags), core_id, dest1,
                                     entry_val.dest[0], cud, cur_entry_id, &hash_key, &found));
                    SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.delete(unit, &hash_key));
                    if (dest2 != BCM_GPORT_INVALID)
                    {
                        /** Remove also the second entry from the hash table mapping unique destinations to MCDB Index */
                        SHR_IF_ERR_EXIT(dnx_multicast_hash_mcdb_hash_key_get
                                        (unit, group, DNX_MULTICAST_IS_INGRESS_GROUP(flags), core_id, dest1,
                                         entry_val.dest[1], cud, cur_entry_id, &hash_key, &found));
                        SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.delete(unit, &hash_key));
                    }
#ifdef BCM_DNX2_SUPPORT
                    /** For Egress MC groups, when removing a replication which is TRUNK the Egress profile must be removed*/
                    if (DNX_MULTICAST_IS_EGRESS_GROUP(flags))
                    {
                        if (BCM_GPORT_IS_TRUNK(dest1))
                        {
                            /** Clear Egress Trunk profile */
                            SHR_IF_ERR_EXIT(dnx_trunk_egress_destroy(unit, BCM_GPORT_TRUNK_GET(dest1)));
                        }
                        if (BCM_GPORT_IS_TRUNK(dest2))
                        {
                            /** Clear Egress Trunk profile */
                            SHR_IF_ERR_EXIT(dnx_trunk_egress_destroy(unit, BCM_GPORT_TRUNK_GET(dest2)));
                        }
                    }
#endif /* BCM_DNX2_SUPPORT */
                    /** Free the entry in SW State data base, unless it is the first entry in the group.
                      * The first entry will be freed in the bcm_multicast_destroy API when the group is destroyed. */
                    if (entry_val.prev_entry != DNX_MULTICAST_END_OF_LIST_PTR)
                    {
                        /** Free entry in SW State data base */
                        SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, cur_entry_id, NULL));
                    }
                    else
                    {
                        /** Clear the first entry from HW. No need to clean other entries,
                         * nothing is pointing to them */
                        SHR_IF_ERR_EXIT(dnx_multicast_entry_clear(unit, cur_entry_id, FALSE, FALSE));
                    }
                    break;

                default:
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid action %u used for this function", action);
            }

            /** Continue to the next entry */
            cur_entry_id = entry_val.next_entry_id;
        }

        /** Ingress is valid only for the first core */
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
    uint32 rep_id;
    bcm_core_t core_id;
    uint32 cur_entry_id, allocated_entry, nof_reps_at_entry;
    int nof_free_elements;
    uint32 total_rep_count_on_all_cores = 0;
    uint32 core_rep_count[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 link_list_is_empty[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 group_first_entry_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 next_entry_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 old_link_list[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };/** the old link list to be removed at the end */
    dnx_multicast_mcdb_dbal_field_t group_first_entry_val;
    dnx_multicast_mcdb_dbal_field_t *entry_values[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { NULL };
    uint32 *is_belong_to_core[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { NULL };
    int rep_count_out;
    mcdb_hash_table_key_t hash_key;
    uint8 success = 0;
    int last_reference = 0;
    int first_free_reference = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate the arrays for replications per core */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_ALLOC_SET_ZERO(is_belong_to_core[core_id], (sizeof(uint32) * rep_count),
                           "MC HW Encoded Entry values per replication array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC_SET_ZERO(entry_values[core_id], (sizeof(dnx_multicast_mcdb_dbal_field_t) * rep_count),
                           "MC Replication belongs to core indication per replication array", "%s%s%s\r\n", EMPTY,
                           EMPTY, EMPTY);
    }

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        /** Get the first entry of the group for the current core */
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry
                        (unit, group, FALSE, DNX_MULTICAST_IS_INGRESS(flags), core_id, &group_first_entry_id[core_id]));
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, group_first_entry_id[core_id], &group_first_entry_val));
        /** For 'set' context always mark the group as empty, because an entirely new Linked List will be created and the old one will be deleted.
         * For 'add' context check if the group is actually empty and mark accordingly. If it is not empty the new entries will be added to the existing Linked List */
        link_list_is_empty[core_id] = is_set ? TRUE : DNX_MULTICAST_IS_EMPTY_GROUP(unit, group_first_entry_val);

        /** For 'set' save the old linked List, so it can be freed in the end */
        if (is_set)
        {
            old_link_list[core_id] = group_first_entry_val.next_entry_id;
            next_entry_id[core_id] = DNX_MULTICAST_END_OF_LIST_PTR;
        }
        else
        {
            old_link_list[core_id] = DNX_MULTICAST_END_OF_LIST_PTR;
            next_entry_id[core_id] = group_first_entry_val.next_entry_id;
        }

        /** Fill the array of entry values based on the replications on the current core  and count the replications per core */
        for (rep_id = 0; rep_id < rep_count; rep_id++)
        {
            /** Convert each replication to HW encoding */
            SHR_IF_ERR_EXIT(dnx_multicast_convert_gport_to_hw(unit, rep_array[rep_id].port, rep_array[rep_id].encap1,
                                                              flags, core_id, &entry_values[core_id][rep_id],
                                                              &is_belong_to_core[core_id][rep_id],
                                                              DNX_MULTICAST_TRUNK_ACTION_CREATE));

            /** Initialize the previous and next pointer values to end of list */
            entry_values[core_id][rep_id].next_entry_id = DNX_MULTICAST_END_OF_LIST_PTR;
            entry_values[core_id][rep_id].prev_entry = DNX_MULTICAST_END_OF_LIST_PTR;

            /** Count the replications per core */
            if (is_belong_to_core[core_id][rep_id])
            {
                (core_rep_count[core_id])++;
            }
        }

        /** Count also the total replications count */
        total_rep_count_on_all_cores += core_rep_count[core_id];

        /** For Ingress there's no core differentiation, so all replications are marked as belonging to
         * core 0, no need to go over other cores */
        if (DNX_MULTICAST_IS_INGRESS(flags))
        {
            break;
        }

        /** For Egress need to enable group replication per core */
        if (is_set || core_rep_count[core_id])
        {
            SHR_IF_ERR_EXIT(dnx_multicast_egr_core_enable_set(unit, group, core_id, core_rep_count[core_id] ? 1 : 0));
        }
    }

    /** Check whether there is enough entries to make the new Linked List */
    SHR_IF_ERR_EXIT(multicast_db.mcdb.nof_free_elements_get(unit, &nof_free_elements));
    if (nof_free_elements < total_rep_count_on_all_cores)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "there are not enough resources. available (%u), needed (%u)", nof_free_elements,
                     core_rep_count[0] + core_rep_count[1]);
    }

    /** Free the old Linked List (Free all entry IDs from SW and clear the first entry from HW) */
    SHR_IF_ERR_EXIT(dnx_multicast_linklist_iter
                    (unit, group, flags, old_link_list, DNX_MULTICAST_ENTRY_ITER_REMOVE_ALL, 0, NULL, &rep_count_out));

    /** Iterate over cores to set the replications */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        /** Loop over the replications */
        for (rep_id = 0; rep_id < rep_count; rep_id++)
        {
            /** Check if the entry belongs to the current core */
            if (is_belong_to_core[core_id][rep_id] == FALSE)
            {
                continue;
            }

            /** Set the next entry pointer */
            entry_values[core_id][rep_id].next_entry_id = next_entry_id[core_id];

            /*
             * Check for double format replication whether current destination and the next destination have the same
             * CUD and format in order to put them at the same entry
             */
            nof_reps_at_entry = 1;
            if ((entry_values[core_id][rep_id].format == DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT)
                && (rep_id < rep_count - 1) && (rep_array[rep_id].encap1 == rep_array[rep_id + 1].encap1))
            {
                if ((entry_values[core_id][rep_id + 1].format == entry_values[core_id][rep_id].format)
                    && (is_belong_to_core[core_id][rep_id + 1]))
                {
                    entry_values[core_id][rep_id].dest[1] = entry_values[core_id][rep_id + 1].dest[0];
                    nof_reps_at_entry = 2;
                }
            }

            /** If the link list was empty, the last replication needs to be written to the group first empty entry */
            if (link_list_is_empty[core_id] && nof_reps_at_entry == core_rep_count[core_id])
            {
                cur_entry_id = group_first_entry_id[core_id];
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_multicast_entry_allocate(unit, flags, core_id, FALSE, &allocated_entry));
                cur_entry_id = allocated_entry;
            }

            /** Write the entry to HW */
            SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, cur_entry_id, &entry_values[core_id][rep_id]));

            /** If the next entry is not the end of the list, set the previous entry pointer of the
             * next entry to the current entry ID */
            if (next_entry_id[core_id] != DNX_MULTICAST_END_OF_LIST_PTR)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, next_entry_id[core_id], cur_entry_id));
            }

            /** Write the entry in the hash table mapping unique destinations to MCDB Index */
            SHR_IF_ERR_EXIT(dnx_multicast_hash_last_reference_get
                            (unit, group, DNX_MULTICAST_IS_INGRESS_GROUP(flags), core_id, rep_array[rep_id].port,
                             entry_values[core_id][rep_id].dest[0], entry_values[core_id][rep_id].cud,
                             &last_reference));

            /** Return the hash key and table index of the first free reference */
            first_free_reference = last_reference + 1;
            if (first_free_reference >= DNX_MULTICAST_MAX_NOF_REF_PER_REP)
            {

                SHR_ERR_EXIT(_SHR_E_CONFIG, "Number of references for unique member of MC group is above max (%d)!",
                             DNX_MULTICAST_MAX_NOF_REF_PER_REP);
            }
            SHR_IF_ERR_EXIT(dnx_multicast_hash_key_construct
                            (unit, group, DNX_MULTICAST_IS_INGRESS_GROUP(flags), core_id, rep_array[rep_id].port,
                             entry_values[core_id][rep_id].dest[0], entry_values[core_id][rep_id].cud,
                             first_free_reference, &hash_key));
            SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.insert(unit, &hash_key, &cur_entry_id, &success));
            if (!success)
            {
                SHR_ERR_EXIT(_SHR_E_FULL,
                             "Failed to create new entry in the destination to MCDB Index mapping hash table\n");
            }

            /** If two replications were written in the current entry also map the second one to the same entry */
            if (nof_reps_at_entry == 2)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_hash_last_reference_get
                                (unit, group, DNX_MULTICAST_IS_INGRESS_GROUP(flags), core_id,
                                 rep_array[rep_id + 1].port, entry_values[core_id][rep_id + 1].dest[0],
                                 entry_values[core_id][rep_id + 1].cud, &last_reference));
                /** Return the hash key and table index of the first free reference */
                first_free_reference = last_reference + 1;
                if (first_free_reference >= DNX_MULTICAST_MAX_NOF_REF_PER_REP)
                {

                    SHR_ERR_EXIT(_SHR_E_CONFIG, "Number of references for unique member of MC group is above max (%d)!",
                                 DNX_MULTICAST_MAX_NOF_REF_PER_REP);
                }
                SHR_IF_ERR_EXIT(dnx_multicast_hash_key_construct
                                (unit, group, DNX_MULTICAST_IS_INGRESS_GROUP(flags), core_id,
                                 rep_array[rep_id + 1].port, entry_values[core_id][rep_id + 1].dest[0],
                                 entry_values[core_id][rep_id + 1].cud, first_free_reference, &hash_key));
                SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.insert(unit, &hash_key, &cur_entry_id, &success));
                if (!success)
                {
                    SHR_ERR_EXIT(_SHR_E_FULL,
                                 "Failed to create new entry in the destination to MCDB Index mapping hash table\n");
                }
            }

            /** If two replications were written in the current entry increase the
             * replication ID in order to skip the replication that was already added */
            if (nof_reps_at_entry == 2)
            {
                rep_id++;
            }

            /** Lower the number of replications remaining to be added for the current core */
            core_rep_count[core_id] -= nof_reps_at_entry;

            /** Set the next entry ID to the current entry ID. This is done because the next entry
             * will point to the current entry */
            next_entry_id[core_id] = cur_entry_id;
        }
    }

    /** Connect the group's first first entry to the new entry if the Linked List was not empty */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
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

exit:
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
    {
        SHR_FREE(entry_values[core_id]);
        SHR_FREE(is_belong_to_core[core_id]);
    }
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
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
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
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
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
                                (unit, core_id, TRUE /** is_egress */ , entry_val.format,
                                 entry_val.dest[rep_idx], FALSE, &dest));

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
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
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
    sw_state_htbl_init_info_t hash_info;
    uint32 nof_ingr_groups = 0;
    uint32 nof_egr_groups = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Init the multicast sw state db.
     */
    SHR_IF_ERR_EXIT(multicast_db.init(unit));

    /*
     * Get BIER size
     */
    bier_size = dnx_bier_mcdb_size_get(unit);

    /*
     * Check whether multicast group numbers are not exceeding the limitation
     */
    nof_ingr_groups =
        (dnx_data_multicast.params.max_ing_mc_groups_get(unit) >=
         0) ? (dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1) : 0;
    nof_egr_groups =
        (dnx_data_multicast.params.max_egr_mc_groups_get(unit) >=
         0) ? (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1) : 0;
    if (nof_ingr_groups + dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit) +
        dnx_data_device.general.nof_cores_get(unit) * (nof_egr_groups +
                                                       dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit)) >
        dnx_data_multicast.params.nof_mcdb_entries_get(unit) - bier_size)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "nof_ingress (%u) + nof_ing_bmp (%u) + (nof_egress (%u) + nof_egr_bmp (%u)) * nof_cores (%u) > nof_entries (%u) - bier entries (%u)",
                     nof_ingr_groups,
                     dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit),
                     nof_egr_groups,
                     dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit),
                     dnx_data_device.general.nof_cores_get(unit),
                     dnx_data_multicast.params.nof_mcdb_entries_get(unit), bier_size);
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

    if (dnx_data_multicast.params.max_egr_mc_groups_get(unit) >= 0)
    {
        /*
         * creating resource manager data bases. Last mcdb entry should be reserved for discard replication.
         */
        sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
        data.first_element = 0;
        data.flags = 0;
        data.nof_elements = dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1;
        sal_strncpy(data.name, DNX_MULTICAST_ALGO_RES_IS_TDM_STR, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(multicast_db.group_is_tdm.create(unit, &data, NULL));

        sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
        data.first_element = 0;
        data.flags = 0;
        data.nof_elements = dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1;
        sal_strncpy(data.name, DNX_MULTICAST_ALGO_RES_IS_SAR_STR, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(multicast_db.group_is_sar.create(unit, &data, NULL));
    }

    /*
     * static egress bitmap table mapping 
     */

    /*
     * groups offset configuration 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MULTICAST_OFFSETS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OFFSET, INST_SINGLE, bier_size);
    if (dnx_data_multicast.params.max_egr_mc_groups_get(unit) >= 0)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_CORE_0_OFFSET, INST_SINGLE,
                                     dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 + bier_size);
        if (dnx_data_device.general.nof_cores_get(unit) > 1)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_CORE_1_OFFSET, INST_SINGLE,
                                         dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
                                         dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1 + bier_size);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    {
        SHR_IF_ERR_EXIT(dnx_multicast_pp_init(unit));
    }
    /** Create SW Hash table for mapping unique replications to the corresponding MCDB entry in HW */
    sal_memset(&hash_info, 0, sizeof(hash_info));
    hash_info.expected_nof_elements = dnx_data_multicast.params.nof_mcdb_entries_get(unit);
    hash_info.max_nof_elements = dnx_data_multicast.params.nof_mcdb_entries_get(unit);
    hash_info.hash_function = NULL;
    hash_info.print_cb_name = "dnx_mcdb_hash_table_entry_print_cb";
    /** Create Hash table for Ingress groups */
    SHR_IF_ERR_EXIT(multicast_db.key_2_mcdb_id_hash.create(unit, &hash_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
