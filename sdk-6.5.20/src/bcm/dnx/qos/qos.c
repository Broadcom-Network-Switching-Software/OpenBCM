/**
 * \file qos.c QOS procedures for DNX. 
     Dedicated set of QOS APIs are distributed between qos_*.c. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_QOS
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */

#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/qos.h>
#include <bcm_int/dnx/qos/qos.h>
#include <bcm_int/dnx/algo/qos/algo_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_qos.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include "qos_internal.h"
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * }
 */

/**
 * \brief 
 * map color to hw value DP
 */
int
dnx_qos_color_encode(
    int unit,
    bcm_color_t color,
    int *dp)
{
    SHR_FUNC_INIT_VARS(unit);

    if (color < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid color %d", color);
    }

    switch (color)
    {
        case bcmColorGreen:
            *dp = DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_GREEN;
            break;
        case bcmColorYellow:
            *dp = DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_YELLOW;
            break;
        case bcmColorRed:
            *dp = DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_RED;
            break;
        case bcmColorBlack:
            *dp = DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_INVALID;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid color %d", color);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * map hw value DP to color
 */
int
dnx_qos_color_decode(
    int unit,
    int dp,
    bcm_color_t * color)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dp < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid dp %d", dp);
    }

    switch (dp)
    {
        case DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_GREEN:
            *color = bcmColorGreen;
            break;
        case DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_YELLOW:
            *color = bcmColorYellow;
            break;
        case DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_RED:
            *color = bcmColorRed;
            break;
        case DBAL_ENUM_FVAL_METER_INTERNAL_COLOR_MAPPING_METER_INTERNAL_COLOR_INVALID:
            *color = bcmColorBlack;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid dp %d", dp);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *  verify qos profile is in range and allocated
 */
static shr_error_e
dnx_qos_map_profile_verify(
    int unit,
    int map_id)
{
    uint8 is_allocated = 0;
    uint32 profile_index;
    int nof_profiles;
    char *qos_type = NULL;
    dnx_qos_map_id_is_allocated_f dnx_qos_profile_is_allocated = NULL;

    SHR_FUNC_INIT_VARS(unit);

    profile_index = DNX_QOS_MAP_PROFILE_GET(map_id);

    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        if (DNX_QOS_MAP_IS_PHB(map_id) && DNX_QOS_MAP_IS_REMARK(map_id))
        {
            dnx_qos_profile_is_allocated = algo_qos_db.ingress_qos.is_allocated;
            nof_profiles = dnx_data_qos.qos.nof_ingress_profiles_get(unit);
            qos_type = "Ingress PHB REMARK";
        }
        else if (DNX_QOS_MAP_IS_POLICER(map_id))
        {
            dnx_qos_profile_is_allocated = algo_qos_db.ingress_qos_policer.is_allocated;
            nof_profiles = dnx_data_qos.qos.nof_ingress_policer_profiles_get(unit);
            qos_type = "Ingress policer";
        }
        else if (DNX_QOS_MAP_IS_VLAN_PCP(map_id))
        {
            dnx_qos_profile_is_allocated = algo_qos_db.ingress_qos_vlan_pcp.is_allocated;
            nof_profiles = dnx_data_qos.qos.nof_ingress_vlan_pcp_profiles_get(unit);
            qos_type = "Ingress vlan pcp";
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "wrong ingress map type [%x]\n", map_id);
        }
    }
    else if (DNX_QOS_MAP_IS_EGRESS(map_id))
    {
        if (DNX_QOS_MAP_IS_POLICER(map_id))
        {
            dnx_qos_profile_is_allocated = algo_qos_db.egress_qos_policer.is_allocated;
            nof_profiles = dnx_data_qos.qos.nof_egress_policer_profiles_get(unit);
            qos_type = "Egress policer";
        }
        else if (DNX_QOS_MAP_IS_REMARK(map_id))
        {
            dnx_qos_profile_is_allocated = algo_qos_db.egress_qos.is_allocated;
            nof_profiles = dnx_data_qos.qos.nof_egress_profiles_get(unit);
            qos_type = "Egress REMARK";
        }
        else if (DNX_QOS_MAP_IS_OAM_PCP(map_id))
        {
            dnx_qos_profile_is_allocated = algo_qos_db.egress_qos_oam_pcp_map.is_allocated;
            nof_profiles = dnx_data_oam.general.oam_nof_priority_map_profiles_get(unit);
            qos_type = "Egress OAM PCP";
        }
        else if (DNX_QOS_MAP_IS_MPLS_PHP(map_id))
        {
            dnx_qos_profile_is_allocated = algo_qos_db.egress_qos_mpls_php.is_allocated;
            nof_profiles = dnx_data_qos.qos.nof_egress_mpls_php_profiles_get(unit);
            qos_type = "Egress MPLS PHP";
        }
        else if (DNX_QOS_MAP_IS_ENCAP_COS(map_id))
        {
            dnx_qos_profile_is_allocated = algo_qos_db.egress_cos_profile.is_allocated;
            nof_profiles = dnx_data_qos.qos.nof_egress_cos_profiles_get(unit);
            qos_type = "Egress COS";
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "wrong egress map type [%x]\n", map_id);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "wrong map type [%x]\n", map_id);
    }

    /** Check if the qos profile is in range.*/
    if (profile_index >= nof_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "%s map [%x]. map is out of range[0, %d)\n", qos_type, map_id, nof_profiles);
    }

    /** Check if the qos profile is allocated already.*/
    SHR_IF_ERR_EXIT(dnx_qos_profile_is_allocated(unit, profile_index, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "%s map [%x]. map is not allocated\n", qos_type, map_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * verify map_opcode is in range and allocated
 */
static shr_error_e
dnx_qos_map_opcode_verify(
    int unit,
    int map_opcode)
{
    uint8 is_allocated = 0;
    uint32 opcode_index;
    int nof_opcodes;
    char *opcode_type = NULL;
    dnx_qos_map_id_is_allocated_f dnx_qos_opcode_is_allocated = NULL;

    SHR_FUNC_INIT_VARS(unit);

    opcode_index = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    if (DNX_QOS_MAP_IS_INGRESS(map_opcode))
    {
        if (DNX_QOS_MAP_IS_PHB(map_opcode))
        {
            dnx_qos_opcode_is_allocated = algo_qos_db.ingress_qos_phb_opcode.is_allocated;
            nof_opcodes = dnx_data_qos.qos.nof_ingress_opcode_profiles_get(unit);
            opcode_type = "ingress PHB";
        }
        else if (DNX_QOS_MAP_IS_REMARK(map_opcode))
        {
            dnx_qos_opcode_is_allocated = algo_qos_db.ingress_qos_remark_opcode.is_allocated;
            nof_opcodes = dnx_data_qos.qos.nof_ingress_opcode_profiles_get(unit);
            opcode_type = "ingress Remark";
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "wrong ingress map type [%x]\n", map_opcode);
        }
    }
    else if (DNX_QOS_MAP_IS_EGRESS(map_opcode))
    {
        if (DNX_QOS_MAP_IS_REMARK(map_opcode))
        {
            dnx_qos_opcode_is_allocated = algo_qos_db.egress_qos_opcode.is_allocated;
            nof_opcodes = dnx_data_qos.qos.nof_egress_opcode_profiles_get(unit);
            opcode_type = "egress remark";
        }
        else if (DNX_QOS_MAP_IS_MPLS_PHP(map_opcode))
        {
            dnx_qos_opcode_is_allocated = algo_qos_db.egress_qos_php_opcode.is_allocated;
            nof_opcodes = dnx_data_qos.qos.nof_egress_php_opcode_profiles_get(unit);
            opcode_type = "egress MPLS PHP";
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "wrong egress map type [%x]\n", map_opcode);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "wrong map type [%x]\n", map_opcode);
    }

    /** Verify if the opcode is in range.*/
    if (opcode_index >= nof_opcodes)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given opcode (%d) is out of range [0, %d)\n", opcode_index, nof_opcodes);
    }

    /** Verify if the opcode is allocated.*/
    SHR_IF_ERR_EXIT(dnx_qos_opcode_is_allocated(unit, opcode_index, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "%s opcode [0x%08X] is not allocated\n", opcode_type, map_opcode);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * verify bcm_dnx_qos_map_create parameters 
 */
static shr_error_e
dnx_qos_map_create_verify(
    int unit,
    uint32 flags,
    int *map_id)
{
    int profile_ndx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(map_id, _SHR_E_PARAM, "The map_id is not a valid pointer!");

    profile_ndx = DNX_QOS_MAP_PROFILE_GET(*map_id);

    if (flags & ~DNX_QOS_MAP_CREATE_SUPPORTED_FLAGS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags not supported [%x]\n", (flags & ~DNX_QOS_MAP_CREATE_SUPPORTED_FLAGS));
    }
    if (flags & BCM_QOS_MAP_POLICER)
    {
        /** verify that egress POLICER is stand alone application */
        if ((flags & DNX_QOS_APPLICATIONS_SUPPORTED_FLAGS) != (flags & BCM_QOS_MAP_POLICER))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "illegal flags combination: BCM_QOS_MAP_POLICER application is not allowed with other applications. (flags=0x%x)\n",
                         flags);
        }
    }

    /** validate Profile index if needed */
    if (flags & BCM_QOS_MAP_WITH_ID)
    {
        if (flags & BCM_QOS_MAP_INGRESS)
        {
            if (profile_ndx > dnx_data_qos.qos.nof_ingress_profiles_get(unit) - 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for profile index %d\n", profile_ndx);
            }
        }
        else if ((flags & BCM_QOS_MAP_EGRESS) && (flags & BCM_QOS_MAP_POLICER))
        {
            if (profile_ndx > dnx_data_qos.qos.nof_egress_policer_profiles_get(unit) - 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for profile index %d\n", profile_ndx);
            }
        }
        else if (flags & BCM_QOS_MAP_EGRESS)
        {
            if (profile_ndx > dnx_data_qos.qos.nof_egress_profiles_get(unit) - 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "illegal value for profile index %d\n", profile_ndx);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "un-supported flags [%x]\n", flags);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *  verify qos profile and opcode
 *  For bcm_dnx_qos_map_add/delete, map_entry must not be NULL.
 *  For bcm_dnx_qos_map_multi_get, map_entry should be NULL.
 */
static shr_error_e
dnx_qos_map_id_verify(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    int flag_group_number = 0;
    int opcode;
    uint8 map_entry_valid;
    char *opcode_field = NULL;

    SHR_FUNC_INIT_VARS(unit);

    map_entry_valid = (map_entry != NULL);

    /** Verify the flags.*/
    if (flags & ~DNX_QOS_MAP_ADD_SUPPORTED_FLAGS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags not supported %x\n", (flags & ~DNX_QOS_MAP_ADD_SUPPORTED_FLAGS));
    }

    /** Verify the possible qos var types.*/
    if (flags & BCM_QOS_MAP_L2)
    {
        flag_group_number++;
    }
    if (flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6))
    {
        flag_group_number++;
    }
    if (flags & (BCM_QOS_MAP_MPLS))
    {
        flag_group_number++;
    }
    if (flags & (BCM_QOS_MAP_PORT))
    {
        flag_group_number++;
    }
    if (flag_group_number > 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [0x%08X] must be from only one protocol group\n", flags);
    }

    /**
     * Verify the map_id and opcode.
     * when add mapping from qos profile to opcode (internal map id), 
     *     map_id is encoded qos profile 
     * when add qos mapping for ingress phb/remark or egress remark/php,
     *     map id is encoded opcode(internal map id) for current layer qos.
     *     map id is encoded qos profile for next layer qos.
     */
    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
        {
            /** Add qos mapping from qos profile and incoming qos to outgoing qos.*/
            if (DNX_QOS_MAP_IS_OPCODE(map_id) || !DNX_QOS_MAP_IS_VLAN_PCP(map_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "map_id [0x%08X] must be PCP_DEI profile for IVE pcpdei mapping in ingress.\n", map_id);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            }
        }
        else if (flags & BCM_QOS_MAP_POLICER)
        {
            /** Add qos mapping from qos profile and incoming qos to outgoing qos.*/
            if (DNX_QOS_MAP_IS_OPCODE(map_id) || !DNX_QOS_MAP_IS_POLICER(map_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] must be dp profile for IVE pcpdei mapping in egress.\n",
                             map_id);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            }
        }
        else if (((flags & BCM_QOS_MAP_L2) && (flags & BCM_QOS_MAP_L2_UNTAGGED)) || (flags & BCM_QOS_MAP_PORT))
        {
            /** Map qos profile to phb/nwk_qos for l2 untag or port default case.*/
            if (DNX_QOS_MAP_IS_OPCODE(map_id) || (!DNX_QOS_MAP_IS_PHB(map_id) && !DNX_QOS_MAP_IS_REMARK(map_id)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] must be QOS profile for PHB/Remark\n", map_id);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            }

            /**map_entry->opcode is not used in this case.*/
            if (map_entry_valid && map_entry->opcode)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "opcode should be 0 for L2 untag and port default mapping\n");
            }
        }
        else if (flags & (BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK))
        {
            if ((flags & BCM_QOS_MAP_OPCODE))
            {
                /** Mapping qos profile to opcode. Verify qos profile first.*/
                if (DNX_QOS_MAP_IS_OPCODE(map_id) || (!DNX_QOS_MAP_IS_PHB(map_id) && !DNX_QOS_MAP_IS_REMARK(map_id)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] must be QOS profile for PHB/Remark\n", map_id);
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
                }

                /** Verify the profile matches with flags.*/
                if ((_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_PHB) && !DNX_QOS_MAP_IS_PHB(map_id)) ||
                    (_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_REMARK) && !DNX_QOS_MAP_IS_REMARK(map_id)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] and flags [0x%08X] must match for PHB/Remark\n",
                                 map_id, flags);
                }

                opcode = map_entry_valid ? map_entry->opcode : BCM_DNX_QOS_MAP_INVALID_ID;
                opcode_field = "map_entry->opcode";
            }
            else
            {
                opcode = map_id;
                opcode_field = "map_id";
            }

            if (opcode != BCM_DNX_QOS_MAP_INVALID_ID)
            {
                /** Verify opcode then for mapping qos profile to opcode or opcode and incoming qos to outgoing qos.*/
                if (!DNX_QOS_MAP_IS_OPCODE(opcode) || (!DNX_QOS_MAP_IS_PHB(opcode) && !DNX_QOS_MAP_IS_REMARK(opcode)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "%s [0x%08X] must be opcode for PHB/Remark\n", opcode_field, opcode);
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_qos_map_opcode_verify(unit, opcode));
                }

                /** Verify the opcode matches with flags.*/
                if ((_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_PHB) && !DNX_QOS_MAP_IS_PHB(opcode)) ||
                    (_SHR_IS_FLAG_SET(flags, BCM_QOS_MAP_REMARK) && !DNX_QOS_MAP_IS_REMARK(opcode)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "%s [0x%08X] and flags [0x%08X] must match for PHB/Remark\n",
                                 opcode_field, map_id, flags);
                }
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags[0x%08X] not supported yet\n", flags);
        }
    }
    else
    {
        if ((flags & BCM_QOS_MAP_OPCODE) && ((flags & BCM_QOS_MAP_REMARK) || (flags & BCM_QOS_MAP_MPLS_PHP)))
        {
            /** Mapping qos profile to opcode. Verify qos profile first.*/
            if (DNX_QOS_MAP_IS_OPCODE(map_id) || (!DNX_QOS_MAP_IS_REMARK(map_id) && !DNX_QOS_MAP_IS_MPLS_PHP(map_id)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] must be QOS profile for Remark/PHP\n", map_id);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            }

            if (map_entry_valid)
            {
                /** Mapping qos profile to opcode. Verify opcode then.*/
                if (map_entry->opcode == BCM_QOS_OPCODE_PRESERVE)
                {
                    /** BCM_QOS_OPCODE_PRESERVE means ignoring the opcode.*/
                    if (!DNX_QOS_MAP_IS_EGRESS(map_id))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_QOS_OPCODE_PRESERVE used in egress only.\n");
                    }

                    if (!(flags & BCM_QOS_MAP_REMARK) || !(flags & BCM_QOS_MAP_OPCODE) ||
                        !(flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6)))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "BCM_QOS_OPCODE_PRESERVE is only used in opcode mapping in L3 forwarding remarking.\n");
                    }
                }
                else if (!DNX_QOS_MAP_IS_OPCODE(map_entry->opcode) ||
                         (!DNX_QOS_MAP_IS_REMARK(map_entry->opcode) && !DNX_QOS_MAP_IS_MPLS_PHP(map_entry->opcode)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "map_entry->opcode [0x%08X] must be opcode for Remark/PHP\n",
                                 map_entry->opcode);
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_qos_map_opcode_verify(unit, map_entry->opcode));
                }
            }
        }
        else if (flags & BCM_QOS_MAP_REMARK)
        {
            /** if map id is opcode, it is for current layer marking, otherwise it is for next layer*/
            if (DNX_QOS_MAP_IS_OPCODE(map_id) && DNX_QOS_MAP_IS_REMARK(map_id))
            {
                if ((0 != (flags & BCM_QOS_MAP_ECN)) ^ (FALSE != (DNX_QOS_MAP_IS_ECN(map_id))))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] is not ECN match flags [0x%x].\n", map_id, flags);
                }
                SHR_IF_ERR_EXIT(dnx_qos_map_opcode_verify(unit, map_id));
            }
            else if (DNX_QOS_MAP_IS_REMARK(map_id))
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] is not working for remarking.\n", map_id);
            }
        }
        else if (flags & BCM_QOS_MAP_MPLS_PHP)
        {
            /** Add qos mapping from qos profile and incoming qos to outgoing qos.*/
            if (!DNX_QOS_MAP_IS_OPCODE(map_id) || !DNX_QOS_MAP_IS_MPLS_PHP(map_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] must be opcode for PHP\n", map_id);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_opcode_verify(unit, map_id));
            }
        }
        else if (flags & BCM_QOS_MAP_POLICER)
        {
            /** Add qos mapping from qos profile and incoming qos to outgoing qos.*/
            if (DNX_QOS_MAP_IS_OPCODE(map_id) || !DNX_QOS_MAP_IS_POLICER(map_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] must be policer profile for policer offset mapping.\n",
                             map_id);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            }
        }
        else if (flags & BCM_QOS_MAP_OAM_PCP)
        {
            /** Add qos mapping from qos profile and incoming qos to outgoing qos.*/
            if (DNX_QOS_MAP_IS_OPCODE(map_id) || !DNX_QOS_MAP_IS_OAM_PCP(map_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] must be OAM PCP profile for OAM PCP mapping.\n", map_id);
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            }
        }
        else if (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR)
        {
            /** Add qos mapping from qos profile and incoming qos to outgoing qos.*/
            if (DNX_QOS_MAP_IS_ENCAP_COS(map_id))
            {
                SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "map_id [0x%08X] must be ENCAP COS profile.\n", map_id);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags[%x] not supported yet\n", flags);
        }
    }

    return _SHR_E_NONE;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * verify bcm_dnx_qos_map_add/delete parameters 
 */
