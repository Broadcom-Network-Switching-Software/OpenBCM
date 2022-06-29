/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        link.h
 * Purpose:     LINK internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNXF_LINK_H_
#define   _BCM_INT_DNXF_LINK_H_

#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF (Ramon) family only!"
#endif

int dnxf_linkscan_init(
    int unit);

int dnxf_linkscan_deinit(
    int unit);

int dnxf_linkscan_thread_stop(
    int unit);

int bcm_dnxf_linkscan_enable_set(
    int unit,
    int us);

int bcm_dnxf_linkscan_enable_get(
    int unit,
    int *us);

int bcm_dnxf_linkscan_mode_set(
    int unit,
    bcm_port_t port,
    int mode);

int bcm_dnxf_linkscan_mode_get(
    int unit,
    bcm_port_t port,
    int *mode);

int bcm_dnxf_linkscan_mode_set_pbm(
    int unit,
    bcm_pbmp_t pbm,
    int mode);

int bcm_dnxf_linkscan_detach(
    int unit);

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

#endif /*_BCM_INT_DNXF_LINK_H_*/
