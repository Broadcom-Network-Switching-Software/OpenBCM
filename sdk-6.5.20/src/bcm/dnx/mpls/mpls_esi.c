/** \file mpls_esi.c ESI DB management for MPLS based EVPN.
 * This file contains the implementation and verification of the ESI DB
 * management APIs for MPLS based EVPN multicast encapsulated traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/mpls.h>
#include <bcm/trunk.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * Utilities
 * {
 */

/**
 * \brief Adds a single entry to the HW DB. This is needed for the different kinds
 * of src_port, so the src_port is provided separately from the other struct fields.
 */
static shr_error_e
dnx_mpls_esi_encap_single_add(
    int unit,
    uint32 src_port,
    bcm_mpls_esi_info_t * esi_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, src_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, esi_info->out_class_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_DUAL_HOMING);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DUAL_HOMING, INST_SINGLE, esi_info->esi_label);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Add entry with src_port that is not a trunk. Trunks get a special treatment
 * in dnx_mpls_esi_encap_trunk_add.
 */
static shr_error_e
dnx_mpls_esi_encap_add(
    int unit,
    bcm_mpls_esi_info_t * esi_info)
{
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, esi_info->src_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

    /*
     * Set in HW
     */
    SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_single_add(unit, gport_phy_info.sys_port, esi_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Adds entries for ESI encap DB with all possible members of a LAG port.
 * This is required since the key to the DB, in case of LAG, contains a member-ID
 * of the "real" in-port.
 */
static shr_error_e
dnx_mpls_esi_encap_trunk_add(
    int unit,
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
            SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_single_add(unit, spa, esi_info));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_add(unit, esi_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief Given a specific src_port (not GPort), deletes the entry from the DB.
 */
static shr_error_e
dnx_mpls_esi_encap_single_delete(
    int unit,
    uint32 src_port,
    bcm_mpls_esi_info_t * esi_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, src_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, esi_info->out_class_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Delete an entry with non-LAG src-port.
 */
static shr_error_e
dnx_mpls_esi_encap_delete(
    int unit,
    bcm_mpls_esi_info_t * esi_info)
{
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, esi_info->src_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

    /*
     * Delete from HW
     */
    SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_single_delete(unit, gport_phy_info.sys_port, esi_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Delete an entry with LAG src-port.
 */
static shr_error_e
dnx_mpls_esi_encap_trunk_delete(
    int unit,
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
         * Delete entry for each member.
         */
        for (member = 0; member < max_members; member++)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, esi_info->src_port, member, &spa));
            SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_single_delete(unit, spa, esi_info));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_delete(unit, esi_info));
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
    allowed_flags = 0;
    if (_SHR_IS_FLAG_SET(esi_info->flags, ~allowed_flags))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal flags value (0x%08x).", esi_info->flags);
    }

    if (esi_info->src_port == _SHR_GPORT_INVALID)
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

    SHR_INVOKE_VERIFY_DNX(dnx_mpls_esi_encap_add_verify(unit, esi_info));

    if (BCM_GPORT_IS_TRUNK(esi_info->src_port))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_trunk_add(unit, esi_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_add(unit, esi_info));
    }

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

    SHR_INVOKE_VERIFY_DNX(dnx_mpls_esi_encap_get_delete_verify(unit, esi_info));

    if (BCM_GPORT_IS_TRUNK(esi_info->src_port))
    {
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_trunk_delete(unit, esi_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_mpls_esi_encap_delete(unit, esi_info));
    }

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
    uint32 result_type;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_mpls_esi_encap_get_delete_verify(unit, esi_info));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, esi_info->src_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, gport_phy_info.sys_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, esi_info->out_class_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));
    if (result_type != DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_DUAL_HOMING)
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
    uint32 entry_handle_id;
    int is_end;
    uint32 src_sysport;
    bcm_mpls_esi_info_t esi_info;
    uint32 expected_res_type = DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_DUAL_HOMING;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, &entry_handle_id));
    /*
     * DBAL iterate over all the entries in the DB with the relevant result type.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                     DBAL_CONDITION_EQUAL_TO, &expected_res_type, NULL));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_mpls_esi_info_t_init(&esi_info);

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
/*
 * }
 * API Implementation
 */
