/** \file mirror_rule.c
 * 
 *
 * Internal MIRROR profile functionality for DNX. \n
 * This file handles mirror profiles (allocation, creation, retrieval, destruction, etc..)
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MIRROR

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <bcm/types.h>
#include <bcm_int/dnx/mirror/mirror.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include "mirror_profile_alloc.h"
#include "mirror_profile.h"

/*
 * }
 */

/*
 * MACROs
 * {
 */
#define DNX_MIRROR_DISABLE_IDX_GET(flags)       ((flags & BCM_MIRROR_PORT_INGRESS) ? DNX_MIRROR_PROFILE_DEFAULT : DNX_MIRROR_EGRESS_DISABLE_PROFILE_IDX)
#define DNX_MIRROR_PROFILE_GET(flags,profile) ((flags & BCM_MIRROR_PORT_INGRESS) ? profile : DNX_MIRROR_EGRESS_PROFILE_GET(profile))
/*
 * }
 */

/*
 * \brief - sets port & vlan mirror rule by dbal
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] pp_port -
 *   \param [in] core_id -
 *   \param [in] vlan_profile_id - vlan (profile) index
 *   \param [in] flags - [BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS]
 *   \param [in] mirror_profile_id - mirror destination (profile) index
 *
 * \remark
 */
shr_error_e
dnx_mirror_rule_dbal_set(
    int unit,
    uint32 pp_port,
    int core_id,
    int vlan_profile_id,
    uint32 flags,
    uint32 mirror_profile_id)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table = (flags & BCM_MIRROR_PORT_INGRESS) ? DBAL_TABLE_SNIF_INGRESS_MIRROR_RULE_TABLE :
        DBAL_TABLE_SNIF_EGRESS_MIRROR_RULE_TABLE;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_PROFILE, vlan_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIRROR_PROFILE, INST_SINGLE, mirror_profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - gets port & vlan mirror rule from dbal
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   \param [in] pp_port -
 *   \param [in] core_id -
 *   \param [in] vlan_profile_id - vlan (profile) index
 *   \param [in] flags - [BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS]
 *   \param [in] mirror_profile_id - mirror destination (profile) index
 *
 * \remark
 */
