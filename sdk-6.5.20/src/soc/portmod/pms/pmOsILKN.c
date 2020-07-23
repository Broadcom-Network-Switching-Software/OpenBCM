/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/wb_engine.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>
#include <soc/portmod/portmod_common.h>
#include <soc/mcm/memregs.h>
        
#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM_OS_ILKN_SUPPORT

#include <soc/portmod/pmOsILKN.h>
#include <soc/portmod/pmOsILKN_shared.h>
#define OS_ILKN_WB_BUFFER_VERSION                   (3)
#define OS_ILKN_LANES_PER_CORE_TWO_ACTIVE_PORTS     (12)
#define OS_ILKN_LANES_PER_PHY_CORE                  (4)
#define OS_ILKN_BURST_32B_VAL                       (0)
#define OS_ILKN_BURST_64B_VAL                       (1)
#define OS_ILKN_BURST_256B_VAL                      (3)
#define OS_ILKN_BURST_128B_VAL                      (2)
#define OS_ILKN_DEFAULT_METAFRAME_SYNC_PERIOD       (2048)
#define OS_ILKN_THREE_LANE_MAP_SIZE                 (3)
#define OS_ILKN_CORE_CLOCK_720000KHZ                (720000)
#define OS_ILKN_CORE_CLOCK_600000KHZ                (600000)

#define OS_ILKN_IS_PORT_OVER_FABRIC_GET(port, pm_info, is_port_over_fabric)                                       \
    do {                                                                                                          \
        int core_id;                                                                              \
        OS_ILKN_CORE_ID_GET(port, pm_info, core_id);                                                              \
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_GET(unit, pm_info, &is_port_over_fabric, core_id));          \
    } while(0)

#define OS_ILKN_NOF_PIPES_MAX(revision)  (OS_ILKN_IS_REVISION_8(revision) ? 24 : 15)

int pmOsILKN_pm_interface_type_is_supported(int unit, soc_port_if_t interface, int* is_supported)
{
    SOC_INIT_FUNC_DEFS;

    switch(interface){
    case SOC_PORT_IF_ILKN:
        *is_supported = TRUE;
        break;
    default:
        *is_supported = FALSE;
    }

    SOC_FUNC_RETURN;
}

/*
 * Initialize the buffer to support warmboot
 * The state of warmboot is store in two arrays: IlknData and ports.. etc.,
 * These variables need to be added to warmboot.
 * Any variables added to save the state of warmboot should be included here.
 */
STATIC
int pmOsIlkn_wb_buffer_init(int unit, int wb_buffer_index, pm_info_t pm_info)
{
    /* Declare the common variables needed for warmboot */
    WB_ENGINE_INIT_TABLES_DEFS;
    int wb_var_id, rv;
    int buffer_id = wb_buffer_index; /*required by SOC_WB_ENGINE_ADD_ Macros*/
    SOC_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(buffer_is_dynamic);

    SOC_WB_ENGINE_ADD_BUFF(SOC_WB_ENGINE_PORTMOD, wb_buffer_index, "pmOsIlkn", NULL, NULL, OS_ILKN_WB_BUFFER_VERSION, 1, SOC_WB_ENGINE_POST_RELEASE);
    _SOC_IF_ERR_EXIT(rv);

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ports", wb_buffer_index, sizeof(soc_port_t), NULL, OS_ILKN_MAX_NOF_INTERNAL_PORTS, VERSION(1));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ports] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "isPortOverFabric", wb_buffer_index, sizeof(int), NULL, OS_ILKN_MAX_NOF_INTERNAL_PORTS, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isPortOverFabric] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "physAligned", wb_buffer_index, sizeof(soc_pbmp_t), NULL, OS_ILKN_MAX_NOF_INTERNAL_PORTS, VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[physAligned] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "is_pm_aggregated", wb_buffer_index, sizeof(int), NULL, (OS_ILKN_MAX_NOF_INTERNAL_PORTS*OS_ILKN_MAX_ILKN_AGGREGATED_PMS), VERSION(2));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[isPmAggregated] = wb_var_id;

    /* Deleting ilknData WB structures while still supporting ISSU from ILKN WB ver 1 to ILKN WB ver 2*/
    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_VAR_WITH_FEATURES(SOC_WB_ENGINE_PORTMOD, wb_var_id, "ilkn_data", wb_buffer_index, sizeof(pmOsIlkn_internal_t), NULL, 1, OS_ILKN_MAX_NOF_INTERNAL_PORTS, 0xffffffff, 0xffffffff, VERSION(1), VERSION(2), NULL);
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[ilknData] = wb_var_id;

    _SOC_IF_ERR_EXIT(portmod_next_wb_var_id_get(unit, &wb_var_id));
    SOC_WB_ENGINE_ADD_ARR(SOC_WB_ENGINE_PORTMOD, wb_var_id, "aggregated_pm_ids", wb_buffer_index, sizeof(int), NULL, (OS_ILKN_MAX_NOF_INTERNAL_PORTS*OS_ILKN_MAX_ILKN_AGGREGATED_PMS), VERSION(3));
    _SOC_IF_ERR_EXIT(rv);
    pm_info->wb_vars_ids[AggregatedPmIds] = wb_var_id;


    _SOC_IF_ERR_EXIT(soc_wb_engine_init_buffer(unit, SOC_WB_ENGINE_PORTMOD, wb_buffer_index, FALSE));

exit:
    SOC_FUNC_RETURN;
}


/* Allocate a new pm_info and initialize it with information from pm_add_info */
int pmOsILKN_pm_init(int unit, const portmod_pm_create_info_internal_t* pm_add_info, int wb_buffer_index, pm_info_t pm_info)
{
    const portmod_os_ilkn_create_info_internal_t *info = &pm_add_info->pm_specific_info.os_ilkn;
    pmOsIlkn_t pmOsIlkn_data = NULL;
    int i, j;
    int temp_enable; /* -1 = not_applicable, 0 = disable, 1 = enable */
    int is_pm_aggregated;
    int pm_id, port_temp;
    portmod_pbmp_t port_phys;
    pm_info_t pm_info_temp;

    SOC_INIT_FUNC_DEFS;

    pm_info->unit = unit;
    pm_info->type = pm_add_info->type;
    pm_info->wb_buffer_id = wb_buffer_index;

    /*init wb buffer*/
    _SOC_IF_ERR_EXIT(pmOsIlkn_wb_buffer_init(unit, wb_buffer_index,  pm_info));


    pmOsIlkn_data = sal_alloc(sizeof(struct pmOsIlkn_s), "os_ilkn_specific_db");
    SOC_NULL_CHECK(pmOsIlkn_data);
    sal_memset(pmOsIlkn_data, 0, sizeof(struct pmOsIlkn_s));

    pm_info->pm_data.pmOsIlkn_db = pmOsIlkn_data;

    pmOsIlkn_data->fec_disable_by_bypass = info->fec_disable_by_bypass;
    if (info->ilkn_port_lane_map_get) {
        pmOsIlkn_data->ilkn_port_lane_map_get = info->ilkn_port_lane_map_get;
    }
    if (info->ilkn_pm_lane_map_get) {
        pmOsIlkn_data->ilkn_pm_lane_map_get = info->ilkn_pm_lane_map_get;
    }
    if (info->ilkn_port_fec_units_set) {
        pmOsIlkn_data->ilkn_port_fec_units_set = info->ilkn_port_fec_units_set;
    }


    for (j = 0; j < OS_ILKN_MAX_NOF_INTERNAL_PORTS; ++j) {
        PORTMOD_PBMP_ASSIGN(pmOsIlkn_data->ilkn_data[j].phys, pm_add_info->phys);

        pmOsIlkn_data->ilkn_data[j].pms = sal_alloc(sizeof(pm_info_t) * OS_ILKN_MAX_ILKN_AGGREGATED_PMS, "controlled_pms"); 
        SOC_NULL_CHECK(pmOsIlkn_data->ilkn_data[j].pms);

        pmOsIlkn_data->ilkn_data[j].pm_ids = sal_alloc(sizeof(int) * OS_ILKN_MAX_ILKN_AGGREGATED_PMS, "controlled_pm_ids");
        SOC_NULL_CHECK(pmOsIlkn_data->ilkn_data[j].pm_ids);

        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            pmOsIlkn_data->ilkn_data[j].pms[i] = info->pms[i];
            pmOsIlkn_data->ilkn_data[j].pm_ids[i] = info->pm_ids[i];

            if(!SOC_WARM_BOOT(unit)){
            	temp_enable = 0;
                _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, temp_enable, j, i));
            }
        }
        
        pmOsIlkn_data->ilkn_data[j].wm_high = info->wm_high[j];
        pmOsIlkn_data->ilkn_data[j].wm_low = info->wm_low[j];


        pmOsIlkn_data->ilkn_data[j].core_clk_khz = info->core_clock_khz;
        if(!SOC_WARM_BOOT(unit)){
        	temp_enable = -1; /* -1 = not_applicable */
            _SOC_IF_ERR_EXIT(PM_ILKN_PORT_SET(unit, pm_info, temp_enable, j));
            temp_enable = 0;
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, temp_enable, j));
        }

        /* In case of dynamic PM association restore PM info and port phys from WB SW-state */
        if(SOC_WARM_BOOT(unit)){
            _SOC_IF_ERR_EXIT(PM_ILKN_PORT_GET(unit, pm_info, &port_temp, j));
            if (port_temp >= 0)
            {
                if  (info->nof_aggregated_pms == 0)
                {
                    _SOC_IF_ERR_EXIT(portmod_port_phys_get(unit, port_temp, &port_phys));
                    PORTMOD_PBMP_ASSIGN(pmOsIlkn_data->ilkn_data[j].phys, port_phys);

                    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
                        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, j, i));
                        if (is_pm_aggregated)
                        {
                            _SOC_IF_ERR_EXIT(PM_ILKN_AGGREGATED_PM_ID_GET(unit, pm_info, &pm_id, j, i));
                            pmOsIlkn_data->ilkn_data[j].pm_ids[i] = pm_id;
                            _SOC_IF_ERR_EXIT(portmod_pm_info_from_pm_id_get(unit, pm_id, &pm_info_temp));
                            pmOsIlkn_data->ilkn_data[j].pms[i] = pm_info_temp;
                        }
                    }
                }
            }
        }

    }

