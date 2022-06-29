/** \file mpls_esi.c ESI DB management for MPLS based EVPN.
 * This file contains the implementation and verification of the ESI DB
 * management APIs for MPLS based EVPN multicast encapsulated traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/mpls.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <soc/dnx/dbal/dbal.h>

typedef enum
{
    ESI_ENCAP_ADD = 0,
    ESI_ENCAP_DEL
} dnx_mpls_esi_action_e;

/*
 * Utilities
 * {
 */

/**
 * \brief Adds/Dels a single entry to the HW DB. This is needed for the different kinds
 * of src_intf_id, so the src_intf_id is provided separately from the other struct fields.
 */
static shr_error_e
dnx_mpls_esi_encap_single_update(
    int unit,
    uint8 src_is_lif,
    uint32 src_intf_id,
    dnx_mpls_esi_action_e esi_action,
    bcm_mpls_esi_info_t * esi_info)
{
    uint32 entry_handle_id;
    uint32 res_type;
    dbal_tables_e esem_table_id;
    dbal_fields_e if_field_id;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (src_is_lif)
    {
        esem_table_id = DBAL_TABLE_ESEM_DUAL_HOMING_DB;
        if_field_id = DBAL_FIELD_GLOB_IN_LIF;
        res_type = DBAL_RESULT_TYPE_ESEM_DUAL_HOMING_DB_ETPS_DUAL_HOMING;
    }
    else
    {
        esem_table_id = DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB;
        if_field_id = DBAL_FIELD_SYSTEM_PORT_AGGREGATE;
        res_type = DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_DUAL_HOMING;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, esem_table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, if_field_id, src_intf_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, esi_info->out_class_id);

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, esem_table_id, &core_mode));

    /** Suppose all cores share the same contents.*/
    if (core_mode == DBAL_CORE_MODE_DPC)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, _SHR_CORE_ALL);
    }

    if (esi_action == ESI_ENCAP_ADD)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DUAL_HOMING, INST_SINGLE, esi_info->esi_label);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Add/Del entries for ESI encap DB with all possible members of a LAG port.
 * This is required since the key to the DB, in case of LAG, contains a member-ID
 * of the "real" in-port.
 */
static shr_error_e
dnx_mpls_esi_encap_trunk_member_update(
    int unit,
    dnx_mpls_esi_action_e esi_action,
    bcm_mpls_esi_info_t * esi_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_key_with_sspa_contains_member_id))
    {
        int member;
        int pool;
        int max_members = 0;
        uint32 spa;

        /*
         * Get max members
         */
        BCM_TRUNK_ID_POOL_GET(pool, BCM_GPORT_TRUNK_GET(esi_info->src_port));
        max_members = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;

        /*
         * Add entry for each member.
         */
        for (member = 0; member < max_members; member++)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, esi_info->src_port, member, &spa));
            SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_single_update(unit, 0, spa, esi_action, esi_info));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "It should not get here if esem_feature_key_with_sspa_contains_member_id is not set.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Add/Del entry with src_port or src_encap_id.
 */
