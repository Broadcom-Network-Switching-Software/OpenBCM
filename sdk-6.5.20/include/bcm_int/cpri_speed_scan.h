/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _CPRI_SPEED_SCAN_H_
#define _CPRI_SPEED_SCAN_H_

#define BCM_SPEED_SCAN_INTERVAL_MIN     70   /* 70 usec. */
#define SPD_SCAN_THREAD_PRI             100
#define SPD_SCAN_NUM_UNITS              SOC_MAX_NUM_DEVICES
#define SPD_SCAN_NUM_PORTS              SOC_MAX_NUM_PORTS
#define SPD_SCAN_MAX_SPEED_LIST         4


#define SPD_SCAN_F_INIT                 0x1 /**<  Module is initialized. */
#define SPD_SCAN_F_ENABLE               0x2 /**<  Scanning is enabled.   */

#define SPD_SCAN_F_INIT_SET(flags)      ((flags) |= SPD_SCAN_F_INIT)
#define SPD_SCAN_F_ENABLE_SET(flags)    ((flags) |= SPD_SCAN_F_ENABLE)

#define SPD_SCAN_F_INIT_CLR(flags)      ((flags) &= ~SPD_SCAN_F_INIT)
#define SPD_SCAN_F_ENABLE_CLR(flags)    ((flags) &= ~SPD_SCAN_F_ENABLE)

#define SPD_SCAN_F_INIT_GET(flags)      ((flags) & SPD_SCAN_F_INIT ? 1 : 0)
#define SPD_SCAN_F_ENABLE_GET(flags)    ((flags) & SPD_SCAN_F_ENABLE? 1 : 0)

typedef enum _cpri_spd_scan_state_e {
    _CPRI_SPD_SCAN_STATE_SCANNING         = 0x00,  /**< Finding Speed */
    _CPRI_SPD_SCAN_STATE_FOUND            = 0x01,  /**< Speed Found   */
    _CPRI_SPD_SCAN_STATE_RESOLVED         = 0x02,  /**< Speed Set     */
} _cpri_spd_scan_state_t;

#ifdef BCM_WARM_BOOT_SUPPORT
int bcm_esw_cpri_speed_scan_sync (int unit);
int bcm_esw_cpri_speed_scan_reload (int unit);
#endif

#endif /* _CPRI_SPEED_SCAN_H_ */

