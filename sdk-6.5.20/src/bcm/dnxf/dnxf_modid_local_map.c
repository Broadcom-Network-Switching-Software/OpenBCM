/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF MODID LOCAL MAP
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC
 #include <shared/bsl.h>
#include <bcm_int/dnxf/dnxf_modid_local_map.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

#include <shared/bitop.h>

#include <soc/drv.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>

/*
 * Clear modid local map
 */
int bcm_dnxf_modid_local_map_clear(int unit)
{
    int i, rc;
    SHR_FUNC_INIT_VARS(unit);
    
    for(i=0 ; i < DNXF_MODID_LOCAL_MAP_ROWS(unit) ; i++)
    {
        rc = bcm_dnxf_modid_local_map_module_clear(unit, i);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Remove local module from the map
 */
int bcm_dnxf_modid_local_map_module_clear(int unit, bcm_module_t local_module_id)
{
    int rv;
    _bcm_dnxf_modid_local_map_t local_modid_map;
    SHR_FUNC_INIT_VARS(unit);
    
    local_modid_map.valid = 0;
    local_modid_map.module_id = 0;
    rv = dnxf_state.modid.local_map.set(unit, local_module_id, &local_modid_map);
    SHR_IF_ERR_EXIT(rv);
    
exit:
    SHR_FUNC_EXIT; 
}

/*
 * Check whether a local module is mapped
 */
int bcm_dnxf_modid_local_map_is_valid(int unit, bcm_module_t local_module_id, int* is_valid)
{  
    int rv;
    _bcm_dnxf_modid_local_map_t local_modid_map;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_valid, _SHR_E_PARAM, "is_valid");

    if(!DNXF_MODID_LOCAL_MAP_ROW_VALIDATE(unit, local_module_id)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "local_module_id is out of range");  
    }

    rv = dnxf_state.modid.local_map.get(unit, local_module_id, &local_modid_map);
    SHR_IF_ERR_EXIT(rv);

    *is_valid = local_modid_map.valid;

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Attach module id with local module id
 */
int bcm_dnxf_modid_local_map_add(int unit, bcm_module_t local_module_id, bcm_module_t module_id, int allow_override)
{ 
    int rv;
    _bcm_dnxf_modid_local_map_t local_modid_map;
    SHR_FUNC_INIT_VARS(unit);
    
    rv = dnxf_state.modid.local_map.get(unit, local_module_id, &local_modid_map);
    SHR_IF_ERR_EXIT(rv);


    if(!DNXF_MODID_LOCAL_MAP_ROW_VALIDATE(unit, local_module_id)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "local_module_id is out of range");
    }
        
    if(0 == allow_override && local_modid_map.valid)  {
        SHR_ERR_EXIT(_SHR_E_PARAM, "local_module_id alerady exist");
    }
      
    local_modid_map.valid = 1;
    local_modid_map.module_id = module_id;

    rv = dnxf_state.modid.local_map.set(unit, local_module_id, &local_modid_map);
    SHR_IF_ERR_EXIT(rv);
    
exit:
    SHR_FUNC_EXIT; 
}

/*
 * Get module id for local module id
 */
int bcm_dnxf_modid_local_map_get(int unit, bcm_module_t local_module_id, bcm_module_t* module_id)
{  
    int rv;
    _bcm_dnxf_modid_local_map_t local_modid_map;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(module_id, _SHR_E_PARAM, "module_id");

    if(!DNXF_MODID_LOCAL_MAP_ROW_VALIDATE(unit, local_module_id)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "local_module_id is out of range");
    }
    
    rv = dnxf_state.modid.local_map.get(unit, local_module_id, &local_modid_map);
    SHR_IF_ERR_EXIT(rv);

    if(!local_modid_map.valid) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "local_module_id doesn't exist");
    }
      
    *module_id = local_modid_map.module_id;
    
exit:
    SHR_FUNC_EXIT; 
}

/*
 * Get local module id for a module id
 */
int bcm_dnxf_modid_local_map_modid_to_local(int unit, bcm_module_t module_id, bcm_module_t* local_module_id)
{  
    int i,
        rv;
    _bcm_dnxf_modid_local_map_t local_modid_map;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(local_module_id, _SHR_E_PARAM, "local_module_id");
    
    for(i = 0; i<DNXF_MODID_LOCAL_MAP_ROWS(unit) ; i++)
    {

        rv = dnxf_state.modid.local_map.get(unit, i, &local_modid_map);
        SHR_IF_ERR_EXIT(rv);

        if(local_modid_map.valid == 0)
          continue;

        if(local_modid_map.module_id == module_id)
        {
          *local_module_id = i;
          SHR_EXIT();
        }
    }
    
    /*module not found*/
    SHR_ERR_EXIT(_SHR_E_PARAM, "module_id wasn't found");

exit:
    SHR_FUNC_EXIT; 
}

#undef BSL_LOG_MODULE

