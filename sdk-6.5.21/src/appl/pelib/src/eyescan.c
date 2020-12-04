/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       eyescan.c
 */

#include "types.h"
#include "error.h"
#include "bcm_utils.h"
#include "eyescan.h"
#include "tsc_functions.h"
#include "bcm_xmod_pe_api.h"

typedef uint8 float8; /* float represented in 8 bits.. as used by the pmd */

/* convert float8 to uint32 */
static uint32 _float8_to_int32 (phy_ctrl_t *pa, float8 input)
{
    uint32 x;

    if (input == 0) return 0;
    x = (input >> 5) + 8;
    if ((input & 0x1F) < 3)
        return x >> (3 - (input & 0x1f));
    return x << ((input & 0x1F) - 3);
}


/* poll for microcontroller to populate the dsc_data register */
int bcm_pe_poll_diag_eye_data (phy_ctrl_t *pa, uint32 *data, uint16 *status, uint32 timeout_ms)
{
    uint8 loop;
    uint16 dscdata;

    if (!data || !status)
        return SOC_E_PARAM;

    for (loop = 0; loop < 100; loop++) {
        ESTM(*status = rdv_usr_diag_status());
        if (((*status & 0x00FF) > 2) || ((*status & 0x8000) > 0)) {
            EFUN(bcm_pe_pmd_uc_cmd (pa,  CMD_READ_DIAG_DATA_WORD, 0, 200));
            ESTM(dscdata = rd_uc_dsc_data());
            data[0] = _float8_to_int32 (pa, (float8)(dscdata >> 8));
            data[1] = _float8_to_int32 (pa, (float8)(dscdata & 0x00FF));

            return SOC_E_NONE;
        }

        if (loop > 10)
            bcm_udelay(10 * timeout_ms);

    }
    return SOC_E_TIMEOUT;
}

int bcm_meas_eye_scan_start (phy_ctrl_t *pa, uint8 direction)
{
    uint8 lock;

    ESTM(lock = rd_pmd_rx_lock());
    if (lock == 0) {
        BCM_LOG_ERR("Error: No PMD_RX_LOCK on lane requesting 2D eye scan\n");
        return SOC_E_FAIL;
    }

    if (direction == EYE_SCAN_VERTICAL)
        EFUN(bcm_pe_pmd_uc_diag_cmd (pa, CMD_UC_DIAG_START_VSCAN_EYE, 200));
    else
        EFUN(bcm_pe_pmd_uc_diag_cmd (pa, CMD_UC_DIAG_START_HSCAN_EYE, 200));

    return SOC_E_NONE;
}

int bcm_display_eye_scan_header (phy_ctrl_t *pa, int8 i)
{
    int8 x;

    BCM_LOG_CLI("\n");
    BCM_LOG_CLI(" Each character N represents approximate error rate 1e-N at that location\n");
    for (x = 1; x <= i; x++)
        BCM_LOG_CLI("  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30");
    BCM_LOG_CLI("\n");
    for (x = 1; x <= i; x++)
        BCM_LOG_CLI("         : -|----|----|----|----|----|----|----|----|----|----|----|----|-");
    BCM_LOG_CLI("\n");
    return SOC_E_NONE;
}

int bcm_read_eye_scan_stripe (phy_ctrl_t *pa, uint32 *buffer, uint16 *status)
{
    uint32 val[2] = { 0, 0 };
    uint16 sts = 0;
    int8 i;

    if (!buffer || !status)
        return SOC_E_PARAM;
    *status = 0;
    for (i = 0; i < 32; i++) {
        {   int err_code = bcm_pe_poll_diag_eye_data (pa, &val[0], &sts, 200);
            *status |= sts & 0xF000;
            if (err_code) {
                BCM_LOG_ERR("bcm_pe_poll_diag_eye_data() returned error: %d\n", err_code);
                return err_code;
            }
        }

        buffer[i * 2]     = val[0];
        buffer[(i * 2) + 1] = val[1];
    }
    *status |= sts & 0x00FF;
    return SOC_E_NONE;
}

int bcm_display_eye_scan_stripe (phy_ctrl_t *pa, int8 y, uint32 *buffer)
{
    const uint32 limits[7] = { 1835008, 183501, 18350, 1835, 184, 18, 2 };

    int8 x, i;
    int16 level;

    level = _LADDER_SETTING_TO_MV(y);

    if (!buffer)
        return SOC_E_PARAM;

    BCM_LOG_CLI("%6dmV : ", level);

    for (x = -31; x < 32; x++) {
        for (i = 0; i < 7; i++)
            if (buffer[x + 31] >= limits[i]) {
                BCM_LOG_CLI("%c", '0' + i + 1);
                break;
            }
        if (i == 7) {
            if ((x % 5) == 0 && (y % 5) == 0) BCM_LOG_CLI("+"); else if ((x % 5) != 0 && (y % 5) == 0) BCM_LOG_CLI("-"); else if ((x % 5) == 0 && (y % 5) != 0) BCM_LOG_CLI(":"); else BCM_LOG_CLI(" ");
        }
    }
    return SOC_E_NONE;
}

int bcm_display_eye_scan_footer (phy_ctrl_t *pa, int8 i)
{
    int8 x;

    for (x = 1; x <= i; x++)
        BCM_LOG_CLI("         : -|----|----|----|----|----|----|----|----|----|----|----|----|-");
    BCM_LOG_CLI("\n");
    for (x = 1; x <= i; x++)
        BCM_LOG_CLI("  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30");
    BCM_LOG_CLI("\n");
    return SOC_E_NONE;
}

int bcm_meas_eye_scan_done (phy_ctrl_t *pa)
{
    EFUN(bcm_pe_pmd_uc_diag_cmd (pa, CMD_UC_DIAG_DISABLE, 200));
    return SOC_E_NONE;
}

int bcm_do_eyescan(phy_ctrl_t *pc, int sys_port)
{
    int ii;
    uint32 stripe[64];
    uint16 status;

    if (!pc->module)
        pc->lane_mask = bcm_port_lane_mask(pc->unit, pc->module, sys_port);

    pc->tsc_sys_port=sys_port;

    SOC_IF_ERROR_RETURN(bcm_meas_eye_scan_start(pc, 0));
    bcm_udelay(100000);

    for (ii = 62; ii >= -62; ii=ii-2) {
        if (ii == 62) {
            BCM_LOG_CLI("\n\n\n");
            BCM_LOG_CLI("    +--------------------------------------------------------------------+\n");
            BCM_LOG_CLI("     EYESCAN %s port: %d; lane_mask: 0x%x\n", sys_port?"System":"Line", pc->phy_id, pc->lane_mask);
            BCM_LOG_CLI("    +--------------------------------------------------------------------+\n");
            SOC_IF_ERROR_RETURN(bcm_display_eye_scan_header(pc, 1));
        }

        SOC_IF_ERROR_RETURN(bcm_read_eye_scan_stripe(pc, stripe, &status));
        SOC_IF_ERROR_RETURN(bcm_display_eye_scan_stripe(pc, ii, &stripe[0]));
        BCM_LOG_CLI("\n");
    }
    SOC_IF_ERROR_RETURN(bcm_display_eye_scan_footer(pc, 1));
    BCM_LOG_CLI("\n");

    SOC_IF_ERROR_RETURN(bcm_meas_eye_scan_done(pc));

    return SOC_E_NONE;
}