exit:
    if(SOC_FUNC_ERROR){
        pmOsILKN_pm_destroy(unit, pm_info);
    }
    SOC_FUNC_RETURN;
}

/* Deallocate the pm_info struct- release resources */
int pmOsILKN_pm_destroy(int unit, pm_info_t pm_info)
{
    int i;
    SOC_INIT_FUNC_DEFS;
    
    if(pm_info->pm_data.pmOsIlkn_db != NULL){
        for (i = 0; i < OS_ILKN_MAX_NOF_INTERNAL_PORTS; ++i) {

            if (pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pms != NULL) {
                sal_free(pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pms);
                pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pms = NULL;
            }

            if (pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pm_ids != NULL) {
                sal_free(pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pm_ids);
                pm_info->pm_data.pmOsIlkn_db->ilkn_data[i].pm_ids = NULL;
            }

        }
        sal_free(pm_info->pm_data.pmOsIlkn_db); 
        pm_info->pm_data.pmOsIlkn_db = NULL;
    }

    SOC_FUNC_RETURN;
}

int pmOsILKN_pm_core_info_get(int unit, pm_info_t pm_info, int phyn, portmod_pm_core_info_t* core_info)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

CONST soc_reg_t remap_lanes_rx_regs[] = {
    ILKN_SLE_RX_REMAP_LANE_4_0r, 
    ILKN_SLE_RX_REMAP_LANE_9_5r, 
    ILKN_SLE_RX_REMAP_LANE_14_10r, 
    ILKN_SLE_RX_REMAP_LANE_19_15r, 
    ILKN_SLE_RX_REMAP_LANE_24_20r
};

CONST soc_reg_t remap_lanes_tx_regs[] = {
    ILKN_SLE_TX_REMAP_LANE_4_0r, 
    ILKN_SLE_TX_REMAP_LANE_9_5r, 
    ILKN_SLE_TX_REMAP_LANE_14_10r, 
    ILKN_SLE_TX_REMAP_LANE_19_15r, 
    ILKN_SLE_TX_REMAP_LANE_24_20r
};

CONST soc_field_t remap_lanes_rx_fields[] = {
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_0f,
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_1f,
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_2f,
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_3f,
    SLE_RX_REMAP_LANE_4_0_REMAP_LANE_4f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_5f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_6f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_7f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_8f,
    SLE_RX_REMAP_LANE_9_5_REMAP_LANE_9f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_10f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_11f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_12f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_13f,
    SLE_RX_REMAP_LANE_14_10_REMAP_LANE_14f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_15f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_16f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_17f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_18f,
    SLE_RX_REMAP_LANE_19_15_REMAP_LANE_19f,
    SLE_RX_REMAP_LANE_24_20_REMAP_LANE_20f,
    SLE_RX_REMAP_LANE_24_20_REMAP_LANE_21f,
    SLE_RX_REMAP_LANE_24_20_REMAP_LANE_22f,
    SLE_RX_REMAP_LANE_24_20_REMAP_LANE_23f
};

CONST soc_field_t remap_lanes_tx_fields[] = {
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_0f,
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_1f,
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_2f,
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_3f,
    SLE_TX_REMAP_LANE_4_0_REMAP_LANE_4f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_5f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_6f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_7f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_8f,
    SLE_TX_REMAP_LANE_9_5_REMAP_LANE_9f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_10f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_11f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_12f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_13f,
    SLE_TX_REMAP_LANE_14_10_REMAP_LANE_14f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_15f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_16f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_17f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_18f,
    SLE_TX_REMAP_LANE_19_15_REMAP_LANE_19f,
    SLE_TX_REMAP_LANE_24_20_REMAP_LANE_20f,
    SLE_TX_REMAP_LANE_24_20_REMAP_LANE_21f,
    SLE_TX_REMAP_LANE_24_20_REMAP_LANE_22f,
    SLE_TX_REMAP_LANE_24_20_REMAP_LANE_23f
};


CONST STATIC soc_field_t enable_pipes_tx_fields[] = {
    SLE_TX_PIPE_ENABLE_PIPE_0f,
    SLE_TX_PIPE_ENABLE_PIPE_1f,
    SLE_TX_PIPE_ENABLE_PIPE_2f,
    SLE_TX_PIPE_ENABLE_PIPE_3f,
    SLE_TX_PIPE_ENABLE_PIPE_4f,
    SLE_TX_PIPE_ENABLE_PIPE_5f,
    SLE_TX_PIPE_ENABLE_PIPE_6f,
    SLE_TX_PIPE_ENABLE_PIPE_7f,
    SLE_TX_PIPE_ENABLE_PIPE_8f,
    SLE_TX_PIPE_ENABLE_PIPE_9f,
    SLE_TX_PIPE_ENABLE_PIPE_10f,
    SLE_TX_PIPE_ENABLE_PIPE_11f,
    SLE_TX_PIPE_ENABLE_PIPE_12f,
    SLE_TX_PIPE_ENABLE_PIPE_13f,
    SLE_TX_PIPE_ENABLE_PIPE_14f,
    SLE_TX_PIPE_ENABLE_PIPE_15f,
    SLE_TX_PIPE_ENABLE_PIPE_16f,
    SLE_TX_PIPE_ENABLE_PIPE_17f,
    SLE_TX_PIPE_ENABLE_PIPE_18f,
    SLE_TX_PIPE_ENABLE_PIPE_19f,
    SLE_TX_PIPE_ENABLE_PIPE_20f,
    SLE_TX_PIPE_ENABLE_PIPE_21f,
    SLE_TX_PIPE_ENABLE_PIPE_22f,
    SLE_TX_PIPE_ENABLE_PIPE_23f
};

CONST STATIC soc_field_t enable_pipes_rx_fields[] = {
    SLE_RX_PIPE_ENABLE_PIPE_0f,
    SLE_RX_PIPE_ENABLE_PIPE_1f,
    SLE_RX_PIPE_ENABLE_PIPE_2f,
    SLE_RX_PIPE_ENABLE_PIPE_3f,
    SLE_RX_PIPE_ENABLE_PIPE_4f,
    SLE_RX_PIPE_ENABLE_PIPE_5f,
    SLE_RX_PIPE_ENABLE_PIPE_6f,
    SLE_RX_PIPE_ENABLE_PIPE_7f,
    SLE_RX_PIPE_ENABLE_PIPE_8f,
    SLE_RX_PIPE_ENABLE_PIPE_9f,
    SLE_RX_PIPE_ENABLE_PIPE_10f,
    SLE_RX_PIPE_ENABLE_PIPE_11f,
    SLE_RX_PIPE_ENABLE_PIPE_12f,
    SLE_RX_PIPE_ENABLE_PIPE_13f,
    SLE_RX_PIPE_ENABLE_PIPE_14f,
    SLE_RX_PIPE_ENABLE_PIPE_15f,
    SLE_RX_PIPE_ENABLE_PIPE_16f,
    SLE_RX_PIPE_ENABLE_PIPE_17f,
    SLE_RX_PIPE_ENABLE_PIPE_18f,
    SLE_RX_PIPE_ENABLE_PIPE_19f,
    SLE_RX_PIPE_ENABLE_PIPE_20f,
    SLE_RX_PIPE_ENABLE_PIPE_21f,
    SLE_RX_PIPE_ENABLE_PIPE_22f,
    SLE_RX_PIPE_ENABLE_PIPE_23f
};