static shr_error_e
dnx_qos_map_params_verify(
    int unit,
    uint32 flags,
    int map_id,
    bcm_qos_map_t * map_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(map_entry, _SHR_E_PARAM, "Map Is NULL\n");

    /** Verify map_id and flags.*/
    SHR_IF_ERR_EXIT(dnx_qos_map_id_verify(unit, flags, map_id, map_entry));

    /** Verify the input qos info.*/
    DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_PRI", map_entry->pkt_pri, dnx_data_qos.qos.packet_max_priority_get(unit));
    DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_CFI", map_entry->pkt_cfi, dnx_data_qos.qos.packet_max_cfi_get(unit));
    DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_PRI", map_entry->inner_pkt_pri,
                                  dnx_data_qos.qos.packet_max_priority_get(unit));
    DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_CFI", map_entry->inner_pkt_cfi, dnx_data_qos.qos.packet_max_cfi_get(unit));
    DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_EXP", map_entry->exp, dnx_data_qos.qos.packet_max_exp_get(unit));
    DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_REMAKR_PRI", map_entry->remark_int_pri,
                                  dnx_data_qos.qos.max_egress_var_get(unit));
    DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_DP", map_entry->remark_color, dnx_data_qos.qos.packet_max_dp_get(unit));

    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_DSCP", map_entry->dscp, dnx_data_qos.qos.packet_max_dscp_get(unit));
        DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_DP", map_entry->color, dnx_data_qos.qos.packet_max_dp_get(unit));
        if (!(flags & DNX_QOS_INGRESS_APPLICATIONS_SUPPORTED_FLAGS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags must be PHB or REMARK[%x]", flags);
        }
        if ((flags & BCM_QOS_MAP_PHB) && (flags & BCM_QOS_MAP_REMARK))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags must be only one of PHB or REMARK[%x]", flags);
        }

        if (flags & BCM_QOS_MAP_PHB)
        {
            if (flags & BCM_QOS_MAP_RCH)
            {
                DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_TC", map_entry->remark_int_pri,
                                              dnx_data_qos.qos.packet_max_tc_get(unit));
            }
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_TC", map_entry->int_pri, dnx_data_qos.qos.packet_max_tc_get(unit));
        }
        else if (flags & BCM_QOS_MAP_POLICER)
        {
            int nof_pcp = dnx_data_qos.qos.packet_max_priority_get(unit) + 1;
            int nof_dei = dnx_data_qos.qos.packet_max_cfi_get(unit) + 1;
            int max_pcp_dei = nof_pcp * nof_dei - 1;
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_PCP_DEI", map_entry->remark_int_pri, max_pcp_dei);
        }
        else if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
        {
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_TC", map_entry->int_pri,
                                          dnx_data_qos.qos.packet_max_priority_get(unit));
            if (!(flags & BCM_QOS_MAP_L2_UNTAGGED))
            {
                DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_COLOR", map_entry->color,
                                              dnx_data_qos.qos.packet_max_cfi_get(unit));
            }
        }
        else
        {
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_INT", map_entry->int_pri,
                                          dnx_data_qos.qos.packet_max_dscp_get(unit));
        }
    }
    else if (DNX_QOS_MAP_IS_EGRESS(map_id))
    {
        if (!(flags & DNX_QOS_EGRESS_APPLICATIONS_SUPPORTED_FLAGS))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "map_id must be REMARK or ECN[%x]", flags);
        }
        DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_RM_PRI", map_entry->remark_int_pri,
                                      dnx_data_qos.qos.max_egress_var_get(unit));

        if (flags & BCM_QOS_MAP_REMARK)
        {
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_PRI", map_entry->int_pri, dnx_data_qos.qos.max_egress_var_get(unit));
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_DSCP", map_entry->dscp, dnx_data_qos.qos.packet_max_dscp_get(unit));
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_COLOR", map_entry->color, dnx_data_qos.qos.packet_max_dp_get(unit));
        }

        if (flags & BCM_QOS_MAP_POLICER)
        {
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_TC", map_entry->int_pri, dnx_data_qos.qos.packet_max_tc_get(unit));
        }
        DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_POLICER_OFFSET", map_entry->policer_offset,
                                      dnx_data_qos.qos.packet_max_egress_policer_offset_get(unit));

        if (flags & BCM_QOS_MAP_OAM_PCP)
        {
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_OAM_TC", map_entry->int_pri,
                                          dnx_data_qos.qos.packet_max_tc_get(unit));
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_PRI", map_entry->pkt_pri,
                                          dnx_data_qos.qos.packet_max_priority_get(unit));
        }
        if (flags & BCM_QOS_MAP_MPLS_PHP)
        {
            DNX_QOS_ERROR_IF_OUT_OF_RANGE("QOS_MAP_MPLS_PHP", map_entry->int_pri,
                                          dnx_data_qos.qos.packet_max_dscp_get(unit));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] is not supported, must have one of INGRESS/EGRESS\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *  Verify the qos map ID for dnx_qos_control_set.
 */