static shr_error_e
dnx_mpls_esi_encap_update(
    int unit,
    dnx_mpls_esi_action_e esi_action,
    bcm_mpls_esi_info_t * esi_info)
{
    uint8 src_is_lif;
    uint32 src_intf_id;
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;

    SHR_FUNC_INIT_VARS(unit);

    src_is_lif = _SHR_IS_FLAG_SET(esi_info->flags, BCM_MPLS_ESI_INTF_NAMESPACE);
    if (src_is_lif)
    {
        /** Src is LIF*/
        src_intf_id = BCM_L3_ITF_VAL_GET(esi_info->src_encap_id);
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_single_update(unit, src_is_lif, src_intf_id, esi_action, esi_info));
    }
    else if (!BCM_GPORT_IS_TRUNK(esi_info->src_port) ||
             !dnx_data_esem.feature.feature_get(unit, dnx_data_esem_feature_key_with_sspa_contains_member_id))
    {
        /** Src is system-port or truck without the dnxdata.*/
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, esi_info->src_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));
        src_intf_id = gport_phy_info.sys_port;
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_single_update(unit, src_is_lif, src_intf_id, esi_action, esi_info));
    }
    else
    {
        /** Src is truck with the dnxdata.*/
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_trunk_member_update(unit, esi_action, esi_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 * Utilities
 */

/*
 * Verifiers
 * {
 */
/**
 * \brief Verify key fields (for all APIs). Key fields are always supplied,
 * verify their validity in this single function.
 */
static shr_error_e
dnx_mpls_esi_encap_keys_verify(
    int unit,
    bcm_mpls_esi_info_t * esi_info)
{
    uint32 allowed_flags;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * No flags so far..
     */
    allowed_flags = BCM_MPLS_ESI_INTF_NAMESPACE;
    if (_SHR_IS_FLAG_SET(esi_info->flags, ~allowed_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal flags value (0x%08x).", esi_info->flags);
    }

    if ((esi_info->src_port == _SHR_GPORT_INVALID) && !_SHR_IS_FLAG_SET(esi_info->flags, BCM_MPLS_ESI_INTF_NAMESPACE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "src_port (0x%08x) is not a valid port.", esi_info->src_port);
    }
    /*
     * The rest of esi_info->src_port validation is done by algo_gpm when it is ordered to retrieve
     * the system port for the supplied port.
     */
    /*
     * Coverity explanation: out_class_id is unsigned, but I want to use the macro anyway for cleanliness 
     */
     /* coverity[unsigned_compare:FALSE]  */
    SHR_RANGE_VERIFY(esi_info->out_class_id, 0, (dnx_data_l2.vlan_domain.nof_vlan_domains_get(unit) - 1),
                     _SHR_E_PARAM, "out_class_id out of range");

    if (_SHR_IS_FLAG_SET(esi_info->flags, BCM_MPLS_ESI_INTF_NAMESPACE))
    {
        if (!BCM_L3_ITF_TYPE_IS_LIF(esi_info->src_encap_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "src_encap_id (0x%08x) is not a valid LIF type of interface.",
                         esi_info->src_encap_id);
        }
        if (BCM_L3_ITF_VAL_GET(esi_info->src_encap_id) > (dnx_data_lif.global_lif.nof_global_in_lifs_get(unit) - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "src_encap_id (0x%08x) is out of range (0, 0x%X)", esi_info->src_encap_id,
                         (dnx_data_lif.global_lif.nof_global_in_lifs_get(unit) - 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Verify integrity and existence of the parameters to ESI DB entry addition.
 * \param [in] unit
 * \param [in] esi_info - All the relevant information
 */
static shr_error_e
dnx_mpls_esi_encap_add_verify(
    int unit,
    bcm_mpls_esi_info_t * esi_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(esi_info, _SHR_E_PARAM, "esi_info");

    SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_keys_verify(unit, esi_info));

    if (!DNX_MPLS_LABEL_IN_RANGE(unit, esi_info->esi_label) || (esi_info->esi_label <= MPLS_MAX_RESERVED_LABEL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "esi_label (%d) out of range (%d-%d).",
                     esi_info->esi_label, MPLS_MAX_RESERVED_LABEL + 1, DNX_MPLS_LABEL_MAX(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief Verify integrity and existence of the parameters to ESI DB entry deletion.
 * \param [in] unit
 * \param [in] esi_info - All the relevant information
 */
static shr_error_e
dnx_mpls_esi_encap_get_delete_verify(
    int unit,
    bcm_mpls_esi_info_t * esi_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(esi_info, _SHR_E_PARAM, "esi_info");

    SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_keys_verify(unit, esi_info));

exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 * Verifiers
 */

/*
 * API Implementation
 * {
 */

/**
 * \brief Adds an entry to the ESI DB. This entry may be used
 * for adding ESI label under IML labels in case the inlif+out_mpls_tunnel
 * indicate this.
 * \param [in] unit
 * \param [in] esi_info - All the relevant information
 *     - flags - No flags are supported. For future use.
 *     - src_port - ingress system port (connected to the ES)
 *     - out_class_id - dual homing identifier which classifies the peer
 *                      as dual homed with me
 *     - esi_label - MPLS label value
 */
int
bcm_dnx_mpls_esi_encap_add(
    int unit,
    bcm_mpls_esi_info_t * esi_info)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_mpls_esi_encap_add_verify(unit, esi_info));

    SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_update(unit, ESI_ENCAP_ADD, esi_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Deletes an entry from the ESI DB.
 * \param [in] unit
 * \param [in] esi_info - All the relevant information
 *     - flags - No flags are supported. For future use.
 *     - src_port - ingress system port (connected to the ES)
 *     - out_class_id - dual homing identifier which classifies the peer
 *                      as dual homed with me
 *     - esi_label - Ignored
 */
int
bcm_dnx_mpls_esi_encap_delete(
    int unit,
    bcm_mpls_esi_info_t * esi_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_mpls_esi_encap_get_delete_verify(unit, esi_info));

    SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_update(unit, ESI_ENCAP_DEL, esi_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Gets an entry from the ESI DB.
 * \param [in] unit
 * \param [in,out] esi_info - All the relevant information
 *     - flags - No flags are supported. For future use.
 *     - src_port - ingress system port (connected to the ES)
 *     - out_class_id - dual homing identifier which classifies the peer
 *                      as dual homed with me
 *     - esi_label - MPLS label value
 */
int
bcm_dnx_mpls_esi_encap_get(
    int unit,
    bcm_mpls_esi_info_t * esi_info)
{
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;
    uint32 result_type, exp_res_type;
    uint32 entry_handle_id;
    dbal_tables_e esem_table_id;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_mpls_esi_encap_get_delete_verify(unit, esi_info));

    if (_SHR_IS_FLAG_SET(esi_info->flags, BCM_MPLS_ESI_INTF_NAMESPACE))
    {
        esem_table_id = DBAL_TABLE_ESEM_DUAL_HOMING_DB;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, esem_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF,
                                   BCM_L3_ITF_VAL_GET(esi_info->src_encap_id));
        exp_res_type = DBAL_RESULT_TYPE_ESEM_DUAL_HOMING_DB_ETPS_DUAL_HOMING;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, esi_info->src_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

        esem_table_id = DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, esem_table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, gport_phy_info.sys_port);
        exp_res_type = DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_DUAL_HOMING;
    }
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, esi_info->out_class_id);

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, esem_table_id, &core_mode));

    /** Suppose the same contents are shared in all cores. */
    if (core_mode == DBAL_CORE_MODE_DPC)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
    if (result_type != exp_res_type)
    {
        /*
         * Wrong result type.
         */
        SHR_ERR_EXIT(_SHR_E_BADID, "Wrong entry type (%d) found for key {src_port=0x%08x, out_class_id=0x%08x}",
                     result_type, esi_info->src_port, esi_info->out_class_id);
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DUAL_HOMING, INST_SINGLE, &esi_info->esi_label));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverses all the entries in the given ESI DB.
 * \param [in] unit
 * \param [in] esi_table - Given ESI DB
 * \param [in] cb - callback routine that will be called with every found entry
 * \param [in] user_data - optional pointer to additional data for usage in the callback
 */
int
dnx_mpls_esi_encap_table_traverse(
    int unit,
    dbal_tables_e esi_table,
    bcm_mpls_esi_encap_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 src_sysport;
    bcm_mpls_esi_info_t esi_info;
    uint32 exp_res_type;
    dbal_core_mode_e core_mode = DBAL_CORE_NONE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, esi_table, &core_mode));

    exp_res_type = (esi_table == DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB) ?
        DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_DUAL_HOMING :
        DBAL_RESULT_TYPE_ESEM_DUAL_HOMING_DB_ETPS_DUAL_HOMING;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, esi_table, &entry_handle_id));
    /*
     * DBAL iterate over all the entries in the DB with the relevant result type.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                     DBAL_CONDITION_EQUAL_TO, &exp_res_type, NULL));
    /** Suppose all cores share the same contents.*/
    if (core_mode == DBAL_CORE_MODE_DPC)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CONDITION_EQUAL_TO, DBAL_CORE_DEFAULT, 0));
    }
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_mpls_esi_info_t_init(&esi_info);

        if (esi_table == DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB)
        {
            /*
             * Table key field: system_port
             * Need to be translated to a gport.
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, &src_sysport));
            /*
             * Get the gport for this system port.
             */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_system_port(unit, src_sysport, &esi_info.src_port));
            if (BCM_GPORT_IS_TRUNK(esi_info.src_port))
            {
                int member;
                SHR_IF_ERR_EXIT(dnx_trunk_spa_to_gport(unit, src_sysport, &member, &esi_info.src_port));
                if (member)
                {
                    /*
                     * For LAG, call the user's cb, only for member 0
                     */
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                    continue;
                }
            }
        }
        else if (esi_table == DBAL_TABLE_ESEM_DUAL_HOMING_DB)
        {
            esi_info.flags |= BCM_MPLS_ESI_INTF_NAMESPACE;
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_GLOB_IN_LIF, (uint32 *) &esi_info.src_encap_id));
            BCM_L3_ITF_SET(esi_info.src_encap_id, BCM_L3_ITF_TYPE_LIF, esi_info.src_encap_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s is not supported for dual-home.",
                         dbal_logical_table_to_string(unit, esi_table));
        }

        /*
         * Table key field: vlan domain
         * Set to out_class_id
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, &esi_info.out_class_id));

        /*
         * Get the entry value - straight forward.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_DUAL_HOMING, INST_SINGLE, &esi_info.esi_label));

        /*
         * Finally, issue the user's callback
         */
        SHR_IF_ERR_EXIT((*cb) (unit, &esi_info, user_data));

        /*
         * Continue loop..
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Traverses all the entries in the ESI DB.
 * \param [in] unit
 * \param [in] cb - callback routine that will be called with every found entry
 * \param [in] user_data - optional pointer to additional data for usage in the callback
 */
int
bcm_dnx_mpls_esi_encap_traverse(
    int unit,
    bcm_mpls_esi_encap_traverse_cb cb,
    void *user_data)
{
    dbal_tables_e esi_table_idx;
    dbal_tables_e esi_tables[] = { DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, DBAL_TABLE_ESEM_DUAL_HOMING_DB };

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");

    for (esi_table_idx = 0; esi_table_idx < sizeof(esi_tables) / sizeof(dbal_tables_e); esi_table_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_table_traverse(unit, esi_tables[esi_table_idx], cb, user_data));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 * API Implementation
 */