STATIC int
_pmOsILKN_lane_map_override_set(uint32 pm_lane_mask, int core_id, phymod_lane_map_t *core_lane_map) {

    int i, optimize, missing = -1, temp, num_lanes = 0;
    uint32 remap_lane = 0;
    int lane_map[OS_ILKN_THREE_LANE_MAP_SIZE] = {0};
    int rev_lane_map[OS_ILKN_THREE_LANE_MAP_SIZE] = {0};
    int mapped_lanes[OS_ILKN_THREE_LANE_MAP_SIZE] = {0};
    int *lane_map_ptr = NULL;
    SOC_INIT_FUNC_DEFS;

    optimize = 1; 
   
    /*optimization - all lanes are mapped to enabled lanes*/
    for (i = 0; i < OS_ILKN_LANES_PER_PHY_CORE; ++i) {
        if ((pm_lane_mask & (1 << i))== 0) {
            /* lane is not enabled -> move on to the next */
            continue;
        }
        remap_lane = core_lane_map->lane_map_rx[i];
        if (((pm_lane_mask) & (1 << remap_lane)) == 0) {
            optimize = 0;
            break;
        }
    }
    if (optimize) {
        SOC_EXIT;
    }

    switch (pm_lane_mask) {
    case 0x1:
    case 0x2:
    case 0x4:
    case 0x8:
        num_lanes = 1;
        break;
    case 0x3:
        lane_map[0]     = 0; lane_map[1]     = 1;
        rev_lane_map[0] = 2; rev_lane_map[1] = 3;
        num_lanes = 2;
        break;
    case 0x6:
        lane_map[0]     = 1; lane_map[1]     = 2;
        rev_lane_map[0] = 1; rev_lane_map[1] = 2;
        num_lanes = 2;
        break;
    case 0xc:
        lane_map[0]     = 2; lane_map[1]     = 3;
        rev_lane_map[0] = 0; rev_lane_map[1] = 1;
        num_lanes = 2;
        break;
    case 0x5:
        lane_map[0]     = 0; lane_map[1]     = 2;
        rev_lane_map[0] = 1; rev_lane_map[1] = 3;
        num_lanes = 2;
        break;
    case 0x9:
        lane_map[0]     = 0; lane_map[1]     = 3;
        rev_lane_map[0] = 0; rev_lane_map[1] = 3;
        num_lanes = 2;
        break;
    case 0xa:
        lane_map[0]     = 1; lane_map[1]     = 3;
        rev_lane_map[0] = 0; rev_lane_map[1] = 2;
        num_lanes = 2;
        break;
    case 0x7:
        lane_map[0]     = 0; lane_map[1]     = 1; lane_map[2]     = 2;
        rev_lane_map[0] = 1; rev_lane_map[1] = 2; rev_lane_map[2] = 3;
        missing = 3;
        num_lanes = 3;
        break;
    case 0xb:
        lane_map[0]     = 0; lane_map[1]     = 1; lane_map[2]     = 3;
        rev_lane_map[0] = 0; rev_lane_map[1] = 2; rev_lane_map[2] = 3;
        missing = 2;
        num_lanes = 3;
        break;
    case 0xd:
        lane_map[0]     = 0; lane_map[1]     = 2; lane_map[2]     = 3;
        rev_lane_map[0] = 0; rev_lane_map[1] = 1; rev_lane_map[2] = 3;
        missing = 1;
        num_lanes = 3;
        break;
    case 0xe:
        lane_map[0]     = 1; lane_map[1]     = 2; lane_map[2]     = 3;
        rev_lane_map[0] = 0; rev_lane_map[1] = 1; rev_lane_map[2] = 2;
        missing = 0;
        num_lanes = 3;
        break;
    default:
        SOC_EXIT;
    }

    for (i = 0; i < num_lanes; ++i) {
        mapped_lanes[i] = core_lane_map->lane_map_rx[core_id ? rev_lane_map[i] : lane_map[i]]; 
    }
    lane_map_ptr = core_id ? rev_lane_map : lane_map;
    if (num_lanes == 3) {
             
        for (i = 0; i < num_lanes; ++i) {
            if ((i < (num_lanes - 1) && mapped_lanes[i] > mapped_lanes[(i + 1)%OS_ILKN_THREE_LANE_MAP_SIZE]) || 
                ((i == (num_lanes - 1)) && mapped_lanes[i] < mapped_lanes[(i + 1)%OS_ILKN_THREE_LANE_MAP_SIZE])) {
                temp = lane_map_ptr[i];
                lane_map_ptr[i] = lane_map_ptr[(i + 1)%OS_ILKN_THREE_LANE_MAP_SIZE];
                lane_map_ptr[(i + 1)%OS_ILKN_THREE_LANE_MAP_SIZE] = temp;
            }
        }
        for (i = 0; i < OS_ILKN_LANES_PER_PHY_CORE; i++) {
            if (i == missing) {
                core_lane_map->lane_map_rx[i] = missing;
            } else if (i < missing) {
                core_lane_map->lane_map_rx[i] = lane_map_ptr[i]; 
            } else if (i > missing) {
                core_lane_map->lane_map_rx[i] = lane_map_ptr[i - 1]; 
            }
        }
    } else if (num_lanes == 2) {
        if (mapped_lanes[0] < mapped_lanes[1]) {
            core_lane_map->lane_map_rx[lane_map_ptr[0]] = lane_map_ptr[0];
            core_lane_map->lane_map_rx[lane_map_ptr[1]] = lane_map_ptr[1];
        } else {
            core_lane_map->lane_map_rx[lane_map_ptr[0]] = lane_map_ptr[1];
            core_lane_map->lane_map_rx[lane_map_ptr[1]] = lane_map_ptr[0];
        }
    } else if (num_lanes == 1) {
        /*one lane optimization - map each lane to itself*/
        for (i = 0; i < OS_ILKN_LANES_PER_PHY_CORE; ++i) {
            core_lane_map->lane_map_rx[i] = i;
        }
    }

exit:
    SOC_FUNC_RETURN;
}


STATIC int 
pmOsILKN_port_logical_lane_order_init(int unit, int port, pm_info_t pm_info)
{
    portmod_access_get_params_t params;
    phymod_lane_map_t pm_lane_map;
    pmOsIlkn_internal_t* ilkn_data;
    int core_id, phy_count, start, end;
    int rv, pm, lane, i, temp, rev_pm, lane_index, reverse_index, swapped_lane_index, shift, is_pm_aggregated=0;
    int reverse_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
    int lane_order[OS_ILKN_TOTAL_LANES_PER_CORE] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
    uint32 disabled_lanes[1], reg_val, lane_mask, pm_mask, unused_lanes;
    uint32 flags = 0;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_LANEr(unit, port, &reg_val)); 
    disabled_lanes[0] = soc_reg_field_get(unit, ILKN_SLE_RX_LANEr, reg_val, SLE_RX_LANE_DISABLEf);

    if (core_id) {
        /*Create reverse order lanes for "small" ilkn*/
        start = 0; 
        end = OS_ILKN_TOTAL_LANES_PER_CORE - 1;
        for (i = 0; i < OS_ILKN_LANES_PER_CORE_TWO_ACTIVE_PORTS / 2; ++i) {
            while (SHR_BITGET(disabled_lanes, start)) {
                ++start;
            }
            while (SHR_BITGET(disabled_lanes, end)) {
                --end;
            }
            temp = reverse_lane_order[start];
            reverse_lane_order[start] = reverse_lane_order[end];
            reverse_lane_order[end] = temp;
            ++start;
            --end;
            if (start >= end) {
                break;
            }
        }
    }
    /*Get RX lane map per PM*/
    for (pm = 0; pm < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; pm++)
    {
        rev_pm = OS_ILKN_MAX_ILKN_AGGREGATED_PMS - pm - 1;
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, pm));
        if (is_pm_aggregated) {

            /*get PM lane map*/
            rv = __portmod__dispatch__[ilkn_data->pms[pm]->type]->f_portmod_port_lane_map_get(unit, port, ilkn_data->pms[pm], flags, &pm_lane_map);
            _SOC_IF_ERR_EXIT(rv);

            /* if only part of the quad is used in the port, 
            need to override the lane_map so no lane would point to non-existing lane */
            shift = (core_id ? rev_pm : pm) * OS_ILKN_LANES_PER_PHY_CORE;
            pm_mask = 0xf << shift;
            unused_lanes = pm_mask & disabled_lanes[0];
            if (unused_lanes) {
                lane_mask = ~(unused_lanes >> shift) & 0xf;
                _SOC_IF_ERR_EXIT(_pmOsILKN_lane_map_override_set(lane_mask, core_id, &pm_lane_map));
            }

            /*Copy to RX lane map to ilkn_lane_order*/
            for (lane = 0; lane < OS_ILKN_LANES_PER_PHY_CORE; lane++)
            {
                lane_index = pm * OS_ILKN_LANES_PER_PHY_CORE + lane;
                if (core_id) {
                    if (SHR_BITGET(disabled_lanes, OS_ILKN_TOTAL_LANES_PER_CORE - lane_index - 1) == 0) {
                        swapped_lane_index = (rev_pm * OS_ILKN_LANES_PER_PHY_CORE) + OS_ILKN_LANES_PER_PHY_CORE - 1 - pm_lane_map.lane_map_rx[lane];
                        reverse_index = reverse_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE - lane_index - 1]; 
                        lane_order[reverse_index] = swapped_lane_index;
                    }
                } else {
                    if (SHR_BITGET(disabled_lanes, lane_index) == 0) {
                        lane_order[(pm)*OS_ILKN_LANES_PER_PHY_CORE + lane] = pm_lane_map.lane_map_rx[lane] + (pm)*OS_ILKN_LANES_PER_PHY_CORE;
                    }
                }
            }
        }
    }

    /*Set lane map*/
    PORTMOD_PBMP_COUNT(ilkn_data->phys, phy_count);
    _SOC_IF_ERR_EXIT(pmOsILKN_port_logical_lane_order_set(unit, port, pm_info, lane_order, phy_count));


exit:
    SOC_FUNC_RETURN;
}

STATIC
int _pmOsILKN_port_num_pipes_get(int unit, int port, pm_info_t pm_info, uint32* num_pipes)
{
    pmOsIlkn_internal_t* ilkn_data;
    portmod_port_interface_config_t config;
    int nof_lanes, serdes_speed=-1, core_id, i, is_pm_aggregated=0;
    uint32 core_clk_khz;

    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]); 
    core_clk_khz = ilkn_data->core_clk_khz;

    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    _SOC_IF_ERR_EXIT(portmod_port_interface_config_t_init(unit, &config));

    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(
                    __portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_interface_config_get(unit, port,
                            ilkn_data->pms[i], &config,
                            PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
            break;
        }
    }

    serdes_speed = config.speed;

    /* pipe number formula by OS */
    *num_pipes = PORTMOD_DIV_ROUND_UP( (10 * serdes_speed * nof_lanes ) , ( (core_clk_khz / 100) * 67) );

    if (*num_pipes > OS_ILKN_SLE_MAX_NOF_PIPES)
    {
        _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("pipe number exceeded limit (%d)"), *num_pipes));
    }

    exit:
    SOC_FUNC_RETURN;
}