static shr_error_e
dnx_qos_control_verify(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int arg)
{
    int map_id = arg;
    int qos_model;

    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmQosControlMplsIngressSwapRemarkProfile:
        {
            if (!DNX_QOS_MAP_IS_EGRESS(map_id) || !DNX_QOS_MAP_IS_REMARK(map_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "egr_map[%x] is not a valid egress QoS map ID!\n", map_id);
            }
            SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            break;
        }
        case bcmQosControlMplsIngressSwapTtlModel:
        {
            if ((arg != bcmQosEgressModelUniform) && (arg != bcmQosEgressModelPipeMyNameSpace))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "The TTL model is used by egress encapsulation. Only bcmQosEgressModelUniform and bcmQosEgressModelPipeMyNameSpace are available! \n");
            }
            break;
        }
        case bcmQosControlMplsIngressSwapTtl:
        {
            if (arg < 0 || arg > 255)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Given value (%d) for TTL is not in the legal range (0 ~ 255)! \n", arg);
            }
            /** Proection to avoid changing it to an illegal value in future possible changes to DNXDATA*/
            if (dnx_data_qos.qos.eei_mpls_ttl_profile_get(unit) % 2)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "TTL profile used by EEI.MPLS must be even number! It was defined in dnx_data_qos.qos.eei_mpls_ttl_profile\n");
            }
            break;
        }
        case bcmQosControlMplsExplicitNullIngressQosProfile:
        {
            if (flags & ~DNX_QOS_CONTROL_SUPPORTED_FLAGS)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flags not supported [%x]\n", (flags & ~DNX_QOS_CONTROL_SUPPORTED_FLAGS));
            }
            if (!DNX_QOS_MAP_IS_INGRESS(map_id) || !DNX_QOS_MAP_IS_REMARK(map_id) || !DNX_QOS_MAP_IS_PHB(map_id))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "egr_map[%x] is not a valid ingress QoS map ID!\n", map_id);
            }
            SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));
            break;
        }
        case bcmQosControlMplsExplicitNullIngressPhbModel:
        case bcmQosControlMplsExplicitNullIngressRemarkModel:
        case bcmQosControlMplsExplicitNullIngressTtlModel:
        {
            if (flags & ~DNX_QOS_CONTROL_SUPPORTED_FLAGS)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flags not supported [%x]\n", (flags & ~DNX_QOS_CONTROL_SUPPORTED_FLAGS));
            }
            qos_model = arg;
            if ((qos_model < bcmQosIngressModelShortpipe) || (qos_model > bcmQosIngressModelUniform))
                SHR_ERR_EXIT(_SHR_E_PARAM, "QoS model %d is not supported! \n", qos_model);
            break;
        }
        case bcmQosControlMplsIngressPopQoSPreserve:
        {
            if (flags & ~DNX_QOS_CONTROL_SUPPORTED_FLAGS)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flags not supported [%x]\n", (flags & ~DNX_QOS_CONTROL_SUPPORTED_FLAGS));
            }
            if (!(flags & BCM_QOS_MAP_L3))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "flags BCM_QOS_MAP_L3 must be set \n");
            }
            break;
        }
        case bcmQosControlMplsEgressForwardQoSPreserve:
        {
            /** nothing needs to check*/
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Bcm QoS control type %d is not supported! \n", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * verify bcm_dnx_qos_map_control_set/get parameters 
 */
static shr_error_e
dnx_qos_map_control_verify(
    int unit,
    uint32 map_id,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!DNX_QOS_MAP_IS_PHB(map_id) || !DNX_QOS_MAP_IS_REMARK(map_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "qos map control only can be used to PHB and remark profile! \n");
    }
    if ((flags & BCM_QOS_MAP_PHB) && (flags & BCM_QOS_MAP_REMARK))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "only PHB or remark flag can be set once! \n");
    }
    SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief 
 * Init for qos module. allocate resource pool, initialize 
 * tables 
 * called in device init 
 */
