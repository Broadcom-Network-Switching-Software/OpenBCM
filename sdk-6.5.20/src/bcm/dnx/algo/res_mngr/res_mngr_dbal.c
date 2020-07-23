/** \file res_mngr_dbal.c
 *
 * Resource management functions for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR
/**
* INCLUDE FILES:
* {
*/

/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include "res_mngr_internal.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/algo/l3/source_address_table_allocation.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
/*
 * }
 */

/*
 * Other include files.
 * {
 */
#include <bcm/types.h>
/*
 * }
 */

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/**
 * }
 */

/** incase that table_id == 0 not checking if it connected to specific table */
shr_error_e
dnx_algo_res_dbal_lif_is_allocated(
    int unit,
    int core_id,
    int in_lif,
    int sub_resource_index,
    dbal_tables_e table_id,
    int *result_type,
    int is_aloc_table_any,
    uint8 *is_allocated)
{
    uint32 rec_table_id = 0;
    uint32 rec_result_type = 0;
    dbal_physical_tables_e physical_table_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.is_allocated(unit, in_lif, is_allocated));
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        if (core_id == _SHR_CORE_ALL || (!DNXCMN_CHIP_IS_MULTI_CORE(unit)))
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.is_allocated(unit, in_lif, is_allocated));
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                            inlif_table_dedicated_per_core.is_allocated(unit, core_id, in_lif, is_allocated));
        }
    }
    else
    {
        (*is_allocated) = FALSE;
    }

    if (*is_allocated && (!is_aloc_table_any))
    {
        int rv;

        rv = dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(unit, in_lif, core_id, physical_table_id,
                                                                     &rec_table_id, &rec_result_type, NULL);

        if (rv)
        {
            (*is_allocated) = FALSE;
        }
        else if (table_id != rec_table_id)
        {
            (*is_allocated) = FALSE;
        }
        if ((*result_type) == -1)
        {
            *result_type = rec_result_type;
        }
        else if ((*result_type) != rec_result_type)
        {
            (*is_allocated) = FALSE;
        }
        /*
         * LOG_CLI((BSL_META("*** in_lif %d table ID requested %s found %s***\n"), in_lif,
         * dbal_logical_table_to_string(unit, table_id), dbal_logical_table_to_string(unit, rec_table_id)));
         */
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_dbal_out_lif_is_allocated(
    int unit,
    int core_id,
    int out_lif,
    dbal_tables_e table_id,
    int *result_type,
    int is_alloc_table_any,
    uint8 *is_allocated)
{
    uint32 rec_table_id;
    uint32 rec_result_type;

    uint32 current_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(out_lif);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.is_allocated(unit, current_bank, out_lif, is_allocated));

    if (*is_allocated && !is_alloc_table_any)
    {
        /** validate that the element is allocated for the requested table */
        int rv;
        rv = dnx_lif_mngr_outlif_sw_info_get(unit, out_lif, &rec_table_id, &rec_result_type, NULL, NULL, NULL, NULL);

        if (rv)
        {
            (*is_allocated) = FALSE;
        }
        else if (table_id != rec_table_id)
        {
            (*is_allocated) = FALSE;
        }
        if ((*result_type) == -1)
        {
            *result_type = rec_result_type;
        }
        else if ((*result_type) != rec_result_type)
        {
            (*is_allocated) = FALSE;
        }
        /*
         * LOG_CLI((BSL_META("*** out_lif %d table ID requested %s found %s***\n"), out_lif,
         * dbal_logical_table_to_string(unit, table_id), dbal_logical_table_to_string(unit, rec_table_id)));
         */
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief - check is certaiom out rif is allocated */
shr_error_e
dnx_algo_res_dbal_out_rif_is_allocated(
    int unit,
    int core_id,
    int out_lif,
    dbal_tables_e table_id,
    int *result_type,
    int is_alloc_table_any,
    uint8 *is_allocated)
{
    uint32 rec_table_id;
    uint32 rec_result_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, out_lif, is_allocated));

    if (*is_allocated && !is_alloc_table_any)
    {
        /** validate that the element is allocated for the requested table */
        int rv;
        rv = dnx_lif_mngr_outlif_sw_info_get(unit, out_lif, &rec_table_id, &rec_result_type, NULL, NULL, NULL, NULL);

        if (rv)
        {
            (*is_allocated) = FALSE;
        }
        else if (table_id != rec_table_id)
        {
            (*is_allocated) = FALSE;
        }
        if ((*result_type) == -1)
        {
            *result_type = rec_result_type;
        }
        else if ((*result_type) != rec_result_type)
        {
            (*is_allocated) = FALSE;
        }
        /*
         * LOG_CLI((BSL_META("*** out_lif %d table ID requested %s found %s***\n"), out_lif,
         * dbal_logical_table_to_string(unit, table_id), dbal_logical_table_to_string(unit, rec_table_id)));
         */
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Receive indication whether the FEC index is allocated.
 * \param [in] unit - The unit number.
 * \param [in] fec_index - FEC index
 * \param [out] is_allocated - first bit indicate whether the first FEC has been allocated and second bit for the second
 *        fec.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_algo_res_dbal_fec_is_allocated(
    int unit,
    int fec_index,
    uint8 *is_allocated)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, fec_index, is_allocated));
    /*
     * Check if super FEC with instance 1 is allocated if super FEC with instance 0 is not.
     */
    if (*is_allocated == 0)
    {
        uint8 is_sec_fec_alloc;
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, fec_index + 1, &is_sec_fec_alloc));
        if (is_sec_fec_alloc)
        {
            (*is_allocated) += 2;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_dbal_source_address_index_is_allocated(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int index,
    int is_aloc_table_any,
    uint8 *is_allocated)
{
    int rv;
    int ref_count;
    source_address_entry_t source_address_entry;

    SHR_FUNC_INIT_VARS(unit);

    *is_allocated = 0;

    switch (table_id)
    {
        case DBAL_TABLE_MAC_SOURCE_ADDRESS:
        case DBAL_TABLE_MAC_SOURCE_ADDRESS_DUAL_HOMING:
        case DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL:
            index = index * 2;
            break;
        default:
            break;
    }

    rv = algo_l3_db.source_address_table_allocation.egress_source_address_table.profile_data_get
        (unit, index, &ref_count, (void *) &source_address_entry);

    if (rv == _SHR_E_NOT_FOUND)
    {
        *is_allocated = 0;
        SHR_EXIT();
    }
    if (ref_count == 0)
    {
        *is_allocated = 0;
        SHR_EXIT();
    }

    if (is_aloc_table_any)
    {
        *is_allocated = 1;
        SHR_EXIT();
    }
    switch (table_id)
    {
        case DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6:
            if (source_address_entry.address_type == source_address_type_ipv6)
            {
                *is_allocated = 1;
            }
            break;
        case DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4:
            if (source_address_entry.address_type == source_address_type_ipv4)
            {
                *is_allocated = 1;
            }
            break;
        case DBAL_TABLE_MAC_SOURCE_ADDRESS:
            if (source_address_entry.address_type == source_address_type_mac)
            {
                *is_allocated = 1;
            }
            break;
        case DBAL_TABLE_MAC_SOURCE_ADDRESS_DUAL_HOMING:
            if (source_address_entry.address_type == source_address_type_mac_dual_homing)
            {
                *is_allocated = 1;
            }
            break;
        case DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL:
            if (source_address_entry.address_type == source_address_type_full_mac)
            {
                *is_allocated = 1;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal table for resource %s\n",
                         dbal_logical_table_to_string(unit, table_id));
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_dbal_lif_get_next(
    int unit,
    int core_id,
    int sub_resource_index,
    dbal_tables_e table_id,
    int *result_type,
    int *in_lif)
{
    uint8 is_found = FALSE;
    dbal_physical_tables_e physical_table_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    if (physical_table_id != DBAL_PHYSICAL_TABLE_INLIF_1 && physical_table_id != DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        *in_lif = DNX_ALGO_RES_ILLEGAL_ELEMENT;
    }

    while ((*in_lif) != DNX_ALGO_RES_ILLEGAL_ELEMENT && (!is_found))
    {
        int next_result_type = -1;
        is_found = FALSE;
        if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.get_next(unit, in_lif));
        }
        else
        {
            if (core_id == _SHR_CORE_ALL || (!DNXCMN_CHIP_IS_MULTI_CORE(unit)))
            {
                core_id = _SHR_CORE_ALL;
                SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.get_next(unit, in_lif));
            }
            else
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dedicated_per_core.get_next(unit, core_id, in_lif));
            }
        }

        if ((*in_lif) != DNX_ALGO_RES_ILLEGAL_ELEMENT)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_lif_is_allocated(unit, core_id,
                                                               (*in_lif),
                                                               sub_resource_index, table_id, &next_result_type, 0,
                                                               &is_found));
            if (!is_found)
            {
                (*in_lif)++;
            }
            else
            {
                *result_type = next_result_type;
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_dbal_out_lif_get_next(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int *result_type,
    int *out_lif)
{
    uint8 is_found = FALSE;
    uint32 current_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(*out_lif);

    SHR_FUNC_INIT_VARS(unit);

    while ((*out_lif) != DNX_ALGO_RES_ILLEGAL_ELEMENT && (!is_found))
    {
        int next_result_type = -1;
        is_found = FALSE;
        SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.get_next(unit, current_bank, out_lif));

        if ((*out_lif) != DNX_ALGO_RES_ILLEGAL_ELEMENT)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_out_lif_is_allocated(unit, core_id,
                                                                   *out_lif, table_id,
                                                                   &next_result_type, 0, &is_found));
            if (is_found)
            {
                *result_type = next_result_type;
                break;
            }
            else
            {
                (*out_lif)++;
            }
        }
        else
        {
            current_bank++;
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_next_bank_get(unit, &current_bank));
            if (current_bank != DNX_ALGO_RES_ILLEGAL_ELEMENT)
            {
                (*out_lif) = OUTLIF_BANK_FIRST_INDEX(unit, current_bank);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** \brief - out rif allocatoe iterator, returning the next allocated out rif */
shr_error_e
dnx_algo_res_dbal_out_rif_get_next(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int *result_type,
    int *out_lif)
{
    uint8 is_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    while ((*out_lif) != DNX_ALGO_RES_ILLEGAL_ELEMENT && (!is_found))
    {
        int next_result_type = -1;
        is_found = FALSE;
        SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.get_next(unit, out_lif));

        if ((*out_lif) != DNX_ALGO_RES_ILLEGAL_ELEMENT)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_out_rif_is_allocated
                            (unit, core_id, *out_lif, table_id, &next_result_type, 0, &is_found));

            if (is_found)
            {
                *result_type = next_result_type;
                break;
            }
            else
            {
                (*out_lif)++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Receive next allocated ECMP group in the ECMP resources (basic and extended).
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core ID
 * \param [in] sub_resource_index - Sub-resource index, value is 0.
 * \param [out] ecmp_index - Index of the next existing ECMP group.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_algo_res_dbal_ecmp_get_next(
    int unit,
    int core_id,
    int sub_resource_index,
    int *ecmp_index)
{
    uint8 is_extended = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Skip ECMP index 0 as it is not valid for allocation */
    if (*ecmp_index == 0)
    {
        *ecmp_index = 1;
    }
    /** Receive the correct name of the resource based on the value of the ECMP index */
    if (*ecmp_index + 1 > dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit))
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_extended_res_manager.get_next(unit, ecmp_index));
        is_extended = 1;
    }
    else
    {
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_res_manager.get_next(unit, ecmp_index));
    }

    /*
     * If the element is invalid, check if the execution has reached to the end of the resource for the basic range.
     * If so, define the ecmp_index to be the last index of the basic range and call get_next for the extended range.
     */
    if (*ecmp_index == DNX_ALGO_RES_ILLEGAL_ELEMENT && is_extended == 0)
    {
        *ecmp_index = dnx_data_l3.ecmp.max_ecmp_basic_mode_get(unit);
        SHR_IF_ERR_EXIT(ecmp_db_info.ecmp_extended_res_manager.get_next(unit, ecmp_index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find next available entry in OAMP MEP DB table.  Should
 *  only be used for dbal testing and nothing else.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core ID
 * \param [in] sub_resource_index - Sub-resource index, value is 0.
 * \param [in,out] index - Input - oam_id to start from; output -
 *        oam_id found.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_res_dbal_oamp_mep_db_get_next(
    int unit,
    int core_id,
    int sub_resource_index,
    int *index)
{
    int full_entry_threshold, short_entries_in_full, misalignment;
    SHR_FUNC_INIT_VARS(unit);

    full_entry_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (*index >= full_entry_threshold)
    {
        /**
         * The entry ID is above the threshold - only entry IDs divisible by
         *  the number of sub-entries are allowed
         */

        /** What is the number of sub-entries? */
        short_entries_in_full = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

        /** If the entry ID is not aligned, what is the offset? */
        misalignment = (*index % short_entries_in_full);

        /**
         *  If the entry ID is not aligned, round up to the nearest
         *  number divisible by number of sub-entries
         */
        if (misalignment > 0)
            *index += (short_entries_in_full - misalignment);

        /** If above the threshold, the next free ID will be aligned */
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.get_next(unit, index));
    }
    else
    {
        /** The next entry is below the threshold - use result from generic get_next */
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.get_next(unit, index));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  in this case there is a different granularity per table type.
 */

shr_error_e
dnx_algo_res_dbal_source_address_index_get_next(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int *index)
{
    uint8 is_allocated;
    int index_to_check = *index;
    SHR_FUNC_INIT_VARS(unit);

    switch (table_id)
    {
        /** for the following tables the key is x but the allocated values is 2x */
        case DBAL_TABLE_MAC_SOURCE_ADDRESS:
        case DBAL_TABLE_MAC_SOURCE_ADDRESS_DUAL_HOMING:
        case DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL:
            index_to_check = index_to_check * 2;
            break;
        default:
            break;
    }

    SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.
                    egress_source_address_table.get_next(unit, &index_to_check));

    while (index_to_check != DNX_ALGO_TEMPLATE_ILLEGAL_PROFILE)
    {
        switch (table_id)
        {
            case DBAL_TABLE_MAC_SOURCE_ADDRESS:
            case DBAL_TABLE_MAC_SOURCE_ADDRESS_DUAL_HOMING:
            case DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL:
                index_to_check = index_to_check / 2;
                break;
            default:
                break;
        }
        SHR_IF_ERR_EXIT(dnx_algo_res_dbal_source_address_index_is_allocated
                        (unit, core_id, table_id, index_to_check, 0, &is_allocated));

        if (is_allocated)
        {
            (*index) = index_to_check;
            SHR_EXIT();
        }
        switch (table_id)
        {
            case DBAL_TABLE_MAC_SOURCE_ADDRESS:
            case DBAL_TABLE_MAC_SOURCE_ADDRESS_DUAL_HOMING:
            case DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL:
                index_to_check = index_to_check * 2;
                break;
            default:
                break;
        }
        index_to_check++;
        SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.
                        egress_source_address_table.get_next(unit, &index_to_check));
    }

    (*index) = DNX_ALGO_RES_ILLEGAL_ELEMENT;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Receive next allocated FEC element in the resource.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core ID
 * \param [in] table_id - The DBAL table to which the FECs are added.
 * \param [out] index - Index of the next existing FEC.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_algo_res_dbal_fec_get_next(
    int unit,
    int core_id,
    dbal_tables_e table_id,
    int *index)
{
    int fec_index;
    uint32 nof_fec_banks;
    uint32 hierarchy = 0;
    uint8 is_found = 0;
    uint32 bank_hierarchy = 0;
    uint32 sub_resource_index;

    SHR_FUNC_INIT_VARS(unit);

    if (table_id == DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY)
    {
        hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
    }
    else if (table_id == DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY)
    {
        hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2;
    }
    else
    {
        hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3;
    }
    /*
     * Find the sub-resource to which this FEC belongs -
     * receive the physical FEC that it represents and divide it by the number of pFECs in each memory bank.
     */
    sub_resource_index =
        DNX_ALGO_L3_PHYSICAL_FEC_GET(unit, *index * 2) / dnx_data_l3.fec.nof_physical_fecs_per_bank_get(unit);
    /*
     * To get the FEC index inside the resource remove from the FEC id the first FEC id of this resource.
     */
    fec_index =
        (*index * dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit)) - MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit,
                                                                                                               sub_resource_index);

    SHR_IF_ERR_EXIT(dnx_algo_l3_nof_fec_banks_get(unit, &nof_fec_banks));

    /*
     * Iterate over all sub_resource indexes starting from *index / bank_size (current sub-resource ID)
     * until a valid FEC is found or until all banks are checked for entries and ILLEGAL_ELEMENT is returned.
     */
    while (sub_resource_index < nof_fec_banks && is_found == 0)
    {
        is_found = 0;

        SHR_IF_ERR_EXIT(mdb_db_infos.fec_hierarchy_info.
                        fec_hierarchy_map.get(unit, sub_resource_index, &bank_hierarchy));
        /*
         * Verify that the hierarchy of this bank belongs to current hierarchy iteration
         * (verify that the elements in the bank of the iteration belong to the current dbal table).
         * If they don't skip iteration.
         */
        if (bank_hierarchy != hierarchy)
        {
            sub_resource_index++;
            fec_index = DNX_ALGO_RES_ILLEGAL_ELEMENT;
            continue;
        }
        /** Set valid value to fec_index before calling get_next. */
        if (fec_index == DNX_ALGO_RES_ILLEGAL_ELEMENT)
        {
            fec_index = 0;
        }
        SHR_IF_ERR_EXIT(algo_l3_db.fec_alloc.l3_fec_res_mngr.
                        fec_res_manager.get_next(unit, sub_resource_index, &fec_index));
        /*
         * If the fec_index we received is invalid,
         * increment the sub-resource ID and continue with next iteration of loop.
         * Otherwise, exit the loop with is_found indication.
         */
        if (fec_index == DNX_ALGO_RES_ILLEGAL_ELEMENT)
        {
            sub_resource_index++;
        }
        else
        {
            is_found = 1;
        }
    }

    if (fec_index != DNX_ALGO_RES_ILLEGAL_ELEMENT)
    {
        /** To get back the FEC ID add to the FEC index inside the resource the first FEC ID of this resource. */
        *index =
            (fec_index +
             MDB_CALC_FEC_ID_START_ADDR_OF_BANK_ID(unit,
                                                   sub_resource_index)) /
            dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit);
    }
    else
    {
        *index = DNX_ALGO_RES_ILLEGAL_ELEMENT;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_dbal_source_address_index_alloc_dealloc(
    int unit,
    dbal_tables_e table_id,
    int *index,
    int is_alloc)
{
    source_address_entry_t source_address_entry;
    uint8 is_first;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&source_address_entry, 0, sizeof(source_address_entry));

    switch (table_id)
    {
        case DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6:
            source_address_entry.address_type = source_address_type_ipv6;
            source_address_entry.address.ipv6_address[0] = *index;/** dummy value just for test*/
            break;
        case DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV4:
            source_address_entry.address_type = source_address_type_ipv4;
            source_address_entry.address.ipv6_address[0] = *index;/** dummy value just for test*/
            break;
        case DBAL_TABLE_MAC_SOURCE_ADDRESS:
            source_address_entry.address_type = source_address_type_mac;
            source_address_entry.address.ipv6_address[0] = *index;/** dummy value just for test*/
            (*index) = (*index) * 2;
            break;
        case DBAL_TABLE_MAC_SOURCE_ADDRESS_DUAL_HOMING:
            source_address_entry.address_type = source_address_type_mac_dual_homing;
            source_address_entry.address.ipv6_address[0] = *index;/** dummy value just for test*/
            (*index) = (*index) * 2;
            break;
        case DBAL_TABLE_MAC_SOURCE_ADDRESS_FULL:
            source_address_entry.address_type = source_address_type_full_mac;
            source_address_entry.address.ipv6_address[0] = *index;/** dummy value just for test*/
            (*index) = (*index) * 2;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal table for resource %s\n",
                         dbal_logical_table_to_string(unit, table_id));
            break;
    }

    if (is_alloc)
    {
        SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.allocate_single
                        (unit, DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &source_address_entry, NULL, index, &is_first));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_l3_db.source_address_table_allocation.egress_source_address_table.free_single
                        (unit, (*index), &is_first));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Allocate a FEC in the resource.
 * \param [in] unit - The unit number.
 * \param [in] table_id - The DBAL table to which the FECs are added.
 * \param [in] index - The super FEC id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_algo_res_dbal_fec_allocate(
    int unit,
    dbal_tables_e table_id,
    int *index)
{
    int fec_id = *index * 2;
    uint32 flags = DNX_ALGO_L3_FEC_WITH_ID;
    uint32 hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_1;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    /** Receive the hierarchy level based on the DBAL table in use. */
    if (table_id == DBAL_TABLE_SUPER_FEC_2ND_HIERARCHY)
    {
        hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_2;
    }
    else if (table_id == DBAL_TABLE_SUPER_FEC_3RD_HIERARCHY)
    {
        hierarchy = DBAL_ENUM_FVAL_HIERARCHY_LEVEL_HIERARCHY_LEVEL_3;
    }
    /** In unit tests the FECs will be allocated with no protection and no statistics. */
    rv = dnx_algo_l3_fec_allocate(unit, &fec_id, flags, hierarchy, DBAL_ENUM_FVAL_FEC_RESOURCE_TYPE_NO_PROT_NO_STAT);

    if (rv != _SHR_E_NONE)
    {
        LOG_CLI((BSL_META("fec %d failed\n"), fec_id));
        SHR_IF_ERR_EXIT(rv);
    }

    *index = fec_id / 2;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Free a FEC from the resource.
 * \param [in] unit - The unit number.
 * \param [in] index - The super FEC id.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_algo_res_dbal_fec_deallocate(
    int unit,
    int index)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_l3_fec_deallocate(unit, index * dnx_data_mdb.direct.nof_fecs_in_super_fec_get(unit)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find if specified RMEP ID is allocated.  Should
 *  only be used for dbal testing and nothing else.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core ID
 * \param [in] sub_resource_index - Sub-resource index, value is 0.
 * \param [in] rmep_id - RMEP ID to check.
 * \param [out] is_allocated - Will be set to TRUE if allocated, FALSE otherwise.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_res_dbal_rmep_is_allocated(
    int unit,
    int core_id,
    int sub_resource_index,
    int rmep_id,
    uint8 *is_allocated)
{
    int self_contained_threshold, index_in_full_pool;
    uint32 oamp_rmep_scale_limitataion = 0;
    uint32 nof_entries_per_rmep = 0;
    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
    oamp_rmep_scale_limitataion =
        dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_rmep_scale_limitataion);
    if (rmep_id >= self_contained_threshold)
    {
        nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 4 : 2;
        /** The above threshold pool starts at 0, and each element represents short_entries_in_self_cont entries */
        index_in_full_pool = (rmep_id + 1 - self_contained_threshold) / nof_entries_per_rmep;
        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_above_threshold.is_allocated(unit, index_in_full_pool, is_allocated));
    }
    else
    {
        nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 2 : 1;
        index_in_full_pool = rmep_id / nof_entries_per_rmep;
        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_below_threshold.is_allocated(unit, index_in_full_pool, is_allocated));
    }

