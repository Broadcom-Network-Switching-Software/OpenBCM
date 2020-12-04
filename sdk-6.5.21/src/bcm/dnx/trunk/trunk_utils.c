/** \file src/bcm/dnx/trunk/trunk_utils.c
 * 
 * 
 *  This file contains the implementation on utils functions of
 *  the trunk module.
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
#include "trunk_temp_structs_to_skip_papi.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/swstate/auto_generated/types/trunk_types.h>
#include <soc/dnx/swstate/auto_generated/access/trunk_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/trunk.h>

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

/**
 * see header file
 */
shr_error_e
dnx_trunk_utils_trunk_id_to_id_info_convert(
    int unit,
    int trunk_id,
    bcm_trunk_id_info_t * id_info)
{
    SHR_FUNC_INIT_VARS(unit);

    BCM_TRUNK_ID_POOL_GET(id_info->pool_index, trunk_id);
    BCM_TRUNK_ID_GROUP_GET(id_info->group_index, trunk_id);

    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_utils_id_info_to_trunk_id_convert(
    int unit,
    int *trunk_id,
    bcm_trunk_id_info_t * id_info)
{
    SHR_FUNC_INIT_VARS(unit);

    BCM_TRUNK_ID_SET(*trunk_id, id_info->pool_index, id_info->group_index);

    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_utils_spa_encode(
    int unit,
    int pool,
    int group,
    int member,
    uint32 *spa)
{
    uint32 members_msb;
    int spa_member_shift;
    int spa_group_shift;
    int spa_pool_shift;
    int spa_type_shift;

    SHR_FUNC_INIT_VARS(unit);

    /** get partion location between group and member bits according to bank */
    members_msb = dnx_data_trunk.parameters.pool_info_get(unit, pool)->pool_hw_mode;

    /** calculate the shifts */
    spa_type_shift = dnx_data_trunk.parameters.spa_type_shift_get(unit);
    spa_pool_shift = dnx_data_trunk.parameters.spa_pool_shift_get(unit);
    spa_member_shift = spa_pool_shift - 1 - members_msb;
    spa_group_shift = 0;

    /** use shifts to build the spa */
    *spa = SAL_BIT(spa_type_shift) | (pool << spa_pool_shift) |
        (member << spa_member_shift) | (group << spa_group_shift);

    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_utils_spa_decode(
    int unit,
    int *pool,
    int *group,
    int *member,
    uint32 spa)
{
    uint32 members_msb;
    int spa_member_shift;
    int spa_group_shift;
    int spa_pool_shift;
    int spa_type_shift;

    SHR_FUNC_INIT_VARS(unit);

    /** calculate the shifts (type and pool) */
    spa_type_shift = dnx_data_trunk.parameters.spa_type_shift_get(unit);
    spa_pool_shift = dnx_data_trunk.parameters.spa_pool_shift_get(unit);

    /** get pool */
    *pool = (SAL_UPTO_BIT(spa_type_shift) & spa) >> spa_pool_shift;

    /** get partion location between group and member bits according to bank */
    members_msb = dnx_data_trunk.parameters.pool_info_get(unit, *pool)->pool_hw_mode;

    /** calculate the shifts (member and group) */
    spa_member_shift = spa_pool_shift - 1 - members_msb;
    spa_group_shift = 0;

    /** get group and member */
    *group = (SAL_UPTO_BIT(spa_member_shift) & spa) >> spa_group_shift;
    *member = (SAL_UPTO_BIT(spa_pool_shift) & spa) >> spa_member_shift;

    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_trunk_utils_members_bits_get(
    int unit,
    uint32 spa,
    uint32 *members_lsb,
    uint32 *members_msb)
{
    int spa_pool_shift;
    int spa_type_shift;
    uint32 members_nof_bits = 0;
    int pool;
    SHR_FUNC_INIT_VARS(unit);

    /** calculate the shifts (type and pool) */
    spa_type_shift = dnx_data_trunk.parameters.spa_type_shift_get(unit);
    spa_pool_shift = dnx_data_trunk.parameters.spa_pool_shift_get(unit);

    /** get pool */
    pool = (SAL_UPTO_BIT(spa_type_shift) & spa) >> spa_pool_shift;

    members_nof_bits = dnx_data_trunk.parameters.pool_info_get(unit, pool)->pool_hw_mode;
    *members_lsb = spa_pool_shift - 1 - members_nof_bits;
    *members_msb = *members_lsb + members_nof_bits - 1;

    SHR_FUNC_EXIT;
}
