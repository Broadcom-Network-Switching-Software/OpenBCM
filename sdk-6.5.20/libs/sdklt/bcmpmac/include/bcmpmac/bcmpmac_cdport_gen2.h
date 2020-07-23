/*! \file bcmpmac_cdport_gen2.h
 *
 * CDPORT driver.
 *
 * A CDPORT contains 1 CDPORT and 2 CDMACs, and supports 8 ports at most.
 *
 * In this driver, we always use the port number 0~7 to access the CDPORT
 * and CDMAC per-port registers without considering which CDMAC should be
 * configured.
 *
 * The lower level register access driver would know how to determine which
 * CDMAC should be accessed, i.e. port 0~3 belongs the first CDMAC and port 4~7
 * belongs to the second CDMAC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_CDPORT_GEN2_H
#define BCMPMAC_CDPORT_GEN2_H

#include <shr/shr_util.h>
#include <sal/sal_sleep.h>

static inline int
cdport_gen2_core_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          bcmpmac_core_port_mode_t *mode)
{
    int rv, i, ioerr = 0;
    CDPORT_MODE_REGr_t cdport_mode;
    uint32_t is_400g, mode_mac0, mode_mac1;
    uint32_t lmap_400g[BCMPMAC_NUM_PORTS_PER_PM_MAX] = { 0xff, 0 };
    uint32_t lmap[BCMPMAC_NUM_PORTS_PER_PM_MAX] = { 0 };
    const bcmpmac_port_mode_map_t port_mode_map[] = {
        { { 0 }, CDPORT_PORT_MODE_QUAD },
        { { 0x1, 0x2, 0x4, 0x8, 0 }, CDPORT_PORT_MODE_QUAD },
        { { 0x3, 0, 0xc, 0 }, CDPORT_PORT_MODE_DUAL },
        { { 0x1, 0x2, 0xc, 0 }, CDPORT_PORT_MODE_TRI_012 },
        { { 0x3, 0x0, 0x4, 0x8, 0 }, CDPORT_PORT_MODE_TRI_023 },
        { { 0xf, 0 }, CDPORT_PORT_MODE_SINGLE }
    };

    CDPORT_MODE_REGr_CLR(cdport_mode);

    if (sal_memcmp(lmap_400g, mode->lane_map, sizeof(lmap_400g)) == 0) {
        /*
         * SINGLE_PORT_MODE_SPEED_400G.
         */
        is_400g = 1;
        mode_mac0 = CDPORT_PORT_MODE_SINGLE;
        mode_mac1 = CDPORT_PORT_MODE_SINGLE;
    } else {
        /*
         * More than one port.
         */
        is_400g = 0;

        /* Get the port mode of the first half part. */
        sal_memset(lmap, 0, sizeof(lmap));
        sal_memcpy(lmap, mode->lane_map, PORTS_PER_CDMAC * sizeof(uint32_t));
        rv = bcmpmac_port_mode_get(port_mode_map, COUNTOF(port_mode_map),
                                   lmap, &mode_mac0);
        if (SHR_FAILURE(rv)) {
            return rv;
        }

        /* Get the port mode of the second half part. */
        sal_memset(lmap, 0, sizeof(lmap));
        for (i = 0; i < PORTS_PER_CDMAC; i++) {
            /*
             * The second part controls the upper 4 lanes, so the lane map
             * will be something like {0x10, 0x20, 0x40, 0x80}.
             *
             * Shift 4 bit on the lane map value and check if the combination is
             * valid.
             */
            lmap[i] = mode->lane_map[PORTS_PER_CDMAC + i] >> 4;
        }
        rv = bcmpmac_port_mode_get(port_mode_map, COUNTOF(port_mode_map),
                                   lmap, &mode_mac1);
        if (SHR_FAILURE(rv)) {
            return rv;
        }
    }

    ioerr += READ_CDPORT_MODE_REGr(pa, port, &cdport_mode);
    CDPORT_MODE_REGr_SINGLE_PORT_MODE_SPEED_400Gf_SET(cdport_mode, is_400g);
    CDPORT_MODE_REGr_MAC0_PORT_MODEf_SET(cdport_mode, mode_mac0);
    CDPORT_MODE_REGr_MAC1_PORT_MODEf_SET(cdport_mode, mode_mac1);
    ioerr += WRITE_CDPORT_MODE_REGr(pa, port, cdport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen2_port_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          uint32_t flags, uint32_t lane_mask)
{
    int ioerr = 0, num_lanes = 0 , mac_id = 0;
    uint32_t is_400g = 0;
    uint32_t port_mode = CDPORT_PORT_MODE_QUAD;
    uint32_t new_port_mode = CDPORT_PORT_MODE_QUAD;
    CDPORT_MODE_REGr_t cdport_mode;

    CDPORT_MODE_REGr_CLR(cdport_mode);
    num_lanes = shr_util_popcount(lane_mask);
    ioerr += READ_CDPORT_MODE_REGr(pa, port, &cdport_mode);
    mac_id = (lane_mask > 0xf) ? 1 : 0;

    if (mac_id) {
        port_mode = CDPORT_MODE_REGr_MAC1_PORT_MODEf_GET(cdport_mode);
    } else {
        port_mode = CDPORT_MODE_REGr_MAC0_PORT_MODEf_GET(cdport_mode);
    }

    if (num_lanes == 8) {
        is_400g = 1;
        new_port_mode = CDPORT_PORT_MODE_SINGLE;
        CDPORT_MODE_REGr_SINGLE_PORT_MODE_SPEED_400Gf_SET(cdport_mode, is_400g);
        CDPORT_MODE_REGr_MAC0_PORT_MODEf_SET(cdport_mode, new_port_mode);
        CDPORT_MODE_REGr_MAC1_PORT_MODEf_SET(cdport_mode, new_port_mode);
    } else if (num_lanes == 4) {
        is_400g = 0;
        if ((lane_mask == 0xf) || (lane_mask == 0xf0)) {
            new_port_mode = CDPORT_PORT_MODE_SINGLE;
        }
        CDPORT_MODE_REGr_SINGLE_PORT_MODE_SPEED_400Gf_SET(cdport_mode, is_400g);
        if (mac_id) {
            CDPORT_MODE_REGr_MAC1_PORT_MODEf_SET(cdport_mode, new_port_mode);
        }else {
            CDPORT_MODE_REGr_MAC0_PORT_MODEf_SET(cdport_mode, new_port_mode);
        }
    } else if (num_lanes == 2) {
        is_400g = 0;
        switch (port_mode) {
            case CDPORT_PORT_MODE_QUAD:
                if ((lane_mask & 0x3) || (lane_mask & 0x30)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                }
                break;
            case CDPORT_PORT_MODE_TRI_012:
                if ((lane_mask & 0x3) || (lane_mask & 0x30)) {
                    new_port_mode = CDPORT_PORT_MODE_DUAL;
                } else {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                }
                break;
            case CDPORT_PORT_MODE_TRI_023:
                if ((lane_mask & 0x3) || (lane_mask & 0x30)) {
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
        CDPORT_MODE_REGr_SINGLE_PORT_MODE_SPEED_400Gf_SET(cdport_mode, is_400g);
        if (mac_id) {
            CDPORT_MODE_REGr_MAC1_PORT_MODEf_SET(cdport_mode, new_port_mode);
        }else {
            CDPORT_MODE_REGr_MAC0_PORT_MODEf_SET(cdport_mode, new_port_mode);
        }
    } else {
        /* num_lanes == 1 */
        is_400g = 0;
        switch (port_mode) {
            case CDPORT_PORT_MODE_QUAD:
                new_port_mode = CDPORT_PORT_MODE_QUAD;
                break;
            case CDPORT_PORT_MODE_TRI_012:
                if ((lane_mask == 0x1) || (lane_mask == 0x2)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else if ((lane_mask == 0x4) || (lane_mask == 0x8)){
                   new_port_mode = CDPORT_PORT_MODE_QUAD;
                } else if ((lane_mask == 0x10) || (lane_mask == 0x20)){
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else if ((lane_mask == 0x40) || (lane_mask == 0x80)){
                   new_port_mode = CDPORT_PORT_MODE_QUAD;
                }
                break;
            case CDPORT_PORT_MODE_TRI_023:
                if ((lane_mask == 0x1) || (lane_mask == 0x2)) {
                    new_port_mode = CDPORT_PORT_MODE_QUAD;
                } else if ((lane_mask == 0x4) || (lane_mask == 0x8)){
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                } else if ((lane_mask == 0x10) || (lane_mask == 0x20)){
                    new_port_mode = CDPORT_PORT_MODE_QUAD;
                } else if ((lane_mask == 0x40) || (lane_mask == 0x80)){
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_DUAL:
                if ((lane_mask == 0x1) || (lane_mask == 0x2)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else if ((lane_mask == 0x4) || (lane_mask == 0x8)) {
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                } else if ((lane_mask == 0x10) || (lane_mask == 0x20)){
                    new_port_mode = CDPORT_PORT_MODE_TRI_012;
                } else if ((lane_mask == 0x40) || (lane_mask == 0x80)){
                    new_port_mode = CDPORT_PORT_MODE_TRI_023;
                }
                break;
            case CDPORT_PORT_MODE_SINGLE:
                new_port_mode = CDPORT_PORT_MODE_QUAD;
                break;
           default:
                return SHR_E_PARAM;
        }
        CDPORT_MODE_REGr_SINGLE_PORT_MODE_SPEED_400Gf_SET(cdport_mode, is_400g);
        if (mac_id) {
            CDPORT_MODE_REGr_MAC1_PORT_MODEf_SET(cdport_mode, new_port_mode);
        } else {
            CDPORT_MODE_REGr_MAC0_PORT_MODEf_SET(cdport_mode, new_port_mode);
        }
    }

    ioerr += WRITE_CDPORT_MODE_REGr(pa, port, cdport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen2_link_status_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
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

static inline int
cdport_gen2_tsc_ctrl_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         bcmpmac_tsc_ctrl_t *tsc_cfg)
{
    int ioerr = 0;
    CDPORT_XGXS0_CTRL_REGr_t tsc_ctrl;

    CDPORT_XGXS0_CTRL_REGr_CLR(tsc_ctrl);

    ioerr += READ_CDPORT_XGXS0_CTRL_REGr(pa, port, &tsc_ctrl);
    tsc_cfg->tsc_rstb = CDPORT_XGXS0_CTRL_REGr_TSC_RSTBf_GET(tsc_ctrl);
    tsc_cfg->tsc_pwrdwn = CDPORT_XGXS0_CTRL_REGr_TSC_PWRDWNf_GET(tsc_ctrl);
    tsc_cfg->tsc_clk_sel = CDPORT_XGXS0_CTRL_REGr_TSC_CLK_SELf_GET(tsc_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_gen2_tsc_ctrl_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
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

#endif /* BCMPMAC_CDPORT_GEN2_H */
