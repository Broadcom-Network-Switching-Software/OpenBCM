/*! \file bcmpmac_cdport_gen3.h
 *
 * CDPORT driver.
 *
 * A CDPORT_GEN3 supports 4 ports at most.
 *
 * In this driver, we always use the port number 0~3 to access the CDPORT
 * and CDMAC per-port registers without considering which CDMAC should be
 * configured.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_CDPORT_GEN3_H
#define BCMPMAC_CDPORT_GEN3_H

#include <shr/shr_util.h>
#include <sal/sal_sleep.h>

static inline int
cdport_gen3_core_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          bcmpmac_core_port_mode_t *mode)
{
    int rv, ioerr = 0;
    CDPORT_MODE_REGr_t cdport_mode;
    uint32_t mode_mac;
    uint32_t lmap[BCMPMAC_NUM_PORTS_PER_PM_MAX] = { 0 };
    const bcmpmac_port_mode_map_t port_mode_map[] = {
        { { 0 }, CDPORT_PORT_MODE_QUAD },
        { { 0x3, 0xc, 0x30, 0xc0 }, CDPORT_PORT_MODE_QUAD },
        { { 0x1, 0x4, 0x10, 0x40 }, CDPORT_PORT_MODE_QUAD },
        { { 0x1, 0x4, 0xf, 0 }, CDPORT_PORT_MODE_TRI_012 },
        { { 0x3, 0xc, 0xf, 0 }, CDPORT_PORT_MODE_TRI_012 },
        { { 0xf, 0x0, 0x10, 0x40 }, CDPORT_PORT_MODE_TRI_023 },
        { { 0xf, 0x0, 0x30, 0xc0 }, CDPORT_PORT_MODE_TRI_023 },
        { { 0xf, 0x0, 0xf0, 0x0 }, CDPORT_PORT_MODE_DUAL },
        { { 0xff, 0 }, CDPORT_PORT_MODE_SINGLE }
    };

    CDPORT_MODE_REGr_CLR(cdport_mode);

    sal_memset(lmap, 0, sizeof(lmap));
    sal_memcpy(lmap, mode->lane_map, PORTS_PER_CDMAC * sizeof(uint32_t));
    rv = bcmpmac_port_mode_get(port_mode_map, COUNTOF(port_mode_map),
                               lmap, &mode_mac);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    ioerr += READ_CDPORT_MODE_REGr(pa, port, &cdport_mode);
    CDPORT_MODE_REGr_MAC_PORT_MODEf_SET(cdport_mode, mode_mac);

    ioerr += WRITE_CDPORT_MODE_REGr(pa, port, cdport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}



static inline int
cdport_gen3_8x100_core_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          bcmpmac_core_port_mode_t *mode)
{
    int rv, ioerr = 0;
    CDPORT_MODE_REGr_t cdport_mode;
    uint32_t mode_mac;
    uint32_t lmap[BCMPMAC_NUM_PORTS_PER_PM_MAX] = { 0 };
    const bcmpmac_port_mode_map_t port_mode_map[] = {
        { { 0 }, CDPORT_PORT_MODE_QUAD },
        { { 0x01, 0x02, 0x04, 0x08 }, CDPORT_PORT_MODE_QUAD },
        { { 0x10, 0x20, 0x40, 0x80 }, CDPORT_PORT_MODE_QUAD },
        { { 0x01, 0x02, 0x0c, 0 },    CDPORT_PORT_MODE_TRI_012 },
        { { 0x10, 0x20, 0xc0, 0 },    CDPORT_PORT_MODE_TRI_012 },
        { { 0x3,  0x0,  0x04, 0x08 }, CDPORT_PORT_MODE_TRI_023 },
        { { 0x30, 0x0,  0x40, 0x80 }, CDPORT_PORT_MODE_TRI_023 },
        { { 0x03, 0x0,  0x0c, 0x0 }, CDPORT_PORT_MODE_DUAL },
        { { 0x30, 0x0,  0xc0, 0x0 }, CDPORT_PORT_MODE_DUAL },
        { { 0x0f, 0x0,  0x00, 0x0 }, CDPORT_PORT_MODE_SINGLE },
        { { 0xf0, 0x0,  0x00, 0x0 }, CDPORT_PORT_MODE_SINGLE },
        { { 0xff, 0 }, CDPORT_PORT_MODE_SINGLE }
    };

    CDPORT_MODE_REGr_CLR(cdport_mode);

    sal_memset(lmap, 0, sizeof(lmap));
    sal_memcpy(lmap, mode->lane_map, PORTS_PER_CDMAC * sizeof(uint32_t));
    rv = bcmpmac_port_mode_get(port_mode_map, COUNTOF(port_mode_map),
                               lmap, &mode_mac);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    ioerr += READ_CDPORT_MODE_REGr(pa, port, &cdport_mode);
    CDPORT_MODE_REGr_MAC_PORT_MODEf_SET(cdport_mode, mode_mac);

    ioerr += WRITE_CDPORT_MODE_REGr(pa, port, cdport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
cdport_gen3_port_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          uint32_t flags, uint32_t lane_mask)
{
    int ioerr = 0, num_lanes = 0;
    uint32_t port_mode = CDPORT_PORT_MODE_QUAD;
    uint32_t new_port_mode = CDPORT_PORT_MODE_QUAD;
    CDPORT_MODE_REGr_t cdport_mode;

    CDPORT_MODE_REGr_CLR(cdport_mode);
    num_lanes = shr_util_popcount(lane_mask);
    ioerr += READ_CDPORT_MODE_REGr(pa, port, &cdport_mode);

    port_mode = CDPORT_MODE_REGr_MAC_PORT_MODEf_GET(cdport_mode);

    if (num_lanes == 8) {
        new_port_mode = CDPORT_PORT_MODE_SINGLE;
    } else if (num_lanes == 4) {
         switch (port_mode) {
            case CDPORT_PORT_MODE_QUAD:
                if (lane_mask == 0xf) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                }
                break;
            case CDPORT_PORT_MODE_TRI_012:
                if (lane_mask == 0xf) {
                    new_port_mode = CDPORT_PORT_MODE_DUAL;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                }
                break;
            case CDPORT_PORT_MODE_TRI_023:
                if (lane_mask == 0xf) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_DUAL;
                }
                break;
            case CDPORT_PORT_MODE_DUAL:
                new_port_mode = CDPORT_PORT_MODE_DUAL;
                break;
            case CDPORT_PORT_MODE_SINGLE:
                new_port_mode = CDPORT_PORT_MODE_DUAL;
                break;
            default:
                return SHR_E_PARAM;
        }
    } else if (num_lanes == 2) {
        switch (port_mode) {
            case CDPORT_PORT_MODE_QUAD:
                new_port_mode = CDPORT_PORT_MODE_QUAD;
                break;
            case CDPORT_PORT_MODE_TRI_012:
                if ((lane_mask == 0x3) || (lane_mask == 0xc)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_QUAD;
                }
                break;
            case CDPORT_PORT_MODE_TRI_023:
                if ((lane_mask & 0x3) || (lane_mask & 0xc)) {
                    new_port_mode = CDPORT_PORT_MODE_QUAD;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_DUAL:
                if ((lane_mask & 0x3) || (lane_mask & 0xc)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_SINGLE:
                new_port_mode = CDPORT_PORT_MODE_QUAD;
                break;
            default:
                return SHR_E_PARAM;
        }
    } else {
        /* num_lanes == 1 */
        switch (port_mode) {
            case CDPORT_PORT_MODE_QUAD:
                new_port_mode = CDPORT_PORT_MODE_QUAD;
                break;
            case CDPORT_PORT_MODE_TRI_012:
                if ((lane_mask == 0x1) || (lane_mask == 0x4)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_QUAD;
                }
                break;
            case CDPORT_PORT_MODE_TRI_023:
                if ((lane_mask & 0x1) || (lane_mask & 0x4)) {
                    new_port_mode = CDPORT_PORT_MODE_QUAD;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_DUAL:
                if ((lane_mask & 0x1) || (lane_mask & 0x4)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_SINGLE:
                new_port_mode = CDPORT_PORT_MODE_QUAD;
                break;
           default:
                return SHR_E_PARAM;
        }
    }

    CDPORT_MODE_REGr_MAC_PORT_MODEf_SET(cdport_mode, new_port_mode);
    ioerr += WRITE_CDPORT_MODE_REGr(pa, port, cdport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen3_8x100_port_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                uint32_t flags, uint32_t lane_mask)
{
    int ioerr = 0, num_lanes = 0;
    uint32_t port_mode = CDPORT_PORT_MODE_QUAD;
    uint32_t new_port_mode = CDPORT_PORT_MODE_QUAD;
    CDPORT_MODE_REGr_t cdport_mode;

    CDPORT_MODE_REGr_CLR(cdport_mode);
    num_lanes = shr_util_popcount(lane_mask);
    ioerr += READ_CDPORT_MODE_REGr(pa, port, &cdport_mode);

    port_mode = CDPORT_MODE_REGr_MAC_PORT_MODEf_GET(cdport_mode);

    if (num_lanes == 8) {
        new_port_mode = CDPORT_PORT_MODE_SINGLE;
    } else if (num_lanes == 4) {
        if (BCMPMAC_PM_PORT_MODE_MUTI_MPP_GET(flags)) {
            new_port_mode = CDPORT_PORT_MODE_SINGLE;
        } else {
            new_port_mode = CDPORT_PORT_MODE_TRI_023;
        }
    } else if (num_lanes == 2) {
        switch (port_mode) {
            case CDPORT_PORT_MODE_QUAD:
                if ((lane_mask == 0x0c) || (lane_mask == 0xc0)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else if ((lane_mask == 0x03) || (lane_mask == 0x30)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_TRI_012:
                if ((lane_mask == 0x3) || (lane_mask == 0x30)) {
                    new_port_mode = CDPORT_PORT_MODE_DUAL;
                } else if ((lane_mask == 0x0c) || (lane_mask == 0xc0)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                }
                break;
            case CDPORT_PORT_MODE_TRI_023:
                if ((lane_mask == 0x3) || (lane_mask == 0x30)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                } else if ((lane_mask == 0x0c) || (lane_mask == 0xc0)) {
                    new_port_mode = CDPORT_PORT_MODE_DUAL;
                }
                break;
            case CDPORT_PORT_MODE_DUAL:
                new_port_mode = CDPORT_PORT_MODE_DUAL;
                break;
            case CDPORT_PORT_MODE_SINGLE:
                if ((lane_mask == 0x3) || (lane_mask == 0x30)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                } else if ((lane_mask == 0xc) || (lane_mask == 0xc0)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                }
                break;
            default:
                return SHR_E_PARAM;
        }
    } else {
        /* num_lanes == 1 */
        switch (port_mode) {
            case CDPORT_PORT_MODE_QUAD:
                new_port_mode = CDPORT_PORT_MODE_QUAD;
                break;
            case CDPORT_PORT_MODE_TRI_012:
                if ((lane_mask == 0x1) || (lane_mask == 0x2) ||
                    (lane_mask == 0x10) || (lane_mask == 0x20) ) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_QUAD;
                }
                break;
            case CDPORT_PORT_MODE_TRI_023:
                if ((lane_mask == 0x1) || (lane_mask == 0x2) ||
                    (lane_mask == 0x10) || (lane_mask == 0x20)) {
                    new_port_mode = CDPORT_PORT_MODE_QUAD;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_DUAL:
                if ((lane_mask == 0x1) || (lane_mask == 0x2) ||
                    (lane_mask == 0x10) || (lane_mask == 0x20)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_SINGLE:
                new_port_mode = CDPORT_PORT_MODE_QUAD;
                break;
           default:
                return SHR_E_PARAM;
        }
    }

    CDPORT_MODE_REGr_MAC_PORT_MODEf_SET(cdport_mode, new_port_mode);
    ioerr += WRITE_CDPORT_MODE_REGr(pa, port, cdport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen3_cdmac_control_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t reset)
{
    int ioerr = 0;
    CDPORT_MAC_CONTROLr_t pmac_ctrl;

    CDPORT_MAC_CONTROLr_CLR(pmac_ctrl);
    ioerr += READ_CDPORT_MAC_CONTROLr(pa, port, &pmac_ctrl);

    if (reset) {
        CDPORT_MAC_CONTROLr_CDMAC_RESETf_SET(pmac_ctrl, 1);
    } else {
        CDPORT_MAC_CONTROLr_CDMAC_RESETf_SET(pmac_ctrl, 0);
    }
    ioerr += WRITE_CDPORT_MAC_CONTROLr(pa, port, pmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen3_cdmac_control_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t *reset)
{
    int ioerr = 0;
    CDPORT_MAC_CONTROLr_t pmac_ctrl;

    CDPORT_MAC_CONTROLr_CLR(pmac_ctrl);
    ioerr += READ_CDPORT_MAC_CONTROLr(pa, port, &pmac_ctrl);

    *reset = CDPORT_MAC_CONTROLr_CDMAC_RESETf_GET(pmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen3_tsc_ctrl_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         bcmpmac_tsc_ctrl_t *tsc_cfg)
{
    int ioerr = 0;
    CDPORT_XGXS0_CTRL_REGr_t tsc_ctrl;

    CDPORT_XGXS0_CTRL_REGr_CLR(tsc_ctrl);

    ioerr += READ_CDPORT_XGXS0_CTRL_REGr(pa, port, &tsc_ctrl);
    tsc_cfg->tsc_rstb = CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_GET(tsc_ctrl);
    tsc_cfg->tsc_pwrdwn = CDPORT_XGXS0_CTRL_REGr_TSC_PWRDWNf_GET(tsc_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen3_tsc_ctrl_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         int tsc_pwr_on)
{
    int ioerr = 0;
    CDPORT_XGXS0_CTRL_REGr_t tsc_ctrl;

    CDPORT_XGXS0_CTRL_REGr_CLR(tsc_ctrl);

    ioerr += READ_CDPORT_XGXS0_CTRL_REGr(pa, port, &tsc_ctrl);
    if (tsc_pwr_on) {
        CDPORT_XGXS0_CTRL_REGr_TSC_PWRDWNf_SET(tsc_ctrl, 1);
        CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_SET(tsc_ctrl, 0);
        ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(pa, port, tsc_ctrl);
        sal_usleep(1000);
        CDPORT_XGXS0_CTRL_REGr_TSC_PWRDWNf_SET(tsc_ctrl, 0);
        CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_SET(tsc_ctrl, 1);
        ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(pa, port, tsc_ctrl);
    } else {
        CDPORT_XGXS0_CTRL_REGr_TSC_PWRDWNf_SET(tsc_ctrl, 1);
        CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_SET(tsc_ctrl, 0);
        ioerr += WRITE_CDPORT_XGXS0_CTRL_REGr(pa, port, tsc_ctrl);
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen3_link_status_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                            uint32_t clear_on_read, uint32_t start_lane,
                            int* link)
{
    int ioerr = 0;
    CDPORT_XGXS0_STATUS_REGr_t ln_status;

    CDPORT_XGXS0_STATUS_REGr_CLR(ln_status);

    ioerr += READ_CDPORT_XGXS0_STATUS_REGr(pa, port, start_lane,
                                           &ln_status);

    *link = CDPORT_XGXS0_STATUS_REGr_LINK_STATUSf_GET(ln_status);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}
#endif /* BCMPMAC_CDPORT_GEN3_H */