shr_error_e
dnx_qos_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_ingress_profiles_init(unit));

    SHR_IF_ERR_EXIT(dnx_qos_ingress_tables_init(unit));

    SHR_IF_ERR_EXIT(dnx_qos_egress_tables_init(unit));

    SHR_IF_ERR_EXIT(dnx_qos_egress_profiles_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *     create the sw qos profile according to the flags
 * \param [in] unit - Relevant unit.
 * \param [in] flags - related flags BCM_QOS_MAP_XXX.
 * \param [in] map_id - ID of the MAP.  Pointer to module
 *     ID of the local device to be filled by the function.
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */
shr_error_e
bcm_dnx_qos_map_create(
    int unit,
    uint32 flags,
    int *map_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verify the profile index value
     */
    SHR_INVOKE_VERIFY_DNX(dnx_qos_map_create_verify(unit, flags, map_id));

    if (flags & BCM_QOS_MAP_INGRESS)
    {
        if (flags & BCM_QOS_MAP_OPCODE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_opcode_create(unit, flags, map_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_map_create(unit, flags, map_id));
        }
    }
    else if (flags & BCM_QOS_MAP_EGRESS)
    {
        if (flags & BCM_QOS_MAP_OPCODE)
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_opcode_create(unit, flags, map_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_map_create(unit, flags, map_id));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags [%x] is not supported, must have one of INGRESS/EGRESS\n", flags);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *     Destroy the sw qos opcode
 * \param [in] unit - Relevant unit.
 * \param [in] map_opcode - opcode of the QOS MAP.
 * \return
 *     \retval Negative in case of an error.  
 *     \retval Zero in case of NO ERROR.
 */
shr_error_e
dnx_qos_opcode_destroy(
    int unit,
    int map_opcode)
{
    uint32 opcode_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_qos_map_opcode_verify(unit, map_opcode));

    opcode_index = DNX_QOS_MAP_PROFILE_GET(map_opcode);

    if (DNX_QOS_MAP_IS_INGRESS(map_opcode))
    {
        if (DNX_QOS_MAP_IS_PHB(map_opcode))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_phb_opcode.free_single(unit, opcode_index));
        }
        else if (DNX_QOS_MAP_IS_REMARK(map_opcode))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_remark_opcode.free_single(unit, opcode_index));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "wrong ingress map type [%x]\n", map_opcode);
        }
    }
    else if (DNX_QOS_MAP_IS_EGRESS(map_opcode))
    {
        if (DNX_QOS_MAP_IS_REMARK(map_opcode))
        {
            if (DNX_QOS_MAP_IS_ECN(map_opcode))
            {
                SHR_IF_ERR_EXIT(algo_qos_db.
                                egress_qos_opcode.free_several(unit, DNX_QOS_ECN_BURN_OPCODE_NUM, opcode_index));
            }
            else
            {
                SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_opcode.free_single(unit, opcode_index));
            }
        }
        else if (DNX_QOS_MAP_IS_MPLS_PHP(map_opcode))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_php_opcode.free_single(unit, opcode_index));

        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "wrong egress map type [%x]\n", map_opcode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 *     Destroy the sw qos profile  
 * \param [in] unit - Relevant unit.
 * \param [in] map_id - ID of the QOS MAP.
 * \return
 *     \retval Negative in case of an error.  
 *     \retval Zero in case of NO ERROR.
 */
