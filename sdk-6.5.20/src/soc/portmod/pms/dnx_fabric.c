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

#ifdef PORTMOD_DNX_FABRIC_SUPPORT

/*MAC PRBS Defines*/
#define DNX_FABRIC_MAC_PRBS_LOCK_SHIFT  (31)
#define DNX_FABRIC_MAC_PRBS_LOCK_MASK   (0x1)
#define DNX_FABRIC_MAC_PRBS_CTR_SHIFT   (0)
#define DNX_FABRIC_MAC_PRBS_CTR_MASK    (0x7FFFFFFF)

#define DNX_FABRIC_FMAC_TX (0)
#define DNX_FABRIC_FMAC_RX (1)

STATIC
int dnx_fabric_phy_tx_lane_control_set(int unit, int port, pm_info_t pm_info, phymod_phy_tx_lane_control_t control)
{
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    int nof_phys, phyn;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

    for (phyn = 0; phyn < fabric_data->nof_phys; phyn++) {
        params.phyn = phyn;
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
        SOCDNX_IF_ERR_EXIT(phymod_phy_tx_lane_control_set(&phy_access, control));
    }

exit:
        SOCDNX_FUNC_RETURN;
}

STATIC
int dnx_fabric_phy_reset_set(int unit, int port, pm_info_t pm_info, phymod_reset_direction_t reset_direction)
{

    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    phymod_phy_reset_t phy_reset;
    int nof_phys, phyn;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

    for (phyn = 0; phyn < fabric_data->nof_phys; phyn++) {
        params.phyn = phyn;
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
        SOCDNX_IF_ERR_EXIT(phymod_phy_reset_get(&phy_access, &phy_reset));
        phy_reset.tx = reset_direction;
        phy_reset.rx = reset_direction;
        SOCDNX_IF_ERR_EXIT(phymod_phy_reset_set(&phy_access, &phy_reset));
    }

    exit:
        SOCDNX_FUNC_RETURN;
}

int dnx_fabric_lane_phychain_core_access_get(int unit, int lane_index, int max_cores, pm_info_t pm_info, 
                                phymod_core_access_t* core_access_arr)
{
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    portmod_ext_phy_core_info_t core_info;
    int phyn, core_index;
    SOCDNX_INIT_FUNC_DEFS;

    if(max_cores < 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("array should be at size 1 at least")));
    }
    for(phyn = 0; phyn < fabric_data->nof_phys; phyn++){
        SOCDNX_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[phyn]));
        if(0 == phyn){
            SOCDNX_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[phyn]));
            sal_memcpy(&core_access_arr[phyn].access, &fabric_data->access, sizeof(fabric_data->access));
            core_access_arr[phyn].type = fabric_data->core_type;
        }
        else{
            core_index = fabric_data->phy_chain[phyn - 1][lane_index].core_index;
            SOCDNX_IF_ERR_EXIT(portmod_phychain_ext_phy_info_get(unit, phyn, core_index, &core_info));
            sal_memcpy(&core_access_arr[phyn], &core_info.core_access, sizeof(phymod_core_access_t));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int dnx_fabric_port_most_ext_phy_lane_access_get(int unit, int port, pm_info_t pm_info, phymod_phy_access_t* phy_access)
{
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    portmod_access_get_params_t params;
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.phyn = fabric_data->nof_phys - 1;
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, phy_access, &nof_phys, NULL));

exit:
    SOCDNX_FUNC_RETURN; 
}

int dnx_fabric_port_phy_index_get(int unit, int port, pm_info_t pm_info, uint32 *phy_index, uint32 *bitmap){
    int first_phy = 0;
    int sub_phy = 0;
    int i, rv = 0, tmp_port = 0;
    uint32 is_bypass;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    *bitmap = 0;

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(portmod_port_first_phy_get(unit, port, &first_phy, &sub_phy));

    *phy_index = (first_phy - fabric_data->first_phy_offset) % DNX_FABRIC_LANES_PER_CORE;

    if (is_bypass) {
        *phy_index = -1;
        for (i = 0; i < MAX_PORTS_PER_DNX_FABRIC_PM; i++) {
            rv = DNX_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
            SOCDNX_IF_ERR_EXIT(rv);

            if(tmp_port == port){
                *phy_index = (*phy_index == -1 ? i : *phy_index);
                SHR_BITSET(bitmap, i);
            }
        }

        if(*phy_index == -1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL,
                   (_BSL_SOCDNX_MSG("port was not found in internal DB %d"), port));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int dnx_fabric_port_duplex_set(int unit, int port, pm_info_t pm_info,int enable)
{
    return SOC_E_NONE; 
}


int dnx_fabric_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    *enable = TRUE; /* Full Duplex */
    return SOC_E_NONE; 
}


int dnx_fabric_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported){
    *is_supported = (interface == SOC_PORT_IF_SFI);
    return SOC_E_NONE;
}

STATIC
int dnx_fabric_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id;
    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    buffer_id = wb_buffer_index;
    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "dnx_fabric", NULL, NULL, VERSION(3), 1, SOC_WB_ENGINE_PRE_RELEASE);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_probed", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_probed] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_initialized", wb_buffer_index, sizeof(uint32), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_initialized] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_DNX_FABRIC_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_ports] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "polarity_rx", wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_DNX_FABRIC_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_polarity_rx] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "polarity_tx", wb_buffer_index, sizeof(uint32), NULL, MAX_PORTS_PER_DNX_FABRIC_PM, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_polarity_tx] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane_map", wb_buffer_index, sizeof(phymod_lane_map_t), NULL, VERSION(1));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_lane_map] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_bypassed", wb_buffer_index, sizeof(uint32), NULL, VERSION(2));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_is_bypassed] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "lane2port_map", wb_buffer_index, sizeof(int), NULL, MAX_PORTS_PER_DNX_FABRIC_PM, VERSION(3));
    SOCDNX_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[wb_lane2port_map] = wb_var_id;

    SOCDNX_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));
exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_pm_phys_get(int unit, pm_info_t pm_info, portmod_pbmp_t* phys)
{
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_ASSIGN(*phys, fabric_data->phys);
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{   
    const portmod_dnx_fabric_create_info_internal_t *info = &pm_add_info->pm_specific_info.dnx_fabric;
    dnx_fabric_t fabric_data;
    int rv, i, j, invalid_port = -1;
    int is_phymod_probed=0;
    uint32 rx_polarity, tx_polarity, is_bypassed, is_core_probed;
    SOCDNX_INIT_FUNC_DEFS;

    fabric_data = NULL;
    fabric_data = sal_alloc(sizeof(*(pm_info->pm_data.dnx_fabric)), "dnx_fabric specific_db");    
    SOCDNX_NULL_CHECK(fabric_data);
    sal_memset(fabric_data, 0, sizeof(*(pm_info->pm_data.dnx_fabric)));

    pm_info->wb_buffer_id = wb_buffer_index;
    pm_info->pm_data.dnx_fabric = fabric_data;

    #ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
        if (pm_add_info->pm_specific_info.dnx_fabric.is_over_nif) {
            pm_info->type = portmodDispatchTypeDnx_fabric_o_nif;

            fabric_data->fabric_o_nif_data.pms = sal_alloc(sizeof(pm_info_t), "fabric over nif pms");
            SOCDNX_NULL_CHECK(fabric_data->fabric_o_nif_data.pms );
            fabric_data->fabric_o_nif_data.pms[0] = info->pms[0];
            pm_info->pm_data.dnx_fabric->fabric_o_nif_data.pms = fabric_data->fabric_o_nif_data.pms;
        } else
    #endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT*/
        {
            pm_info->type = portmodDispatchTypeDnx_fabric;
        }

    
    fabric_data->core_type = phymodDispatchTypeInvalid; 


    if ((info->ref_clk != phymodRefClk156Mhz) && (info->ref_clk != phymodRefClk125Mhz))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("illegal ref clock")));
    }
    if (info->lane_map.num_of_lanes != DNX_FABRIC_LANES_PER_CORE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("lane map is not supported for dnx_fabric")));
    }
    SOCDNX_IF_ERR_EXIT(phymod_firmware_load_method_t_validate(pm_add_info->pm_specific_info.dnx_fabric.fw_load_method));

    sal_memcpy(&(fabric_data->access), &info->access, sizeof(phymod_access_t));
    fabric_data->first_phy_offset = pm_add_info->pm_specific_info.dnx_fabric.first_phy_offset;
    fabric_data->core_index = pm_add_info->pm_specific_info.dnx_fabric.core_index;
    fabric_data->fmac_schan_id = pm_add_info->pm_specific_info.dnx_fabric.fmac_schan_id;
    fabric_data->fsrd_schan_id = pm_add_info->pm_specific_info.dnx_fabric.fsrd_schan_id;
    fabric_data->fsrd_internal_quad = pm_add_info->pm_specific_info.dnx_fabric.fsrd_internal_quad;
    fabric_data->ref_clk = info->ref_clk;
    fabric_data->com_clk = info->com_clk;
    fabric_data->fw_load_method = pm_add_info->pm_specific_info.dnx_fabric.fw_load_method;
    fabric_data->fw_loader = pm_add_info->pm_specific_info.dnx_fabric.external_fw_loader;
    fabric_data->nof_phys = 1; /*internal PHY only*/
    PORTMOD_PBMP_ASSIGN(fabric_data->phys, pm_add_info->phys);
    for(i = 0; i < MAX_PHYN; i++){
        for(j = 0; j < DNX_FABRIC_LANES_PER_CORE; j++){
            SOCDNX_IF_ERR_EXIT(portmod_lane_connection_t_init(unit, &(fabric_data->phy_chain[i][j])));
        }
    }

    /*init wb buffer*/
    SOCDNX_IF_ERR_EXIT(dnx_fabric_wb_buffer_init(unit, wb_buffer_index,  pm_info));
    if (!SOC_WARM_BOOT(unit))
    {
        rv = SOC_WB_ENGINE_MEMSET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], -1);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane_map], &info->lane_map);
        SOCDNX_IF_ERR_EXIT(rv);
        is_bypassed = 0;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_bypassed], &is_bypassed);
        SOCDNX_IF_ERR_EXIT(rv);
        for (i = 0; i < DNX_FABRIC_LANES_PER_CORE; i++)
        {
            rx_polarity = (info->polarity.rx_polarity >> i) & 0x1;
            tx_polarity = (info->polarity.tx_polarity >> i) & 0x1;

            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
            SOCDNX_IF_ERR_EXIT(rv);
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane2port_map], &invalid_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } 
    else 
    {
        rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);

        if (is_core_probed)
        {
            /* Probe */
            SOCDNX_IF_ERR_EXIT(phymod_core_probe(&info->access, &(fabric_data->core_type), &is_phymod_probed));
            if (!is_phymod_probed)
            {
                SOCDNX_EXIT_WITH_ERR(PHYMOD_E_INIT, (_BSL_SOCDNX_MSG("failed to probe core")));
            }
        }
    }