shr_error_e
dnx_mirror_rule_dbal_get(
    int unit,
    uint32 pp_port,
    int core_id,
    int vlan_profile_id,
    uint32 flags,
    uint32 *mirror_profile_id)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table = (flags & BCM_MIRROR_PORT_INGRESS) ? DBAL_TABLE_SNIF_INGRESS_MIRROR_RULE_TABLE :
        DBAL_TABLE_SNIF_EGRESS_MIRROR_RULE_TABLE;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_PROFILE, vlan_profile_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIRROR_PROFILE, INST_SINGLE, mirror_profile_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_rule_add(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    uint32 flags,
    bcm_gport_t destid)
{
    int profile_id;
    uint32 entry_handle_id;
    uint32 mirror_profile_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint8 first_reference;
    dbal_tables_e dbal_table;
    int pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * retrieve pp_port and mirror profile id
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &gport_info));
    mirror_profile_id = BCM_GPORT_MIRROR_GET(destid);

    /*
     * handle vid configuration if specified
     */
    if (vid != BCM_VLAN_INVALID)
    {
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            /*
             * allocate vlan profile.
             */
            SHR_IF_ERR_EXIT(dnx_mirror_vlan_profile_allocate
                            (unit, gport_info.internal_port_pp_info.core_id[pp_port_index], flags, vid, &profile_id,
                             &first_reference));
            /*
             * setting mirror ingress vid profile if it's the first usage of this vid
             */
            if (first_reference)
            {
                dbal_table =
                    (flags & BCM_MIRROR_PORT_INGRESS) ? DBAL_TABLE_SNIF_INGRESS_MIRROR_VID_PROFILE_TABLE :
                    DBAL_TABLE_SNIF_EGRESS_MIRROR_VID_PROFILE_TABLE;
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_PROFILE, profile_id);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                           gport_info.internal_port_pp_info.core_id[pp_port_index]);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_ID, INST_SINGLE, vid);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }
    else
    {
        /*
         * setting default tagged profile for ingress
         */
        if (flags & BCM_MIRROR_PORT_INGRESS)
        {
            if (flags & BCM_MIRROR_PORT_UNTAGGED_ONLY)
            {
                profile_id = dnx_data_snif.ingress.mirror_untagged_profiles_get(unit);
            }
            else if (flags & BCM_MIRROR_PORT_TAGGED_DEFAULT)
            {
                profile_id = dnx_data_snif.ingress.mirror_default_tagged_profile_get(unit);
            }
            else
            {
                /*
                 * for ingress port configuration both untagged and default should be set
                 */
                profile_id = dnx_data_snif.ingress.mirror_default_tagged_profile_get(unit);
                for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_mirror_rule_dbal_set
                                    (unit, gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                     gport_info.internal_port_pp_info.core_id[pp_port_index], profile_id, flags,
                                     mirror_profile_id));
                }
                profile_id = dnx_data_snif.ingress.mirror_untagged_profiles_get(unit);
            }
        }
        else
        {
            profile_id = dnx_data_snif.egress.mirror_default_profile_get(unit);
        }
    }

    /*
     * setting mirror rule table for port or port & VLAN rule
     */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_rule_dbal_set(unit, gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                 gport_info.internal_port_pp_info.core_id[pp_port_index], profile_id,
                                                 flags, DNX_MIRROR_PROFILE_GET(flags, mirror_profile_id)));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_rule_delete(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    uint32 flags,
    uint32 current_profile_id)
{
    int profile_id;
    uint32 entry_handle_id;
    uint8 last_reference;
    dbal_tables_e dbal_table;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * retrieve pp_port and mirror profile id
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &gport_info));

    /*
     * handle vid configuration if specified
     */
    if (vid != BCM_VLAN_INVALID)
    {
        for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
        {
            /*
             * deallocate vlan profile
             */
            SHR_IF_ERR_EXIT(dnx_mirror_vlan_profile_get
                            (unit, gport_info.internal_port_pp_info.core_id[pp_port_index], flags, vid, &profile_id));
            SHR_IF_ERR_EXIT(dnx_mirror_vlan_profile_deallocate
                            (unit, gport_info.internal_port_pp_info.core_id[pp_port_index], flags, vid,
                             &last_reference));

            /*
             * setting mirror ingress vid profile to null if it's the last usage of this vid
             */
            if (last_reference)
            {
                dbal_table =
                    (flags & BCM_MIRROR_PORT_INGRESS) ? DBAL_TABLE_SNIF_INGRESS_MIRROR_VID_PROFILE_TABLE :
                    DBAL_TABLE_SNIF_EGRESS_MIRROR_VID_PROFILE_TABLE;
                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VID_PROFILE, profile_id);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                           gport_info.internal_port_pp_info.core_id[pp_port_index]);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_ID, INST_SINGLE, BCM_VLAN_NONE);
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }
    else
    {
        if (flags & BCM_MIRROR_PORT_INGRESS)
        {
            /*
             * determining profile for ingress port
             */
            if (flags & BCM_MIRROR_PORT_UNTAGGED_ONLY)
            {
                profile_id = dnx_data_snif.ingress.mirror_untagged_profiles_get(unit);
            }
            else if ((flags & BCM_MIRROR_PORT_TAGGED_DEFAULT) && (flags & BCM_MIRROR_PORT_INGRESS))
            {
                profile_id = dnx_data_snif.ingress.mirror_default_tagged_profile_get(unit);
            }
            else
            {
                /*
                 * for ingress port configuration both untagged and default should be deleted
                 */
                profile_id = dnx_data_snif.ingress.mirror_default_tagged_profile_get(unit);
                for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
                {
                    SHR_IF_ERR_EXIT(dnx_mirror_rule_dbal_set
                                    (unit, gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                     gport_info.internal_port_pp_info.core_id[pp_port_index], profile_id, flags,
                                     DNX_MIRROR_DISABLE_IDX_GET(flags)));
                }
                profile_id = dnx_data_snif.ingress.mirror_untagged_profiles_get(unit);
            }
        }
        else
        {
            profile_id = dnx_data_snif.egress.mirror_default_profile_get(unit);
        }
    }

    /*
     * setting mirror rule table for port or port & VLAN rule
     */
    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        SHR_IF_ERR_EXIT(dnx_mirror_rule_dbal_set(unit, gport_info.internal_port_pp_info.pp_port[pp_port_index],
                                                 gport_info.internal_port_pp_info.core_id[pp_port_index], profile_id,
                                                 flags, DNX_MIRROR_DISABLE_IDX_GET(flags)));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mirror_rule_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    uint32 flags,
    uint32 *snif_profile_id)
{
    int vlan_profile_id;
    uint32 ing_untagged_profile_id;
    shr_error_e ret;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * retrieve pp_port and mirror profile id
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                    &gport_info));

    /*
     * handle vlan configuration if specified
     */
    if (vid != BCM_VLAN_INVALID)
    {
        ret = dnx_mirror_vlan_profile_get(unit, gport_info.internal_port_pp_info.core_id[0], flags, vid,
                                          &vlan_profile_id);
        if (ret == _SHR_E_NOT_FOUND)
        {
            *snif_profile_id = BCM_MIRROR_INVALID_PROFILE;
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(ret);
    }
    else
    {
        /*
         * determining vlan profile. allocate if needed.
         */
        if (flags & BCM_MIRROR_PORT_UNTAGGED_ONLY)
        {
            vlan_profile_id = dnx_data_snif.ingress.mirror_untagged_profiles_get(unit);
        }
        else if ((flags & BCM_MIRROR_PORT_TAGGED_DEFAULT) && (flags & BCM_MIRROR_PORT_INGRESS))
        {
            vlan_profile_id = dnx_data_snif.ingress.mirror_default_tagged_profile_get(unit);
        }
        else
        {
            vlan_profile_id =
                (flags & BCM_MIRROR_PORT_INGRESS) ? dnx_data_snif.ingress.mirror_default_tagged_profile_get(unit) :
                dnx_data_snif.egress.mirror_default_profile_get(unit);
        }
    }

    /*
     * getting the profile from mirror rule table
     */
    SHR_IF_ERR_EXIT(dnx_mirror_rule_dbal_get(unit, gport_info.internal_port_pp_info.pp_port[0],
                                             gport_info.internal_port_pp_info.core_id[0], vlan_profile_id, flags,
                                             snif_profile_id));
    /*
     * if vlan is not specified for ingress port, the untagged and default tagged profiles need to be equal
     */
    if ((vid == BCM_VLAN_INVALID) && (flags & BCM_MIRROR_PORT_INGRESS) &&
        !(flags & BCM_MIRROR_PORT_UNTAGGED_ONLY) && !(flags & BCM_MIRROR_PORT_TAGGED_DEFAULT))
    {
        vlan_profile_id = dnx_data_snif.ingress.mirror_untagged_profiles_get(unit);
        SHR_IF_ERR_EXIT(dnx_mirror_rule_dbal_get(unit, gport_info.internal_port_pp_info.pp_port[0],
                                                 gport_info.internal_port_pp_info.core_id[0], vlan_profile_id, flags,
                                                 &ing_untagged_profile_id));
        if (ing_untagged_profile_id != *snif_profile_id)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "In order to determine the profile per port, tagged default profile (%d)"
                         " and the untagged profile (%d) need to be the same", *snif_profile_id,
                         ing_untagged_profile_id);
        }
    }

    *snif_profile_id = DNX_MIRROR_PROFILE_GET(flags, *snif_profile_id);
    if (*snif_profile_id == DNX_MIRROR_PROFILE_DEFAULT)
    {
        *snif_profile_id = BCM_MIRROR_INVALID_PROFILE;
    }

exit:
    SHR_FUNC_EXIT;
}
