/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_flexe_core.h
 *
 *  Flexe core algo functions declaration.
 *
 */

#ifndef _ALGO_FLEXE_CORE_H
/*
 * {
 */
#define _ALGO_FLEXE_CORE_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

/*
 * Includes.
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

/*
 * }
 */
/*
 * Macros.
 * {
 */
#define DNX_ALGO_FLEXE_CORE_LPHY_ALLOC_CHECK_ONLY 1

#ifndef MIN
#define MIN(a, b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) (((a)>(b)) ? (a) : (b))
#endif

#ifndef FLEXE_PHY_SPEED_100G
#define FLEXE_PHY_SPEED_100G 100000
#endif

#ifndef FLEXE_PHY_NOF_INSTANCES
#define FLEXE_PHY_NOF_INSTANCES(_speed) MAX(_speed/FLEXE_PHY_SPEED_100G, 1)
#endif
/*
 * }
 */
/*
 * TypeDefs.
 * {
 */

/*
 * }
 */

/*
 * Module Init / Deinit
 * {
 */
/**
 * \brief -
 * Init algo flexe general module for cool boot mode.
 *
 * \param [in] unit - Unit #.
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_flexe_core_init(
    int unit);

shr_error_e dnx_algo_flexe_core_demux_lphy_alloc(
    int unit,
    int flags,
    int speed,
    int start_pos,
    int end_pos,
    SHR_BITDCL * lphy_bitmap,
    int *is_success);

shr_error_e dnx_algo_flexe_core_demux_lphy_free(
    int unit,
    SHR_BITDCL * lphy_bitmap);

shr_error_e dnx_algo_flexe_core_mux_lphy_alloc(
    int unit,
    int speed,
    SHR_BITDCL * lphy_bitmap);

shr_error_e dnx_algo_flexe_core_mux_lphy_free(
    int unit,
    SHR_BITDCL * lphy_bitmap);

shr_error_e dnx_algo_flexe_core_mac_ts_alloc(
    int unit,
    int nof_timeslots,
    SHR_BITDCL * ts_bitmap);

shr_error_e dnx_algo_flexe_core_mac_ts_free(
    int unit,
    SHR_BITDCL * ts_bitmap);

shr_error_e dnx_algo_flexe_core_sar_rx_ts_alloc(
    int unit,
    int nof_timeslots,
    SHR_BITDCL * ts_bitmap);

shr_error_e dnx_algo_flexe_core_sar_rx_ts_free(
    int unit,
    SHR_BITDCL * ts_bitmap);

shr_error_e dnx_algo_flexe_core_sar_tx_ts_alloc(
    int unit,
    int nof_timeslots,
    SHR_BITDCL * ts_bitmap);

shr_error_e dnx_algo_flexe_core_sar_tx_ts_free(
    int unit,
    SHR_BITDCL * ts_bitmap);
shr_error_e dnx_algo_flexe_core_rateadpt_ts_alloc(
    int unit,
    int nof_timeslots,
    SHR_BITDCL * ts_bitmap);

shr_error_e dnx_algo_flexe_core_rateadpt_ts_free(
    int unit,
    SHR_BITDCL * ts_bitmap);
/*
 * \brief - FlexE core core port DB init.
 */
shr_error_e dnx_algo_flexe_core_port_db_init(
    int unit,
    int flexe_core_port);
/*
 * \brief - Get flexe instance ID.
 */
shr_error_e dnx_algo_flexe_core_port_to_instance_id_get(
    int unit,
    int flexe_core_port,
    int *instance_id);
/*
 * \brief - Configure flexe instance ID.
 */
shr_error_e dnx_algo_flexe_core_port_to_instance_id_set(
    int unit,
    int flexe_core_port,
    int instance_id);
/*
 * \brief - Alloc flexe instance ID.
 */
shr_error_e dnx_algo_flexe_instance_id_alloc(
    int unit,
    int speed,
    int flexe_core_port,
    int is_check_only,
    int *instance_id,
    int *nof_instances);
/*
 * \brief - Free flexe instance ID.
 */