exit:
    if (SOC_FUNC_ERROR)
    {
        dnx_fabric_pm_destroy(unit, pm_info);
    }
    SOCDNX_FUNC_RETURN;
}



int dnx_fabric_pm_destroy(int unit, pm_info_t pm_info)
{   
    if(pm_info->pm_data.dnx_fabric != NULL){
        #ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
            if (pm_info->type == portmodDispatchTypeDnx_fabric_o_nif) {
                if(pm_info->pm_data.dnx_fabric->fabric_o_nif_data.pms != NULL) {
                    sal_free(pm_info->pm_data.dnx_fabric->fabric_o_nif_data.pms);
                    pm_info->pm_data.dnx_fabric->fabric_o_nif_data.pms = NULL;
                }
            }
        #endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT*/

        sal_free(pm_info->pm_data.dnx_fabric);
        pm_info->pm_data.dnx_fabric = NULL;
    }
    
    return SOC_E_NONE;
}

int dnx_fabric_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    core_info->ref_clk = pm_info->pm_data.dnx_fabric->ref_clk;
    return SOC_E_NONE;
}


STATIC
int dnx_fabric_core_init_phase1(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    phymod_core_access_t core_access[MAX_PHYN];
    portmod_ext_phy_core_info_t core_info;
    phymod_core_status_t status;
    phymod_core_init_config_t core_config;
    int rv, fsrd_schan_id, fsrd_internal_quad;
    int phyn, lane_index, core_index, i, phy_count;
    portmod_pbmp_t pm_port_phys;
    uint32 reg_val, is_core_probed, is_bypass;
    const portmod_port_interface_config_t* config = &(add_info->interface_config);
    SOCDNX_INIT_FUNC_DEFS;

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    PORTMOD_PBMP_ASSIGN(pm_port_phys, fabric_data->phys);
    PORTMOD_PBMP_AND(pm_port_phys, add_info->phys);

    fsrd_schan_id = pm_info->pm_data.dnx_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    fsrd_internal_quad = pm_info->pm_data.dnx_fabric->fsrd_internal_quad;

    if(!PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        /* Get Phy Out Of Reset*/
        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, &reg_val));

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_RX_H_PWRDNf, 0);
        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_TX_H_PWRDNf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_IDDQf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));

        sal_usleep(10);

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_POR_H_RSTBf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));

        /* Probe */
        /* lane of inner phy might be connected to the lane in differrent cores in outer phy,
                 and all the different cores should be probed in outer phy*/
        for(lane_index = 0; lane_index < DNX_FABRIC_LANES_PER_CORE; lane_index++){            
            SOCDNX_IF_ERR_EXIT(dnx_fabric_lane_phychain_core_access_get(unit, lane_index, 1, pm_info, core_access));
            SOCDNX_IF_ERR_EXIT(portmod_port_phychain_core_probe(unit, port, core_access, fabric_data->nof_phys));
            for (phyn = 0; phyn < fabric_data->nof_phys; phyn++) {
                if (0 == phyn) { 
                    fabric_data->core_type = core_access[phyn].type;
                }
                else { 
                    core_index = fabric_data->phy_chain[phyn - 1][lane_index].core_index;
                    portmod_phychain_ext_phy_info_get(unit, phyn, core_index, &core_info); 
                    core_info.core_access.type = core_access[phyn].type;
                    portmod_phychain_ext_phy_info_set(unit, phyn, core_index, &core_info);           
                }
            }
        }

        /* Mark DB as probed */
        is_core_probed = 1;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);
    } else {
        /* Mark DB as fully probed */
        is_core_probed = 2;
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    /*core status*/
    rv = phymod_core_status_t_init(&status);
    SOCDNX_IF_ERR_EXIT(rv);
    status.pmd_active = FALSE;

    /*core config*/
    phymod_core_init_config_t_init(&core_config);
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane_map], &core_config.lane_map);
    SOCDNX_IF_ERR_EXIT(rv);
    core_config.firmware_load_method = fabric_data->fw_load_method;
    if(fabric_data->fw_load_method == phymodFirmwareLoadMethodExternal){
        /* Fast firmware load */
        core_config.firmware_loader = fabric_data->fw_loader;
    }
    else{
        core_config.firmware_loader = NULL;
    }

    core_config.interface.data_rate = config->speed;
    core_config.interface.interface_modes = config->interface_modes;
    core_config.interface.interface_type = phymodInterfaceBypass;
    core_config.interface.ref_clock = fabric_data->ref_clk;
    core_config.interface.com_clock = fabric_data->com_clk;

    if(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_SET(&core_config);
    }
    
    if(PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info)) {
        PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_SET(&core_config);
    }

    if(PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_GET(add_info)) {
        PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_SET(&core_config);
    }

    for(lane_index = 0; lane_index < DNX_FABRIC_LANES_PER_CORE; lane_index++)
    {
        SOCDNX_IF_ERR_EXIT(dnx_fabric_lane_phychain_core_access_get(unit, lane_index, fabric_data->nof_phys, pm_info, core_access));
        for (phyn = 0; phyn < fabric_data->nof_phys; phyn++) {

            /* internal PHY */
            if (0 == phyn)
            {
                /* initialized internal PHY only once */
                if (lane_index == 0) {
                    SOC_PBMP_COUNT(pm_port_phys, phy_count);
                    for (i = 0; i < phy_count; ++i) {
                        core_access[phyn].access.lane_mask |= 1 << i;
                    }
                    PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(core_access[phyn].device_op_mode);
                    SOCDNX_IF_ERR_EXIT(phymod_core_init(&core_access[phyn], &core_config, &status));
                    PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_CLR(core_access[phyn].device_op_mode);
                }

            }
            else {

                core_index = fabric_data->phy_chain[phyn - 1][lane_index].core_index;
                SOCDNX_IF_ERR_EXIT(portmod_phychain_ext_phy_info_get(unit, phyn, core_index, &core_info));

                /* if already initialized continue */
                if (core_info.is_initialized == 1)
                {
                    continue;
                }

                /* read from configuration  */
                core_config.firmware_load_method = phymodFirmwareLoadMethodInternal;
                SOCDNX_IF_ERR_EXIT(phymod_core_init(&core_access[phyn], &core_config, &status));

                /* mark initialized */
                core_info.is_initialized = 1;
                SOCDNX_IF_ERR_EXIT(portmod_phychain_ext_phy_info_set(unit, phyn, core_index, &core_info));

            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC
int dnx_fabric_core_init_phase2(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config)
{
    int rv, fsrd_schan_id, fsrd_internal_quad;
    uint32 reg_val, is_core_initialized;
    SOCDNX_INIT_FUNC_DEFS;

    fsrd_schan_id = pm_info->pm_data.dnx_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    fsrd_internal_quad = pm_info->pm_data.dnx_fabric->fsrd_internal_quad;

    SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, &reg_val));
    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_CORE_DP_H_RSTBf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
    sal_usleep(1);

    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_H_RSTBf, 0xf);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
    sal_usleep(1);

    soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_DP_H_RSTBf, 0xf);
    SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
    sal_usleep(1);

    /* Mark DB as initialized */
    is_core_initialized = TRUE;
    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN; 
}

int
dnx_fabric_interface_config_validate(int unit, pm_info_t pm_info, const portmod_port_interface_config_t* config) 
{
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

   if(fabric_data->ref_clk == phymodRefClk156Mhz){
        switch(config->speed){
        case 25781: /*156.25Mhz only*/
        case 21875:
        case 10937:
        case 25000: /*shared speeds*/
        case 20625: 
        case 12500:
        case 10312:
        case 6250:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM , (_BSL_SOCDNX_MSG("unsupported speed %d for 156.25Mhz"), config->speed));
        }
    } else{
        switch(config->speed){
        case 23000: /*125Mhz only*/
        case 22500:
        case 11500:
        case 11250:
        case 5750:
        case 25000: /*shared speeds*/
        case 20625: 
        case 12500:
        case 10312:
        case 6250:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM , (_BSL_SOCDNX_MSG("unsupported speed %d for 125Mhz"), config->speed));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}




