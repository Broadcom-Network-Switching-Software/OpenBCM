/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file flow_virtual_lif_mgmt.c
 *
 *  The Virtual LIF Management provides a general mechanism
 *   to address the complexity of ingress and egress virtual LIFs
 * 
 *   Virtual LIF - an object that represents a LIF (can be local LIF) that doesn?t have a global LIF in the system,
 *   instead it has a virtual ID. The virtual ID is exposed to the customer in order to manage the entry.
 *       There are four types of Virtual LIFs:
 *               Ingress lif (used for intermediate lif) - in this case no global lif is created.
 *               Egress lif(used for egress pointed)
 *               Esem match key pointing to lif attributes (no local lif)
 *               Esem default (part of egress lif) index pointing to lif attributes (no local lif)
 *  In those cases, an entry will be created without a Global-LIF and there will be virtual-ID (SW allocated ID) tha will
 *  be returned to the user.
 **/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/flow/flow_lif_mgmt.h>
#include <bcm_int/dnx/flow/flow_virtual_lif_mgmt.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_flow_lif_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_table_mngr_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_port_pp_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */
extern shr_error_e dbal_entry_handle_get_internal(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_handle_t ** entry_handle);

extern shr_error_e dbal_entry_key_as_buffer_set(
    int unit,
    uint32 entry_handle_id,
    uint32 *key_buffer,
    int size_in_bits);

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */
/*
 * Declaration of extern functions
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/

/*
 * Allocate a Flow virtual value (with no encoding), encode it to a gport and store as flow_id
 */
shr_error_e
dnx_flow_egress_virtual_gport_allocate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info,
    int is_local_lif)
{
    int virtual_id = 0;
    int virtual_val;
    uint32 sw_handle_alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Handle the WITH_ID case similarly for all Virtual gports */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
    {
        sw_handle_alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        virtual_val = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id);
    }

    if (!is_local_lif)
    {
        if (FLOW_LIF_DBAL_TABLE_GET(flow_app_info) == DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE)
        {
            SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.allocate_single(unit,
                                                                                        sw_handle_alloc_flags,
                                                                                        NULL, &virtual_val));

            /** Encode the virtual_val as a flow virtual gport with a subtype */
            BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_SET(virtual_id, virtual_val);
        }
        else
        {
            SHR_IF_ERR_EXIT(algo_flow_lif.flow_egress_esem_virtual_gport_id.allocate_single(unit,
                                                                                            sw_handle_alloc_flags,
                                                                                            NULL, &virtual_val));

            /** Encode the virtual_val as a flow virtual gport with a subtype */
            BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_SET(virtual_id, virtual_val);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_flow_lif.
                        flow_egress_virtual_gport_id.allocate_single(unit, sw_handle_alloc_flags, NULL, &virtual_val));
        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(virtual_id, virtual_val);
    }

    /** Set the virtual id as a gport flow_id */
    BCM_GPORT_FLOW_LIF_ID_SET(flow_handle_info->flow_id, virtual_id);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Free an allocated Flow virtual gport 
 */
shr_error_e
dnx_flow_egress_virtual_gport_free(
    int unit,
    bcm_gport_t virtual_lif_gport)
{
    uint32 gport_subtype, virtual_val;

    SHR_FUNC_INIT_VARS(unit);

    gport_subtype = BCM_GPORT_SUB_TYPE_GET(virtual_lif_gport);
    virtual_val = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(virtual_lif_gport);

    switch (gport_subtype)
    {
        case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH:
            SHR_IF_ERR_EXIT(algo_flow_lif.flow_egress_esem_virtual_gport_id.free_single(unit, virtual_val, NULL));
            break;
        case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT:
            SHR_IF_ERR_EXIT(algo_port_pp_db.esem_default_result_profile.free_single(unit, virtual_val, NULL));
            break;
        case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED:
        {
            /** Release (deallocate) this 'egr_pointed' object. */
            SHR_IF_ERR_EXIT(algo_flow_lif.flow_egress_virtual_gport_id.free_single(unit, virtual_val, NULL));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Unsupported gport sub-type %d for virtual gport 0x%x\n", gport_subtype, virtual_lif_gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  Add Virtual match-info per gport to SW state.
 *  Currently applicable for ESEM entries.
 *  See .h file
 */
shr_error_e
dnx_flow_egress_virtual_sw_info_add(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 entry_handle_id,
    uint32 result_type,
    dnx_flow_app_config_t * flow_app_info)
{
    uint32 sw_entry_handle_id = 0;
    dbal_entry_handle_t *hw_entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &hw_entry_handle));


    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, result_type);
    dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE,
                                 FLOW_LIF_DBAL_TABLE_GET(flow_app_info));


    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_egress_virtual_sw_info_get(
    int unit,
    bcm_gport_t virtual_lif_gport,
    dbal_tables_e * table_id,
    uint32 *result_type)
{
    uint32 sw_entry_handle_id = 0, dbal_table_as_uint32;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set the gport key */
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Retrieve the mapping entry from the SW DBAL */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Retrieve the result type from the DBAL handler */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, result_type));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &dbal_table_as_uint32));

    (*table_id) = dbal_table_as_uint32;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** update the generic SW info, in case the value for the result type/ dbal table is -1, update is not needed */
