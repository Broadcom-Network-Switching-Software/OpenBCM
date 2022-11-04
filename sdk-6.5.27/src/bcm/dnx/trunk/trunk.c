/** \file src/bcm/dnx/trunk/trunk.c
 *
 *  This file contains the implementation of external API trunk
 *  functions.
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include "trunk_utils.h"
#include "trunk_sw_db.h"
#include "trunk_dbal_access.h"
#include "trunk_temp_structs_to_skip_papi.h"
#include "trunk_verify.h"
#include "trunk_chm.h"
#include "trunk_protection.h"
#include "trunk_egress/trunk_egress.h"
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_visibility.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/trunk/trunk_init.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/trunk_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/auto_generated/dnx_trunk_dispatch.h>
#include <bcm_int/dnx/auto_generated/dnx_switch_dispatch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/stk/stk_trunk.h>
#include <shared/trunk.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/port/port_prt_tcam.h>
#include <bcm_int/dnx/algo/consistent_hashing/consistent_hashing_manager.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_trunk_access.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#endif /* INCLUDE_XFLOW_MACSEC */
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

#define MAX_POSSIBLE_NOF_MEMBER_IN_GROUP 256

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

/* *INDENT-OFF* */
/* { */
/**
 *  this is a static array of call back functions that the trunk will call when its composition is changed.
 *  there are 4 call backs, that are called in different contexts, all of them has the same signature.
 *  create_cb is called from the context of bcm_trunk create, it will provide only the trunk_id as a parameter.
 *  destroy_cb is called from the context of bcm_trunk destroy, it will provide only the trunk_id as a parameter.
 *  add_cb is called from the context of bcm_trunk_set and bcm_trunk_member_add, it will provide the trunk_id and logical port of the added port as parameters.
 *  delete_cb is called from the context of bcm_trunk_set and bcm_trunk_member_delete, it will provide the trunk_id and logical port of the deleted port as parameters.
 */
static const dnx_trunk_feature_update_t dnx_trunk_feature_update_cbs[] = {
/** create_cb                           | destroy_cb                     | add_cb                                  | delete_cb              */
    {NULL,                                NULL,                           dnx_trunk_egress_pp_port_update,         dnx_port_pp_deleted_trunk_member_update},
#ifdef INCLUDE_XFLOW_MACSEC
    {NULL,                                dnx_xflow_macsec_trunk_destroy, dnx_xflow_macsec_trunk_member_add,       dnx_xflow_macsec_trunk_member_delete   }
#endif /* INCLUDE_XFLOW_MACSEC */
};
/* } */
/* *INDENT-ON* */

/**
 * BCM APIs
 * {
 */

/**
 * \brief - run all features CBs based on the wanted call back type chosen
 *
 * \param [in] unit - unit number
 * \param [in] cb_type - call back type
 * \param [in] trunk_id - trunk id, to move to CB
 * \param [in] logical_port - logical port, to move to CB
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_features_cbs_run(
    int unit,
    dnx_trunk_feature_update_cb_types_e cb_type,
    bcm_trunk_t trunk_id,
    bcm_port_t logical_port)
{
    int trunk_feature_update_arr_size = sizeof(dnx_trunk_feature_update_cbs) / sizeof(dnx_trunk_feature_update_cbs[0]);
    dnx_trunk_update_cb cb;
    SHR_FUNC_INIT_VARS(unit);

    /** run feature update CBs with the trunk id and the deleted member */
    for (int current_feature = 0; current_feature < trunk_feature_update_arr_size; ++current_feature)
    {
        switch (cb_type)
        {
            case DNX_TRUNK_FEATURE_UPDATE_CB_CREATE:
            {
                cb = dnx_trunk_feature_update_cbs[current_feature].create_cb;
                break;
            }
            case DNX_TRUNK_FEATURE_UPDATE_CB_DESTROY:
            {
                cb = dnx_trunk_feature_update_cbs[current_feature].destroy_cb;
                break;
            }
            case DNX_TRUNK_FEATURE_UPDATE_CB_ADD:
            {
                cb = dnx_trunk_feature_update_cbs[current_feature].add_cb;
                break;
            }
            case DNX_TRUNK_FEATURE_UPDATE_CB_DELETE:
            {
                cb = dnx_trunk_feature_update_cbs[current_feature].delete_cb;
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "cb_type is not valid\n");
            }
        }
        if (cb != NULL)
        {
            SHR_IF_ERR_EXIT(cb(unit, trunk_id, logical_port));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_trunk_system_port_is_spa(
    int unit,
    int system_port,
    int *is_spa)
{
    uint32 spa_mask = 0;
    uint32 spa_type_shift;
    SHR_FUNC_INIT_VARS(unit);

    /** Create mask to verify the SPA. Set "spa_type_shift" bits starting from 0 in bit array "spa_mask" */
    spa_type_shift = dnx_data_trunk.parameters.spa_type_shift_get(unit);
    SHR_BITSET_RANGE(&spa_mask, 0, spa_type_shift);
    if ((system_port & ~spa_mask) != 0)
    {
        *is_spa = TRUE;
    }
    else
    {
        *is_spa = FALSE;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_gport_to_spa_id
 *
 * \param [in] unit - unit number
 * \param [in] trunk_gport - trunk gport
 * \param [out] spa - pointer for result spa
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_gport_to_spa_verify(
    int unit,
    bcm_gport_t trunk_gport,
    uint32 *spa)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(spa, _SHR_E_PARAM, "spa");
    /** verify that gport is trunk gport */
    if (!BCM_GPORT_IS_TRUNK(trunk_gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport type is not supported, use trunk gport\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_gport_to_spa(
    int unit,
    bcm_gport_t trunk_gport,
    int member,
    uint32 *spa)
{
    int pool;
    int group;
    uint32 trunk_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_gport_to_spa_verify(unit, trunk_gport, spa));
    /** get pool and group from gport */
    trunk_id = BCM_GPORT_TRUNK_GET(trunk_gport);
    BCM_TRUNK_ID_GROUP_GET(group, trunk_id);
    BCM_TRUNK_ID_POOL_GET(pool, trunk_id);
    /** build spa */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_encode(unit, pool, group, member, spa));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_spa_to_gport
 *
 * \param [in] unit - unit number
 * \param [in] spa - spa
 * \param [out] member - trunk member
 * \param [out] trunk_gport - pointer for result trunk gport
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_spa_to_gport_verify(
    int unit,
    uint32 spa,
    int *member,
    bcm_gport_t * trunk_gport)
{
    int is_spa;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(member, _SHR_E_PARAM, "member");
    SHR_NULL_CHECK(trunk_gport, _SHR_E_PARAM, "trunk_gport");

    /** Verify that given spa_id is indeed spa. */
    SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(unit, spa, &is_spa));

    if (!is_spa)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid SPA.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_spa_to_gport(
    int unit,
    uint32 spa,
    int *member,
    bcm_gport_t * trunk_gport)
{
    int pool;
    int group;
    uint32 trunk_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_spa_to_gport_verify(unit, spa, member, trunk_gport));
    /** get pool, member and group from SPA */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_decode(unit, &pool, &group, member, spa));

    /** Set the trunk ID and create the trunk Gport */
    BCM_TRUNK_ID_SET(trunk_id, pool, group);
    BCM_GPORT_TRUNK_SET(*trunk_gport, trunk_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for dnx_trunk_spa_member_mask
 *
 * \param [in] unit - unit number
 * \param [in] spa - spa
 * \param [out] spa_with_masked_member - spa with masked member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_spa_member_mask_verify(
    int unit,
    uint32 spa,
    uint32 *spa_with_masked_member)
{
    int is_spa;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(spa_with_masked_member, _SHR_E_PARAM, "spa_with_masked_member");
    SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(unit, spa, &is_spa));
    if (is_spa == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "input SPA: 0x%x is not really an SPA", spa);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_spa_member_mask(
    int unit,
    uint32 spa,
    uint32 *spa_with_masked_member)
{
    int pool;
    int group;
    int member;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_spa_member_mask_verify(unit, spa, spa_with_masked_member));
    /** decode SPA */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_decode(unit, &pool, &group, &member, spa));
    /** encode SPA without member */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_encode(unit, pool, group, 0, spa_with_masked_member));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_spa_id_to_gport(
    int unit,
    uint32 spa_id,
    int *member,
    bcm_gport_t * trunk_gport)
{
    uint32 spa;
    SHR_FUNC_INIT_VARS(unit);

    spa = spa_id | (1 << dnx_data_trunk.parameters.spa_type_shift_get(unit));
    SHR_IF_ERR_EXIT(dnx_trunk_spa_to_gport(unit, spa, member, trunk_gport));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - free trunk's pp ports according to core bitmap. for
 *        each bit set in the core bitmap a pp port for this
 *        core will be freed for given trunk
 *
 * \param [in] unit - unit number
 * \param [in] core_bitmap - core bitmap
 * \param [in] id_info - trunk id info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_pp_ports_free(
    int unit,
    uint32 core_bitmap,
    bcm_trunk_id_info_t * id_info)
{
    uint32 pp_port;
    int current_core;
    SHR_FUNC_INIT_VARS(unit);

    /** get number of cores */
    /** iterate over bitmap up-to nof_cores and do action on relevant cores */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, current_core)
    {
        if ((SAL_BIT(current_core) & core_bitmap) == 0)
        {
            continue;
        }

        /** get this core pp_port */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_get(unit, id_info->pool_index,
                                                          id_info->group_index, current_core, &pp_port));

        /** free pp-port and set it to invalid in trunk group */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_set(unit, id_info->pool_index,
                                                          id_info->group_index, current_core,
                                                          dnx_data_port.general.nof_pp_ports_get(unit)));
        /** disable visibility */
        SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit, current_core, pp_port, FALSE));
        /*
         * Free the pp port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_free(unit, current_core, pp_port));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate trunk's pp ports according to core bitmap.
 *        for each bit set in the core bitmap a pp port for this
 *        core will be allocated for given trunk
 *
 * \param [in] unit - unit number
 * \param [in] core_bitmap - core bitmap
 * \param [in] id_info - trunk id info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_pp_ports_allocate(
    int unit,
    uint32 core_bitmap,
    bcm_trunk_id_info_t * id_info)
{
    uint32 algo_port_flags;
    /** assigning 0 to pp_port just to dismiss a faulty coverity issue - actual value is retrieved by the function */
    uint32 pp_port = 0;
    int current_core;
    bcm_trunk_t tid;
    SHR_FUNC_INIT_VARS(unit);

    /** iterate over bitmap up-to nof_cores and do action on relevant cores */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, current_core)
    {
        if ((SAL_BIT(current_core) & core_bitmap) == 0)
        {
            continue;
        }

        /** allocate pp-port and save it in trunk group */
        algo_port_flags = DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_LAG;
        BCM_TRUNK_ID_SET(tid, id_info->pool_index, id_info->group_index);
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_allocate(unit, algo_port_flags, current_core, tid, &pp_port));
        /** enable visibility */
        SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit, current_core, pp_port, TRUE));
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_set(unit, id_info->pool_index,
                                                          id_info->group_index, current_core, pp_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify funciton for bcm_dnx_trunk_create
 *
 * \param [in] unit - unit number
 * \param [in] flags - flags
 * \param [in] trunk_id - trunk id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_create_verify(
    int unit,
    uint32 flags,
    bcm_trunk_t * trunk_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trunk_id, _SHR_E_PARAM, "trunk_id");
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, *trunk_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - create a trunk, mark in sw db that trunk is used and
 *        allocate a pp port per core.
 *
 * \param [in] unit - unit number
 * \param [in] flags - supported flags are:
 *        BCM_TRUNK_FLAG_DONT_ALLOCATE_PP_PORTS - if this flags
 *        is used when the user wants to manage the pp ports
 *        allocated on his own ( from resources POV ). if used,
 *        he is required to allocate pp ports on each fap he
 *        choose to do so using bcm_trunk_pp_port_allocation_set
 * \param [in] trunk_id - trunk id, the trunk id must be created
 *        using the macro BCM_TRUNK_ID_SET.
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
bcm_dnx_trunk_create(
    int unit,
    uint32 flags,
    bcm_trunk_t * trunk_id)
{
    bcm_trunk_id_info_t id_info;
    int in_use;
    uint32 sw_db_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_create_verify(unit, flags, trunk_id));

    /** get id info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, *trunk_id, &id_info));
    /** check if ID was already created */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_get(unit, id_info.pool_index, id_info.group_index, &in_use));
    if (in_use != 0)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "trunk_id 0x%x was already created\n", *trunk_id);
    }
    /** reset group in sw db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_reset(unit, id_info.pool_index, id_info.group_index, TRUE));
    /** mark group as used */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_set(unit, id_info.pool_index, id_info.group_index, 1));

    /** if needs to allocate pp ports (pp port should not be allocated for remote or virtual lags) */
    if ((flags & (BCM_TRUNK_FLAG_DONT_ALLOCATE_PP_PORTS | BCM_TRUNK_FLAG_VP)) == 0)
    {
        /** allocate on all cores */
        SHR_IF_ERR_EXIT(dnx_trunk_pp_ports_allocate(unit, -1, &id_info));
    }

    /** update flags - flags which are used solely in this API are cleared out to avoid confusion */
    sw_db_flags = flags & ~(BCM_TRUNK_FLAG_DONT_ALLOCATE_PP_PORTS | BCM_TRUNK_FLAG_WITH_ID);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_set(unit, id_info.pool_index, id_info.group_index, sw_db_flags));

    /** run feature update CBs with the created trunk id */
    SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run(unit, DNX_TRUNK_FEATURE_UPDATE_CB_CREATE, *trunk_id, -1));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_pp_port_allocation_set
 *
 * \param [in] unit - uint number
 * \param [in] trunk_id - trunk id
 * \param [in] flags - flags
 * \param [in] allocation_info - allocation info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_pp_port_allocation_set_verify(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 flags,
    bcm_trunk_pp_port_allocation_info_t * allocation_info)
{
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    SHR_NULL_CHECK(allocation_info, _SHR_E_PARAM, "allocation_info");
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - check if any of the ports on the trunk are found in
 *        the recieved core bitmap.
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] core_bitmap - bitmaps of cores
 * \param [in] ports_found_on_cores - return by ptr, set to True
 *        if ports were found on the cores recieved.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_ports_on_cores_check(
    int unit,
    int pool,
    int group,
    uint32 core_bitmap,
    int *ports_found_on_cores)
{
    int nof_members_in_group;
    int member_count;
    int member;
    uint32 invalid_system_port;
    uint32 member_core_id;

    SHR_FUNC_INIT_VARS(unit);

    *ports_found_on_cores = 0;
    invalid_system_port = dnx_data_device.general.invalid_system_port_get(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, pool, group, &nof_members_in_group));
    for (member_count = 0, member = 0; member_count < nof_members_in_group; ++member)
    {
        trunk_group_member_info_t member_info;
        dnx_algo_gpm_gport_phy_info_t phy_gport_info;
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member, &member_info));
        /** if member is not valid continue */
        if (member_info.system_port == invalid_system_port)
        {
            continue;
        }
        ++member_count;

        /** if member is not local continue */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member_info.system_port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));
        if ((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) == 0)
        {
            continue;
        }

        /** if member is found on cores_bitmap return true and exit */
        /** Get core */
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, phy_gport_info.local_port, (bcm_core_t *) & member_core_id));
        if (SAL_BIT(member_core_id) & core_bitmap)
        {
            *ports_found_on_cores = 1;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocate and free pp ports for a trunk according to
 *        the given allocation info. the core bitmap in
 *        allocation info determines to which core a pp port
 *        will be allocate or freed if it is set or cleared. if
 *        a trunk has a pp port on a core that the user provided
 *        in the core bitmap a new pp port will NOT be allocate.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] flags - flags
 * \param [in] allocation_info - allocation info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   an attempt to free a PP port that is currently being used
 *   will fail, the proper sequence is to first remove from the
 *   trunk all the members that use this port. once a new pp port
 *   was added to the trunk, all PP configuration needs to be
 *   repeated with this trunk's gport to achieve a seamless
 *   additions and removals of members from this trunk prior to
 *   actually adding or removing members.
 *   allowed transition between different number of pp ports must
 *   always go through 0 - meaning a transition between
 *   bitmap 0x1 -> 0x2 is Invalid, and has to be done in 2 steps,
 *   0x1 -> 0x0, 0x0 -> 0x2. this principale is used to force user
 *   to perform his needed pp configurations.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_trunk_pp_port_allocation_set(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 flags,
    bcm_trunk_pp_port_allocation_info_t * allocation_info)
{
    int ports_found_on_cores;
    uint32 current_core_bitmap;
    uint32 wanted_core_bitmap;
    uint32 core_bitmap_to_free;
    uint32 core_bitmap_to_alloc;
    bcm_trunk_id_info_t id_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_pp_port_allocation_set_verify(unit, trunk_id, flags, allocation_info));

    /** get id info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** get wanted core bitmap */
    wanted_core_bitmap = allocation_info->core_bitmap;
    /** get current core bitmap */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_core_bitmap_get(unit, id_info.pool_index,
                                                                  id_info.group_index, &current_core_bitmap));
    /** check if allocation is allowed - the transition must be through 0,
     *  meaning the following are allowed:
     *  0x1,0x2,0x3 <-> 0x0.
     *  and the following are forbidden:
     *  0x1 <-> 0x2,  0x1 <-> 0x3, 0x2 <-> 0x3 */
    if ((wanted_core_bitmap != 0) && (current_core_bitmap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Invalid core bitmap (0x%x) received, as a result an attempt to make an invalid transition was made.\n"
                     "transition between current state (0x%x) to new state (0x%x) has to be done in 2 stages:\n"
                     "0x%x -> 0x0, 0x0 -> 0x%x.\nafterwards relevant PP configurations has to be remade\n",
                     wanted_core_bitmap, current_core_bitmap, wanted_core_bitmap, current_core_bitmap,
                     wanted_core_bitmap);
    }

    /** get Deltas between the 2 states, decide which needs to be allocated and which freed */
    core_bitmap_to_alloc = wanted_core_bitmap & ~current_core_bitmap;
    core_bitmap_to_free = current_core_bitmap & ~wanted_core_bitmap;

    /** check that no local ports are found on a core whose pp port about to be freed. */
    /** and free if needed */
    if (core_bitmap_to_free != 0)
    {
        uint32 trunk_in_header_type;
        uint32 trunk_out_header_type;
        bcm_gport_t trunk_gport;

        SHR_IF_ERR_EXIT(dnx_trunk_ports_on_cores_check(unit, id_info.pool_index, id_info.group_index,
                                                       core_bitmap_to_free, &ports_found_on_cores));
        if (ports_found_on_cores)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid core bitmap (0x%x) received, as a result an attempt to free"
                         "a core's pp port when still have local trunk members which"
                         "use this port\n "
                         "remove trunk members from the core you're trying to free\n", wanted_core_bitmap);
        }

        /*
         * See dnx_port_initial_eth_properties_set and dnx_port_pp_egress_set - need to deinit trunk resources
         */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get
                        (unit, id_info.pool_index, id_info.group_index, &trunk_in_header_type, &trunk_out_header_type));
        BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);

        LOG_VERBOSE_EX(BSL_LOG_MODULE,
                       "bcm_dnx_trunk_pp_port_allocation_set: removing members: trunk_gport = 0x%08X, trunk_in_header_type = %d, trunk_out_header_type = %d, core_bitmap_to_free = 0x%08X\n",
                       trunk_gport, trunk_in_header_type, trunk_out_header_type, core_bitmap_to_free);

        if (BCM_SWITCH_PORT_HEADER_TYPE_NONE != trunk_in_header_type)
        {
            SHR_IF_ERR_EXIT(dnx_port_initial_eth_properties_unset(unit, trunk_gport, trunk_in_header_type));
        }

        if (BCM_SWITCH_PORT_HEADER_TYPE_NONE != trunk_out_header_type)
        {
            SHR_IF_ERR_EXIT(dnx_port_pp_egress_unset(unit, trunk_gport, trunk_out_header_type));
        }

        /** Now, free the PP-Ports*/
        SHR_IF_ERR_EXIT(dnx_trunk_pp_ports_free(unit, core_bitmap_to_free, &id_info));
    }

    /** allocate if needed */
    if (core_bitmap_to_alloc != 0)
    {
        uint32 trunk_in_header_type;
        uint32 trunk_out_header_type;
        bcm_gport_t trunk_gport;
        bcm_switch_control_key_t key;
        bcm_switch_control_info_t value;

        SHR_IF_ERR_EXIT(dnx_trunk_pp_ports_allocate(unit, core_bitmap_to_alloc, &id_info));
        /** after allocating, need to set headers again. before doing that,
         * need to check that headers were already assigned to trunk */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get
                        (unit, id_info.pool_index, id_info.group_index, &trunk_in_header_type, &trunk_out_header_type));
        BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);
        key.type = bcmSwitchPortHeaderType;
        if (BCM_SWITCH_PORT_HEADER_TYPE_NONE != trunk_in_header_type)
        {
            key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN;
            value.value = trunk_in_header_type;
            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_set(unit, trunk_gport, key, value));
        }

        if (BCM_SWITCH_PORT_HEADER_TYPE_NONE != trunk_out_header_type)
        {
            key.index = DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT;
            value.value = trunk_out_header_type;
            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_port_set(unit, trunk_gport, key, value));
        }

    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_pp_port_allocation_get
 *
 * \param [in] unit - uint number
 * \param [in] trunk_id - trunk id
 * \param [in] flags - flags
 * \param [in] allocation_info - allocation info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_pp_port_allocation_get_verify(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 flags,
    bcm_trunk_pp_port_allocation_info_t * allocation_info)
{
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    SHR_NULL_CHECK(allocation_info, _SHR_E_PARAM, "allocation_info");
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the pp port allocation info of a given trunk.
 *          this is including which pp port is allocated on which core.
 *          the pp port itself is not usable in other configurations
 *          and should not be accessed directly.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] flags - flags
 * \param [in] allocation_info - allocation info
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
bcm_dnx_trunk_pp_port_allocation_get(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 flags,
    bcm_trunk_pp_port_allocation_info_t * allocation_info)
{
    bcm_trunk_id_info_t id_info;
    int nof_cores;
    int core;
    /** Needed to prevent Coverity issue of
     * Out-of-bounds access (ARRAY_VS_SINGLETON) */
    uint32 core_bitmap[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_pp_port_allocation_get_verify(unit, trunk_id, flags, allocation_info));

    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_core_bitmap_get(unit, id_info.pool_index, id_info.group_index,
                                                                  &(allocation_info->core_bitmap)));
    /** iterate on bitmap and get pp port per core */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    core_bitmap[0] = allocation_info->core_bitmap;
    SHR_BIT_ITER(&core_bitmap[0], nof_cores, core)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_get
                        (unit, id_info.pool_index, id_info.group_index, core,
                         &allocation_info->pp_port_per_core_array[core]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the psc_profile according to bcm_psc, psc_info
 *        and nof_enabled members. if bcm_psc is M&D use
 *        nof_enabled_members(nof_enabled_members - 1). if
 *        bcm_psc is Smooth Division use a logic that shuold be
 *        placed in algo to allow advanced users to change it if
 *        they so choose to but essentially it will be
 *        (nof_enabled_members - 1). if bcm_psc is c-lag use the
 *        psc_info and allocate a profile.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk ID
 * \param [in] bcm_psc - PSC in BCM format
 * \param [in] psc_info - psc info, relevant for c-lag to
 *        allocate the needed profile
 * \param [out] psc_profile - resulted psc profile to be updated
 *        to the HW.
 * \param [out] psc_profile_type - resulted psc profile type if relevant
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_psc_profile_alloc(
    int unit,
    bcm_trunk_t trunk_id,
    int bcm_psc,
    bcm_trunk_psc_profile_info_t * psc_info,
    int *psc_profile,
    int *psc_profile_type)
{
    int arr_size = MAX_POSSIBLE_NOF_MEMBER_IN_GROUP;
    int nof_enabled_members;
    int member_index_arr[MAX_POSSIBLE_NOF_MEMBER_IN_GROUP];
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_enabled_members_get
                    (unit, id_info.pool_index, id_info.group_index, arr_size, &nof_enabled_members, member_index_arr));

    switch (bcm_psc)
    {
        case BCM_TRUNK_PSC_USER_PROFILE:
        {
            int user_profile = psc_info->profile_id;
            /** increase ref count for profile */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_ref_count_increment(unit, user_profile));
            *psc_profile = psc_info->profile_id;
            break;
        }
        case BCM_TRUNK_PSC_C_LAG:
        {
            const uint32 HIRARCHY0 = 0;
            const uint32 SINGLE_PROFILE = 1;
            const uint32 FIRST_INDEX = 0;
            uint32 chm_handle;
            int psc;
            uint32 member_id_arr[MAX_POSSIBLE_NOF_MEMBER_IN_GROUP];
            trunk_group_member_info_t member_info;
            profile_assign_user_info_t user_info;
            /** get chm_handle from sw-state */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_chm_handle_get(unit, &chm_handle));
            /** get required profile type*/
            SHR_IF_ERR_EXIT(dnx_trunk_c_lag_profile_type_get
                            (unit, psc_info->max_nof_members_in_profile, psc_profile_type));
            /** allocate a profile in CHM */
            user_info.max_nof_members_in_profile = psc_info->max_nof_members_in_profile;
            user_info.profile_type = *psc_profile_type;
            /** Get all active member indexes from trunk from SW DB to support specific members addition for C-Lag as well */
            for (int arr_index = 0; arr_index < nof_enabled_members; ++arr_index)
            {
                SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get
                                (unit, id_info.pool_index, id_info.group_index, member_index_arr[arr_index],
                                 &member_info));
                member_id_arr[arr_index] = member_info.index;
            }
            SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                            (unit, chm_handle, trunk_id, HIRARCHY0, *psc_profile_type, SINGLE_PROFILE, FIRST_INDEX,
                             psc_info->max_nof_members_in_profile, nof_enabled_members, member_id_arr, NULL,
                             (void *) &user_info));

            /** get psc_profile from trunk - profile_alloc above should've took care of filling the sw db */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get
                            (unit, id_info.pool_index, id_info.group_index, &psc, psc_profile));
            break;
        }
        case BCM_TRUNK_PSC_SMOOTH_DIVISION:
        {
            SHR_IF_ERR_EXIT(dnx_algo_trunk_smooth_division_profile_set(unit, psc_info,
                                                                       nof_enabled_members, psc_profile));
            *psc_profile_type = TRUNK_INVALID_PROFILE;
            break;
        }
        case BCM_TRUNK_PSC_PORTFLOW:
        {
            *psc_profile = (nof_enabled_members == 0) ? 0 : nof_enabled_members - 1;
            *psc_profile_type = TRUNK_INVALID_PROFILE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid PSC\n");
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get psc_info according to the
 *        bcm_psc and the psc profile. for M&D this function is
 *        not relevant for it has no psc_info, for Smooth
 *        Division return according to logic found in algo that
 *        unless changed by users  is currently not relevant for
 *        it has no psc_info, for c-lag return
 *        psc_info.
 *
 * \param [in] unit - unit number
 * \param [in] bcm_psc - PSC in BCm format
 * \param [in] psc_profile - psc profile in the HW
 * \param [out] psc_info - retrieved psc info
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_psc_info_get(
    int unit,
    int bcm_psc,
    int psc_profile,
    bcm_trunk_psc_profile_info_t * psc_info)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (bcm_psc)
    {
        case BCM_TRUNK_PSC_USER_PROFILE:
        {
            psc_info->profile_id = psc_profile;
            psc_info->psc_flags = BCM_TRUNK_PSC_WITH_ID;
            break;
        }
        case BCM_TRUNK_PSC_C_LAG:
        {
            int profile_type;
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_get
                            (unit, psc_profile, &profile_type, &psc_info->max_nof_members_in_profile));
            break;
        }
        case BCM_TRUNK_PSC_SMOOTH_DIVISION:
        {
            SHR_IF_ERR_EXIT(dnx_algo_trunk_smooth_division_profile_get(unit, psc_profile, psc_info));
            break;
        }
        case BCM_TRUNK_PSC_PORTFLOW:
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "PSC M&D doesn't have any psc_info\n")));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid PSC\n");
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_set for virtual trunks
 *
 * \param [in] unit - uint number
 * \param [in] trunk_id - trunk id
 * \param [in] trunk_info - trunk info
 * \param [in] member_count - nof members in array
 * \param [in] member_array - array of members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_set_virtual_verify(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_info_t * trunk_info,
    int member_count,
    bcm_trunk_member_t * member_array)
{
    bcm_trunk_id_info_t master_id_info;
    bcm_trunk_info_t master_trunk_info;
    int master_member_count;
    bcm_trunk_member_t *master_member_array = NULL;
    int master_nof_members, member_i;

    SHR_FUNC_INIT_VARS(unit);

    /** Check master trunk id is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_info->master_tid));

    /** get master id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_info->master_tid, &master_id_info));

    /** Check master trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &master_id_info));

    /** verify virtual trunk and master trunk match (members mapping, psc, amount of members, etc...) */
    {
        /** Get number of members in master trunk */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, master_id_info.pool_index,
                                                                 master_id_info.group_index, &master_nof_members));

        /** allocate master trunk members array according to number of members */
        SHR_ALLOC(master_member_array, master_nof_members * sizeof(bcm_trunk_member_t), "master trunk members array",
                  "%s%s%s", EMPTY, EMPTY, EMPTY);

        /** get master trunk info*/
        SHR_IF_ERR_EXIT(bcm_dnx_trunk_get(unit, trunk_info->master_tid, &master_trunk_info, master_nof_members,
                                          master_member_array, &master_member_count));

        /** verify members in virtual trunk are subset of physical destinations (modports) in master trunk */
        for (member_i = 0; member_i < member_count; member_i++)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_modport_membership_verify(unit, member_array[member_i].gport,
                                                                master_nof_members, master_member_array));
        }
    }