int dnx_fabric_port_disable_init(int unit, int port, pm_info_t pm_info)
{
    uint32 reg_val;
    uint32 field_val[1];
    uint32 phy_index, is_bypass, bitmap;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    int i, rv;
    SOCDNX_INIT_FUNC_DEFS;
    
    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    /*Port (MAC + PHY) Enable Disable supprt*/

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));


    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, &reg_val)); 
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }
        SHR_BITSET(field_val, phy_index + i);
    }
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, reg_val));

    /*SerDes TX Elctrical idle*/
    SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_tx_lane_control_set(unit, port, pm_info, phymodTxElectricalIdleEnable));

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }
        SHR_BITSET(field_val, phy_index + i);
    }
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, reg_val));

    SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionIn));
    
exit:
    SOCDNX_FUNC_RETURN; 
    
}

STATIC 
int dnx_fabric_power_set(int unit, int port, pm_info_t pm_info, int power)
{
    portmod_access_get_params_t params;
    phymod_phy_power_t phy_power;
    phymod_phy_access_t phy_access;
    int nof_phys, phyn;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_power_t_init(&phy_power));
    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access)); 

    if (power) {
        phy_power.rx = phymodPowerOn; 
        phy_power.tx = phymodPowerOn;
    } else {
        phy_power.rx = phymodPowerOff; 
        phy_power.tx = phymodPowerOff;
    }
       
    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    for (phyn = 0; phyn < fabric_data->nof_phys; phyn++) {
        params.phyn = phyn;
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
        SOCDNX_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

int dnx_fabric_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    int i = 0, ii, phy;
    int found = FALSE;
    int rv = 0;
    int ports_db_entry = -1;
    phymod_phy_init_config_t init_config;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int nof_phys, phyn;
    portmod_pbmp_t port_phys_in_pm;
    uint32 reg_val, phy_index, bitmap;
    uint64 reg64_val;
    uint32 is_core_probed = 0, is_core_initialized = 0;
    uint32 is_bypass = 0, phys_count;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    int fmac_blk_id_masked = fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    uint32 rx_polarity, tx_polarity;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    PORTMOD_PBMP_ASSIGN(port_phys_in_pm, add_info->phys);
    PORTMOD_PBMP_AND(port_phys_in_pm, fabric_data->phys);
    PORTMOD_PBMP_COUNT(port_phys_in_pm, phys_count);

    ii = 0;
    SOC_PBMP_ITER(fabric_data->phys, phy){
        if(PORTMOD_PBMP_MEMBER(port_phys_in_pm, phy)){
            rv = DNX_FABRIC_LANE2PORT_SET(unit, pm_info, ii, port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        ii++;
    }

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

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &is_core_probed);
    SOCDNX_IF_ERR_EXIT(rv);
    if(!is_core_probed || (is_core_probed == 1 && PORTMOD_PORT_ADD_F_INIT_PASS2_GET(add_info))){ 
        SOCDNX_IF_ERR_EXIT(dnx_fabric_interface_config_validate(unit, pm_info, &add_info->interface_config));
        SOCDNX_IF_ERR_EXIT(dnx_fabric_core_init_phase1(unit, port, pm_info , add_info));
    }
    
    if(PORTMOD_PORT_ADD_F_INIT_PASS1_GET(add_info)) {
        SOC_EXIT;
    }
    
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized);
    SOCDNX_IF_ERR_EXIT(rv);
    if(!is_core_initialized) {
        SOCDNX_IF_ERR_EXIT(dnx_fabric_core_init_phase2(unit, port, pm_info , &add_info->interface_config));
    }  

    SOCDNX_IF_ERR_EXIT(dnx_fabric_power_set(unit, port, pm_info, 1));
        
    SOCDNX_IF_ERR_EXIT(phymod_phy_init_config_t_init(&init_config));
    init_config.an_en = add_info->autoneg_en;
    init_config.cl72_en = add_info->link_training_en;

    for (i = 0; i < DNX_FABRIC_LANES_PER_CORE; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i); 
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);

        init_config.polarity.rx_polarity |= (rx_polarity << i) & 0xf;
        init_config.polarity.tx_polarity |= (tx_polarity << i) & 0xf;
    }

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if (!is_bypass)
    {
        for (phyn = 0; phyn < fabric_data->nof_phys; phyn++) {
            params.phyn = phyn;
            SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
            SOCDNX_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access, phymodMediaTypeChipToChip, init_config.tx));
            SOCDNX_IF_ERR_EXIT(phymod_phy_init(&phy_access, &init_config));
        }
    }
    else {
        params.phyn = 0;
        for (phyn = 0; phyn < DNX_FABRIC_LANES_PER_CORE; phyn++) {
            SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy_access, &nof_phys, NULL));
            SOCDNX_IF_ERR_EXIT(phymod_phy_media_type_tx_get(&phy_access, phymodMediaTypeChipToChip, &init_config.tx[phyn]));
        }
        PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(phy_access.device_op_mode);
        SOCDNX_IF_ERR_EXIT(phymod_phy_init(&phy_access, &init_config));
        PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_CLR(phy_access.device_op_mode);
    }


    if (is_bypass) {
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        for (i = 0; i < DNX_FABRIC_LANES_PER_CORE; ++i) {
            if (bitmap & (1 << i)) {
                SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_blk_id_masked, i, &reg_val)); 
                soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_TX_ILKN_SHARING_ENf, 1);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_blk_id_masked, i, reg_val));

                SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_id_masked, i, &reg64_val));
                soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_TX_ILKN_ENf, 1);
                soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_RX_ILKN_ENf, 1);
                soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_SLOW_READ_RATEf, 1);
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_id_masked, i, reg64_val));
            }
        }
    }

    /*disable phy and mac*/  
    /*SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_set(unit, port, pm_info, flags, 0));*/
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_disable_init(unit, port, pm_info));
   
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_interface_config_set(unit, port, pm_info , &add_info->interface_config, 0));

exit:
    SOCDNX_FUNC_RETURN; 
}


int dnx_fabric_port_detach(int unit, int port, pm_info_t pm_info)
{
    int i = 0;
    int found = FALSE;
    int invalid_port = -1;
    int ports_db_entry = -1;
    int rv = 0;
    int is_last = TRUE;
    int unintialized = 0;
    int fsrd_schan_id, fsrd_internal_quad;
    uint32 reg_val, tmp_port;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(dnx_fabric_power_set(unit, port, pm_info, 0));
     
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
        rv = DNX_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if(tmp_port == port){
            rv = DNX_FABRIC_LANE2PORT_SET(unit, pm_info, i, invalid_port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }
    if(!found){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("port was not found in the PM db")));
    }
    if (is_last){
        /*Quad in-reset*/
        fsrd_schan_id = pm_info->pm_data.dnx_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
        fsrd_internal_quad = pm_info->pm_data.dnx_fabric->fsrd_internal_quad;


        SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, &reg_val));
        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_DP_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
        sal_usleep(1);

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_DP_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
        sal_usleep(1);

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_H_RSTBf, 0x0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
        sal_usleep(1);

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_CORE_DP_H_RSTBf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
        sal_usleep(1);

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_POR_H_RSTBf, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));

         sal_usleep(10);

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_IDDQf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));

        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_RX_H_PWRDNf, 0xf);
        soc_reg_field_set(unit, FSRD_SRD_QUAD_CTRLr, &reg_val, SRD_QUAD_N_PMD_LN_TX_H_PWRDNf, 0xf);
        SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_CTRLr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
        
        sal_usleep(10);

        /*remove from data - base*/  
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &unintialized);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &unintialized);
        SOCDNX_IF_ERR_EXIT(rv);       
        rv = DNX_FABRIC_IS_BYPASSED_SET(unit, pm_info, unintialized);
        SOCDNX_IF_ERR_EXIT(rv);
    }
exit:
    SOCDNX_FUNC_RETURN; 
}

int dnx_fabric_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
        
    int i = 0;
    int rv = 0;
    int ports_db_entry;
    uint32 tmp_port;
    SOCDNX_INIT_FUNC_DEFS;
    
    for (i = 0; i < MAX_PORTS_PER_DNX_FABRIC_PM; i++){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, i);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_db_entry == port){
            rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &new_port, i);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        rv = DNX_FABRIC_LANE2PORT_GET(unit, pm_info, i, &tmp_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if (tmp_port == port) {
            rv = DNX_FABRIC_LANE2PORT_SET(unit, pm_info, i, new_port);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }
                   
exit:
    SOCDNX_FUNC_RETURN; 
    
}


