/** \file vswitch_port.c
 *
 *  Vswitch association with gport procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>
#include <bcm/types.h>
#include <bcm/vswitch.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>
#include <bcm/error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <bcm_int/dnx/vsi/vsi.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <bcm_int/dnx/vswitch/vswitch.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>

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

/**
 * \brief - Verify function for bcm_dnx_vswitch_port_add
 */
static shr_error_e
dnx_vswitch_port_add_verify(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t port)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint8 is_allocated = 0;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the local In-LIF from the supplied gport
     * Get local In-LIF using DNX Algo Gport Managment:
     * This will also verify that the lif is legal and has an ingress side.
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));

    /*
     * verify gport is vlan/extender port
     */
    if (!BCM_GPORT_IS_VLAN_PORT(port) && !BCM_GPORT_IS_EXTENDER_PORT(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. gport = 0x%08X, PORT TYPE is not a VLAN port or Extender port LIF. \n", port);
    }

    /*
     * verify vsi is valid
     */
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    if (vsi >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.\n", vsi);
    }

    /*
     * Verify that the VSI is allocated
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "vsi %d doesn't exist\n", vsi);
    }

    /*
     * Verify that the LIF type is the right one and has a VSI field:
     *   - AC symmetric VLAN-PORT
     *   - Native AC
     */
    if (BCM_GPORT_IS_VLAN_PORT(port))
    {
        uint8 has_vsi;
        /*
         * Check that it has a VSI field
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_is_valid_field
                        (unit, _SHR_CORE_ALL, gport_hw_resources.local_in_lif, gport_hw_resources.inlif_dbal_table_id,
                         TRUE, DBAL_FIELD_VSI, &has_vsi));
        if (has_vsi == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Wrong setting! port = 0x%08X is a Ingress Native/symmertic AC LIF but it's type = %d is without VSI field!!\n",
                         port, gport_hw_resources.inlif_dbal_result_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vswitch_port_get
 */
static shr_error_e
dnx_vswitch_port_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t * vsi)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Checking that the vsi pointer is not NULL:
     */
    SHR_NULL_CHECK(vsi, _SHR_E_PARAM, "vsi");

    /*
     * Retrieve the local In-LIF from the supplied gport
     * Get local In-LIF using DNX Algo Gport Managment:
     * This will also verify that the lif is legal and has an ingress side.
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources));
    /*
     * verify gport is vlan port
     */
    if (!BCM_GPORT_IS_VLAN_PORT(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. gport = 0x%08X, PORT TYPE is not a VLAN port LIF. \n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vswitch_port_delete
 */
static shr_error_e
dnx_vswitch_port_delete_verify(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t port)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint8 is_allocated = 0;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the local In-LIF from the supplied gport
     * Get local In-LIF using DNX Algo Gport Managment:
     * This will also verify that the lif is legal and has an ingress side.
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * verify gport is vlan port
     */
    if (!BCM_GPORT_IS_VLAN_PORT(port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. port = 0x%08X, PORT TYPE is not a VLAN port LIF. \n", port);
    }

    /*
     * verify vsi is valid
     */
    SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
    if (vsi >= nof_vsis)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.\n", vsi);
    }

    /*
     * Verify that the VSI is allocated
     */
    SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "vsi %d doesn't exist\n", vsi);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_vswitch_port_delete_all
 */
static shr_error_e
dnx_vswitch_port_delete_all_verify(
    int unit,
    bcm_vlan_t vsi)
{
    uint8 is_allocated;
    int nof_vsis;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify vsi is valid
     * Note: vsi == 0 means "ALL VSIs"
     */
    if (vsi != 0)
    {
        SHR_IF_ERR_EXIT(dnx_vsi_count_get(unit, &nof_vsis));
        if (vsi >= nof_vsis)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Wrong setting. vsi = %d is not a valid vsi.\n", vsi);
        }

        /*
         * Verify that the VSI is allocated
         */
        SHR_IF_ERR_EXIT(vlan_db.vsi.is_allocated(unit, vsi, &is_allocated));

        if (is_allocated == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "vsi %d doesn't exist\n", vsi);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API associated a gport with a VSI by updating the vsi
 * attribute of an In-LIF.
 * HW table DBAL_TABLE_IN_AC_INFO_DB is modified with the following fields:
 *  - DBAL_FIELD_VSI - sets to VSI value
 *
 *  FIXME:
 *  change input from port to lif_type + in_lif_value
 */
static shr_error_e
dnx_vswitch_port_in_lif_table_update_set(
    int unit,
    bcm_vlan_t vsi,
    int learn_enable,
    bcm_gport_t port)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    int is_p2p;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get local In-LIF using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * Update the In-LIF information with the supplied VSI value
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, vsi);

    /*
     * Update the learn enable for applicable LIFs:
     *  - not P2P In-LIFs
     *  - not virtual In-LIFs
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_is_p2p(unit, port, &is_p2p));

    if ((is_p2p == FALSE) && (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(port) == FALSE))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ENABLE, INST_SINGLE, learn_enable);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API gets the VSI which is associated to gport (the vsi attribute of an In-LIF).
 *
 *  FIXME:
 *  change input from port to lif_type + in_lif_value
 */
static shr_error_e
dnx_vswitch_port_in_lif_table_update_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t * vsi)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 tmp32bitVal;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get local In-LIF using DNX Algo Gport Managment:
     */
    SHR_IF_ERR_REPLACE_AND_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                (unit, port, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_resources),
                                _SHR_E_NOT_FOUND, _SHR_E_PARAM);

    /*
     * Get the VSI value from the In-LIF table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, gport_hw_resources.inlif_dbal_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF, gport_hw_resources.local_in_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 gport_hw_resources.inlif_dbal_result_type);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, &tmp32bitVal));
    *vsi = (bcm_vlan_t) tmp32bitVal;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 */
