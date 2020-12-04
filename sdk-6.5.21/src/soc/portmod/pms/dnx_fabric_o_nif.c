/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <sal/core/alloc.h>
#include <soc/wb_engine.h>
#include <soc/drv.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/dnx_fabric.h>
#include <soc/portmod/dnx_fabric_o_nif.h>
#include <soc/portmod/dnx_fabric_shared.h>

#ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT

int dnx_fabric_o_nif_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int i, rv, link;
    uint32 phy_index;
    int found = FALSE;
    int ports_db_entry = -1;
    uint32 reg_val, bitmap;
    dnx_fabric_internal_t* fabric_o_nif_data;
    portmod_port_add_info_t add_info_tmp;
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    link = (pm_info->pm_data.dnx_fabric->core_index * DNX_FABRIC_LANES_PER_CORE) + phy_index;

    if(!PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        for( i = 0 ; i < MAX_PORTS_PER_DNX_FABRIC_PM; i++){
            rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
            SOCDNX_IF_ERR_EXIT(rv);
            if(ports_db_entry < 0 ){ /*free slot found*/
                rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &port, i);
                SOCDNX_IF_ERR_EXIT(rv);
                found = TRUE;
                break;
            }
        }
        if(!found){
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("no free space in the PM db")));
        }
    }

    SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PMH_DATA_BYPASS_MUXr(unit, link, &reg_val));
    soc_reg_field_set(unit, NBIH_NIF_PMH_DATA_BYPASS_MUXr, &reg_val, DATA_BYPASS_MODE_ILKN_NOT_FMAL_LANE_Nf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_DATA_BYPASS_MUXr(unit, link, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PMH_CLK_BYPASS_MUXr(unit, link, &reg_val));
    soc_reg_field_set(unit, NBIH_NIF_PMH_CLK_BYPASS_MUXr, &reg_val, BYPASS_CLK_MUX_PMH_LANE_Nf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_CLK_BYPASS_MUXr(unit, link, reg_val));

    fabric_o_nif_data = &(pm_info->pm_data.dnx_fabric->fabric_o_nif_data);

    /*For the port_attach part, fabric over nif ports behave as ILKN ports*/
    sal_memcpy(&add_info_tmp, \
        add_info, sizeof(portmod_port_add_info_t));
    add_info_tmp.interface_config.interface = SOC_PORT_IF_ILKN;

    SOCDNX_IF_ERR_EXIT(__portmod__dispatch__[fabric_o_nif_data->pms[0]->type]->f_portmod_pm_bypass_set(unit, fabric_o_nif_data->pms[0], 1)); 
            
    SOCDNX_IF_ERR_EXIT(__portmod__dispatch__[fabric_o_nif_data->pms[0]->type]->f_portmod_port_attach(unit, port, fabric_o_nif_data->pms[0], &add_info_tmp));

    if(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_disable_init(unit, port, pm_info));
   
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_interface_config_set(unit, port, pm_info , &add_info->interface_config, 0));

exit:
    SOCDNX_FUNC_RETURN; 
}


int dnx_fabric_o_nif_port_detach(int unit, int port, pm_info_t pm_info)
{
    dnx_fabric_internal_t* fabric_o_nif_data;
    int i, rv, invalid_port = -1;
    int found = FALSE;
    int is_last = TRUE;
    int ports_db_entry = -1;
    int unintialized = 0;
    SOCDNX_INIT_FUNC_DEFS;

    for( i = 0 ; i < MAX_PORTS_PER_DNX_FABRIC_PM; i++){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_db_entry == port){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &invalid_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
            found = TRUE;
        } else if(ports_db_entry >= 0){
            is_last = FALSE;
        }
    }
    if(!found){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("port was not found in the PM db")));
    }

    fabric_o_nif_data = &(pm_info->pm_data.dnx_fabric->fabric_o_nif_data);

    SOCDNX_IF_ERR_EXIT(__portmod__dispatch__[fabric_o_nif_data->pms[0]->type]->f_portmod_port_detach(unit, port, fabric_o_nif_data->pms[0]));

    SOCDNX_IF_ERR_EXIT(__portmod__dispatch__[fabric_o_nif_data->pms[0]->type]->f_portmod_pm_bypass_set(unit, fabric_o_nif_data->pms[0], 0));

    if (is_last){
        /*remove from data - base*/  
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &unintialized);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &unintialized);
        SOCDNX_IF_ERR_EXIT(rv);       
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


int dnx_fabric_o_nif_slow_read_rate_set(int unit, int port, pm_info_t pm_info, int slow_read_rate) {
    int i, is_bypass, rv;
    uint32 phy_index, bitmap;
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && (bitmap & (1 << (i + phy_index) == 0))) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(READ_NBIH_NIF_PMH_FGS_CFGr(unit, port - SOC_INFO(unit).fabric_logical_port_base, &reg_val));
        soc_reg_field_set(unit, NBIH_NIF_PMH_FGS_CFGr, &reg_val, FGS_RX_SLOW_READ_RATE_LANE_Nf, slow_read_rate);
        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_NIF_PMH_FGS_CFGr(unit, port - SOC_INFO(unit).fabric_logical_port_base, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#endif /* PORTMOD_DNX_FABRIC_O_NIF_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

