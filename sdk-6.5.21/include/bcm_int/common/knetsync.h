/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains knetsync Interface definitions internal to the BCM library.
 */

#ifndef _BCM_INT_COMMON_KNETSYNC_H
#define _BCM_INT_COMMON_KNETSYNC_H

int _bcm_common_knetsync_mtp_update(int unit, int mtp_index, int start);

int bcm_common_knetsync_init(int unit);
int bcm_common_knetsync_deinit(int unit);

#endif /* _BCM_INT_COMMON_KNETSYNC_H */

