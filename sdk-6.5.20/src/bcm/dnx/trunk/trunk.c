/** \file src/bcm/dnx/trunk/trunk.c
 *
 *
 *  This file contains the implementation of external API trunk
 *  functions.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TRUNK

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include "trunk_utils.h"
#include "trunk_sw_db.h"
#include "trunk_dbal_access.h"
#include "trunk_temp_structs_to_skip_papi.h"
#include "trunk_verify.h"
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/trunk/trunk_init.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/trunk_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/switch/switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_port.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/stk/stk_trunk.h>
#include <bcm_int/dnx_dispatch.h>
#include <shared/trunk.h>
#include <bcm_int/dnx/port/port_sit.h>
#include <bcm_int/dnx/port/port_prt_tcam.h>

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
 *  add_cb is called from the context of bcm_trunk_set and bcm_trunk_member_add, it will provide the trunk_id and the core and tm_port of the added port as parameters.
 *  delete_cb is called from the context of bcm_trunk_set and bcm_trunk_member_delete, it will provide the trunk_id and the core and tm_port of the deleted port as parameters.
 */
static const dnx_trunk_feature_update_t dnx_trunk_feature_update_cbs[] = {
/** create_cb                           | destroy_cb               | add_cb                                  | delete_cb              */
    {NULL,                                NULL,                      dnx_trunk_egress_pp_port_update,         dnx_port_pp_deleted_trunk_member_update},
#ifdef INCLUDE_XFLOW_MACSEC
    {NULL,                                NULL,                      dnx_xflow_macsec_trunk_member_add,       dnx_xflow_macsec_trunk_member_delete   }
#endif /* INCLUDE_XFLOW_MACSEC */
};
/* } */
/* *INDENT-ON* */

/*
 * Declaration of static functions
 * {
 */
static shr_error_e dnx_trunk_egress_align(
    int unit,
    bcm_trunk_t trunk_id);
/*
 * }
 */

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
 * \param [in] core - core, to move to CB
 * \param [in] tm_port - tm port, to move to CB
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
    bcm_core_t core,
    uint32 tm_port)
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
            SHR_IF_ERR_EXIT(cb(unit, trunk_id, core, tm_port));
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

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_gport_to_spa_verify(unit, trunk_gport, spa));
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

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_spa_to_gport_verify(unit, spa, member, trunk_gport));
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

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_spa_member_mask_verify(unit, spa, spa_with_masked_member));
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
    uint32 nof_cores;
    uint32 pp_port;
    int current_core;
    SHR_FUNC_INIT_VARS(unit);

    /** get number of cores */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    /** iterate over bitmap up-to nof_cores and do action on relevant cores */
    for (current_core = 0; current_core < nof_cores; ++current_core)
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
    uint32 nof_cores;
    uint32 algo_port_flags;
    /** assigning 0 to pp_port just to dismiss a faulty coverity issue - actual value is retrieved by the function */
    uint32 pp_port = 0;
    int current_core;
    bcm_trunk_t tid;
    SHR_FUNC_INIT_VARS(unit);

    /** get number of cores */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);
    /** iterate over bitmap up-to nof_cores and do action on relevant cores */
    for (current_core = 0; current_core < nof_cores; ++current_core)
    {
        if ((SAL_BIT(current_core) & core_bitmap) == 0)
        {
            continue;
        }

        /** allocate pp-port and save it in trunk group */
        algo_port_flags = DNX_ALGO_PORT_PP_PORT_ALLOCATE_F_LAG;
        BCM_TRUNK_ID_SET(tid, id_info->pool_index, id_info->group_index);
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_allocate(unit, algo_port_flags, current_core, tid, &pp_port));
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
    SHR_INVOKE_VERIFY_DNX(dnx_trunk_create_verify(unit, flags, trunk_id));

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
    SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run(unit, DNX_TRUNK_FEATURE_UPDATE_CB_CREATE, *trunk_id, -1, -1));

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
    uint32 member_core_id, port_tm;

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
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get
                        (unit, phy_gport_info.local_port, (bcm_core_t *) & member_core_id, &port_tm));
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
    SHR_INVOKE_VERIFY_DNX(dnx_trunk_pp_port_allocation_set_verify(unit, trunk_id, flags, allocation_info));

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
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_trunk_pp_port_allocation_get_verify(unit, trunk_id, flags, allocation_info));

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
    DNX_ERR_RECOVERY_END(unit);
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
 *        psc_info.
 *
 * \param [in] unit - unit number
 * \param [in] bcm_psc - PSC in BCM format
 * \param [in] psc_info - psc info, relevant for c-lag to
 *        allocate the needed profile
 * \param [in] nof_enabled_members - number of enabled members
 *        in the trunk
 * \param [in] psc_profile - resulted psc profile to be updated
 *        to the HW.
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
dnx_trunk_psc_profile_get(
    int unit,
    int bcm_psc,
    bcm_trunk_psc_profile_info_t * psc_info,
    int nof_enabled_members,
    int *psc_profile)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (bcm_psc)
    {
        case BCM_TRUNK_PSC_C_LAG:
        {
            
            break;
        }
        case BCM_TRUNK_PSC_SMOOTH_DIVISION:
        {
            SHR_IF_ERR_EXIT(dnx_algo_trunk_smooth_division_profile_set(unit, psc_info,
                                                                       nof_enabled_members, psc_profile));
            break;
        }
        case BCM_TRUNK_PSC_PORTFLOW:
        {
            *psc_profile = (nof_enabled_members == 0) ? 0 : nof_enabled_members - 1;
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
 * \param [in] psc_info - retrieved psc info
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
        case BCM_TRUNK_PSC_C_LAG:
        {
            
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

        /** verify amount of members is not more than in master trunk */
        if (member_count > master_member_count)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Virtual trunk can't have more members than in master trunk!"
                         "Virtual trunk has %d members but master has %d members", member_count, master_member_count);
        }

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
    SHR_IF_ERR_EXIT(dnx_trunk_psc_verify(unit, trunk_info->psc));

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, id_info.pool_index, id_info.group_index, &trunk_flags));

    /** verify */
    if (trunk_flags & BCM_TRUNK_FLAG_VP)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_trunk_set_virtual_verify(unit, trunk_id, trunk_info, member_count, member_array));
    }

    
    /** will be implemented once lag scheduling is done */
    /** Check if (has_local_members == 1) and (trunk_group_flags & BCM_TRUNK_FLAG_ALLOW_LOCAL_MEMBERS) == 0 */
    /** Fail, exit with error on param */

