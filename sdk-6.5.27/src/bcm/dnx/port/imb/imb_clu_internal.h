/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef IMB_CLU_INTERNAL_H_INCLUDED
/*
 * {
 */
#define IMB_CLU_INTERNAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (J2C) family only!"
#endif

int imb_clu_rx_pm_enable_hw_set(
    int unit,
    int clu_id,
    int pm_id,
    uint32 value);

int imb_clu_rx_pm_port_mode_hw_set(
    int unit,
    int clu_id,
    int pm_id,
    int mac_mode,
    int enable);

int imb_clu_rx_pm_aligner_reset_hw_set(
    int unit,
    int clu_id,
    int pm_id,
    uint32 value);

int imb_clu_rx_pm_reset_hw_set(
    int unit,
    int clu_id,
    int pm_id,
    uint32 in_reset);

int imb_clu_tx_start_thr_hw_set(
    int unit,
    uint32 clu_id,
    int first_lane_in_port,
    uint32 start_thr);

int imb_clu_rx_gear_shift_reset_hw_set(
    int unit,
    uint32 clu_id,
    int first_lane_in_port,
    int reset);

int imb_clu_rx_prd_port_profile_map_hw_set(
    int unit,
    uint32 clu_id,
    int first_lane_in_port,
    int prd_profile);
int imb_clu_prd_threshold_hw_set(
    int unit,
    int clu_id,
    uint32 rmc,
    uint32 priority,
    uint32 threshold);
int imb_clu_prd_threshold_hw_get(
    int unit,
    int clu_id,
    uint32 rmc,
    uint32 priority,
    uint32 *threshold);
int imb_clu_eee_enable_set(
    int unit,
    int clu_id,
    int pm_id,
    int enable);
int imb_clu_internal_pm_reset_get(
    int unit,
    int clu_id,
    int pm_in_clu,
    int *in_reset);
#undef _ERR_MSG_MODULE_NAME
#endif /* IMB_CLU_INTERNAL_H_INCLUDED */
