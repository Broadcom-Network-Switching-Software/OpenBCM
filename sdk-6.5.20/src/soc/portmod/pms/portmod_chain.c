
/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/wb_engine.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/portmod/portmod_chain.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define PM_EXTPHY_ITER(rv, phy_idx)    while((PHYMOD_E_UNAVAIL== rv) && (phy_idx >= 0))
#define PM_ALLPHY_ITER(rv, phy_idx)    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_idx >= 0))

typedef enum xphy_wb_vars {
    xphy_wb_is_initialized,                 /* keep track of xphy initialization state.     */
    xphy_wb_phy_type,                       /* xphy probed phy type information.            */
    xphy_wb_legacy_phy_present,             /* indicates if port has legacy external phy    */
    xphy_wb_ref_clk,                        /* xphy ref clk                                 */
    xphy_wb_fw_load_method,                 /* xphy firmware load method.                   */
    xphy_wb_force_fw_load,                  /* xphy force firmware load indication.         */
    xphy_wb_polarity,                       /* xphy polarity information.                   */
    xphy_wb_lane_map,                       /* xphy lane map information.                   */
    xphy_wb_device_op_mode,                 /* xphy device_op_mode of core_access.          */
    xphy_wb_primary_core_num,               /* xphy primary core information.               */
    xphy_wb_core_probed,                    /* indicate xphy core has been probed or not    */
    xphy_wb_active_lanes,                   /* indicate xphy lanes that are connected/used. */
    xphy_wb_num_var
}xphy_wb_vars_t;

typedef struct portmod_xphy_info_s {
    phymod_core_access_t core_access; /**< core access */
    int wb_var_ids[xphy_wb_num_var];
} portmod_xphy_info_t;

#define XPHY_WB_BUFFER_VERSION      (4)
#define XPHY_IS_INITIALIZED_SET(unit, xphy_info, is_initialized)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_is_initialized], &is_initialized);
#define XPHY_IS_INITIALIZED_GET(unit, xphy_info, is_initialized)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_is_initialized], is_initialized);

#define XPHY_PHY_TYPE_SET(unit, xphy_info, phy_type)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_phy_type], &phy_type);
#define XPHY_PHY_TYPE_GET(unit, xphy_info, phy_type)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_phy_type], phy_type);

#define XPHY_LEGACY_PHY_PRESENT_SET(unit, xphy_info, leagacy_phy_present)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_legacy_phy_present], &leagacy_phy_present);
#define XPHY_LEGACY_PHY_PRESENT_GET(unit, xphy_info, leagacy_phy_present)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_legacy_phy_present], leagacy_phy_present);

#define XPHY_REF_CLK_SET(unit, xphy_info, ref_clk)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_ref_clk], &ref_clk);
#define XPHY_REF_CLK_GET(unit, xphy_info, ref_clk)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_ref_clk], ref_clk );

#define XPHY_FW_LOAD_METHOD_SET(unit, xphy_info, fw_load_method)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_fw_load_method], &fw_load_method);
#define XPHY_FW_LOAD_METHOD_GET(unit, xphy_info, fw_load_method)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_fw_load_method], fw_load_method);

#define XPHY_FORCE_FW_LOAD_SET(unit, xphy_info, force_fw_load)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_force_fw_load], &force_fw_load);
#define XPHY_FORCE_FW_LOAD_GET(unit, xphy_info, force_fw_load)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_force_fw_load], force_fw_load);

#define XPHY_POLARITY_SET(unit, xphy_info, polarity)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_polarity], &polarity);
#define XPHY_POLARITY_GET(unit, xphy_info, polarity)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_polarity], polarity);

#define XPHY_LANE_MAP_SET(unit, xphy_info, lane_map)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_lane_map], &lane_map);
#define XPHY_LANE_MAP_GET(unit, xphy_info, lane_map)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_lane_map], lane_map );

#define XPHY_DEVICE_OP_MODE_SET(unit, xphy_info, device_op_mode)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_device_op_mode], &device_op_mode);
#define XPHY_DEVICE_OP_MODE_GET(unit, xphy_info, device_op_mode)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_device_op_mode], device_op_mode);

#define XPHY_PRIMARY_CORE_NUM_SET(unit, xphy_info, primary_core_num )  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_primary_core_num], &primary_core_num);
#define XPHY_PRIMARY_CORE_NUM_GET(unit, xphy_info, primary_core_num)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_primary_core_num], primary_core_num);

#define XPHY_CORE_PROBED_SET(unit, xphy_info, core_probed)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_core_probed], &core_probed);
#define XPHY_CORE_PROBED_GET(unit, xphy_info, core_probed)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_core_probed], core_probed);

#define XPHY_ACTIVE_LANES_SET(unit, xphy_info, active_lanes)  \
    SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_active_lanes], &active_lanes);
#define XPHY_ACTIVE_LANES_GET(unit, xphy_info, active_lanes)  \
    SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, xphy_info->wb_var_ids[xphy_wb_active_lanes], active_lanes);

portmod_ext_phy_core_info_t *_ext_phy_info[SOC_MAX_NUM_DEVICES][MAX_PHYN][SOC_MAX_NUM_PORTS] = {{{NULL}}};

STATIC
portmod_xphy_info_t* _xphy_info[SOC_MAX_NUM_DEVICES][PORTMOD_MAX_NUM_XPHY_SUPPORTED];

STATIC void* _xphy_user_access[SOC_MAX_NUM_DEVICES * PORTMOD_MAX_NUM_XPHY_SUPPORTED] = {NULL};

STATIC
phymod_core_init_config_t _xphy_core_config[SOC_MAX_NUM_DEVICES][PORTMOD_MAX_NUM_XPHY_SUPPORTED];

STATIC
int _xphy_addr[SOC_MAX_NUM_DEVICES][PORTMOD_MAX_NUM_XPHY_SUPPORTED];

int portmod_xphy_addr_set(int unit, int idx, int xphy_addr)
{
    _xphy_addr[unit][idx] = xphy_addr;
    return SOC_E_NONE;
}

int portmod_phychain_ext_phy_info_set(int unit, int phyn, int core_index, const portmod_ext_phy_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn is out of the range of allowed external phys")));
    }
    if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
        _ext_phy_info[unit][phyn - 1][core_index] = sal_alloc(sizeof(portmod_ext_phy_core_info_t), "core_info");
        if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
            _SOC_EXIT_WITH_ERR(SOC_E_MEMORY, (_SOC_MSG("phyn core info mem allocation failed")));
        }
    }
    sal_memcpy(_ext_phy_info[unit][phyn - 1][core_index], core_info, sizeof(portmod_ext_phy_core_info_t));

exit:
    SOC_FUNC_RETURN; 
}

int portmod_phychain_ext_phy_info_get(int unit, int phyn, int core_index, portmod_ext_phy_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn is out of the range of allowed external phys")));
    }
    if(NULL == _ext_phy_info[unit][phyn - 1][core_index]){
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("for phyn core,the information is not available")));
    }
    sal_memcpy(core_info, _ext_phy_info[unit][phyn - 1][core_index], sizeof(portmod_ext_phy_core_info_t));

exit:
    SOC_FUNC_RETURN; 
}

int portmod_ext_phy_core_info_t_init(int unit, portmod_ext_phy_core_info_t* portmod_ext_phy_core_info)
{
    SOC_INIT_FUNC_DEFS;

    if(portmod_ext_phy_core_info == NULL){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("portmod_ext_phy_core_info NULL parameter"));
    }
    sal_memset(portmod_ext_phy_core_info, 0, sizeof(portmod_ext_phy_core_info_t));
    phymod_core_access_t_init(&portmod_ext_phy_core_info->core_access);
    portmod_ext_phy_core_info->is_initialized = 0;

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_wb_buffer_init(int unit, uint32 wb_buffer_index, portmod_xphy_info_t* xphy_core_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv = 0;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "xphy", NULL, NULL, XPHY_WB_BUFFER_VERSION, 1, SOC_WB_ENGINE_PRE_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_is_initialized ]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "phy_type", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_phy_type]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_legacy_ext_phy", wb_buffer_index, sizeof(uint32), NULL, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_legacy_phy_present]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_ref_clk", wb_buffer_index, sizeof(phymod_ref_clk_t), NULL, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_ref_clk]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_fw_load_method", wb_buffer_index, sizeof(phymod_firmware_load_method_t ), NULL, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_fw_load_method]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_force_fw_load", wb_buffer_index, sizeof(int), NULL, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_force_fw_load]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_polarity", wb_buffer_index, sizeof(phymod_polarity_t ), NULL, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_polarity]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_lane_map", wb_buffer_index, sizeof(phymod_lane_map_t), NULL, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_lane_map]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_device_op_mode", wb_buffer_index, sizeof(uint32), NULL, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_device_op_mode]= wb_var_id;


    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_primary_core_num", wb_buffer_index, sizeof(uint32), NULL, VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    xphy_core_info->wb_var_ids[xphy_wb_primary_core_num]= wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_core_probed", wb_buffer_index, sizeof(int), NULL, VERSION(3));
    xphy_core_info->wb_var_ids[xphy_wb_core_probed]= wb_var_id;
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "xphy_wb_active_lanes", wb_buffer_index, sizeof(uint32), NULL, VERSION(4));
    xphy_core_info->wb_var_ids[xphy_wb_active_lanes]= wb_var_id;
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _xphy_find_index(int unit, int xphy_addr, int* xphy_id)
{
    int idx;

    *xphy_id = PORTMOD_MAX_NUM_XPHY_SUPPORTED;
    for (idx=0; idx<PORTMOD_MAX_NUM_XPHY_SUPPORTED; idx++) {
        if (_xphy_info[unit][idx] != NULL) {
            if (_xphy_info[unit][idx]->core_access.access.addr == xphy_addr) {
                *xphy_id = idx;
                return SOC_E_NONE;
            }
        } else {
            if (*xphy_id == PORTMOD_MAX_NUM_XPHY_SUPPORTED)
                *xphy_id = idx;
        }
    }

    return SOC_E_PARAM;
}

