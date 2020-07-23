/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
 */
/*
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/vlan/algo_vlan.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_vlan.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * MACROS    *
 */
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
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */
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
    int ing_table_capacity, eg_table_capacity;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init Ingress resource pool
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_mngr_nof_inlifs_get(unit, NULL, &ing_table_capacity));

    sal_memset(&ing_res_data, 0, sizeof(dnx_algo_res_create_data_t));
    ing_res_data.first_element = 0;
    ing_res_data.nof_elements = ing_table_capacity;
    ing_res_data.flags = 0;
    sal_strncpy(ing_res_data.name, DNX_VLAN_PORT_INGRESS_VIRTUAL_GPORT_ID_RESOURCE_NAME,
                DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    SHR_IF_ERR_EXIT(vlan_db.vlan_port_ingress_virtual_gport_id.create(unit, &ing_res_data, NULL));

    /*
     * Init Egress resource pool
     */
    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_ESEM, &eg_table_capacity));

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
    data.first_element = 1;
    /*
     * Element 0 cannot be used, vlan 0 is not a valid value - by protocol, nof is minus 1
     */
    data.nof_elements = nof_vsis - 1;
    data.flags = 0;
    sal_strncpy(data.name, DNX_ALGO_VLAN_VSI_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
    /*
     * data.desc = "VSI - L2 Forwarding Domain allocated IDs";
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.create(unit, &data, NULL));

    /*
     * ACTION_ID resource management - Allocate Ingress IDs
     */
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
/*
 * }
 */
