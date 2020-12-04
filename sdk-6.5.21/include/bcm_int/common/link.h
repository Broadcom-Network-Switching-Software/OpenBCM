/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * Common internal definitions for BCM Linkscan module
 */

#ifndef _BCM_INT_LINK_H_
#define _BCM_INT_LINK_H_

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/link.h>
#include <soc/types.h>

#define NUM_PORTS    SOC_MAX_NUM_PORTS

/*
 * Driver specific routines
 *
 * (See description for routines below)
 */
typedef struct _bcm_ls_driver_s {
    void (*ld_hw_interrupt)(int, bcm_pbmp_t*);
    int  (*ld_port_link_get)(int, bcm_port_t, int, int*);
    int  (*ld_internal_select)(int, bcm_port_t);
    int  (*ld_update_asf)(int, bcm_port_t, int, int, int);
    int  (*ld_trunk_sw_failover_trigger)(int, bcm_pbmp_t, bcm_pbmp_t);
} _bcm_ls_driver_t;

/*
 * Driver specific routines description
 *
 * Function:
 *     ld_hw_interrupt
 * Purpose:
 *     Routine handler for hardware linkscan interrupt.
 * Parameters:
 *     unit - Device unit number
 *     pbmp - (OUT) Returns bitmap of ports that require hardware re-scan
 *
 *
 * Function:
 *     ld_port_link_get
 * Purpose:
 *     Return current PHY up/down status.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *     hw   - If TRUE, assume hardware linkscan is active and use it
 *              to reduce PHY reads.
 *            If FALSE, do not use information from hardware linkscan.
 *     up   - (OUT) TRUE for link up, FALSE for link down.
 *
 *
 * Function:
 *     ld_internal_select
 * Purpose:
 *     Select the source of the CMIC link status interrupt
 *     to be the Internal Serdes on given port.
 * Parameters:
 *     unit - Device unit number
 *     port - Device port number
 *
 *
 * Function:
 *     ld_update_asf
 * Purpose:
 *     Update Alternate Store and Forward parameters for a port.
 * Parameters:
 *     unit   - Device unit number
 *     port   - Device port number
 *     linkup - port link state (0=down, 1=up)
 *     speed  - port speed
 *     duplex - port duplex (0=half, 1=full)
 *
 *
 * Function:
 *     ld_trunk_sw_failover_trigger
 * Purpose:
 *     Remove specified ports with link down from trunks.
 * Parameters:
 *     unit        - Device unit number
 *     pbmp_active - Bitmap of ports
 *     pbmp_status - Bitmap of port status
 */

extern int _bcm_linkscan_init(int unit, const _bcm_ls_driver_t *driver);

extern int _bcm_link_get(int unit, bcm_port_t port, int *link);
extern int _bcm_link_force(int unit, bcm_port_t port, int force, int link);

extern int _bcm_linkscan_pause(int unit);
extern int _bcm_linkscan_continue(int unit);
extern int _bcm_linkscan_available(int unit);
extern int _bcm_linkscan_port_init(int unit, int port);

#ifdef BCM_WARM_BOOT_SUPPORT
int bcm_linkscan_sync(int unit, int sync);
#endif
#endif /* _BCM_INT_LINK_H_ */