static shr_error_e
dnx_vswitch_port_delete_all_ingress_non_native(
    int unit,
    bcm_vlan_t vsi)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 drop_in_lif;
    uint32 initial_bridge_in_lif;
    uint32 recycle_default_inlif;
    uint32 vsi_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse on Ingress non-native LIFs:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_IN_AC_INFO_DB, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * 1. All the entries (that is all in-lifs) that are not created by default (see dnx_vlan_port_default_init):
     *  - drop LIF
     *  - bridge LIF
     *  - recycle LIF
     * 2. The LIF VSI value is as requested by user
     */
    drop_in_lif = dnx_data_lif.in_lif.drop_in_lif_get(unit);
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &drop_in_lif, NULL));

    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_initial_bridge_lif_get(unit, (int *) &initial_bridge_in_lif));
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &initial_bridge_in_lif,
                     NULL));

    SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_default_recyle_lif_get(unit, &recycle_default_inlif));
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &recycle_default_inlif,
                     NULL));
    /*
     * Note: vsi ==0 means "ALL VSIs"
     */
    if (vsi != 0)
    {
        vsi_value = vsi;
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &vsi_value,
                         NULL));
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 local_in_lif;
        bcm_gport_t gport;

        /*
         * Get local In-LIF -- table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

        /*
         * Get gport from In-LIF
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS, DBAL_CORE_ALL, local_in_lif,
                         &gport));

        /*
         * set back the vsi to default (see DNX_VSI_DEFAULT definition)
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_set(unit, DNX_VSI_DEFAULT, FALSE, gport));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 */
static shr_error_e
dnx_vswitch_port_delete_all_ingress_native(
    int unit,
    bcm_vlan_t vsi)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 vsi_value;
    uint32 vtt5_default_in_lif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the vtt5 native default trap lif value from the virtual register
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DEFAULT_LIF, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VTT5___NATIVE_ETHERNET___TRAP_DEFAULT_LIF, INST_SINGLE,
                               &vtt5_default_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Traverse on Ingress non-native LIFs:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Set iterator rules:
     * 1. The LIF VSI value is as requested by user
     * Note: vsi ==0 means "ALL VSIs"
     * 2. Avoid deletion of the vtt5 native default trap lif
     */
    if (vsi != 0)
    {
        vsi_value = vsi;
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &vsi_value,
                         NULL));
    }

    /*
     * All the entries (that is all in-lifs) that are not created by default (see dnx_vlan_native_vpls_service_tagged_miss_init):
     *  - PWE tagged native miss lif
     */
    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &vtt5_default_in_lif,
                     NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 local_in_lif;
        bcm_gport_t gport;

        /*
         * Get local In-LIF -- table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

        /*
         * Non-virtual native
         * Get gport from In-LIF
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, DBAL_CORE_ALL, local_in_lif,
                         &gport));

        /*
         * set back the vsi to default (see DNX_VSI_DEFAULT definition)
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_set(unit, DNX_VSI_DEFAULT, FALSE, gport));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 */
static shr_error_e
dnx_vswitch_port_delete_all_ingress_native_virtual(
    int unit,
    bcm_vlan_t vsi)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 vsi_value, vtt5_default_in_lif;
    uint32 native_default_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Traverse Ingress native virtual.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Note: vsi ==0 means "ALL VSIs"
     */
    if (vsi != 0)
    {
        vsi_value = vsi;
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_VSI, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &vsi_value,
                         NULL));
    }

    /*
     * Get lif value from virtual register of vtt5 native default 0 tags
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PEMLA_VTT5_DEFAULT_LIF, &native_default_handle_id));
    dbal_value_field32_request(unit, native_default_handle_id,
                               DBAL_FIELD_VTT5___NATIVE_ETHERNET___0_TAGS_DEFAULT_LIF, INST_SINGLE,
                               &vtt5_default_in_lif);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, native_default_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_NOT_EQUAL_TO, &vtt5_default_in_lif,
                     NULL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        uint32 local_in_lif;
        bcm_gport_t gport;

        /*
         * Get local In-LIF -- table key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IN_LIF, &local_in_lif));

        /*
         * Virtual native
         * The local in-lif is "virtual", look for it in the SW DB
         */
        SHR_IF_ERR_EXIT(dnx_vlan_port_find_ingress_native_virtual_match_sw_state(unit, (int) local_in_lif, &gport));
        /*
         * set back the vsi to default (see DNX_VSI_DEFAULT definition)
         */
        SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_set(unit, DNX_VSI_DEFAULT, FALSE, gport));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API associated a gport with a VSI by updating the vsi
 * attribute of an In-LIF.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi -
 *     The VSI that the In-LIF is associated with
 *   \param [in] port -
 *     A gport that represent an In-LIF that as is assciated
 *     with a VSI
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *           example: invalid vsi.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 * HW table DBAL_TABLE_IN_AC_INFO_DB is modified with the following fields:
 *  - DBAL_FIELD_RESULT_TYPE - sets to DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_IN_LIF_FORMAT_AC_MP
 *  - DBAL_FIELD_VSI - sets to VSI value
 * \remark
 * This function also enable learning for MP LIFs.
 */
