/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        bfd.h
 * Purpose:     Definitions for BFD.
 */

#ifndef _BCM_INT_BFD_H_
#define _BCM_INT_BFD_H_

#if defined(INCLUDE_BFD)

#include <bcm/bfd.h>

/*
 * BFD Function Vector Driver
 */
typedef struct bcm_esw_bfd_drv_s {
    /*
     * BFD APIs
     */
    /* Initalize BFD module */
    int (*init)(int unit);
    /* Detach BFD module */
    int (*detach)(int unit);
    /* Create BFD endpoint */
    int (*endpoint_create)(int unit, bcm_bfd_endpoint_info_t *endpoint_info);
    /* Get BFD endpoint */
    int (*endpoint_get)(int unit, bcm_bfd_endpoint_t endpoint, 
                        bcm_bfd_endpoint_info_t *endpoint_info);
    /* Destroy BFD endpoint */
    int (*endpoint_destroy)(int unit, bcm_bfd_endpoint_t endpoint);
    /* Destroy all BFD endpoints */
    int (*endpoint_destroy_all)(int unit);
    /* Generate BFD poll sequence */
    int (*endpoint_poll)(int unit, bcm_bfd_endpoint_t endpoint);
    /* Register BFD callback for event notification */
    int (*event_register)(int unit, bcm_bfd_event_types_t event_types, 
                          bcm_bfd_event_cb cb, void *user_data);
    /* Unregister BFD callback */
    int (*event_unregister)(int unit, bcm_bfd_event_types_t event_types, 
                            bcm_bfd_event_cb cb);
    /* Get BFD endpoint statistics */
    int (*endpoint_stat_get)(int unit, bcm_bfd_endpoint_t endpoint, 
                             bcm_bfd_endpoint_stat_t *ctr_info, uint32 options);
    /* Set BFD SHA1 authentication */
    int (*auth_sha1_set)(int unit, int index, bcm_bfd_auth_sha1_t *sha1);
    /* Get BFD SHA1 authentication */
    int (*auth_sha1_get)(int unit, int index, bcm_bfd_auth_sha1_t *sha1);
    /* Set BFD Simple Password authentication */
    int (*auth_simple_password_set)(int unit, int index, 
                                    bcm_bfd_auth_simple_password_t *sp);
    /* Get BFD Simple Password authentication */
    int (*auth_simple_password_get)(int unit, int index, 
                                    bcm_bfd_auth_simple_password_t *sp);
#ifdef BCM_WARM_BOOT_SUPPORT
    /* BFD Warm boot sync */
    int (*bfd_sync)(int unit);

    int (*_bfd_wb_downgrade_config_set)(int unit, uint32 warmboot_ver);
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
    /* BFD sessions dump support */
    void (*bfd_sw_dump)(int unit); 
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
    /* Start TX BFD PDUs  */
    int (*tx_start)(int unit);
    /* Stop TX BFD PDUs */
    int (*tx_stop)(int unit);
    /* bfd_status_multi_get */
    int (*bfd_status_multi_get)(int unit, int max_endpoints,
                                bcm_bfd_status_t *status_arr,
                                int *count);
    int (*bfd_discard_stat_set) (int unit,
                                 bcm_bfd_discard_stat_t *discarded_info);
    int (*bfd_discard_stat_get) (int unit,
                                 bcm_bfd_discard_stat_t *discarded_info);
    /* Dump uC backtrace */
    void (*bfd_dump_trace)(int unit);
} bcm_esw_bfd_drv_t;

extern bcm_esw_bfd_drv_t      *bcm_esw_bfd_drv[BCM_MAX_NUM_UNITS];

#define BCM_ESW_BFD_DRV(_u)   (bcm_esw_bfd_drv[(_u)])


/*
 * Default BFD RX Event Notification thread priority
 */
#define BCM_ESW_BFD_THREAD_PRI_DFLT     200

#ifdef BCM_WARM_BOOT_SUPPORT
int _bcm_esw_bfd_sync(int unit);

int _bcm_esw_bfd_wb_downgrade_config_set(int unit, uint32 warmboot_ver);

#define BCM_BFD_WB_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_BFD_WB_VERSION_1_1      SOC_SCACHE_VERSION(1,1)
#define BCM_BFD_WB_VERSION_1_2      SOC_SCACHE_VERSION(1,2)
#define BCM_BFD_WB_VERSION_1_3      SOC_SCACHE_VERSION(1,3)
#define BCM_BFD_WB_VERSION_1_4      SOC_SCACHE_VERSION(1,4)
#define BCM_BFD_WB_VERSION_1_5      SOC_SCACHE_VERSION(1,5)
#define BCM_BFD_WB_VERSION_1_6      SOC_SCACHE_VERSION(1,6)
#define BCM_BFD_WB_DEFAULT_VERSION  BCM_BFD_WB_VERSION_1_6

#define VERSION_1_2_EGRESS_LABEL_SIZE 48

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void _bcm_bfd_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

void _bcm_bfd_dump_trace(int unit);

/*
 * BFD Feature Enable Bit Flags
 */
#define BFD_MHOP_FEATURE_ENABLE      (1 << 0)
#define BFD_MICRO_FEATURE_ENABLE     (1 << 1)
#define BFD_ECHO_FEATURE_ENABLE      (1 << 2)

#define BFD_ECHO_DEST_PORT           3785
#define BFD_UDP_MULTI_HOP_DEST_PORT  4784
#define MICRO_BFD_DEST_PORT          6784

#endif /* INCLUDE_BFD */

#endif /* _BCM_INT_BFD_H_ */
