/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pfc.h
 * Purpose:     Declarations for PFC BCM Internal functions/data structures.
 */

#ifndef _BCM_INT_TOMAHAWK3_PFC_H_
#define _BCM_INT_TOMAHAWK3_PFC_H_
#include <soc/defs.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/tomahawk3_dispatch.h>
#include <bcm_int/esw/tomahawk3.h>


int
bcm_th3_port_priority_group_config_set(
    int unit,
    bcm_gport_t gport,
    int priority_group,
    bcm_port_priority_group_config_t *prigrp_config);

int
bcm_th3_port_priority_group_config_get(
    int unit,
    bcm_gport_t gport,
    int priority_group,
    bcm_port_priority_group_config_t *prigrp_config);

int
bcm_th3_mmu_pfc_tx_config_set(
    int unit,
    bcm_port_t port,
    int pfc_enable,
    uint32 pri_bmp);

int
bcm_th3_mmu_pfc_rx_config_set(
    int unit,
    bcm_port_t port,
    int pfc_enable);



#endif
#endif