int portmod_chain_xphy_add_by_index(int unit, int xphy_idx, int xphy_addr, const phymod_core_access_t* core_access)
{
    int rv;
    int max_pms=0;
    uint32 wb_buffer_index;
    phymod_dispatch_type_t phy_type;
    phymod_ref_clk_t ref_clk; 
    phymod_firmware_load_method_t fw_load_method;
    int is_initialized;
    int core_probed; 
    uint32 active_lanes;
 
    SOC_INIT_FUNC_DEFS;

    /* if new one need to be added, create one.  */
    if( NULL == _xphy_info[unit][xphy_idx]){

        _xphy_info[unit][xphy_idx] = sal_alloc(sizeof(portmod_xphy_info_t), "xphy_info");

        if(NULL == _xphy_info[unit][xphy_idx]){
            _SOC_EXIT_WITH_ERR(SOC_E_MEMORY, (_SOC_MSG("xphy core info mem allocation failed")));
        }
        /* start with clean memory. */
        sal_memset(_xphy_info[unit][xphy_idx], 0, sizeof(portmod_xphy_info_t));

    } else {
        /* existing one, does not need any action. */
        SOC_EXIT;
    }

    sal_memcpy(&_xphy_info[unit][xphy_idx]->core_access, core_access, sizeof(phymod_core_access_t));
    _xphy_info[unit][xphy_idx]->core_access.access.addr = xphy_addr;

    /* initialize the warmboot variables */
    portmod_max_pms_get(unit, &max_pms);
    wb_buffer_index = xphy_idx + max_pms + 1;
    _SOC_IF_ERR_EXIT(portmod_xphy_wb_buffer_init(unit, wb_buffer_index, _xphy_info[unit][xphy_idx]));

    /* if warmboot, restore the probed phy type into the phymod core_access. */
    if(SOC_WARM_BOOT(unit)){

        rv = XPHY_PHY_TYPE_GET(unit,_xphy_info[unit][xphy_idx],
                            &_xphy_info[unit][xphy_idx]->core_access.type);
        _SOC_IF_ERR_EXIT(rv);

        rv = XPHY_DEVICE_OP_MODE_GET(unit,_xphy_info[unit][xphy_idx],
                            &_xphy_info[unit][xphy_idx]->core_access.device_op_mode);
        _SOC_IF_ERR_EXIT(rv);

    }

    if(!SOC_WARM_BOOT(unit)){
        portmod_xphy_db_addr_set(unit, xphy_idx, xphy_addr);

        phy_type = phymodDispatchTypeInvalid;
        rv = XPHY_PHY_TYPE_SET(unit,_xphy_info[unit][xphy_idx], phy_type);
        _SOC_IF_ERR_EXIT(rv);

        is_initialized =0;
        rv = XPHY_IS_INITIALIZED_SET(unit,_xphy_info[unit][xphy_idx],is_initialized);
        _SOC_IF_ERR_EXIT(rv);

        ref_clk = phymodRefClkCount;
        rv = XPHY_REF_CLK_SET(unit,_xphy_info[unit][xphy_idx],ref_clk);
        _SOC_IF_ERR_EXIT(rv);

        fw_load_method = phymodFirmwareLoadMethodCount;
        rv = XPHY_FW_LOAD_METHOD_SET(unit,_xphy_info[unit][xphy_idx],fw_load_method);
        _SOC_IF_ERR_EXIT(rv);

        core_probed =  0;
        rv = XPHY_CORE_PROBED_SET(unit,_xphy_info[unit][xphy_idx],core_probed);
        _SOC_IF_ERR_EXIT(rv);

        /*
         * clear all the active lanes.
         */
        active_lanes = 0;
        rv = XPHY_ACTIVE_LANES_SET(unit,_xphy_info[unit][xphy_idx],active_lanes);
        _SOC_IF_ERR_EXIT(rv);
        
    }
exit:
    SOC_FUNC_RETURN;
}

int portmod_chain_xphy_add(int unit, int xphy_addr, const phymod_core_access_t* core_access, int *xphy_idx_out)
{
    int xphy_idx ;
    SOC_INIT_FUNC_DEFS;

    /* looking for the matching valid one, if found, skip and return. */
    *xphy_idx_out = PORTMOD_MAX_NUM_XPHY_SUPPORTED;
    if (_xphy_find_index(unit,xphy_addr, xphy_idx_out) == SOC_E_NONE){
        /*
         * if the xphy is already in database, the user_acc need to be 
         * release from  caller.
         */

        *xphy_idx_out = PORTMOD_XPHY_EXISTING_IDX;
        SOC_EXIT;
    }
    xphy_idx = *xphy_idx_out; 

    if ( PORTMOD_MAX_NUM_XPHY_SUPPORTED <= xphy_idx){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("xphy_idx is out of the range of allowed external phys - 0- %d"), (PORTMOD_MAX_NUM_XPHY_SUPPORTED-1)));
    }

    _SOC_IF_ERR_EXIT(portmod_chain_xphy_add_by_index(unit, xphy_idx, xphy_addr, core_access));
exit:
    SOC_FUNC_RETURN;
}

int portmod_chain_xphy_user_access_store(int unit, int xphy_idx, void* user_acc)
{
    _xphy_user_access[(unit * PORTMOD_MAX_NUM_XPHY_SUPPORTED)+xphy_idx] = user_acc;
    return SOC_E_NONE;
}


void portmod_chain_xphy_user_access_release(int unit)
{
    int idx;
    portmod_default_user_access_t *local_user_access;

    for (idx=0; idx<PORTMOD_MAX_NUM_XPHY_SUPPORTED; idx++) {
        local_user_access = (portmod_default_user_access_t*)_xphy_user_access[(unit * PORTMOD_MAX_NUM_XPHY_SUPPORTED)+idx];
        if(local_user_access != NULL) {
            if(local_user_access->mutex != NULL) {
                sal_mutex_destroy(local_user_access->mutex);
            }
            sal_free(local_user_access);
            _xphy_user_access[(unit * PORTMOD_MAX_NUM_XPHY_SUPPORTED)+idx] = NULL;
        }
    }
}

int _xphy_resource_release(int unit, int xphy_idx)
{

    portmod_xphy_info_t* xphy_info;
    SOC_INIT_FUNC_DEFS;

    xphy_info = _xphy_info[unit][xphy_idx];

    if( xphy_info != NULL ) {
        sal_free(_xphy_info[unit][xphy_idx]);
        _xphy_info[unit][xphy_idx] = NULL ;
        /* How to remove wb data for the xphy? */
    }
    SOC_FUNC_RETURN;
}

int portmod_chain_xphy_delete(int unit, int xphy_addr)
{
    int xphy_idx;

    SOC_INIT_FUNC_DEFS;
    
    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    _SOC_IF_ERR_EXIT(portmod_xphy_valid_phy_set (unit, xphy_idx, FALSE));

    /* release all resource of xphy_info */
    _SOC_IF_ERR_EXIT(_xphy_resource_release(unit, xphy_idx));

exit:
    SOC_FUNC_RETURN;
}

int portmod_chain_xphy_delete_all(int unit)
{
    int xphy_idx;

    SOC_INIT_FUNC_DEFS;

    for(xphy_idx = 0; xphy_idx < PORTMOD_MAX_NUM_XPHY_SUPPORTED; xphy_idx++){
        /* release all resource of xphy_info */
        _SOC_IF_ERR_EXIT(_xphy_resource_release(unit, xphy_idx));
    }
exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_core_access_set(int unit, uint32 xphy_addr, phymod_core_access_t *core_access, int is_legacy_phy)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));

    sal_memcpy( &_xphy_info[unit][xphy_idx]->core_access, core_access,  sizeof(phymod_core_access_t));

    rv = XPHY_PHY_TYPE_SET(unit,_xphy_info[unit][xphy_idx], core_access->type);
    _SOC_IF_ERR_EXIT(rv);

    rv = XPHY_LEGACY_PHY_PRESENT_SET(unit,_xphy_info[unit][xphy_idx], is_legacy_phy);
    _SOC_IF_ERR_EXIT(rv);

    rv = XPHY_DEVICE_OP_MODE_SET(unit,_xphy_info[unit][xphy_idx], core_access->device_op_mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_core_access_get(int unit, uint32 xphy_addr, phymod_core_access_t *core_access, int* is_legacy_phy)
{
    int rv;
    int xphy_idx;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    sal_memcpy( core_access, &_xphy_info[unit][xphy_idx]->core_access, sizeof(phymod_core_access_t));

    rv = XPHY_PHY_TYPE_GET(unit,_xphy_info[unit][xphy_idx], &core_access->type);
    _SOC_IF_ERR_EXIT(rv);

    rv = XPHY_LEGACY_PHY_PRESENT_GET(unit,_xphy_info[unit][xphy_idx], is_legacy_phy);
    _SOC_IF_ERR_EXIT(rv);

    rv = XPHY_DEVICE_OP_MODE_GET(unit,_xphy_info[unit][xphy_idx], &core_access->device_op_mode);
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_core_config_set(int unit, uint32 xphy_addr, phymod_core_init_config_t* core_config)
{
    int xphy_idx ;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    sal_memcpy( &_xphy_core_config[unit][xphy_idx], core_config,  sizeof(phymod_core_init_config_t));

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_core_config_get(int unit, uint32 xphy_addr,phymod_core_init_config_t* core_config)
{
    int xphy_idx ;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    sal_memcpy( core_config, &_xphy_core_config[unit][xphy_idx], sizeof(phymod_core_init_config_t));

exit:
    SOC_FUNC_RETURN;
}


int portmod_xphy_is_initialized_set(int unit, uint32 xphy_addr, int is_initialized)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_IS_INITIALIZED_SET(unit,_xphy_info[unit][xphy_idx], is_initialized);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_is_initialized_get(int unit, uint32 xphy_addr, int* is_initialized)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_IS_INITIALIZED_GET(unit,_xphy_info[unit][xphy_idx], is_initialized);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_phy_type_set(int unit, uint32 xphy_addr, phymod_dispatch_type_t phy_type)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_PHY_TYPE_SET(unit,_xphy_info[unit][xphy_idx], phy_type);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_phy_type_get(int unit, uint32 xphy_addr, phymod_dispatch_type_t* phy_type)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_PHY_TYPE_GET(unit,_xphy_info[unit][xphy_idx], phy_type);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_is_legacy_phy_set(int unit, uint32 xphy_addr, int is_legacy_phy)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_LEGACY_PHY_PRESENT_SET(unit,_xphy_info[unit][xphy_idx], is_legacy_phy);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_is_legacy_phy_get(int unit, uint32 xphy_addr, int* is_legacy_phy)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_LEGACY_PHY_PRESENT_GET(unit,_xphy_info[unit][xphy_idx], is_legacy_phy);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_ref_clk_set(int unit, uint32 xphy_addr, phymod_ref_clk_t ref_clk)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_REF_CLK_SET(unit,_xphy_info[unit][xphy_idx],ref_clk);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_ref_clk_get(int unit, uint32 xphy_addr, phymod_ref_clk_t* ref_clk)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_REF_CLK_GET(unit,_xphy_info[unit][xphy_idx],ref_clk);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_fw_load_method_set(int unit, uint32 xphy_addr, phymod_firmware_load_method_t  fw_load_method)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_FW_LOAD_METHOD_SET(unit,_xphy_info[unit][xphy_idx], fw_load_method);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_fw_load_method_get(int unit, uint32 xphy_addr, phymod_firmware_load_method_t* fw_load_method)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_FW_LOAD_METHOD_GET(unit,_xphy_info[unit][xphy_idx], fw_load_method);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_force_fw_load_set(int unit, uint32 xphy_addr, int force_fw_load)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_FORCE_FW_LOAD_SET(unit,_xphy_info[unit][xphy_idx], force_fw_load);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_force_fw_load_get(int unit, uint32 xphy_addr, int* force_fw_load)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_FORCE_FW_LOAD_GET(unit,_xphy_info[unit][xphy_idx], force_fw_load);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_polarity_set(int unit, uint32 xphy_addr, phymod_polarity_t polarity)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_POLARITY_SET(unit,_xphy_info[unit][xphy_idx], polarity);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_polarity_get(int unit, uint32 xphy_addr, phymod_polarity_t* polarity)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_POLARITY_GET(unit,_xphy_info[unit][xphy_idx], polarity);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_lane_map_set(int unit, uint32 xphy_addr, phymod_lane_map_t lane_map)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_LANE_MAP_SET(unit,_xphy_info[unit][xphy_idx], lane_map);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_lane_map_get(int unit, uint32 xphy_addr, phymod_lane_map_t* lane_map)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_LANE_MAP_GET(unit,_xphy_info[unit][xphy_idx], lane_map);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_device_op_mode_set(int unit, uint32 xphy_addr, uint32 device_op_mode)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_DEVICE_OP_MODE_SET(unit,_xphy_info[unit][xphy_idx], device_op_mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_device_op_mode_get(int unit, uint32 xphy_addr, uint32* device_op_mode)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_DEVICE_OP_MODE_GET(unit,_xphy_info[unit][xphy_idx], device_op_mode);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_primary_core_num_set(int unit, uint32 xphy_addr, uint32 primary_core_num)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_PRIMARY_CORE_NUM_SET(unit,_xphy_info[unit][xphy_idx], primary_core_num);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_primary_core_num_get(int unit, uint32 xphy_addr, uint32* primary_core_num)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_PRIMARY_CORE_NUM_GET(unit,_xphy_info[unit][xphy_idx], primary_core_num);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_core_probed_set(int unit, uint32 xphy_addr, int core_probed)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_CORE_PROBED_SET(unit,_xphy_info[unit][xphy_idx], core_probed);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_core_probed_get(int unit, uint32 xphy_addr, int* core_probed)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_CORE_PROBED_GET(unit,_xphy_info[unit][xphy_idx], core_probed);
    _SOC_IF_ERR_EXIT(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_active_lane_set (int unit, uint32 xphy_addr, uint32 active_lanes)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_ACTIVE_LANES_SET(unit, _xphy_info[unit][xphy_idx], active_lanes)
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

int portmod_xphy_active_lane_get (int unit, uint32 xphy_addr, uint32 *active_lanes)
{
    int xphy_idx, rv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(_xphy_find_index(unit,xphy_addr,&xphy_idx));
    rv = XPHY_ACTIVE_LANES_GET(unit, _xphy_info[unit][xphy_idx], active_lanes)
    _SOC_IF_ERR_EXIT(rv);
exit:
    SOC_FUNC_RETURN;
}

/*
 * This function is used to get the working operation mode from ext-phy.
 * It is mainly used to differ 1G pass thru mode and other modes like
 *pcs_repeater/retimer mode.
 */
int portmod_port_phychain_operation_mode_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_operation_mode_t* phy_op_mode)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_operation_mode_t op_mode = phymodOperationModeRetimer;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* if it is external phy, check to see if it is legacy phy. */
    if (phy_index > 0) {
        rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
        if (rv != PHYMOD_E_NONE) return rv;
        if (is_legacy_phy) {
            rv = portmod_port_legacy_op_mode_get(unit, port, &op_mode);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = phymod_phy_op_mode_get(&phy_access[phy_index], &op_mode);
            } else {
                rv = PHYMOD_E_UNAVAIL;
            }
        }
    } else {
        op_mode = phymodOperationModeCount;
    }

    *phy_op_mode = op_mode;

    return (rv);
}

int portmod_port_phychain_core_access_set(int unit, int phyn, int core_index, const phymod_core_access_t* core_access)
{
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("phyn is out of the range of allowed external phys")));
    }
    if(NULL != _ext_phy_info[unit][phyn - 1][core_index]){
        sal_memcpy(&(_ext_phy_info[unit][phyn - 1][core_index]->core_access), core_access, sizeof(phymod_core_access_t));
    }
    