exit:
    SHR_FUNC_EXIT;
}

/** note - if table_id is 0, it means that asking if the resource is alocated at all not related to specific table */
shr_error_e
dnx_algo_res_dbal_is_allocated(
    int unit,
    int core_id,
    int sub_resource_index,
    int index,
    dbal_tables_e table_id,
    int result_type,
    dbal_fields_e field_id,
    int is_aloc_table_any,
    uint8 *is_allocated)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    (*is_allocated) = 0;

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    switch (field_type)
    {
        case DBAL_FIELD_TYPE_DEF_OUT_RIF:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_out_rif_is_allocated(unit, core_id, index, table_id, &result_type,
                                                                   is_aloc_table_any, is_allocated));
            break;
        case DBAL_FIELD_TYPE_DEF_IN_LIF:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_lif_is_allocated(unit, core_id, index, sub_resource_index,
                                                               table_id, &result_type, is_aloc_table_any,
                                                               is_allocated));
            break;
        case DBAL_FIELD_TYPE_DEF_OUT_LIF:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_out_lif_is_allocated(unit, core_id, index, table_id, &result_type,
                                                                   is_aloc_table_any, is_allocated));
            break;
        case DBAL_FIELD_TYPE_DEF_ECMP_ID:
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_is_allocated(unit, index, is_allocated));
            break;
        case DBAL_FIELD_TYPE_DEF_QOS_PROFILE:
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.is_allocated(unit, index, is_allocated));
            break;
        case DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE:
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn.is_allocated(unit, index, is_allocated));
            break;
        case DBAL_FIELD_TYPE_DEF_RMEP_INDEX:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_rmep_is_allocated
                            (unit, core_id, sub_resource_index, index, is_allocated));
            break;
        case DBAL_FIELD_TYPE_DEF_OAMP_ENTRY_ID:
            SHR_IF_ERR_EXIT(algo_oamp_db.oamp_mep_db_entry.is_allocated(unit, index, is_allocated));
            break;
        case DBAL_FIELD_TYPE_DEF_SUPER_FEC_ID:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_fec_is_allocated(unit, index * 2, is_allocated));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_SOURCE_ADDRESS_INDEX:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_source_address_index_is_allocated
                            (unit, core_id, table_id, index, is_aloc_table_any, is_allocated));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_ECMP_GROUP_PROFILE:
        {
            SHR_IF_ERR_EXIT(dnx_algo_l3_res_dbal_ecmp_group_profile_is_allocated(unit, index, is_allocated));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Add relevant case for the relevant resource.\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find next available valid in OAMP RMEP DB table.  Should
 *  only be used by dbal (for iterator) and nothing else.
 * \param [in] unit - The unit number.
 * \param [in] core_id - Core ID
 * \param [in] sub_resource_index - Sub-resource index, value is 0.
 * \param [in,out] index - Input - oam_id to start from; output -
 *        rmep entry ID found.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
dnx_algo_res_dbal_oamp_rmep_db_get_next(
    int unit,
    int core_id,
    int sub_resource_index,
    int *index)
{
    int self_contained_threshold, index_in_full_pool;
    uint32 oamp_rmep_scale_limitataion = 0;
    uint32 nof_entries_per_rmep = 2;
    SHR_FUNC_INIT_VARS(unit);

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
    oamp_rmep_scale_limitataion =
        dnx_data_oam.feature.feature_get(unit, dnx_data_oam_feature_oamp_rmep_scale_limitataion);

    if (*index < self_contained_threshold)
    {
        nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 2 : 1;
        index_in_full_pool = ((*index) + (nof_entries_per_rmep - 1)) / nof_entries_per_rmep;
        SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_below_threshold.get_next(unit, &index_in_full_pool));
        if (index_in_full_pool != DNX_ALGO_RES_ILLEGAL_ELEMENT)
        {
            *index = nof_entries_per_rmep * index_in_full_pool;
            SHR_EXIT();
        }
        else
        {
            *index = self_contained_threshold - 1;
        }
    }

    nof_entries_per_rmep = oamp_rmep_scale_limitataion ? 4 : 2;
    index_in_full_pool = ((*index) + (nof_entries_per_rmep - 1) - self_contained_threshold) / nof_entries_per_rmep;

    /** Get next in this pool */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_rmep_id_above_threshold.get_next(unit, &index_in_full_pool));

    if (index_in_full_pool == DNX_ALGO_RES_ILLEGAL_ELEMENT)
    {
        *index = DNX_ALGO_RES_ILLEGAL_ELEMENT;

        SHR_EXIT();
    }

    *index = self_contained_threshold + nof_entries_per_rmep * index_in_full_pool;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_res_dbal_get_next(
    int unit,
    int core_id,
    int sub_resource_index,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int *result_type,
    int *index)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    switch (field_type)
    {
        case DBAL_FIELD_TYPE_DEF_OUT_RIF:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_out_rif_get_next(unit, core_id, table_id, result_type, index));
            break;
        case DBAL_FIELD_TYPE_DEF_IN_LIF:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_lif_get_next
                            (unit, core_id, sub_resource_index, table_id, result_type, index));
            break;
        case DBAL_FIELD_TYPE_DEF_OUT_LIF:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_out_lif_get_next(unit, core_id, table_id, result_type, index));
            break;
        case DBAL_FIELD_TYPE_DEF_ECMP_ID:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_ecmp_get_next(unit, core_id, sub_resource_index, index));
            break;
        case DBAL_FIELD_TYPE_DEF_QOS_PROFILE:
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.get_next(unit, index));
            break;
        case DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE:
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn.get_next(unit, index));
            break;
        case DBAL_FIELD_TYPE_DEF_RMEP_INDEX:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_oamp_rmep_db_get_next(unit, core_id, sub_resource_index, index));
            break;
        case DBAL_FIELD_TYPE_DEF_OAMP_ENTRY_ID:
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_oamp_mep_db_get_next(unit, core_id, sub_resource_index, index));
            break;
        case DBAL_FIELD_TYPE_DEF_SUPER_FEC_ID:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_fec_get_next(unit, core_id, table_id, index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_SOURCE_ADDRESS_INDEX:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_source_address_index_get_next(unit, core_id, table_id, index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_ECMP_GROUP_PROFILE:
        {
            uint32 hierarchy = 0;
            SHR_IF_ERR_EXIT(dnx_algo_l3_res_dbal_ecmp_group_profile_get_next(unit, hierarchy, index));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Add relevant case for the relevant resource.\n");
            break;
    }

    if ((*index) == DNX_ALGO_RES_ILLEGAL_ELEMENT)
    {
        (*index) = DNX_ALGO_RES_DBAL_ILLEGAL_ELEMENT;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_res_dbal_alloc_in_lif(
    int unit,
    int core_id,
    int index,
    dbal_tables_e table_id,
    int result_type)
{
    dbal_physical_tables_e physical_table_id;
    int entry_size_bits;
    int entry_size_indexes;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the physical table associated with this lif format.
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, table_id, result_type, &entry_size_bits));

    entry_size_indexes = UTILEX_DIV_ROUND_UP(entry_size_bits, dnx_data_lif.in_lif.inlif_index_bit_size_get(unit));

    if (entry_size_indexes < 2)
    {
        /*
         * Inlif entry size may be small, but it always takes at least the minimum number of indexes.
         */
        entry_size_indexes = 2;
    }

    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.allocate_single(unit,
                                                                                            DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                                                            &entry_size_indexes,
                                                                                            &index));
    }
    else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        if (core_id == _SHR_CORE_ALL || (!DNXCMN_CHIP_IS_MULTI_CORE(unit)))
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.allocate_single(unit,
                                                                                              DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                                                              &entry_size_indexes,
                                                                                              &index));
        }
        else
        {
            SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dedicated_per_core.allocate_single(unit, core_id,
                                                                                                   DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                                                                   &entry_size_indexes,
                                                                                                   &index));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table id doesn't match lif input type..\n");
    }

    SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_set
                    (unit, index, core_id, table_id, result_type, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_res_dbal_alloc_out_lif(
    int unit,
    int index,
    dbal_tables_e table_id,
    int result_type)
{
    lif_mngr_local_outlif_info_t outlif_info;
    uint32 current_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(index);
    int element = index;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = table_id;
    outlif_info.dbal_result_type = result_type;
    outlif_info.outlif_phase = 0;

    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.allocate_single(unit, current_bank,
                                                                 DNX_ALGO_RES_ALLOCATE_WITH_ID,
                                                                 (void *) &outlif_info, &element));

    outlif_info.local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;
    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set
                    (unit, &outlif_info, index, table_id, result_type, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_res_dbal_alloc_out_rif(
    int unit,
    int index,
    dbal_tables_e table_id,
    int result_type)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = table_id;
    outlif_info.dbal_result_type = result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.
                    rif_res_manager.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, (void *) &outlif_info,
                                                    &index));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_set
                    (unit, &outlif_info, index, table_id, result_type, 0));

