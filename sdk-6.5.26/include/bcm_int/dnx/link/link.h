/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        link.h
 * Purpose:     LINK internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DNX_LINK_H_
#define   _BCM_INT_DNX_LINK_H_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shr_thread_manager.h>

typedef struct
{
    /*
     * SW FLR data access lock 
     */
    sal_mutex_t sw_flr_lock;
    /*
     * SW FLR thread handle 
     */
    shr_thread_manager_handler_t handler;

    /*
     * Ports needed to polling on SW FLR thread 
     */
    pbmp_t pbmp_sw_flr;
    /*
     * FLR timeout window in us 
     */
    sal_usecs_t timeout_usecs[SOC_MAX_NUM_PORTS];

    /*
     * Indicate if FLR ports is currently up 
     */
    pbmp_t pbmp_link;
    /*
     * Indicate if FLR ports is in FLR delay check state 
     */
    pbmp_t pbm_flr_delay_check;
    /*
     * Record the time usecs when port was detect link down transition 
     */
    sal_usecs_t flr_down_usecs[SOC_MAX_NUM_PORTS];

    /*
     * Indicate if last link up is successfully FLR up 
     */
    pbmp_t pbmp_flr_up;

    /*
     * Indicate there is FLR fail link down, This link down event must be triggered to linkscan thread 
     */
    pbmp_t pbmp_flr_fail;

    /*
     * Record the delay for link down->up Flap in successful FLR 
     */
    sal_usecs_t flr_up_delay[SOC_MAX_NUM_PORTS];

    /*
     * Below is the debug counter on FLR process 
     */
    uint32 flr_counter[SOC_MAX_NUM_PORTS];
    uint32 flr_fail_counter[SOC_MAX_NUM_PORTS];
    uint32 delay_check_counter[SOC_MAX_NUM_PORTS];
    uint32 keep_up_counter[SOC_MAX_NUM_PORTS];
    uint32 keep_down_counter[SOC_MAX_NUM_PORTS];
    uint32 down2up_counter[SOC_MAX_NUM_PORTS];
    uint32 up2down_counter[SOC_MAX_NUM_PORTS];
} dnx_sw_flr_info_t;

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
extern int bcm_dnx_linkscan_flr_enable_set(
    int unit,
    bcm_port_t port,
    int enable,
    bcm_port_flr_timeout_t timeout);
int dnx_flr_sw_linkscan_thread_init(
    int unit);
int dnx_flr_sw_linkscan_thread_deinit(
    int unit);
int dnx_port_flr_sw_linkscan_update(
    int unit,
    int port);
int dnx_flr_sw_link_dump(
    int unit,
    int clear);

#endif /* _BCM_INT_DNX_LINK_H_ */