exit:
    SOC_FUNC_RETURN; 

}

int portmod_port_phychain_core_access_get(int unit, int port, const int chain_length, int max_cores, phymod_core_access_t* core_access)
{
    int phyn = 0;
    int nof_cores = 0;
    int is_most_ext = 0;
    SOC_INIT_FUNC_DEFS;

    if (((1 + MAX_PHYN) <= chain_length) || (0 >= chain_length)){
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("chain_length is out of the range of allowed external phys")));
    }
    for (phyn = 0; phyn < chain_length; phyn++) {
        _SOC_IF_ERR_EXIT(portmod_port_core_access_get(unit, port, phyn, max_cores, &core_access[phyn], &nof_cores, &is_most_ext));
    }
    
exit:
    SOC_FUNC_RETURN; 
}

int portmod_port_phychain_core_probe(int unit, int port, phymod_core_access_t* core,
                                      const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int phy_addr = 0x0;
    int is_phymod_probed=0;
    int is_legacy_phy = 0;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        /* currently portmod supports only one external phy (1(int) + 1(ext))
         * external phy is the outermost one. check for legacy ext phy if present */
        if (phy_index) {
            rv = phymod_core_probe(&(core[phy_index].access), &(core[phy_index].type), &is_phymod_probed);

            if ( (rv == PHYMOD_E_NONE) && (!is_phymod_probed) )
            {
                if(portmod_port_legacy_phy_probe(unit, port))
                {
                    is_legacy_phy = 1;
                    rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
                    if (rv != PHYMOD_E_NONE) return rv;
                    /* Update the phy address in core access struct -- used by phy info */
                    phy_addr = portmod_port_legacy_phy_addr_get(unit, port);
                    core[phy_index].access.addr = phy_addr;
                    rv = PHYMOD_E_NONE;
                }
            }
        } else {
            rv = phymod_core_probe( &(core[phy_index].access), &(core[phy_index].type), &is_phymod_probed);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_core_identify (int unit, phymod_core_access_t* core,
                                          const int chain_length,
                                          uint32_t core_id,
                                          uint32_t* identified) 
{
int rv = PHYMOD_E_UNAVAIL;
    return(rv);
}

int portmod_port_phychain_core_lane_map_get(int unit ,const phymod_core_access_t* core,
                                            const int chain_length,
                                            phymod_lane_map_t* lane_map) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while( (PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_core_lane_map_get(&core[phy_index], lane_map); 
        }        
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_core_reset_set(int unit, const phymod_core_access_t* core,
                                         const int chain_length,
                                         phymod_reset_mode_t reset_mode,
                                         phymod_reset_direction_t direction) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_core_reset_set(&core[phy_index], reset_mode, direction);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_core_reset_get(int unit, const phymod_core_access_t* core,
                                         const int chain_length,
                                         phymod_reset_mode_t reset_mode,
                                         phymod_reset_direction_t* direction ) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((rv == PHYMOD_E_UNAVAIL) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_core_reset_get(&core[phy_index], reset_mode, direction);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_core_firmware_info_get(int unit, const phymod_core_access_t* core,
                                                 const int chain_length,
                                                 phymod_core_firmware_info_t* fw_info) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_core_firmware_info_get(&core[phy_index], fw_info);
        }
        phy_index--;
    }
    return(rv);
}


int portmod_port_phychain_loopback_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                       const int chain_length,  
                                       phymod_loopback_mode_t loopback, 
                                       uint32_t *enable) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    PHYMOD_NULL_CHECK(phy_access);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_loopback_get(unit, port, loopback, enable);

        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_loopback_get(&phy_access[phy_index], loopback, enable);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--; 
    }
    return(rv);
}

int portmod_port_phychain_loopback_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                       const int chain_length,  
                                       phymod_loopback_mode_t loopback, 
                                       uint32_t enable) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    PHYMOD_NULL_CHECK(phy_access);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    
    /* Loopback should be enabled always on the outermost phy */
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_loopback_set(unit, port, loopback, enable);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_loopback_set(&phy_access[phy_index], loopback, enable);
            } else {
                rv = PHYMOD_E_NONE;
            }

        }

        if (rv == PHYMOD_E_UNAVAIL) {
            if (enable != 0){
                LOG_CLI((BSL_META_U(unit,
                                 "phy_index=%d Loopback=%d Unavail - try next in chain\n"),
                                  phy_index, loopback));
           }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_firmware_core_config_set(int unit, const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_core_config_t fw_core_config) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    PHYMOD_NULL_CHECK(phy_access);

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_firmware_core_config_set(&phy_access[phy_index], fw_core_config);
        }
        phy_index--;
    }
    return(rv);
}


int phymod_chain_phy_firmware_core_config_get(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_firmware_core_config_t *fw_core_config) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_firmware_core_config_get(&phy_access[phy_index], fw_core_config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_firmware_lane_config_set(int unit, const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_lane_config_t fw_lane_config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_firmware_lane_config_set(&phy_access[phy_index], fw_lane_config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_firmware_lane_config_get(int unit, const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_lane_config_t *fw_lane_config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                  
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_firmware_lane_config_get(&phy_access[phy_index], fw_lane_config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_core_pll_sequencer_restart(int unit, const phymod_core_access_t* core, 
                                                     const int chain_length,
                                                     uint32_t flags, 
                                                     phymod_sequencer_operation_t operation)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_core_pll_sequencer_restart(&core[phy_index], flags, operation);
        }
        phy_index--;
    }
    return(rv);
}

int phymod_chain_core_wait_event(int unit, const phymod_core_access_t* core, 
                                 const int chain_length,
                                 phymod_core_event_t event, 
                                 uint32_t timeout) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_core_wait_event(&core[phy_index], event, timeout);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_restart(int unit, const phymod_phy_access_t* phy_access,
                                     const int chain_length) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
       if (phy_index){  
           rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_rx_restart(&phy_access[phy_index]);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_polarity_set(int unit, const phymod_phy_access_t* phy_access,
                                       const int chain_length, 
                                       const phymod_polarity_t* polarity) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_polarity_set(&phy_access[phy_index], polarity);
        }
        phy_index--;
    }
    return(rv);
}


int portmod_port_phychain_polarity_get(int unit, const phymod_phy_access_t* phy_access,
                                       const int chain_length, 
                                       phymod_polarity_t* polarity) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_polarity_get(&phy_access[phy_index], polarity);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_set(int unit, const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 const phymod_tx_t tx[]) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int i, port_start_lane=-1;
    phymod_phy_access_t phy_acc_copy;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_tx_set(phy_access, chain_length, tx); 
        } else {
            if(phy_access[phy_index].access.lane_mask){
                sal_memcpy(&phy_acc_copy, &phy_access[phy_index], sizeof(phy_acc_copy));
                /* set tx parameters for each lane of the core */
                for(i=0; i<PHYMOD_MAX_LANES_PER_CORE; i++){
                    phy_acc_copy.access.lane_mask = 0x1 << i;
                    if(phy_acc_copy.access.lane_mask & phy_access[phy_index].access.lane_mask){
                        /* port_start_lane is the index of the very first lane of the current
                         * core. port_start_lane is not always 0. However, the tx[] data always
                         * starts from index 0. For instance, a Dino 40G port could have its
                         * first lane at 0b00010000 in phy_access.access.lane_mask. In this
                         * case, tx[0] should be given to this fifth lane instead of tx[4].
                         */
                        if (port_start_lane == -1){
                            port_start_lane = i;
                        }
                        rv = phymod_phy_tx_set(&phy_acc_copy, &tx[i-port_start_lane]);
                        SOC_IF_ERROR_RETURN(rv);
                    }
                }
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--; 
    }
    return(rv);
}