/* in ILKN, PCS is in Bypass mode,                           *
 * so we need to override the polarity to the actual lanes.  *
 * for Eth polarity is on swapped lanes                      */
STATIC
int pmOsILKN_port_polarity_override_set(int unit, int port, pm_info_t pm_info) {

    pmOsIlkn_internal_t* ilkn_data;
    phymod_polarity_t polarity, orig_polarity, new_polarity;
    phymod_lane_map_t pm_lane_map;
    phymod_phy_access_t phy_access;
    portmod_access_get_params_t params;
    int core_id, i, mapped_lane, nof_phys, lane, is_pm_aggregated=0;
    uint32 flags = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = -1;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(phymod_lane_map_t_init(&pm_lane_map));
            _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&phy_access));
            _SOC_IF_ERR_EXIT(phymod_polarity_t_init(&polarity));
            _SOC_IF_ERR_EXIT(phymod_polarity_t_init(&orig_polarity));
            _SOC_IF_ERR_EXIT(phymod_polarity_t_init(&new_polarity));

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_lane_map_get(unit, port, ilkn_data->pms[i], flags, &pm_lane_map));
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_polarity_get(unit, port, ilkn_data->pms[i], &orig_polarity)); 
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_phy_lane_access_get(unit, port, ilkn_data->pms[i], &params, 1, &phy_access, &nof_phys, NULL));

            for (lane = 0; lane < OS_ILKN_LANES_PER_PHY_CORE; ++lane) {
                if (orig_polarity.rx_polarity & (1 << lane)) {
                    mapped_lane = pm_lane_map.lane_map_rx[lane];
                    new_polarity.rx_polarity |= (1 << mapped_lane);
                }
                if (orig_polarity.tx_polarity & (1 << lane)) {
                    mapped_lane = pm_lane_map.lane_map_rx[lane];
                    new_polarity.tx_polarity |= (1 << mapped_lane);

                }
            }
            for (lane = 0; lane < OS_ILKN_LANES_PER_PHY_CORE; ++lane) {
                phy_access.access.lane_mask = 1 << lane;
                polarity.rx_polarity = (new_polarity.rx_polarity & (1 << lane)) ? 1 : 0;
                polarity.tx_polarity = (new_polarity.tx_polarity & (1 << lane)) ? 1 : 0;
                _SOC_IF_ERR_EXIT(phymod_phy_polarity_set(&phy_access, &polarity));
            }
        }
    }
exit:
    SOC_FUNC_RETURN;
}