shr_error_e
bcm_dnx_vswitch_port_add(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t port)
{
    int learn_enable = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_port_add_verify(unit, vsi, port));

    SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_set(unit, vsi, learn_enable, port));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This API gets the VSI which is associated to gport (the vsi attribute of an In-LIF).
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] port -
 *     A gport that represent an In-LIF that as is assciated with a VSI
 *   \param [in] vsi -
 *   Pointer to the VSI (that the In-LIF is associated with)
 * \par INDIRECT INPUT:
 * DBAL_TABLE_IN_AC_INFO_DB
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *           example: invalid port.
 * \par INDIRECT OUTPUT
  *   the VSI value (that the In-LIF is associated with, at table DBAL_TABLE_IN_AC_INFO_DB, see vsi above)
 * \remark
 *   * None
 * \see
 *   * bcm_dnx_vswitch_port_add
 */
shr_error_e
bcm_dnx_vswitch_port_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t * vsi)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_port_get_verify(unit, port, vsi));

    SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_get(unit, port, vsi));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API disassociates a gport from a VSI
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi -
 *     The VSI that the In-LIF is associated with
 *   \param [in] port -
 *     A gport that represent an In-LIF that as is assciated
 *     with a VSI - as should be removed
 * \par INDIRECT INPUT:
 *   * DBAL_TABLE_IN_AC_INFO_DB - see INDIRECT OUTPUT
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *           example: invalid vsi.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 * HW table DBAL_TABLE_IN_AC_INFO_DB is modified with the following fields:
 * DBAL_FIELD_VSI - sets to DNX_VSI_DEFAULT value
 * \see
 *   * bcm_dnx_vswitch_port_add, bcm_dnx_vswitch_port_get
 */

shr_error_e
bcm_dnx_vswitch_port_delete(
    int unit,
    bcm_vlan_t vsi,
    bcm_gport_t port)
{
    bcm_vlan_t vsi_dbal;
    int learn_enable = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_port_delete_verify(unit, vsi, port));

    /*
     * check that the port is indeed associated with the given vsi
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_get(unit, port, &vsi_dbal));

    if (vsi != vsi_dbal)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Wrong setting. gport = 0x%08X is associated with vsi = %d.  Input vsi = %d.\n",
                     port, (bcm_vlan_t) vsi_dbal, vsi);
    }

    /*
     * set back the vsi to default (see DNX_VSI_DEFAULT definition)
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_port_in_lif_table_update_set(unit, DNX_VSI_DEFAULT, learn_enable, port));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This API disassociates all gports that are associated with the VSI.
 * It is done by traversing all In-LIF range and setting the vsi
 * attribute of an associated In-LIF to DNX_VSI_DEFAULT value.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Relevant unit.
 *   \param [in] vsi -
 *     The VSI that the In-LIFs are associated with
 * \par INDIRECT INPUT:
 *    DBAL_TABLE_IN_AC_INFO_DB
 *
 * \par DIRECT OUTPUT:
 *   \retval Negative in case of an error. See shr_error_e, for
 *           example: invalid vsi.
 *   \retval Zero in case of NO ERROR
 * \par INDIRECT OUTPUT
 * HW table DBAL_TABLE_IN_AC_INFO_DB is modified with the following fields:
 * DBAL_FIELD_VSI - sets to DNX_VSI_DEFAULT value
 * \see
 * bcm_dnx_vswitch_port_add, bcm_dnx_vswitch_port_get
 */

shr_error_e
bcm_dnx_vswitch_port_delete_all(
    int unit,
    bcm_vlan_t vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_vswitch_port_delete_all_verify(unit, vsi));

    /*
     * Traverse on Ingress non-native LIFs:
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_port_delete_all_ingress_non_native(unit, vsi));

    /*
     * Traverse on Ingress native non-virtual LIFs:
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_port_delete_all_ingress_native(unit, vsi));

    /*
     * Traverse on Ingress native virtual LIFs:
     */
    SHR_IF_ERR_EXIT(dnx_vswitch_port_delete_all_ingress_native_virtual(unit, vsi));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;

}