shr_error_e
bcm_dnx_qos_map_destroy(
    int unit,
    int map_id)
{
    uint32 profile_index;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (DNX_QOS_MAP_IS_OPCODE(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_opcode_destroy(unit, map_id));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, map_id));

    profile_index = DNX_QOS_MAP_PROFILE_GET(map_id);

    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        if (DNX_QOS_MAP_IS_PHB(map_id) && DNX_QOS_MAP_IS_REMARK(map_id))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.free_single(unit, profile_index));
            SHR_IF_ERR_EXIT(dnx_qos_ingress_map_control_clear(unit, profile_index));
        }
        else if (DNX_QOS_MAP_IS_VLAN_PCP(map_id))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_vlan_pcp.free_single(unit, profile_index));
        }
        else if (DNX_QOS_MAP_IS_POLICER(map_id))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_policer.free_single(unit, profile_index));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "wrong ingress map type [%x]\n", map_id);
        }
    }
    else if (DNX_QOS_MAP_IS_EGRESS(map_id))
    {
        if (DNX_QOS_MAP_IS_POLICER(map_id))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_policer.free_single(unit, profile_index));
        }
        else if ((DNX_QOS_MAP_IS_REMARK(map_id)))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos.free_single(unit, profile_index));
        }
        else if ((DNX_QOS_MAP_IS_MPLS_PHP(map_id)))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_mpls_php.free_single(unit, profile_index));
        }
        else if ((DNX_QOS_MAP_IS_ENCAP_COS(map_id)))
        {
            SHR_IF_ERR_EXIT(algo_qos_db.egress_cos_profile.free_single(unit, profile_index));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "wrong egress map type [%x]\n", map_id);
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief 
 * verify the Ingress and Egress QOS profile to the port (LIF)
 *    \param [in] unit - Relevant unit.
 *    \param [in] port - ID of port.
 *    \param [in] ing_map - ID of the INGRESS MAP.
 *    \param [in] egr_map - ID of the EGRESS MAP.
 * \return
 *    \retval Negative in case of an error. 
 *    \retval Zero in case of NO ERROR
 */
static shr_error_e
dnx_qos_port_map_verify(
    int unit,
    bcm_gport_t port,
    int ing_map,
    int egr_map)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((ing_map == BCM_DNX_QOS_MAP_INVALID_ID) && (egr_map == BCM_DNX_QOS_MAP_INVALID_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "both ingress & egress maps are invalid \n");
    }

    if (ing_map != BCM_DNX_QOS_MAP_INVALID_ID)
    {
        /** check ing_map is actually of type INGRESS */
        if (!DNX_QOS_MAP_IS_INGRESS(ing_map))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ing_map[%x] type is not INGRESS\n", ing_map);
        }

        /** check ing_map is actually allocated */
        SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, ing_map));
    }

    if (egr_map != BCM_DNX_QOS_MAP_INVALID_ID)
    {
        /** check egr_map is actually of type EGRESS */
        if (!DNX_QOS_MAP_IS_EGRESS(egr_map))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "egr_map[%x] type is not EGRESS\n", egr_map);
        }

        /** check egr_map is actually allocated */
        SHR_IF_ERR_EXIT(dnx_qos_map_profile_verify(unit, egr_map));

        if ((BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(port)) && (DNX_QOS_MAP_IS_OAM_PCP(egr_map)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "egr_map of type OAM PCP used with illegal gport \n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief 
 *     set the QOS profile to the gport (layer, lif) 
 *     The function sets ingress & egress map IDs, each of them 
 *     should have the appropriate prefix set according to its type.
 *     if one of them is BCM_DNX_QOS_MAP_INVALID_ID (-1), means it won't be changed
 * \param [in] unit - Relevant unit.
 * \param [in] port - ID of port.
 * \param [in] ing_map - ID of the INGRESS MAP.
 * \param [in] egr_map - ID of the EGRESS MAP.
 * \return
 *    \retval Negative in case of an error
 *    \retval Zero in case of NO ERROR
 * \see
 *    dnx_qos_ingress_port_map_set
 *    dnx_qos_egress_port_map_set
 */
shr_error_e
bcm_dnx_qos_port_map_set(
    int unit,
    bcm_gport_t port,
    int ing_map,
    int egr_map)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_qos_port_map_verify(unit, port, ing_map, egr_map));

    if (ing_map != BCM_DNX_QOS_MAP_INVALID_ID)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_port_map_set(unit, port, ing_map));
    }

    if (egr_map != BCM_DNX_QOS_MAP_INVALID_ID)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_port_map_set(unit, port, egr_map));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     get the QOS profile from the gport (layer, lif)
 *     The function gets ingress & egress map ID's for a given gport.
 * \param [in] unit - Relevant unit.
 * \param [in] port - ID of port.
 * \param [out] *ing_map - ID of the INGRESS MAP (profile id and direction flag), if NULL then won't get it
 * \param [out] *egr_map - ID of the EGRESS MAP (profile id and direction flag),  if NULL then won't get it
 * \return
 *     \retval Negative in case of an error
 *     \retval Zero in case of NO ERROR
 * \see
 *    dnx_qos_ingress_port_map_get
 *    dnx_qos_egress_port_map_get
 */
shr_error_e
bcm_dnx_qos_port_map_get(
    int unit,
    bcm_gport_t port,
    int *ing_map,
    int *egr_map)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (ing_map)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_port_map_get(unit, port, ing_map, 0));
    }
    if (egr_map)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_port_map_get(unit, port, egr_map, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *     get the QOS profile from the gport accoding to flags
 *     The function gets ingress & egress map ID's for a given gport.
 * \param [in] unit - Relevant unit.
 * \param [in] port - ID of port.
 * \param [in] flags -flags for QoS profile get
 *       BCM_QOS_MAP_INGRESS
 *       BCM_QOS_MAP_EGRESS
 *       BCM_QOS_MAP_PHB
 *       BCM_QOS_MAP_REMARK
 *       BCM_QOS_MAP_OAM_PCP
 *       BCM_QOS_MAP_ECN
 * \param [out] *map_id - ID of MAP (profile id and direction flag and application type), if NULL then return ERROR
 * \return
 *     \retval Negative in case of an error
 *     \retval Zero in case of NO ERROR
 * \see
 *    dnx_qos_ingress_port_map_get
 *    dnx_qos_egress_port_map_get
 */

int
bcm_dnx_qos_port_map_type_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    int *map_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(map_id, _SHR_E_PARAM, "Map ID Is NULL\n");

    if (!(flags & ~DNX_QOS_APPLICATIONS_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "one of flags [%x] must be set\n", DNX_QOS_APPLICATIONS_SUPPORTED_FLAGS);
    }

    if (flags & BCM_QOS_MAP_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_port_map_get(unit, port, map_id, flags));
    }
    else if (flags & BCM_QOS_MAP_EGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_port_map_get(unit, port, map_id, flags));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ingress or egress flag must be set\n");
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief 
 *   Add entries to internal map ID. The map entry values should
 *   relate to the internal map type. such as:
 *       dscp relate to internal map type for L3
 *       pkt_pri and/or pkt_cfi relate to internal map type for L2
 *       exp realte to internal map type for MPLS
 *   In case BCM_QOS_MAP_OPCODE flag is set we only set the
 *   internal map ID
 * \param [in] unit - Relevant unit.
 * \param [in] flags - flags for internal map:
 *       BCM_QOS_MAP_INGRESS
 *       BCM_QOS_MAP_EGRESS
 *       BCM_QOS_MAP_PHB
 *       BCM_QOS_MAP_REMARK
 *       BCM_QOS_MAP_ECN
 *       BCM_QOS_MAP_POLICER 
 *       BCM_QOS_MAP_L2_UNTAGGED
 *       BCM_QOS_MAP_L2_INNER_TAG
 *       BCM_QOS_MAP_L2_OUTER_TAG
 *       BCM_QOS_MAP_IPV4
 *       BCM_QOS_MAP_IPV6
 *       BCM_QOS_MAP_MPLS
 *       BCM_QOS_MAP_OPCODE
 * \param [in] map_entry - relevant map entry.
 * \param [in] map_id - map id.
 * \return
 *     \retval Negative in case of an error. 
 *     \retval Zero in case of NO ERROR
 */