/* This function is not relevant for 4x25 port macros, for 8x50 the corresponding pmOsILKN_50G_pm_aggregated_update is called*/
int pmOsILKN_pm_aggregated_update(int unit,  pm_info_t pm_info, int core_id, int nof_aggregated_pms,  const pm_info_t* pms, const int* pm_ids)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pmOsILKN_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{
    pmOsIlkn_internal_t* ilkn_data;
    uint32 reg_val, bmp, burst_short_val, burst_max_val, data_pack, ilkn_core_id, burst_min_val;
    portmod_pbmp_t mirror_bmp, phys_temp;
    soc_port_t last_lane;
    portmod_port_add_info_t add_info_copy;
    int nof_lanes, nof_lanes_temp, index, i, last_lane_pos, count, lane, first_pm, metaframe_period;
    int is_pm_aggregated=0;
    int is_other_pm_aggregated=0;
    int temp_enable;
    uint32 nof_pipes = 0;
    SOC_INIT_FUNC_DEFS;

    ilkn_core_id = add_info->ilkn_core_id;
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[ilkn_core_id]);

    _SOC_IF_ERR_EXIT(PM_ILKN_PORT_SET(unit, pm_info, port, ilkn_core_id));
    _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, add_info->ilkn_port_is_over_fabric, ilkn_core_id));

    PORTMOD_PBMP_ASSIGN(ilkn_data->phys, add_info->phys);
    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    for (i=0 ; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS ; i++)
    {
        if (ilkn_data->pms[i])
        {
            PORTMOD_PBMP_CLEAR(phys_temp);
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_pm_phys_get(unit, ilkn_data->pms[i], &phys_temp));
            PORTMOD_PBMP_AND(phys_temp, ilkn_data->phys);
            PORTMOD_PBMP_COUNT(phys_temp, nof_lanes_temp);
            if (nof_lanes_temp > 0)
            {
                temp_enable = 1;
            	_SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, temp_enable, ilkn_core_id, i));
            }
        }

    }
    
    /**** Configure OpenSilicon ILKN core ****/
    
    /* Lane Disable */
    /* if ILKN core 1,3,5, mirror the bitmap */
    PORTMOD_PBMP_CLEAR(mirror_bmp);

    PORTMOD_PBMP_ITER(add_info->ilkn_lanes, lane)
    {
        index = (ilkn_core_id) ? OS_ILKN_TOTAL_LANES_PER_CORE - 1 - lane: lane;
        PORTMOD_PBMP_PORT_ADD(mirror_bmp, index);
    }

    /*penetrating aligned phys to WB engine*/
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_SET(unit, pm_info, add_info->ilkn_lanes, ilkn_core_id));


    bmp = SOC_PBMP_WORD_GET(mirror_bmp, 0);

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_LANEr, &reg_val, SLE_TX_LANE_DISABLEf, ~bmp);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_LANEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_LANEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_LANEr, &reg_val, SLE_RX_LANE_DISABLEf, ~bmp);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_LANEr(unit, port, reg_val));
    
    sal_usleep(1000);

    /* get last lane for stall selection */
    PORTMOD_PBMP_COUNT(mirror_bmp, count);
    last_lane_pos = (ilkn_core_id) ? 0 : count - 1;
    i = 0;
    PORTMOD_PBMP_ITER(mirror_bmp, last_lane){
        if (i == last_lane_pos) {
            break; 
        }
        ++i;
    }
    /* prevent skew probles between lanes */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr(unit, port, &reg_val));
    /* selected lane should be one of the enabled lanes */
    soc_reg_field_set(unit, ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr, &reg_val, SLE_TX_SERDES_AFIFO_STALL_SEL_LANEf, last_lane); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr, &reg_val, SLE_TX_SERDES_AFIFO_STALL_SEL_ENf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_SERDES_AFIFO_STALL_SELr(unit, port, reg_val));

    /** Common regs **/
    /* SLE default values depend on LA-mode (on/off). just to make sure, we set these regs to 0, than configure fields. */
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, 0));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_USERf, add_info->rx_retransmit ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_MEM_WAITf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_USERf, add_info->tx_retransmit ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_MEM_WAITf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_RETRANSMIT_CONFIGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_RETRANSMIT_CONFIGr, &reg_val, SLE_RX_RETRANSMIT_CONFIG_RETRANSMIT_ENABLEf, add_info->rx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_RETRANSMIT_CONFIGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_RETRANSMIT_CONFIGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_RETRANSMIT_CONFIGr, &reg_val, SLE_TX_RETRANSMIT_CONFIG_RETRANSMIT_ENABLEf, add_info->tx_retransmit ? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_RETRANSMIT_CONFIGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr, &reg_val, SLE_RX_FCOB_RETRANSMIT_SLOT_RETRANSREQ_IDf, 
                                                    add_info->reserved_channel_rx);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_FCOB_RETRANSMIT_SLOTr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr, &reg_val, SLE_TX_FCOB_RETRANSMIT_SLOT_RETRANSREQ_IDf, 
    		                                        add_info->reserved_channel_tx);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_FCOB_RETRANSMIT_SLOTr(unit, port, reg_val));

    /* metaframe period */
    metaframe_period = (add_info->ilkn_metaframe_period != -1) ? add_info->ilkn_metaframe_period : OS_ILKN_DEFAULT_METAFRAME_SYNC_PERIOD;
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_METAFRAMEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_METAFRAMEr, &reg_val, SLE_TX_METAFRAME_PERIODf, metaframe_period);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_METAFRAMEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_METAFRAMEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_METAFRAMEr, &reg_val, SLE_RX_METAFRAME_PERIODf, metaframe_period);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_METAFRAMEr(unit, port, reg_val));

    /* ilkn segment enable */
    /* val =  (ports 1,3,5 || ELK || two_active_cores) ? 0x3 : 0xf; */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_0f, 1);
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_1f, 1);
    if (add_info->ilkn_nof_segments > 2) {
        soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_2f, 1); 
        soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_3f, 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_0f, 1);
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_1f, 1);
    if (add_info->ilkn_nof_segments > 2) {
        soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_2f, 1); 
        soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_3f, 1);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, reg_val));

    /* set watermarks, different between PMH/PML */
    /* hard coded: PMH cores should be 14/15 to H/L accordingly, PML core should be 9/10 */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_FIFO_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_FIFO_CFGr, &reg_val, SLE_TX_FIFO_CFG_WM_HIGHf, ilkn_data->wm_high);
    soc_reg_field_set(unit, ILKN_SLE_TX_FIFO_CFGr, &reg_val, SLE_TX_FIFO_CFG_WM_LOWf, ilkn_data->wm_low);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_FIFO_CFGr(unit, port, reg_val));
    
    /* set_ilkn_cfg_inband, set_ilkn_cfg_outband, set_ilkn_cfg_mult_ext*/
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_INBANDf, 
        ((add_info->ilkn_inb_cal_len_tx>=1) ? 1 : 0));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_OUTBANDf, 1);
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_MULT_EXTf, add_info->tx_retransmit ? 0 : 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_INBANDf, 
        ((add_info->ilkn_inb_cal_len_rx>=1) ? 1 : 0));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_OUTBANDf, 1);
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_MULT_EXTf, add_info->rx_retransmit ? 0 : 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    /* set_ilkn_cal_inband_last */    
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CAL_INBANDr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CAL_INBANDr, &reg_val, SLE_TX_CAL_INBAND_LASTf, 
        ((add_info->ilkn_inb_cal_len_tx-1 >= 0) ? (add_info->ilkn_inb_cal_len_tx-1) : 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CAL_INBANDr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CAL_INBANDr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CAL_INBANDr, &reg_val, SLE_RX_CAL_INBAND_LASTf, 
        ((add_info->ilkn_inb_cal_len_rx-1 >= 0) ? (add_info->ilkn_inb_cal_len_rx-1) : 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CAL_INBANDr(unit, port, reg_val));
    
    /* set_ilkn_cal_outband_last */ /*configure to num of channels - 1*/
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CAL_OUTBANDr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CAL_OUTBANDr, &reg_val, SLE_TX_CAL_OUTBAND_LASTf, 
        ((add_info->ilkn_oob_cal_len_tx-1 >= 0) ? (add_info->ilkn_oob_cal_len_tx-1) : 0) );
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CAL_OUTBANDr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CAL_OUTBANDr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_CAL_OUTBANDr, &reg_val, SLE_RX_CAL_OUTBAND_LASTf, 
        ((add_info->ilkn_oob_cal_len_rx-1 >= 0) ? (add_info->ilkn_oob_cal_len_rx-1) : 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CAL_OUTBANDr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_OUTBANDf,
        ((add_info->ilkn_oob_cal_len_tx>=1) ? 1 : 0) );
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_OUTBANDf, 
        ((add_info->ilkn_oob_cal_len_rx>=1) ? 1 : 0));
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

    /* When 1, data pack block is enabled, by default this block is disabled. In the Rx, this bit has no function */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_DATA_PCKf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    /* reset all SLE counters */
    for (i = 0; i < OS_ILKN_MAX_NOF_CHANNELS; ++i) {
        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_CMDf, 0x4);
        soc_reg_field_set(unit, ILKN_SLE_RX_STATS_ACCr, &reg_val, SLE_RX_STATS_ACC_ADDRf, i);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_STATS_ACCr(unit, port, reg_val));

        _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_STATS_ACCr(unit, port, &reg_val));
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_TYPEf, 0x0);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_CMDf, 0x4);
        soc_reg_field_set(unit, ILKN_SLE_TX_STATS_ACCr, &reg_val, SLE_TX_STATS_ACC_ADDRf, i);
        _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_STATS_ACCr(unit, port, reg_val));
        
        sal_usleep(1000);
    }

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CTLr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CTLr, &reg_val, SLE_RX_CTL_UNALIGN_XOFF_DISf, 1);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CTLr(unit, port, reg_val));

    /* call PMs */
    sal_memcpy(&add_info_copy, add_info, sizeof(portmod_port_add_info_t));
    first_pm = -1;
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, ilkn_core_id, i));
        if (is_pm_aggregated) {
            first_pm = (first_pm == -1) ? i : first_pm;
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_pm_bypass_set(unit, ilkn_data->pms[i], 1)); 

            OS_ILKN_IS_PM_SHARED_GET(port, pm_info, ilkn_core_id, i, &is_other_pm_aggregated);
            if (is_other_pm_aggregated){
                add_info_copy.interface_config.flags |= PHYMOD_INTF_F_DONT_TURN_OFF_PLL;
                LOG_WARN(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "pm %d of port %d is shared with other active ILKN port. Therefore, activating this shared pm without setting its speed!\n"),
                                i, port));
            }
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_attach(unit, port, ilkn_data->pms[i], &add_info_copy));
            if (is_other_pm_aggregated){
                add_info_copy.interface_config.flags &= (~PHYMOD_INTF_F_DONT_TURN_OFF_PLL);
            }
        }
    }

    /*when interface is 400G or over, burst short should be 64B instaed of 32B*/

    if (add_info->ilkn_burst_short != -1) {
        int i = 0;
        int temp_val = add_info->ilkn_burst_short;
        while (temp_val > 32) {
            temp_val /= 2;
            ++i;
        }
        burst_short_val = i;
    } else if (nof_lanes > 15) {
        portmod_port_interface_config_t config;
        _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[first_pm]->type]->f_portmod_port_interface_config_get(unit, port, ilkn_data->pms[first_pm], 
                                                                                                                    &config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY)); 
        burst_short_val = (config.speed == 25000) ? OS_ILKN_BURST_64B_VAL : OS_ILKN_BURST_32B_VAL;
    } else {
        burst_short_val = OS_ILKN_BURST_32B_VAL;
    }

    burst_min_val = add_info->ilkn_burst_min / 32 - 1;

    burst_max_val = (add_info->ilkn_burst_max == 128) ? OS_ILKN_BURST_128B_VAL : OS_ILKN_BURST_256B_VAL;
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_BURSTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_SHORTf, burst_short_val);
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_MINf, burst_min_val);
    soc_reg_field_set(unit, ILKN_SLE_RX_BURSTr, &reg_val, SLE_RX_BURST_MAXf, burst_max_val);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_BURSTr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_BURSTr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_SHORTf, burst_short_val);
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_MINf, burst_min_val);
    soc_reg_field_set(unit, ILKN_SLE_TX_BURSTr, &reg_val, SLE_TX_BURST_MAXf, burst_max_val);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_BURSTr(unit, port, reg_val));

    data_pack = (PORTMOD_PORT_ADD_F_ELK_GET(add_info)) ? 1 : 0;
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_DATA_PCKf, data_pack);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    /*polarity override*/
    _SOC_IF_ERR_EXIT(pmOsILKN_port_polarity_override_set(unit, port, pm_info));

    /* pipe enable */
    /* enable num_lanes pipes, or all pipes */
    _SOC_IF_ERR_EXIT(_pmOsILKN_port_num_pipes_get(unit, port, pm_info, &nof_pipes));
    _SOC_IF_ERR_EXIT(pmOsILKN_port_pipe_config(unit, port, pm_info, nof_pipes));

    /* ILKN remap configuration */
    _SOC_IF_ERR_EXIT(pmOsILKN_port_logical_lane_order_init(unit, port, pm_info)); 

exit:
    SOC_FUNC_RETURN;
}

int
pmOsILKN_port_pipe_config(int unit, int port, pm_info_t pm_info, uint32 nof_pipes)
{
    uint32 reg_val;
    int i;
    pmOsIlkn_revision_t revision;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(pmOsILKN_revision_get(unit, port, &revision));
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_PIPE_ENABLEr(unit, port, &reg_val));
    for (i = 0; i < nof_pipes; ++i) {
        soc_reg_field_set(unit, ILKN_SLE_TX_PIPE_ENABLEr, &reg_val, enable_pipes_tx_fields[i], 1);
    }
    for (i = nof_pipes; i < OS_ILKN_NOF_PIPES_MAX(revision) ; ++i) {
        soc_reg_field_set(unit, ILKN_SLE_TX_PIPE_ENABLEr, &reg_val, enable_pipes_tx_fields[i], 0);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_PIPE_ENABLEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_PIPE_ENABLEr(unit, port, &reg_val));
    for (i = 0; i < nof_pipes ; ++i) {
        soc_reg_field_set(unit, ILKN_SLE_RX_PIPE_ENABLEr, &reg_val, enable_pipes_rx_fields[i], 1);
    }
    for (i = nof_pipes; i < OS_ILKN_NOF_PIPES_MAX(revision); ++i) {
        soc_reg_field_set(unit, ILKN_SLE_RX_PIPE_ENABLEr, &reg_val, enable_pipes_rx_fields[i], 0);
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_PIPE_ENABLEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int 
pmOsILKN_port_logical_lane_order_get(int unit, int port, pm_info_t pm_info, int lane_order_max_size, int* lane_order, int* lane_order_actual_size)
{
    int lane_index;
    int actual_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE];
    int core_id;
    int port_i, nof_lanes;
    int field_index, reg_index;
    uint32 reg_val;
    soc_pbmp_t temp_phys_aligned;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &temp_phys_aligned, core_id));

    sal_memset(actual_lane_order, 0, sizeof(int) * OS_ILKN_TOTAL_LANES_PER_CORE);
    lane_index = 0;
    PORTMOD_PBMP_ITER(temp_phys_aligned, port_i){
        /*reverse_i = (!reverse_i) ? (((port_i / OS_ILKN_TOTAL_LANES_PER_CORE) + 1) * OS_ILKN_TOTAL_LANES_PER_CORE) : reverse_i ;*/

        /*calc relevant reg and field*/
        field_index = lane_index % OS_ILKN_TOTAL_LANES_PER_CORE;
        if (field_index > OS_ILKN_TOTAL_LANES_PER_CORE - 1) { /* coverity - protect from reading out of bounds */
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("field index %d is out of bounds"), field_index));
        }
        reg_index = (lane_index % OS_ILKN_TOTAL_LANES_PER_CORE)/ 5;

        /*configure rx remap*/
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[reg_index], port, 0, &reg_val));
        actual_lane_order[lane_index] = soc_reg_field_get(unit, remap_lanes_rx_regs[reg_index], reg_val, remap_lanes_rx_fields[field_index]);

        ++lane_index;
    }

    _SOC_IF_ERR_EXIT(pmOsILKN_port_nof_lanes_get(unit, port, pm_info, &nof_lanes));
    *lane_order_actual_size = lane_order_max_size < nof_lanes ? lane_order_max_size : nof_lanes;

    if (core_id)
    {
        /*Convert to reverse order for "small" ilkn*/
        for (lane_index = 0;  lane_index < *lane_order_actual_size; lane_index++)
        {
            lane_order[lane_index] = actual_lane_order[OS_ILKN_TOTAL_LANES_PER_CORE - lane_index - 1];
        }
    } else {
        for (lane_index = 0;  lane_index < *lane_order_actual_size; lane_index++)
        {
            lane_order[lane_index] = actual_lane_order[lane_index];
        }
    }


