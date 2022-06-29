/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This file contains knetsync Interface definitions internal to the BCM library.
 *
 */

#ifndef _BCM_INT_COMMON_KNETSYNC_H
#define _BCM_INT_COMMON_KNETSYNC_H
#include <bcm/types.h>

typedef struct _bcm_knetsync_info_s {
    uint32 pci_knetsync_cosq;
    uint32 uc_port_num;
    uint32 uc_port_sysport;
    uint32 host_cpu_port;
    uint32 host_cpu_sysport;
    uint32 udh_len;
} _bcm_knetsync_info_t;

int _bcm_common_knetsync_mtp_update(int unit, int mtp_index, int start);

int bcm_common_knetsync_init(int unit, _bcm_knetsync_info_t info);
int bcm_common_knetsync_deinit(int unit);

#endif /* _BCM_INT_COMMON_KNETSYNC_H */