int portmod_port_phychain_tx_get(int unit, const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_tx_t* tx) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int i, port_start_lane=-1;
    phymod_phy_access_t phy_acc_copy;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            if(phy_access[phy_index].access.lane_mask){
                sal_memcpy(&phy_acc_copy, &phy_access[phy_index], sizeof(phy_acc_copy));
                for(i=0; i<PHYMOD_MAX_LANES_PER_CORE; i++){
                    phy_acc_copy.access.lane_mask = 0x1 << i;
                    if(phy_acc_copy.access.lane_mask & phy_access[phy_index].access.lane_mask){
                        /* See comments in portmod_port_phychain_tx_set for why there is a
                         * discrepancy between lane_mask and tx[] in terms of lane index.
                         */
                        if (port_start_lane == -1){
                            port_start_lane = i;
                        }
                        rv = phymod_phy_tx_get(&phy_acc_copy, &tx[i-port_start_lane]);
                        if(rv!=PHYMOD_E_NONE){
                            return rv;
                        }
                    }
                }
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_media_type_tx_get(int unit, const phymod_phy_access_t* phy_access,
                                           const int chain_length,
                                           phymod_media_typed_t media, 
                                           phymod_tx_t* tx) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy){
            rv = phymod_phy_media_type_tx_get(&phy_access[phy_index], media, tx);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_override_set(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          const phymod_tx_override_t* tx_override) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_tx_override_set(&phy_access[phy_index], tx_override);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_override_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_tx_override_t* tx_override) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_tx_override_get(&phy_access[phy_index], tx_override);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_txpi_config_set(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          const phymod_txpi_config_t* config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_txpi_config_set(&phy_access[phy_index], config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_txpi_config_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_txpi_config_t* config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_txpi_config_get(&phy_access[phy_index], config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_set(int unit, const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 const phymod_rx_t* rx) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_rx_set(&phy_access[phy_index], rx);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_get(int unit, const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_rx_t* rx) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            if (phy_index && (phy_access[phy_index].access.lane_mask == 0)) {
                rv = PHYMOD_E_NONE; 
            } else { 
                rv = phymod_phy_rx_get(&phy_access[phy_index], rx);
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_reset_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    const phymod_phy_reset_t* reset) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

             
        if (is_legacy_phy) {
                rv = portmod_port_legacy_phy_reset_set(unit, port);
        } else {
            if((phy_access[phy_index].access.lane_mask == 0) && phy_index) {
                rv = PHYMOD_E_NONE;
            } else {
                rv = phymod_phy_reset_set(&phy_access[phy_index], reset);
            }
        }
        phy_index--; 
    }
    return(rv);
}

int portmod_port_phychain_reset_get(int unit, const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    phymod_phy_reset_t* reset) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index){  
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_reset_get(&phy_access[phy_index], reset);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_power_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length, uint32 flags,
                                    const phymod_phy_power_t* power) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return  phymod_phy_power_set(&phy_access[0], power);
    }
    
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_power_set(unit, port, power); 
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_power_set(&phy_access[phy_index], power);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--; 
    }
    return(rv);
}

int portmod_port_phychain_power_get(int unit, int port,  const phymod_phy_access_t* phy_access,
                                    const int chain_length, uint32 flags,
                                    phymod_phy_power_t* power) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return  phymod_phy_power_get(&phy_access[0], power);
    }
    
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_power_get(unit, port, power); 
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_power_get(&phy_access[phy_index], power);
            } else {
                rv = PHYMOD_E_NONE;
            }

        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_lane_control_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_phy_tx_lane_control_t tx_control) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return  phymod_phy_tx_lane_control_set(&phy_access[0], tx_control);
    }

    /* Most EXT */
    /* Program only the outer most phy */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_tx_ln_ctrl_set(unit, port, &tx_control);           
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_tx_lane_control_set(&phy_access[phy_index], tx_control);
            } else {
                rv = PHYMOD_E_NONE;
            }

        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_tx_lane_control_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_phy_tx_lane_control_t *tx_control) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    
    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return  phymod_phy_tx_lane_control_get(&phy_access[0], tx_control);
    }
    
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
                rv = portmod_port_legacy_tx_ln_ctrl_get(unit, port, tx_control);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_tx_lane_control_get(&phy_access[phy_index], tx_control);
            } else {
                rv = PHYMOD_E_NONE;
            }
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_lane_control_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_phy_rx_lane_control_t rx_control)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return phymod_phy_rx_lane_control_set(&phy_access[0], rx_control);
    }
    
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_rx_ln_ctrl_set(unit, port, &rx_control);               
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_rx_lane_control_set(&phy_access[phy_index], rx_control);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_lane_control_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_phy_rx_lane_control_t *rx_control)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return phymod_phy_rx_lane_control_get(&phy_access[0], rx_control);
    }

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_rx_ln_ctrl_get(unit, port, rx_control);                 
        } else {
            if ((phy_access[phy_index].access.lane_mask == 0) && phy_index) {
                rv = PHYMOD_E_NONE;
            } else {
                rv = phymod_phy_rx_lane_control_get(&phy_access[phy_index], rx_control);
            }
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_fec_enable_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length, uint32 flags,
                                         uint32_t enable) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return phymod_phy_fec_enable_set(&phy_access[0], enable);
    }

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_fec_set(unit, port, enable);
            return rv;
        } else {
            rv = phymod_phy_fec_enable_set(&phy_access[phy_index], enable); 
            return rv;
        }

        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_fec_enable_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length, uint32 flags,
                                         uint32_t *enable) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return phymod_phy_fec_enable_get(&phy_access[0], enable);
    }

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_fec_get(unit, port, enable);
            return rv;
        } else {
            rv = phymod_phy_fec_enable_get(&phy_access[phy_index], enable); 
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_eee_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t enable) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_eee_set(unit, port, enable); 
            return rv; 
        } else {
            rv = phymod_phy_eee_set(&phy_access[phy_index], enable); 
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_eee_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t *enable)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_eee_get(unit, port, enable);
            return rv;
        } else {
            rv = phymod_phy_eee_get(&phy_access[phy_index], enable); 
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_interface_config_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t flags,
                                               const phymod_phy_inf_config_t* config,
                                               phymod_interface_t serdes_interface,
                                               int ref_clk, 
                                               const uint32 phy_init_flags)
{
    int rv = PHYMOD_E_NONE;
    int phy_index;
    phymod_phy_inf_config_t int_config = *(phymod_phy_inf_config_t*)config;
    phymod_autoneg_control_t an;
    uint32_t an_done;
    int is_legacy_phy = 0;
    soc_port_if_t interface = SOC_PORT_IF_NULL;
    SOC_INIT_FUNC_DEFS;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    if(phy_init_flags == PORTMOD_INIT_F_ALL_PHYS) { 
        /* ALL PHYS */
        while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {

            is_legacy_phy = 0;

            /* if it is external phy, check to see if it is legacy phy. */
            if (phy_index > 0) {
                rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
                if (rv != PHYMOD_E_NONE) return rv;
            }

            if (is_legacy_phy) {
                if (flags & PHYMOD_INTF_F_UPDATE_SPEED_LINKUP) {
                    rv = portmod_port_legacy_an_get(unit, port, &an, &an_done);
                    if (rv != PHYMOD_E_NONE) {
                        return rv;
                    }
                    if (an.enable) {
                        phy_index--;
                        continue;
                    }
                }
                rv = portmod_port_legacy_interface_config_set(unit, port, config);
            } else {
                if (phy_access[phy_index].access.lane_mask) {
                    if (flags & PHYMOD_INTF_F_UPDATE_SPEED_LINKUP){
                        rv = phymod_phy_autoneg_get(&phy_access[phy_index], &an, &an_done);
                        if (rv != PHYMOD_E_NONE) {
                            return rv;
                        }
                        if (an.enable) {
                            phy_index--;
                            continue;
                        }
                    }
                    if (phy_index < (chain_length-1)) {
                        if ((serdes_interface != phymodInterfaceCount) && (phy_index == 0)) {
                            /* if user specify the serdes_interface, it will get priority. */
                            int_config.interface_type = serdes_interface;

                            /* update the medium type within interface_mode according to the new interface type */
                            _SOC_IF_ERR_EXIT(portmod_intf_from_phymod_intf(unit, serdes_interface, &interface));
                            _SOC_IF_ERR_EXIT(portmod_media_type_from_port_intf(unit, interface, &int_config));
                        } else {
                            rv = portmod_port_line_to_sys_intf_map(unit, &phy_access[phy_index+1], &phy_access[phy_index], port,
                                           config, ref_clk, phy_init_flags,
                                           int_config.interface_type, &int_config.interface_type);
                        }
                        if (rv) return (rv);
                    }
                    rv = phymod_phy_interface_config_set(&phy_access[phy_index], flags, &int_config);
                } 
            }
            phy_index--;
        }
    } else if (phy_init_flags == PORTMOD_INIT_F_EXTERNAL_MOST_ONLY) {
        /* Most EXT, reprogram the internal and intermediate phy with existing setting. */
        while((PHYMOD_E_NONE == rv) && (phy_index >= 0)) {

            is_legacy_phy = 0;

            /* if it is external phy, check to see if it is legacy phy. */
            if (phy_index > 0) {
                rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
                if (rv != PHYMOD_E_NONE) return rv;
            }

            if (is_legacy_phy) {
                rv = portmod_port_legacy_interface_config_set(unit, port, config);
            } else {
                if ((phy_index == (chain_length - 1)) || (!(flags & PHYMOD_INTF_F_SET_SPD_NO_TRIGGER))) {
                    if (phy_index < (chain_length-1)) {
                        if ((serdes_interface != phymodInterfaceCount) && (phy_index == 0)) {
                            /* if user specify the serdes_interface, it will get priority. */
                            int_config.interface_type = serdes_interface;

                            /* update the medium type within interface_mode according to the new interface type */
                            _SOC_IF_ERR_EXIT(portmod_intf_from_phymod_intf(unit, serdes_interface, &interface));
                            _SOC_IF_ERR_EXIT(portmod_media_type_from_port_intf(unit, interface, &int_config));
                        } else {
                            rv = portmod_port_line_to_sys_intf_map(unit, &phy_access[phy_index+1], &phy_access[phy_index], port,
                                           config, ref_clk, phy_init_flags,
                                           int_config.interface_type, &int_config.interface_type);
                        }
                        if (rv) return (rv);
                    }

                    if(phy_access[phy_index].access.lane_mask){
                        rv = phymod_phy_interface_config_set(&phy_access[phy_index], flags, &int_config);
                    }
                }
            }
            phy_index--;
        }
    } else if (phy_init_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        while(phy_index >= 0) {
            if (phy_index < (chain_length-1)) {
                if ((serdes_interface != phymodInterfaceCount) && (phy_index == 0)) {
                    /* if user specify the serdes_interface, it will get priority. */ 
                    int_config.interface_type = serdes_interface; 

                    /* update the medium type within interface_mode according to the new interface type */
                    _SOC_IF_ERR_EXIT(portmod_intf_from_phymod_intf(unit, serdes_interface, &interface));
                    _SOC_IF_ERR_EXIT(portmod_media_type_from_port_intf(unit, interface, &int_config));
                } else {
                    rv = portmod_port_line_to_sys_intf_map(unit, &phy_access[phy_index+1], &phy_access[phy_index], port,
                                   config, ref_clk, phy_init_flags,
                                   int_config.interface_type, &int_config.interface_type);
                }
                if (rv) return (rv);
            }
            phy_index--;
        }
        rv = phymod_phy_interface_config_set(&phy_access[0], flags, &int_config);
    }
    return(rv);

exit:
    SOC_FUNC_RETURN;
}

