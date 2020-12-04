/*! \file bcma_bcmpc_diag_internal.h
 *
 * Internal functions for BCMPC diagnostics.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPC_DIAG_INTERNAL_H
#define BCMA_BCMPC_DIAG_INTERNAL_H

#include <bcmpc/bcmpc_diag.h>

/*! Maximum number of lanes. */
#define BCMA_BCMPC_PM_MAX_LANES            8

/*!
 * \brief Get speed rate.
 *
 * The returned \c rate will be 0 if no matching rate for the specified
 * parameters.
 *
 * \param [in] speed Port speed in Mbps.
 * \param [in] num_lanes Number of lanes.
 * \param [in] fec_type FEC type.
 * \param [out] rate Base rate for speed.
 *
 * \retval SHR_E_NONE.
 */
int
bcma_bcmpc_diag_speed_rate_get(uint32_t speed, int num_lanes,
                               bcmpc_cmd_fec_t fec_type, double *rate);

#endif /* BCMA_BCMPC_DIAG_INTERNAL_H */
