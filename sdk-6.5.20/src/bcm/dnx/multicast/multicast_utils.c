/** \file multicast_utils.c
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
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/multicast.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <bcm_int/dnx/stk/stk_sys.h>

#include <bcm_int/common/multicast.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <include/bcm/mcast.h>

#include <bcm_int/dnx/bier/bier.h>

#include "multicast_imp.h"
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * see .h file
 */
shr_error_e
dnx_multicast_exr_core_rep_exist(
    int unit,
    uint32 group_id,
    uint32 core_id,
    uint32 *rep_exist)
{
    uint32 entry_id;
    dnx_multicast_mcdb_dbal_field_t entry_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, group_id, FALSE, FALSE, core_id, &entry_id));
    SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, entry_id, &entry_val));

    if ((entry_val.format == DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT) &&
        (entry_val.dest[0] == DNX_MULTICAST_ING_INVALID_DESTINATION(unit)) &&
        (entry_val.next_entry_id == DNX_MULTICAST_END_OF_LIST_PTR))
    {
        *rep_exist = FALSE;
    }
    else
    {
        *rep_exist = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - given MCDB entry index, the region and the index inside the region is returned.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [out] id - internal region index
 *   \param [out] region - MCDB regions
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   None
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_entry_to_id(
    int unit,
    uint32 entry_id,
    uint32 *id,
    dnx_multicast_group_region_t * region)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_id < dnx_bier_mcdb_size_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "incorrect entry id (%u)", entry_id);
    }
    entry_id -= dnx_bier_mcdb_size_get(unit);

    if (entry_id < dnx_data_multicast.params.max_ing_mc_groups_get(unit))
    {
        *id = entry_id;
        *region = DNX_MULTICAST_GROUP_REGION_INGRESS;
    }
    else if (entry_id < dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
             dnx_data_multicast.params.max_egr_mc_groups_get(unit))
    {
        *id = entry_id - dnx_data_multicast.params.max_ing_mc_groups_get(unit) - 1;
        *region = DNX_MULTICAST_GROUP_REGION_EG_CORE0;
    }
    else if (entry_id < dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
             dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1))
    {
        *id = entry_id - dnx_data_multicast.params.max_ing_mc_groups_get(unit) - 1 -
            dnx_data_multicast.params.max_egr_mc_groups_get(unit);
        *region = DNX_MULTICAST_GROUP_REGION_EG_CORE1;
    }
    else if (entry_id < dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
             dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1) +
             dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit))
    {
        *id = entry_id - dnx_data_multicast.params.max_ing_mc_groups_get(unit) - 1 -
            dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1);
        *region = DNX_MULTICAST_GROUP_REGION_BITMAP_INGRESS;
    }
    else if (entry_id < dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
             dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1) +
             dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit) +
             dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit))
    {
        *id = entry_id - dnx_data_multicast.params.max_ing_mc_groups_get(unit) - 1 -
            dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1) -
            dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit);
        *region = DNX_MULTICAST_GROUP_REGION_BITMAP_EG_CORE0;
    }
    else if (entry_id < dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
             dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1) +
             dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit) +
             dnx_data_device.general.nof_cores_get(unit) * dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit))
    {
        *id = entry_id - dnx_data_multicast.params.max_ing_mc_groups_get(unit) - 1 -
            dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1) -
            dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit) -
            dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit);
        *region = DNX_MULTICAST_GROUP_REGION_BITMAP_EG_CORE1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "incorrect entry id (%u)", entry_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - given group/bitmap id and ingress/core indications, the MCDB entry index is returned.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] id - group/bitmap id
 *   \param [in] is_bmp - group/bitmap
 *   \param [in] is_ingress - group/bitmap is ingress
 *   \param [in] core_id - relevant for is_ingress==0
 *   \param [out] entry_id - MCDB index
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
dnx_multicast_id_to_entry(
    int unit,
    uint32 id,
    uint32 is_bmp,
    uint32 is_ingress,
    uint32 core_id,
    uint32 *entry_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_bmp && is_ingress)
    {
        *entry_id = id + dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
            dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1);
    }
    else if (is_bmp && !is_ingress)
    {
        *entry_id = id + dnx_data_multicast.params.nof_ing_mc_bitmaps_get(unit) +
            dnx_data_device.general.nof_cores_get(unit) * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1) +
            dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
            core_id * dnx_data_multicast.params.nof_egr_mc_bitmaps_get(unit);
    }
    else if (!is_bmp && is_ingress)
    {
        *entry_id = id;
    }
    else
    {
        /*
         * (!is_bmp && !is_ingress) 
         */
        *entry_id = id + dnx_data_multicast.params.max_ing_mc_groups_get(unit) + 1 +
            core_id * (dnx_data_multicast.params.max_egr_mc_groups_get(unit) + 1);
    }

    *entry_id += dnx_bier_mcdb_size_get(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocates entry and returns the ID. allocation referring flags and core_id will be supported in the future.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] with_id - whether the allocation needs to be done with id
 *   \param [in] entry_id - MCDB index
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
dnx_multicast_entry_allocate(
    int unit,
    uint32 flags,
    uint32 core_id,
    uint32 with_id,
    uint32 *entry_id)
{
    uint32 with_id_flag;
    SHR_FUNC_INIT_VARS(unit);

    with_id_flag = with_id ? DNX_ALGO_RES_ALLOCATE_WITH_ID : 0;
    SHR_IF_ERR_EXIT(multicast_db.mcdb.allocate_single(unit, with_id_flag, NULL, (int *) entry_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocates an entry from the requested region and moves the current MCDB entry to the allocated entry
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] entry_id - MCDB index
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
dnx_multicast_entry_relocate(
    int unit,
    uint32 flags,
    uint32 core_id,
    uint32 entry_id)
{
    uint32 new_entry_id;
    dnx_multicast_mcdb_dbal_field_t entry_val;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * allocate new entry 
     */
    SHR_IF_ERR_EXIT(dnx_multicast_entry_allocate(unit, flags, core_id, FALSE, &new_entry_id));

    /*
     * copy the the current to new entry 
     */
    SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, entry_id, &entry_val));
    SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, new_entry_id, &entry_val));

    /*
     * update the link list with relocation 
     */
    SHR_IF_ERR_EXIT(dnx_multicast_entry_next_ptr_set(unit, entry_val.prev_entry, new_entry_id));
    SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, entry_val.next_entry_id, new_entry_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - clears an MCDB entry through DBAL interface
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] is_tdm - is the entry a tdm
 *   \param [in] is_bmp - is the entry a bitmap
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
dnx_multicast_entry_clear(
    int unit,
    uint32 entry_id,
    uint32 is_tdm,
    uint32 is_bmp)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    int ext_mode;
    uint32 egr_invalid_destination;
    SHR_FUNC_INIT_VARS(unit);

    entry_val.cud = DNX_MULTICAST_INVALID_CUD;
    entry_val.next_entry_id = DNX_MULTICAST_END_OF_LIST_PTR;
    entry_val.prev_entry = DNX_MULTICAST_END_OF_LIST_PTR;

    if (is_tdm)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));
        SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));
        entry_val.format = (ext_mode ? DBAL_RESULT_TYPE_MCDB_TDM_EXT_FORMAT : DBAL_RESULT_TYPE_MCDB_TDM_FORMAT);
        entry_val.dest[0] = entry_val.dest[1] = entry_val.dest[2] = entry_val.dest[3] = egr_invalid_destination;
    }
    else if (is_bmp)
    {
        entry_val.format = DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT;
        entry_val.bmp_ptr = DNX_MULTICAST_END_OF_LIST_PTR;
    }
    else
    {
        entry_val.format = DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT;
        entry_val.dest[0] = DNX_MULTICAST_ING_INVALID_DESTINATION(unit);
    }

    SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, entry_id, &entry_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - removes MCDB entry through DBAL interface and updates the link list by updating next and prev pointers.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_id - MCDB index
 *   \param [in] prev_entry_id - pointer to the previous MCDB entry in the link list
 *   \param [in] next_entry_id - pointer to the next MCDB entry in the link list
 *   \param [in] is_bmp - is the entry a bitmap
 *   \param [out] cur_entry_replaced - indicates whether current entry id is replaced with other entry
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
dnx_multicast_entry_remove(
    int unit,
    uint32 entry_id,
    uint32 prev_entry_id,
    uint32 next_entry_id,
    uint32 is_bmp,
    uint32 *cur_entry_replaced)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    SHR_FUNC_INIT_VARS(unit);
    *cur_entry_replaced = FALSE;

    /*
     * previous entry next_pointer set to next entry 
     */
    if (prev_entry_id == DNX_MULTICAST_END_OF_LIST_PTR)
    {
        /**
         * if this is the only entry of the group we can simply clear the entry. Otherwise, we will move the next entry
         * to the current entry, update the link list pointers and free the next entry
         */
        if (next_entry_id == DNX_MULTICAST_END_OF_LIST_PTR)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_entry_clear(unit, entry_id, FALSE, is_bmp));
        }
        else
        {
            *cur_entry_replaced = TRUE;
            SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, next_entry_id, &entry_val));
            entry_val.prev_entry = DNX_MULTICAST_END_OF_LIST_PTR;
            SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, entry_id, &entry_val));

            /*
             * next next entry prev_pointer set to next entry
             */
            if (next_entry_id != DNX_MULTICAST_END_OF_LIST_PTR)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, entry_val.next_entry_id, entry_id));
            }

            /*
             * free next entry SW data base
             */
            SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, next_entry_id));
        }
    }
    else
    {
        /*
         * If the entry is the first Bitmap entry in the group, 
         * the bitmap pointer in the previous entry needs to be updated
         */
        SHR_IF_ERR_EXIT(dnx_multicast_entry_get(unit, prev_entry_id, &entry_val));
        if ((entry_val.format == DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT) && is_bmp)
        {
            entry_val.next_entry_id = next_entry_id;
            entry_val.bmp_ptr = next_entry_id;
            if (next_entry_id == DNX_MULTICAST_END_OF_LIST_PTR)
            {
                entry_val.cud = DNX_MULTICAST_INVALID_CUD;
            }
            SHR_IF_ERR_EXIT(dnx_multicast_entry_set(unit, prev_entry_id, &entry_val));
        }
        else
        {
            /*
             * previous entry next_pointer set to next entry
             */
            SHR_IF_ERR_EXIT(dnx_multicast_entry_next_ptr_set(unit, prev_entry_id, next_entry_id));
        }
        /*
         * next entry prev_pointer set to next entry
         */
        if (next_entry_id != DNX_MULTICAST_END_OF_LIST_PTR)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_entry_prev_ptr_set(unit, next_entry_id, prev_entry_id));
        }

        /*
         * free SW data base
         */
        SHR_IF_ERR_EXIT(multicast_db.mcdb.free_single(unit, entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_multicast_utils_ext_mode_get(
    int unit,
    int *ext_mode)
{
    int nof_fap_ids_per_core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_stk_sys_nof_fap_ids_per_core_get(unit, &nof_fap_ids_per_core));
    *ext_mode = (nof_fap_ids_per_core > 1);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_multicast_utils_egr_invalid_destination_get(
    int unit,
    uint32 *egr_invalid_destination)
{
    int ext_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));

    if (ext_mode)
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                              DBAL_TABLE_MCDB, DBAL_FIELD_EGRESS_DESTINATION_EXT,
                                                              0, DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, egr_invalid_destination));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                              DBAL_TABLE_MCDB, DBAL_FIELD_EGRESS_DESTINATION,
                                                              0, DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, egr_invalid_destination));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * obtain gport from multicast destination
 *
 * \param [in] unit - Unit ID
 * \param [in] core_id - core ID
 * \param [in] is_egress - is destination egress
 * \param [in] destination - dbal representation of multicast destination
 * \param [out] gport - gport represensation of the destination
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_gport_from_encoded_destination_field(
    int unit,
    bcm_core_t core_id,
    int is_egress,
    uint32 destination,
    bcm_gport_t * gport)
{
    dbal_fields_e dbal_dest_type;
    uint32 dest_val = 0;
    bcm_port_t port;
    bcm_trunk_t trunk_id;

    dbal_fields_e egress_destination_field[] = { DBAL_FIELD_EGRESS_DESTINATION, DBAL_FIELD_EGRESS_DESTINATION_EXT };
    dbal_fields_e dsp_agg_field[] = { DBAL_FIELD_DSP_NOEXT_AGG, DBAL_FIELD_DSP_EXT_AGG };

    int ext_mode;
    uint32 egr_invalid_destination;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));

    SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));

    if (is_egress)
    {
        if (destination == egr_invalid_destination)
        {
            *gport = BCM_GPORT_INVALID;
        }
        else
        {
            /*
             * Decode egress destination -- aggregation port or TM port
             */
            SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, egress_destination_field[ext_mode], destination,
                                                                  &dbal_dest_type, &dest_val));

            if (dbal_dest_type == dsp_agg_field[ext_mode])
            {

                /** For egress trunks are represented by egress trunk profiles.
                 * Get the corresponding trunk ID from the egress trunk profile decoded from HW */
                SHR_IF_ERR_EXIT(dnx_trunk_containing_egress_trunk_get(unit, dest_val, &trunk_id));
                BCM_GPORT_TRUNK_SET(*gport, trunk_id);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core_id, dest_val, &port));
                BCM_GPORT_LOCAL_SET(*gport, port);
            }
        }
    }
    else
    {
        /*
         * Ingress
         */

        if (destination == DNX_MULTICAST_ING_INVALID_DESTINATION(unit))
        {
            *gport = BCM_GPORT_INVALID;
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit,
                                                                          ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE,
                                                                          destination, gport));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - given MCDB entry returns the destination gport & CUD information according to format
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] entry_val - structure filled with the entry values
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [out] dest - gport representing the first destination
 *   \param [out] dest_2 - gport representing the second destination
 *   \param [out] cud - encaps id of the replication
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
dnx_multicast_convert_hw_to_gport(
    int unit,
    dnx_multicast_mcdb_dbal_field_t entry_val,
    uint32 core_id,
    bcm_gport_t * dest,
    bcm_gport_t * dest_2,
    bcm_if_t * cud)
{
    dnx_multicast_group_region_t region;
    uint32 id = 0;

    SHR_FUNC_INIT_VARS(unit);

    *cud = (entry_val.cud == DNX_MULTICAST_INVALID_CUD) ? BCM_IF_INVALID : entry_val.cud;
    switch (entry_val.format)
    {
        case DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT:
            SHR_IF_ERR_EXIT(dnx_multicast_gport_from_encoded_destination_field(unit, core_id, FALSE /** is_egress */ ,
                                                                               entry_val.dest[0], dest));
            break;
        case DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT:
        case DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT:
            SHR_IF_ERR_EXIT(dnx_multicast_gport_from_encoded_destination_field(unit, core_id, TRUE /** is_egress */ ,
                                                                               entry_val.dest[0], dest));

            if (entry_val.format == DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT)
            {
                SHR_IF_ERR_EXIT(dnx_multicast_gport_from_encoded_destination_field(unit, core_id,
                                                                                   TRUE /** is_egress */ ,
                                                                                   entry_val.dest[1], dest_2));
            }

            break;
        case DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT:
            SHR_IF_ERR_EXIT(dnx_multicast_entry_to_id(unit, entry_val.bmp_ptr, &id, &region));
            if (region == DNX_MULTICAST_GROUP_REGION_BITMAP_INGRESS ||
                region == DNX_MULTICAST_GROUP_REGION_BITMAP_EG_CORE0 ||
                region == DNX_MULTICAST_GROUP_REGION_BITMAP_EG_CORE1)
            {
                BCM_MCAST_GPORT_BITMAP_ID_SET(*dest, id);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "bit map pointer (%u) is not at bitmap range", entry_val.bmp_ptr);
            }
            break;
        case DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT:
            SHR_IF_ERR_EXIT(dnx_multicast_entry_to_id(unit, entry_val.link_list_ptr, &id, &region));
            if (region == DNX_MULTICAST_GROUP_REGION_INGRESS ||
                region == DNX_MULTICAST_GROUP_REGION_EG_CORE0 || region == DNX_MULTICAST_GROUP_REGION_EG_CORE1)
            {
                *cud = BCM_IF_INVALID;
                BCM_MCAST_GPORT_GROUP_ID_SET(*dest, id);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "link list pointer (%u) is not at group id range", entry_val.link_list_ptr);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_CONFIG, "unexpected format %u found", entry_val.format);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - convert gport to MCDB HW values. According to the given gport, the entry values and core information
 *          are returned.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [out] dest - gport representing the first destination
 *   \param [out] encap - encaps id of the replication
 *   \param [in] flags - specifies whether group id is ingress/egress and also whether the id is bitmap/group id
 *   \param [in] core_id - core_id requested
 *   \param [in] entry_val - structure filled with the entry values by this function
 *   \param [out] is_belong_to_core - whether the replication belongs to specified core_id
 *   \param [in] trunk_action - indicating the action to be taken in regards of egress trunk profile
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
dnx_multicast_convert_gport_to_hw(
    int unit,
    bcm_gport_t dest,
    bcm_if_t encap,
    uint32 flags,
    uint32 core_id,
    dnx_multicast_mcdb_dbal_field_t * entry_val,
    uint32 *is_belong_to_core,
    dnx_multicast_trunk_action_t trunk_action)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int egress_trunk_id;
    uint32 is_open = 0;
    bcm_multicast_t group_id;
    uint32 egr_invalid_destination;
    int ext_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));
    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));

    entry_val->dest[1] = egr_invalid_destination;
    entry_val->cud = (encap == BCM_IF_INVALID) ? DNX_MULTICAST_INVALID_CUD : encap;

    if (dest == BCM_GPORT_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "invalid gport (0x%x)", dest);
    }

    /**
     * ingress is valid for the first core only
     */
    if (core_id && DNX_MULTICAST_IS_INGRESS(flags))
    {
        *is_belong_to_core = FALSE;
    }
    else if (BCM_MCAST_GPORT_IS_GROUP_ID(dest))
    {
        group_id = BCM_MCAST_GPORT_GROUP_ID_GET(dest);
        SHR_IF_ERR_EXIT(dnx_multicast_group_open_get(unit, group_id, flags, &is_open));
        if (!is_open)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Multicast ID that is added as replication is not created!\n");
        }

        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, BCM_MCAST_GPORT_GROUP_ID_GET(dest), FALSE,
                                                  DNX_MULTICAST_IS_INGRESS(flags), core_id, &entry_val->link_list_ptr));
        entry_val->format = DBAL_RESULT_TYPE_MCDB_LINK_LIST_FORMAT;
        *is_belong_to_core = TRUE;
    }
    else if (BCM_MCAST_GPORT_IS_BITMAP_ID(dest))
    {
        SHR_IF_ERR_EXIT(dnx_multicast_id_to_entry(unit, BCM_MCAST_GPORT_BITMAP_ID_GET(dest), TRUE,
                                                  DNX_MULTICAST_IS_INGRESS(flags), core_id, &entry_val->bmp_ptr));
        entry_val->format = DBAL_RESULT_TYPE_MCDB_BITMAP_PTR_FORMAT;
        *is_belong_to_core = TRUE;
    }
    else if (DNX_MULTICAST_IS_INGRESS(flags))
    {
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, dest, &entry_val->dest[0]));
        entry_val->format = DBAL_RESULT_TYPE_MCDB_SINGLE_FORMAT;
        *is_belong_to_core = TRUE;
    }
    else
    {
        /*
         * DNX_MULTICAST_IS_EGRESS 
         */
        if (BCM_GPORT_IS_TRUNK(dest))
        {
            /** In case of TRUNK belong to core is 1 */
            *is_belong_to_core = TRUE;

            /** When adding a TRUNK to an egress MC group, a trunk egress profile must be allocated. */
            if (trunk_action == DNX_MULTICAST_TRUNK_ACTION_CREATE)
            {
                /** Allocate egress profile for trunk */
                SHR_IF_ERR_EXIT(dnx_trunk_egress_create(unit, BCM_GPORT_TRUNK_GET(dest), &egress_trunk_id));
            }
        }
        else
        {
            /** Get core using local port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, dest, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

            if (gport_info.internal_port_pp_info.core_id[0] == core_id)
            {
                *is_belong_to_core = TRUE;
            }
            else
            {
                *is_belong_to_core = FALSE;
            }
        }

        /** When called from dnx_multicast_nof_reps_per_core profile is not created yet and destination is not relevant */
        if ((trunk_action != DNX_MULTICAST_TRUNK_ACTION_INVALID) || (!(BCM_GPORT_IS_TRUNK(dest))))
        {
            SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                            (unit, ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST, dest, &entry_val->dest[0]));
            entry_val->format =
                (ext_mode ? DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT : DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - returns the number of replications per core
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the group/bitmap replications array
 *   \param [in] nof_reps_per_core - pointer to number of replications
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
dnx_multicast_nof_egress_reps_per_core_get(
    int unit,
    uint32 core_id,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    uint32 *nof_reps_per_core)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 rep_id, is_belong_to_core;
    SHR_FUNC_INIT_VARS(unit);

    *nof_reps_per_core = 0;
    for (rep_id = 0; rep_id < rep_count_in; rep_id++)
    {
        SHR_IF_ERR_EXIT(dnx_multicast_convert_gport_to_hw(unit, rep_array[rep_id].port, rep_array[rep_id].encap1,
                                                          BCM_MULTICAST_EGRESS_GROUP, core_id, &entry_val,
                                                          &is_belong_to_core, DNX_MULTICAST_TRUNK_ACTION_INVALID));
        if (is_belong_to_core)
        {
            (*nof_reps_per_core)++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - converts replication array to bitmap. for egress a bitmap per core is returned
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the group/bitmap replications array
 *   \param [in] bmp - pointer to an array which hold the bitmap for both cores
 *   \param [in] bmp_cud - an array which holds the cud for each cores
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * bmp is of size DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit)
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_bitmap_reps_to_bmp(
    int unit,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    uint32 *bmp[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES],
    uint32 bmp_cud[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES])
{
    uint32 rep_id, core_id, is_belong_to_core;
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 is_first_cud[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { TRUE, TRUE };
    SHR_FUNC_INIT_VARS(unit);

    for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
    {
        SHR_BITCLR_RANGE(bmp[core_id], 0, DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit));
        bmp_cud[core_id] = DNX_MULTICAST_INVALID_CUD;
    }

    for (rep_id = 0; rep_id < rep_count_in; rep_id++)
    {
        for (core_id = 0; core_id < dnx_data_device.general.nof_cores_get(unit); core_id++)
        {
            SHR_IF_ERR_EXIT(dnx_multicast_convert_gport_to_hw(unit, rep_array[rep_id].port, rep_array[rep_id].encap1,
                                                              0, core_id, &entry_val, &is_belong_to_core,
                                                              DNX_MULTICAST_TRUNK_ACTION_INVALID));
            if (is_belong_to_core == FALSE)
            {
                continue;
            }
            /**
             * gport validation
             */
            if (entry_val.format != DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT
                && entry_val.format != DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "incorrect gport for bitmap (%x)", rep_array[rep_id].port);
            }

            if (is_first_cud[core_id])
            {
                is_first_cud[core_id] = FALSE;
                bmp_cud[core_id] = entry_val.cud;
            }
            /**
             * cud validation
             */
            if (bmp_cud[core_id] != entry_val.cud)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "core replications with different cuds can't be used"
                             "for bitmap group. cud1 (%u), cud2 (%u)", bmp_cud[core_id], entry_val.cud);
            }
            SHR_BITSET(bmp[core_id], entry_val.dest[0]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - given a bitmap and core_id replication array returned.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -  Unit-ID
 *   \param [in] cud - encaps id of the bitmap replications
 *   \param [in] core_id - device core_id relevant for egress
 *   \param [in] bmp - array which hold the bitmap for core_id
 *   \param [in] rep_count_in - specifies the rep_array size
 *   \param [in] rep_array - pointer to the the group/bitmap replications array returned by this function
 *   \param [in] rep_count_out - pointer to number of replications returned by this function
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * bmp is of size DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit)
 * \see
 *   * None
 */
shr_error_e
dnx_multicast_bitmap_bmp_to_reps(
    int unit,
    uint32 cud,
    uint32 core_id,
    uint32 *bmp,
    int rep_count_in,
    bcm_multicast_replication_t * rep_array,
    int *rep_count_out)
{
    dnx_multicast_mcdb_dbal_field_t entry_val;
    uint32 bit_id;
    bcm_gport_t second_dest;

    int ext_mode;
    uint32 egr_invalid_destination;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));

    SHR_IF_ERR_EXIT(dnx_multicast_utils_egr_invalid_destination_get(unit, &egr_invalid_destination));

    *rep_count_out = 0;
    SHR_BIT_ITER(bmp, DNX_MULTICAST_BMP_NOF_REPLICATIONS_PER_DEV(unit), bit_id)
    {
        if (*rep_count_out >= rep_count_in)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "There are not enough buffers (%u) for replications\n", rep_count_in);
        }
        entry_val.dest[0] = bit_id;
        entry_val.dest[1] = egr_invalid_destination;
        entry_val.cud = cud;
        entry_val.format = (ext_mode ? DBAL_RESULT_TYPE_MCDB_SINGLE_EXT_FORMAT : DBAL_RESULT_TYPE_MCDB_DOUBLE_FORMAT);

        SHR_IF_ERR_EXIT(dnx_multicast_convert_hw_to_gport
                        (unit, entry_val, core_id, &rep_array[*rep_count_out].port, &second_dest,
                         &rep_array[*rep_count_out].encap1));
        (*rep_count_out)++;
    }

exit:
    SHR_FUNC_EXIT;
}
