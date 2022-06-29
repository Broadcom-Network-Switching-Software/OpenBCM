/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_vlan.c
 *
 *  VLAN algorithms initialization and deinitialization.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/vlan/algo_vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/vlan/vlan.h>

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
#define DNX_VLAN_RANGE_PROFILE_FIRST_PROFILE_ID        (0)
#define DNX_VLAN_RANGE_PROFILE_MAX_REFERENCE           (512)

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

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
 * }
 */

/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

extern shr_error_e dbal_tables_mdb_hw_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size);
/**
 * See vlan.h
 */
shr_error_e
dnx_vlan_range_template_init(
    int unit)
{
    vlan_range_profile_template_t default_profile;
    dnx_algo_template_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&data, 0, sizeof(data));
    sal_memset(&default_profile, VLAN_RANGE_DEFAULT, sizeof(default_profile));

    /** Set data for template create */
    data.first_profile = DNX_VLAN_RANGE_PROFILE_FIRST_PROFILE_ID;
    data.nof_profiles = dnx_data_vlan.vlan_translate.nof_vlan_range_entries_get(unit);
    data.max_references = DNX_VLAN_RANGE_PROFILE_MAX_REFERENCE;
    data.data_size = sizeof(vlan_range_profile_template_t);
    data.default_data = &default_profile;
    data.default_profile = DNX_VLAN_RANGE_PROFILE_FIRST_PROFILE_ID;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(data.name, DNX_ALGO_VLAN_RANGE_TEMPLATE_INNER, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(vlan_db.vlan_range_inner_profile.create(unit, &data, NULL));

    sal_memset(&data, 0, sizeof(data));
    data.first_profile = DNX_VLAN_RANGE_PROFILE_FIRST_PROFILE_ID;
    data.nof_profiles = dnx_data_vlan.vlan_translate.nof_vlan_range_entries_get(unit);
    data.max_references = DNX_VLAN_RANGE_PROFILE_MAX_REFERENCE;
    data.data_size = sizeof(vlan_range_profile_template_t);
    data.default_data = &default_profile;
    data.default_profile = DNX_VLAN_RANGE_PROFILE_FIRST_PROFILE_ID;
    data.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(data.name, DNX_ALGO_VLAN_RANGE_TEMPLATE_OUTER, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(vlan_db.vlan_range_outer_profile.create(unit, &data, NULL));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize the resources used for managing virtual vlan port IDs
 * \param [in] unit - Unit #
 * \return shr_error_e Standard error
 */
static shr_error_e
dnx_algo_vlan_port_virtual_id_res_init(
    int unit)
{
    dnx_algo_res_create_data_t ing_res_data;
    dnx_algo_res_create_data_t egr_res_data;
    int ing_table_capacity, eg_table_capacity = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init Ingress resource pool
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_max_inlif_id_get(unit, NULL, &ing_table_capacity));

    if (!(dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications)))
    {
        sal_memset(&ing_res_data, 0, sizeof(dnx_algo_res_create_data_t));
        ing_res_data.first_element = 0;
        ing_res_data.nof_elements = ing_table_capacity;
        ing_res_data.flags = 0;
        sal_strncpy(ing_res_data.name, DNX_VLAN_PORT_INGRESS_VIRTUAL_GPORT_ID_RESOURCE_NAME,
                    DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        SHR_IF_ERR_EXIT(vlan_db.vlan_port_ingress_virtual_gport_id.create(unit, &ing_res_data, NULL));
    }

    /*
     * Init Egress resource pool
     */
    SHR_IF_ERR_EXIT(dnx_algo_vlan_esem_size_get(unit, &eg_table_capacity));

    sal_memset(&egr_res_data, 0, sizeof(dnx_algo_res_create_data_t));
    egr_res_data.first_element = ing_table_capacity;
    egr_res_data.nof_elements = eg_table_capacity;
    egr_res_data.flags = 0;
    sal_strncpy(egr_res_data.name, DNX_VLAN_PORT_EGRESS_VIRTUAL_GPORT_ID_RESOURCE_NAME,
                DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_egress_virtual_gport_id.create(unit, &egr_res_data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Destroy the resources used for managing virtual vlan port IDs
 * \param [in] unit - Unit #
 * \return shr_error_e Standard error
 */
static shr_error_e
dnx_algo_vlan_port_virtual_id_res_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sw state resources will be deinit centralize.
     */
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * See algo_vlan.h
 */
shr_error_e
dnx_algo_vlan_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init alloc manager for virtual vlan-port IDs
     */
    SHR_IF_ERR_EXIT(dnx_algo_vlan_port_virtual_id_res_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   deallocate resource pool for all VLAN profiles types
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval Negative in case of an error.
 *   \retval Zero in case of NO ERROR
 */
shr_error_e
dnx_algo_vlan_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * De-init alloc manager for virtual vlan-port IDs
     */
    SHR_IF_ERR_EXIT(dnx_algo_vlan_port_virtual_id_res_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_vlan_res_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    uint8 is_init;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    /*
     * Init vlan sw state
     */
    SHR_IF_ERR_EXIT(vlan_db.is_init(unit, &is_init));
    if (!is_init)
    {
        SHR_IF_ERR_EXIT(vlan_db.init(unit));
    }

    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    /*
     * VSI resource management - Allocate
     */
    data.first_element = 0;
    /*
     * Element 0 cannot be used, vlan 0 is not a valid value - by protocol.
     * However the first element is 0 as it can be allocated only at init.
     * An advanced algorithm is used to allow allocating VSI 0 only at init.
     */
    data.nof_elements = nof_vsis;
    data.flags = DNX_ALGO_RES_CREATE_USE_ADVANCED_ALGORITHM;
    data.advanced_algorithm = DNX_ALGO_RESOURCE_ADVANCED_ALGORITHM_VSI;
    sal_strncpy(data.name, DNX_ALGO_VLAN_VSI_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    /*
     * data.desc = "VSI - L2 Forwarding Domain allocated IDs";
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.create(unit, &data, NULL));

    /*
     * ACTION_ID resource management - Allocate Ingress IDs
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_vlan.vlan_translate.nof_ingress_vlan_edit_action_ids_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_VLAN_TRANSLATE_ACTION_ID_INGRESS, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * data.desc = "ACTION_ID - VLAN translation command ID";
     */
    SHR_IF_ERR_EXIT(vlan_db.action_id_ingress.create(unit, &data, NULL));

    /*
     * ACTION_ID resource management - Allocate Egress IDs
     */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));
    data.first_element = 0;
    data.nof_elements = dnx_data_vlan.vlan_translate.nof_egress_vlan_edit_action_ids_get(unit);
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_VLAN_TRANSLATE_ACTION_ID_EGRESS, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /*
     * data.desc = "ACTION_ID - ID number for VLAN edit action";
     */
    SHR_IF_ERR_EXIT(vlan_db.action_id_egress.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

void
dnx_vlan_range_template_print(
    int unit,
    const void *data)
{
    int i;
    vlan_range_profile_template_t *template_data;
    template_data = (vlan_range_profile_template_t *) data;
    DNX_SW_STATE_PRINT(unit, "keys ={[vlan range id]}, data = {vlan range low, vlan range high}\n");
    for (i = 0; i < dnx_data_vlan.vlan_translate.nof_vlan_range_entries_get(unit); i++)
    {
        DNX_SW_STATE_PRINT(unit, "%s[%2d][%4d,%4d]", i % 8 ? "" : "\n",
                           i, template_data->vlan_range[i].lower, template_data->vlan_range[i].upper);
    }
}
/*
* See header file algo_vlan.h for description.
*/
void
dnx_vlan_range_template_inner_print_cb(
    int unit,
    const void *data)
{
    dnx_vlan_range_template_print(unit, data);
}
/*
* See header file algo_vlan.h for description.
*/
void
dnx_vlan_range_template_outer_print_cb(
    int unit,
    const void *data)
{
    dnx_vlan_range_template_print(unit, data);
}
/**
 * Refer to the algo_vlan.h file for more information
 */
shr_error_e
dnx_algo_vlan_vsi_res_mngr_allocate(
    int unit,
    uint32 module_id,
    void *res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element)
{
    int nof_vsis;
    int is_init_done;
    uint32 range_start;
    uint32 range_end;
    resource_tag_bitmap_utils_alloc_info_t res_tag_bitmap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(resource_tag_bitmap_utils_alloc_info_t));

    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    range_end = nof_vsis - 1;

    /*
     * Vlan 0 is not a valid value, but it is allowed to be allocated and configured only at init.
     */
    is_init_done = dnxc_init_is_init_done_get(unit);
    if (!is_init_done)
    {
        range_start = 0;
    }
    else
    {
        range_start = 1;
    }

    /** Set range_start and range_end */
    res_tag_bitmap_alloc_info.range_start = range_start;
    res_tag_bitmap_alloc_info.range_end = range_end;
    res_tag_bitmap_alloc_info.count = 1;
    res_tag_bitmap_alloc_info.flags = flags;
    SHR_IF_ERR_EXIT(resource_tag_bitmap_alloc
                    (unit, module_id, (resource_tag_bitmap_t) res_tag_bitmap, &res_tag_bitmap_alloc_info, element));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_vlan_esem_size_get(
    int unit,
    int *esem_size)
{
    int eg_table_capacity = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_ESEM, &eg_table_capacity));

    *esem_size = eg_table_capacity;
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