soc_error_t dnx_fabric_fmac_enable_set(int unit, int port, pm_info_t pm_info, int rx_tx, int enable)
{
    soc_field_t field;
    uint32 field_val[1];
    uint32 reg_val, phy_index, is_bypass, bitmap;
    int i;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    int fmac_blk_id_masked = fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass));

    field = ((rx_tx == DNX_FABRIC_FMAC_TX) ? FMAC_TX_RST_Nf : FMAC_RX_RST_Nf);

    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_id_masked, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, field);

    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }
        if (enable){
            SHR_BITCLR(field_val, phy_index + i);
        }else{
            SHR_BITSET(field_val, phy_index + i);
        }
    }

    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, field, *field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_blk_id_masked, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    uint32 flags_mask = flags;
    uint32 is_bypass;
    int cur_enable, rv;
    phymod_phy_access_t phy_access;
    phymod_phy_power_t phy_power;
    phymod_phy_rx_lane_control_t rx_control;
    SOCDNX_INIT_FUNC_DEFS;

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(flags_mask);
        PORTMOD_PORT_ENABLE_TX_SET(flags_mask);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(flags_mask);
        PORTMOD_PORT_ENABLE_MAC_SET(flags_mask);
    }

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) && 
             PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        /*Port (MAC + PHY) Enable Disable supprt*/

        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_get(unit, port, pm_info, flags, &cur_enable)); 
        if ((cur_enable ? 1 : 0) == (enable ? 1 : 0))
        {
            SOC_EXIT;
        }

        if(enable){
            
            /*SerDes TX Elctrical idle*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_tx_lane_control_set(unit, port, pm_info, phymodTxElectricalIdleEnable));
            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionOut));

            /* Enable FMAC Tx*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_TX, enable));

            /*SerDes TX Elctrical idle disable*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_tx_lane_control_set(unit, port, pm_info, phymodTxElectricalIdleDisable));

            /* Enable FMAC Rx*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_RX, enable));

        } 
        else
        {
            /* Disable FMAC Rx*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_RX, enable));

            /*SerDes TX Elctrical idle*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_tx_lane_control_set(unit, port, pm_info, phymodTxElectricalIdleEnable));

            /* Disable FMAC Tx*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_TX, enable));

            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionIn));
        }
    }
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags) && PORTMOD_PORT_ENABLE_PHY_GET(flags) &&
             PORTMOD_PORT_ENABLE_RX_GET(flags) && !PORTMOD_PORT_ENABLE_TX_GET(flags) && is_bypass)
    {
        SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_most_ext_phy_lane_access_get(unit, port, pm_info, &phy_access));

        if (enable) {
            /* Enable the RX in the PHY */
            phy_power.rx = phymodPowerOn;
            phy_power.tx = phymodPowerNoChange;
            SOCDNX_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));

            /* Set RxSquelchOff */
            rx_control = phymodRxSquelchOff;
            SOCDNX_IF_ERR_EXIT(phymod_phy_rx_lane_control_set(&phy_access, rx_control));

            /* Enable the RX in FMAC */
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_RX, 1));
        } else {
            /* Disable the RX in FMAC */
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_RX, 0));

            /* Set RxSquelchOn */
            rx_control = phymodRxSquelchOn;
            phymod_phy_rx_lane_control_set(&phy_access, rx_control);

            /* Disable the RX in the PHY */
            phy_power.rx = phymodPowerOff;
            phy_power.tx = phymodPowerNoChange;
            SOCDNX_IF_ERR_EXIT(phymod_phy_power_set(&phy_access, &phy_power));
        }
    }
    else if ((PORTMOD_PORT_ENABLE_RX_GET(flags)) && ((PORTMOD_PORT_ENABLE_MAC_GET(flags)) || is_bypass))
    {
        /*MAC RX Enable/Disable support*/
        SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_RX, enable));
    } 
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (!is_bypass){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Fabric doesn't support only MAC option. (Only ILKN over fabric supports that mode).\n")));
        }
        if(enable){
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_TX, enable));
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_RX, enable));
        }else{
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_RX, enable));
            SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_TX, enable));
        }
    }
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            !PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (!is_bypass){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Fabric doesn't support only Tx MAC option. (Only ILKN over fabric supports that mode).\n")));
        }
        SOCDNX_IF_ERR_EXIT(dnx_fabric_fmac_enable_set(unit, port, pm_info, DNX_FABRIC_FMAC_TX, enable));
    }
    else if (!PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) &&
             PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        if (!is_bypass){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Fabric doesn't support only Phy option. (Only ILKN over fabric supports that mode).\n")));
        }
        if(enable){

            /*SerDes TX Elctrical idle*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_tx_lane_control_set(unit, port, pm_info, phymodTxElectricalIdleEnable));
            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionOut));

            /*SerDes TX Elctrical idle disable*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_tx_lane_control_set(unit, port, pm_info, phymodTxElectricalIdleDisable));

        }
        else
        {

            /*SerDes TX Elctrical idle*/
            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_tx_lane_control_set(unit, port, pm_info, phymodTxElectricalIdleEnable));

            SOCDNX_IF_ERR_EXIT(dnx_fabric_phy_reset_set(unit, port, pm_info, phymodResetDirectionIn));
        }
    }
    else
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Supported options are only: (1) all, (2) MAC + RX \n (3) MAC (but for ILKN over fabric only) (4) Tx MAC (but for ILKN over fabric only)\n (5) Phy (but for ILKN over fabric only)\n")));
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}


int dnx_fabric_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int *enable)
{
    uint32 reg_val, flags_mask = flags;
    uint32 field_val[1];
    uint32 is_bypass, mac_reset, tx_mac_reset, bitmap;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    uint32 phy_index;
    phymod_phy_access_t phy_access;
    phymod_phy_reset_t phy_reset;
    phymod_phy_power_t phy_power;
    phymod_phy_rx_lane_control_t rx_control;
    int rx_enable, tx_enable;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));    
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_most_ext_phy_lane_access_get(unit, port, pm_info, &phy_access));
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    mac_reset = SHR_BITGET(field_val, phy_index);
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    tx_mac_reset = SHR_BITGET(field_val, phy_index);

    /* If no RX\TX flags - set both*/
    if((!PORTMOD_PORT_ENABLE_TX_GET(flags)) && (!PORTMOD_PORT_ENABLE_RX_GET(flags))){
        PORTMOD_PORT_ENABLE_RX_SET(flags_mask);
        PORTMOD_PORT_ENABLE_TX_SET(flags_mask);
    }

    /* if no MAC\Phy flags - set both*/
    if((!PORTMOD_PORT_ENABLE_PHY_GET(flags)) && (!PORTMOD_PORT_ENABLE_MAC_GET(flags))){
        PORTMOD_PORT_ENABLE_PHY_SET(flags_mask);
        PORTMOD_PORT_ENABLE_MAC_SET(flags_mask);
    }
    
    if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) && 
         PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        SOCDNX_IF_ERR_EXIT(phymod_phy_reset_get(&phy_access, &phy_reset)); 
         rx_enable = ((!mac_reset) && (phy_reset.rx == phymodResetDirectionOut));
         tx_enable = ((!mac_reset) && (phy_reset.tx == phymodResetDirectionOut));
         *enable = rx_enable || tx_enable;
    } 
    else if ((PORTMOD_PORT_ENABLE_RX_GET(flags)) && ((PORTMOD_PORT_ENABLE_MAC_GET(flags)) || is_bypass))
    {
        *enable = !mac_reset;
    } 
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && (!PORTMOD_PORT_ENABLE_PHY_GET(flags_mask)) &&
            !PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        *enable = !tx_mac_reset;
    }
    else if (PORTMOD_PORT_ENABLE_MAC_GET(flags_mask) && PORTMOD_PORT_ENABLE_PHY_GET(flags_mask) &&
             PORTMOD_PORT_ENABLE_RX_GET(flags_mask) && !PORTMOD_PORT_ENABLE_TX_GET(flags_mask))
    {
        SOCDNX_IF_ERR_EXIT(phymod_phy_power_get(&phy_access, &phy_power));
        SOCDNX_IF_ERR_EXIT(phymod_phy_rx_lane_control_get(&phy_access, &rx_control));
        *enable = ((!mac_reset) && (phy_power.rx == phymodPowerOn) && (rx_control == phymodRxSquelchOff));
    }
    else 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL , (_BSL_SOCDNX_MSG("Supported options are only (1) Rx MAC (2) Tx MAC or (3) all - MAC, PHY, TX, RX")));
    }


exit:
    SOCDNX_FUNC_RETURN;    
}

int dnx_fabric_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, int phy_init_flags)
{
    phymod_phy_inf_config_t interface_config;
    phymod_phy_access_t phy;
    uint32 phy_index = 0, val, reg_val, bitmap;
    int phyn, nof_phys, fsrd_schan_id, fsrd_internal_quad;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    portmod_access_get_params_t params;
    phymod_phy_diagnostics_t phy_diag;
    soc_field_t strap_fields[] = {SRD_QUAD_N_LN_0_STRAPSf, SRD_QUAD_N_LN_1_STRAPSf, SRD_QUAD_N_LN_2_STRAPSf, SRD_QUAD_N_LN_3_STRAPSf};
    uint32 flags;
    int enabled = 0;
    SOCDNX_INIT_FUNC_DEFS;

    fsrd_schan_id = pm_info->pm_data.dnx_fabric->fsrd_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    fsrd_internal_quad = pm_info->pm_data.dnx_fabric->fsrd_internal_quad;

    /*nothing to do for warm boot*/
    if (SOC_WARM_BOOT(unit))
    {
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(dnx_fabric_interface_config_validate(unit, pm_info, config));

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    SOCDNX_IF_ERR_EXIT(phymod_phy_inf_config_t_init(&interface_config));

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    interface_config.data_rate = config->speed;
    interface_config.ref_clock = fabric_data->ref_clk;
    interface_config.interface_type = phymodInterfaceBypass;
    interface_config.interface_modes = config->interface_modes;

    /*Disable ports*/
    flags = 0;
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));

    /*disable phy and mac*/  
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_set(unit, port, pm_info, flags, 0));

    for (phyn = 0; phyn < fabric_data->nof_phys; phyn++) {
        params.phyn = phyn;
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, &phy, &nof_phys, NULL));
        SOCDNX_IF_ERR_EXIT(phymod_phy_interface_config_set(&phy, config->flags, &interface_config));
    }

    SOCDNX_IF_ERR_EXIT(phymod_phy_diagnostics_get(&phy, &phy_diag));
    switch(phy_diag.osr_mode) {
        case phymodOversampleMode1:
            val = 0;
            break;
        case phymodOversampleMode2:
            val = 1;
            break;
        default:
            val = 2;
            break;
    }

    #ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
        if (pm_info->type == portmodDispatchTypeDnx_fabric_o_nif) {
            /* The below register is not relevant for fabric_o_nif as fabric_o_nif doesn't have FSRD */
        } else
    #endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT*/
        {
            SOCDNX_IF_ERR_EXIT(READ_FSRD_SRD_QUAD_LN_STRAPSr(unit, fsrd_schan_id, fsrd_internal_quad, &reg_val));
            soc_reg_field_set(unit, FSRD_SRD_QUAD_LN_STRAPSr, &reg_val, strap_fields[phy_index], val);
            SOCDNX_IF_ERR_EXIT(WRITE_FSRD_SRD_QUAD_LN_STRAPSr(unit, fsrd_schan_id, fsrd_internal_quad, reg_val));
        }

    /*restore MAC and PHY */
    flags = 0;
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_set(unit, port, pm_info, flags, enabled));