shr_error_e
dnx_flow_egress_virtual_sw_info_update(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 table_id,
    uint32 result_type)
{
    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

        /** Set the gport key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Set the modified result type */
    if (result_type != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE, result_type);
    }

    if (table_id != -1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, table_id);
    }

    /** Insert the mapping entry to the SW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** sets on the entry handle the match key fields related to the specific gport (from SW state) */
shr_error_e
dnx_flow_egress_virtual_key_fields_set(
    int unit,
    bcm_gport_t virtual_lif_gport,
    uint32 entry_handle_id)
{
    uint32 sw_entry_handle_id = 0;
    uint32 match_key[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    int size_in_bits = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    /** Set the gport key */
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Retrieve the mapping entry from DBAL and get the result-type */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** setting the HW entry Key */
    SHR_IF_ERR_EXIT(dbal_entry_key_as_buffer_set(unit, entry_handle_id, match_key, size_in_bits));

    if (dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_esem_is_dpc))
    {
        dbal_tables_e dbal_table_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
        if (dbal_table_id != DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_egress_virtual_match_clear(
    int unit,
    bcm_gport_t virtual_lif_gport)
{
    uint32 sw_entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Set the virtual gport as the key for the mapping table */
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_lif_gport);

    /** Clear the entry from the SW DBAL mapping table */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_egr_pointed_virtual_mapping_add(
    int unit,
    uint32 virtual_id,
    int local_lif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     *  EGR_POINTED to local
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_POINTED_TO_EGRESS_LIF_MAP_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, virtual_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE, local_lif);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     *  Local to EGR_POINTED
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_LIF_TO_EGRESS_POINTED_MAP_SW, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, INST_SINGLE, virtual_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_egr_pointed_virtual_mapping_remove(
    int unit,
    uint32 virtual_id)
{
    uint32 entry_handle_id;
    int local_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     *  EGR_POINTED to local
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_POINTED_TO_EGRESS_LIF_MAP_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, virtual_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE,
                                                        (uint32 *) (&local_lif)));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    /*
     *  Local to EGR_POINTED
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_LIF_TO_EGRESS_POINTED_MAP_SW, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_egr_pointed_virtual_gport_get(
    int unit,
    int local_lif,
    uint32 *virtual_id)
{
    uint32 entry_handle_id;
    shr_error_e shr_error;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get EGR_POINTED from the map.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_LIF_TO_EGRESS_POINTED_MAP_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif);
    shr_error = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (shr_error != _SHR_E_NONE)
    {
        /*
         * If dbal_entry_get() returned with an error which is NOT_FOUND then just
         * roll this error to the caller but do not log any error message. Caller
         * is responsible for further handling.
         * Otherwise, apply standard error handling.
         */
        if (shr_error == _SHR_E_NOT_FOUND)
        {
            *virtual_id = DNX_FLOW_VIRTUAL_ID_INVALID;
            SHR_SET_CURRENT_ERR(shr_error);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(shr_error);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, INST_SINGLE, virtual_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_flow_egr_pointed_virtual_local_lif_get(
    int unit,
    uint32 virtual_id,
    int *local_lif_p)
{
    uint32 entry_handle_id;
    shr_error_e shr_error;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Check and analyze input. 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_POINTED_TO_EGRESS_LIF_MAP_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VIRTUAL_EGRESS_POINTED, virtual_id);
    shr_error = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (shr_error != _SHR_E_NONE)
    {
        /*
         * If dbal_entry_get() returned with an error which is NOT_FOUND then just
         * roll this error to the caller but do not log any error message. Caller
         * is responsible for further handling.
         * Otherwise, apply standard error handling.
         */
        if (shr_error == _SHR_E_NOT_FOUND)
        {
            *local_lif_p = DNX_FLOW_VIRTUAL_ID_INVALID;
            SHR_SET_CURRENT_ERR(shr_error);
            SHR_EXIT();
        }
        else
        {
            SHR_IF_ERR_EXIT(shr_error);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE,
                                                        (uint32 *) (local_lif_p)));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Allocate a Flow virtual value (with no encoding), encode it to a gport and store as flow_id
 */
shr_error_e
dnx_flow_ingress_virtual_gport_allocate(
    int unit,
    bcm_flow_handle_info_t * flow_handle_info,
    dnx_flow_app_config_t * flow_app_info)
{
    int virtual_id = 0;
    int virtual_val;
    uint32 sw_handle_alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Handle the WITH_ID case similarly for all Virtual gports */
    if (_SHR_IS_FLAG_SET(flow_handle_info->flags, BCM_FLOW_HANDLE_INFO_WITH_ID))
    {
        sw_handle_alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        virtual_val = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_handle_info->flow_id);
    }

    SHR_IF_ERR_EXIT(algo_flow_lif.flow_ingress_virtual_gport_id.allocate_single(unit, sw_handle_alloc_flags, NULL,
                                                                                &virtual_val));

    /** Encode the virtual_val as a flow virtual gport with a subtype */
    BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_SET(virtual_id, virtual_val);

    /** Set the virtual id as a gport flow_id */
    BCM_GPORT_FLOW_LIF_ID_SET(flow_handle_info->flow_id, virtual_id);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Free an allocated Flow virtual gport 
 */
shr_error_e
dnx_flow_ingress_virtual_gport_free(
    int unit,
    bcm_gport_t virtual_lif_gport)
{
    uint32 virtual_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    virtual_val = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(virtual_lif_gport);
    SHR_IF_ERR_EXIT(algo_flow_lif.flow_ingress_virtual_gport_id.free_single(unit, virtual_val, NULL));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 *  Add a terminator mapping from a Virtual gport to a local In-LIF
 *  
 */

shr_error_e
dnx_flow_ingress_virtual_mapping_add(
    int unit,
    bcm_gport_t virtual_id,
    int nof_in_lifs,
    int *local_lifs)
{
    uint32 sw_entry_handle_id;
    int in_lif_idx;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take the SW table handle and set the vitrual_id key */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &sw_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_id);

    /** Set the local In-LIFs payload (the local_lifs array is set with LIF_MNGR_INLIF_INVALID for unused cores */
    for (in_lif_idx = 0; in_lif_idx < nof_in_lifs; in_lif_idx++)
    {
        dbal_entry_value_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, in_lif_idx,
                                     local_lifs[in_lif_idx]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 *  Remove a terminator mapping from a Virtual gport to a local In-LIF
 */
shr_error_e
dnx_flow_ingress_virtual_mapping_clear(
    int unit,
    bcm_gport_t virtual_id)
{
    uint32 sw_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take the SW table handle and set the vitrual_id key */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &sw_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 *  Retrieve a terminator Virtual local LIF from a virtual gport and core-id using SW mapping
 *  See .h file
 */
shr_error_e
dnx_flow_ingress_virtual_local_lif_get(
    int unit,
    int virtual_id,
    int core_idx,
    int *local_in_lif)
{
    uint32 sw_entry_handle_id;
    int instance_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take the SW table handle and set the vitrual_id key */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &sw_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_id);

    /** The instance_id represents the core from which to retrieve the local In-LIF for */
    instance_id = (dnx_data_lif.in_lif.feature_get(unit, dnx_data_lif_in_lif_phy_db_dpc)) ? core_idx : 0;

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, instance_id, (uint32 *) local_in_lif));

    if (*local_in_lif == LIF_MNGR_INLIF_INVALID)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 *  Retrieve for a terminator Virtual an array of local LIFs from a specified virtual gport.
 *  This API is intended for use when the local In-LIFs are configured per core.
 *  If the Virtual-ID isn't associated with a core, a value of LIF_MNGR_INLIF_INVALID is expected.
 *  See .h file
 */
shr_error_e
dnx_flow_ingress_virtual_local_lif_get_all(
    int unit,
    int virtual_id,
    inlif_array_t * local_in_lifs)
{
    int nof_cores = dnx_data_device.general.nof_cores_get(unit);
    uint32 sw_entry_handle_id;
    int core_idx, instance_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take the SW table handle and set the vitrual_id key */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &sw_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, virtual_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (core_idx = 0; core_idx < nof_cores; core_idx++)
    {
        instance_id = core_idx;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, sw_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, instance_id,
                         (uint32 *) &(local_in_lifs->local_lifs[core_idx])));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 *  Retrieve a terminator Virtual local gport from a virtual local LIF and core-id using SW mapping
 *  See .h file
 */
shr_error_e
dnx_flow_ingress_virtual_gport_get(
    int unit,
    int local_in_lif,
    int core_idx,
    int *virtual_id)
{
    uint32 sw_entry_handle_id;
    int rule_local_in_lif, is_end, instance_id = 0;
    uint32 key_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take the SW table handle and initialize the DBAL iterator */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &sw_entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, sw_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /** The instance_id represents the core from which to retrieve the local In-LIF for */

    /** Set iterator rules: The local_in_lif needs to be equal to the supplied value */
    rule_local_in_lif = local_in_lif;
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, sw_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, instance_id,
                     DBAL_CONDITION_EQUAL_TO, (uint32 *) &rule_local_in_lif, NULL));

    /** Get the correct entry */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, sw_entry_handle_id, &is_end));
    if (is_end == TRUE)
    {
        /** No entry found - return "not found" */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, sw_entry_handle_id, DBAL_FIELD_GPORT, key_field_val));
    *virtual_id = key_field_val[0];

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 */