exit:
    SHR_FUNC_EXIT;
}

/** this function is used only for DBAL unit-tests do not use it  */
shr_error_e
dnx_algo_res_dbal_alloc(
    int unit,
    int core_id,
    int sub_resource_index,
    int index,
    dbal_tables_e table_id,
    int result_type,
    dbal_fields_e field_id)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    switch (field_type)
    {
        case DBAL_FIELD_TYPE_DEF_OUT_RIF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_alloc_out_rif(unit, index, table_id, result_type));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_IN_LIF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_alloc_in_lif(unit, core_id, index, table_id, result_type));
            break;
        }

        case DBAL_FIELD_TYPE_DEF_OUT_LIF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_alloc_out_lif(unit, index, table_id, result_type));
            break;
        }

        case DBAL_FIELD_TYPE_DEF_ECMP_ID:
        {
            /** All ECMP groups in DBAL unit tests will be allocated with hierarchy 1. */
            uint32 hierarchy = 0;
            uint32 ecmp_group_flags = 0;
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_allocate(unit, &index, BCM_L3_WITH_ID, ecmp_group_flags, hierarchy));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_QOS_PROFILE:
        {
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE:
        {
            SHR_IF_ERR_EXIT(algo_qos_db.
                            ingress_qos_ecn.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_RMEP_INDEX:
        {
            SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_alloc(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, &index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_OAMP_ENTRY_ID:
        {
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_entry_alloc(unit, &index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_SUPER_FEC_ID:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_fec_allocate(unit, table_id, &index));
            break;
        }

        case DBAL_FIELD_TYPE_DEF_SOURCE_ADDRESS_INDEX:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_source_address_index_alloc_dealloc(unit, table_id, &index, 1));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_ECMP_GROUP_PROFILE:
        {
            SHR_IF_ERR_EXIT(dnx_algo_l3_res_dbal_ecmp_group_profile_allocate(unit, &index));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Add relevant case for the resource you need to alloc.\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_res_dbal_free_out_lif(
    int unit,
    int index,
    dbal_tables_e table_id,
    int result_type)
{
    lif_mngr_local_outlif_info_t outlif_info;
    int outlif_bank;
    int deallocation_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = table_id;
    outlif_info.dbal_result_type = result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_INVALID;
    outlif_info.local_outlif = index;

    /*
     * Update the outlif info with its phase. This can be deduced from the outlif bank, but only in this function
     * and not in any of the calling functions.
     */
    outlif_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(index);

    /*
     * Get the number of entries to free.
     */
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(unit, &outlif_info, &deallocation_size));

    SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.free_several(unit, outlif_bank, deallocation_size, index));
    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(unit, index));

exit:
    SHR_FUNC_EXIT;
}

/** \brief - out rif allocatoe free, releasing an allocation of a given out rif */
static shr_error_e
dnx_algo_res_dbal_free_out_rif(
    int unit,
    int index,
    dbal_tables_e table_id,
    int result_type)
{
    int nof_rifs_per_entry;
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&outlif_info, 0, sizeof(outlif_info));
    outlif_info.dbal_table_id = table_id;
    outlif_info.dbal_result_type = result_type;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_RIF;
    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_info_to_nof_outlifs_per_entry(unit, &outlif_info, &nof_rifs_per_entry));

    SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.free_several(unit, nof_rifs_per_entry, index));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_delete(unit, index));

