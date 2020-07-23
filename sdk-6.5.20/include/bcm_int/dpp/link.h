/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        link.h
 * Purpose:     LINK internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_LINK_H_
#define   _BCM_INT_DPP_LINK_H_

extern int bcm_common_linkscan_mode_set(int,bcm_port_t,int);
extern int bcm_petra_linkscan_mode_get(int unit, bcm_port_t port, int *mode);

extern int bcm_common_linkscan_trigger_event_set(int unit,
                                                bcm_port_t port,
                                                uint32 flags,
                                                bcm_linkscan_trigger_event_t trigger_event,
                                                int enable);
extern int bcm_common_linkscan_trigger_event_get(int unit,
                                                bcm_port_t port,
                                                uint32 flags,
                                                bcm_linkscan_trigger_event_t trigger_event,
                                                int *enable);

#endif /* _BCM_INT_PETRA_LINK_H_ */
