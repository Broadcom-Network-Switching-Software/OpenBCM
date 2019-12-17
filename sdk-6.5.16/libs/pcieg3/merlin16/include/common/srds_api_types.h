/*****************************************************************************************
 *****************************************************************************************
 *                                                                                       *
 *  Revision      :   *
 *                                                                                       *
 *  Description   :  Functions used internally and available in debug shell only         *
 *                                                                                       *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.                                                                 *
 *  No portions of this material may be reproduced in any form without                   *
 *  the written permission of:                                                           *
 *      Broadcom Corporation                                                             *
 *      5300 California Avenue                                                           *
 *      Irvine, CA  92617                                                                *
 *                                                                                       *
 *  All information contained in this document is Broadcom Corporation                   *
 *  company private proprietary, and trade secret.                                       *
 */

/** @file srds_api_types.h
 * Common types used by Serdes API functions
 * This file contains types which are common to all serdes cores
 */

#ifndef SRDS_API_TYPES_H
#define SRDS_API_TYPES_H

/*!
 * srds_bus_read_f
 *
 * @brief function definition for register read operations
 *
 * @param [inout] user_acc Optional application data
 * @param [in] addr Register address to read
 * @param [out] val Read value
 *
 * @retval 0 No errors.
 * @retval -1 Failed to read.
 */
typedef int (*srds_bus_read_f)(void* user_acc, uint32_t addr, uint16_t* val);

/*!
 * srds_bus_write_f
 *
 * @brief function definition for register write operations
 *
 * @param [inout] user_acc Optional application data
 * @param [in] addr Register address to write
 * @param [in] val Value to write
 *
 * @retval 0 No errors.
 * @retval -1 Failed to read.
 */
typedef int (*srds_bus_write_f)(void* user_acc, uint32_t addr, uint16_t val);

typedef struct srds_bus_s {
    char* name;
    srds_bus_read_f read;
    srds_bus_write_f write;
} srds_bus_t;

typedef struct srds_access_s {
    void* user_acc; /**< Optional application data - not used by PHY driver */
    srds_bus_t *bus; /**< PHY bus driver */
    int unit; /* Switch unit */
    uint8_t core; /* Serdes core number, 0 - 3, each core has 4 lanes */
    uint8_t lane_mask; /* specific lanes bitmap */
    uint16_t devid; /* device id, should be 1 for this device */
} srds_access_t;

#define SRDS_BUS_VALIDATE(pa_) (pa_ && pa_->bus && pa_->bus->read && pa_->bus->write)
#define SRDS_BUS_READ(pa_,r_,v_) pa_->bus->read(pa_,r_,v_)
#define SRDS_BUS_WRITE(pa_,r_,v_) pa_->bus->write(pa_,r_,v_)

typedef enum srds_error_e {
    SRDS_E_NONE = 0,
    SRDS_E_FAIL = -1,
    SRDS_E_PARAM = -2
} srds_error_t;

#endif
