/*! \file bcma_sys_sim.h
 *
 * Simulator-specific APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_SYS_SIM_H
#define BCMA_SYS_SIM_H

#include <sal/sal_types.h>

/*!
 * \brief Get information about a simulated device.
 *
 * Calling this function corresponds to probing for a real hardware
 * device.
 *
 * For a given device index, the corresponding device idenitifcation
 * information is returned. If no device is present at the given
 * device index, the output information is undefined.
 *
 * A device probing function will typically call this function with a
 * device index incrementing from zero and until an error is returned.
 *
 * \param [in] dev_idx Device index to query.
 * \param [out] vendor_id Vendor ID for this device index.
 * \param [out] device_id Device ID for this device index.
 * \param [out] revision Device revision for this device index.
 * \param [out] model Model number for this device index.
 * \param [out] name Name of device at this device index.
 *
 * \retval 0 Device present at this device index.
 * \retval -1 No device present at this device index.
 */
int
bcma_sys_sim_dev_get(unsigned int dev_idx,
                     uint16_t *vendor_id, uint16_t *device_id,
                     uint16_t *revision, uint16_t *model, const char **name);

#endif /* BCMA_SYS_SIM_H */
