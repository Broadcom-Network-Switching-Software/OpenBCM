/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file src/bcm/dnx/cosq/scheduler_dbal.h
 * Reserved.$ 
 */

#ifndef _DNX_SCHEDULER_DEVICE_H_INCLUDED_
#define _DNX_SCHEDULER_DEVICE_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>

/**
 * \brief - set an entry of Device DRM table
 */
shr_error_e dnx_scheduler_device_shared_rate_entry_set(
    int unit,
    int rci_level,
    int num_links,
    uint32 rate);

/**
 * \brief - get an entry of Device DRM table
 */
shr_error_e dnx_scheduler_device_shared_rate_entry_get(
    int unit,
    int rci_level,
    int num_links,
    uint32 *rate);

/**
 * \brief - set an entry of Core DRM table
 */
shr_error_e dnx_scheduler_device_core_rate_entry_set(
    int unit,
    int core,
    int rci_level,
    int num_links,
    uint32 rate);

/**
 * \brief - get an entry of Core DRM table
 */
shr_error_e dnx_scheduler_device_core_rate_entry_get(
    int unit,
    int core,
    int rci_level,
    int num_links,
    uint32 *rate);

#endif
