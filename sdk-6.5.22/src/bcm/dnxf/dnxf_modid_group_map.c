/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF MODID GROUP MAP
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnxf/dnxf_modid_group_map.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

/*
 * Clear modid group id map
 */
int
dnxf_modid_group_map_clear(
    int unit)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);

    /** clear all modid.group_map */
    for (i = 0; i < SOC_DNXF_MODID_GROUP_NOF(unit); i++)
    {
        SHR_IF_ERR_EXIT(dnxf_state.modid.group_map.
                        raw.bit_range_clear(unit, i, 0, dnxf_data_fabric.topology.nof_system_modid_get(unit)));
    }

    /** clear all modid.modid_to_group_map */
    for (i = 0; i < dnxf_data_fabric.topology.nof_system_modid_get(unit); i++)
    {
        SHR_IF_ERR_EXIT(dnxf_state.modid.modid_to_group_map.set(unit, i, DNXF_MODID_GROUP_INVALID));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Clear modid single group
 */
int
dnxf_modid_group_map_clear_group(
    int unit,
    bcm_module_t group)
{
    int modid;
    SHR_BITDCL *modid_bitmap = NULL;
    uint32 nof_system_modid = dnxf_data_fabric.topology.nof_system_modid_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(modid_bitmap, SHR_BITALLOCSIZE(nof_system_modid),
                       "modid bitmap", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** read groups relevant modids before clearing them */
    SHR_IF_ERR_EXIT(dnxf_state.modid.group_map.raw.bit_range_read(unit, group, 0, 0, nof_system_modid, modid_bitmap));

    /** clear group's modids */
    SHR_IF_ERR_EXIT(dnxf_state.modid.group_map.raw.bit_range_clear(unit, group, 0, nof_system_modid));

    /** clear modid to group mapping */
    SHR_BIT_ITER(modid_bitmap, nof_system_modid, modid)
    {
        SHR_IF_ERR_EXIT(dnxf_state.modid.modid_to_group_map.set(unit, modid, DNXF_MODID_GROUP_INVALID));
    }

exit:
    SHR_FREE(modid_bitmap);
    SHR_FUNC_EXIT;
}

/*
 * Add module to a group
 */
int
dnxf_modid_group_map_add(
    int unit,
    bcm_module_t group,
    bcm_module_t module)
{
    SHR_FUNC_INIT_VARS(unit);

    /** map group to modid */
    SHR_IF_ERR_EXIT(dnxf_state.modid.group_map.raw.bit_set(unit, group, module));

    /** map modid to group */
    SHR_IF_ERR_EXIT(dnxf_state.modid.modid_to_group_map.set(unit, module, group));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Get list of modules for a group
 */
int
dnxf_modid_group_map_get_list(
    int unit,
    bcm_module_t group,
    int modid_max_count,
    bcm_module_t * moidid_array,
    int *modid_count)
{
    int modid_iter;
    SHR_BITDCL *modid_bitmap = NULL;
    uint32 nof_system_modid = dnxf_data_fabric.topology.nof_system_modid_get(unit);
    int modid_cnt_local = 0;

    SHR_FUNC_INIT_VARS(unit);

    *modid_count = 0;

    /** alloc bitmap */
    SHR_ALLOC_SET_ZERO(modid_bitmap, SHR_BITALLOCSIZE(nof_system_modid),
                       "modid bitmap", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** read modid bitmap of relevant group */
    SHR_IF_ERR_EXIT(dnxf_state.modid.group_map.raw.bit_range_read(unit, group, 0, 0, nof_system_modid, modid_bitmap));

    /** verify output array size */
    SHR_BITCOUNT_RANGE(modid_bitmap, modid_cnt_local, 0, nof_system_modid);
    if (modid_cnt_local >= modid_max_count)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Array is too small");
    }

    /** populate output array */
    modid_cnt_local = 0;
    SHR_BIT_ITER(modid_bitmap, nof_system_modid, modid_iter)
    {
        moidid_array[modid_cnt_local++] = modid_iter;
    }

    *modid_count = modid_cnt_local;

exit:
    SHR_FREE(modid_bitmap);
    SHR_FUNC_EXIT;
}

/*
 * Get group id of a module
 */
int
dnxf_modid_group_map_get_group(
    int unit,
    bcm_module_t module,
    bcm_module_t * group)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxf_state.modid.modid_to_group_map.get(unit, module, group));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