shr_error_e
bcm_dnx_qos_map_add(
    int unit,
    uint32 flags,
    bcm_qos_map_t * map_entry,
    int map_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_qos_map_params_verify(unit, flags, map_id, map_entry));

    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_add(unit, flags, map_id, map_entry));
    }
    else if (DNX_QOS_MAP_IS_EGRESS(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_add(unit, flags, map_id, map_entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "map_id[%x] not supported yet\n", map_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * delete entry from internal map ID. The map entry values should
 * relate to the internal map type. such as:
 *     dscp relate to internal map type for L3
 *     pkt_pri and/or pkt_cfi relate to internal map type for L2
 *     exp realte to internal map type for MPLS
 *   In case BCM_QOS_MAP_OPCODE flag is set we only delete the
 *   internal map ID
 * \param [in] unit - Relevant unit.
 * \param [in] flags - flags for internal map:
 *       BCM_QOS_MAP_INGRESS
 *       BCM_QOS_MAP_EGRESS
 *       BCM_QOS_MAP_PHB
 *       BCM_QOS_MAP_REMARK
 *       BCM_QOS_MAP_ECN
 *       BCM_QOS_MAP_POLICER 
 *       BCM_QOS_MAP_L2_UNTAGGED
 *       BCM_QOS_MAP_L2_INNER_TAG
 *       BCM_QOS_MAP_L2_OUTER_TAG
 *       BCM_QOS_MAP_IPV4
 *       BCM_QOS_MAP_IPV6
 *       BCM_QOS_MAP_MPLS
 *       BCM_QOS_MAP_OPCODE
 * \param [in] map_entry - relevant map entry.
 * \param [in] map_id - relevant qos map id.
 * 
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
shr_error_e
bcm_dnx_qos_map_delete(
    int unit,
    uint32 flags,
    bcm_qos_map_t * map_entry,
    int map_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_qos_map_params_verify(unit, flags, map_id, map_entry));

    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_delete(unit, flags, map_id, map_entry));
    }
    else if (DNX_QOS_MAP_IS_EGRESS(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_delete(unit, flags, map_id, map_entry));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "map_id type [%x] not supported yet\n", map_id);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 * get mapping entries total number, or get certain number mapping entries
 *     for a given map ID and mapping flag.
 * \param [in] unit - Relevant unit.
 * \param [in] flags - flags for map:
 *       BCM_QOS_MAP_PHB
 *       BCM_QOS_MAP_REMARK
 *       BCM_QOS_MAP_ECN
 *       BCM_QOS_MAP_POLICER
 *       BCM_QOS_MAP_OPCODE
 *       BCM_QOS_MAP_L2_UNTAGGED
 *       BCM_QOS_MAP_L2_INNER_TAG
 *       BCM_QOS_MAP_L2_OUTER_TAG
 *       BCM_QOS_MAP_IPV4
 *       BCM_QOS_MAP_IPV6
 *       BCM_QOS_MAP_MPLS
 * \param [in] map_id - qos map id, 
         if flags with BCM_QOS_MAP_OPCODE, it should be opcode profile
         if flags with BCM_QOS_MAP_REMARK, it should be remark profile
         if flags with BCM_QOS_MAP_PHB, it should be phb profile
 * \param [in] array_size - if 0, means only to get entries total number
 * \param [out] array - map entries array.
 * \param [out] array_count - collected entries number
 * 
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */

int
bcm_dnx_qos_map_multi_get(
    int unit,
    uint32 flags,
    int map_id,
    int array_size,
    bcm_qos_map_t * array,
    int *array_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(array, _SHR_E_PARAM, "array Is NULL\n");
    SHR_NULL_CHECK(array_count, _SHR_E_PARAM, "array_count Is NULL\n");

    SHR_INVOKE_VERIFY_DNX(dnx_qos_map_id_verify(unit, flags, map_id, NULL));

    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_multi_get(unit, flags, map_id, array_size, array, array_count));
    }
    else if (DNX_QOS_MAP_IS_EGRESS(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_egress_map_multi_get(unit, flags, map_id, array_size, array, array_count));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  get number of allocated qos opcode,
 */

int
dnx_qos_multi_get_opcode_number(
    int unit,
    int *array_count)
{
    int idx = 0, count = 0;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(array_count, _SHR_E_PARAM, "array_count Is NULL\n");
    count = 0;
    *array_count = 0;

    /** ingress phb opcode profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_ingress_opcode_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_phb_opcode.is_allocated(unit, idx, &is_allocated));

        if (is_allocated)
        {
            count++;
        }
    }
    /** ingress remark opcode profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_ingress_opcode_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_remark_opcode.is_allocated(unit, idx, &is_allocated));

        if (is_allocated)
        {
            count++;
        }
    }
    /** egress opcode profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_egress_opcode_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_opcode.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    /** egress mpls php opcode profile, 0 is reserved*/
    for (idx = 1; idx < dnx_data_qos.qos.nof_egress_php_opcode_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_php_opcode.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    *array_count = count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  get number of allocated qos profile,
 */

int
dnx_qos_multi_get_number(
    int unit,
    int *array_count)
{
    int idx = 0, count = 0;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(array_count, _SHR_E_PARAM, "array_count Is NULL\n");
    count = 0;
    *array_count = 0;

    /** ingress phb/remark profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_ingress_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    /** ingress policer profile*/
    for (idx = 1; idx < dnx_data_qos.qos.nof_ingress_policer_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_policer.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    /** ingress vlan pcp profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_ingress_vlan_pcp_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_vlan_pcp.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    /** egress policer profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_egress_policer_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_policer.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    /** egress remark profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_egress_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    /** egress MPLS PHP profile, 0 is reserved*/
    for (idx = 1; idx < dnx_data_qos.qos.nof_egress_mpls_php_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_mpls_php.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    /** egress cos profile, 0 is reserved*/
    for (idx = 1; idx < dnx_data_qos.qos.nof_egress_cos_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.egress_cos_profile.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            count++;
        }
    }
    *array_count = count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  get allocated qos opcode with creation flags, and count number.
 */
int
dnx_qos_multi_get_map_opcodes(
    int unit,
    int array_size,
    int *map_ids_array,
    int *flags_array,
    int *array_count)
{
    int idx = 0, count = 0, map_id = 0;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(array_count, _SHR_E_PARAM, "array_count Is NULL\n");
    SHR_NULL_CHECK(map_ids_array, _SHR_E_PARAM, "map_ids_array Is NULL\n");
    SHR_NULL_CHECK(flags_array, _SHR_E_PARAM, "flags_array Is NULL\n");

    count = 0;
    *array_count = 0;

    /** ingress phb opcode profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_ingress_opcode_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_phb_opcode.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map phb */
            DNX_QOS_PHB_MAP_SET(map_id);
            /** set map opcode */
            DNX_QOS_OPCODE_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_INGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_OPCODE;
            count++;
        }
    }
    /** ingress remark opcode profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_ingress_opcode_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_remark_opcode.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map remark */
            DNX_QOS_REMARK_MAP_SET(map_id);
            /** set map opcode */
            DNX_QOS_OPCODE_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_INGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_OPCODE;
            count++;
        }
    }
    /** egress opcode profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_egress_opcode_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_opcode.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map remark */
            DNX_QOS_REMARK_MAP_SET(map_id);
            /** set map opcode */
            DNX_QOS_OPCODE_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_EGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OPCODE;
            count++;
        }
    }
    /** egress mpls php opcode profile, 0 is reserved*/
    for (idx = 1; idx < dnx_data_qos.qos.nof_egress_php_opcode_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_php_opcode.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map PHP */
            DNX_QOS_MPLS_PHP_MAP_SET(map_id);
            /** set map opcode */
            DNX_QOS_MPLS_PHP_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_EGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_MPLS_PHP;
            count++;
        }
    }
    *array_count = count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  get allocated qos profiles with creation flags, and count number.
 */

