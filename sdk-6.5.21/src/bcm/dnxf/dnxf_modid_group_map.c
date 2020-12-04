/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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

#include <shared/bitop.h>

#include <sal/core/libc.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

/*
 * Clear modid group id map
 */
int bcm_dnxf_modid_group_map_clear(int unit)
{
    int i;
    _bcm_dnxf_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    
    sal_memset(group_map.raw,0, SOC_DNXF_MODID_NOF_UINT32_SIZE * sizeof(uint32));
    for(i=0 ; i< DNXF_MODID_GROUP_MAP_ROWS(unit) ; i++)
    {
        rv = dnxf_state.modid.group_map.set(unit, i, &group_map);
        SHR_IF_ERR_EXIT(rv);
    }

    modid_to_group_id = DNXF_MODID_GROUP_INVALID;
    for(i=0 ; i< SOC_DNXF_MODID_NOF ; i++)
    {
        rv = dnxf_state.modid.modid_to_group_map.set(unit, i, modid_to_group_id);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Clear modid single group 
 */
int bcm_dnxf_modid_group_map_clear_group(int unit, bcm_module_t group)
{
    int i,
        rv;
    _bcm_dnxf_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(group_map.raw,0,SOC_DNXF_MODID_NOF_UINT32_SIZE * sizeof(uint32));
    rv = dnxf_state.modid.group_map.set(unit, group, &group_map);
    SHR_IF_ERR_EXIT(rv);

    for(i=0 ; i < SOC_DNXF_MODID_NOF ; i++)
    {
        rv = dnxf_state.modid.modid_to_group_map.get(unit, i, &modid_to_group_id);
        SHR_IF_ERR_EXIT(rv);
        if(group == modid_to_group_id) {
             modid_to_group_id = DNXF_MODID_GROUP_INVALID;
             rv = dnxf_state.modid.modid_to_group_map.set(unit, i, modid_to_group_id);
             SHR_IF_ERR_EXIT(rv);
        } 
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Add module to a group 
 */
int bcm_dnxf_modid_group_map_add(int unit, bcm_module_t group, bcm_module_t module)
{  
    _bcm_dnxf_modid_group_map_t group_map;
    bcm_module_t modid_to_group_id;
    int rv;
    SHR_FUNC_INIT_VARS(unit);


    rv = dnxf_state.modid.group_map.get(unit, group, &group_map);
    SHR_IF_ERR_EXIT(rv);
    SHR_BITSET(group_map.raw, module);

    rv = dnxf_state.modid.group_map.set(unit, group, &group_map);
    SHR_IF_ERR_EXIT(rv);

    modid_to_group_id = group;
    rv = dnxf_state.modid.modid_to_group_map.set(unit, module, modid_to_group_id);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Get list of modules for a group
 */
int bcm_dnxf_modid_group_map_get_list(int unit, bcm_module_t group, int modid_max_count, bcm_module_t *moidid_array, int *modid_count)
{ 
    int i;
    _bcm_dnxf_modid_group_map_t group_map;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(moidid_array, _SHR_E_PARAM, "moidid_array");
    SHR_NULL_CHECK(modid_count, _SHR_E_PARAM, "modid_count");
     
    *modid_count = 0;
    
    rv = dnxf_state.modid.group_map.get(unit, group, &group_map);
    SHR_IF_ERR_EXIT(rv);

    for(i = 0 ; i < SOC_DNXF_MODID_NOF ; i++)
    {
        if(SHR_BITGET(group_map.raw, i))
        {
            if(*modid_count >= modid_max_count) {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Array is too small");  
            }
              
            moidid_array[*modid_count] = i;
            (*modid_count)++;
        }
    }
    
exit:
    SHR_FUNC_EXIT; 
}

/*
 * Get group id of a module
 */
int bcm_dnxf_modid_group_map_get_group(int unit, bcm_module_t module, bcm_module_t* group)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group, _SHR_E_PARAM, "group");

    rv = dnxf_state.modid.modid_to_group_map.get(unit, module, group);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT; 
}

#undef BSL_LOG_MODULE