exit:
    SHR_FREE(master_member_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_set. the point in
 *        this verify function is to check in advance that the
 *        changes can be made to the trunk and fail if not
 *        before the changes began. this is cruicial in the
 *        trunk because it is a system feature and not local to
 *        a single fap
 *
 * \param [in] unit - uint number
 * \param [in] trunk_id - trunk id
 * \param [in] trunk_info - trunk info
 * \param [in] member_count - nof members in array
 * \param [in] member_array - array of members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_set_verify(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_info_t * trunk_info,
    int member_count,
    bcm_trunk_member_t * member_array)
{
    bcm_trunk_id_info_t id_info;
    uint32 trunk_flags;
    int profile_change_only;
    int psc_is_user_profile;
    SHR_FUNC_INIT_VARS(unit);

    /** Check trunk id is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** Check Null pointers */
    SHR_NULL_CHECK(trunk_info, _SHR_E_PARAM, "trunk_info");
    if (member_count > 0)
    {
        SHR_NULL_CHECK(trunk_info, _SHR_E_PARAM, "member_array");
    }

    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** Check that number of members can fit into group */
    SHR_IF_ERR_EXIT(dnx_trunk_can_fit_new_amount_of_members_verify(unit, &id_info, member_count));
    /** Check that PSC is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_psc_verify(unit, trunk_info->psc, &trunk_info->psc_info, &id_info));
    /** Check if changing profiles is valid */
    profile_change_only = _SHR_IS_FLAG_SET(trunk_info->flags, BCM_TRUNK_FLAG_PROFILE_CHANGE_ONLY);
    psc_is_user_profile = (trunk_info->psc == BCM_TRUNK_PSC_USER_PROFILE);
    if (profile_change_only && !psc_is_user_profile)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "changing profile only is supported only with User Profile PSC\n");
    }
    else if (profile_change_only && psc_is_user_profile && member_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "member_count is expected to be 0 when only changing profile\n");
    }

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, id_info.pool_index, id_info.group_index, &trunk_flags));

    /** verify */
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        SHR_INVOKE_VERIFY_DNXC(dnx_trunk_set_virtual_verify(unit, trunk_id, trunk_info, member_count, member_array));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - this function is used to manage the allocated
 *        profiles of certain PSCs (currently only c-lag) this
 *        function will free unused profiles. if another lag is
 *        using the profile do nothing.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk_id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_psc_profile_free(
    int unit,
    bcm_trunk_t trunk_id)
{
    bcm_trunk_id_info_t id_info;
    int pool;
    int group;
    int old_psc;
    int old_psc_profile;
    SHR_FUNC_INIT_VARS(unit);

    /** Get ID info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    /** at this point sw db should still contain the old info */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &old_psc, &old_psc_profile));

    if (old_psc == BCM_TRUNK_PSC_C_LAG)
    {
        uint32 chm_handle;
        uint8 is_last;
        int old_psc_profile_type;
        int old_psc_max_nof_members;

        /** get chm_handle from sw-state */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_chm_handle_get(unit, &chm_handle));
        /** get profile type from sw-state */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_get
                        (unit, old_psc_profile, &old_psc_profile_type, &old_psc_max_nof_members));

        /** free profile in CHM */
        SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free
                        (unit, chm_handle, trunk_id, old_psc_profile_type));

        /** free profile in template manager */
        SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_destroy(unit, old_psc_profile, &is_last));
        if (is_last)
        {
            /** clear profile data */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_set(unit, old_psc_profile, TRUNK_INVALID_PROFILE, -1));
        }
    }
    else if (old_psc == BCM_TRUNK_PSC_USER_PROFILE)
    {
        /** reduce ref count for freed profile */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_ref_count_decrement(unit, old_psc_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify that a trunk has a pp port on given core, if
 *        it doesn't (recieved pp port will be max nof pp ports)
 *        return an error.
 *
 * \param [in] unit - unit number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] core - core number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_has_a_pp_port_on_core_verify(
    int unit,
    int pool,
    int group,
    int core)
{
    uint32 pp_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_get(unit, pool, group, core, &pp_port));
    if (pp_port == dnx_data_port.general.nof_pp_ports_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "current trunk doesn't have a dedicated pp port"
                     "for core %d, on unit %d\n use API bcm_trunk_pp_port_allocation_set"
                     "and try again\n", core, unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - check that a member can be added with set API, check
 *        collision with other trunks and with the flags of the
 *        member
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] psc - psc of the trunk
 * \param [in] added_member - info of the added member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_member_can_be_added_with_set_verify(
    int unit,
    bcm_trunk_t trunk_id,
    int psc,
    bcm_trunk_member_t * added_member)
{
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags;
    uint32 system_port;
    int system_port_is_not_a_member_of_another_trunk;
    int system_port_is_already_a_member_of_current_trunk;
    int added_member_is_enabled;
    int current_member_is_enabled;
    algo_gpm_gport_verify_type_e allowed_port_infos[] = { ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT };
    bcm_trunk_id_info_t id_info;

    SHR_FUNC_INIT_VARS(unit);

    /** check validity of system port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, added_member->gport, COUNTOF(allowed_port_infos), allowed_port_infos));

    /** check validity of flags - including overruning legacy flags with new ones */
    SHR_IF_ERR_EXIT(dnx_trunk_member_flags_verify(unit, &(added_member->flags)));

    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(added_member->gport);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, system_port, &curr_trunk_id, &curr_flags));

    /** define conditions under which the addition of a member is valid */
    system_port_is_not_a_member_of_another_trunk = (curr_trunk_id == TRUNK_SW_DB_INVALID_TRUNK_ID);
    system_port_is_already_a_member_of_current_trunk = (curr_trunk_id == trunk_id);
    added_member_is_enabled = ((added_member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0);
    current_member_is_enabled = ((curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0);

    if ((system_port_is_not_a_member_of_another_trunk) ||
        (system_port_is_already_a_member_of_current_trunk && added_member_is_enabled && current_member_is_enabled))
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "this member (system port = 0x%x) is valid,"
                                                " and can be added to trunk.\n"), system_port));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid configuration, either member is already found in another trunk, or a"
                     "missconfiguration was detected with member flags");
    }

    /** check that trunk and new member has the same header_type if it is not already part of this trunk*/
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    if (!system_port_is_already_a_member_of_current_trunk)
    {
        /** check for matching header types */
        SHR_IF_ERR_EXIT(dnx_trunk_new_member_has_matching_header_type_verify(unit, &id_info, added_member->gport));
    }

    /** verify validity of spa_id in case BCM_TRUNK_MEMBER_SPA_WITH_ID was provided*/
    SHR_IF_ERR_EXIT(dnx_trunk_member_spa_with_id_verify(unit, added_member, id_info.pool_index, id_info.group_index));

    /** verify validity of member_id in case BCM_TRUNK_MEMBER_WITH_ID was provided */
    SHR_IF_ERR_EXIT(dnx_trunk_member_with_id_verify(unit, added_member, psc, id_info.pool_index, id_info.group_index));

    /** verify backup member for members with protection */
    if (added_member->flags & BCM_TRUNK_MEMBER_WITH_PROTECTION)
    {
        bcm_trunk_member_t backup_member;
        sal_memset(&backup_member, 0, sizeof(backup_member));
        backup_member.gport = added_member->backup_member.gport;
        backup_member.flags = added_member->backup_member.flags;
        backup_member.spa_id = added_member->backup_member.spa_id;
        SHR_IF_ERR_EXIT(dnx_trunk_backup_member_verify(unit, trunk_id, &backup_member));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - change trunk profile without changing its member composition.
 *          This is applicable only for USER_PROFILE PSC
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] trunk_info - trunk info containing the PSC info and PSC type
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_profile_only_change(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_info_t * trunk_info)
{
    bcm_trunk_id_info_t id_info;
    int new_psc_profile = 0;
    int dummy_new_psc_profile_type = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get ID info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));

    /** Allocation and set to dbal has to come before clean-up to prevent disruption to traffic */
    /** Alloc new PSC resources, dummy inputs and outs are used since this is relevant only in User Profile context */
    SHR_IF_ERR_EXIT(dnx_trunk_psc_profile_alloc(unit, trunk_id, trunk_info->psc, &trunk_info->psc_info,
                                                &new_psc_profile, &dummy_new_psc_profile_type));
    /** Set to dbal */
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_attributes_set
                    (unit, id_info.pool_index, id_info.group_index, trunk_info->psc, new_psc_profile));

    /** old psc cleaning has to come before sw db update since values for clean-up are taken from sw db */
    /** Clean old psc */
    SHR_IF_ERR_EXIT(dnx_trunk_psc_profile_free(unit, trunk_id));
    /** Update sw db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_set
                    (unit, id_info.pool_index, id_info.group_index, trunk_info->psc, new_psc_profile));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_add_port_to_pp(
    int unit,
    bcm_trunk_t trunk_id,
    trunk_group_member_info_t * member_info)
{
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    bcm_trunk_id_info_t id_info;
    int pool;
    int group;
    uint32 trunk_flags;
    uint32 trunk_in_header_type;
    uint32 trunk_out_header_type;
    uint32 flags;
    bcm_core_t core;
    const uint32 IS_LAG = 1;
    int port_in_header_type = BCM_SWITCH_PORT_HEADER_TYPE_NONE;
    int port_out_header_type = BCM_SWITCH_PORT_HEADER_TYPE_NONE;
    uint32 tcam_access_id = 0;
    uint32 prt_tcam_key = 0;
    uint32 coe_pp_port;
    bcm_core_t coe_core;
    uint32 spa;
    uint32 pp_port;
    SHR_FUNC_INIT_VARS(unit);

    /** get system port info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, member_info->system_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    /** get trunk flags */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));

    /** add member to PP part of the trunk */
    /** check if member is local to this unit and trunk is not virtual */
    if (!(phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT))
    {
        /** port is not local to this FAP, get out */
        SHR_EXIT();
    }
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        /** Trunk is virtual, get out */
        SHR_EXIT();
    }

    /** Get core from local port */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, phy_gport_info.local_port, &core));
    SHR_IF_ERR_EXIT(dnx_trunk_has_a_pp_port_on_core_verify(unit, pool, group, core));

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get(unit, pool, group, &trunk_in_header_type, &trunk_out_header_type));

    LOG_VERBOSE_EX(BSL_LOG_MODULE,
                   "dnx_trunk_add_port_to_pp: adding member: local_port = %d, pp_port = %d, core_id = %d %s\n",
                   phy_gport_info.local_port, phy_gport_info.internal_port_pp_info.pp_port[0],
                   phy_gport_info.internal_port_pp_info.core_id[0], EMPTY);

    SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, phy_gport_info.local_port, &flags));
    if (DNX_PORT_IS_COE_PORT(flags))
    {
        int port_in_lag;
        uint32 key_value = 0;
        SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_access_id_get_by_port
                        (unit, phy_gport_info.local_port, DBAL_FIELD_PRTTCAM_KEY_VP_COE, &prt_tcam_key, &coe_core,
                         &tcam_access_id));
        SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, phy_gport_info.local_port, &port_in_lag));
        /**COE port already in the LAG, process for duplicate member*/
        if (port_in_lag)
        {
            SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_get
                            (unit, phy_gport_info.local_port, (uint32 *) &coe_core, &coe_pp_port, &tcam_access_id,
                             &key_value));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, phy_gport_info.local_port, &coe_core, &coe_pp_port));
        }
    }

    /** Get the incoming port header type of phy_gport_info.local_port */
    SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                    (unit, phy_gport_info.local_port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN, &port_in_header_type));

    /** Get the outgoing port header type of phy_gport_info.local_port */
    SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                    (unit, phy_gport_info.local_port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT, &port_out_header_type));

    /** SW and HW map the new member with the lag PP port and SPA as system port */
    /** SPA is encoded using the spa member id assigned to this member */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_encode(unit, pool, group, member_info->spa_member_id, &spa));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_get(unit, pool, group, core, &pp_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_modify(unit, phy_gport_info.local_port, core, pp_port));
    SHR_IF_ERR_EXIT(dnx_port_pp_mapping_set(unit, phy_gport_info.local_port, pp_port, spa, IS_LAG));

    /** See dnx_port_initial_eth_properties_set  - need to deinit pp_port resources */
    SHR_IF_ERR_EXIT(dnx_port_initial_eth_properties_per_pp_port_unset
                    (unit, phy_gport_info.internal_port_pp_info.pp_port[0],
                     phy_gport_info.internal_port_pp_info.core_id[0], port_in_header_type, TRUE));

    /** See dnx_port_pp_egress_set - need to deinit pp_port resources */
    SHR_IF_ERR_EXIT(dnx_port_pp_egress_per_pp_port_unset
                    (unit, phy_gport_info.internal_port_pp_info.pp_port[0],
                     phy_gport_info.internal_port_pp_info.core_id[0], port_out_header_type, TRUE, TRUE));

    /** disable visibility */
    SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set
                    (unit, phy_gport_info.internal_port_pp_info.core_id[0],
                     phy_gport_info.internal_port_pp_info.pp_port[0], FALSE));
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_free
                    (unit, phy_gport_info.internal_port_pp_info.core_id[0],
                     phy_gport_info.internal_port_pp_info.pp_port[0]));

    /** override pp_port in phy_gport_info with the trunk pp_port */
    phy_gport_info.internal_port_pp_info.pp_port[0] = pp_port;
    phy_gport_info.internal_port_pp_info.core_id[0] = core;
    phy_gport_info.internal_port_pp_info.nof_pp_ports = 1;
    /** map new member PTC to PP port according to trunk's header type */
    SHR_IF_ERR_EXIT(dnx_port_pp_prt_ptc_profile_internal_set(unit, &phy_gport_info, trunk_in_header_type));

    /** run feature update CBs with the added member and trunk id, after the member was added to pp */
    SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run
                    (unit, DNX_TRUNK_FEATURE_UPDATE_CB_ADD, trunk_id, phy_gport_info.local_port));

    /** handle coe */
    SHR_IF_ERR_EXIT(dnx_port_sit_coe_port_to_trunk_group_add
                    (unit, phy_gport_info.local_port, trunk_id, coe_core, coe_pp_port, spa, tcam_access_id,
                     prt_tcam_key));

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_delete_port_from_pp(
    int unit,
    bcm_trunk_t trunk_id,
    trunk_group_member_info_t * member_info)
{
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    bcm_trunk_id_info_t id_info;
    int pool;
    int group;
    uint32 trunk_in_header_type;
    uint32 trunk_out_header_type;
    bcm_switch_control_info_t in_header_type;
    bcm_switch_control_info_t out_header_type;
    bcm_switch_control_key_t key;
    const uint32 NOT_LAG = 0;
    uint32 trunk_flags;
    bcm_core_t core;
    uint32 pp_port;
    uint32 system_port_id;
    SHR_FUNC_INIT_VARS(unit);

    /** get system port info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, member_info->system_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    /** get trunk flags */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));

    /** remove member from PP part of the trunk */
    /** check if member is local to this unit and trunk is not virtual */
    if (!(phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT))
    {
        /** port is not local to this FAP, get out */
        SHR_EXIT();
    }
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        /** Trunk is virtual, get out */
        SHR_EXIT();
    }

    /** Get core from local port */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, phy_gport_info.local_port, (bcm_core_t *) & core));
    /** allocate new pp port to removed port */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_allocate(unit, 0, core, 0, &pp_port));
    /** enable visibility */
    SHR_IF_ERR_EXIT(dnx_visibility_pp_port_enable_set(unit, core, pp_port, TRUE));
    /** sw and hw map removed port to new pp port and its original system port */
    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_modify(unit, phy_gport_info.local_port, core, pp_port));
    system_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(member_info->system_port);
    SHR_IF_ERR_EXIT(dnx_port_pp_mapping_set(unit, phy_gport_info.local_port, pp_port, system_port_id, NOT_LAG));
    /** map removed member ptc to pp port according to trunk's header type */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get(unit, pool, group, &trunk_in_header_type, &trunk_out_header_type));
    /** set removed member header types */
    key.type = bcmSwitchPortHeaderType;
    /** in direction*/
    key.index = 1;
    in_header_type.value = trunk_in_header_type;
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, phy_gport_info.local_port, key, in_header_type));
    /** out direction*/
    key.index = 2;
    out_header_type.value = trunk_out_header_type;
    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set(unit, phy_gport_info.local_port, key, out_header_type));
    /** run feature update CBs with the removed member and trunk id, after the member was removed from pp */
    SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run
                    (unit, DNX_TRUNK_FEATURE_UPDATE_CB_DELETE, trunk_id, phy_gport_info.local_port));
    /** handle coe stuff */
    SHR_IF_ERR_EXIT(dnx_port_sit_coe_port_to_trunk_group_del(unit, phy_gport_info.local_port, trunk_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set trunk with members and PSC. possible PSCs are:
 *        M&D (multiply and divide) - chosen by setting
 *          BCM_TRUNK_PSC_PORTFLOW as PSC. using this PSC is
 *          possible for trunks of all sizes and will divide
 *          traffic between members according to a hashing
 *          polynomial chosen by the user. this PSC will not hash
 *          consistently when changing the number of members.
 *        Smooth Division - chosen by setting
 *          BCM_TRUNK_PSC_SMOOTH_DIVISION as PSC. using this PSC
 *          is possible for trunks with up to 16 members. traffic
 *          is hashed according to the lb-key to predefined
 *          members. the default state is 16 profiles with equal
 *          distribution for the members. this PSC will remain
 *          partially consistent when changing number of members -
 *          only for one of the members in the trunk the hashing
 *          will be changed
 *        C-LAG - chosen by setting BCM_TRUNK_PSC_C_LAG as PSC.
 *          this PSC will remain consistent when changing number of
 *          members - only for changed members in the trunk the hashing
 *          will be changed
 *
 *        possible member flag is
 *        BCM_TRUNK_MEMBER_EGRESS_DISABLE - this flag will make
 *        the member not to be added to the TM tables of the
 *        trunk and as a result traffic will not be routed to
 *        this port in the trunk. usually members can have
 *        replications - meaning the same system port appears in
 *        several entries - however this is not the case when
 *        using this flag, under those circumstances only a
 *        single replication can be used.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] trunk_info - trunk info
 * \param [in] member_count - number of members in array
 * \param [in] member_array - array of members to put in the
 *        trunk
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
bcm_dnx_trunk_set(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_info_t * trunk_info,
    int member_count,
    bcm_trunk_member_t * member_array)
{
    bcm_trunk_id_info_t id_info;
    int nof_cores;
    int core;
    int pool;
    int group;
    int member_index;
    int remaining_members;
    int max_nof_members_in_group;
    int new_psc_profile = 0;
    int new_psc_profile_type;
    int nof_enabled_members;
    int sw_db_member_index;
    int invalid_system_port;
    int max_nof_members;
    uint32 member_system_port;
    uint32 member_flags, trunk_flags;
    uint32 core_bitmap[1];
    uint32 pp_ports[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    trunk_group_member_info_t member_info;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    bcm_trunk_t current_member_trunk;
    uint32 current_member_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** stacking trunk */
    if (_SHR_TRUNK_ID_IS_STACKING(trunk_id))
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_set(unit, trunk_id, member_count, member_array));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_set_verify(unit, trunk_id, trunk_info, member_count, member_array));

    /** Get ID info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;
    /** Get pp ports */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_core_bitmap_get(unit, pool, group, core_bitmap));
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    SHR_BIT_ITER(core_bitmap, nof_cores, core)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_get(unit, pool, group, core, &pp_ports[core]));
    }

    /** handle virtual trunks */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        /** update db with master trunk id */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_master_trunk_id_set(unit, pool, group, trunk_info->master_tid));
    }

    /** handle changing profile only */
    if (_SHR_IS_FLAG_SET(trunk_info->flags, BCM_TRUNK_FLAG_PROFILE_CHANGE_ONLY))
    {
        SHR_IF_ERR_EXIT(dnx_trunk_profile_only_change(unit, trunk_id, trunk_info));
        SHR_EXIT();
    }

    /** clean old members */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, pool, group, &remaining_members));
    max_nof_members_in_group = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    invalid_system_port = dnx_data_device.general.invalid_system_port_get(unit);
    for (member_index = 0; (member_index < max_nof_members_in_group) && (remaining_members); ++member_index)
    {
        /** get member */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member_index, &member_info));
        /** check if member is valid - a member that is invalid means that another replication
         *  of the same member was already cleaned, thus the actions below are not required */
        if (member_info.system_port == invalid_system_port)
        {
            continue;
        }
        /** change member TM mapping to invalid system port - do this for all members that has a TM mapping */
        if (member_info.index >= 0)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set
                            (unit, pool, group, member_info.index, invalid_system_port));
            if (dnx_data_trunk.protection.feature_get(unit, dnx_data_trunk_protection_backup_destination))
            {
                SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_protection_members_set
                                (unit, pool, group, member_info.index, invalid_system_port));
            }
        }
        /** check if member is local and trunk is not virtual */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member_info.system_port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

        /** delete from pp only if this is the first replication of this
         *  port, this can be identified by checking if the
         *  trunk_sw_db_trunk_system_port is still trunk id or if it was already updated to invalid */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, phy_gport_info.sys_port,
                                                                 &current_member_trunk, &current_member_flags));
        if (current_member_trunk == trunk_id)
        {
            /** this means that this is the first replication of this member (identified by system port) - so we need to delete from pp */
            SHR_IF_ERR_EXIT(dnx_trunk_delete_port_from_pp(unit, trunk_id, &member_info));

            /** update member in trunk system port db to invalid */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_set
                            (unit, phy_gport_info.sys_port, TRUNK_SW_DB_INVALID_TRUNK_ID, 0));
        }
        else if (current_member_trunk != TRUNK_SW_DB_INVALID_TRUNK_ID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected member's trunk ID to be either: \n"
                         "\t invalid: 0x%x,\n "
                         "\t or input trunk_id: 0x%x. \n"
                         "instead received value: 0x%x", TRUNK_SW_DB_INVALID_TRUNK_ID, trunk_id, current_member_trunk);
        }
        --remaining_members;
    }

    /** change PSC to M&D with 0 members - as a transition state that should allow freeing PSC resources and such */
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_attributes_set(unit, pool, group, BCM_TRUNK_PSC_PORTFLOW, 0));
    /** clean old psc */
    SHR_IF_ERR_EXIT(dnx_trunk_psc_profile_free(unit, trunk_id));
    /** reset group in sw db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_reset(unit, pool, group, FALSE));
    /** prepare group sw db for new group */
    if (trunk_info->psc == BCM_TRUNK_PSC_C_LAG)
    {
        max_nof_members = trunk_info->psc_info.max_nof_members_in_profile;
    }
    else
    {
        max_nof_members = max_nof_members_in_group;
    }
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_res_manager_size_limit(unit, pool, group, max_nof_members));

    /** set new members */
    for (nof_enabled_members = 0, member_index = 0; member_index < member_count; ++member_index)
    {
        bcm_trunk_t curr_trunk_id;
        uint32 curr_flags;
        int member_is_first_replication = 0;

        /** Get system port info */
        member_system_port = member_array[member_index].gport;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member_system_port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

        SHR_IF_ERR_EXIT(dnx_trunk_member_can_be_added_with_set_verify
                        (unit, trunk_id, trunk_info->psc, &member_array[member_index]));

        /** has to come after dnx_trunk_member_can_be_added_with_set_verify, because it might change the member flags */
        member_flags = member_array[member_index].flags;

        /** check if this is the first replication of this member,
         *  if it is - its system port shouldn't be mapped to the trunk_id yet.
         *  this has to come before adding the member to trunk SW DB */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, phy_gport_info.sys_port,
                                                                 &curr_trunk_id, &curr_flags));
        member_is_first_replication = (curr_trunk_id != trunk_id);
        /** Update member to trunk sw db */
        dnx_trunk_sw_db_member_init(unit, &member_info);
        member_info.system_port = member_system_port;
        member_info.flags = member_flags;
        /** will be used only if BCM_TRUNK_MEMBER_SPA_WITH_ID was provided, otherwise will be overridden */
        member_info.spa_member_id = member_array[member_index].spa_id;
        if ((member_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) != 0)
        {
            member_info.index = INVALID_TM_INDEX_TO_INDICATE_DISABLED_MEMBER;
        }
        else if (member_flags & BCM_TRUNK_MEMBER_WITH_ID)
        {
            /** this option is applicable only for c-lag and user_profile PSCs, check done in verify func */
            member_info.index = member_array[member_index].member_id;
        }
        else if ((trunk_info->psc == BCM_TRUNK_PSC_C_LAG) || (trunk_info->psc == BCM_TRUNK_PSC_USER_PROFILE))
        {
            /** for those PSCs, if specific ID was not specified, need alloc manager to find an empty index and use it */
            member_info.index = INVALID_TM_INDEX_BUT_NEED_ALLOCATE_ONE;
        }
        else
        {
            member_info.index = nof_enabled_members;
            ++nof_enabled_members;
        }
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_add(unit, pool, group, &member_info, &sw_db_member_index));

        /** check if member is local to this unit and that this is the first replication,
         *  in that case need to do PP part of adding trunk */
        if (member_is_first_replication)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_add_port_to_pp(unit, trunk_id, &member_info));
        }

        /** Change TM mapping for enabled members */
        if ((member_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group, member_info.index,
                                                                    phy_gport_info.sys_port));
        }

        /** if member has protection, add protection using internal API */
        if (member_array[member_index].flags & BCM_TRUNK_MEMBER_WITH_PROTECTION)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_member_protection_add
                            (unit, pool, group, sw_db_member_index, &member_array[member_index]));
        }
    }
    /** Update last member added index */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_last_member_added_set(unit, pool, group, member_index - 1));

    /** set new PSC */
    SHR_IF_ERR_EXIT(dnx_trunk_psc_profile_alloc(unit, trunk_id, trunk_info->psc, &trunk_info->psc_info,
                                                &new_psc_profile, &new_psc_profile_type));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_set(unit, pool, group, trunk_info->psc, new_psc_profile));
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_attributes_set(unit, pool, group, trunk_info->psc, new_psc_profile));

    if (dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_is_supported))
    {
        /** Align Egress trunk profile's LB Keys distribution with trunk members */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_align(unit, trunk_id));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_get
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] trunk_info - trunk info
 * \param [in] member_max - member array size
 * \param [in] member_array - member array
 * \param [in] member_count - nof retrieved members
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_get_verify(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_info_t * trunk_info,
    int member_max,
    bcm_trunk_member_t * member_array,
    int *member_count)
{
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Check trunk id is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** Check Null pointers */
    SHR_NULL_CHECK(trunk_info, _SHR_E_PARAM, "trunk_info");
    SHR_NULL_CHECK(member_array, _SHR_E_PARAM, "member_array");
    SHR_NULL_CHECK(member_count, _SHR_E_PARAM, "member_count");
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get trunk info and members
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] trunk_info - trunk info
 * \param [in] member_max - max members to get
 * \param [out] member_array - array for retrived members
 * \param [out] member_count - nof retrieved members
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
bcm_dnx_trunk_get(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_info_t * trunk_info,
    int member_max,
    bcm_trunk_member_t * member_array,
    int *member_count)
{
    int pool;
    int group;
    int psc_profile_id;
    int max_nof_member_in_group;
    int invalid_system_port;
    int arr_index = 0;
    bcm_trunk_id_info_t id_info;
    trunk_group_member_info_t member_info;
    uint32 trunk_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** stacking trunk */
    if (_SHR_TRUNK_ID_IS_STACKING(trunk_id))
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_get(unit, trunk_id, member_max, member_array, member_count));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_get_verify(unit, trunk_id, trunk_info, member_max, member_array, member_count));

    /** Get ID info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    /** Get PSC */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &trunk_info->psc, &psc_profile_id));
    SHR_IF_ERR_EXIT(dnx_trunk_psc_info_get(unit, trunk_info->psc, psc_profile_id, &trunk_info->psc_info));
    /** iterate over members and get them */
    max_nof_member_in_group = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    invalid_system_port = dnx_data_device.general.invalid_system_port_get(unit);
    *member_count = 0;
    for (int member = 0; member < max_nof_member_in_group; ++member)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member, &member_info));
        /** Skip invalid members */
        if (member_info.system_port == invalid_system_port)
        {
            continue;
        }

        /** Skip members that are only protecting */
        if (member_info.index == INVALID_TM_INDEX_TO_INDICATE_MEMBER_IS_ONLY_PROTECTING)
        {
            continue;
        }

        if (*member_count < member_max)
        {
            member_array[arr_index].gport = member_info.system_port;
            member_array[arr_index].flags = member_info.flags;
            member_array[arr_index].spa_id = member_info.spa_member_id;
            member_array[arr_index].member_id = member_info.index;
            member_array[arr_index].backup_member.gport = member_info.protecting_system_port;
            ++arr_index;
        }
        ++(*member_count);
    }

    /** get trunk flags */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));
    trunk_info->flags = trunk_flags;

    /** get master trunk in case of virtual trunk */
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_master_trunk_id_get(unit, pool, group, &trunk_info->master_tid));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_member_add for virtual trunks
 *
 * \param [in] unit - unit number
 * \param [in] id_info - trunk id info
 * \param [in] member - info of added member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_virtual_trunk_member_add_verify(
    int unit,
    bcm_trunk_id_info_t * id_info,
    bcm_trunk_member_t * member)
{
    int pool, group;
    bcm_trunk_t master_trunk_id;
    bcm_trunk_member_t *master_member_array = NULL;
    int master_nof_members;
    bcm_trunk_id_info_t master_id_info;
    int master_member_count;
    bcm_trunk_info_t master_trunk_info;

    SHR_FUNC_INIT_VARS(unit);

    pool = id_info->pool_index;
    group = id_info->group_index;

    /** get master trunk id */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_master_trunk_id_get(unit, pool, group, &master_trunk_id));

    /** get master id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, master_trunk_id, &master_id_info));

    /** Get number of members in master trunk */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, master_id_info.pool_index,
                                                             master_id_info.group_index, &master_nof_members));

    /** allocate master trunk members array according to number of members */
    SHR_ALLOC(master_member_array, master_nof_members * sizeof(bcm_trunk_member_t), "master trunk members array",
              "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** get master trunk info*/
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_get(unit, master_trunk_id, &master_trunk_info, master_nof_members,
                                      master_member_array, &master_member_count));

    /** verify new member physical membership in master trunk */
    SHR_IF_ERR_EXIT(dnx_trunk_modport_membership_verify(unit, member->gport, master_nof_members, master_member_array));