int portmod_port_phychain_interface_config_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t flags,
                                  phymod_ref_clk_t ref_clock,
                                  phymod_phy_inf_config_t* config,
                                  const uint32 phy_init_flags)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_operation_mode_t op_mode = phymodOperationModeRetimer;
    int interface_available = 1;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    
    if (phy_init_flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) {
        return phymod_phy_interface_config_get(&phy_access[0], flags, ref_clock, config); 
    }

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_interface_config_get(unit, port, config);
            return rv;
        } else {
            if ((phy_access[phy_index].access.lane_mask == 0) & phy_index) {
                rv = PHYMOD_E_NONE;
            } else {
                /*
                 * If external phy, check to see if it is in Passthru mode,  get
                 * the interface information only when it is not in Passthru mode.
                 * Otherwise, back off to the internal serdes.
                 */

                interface_available = TRUE;
                if(phy_index){
                    rv = phymod_phy_op_mode_get(&phy_access[phy_index], &op_mode);
                    if ((rv == PHYMOD_E_NONE) && (op_mode == phymodOperationModePassthru)) {
                        interface_available = FALSE;
                    } 
                }

                if (interface_available){ 
                    rv = phymod_phy_interface_config_get(&phy_access[phy_index], flags, ref_clock, config);
                } else {
                    rv = PHYMOD_E_UNAVAIL;
                }
            }
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_duplex_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         phymod_duplex_mode_t *full_duplex)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (is_legacy_phy) {
            rv = portmod_port_legacy_duplex_get(unit, port, (int*)full_duplex);
        } else {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_duplex_get(&phy_access[phy_index], full_duplex);
            } else {
                rv = PHYMOD_E_NONE;
            }
            return rv;
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_cl72_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t cl72_en)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_cl72_set(unit, port, cl72_en);
            return rv;
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = phymod_phy_cl72_set(&phy_access[phy_index], cl72_en);
            } else {
                rv = PHYMOD_E_NONE;
            }
            return rv;
        }
        /*
         * COVERITY
         *
         * This default is unreachable. It is kept intentionally as a defensive default for future
         * development.
         */
        /* coverity[unreachable] */
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_cl72_get(int unit, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *cl72_en)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_cl72_get(&phy_access[phy_index], cl72_en);
            } else {
                rv = PHYMOD_E_NONE;
            }
            return rv;
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_cl72_status_get(int unit, const phymod_phy_access_t* phy_access, 
                                          const int chain_length,
                                          phymod_cl72_status_t *status)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_cl72_status_get(&phy_access[phy_index], status);
            return rv;
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_ability_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              const phymod_autoneg_ability_t* an_ability,
                                              const portmod_port_ability_t * port_ability)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_operation_mode_t phy_op_mode = phymodOperationModeRetimer;

    rv = portmod_port_phychain_operation_mode_get(unit, port, phy_access, chain_length, &phy_op_mode);

    if ((flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) || ((phy_op_mode == phymodOperationModePassthru) &&
        (rv == PHYMOD_E_NONE))) {
        return phymod_phy_autoneg_ability_set(&phy_access[0], an_ability); 
    }

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    rv = PHYMOD_E_UNAVAIL;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_advert_set(unit, port, port_ability);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = phymod_phy_autoneg_ability_set(&phy_access[phy_index], an_ability);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_ability_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_autoneg_ability_t* an_ability,
                                              portmod_port_ability_t * port_ability)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_operation_mode_t phy_op_mode = phymodOperationModeRetimer;

    rv = portmod_port_phychain_operation_mode_get(unit, port, phy_access, chain_length, &phy_op_mode);
    
    /* When flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY returns 1,
     * it means that the current function is called by a legacy external
     * PHY API through the portmod callback function. In this case,
     * the target to retrieve information is always the internal PHY.
     */
    if ((flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) || ((phy_op_mode == phymodOperationModePassthru) &&
        (rv == PHYMOD_E_NONE))) {
        rv = phymod_phy_autoneg_ability_get(&phy_access[0], an_ability);

        /* when there a legacy ext PHY attaching to a port, the caller of this function
         * expects that the retrieved abilities are stored in port_ability.
         */
        portmod_common_phy_to_port_ability(an_ability, port_ability);
        return(rv);
    }

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    rv = PHYMOD_E_UNAVAIL;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_advert_get(unit, port, port_ability);
        } else {
            rv = phymod_phy_autoneg_ability_get(&phy_access[phy_index], an_ability);
        }
        phy_index--;
    }
    return(rv);
}
int portmod_port_phychain_local_ability_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            portmod_port_ability_t* port_ability)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
            if (is_legacy_phy) {
                rv = portmod_port_legacy_ability_local_get(unit, port, port_ability);
            } else {
                return rv;
            }
        } else {
            return rv;
        }
        phy_index--;
    }
    return(rv);
}
int portmod_port_phychain_autoneg_restart_set(int unit, const phymod_phy_access_t* phy_access,
                                                              const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    phymod_autoneg_control_t an;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            phymod_autoneg_control_t_init(&an);
            an.an_mode = phymod_AN_MODE_CL37_SGMII;
            rv = phymod_phy_autoneg_restart_set(&phy_access[phy_index], &an);
        }
        phy_index--;
    }
    return(rv);

}

int portmod_port_phychain_autoneg_remote_ability_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                                     const int chain_length, uint32 flags,
                                                     phymod_autoneg_ability_t* an_ability,
                                                     portmod_port_ability_t * port_ability)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_operation_mode_t phy_op_mode = phymodOperationModeRetimer;

    rv = portmod_port_phychain_operation_mode_get(unit, port, phy_access, chain_length, &phy_op_mode);
    
    /* When flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY returns 1,
     * it means that the current function is called by a legacy external
     * PHY API through the portmod callback function. In this case,
     * the target to retrieve information is always the internal PHY.
     */
    if ((flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) || ((phy_op_mode == phymodOperationModePassthru) &&
        (rv == PHYMOD_E_NONE))) {
        rv = phymod_phy_autoneg_remote_ability_get(&phy_access[0], an_ability);
        /* when there a legacy ext PHY attaching to a port, the caller of this function
         * expects that the retrieved abilities are stored in port_ability.
         */
        portmod_common_phy_to_port_ability(an_ability, port_ability);
        return(rv);
    }

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    rv = PHYMOD_E_UNAVAIL;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_ability_remote_get(unit, port, port_ability);
        } else {
            rv = phymod_phy_autoneg_remote_ability_get(&phy_access[phy_index], an_ability);
        }
        phy_index--; 
    }
    return(rv);
}

int portmod_port_phychain_autoneg_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                      const int chain_length, uint32 flags,
                                      const phymod_autoneg_control_t *an_config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_operation_mode_t phy_op_mode = phymodOperationModeRetimer;

    rv = portmod_port_phychain_operation_mode_get(unit, port, phy_access, chain_length, &phy_op_mode);

    if ((flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) || ((phy_op_mode == phymodOperationModePassthru) &&
        (rv == PHYMOD_E_NONE))) {
        return phymod_phy_autoneg_set(&phy_access[0], an_config); 
    }

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    rv = PHYMOD_E_UNAVAIL;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
                rv = portmod_port_legacy_an_set(unit, port, an_config);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = phymod_phy_autoneg_set(&phy_access[phy_index], an_config);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                      const int chain_length, uint32 flags,
                                      phymod_autoneg_control_t *an_config,
                                      uint32_t * an_done)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_operation_mode_t phy_op_mode = phymodOperationModeRetimer;

    rv = portmod_port_phychain_operation_mode_get(unit, port, phy_access, chain_length, &phy_op_mode);

    if ((flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY) || ((phy_op_mode == phymodOperationModePassthru) &&
        (rv == PHYMOD_E_NONE))) {
        return phymod_phy_autoneg_get(&phy_access[0], an_config, an_done); 
    }

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    rv = PHYMOD_E_UNAVAIL;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_an_get(unit, port, an_config, an_done);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = phymod_phy_autoneg_get(&phy_access[phy_index], an_config, an_done);
            } else {
                rv = PHYMOD_E_NONE;
                *an_done = 1;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_autoneg_status_get(int unit, int port, const phymod_phy_access_t* phy_access, 
                                          const int chain_length,
                                          phymod_autoneg_status_t *status)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    phymod_autoneg_control_t an;
    uint32 an_done;
    int is_legacy_phy = 0;
    phymod_operation_mode_t phy_op_mode = phymodOperationModeRetimer;

    rv = portmod_port_phychain_operation_mode_get(unit, port, phy_access, chain_length, &phy_op_mode);
    if ((phy_op_mode == phymodOperationModePassthru) && (rv == PHYMOD_E_NONE)) {
        rv = phymod_phy_autoneg_get(&phy_access[0], &an, &an_done);
        if (!rv) {
            status->enabled   = an.enable;
            status->locked    = an_done;
        }
        return rv;
    }
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;
    rv = PHYMOD_E_UNAVAIL;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_an_get(unit, port, &an, &an_done);
        } else {
            rv = phymod_phy_autoneg_get(&phy_access[phy_index], &an, &an_done);
        }

        phy_index--;
        if (!rv) {
            status->enabled   = an.enable; 
            status->locked    = an_done; 
        }
        return rv;
    }
    return(rv);
}