shr_error_e dnx_algo_flexe_instance_id_free(
    int unit,
    int instance_id,
    int nof_instances);
/*
 * \brief - Set flexe core port speed.
 */
shr_error_e dnx_algo_flexe_core_port_speed_set(
    int unit,
    int flexe_core_port,
    int speed);
/*
 * \brief - Get flexe core port speed.
 */
shr_error_e dnx_algo_flexe_core_port_speed_get(
    int unit,
    int flexe_core_port,
    int *speed);
/*
 * \brief - Set flexe core port AM transparent configuration.
 */
shr_error_e dnx_algo_flexe_core_port_am_transparent_set(
    int unit,
    int flexe_core_port,
    int am_transparent);
/*
 * \brief - Get flexe core port speed AM transparent configuration.
 */
shr_error_e dnx_algo_flexe_core_port_am_transparent_get(
    int unit,
    int flexe_core_port,
    int *am_transparent);
/*
 * \brief - Set number of flexe instances.
 */
shr_error_e dnx_algo_flexe_core_port_nof_instances_set(
    int unit,
    int flexe_core_port,
    int nof_instances);
/*
 * \brief - Get number of flexe instances.
 */
shr_error_e dnx_algo_flexe_core_port_nof_instances_get(
    int unit,
    int flexe_core_port,
    int *nof_instances);
/*
 * \brief - Get FlexE group index according to flexe core port.
 */
shr_error_e dnx_algo_flexe_core_port_to_group_index_get(
    int unit,
    int flexe_core_port,
    int *group_index);
/*
 * \brief - Get instance bitmap per flexe core port.
 */
shr_error_e dnx_algo_flexe_core_port_instances_get(
    int unit,
    int flexe_core_port,
    uint32 *instances);
/*
 * \brief - Get instance bitmap per flexe group.
 */
shr_error_e dnx_algo_flexe_group_instances_get(
    int unit,
    int group,
    uint32 *instances);
/*
 * \brief - Set SAR cell mode.
 */
shr_error_e dnx_algo_flexe_core_sar_cell_mode_set(
    int unit,
    int client_channel,
    int is_rx,
    int cell_mode);
/*
 * \brief - Get SAR cell mode.
 */
shr_error_e dnx_algo_flexe_core_sar_cell_mode_get(
    int unit,
    int client_channel,
    int is_rx,
    int *cell_mode);
/*
 * \brief - Set SAR calendar granularity.
 */
shr_error_e dnx_algo_flexe_core_sar_cal_granularity_get(
    int unit,
    int client_channel,
    int is_rx,
    bcm_port_flexe_phy_slot_mode_t * granularity);
/*
 * \brief - Get SAR calendar granularity.
 */
shr_error_e dnx_algo_flexe_core_sar_cal_granularity_set(
    int unit,
    int client_channel,
    int is_rx,
    bcm_port_flexe_phy_slot_mode_t granularity);
/*
 * \brief - Calculate SAR calendar granularity
 */
shr_error_e dnx_algo_flexe_core_sar_cal_granularity_calculate(
    int unit,
    int speed,
    bcm_port_flexe_phy_slot_mode_t * granularity);
/*
 * \brief - Set SAR channel valid status.
 */
shr_error_e dnx_algo_flexe_core_sar_channel_valid_get(
    int unit,
    int client_channel,
    int is_rx,
    int *valid);
/*
 * \brief - Get SAR channel valid status.
 */
shr_error_e dnx_algo_flexe_core_sar_channel_valid_set(
    int unit,
    int client_channel,
    int is_rx,
    int valid);
/*
 * \brief - Set FlexE client slot mode
 */
shr_error_e dnx_algo_flexe_client_slot_mode_set(
    int unit,
    int client_channel,
    int is_rx,
    bcm_port_flexe_phy_slot_mode_t slot_mode);
/*
 * \brief - Set FlexE client slot mode
 */
shr_error_e dnx_algo_flexe_client_slot_mode_get(
    int unit,
    int client_channel,
    int is_rx,
    bcm_port_flexe_phy_slot_mode_t * slot_mode);
/*
 * }
 */
#endif /*_ALGO_FLEXE_CORE_H*/