exit:
    SHR_FREE(master_member_array);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_member_add
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member - info of added member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_member_add_verify(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member)
{
    int member_count;
    int pool;
    int group;
    int system_port_is_not_a_member_of_another_trunk;
    int system_port_is_already_a_member_of_current_trunk;
    int added_member_is_enabled;
    int action_is_enabling_a_disabled_member;
    int system_port_exists_in_trunk_only_as_protecting = 0;
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags, trunk_flags;
    bcm_trunk_id_info_t id_info;
    uint32 system_port;
    int psc;
    int psc_profile_id;
    algo_gpm_gport_verify_type_e allowed_port_infos[] = { ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    /** get trunk flags */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));

    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** NULL checks */
    SHR_NULL_CHECK(member, _SHR_E_PARAM, "member");
    /** check validity of system port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, member->gport, COUNTOF(allowed_port_infos), allowed_port_infos));

    /** check validity of flags - including overruning legacy flags with new ones */
    SHR_IF_ERR_EXIT(dnx_trunk_member_flags_verify(unit, &(member->flags)));

    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(member->gport);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, system_port, &curr_trunk_id, &curr_flags));
    /** define conditions under which the addition of a member is valid */
    system_port_is_not_a_member_of_another_trunk = (curr_trunk_id == TRUNK_SW_DB_INVALID_TRUNK_ID);
    system_port_is_already_a_member_of_current_trunk = (curr_trunk_id == trunk_id);
    /** if system port is in trunk */
    if (system_port_is_already_a_member_of_current_trunk)
    {
        trunk_group_member_info_t member_info = {.system_port = member->gport };
        int first;
        int last;
        int count;
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find(unit, pool, group, &member_info, &first, &last, &count));
        if (count == 1)
        {
            /** get it and check if its only protecting */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, first, &member_info));
            if (member_info.index == INVALID_TM_INDEX_TO_INDICATE_MEMBER_IS_ONLY_PROTECTING)
            {
                system_port_exists_in_trunk_only_as_protecting = 1;
            }
        }
    }
    added_member_is_enabled = ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0);
    if ((system_port_is_not_a_member_of_another_trunk) ||
        (system_port_is_already_a_member_of_current_trunk && added_member_is_enabled) ||
        (system_port_exists_in_trunk_only_as_protecting))
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "this member (system port = 0x%x) is valid,"
                                                " and can be added to trunk.\n"), system_port));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid configuration, either member is already found in another trunk, or a"
                     "miss-configuration was detected with member flags");
    }

    action_is_enabling_a_disabled_member = (system_port_is_already_a_member_of_current_trunk &&
                                            added_member_is_enabled && (curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE));
    if (action_is_enabling_a_disabled_member && (member->flags & BCM_TRUNK_MEMBER_WITH_PROTECTION))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Invalid configuration, cannot enable a member and add it with protection at the same time."
                     "try to add protection and enable after, or enable and add protection after.");
    }

    /** Check that number of members can fit into group */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, pool, group, &member_count));
    if ((system_port_is_already_a_member_of_current_trunk) && (curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE))
    {
        /** member already appears in the trunk as disabled - under
         *  those circumstances the new number of members in the trunk
         *  is not changed so no need to check it */
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_trunk_can_fit_new_amount_of_members_verify(unit, &id_info, member_count + 1));
    }

    /** check that trunk and new member has the same header_type if it is not already part of this trunk*/
    if (!system_port_is_already_a_member_of_current_trunk)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_new_member_has_matching_header_type_verify(unit, &id_info, member->gport));
    }

    /** verify virtual trunk new member physical membership in master trunk */
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        SHR_IF_ERR_EXIT(dnx_virtual_trunk_member_add_verify(unit, &id_info, member));
    }
    /** verify validity of spa_id in case BCM_TRUNK_MEMBER_SPA_WITH_ID was provided */
    SHR_IF_ERR_EXIT(dnx_trunk_member_spa_with_id_verify(unit, member, pool, group));

    /** verify validity of member_id in case BCM_TRUNK_MEMBER_WITH_ID was provided */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &psc, &psc_profile_id));
    SHR_IF_ERR_EXIT(dnx_trunk_member_with_id_verify(unit, member, psc, pool, group));

    /** verify backup member for members with protection */
    if (member->flags & BCM_TRUNK_MEMBER_WITH_PROTECTION)
    {
        bcm_trunk_member_t backup_member;
        sal_memset(&backup_member, 0, sizeof(backup_member));
        backup_member.gport = member->backup_member.gport;
        backup_member.flags = member->backup_member.flags;
        backup_member.spa_id = member->backup_member.spa_id;
        SHR_IF_ERR_EXIT(dnx_trunk_backup_member_verify(unit, trunk_id, &backup_member));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - update to dbal the changes done to PSC as a result
 *        of adding or removing a member from a trunk.
 *
 * \param [in] unit - uint number
 * \param [in] pool - pool index
 * \param [in] group - group index
 * \param [in] is_action_add - if TRUE - adding member, if FALSE
 *        - removing member
 * \param [in] tm_index_to_add_or_remove - the trunk TM index
 *        that is being added or removed, this is relevant for
 *        c-lag where this actual index will have to be removed
 *        from the relevant c-lag table and replaced with others
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_psc_resolve(
    int unit,
    int pool,
    int group,
    int is_action_add,
    int tm_index_to_add_or_remove)
{
    int bcm_psc;
    int psc_profile_id;
    int psc_algo_input;
    int nof_enabled_members_in_group;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &bcm_psc, &psc_profile_id));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_enabled_members_get(unit, pool, group, &nof_enabled_members_in_group));
    switch (bcm_psc)
    {
        case BCM_TRUNK_PSC_USER_PROFILE:
        {
            /** User profile PSC doesn't require any resolution, it is up to the user to update the profile to match trunk's members composition */
            break;
        }
        case BCM_TRUNK_PSC_C_LAG:
        {
            uint32 chm_handle;
            bcm_trunk_t trunk_id;
            bcm_trunk_id_info_t id_info;
            profile_assign_user_info_t user_info;

            /** get CHM handle */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_chm_handle_get(unit, &chm_handle));
            /** get trunk_id */
            id_info.group_index = group;
            id_info.pool_index = pool;
            SHR_IF_ERR_EXIT(dnx_trunk_utils_id_info_to_trunk_id_convert(unit, &trunk_id, &id_info));

            /** init user info */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_get(unit, psc_profile_id, &user_info.profile_type,
                                                             &user_info.max_nof_members_in_profile));
            if (is_action_add)
            {
                /** member was already added to sw db but not added yet to chm. so need to take (nof_enabled - 1) */
                int nof_members_currently_in_chm = nof_enabled_members_in_group - 1;
                SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_add(unit, chm_handle, trunk_id, 0,
                                                                                       tm_index_to_add_or_remove,
                                                                                       nof_members_currently_in_chm,
                                                                                       (void *) &user_info));
            }
            else
            {
                /** in member delete context - nof_enabled_members_in_group truly represents the number of members in chm */
                SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_member_remove(unit, chm_handle, trunk_id, 0,
                                                                                          tm_index_to_add_or_remove,
                                                                                          nof_enabled_members_in_group,
                                                                                          (void *) &user_info));
            }
            break;
        }
        case BCM_TRUNK_PSC_SMOOTH_DIVISION:
        {
            /** This is not part of algo trunk because this is suitable
             *  only for this version of smooth division. any customer
             *  that wants to override the smooth dibision will not be
             *  able to use add/remove functions but will have to set the
             *  trunk a new on each change of members anyways. */

            /** in smooth division there's a corresponding profile that
             *  matches to the nof_enabled_members - 1, meaning that if
             *  number of enabled members is 13 - the coresponding
             *  profile should be 12 */
            if (is_action_add)
            {
                /** when action is add the trunk SW DB is already updated
                 *  with the correct number of enabled members - meaning the
                 *  member was already added to the trunk SW DB */
                psc_algo_input = nof_enabled_members_in_group - 1;
                if (nof_enabled_members_in_group > dnx_data_trunk.psc.smooth_division_max_nof_member_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Smooth Division PSC doesn't support above %d members\n",
                                 dnx_data_trunk.psc.smooth_division_max_nof_member_get(unit));
                }
            }
            else
            {
                /** when action is remove the trunk SW DB is not updated yet.
                 *  meaning that the number of enabled member read from the SW
                 *  DB is actually smaller by one and not updated yet. */
                psc_algo_input = (nof_enabled_members_in_group < 2) ? 0 : nof_enabled_members_in_group - 2;
            }
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_attributes_set(unit, pool, group, bcm_psc, psc_algo_input));
            break;
        }
        case BCM_TRUNK_PSC_PORTFLOW:
        {
            /** in PORTFLOW - AKA Multiply and Divide the psc_algo_input
             *  represents the number of enabled members - 1, meaning that
             *  if number of enabled members is 13 - the corresponding
             *  psc_algo_input is 12 */
            if (is_action_add)
            {
                /** when action is add the trunk SW DB is already updated
                 *  with the correct number of enabled members - meaning the
                 *  member was already added to the trunk SW DB */
                psc_algo_input = nof_enabled_members_in_group - 1;
            }
            else
            {
                /** when action is remove the trunk SW DB is not updated yet.
                 *  meaning that the number of enabled member read from the SW
                 *  DB is actually smaller by one and not updated yet. */
                psc_algo_input = (nof_enabled_members_in_group < 2) ? 0 : nof_enabled_members_in_group - 2;
            }
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_attributes_set(unit, pool, group, bcm_psc, psc_algo_input));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid PSC\n");
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - add member to trunk. added member can have flags as
 *        mentioned in bcm_dnx_trunk_set API.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member - member info
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
bcm_dnx_trunk_member_add(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member)
{
    int sw_db_member_index;
    int pool;
    int group;
    int nof_enabled_members;
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags;
    int member_found_in_trunk;
    int enable_a_disabled_member;
    trunk_group_member_info_t member_info;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    bcm_trunk_id_info_t id_info;
    int psc;
    int psc_profile_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** stacking trunk */
    if (_SHR_TRUNK_ID_IS_STACKING(trunk_id))
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_member_add(unit, trunk_id, member));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_member_add_verify(unit, trunk_id, member));

    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;
    /** find nof enabled memebers */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_enabled_members_get(unit, pool, group, &nof_enabled_members));

    /** get system port info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member->gport,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

    /** get group PSC */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &psc, &psc_profile_id));

    /** check if member is already found in trunk - this has to come before adding the member to trunk SW DB*/
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, phy_gport_info.sys_port,
                                                             &curr_trunk_id, &curr_flags));
    member_found_in_trunk = (trunk_id == curr_trunk_id);
    dnx_trunk_sw_db_member_init(unit, &member_info);
    member_info.flags = member->flags;
    member_info.system_port = member->gport;
    /** will be used only if BCM_TRUNK_MEMBER_SPA_WITH_ID was provided, otherwise will be overridden */
    member_info.spa_member_id = member->spa_id;
    if (member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE)
    {
        /** this index won't be used if added member is disabled */
        member_info.index = INVALID_TM_INDEX_TO_INDICATE_DISABLED_MEMBER;
    }
    else if (member->flags & BCM_TRUNK_MEMBER_WITH_ID)
    {
        /** this option is applicable only for c-lag and user_profile PSCs, check done in verify func */
        member_info.index = member->member_id;
    }
    else if ((psc == BCM_TRUNK_PSC_C_LAG) || (psc == BCM_TRUNK_PSC_USER_PROFILE))
    {
        /** for those PSCs, if specific ID was not specified, need alloc manager to find an empty index and use it */
        member_info.index = INVALID_TM_INDEX_BUT_NEED_ALLOCATE_ONE;
    }
    else
    {
        /** for all other cases use next available index - always last */
        member_info.index = nof_enabled_members;
    }
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_add(unit, pool, group, &member_info, &sw_db_member_index));
    /** if member is already found in trunk, no need to add it again from PP perspective */
    if (!member_found_in_trunk)
    {
        /** add member to PP part of the trunk */
        SHR_IF_ERR_EXIT(dnx_trunk_add_port_to_pp(unit, trunk_id, &member_info));
    }

    enable_a_disabled_member = (curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE)
        && !(member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE);

    if (enable_a_disabled_member)
    {
        trunk_group_member_info_t updated_member_info;
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, sw_db_member_index, &updated_member_info));
        if (updated_member_info.flags & BCM_TRUNK_MEMBER_WITH_PROTECTION)
        {
            /** Just need to update HW, no need to change SW db */
            dnx_algo_gpm_gport_phy_info_t backup_gport_info;
            /** get system port info */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, updated_member_info.protecting_system_port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE,
                             &backup_gport_info));
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_protection_members_set
                            (unit, pool, group, member_info.index, backup_gport_info.sys_port));
        }
    }
    else if (member->flags & BCM_TRUNK_MEMBER_WITH_PROTECTION)
    {
        /** if member has protection, add protection using internal APIs */
        SHR_IF_ERR_EXIT(dnx_trunk_member_protection_add(unit, pool, group, sw_db_member_index, member));
    }

    /** if member is enabled */
    if ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0)
    {
        /** add member to TM */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group,
                                                                member_info.index, phy_gport_info.sys_port));
        /** resolve PSC */
        SHR_IF_ERR_EXIT(dnx_trunk_psc_resolve(unit, pool, group, TRUE, member_info.index));
    }

    if (dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_is_supported))
    {
            /** Align Egress trunk profile's LB Keys distribution with trunk members */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_align(unit, trunk_id));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_member_delete
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member - deleted member
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_member_delete_verify(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member)
{
    int group;
    int pool;
    int nof_replications;
    uint32 flags;
    uint32 system_port;
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags;
    int trying_to_disable_a_disabled_member;
    int trying_to_disable_member_with_multiple_replications;
    int member_found_in_trunk;
    bcm_trunk_id_info_t id_info;
    algo_gpm_gport_verify_type_e allowed_port_infos[] = { ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** NULL checks */
    SHR_NULL_CHECK(member, _SHR_E_PARAM, "member");

    /** check validity of system port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, member->gport, COUNTOF(allowed_port_infos), allowed_port_infos));

    /** check validity of flags - including overruning legacy flags with new ones */
    SHR_IF_ERR_EXIT(dnx_trunk_member_flags_verify(unit, &(member->flags)));
    flags = member->flags;

    /** check for collisions between members */
    /** get current entry from system port */
    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(member->gport);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, system_port, &curr_trunk_id, &curr_flags));
    /** if member is not part of the trunk, fail */
    member_found_in_trunk = (curr_trunk_id == trunk_id);
    if (!member_found_in_trunk)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "given member(system port %u) is not part of the trunk(trunk id %u)\n",
                     system_port, trunk_id);
    }
    /** check collision of flags */
    trying_to_disable_a_disabled_member = (flags & curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_replication_get(unit, pool, group, member->gport, &nof_replications));
    trying_to_disable_member_with_multiple_replications = (nof_replications > 1) &&
        (flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE);
    if (trying_to_disable_a_disabled_member || trying_to_disable_member_with_multiple_replications)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "given member miss using trunk member flag BCM_TRUNK_MEMBER_EGRESS_DISABLE\n");
    }

    /** add a special verification step for members with ID. */
    if (member->flags & BCM_TRUNK_MEMBER_WITH_ID)
    {
        /** need to make sure that exact member exists. */
        const uint32 IGNORE_FLAGS_IN_MATCH_CRITERIA = 0;
        trunk_group_member_info_t member_info;
        int count;
        int psc;
        int psc_profile;
        sal_memset(&member_info, 0, sizeof(member_info));
        member_info.index = member->member_id;
        member_info.system_port = member->gport;
        member_info.spa_member_id = INVALID_SPA_MEMBER_ID;
        member_info.protecting_system_port = dnx_data_device.general.invalid_system_port_get(unit);
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_per_criteria_find
                        (unit, pool, group, &member_info, IGNORE_FLAGS_IN_MATCH_CRITERIA, 0, NULL, &count));
        if (count == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "given member(system port %u) with given member_id(%d) is not part of the trunk(trunk id %u)\n",
                         system_port, member->member_id, trunk_id);
        }

        /** Check that PSC supports deleting with ID (c-lag and user_profile) */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &psc, &psc_profile));
        if (!((psc == BCM_TRUNK_PSC_C_LAG) || (psc == BCM_TRUNK_PSC_USER_PROFILE)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "setting member with id is not supported for given trunk(trunk id %u) with its current PSC\n",
                         trunk_id);
        }
    }
    else if (member->flags & BCM_TRUNK_MEMBER_WITH_PROTECTION)
    {
        trunk_group_member_info_t member_info;
        int count;
        uint32 flags_mask = BCM_TRUNK_MEMBER_WITH_PROTECTION;
        member_info.system_port = member->gport;
        member_info.flags = member->flags;
        member_info.index = INVALID_TM_INDEX;
        member_info.spa_member_id = INVALID_SPA_MEMBER_ID;
        member_info.protecting_system_port = member->backup_member.gport;
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_per_criteria_find
                        (unit, pool, group, &member_info, flags_mask, 0, NULL, &count));
        if (count == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "given member(system port %u) with given protecting system port(%u) is not part of the trunk(trunk id %u)\n",
                         member->gport, member->backup_member.gport, trunk_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - delete a member from trunk. it is possible to
 *        disable a member instead of deleting it. this can be
 *        done by adding the flag
 *        BCM_TRUNK_MEMBER_EGRESS_DISABLE. this is possible only
 *        if there is a single replication of the member.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 * \param [in] member - member info
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
bcm_dnx_trunk_member_delete(
    int unit,
    bcm_trunk_t trunk_id,
    bcm_trunk_member_t * member)
{
    int nof_replications;
    int removed_sw_db_index;
    int first_sw_db_index;
    int pool;
    int group;
    int deleting_a_regular_member;
    int disabling_a_regular_member;
    int deleting_a_disabled_member;
    int removed_member_has_protection;
    int psc;
    int psc_profile;
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    trunk_group_member_info_t member_info;
    bcm_trunk_id_info_t id_info;
    trunk_group_member_info_t removed_member_info;
    int nof_enabled_members;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** stacking trunk */
    if (_SHR_TRUNK_ID_IS_STACKING(trunk_id))
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_member_delete(unit, trunk_id, member));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_member_delete_verify(unit, trunk_id, member));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &psc, &psc_profile));

    /**  there are 2 conceptual steps in removing a member from a
     *   trunk - PP related actions and TM related actions,
     *   according to the state of the trunk one or both of those
     *   steps are required. when deleting completely a regular
     *   member need to do both. when deleting a disabled member
     *   need to do PP part. when disabling a regular member need
     *   to do TM part */

    /** get port info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member->gport,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

    /** get current member info */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, phy_gport_info.sys_port, &curr_trunk_id,
                                                             &curr_flags));
    /** define conditions to perform each part */
    deleting_a_regular_member = ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0) &&
        ((curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0);
    disabling_a_regular_member = ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) != 0) &&
        ((curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0);
    deleting_a_disabled_member = ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0) &&
        ((curr_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) != 0);
    /** find member to remove */
    sal_memset(&member_info, 0, sizeof(member_info));
    /** When member to be deleted is supplied with ID get it instead of last */
    if (member->flags & BCM_TRUNK_MEMBER_WITH_ID)
    {
        int count;
        const uint32 IGNORE_FLAGS_IN_MATCH_CRITERIA = 0;
        member_info.system_port = member->gport;
        member_info.flags = member->flags;
        member_info.index = member->member_id;
        member_info.spa_member_id = INVALID_SPA_MEMBER_ID;
        member_info.protecting_system_port = dnx_data_device.general.invalid_system_port_get(unit);
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_per_criteria_find(unit, pool, group, &member_info,
                                                                          IGNORE_FLAGS_IN_MATCH_CRITERIA, 1,
                                                                          &removed_sw_db_index, &count));
    }
    else if (member->flags & BCM_TRUNK_MEMBER_WITH_PROTECTION)
    {
        int count;
        uint32 flags_mask = BCM_TRUNK_MEMBER_WITH_PROTECTION;
        member_info.system_port = member->gport;
        member_info.flags = member->flags;
        member_info.index = INVALID_TM_INDEX;
        member_info.spa_member_id = INVALID_SPA_MEMBER_ID;
        member_info.protecting_system_port = member->backup_member.gport;
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_per_criteria_find(unit, pool, group, &member_info,
                                                                          flags_mask, 1, &removed_sw_db_index, &count));
    }
    else
    {

        /** search a member without protection */
        int count;
        uint32 flags_mask = BCM_TRUNK_MEMBER_WITH_PROTECTION;
        member_info.system_port = member->gport;
        member_info.flags = member->flags;
        member_info.index = INVALID_TM_INDEX;
        member_info.spa_member_id = INVALID_SPA_MEMBER_ID;
        member_info.protecting_system_port = dnx_data_device.general.invalid_system_port_get(unit);
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_per_criteria_find(unit, pool, group, &member_info,
                                                                          flags_mask, 1, &removed_sw_db_index, &count));
        /** if not found get whatever member with the same system port */
        if (count == 0)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find(unit, pool, group, &member_info, &first_sw_db_index,
                                                                 &removed_sw_db_index, &nof_replications));
        }

    }
    /** get member to be removed */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, removed_sw_db_index, &removed_member_info));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_replication_get
                    (unit, pool, group, removed_member_info.system_port, &nof_replications));
    removed_member_has_protection = (removed_member_info.flags & BCM_TRUNK_MEMBER_WITH_PROTECTION)
        && (dnx_data_trunk.protection.feature_get(unit, dnx_data_trunk_protection_backup_destination));

    if (deleting_a_regular_member || disabling_a_regular_member)
    {
        /** TM related actions */
        if ((psc == BCM_TRUNK_PSC_C_LAG) || (psc == BCM_TRUNK_PSC_USER_PROFILE))
        {
            /** resolve PSC */
            SHR_IF_ERR_EXIT(dnx_trunk_psc_resolve(unit, pool, group, FALSE, removed_member_info.index));

            /** delete from HW - deletion from sw_db is handled at the end */
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group, removed_member_info.index,
                                                                    dnx_data_device.
                                                                    general.invalid_system_port_get(unit)));
            /** delete protection from HW */
            if (removed_member_has_protection)
            {
                SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_protection_members_set
                                (unit, pool, group, removed_member_info.index,
                                 dnx_data_device.general.invalid_system_port_get(unit)));
            }
        }
        else
        {
            /** we will always remove the last member, so - need to switch the member that we actually
             *  want to remove with the last member */
            /** get tm index of last member ( member with biggest TM index) */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_enabled_members_get(unit, pool, group, &nof_enabled_members));
            /** copy last to removed in dbal, both for primary and protection if supported*/
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_copy
                            (unit, pool, group, nof_enabled_members - 1, removed_member_info.index));

            /** resolve PSC */
            SHR_IF_ERR_EXIT(dnx_trunk_psc_resolve(unit, pool, group, FALSE, nof_enabled_members));

            /** set last to invalid - this must be done only after
             *  resolving PSC issue - to preventing a packet from being
             *  routed to an invalid member (on a local scale) - on  a
             *  system wide scale this cannot be resolved */
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group, nof_enabled_members - 1,
                                                                    dnx_data_device.
                                                                    general.invalid_system_port_get(unit)));
            /** delete protection from HW */
            if (dnx_data_trunk.protection.feature_get(unit, dnx_data_trunk_protection_backup_destination))
            {
                SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_protection_members_set
                                (unit, pool, group, nof_enabled_members - 1,
                                 dnx_data_device.general.invalid_system_port_get(unit)));
            }
        }
    }

    if (removed_member_has_protection && (deleting_a_regular_member || deleting_a_disabled_member))
    {
        /** It is possible that the tm index in the DB is not relevant by this point, also the member table was already updated above */
        uint32 flags = DNX_TRUNK_PROTECTION_REMOVE_IGNORE_MEMBER_TABLE;
        SHR_IF_ERR_EXIT(dnx_trunk_member_protection_remove(unit, flags, pool, group, removed_sw_db_index));
    }

    /** delete member from sw db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_delete(unit, pool, group, removed_sw_db_index,
                                                  nof_replications, disabling_a_regular_member));

    if (dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_is_supported))
    {
            /** Align Egress trunk profile's LB Keys distribution with trunk members */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_align(unit, trunk_id));
    }

    if ((deleting_a_regular_member || deleting_a_disabled_member) && (nof_replications == 1))
    {
        /** PP related actions - relevant only for members which are
         *  local to this fap, and for system_port that this is their last
         *  replication and non virtual trunks */
        SHR_IF_ERR_EXIT(dnx_trunk_delete_port_from_pp(unit, trunk_id, &removed_member_info));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_member_delete_all
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_member_delete_all_verify(
    int unit,
    bcm_trunk_t trunk_id)
{
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - delete all members from trunk, this is done by
 *        getting the trunk and then setting it again with 0
 *        members.
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
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
bcm_dnx_trunk_member_delete_all(
    int unit,
    bcm_trunk_t trunk_id)
{
    bcm_trunk_info_t trunk_info;
    bcm_trunk_member_t member_array[1];
    int member_count;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_member_delete_all_verify(unit, trunk_id));
    /** get trunk */
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_get(unit, trunk_id, &trunk_info, 1, member_array, &member_count));
    /** set trunk with 0 members but same trunk_info */
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_set(unit, trunk_id, &trunk_info, 0, member_array));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_find
 *
 * \param [in] unit - unit number
 * \param [in] modid - module id
 * \param [in] port - system port gport to search for, or local
 *        port
 * \param [in] trunk_id - returned trunk id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_find_verify(
    int unit,
    bcm_module_t modid,
    bcm_gport_t port,
    bcm_trunk_t * trunk_id)
{
    algo_gpm_gport_verify_type_e allowed_type = ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trunk_id, _SHR_E_PARAM, "trunk_id");
    /** make sure input is system port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, port, 1, &allowed_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - find if a system port is a member in a trunk and
 *        return the trunk's id, if it is not a member of any
 *        trunk, fail with _SHR_E_NOT_FOUND
 *
 * \param [in] unit - unit number
 * \param [in] modid - not used
 * \param [in] port - system port gport
 *        port
 * \param [in] trunk_id - returned trunk id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * work with system port gport as input for the port argument. modid is ignored.
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_trunk_find(
    int unit,
    bcm_module_t modid,
    bcm_gport_t port,
    bcm_trunk_t * trunk_id)
{
    bcm_gport_t system_port_gport;
    uint32 system_port;
    uint32 flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_find_verify(unit, modid, port, trunk_id));

    /** port has to be system port, was checked in verify function */
    system_port_gport = port;

    /** search is system port is part of any trunk */
    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(system_port_gport);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, system_port, trunk_id, &flags));

    /** return error if not found */
    if (*trunk_id == TRUNK_SW_DB_INVALID_TRUNK_ID)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "system port %u is not found in any trunk\n"), system_port));
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_agg_set_verify(
    int unit,
    int flow_agg_id,
    uint32 flags,
    bcm_flow_agg_info_t * flow_agg_info)
{
    int flow_aggs_in_bundle = dnx_data_trunk.flow_agg.flow_aggs_in_bundle_get(unit);
    int base_voq_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_agg_info, _SHR_E_PARAM, "flow_agg_info");

    /** verify base_voq is set using relevant MACRO */
    if (!_SHR_GPORT_IS_UCAST_QUEUE_GROUP(flow_agg_info->base_voq))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "base_voq should be supplied using BCM_GPORT_UNICAST_QUEUE_GROUP_SET\n");
    }

    /** verify base_voq is provided with correct granularity */
    base_voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(flow_agg_info->base_voq);
    if ((base_voq_id % flow_aggs_in_bundle) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "base_voq should be aligned to number of flow aggregates in bundle (%d)\n",
                     flow_aggs_in_bundle);
    }

    /** verify base VOQ is in valid range - this check is only for the base VOQ! */
    /** the flow aggregate requires a block of specific VOQs starting from base VOQ */
    /** there's no verification that such a range is free or even exists and it is up
     * to the user to make sure there's no miss-configuration in that regard */
    if (base_voq_id >= dnx_data_ipq.queues.nof_queues_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid base VOQ (0x%x)\n", base_voq_id);
    }

    /** verify trunk ID is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, flow_agg_info->trunk_id));

    /** verify nof_cosq_levels is valid */
    switch (flow_agg_info->nof_cosq_levels)
    {
        case 8:
        case 4:
        case 2:
        case 1:
        {
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid value of nof cosq level (%d)\n", flow_agg_info->nof_cosq_levels);
        }
    }

    /** verify flow_agg_id is in granularity of bundle */
    if ((flow_agg_id % flow_aggs_in_bundle) != 0)
    {
        int base_flow_agg_id = flow_agg_id / flow_aggs_in_bundle * flow_aggs_in_bundle;
        SHR_ERR_EXIT(_SHR_E_PARAM, "flow_agg_id should be aligned to number of flow aggregates in bundle (%d)\n"
                     "use flow_agg_id = %d instead of %d", flow_aggs_in_bundle, base_flow_agg_id, flow_agg_id);
    }

    /** verify this destination is in the range of flow_agg */
    if ((flow_agg_id < 0) || (flow_agg_id >= dnx_data_trunk.flow_agg.nof_flow_aggs_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flow_agg_id (%d), should be correct range (0 - %d)\n",
                     flow_agg_id, dnx_data_trunk.flow_agg.nof_flow_aggs_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
bcm_dnx_trunk_flow_agg_set(
    int unit,
    int flow_agg_id,
    int flags,
    bcm_flow_agg_info_t * flow_agg_info)
{

    int base_flow_agg_bundle;
    int flow_aggs_in_bundle = dnx_data_trunk.flow_agg.flow_aggs_in_bundle_get(unit);
    int base_voq_id;
    int base_flow_id;
    int trunk_pool;
    int trunk_group;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_flow_agg_set_verify(unit, flow_agg_id, flags, flow_agg_info));

    /** get relevant base_flow_agg_bundle from flow_agg_id */
    base_flow_agg_bundle = flow_agg_id / flow_aggs_in_bundle;

    /** get relevant base_flow_id from base_voq */
    base_voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(flow_agg_info->base_voq);
    base_flow_id = base_voq_id / flow_aggs_in_bundle;

    /** configure table */
    BCM_TRUNK_ID_POOL_GET(trunk_pool, flow_agg_info->trunk_id);
    BCM_TRUNK_ID_GROUP_GET(trunk_group, flow_agg_info->trunk_id);
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_flow_agg_group_map_set
                    (unit, base_flow_agg_bundle, base_flow_id, trunk_pool, trunk_group,
                     flow_agg_info->nof_cosq_levels));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_flow_agg_get_verify(
    int unit,
    int flow_agg_id,
    uint32 flags,
    bcm_flow_agg_info_t * flow_agg_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flow_agg_info, _SHR_E_PARAM, "flow_agg_info");

    /** verify this destination is in the range of flow_agg */
    if ((flow_agg_id < 0) || (flow_agg_id >= dnx_data_trunk.flow_agg.nof_flow_aggs_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid flow_agg_id (%d), should be correct range (0 - %d)\n",
                     flow_agg_id, dnx_data_trunk.flow_agg.nof_flow_aggs_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
bcm_dnx_trunk_flow_agg_get(
    int unit,
    int flow_agg_id,
    int flags,
    bcm_flow_agg_info_t * flow_agg_info)
{
    int base_flow_agg_bundle;
    int flow_aggs_in_bundle = dnx_data_trunk.flow_agg.flow_aggs_in_bundle_get(unit);
    int base_voq_id;
    int base_flow_id;
    int trunk_pool;
    int trunk_group;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_flow_agg_get_verify(unit, flow_agg_id, flags, flow_agg_info));

    /** get relevant base_flow_agg_bundle from flow_agg_id */
    base_flow_agg_bundle = flow_agg_id / flow_aggs_in_bundle;

    /** get info from dbal */
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_flow_agg_group_map_get
                    (unit, base_flow_agg_bundle, &base_flow_id, &trunk_pool, &trunk_group,
                     &flow_agg_info->nof_cosq_levels));

    /** configure output */
    BCM_TRUNK_ID_SET(flow_agg_info->trunk_id, trunk_pool, trunk_group);
    base_voq_id = base_flow_id * flow_aggs_in_bundle;
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(flow_agg_info->base_voq, base_voq_id);

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - verify function for bcm_dnx_trunk_destroy
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_destroy_verify(
    int unit,
    bcm_trunk_t trunk_id)
{
    bcm_trunk_id_info_t id_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - destroy a trunk and free all of the resources it
 *        occupied
 *
 * \param [in] unit - unit number
 * \param [in] trunk_id - trunk id
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
bcm_dnx_trunk_destroy(
    int unit,
    bcm_trunk_t trunk_id)
{
    int in_use;
    bcm_trunk_info_t trunk_info;
    bcm_trunk_id_info_t id_info;
    uint32 trunk_flags;

    bcm_trunk_pp_port_allocation_info_t allocation_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_destroy_verify(unit, trunk_id));

    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));

    /** if trunk not in use - nothing to do - get out */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_get(unit, id_info.pool_index, id_info.group_index, &in_use));
    if (!in_use)
    {
        SHR_EXIT();
    }

    /** set trunk to M&D psc with 0 members - as a preliminary step before destroying it */
    /** this is done to make sure all resources (profiles) were freed */
    /** in addition this will allow freeing all pp ports allocated for the trunk without collision */
    bcm_trunk_info_t_init(&trunk_info);
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;

    /** get trunk flags */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, id_info.pool_index, id_info.group_index, &trunk_flags));

    /** set master tid for virtual trunks before calling to bcm_dnx_trunk_set() API */
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_master_trunk_id_get(unit, id_info.pool_index, id_info.group_index,
                                                                  &trunk_info.master_tid));
    }

    SHR_IF_ERR_EXIT(bcm_dnx_trunk_set(unit, trunk_id, &trunk_info, 0, NULL));

    /** run feature update CBs with the trunk id, before freeing the pp ports */
    SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run(unit, DNX_TRUNK_FEATURE_UPDATE_CB_DESTROY, trunk_id, -1));

    /** free all pp ports */

    bcm_trunk_pp_port_allocation_info_t_init(&allocation_info);
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_pp_port_allocation_set(unit, trunk_id, 0, &allocation_info));

    /** set trunk_id to not used */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_set(unit, id_info.pool_index, id_info.group_index, FALSE));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_pp_port_get(
    int unit,
    bcm_trunk_t trunk_id,
    int core,
    uint32 *pp_port)
{
    int group;
    int pool;
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);
    /** verify trunk_id */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** NULL check */
    SHR_NULL_CHECK(pp_port, _SHR_E_PARAM, "pp_port");
    DNXCMN_CORE_VALIDATE(unit, core, FALSE);

    pool = id_info.pool_index;
    group = id_info.group_index;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_get(unit, pool, group, core, pp_port));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_local_port_bmp_get(
    int unit,
    bcm_trunk_t trunk_id,
    int core,
    bcm_pbmp_t * local_port_bmp)
{
    int group;
    int pool;
    int arr_size;
    int nof_unique_members;
    bcm_trunk_id_info_t id_info;
    dnx_trunk_unique_member_t *unique_members_arr = NULL;
    uint32 member_core_id;

    SHR_FUNC_INIT_VARS(unit);

    /** verify trunk_id */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** NULL check */
    SHR_NULL_CHECK(local_port_bmp, _SHR_E_PARAM, "local_port_bmp");

    /** get unique members info */
    pool = id_info.pool_index;
    group = id_info.group_index;
    arr_size = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    SHR_ALLOC_ERR_EXIT(unique_members_arr, sizeof(dnx_trunk_unique_member_t) * arr_size,
                       "unique members array", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_unique_members_info_get(unit, pool, group, unique_members_arr,
                                                            arr_size, &nof_unique_members));

    for (int member = 0; member < nof_unique_members; ++member)
    {
        dnx_algo_gpm_gport_phy_info_t phy_gport_info;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, unique_members_arr[member].system_port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));
        if (phy_gport_info.flags == DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT)
        {
            /** Get core */
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, phy_gport_info.local_port, (bcm_core_t *) & member_core_id));
            if (member_core_id == core)
            {
                BCM_PBMP_PORT_ADD(*local_port_bmp, phy_gport_info.local_port);
            }
        }
    }

