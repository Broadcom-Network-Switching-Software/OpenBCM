/*! \file bcm56780_a0_pmac_cdport.c
 *
 * The CDPORT driver for BCM56780.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmpmac/chip/bcm56780_a0_cdport_defs.h>
#include <bcmpmac/bcmpmac_cdport.h>
#include <bcmpmac/bcmpmac_cdport_gen2.h>
#include <shr/shr_timeout.h>


/******************************************************************************
 * Private functions
 */

static int
bcm56780_a0_cdport_port_init(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             uint32_t init)
{
    int rv, ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;
    CDPORT_FLOW_CONTROL_CONFIGr_t flowctrl;
    CDMAC_MEM_CTRLr_t mem_ctrl;
    uint32_t cdc_mem_ctrl = 0;

    rv = cdport_port_init(pa, port, init);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    /* Enable flow control. */
    ioerr += READ_CDPORT_FLOW_CONTROL_CONFIGr(pa, port, &flowctrl);
    CDPORT_FLOW_CONTROL_CONFIGr_MERGE_MODE_ENf_SET(flowctrl, 1);
    ioerr += WRITE_CDPORT_FLOW_CONTROL_CONFIGr(pa, port, flowctrl);

    /* Use CRC_MODE_REPLACE for BCM56880. */
    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_CRC_MODEf_SET(tx_ctrl, CDMAC_CRC_MODE_REPLACE);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    /* Enable 16-byte credit granularity. */
    CDMAC_MEM_CTRLr_CLR(mem_ctrl);
    ioerr += READ_CDMAC_MEM_CTRLr(pa, port, &mem_ctrl);
    cdc_mem_ctrl = CDMAC_MEM_CTRLr_TX_CDC_MEM_CTRL_TMf_GET(mem_ctrl);
    cdc_mem_ctrl |= 0x10;
    CDMAC_MEM_CTRLr_TX_CDC_MEM_CTRL_TMf_SET(mem_ctrl, cdc_mem_ctrl);
    ioerr += WRITE_CDMAC_MEM_CTRLr(pa, port, mem_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static int
bcm56780_a0_cdport_tx_flush(bcmpmac_access_t *pa, bcmpmac_pport_t port,
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
bcm56780_a0_cdport_op_exec(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           const char *op, uint32_t op_param)
{
    bcmpmac_drv_op_f op_func;
    const bcmpmac_drv_op_hdl_t ops[] = {
        { "port_reset", cdport_port_reset_set },
        { "mac_reset", cdport_mac_reset_set },
        { "rx_enable", cdport_rx_enable_set },
        { "tx_enable", cdport_tx_enable_set },
        { "tx_flush", bcm56780_a0_cdport_tx_flush },
        { "speed_set", cdport_speed_set },
        { "failover_toggle", cdport_failover_status_toggle},
        { "force_pfc_xon_set", cdmac_force_pfc_xon_set},
        { "rx_strip_crc_set", cdmac_strip_crc_set},
        { "tx_threshold_set", cdmac_tx_threshold_set},
        { "tx_crc_mode", cdmac_tx_crc_mode_set}
    };

    op_func = bcmpmac_drv_op_func_get(ops, COUNTOF(ops), op);
    if (!op_func) {
        return SHR_E_UNAVAIL;
    }

    return op_func(pa, port, op_param);
}

static inline int
bcm56780_a0_cdmac_control_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t reset)
{
    int ioerr = 0;
    CDPORT_MAC_CONTROLr_t pmac_ctrl;

    CDPORT_MAC_CONTROLr_CLR(pmac_ctrl);
    ioerr += READ_CDPORT_MAC_CONTROLr(pa, port, &pmac_ctrl);

    if (reset) {
        CDPORT_MAC_CONTROLr_CDMAC0_RESETf_SET(pmac_ctrl, 1);
    } else {
        CDPORT_MAC_CONTROLr_CDMAC0_RESETf_SET(pmac_ctrl, 0);
    }
    ioerr += WRITE_CDPORT_MAC_CONTROLr(pa, port, pmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
bcm56780_a0_cdmac_control_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t *reset)
{
    int ioerr = 0;
    CDPORT_MAC_CONTROLr_t pmac_ctrl;

    CDPORT_MAC_CONTROLr_CLR(pmac_ctrl);
    ioerr += READ_CDPORT_MAC_CONTROLr(pa, port, &pmac_ctrl);
    *reset = CDPORT_MAC_CONTROLr_CDMAC0_RESETf_GET(pmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
bcm56780_a0_credit_granularity_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                   bool *enable)
{
    int ioerr = 0;
    CDMAC_MEM_CTRLr_t mem_ctrl;

    CDMAC_MEM_CTRLr_CLR(mem_ctrl);
    ioerr += READ_CDMAC_MEM_CTRLr(pa, port, &mem_ctrl);
    *enable = CDMAC_MEM_CTRLr_CREDIT_16B_GRANULARITYf_GET(mem_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
bcm56780_a0_credit_granularity_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                   bool enable)
{
    int ioerr = 0;
    CDMAC_MEM_CTRLr_t mem_ctrl;

    CDMAC_MEM_CTRLr_CLR(mem_ctrl);
    ioerr += READ_CDMAC_MEM_CTRLr(pa, port, &mem_ctrl);
    CDMAC_MEM_CTRLr_CREDIT_16B_GRANULARITYf_SET(mem_ctrl, enable? 1 : 0);
    ioerr += WRITE_CDMAC_MEM_CTRLr(pa, port, mem_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


/******************************************************************************
 * Public functions
 */

bcmpmac_drv_t bcm56780_a0_pmac_cdport = {
    .name = "cdport",
    .port_reset_set = cdport_port_reset_set,
    .port_reset_get = cdport_port_reset_get,
    .port_enable_set = cdport_port_enable_set,
    .port_enable_get = cdport_port_enable_get,
    .port_init = bcm56780_a0_cdport_port_init,
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
    .core_mode_set = cdport_gen2_core_mode_set,
    .op_exec = bcm56780_a0_cdport_op_exec,
    .failover_enable_set = cdport_failover_enable_set,
    .failover_enable_get = cdport_failover_enable_get,
    .failover_loopback_get = cdport_failover_lpbk_get,
    .failover_loopback_remove = cdport_failover_lpbk_remove,
    .pfc_set = cdport_pfc_set,
    .pfc_get = cdport_pfc_get,
    .port_mode_set = cdport_gen2_port_mode_set,
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
    .stall_tx_enable_get = cdmac_stall_tx_enable_get,
    .stall_tx_enable_set = cdmac_stall_tx_enable_set,
    .port_link_status_get = cdport_gen2_link_status_get,
    .port_mac_control_set = bcm56780_a0_cdmac_control_set,
    .port_mac_control_get = bcm56780_a0_cdmac_control_get,
    .tsc_ctrl_set = cdport_gen2_tsc_ctrl_set,
    .tsc_ctrl_get = cdport_gen2_tsc_ctrl_get,
    .runt_threshold_set = cdmac_runt_threshold_set,
    .runt_threshold_get = cdmac_runt_threshold_get,
    .force_pfc_xon_set = cdmac_force_pfc_xon_set,
    .force_pfc_xon_get = cdmac_force_pfc_xon_get,
    .rsv_selective_mask_get = cdmac_rsv_selective_mask_get,
    .rsv_selective_mask_set = cdmac_rsv_selective_mask_set,
    .rsv_mask_get = cdmac_rsv_mask_get,
    .rsv_mask_set = cdmac_rsv_mask_set,
    .credit_granularity_get = bcm56780_a0_credit_granularity_get,
    .credit_granularity_set = bcm56780_a0_credit_granularity_set,
    .strip_crc_get = cdmac_strip_crc_get,
    .strip_crc_set= cdmac_strip_crc_set,
    .tx_threshold_get = cdmac_tx_threshold_get,
    .tx_threshold_set = cdmac_tx_threshold_set,
    .tx_crc_mode_get = cdmac_tx_crc_mode_get,
    .tx_crc_mode_set = cdmac_tx_crc_mode_set
};