int portmod_port_phychain_core_init(int unit, const phymod_core_access_t* core,
                                    const int chain_length,
                                    const phymod_core_init_config_t* init_config, 
                                    const phymod_core_status_t* core_status)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    phymod_core_init_config_t core_config;
    int lane;
    phymod_core_access_t core_access;
    phymod_firmware_load_method_t  fw_load_method;
    int force_fw_load;
    phymod_lane_map_t lane_map;
    int is_legacy_phy = 0;
 
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        if (phy_index) {
            /* core init for the legacy phys are done as part of phy init */
            rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (!is_legacy_phy) {
            sal_memcpy(&core_config, init_config,sizeof(phymod_core_init_config_t));
            if(phy_index > 0) { /* external phy */

                SOC_IF_ERROR_RETURN(portmod_xphy_core_access_get(unit, core[phy_index].access.addr, &core_access, &is_legacy_phy));
                SOC_IF_ERROR_RETURN(portmod_xphy_fw_load_method_get(unit, core[phy_index].access.addr, &fw_load_method));
                SOC_IF_ERROR_RETURN(portmod_xphy_force_fw_load_get(unit, core[phy_index].access.addr, &force_fw_load));
                SOC_IF_ERROR_RETURN(portmod_xphy_lane_map_get(unit, core[phy_index].access.addr, &lane_map));
                /* for external phys, FW load method is default to Internal
                   unless it is force to NONE. */
                core_config.firmware_load_method = fw_load_method;
                core_config.firmware_loader = NULL;
                if (force_fw_load == phymodFirmwareLoadForce) {
                    PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_SET(&core_config);
                }
                else if (force_fw_load == phymodFirmwareLoadAuto) {
                    PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_SET(&core_config);
                }


                /* config "no swap" for external phys.set later using APIs*/
                for(lane=0 ; lane < lane_map.num_of_lanes; lane++) {
                    core_config.lane_map.lane_map_rx[lane] = lane_map.lane_map_rx[lane];
                    core_config.lane_map.lane_map_tx[lane] = lane_map.lane_map_tx[lane];
                }
            }
            rv = phymod_core_init(&core[phy_index], &core_config, core_status);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pll_multiplier_get(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t *core_vco_pll_multiplier)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_pll_multiplier_get(&phy_access[phy_index], core_vco_pll_multiplier);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_init(int unit, const phymod_phy_access_t* phy_access,
                                   const int chain_length,
                                   const phymod_phy_init_config_t* init_config)
{
    int rv = PHYMOD_E_NONE;
    int tx_rv = PHYMOD_E_NONE;
    int phy_index;
    phymod_phy_init_config_t local_init_config;
    phymod_polarity_t polarity;
    phymod_tx_t phymod_tx;
    int i;
    int is_legacy_phy = 0;

    sal_memcpy(&local_init_config, init_config, sizeof(phymod_phy_init_config_t));

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* ALL PHYS */
    while(((rv == PHYMOD_E_NONE) || (rv == PHYMOD_E_UNAVAIL)) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            phy_index--;
            sal_memcpy(&local_init_config, init_config, sizeof(phymod_phy_init_config_t));
            /* get default tx paramters for internal and intermediary phys. */
            tx_rv = phymod_phy_media_type_tx_get(&phy_access[phy_index], phymodMediaTypeChipToChip, &phymod_tx);
            /* if we can get the custom default tx paramter, use it.  Otherwise, use whatever it has in there. */
            if( tx_rv == PHYMOD_E_NONE ) {
                for(i=0 ; i<PHYMOD_MAX_LANES_PER_CORE; i++) {
                    local_init_config.tx[i].pre  = phymod_tx.pre  ;
                    local_init_config.tx[i].main = phymod_tx.main ;
                    local_init_config.tx[i].post = phymod_tx.post ;
                    local_init_config.tx[i].post2= phymod_tx.post2;
                    local_init_config.tx[i].post3= phymod_tx.post3;
                    local_init_config.tx[i].amp  = phymod_tx.amp  ;
                }
            }

        } else {
            sal_memcpy(&local_init_config, init_config, sizeof(phymod_phy_init_config_t));

            if(phy_index != 0 ){
                phymod_polarity_t_init(&local_init_config.polarity);
                rv = portmod_xphy_polarity_get(unit, phy_access[phy_index].access.addr, &polarity);
                if (rv == PHYMOD_E_NONE) {
                sal_memcpy(&(local_init_config.polarity), &(polarity), sizeof(phymod_polarity_t));

                /* overwrite tx params if set by config. phymod_phy_init only use tx, not ext_phy_tx */
                for(i=0 ; i<PHYMOD_MAX_LANES_PER_CORE; i++) {
                    local_init_config.tx[i].pre  = init_config->ext_phy_tx[i].pre  ;
                    local_init_config.tx[i].main = init_config->ext_phy_tx[i].main ;
                    local_init_config.tx[i].post = init_config->ext_phy_tx[i].post ;
                    local_init_config.tx[i].post2 = init_config->ext_phy_tx[i].post2 ;
                    local_init_config.tx[i].post3 = init_config->ext_phy_tx[i].post3 ;
                    local_init_config.tx[i].amp  = init_config->ext_phy_tx[i].amp  ;
                }
                } else return (rv);
            }

            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_init(&phy_access[phy_index], &local_init_config);
            }

            phy_index--;
        }
    } /* while */
    return(rv);
}
int portmod_port_phychain_rx_pmd_locked_get(int unit, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *rx_pmd_locked)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_rx_pmd_locked_get(&phy_access[phy_index], rx_pmd_locked);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_pmd_lock_counter_get(int unit, const phymod_phy_access_t* phy_access,
                                                  const int chain_length, uint32_t *rx_pmd_lock_count_sum)
{
    int rv = 0;
    int phy_index;
    int is_legacy_phy = 0;
    uint32 rx_pmd_lock_counter = 0;

    /* clear output value */
    *rx_pmd_lock_count_sum = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while(phy_index >= 0) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_rx_pmd_lock_counter_get(&phy_access[phy_index], &rx_pmd_lock_counter);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        *rx_pmd_lock_count_sum += rx_pmd_lock_counter;
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_link_status_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length, uint32 flags,
                                         uint32_t *link_status)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    uint32_t link_status_chain = 0;
    int psc_repeater_mode = 0;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;
    psc_repeater_mode = PHYMOD_INTF_CONFIG_PHY_PSC_REPEATER_MODE_GET(&phy_access[chain_length - 1]) ;

    /* Check if the phy access is targetted for internal phy only */
    if (flags == PORTMOD_INIT_F_INTERNAL_SERDES_ONLY){ 
        return  phymod_phy_link_status_get(&phy_access[0], link_status);
    }
    /*if external PHY is working on psc_repeater  Most EXT & SerDes else Most EXT */
    while(((PHYMOD_E_UNAVAIL== rv) || psc_repeater_mode) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
                rv = portmod_port_legacy_phy_link_get(unit, port, link_status);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = phymod_phy_link_status_get(&phy_access[phy_index], &link_status_chain);
                if (psc_repeater_mode) {
                    if (phy_index == chain_length -1) {
                                *link_status = link_status_chain ;
                    } else {
                        *link_status &= link_status_chain;
                    }
                } else {
                    *link_status = link_status_chain;
                }
            } else {
                rv = PHYMOD_E_NONE;
                *link_status = 1;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pcs_userspeed_set(int unit, const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            const phymod_pcs_userspeed_config_t* config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_pcs_userspeed_set(&phy_access[phy_index], config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pcs_userspeed_get(int unit, const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            phymod_pcs_userspeed_config_t* config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }if (!is_legacy_phy) {
            rv = phymod_phy_pcs_userspeed_get(&phy_access[phy_index], config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_reg_write(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    int lane,
                                    uint32_t flags,
                                    uint32_t reg_addr,
                                    uint32_t reg_val) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    uint8 data8;
    int is_legacy_phy = 0;

    if (flags & SOC_PHY_INTERNAL) { 
        phy_index = 0;
    } else {
        /* start from the most external phy. */
        phy_index = chain_length - 1;
    }

    /* Most EXT if internal is not specified */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_reg_write(unit, port, flags, reg_addr, reg_val);
        } else {
            if (flags & BCM_PORT_PHY_I2C_DATA8) {
                data8 = reg_val & 0xFF;
                rv = phymod_phy_i2c_write( &phy_access[phy_index], 0,
                                          BCM_PORT_PHY_I2C_DEVAD(reg_addr),
                                          BCM_PORT_PHY_I2C_REGAD(reg_addr),
                                          1,&data8 );
            } else {
                if (lane >= 0) {
                    reg_addr |= PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE;
                    reg_addr &= ~(0x7 << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
                    reg_addr |= ((lane & 0x7) << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
                }
                rv = phymod_phy_reg_write(&phy_access[phy_index], reg_addr, reg_val);
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_reg_read(int unit, int port, const phymod_phy_access_t* phy_access,
                                   const int chain_length,
                                   int lane,
                                   uint32_t flags,
                                   uint32_t reg_addr,
                                   uint32_t *reg_val)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    uint8 data8;
    int is_legacy_phy = 0;

    if (flags & SOC_PHY_INTERNAL) { 
        phy_index = 0;
    } else {
        /* start from the most external phy. */
        phy_index = chain_length - 1;
    }

    /* Most EXT if internal is not specified */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_reg_read(unit, port, flags, reg_addr, reg_val);
        } else {
            if (flags & BCM_PORT_PHY_I2C_DATA8) {
                rv = phymod_phy_i2c_read( &phy_access[phy_index], 0,
                                          BCM_PORT_PHY_I2C_DEVAD(reg_addr),
                                          BCM_PORT_PHY_I2C_REGAD(reg_addr),
                                          1, &data8);
                *reg_val = data8;
            } else {
                if (lane >= 0) {
                    reg_addr |= PHYMOD_REG_ACC_AER_IBLK_FORCE_LANE;
                    reg_addr &= ~(0x7 << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
                    reg_addr |= ((lane & 0x7) << PHYMOD_REG_ACCESS_FLAGS_SHIFT);
                }
                rv = phymod_phy_reg_read(&phy_access[phy_index], reg_addr, reg_val);
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_slicer_position_set(int unit, const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 const phymod_slicer_position_t* position)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_rx_slicer_position_set(&phy_access[phy_index], flags, position);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_slicer_position_get(int unit, const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 phymod_slicer_position_t* position)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;           
                                            
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;           
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_rx_slicer_position_get(&phy_access[phy_index], flags, position);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_rx_slicer_position_max_get(int unit, const phymod_phy_access_t* phy_access,
                                                     const int chain_length,
                                                     uint32_t flags,
                                                     phymod_slicer_position_t* position_min,
                                                     phymod_slicer_position_t* position_max)
{ 
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */    
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_rx_slicer_position_max_get(&phy_access[phy_index], flags, position_min, position_max);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_config_set(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          const phymod_prbs_t* prbs)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_phy_prbs_config_set(&phy_access[phy_index], flags, prbs);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_config_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          phymod_prbs_t* prbs)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_prbs_config_get(&phy_access[phy_index], flags, prbs);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_enable_set(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          uint32_t enable)                                                   
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_prbs_enable_set(&phy_access[phy_index], flags, enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_enable_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          uint32_t *enable)                                                   
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_prbs_enable_get(&phy_access[phy_index], flags, enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_prbs_status_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          phymod_prbs_status_t *prbs_status)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_prbs_status_get(&phy_access[phy_index], flags, prbs_status);
        }
        phy_index--;
    }
    return(rv);
}


int portmod_port_phychain_pattern_config_set(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             const phymod_pattern_t* pattern)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_pattern_config_set(&phy_access[phy_index], pattern);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pattern_config_get(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_pattern_t* pattern)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_pattern_config_get(&phy_access[phy_index], pattern);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pattern_enable_set(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t enable,
                                             phymod_pattern_t* pattern)                                                   
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_pattern_enable_set(&phy_access[phy_index], enable, pattern);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pattern_enable_get(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t *enable)                                                   
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
 
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_pattern_enable_get(&phy_access[phy_index], enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_core_diagnostics_get(int unit, const phymod_core_access_t* core,
                                               const int chain_length,
                                               phymod_core_diagnostics_t* diag) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    PHYMOD_NULL_CHECK(core);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while( (PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, core[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_core_diagnostics_get(&core[phy_index], diag);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_phy_diagnostics_get(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_diagnostics_t *diag)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_diagnostics_get(&phy_access[phy_index], diag);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pmd_info_dump(int unit, const phymod_phy_access_t* phy_access,
                                        const int chain_length,
                                        char *type)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            if(phy_access[phy_index].access.lane_mask){
                rv = phymod_phy_pmd_info_dump(&phy_access[phy_index], type);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_pcs_info_dump(int unit, const phymod_phy_access_t* phy_access,
                                        const int chain_length,
                                        char *type)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            if(phy_access[phy_index].access.lane_mask) {
                rv = phymod_phy_pcs_info_dump(&phy_access[phy_index], type);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

#ifdef _OLD_EYE_
int portmod_port_phychain_meas_lowber_eye(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_phy_eyescan_options_t eyescan_options,
                                          uint32_t *buffer)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_meas_lowber_eye(&phy_access[phy_index], eyescan_options, buffer);
        }
        phy_index--;
    }
    return(rv);
}
int portmod_port_phychain_display_lowber_eye(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_phy_eyescan_options_t eyescan_options,
                                             uint32_t *buffer)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_display_lowber_eye(&phy_access[phy_index], eyescan_options, buffer);
        }
        phy_index--;
    }
    return(rv);
}


int portmod_port_phychain_pmd_ber_end_cmd(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint8_t supp_info,
                                          uint32_t timeout_ms)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_pmd_ber_end_cmd(&phy_access[phy_index], supp_info, timeout_ms); 
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_meas_eye_scan_start(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              uint8_t direction)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_meas_eye_scan_start(&phy_access[phy_index], direction); 
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_meas_eye_scan_done(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_meas_eye_scan_done(&phy_access[phy_index]);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_read_eye_scan_stripe(int unit, const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t *buffer,
                                               uint16_t *status)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_read_eye_scan_stripe(&phy_access[phy_index], buffer, status);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_display_eye_scan_header(int unit, const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_display_eye_scan_header(&phy_access[phy_index], index);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_display_eye_scan_footer(int unit, const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_display_eye_scan_footer(&phy_access[phy_index], index);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_display_eye_scan_stripe(int unit, const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index,
                                                  uint32_t *buffer)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_display_eye_scan_stripe(&phy_access[phy_index], index,buffer);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_eye_scan_debug_info_dump(int unit, const phymod_phy_access_t* phy_access,
                                                   const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    
    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_eye_scan_debug_info_dump(&phy_access[phy_index]);
        }
        phy_index--;
    }
    return(rv);
}
#else
/* add phy_eyescan_run here */

#endif /* _OLD_EYE_ */

int portmod_port_phychain_timesync_tx_info_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_ts_fifo_status_t* ts_tx_info)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (!is_legacy_phy) {
            rv = phymod_timesync_tx_info_get(&phy_access[phy_index], ts_tx_info);
        }

        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_timesync_config_set(int unit, int port, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, 
                                              const portmod_phy_timesync_config_t* config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    uint32_t enable = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_timesync_config_set(unit, port, config);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                enable = (config->flags & SOC_PORT_PHY_TIMESYNC_ENABLE) ? 1 : 0;
                rv = phymod_timesync_enable_set(&phy_access[phy_index], 0, enable);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_config_get(int unit, int port, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, 
                                              portmod_phy_timesync_config_t* config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    uint32_t enable = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {

        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_timesync_config_get(unit, port, config);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = phymod_timesync_enable_get(&phy_access[phy_index], 0, &enable);
                config->flags |= enable ? SOC_PORT_PHY_TIMESYNC_ENABLE : 0;
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    
    return(rv);
}


int portmod_port_phychain_timesync_enable_set(int unit, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, uint32 enable)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_enable_set(&phy_access[phy_index], 0, enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_enable_get(int unit, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, uint32* enable)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_enable_get(&phy_access[phy_index], 0, enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_nco_addend_set(int unit, const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, uint32 freq_step)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_nco_addend_set(&phy_access[phy_index], freq_step);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_nco_addend_get(int unit, const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  uint32* freq_step)
    {
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;                   
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                      
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_nco_addend_get(&phy_access[phy_index], freq_step);
        }
        phy_index--;
    }                                             
    return(rv);
}

int portmod_port_phychain_timesync_framesync_mode_set(int unit, const phymod_phy_access_t* phy_access, 
                                                      const int chain_length, 
                                                      const portmod_timesync_framesync_t* framesync)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;                   
    int is_legacy_phy = 0;

    phymod_timesync_framesync_t phymod_framesync;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
    sal_memcpy(&phymod_framesync, framesync, sizeof(phymod_timesync_framesync_t));
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_framesync_mode_set(&phy_access[phy_index], &phymod_framesync);
        }
        phy_index--;
    }                                             
    return(rv);
}

int portmod_port_phychain_timesync_framesync_mode_get(int unit, const phymod_phy_access_t* phy_access, 
                                                      const int chain_length, 
                                                      phymod_timesync_framesync_t* framesync)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    phymod_timesync_framesync_t phymod_framesync;
                  
    sal_memset(&phymod_framesync, 0, sizeof(phymod_timesync_framesync_t));
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                      
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;                 
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_framesync_mode_get(&phy_access[phy_index], &phymod_framesync);
        }
        phy_index--;
    }
    sal_memcpy(framesync,&phymod_framesync,  sizeof(phymod_timesync_framesync_t));
                                                      
    return(rv);
}

int portmod_port_phychain_timesync_local_time_set(int unit, const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  const uint64 local_time)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
                  
    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                      
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;                 
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_local_time_set(&phy_access[phy_index], local_time);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_local_time_get(int unit, const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  uint64* local_time)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                       
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;                 
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_local_time_get(&phy_access[phy_index], local_time);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_load_ctrl_set(int unit, const phymod_phy_access_t* phy_access, 
                                                 const int chain_length, 
                                                 uint32 load_once, 
                                                 uint32 load_always)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                       
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;                 
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_load_ctrl_set(&phy_access[phy_index], load_once,
                                                         load_always);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_load_ctrl_get(int unit, const phymod_phy_access_t* phy_access, 
                                                 const int chain_length, 
                                                 uint32* load_once, 
                                                 uint32* load_always)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                                                       
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_load_ctrl_get(&phy_access[phy_index], load_once,
                                                         load_always);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_tx_timestamp_offset_set(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32 ts_offset)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_tx_timestamp_offset_set(&phy_access[phy_index],ts_offset);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_tx_timestamp_offset_get(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32* ts_offset)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;                  

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;                          
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_tx_timestamp_offset_get(&phy_access[phy_index],ts_offset);
        }
        phy_index--;                     
    }
    return(rv);
}

