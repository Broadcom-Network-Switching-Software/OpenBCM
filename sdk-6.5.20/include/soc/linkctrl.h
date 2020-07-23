/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        linkctrl.h
 * Purpose:     Hardware Linkscan Control module
 */

#ifndef   _SOC_LINKCTRL_H_
#define   _SOC_LINKCTRL_H_

#include <soc/types.h>


/*
 * Provides port number defined in PC MIIM registers for
 * write/read of a port PHY link status.
 * Port values in MIIM are fixed based on the port physical
 * block type/instance/index.
 */
typedef struct soc_linkctrl_port_info_s {
    int     block_type;
    int     block_number;
    int     block_index;
    int     port;
} soc_linkctrl_port_info_t;


/*
 * Driver specific routines
 *
 * (See description for routines below)
 */
typedef struct soc_linkctrl_driver_s {
    soc_linkctrl_port_info_t  *port_info;  /* null indicates no mapping */
    int  (*ld_linkscan_hw_init)(int);
    int  (*ld_linkscan_config)(int, pbmp_t, pbmp_t);
    int  (*ld_linkscan_pause)(int);
    int  (*ld_linkscan_continue)(int);
    int  (*ld_update)(int);
    int  (*ld_hw_link_get)(int, pbmp_t *);
} soc_linkctrl_driver_t;

/*
 * Driver specific routines description
 *
 * Function:
 *     ld_linkscan_hw_init
 * Purpose:
 *     Initialize device specific HW linkscan.
 * Parameters:
 *     unit - Device unit number
 *
 *
 * Function:
 *     ld_linkscan_config
 * Purpose:
 *     Set ports to hardware linkscan.
 * Parameters:
 *     unit       - Device unit number
 *     mii_pbm    - Port bitmap of ports to scan with MIIM registers
 *     direct_pbm - Port bitmap of ports to scan using NON MII
 *
 *
 * Function:
 *     ld_linkscan_pause
 * Purpose:
 *     Pause hardware link scanning, without disabling it.
 *     This call is used to pause scanning temporarily.
 * Parameters:
 *     unit - Device unit number
 *
 *
 * Function:
 *     ld_linkscan_continue
 * Purpose:
 *     Continue hardware link scanning after it has been paused.
 * Parameters:
 *     unit - Device unit number
 *
 *
 * Function:    
 *     ld_update
 * Purpose:
 *     Update the forwarding state in device.
 * Parameters:  
 *     unit - Device unit number
 */


extern int soc_linkctrl_init(int unit, CONST soc_linkctrl_driver_t *driver);
extern int soc_linkctrl_deinit(int unit);

/* These are called within src/bcm/ source files */
extern int soc_linkctrl_link_fwd_set(int unit, pbmp_t fwd);
extern int soc_linkctrl_link_fwd_get(int unit, pbmp_t *fwd);
extern int soc_linkctrl_link_mask_set(int unit, pbmp_t mask);
extern int soc_linkctrl_link_mask_get(int unit, pbmp_t *mask);

/* These are called within src/soc/ and src/bcm/ source files */
extern int soc_linkctrl_linkscan_register(int unit, void (*f)(int));
extern int soc_linkctrl_linkscan_hw_init(int unit);
extern int soc_linkctrl_linkscan_config(int unit, pbmp_t hw_mii_pbm,
                                        pbmp_t hw_direct_pbm);
extern int soc_linkctrl_linkscan_pause(int unit);
extern int soc_linkctrl_linkscan_continue(int unit);

extern int soc_linkctrl_miim_port_get(int unit, soc_port_t port,
                                      int *miim_port);
extern int soc_linkctrl_hw_link_get(int unit, soc_pbmp_t *hw_link);
#endif /* _SOC_LINKCTRL_H_ */