exit:
    SHR_FUNC_EXIT;
}


/** this function should manage the PSC profiles, if profile
 *  is no longer used it should be freed. if another lag is
 *  using the profile do nothing. */
/**
 * \brief - this function is used to manage the allocated
 *        profiles of certain PSCs (currently only c-lag) this
 *        function will free unused profiles. if another lag is
 *        using the profile do nothing.
 *
 * \param [in] unit - unit number
 * \param [in] old_psc - old PSC
 * \param [in] old_psc_profile - old PSC profile
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
dnx_trunk_psc_profile_clear(
    int unit,
    int old_psc,
    int old_psc_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}
/** }  */

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
    if (!system_port_is_already_a_member_of_current_trunk)
    {
        bcm_trunk_id_info_t id_info;
        /** get id_info */
        SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
        /** check for matching header types */
        SHR_IF_ERR_EXIT(dnx_trunk_new_member_has_matching_header_type_verify(unit, &id_info, added_member->gport));
    }

exit:
    SHR_FUNC_EXIT;
}


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
    int old_psc;
    int old_psc_profile;
    int nof_enabled_members;
    int pp_member_index;
    int invalid_system_port;
    int system_port_id;
    uint32 member_system_port;
    uint32 member_flags, trunk_flags;
    uint32 core_bitmap[1];
    uint32 pp_ports[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    uint32 pp_port;
    trunk_group_member_info_t member_info;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    bcm_trunk_t current_member_trunk;
    uint32 current_member_flags;
    uint32 port_tm;
    uint32 coe_pp_port;
    bcm_core_t coe_core;
    uint32 flags = 0;
    uint32 tcam_access_id = 0;
    uint32 prt_tcam_key = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** stacking trunk */
    if (_SHR_TRUNK_ID_IS_STACKING(trunk_id))
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_set(unit, trunk_id, member_count, member_array));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_set_verify(unit, trunk_id, trunk_info, member_count, member_array));

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
        if (member_info.index != INVALID_TM_INDEX)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group, member_info.index,
                                                                    dnx_data_device.
                                                                    general.invalid_system_port_get(unit)));
        }
        /** check if member is local and trunk is not virtual */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member_info.system_port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

        if (((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) != 0) &&
            ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0))
        {
            uint32 trunk_in_header_type;
            uint32 trunk_out_header_type;
            bcm_switch_control_info_t in_header_type;
            bcm_switch_control_info_t out_header_type;
            bcm_switch_control_key_t key;
            const uint32 NOT_LAG = 0;
            /** continue if this is not the first replication of this
             *  port, this can be identified by checking if the
             *  trunk_sw_db_trunk_system_port was already updated or not */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, phy_gport_info.sys_port,
                                                                     &current_member_trunk, &current_member_flags));
            if (current_member_trunk == TRUNK_SW_DB_INVALID_TRUNK_ID)
            {
                /** this means that we already changed a member with the same
                 *  system port - so no need to do all of those actions */
                continue;
            }
            /** get tm port from first local port ( there is only one local port on system gport) */
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get
                            (unit, phy_gport_info.local_port, (bcm_core_t *) & core, &port_tm));
            SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, phy_gport_info.local_port, &flags));
            /** allocate new pp port to removed port */
            if (!DNX_PORT_IS_COE_PORT(flags))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_allocate(unit, 0, core, 0, &pp_port));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_get
                                (unit, (uint16) phy_gport_info.local_port, (uint32 *) &coe_core, &coe_pp_port,
                                 &tcam_access_id, &prt_tcam_key));
                SHR_VAL_VERIFY(core, coe_core, _SHR_E_INTERNAL, "COE port Core changed after adding to a trunk!\n");
                pp_port = coe_pp_port;
            }

            /** sw and hw map removed port to new pp port and its original system port */
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_modify(unit, phy_gport_info.local_port, core, pp_port));
            system_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(member_info.system_port);
            SHR_IF_ERR_EXIT(dnx_port_pp_mapping_set(unit, core, pp_port, port_tm, system_port_id, NOT_LAG));
            /** map removed member ptc to pp port according to trunk's header type */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get
                            (unit, pool, group, &trunk_in_header_type, &trunk_out_header_type));
            /** set removed member header types */
            if (!DNX_PORT_IS_COE_PORT(flags))
            {
                key.type = bcmSwitchPortHeaderType;
                /** in direction*/
                key.index = 1;
                in_header_type.value = trunk_in_header_type;
                SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set
                                (unit, phy_gport_info.local_port, key, in_header_type));
                /** out direction*/
                key.index = 2;
                out_header_type.value = trunk_out_header_type;
                SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set
                                (unit, phy_gport_info.local_port, key, out_header_type));
            }

            /** run feature update CBs with the trunk id and the deleted member */
            SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run
                            (unit, DNX_TRUNK_FEATURE_UPDATE_CB_DELETE, trunk_id, core, port_tm));
        }

        /** update member in trunk system port db to invalid */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_set(unit, phy_gport_info.sys_port,
                                                                 TRUNK_SW_DB_INVALID_TRUNK_ID, 0));
        if (DNX_PORT_IS_COE_PORT(flags))
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_coe_port_to_trunk_group_del(unit, phy_gport_info.local_port, trunk_id));
        }
        --remaining_members;
    }

    /** change PSC to M&D with 0 members - as a transition state that should allow freeing PSC resources and such */
    
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_attributes_set(unit, pool, group, BCM_TRUNK_PSC_PORTFLOW, 0));
    /** clean old psc */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &old_psc, &old_psc_profile));
    
    SHR_IF_ERR_EXIT(dnx_trunk_psc_profile_clear(unit, old_psc, old_psc_profile));
    /** reset group in sw db */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_reset(unit, pool, group, FALSE));

    /** set new members */
    for (nof_enabled_members = 0, member_index = 0; member_index < member_count; ++member_index)
    {
        bcm_trunk_t curr_trunk_id;
        uint32 curr_flags;
        int member_is_first_replication = 0;
        uint32 spa;
        int port_in_lag;
        uint32 key_value = 0;

        /** Get system port info */
        member_system_port = member_array[member_index].gport;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member_system_port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

        SHR_IF_ERR_EXIT(dnx_trunk_member_can_be_added_with_set_verify(unit, trunk_id, &member_array[member_index]));

        if (((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) != 0)
            && ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0))
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, phy_gport_info.local_port, &flags));
        }
        if (DNX_PORT_IS_COE_PORT(flags))
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_access_id_get_by_port
                            (unit, phy_gport_info.local_port, DBAL_FIELD_PRTTCAM_KEY_VP_COE, &prt_tcam_key, &coe_core,
                             &tcam_access_id));
            SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, phy_gport_info.local_port, &port_in_lag));
            /**COE port already in the LAG, process for duplicate member*/
            if (port_in_lag)
            {
                SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_get
                                (unit, phy_gport_info.local_port, (uint32 *) &coe_core, &coe_pp_port,
                                 &tcam_access_id, &key_value));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, phy_gport_info.local_port, &coe_core, &coe_pp_port));
            }
        }
        /** has to come after dnx_trunk_member_can_be_added_with_set_verify, because it might change the member flags */
        member_flags = member_array[member_index].flags;

        /** check if this is the first replication of this member,
         *  if it is - its system port shouldn't be mapped to the trunk_id yet.
         *  this has to come before adding the member to trunk SW DB */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, phy_gport_info.sys_port,
                                                                 &curr_trunk_id, &curr_flags));
        member_is_first_replication = (curr_trunk_id != trunk_id);
        /** Update member to trunk sw db */
        member_info.system_port = member_system_port;
        member_info.flags = member_flags;
        if ((member_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) != 0)
        {
            member_info.index = INVALID_TM_INDEX;
        }
        else
        {
            member_info.index = nof_enabled_members;
            ++nof_enabled_members;
        }
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_add(unit, pool, group, &member_info, &pp_member_index));

        /** check if member is local to this unit and that this is the first replication,
         *  in that case need to do PP part of adding trunk */
        if (((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) != 0) && member_is_first_replication &&
            ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0))
        {
            uint32 trunk_in_header_type;
            uint32 trunk_out_header_type;
            const uint32 IS_LAG = 1;
            int port_in_header_type;
            int port_out_header_type;

            /** get tm port and core from local port */
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get
                            (unit, phy_gport_info.local_port, (bcm_core_t *) & core, &port_tm));
            /** verify that the core on which the member's port is
             *  sitting has a corresponding pp port on the trunk */
            SHR_IF_ERR_EXIT(dnx_trunk_has_a_pp_port_on_core_verify(unit, pool, group, core));

            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get
                            (unit, pool, group, &trunk_in_header_type, &trunk_out_header_type));

            LOG_VERBOSE_EX(BSL_LOG_MODULE,
                           "bcm_dnx_trunk_set: adding member: local_port = %d, pp_port = %d, core_id = %d %s\n",
                           phy_gport_info.local_port, phy_gport_info.internal_port_pp_info.pp_port[0],
                           phy_gport_info.internal_port_pp_info.core_id[0], EMPTY);

            if (!DNX_PORT_IS_COE_PORT(flags))
            {
                if (BCM_SWITCH_PORT_HEADER_TYPE_NONE != trunk_in_header_type)
                {
                    /** Get the PP-Port type: */
                    SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                                    (unit, phy_gport_info.local_port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN,
                                     &port_in_header_type));
                    /** See dnx_port_initial_eth_properties_set  - need to deinit pp_port resources */
                    SHR_IF_ERR_EXIT(dnx_port_initial_eth_properties_unset
                                    (unit, phy_gport_info.local_port, port_in_header_type));

                }
                if (BCM_SWITCH_PORT_HEADER_TYPE_NONE != trunk_out_header_type)
                {
                    /** Get the PP-Port type: */
                    SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                                    (unit, phy_gport_info.local_port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT,
                                     &port_out_header_type));
                    /** See dnx_port_pp_egress_set - need to deinit pp_port resources */
                    SHR_IF_ERR_EXIT(dnx_port_pp_egress_unset(unit, phy_gport_info.local_port, port_out_header_type));
                }
            }

            /** sw and hw map the new member with the lag pp port and SPA as system port */
            /** SPA is encoded using the actual member in the LAG group  */
            SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_encode(unit, pool, group, pp_member_index, &spa));
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_modify(unit, phy_gport_info.local_port, core, pp_ports[core]));
            if (DNX_PORT_IS_COE_PORT(flags))
            {
                SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_set
                                (unit, (uint16) phy_gport_info.local_port, (uint32) coe_core, coe_pp_port,
                                 tcam_access_id, prt_tcam_key, 0));
            }
            SHR_IF_ERR_EXIT(dnx_port_pp_mapping_set(unit, core, pp_ports[core], port_tm, spa, IS_LAG));

            /** if COE port added, keep the pp_port */
            if (!DNX_PORT_IS_COE_PORT(flags))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_free
                                (unit, phy_gport_info.internal_port_pp_info.core_id[0],
                                 phy_gport_info.internal_port_pp_info.pp_port[0]));
            }

            /** override pp_port in phy_gport_info with the trunk pp_port */
            phy_gport_info.internal_port_pp_info.pp_port[0] = pp_ports[core];
            phy_gport_info.internal_port_pp_info.core_id[0] = core;
            phy_gport_info.internal_port_pp_info.nof_pp_ports = 1;
            /** map new member ptc to pp port according to trunk's header type */
            SHR_IF_ERR_EXIT(dnx_port_pp_prt_ptc_profile_internal_set(unit, &phy_gport_info, trunk_in_header_type));

            /** run feature update CBs with the trunk id and the added member */
            SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run(unit, DNX_TRUNK_FEATURE_UPDATE_CB_ADD, trunk_id, core, port_tm));
        }
        /** Change TM mapping for enabled members */
        if ((member_flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0)
        {
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group, member_info.index,
                                                                    phy_gport_info.sys_port));
        }

        if (((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) != 0)
            && ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0) && DNX_PORT_IS_COE_PORT(flags))
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_coe_port_to_trunk_group_add
                            (unit, phy_gport_info.local_port, trunk_id, coe_core, coe_pp_port, spa, tcam_access_id,
                             prt_tcam_key));
        }
    }
    /** Update last member added index */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_last_member_added_set(unit, pool, group, member_index - 1));

    /** set new PSC */
    SHR_IF_ERR_EXIT(dnx_trunk_psc_profile_get(unit, trunk_info->psc, &trunk_info->psc_info,
                                              nof_enabled_members, &new_psc_profile));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_set(unit, pool, group, trunk_info->psc, new_psc_profile));
    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_attributes_set(unit, pool, group, trunk_info->psc, new_psc_profile));

    /** align egress trunk for changes done in trunk */
    SHR_IF_ERR_EXIT(dnx_trunk_egress_align(unit, trunk_id));

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
 * \param [in] member_array - array for retrived members
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
    int nof_members_in_group;
    int max_nof_member_in_group;
    int nof_retrived_members;
    int invalid_system_port;
    int arr_index = 0;
    bcm_trunk_id_info_t id_info;
    trunk_group_member_info_t member_info;
    uint32 trunk_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** stacking trunk */
    if (_SHR_TRUNK_ID_IS_STACKING(trunk_id))
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_get(unit, trunk_id, member_max, member_array, member_count));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_get_verify(unit, trunk_id, trunk_info, member_max, member_array, member_count));

    /** Get ID info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    /** Get PSC */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &trunk_info->psc, &psc_profile_id));
    SHR_IF_ERR_EXIT(dnx_trunk_psc_info_get(unit, trunk_info->psc, psc_profile_id, &trunk_info->psc_info));
    /** get number of members */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, pool, group, &nof_members_in_group));
    /** determine how many members to get from the group  */
    nof_retrived_members = nof_members_in_group > member_max ? member_max : nof_members_in_group;
    *member_count = nof_retrived_members;
    /** iterate over members and get them */
    max_nof_member_in_group = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    invalid_system_port = dnx_data_device.general.invalid_system_port_get(unit);
    for (int member = 0; (member < max_nof_member_in_group) && (nof_retrived_members > 0); ++member)
    {
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member, &member_info));
        /** Skip invalid members */
        if (member_info.system_port == invalid_system_port)
        {
            continue;
        }
        member_array[arr_index].gport = member_info.system_port;
        member_array[arr_index].flags = member_info.flags;
        --nof_retrived_members;
        ++arr_index;
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
    DNX_ERR_RECOVERY_END(unit);
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
    int master_nof_members, member_count;
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

    /** get number of members in virtual trunk */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_members_in_group_get(unit, pool, group, &member_count));

    /** verify amount of members in virtual trunk is not more than in master trunk */
    if ((member_count + 1) > master_member_count)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Virtual trunk can't have more members than in master trunk!"
                     "Virtual trunk has %d members but master has %d members", (member_count + 1), master_member_count);
    }

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
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags, trunk_flags;
    bcm_trunk_id_info_t id_info;
    uint32 system_port;
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
    added_member_is_enabled = ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0);
    if ((system_port_is_not_a_member_of_another_trunk) ||
        (system_port_is_already_a_member_of_current_trunk && added_member_is_enabled))
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "this member (system port = 0x%x) is valid,"
                                                " and can be added to trunk.\n"), system_port));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Invalid configuration, either member is already found in another trunk, or a"
                     "missconfiguration was detected with member flags");
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
        case BCM_TRUNK_PSC_C_LAG:
        {
            
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
    int pp_member_index;
    int pool;
    int group;
    int core;
    int nof_enabled_members;
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags, trunk_flags;
    uint32 pp_port;
    int member_found_in_trunk;
    trunk_group_member_info_t member_info;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    bcm_trunk_id_info_t id_info;
    uint32 coe_pp_port;
    bcm_core_t coe_core;
    uint32 tcam_access_id = 0;
    uint32 prt_tcam_key = 0;
    uint32 flags = 0;
    uint32 spa;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** stacking trunk */
    if (_SHR_TRUNK_ID_IS_STACKING(trunk_id))
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_member_add(unit, trunk_id, member));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_member_add_verify(unit, trunk_id, member));

    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;
    /** find nof enabled memebers */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_enabled_members_get(unit, pool, group, &nof_enabled_members));

    /** get trunk flags */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));

    /** get system port info */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, member->gport,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));

    /** check if member is already found in trunk - this has to come before adding the member to trunk SW DB*/
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_trunk_system_port_db_get(unit, phy_gport_info.sys_port,
                                                             &curr_trunk_id, &curr_flags));
    member_found_in_trunk = (trunk_id == curr_trunk_id);
    /** add member to sw db */
    member_info.flags = member->flags;
    member_info.system_port = member->gport;
    /** this index won't be used if added member is disabled */
    if (member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE)
    {
        member_info.index = INVALID_TM_INDEX;
    }
    else
    {
        member_info.index = nof_enabled_members;
    }
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_add(unit, pool, group, &member_info, &pp_member_index));
    /** if member is already found in trunk, no need to add it again from PP perspective */
    if (!member_found_in_trunk)
    {
        /** add member to PP part of the trunk */
        /** check if member is local to this unit and trunk is not virtual */
        if (((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) != 0) &&
            ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0))
        {
            uint32 port_tm;
            uint32 trunk_in_header_type;
            uint32 trunk_out_header_type;
            const uint32 IS_LAG = 1;
            int port_in_header_type;
            int port_out_header_type;

            /** Get tm port and core from local port */
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get
                            (unit, phy_gport_info.local_port, (bcm_core_t *) & core, &port_tm));
            SHR_IF_ERR_EXIT(dnx_trunk_has_a_pp_port_on_core_verify(unit, pool, group, core));

            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get
                            (unit, pool, group, &trunk_in_header_type, &trunk_out_header_type));

            LOG_VERBOSE_EX(BSL_LOG_MODULE,
                           "bcm_dnx_trunk_member_add: adding member: local_port = %d, pp_port = %d, core_id = %d %s\n",
                           phy_gport_info.local_port, phy_gport_info.internal_port_pp_info.pp_port[0],
                           phy_gport_info.internal_port_pp_info.core_id[0], EMPTY);

            SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, phy_gport_info.local_port, &flags));
            if (DNX_PORT_IS_COE_PORT(flags))
            {
                SHR_IF_ERR_EXIT(dnx_port_sit_virtual_port_access_id_get_by_port
                                (unit, phy_gport_info.local_port, DBAL_FIELD_PRTTCAM_KEY_VP_COE, &prt_tcam_key,
                                 &coe_core, &tcam_access_id));
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, phy_gport_info.local_port, &coe_core, &coe_pp_port));
            }
            if (!DNX_PORT_IS_COE_PORT(flags))
            {
                if (BCM_SWITCH_PORT_HEADER_TYPE_NONE != trunk_in_header_type)
                {
                    /** Get the PP-Port type: */
                    SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                                    (unit, phy_gport_info.local_port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_IN,
                                     &port_in_header_type));

                    /** See dnx_port_initial_eth_properties_set - need to deinit pp_port resources */
                    SHR_IF_ERR_EXIT(dnx_port_initial_eth_properties_unset
                                    (unit, phy_gport_info.local_port, port_in_header_type));

                }
                if (BCM_SWITCH_PORT_HEADER_TYPE_NONE != trunk_out_header_type)
                {
                    /** Get the PP-Port type: */
                    SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                                    (unit, phy_gport_info.local_port, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT,
                                     &port_out_header_type));
                    /** See dnx_port_initial_eth_properties_set and dnx_port_pp_egress_set - need to deinit pp_port resources */
                    SHR_IF_ERR_EXIT(dnx_port_pp_egress_unset(unit, phy_gport_info.local_port, port_out_header_type));
                }
            }

            /** SW and HW map the new member with the lag PP port and SPA as system port */
            /** SPA is encoded using the actual member in the LAG group */
            SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_encode(unit, pool, group, pp_member_index, &spa));
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_pp_port_get(unit, pool, group, core, &pp_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_modify(unit, phy_gport_info.local_port, core, pp_port));
            if (DNX_PORT_IS_COE_PORT(flags))
            {
                SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_set
                                (unit, (uint16) phy_gport_info.local_port, (uint32) coe_core, coe_pp_port,
                                 tcam_access_id, prt_tcam_key, 0));
            }
            SHR_IF_ERR_EXIT(dnx_port_pp_mapping_set(unit, core, pp_port, port_tm, spa, IS_LAG));

            /** if COE port added, keep the pp_port */
            if (!DNX_PORT_IS_COE_PORT(flags))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_free
                                (unit, phy_gport_info.internal_port_pp_info.core_id[0],
                                 phy_gport_info.internal_port_pp_info.pp_port[0]));
            }

            /** override pp_port in phy_gport_info with the trunk pp_port */
            phy_gport_info.internal_port_pp_info.pp_port[0] = pp_port;
            phy_gport_info.internal_port_pp_info.core_id[0] = core;
            phy_gport_info.internal_port_pp_info.nof_pp_ports = 1;
            /** map new member PTC to PP port according to trunk's header type */
            SHR_IF_ERR_EXIT(dnx_port_pp_prt_ptc_profile_internal_set(unit, &phy_gport_info, trunk_in_header_type));

            /** run feature update CBs with the added member and trunk id, after the member was added to pp */
            SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run(unit, DNX_TRUNK_FEATURE_UPDATE_CB_ADD, trunk_id, core, port_tm));
        }
    }

    /** if member is enabled */
    if ((member->flags & BCM_TRUNK_MEMBER_EGRESS_DISABLE) == 0)
    {
        /** add member to TM */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group,
                                                                nof_enabled_members, phy_gport_info.sys_port));
        /** resolve PSC */
        SHR_IF_ERR_EXIT(dnx_trunk_psc_resolve(unit, pool, group, TRUE, nof_enabled_members));
    }

    /** align egress trunk for changes done in trunk */
    SHR_IF_ERR_EXIT(dnx_trunk_egress_align(unit, trunk_id));

    /** process for COE port */
    if (!member_found_in_trunk)
    {
        if (((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) != 0) &&
            ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0))
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_coe_port_to_trunk_group_add
                            (unit, phy_gport_info.local_port, trunk_id, coe_core, coe_pp_port, spa, tcam_access_id,
                             prt_tcam_key));
        }
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

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - delete a member from trunk. it is possible to
 *        disable a member instead of deleting it. this can be
 *        done by adding the flag
 *        BCM_TRUNK_MEMBER_EGRESS_DISABLE. this is posible only
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
    int first_pp_index;
    int last_pp_index;
    int pool;
    int group;
    int deleting_a_regular_member;
    int disabling_a_regular_member;
    int deleting_a_disabled_member;
    int psc;
    int psc_profile;
    bcm_trunk_t curr_trunk_id;
    uint32 curr_flags, trunk_flags;
    dnx_algo_gpm_gport_phy_info_t phy_gport_info;
    trunk_group_member_info_t member_info;
    bcm_trunk_id_info_t id_info;
    trunk_group_member_info_t removed_member_info;
    int nof_enabled_members;
    int member_is_local_to_this_fap, trunk_is_virtual;
    int core;
    uint32 pp_port;
    uint32 switched_system_port;
    uint32 system_port_id;
    uint32 port_tm;
    uint32 coe_pp_port;
    bcm_core_t coe_core;
    uint32 tcam_access_id = 0;
    uint32 prt_tcam_key = 0;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** stacking trunk */
    if (_SHR_TRUNK_ID_IS_STACKING(trunk_id))
    {
        SHR_IF_ERR_EXIT(dnx_stk_trunk_stacking_member_delete(unit, trunk_id, member));
        SHR_EXIT();
    }

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_member_delete_verify(unit, trunk_id, member));
    /** get id_info */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;

    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_psc_get(unit, pool, group, &psc, &psc_profile));
    if (psc == BCM_TRUNK_PSC_C_LAG)
    {
        
        /** delete from tm and audjust according to c-lag flow ( forbidden to change tm indexes after creation )*/
    }
    else
    {
        /**  there are 2 conceptual steps in removing a member from a
         *   trunk - PP related actions and TM related actions,
         *   according to the state of the trunk one or both of those
         *   steps are required. when deleting completely a regular
         *   member need to do both. when deleting a disabled member
         *   need to do PP part. when disabling a regular member need
         *   to do TM part */

        /** get trunk flags */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_flags_get(unit, pool, group, &trunk_flags));

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
        member_info.system_port = member->gport;
        member_info.flags = member->flags;
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_in_group_find(unit, pool, group, &member_info, &first_pp_index,
                                                             &last_pp_index, &nof_replications));
        if (deleting_a_regular_member || disabling_a_regular_member)
        {
            /** TM related actions */
            /** we will always remove the last member, so - need to switch the member that we actually
             *  want to remove with the last member */
            /** get member to be removed */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, last_pp_index, &removed_member_info));
            /** get tm index of last member ( member with biggest TM index) */
            SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_enabled_members_get(unit, pool, group, &nof_enabled_members));
            /** swap last with removed in dbal */
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_get(unit, pool, group, nof_enabled_members - 1,
                                                                    &switched_system_port));
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group, removed_member_info.index,
                                                                    switched_system_port));
            /** resolve PSC */
            SHR_IF_ERR_EXIT(dnx_trunk_psc_resolve(unit, pool, group, FALSE, nof_enabled_members));

            /** set last to invalid - this must be done only after
             *  resolving PSC issue - to preventing a packet from being
             *  routed to an invalid member (on a local scale) - on  a
             *  system wide scale this cannot be resolved */
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_members_set(unit, pool, group, nof_enabled_members - 1,
                                                                    dnx_data_device.
                                                                    general.invalid_system_port_get(unit)));
        }

        if (deleting_a_regular_member || deleting_a_disabled_member)
        {
            /** PP related actions - relevant only for members which are
             *  local to this fap, and for system_port that this is their last
             *  replication and non virtual trunks */
            member_is_local_to_this_fap = (phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT);
            trunk_is_virtual = (trunk_flags & BCM_TRUNK_FLAG_VP);
            if ((nof_replications == 1) && (member_is_local_to_this_fap) && !(trunk_is_virtual))
            {
                uint32 trunk_in_header_type;
                uint32 trunk_out_header_type;
                bcm_switch_control_info_t in_header_type;
                bcm_switch_control_info_t out_header_type;
                bcm_switch_control_key_t key;
                const uint32 NOT_LAG = 0;
                /** Get tm port and core from local port */
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get
                                (unit, phy_gport_info.local_port, (bcm_core_t *) & core, &port_tm));

                SHR_IF_ERR_EXIT(dnx_port_sit_port_flag_get(unit, phy_gport_info.local_port, &flags));
                if (!DNX_PORT_IS_COE_PORT(flags))
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_allocate(unit, 0, core, 0, &pp_port));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_get
                                    (unit, (uint16) phy_gport_info.local_port, (uint32 *) &coe_core, &coe_pp_port,
                                     &tcam_access_id, &prt_tcam_key));
                    SHR_VAL_VERIFY(core, coe_core, _SHR_E_INTERNAL, "COE port Core changed after adding to a trunk!\n");
                    pp_port = coe_pp_port;
                }

                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_modify(unit, phy_gport_info.local_port, core, pp_port));

                system_port_id = BCM_GPORT_SYSTEM_PORT_ID_GET(member_info.system_port);
                SHR_IF_ERR_EXIT(dnx_port_pp_mapping_set(unit, core, pp_port, port_tm, system_port_id, NOT_LAG));

                if (!DNX_PORT_IS_COE_PORT(flags))
                {
                    /** map removed member ptc to pp port according to trunk's header type */
                    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_header_types_get
                                    (unit, pool, group, &trunk_in_header_type, &trunk_out_header_type));
                    /** set removed member header types */
                    key.type = bcmSwitchPortHeaderType;
                    /** in direction*/
                    key.index = 1;
                    in_header_type.value = trunk_in_header_type;
                    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set
                                    (unit, phy_gport_info.local_port, key, in_header_type));
                    /** out direction*/
                    key.index = 2;
                    out_header_type.value = trunk_out_header_type;
                    SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_port_set
                                    (unit, phy_gport_info.local_port, key, out_header_type));
                }

                /** run feature update CBs with the removed member and trunk id, after the member was removed from pp */
                SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run
                                (unit, DNX_TRUNK_FEATURE_UPDATE_CB_DELETE, trunk_id, core, port_tm));
            }
        }
        /** delete member from sw db */
        SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_delete(unit, pool, group, last_pp_index,
                                                      nof_replications, disabling_a_regular_member));
        /** align egress trunk for changes done in trunk */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_align(unit, trunk_id));

        if (((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) != 0)
            && ((trunk_flags & BCM_TRUNK_FLAG_VP) == 0)
            && (deleting_a_regular_member || deleting_a_disabled_member) && DNX_PORT_IS_COE_PORT(flags))
        {
            SHR_IF_ERR_EXIT(dnx_port_sit_coe_port_to_trunk_group_del(unit, phy_gport_info.local_port, trunk_id));
        }
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

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_member_delete_all_verify(unit, trunk_id));
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
    SHR_INVOKE_VERIFY_DNX(dnx_trunk_find_verify(unit, modid, port, trunk_id));

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