exit:
    SHR_FUNC_EXIT;
}

/** this function is used only for DBAL unit-tests do not use it  */
shr_error_e
dnx_algo_res_dbal_free(
    int unit,
    int core_id,
    int sub_resource_index,
    int index,
    dbal_tables_e table_id,
    int result_type,
    dbal_fields_e field_id)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    switch (field_type)
    {
        case DBAL_FIELD_TYPE_DEF_OUT_RIF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_free_out_rif(unit, index, table_id, result_type));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_IN_LIF:
        {
            dbal_physical_tables_e physical_table_id;

            /*
             * Get the physical table associated with this lif format.
             */
            SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                            (unit, table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

            if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1)
            {
                SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_shared_by_cores.free_single(unit, index));
            }
            else if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
            {
                if (core_id == _SHR_CORE_ALL || (!DNXCMN_CHIP_IS_MULTI_CORE(unit)))
                {
                    SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.inlif_table_dpc_all_cores.free_single(unit, index));
                }
                else
                {
                    SHR_IF_ERR_EXIT(lif_mngr_db.local_inlif.
                                    inlif_table_dedicated_per_core.free_single(unit, core_id, index));
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table id doesn't match lif input type.\n");
            }

            SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_delete(unit, core_id, index, table_id));
            break;
        }

        case DBAL_FIELD_TYPE_DEF_OUT_LIF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_free_out_lif(unit, index, table_id, result_type));
            break;
        }

        case DBAL_FIELD_TYPE_DEF_ECMP_ID:
        {
            SHR_IF_ERR_EXIT(dnx_algo_l3_ecmp_deallocate(unit, index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_QOS_PROFILE:
        {
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.free_single(unit, index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE:
        {
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn.free_single(unit, index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_RMEP_INDEX:
        {
            SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_free(unit, index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_OAMP_ENTRY_ID:
        {
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_entry_dealloc(unit, index));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_SUPER_FEC_ID:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_fec_deallocate(unit, index));
            break;
        }

        case DBAL_FIELD_TYPE_DEF_SOURCE_ADDRESS_INDEX:
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_source_address_index_alloc_dealloc(unit, table_id, &index, 0));
            break;
        }
        case DBAL_FIELD_TYPE_DEF_ECMP_GROUP_PROFILE:
        {
            SHR_IF_ERR_EXIT(dnx_algo_l3_res_dbal_ecmp_group_profile_free(unit, index));
            break;
        }

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Add relevant case for the resource you want to free.\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
