/*! \file bcmbd_cmicx_hotswap.c
 *
 * Hotswap management for iProc/CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>

#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_cmicx_hotswap.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

#define HSWP_IDLE_USEC  100
#define HSWP_READY_USEC 100

#define FSM_IDLE        0
#define FSM_ACTIVE      1
#define FSM_WAIT_IDLE   2
#define FSM_DUMMY_RESP  3

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_hotswap_recover(int unit)
{
    int ioerr = 0;
    int rv;
    PAXB_0_PAXB_HOTSWAP_STATr_t hswp_stat;
    PAXB_0_PAXB_HOTSWAP_CTRLr_t hswp_ctrl;
    PAXB_0_PAXB_HOTSWAP_DEBUG_CTRLr_t hswp_dbg_ctrl;
    PAXB_0_PAXB_HOTSWAP_DEBUG_STATr_t hswp_dbg_stat;
    PAXB_0_PAXB_INTR_ENr_t intr_en;
    PAXB_0_PAXB_INTR_STATUSr_t intr_stat;
    CMICX_M0_IDM_IDM_RESET_STATUSr_t cmic_rst_stat;
    AXI_PCIE_S0_IDM_IDM_RESET_STATUSr_t axi_rst_stat;
    shr_timeout_t to;
    int hswp_cause, fsm_state;
    uint32_t intr_mask;

    SHR_FUNC_ENTER(unit);


    ioerr += READ_PAXB_0_PAXB_HOTSWAP_DEBUG_CTRLr(unit, &hswp_dbg_ctrl);
    ioerr += READ_PAXB_0_PAXB_HOTSWAP_DEBUG_STATr(unit, &hswp_dbg_stat);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Before Recovery  Hot Swap debug ctrl = 0x%x, \
                             Hot swap debug stat =  0x%x\n"),
                             PAXB_0_PAXB_HOTSWAP_DEBUG_CTRLr_GET(hswp_dbg_ctrl),
                             PAXB_0_PAXB_HOTSWAP_DEBUG_STATr_GET(hswp_dbg_stat)));

    /* Enable hotswap manager to handle CPU hotswap */
    ioerr += READ_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, &hswp_ctrl);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Hot Swap ctrl before Recovery = 0x%x\n"),
                             PAXB_0_PAXB_HOTSWAP_CTRLr_GET(hswp_ctrl)));
    PAXB_0_PAXB_HOTSWAP_CTRLr_ENABLEf_SET(hswp_ctrl, 1);
    ioerr += WRITE_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, hswp_ctrl);

    /* Hot Swap status before Recovery */
    ioerr += READ_PAXB_0_PAXB_HOTSWAP_STATr(unit, &hswp_stat);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Hot Swap status before Recovery = 0x%x\n"),
                             PAXB_0_PAXB_HOTSWAP_STATr_GET(hswp_stat)));

    hswp_cause = PAXB_0_PAXB_HOTSWAP_STATr_HOTSWAP_CAUSEf_GET(hswp_stat);
    fsm_state = PAXB_0_PAXB_HOTSWAP_STATr_FSM_STATEf_GET(hswp_stat);

    /* Determine if hotswap event has happened */
    if ((fsm_state == 0) && (hswp_cause == 0)) {
        /* No hotswap events detected */
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "No pending hotswap events.\n")));
        SHR_EXIT();
    }

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "Recovering pending hotswap events (0x%x).\n"),
                 hswp_cause));

    /* Save PAXB interrupt enable mask and disable all interrupts */
    ioerr += READ_PAXB_0_PAXB_INTR_ENr(unit, &intr_en);
    intr_mask = PAXB_0_PAXB_INTR_ENr_GET(intr_en);
    PAXB_0_PAXB_INTR_ENr_CLR(intr_en);
    ioerr += WRITE_PAXB_0_PAXB_INTR_ENr(unit, intr_en);

    /* Clear any active PAXB interrupts */
    ioerr += READ_PAXB_0_PAXB_INTR_STATUSr(unit, &intr_stat);
    ioerr += WRITE_PAXB_0_PAXB_INTR_STATUSr(unit, intr_stat);

    /* Wait for idle state */
    shr_timeout_init(&to, HSWP_IDLE_USEC, 0);
    rv = SHR_E_BUSY;
    while (rv == SHR_E_BUSY) {
        if (shr_timeout_check(&to)) {
            rv = SHR_E_TIMEOUT;
        }
        ioerr += READ_PAXB_0_PAXB_HOTSWAP_STATr(unit, &hswp_stat);
        fsm_state = PAXB_0_PAXB_HOTSWAP_STATr_FSM_STATEf_GET(hswp_stat);
        if (fsm_state == FSM_IDLE || fsm_state == FSM_DUMMY_RESP) {
            rv = SHR_E_NONE;
        }
    }
    if (rv == SHR_E_TIMEOUT) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                            "Hot Swap Wait Time Out for IDLE/DUMMYRESP\n")));
    }
    SHR_IF_ERR_EXIT(rv);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Hotswap logic IDLE/DUMMYRESP done in %d usec.\n"),
                 shr_timeout_elapsed(&to)));

    /* Signal that hotswap processing complete */
    ioerr += READ_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, &hswp_ctrl);
    PAXB_0_PAXB_HOTSWAP_CTRLr_SERVICE_COMPLETEf_SET(hswp_ctrl, 1);
    ioerr += WRITE_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, hswp_ctrl);

    /* Wait for PAXB ready state */
    shr_timeout_init(&to, HSWP_READY_USEC, 0);
    rv = SHR_E_BUSY;
    while (rv == SHR_E_BUSY) {
        if (shr_timeout_check(&to)) {
            rv = SHR_E_TIMEOUT;
        }
        ioerr += READ_PAXB_0_PAXB_HOTSWAP_STATr(unit, &hswp_stat);
        if (PAXB_0_PAXB_HOTSWAP_STATr_PAXB_RDYf_GET(hswp_stat)) {
            rv = SHR_E_NONE;
        }
    }
    if (rv == SHR_E_TIMEOUT) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                            "Hot Swap Wait Time Out for READY\n")));
        ioerr += READ_CMICX_M0_IDM_IDM_RESET_STATUSr(unit, &cmic_rst_stat);
        ioerr += READ_AXI_PCIE_S0_IDM_IDM_RESET_STATUSr(unit, &axi_rst_stat);
        if (ioerr) {
            SHR_ERR_EXIT(SHR_E_IO);
        }
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "CMICX_M0_IDM_IDM_RESET_STATUSr=0x%x.\n"
                             "AXI_PCIE_S0_IDM_IDM_RESET_STATUSr=0x%x.\n"),
                  CMICX_M0_IDM_IDM_RESET_STATUSr_GET(cmic_rst_stat),
                  AXI_PCIE_S0_IDM_IDM_RESET_STATUSr_GET(axi_rst_stat)));
    }
    SHR_IF_ERR_EXIT(rv);
    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "PCI subsystem READY in %d usec.\n"),
                 shr_timeout_elapsed(&to)));

    /* Clear hotswap status */
    ioerr += READ_PAXB_0_PAXB_HOTSWAP_STATr(unit, &hswp_stat);
    ioerr += WRITE_PAXB_0_PAXB_HOTSWAP_STATr(unit, hswp_stat);

    /* Clear any active PAXB interrupts */
    ioerr += READ_PAXB_0_PAXB_INTR_STATUSr(unit, &intr_stat);
    ioerr += WRITE_PAXB_0_PAXB_INTR_STATUSr(unit, intr_stat);

    /* Restore PAXB interrupt enable mask */
    PAXB_0_PAXB_INTR_ENr_SET(intr_en, intr_mask);
    ioerr += WRITE_PAXB_0_PAXB_INTR_ENr(unit, intr_en);

    if (ioerr) {
        SHR_ERR_EXIT(SHR_E_IO);
    }

exit:
    SHR_FUNC_EXIT();
}