int portmod_port_phychain_timesync_rx_timestamp_offset_set(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32 ts_offset)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;                  

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;                          
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_rx_timestamp_offset_set(&phy_access[phy_index],ts_offset);
        }
        phy_index--;                             
    }
    return(rv);
}

int portmod_port_phychain_timesync_rx_timestamp_offset_get(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32* ts_offset)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;                  

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;                          
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_rx_timestamp_offset_get(&phy_access[phy_index],ts_offset);
        }
        phy_index--;                             
    }
    return(rv);
}

int portmod_port_phychain_phy_intr_enable_set(int unit, const phymod_phy_access_t* phy_access, 
                                       const int chain_length, 
                                       uint32_t enable)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;                  

    /* start from the most external phy. */
    phy_index = chain_length - 1;
                
    /* Most EXT */               
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;                          
        } 
        if (!is_legacy_phy){
            rv = phymod_phy_intr_enable_set(&phy_access[phy_index], enable);
        }
        phy_index--;                             
    }
    return(rv);
}

int portmod_port_phychain_phy_intr_enable_get(int unit, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, 
                                              uint32_t* enable)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_intr_enable_get(&phy_access[phy_index],enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_intr_status_get(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              uint32_t* enable)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_intr_status_get(&phy_access[phy_index],enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_intr_status_clear(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    uint32_t enable=0;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_phy_intr_status_get(&phy_access[phy_index], &enable);
            if (rv) return (rv);

            rv = phymod_phy_intr_status_clear(&phy_access[phy_index], enable);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_timesync_do_sync(int unit, const phymod_phy_access_t* phy_access,
                                               const int chain_length )
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
             rv = phymod_timesync_do_sync(&phy_access[phy_index]); 
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_capture_timestamp_get(int unit, const phymod_phy_access_t* phy_access, 
                                                         const int chain_length, uint64* cap_ts)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_capture_timestamp_get(&phy_access[phy_index],cap_ts);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_timesync_heartbeat_timestamp_get(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, uint64* hb_ts)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_timesync_heartbeat_timestamp_get(&phy_access[phy_index],hb_ts);
        }
        phy_index--;
    }
    return(rv);
}
int portmod_port_phychain_edc_config_set(int unit, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         const phymod_edc_config_t* config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    phymod_edc_config_t phymod_config;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    sal_memcpy(&phymod_config, config,  sizeof(phymod_edc_config_t));
    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_edc_config_set(&phy_access[phy_index],&phymod_config);
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_edc_config_get(int unit, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         phymod_edc_config_t* config)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_edc_config_t phymod_config;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    sal_memset(&phymod_config,0,sizeof(phymod_edc_config_t));

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        } 
        if (!is_legacy_phy) {
            rv = phymod_edc_config_get(&phy_access[phy_index],&phymod_config);
        }
        phy_index--;
    }
    sal_memcpy(config,&phymod_config,  sizeof(phymod_edc_config_t));
    return(rv);
}

int portmod_port_phychain_failover_mode_get(int unit, const phymod_phy_access_t *phy_access, 
                                  int chain_length, phymod_failover_mode_t *failover)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_failover_mode_get(&phy_access[phy_index], failover);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_failover_mode_set(int unit, const phymod_phy_access_t *phy_access,
                                  int chain_length, phymod_failover_mode_t failover)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }
        if (!is_legacy_phy) {
            rv = phymod_failover_mode_set(&phy_access[phy_index], failover);
        }
        phy_index--;
    }

    return(rv);
}

int portmod_port_phychain_phy_link_up_event(int unit, int port, const phymod_phy_access_t* phy_access,
                                            const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
            if (is_legacy_phy) {
                rv = portmod_port_legacy_phy_link_up_event(unit, port);
            } else {
                rv = PHYMOD_E_NONE;
            }
        } else {
            rv = PHYMOD_E_NONE;
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_phy_link_down_event(int unit, int port, const phymod_phy_access_t* phy_access,
                                            const int chain_length)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        if (phy_index) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
            if (is_legacy_phy) {
                rv = portmod_port_legacy_phy_link_down_event(unit, port);
            } else {
                rv = PHYMOD_E_NONE;
            }
        } else {
            rv = PHYMOD_E_NONE;
        }
        phy_index--;
    }
    return(rv);
}

/*
 * common routines between pm4x10 and pm4x25 
 */

