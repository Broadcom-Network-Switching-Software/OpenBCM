/*! \file bcm56990_a0_pmac_cdport.c
 *
 * The CDPORT driver for BCM56990_A0.
 *
 * Each CDPORT on BCM56990_A0 only has 4 block ports, and its lane combination
 * has "even lane-boundary" limitation, i.e. the starting lane of a logical
 * port needs be on the even lane e.g. 0, 2, 4, 6.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmpmac/chip/bcm56990_a0_cdport_defs.h>
#include <bcmpmac/bcmpmac_cdport.h>
#include <bcmpmac/bcmpmac_cdport_gen3.h>
#include <shr/shr_timeout.h>

/*! The number of the PM physical ports. */
#define NUM_PM_PORTS 4

/*! The half number of the PM physical ports. */
#define HALF_NUM_PM_PORTS (NUM_PM_PORTS / 2)


/******************************************************************************
 * Private functions
 */

static int
bcm56990_a0_cdport_tx_flush(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                            uint32_t flush)
{
    int ioerr = 0;
    shr_timeout_t to;
    sal_usecs_t timeout_usec;
    CDMAC_TX_CTRLr_t tx_ctrl;
    CDMAC_TXFIFO_STATUSr_t cell_cnt;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);
    CDMAC_TXFIFO_STATUSr_CLR(cell_cnt);

    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_DISCARDf_SET(tx_ctrl, flush);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    if (flush == 0) {
        return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
    }

    timeout_usec = DRAIN_WAIT_MSEC * 1000;
    shr_timeout_init(&to, timeout_usec, 0);
    while (1) {
        ioerr += READ_CDMAC_TXFIFO_STATUSr(pa, port, &cell_cnt);
        if (ioerr || (CDMAC_TXFIFO_STATUSr_CELL_CNTf_GET(cell_cnt) == 0)) {
            break;
        }
        if (shr_timeout_check(&to)) {
            return SHR_E_BUSY;
        }
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static int
bcm56990_a0_cdport_op_exec(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           const char *op, uint32_t op_param)
{
    bcmpmac_drv_op_f op_func;
    const bcmpmac_drv_op_hdl_t ops[] = {
        { "port_reset", cdport_port_reset_set },
        { "mac_reset", cdport_mac_reset_set },
        { "rx_enable", cdport_rx_enable_set },
        { "tx_enable", cdport_tx_enable_set },
        { "tx_flush", bcm56990_a0_cdport_tx_flush },
        { "speed_set", cdport_speed_set },
        { "failover_toggle", cdport_failover_status_toggle},
        { "force_pfc_xon_set", cdmac_force_pfc_xon_set}
    };

    op_func = bcmpmac_drv_op_func_get(ops, COUNTOF(ops), op);
    if (!op_func) {
        return SHR_E_UNAVAIL;
    }

    return op_func(pa, port, op_param);
}

static int
bcm56990_a0_cdport_port_init(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             uint32_t init)
{
    int rv, ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    rv = cdport_port_init(pa, port, init);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Set TX_THRESHOLD as 2 for BCM56990. */
    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_TX_THRESHOLDf_SET(tx_ctrl, 2);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

/******************************************************************************
 * Public functions
 */

bcmpmac_drv_t bcm56990_a0_pmac_cdport = {
    .name = "cdport",
    .port_reset_set = cdport_port_reset_set,
    .port_reset_get = cdport_port_reset_get,
    .port_enable_set = cdport_port_enable_set,
    .port_enable_get = cdport_port_enable_get,
    .port_init = bcm56990_a0_cdport_port_init,
    .mac_reset_set = cdport_mac_reset_set,
    .mac_reset_get = cdport_mac_reset_get,
    .rx_enable_set = cdport_rx_enable_set,
    .rx_enable_get = cdport_rx_enable_get,
    .tx_enable_set = cdport_tx_enable_set,
    .tx_enable_get = cdport_tx_enable_get,
    .speed_set = cdport_speed_set,
    .speed_get = cdport_speed_get,
    .encap_set = cdport_encap_set,
    .encap_get = cdport_encap_get,
    .lpbk_set = cdport_lpbk_set,
    .lpbk_get = cdport_lpbk_get,
    .pause_set = cdport_pause_set,
    .pause_get = cdport_pause_get,
    .pause_addr_set = cdport_pause_addr_set,
    .pause_addr_get = cdport_pause_addr_get,
    .frame_max_set = cdport_frame_max_set,
    .frame_max_get = cdport_frame_max_get,
    .fault_status_get = cdport_fault_status_get,
    .core_mode_set = cdport_gen3_core_mode_set,
    .op_exec = bcm56990_a0_cdport_op_exec,
    .failover_enable_set = cdport_failover_enable_set,
    .failover_enable_get = cdport_failover_enable_get,
    .failover_loopback_get = cdport_failover_lpbk_get,
    .failover_loopback_remove = cdport_failover_lpbk_remove,
    .pfc_set = cdport_pfc_set,
    .pfc_get = cdport_pfc_get,
    .port_mode_set = cdport_gen3_port_mode_set,
    .fault_disable_set = cdport_fault_disable_set,
    .fault_disable_get = cdport_fault_disable_get,
    .avg_ipg_set = cdport_avg_ipg_set,
    .avg_ipg_get = cdport_avg_ipg_get,
    .interrupt_enable_set = cdport_interrupt_enable_set,
    .interrupt_enable_get = cdport_interrupt_enable_get,
    .mib_oversize_set = cdmac_mib_oversize_set,
    .mib_oversize_get = cdmac_mib_oversize_get,
    .pass_pause_set = cdmac_pass_pause_frame_set,
    .pass_pause_get = cdmac_pass_pause_frame_get,
    .pass_pfc_set = cdmac_pass_pfc_frame_set,
    .pass_pfc_get = cdmac_pass_pfc_frame_get,
    .pass_control_set = cdmac_pass_control_frame_set,
    .pass_control_get = cdmac_pass_control_frame_get,
    .discard_set = cdmac_discard_set,
    .discard_get = cdmac_discard_get,
    .sw_link_status_set = cdmac_sw_link_status_set,
    .sw_link_status_get = cdmac_sw_link_status_get,
    .link_status_select_set = cdmac_sw_link_status_select_set,
    .link_status_select_get = cdmac_sw_link_status_select_get,
    .link_interrupt_ordered_set = cdmac_link_interrupt_ordered_set_enable,
    .link_interrupt_ordered_get = cdmac_link_interrupt_ordered_set_enable_get,
    .link_interrupt_live_status_get = cdmac_link_interruption_live_status_get,
    .port_link_status_get = cdport_gen3_link_status_get,
    .port_mac_control_set = cdport_gen3_cdmac_control_set,
    .port_mac_control_get = cdport_gen3_cdmac_control_get,
    .tsc_ctrl_set = cdport_gen3_tsc_ctrl_set,
    .tsc_ctrl_get = cdport_gen3_tsc_ctrl_get,
    .runt_threshold_set = cdmac_runt_threshold_set,
    .runt_threshold_get = cdmac_runt_threshold_get,
    .force_pfc_xon_set = cdmac_force_pfc_xon_set,
    .force_pfc_xon_get = cdmac_force_pfc_xon_get,
    .stall_tx_enable_get = cdmac_stall_tx_enable_get,
    .stall_tx_enable_set = cdmac_stall_tx_enable_set,
    .rsv_selective_mask_get = cdmac_rsv_selective_mask_get,
    .rsv_selective_mask_set = cdmac_rsv_selective_mask_set,
    .rsv_mask_get = cdmac_rsv_mask_get,
    .rsv_mask_set = cdmac_rsv_mask_set,
};