exit:
    SOCDNX_FUNC_RETURN; 
}


int dnx_fabric_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{

    portmod_access_get_params_t params;
    phymod_phy_access_t phy;
    int phys_num;
    phymod_phy_inf_config_t phy_config;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(pm_info);

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1 , &phy , &phys_num, NULL));
    if(phys_num != 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL , (_BSL_SOCDNX_MSG("unexpected phys num %d"), phys_num));
    }
        
    SOCDNX_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, config));
    SOCDNX_IF_ERR_EXIT(phymod_phy_interface_config_get(&phy, config->flags, fabric_data->ref_clk, &phy_config));

    config->speed = phy_config.data_rate;
    config->interface_modes = phy_config.interface_modes;
    config->interface = SOC_PORT_IF_SFI;

exit:
    SOCDNX_FUNC_RETURN; 
    
}

int dnx_fabric_port_default_interface_get(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, soc_port_if_t* interface)
{
    *interface = SOC_PORT_IF_NULL;
    return SOC_E_NONE;
}

int dnx_fabric_port_encoding_set(int unit, int port, pm_info_t pm_info, uint32 properties, portmod_port_pcs_t encoding)
{
    int fec_enable = 1;
    int encoding_type = 0;
    int rx_half_frame = 0;
    int tx_half_frame = 0;
    int slow_read_rate = 0;
    int rx_ctrl_bypass = 0;
    int flags = 0;
    int enabled = 0;
    int llfc_cig = 0;
    int low_latency_llfc = 0;
    int rsf_err_mark = 0;
    int fec_err_mark = 0;
    int cig_ignore = 0;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    int fmac_blk_id_masked = fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    uint32 phy_index, bitmap;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64;
    int i , is_bypass, rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    switch(encoding){
    case PORTMOD_PCS_64B66B_FEC:
        encoding_type = 2;
        slow_read_rate = 3;
        break;
    case PORTMOD_PCS_64B66B:
        encoding_type = 2;
        slow_read_rate = 1;
        fec_enable = 0;
        break;
    case PORTMOD_PCS_64B66B_RS_FEC:
        encoding_type = 4;
        slow_read_rate = 2;
        rx_ctrl_bypass = 3;
        low_latency_llfc = 1;
        llfc_cig = 1;
        break;
    case PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC:
        encoding_type = 4;
        slow_read_rate = 2;
        rx_ctrl_bypass = 3;
        rx_half_frame = 1;
        tx_half_frame = 1;
        low_latency_llfc = 1;
        llfc_cig = 1;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported pcs type %d"), encoding));
    }

    /* handle properties*/
    /*Low latency llfc*/
    if(PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("low latency llfc is not supported 64/66 pcs")));
    }
    if(!PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties) && (encoding == PORTMOD_PCS_64B66B_RS_FEC || encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("RS_FEC pcs: low latency llfc is always enabled")));
    }
    if(PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties)){
        low_latency_llfc = 1;
    }

    /*error detect*/
    if(PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties) && (encoding == PORTMOD_PCS_64B66B)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("error detect is not supported 64/66 pcs")));
    }
    if(encoding == PORTMOD_PCS_64B66B_FEC){
        fec_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    } else if((encoding == PORTMOD_PCS_64B66B_RS_FEC) || (encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)){
        rsf_err_mark = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);
    }

    /*extract cig from llfc cells*/
    if(PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties) && (encoding != PORTMOD_PCS_64B66B_FEC)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("error detect is supported just for 64/66 kr fec")));
    }
    cig_ignore = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties) ? 0 : 1;

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    flags = 0;
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));
    /*disable phy and mac*/
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_set(unit, port, pm_info, flags, 0));
    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, &reg_val));
        /*encoding type set*/
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_ENCODING_TYPEf, encoding_type);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, reg_val));
    }

    /*slow read rate configuration*/
    SOCDNX_IF_ERR_EXIT(__portmod__dispatch__[pm_info->type]->f_portmod_slow_read_rate_set(unit, port, pm_info, slow_read_rate));

    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, &reg_val));
        /*enable/disable FEC for 64\66 and 64\66 KR*/
        soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr,&reg_val,FPS_N_RX_FEC_FEC_ENf, fec_enable); 
        /*error mark all - releavnt only for KR FEC(64B66B_FEC)*/
        soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr,&reg_val,FPS_N_RX_FEC_ERR_MARK_ENf, fec_err_mark); 
        soc_reg_field_set(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr,&reg_val,FPS_N_RX_FEC_ERR_MARK_ALLf, fec_err_mark); 
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, reg_val));
    }

    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_TX_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, &reg_val));
        soc_reg_field_set(unit,FMAC_FPS_TX_CONFIGURATIONr,&reg_val,FPS_N_TX_FEC_ENf, fec_enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FPS_TX_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, reg_val));
    }


    /*RS LLFEC settings*/
    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, &reg_val));
        /*set llfc low latency and enable cig*/
        soc_reg_field_set(unit,FMAC_FMAL_RX_GENERAL_CONFIGURATIONr,&reg_val,FMAL_N_LOW_LATENCY_LLFCf, low_latency_llfc);
        soc_reg_field_set(unit,FMAC_FMAL_RX_GENERAL_CONFIGURATIONr,&reg_val,FMAL_N_LOW_LATENCY_CIGf, llfc_cig);
        soc_reg_field_set(unit,FMAC_FMAL_RX_GENERAL_CONFIGURATIONr,&reg_val,FMAL_N_IGNORE_CIG_LLFC_CELLf, cig_ignore);
        soc_reg_field_set(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_RX_CTRL_BYPf, rx_ctrl_bypass);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, reg_val));
    }

    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, reg_above64));
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_ERR_MARK_ENf, rsf_err_mark);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_HALF_FRAMEf, rx_half_frame);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_TX_HALF_FRAMEf, tx_half_frame);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_TX_SCRAMBLE_LLFC_BITS_ENf, 0x1);
        soc_reg_above_64_field32_set(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_SCRAMBLE_LLFC_BITS_ENf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, reg_above64));
    }

    /*restore MAC and PHY */
    flags = 0;
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_set(unit, port, pm_info, flags, enabled));
exit:
    SOCDNX_FUNC_RETURN; 
}

int dnx_fabric_port_encoding_get(int unit, int port, pm_info_t pm_info, uint32 *properties, portmod_port_pcs_t *encoding)
{
    int encoding_type = 0;
    int rx_half_frame = 0;
    int low_latency_llfc = 0;
    int err_mark = 0;
    int cig_ignore = 0;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    int fmac_blk_id_masked = fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    uint32 phy_index, bitmap;
    uint32 reg_val;
    soc_reg_above_64_val_t reg_above64;
    int fec_enable;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
 
    *properties = 0;
    encoding_type = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_ENCODING_TYPEf);
    switch(encoding_type){
    case 2:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
        fec_enable = soc_reg_field_get(unit,FMAC_FPS_RX_FEC_CONFIGURATIONr, reg_val,FPS_N_RX_FEC_FEC_ENf); 
        *encoding = fec_enable ? PORTMOD_PCS_64B66B_FEC : PORTMOD_PCS_64B66B;
        break;
    case 4:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, reg_above64));
        rx_half_frame = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_HALF_FRAMEf);
        if(rx_half_frame == 0){
            *encoding = PORTMOD_PCS_64B66B_RS_FEC;
        }
        else{
            *encoding = PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC;
        }
        break;
    case 0:
        *encoding = PORTMOD_PCS_UNKNOWN;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unknown pcs type %d"), encoding_type));
    }

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_RX_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
    /*set llfc low latency and enable cig*/
    low_latency_llfc = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val ,FMAL_N_LOW_LATENCY_LLFCf);
    cig_ignore = soc_reg_field_get(unit, FMAC_FMAL_RX_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_IGNORE_CIG_LLFC_CELLf);
    if (*encoding == PORTMOD_PCS_64B66B_FEC)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FPS_RX_FEC_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
        err_mark = soc_reg_field_get(unit, FMAC_FPS_RX_FEC_CONFIGURATIONr, reg_val, FPS_N_RX_FEC_ERR_MARK_ALLf); 
    } else if (*encoding == PORTMOD_PCS_64B66B_RS_FEC || *encoding == PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)
    {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_RSF_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, reg_above64));
        err_mark = soc_reg_above_64_field32_get(unit, FMAC_RSF_CONFIGURATIONr, reg_above64, RSF_N_RX_ERR_MARK_ENf);

    } else {
        err_mark = 0;
    }

    if(low_latency_llfc){
         PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(*properties);
    }
    if(!cig_ignore){
         PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(*properties);
    }
    if(err_mark){
         PORTMOD_ENCODING_FEC_ERROR_DETECT_SET(*properties);
    }
exit:
    SOCDNX_FUNC_RETURN; 
}