exit:
    SHR_FREE(unique_members_arr);
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_flip_active_configuration_selector(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** if graceful lag modification is supported */
    if (dnx_data_trunk.graceful.feature_get(unit, dnx_data_trunk_graceful_allow))
    {
        uint32 multiple_configuration_enable;
        uint32 active_configuration_selector;
        /** get current configuration */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_active_configuration_selector_get(unit,
                                                                                      &multiple_configuration_enable,
                                                                                      &active_configuration_selector));
        /** flip configuration */
        active_configuration_selector = (active_configuration_selector == 0) ? 1 : 0;
        /** set configuration */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_active_configuration_selector_set(unit,
                                                                                      multiple_configuration_enable,
                                                                                      active_configuration_selector));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid when lag_graceful_modification is disabled\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_header_types_set(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 in_header_type,
    uint32 out_header_type)
{
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify trunk_id */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** set header types */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_set
                    (unit, id_info.pool_index, id_info.group_index, in_header_type, out_header_type));
exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_header_types_get(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 *in_header_type,
    uint32 *out_header_type)
{
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify trunk_id */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** get header types */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get
                    (unit, id_info.pool_index, id_info.group_index, in_header_type, out_header_type));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e static
dnx_trunk_spa_to_system_phys_port_map_get_verify(
    int unit,
    uint32 flags,
    uint32 system_port_aggregate,
    bcm_gport_t * gport)
{
    int spa_type_shift, is_spa;
    uint32 spa_mask;
    SHR_FUNC_INIT_VARS(unit);

    /** NULL check */
    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");
    /** make sure valid SPA */
    SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(unit, system_port_aggregate, &is_spa));
    if (!is_spa)
    {
        /** value received is not encoded as SPA */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid SPA.\n");
    }

    /** Create mask to verify the SPA. Set "spa_type_shift" bits starting from 0 in bit array "spa_mask" */
    spa_type_shift = dnx_data_trunk.parameters.spa_type_shift_get(unit);
    /** set SPA mask */
    SHR_BITSET_RANGE(&spa_mask, 0, spa_type_shift + 1);
    /** make sure no additional bits are set in spa_id */
    SHR_MASK_VERIFY(system_port_aggregate, spa_mask, _SHR_E_PARAM, "Invalid SPA.\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Map between SPA(system port aggregate) to its corresponding
 *          system port. the mapping is relevant only when the API is called
 *          and might change afterwards.
 *
 * \param [in] unit - unit number
 * \param [in] flags - currently no flags are used
 * \param [in] system_port_aggregate - SPA to transform
 * \param [out] gport - number of members in array
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
bcm_dnx_trunk_spa_to_system_phys_port_map_get(
    int unit,
    uint32 flags,
    uint32 system_port_aggregate,
    bcm_gport_t * gport)
{
    int pool;
    int group;
    int member;
    int is_used = 0;
    trunk_group_member_info_t member_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_spa_to_system_phys_port_map_get_verify(unit, flags, system_port_aggregate, gport));

    /** get pool, member and group from SPA ID */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_decode(unit, &pool, &group, &member, system_port_aggregate));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_get(unit, pool, group, &is_used));
    member_info.spa_member_id = member;
    SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_trunk_sw_db_spa_member_id_in_group_find(unit, pool, group, &member_info),
                              _SHR_E_NOT_FOUND);

    /** handle error state (group not used, Invalid system port) */
    if (!is_used)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SPA 0x%x is not mapped to a trunk group that is used\n", system_port_aggregate);
    }
    if (member_info.system_port == dnx_data_device.general.invalid_system_port_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "SPA 0x%x mapped to an invalid system port\n", system_port_aggregate);
    }

    /** return value */
    *gport = member_info.system_port;

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_system_port_to_spa_map_get(
    int unit,
    bcm_gport_t system_port,
    bcm_trunk_t trunk_id,
    uint32 *system_port_aggregate)
{
    bcm_trunk_id_info_t id_info;
    trunk_group_member_info_t member_info;
    int first_replication_index;
    int last_replication_index;
    int count;
    SHR_FUNC_INIT_VARS(unit);

    /** verify trunk_id */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));

    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));

    /** Check trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));

    /** null check */
    SHR_NULL_CHECK(system_port_aggregate, _SHR_E_PARAM, "system_port_aggregate");

    /** check system port is valid */
    if (!BCM_GPORT_IS_SYSTEM_PORT(system_port))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "input system_port 0x%x is not a system port\n", system_port);
    }

    /** check if system port is member in trunk, if not get out */
    member_info.system_port = system_port;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find(unit, id_info.pool_index, id_info.group_index,
                                                         &member_info, &first_replication_index,
                                                         &last_replication_index, &count));
    if (count == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "input system_port 0x%x is not a member of trunk_id 0x%x\n", system_port, trunk_id);
    }

    /** encoding spa with mapping info */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get
                    (unit, id_info.pool_index, id_info.group_index, first_replication_index, &member_info));
    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_encode
                    (unit, id_info.pool_index, id_info.group_index, member_info.spa_member_id, system_port_aggregate));

