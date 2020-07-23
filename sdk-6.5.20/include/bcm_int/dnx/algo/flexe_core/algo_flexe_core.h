/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_flexe_core.h
 *
 *  Flexe core algo functions declaration.
 */

#ifndef _ALGO_FLEXE_CORE_H
/*
 * {
 */
#define _ALGO_FLEXE_CORE_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Includes.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
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

shr_error_e dnx_algo_flexe_core_instance_alloc(
    int unit,
    int speed,
    int start_pos,
    int end_pos,
    int *instance);

shr_error_e dnx_algo_flexe_core_instance_free(
    int unit,
    int speed,
    int instance);

shr_error_e dnx_algo_flexe_core_lphy_alloc(
    int unit,
    int flags,
    int speed,
    int start_pos,
    int end_pos,
    SHR_BITDCL * lphy_bitmap);

shr_error_e dnx_algo_flexe_core_lphy_free(
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
 * }
 */
#endif /*_ALGO_FLEXE_CORE_H*/
