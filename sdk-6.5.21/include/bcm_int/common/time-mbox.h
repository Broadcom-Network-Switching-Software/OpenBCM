/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains BroadSync Time Interface definitions internal to the BCM library.
 */

#ifndef _BCM_INT_TIME_MBOX_H
#define _BCM_INT_TIME_MBOX_H

#include <bcm/time.h>

int _bcm_time_bs_init(int unit, unsigned isOutput);
int _bcm_time_bs_frequency_offset_set(int unit, bcm_time_spec_t new_offset);
int _bcm_time_bs_phase_offset_set(int unit, bcm_time_spec_t new_offset);
int _bcm_time_bs_ntp_offset_set(int unit, bcm_time_spec_t new_offset);
int _bcm_time_bs_debug_1pps_set(int unit, uint8 enableOutput);
int _bcm_time_bs_status_get(int unit, int *lock_status);
int _bcm_time_bs_debug(uint32 flags);
int _bcm_time_bs_log_configure(int unit,
                               uint32 debug_mask, uint64 udp_log_mask,
                               bcm_mac_t src_mac, bcm_mac_t dest_mac,
                               uint16 tpid, uint16 vid, uint8 ttl,
                               bcm_ip_t src_addr, bcm_ip_t dest_addr,
                               uint16 udp_port);
int _bcm_time_bs_log_configure_get(int unit, bcm_time_bs_log_cfg_t *bs_log_cfg);
int _bcm_time_bs_time_get(int unit, bcm_time_spec_t* bs_time); /* get broadsync time */
int _bcm_time_bs_time_set(int unit, bcm_time_spec_t bs_time); /* set UTC time or system time for broadsync */

#endif /* _BCM_INT_TIME_MBOX_H */