exit:
    SHR_FUNC_EXIT;
}

/** see header file */
shr_error_e
dnx_trunk_is_used_get(
    int unit,
    uint32 pool,
    uint32 group,
    int *is_used)
{
    bcm_trunk_id_info_t id_info;
    int trunk_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_used, _SHR_E_PARAM, "is_used");
    id_info.group_index = group;
    id_info.pool_index = pool;

    SHR_IF_ERR_EXIT(dnx_trunk_utils_id_info_to_trunk_id_convert(unit, &trunk_id, &id_info));
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_get(unit, id_info.pool_index, id_info.group_index, is_used));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_trunk_member_range_set/get
 *
 * \param [in] unit - unit number
 * \param [in] flags - flags
 * \param [in] trunk_id - trunk id
 * \param [in] member - system gport of member
 * \param [in] range - structire containing the max and min hashes for the member
 * \param [in] is_set - indication if the verify function is called from the set or get API
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_member_range_set_get_verify(
    int unit,
    uint32 flags,
    bcm_trunk_t trunk_id,
    bcm_gport_t member,
    bcm_trunk_member_range_t * range,
    uint8 is_set)
{
    uint32 curr_flags;
    uint32 system_port;
    uint32 nof_lb_keys_available;
    bcm_trunk_t curr_trunk_id;
    bcm_trunk_id_info_t id_info;
    algo_gpm_gport_verify_type_e allowed_port_infos[] = { ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT };
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    SHR_FUNC_INIT_VARS(unit);

    /** Check device support */
    if (!dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_is_supported))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Current device does not support Egress trunks\n");
    }

    /** Check if Trunk ID is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));

    /** Get ID Info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));

    /** Check if Trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));

    /** Check validity of system port */
    SHR_IF_ERR_EXIT(algo_gpm_gport_verify(unit, member, COUNTOF(allowed_port_infos), allowed_port_infos));

    /** Verify that the port is a member of the Trunk */
    system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(member);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, system_port, &curr_trunk_id, &curr_flags));
    if (curr_trunk_id != trunk_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given member(system port %u) is not a part of the Trunk(trunk ID %u)\n",
                     system_port, trunk_id);
    }

    if (is_set == TRUE)
    {
        /** Handle Graceful LAG if supported */
        if (dnx_data_trunk.graceful.feature_get(unit, dnx_data_trunk_graceful_allow))
        {
            /** In Graceful LAG only half the range is used at a time */
            nof_lb_keys_available = (dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit) / 2);
        }
        else
        {
            nof_lb_keys_available = dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit);
        }

        /** Check that the min hash is within range */
        if ((range->min_hash >= nof_lb_keys_available) || (range->min_hash < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MIN hash(%d) is out of range, must be between 0 and %d\n", range->min_hash,
                         (dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit) - 1));
        }

        /** Check that the max hash is within range */
        if ((range->max_hash >= nof_lb_keys_available) || (range->max_hash < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MAX hash(%d) is out of range, must be between 0 and %d\n", range->max_hash,
                         (dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit) - 1));
        }

        /** Check that the min hash is not bigger than the max hash */
        if (range->min_hash > range->max_hash)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "MIN hash(%d) is bigger than MAX hash(%d)\n", range->min_hash, range->max_hash);
        }
    }
    else
    {
        /** NULL Check for range struct pointer */
        SHR_NULL_CHECK(range, _SHR_E_PARAM, "range");

        /** Check if requested member is local. Non-local ports do not have valid LB-Key ranges */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, member, DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));
        if (phy_gport_info.flags != DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Given member(system port %u) is not a local port, no valid hash range on current device! \n",
                         system_port);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * API to set the Hash range for Egress Load Balancing of specified
 * member of a Trunk
 */
