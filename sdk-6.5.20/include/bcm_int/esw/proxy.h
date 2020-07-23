/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains Proxy definitions internal to the BCM library.
 */

#ifndef _BCM_INT_PROXY_H
#define _BCM_INT_PROXY_H

#include <bcm/proxy.h>

#define  _BCM_PROXY_EGR_NEXT_HOP_SDTAG_VIEW  0x2
#define  _BCM_PROXY_RESERVED_DGLP_NUMBER 2

extern int
_bcm_proxy_client_enabled(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
extern int
bcmi_proxy_server_sync(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

/* Book-keeping structure for proxy_server */
typedef struct bcmi_proxy_bookkeep_s {
    bcm_proxy_server_t   **proxy_server_copy;
} bcmi_proxy_bookkeep_t;
#endif /* _BCM_INT_PROXY_H */
