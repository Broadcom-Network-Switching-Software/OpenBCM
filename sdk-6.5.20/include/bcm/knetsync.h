/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_KNETSYNC_H__
#define __BCM_KNETSYNC_H__

#include <bcm/types.h>


#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize the KNETSync ptp clock subsystem and Firmware. */
extern int bcm_knetsync_init(
    int unit);

/* Deinitailize the KNETSync ptp clock subsystem and Firmware. */
extern int bcm_knetsync_deinit(
    int unit);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_KNETSYNC_H__ */