int
bcm_dnx_trunk_member_hash_range_set(
    int unit,
    uint32 flags,
    bcm_trunk_t trunk_id,
    bcm_gport_t member,
    bcm_trunk_member_range_t range)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_member_range_set_get_verify(unit, flags, trunk_id, member, &range, TRUE));

    if (dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_trunk_egress_member_hash_range_set(unit, flags, trunk_id, member, range));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * API to get the Hash range for Egress Load Balancing of specified
 * member of a Trunk
 */
int
bcm_dnx_trunk_member_hash_range_get(
    int unit,
    uint32 flags,
    bcm_trunk_t trunk_id,
    bcm_gport_t member,
    bcm_trunk_member_range_t * range)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verify input parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_trunk_member_range_set_get_verify(unit, flags, trunk_id, member, range, FALSE));

    if (dnx_data_trunk.egress_trunk.feature_get(unit, dnx_data_trunk_egress_trunk_is_supported))
    {
        SHR_IF_ERR_EXIT(dnx_trunk_egress_member_hash_range_get(unit, flags, trunk_id, member, range));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/** verify function */
static shr_error_e
bcm_dnx_trunk_profile_create_verify(
    int unit,
    uint32 flags,
    bcm_trunk_profile_t * profile,
    int *profile_id)
{
    uint32 valid_flags = BCM_TRUNK_PROFILE_WITH_ID;
    int max_member_id;
    int max_size;
    int profile_type;
    const uint32 HIRARCHY0 = 0;
    const uint32 SINGLE_PROFILE = 1;
    uint32 chm_handle;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");
    SHR_NULL_CHECK(profile_id, _SHR_E_PARAM, "profile_id");
    /** Verify only acceptable flags are used */
    if (flags & ~valid_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "some flags of flags 0x%x are Not supported\n", flags);
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_TRUNK_PROFILE_WITH_ID))
    {
        uint8 is_allocated;
        int total_nof_supported_user_profiles;
        /** if with ID check that profile is in valid range. */
        SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_nof_elements_get(unit, &total_nof_supported_user_profiles));
        if (*profile_id >= total_nof_supported_user_profiles)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile ID (%d) valid range is 0-%d", *profile_id,
                         (total_nof_supported_user_profiles - 1));
        }
        /** if with ID check that profile is free. */
        SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_is_allocated(unit, *profile_id, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "user profile ID %d is already allocated\n", *profile_id);
        }
    }
    else
    {
        int nof_free_elements;
        /** check if there are remaining free IDs to allocate */
        SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_nof_free_elements_get(unit, &nof_free_elements));
        if (nof_free_elements == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "out of profile_ids\n");
        }
    }

    /** Check that nof_members is a valid number */
    /** when dnx_data_trunk_psc_consistant_hashing_max_group is supported it is possible to use MAX_PROFILE_SIZE,
     *  if it is not supported we need to reduce this number by 2 */
    if (dnx_data_trunk.psc.feature_get(unit, dnx_data_trunk_psc_consistant_hashing_max_group))
    {
        max_size = BCM_TRUNK_MAX_PROFILE_SIZE;
    }
    else
    {
        max_size = BCM_TRUNK_MAX_PROFILE_SIZE - 2;
    }
    if ((profile->nof_members > max_size) || (profile->nof_members <= 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid number of members (%d) in profile, max allowed number is %d\n",
                     profile->nof_members, BCM_TRUNK_MAX_PROFILE_SIZE);
    }
    if ((profile->nof_members % 2) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid number of members (%d) in profile, number of members has to be even\n",
                     profile->nof_members);
    }

    /** check that members array has valid members only */
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_max_member_id_get(unit, profile, &max_member_id));
    for (int member_index = 0; member_index < profile->nof_members; ++member_index)
    {
        if (profile->members_array[member_index] > max_member_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid member_id (%d) used, max allowed value is %d",
                         profile->members_array[member_index], max_member_id);
        }
    }

    /** Get profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_type_get(unit, profile, &profile_type));
    /** Get chm_handle from sw-state */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_chm_handle_get(unit, &chm_handle));
    /** Verify sufficient resources available for profile */
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_consistent_hashing_manager_verify_sufficient_resources
                             (unit, chm_handle, profile_type, HIRARCHY0, SINGLE_PROFILE),
                             "User profile cannot be allocated, there are not enough resources %s%s%s\n", EMPTY, EMPTY,
                             EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create a user profile for trunk.
 *          User profile's compositions are managed by the users via dedicated APIs.
 *          They are not affected by trunk actions such as adding/removing members.
 *          It is up to the user to update the profiles after changing the trunks' composition
 *
 * \param [in] unit - unit number
 * \param [in] flags - BCM_TRUNK_PROFILE_... flags are supported
 * \param [in] profile - profile info to create
 * \param [in,out] profile_id - created profile id,
 *                              if was created with 'with_id' flag
 *                              this is an input, else an output
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_trunk_profile_create(
    int unit,
    uint32 flags,
    bcm_trunk_profile_t * profile,
    int *profile_id)
{
    const uint32 HIRARCHY0 = 0;
    const uint32 SINGLE_PROFILE = 1;
    const uint32 FIRST_INDEX = 0;
    uint32 profile_offset;
    uint32 chm_handle;
    int profile_type;
    uint32 unique_profile_id;
    int is_with_id;
    int psc_profile;
    uint8 is_first;
    consistent_hashing_calendar_t calendar;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verifications */
    SHR_INVOKE_VERIFY_DNXC(bcm_dnx_trunk_profile_create_verify(unit, flags, profile, profile_id));

    /** Get chm_handle from sw-state */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_chm_handle_get(unit, &chm_handle));

    /** Determine required profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_type_get(unit, profile, &profile_type));

    /** allocate a user profile with/without ID and encode it with unique ID for CHM to use */
    is_with_id = _SHR_IS_FLAG_SET(flags, BCM_TRUNK_PROFILE_WITH_ID);
    SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_create(unit, is_with_id, profile_id));
    _SHR_TRUNK_USER_PROFILE_SET(unique_profile_id, *profile_id);

    /** Allocate a profile in CHM, assign it externally by passing valid profile_offset pointer */
    /** in the following allocation function, values of (max_nof_members_in_profile = 5) and (current_nof_members_in_profile = 0) are
     *  used as input parameters to make sure that the CHM will create a unique profile that will not be shared. A unique profile is
     *  created whenever a profile is not full. Since for user profiles we only use the chm_profile_alloc and chm_profile_free functions,
     *  we always maintain the profile as not full from CHM perspective so it will never be shared with others like standard c_lag. Any
     *  other valid value could've worked as well, but 5 is such a special number.*/
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_alloc
                    (unit, chm_handle, unique_profile_id, HIRARCHY0, profile_type, SINGLE_PROFILE, FIRST_INDEX,
                     5, 0, NULL, &profile_offset, NULL));

    /** allocate a c-lag profile using profile offset */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_create(unit, profile_offset, &psc_profile, &is_first));
    if (is_first)
    {
        /** should always be first */
        profile_assign_user_info_t psc_profile_info;
        psc_profile_info.profile_type = profile_type;
        /** max nof_members is relevant only for C_LAG_PROFILE_256 profile type from HW perspective. */
        psc_profile_info.max_nof_members_in_profile = profile->nof_members / 2;
        SHR_IF_ERR_EXIT(dnx_trunk_chm_psc_profile_is_first_config(unit, psc_profile, &psc_profile_info));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "PSC profile allocated for user profile should've been first instance, something went wrong.\n");
    }

    /** map user profile to c-lag profile */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_psc_profile_set(unit, *profile_id, psc_profile));

    /** populate profile with provided members */
    calendar.profile_type = profile_type;
    sal_memcpy(calendar.lb_key_member_array, profile->members_array, sizeof(calendar.lb_key_member_array));
    SHR_IF_ERR_EXIT(dnx_trunk_c_lag_calendar_set(unit, profile_offset, &calendar));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/** verify function */