shr_error_e
dnx_trunk_flow_agg_group_set(
    int unit,
    uint32 flags,
    bcm_flow_agg_info_t * flow_agg_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "%s TBD TBD TBD.", FUNCTION_NAME());

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_trunk_flow_agg_group_get(
    int unit,
    uint32 flags,
    bcm_flow_agg_info_t * flow_agg_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "%s TBD TBD TBD.", FUNCTION_NAME());

exit:
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
    SHR_INVOKE_VERIFY_DNX(dnx_trunk_destroy_verify(unit, trunk_id));

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
    SHR_IF_ERR_EXIT(dnx_trunk_features_cbs_run(unit, DNX_TRUNK_FEATURE_UPDATE_CB_DESTROY, trunk_id, -1, -1));

    /** free all pp ports */

    bcm_trunk_pp_port_allocation_info_t_init(&allocation_info);
    SHR_IF_ERR_EXIT(bcm_dnx_trunk_pp_port_allocation_set(unit, trunk_id, 0, &allocation_info));

    /** set trunk_id to not used */
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_set(unit, id_info.pool_index, id_info.group_index, FALSE));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - update the dbal with ranges of lb-keys. system ports
 *        which are mapped to local members will be updated
 *        according to their TM port on the relevant core. the
 *        other core or in case that the system port is not
 *        mapped to a local port, the value for the lb key will
 *        be a dedicated invalid value that should cause drop.
 *
 * \param [in] unit - unit number
 * \param [in] egress_trunk_id - egress trunk id - key for the
 *        dbal
 * \param [in] lb_key_ranges_arr_size - size of lb-key ranges
 *        array
 * \param [in] lb_key_ranges_arr - lb-key ranges array
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
dnx_trunk_egress_lb_key_ranges_dbal_update(
    int unit,
    int egress_trunk_id,
    int lb_key_ranges_arr_size,
    dnx_algo_trunk_member_lb_key_ranges_t * lb_key_ranges_arr)
{
    int nof_cores;

    SHR_FUNC_INIT_VARS(unit);

    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    for (int lb_key_range_entry = 0; lb_key_range_entry < lb_key_ranges_arr_size; ++lb_key_range_entry)
    {
        /** update dbal - if local with relevant entries, if not with invalid ones */
        dnx_algo_gpm_gport_phy_info_t phy_gport_info;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, lb_key_ranges_arr[lb_key_range_entry].system_port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_NONE, &phy_gport_info));
        for (int core = 0; core < nof_cores; ++core)
        {
            int first;
            int range;
            uint32 out_port;
            uint32 tm_core_id, port_tm;

            /** Get tm port and core from local port */
            if (phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get
                                (unit, phy_gport_info.local_port, (bcm_core_t *) & tm_core_id, &port_tm));
            }

            if ((phy_gport_info.flags & DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT) && (core == tm_core_id))
            {
                out_port = port_tm;
            }
            else
            {
                out_port = dnx_data_trunk.egress_trunk.invalid_pp_dsp_get(unit);
            }
            first = lb_key_ranges_arr[lb_key_range_entry].first_lb_key;
            range = lb_key_ranges_arr[lb_key_range_entry].lb_key_range;
            SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_range_set(unit, core, egress_trunk_id,
                                                                         first, range, out_port));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - align egress trunks to current number and
 *        composition of members in the trunk - this is to allow
 *        egress MC proper behavior.
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
dnx_trunk_egress_align(
    int unit,
    bcm_trunk_t trunk_id)
{
    int first_in_range = 0;
    int total_range;
    int egress_trunk_id;
    int profile_found;
    int nof_enabled_members;
    int arr_size;
    int nof_unique_members;
    int pool;
    int group;
    int lb_key_ranges_arr_size;
    bcm_trunk_id_info_t id_info;
    dnx_trunk_unique_member_t *unique_members_arr = NULL;
    dnx_algo_trunk_member_lb_key_ranges_t *lb_key_ranges_arr = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** if input trunk doesn't have a corresponding egress trunk - get out, nothing to do */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get(unit, trunk_id, &egress_trunk_id, &profile_found));
    if (!profile_found)
    {
        SHR_EXIT();
    }

    total_range = dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit);
    /** if graceful lag modification is supported */
    if (dnx_data_trunk.graceful.feature_get(unit, dnx_data_trunk_graceful_allow))
    {
        uint32 multiple_configuration_enable;
        uint32 active_configuration_selector;
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_active_configuration_selector_get(unit,
                                                                                      &multiple_configuration_enable,
                                                                                      &active_configuration_selector));

        /** range is halved, this is to create a stand by and current configurations */
        total_range /= 2;
        /** set first in range with correct position according to stand-by configuration at this point of time */
        first_in_range = (active_configuration_selector ==
                          DBAL_ENUM_FVAL_GRACEFUL_ACTIVE_CONFIGURATION_GRACEFUL_ACTIVE_CONFIGURATION_FIRST) ?
            total_range : 0;
    }

    /** get number of enabled members in trunk  */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    pool = id_info.pool_index;
    group = id_info.group_index;
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_nof_enabled_members_get(unit, pool, group, &nof_enabled_members));

    /** get unique members info */
    arr_size = dnx_data_trunk.parameters.pool_info_get(unit, pool)->max_nof_members_in_group;
    unique_members_arr = sal_alloc(sizeof(dnx_trunk_unique_member_t) * arr_size, "unique members array");
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_unique_members_info_get(unit, pool, group, unique_members_arr,
                                                            arr_size, &nof_unique_members));
    /** allocate place for returned array of lb key ranges, the allocated size is +1 to cover the trivial case of empty trunk */
    lb_key_ranges_arr = sal_alloc(sizeof(dnx_algo_trunk_member_lb_key_ranges_t) * (nof_unique_members + 1),
                                  "lb key ranges array");
    /** get lb-key distribution per unique member */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_trunk_lb_key_destribution_get(unit, total_range, first_in_range,
                                                                        nof_enabled_members, nof_unique_members,
                                                                        unique_members_arr, &lb_key_ranges_arr_size,
                                                                        lb_key_ranges_arr));
    /** update dbal with received distribution */
    SHR_IF_ERR_EXIT(dnx_trunk_egress_lb_key_ranges_dbal_update(unit, egress_trunk_id, lb_key_ranges_arr_size,
                                                               lb_key_ranges_arr));

