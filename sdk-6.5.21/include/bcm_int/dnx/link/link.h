/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        link.h
 * Purpose:     LINK internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNX_LINK_H_
#define   _BCM_INT_DNX_LINK_H_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

extern int bcm_common_linkscan_mode_set(
    int,
    bcm_port_t,
    int);
extern int bcm_dnx_linkscan_mode_get(
    int unit,
    bcm_port_t port,
    int *mode);

extern int bcm_common_linkscan_trigger_event_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_linkscan_trigger_event_t trigger_event,
    int enable);
extern int bcm_common_linkscan_trigger_event_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_linkscan_trigger_event_t trigger_event,
    int *enable);
extern int dnx_linkscan_init(
    int unit);
extern int dnx_linkscan_deinit(
    int unit);
extern int dnx_linkscan_port_init(
    int unit);
extern int dnx_linkscan_port_remove(
    int unit);
extern int bcm_common_linkscan_detach(
    int unit);

#endif /* _BCM_INT_DNX_LINK_H_ */