static shr_error_e
bcm_dnx_trunk_profile_destroy_verify(
    int unit,
    uint32 flags,
    int profile_id)
{
    uint8 is_allocated;
    int ref_counter;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify flags is 0 */
    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags 0x%x are Not supported\n", flags);
    }

    /** verify profile ID is valid */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_is_allocated(unit, profile_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "user profile ID %d is not allocated\n", profile_id);
    }

    /** verify ref_count for profile id is 0 */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_ref_count_get(unit, profile_id, &ref_counter));
    if (ref_counter)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "provided profile_id %d still has %d objects using it. Profile must not be used before destroying it\n",
                     profile_id, ref_counter);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Destroy a user profile for trunk.
 *          It is possible to destory a user's profile only when there aren't any trunks that use it.
 *
 * \param [in] unit - unit number
 * \param [in] flags - not used at the moment
 * \param [in] profile_id - profile_id to destroy
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_trunk_profile_destroy(
    int unit,
    uint32 flags,
    int profile_id)
{
    uint32 chm_handle;
    int psc_profile_type;
    int psc_max_nof_members;
    int psc_profile;
    uint32 unique_profile_id;
    uint8 is_last;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verifications */
    SHR_INVOKE_VERIFY_DNXC(bcm_dnx_trunk_profile_destroy_verify(unit, flags, profile_id));

    /** Get c-lag profile from profile_id */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_psc_profile_get(unit, profile_id, &psc_profile));

    /** Get profile type from sw-state */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_get(unit, psc_profile, &psc_profile_type, &psc_max_nof_members));

    /** Get chm_handle from sw-state */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_chm_handle_get(unit, &chm_handle));

    /** Get unique_id */
    _SHR_TRUNK_USER_PROFILE_SET(unique_profile_id, profile_id);

    /** Free profile in CHM */
    SHR_IF_ERR_EXIT(dnx_algo_consistent_hashing_manager_profile_free
                    (unit, chm_handle, unique_profile_id, psc_profile_type));

    /** Free profile in template manager */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_destroy(unit, psc_profile, &is_last));
    if (is_last)
    {
        /** Clear profile data */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_set(unit, psc_profile, TRUNK_INVALID_PROFILE, -1));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "PSC profile freed should've been last instance, something went wrong.\n");
    }

    /** Clear mapping */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_clear(unit, profile_id));
    /** Free user profile */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_destroy(unit, profile_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_trunk_user_profile_match_verify(
    int unit,
    int profile_id,
    bcm_trunk_profile_t * profile)
{
    int psc_profile;
    int configured_psc_profile_type;
    int psc_max_nof_members;
    int provided_profile_type;
    SHR_FUNC_INIT_VARS(unit);

    /** Get from profile_id the c-lag profile */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_psc_profile_get(unit, profile_id, &psc_profile));

    /** Get configured profile type from sw-state */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_get
                    (unit, psc_profile, &configured_psc_profile_type, &psc_max_nof_members));

    /** Get provided profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_type_get(unit, profile, &provided_profile_type));

    /** make sure they match */
    if (provided_profile_type != configured_psc_profile_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "profile->nof_members has different size than was originally configured, those should match\n");
    }

    /** check that number of members in profile is valid for relevant profile types */
    if (configured_psc_profile_type == C_LAG_PROFILE_256)
    {
        int configured_nof_members = psc_max_nof_members * 2;
        if (configured_nof_members != profile->nof_members)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "profile->nof_members has different size than was originally configured, those should match\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** verify function */
static shr_error_e
bcm_dnx_trunk_profile_set_verify(
    int unit,
    uint32 flags,
    int profile_id,
    bcm_trunk_profile_t * profile)
{
    uint8 is_allocated;
    int max_member_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");

    /** Verify flags is 0 */
    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags 0x%x are Not supported\n", flags);
    }

    /** verify profile ID is valid */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_is_allocated(unit, profile_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "user profile ID %d is not allocated\n", profile_id);
    }

    /** check that provided profile type matches configured profile type */
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_match_verify(unit, profile_id, profile));

    /** check that members array has valid members only */
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_max_member_id_get(unit, profile, &max_member_id));
    for (int member_index = 0; member_index < profile->nof_members; ++member_index)
    {
        if ((profile->members_array[member_index] > max_member_id) || (profile->members_array[member_index] < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid member_id (%d) used, max allowed value is %d",
                         profile->members_array[member_index], max_member_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set a created profile with new members data.
 *
 * \param [in] unit - unit number
 * \param [in] flags - currently not used
 * \param [in] profile_id - profile_id to set with profile_info
 * \param [in] profile - profile info to set
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_trunk_profile_set(
    int unit,
    uint32 flags,
    int profile_id,
    bcm_trunk_profile_t * profile)
{
    int psc_profile;
    int profile_offset;
    int psc_profile_type;
    int psc_max_nof_members;
    consistent_hashing_calendar_t calendar;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verifications */
    SHR_INVOKE_VERIFY_DNXC(bcm_dnx_trunk_profile_set_verify(unit, flags, profile_id, profile));

    /** Get from profile_id the c-lag profile */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_psc_profile_get(unit, profile_id, &psc_profile));

    /** get from c-lag profile the profile_offset */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_data_get(unit, psc_profile, &profile_offset));

    /** Get profile type from sw-state */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_get(unit, psc_profile, &psc_profile_type, &psc_max_nof_members));

    /** populate profile with provided members */
    calendar.profile_type = psc_profile_type;
    sal_memcpy(calendar.lb_key_member_array, profile->members_array, sizeof(calendar.lb_key_member_array));
    SHR_IF_ERR_EXIT(dnx_trunk_c_lag_calendar_set(unit, profile_offset, &calendar));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/** verify function */
static shr_error_e
bcm_dnx_trunk_profile_get_verify(
    int unit,
    uint32 flags,
    int profile_id,
    bcm_trunk_profile_t * profile,
    bcm_trunk_profile_info_t * profile_info)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(profile, _SHR_E_PARAM, "profile");
    SHR_NULL_CHECK(profile_info, _SHR_E_PARAM, "profile_info");

    /** Verify flags is 0 */
    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "flags 0x%x are Not supported\n", flags);
    }

    /** verify profile ID is valid */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_user_profile_is_allocated(unit, profile_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "user profile ID %d is not allocated\n", profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/** TBD header */
int
bcm_dnx_trunk_profile_get(
    int unit,
    uint32 flags,
    int profile_id,
    bcm_trunk_profile_t * profile,
    bcm_trunk_profile_info_t * profile_info)
{
    int psc_profile;
    int profile_offset;
    consistent_hashing_calendar_t calendar;
    int psc_max_nof_members;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verifications */
    SHR_INVOKE_VERIFY_DNXC(bcm_dnx_trunk_profile_get_verify(unit, flags, profile_id, profile, profile_info));

    /** Get from profile_id the c-lag profile */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_user_profile_info_psc_profile_get(unit, profile_id, &psc_profile));

    /** Get profile type from sw-state */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_profile_info_get(unit, psc_profile, &calendar.profile_type, &psc_max_nof_members));

    /** Get from c-lag profile the profile_offset */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_psc_profiles_profile_data_get(unit, psc_profile, &profile_offset));

    /** Populate profile with configured members */
    SHR_IF_ERR_EXIT(dnx_trunk_c_lag_calendar_get(unit, profile_offset, &calendar));
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_get(unit, psc_profile, &calendar, profile));

    /** Get profile_info */
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_max_member_id_get(unit, profile, &profile_info->max_member_id));
    SHR_IF_ERR_EXIT(dnx_trunk_user_profile_hw_entries_used_get(unit, profile, &profile_info->hw_entries_used));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/** enum describing the different acceptable actions performed during bcm_trunk_member_update routine */
