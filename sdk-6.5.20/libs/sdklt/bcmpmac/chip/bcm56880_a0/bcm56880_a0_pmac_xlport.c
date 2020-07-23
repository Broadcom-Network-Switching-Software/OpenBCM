/*! \file bcm56880_a0_pmac_xlport.c
 *
 * The XLPORT driver for BCM56880.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmpmac/chip/bcm56880_a0_xlport_defs.h>
#include <bcmpmac/bcmpmac_xlport.h>
#include <bcmpmac/bcmpmac_xlport_gen2.h>


/******************************************************************************
 * Private functions
 */

static int
bcm56880_a0_xlport_port_init(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             uint32_t init)
{
    int rv, ioerr = 0;
    XLMAC_TX_CTRLr_t tx_ctrl;
    bcmpmac_egr_timestamp_mode_t timestamp_mode;

    rv = xlport_port_init(pa, port, init);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Use CRC_MODE_REPLACE for BCM56880. */
    ioerr += READ_XLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    XLMAC_TX_CTRLr_CRC_MODEf_SET(tx_ctrl, XLMAC_CRC_MODE_REPLACE);
    ioerr += WRITE_XLMAC_TX_CTRLr(pa, port, tx_ctrl);

    /* Enable 48-bit 1588 egress timestamping. */
    timestamp_mode = BCMPMAC_48_BIT_EGRESS_TIMESTAMP;

    rv = xlport_egr_timestap_mode_set(pa, port, timestamp_mode);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


/******************************************************************************
 * Public functions
 */

bcmpmac_drv_t bcm56880_a0_pmac_xlport = {
    .name = "xlport",
    .port_reset_set = xlport_port_reset_set,
    .port_reset_get = xlport_port_reset_get,
    .port_enable_set = xlport_port_enable_set,
    .port_enable_get = xlport_port_enable_get,
    .port_init = bcm56880_a0_xlport_port_init,
    .mac_reset_set = xlport_mac_reset_set,
    .mac_reset_get = xlport_mac_reset_get,
    .rx_enable_set = xlport_rx_enable_set,
    .rx_enable_get = xlport_rx_enable_get,
    .tx_enable_set = xlport_tx_enable_set,
    .tx_enable_get = xlport_tx_enable_get,
    .speed_set = xlport_speed_set,
    .speed_get = xlport_speed_get,
    .encap_set = xlport_encap_set,
    .encap_get = xlport_encap_get,
    .lpbk_set = xlport_lpbk_set,
    .lpbk_get = xlport_lpbk_get,
    .pause_set = xlport_pause_set,
    .pause_get = xlport_pause_get,
    .pause_addr_set = xlport_pause_addr_set,
    .pause_addr_get = xlport_pause_addr_get,
    .frame_max_set = xlport_frame_max_set,
    .frame_max_get = xlport_frame_max_get,
    .fault_status_get = xlport_fault_status_get,
    .core_mode_set = xlport_core_mode_set,
    .op_exec = xlport_op_exec,
    .failover_enable_set = xlport_failover_enable_set,
    .failover_enable_get = xlport_failover_enable_get,
    .failover_loopback_get = xlport_failover_lpbk_get,
    .failover_loopback_remove = xlport_failover_lpbk_remove,
    .pfc_set = xlport_pfc_set,
    .pfc_get = xlport_pfc_get,
    .port_mode_set = xlport_port_mode_set,
    .pass_pause_set = xlmac_gen2_pass_pause_frame_set,
    .pass_pause_get = xlmac_gen2_pass_pause_frame_get,
    .pass_pfc_set = xlmac_gen2_pass_pfc_frame_set,
    .pass_pfc_get = xlmac_gen2_pass_pfc_frame_get,
    .pass_control_set = xlmac_gen2_pass_control_frame_set,
    .pass_control_get = xlmac_gen2_pass_control_frame_get,
    .fault_disable_set = xlport_fault_disable_set,
    .fault_disable_get = xlport_fault_disable_get,
    .avg_ipg_set = xlport_avg_ipg_set,
    .avg_ipg_get = xlport_avg_ipg_get,
    .mib_oversize_set = xlport_mib_oversize_set,
    .mib_oversize_get = xlport_mib_oversize_get,
    .tsc_ctrl_set = xlport_tsc_ctrl_set,
    .tsc_ctrl_get = xlport_tsc_ctrl_get,
    .runt_threshold_set = xlmac_runt_threshold_set,
    .runt_threshold_get = xlmac_runt_threshold_get,
    .force_pfc_xon_set = xlmac_force_pfc_xon_set,
    .force_pfc_xon_get = xlmac_force_pfc_xon_get,
    .rsv_selective_mask_get = xlport_rsv_selective_mask_get,
    .rsv_selective_mask_set = xlport_rsv_selective_mask_set,
    .rsv_mask_get = xlport_rsv_mask_get,
    .rsv_mask_set = xlport_rsv_mask_set,
    .tx_timestamp_info_get = xlmac_tx_timestamp_info_get,
    .egress_timestamp_mode_get = xlport_egr_timestap_mode_get,
    .egress_timestamp_mode_set = xlport_egr_timestap_mode_set
};