int dnx_fabric_slow_read_rate_set(int unit, int port, pm_info_t pm_info, int slow_read_rate) {
    int i, is_bypass, rv;
    uint32 phy_index, bitmap;
    uint64 reg64_val;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    int fmac_blk_id_masked = fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;

    SOCDNX_INIT_FUNC_DEFS;

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    for (i = 0; i < (is_bypass ? DNX_FABRIC_LANES_PER_CORE - phy_index : 1); i++)
    {
        if (is_bypass && ((bitmap & (1 << (i + phy_index))) == 0)) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, &reg64_val));
        soc_reg64_field32_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg64_val, FMAL_N_RX_SLOW_READ_RATEf, slow_read_rate);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index + i, reg64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
_dnx_fabric_force_signal_detect_set(int unit, pm_info_t pm_info)
{
    int i, port_index;
    int rv = 0;
    int force_signal_detect = 0;
    uint32 reg_val;
    int ports_db_entry = -1;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    int fmac_blk_id_masked = fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    portmod_loopback_mode_t mac_loopbacks[]= { portmodLoopbackMacAsyncFifo, portmodLoopbackMacOuter, portmodLoopbackMacPCS, portmodLoopbackPhyGloopPMD,portmodLoopbackPhyRloopPMD};
    int lb_enable = FALSE;
    SOCDNX_INIT_FUNC_DEFS;

    for(port_index = 0 ; port_index < MAX_PORTS_PER_DNX_FABRIC_PM; port_index++){
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_ports], &ports_db_entry, port_index);
        SOCDNX_IF_ERR_EXIT(rv);
        if(ports_db_entry < 0){
            continue;
        }
        for( i = 0 ; i < sizeof(mac_loopbacks)/sizeof(mac_loopbacks[0]); i++){
            SOCDNX_IF_ERR_EXIT(dnx_fabric_port_loopback_get(unit, ports_db_entry, pm_info, mac_loopbacks[i], &lb_enable));
            if(lb_enable){
                force_signal_detect = 1;
                break;
            }
        }
    }
    SOCDNX_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, fmac_blk_id_masked, &reg_val));
    soc_reg_field_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &reg_val, FORCE_SIGNAL_DETECTf, force_signal_detect);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, fmac_blk_id_masked, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int dnx_fabric_mac_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    uint32 reg_val = 0;
    uint32 field[1] = {0};
    uint32 fmac_blk_id_masked = pm_info->pm_data.dnx_fabric->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    uint32 phy_index, bitmap;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));

    if (SOC_REG_IS_VALID(unit, FMAC_MACT_PM_CREDIT_FILTERr)){
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FMAC_MACT_PM_CREDIT_FILTERr, fmac_blk_id_masked, phy_index, MACT_N_PM_CREDIT_FILTERf, enable));
    }

    switch(loopback_type){
    case portmodLoopbackMacOuter:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id_masked, &reg_val));
        *field = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
        if(enable) {
            SHR_BITSET(field, phy_index);
        } else {
            SHR_BITCLR(field, phy_index);
        }
        soc_reg_field_set(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, &reg_val, LCL_LPBK_ONf, *field);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id_masked, reg_val));
        if (enable)
        {
            LOG_WARN(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "Warning: MAC outer loopback is supported over rates 20.625 and higher\n")));

        }
        break;

    case portmodLoopbackMacAsyncFifo:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_CORE_LOOPBACKf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, reg_val));

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_id_masked, phy_index, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CNTRL_BURST_PERIODf, enable ? 0xc : 8);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_id_masked, phy_index, reg_val));
        break;

    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
        soc_reg_field_set(unit, FMAC_KPCS_CONFIGURATIONr, &reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, reg_val));

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_id_masked, phy_index, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, &reg_val, FMAL_N_CNTRL_BURST_PERIODf, enable ? 0xd : 8);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, fmac_blk_id_masked, phy_index, reg_val));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("shouldnot reach here")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

 
int dnx_fabric_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int is_phy_loopback = FALSE;
    int lb_enable = 0;
    uint32 encoding_properties;
    portmod_port_pcs_t encoding = 0;
    uint32 flags;
    int enabled;
    SOCDNX_INIT_FUNC_DEFS;

    /*Get phy and mac reset state*/
    flags = 0;
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_get(unit, port, pm_info, flags, &enabled));

    /*Disable phy and mac*/
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_set(unit, port, pm_info, flags, 0));    

    /* loopback type validation*/
    switch(loopback_type){
    case portmodLoopbackMacAsyncFifo:
    case portmodLoopbackMacOuter:
        break;
    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_encoding_get(unit, port, pm_info, &encoding_properties ,&encoding));
        if((encoding != PORTMOD_PCS_64B66B_FEC) && (encoding != PORTMOD_PCS_64B66B)){
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported encoding type %d for MAC PCS loopback"), encoding));
        }
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
        is_phy_loopback = TRUE;
        break;
    /*PHY PCS modes and MAC core are not supported*/
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("unsupported loopback type %d"), loopback_type));
    }

    /*check if not already defined*/
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_loopback_get(unit, port, pm_info, loopback_type, &lb_enable));
    if(enable == lb_enable){
        SOC_EXIT;
    }

    /*loopback set*/
    if(is_phy_loopback){
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_loopback_set(unit, port, pm_info,loopback_type, enable));
    } else { /*MAC looopback*/
        SOCDNX_IF_ERR_EXIT(dnx_fabric_mac_loopback_set(unit, port, pm_info, loopback_type, enable));
    }

    SOCDNX_IF_ERR_EXIT(_dnx_fabric_force_signal_detect_set(unit, pm_info));

    /*restore MAC and PHY */
    flags = 0;
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_set(unit, port, pm_info, flags, enabled));
exit:
    SOCDNX_FUNC_RETURN; 
}


int dnx_fabric_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    uint32 reg_val = 0;
    uint32 field[1] = {0};
    uint32 fmac_blk_id_masked = pm_info->pm_data.dnx_fabric->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK;
    uint32 phy_index = 0, bitmap;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    switch(loopback_type){
    case portmodLoopbackMacAsyncFifo:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
        *enable = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg_val, FMAL_N_CORE_LOOPBACKf);
        break;
    case portmodLoopbackMacOuter:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_LOOPBACK_ENABLE_REGISTERr(unit, fmac_blk_id_masked, &reg_val));
        *field = soc_reg_field_get(unit, FMAC_LOOPBACK_ENABLE_REGISTERr, reg_val, LCL_LPBK_ONf);
        *enable = SHR_BITGET(field, phy_index);
        break;
    case portmodLoopbackMacPCS:
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_CONFIGURATIONr(unit, fmac_blk_id_masked, phy_index, &reg_val));
        *enable = soc_reg_field_get(unit, FMAC_KPCS_CONFIGURATIONr, reg_val, KPCS_N_RX_DSC_LOOPBACK_ENf);
        break;
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD: /*slide*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_loopback_get(unit, port, pm_info, loopback_type, enable));
        break;
    /*PHY pcs modes and MAC core are not supported*/
    default:
        *enable = 0; /*not supported - no loopback*/
    }
exit:
    SOCDNX_FUNC_RETURN; 
}


int dnx_fabric_port_link_get(int unit, int port, pm_info_t pm_info, int flag, int* link)
{
    int enable,
        chain_length = 0,
        rv;
    uint32 phy_index, bitmap;
    uint32 
        is_down=0,
        reg_val = 0,
        fld_val[1] = {0},
        rx_pmd_locked;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    phymod_phy_access_t phy_access[1];
    SOCDNX_INIT_FUNC_DEFS;

    /*link is down unless we find it's up*/
    *link = 0;

    /*check if port is enabled*/
    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_enable_get(unit, port, pm_info, 0, &enable));

    if (enable) {
        /*clear rxlos interrupt*/
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        reg_val = 0;
        SHR_BITSET(fld_val, phy_index);
        soc_reg_field_set(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, &reg_val, RX_LOST_OF_SYNCf, *fld_val);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit,  fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, reg_val));

        /*check if Serdes is locked*/
        SOCDNX_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access, 1, &chain_length));
        rv = phymod_phy_rx_pmd_locked_get(phy_access, &rx_pmd_locked);
        if (rv != PHYMOD_E_NONE) {
            return rv;
        }

        if(rx_pmd_locked == 1)
        {
            /*check if FMAC is synced - check rxlos interrupt*/
            SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAC_INTERRUPT_REGISTER_2r(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, &reg_val));

            *fld_val = soc_reg_field_get(unit, FMAC_FMAC_INTERRUPT_REGISTER_2r, reg_val, RX_LOST_OF_SYNCf);
            is_down = SHR_BITGET(fld_val, phy_index);
            *link = (is_down == 0);
        }
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}

int dnx_fabric_port_prbs_config_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;
    uint32 poly_val = 0;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_config_set(unit, port, pm_info, flags, config));
    }
    else{
        switch(config->poly){
        case phymodPrbsPoly31:
            poly_val = 1;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported polynomial for MAC PRBS %d"), config->poly));
        }
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, &reg_val, KPCS_N_TST_RX_PTRN_SELf, poly_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, reg_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, &reg_val, KPCS_N_TST_TX_PTRN_SELf, poly_val);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, reg_val));
        }
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}

int dnx_fabric_port_prbs_config_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{
    uint32 reg_val;
    uint32 tx_poly;
    uint32 poly_val = 0; 
    uint32 phy_index, bitmap;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_config_get(unit, port, pm_info, flags, config));
    }
    else{
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
            poly_val = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_PTRN_SELf);
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
            tx_poly = soc_reg_field_get(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, reg_val, KPCS_N_TST_TX_PTRN_SELf);
            if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
                if(poly_val != tx_poly){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("rx and tx are not configured the same")));
                }
            }
            poly_val = tx_poly;
        }
        switch(poly_val){
        case 1:
            config->poly = phymodPrbsPoly31;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unknown polynomial %u"), poly_val));
        }
        /*no meaning for invert in MAC PRBS*/
        config->invert = 0;
    }