typedef enum member_update_action_e
{
    bcmTrunkMemberUpdateActionAdd,          /** add protection to member */
    bcmTrunkMemberUpdateActionRemove,       /** remove protection from member */
    bcmTrunkMemberUpdateActionReplace,      /** replace existing protection with another  */
    bcmTrunkMemberUpdateActionSwap,         /** swap between protected and protecting ports */
    bcmTrunkMemberUpdateActionNof
} member_update_action_t;

/**
 * \brief - identify needed update action based on input parameters
 *
 * \param [in] unit - unit number
 * \param [in] matched_member - matched member
 * \param [in] updated_member - updated member
 * \param [out] action - chosen action
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_trunk_member_update_identify_actions(
    int unit,
    bcm_trunk_member_t * matched_member,
    bcm_trunk_member_t * updated_member,
    member_update_action_t * action)
{
    int matched_member_has_protection = _SHR_IS_FLAG_SET(matched_member->flags, BCM_TRUNK_MEMBER_WITH_PROTECTION);
    int updated_member_has_protection = _SHR_IS_FLAG_SET(updated_member->flags, BCM_TRUNK_MEMBER_WITH_PROTECTION);
    int gports_are_matching = (matched_member->gport == updated_member->gport);
    int gports_are_swapped = ((matched_member->gport == updated_member->backup_member.gport)
                              && (updated_member->gport == matched_member->backup_member.gport));
    SHR_FUNC_INIT_VARS(unit);

    *action = bcmTrunkMemberUpdateActionNof;

    /** if both members has protection */
    if (matched_member_has_protection && updated_member_has_protection)
    {
        if (gports_are_swapped)
        {
            *action = bcmTrunkMemberUpdateActionSwap;
        }
        else if (gports_are_matching)
        {
            *action = bcmTrunkMemberUpdateActionReplace;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "undefined action for trunk member update routine \n");
        }
    }
    else if (matched_member_has_protection && gports_are_matching)
    {
        *action = bcmTrunkMemberUpdateActionRemove;
    }
    else if (updated_member_has_protection && gports_are_matching)
    {
        *action = bcmTrunkMemberUpdateActionAdd;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "undefined action for trunk member update routine \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/** perform additional verifications required for updated_member */
static shr_error_e
dnx_trunk_member_update_per_action_verification_for_updated_member(
    int unit,
    member_update_action_t action,
    bcm_trunk_t trunk_id,
    int matched_member_index,
    bcm_trunk_member_t * updated_member)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (action)
    {
        case bcmTrunkMemberUpdateActionAdd:
        case bcmTrunkMemberUpdateActionReplace:
        {
            bcm_trunk_member_t backup_member;
            sal_memset(&backup_member, 0, sizeof(backup_member));
            backup_member.gport = updated_member->backup_member.gport;
            backup_member.flags = updated_member->backup_member.flags;
            backup_member.spa_id = updated_member->backup_member.spa_id;
            SHR_IF_ERR_EXIT(dnx_trunk_backup_member_verify(unit, trunk_id, &backup_member));
            break;
        }
        case bcmTrunkMemberUpdateActionSwap:
        {
            int count;
            int first;
            int last;
            int pool;
            int group;
            bcm_trunk_id_info_t id_info;
            trunk_group_member_info_t member_info;

            /** get pool and group */
            SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
            pool = id_info.pool_index;
            group = id_info.group_index;

            /** get count of member to be added */
            dnx_trunk_sw_db_member_init(unit, &member_info);
            member_info.system_port = updated_member->gport;
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find
                            (unit, pool, group, &member_info, &first, &last, &count));

            /** if member to be added already found in trunk */
            if (count > 0)
            {
                trunk_group_member_info_t existing_backup_member_info;
                trunk_group_member_info_t existing_master_member_info;

                /** get data of both existing backup member and existing master member, if one of them is disabled, swap cannot be performed */
                /** This is due to limitations that only members with a single replications are allowed to be disabled */
                SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, first, &existing_backup_member_info));
                SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get
                                (unit, pool, group, matched_member_index, &existing_master_member_info));
                if ((existing_backup_member_info.flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE)
                    || ((existing_master_member_info.flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE)
                        && (existing_backup_member_info.index >= 0)))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "trying to swap protection for member that will result in violation of member disabling rules");
                }
            }
            break;
        }
        case bcmTrunkMemberUpdateActionRemove:
        {
            /** No extra verifications are needed */
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "unknown action\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** verify function */
static shr_error_e
bcm_dnx_trunk_member_update_verify(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 flags,
    bcm_trunk_member_t * matched_member,
    bcm_trunk_member_t * updated_member)
{
    member_update_action_t action;
    bcm_trunk_id_info_t id_info;
    uint32 valid_flags = BCM_TRUNK_FLAG_ALL_MATCHES;
    trunk_group_member_info_t member_info;
    uint32 flags_mask;
    int matched_member_index;
    int nof_matches;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_trunk.protection.feature_get(unit, dnx_data_trunk_protection_backup_destination))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Lag protection is not supported for this device\n");
    }

    SHR_NULL_CHECK(matched_member, _SHR_E_PARAM, "matched_member");
    SHR_NULL_CHECK(updated_member, _SHR_E_PARAM, "updated_member");

    /** check that trunk_id is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));

    /** Verify only acceptable flags are used */
    if (flags & ~valid_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "some flags of flags 0x%x are Not supported\n", flags);
    }

    /** make sure matched_member is found */
    sal_memset(&member_info, 0, sizeof(member_info));
    flags_mask = BCM_TRUNK_MEMBER_WITH_PROTECTION;
    member_info.system_port = matched_member->gport;
    member_info.flags = matched_member->flags;
    member_info.index =
        (_SHR_IS_FLAG_SET(matched_member->flags, BCM_TRUNK_MEMBER_WITH_ID)) ? matched_member->member_id :
        INVALID_TM_INDEX;
    member_info.spa_member_id =
        (_SHR_IS_FLAG_SET(matched_member->flags, BCM_TRUNK_MEMBER_SPA_WITH_ID)) ? matched_member->spa_id :
        INVALID_SPA_MEMBER_ID;
    member_info.protecting_system_port =
        (_SHR_IS_FLAG_SET(matched_member->flags, BCM_TRUNK_MEMBER_WITH_PROTECTION)) ? matched_member->
        backup_member.gport : dnx_data_device.general.invalid_system_port_get(unit);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_per_criteria_find
                    (unit, id_info.pool_index, id_info.group_index, &member_info, flags_mask, 1, &matched_member_index,
                     &nof_matches));
    if (nof_matches == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "matched_member was not found in trunk\n");
    }

    /** identify wanted action */
    SHR_IF_ERR_EXIT(dnx_trunk_member_update_identify_actions(unit, matched_member, updated_member, &action));

    /** make sure updated member is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_member_update_per_action_verification_for_updated_member
                    (unit, action, trunk_id, matched_member_index, updated_member));

exit:
    SHR_FUNC_EXIT;
}

/* Update protection to existing member. */
/** TBD header */
int
bcm_dnx_trunk_member_update(
    int unit,
    bcm_trunk_t trunk_id,
    uint32 flags,
    bcm_trunk_member_t * matched_member,
    bcm_trunk_member_t * updated_member)
{
    int nof_matches;
    int pool;
    int group;
    bcm_trunk_id_info_t id_info;
    trunk_group_member_info_t member_info;
    member_update_action_t action;
    int nof_members_in_group;
    uint32 flags_mask;
    int *matched_indexes = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Verifications */
    SHR_INVOKE_VERIFY_DNXC(bcm_dnx_trunk_member_update_verify(unit, trunk_id, flags, matched_member, updated_member));

    /** Get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    /** find member or members to update */
    sal_memset(&member_info, 0, sizeof(member_info));
    flags_mask = BCM_TRUNK_MEMBER_WITH_PROTECTION;
    member_info.system_port = matched_member->gport;
    member_info.flags = matched_member->flags;
    member_info.index =
        (_SHR_IS_FLAG_SET(matched_member->flags, BCM_TRUNK_MEMBER_WITH_ID)) ? matched_member->member_id :
        INVALID_TM_INDEX;
    member_info.spa_member_id =
        (_SHR_IS_FLAG_SET(matched_member->flags, BCM_TRUNK_MEMBER_SPA_WITH_ID)) ? matched_member->spa_id :
        INVALID_SPA_MEMBER_ID;
    member_info.protecting_system_port =
        (_SHR_IS_FLAG_SET(matched_member->flags, BCM_TRUNK_MEMBER_WITH_PROTECTION)) ? matched_member->
        backup_member.gport : dnx_data_device.general.invalid_system_port_get(unit);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, pool, group, &nof_members_in_group));
    SHR_ALLOC_ERR_EXIT(matched_indexes, sizeof(*matched_indexes) * nof_members_in_group, "matched_indexes", "%s%s%s",
                       EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_per_criteria_find
                    (unit, pool, group, &member_info, flags_mask, nof_members_in_group, matched_indexes, &nof_matches));

    /** identify wanted action */
    SHR_IF_ERR_EXIT(dnx_trunk_member_update_identify_actions(unit, matched_member, updated_member, &action));

    /** Update members */
    if (!_SHR_IS_FLAG_SET(flags, BCM_TRUNK_FLAG_ALL_MATCHES))
    {
        /** if all matches flag is not set, update only first match */
        nof_matches = 1;
    }
    for (int match_index = 0; match_index < nof_matches; ++match_index)
    {
        if (action == bcmTrunkMemberUpdateActionAdd)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_member_protection_add
                            (unit, pool, group, matched_indexes[match_index], updated_member));
        }
        else if (action == bcmTrunkMemberUpdateActionRemove)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_member_protection_remove(unit, 0, pool, group, matched_indexes[match_index]));
        }
        else if (action == bcmTrunkMemberUpdateActionReplace)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_member_protection_replace
                            (unit, pool, group, matched_indexes[match_index], updated_member));
        }
        else if (action == bcmTrunkMemberUpdateActionSwap)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_member_protection_swap(unit, pool, group, matched_indexes[match_index]));
        }
    }

exit:
    SHR_FREE(matched_indexes);
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * }
 * BCM APIs
 */
