/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE MODID GROUP MAP
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FABRIC
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dfe/dfe_modid_group_map.h>

#include <shared/bitop.h>

#include <sal/core/libc.h>

#include <soc/dfe/cmn/dfe_warm_boot.h>

/*
 * Clear modid group id map
 */
int bcm_dfe_modid_group_map_clear(int unit)
{
    int i;
    _bcm_dfe_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    
    sal_memset(group_map.raw,0, SOC_DFE_MODID_NOF_UINT32_SIZE * sizeof(uint32));
    for(i=0 ; i< DFE_MODID_GROUP_MAP_ROWS(unit) ; i++)
    {
        rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_GROUP_MAP, i, &group_map);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    modid_to_group_id = DFE_MODID_GROUP_INVALID;
    for(i=0 ; i< SOC_DFE_MODID_NOF ; i++)
    {
        rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_MODID_TO_GROUP_MAP, i, &modid_to_group_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Clear modid single group 
 */
int bcm_dfe_modid_group_map_clear_group(int unit, bcm_module_t group)
{
    int i,
        rv;
    _bcm_dfe_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(group_map.raw,0,SOC_DFE_MODID_NOF_UINT32_SIZE * sizeof(uint32));
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_GROUP_MAP, group, &group_map);
    BCMDNX_IF_ERR_EXIT(rv);

    for(i=0 ; i < SOC_DFE_MODID_NOF ; i++)
    {
        rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_MODID_TO_GROUP_MAP, i, &modid_to_group_id);
        BCMDNX_IF_ERR_EXIT(rv);
        if(group == modid_to_group_id) {
             modid_to_group_id = DFE_MODID_GROUP_INVALID;
             rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_MODID_TO_GROUP_MAP, i, &modid_to_group_id);
             BCMDNX_IF_ERR_EXIT(rv);
        } 
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Add module to a group 
 */
int bcm_dfe_modid_group_map_add(int unit, bcm_module_t group, bcm_module_t module)
{  
    _bcm_dfe_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_GROUP_MAP, group, &group_map);
    BCMDNX_IF_ERR_EXIT(rv);
    SHR_BITSET(group_map.raw, module);
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_GROUP_MAP, group, &group_map);
    BCMDNX_IF_ERR_EXIT(rv);

    modid_to_group_id = group;
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, MODID_MODID_TO_GROUP_MAP, module, &modid_to_group_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get list of modules for a group
 */
int bcm_dfe_modid_group_map_get_list(int unit, bcm_module_t group, int modid_max_count, bcm_module_t *moidid_array, int *modid_count)
{ 
    int i;
    _bcm_dfe_modid_group_map_t group_map;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(moidid_array);
    BCMDNX_NULL_CHECK(modid_count);
     
    *modid_count = 0;
    
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_GROUP_MAP, group, &group_map);
    BCMDNX_IF_ERR_EXIT(rv);

    for(i = 0 ; i < SOC_DFE_MODID_NOF ; i++)
    {
        if(SHR_BITGET(group_map.raw, i))
        {
            if(*modid_count >= modid_max_count) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Array is too small")));  
            }
              
            moidid_array[*modid_count] = i;
            (*modid_count)++;
        }
    }
    
exit:
    BCMDNX_FUNC_RETURN; 
}

/*
 * Get group id of a module
 */
int bcm_dfe_modid_group_map_get_group(int unit, bcm_module_t module, bcm_module_t* group)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(group);

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, MODID_MODID_TO_GROUP_MAP, module, group);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN; 
}

#undef _ERR_MSG_MODULE_NAME

