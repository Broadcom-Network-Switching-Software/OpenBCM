/** \file rx_trap_map_dispatch.c
 * 
 *
 * Data for bcm to dnx and dnx to HW mapping
 *
 * This file implements mapping from BCM to DNX qualifiers and access for per DNX qualifier/action information
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <include/bcm_int/dnx/rx/rx_trap_map.h>

/*
 * }Include files
 */

shr_error_e
dnx_rx_trap_is_user_defined_dispatch(
    int unit,
    dnx_rx_trap_block_e trap_block,
    int trap_id,
    uint8 *is_user_defined_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_is_user_defined(unit, trap_block, trap_id, is_user_defined_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_type_map_info_get_dispatch(
    int unit,
    bcm_rx_trap_t trap_type,
    dnx_rx_trap_map_type_t * trap_type_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_map_info_get(unit, trap_type, trap_type_info_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_ingress_ud_map_info_get_dispatch(
    int unit,
    int trap_id,
    dnx_rx_trap_map_ingress_ud_t * ingress_ud_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_ingress_ud_map_info_get(unit, trap_id, ingress_ud_info_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_map_ingress_ud_alloc_get_dispatch(
    int unit,
    int alloc_index,
    int *trap_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_map_ingress_ud_alloc_get(unit, alloc_index, trap_id_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_id_boundry_ranges_map_info_get_dispatch(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_map_ranges_t * trap_id_range_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_id_boundry_ranges_map_info_get(unit, trap_block, trap_id_range_info_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_etpp_lif_profile_map_to_dbal_trap_id_dispatch(
    int unit,
    dnx_rx_trap_etpp_lif_profiles_e lif_trap_profile,
    dbal_enum_value_field_etpp_trap_id_e * dbal_trap_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_lif_profile_map_to_dbal_trap_id(unit, lif_trap_profile, dbal_trap_id_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_appl_trap_map_info_get_dispatch(
    int unit,
    bcm_rx_trap_t trap_type,
    dnx_rx_trap_map_type_t * appl_trap_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_type, appl_trap_info_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_user_defined_block_map_info_get_dispatch(
    int unit,
    dnx_rx_trap_block_e trap_block,
    dnx_rx_trap_map_ud_t * ud_block_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_user_defined_block_map_info_get(unit, trap_block, ud_block_info_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_stat_obj_map_info_get_dispatch(
    int unit,
    uint32 stat_obj_profile,
    dnx_rx_trap_map_stat_obj_dbal_fields_t * stat_obj_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_stat_obj_map_info_get(unit, stat_obj_profile, stat_obj_info_p));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_type_description_map_info_get_dispatch(
    int unit,
    bcm_rx_trap_t trap_type,
    char **description)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_type_description_map_info_get(unit, trap_type, description));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_rx_trap_description_init_check_dispatch(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    {
#ifdef BCM_DNX2_SUPPORT
        SHR_IF_ERR_EXIT(dnx_rx_trap_description_init_check(unit));
#endif
    }

exit:
    SHR_FUNC_EXIT;
}