exit:
    SOC_FUNC_RETURN;
}

int 
pmOsILKN_port_logical_lane_order_set(int unit, int port, pm_info_t pm_info, const int* lane_order, int lane_order_size)
{
    int lane_index;
    int core_id;
    int port_i;
    int field_index, reg_index;
    uint32 reg_val;
    soc_pbmp_t temp_phys_aligned;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &temp_phys_aligned, core_id));
    
    PORTMOD_PBMP_ITER(temp_phys_aligned, port_i){
        lane_index = port_i;

        /*calc relevant reg and field*/
        field_index = (core_id) ? OS_ILKN_TOTAL_LANES_PER_CORE - lane_index - 1 : lane_index;
        if (field_index > OS_ILKN_TOTAL_LANES_PER_CORE - 1) { /* coverity - protect from reading out of bounds */
            _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("field index %d is out of bounds"), field_index));
        }
        reg_index = field_index / 5; 

        /*configure rx remap*/
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_rx_regs[reg_index], port, 0, &reg_val));
        soc_reg_field_set(unit, remap_lanes_rx_regs[reg_index], &reg_val, remap_lanes_rx_fields[field_index], lane_order[field_index]);
        _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_rx_regs[reg_index], port, 0, reg_val));

        /*configure tx remap - should be identical to rx*/
        _SOC_IF_ERR_EXIT(soc_custom_reg32_get(unit, remap_lanes_tx_regs[reg_index], port, 0, &reg_val));
        soc_reg_field_set(unit, remap_lanes_tx_regs[reg_index], &reg_val, remap_lanes_tx_fields[field_index], lane_order[field_index]);
        _SOC_IF_ERR_EXIT(soc_custom_reg32_set(unit, remap_lanes_tx_regs[reg_index], port, 0, reg_val));

        lane_index++;
    }

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_detach(int unit, int port, pm_info_t pm_info)
{
    int enable, i, core_id, is_pm_aggregated=0;
    int temp_enable; /* -1 = not_applicable, 0 = disable, 1 = enable */
    pmOsIlkn_internal_t* ilkn_data;
    soc_pbmp_t temp_phys_aligned;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    _SOC_IF_ERR_EXIT(pmOsILKN_port_enable_get(unit, port, pm_info, 0, &enable));
    if (enable) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("can't detach active port %d"), port));
    }

    /* call PMs */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_detach(unit, port, ilkn_data->pms[i])); 
        }
        temp_enable = 0;
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, temp_enable, core_id, i));
    }

    /* Disable - clean db */
    SOC_PBMP_CLEAR(ilkn_data->phys);
    /*clearing WB engine*/
    SOC_PBMP_CLEAR(temp_phys_aligned);
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_SET(unit, pm_info, temp_phys_aligned, core_id));
    temp_enable = -1; /* -1 = not_applicable */
    _SOC_IF_ERR_EXIT(PM_ILKN_PORT_SET(unit, pm_info, temp_enable, core_id));
    temp_enable = 0;
    _SOC_IF_ERR_EXIT(PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, temp_enable, core_id));

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_replace(int unit, int port, pm_info_t pm_info, int new_port)
{
    int  i, core_id, rv, is_pm_aggregated=0;
    pmOsIlkn_internal_t* ilkn_data;
            
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    rv = PM_ILKN_PORT_SET(unit, pm_info, new_port, core_id);
    _SOC_IF_ERR_EXIT(rv);

    /* call PMs */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_replace(unit, port, ilkn_data->pms[i], new_port));
        }
    }


exit:
    SOC_FUNC_RETURN; 
    
}

int pmOsILKN_port_enable_set(int unit, int port, pm_info_t pm_info, int flags, int enable)
{
    int i, core_id, is_port_over_fabric, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    OS_ILKN_IS_PORT_OVER_FABRIC_GET(port, pm_info, is_port_over_fabric);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    if (!is_port_over_fabric) {

        /* When working over NIF: ILKN replaces the PM MAC,
         * and ilkn core is enabled/disabled through wrapper.
         * cannot configure MAC here
         * (however, when working over fabric this is legal since FMAC is equal to PM bypass interface) */

        if (PORTMOD_PORT_ENABLE_MAC_GET(flags) && !(PORTMOD_PORT_ENABLE_PHY_GET(flags))) {
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("MAC only configuration on ILKN is not supported.")));
        }

        /* clear MAC flag and set PHY flag before calling PMs below */
        PORTMOD_PORT_ENABLE_MAC_CLR(flags);
        PORTMOD_PORT_ENABLE_PHY_SET(flags);
    }

    /* call PMs */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_enable_set(unit, port, ilkn_data->pms[i], flags, enable));
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_enable_get(int unit, int port, pm_info_t pm_info, int flags, int* enable)
{
    int i, core_id, phy_en = 0, is_port_over_fabric, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    OS_ILKN_IS_PORT_OVER_FABRIC_GET(port, pm_info, is_port_over_fabric);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);


    if (!is_port_over_fabric) {
        /* ILKN replaces the PM MAC,
         * and ilkn core is enabled/disabled throgh wrapper.
         * cannot configure MAC here */
        if (PORTMOD_PORT_ENABLE_MAC_GET(flags) && !(PORTMOD_PORT_ENABLE_PHY_GET(flags))) {
            _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("MAC only status on ILKN is not supported.")));
        }
        /* clear MAC flag and set PHY flag before calling PMs below */
        PORTMOD_PORT_ENABLE_MAC_CLR(flags);
        PORTMOD_PORT_ENABLE_PHY_SET(flags);
    }

    /* call PMs */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_enable_get(unit, port, ilkn_data->pms[i], flags, &phy_en));
            break;
        }
    }
        
    *enable = phy_en; 
    
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_interface_config_set(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config,
                                        int phy_init_flags)
{
    int i, core_id, is_pm_aggregated=0;
    uint32 nof_pipes = 0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_interface_config_set(unit, port, ilkn_data->pms[i], config, phy_init_flags));
        }
    }

     /* pipe enable */
    /* enable num_lanes pipes, or all pipes */
    _SOC_IF_ERR_EXIT(_pmOsILKN_port_num_pipes_get(unit, port, pm_info, &nof_pipes));
    _SOC_IF_ERR_EXIT(pmOsILKN_port_pipe_config(unit, port, pm_info, nof_pipes));

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_interface_config_get(int unit, int port, pm_info_t pm_info, portmod_port_interface_config_t* config, int phy_init_flags)
{
    int core_id, index, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    for (index = 0; index < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++index) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, index));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[index]->type]->f_portmod_port_interface_config_get(unit, port, ilkn_data->pms[index], 
                                                                                                                     config, phy_init_flags));
            break;
        }
    }
    
    config->interface = SOC_PORT_IF_ILKN;

exit:
    SOC_FUNC_RETURN; 
}

int pmOsILKN_port_default_interface_get(int unit, int port, pm_info_t pm_info, const portmod_port_interface_config_t* config, soc_port_if_t* interface)
{
    *interface = SOC_PORT_IF_NULL;
    return SOC_E_NONE;
}

STATIC int _pmOsILKN_port_loopback_set(int unit, int port, int enable)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_CFGr, &reg_val, SLE_TX_CFG_DATA_TX_TO_RX_LBf, enable);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_CFGr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_RX_CFGr, &reg_val, SLE_RX_CFG_DATA_TX_TO_RX_LBf, enable);
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_CFGr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN; 
}

STATIC int _pmOsILKN_port_loopback_get(int unit, int port, int *enable)
{
    uint32 reg_val, is_tx, is_rx;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_CFGr(unit, port, &reg_val));
    is_tx = soc_reg_field_get(unit, ILKN_SLE_TX_CFGr, reg_val, SLE_TX_CFG_DATA_TX_TO_RX_LBf);

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_CFGr(unit, port, &reg_val));
    is_rx = soc_reg_field_get(unit, ILKN_SLE_RX_CFGr, reg_val, SLE_RX_CFG_DATA_TX_TO_RX_LBf);

    *enable = (is_tx && is_rx) ? 1 : 0;

exit:
    SOC_FUNC_RETURN; 
}