int
dnx_qos_multi_get_map_ids(
    int unit,
    int array_size,
    int *map_ids_array,
    int *flags_array,
    int *array_count)
{
    int idx = 0, count = 0, map_id = 0;
    uint8 is_allocated = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(array_count, _SHR_E_PARAM, "array_count Is NULL\n");
    SHR_NULL_CHECK(map_ids_array, _SHR_E_PARAM, "map_ids_array Is NULL\n");
    SHR_NULL_CHECK(flags_array, _SHR_E_PARAM, "flags_array Is NULL\n");

    count = 0;
    *array_count = 0;
    /** ingress phb/remark profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_ingress_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map phb */
            DNX_QOS_PHB_MAP_SET(map_id);
            /** set map Remark */
            DNX_QOS_REMARK_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_INGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_PHB;
            count++;
        }
    }
    /** ingress policer profile*/
    for (idx = 1; idx < dnx_data_qos.qos.nof_ingress_policer_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_policer.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map policer */
            DNX_QOS_POLICER_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_INGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_OPCODE;
            count++;
        }
    }
    /** ingress vlan pcp profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_ingress_vlan_pcp_profiles_get(unit); idx++)
    {
        SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_vlan_pcp.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map vlan pcp */
            DNX_QOS_VLAN_PCP_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_INGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_OPCODE;
            count++;
        }
    }
    /** egress policer profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_egress_policer_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }
        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_policer.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map opcode */
            DNX_QOS_POLICER_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_EGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_POLICER;
            count++;
        }
    }
    /** egress remark profile*/
    for (idx = 0; idx < dnx_data_qos.qos.nof_egress_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }

        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map remark*/
            DNX_QOS_REMARK_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_EGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
            count++;
        }
    }
    /** egress MPLS PHP profile, 0 is reserved*/
    for (idx = 1; idx < dnx_data_qos.qos.nof_egress_mpls_php_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }

        SHR_IF_ERR_EXIT(algo_qos_db.egress_qos_mpls_php.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map remark*/
            DNX_QOS_MPLS_PHP_MAP_SET(map_id);
            /** set map direction*/
            DNX_QOS_EGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_MPLS_PHP;
            count++;
        }
    }
    /** egress cos profile, 0 is reserved*/
    for (idx = 1; idx < dnx_data_qos.qos.nof_egress_cos_profiles_get(unit); idx++)
    {
        if (count >= array_size)
        {
            break;
        }

        SHR_IF_ERR_EXIT(algo_qos_db.egress_cos_profile.is_allocated(unit, idx, &is_allocated));
        if (is_allocated)
        {
            map_id = idx;
            /** set map encap cos*/
            DNX_QOS_ENCAP_COS_SET(map_id);
            /** set map direction*/
            DNX_QOS_EGRESS_MAP_SET(map_id);
            *(map_ids_array + count) = map_id;
            *(flags_array + count) = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_ENCAP_INTPRI_COLOR;
            count++;
        }
    }
    *array_count = count;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * if array_size is zero, get number of allocated qos profile,
 * otherwise get allocated qos profile and creation flags.
 * profile id and QoS map type according flags
 * \param [in] unit - Relevant unit.
 * \param [in] array_size - the size of map_ids_array and flags_array
 * \param [out] map_ids_array - array for store of allocated profile id
 * \param [out] flags_array - array for creation flags for each allocated profile
 * \param [out] array_count - number of found qos profile
 * 
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */

