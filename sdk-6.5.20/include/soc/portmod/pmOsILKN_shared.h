/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PM_OS_ILKN_SHARED_H_
#define _PM_OS_ILKN_SHARED_H_

#define OS_ILKN_TOTAL_LANES_PER_CORE                (24)
#define OS_ILKN_MAX_NOF_INTERNAL_PORTS              (2)
#define OS_ILKN_MAX_ILKN_AGGREGATED_PMS             (6)
#define OS_ILKN_MAX_NOF_CHANNELS                    (256)
#define OS_ILKN_SLE_MAX_NOF_PIPES                   (24)
#define OS_ILKN_LANE_MAP_REGISTER_RESOLUTION          (5)

typedef struct pmOsIlkn_internal_s{
    portmod_pbmp_t phys;
    pm_info_t *pms; /*pm_info_t is also a pointer (on pm_info_s) so pms is actually pointer to an array of OS_ILKN_MAX_ILKN_AGGREGATED_PMS pointers*/
    int *pm_ids;
    int wm_high;
    int wm_low;
    uint32 core_clk_khz;
} pmOsIlkn_internal_t;

struct pmOsIlkn_s{
    pmOsIlkn_internal_t ilkn_data[OS_ILKN_MAX_NOF_INTERNAL_PORTS];
    portmod_ilkn_port_lane_map_callback_get_f ilkn_port_lane_map_get; /* Callback function */
    portmod_ilkn_pm_lane_map_callback_get_f ilkn_pm_lane_map_get; /* Callback function */
    portmod_ilkn_port_fec_units_set_callback_f ilkn_port_fec_units_set; /* Callback function */
    uint32 fec_disable_by_bypass;
};

/* Ilkn core lane map structure */
typedef struct pmOsIlkn_core_lane_map_s {
    int num_of_lanes;
    int rx_lane_map[OS_ILKN_TOTAL_LANES_PER_CORE];
    int tx_lane_map[OS_ILKN_TOTAL_LANES_PER_CORE];
} pmOsIlkn_core_lane_map_t;

typedef enum pmOsIlkn_wb_vars{ /*No need for isActive, isInitialized and isBypassed since they are treated from the scope of the relevant associated pms (pm4x25 / pm4x10 / pm4x10q etc).*/
    ports = 0,
    isPortOverFabric,
    physAligned,
    isPmAggregated,
    ilknData,
    AggregatedPmIds
}pmOsIlkn_wb_vars_t;

/*
 * Define the ILKN revision
 */
typedef enum
{
    OS_ILKN_REVISION_0 = 0,
    OS_ILKN_REVISION_7 = 7,
    OS_ILKN_REVISION_8 = 8
} pmOsIlkn_revision_t;

#define OS_ILKN_IS_REVISION_0(revision)   (revision == OS_ILKN_REVISION_0)
#define OS_ILKN_IS_REVISION_7(revision)   (revision == OS_ILKN_REVISION_7)
#define OS_ILKN_IS_REVISION_8(revision)   (revision == OS_ILKN_REVISION_8)

/* WB defines */
#define PM_ILKN_PORT_SET(unit, pm_info, ilkn_port, ilkn_core_id) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], &ilkn_port, ilkn_core_id);
#define PM_ILKN_PORT_GET(unit, pm_info, ilkn_port, ilkn_core_id) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[ports], ilkn_port, ilkn_core_id)

#define PM_ILKN_IS_PORT_OVER_FABRIC_SET(unit, pm_info, is_port_over_fabric, ilkn_core_id) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isPortOverFabric], &is_port_over_fabric, ilkn_core_id);
#define PM_ILKN_IS_PORT_OVER_FABRIC_GET(unit, pm_info, is_port_over_fabric, ilkn_core_id) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isPortOverFabric], is_port_over_fabric, ilkn_core_id)

#define PM_ILKN_PHYS_ALIGNED_SET(unit, pm_info, phys_aligned, ilkn_core_id) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[physAligned], &phys_aligned, ilkn_core_id);
#define PM_ILKN_PHYS_ALIGNED_GET(unit, pm_info, phys_aligned, ilkn_core_id) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[physAligned], phys_aligned, ilkn_core_id)

#define PM_ILKN_IS_PM_AGGREGATED_SET(unit, pm_info, is_pm_aggregated, ilkn_core_id, agg_pm) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isPmAggregated], &is_pm_aggregated, ((ilkn_core_id*(OS_ILKN_MAX_ILKN_AGGREGATED_PMS))+agg_pm));
#define PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, is_pm_aggregated, ilkn_core_id, agg_pm) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[isPmAggregated], is_pm_aggregated, ((ilkn_core_id*(OS_ILKN_MAX_ILKN_AGGREGATED_PMS))+agg_pm))

#define PM_ILKN_AGGREGATED_PM_ID_SET(unit, pm_info, pm_id, ilkn_core_id, agg_pm) \
            SOC_WB_ENGINE_SET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[AggregatedPmIds], &pm_id, ((ilkn_core_id*(OS_ILKN_MAX_ILKN_AGGREGATED_PMS))+agg_pm));
#define PM_ILKN_AGGREGATED_PM_ID_GET(unit, pm_info, pm_id, ilkn_core_id, agg_pm) \
            SOC_WB_ENGINE_GET_ARR(unit, SOC_WB_ENGINE_PORTMOD, pm_info->wb_vars_ids[AggregatedPmIds], pm_id, ((ilkn_core_id*(OS_ILKN_MAX_ILKN_AGGREGATED_PMS))+agg_pm))

#define OS_ILKN_CORE_ID_GET(port, pm_info, core_id)                                           \
    do {                                                                                      \
        int port0, port1;                                                                     \
        _SOC_IF_ERR_EXIT(PM_ILKN_PORT_GET(unit, pm_info, &port0, 0));                         \
        _SOC_IF_ERR_EXIT(PM_ILKN_PORT_GET(unit, pm_info, &port1, 1));                         \
        core_id = (port0 == port) ? 0 : ((port1 == port) ? 1 : -1);                           \
        if (core_id == -1) {                                                                  \
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("port %d is not attached"), port));     \
            SOC_EXIT;                                                                         \
        }                                                                                     \
    } while(0)

#define OS_ILKN_IS_PM_SHARED_GET(port, pm_info, core_id, pm, is_other_pm_aggregated)                                         \
    do {                                                                                                                     \
        int other_ilkn_core_id, other_port;                                                                                  \
        *is_other_pm_aggregated = 0;                                                                                         \
        other_ilkn_core_id = (~core_id) & 0x1;                                                                               \
        _SOC_IF_ERR_EXIT(PM_ILKN_PORT_GET(unit, pm_info, &other_port, other_ilkn_core_id));                                  \
        if (other_port != -1){                                                                                               \
            _SOC_IF_ERR_EXIT(PM_ILKN_IS_PM_AGGREGATED_GET(unit, pm_info, is_other_pm_aggregated, other_ilkn_core_id, pm));   \
        }                                                                                                                    \
    } while(0)

/**
 * \brief - configure the number of pipes of OS ilkn core
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] pm_info - PM information.
 * \param [in] nof_pipes - number of pipes.
 *
 * \return
 *   STATIC int - see SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_port_pipe_config(int unit, int port, pm_info_t pm_info, uint32 nof_pipes);

/**
 * \brief - Get the OS ILKN revision
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] revision - revision ID.
 *
 * \return
 *   STATIC int - see SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int pmOsILKN_revision_get(int unit, int port, pmOsIlkn_revision_t *revision);

#endif /*_PM_OS_ILKN_SHARED_H_*/