int pmOsILKN_port_loopback_set(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int enable)
{
    int i, core_id, rv, is_pm_aggregated=0;
    portmod_dispatch_type_t type;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    switch (loopback_type) {
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacCore:
    case portmodLoopbackMacPCS:
    case portmodLoopbackMacAsyncFifo:
        _SOC_IF_ERR_EXIT(_pmOsILKN_port_loopback_set(unit, port, enable));
        break;
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD:

        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {
                type = ilkn_data->pms[i]->type;
                if ((type != portmodDispatchTypePmOsILKN) &&
                    (NULL != __portmod__dispatch__[type]->f_portmod_port_loopback_set)){
                    rv = __portmod__dispatch__[type]->f_portmod_port_loopback_set(unit, port, ilkn_data->pms[i], loopback_type, enable);
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
        }
        break;
    case portmodLoopbackPhyRloopPCS:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Rloop PCS is not supported.")));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("unsupported loopback type %d"), loopback_type));
    }


exit:
    SOC_FUNC_RETURN; 
    
}

int pmOsILKN_port_loopback_get(int unit, int port, pm_info_t pm_info, portmod_loopback_mode_t loopback_type, int* enable)
{
    int i, core_id, rv, is_pm_aggregated=0;
    portmod_dispatch_type_t type;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    switch (loopback_type) {
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacCore:
    case portmodLoopbackMacPCS:
    case portmodLoopbackMacAsyncFifo:
        _SOC_IF_ERR_EXIT(_pmOsILKN_port_loopback_get(unit, port, enable));
        break;
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
    case portmodLoopbackPhyRloopPMD:
        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {
                type = ilkn_data->pms[i]->type;
                if ((type != portmodDispatchTypePmOsILKN) &&
                    (NULL != __portmod__dispatch__[type]->f_portmod_port_loopback_get)){
                    rv = __portmod__dispatch__[type]->f_portmod_port_loopback_get(unit, port, ilkn_data->pms[i], loopback_type, enable);
                    _SOC_IF_ERR_EXIT(rv);
                }
            }
        }
        break;
    case portmodLoopbackPhyRloopPCS:
        (*enable) = 0; /* not supported --> no loopback */
        break;
    default:
        (*enable) = 0; /* not supported --> no loopback */
    }

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_core_access_get(int unit, int port, pm_info_t pm_info, int phyn, int max_cores, 
                                  phymod_core_access_t* core_access_arr, int* nof_cores, int* is_most_ext)
{
    int i, core_id, arr_inx, rv, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    int is_most_ext_i;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    if (is_most_ext) {
        *is_most_ext = 0;
    }

    _SOC_IF_ERR_EXIT(phymod_core_access_t_init(&core_access_arr[0]));

    *nof_cores = arr_inx = 0;
    /* call PMs below - aggregate here */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            rv = __portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_core_access_get(unit, port, 
                                                ilkn_data->pms[i], phyn, max_cores - arr_inx, core_access_arr + arr_inx, nof_cores, &is_most_ext_i);
            _SOC_IF_ERR_EXIT(rv);

            arr_inx += *nof_cores;
            if (arr_inx >= max_cores) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("core_access_arr size %d isn't big enough"), max_cores));
            }

            if (is_most_ext) {
                *is_most_ext = (*is_most_ext | is_most_ext_i ? 1 : 0);
            }
        }
    }
    
    *nof_cores = arr_inx;

exit:    
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_phy_lane_access_get(int unit, int port, pm_info_t pm_info, const portmod_access_get_params_t* params,
                                      int max_phys, phymod_phy_access_t* access, int* nof_phys, int* is_most_ext)
{
    int i, core_id, arr_inx, rv, first_phy_in_pm, active_phys_in_pm, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    int is_most_ext_i, orig_lane = 0;
    portmod_access_get_params_t params_copy;
    int lane_index, active_lane_counter, phy_mask;
    soc_pbmp_t temp_phys_aligned;
    SOC_INIT_FUNC_DEFS;
    
    sal_memcpy(&params_copy, params, sizeof(portmod_access_get_params_t));

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    _SOC_IF_ERR_EXIT(PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, &temp_phys_aligned, core_id));
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* param.lane is counting only active lanes while pm4xXX_port_phy_lane_access_get() functions are counting
     * all lanes, including the non-active lanes. Therefore, the below loops translates:
     * params.lane (ignoring active lanes) => params_copy.lane (counting active lanes) */
    if (params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE){
        active_lane_counter = -1;
        PORTMOD_PBMP_ITER(temp_phys_aligned, lane_index) {
            active_lane_counter ++;
            if (active_lane_counter == params->lane)
                break;
        }

        params_copy.lane = lane_index;
    }

    if (is_most_ext) {
        *is_most_ext = 0;
    }

    _SOC_IF_ERR_EXIT(phymod_phy_access_t_init(&access[0]));

    *nof_phys = arr_inx = 0;


    /* call PMs below - aggregate here */    
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            if (arr_inx >= max_phys) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("access size %d isn't big enough"), max_phys));
            }
            /* ILKN counts lanes starting from first actual lane, 
               as oppose to the PM below which counts from first active lane.
               here we subtract the first non-active lanes before calling PM below*/
            if (params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE){
                phy_mask = 0xf << (i * OS_ILKN_LANES_PER_PHY_CORE);
                active_phys_in_pm = SOC_PBMP_WORD_GET(temp_phys_aligned, 0) & phy_mask;
                for (first_phy_in_pm = 0; (active_phys_in_pm & (1 << first_phy_in_pm)) == 0; ++first_phy_in_pm);
                orig_lane = params_copy.lane;
                if (first_phy_in_pm % OS_ILKN_LANES_PER_PHY_CORE) {
                    params_copy.lane -= first_phy_in_pm % OS_ILKN_LANES_PER_PHY_CORE; 
                }
            }
            rv = __portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_phy_lane_access_get(unit, port, 
                                                                                     ilkn_data->pms[i], (const portmod_access_get_params_t*) &params_copy, max_phys - arr_inx, access + arr_inx, nof_phys, &is_most_ext_i);
            if (params->lane != PORTMOD_ALL_LANES_ARE_ACTIVE){
                params_copy.lane = orig_lane;
            }

            _SOC_IF_ERR_EXIT(rv);

            PHYMOD_DEVICE_OP_MODE_PCS_BYPASS_SET(access[arr_inx].device_op_mode);
            arr_inx += *nof_phys;

            if (is_most_ext) {
                *is_most_ext = (*is_most_ext | is_most_ext_i ? 1 : 0);
            }
        }
        if (params_copy.lane >= 0) {
            params_copy.lane -= OS_ILKN_LANES_PER_PHY_CORE;
            if (params_copy.lane < 0){
                params_copy.lane = PORTMOD_NO_LANE_IS_ACTIVE; /* no lane in this pm since we already passed the specific lane*/
            }
        }

    }
    *nof_phys = arr_inx;

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_link_get(int unit, int port, pm_info_t pm_info, int flag, int* link)
{
    uint32 reg_val;
    int is_aligned, is_err;    
    SOC_INIT_FUNC_DEFS;
    
    /* link up indecation - in ILKN require interface will be in alignment */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_ALIGNr(unit, port, &reg_val));
    is_aligned = soc_reg_field_get(unit, ILKN_SLE_RX_ALIGNr, reg_val, SLE_RX_ALIGN_GOODf);
    is_err = soc_reg_field_get(unit, ILKN_SLE_RX_ALIGNr, reg_val, SLE_RX_ALIGN_ERRf);

    *link = (is_aligned && !is_err);

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_nof_lanes_get(int unit, int port, pm_info_t pm_info, int* nof_lanes)
{
    int core_id;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* pbmp count */
    PORTMOD_PBMP_COUNT(ilkn_data->phys, *nof_lanes);

exit:        
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_firmware_mode_set(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t fw_mode)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_firmware_mode_set(unit, port, ilkn_data->pms[i], fw_mode));
        }
    }
        
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_firmware_mode_get(int unit, int port, pm_info_t pm_info, phymod_firmware_mode_t* fw_mode)
{
    int core_id, index, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;        
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    for (index = 0; index < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++index) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, index));
        if (is_pm_aggregated) {
            /* assume all PMs are equal */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[index]->type]->f_portmod_port_firmware_mode_get(unit, port, ilkn_data->pms[index], fw_mode));
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;    
}


int pmOsILKN_port_update(int unit, int port, pm_info_t pm_info, const portmod_port_update_control_t* update_control)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_update(unit, port, ilkn_data->pms[i], update_control));
        }
    }
        
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_phy_reg_read(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32* value)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_phy_reg_read(unit, port, ilkn_data->pms[i], lane, flags, reg_addr, value));
        }
    }
    
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_phy_reg_write(int unit, int port, pm_info_t pm_info, int lane, int flags, int reg_addr, uint32 value)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_phy_reg_write(unit, port, ilkn_data->pms[i], lane, flags, reg_addr, value));
        }
    }

exit:
    SOC_FUNC_RETURN;    
}


int pmOsILKN_port_reset_set(int unit, int port, pm_info_t pm_info, int mode, int opcode, int value)
{
    SOC_INIT_FUNC_DEFS;
    
    /*TBD*/

    SOC_FUNC_RETURN;
}


int pmOsILKN_port_reset_get(int unit, int port, pm_info_t pm_info, int mode, int opcode, int* value)
{        
    SOC_INIT_FUNC_DEFS;

    /* TBD */

    SOC_FUNC_RETURN;     
}


int pmOsILKN_port_ref_clk_get(int unit, int port, pm_info_t pm_info, int* ref_clk)
{
    int core_id, index, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;        
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    for (index = 0; index < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++index) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, index));
        if (is_pm_aggregated) {
            /* assume all PMs are equal */
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[index]->type]->f_portmod_port_ref_clk_get(unit, port, ilkn_data->pms[index], ref_clk));
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_soft_reset_toggle(int unit, int port, pm_info_t pm_info, int idx)
{   
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* only call PMs below */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_soft_reset_toggle(unit, port, ilkn_data->pms[i], idx));
        }
    }
    
exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_prbs_config_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, const phymod_prbs_t* config)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    
    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {

                _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_config_set(unit, port, ilkn_data->pms[i], mode, flags, config));
            }
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }
    
exit:
    SOC_FUNC_RETURN;   
}


int pmOsILKN_port_prbs_config_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_t* config)
{            
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    
    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {
                _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_config_get(unit, port, ilkn_data->pms[i], mode, flags, config));
                break;
            }
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }

exit:
    SOC_FUNC_RETURN;    
}


int pmOsILKN_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int enable)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {
                _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_enable_set(unit, port, ilkn_data->pms[i], mode, flags, enable));
            }
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }

exit:
    SOC_FUNC_RETURN;
}


int pmOsILKN_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, int* enable)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
   
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    
    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {

                _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_enable_get(unit, port, ilkn_data->pms[i], mode, flags, enable));
                break;
            }
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }

exit:
    SOC_FUNC_RETURN;    
}


int pmOsILKN_port_prbs_status_get(int unit, int port, pm_info_t pm_info, portmod_prbs_mode_t mode, int flags, phymod_prbs_status_t* status)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
       
    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    
    if (mode == portmodPrbsModePhy) {
        /* call PMs */
        for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
            if (is_pm_aggregated) {

                _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_prbs_status_get(unit, port, ilkn_data->pms[i], mode, flags, status));
                break;
            }
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("mode % is unsupported"), mode));
    }

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_mode_set (int unit, soc_port_t port, 
                     pm_info_t pm_info, const portmod_port_mode_info_t *mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_SOC_MSG("mode set isn't supported")));

exit:
    SOC_FUNC_RETURN;  
}

int pmOsILKN_port_mode_get (int unit, soc_port_t port, 
                     pm_info_t pm_info, portmod_port_mode_info_t *mode)
{
    int nof_lanes, core_id;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);
    if (core_id == -1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("port %d is not attched"), port));
    }

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);
    PORTMOD_PBMP_COUNT(ilkn_data->phys, nof_lanes);

    mode->lanes = nof_lanes;
    mode->cur_mode = portmodPortModeQuad;
    
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_lag_failover_status_toggle(int unit, int port, pm_info_t pm_info)
{
    int core_id, i, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;

    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_lag_failover_status_toggle(unit, port, ilkn_data->pms[i]));
        }
    }
exit:
    SOC_FUNC_RETURN;
}
int pmOsILKN_xphy_lane_attach_to_pm(int unit, pm_info_t pm_info, int iphy, int phyn, const portmod_xphy_lane_connection_t* lane_connection)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pmOsILKN_xphy_lane_detach_from_pm(int unit, pm_info_t pm_info, int iphy, int phyn, portmod_xphy_lane_connection_t* lane_connection)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pmOsILKN_ext_phy_attach_to_pm(int unit, pm_info_t pm_info, const phymod_core_access_t *ext_phy_access, uint32 first_phy_lane)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pmOsILKN_ext_phy_detach_from_pm(int unit, pm_info_t pm_info, phymod_core_access_t *ext_phy_access)
{
    SOC_INIT_FUNC_DEFS;

    SOC_FUNC_RETURN;
}

int pmOsILKN_port_autoneg_set(int unit, int port, pm_info_t pm_info, uint32 phy_flags, const phymod_autoneg_control_t* an)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("ILKN does not support autoneg set")));
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_autoneg_get(int unit, int port, pm_info_t pm_info, uint32 phy_flags, phymod_autoneg_control_t* an)
{
    SOC_INIT_FUNC_DEFS;
    /*autoneg disabled for ILKN*/
    an->enable = 0;
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_duplex_set(int unit, int port, pm_info_t pm_info, int enable)
{
    SOC_INIT_FUNC_DEFS;
    
   _SOC_EXIT_WITH_ERR(SOC_E_FAIL, (_SOC_MSG("ILKN does not support duplex set")));
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_duplex_get(int unit, int port, pm_info_t pm_info, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    *enable = 1;

    SOC_FUNC_RETURN;
}

int pmOsILKN_port_local_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {

            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_local_fault_status_clear(unit, port, ilkn_data->pms[i]));
            break;
        }
    }
    
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_ilkn_nof_segments_set(int unit, int port, pm_info_t pm_info, uint32 nof_segments)
{
    uint32 reg_val;
    pmOsIlkn_revision_t revision;

    SOC_INIT_FUNC_DEFS;

    /* ilkn segment enable -  no validity check here */
    /* val =  (ports 1,3,5 || ELK || two_active_cores) ? 0x3 : 0xf; */
    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, &reg_val));
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_2f, (nof_segments >= 4) ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_3f, (nof_segments >= 4) ? 1 : 0);
    _SOC_IF_ERR_EXIT(pmOsILKN_revision_get(unit, port, &revision));
    /*
     * In SIM, the revision is not retrieved correctly
     */
    if (!SAL_BOOT_PLISIM)
    {
        if (OS_ILKN_IS_REVISION_8(revision)) {
            soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_4f, (nof_segments == 8) ? 1 : 0);
            soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_5f, (nof_segments == 8) ? 1 : 0);
            soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_6f, (nof_segments == 8) ? 1 : 0);
            soc_reg_field_set(unit, ILKN_SLE_TX_SEGMENT_ENABLEr, &reg_val, SLE_TX_SEGMENT_ENABLE_SEG_7f, (nof_segments == 8) ? 1 : 0);
        }
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_TX_SEGMENT_ENABLEr(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_2f, (nof_segments >= 4) ? 1 : 0);
    soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_3f, (nof_segments >= 4) ? 1 : 0);

    if (!SAL_BOOT_PLISIM)
    {
        if (OS_ILKN_IS_REVISION_8(revision)) {
            soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_4f, (nof_segments == 8) ? 1 : 0);
            soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_5f, (nof_segments == 8) ? 1 : 0);
            soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_6f, (nof_segments == 8) ? 1 : 0);
            soc_reg_field_set(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, &reg_val, SLE_RX_SEGMENT_ENABLE_SEG_7f, (nof_segments == 8) ? 1 : 0);
        }
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_ilkn_nof_segments_get(int unit, int port, pm_info_t pm_info, uint32* nof_segments)
{
    uint32 reg_val, seg_2_rx_en = 0, seg_4_rx_en = 0;
    pmOsIlkn_revision_t revision;

    SOC_INIT_FUNC_DEFS;

    /* ilkn segment enable -  no validity check here */
    /* val =  (ports 1,3,5 || ELK || two_active_cores) ? 0x3 : 0xf; */
    /* since segment 2 and 3 RX/TX are always enabled together, we can only check one of them to conclude 2/4 segments*/

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, &reg_val)); 
    seg_2_rx_en = soc_reg_field_get(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, reg_val, SLE_RX_SEGMENT_ENABLE_SEG_2f); 
    _SOC_IF_ERR_EXIT(pmOsILKN_revision_get(unit, port, &revision));
    /*
     * In SIM, the revision is not retrieved correctly
     */
    if (!SAL_BOOT_PLISIM)
    {
        if (OS_ILKN_IS_REVISION_8(revision))
        {
            seg_4_rx_en = soc_reg_field_get(unit, ILKN_SLE_RX_SEGMENT_ENABLEr, reg_val, SLE_RX_SEGMENT_ENABLE_SEG_4f);
        }
    }
    _SOC_IF_ERR_EXIT(WRITE_ILKN_SLE_RX_SEGMENT_ENABLEr(unit, port, reg_val));

    *nof_segments = ( seg_4_rx_en ) ? 8 : ((seg_2_rx_en) ? 4 : 2);
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_remote_fault_status_clear(int unit, int port, pm_info_t pm_info)
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data ;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_remote_fault_status_clear(unit, port, ilkn_data->pms[i]));
            break;
        }
    }
    
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_port_diag_ctrl(int unit, soc_port_t port, pm_info_t pm_info,
                      uint32 inst, int op_type, int op_cmd, const void *arg) 
{
    int i, core_id, is_pm_aggregated=0;
    pmOsIlkn_internal_t *ilkn_data;
    SOC_INIT_FUNC_DEFS;
    
    OS_ILKN_CORE_ID_GET(port, pm_info, core_id);

    ilkn_data = &(pm_info->pm_data.pmOsIlkn_db->ilkn_data[core_id]);

    /* call PMs */
    for (i = 0; i < OS_ILKN_MAX_ILKN_AGGREGATED_PMS; ++i) {
        _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, &is_pm_aggregated, core_id, i));
        if (is_pm_aggregated) {
            _SOC_IF_ERR_EXIT(__portmod__dispatch__[ilkn_data->pms[i]->type]->f_portmod_port_diag_ctrl(unit, port, ilkn_data->pms[i], inst, op_type, op_cmd, arg));
        }
    }
    
exit:
    SOC_FUNC_RETURN;
}

int pmOsILKN_revision_get(int unit, int port, pmOsIlkn_revision_t *revision)
{
    uint32 reg_val;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_ILKN_SLE_RX_INFOr(unit, port, &reg_val));
    *revision = soc_reg_field_get(unit, ILKN_SLE_RX_INFOr, reg_val, SLE_RX_INFO_RTL_MAJOR_REVf);

exit:
    SOC_FUNC_RETURN;
}

#endif /* PORTMOD_PM_OS_ILKN_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