int
bcm_dnx_qos_multi_get(
    int unit,
    int array_size,
    int *map_ids_array,
    int *flags_array,
    int *array_count)
{
    int profile_count = 0;
    int opcode_count = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (array_size == 0)
    {
        /** querying the number of map-ids for storage allocation*/
        SHR_IF_ERR_EXIT(dnx_qos_multi_get_number(unit, &profile_count));
        SHR_IF_ERR_EXIT(dnx_qos_multi_get_opcode_number(unit, &opcode_count));
        *array_count = profile_count + opcode_count;
    }
    else
    {
        /** querying all map-ids for storage allocation*/
        SHR_IF_ERR_EXIT(dnx_qos_multi_get_map_ids(unit, array_size, map_ids_array, flags_array, &profile_count));
        SHR_IF_ERR_EXIT(dnx_qos_multi_get_map_opcodes
                        (unit, (array_size - profile_count), (map_ids_array + profile_count),
                         (flags_array + profile_count), &opcode_count));
        *array_count = profile_count + opcode_count;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get qos map id for given profile.
 * The function will return encoded map id, which combines
 * profile id and QoS map type according flags
 * \param [in] unit - Relevant unit.
 * \param [in] flags - Relevant as following:
 *       BCM_QOS_MAP_PHB
 *       BCM_QOS_MAP_REMARK
 *       BCM_QOS_MAP_ECN
 *       BCM_QOS_MAP_POLICER 
 *       BCM_QOS_MAP_OPCODE
 *       BCM_QOS_MAP_INGRESS
 *       BCM_QOS_MAP_EGRESS
 * \param [in] profile_id - allocated profile id
 * \param [out] map_id - encoded qos map id
 * 
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */

int
bcm_dnx_qos_map_id_get_by_profile(
    int unit,
    uint32 flags,
    int profile_id,
    int *map_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    *map_id = profile_id;

    if (flags & BCM_QOS_MAP_INGRESS)
    {
        if (flags & BCM_QOS_MAP_OPCODE)
        {
           /** set map opcode */
            DNX_QOS_OPCODE_MAP_SET(*map_id);
        }
        if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
        {
            /** set map vlan */
            DNX_QOS_VLAN_PCP_MAP_SET(*map_id);
        }
        else if ((flags & BCM_QOS_MAP_PHB) && (flags & BCM_QOS_MAP_REMARK))
        {
            /** set map PHB */
            DNX_QOS_PHB_MAP_SET(*map_id);
            /** set map REMARK */
            DNX_QOS_REMARK_MAP_SET(*map_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags combination not supported for INGRESS QOS profile [%x]\n", flags);
        }
        /** set map direction  */
        DNX_QOS_INGRESS_MAP_SET(*map_id);
    }
    else if (flags & BCM_QOS_MAP_EGRESS)
    {
        if (flags & BCM_QOS_MAP_OPCODE)
        {
           /** set map opcode */
            DNX_QOS_OPCODE_MAP_SET(*map_id);
        }
        if (flags & BCM_QOS_MAP_REMARK)
        {
            /** set map remark */
            DNX_QOS_REMARK_MAP_SET(*map_id);
            if (flags & BCM_QOS_MAP_ECN)
            {
                /** set map ECN */
                DNX_QOS_ECN_MAP_SET(*map_id);
            }
        }
        else if (flags & BCM_QOS_MAP_POLICER)
        {
            /** set map policer */
            DNX_QOS_POLICER_MAP_SET(*map_id);
        }
        else if (flags & BCM_QOS_MAP_MPLS_PHP)
        {
            /** set map php */
            DNX_QOS_MPLS_PHP_MAP_SET(*map_id);
        }
        else if (flags & BCM_QOS_MAP_ENCAP_INTPRI_COLOR)
        {
            /** set map encap cos */
            DNX_QOS_ENCAP_COS_SET(*map_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags combination not supported for EGRESS QOS profile [%x]\n", flags);
        }
        /** set map direction  */
        DNX_QOS_EGRESS_MAP_SET(*map_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_QOS_MAP_INGRESS or BCM_QOS_MAP_EGRESS flag must be set [%x]\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get qos map id for given profile.
 * The function will return encoded map id, which combines
 * profile id and QoS map type according flags
 * \param [in] unit - Relevant unit.
 * \param [in] flags - Relevant as following:
 *       BCM_QOS_MAP_PHB
 *       BCM_QOS_MAP_REMARK
 *       BCM_QOS_MAP_ECN
 *       BCM_QOS_MAP_POLICER 
 *       BCM_QOS_MAP_OPCODE
 *       BCM_QOS_MAP_INGRESS
 *       BCM_QOS_MAP_EGRESS
 * \param [in] profile_id - allocated profile id
 * \param [out] map_id - encoded qos map id
 * 
 * \return
 *     \retval Negative in case of an error.
 *     \retval Zero in case of NO ERROR
 */
int
dnx_qos_map_opcode_get_by_id(
    int unit,
    uint32 flags,
    int profile_id,
    int *map_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    *map_id = profile_id;

    if (flags & BCM_QOS_MAP_INGRESS)
    {
        if (flags & BCM_QOS_MAP_PHB)
        {
            /** set map PHB */
            DNX_QOS_PHB_MAP_SET(*map_id);
        }
        else if (flags & BCM_QOS_MAP_REMARK)
        {
            /** set map REMARK */
            DNX_QOS_REMARK_MAP_SET(*map_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags combination not supported for INGRESS QOS profile [%x]\n", flags);
        }
        /** set map opcode */
        DNX_QOS_OPCODE_MAP_SET(*map_id);
        /** set map direction  */
        DNX_QOS_INGRESS_MAP_SET(*map_id);
    }
    else if (flags & BCM_QOS_MAP_EGRESS)
    {
        if (flags & BCM_QOS_MAP_REMARK)
        {
            /** set map opcode */
            DNX_QOS_REMARK_MAP_SET(*map_id);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flags combination not supported for EGRESS QOS profile [%x]\n", flags);
        }
        /** set map opcode */
        DNX_QOS_OPCODE_MAP_SET(*map_id);
        /** set map direction  */
        DNX_QOS_EGRESS_MAP_SET(*map_id);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_QOS_MAP_INGRESS or BCM_QOS_MAP_EGRESS flag must be set [%x]\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief 
 *  Set the QOS control properties per the control type. 
 *
 * \param [in] unit - Relevant unit.
  * \param [in] flags - QOS control flags, 
 *       BCM_QOS_MAP_IPV4
 *       BCM_QOS_MAP_IPV6
 * \param [in] type - QOS control type. Available control types:
 *                    refer to bcm_qos_control_type_t.
 * \param [in] arg - The property for the control type.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
int
bcm_dnx_qos_control_set(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_qos_control_verify(unit, flags, type, arg));

    switch (type)
    {
        /** QoS attributes for ingress mpls swap (by EEI)*/
        case bcmQosControlMplsIngressSwapRemarkProfile:
        case bcmQosControlMplsIngressSwapTtlModel:
        case bcmQosControlMplsIngressSwapTtl:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_control_mpls_swap_qos_attributes_set(unit, flags, type, arg));
            break;
        }
        /** explicit null label attribute set*/
        case bcmQosControlMplsExplicitNullIngressQosProfile:
        case bcmQosControlMplsExplicitNullIngressPhbModel:
        case bcmQosControlMplsExplicitNullIngressRemarkModel:
        case bcmQosControlMplsExplicitNullIngressTtlModel:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_control_explicit_null_label_qos_attributes_set(unit, flags, type, arg));
            break;
        }
        case bcmQosControlMplsIngressPopQoSPreserve:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_control_mpls_pop_dscp_preserve_set(unit, flags, arg));
            break;
        }
        case bcmQosControlMplsEgressForwardQoSPreserve:
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_control_mpls_fwd_exp_preserve_set(unit, flags, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Bcm QoS control type is not supported! \n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief 
 *  Get the QOS control properties per the control type. 
 *
 * \param [in] unit - Relevant unit.
 * \param [in] flags - QOS control flags, 
 *       BCM_QOS_MAP_IPV4
 *       BCM_QOS_MAP_IPV6
 * \param [in] type - QOS control type.
 * \param [out] arg - The property for the control type.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 *
 * \remark
 *  None
 *
 * \see
 *  None
 */
int
bcm_dnx_qos_control_get(
    int unit,
    uint32 flags,
    bcm_qos_control_type_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg is not a valid pointer!");

    switch (type)
    {
        case bcmQosControlMplsIngressSwapRemarkProfile:
        case bcmQosControlMplsIngressSwapTtlModel:
        case bcmQosControlMplsIngressSwapTtl:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_control_mpls_swap_qos_attributes_get(unit, flags, type, arg));
            break;
        }
        /**explicit null label get*/
        case bcmQosControlMplsExplicitNullIngressQosProfile:
        case bcmQosControlMplsExplicitNullIngressPhbModel:
        case bcmQosControlMplsExplicitNullIngressRemarkModel:
        case bcmQosControlMplsExplicitNullIngressTtlModel:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_control_explicit_null_label_qos_attributes_get(unit, flags, type, arg));
            break;
        }
        case bcmQosControlMplsIngressPopQoSPreserve:
        {
            SHR_IF_ERR_EXIT(dnx_qos_ingress_control_mpls_pop_dscp_preserve_get(unit, flags, arg));
            break;
        }
        case bcmQosControlMplsEgressForwardQoSPreserve:
        {
            SHR_IF_ERR_EXIT(dnx_qos_egress_control_mpls_fwd_exp_preserve_get(unit, flags, arg));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Bcm QoS control type is not supported! \n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief 
 *  Set the QOS map control type. 
 *
 * \param [in] unit - Relevant unit.
 * \param [in] map_id - QOS map id
 * \param [in] flags - Relevant flags, BCM_QOS_MAP_PHB, BCM_QOS_MAP_REMARK
 * \param [in] type - The type for qos map control, 
        bcmQosMapControlL3L2 
        bcmQosMapControlL2TwoTagsMode
        bcmQosMapControlMplsPortModeServiceTag 
 * \param [in] arg - The value of map control type.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 *
 * \see
 *   bcm_dnx_ingress_qos_map_control_set
 */

int
bcm_dnx_qos_map_control_set(
    int unit,
    uint32 map_id,
    uint32 flags,
    bcm_qos_map_control_type_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_qos_map_control_verify(unit, map_id, flags));

    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_control_set(unit, map_id, flags, type, arg));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "map id type is invalid! \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief 
 *  Get the QOS map control type. 
 *
 * \param [in] unit - Relevant unit.
 * \param [in] map_id - QOS map id
 * \param [in] flags - Relevant flags, BCM_QOS_MAP_PHB, BCM_QOS_MAP_REMARK
 * \param [in] type - The type for qos map control, 
        bcmQosMapControlL3L2 
        bcmQosMapControlL2TwoTagsMode
        bcmQosMapControlMplsPortModeServiceTag 
 * \param [out] arg - The value of map control type.
 *
 * \return
 *  \retval Negative in case of an error
 *  \retval Zero in case of NO ERROR
 *
 * \see
 *   bcm_dnx_ingress_qos_map_control_get
 */

int
bcm_dnx_qos_map_control_get(
    int unit,
    uint32 map_id,
    uint32 flags,
    bcm_qos_map_control_type_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_qos_map_control_verify(unit, map_id, flags));

    if (DNX_QOS_MAP_IS_INGRESS(map_id))
    {
        SHR_IF_ERR_EXIT(dnx_qos_ingress_map_control_get(unit, map_id, flags, type, arg));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "map id must be ingress! \n");
    }

exit:
    SHR_FUNC_EXIT;
}
