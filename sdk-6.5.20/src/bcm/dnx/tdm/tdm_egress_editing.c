/** \file tdm_egress_editing.c
 * 
 *
 * TDM egress editing procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TDM

/*
 * Include
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>
#include <shared/utilex/utilex_bitstream.h>

#include <shared/bslenum.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/tdm.h>

#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

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
 * Defines.
 * {
 */

/** 
 * \brief - Verify procedure for bcm_dnx_tdm_egress_editing_set API
 */
static int
dnx_tdm_egress_editing_set_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_tdm_egress_editing_t * editing)
{
    int max_user_data = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify */
    SHR_NULL_CHECK(editing, _SHR_E_PARAM, "editing");

    /** Verify editing->type with editing->user_data */
    if ((editing->type != bcmTdmEgressEditingStampMulticastUserDefined) && (editing->user_data != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "User data is only valid for bcmTdmEgressEditingStampMulticastUserDefined editing type!");
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, DBAL_TABLE_EGRESS_PP_PORT, DBAL_FIELD_STAMP_VALUE,
                                                    FALSE, 0, 0, &max_user_data));

    /** Verify current user_data */
    if (editing->user_data > max_user_data)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "editing->user_data is out of range, it can be maximum %d!", max_user_data);
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See bcm_tdm_egress_editing_set (BCM API)
 */
int
bcm_dnx_tdm_egress_editing_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_tdm_egress_editing_t * editing)
{
    dbal_enum_value_field_etpp_trap_context_port_profile_e etpp_trap_context_port_profile;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 pp_port_index = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_tdm_egress_editing_set_verify(unit, flags, gport, editing));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    switch (editing->type)
    {
        case bcmTdmEgressEditingNoHeader:
        {
            etpp_trap_context_port_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_NORMAL;
            break;
        }
        case bcmTdmEgressEditingNoStamp:
        {
            etpp_trap_context_port_profile =
                DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_NOTHING_OR_BUILD_PTCH1;
            break;
        }
        case bcmTdmEgressEditingStampMulticastEncapId:
        {
            etpp_trap_context_port_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_MC_CUD;
            break;
        }
        case bcmTdmEgressEditingStampMulticastUserDefined:
        {
            etpp_trap_context_port_profile = DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_PORT_VAL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "editing->type %d is not supported!", editing->type);
            break;
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

    for (pp_port_index = 0; pp_port_index < gport_info.internal_port_pp_info.nof_pp_ports; pp_port_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT,
                                   gport_info.internal_port_pp_info.pp_port[pp_port_index]);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID,
                                   gport_info.internal_port_pp_info.core_id[pp_port_index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                                     etpp_trap_context_port_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAMP_VALUE, INST_SINGLE, editing->user_data);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See bcm_tdm_egress_editing_get (BCM API)
 */
int
bcm_dnx_tdm_egress_editing_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_tdm_egress_editing_t * editing)
{
    dbal_enum_value_field_etpp_trap_context_port_profile_e etpp_trap_context_port_profile;
    uint32 entry_handle_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, gport_info.internal_port_pp_info.pp_port[0]);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, gport_info.internal_port_pp_info.core_id[0]);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_CONTEXT_PORT_PROFILE, INST_SINGLE,
                               &etpp_trap_context_port_profile);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STAMP_VALUE, INST_SINGLE, &editing->user_data);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    switch (etpp_trap_context_port_profile)
    {
        case DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_NORMAL:
        {
            editing->type = bcmTdmEgressEditingNoHeader;
            break;
        }
        case DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_NOTHING_OR_BUILD_PTCH1:
        {
            editing->type = bcmTdmEgressEditingNoStamp;
            break;
        }
        case DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_MC_CUD:
        {
            editing->type = bcmTdmEgressEditingStampMulticastEncapId;
            break;
        }
        case DBAL_ENUM_FVAL_ETPP_TRAP_CONTEXT_PORT_PROFILE_TDM_STAMP_PORT_VAL:
        {
            editing->type = bcmTdmEgressEditingStampMulticastUserDefined;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "etpp_trap_context_port_profile %d is not supported!",
                         etpp_trap_context_port_profile);
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