exit:
    SOCDNX_FUNC_RETURN; 
    
}


int dnx_fabric_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int enable)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_enable_set(unit, port, pm_info, flags, enable));
    }
    else{
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, &reg_val, KPCS_N_TST_RX_ENf, enable ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, reg_val));
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
            soc_reg_field_set(unit, FMAC_KPCS_TEST_TX_CONFIGURATIONr, &reg_val, KPCS_N_TST_TX_ENf, enable ? 1 : 0);
            SOCDNX_IF_ERR_EXIT(WRITE_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, reg_val));
        }
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}

int dnx_fabric_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int* enable)
{
    uint32 reg_val;
    int tx_enable;
    uint32 phy_index, bitmap;
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_enable_get(unit, port, pm_info, flags, enable));
    }
    else{
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
            *enable = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_ENf);
        }
        if(PHYMOD_PRBS_DIRECTION_TX_GET(flags) || (flags == 0)){
            SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_TX_CONFIGURATIONr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
            tx_enable = soc_reg_field_get(unit, FMAC_KPCS_TEST_RX_CONFIGURATIONr, reg_val, KPCS_N_TST_RX_ENf);
            /*validate rx and tx are configured the same*/
            if(PHYMOD_PRBS_DIRECTION_RX_GET(flags) || (flags == 0)){
                if(*enable != tx_enable){
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("rx and tx are not configured the same")));
                }
            }
            *enable = tx_enable;
        }
    }        
exit:
    SOCDNX_FUNC_RETURN;
}


int dnx_fabric_port_prbs_status_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    uint32 reg_val;
    uint32 phy_index, bitmap;  
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    SOCDNX_INIT_FUNC_DEFS;

    if(mode == 0){ /*phy*/
        SOCDNX_IF_ERR_EXIT(portmod_common_phy_prbs_status_get(unit, port, pm_info, flags, status));
    } else {
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        SOCDNX_IF_ERR_EXIT(READ_FMAC_KPCS_TEST_RX_STATUSr(unit, fabric_data->fmac_schan_id | SOC_REG_ADDR_SCHAN_ID_MASK, phy_index, &reg_val));
        status->prbs_lock = (reg_val >>  DNX_FABRIC_MAC_PRBS_LOCK_SHIFT ) & DNX_FABRIC_MAC_PRBS_LOCK_MASK;
        status->prbs_lock_loss = 0; /*Not supported*/
        if (status->prbs_lock) {
            status->error_count = (reg_val >>  DNX_FABRIC_MAC_PRBS_CTR_SHIFT ) & DNX_FABRIC_MAC_PRBS_CTR_MASK;
        } else {
            status->error_count = -1;
        }
    }
exit:
    SOCDNX_FUNC_RETURN; 
    
}

int
dnx_fabric_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params, int max_phys, 
                                    phymod_phy_access_t* phy_access_arr, int* nof_phys, int* is_most_ext)
{
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;

    #ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
        dnx_fabric_internal_t* fabric_o_nif_data = &(pm_info->pm_data.dnx_fabric->fabric_o_nif_data);
    #endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT*/
    portmod_ext_phy_core_info_t core_info;
    int core_index, lane_index;
    int phyn, rv;
    uint32 phy_index, is_bypass, bitmap;
    SOCDNX_INIT_FUNC_DEFS;

    #ifdef PORTMOD_DNX_FABRIC_O_NIF_SUPPORT
        if (pm_info->type == portmodDispatchTypeDnx_fabric_o_nif) {
            rv = __portmod__dispatch__[fabric_o_nif_data->pms[0]->type]->f_portmod_port_phy_lane_access_get(unit, port,
                                                                                         fabric_o_nif_data->pms[0], params, max_phys, phy_access_arr, nof_phys, is_most_ext);
            return rv;
        }
    #endif /*PORTMOD_DNX_FABRIC_O_NIF_SUPPORT*/

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &is_bypass);
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
    if(max_phys < 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("array should be at size 1 at least")));
    }
    if(params->lane != 0 && params->lane != -1 && !is_bypass){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("dnx fabric ports are single lane only")));
    }
    
    phyn = params->phyn;
    if((0 > phyn) || (phyn > (fabric_data->nof_phys - 1))){
        phyn = fabric_data->nof_phys - 1;
    }
    
    SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access_arr[0]));

    *nof_phys = 1;

    if (is_bypass) {
        SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access_arr[0]));
        sal_memcpy(&phy_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access)); 
        if (params->lane == PORTMOD_ALL_LANES_ARE_ACTIVE) {
            phy_access_arr[0].access.lane_mask = 0xf & bitmap;
        } else {
            phy_access_arr[0].access.lane_mask = 0xf & (1 << (params->lane + phy_index)); 
            if (phy_access_arr[0].access.lane_mask == 0) {
                *nof_phys = 0;
            }
        }
        phy_access_arr[0].type = fabric_data->core_type;
    } else if (0 == phyn) {
        SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access_arr[0]));
        sal_memcpy(&phy_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access)); 
        SHR_BITSET(&phy_access_arr[0].access.lane_mask, phy_index);
        phy_access_arr[0].type = fabric_data->core_type;
    }
    else{
        core_index = fabric_data->phy_chain[phyn - 1][phy_index].core_index;
        lane_index = fabric_data->phy_chain[phyn - 1][phy_index].lane_index;
        portmod_phychain_ext_phy_info_get(unit, phyn, core_index, &core_info);
        SOCDNX_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access_arr[0]));
        sal_memcpy(&phy_access_arr[0].access, &core_info.core_access.access, sizeof(phymod_access_t));
        SHR_BITSET(&phy_access_arr[0].access.lane_mask, lane_index);
        phy_access_arr[0].type = core_info.core_access.type;
    }
    if(is_most_ext) {
        *is_most_ext = (phyn == (fabric_data->nof_phys - 1)) ? 1 : 0;
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


int dnx_fabric_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    SOCDNX_INIT_FUNC_DEFS;
    /*Do nothing*/
    SOCDNX_FUNC_RETURN;
}

int
dnx_fabric_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, 
                                phymod_core_access_t* core_access_arr, int* cores_num, int* is_most_ext)
{
    dnx_fabric_t fabric_data = pm_info->pm_data.dnx_fabric;
    portmod_ext_phy_core_info_t core_info;
    uint32 phy_index, bitmap;
    int core_index;
    SOCDNX_INIT_FUNC_DEFS;

    if(max_cores < 1){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("array should be at size 1 at least")));
    }
    if((0 > phyn) || (phyn > (fabric_data->nof_phys - 1))){
        phyn = fabric_data->nof_phys - 1;
    }
    
    SOCDNX_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));
    if(0 == phyn){
        sal_memcpy(&core_access_arr[0].access, &fabric_data->access, sizeof(fabric_data->access));
        core_access_arr[0].type = fabric_data->core_type;
        *cores_num = 1;
    }
    else{
        SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_index_get(unit, port, pm_info, &phy_index, &bitmap));
        core_index = fabric_data->phy_chain[phyn - 1][phy_index].core_index;
        portmod_phychain_ext_phy_info_get(unit, phyn, core_index, &core_info);
        sal_memcpy(&core_access_arr[0], &core_info.core_access, sizeof(phymod_core_access_t));
    }
    *cores_num = 1;
    if(is_most_ext) {
        *is_most_ext = (phyn == (fabric_data->nof_phys - 1)) ? 1 : 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_ext_phy_lane_attach_to_pm (int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_lane_connection_t* lane_connection)
{   portmod_lane_connection_t *candidate_lane_connection = NULL;
    int lane_index = 0, phy_index = 0;
    int rv = 0;
    uint32 is_core_initialized = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /* Making sure the port macro is not initialized */
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized);
    SOCDNX_IF_ERR_EXIT(rv);
    if(is_core_initialized){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("External Phy cannot be attached from a phy chain of an initialized port macro")));
    }
    /* Validate parameters */
    if (((1 + MAX_PHYN) <= phyn) || (0 >= phyn)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("phyn is out of the range of allowed external phys")));
    }
    for(lane_index = 0; lane_index < DNX_FABRIC_LANES_PER_CORE; lane_index++){
        candidate_lane_connection = &(pm_info->pm_data.dnx_fabric->phy_chain[phyn - 1][lane_index]);
        if(PORTMOD_LANE_INDEX_INVALID != candidate_lane_connection->lane_index){
            break;
        }
    }
    if(DNX_FABRIC_LANES_PER_CORE == lane_index){
        pm_info->pm_data.dnx_fabric->nof_phys++;
    }    
    lane_index = 0;
    PORTMOD_PBMP_ITER(pm_info->pm_data.dnx_fabric->phys, phy_index) {
        if (phy_index == iphy) {
            break;
        }
        lane_index++;
    }
    candidate_lane_connection = &(pm_info->pm_data.dnx_fabric->phy_chain[phyn - 1][lane_index]);
    if(NULL != lane_connection){
        sal_memcpy(candidate_lane_connection, lane_connection , sizeof(portmod_lane_connection_t));
    }
exit:
   SOCDNX_FUNC_RETURN;
}

