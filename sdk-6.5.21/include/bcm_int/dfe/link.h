/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        link.h
 * Purpose:     LINK internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DFE_LINK_H_
#define   _BCM_INT_DFE_LINK_H_

int  bcm_dfe_linkscan_enable_set(int unit, int us);
int  bcm_dfe_linkscan_mode_set(int unit,  bcm_port_t port,  int mode);
int  bcm_dfe_linkscan_mode_get(int unit,  bcm_port_t port,  int *mode);
int  bcm_dfe_linkscan_mode_set_pbm(int unit, bcm_pbmp_t pbm, int mode);
int bcm_dfe_linkscan_detach(int unit);

#endif /*_BCM_INT_DFE_LINK_H_*/