int portmod_phy_port_diag_ctrl(int unit, soc_port_t port, phymod_phy_access_t *phy_access, 
                      int nof_phys, uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    phymod_tx_t  ln_txparam[PHYMOD_MAX_LANES_PER_CORE];
    uint32 lane_map = phy_access[0].access.lane_mask; 
    int i;

    for (i = 0; i < PHYMOD_MAX_LANES_PER_CORE; i++) {
        SOC_IF_ERROR_RETURN(phymod_tx_t_init(&ln_txparam[i]));
    }
    
    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_DSC));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pmd_info_dump(unit, phy_access, nof_phys,
                                                 (void*)arg));

            break;

        case PHY_DIAG_CTRL_PCS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_PCS));

            SOC_IF_ERROR_RETURN
                (portmod_port_phychain_pcs_info_dump(unit, phy_access, nof_phys,
                                                    (void*)arg));
            break;

        case PHY_DIAG_CTRL_LINKMON_MODE:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_LINKMON_MODE  0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_LINKMON_MODE));

            if (phy_access[0].access.lane_mask) {
                SOC_IF_ERROR_RETURN
                    (portmod_pm_phy_link_mon_enable_set(phy_access, nof_phys, PTR_TO_INT(arg)));
            }
            break;

        case PHY_DIAG_CTRL_LINKMON_STATUS:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_LINKMON_STATUS 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_LINKMON_STATUS));

            if (phy_access[0].access.lane_mask) {
                SOC_IF_ERROR_RETURN
                    (portmod_pm_phy_link_mon_status_get(phy_access, nof_phys));
            }
            break;
       case PHY_DIAG_CTRL_BER_PROJ:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_BER_PROJ 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_BER_PROJ));

            if (phy_access[0].access.lane_mask) {
                SOC_IF_ERROR_RETURN
                    (portmod_pm_phy_ber_proj(phy_access, (soc_port_phy_ber_proj_params_t*)arg));
            }
            break;
       case PHY_DIAG_CTRL_RSFEC_SYMB_ERR:
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "portmod_phy_port_diag_ctrl: "
                                 "u=%d p=%d PHY_DIAG_CTRL_RSFEC_SYMB_ERR 0x%x\n"),
                      unit, port, PHY_DIAG_CTRL_RSFEC_SYMB_ERR));

            if (phy_access[0].access.lane_mask) {
                SOC_IF_ERROR_RETURN
                    (portmod_pm_phy_rsfec_symb_err_get(phy_access,
                                      (soc_port_phy_rsfec_symb_errcnt_t*)arg));
            }
            break;
       default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "portmod_phy_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_SET 0x%x\n"),
                          unit, port, PHY_DIAG_CTRL_SET));
                if (!SAL_BOOT_SIMULATION) {
                    if (!(phy_access->access.lane_mask == 0)) {
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_set(phy_access, nof_phys,
                                        op_cmd, ln_txparam, lane_map, PTR_TO_INT(arg)));
                    }
                }
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "portmod_phy_port_diag_ctrl: "
                                     "u=%d p=%d PHY_DIAG_CTRL_GET 0x%x\n"),
                          unit, port, PHY_DIAG_CTRL_GET));
                if (!SAL_BOOT_SIMULATION) {
                    if (!(phy_access->access.lane_mask == 0)) {
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_get(phy_access,nof_phys,
                                         op_cmd, ln_txparam, lane_map, (uint32 *)arg));
                    } else {
                        *(uint32 *)arg = 0;
                    }
                }
            } else {
                return (SOC_E_UNAVAIL);
            }

            break;
    }

    return (SOC_E_NONE);
}

int portmod_phy_port_reset_get (int unit, int port, pm_info_t pm_info,
                           int reset_mode, int opcode, int* direction)
{
    phymod_core_access_t core_access[1+MAX_PHYN];
    int nof_phys;
    phymod_reset_direction_t reset_direction;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_core_access_get(unit, port, pm_info,
                                           core_access, (1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_core_reset_get(unit, core_access, nof_phys,
                                              reset_mode,
                                              &reset_direction));
    *direction = reset_direction;

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_reset_set (int unit, int port, pm_info_t pm_info,
                           int reset_mode, int opcode, int direction)
{
    phymod_core_access_t core_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_core_access_get(unit, port, pm_info,
                                           core_access, (1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_core_reset_set(unit, core_access, nof_phys,
                                              reset_mode,
                                              direction));
exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_prbs_config_set (int unit, int port, pm_info_t pm_info, 
                                      portmod_prbs_mode_t mode, int flags, 
                                      const phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
   
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("MAC PRBS is not supported")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_set(unit, phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_prbs_config_get (int unit, int port, pm_info_t pm_info, 
                                      portmod_prbs_mode_t mode, int flags, 
                                      phymod_prbs_t* config)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                      (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_config_get(unit, phy_access, nof_phys, flags, config));

exit:
    SOC_FUNC_RETURN;
}


int portmod_phy_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, int enable)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (
              _SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_set(unit, phy_access, nof_phys, flags, enable));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, int* enable)
{ 
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    uint32 is_enabled;
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_enable_get(unit, phy_access, nof_phys, flags, &is_enabled));

    (*enable) = (is_enabled ? 1 : 0);

exit:
    SOC_FUNC_RETURN;
}


int portmod_phy_port_prbs_status_get(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, 
                                     phymod_prbs_status_t* status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;
    SOC_INIT_FUNC_DEFS;

    if(mode == 1 /*MAC*/) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                (_SOC_MSG("MAC PRBS is not supported for PM4x25")));
    }

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_prbs_status_get(unit, phy_access, nof_phys, flags, status));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_link_get(int unit, int port, pm_info_t pm_info, int* link)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access ,(1+MAX_PHYN),
                                           &nof_phys));

    _SOC_IF_ERR_EXIT
            (portmod_port_phychain_link_status_get(unit, port, phy_access, nof_phys, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, (uint32_t*) link));

exit:
    SOC_FUNC_RETURN;
}

int portmod_phy_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status)
{
    phymod_phy_access_t phy_access[1+MAX_PHYN];
    int nof_phys;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                               phy_access ,(1+MAX_PHYN),
                                               &nof_phys));
    _SOC_IF_ERR_EXIT
        (portmod_port_phychain_autoneg_status_get(unit, port, phy_access, nof_phys, an_status));

exit:
    SOC_FUNC_RETURN;
}

int portmod_port_phychain_control_phy_timesync_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                                    const int chain_length,
                                                    const portmod_port_control_phy_timesync_t type,
                                                    uint64_t value)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    PHYMOD_NULL_CHECK(phy_access);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    rv = PHYMOD_E_UNAVAIL; 
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_control_phy_timesync_set(unit, port, type, value);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = portmod_common_control_phy_timesync_set(&phy_access[phy_index], type, value);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_control_phy_timesync_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                                    const int chain_length,
                                                    const portmod_port_control_phy_timesync_t type,
                                                    uint64_t* value)
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;

    PHYMOD_NULL_CHECK(phy_access);

    /* Most EXT first. */
    phy_index = chain_length - 1;

    /* Most EXT */
    rv = PHYMOD_E_UNAVAIL; 
    while((PHYMOD_E_UNAVAIL == rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
            rv = portmod_port_legacy_control_phy_timesync_get(unit, port, type, value);
        } else {
            if (phy_access[phy_index].access.lane_mask) {
                rv = portmod_common_control_phy_timesync_get(&phy_access[phy_index], type, value);
            } else {
                rv = PHYMOD_E_NONE;
            }
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_medium_config_set(int unit, int port, soc_port_medium_t medium,
                                             soc_phy_config_t* config)
{
    return portmod_port_legacy_medium_config_set(unit, port, medium, config);
}

int portmod_port_phychain_medium_config_get(int unit, int port, soc_port_medium_t medium,
                                             soc_phy_config_t* config)
{
    return portmod_port_legacy_medium_config_get(unit, port, medium, config);
}

int portmod_port_phychain_medium_get(int unit, int port, soc_port_medium_t* medium)
{
    return portmod_port_legacy_medium_get(unit, port, medium);
}

int portmod_port_phychain_multi_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    portmod_multi_get_t* multi_get) 
{
    int rv = PHYMOD_E_UNAVAIL;
    int phy_index;
    int is_legacy_phy = 0;
    phymod_multi_data_t  datablk ;  

    /* start from the most external phy. */
    phy_index = chain_length - 1;

    /* Most EXT */
    while((PHYMOD_E_UNAVAIL== rv) && (phy_index >= 0)) {
        is_legacy_phy = 0;

        /* if it is external phy, check to see if it is legacy phy. */
        if (phy_index > 0) {
            rv = portmod_xphy_is_legacy_phy_get(unit, phy_access[phy_index].access.addr, &is_legacy_phy);
            if (rv != PHYMOD_E_NONE) return rv;
        }

        if (is_legacy_phy) {
                rv = portmod_port_legacy_multi_get(unit, port,
                                                   multi_get->flags,
                                                   multi_get->dev_addr,
                                                   multi_get->offset,
                                                   multi_get->max_size,
                                                   multi_get->data,
                                                   (int *) multi_get->actual_size);

        } else {
            datablk.flags       = multi_get->flags;
            datablk.dev_addr    = multi_get->dev_addr ;
            datablk.offset      = multi_get->offset ;
            datablk.max_size    = multi_get->max_size ;
            datablk.data        = multi_get->data ;
            datablk.actual_size = multi_get->actual_size ;

            rv = phymod_phy_multi_get(&phy_access[phy_index], &datablk) ;
        }
        phy_index--;
    }
    return(rv);
}

int portmod_port_phychain_master_set(int unit, int port, pm_info_t pm_info, int master_mode)
{
    phymod_phy_access_t phy_access_arr[1+MAX_PHYN];
    int xphy_id;
    int is_legacy_phy;
    int nof_phys;
    phymod_master_mode_t ms_mode = phymodMSNone;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access_arr ,(1+MAX_PHYN),
                                           &nof_phys));

    if (nof_phys == 1) {
        switch (master_mode) {
            case SOC_PORT_MS_SLAVE:
                ms_mode = phymodMSSlave;
                break;
            case SOC_PORT_MS_MASTER:
                ms_mode = phymodMSMaster;
                break;
            case SOC_PORT_MS_AUTO:
                ms_mode = phymodMSAuto;
                break;
            default:
                return PHYMOD_E_PARAM;
        }
        _SOC_IF_ERR_EXIT
            (phymod_phy_master_set(&phy_access_arr[0], ms_mode));
    } else {
        xphy_id = phy_access_arr[nof_phys-1].access.addr;
        _SOC_IF_ERR_EXIT
            (portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));

        if (is_legacy_phy) {
            _SOC_IF_ERR_EXIT
                (portmod_port_legacy_master_set(unit, port, master_mode));
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/* 
 * This function retrieves the SGMII AN mode (master/slave/auto/none).
 * Currently it only retrieves the register value for the legacy PHYs.
 * For internal PHY, master mode is hard-coded as slave mode.
 */
int portmod_port_phychain_master_get(int unit, int port, pm_info_t pm_info, int* master_mode)
{
    phymod_phy_access_t phy_access_arr[1+MAX_PHYN];
    int xphy_id;
    int is_legacy_phy;
    int nof_phys;
    int rv = PHYMOD_E_NONE;
    phymod_master_mode_t ms_mode = phymodMSNone;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access_arr ,(1+MAX_PHYN),
                                           &nof_phys));

    if (nof_phys == 1) {
        rv = phymod_phy_master_get(&phy_access_arr[0], &ms_mode);
        /*The internal PHY is mainly in slave mode */
        if (rv == PHYMOD_E_UNAVAIL) {
            *master_mode = SOC_PORT_MS_SLAVE;
        } else {
            switch (ms_mode) {
                case phymodMSNone:
                    *master_mode = SOC_PORT_MS_NONE;
                    break;
                case phymodMSSlave:
                    *master_mode = SOC_PORT_MS_SLAVE;
                    break;
                case phymodMSMaster:
                    *master_mode = SOC_PORT_MS_MASTER;
                    break;
                case phymodMSAuto:
                    *master_mode = SOC_PORT_MS_AUTO;
                    break;
                default:
                    *master_mode = SOC_PORT_MS_NONE;
                    break;
            }
        }
    } else {
        xphy_id = phy_access_arr[nof_phys-1].access.addr;
        _SOC_IF_ERR_EXIT
            (portmod_xphy_is_legacy_phy_get(unit, xphy_id, &is_legacy_phy));

        if (is_legacy_phy) {
            _SOC_IF_ERR_EXIT
                (portmod_port_legacy_master_get(unit, port, master_mode));
        } else {
            /*
             * PHYMOD-based ext PHYs have not implemented the matser_get API,
             * so we follow the convention of bcmi_esw_portctrl_master_get()
             * to make master_mode BCM_PORT_MS_NONE when the API is not supported.
             * Otherwise there will be error messages popping up during init
             * and ps.
             */
            *master_mode = BCM_PORT_MS_NONE;
        }
    }

exit:
    SOC_FUNC_RETURN;
} 