exit:
    SHR_FREE(unique_members_arr);
    SHR_FREE(lb_key_ranges_arr);
    SHR_FUNC_EXIT;
}

/**
 * \brief - init egress_trunk profile.
 *
 * \param [in] unit - unit number
 * \param [in] egress_trunk_id - egress trunk id
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
dnx_trunk_egress_profile_init(
    int unit,
    int egress_trunk_id)
{
    int first_in_range = 0;
    int range = dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit);
    int out_port = dnx_data_trunk.egress_trunk.invalid_pp_dsp_get(unit);
    int nof_cores = dnx_data_device.general.nof_cores_get(unit);

    SHR_FUNC_INIT_VARS(unit);
    for (int core = 0; core < nof_cores; ++core)
    {
        /** set range of entire profile to invalid out port */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_range_set(unit, core, egress_trunk_id,
                                                                     first_in_range, range, out_port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_egress_create(
    int unit,
    bcm_trunk_t trunk_id,
    int *egress_trunk_id)
{
    uint8 is_first;
    bcm_trunk_id_info_t id_info;
    SHR_FUNC_INIT_VARS(unit);

    /** verify input */
    /** Verify trunk_id is valid */
    SHR_IF_ERR_EXIT(dnx_trunk_t_verify(unit, trunk_id));
    /** Verify trunk was created */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_trunk_id_to_id_info_convert(unit, trunk_id, &id_info));
    SHR_IF_ERR_EXIT(dnx_trunk_was_created_verify(unit, &id_info));
    /** NULL Check */
    SHR_NULL_CHECK(egress_trunk_id, _SHR_E_PARAM, "egress_trunk_id");

    /** create egress trunk profile */
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_algo_trunk_egress_create(unit, trunk_id, egress_trunk_id, &is_first),
                             "Out of free Egress LAGs %s%s%s\n", EMPTY, EMPTY, EMPTY);

    if (is_first)
    {
        /** init profile in HW (basically clear the HW for this profile) */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_profile_init(unit, *egress_trunk_id));

        /** align profile with trunk members */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_align(unit, trunk_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_egress_destroy(
    int unit,
    bcm_trunk_t trunk_id)
{
    int egress_trunk_id;
    int profile_found;
    uint8 is_last = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the  egress profile of the trunk */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get(unit, trunk_id, &egress_trunk_id, &profile_found));
    if (!profile_found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Trunk ID %d does not have egress profile allocated\n", trunk_id);
    }

    /** Destroy the trunk egress profile. */
    SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_destroy(unit, egress_trunk_id, &is_last));
    if (is_last)
    {
        /** init profile in HW (basically clear the HW for this profile) */
        SHR_IF_ERR_EXIT(dnx_trunk_egress_profile_init(unit, egress_trunk_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * see header file
 */
shr_error_e
dnx_trunk_containing_egress_trunk_get(
    int unit,
    int egress_trunk_id,
    bcm_trunk_t * trunk_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trunk_id, _SHR_E_PARAM, "trunk_id");
    SHR_IF_ERR_EXIT(dnx_algo_trunk_containing_egress_trunk_get(unit, egress_trunk_id, trunk_id));
exit:
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
    if (core >= dnx_data_device.general.nof_cores_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid core (%d) for this device\n", core);
    }

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
    uint32 member_core_id, port_tm;

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
    unique_members_arr = sal_alloc(sizeof(dnx_trunk_unique_member_t) * arr_size, "unique members array");
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
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get
                            (unit, phy_gport_info.local_port, (bcm_core_t *) & member_core_id, &port_tm));
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

/*
 * see header file
 */
shr_error_e
dnx_trunk_active_configuration_to_stand_by_configuration_copy(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    /** if graceful lag modification is supported */
    if (dnx_data_trunk.graceful.feature_get(unit, dnx_data_trunk_graceful_allow))
    {
        int nof_cores;
        uint32 single_configuration_range;
        uint32 single_configuration_msb;
        uint32 single_configuration_mask;
        uint32 get_mask;
        uint32 set_mask;
        uint32 nof_egress_trunks;
        uint32 multiple_configuration_enable;
        uint32 active_configuration_selector;
        /** get current configuration */
        SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_active_configuration_selector_get(unit,
                                                                                      &multiple_configuration_enable,
                                                                                      &active_configuration_selector));
        single_configuration_range = dnx_data_trunk.egress_trunk.nof_lb_keys_per_profile_get(unit);
        /** determine set and get masks for entries, this algo goes
         *  over half the range of a single configuration (when
         *  working in graceful mode half the range is active and
         *  half is standby) and use a mask to manipulate active and
         *  standby ranges, the MSB is used to determine which is
         *  which */
        single_configuration_msb = utilex_msb_bit_on(single_configuration_range - 1);
        single_configuration_mask = 1 << single_configuration_msb;
        if (active_configuration_selector == 0)
        {
            get_mask = 0;
            set_mask = single_configuration_mask;
        }
        else
        {
            get_mask = single_configuration_mask;
            set_mask = 0;
        }
        nof_cores = dnx_data_device.general.nof_cores_get(unit);
        nof_egress_trunks = dnx_data_trunk.egress_trunk.nof_get(unit);
        /** iterate on all cores */
        for (int core = 0; core < nof_cores; ++core)
        {
            /** iterate on all egress trunks */
            for (int egress_trunk = 0; egress_trunk < nof_egress_trunks; ++egress_trunk)
            {
                /** iterate on half configuation range, and copy
                 *  configuration from active half to inactive half according
                 *  to set and get masks */
                for (uint32 entry = 0; entry < (single_configuration_range / 2); ++entry)
                {
                    uint32 tm_port;
                    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_mc_resolution_get
                                    (unit, core, egress_trunk, (entry | get_mask), &tm_port));
                    SHR_IF_ERR_EXIT(dnx_trunk_dbal_access_trunk_egress_mc_resolution_set
                                    (unit, core, egress_trunk, (entry | set_mask), tm_port));
                }
            }
        }

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
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_trunk_spa_to_system_phys_port_map_get_verify(unit, flags, system_port_aggregate, gport));

    /** get pool, member and group from SPA ID */
    SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_decode(unit, &pool, &group, &member, system_port_aggregate));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_member_get(unit, pool, group, member, &member_info));
    SHR_IF_ERR_EXIT(dnx_trunk_sw_db_group_is_used_get(unit, pool, group, &is_used));

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
    DNX_ERR_RECOVERY_END(unit);
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
 * }
 * BCM APIs
 */
