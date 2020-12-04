/*! \file bcmpmac_xlport_gen2.h
 *
 * XLPORT gen2 driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_XLPORT_GEN2_H
#define BCMPMAC_XLPORT_GEN2_H

#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpmac/bcmpmac_util_internal.h>
#include <shr/shr_util.h>


/*******************************************************************************
* Local definitions
 */

static inline int
xlmac_gen2_pass_control_frame_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                  uint32_t enable)
{
    int ioerr = 0;
    XLMAC_RX_CTRLr_t rx_ctrl;

    /*
     * This configuration is used to drop or pass all control frames
     * (with ether type 0x8808) except pause packets.
     * If set, all control frames are passed to system side.
     * if reset, control frames (including pfc frames wih ether type 0x8808)i
     * are dropped in XLMAC.
     */
    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    XLMAC_RX_CTRLr_RX_PASS_CTRLf_SET(rx_ctrl, (enable? 1: 0));
    ioerr += WRITE_XLMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
xlmac_gen2_pass_control_frame_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                  uint32_t *enable)
{
    int ioerr = 0;
    XLMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *enable = XLMAC_RX_CTRLr_RX_PASS_CTRLf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
xlmac_gen2_pass_pfc_frame_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t enable)
{
    int ioerr = 0;
    XLMAC_RX_CTRLr_t rx_ctrl;

    /*
     * This configuration is used to pass or drop PFC packets when
     * PFC_ETH_TYPE is not equal to 0x8808.
     * If set, PFC frames are passed to system side.
     * If reset, PFC frames are dropped in XLMAC.
     */
    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    XLMAC_RX_CTRLr_RX_PASS_PFCf_SET(rx_ctrl, (enable? 1: 0));
    ioerr += WRITE_XLMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
xlmac_gen2_pass_pfc_frame_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t *enable)
{
    int ioerr = 0;
    XLMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *enable = XLMAC_RX_CTRLr_RX_PASS_PFCf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
xlmac_gen2_pass_pause_frame_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                uint32_t enable)
{
    int ioerr = 0;
    XLMAC_RX_CTRLr_t rx_ctrl;

    /*
     * If set, PAUSE frames are passed to sytem side.
     * If reset, PAUSE frames are dropped in XLMAC
     */
    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    XLMAC_RX_CTRLr_RX_PASS_PAUSEf_SET(rx_ctrl, (enable? 1: 0));
    ioerr += WRITE_XLMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
xlmac_gen2_pass_pause_frame_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                uint32_t *enable)
{
    int ioerr = 0;
    XLMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *enable = XLMAC_RX_CTRLr_RX_PASS_PAUSEf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

#endif /* BCMPMAC_XLPORT_GEN2_H */