int dnx_fabric_ext_phy_lane_detach_from_pm (int unit, pm_info_t pm_info, int iphy, int phyn, portmod_lane_connection_t* lane_connection)
{  
    portmod_lane_connection_t *candidate_lane_connection = NULL;
    int lane_index = 0, phy_index = 0;
    int rv = 0;
    uint32 is_core_initialized = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /* Making sure the port macro is not initialized */
    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_initialized], &is_core_initialized);
    SOCDNX_IF_ERR_EXIT(rv);
    if(is_core_initialized){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("External Phy cannot be detached from a phy chain of an initialized port macro")));
    }
    /* Validate parameters */
    if(((pm_info->pm_data.dnx_fabric->nof_phys - 1) < phyn) || (0 >= phyn)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("phyn is out of the range of allowed external phys")));
    }
    PORTMOD_PBMP_ITER(pm_info->pm_data.dnx_fabric->phys, phy_index){
        if(phy_index == iphy){
            break;
        }
        lane_index++;
    }
    candidate_lane_connection = &(pm_info->pm_data.dnx_fabric->phy_chain[phyn - 1][lane_index]);
    if(NULL != lane_connection){
        sal_memcpy(lane_connection, candidate_lane_connection , sizeof(portmod_lane_connection_t));
    }
    SOCDNX_IF_ERR_EXIT(portmod_lane_connection_t_init(unit, candidate_lane_connection));
    for(lane_index = 0; lane_index < DNX_FABRIC_LANES_PER_CORE; lane_index++){
        candidate_lane_connection = &(pm_info->pm_data.dnx_fabric->phy_chain[phyn - 1][lane_index]);
        if(PORTMOD_LANE_INDEX_INVALID != candidate_lane_connection->lane_index){
            break;
        }
    }
    if(DNX_FABRIC_LANES_PER_CORE == lane_index){
        pm_info->pm_data.dnx_fabric->nof_phys--;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_mode_set (int unit, soc_port_t port, 
                     pm_info_t pm_info, const portmod_port_mode_info_t *mode)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("mode set isn't supported")));

exit:
    SOCDNX_FUNC_RETURN;  
}

int dnx_fabric_port_mode_get (int unit, soc_port_t port, 
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    SOCDNX_INIT_FUNC_DEFS;

    mode->lanes = 1;
    mode->cur_mode = portmodPortModeSingle;
    
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_cl72_set(int unit, int port, pm_info_t pm_info, uint32 enable)
{
    phymod_phy_access_t phy_access[1];
    int chain_length = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,
                                                        1, &chain_length));
    SOCDNX_IF_ERR_EXIT(portmod_port_phychain_cl72_set(unit, port, phy_access, chain_length, enable));

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_cl72_get(int unit, int port, pm_info_t pm_info, uint32* enable)
{
    phymod_phy_access_t phy_access[1];
    int chain_length = 0;
    uint32_t tmp_enable;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_port_chain_phy_access_get(unit, port, pm_info, phy_access,
                                                        1, &chain_length));
    SOCDNX_IF_ERR_EXIT(portmod_port_phychain_cl72_get(unit, phy_access, chain_length, &tmp_enable));
    *enable = tmp_enable;

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,uint32 inst, int op_type, 
    int op_cmd, const void *arg) 
{
    phymod_phy_access_t phy_access[1];
    int nof_phys;
    portmod_access_get_params_t params;
    phymod_tx_t  ln_txparam[PHYMOD_MAX_LANES_PER_CORE];
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(portmod_access_get_params_t_init(unit, &params));  
    if( PHY_DIAG_INST_DEV(inst) == PHY_DIAG_DEV_INT ) {
        params.phyn = 0 ;
    } else { /* most external is default */  
        params.phyn = -1 ;  
    } 

    if( PHY_DIAG_INST_INTF(inst) == PHY_DIAG_INTF_SYS ) {
        params.sys_side = PORTMOD_SIDE_SYSTEM;
    } else { /* most external is default */
        params.sys_side = PORTMOD_SIDE_LINE ;
    }

    for (i = 0; i < PHYMOD_MAX_LANES_PER_CORE; i++) {
        SOC_IF_ERROR_RETURN(phymod_tx_t_init(&ln_txparam[i]));
    }

    switch(op_cmd) {
        case PHY_DIAG_CTRL_DSC:
            LOG_INFO(BSL_LS_SOC_PHY,(BSL_META_U(unit, 
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"), unit, 
                port, PHY_DIAG_CTRL_DSC));
            SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(phymod_phy_pmd_info_dump(phy_access, (void*)arg));
            }
            break;

        case PHY_DIAG_CTRL_PCS:
            LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_PCS 0x%x\n"), unit, 
                port, PHY_DIAG_CTRL_PCS));
            SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(phymod_phy_pcs_info_dump(phy_access, (void*)arg));
            }
            break;

        case PHY_DIAG_CTRL_LINKMON_MODE:
            LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_LINKMON_MODE  0x%x\n"), unit, port, 
                PHY_DIAG_CTRL_LINKMON_MODE));
            SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(portmod_pm_phy_link_mon_enable_set(phy_access, nof_phys, PTR_TO_INT(arg)));
            }
            break;

       case PHY_DIAG_CTRL_LINKMON_STATUS:
            LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_LINKMON_STATUS 0x%x\n"), unit, 
                port, PHY_DIAG_CTRL_LINKMON_STATUS));
            SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            if((*phy_access).access.lane_mask){
                SOC_IF_ERROR_RETURN(portmod_pm_phy_link_mon_status_get(phy_access, nof_phys));
            }
            break;

       default:
            SOCDNX_IF_ERR_EXIT(dnx_fabric_port_phy_lane_access_get(unit, port, pm_info, &params, 1, 
                phy_access, &nof_phys, NULL));
            
            if(op_type == PHY_DIAG_CTRL_SET) {
                LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                    "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_SET 0x%x\n"),
                    unit, port, PHY_DIAG_CTRL_SET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !((*phy_access).access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_set(phy_access, nof_phys, 
                            op_cmd, ln_txparam, (*phy_access).access.lane_mask, PTR_TO_INT(arg)));
                    }
                }
            } else if(op_type == PHY_DIAG_CTRL_GET) {
                LOG_INFO(BSL_LS_SOC_PHY, (BSL_META_U(unit,
                    "portmod_phy_port_diag_ctrl: u=%d p=%d PHY_DIAG_CTRL_GET 0x%x\n"),
                    unit, port, PHY_DIAG_CTRL_GET));
                if (!SAL_BOOT_SIMULATION) {
                    if( !((*phy_access).access.lane_mask == 0)){
                        SOC_IF_ERROR_RETURN(portmod_pm_phy_control_get(phy_access,nof_phys,
                            op_cmd, ln_txparam, (*phy_access).access.lane_mask, (uint32 *)arg));
                    } else {
                        *(uint32 *)arg = 0;
                    }
                }
            } else {
                SOC_IF_ERROR_RETURN(SOC_E_UNAVAIL);
            }

            break;
    }

exit:
    SOCDNX_FUNC_RETURN;

}

int dnx_fabric_pm_bypass_set(int unit, pm_info_t pm_info, int bypass_enable)
{
    int     rv = 0;
    uint32  pm_is_active = 0, bypass_enable_get;

    SOCDNX_INIT_FUNC_DEFS;

    rv = DNX_FABRIC_IS_BYPASSED_GET(unit, pm_info, &bypass_enable_get);
    SOCDNX_IF_ERR_EXIT(rv);

    if (bypass_enable_get != bypass_enable) {
        rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_is_probed], &pm_is_active);
        SOCDNX_IF_ERR_EXIT(rv);

        if(pm_is_active) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("can't chenge bypass mode for active pm")));
        }

        rv = DNX_FABRIC_IS_BYPASSED_SET(unit, pm_info, bypass_enable);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_lane_map_set(int unit, int port, pm_info_t pm_info, uint32 flags, const phymod_lane_map_t* lane_map)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_SET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane_map], lane_map);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_lane_map_get(int unit, int port, pm_info_t pm_info, uint32 flags,phymod_lane_map_t* lane_map)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = SOC_WB_ENGINE_GET_VAR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_lane_map], lane_map);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_polarity_set(int unit, int port, pm_info_t pm_info, const phymod_polarity_t* polarity)
{
    int rv, i, rx_polarity, tx_polarity;
    SOCDNX_INIT_FUNC_DEFS;

    for (i = 0; i < DNX_FABRIC_LANES_PER_CORE; i++) {
        rx_polarity = (polarity->rx_polarity >> i) & 0x1;
        tx_polarity = (polarity->tx_polarity >> i) & 0x1;

        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_polarity_get(int unit, int port, pm_info_t pm_info, phymod_polarity_t* polarity)
{
    int rv, i, rx_polarity, tx_polarity;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(phymod_polarity_t_init(polarity));

    for (i = 0; i < DNX_FABRIC_LANES_PER_CORE; i++) {
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_rx], &rx_polarity, i); 
        SOCDNX_IF_ERR_EXIT(rv);
        rv = SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[wb_polarity_tx], &tx_polarity, i);
        SOCDNX_IF_ERR_EXIT(rv);

        polarity->rx_polarity |= rx_polarity << i; 
        polarity->tx_polarity |= tx_polarity << i; 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32* value)
{
    phymod_phy_access_t phy_access[1];
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access , 1,
                                           &nof_phys));
    SOCDNX_IF_ERR_EXIT
        (portmod_port_phychain_reg_read(unit, port, phy_access,
                                        nof_phys, lane, flags, reg_addr,value));
exit:
    SOCDNX_FUNC_RETURN;
}

int dnx_fabric_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32 value)
{
    phymod_phy_access_t phy_access[1];
    int nof_phys;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT
        (portmod_port_chain_phy_access_get(unit, port, pm_info,
                                           phy_access , 1,
                                           &nof_phys));
    SOCDNX_IF_ERR_EXIT
        (portmod_port_phychain_reg_write(unit, port, phy_access,
                                         nof_phys, lane, flags, reg_addr,value));
exit:
    SOCDNX_FUNC_RETURN;
}

#endif /* PORTMOD_DNX_FABRIC_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

