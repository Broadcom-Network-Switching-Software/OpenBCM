/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        helix5_mac_flexport.h
 * Purpose:
 * Requires:
 */

#ifndef HELIX5_MAC_FLEXPORT_H
#define HELIX5_MAC_FLEXPORT_H

extern int soc_helix5_flex_mac_rx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_helix5_flex_mac_tx_port_down(
    int unit, soc_port_schedule_state_t *port_schedule_state);


extern int soc_helix5_flex_mac_port_up(
    int unit, soc_port_schedule_state_t *port_schedule_state);


#endif
