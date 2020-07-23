/*! \file pciephy_merlin16_driver.c
 *
 * PCIe driver for merlin 16nm.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <pciephy/pciephy.h>
#include <pciephy/pciephy_driver.h>

#include "merlin16/tier1/merlin16_pcieg3_dependencies.h"
#include "merlin16/tier1/merlin16_pcieg3_internal.h"
#include "merlin16/tier1/merlin16_pcieg3_diag.h"
#include "merlin16/tier1/merlin16_pcieg3_debug_functions.h"
#include "merlin16/tier1/merlin16_pcieg3_config.h"

static int
merlin16_reg_read(pciephy_access_t *sa, uint16_t address, uint16_t *pdata)
{
    int rv;

    rv = merlin16_pcieg3_pmd_rdt_reg(sa, address, pdata);

    return (rv == ERR_CODE_NONE) ? PCIEPHY_E_NONE : PCIEPHY_E_FAIL;
}

static int
merlin16_reg_write(pciephy_access_t *sa, uint16_t address, uint16_t data)
{
    int rv;

    rv = merlin16_pcieg3_pmd_wr_reg(sa, address, data);

    return (rv == ERR_CODE_NONE) ? PCIEPHY_E_NONE : PCIEPHY_E_FAIL;
}

static int
merlin16_reg_mwr(pciephy_access_t *sa, uint16_t address, uint16_t mask,
                 uint8_t lsb, uint16_t val)
{
    int rv;

    rv = merlin16_pcieg3_pmd_mwr_reg(sa, address, mask, lsb, val);

    return (rv == ERR_CODE_NONE) ? PCIEPHY_E_NONE : PCIEPHY_E_FAIL;
}

static int
merlin16_pram_read(pciephy_access_t *sa, uint32_t address, uint32_t size)
{
    uint16_t addr_l, addr_h;
    uint16_t data_l;
    uint32_t i;
    int rv = PCIEPHY_E_NONE;

    addr_l = (uint16_t) address & 0xffff;
    addr_h = (uint16_t) ((address >> 16) & 0xffff);

    /* Enable pram if */
    PCIEPHY_DIAG_OUT("\nEnable pram interface\n");
    rv= merlin16_reg_mwr(sa, 0xd202, 1 << 13, 13, 1);
    if (rv != PCIEPHY_E_NONE) {
        PCIEPHY_DIAG_OUT("pram read failed: Error 0x%x \n", rv);
        return rv;
    }

    /* Set 16-bit read size */
    rv= merlin16_reg_mwr(sa, 0xd202, 3 << 4, 4, 1);
    if (rv != PCIEPHY_E_NONE) {
        PCIEPHY_DIAG_OUT("pram read failed: Error 0x%x \n", rv);
        return rv;
    }

    rv = merlin16_reg_write(sa, 0xd208, addr_l); /* LSW ADDR */
    if (rv != PCIEPHY_E_NONE) {
        PCIEPHY_DIAG_OUT("pram read failed: Error 0x%x \n", rv);
        return rv;
    }

    rv = merlin16_reg_write(sa, 0xd209, addr_h); /* MSW ADDR */
    if (rv != PCIEPHY_E_NONE) {
        PCIEPHY_DIAG_OUT("pram read failed: Error 0x%x \n", rv);
        return rv;
    }

    for (i = 0; i < size; i++) {
        rv = merlin16_reg_read(sa, 0xd20a, &data_l); /* LSW ADDR */
        if (rv != PCIEPHY_E_NONE) {
            PCIEPHY_DIAG_OUT("pram read failed: Error 0x%x \n", rv);
            return rv;
        }
        PCIEPHY_DIAG_OUT("address: 0x%x,  data = 0x%x\n", address, data_l);
        address += 2;
    }

    return rv;
}

static int
merlin16_dsc(pciephy_access_t *sa, int array_size)
{
    int i, rv = ERR_CODE_NONE;
    uint32_t lane, lidx;

    if (sa == NULL || array_size == 0) {
        return PCIEPHY_E_PARAM;
    }

    for (i = 0 ; i < array_size ; i++) {
        lane = sa[i].lane_mask;
        rv = merlin16_pcieg3_display_core_state(&sa[i]);
        if (rv != ERR_CODE_NONE) {
           PCIEPHY_DIAG_OUT("Failed to get core state: 0x%x\n", rv);
           return PCIEPHY_E_FAIL;
        }

        rv = merlin16_pcieg3_display_lane_state_hdr();
        if (rv != ERR_CODE_NONE) {
           PCIEPHY_DIAG_OUT("Failed to get lane state header: 0x%x\n", rv);
           return PCIEPHY_E_FAIL;
        }

        for (lidx = 0 ; lidx < PCIEPHY_MAX_LANES_PER_CORE; lidx++) {
            sa[i].lane_mask = lane & (1<<lidx);
            /*if lane is not selected pass*/
            if(sa[i].lane_mask == 0){
                continue;
            }
            /*collect info*/
            rv = merlin16_pcieg3_display_lane_state(&sa[i]);
            if (rv != ERR_CODE_NONE) {
                PCIEPHY_DIAG_OUT("Failed to get lane state: 0x%x\n", rv);
                break;
            }
        }
        sa[i].lane_mask = lane;
        if (rv != ERR_CODE_NONE) {
            break;
        }
    }

    return (rv == ERR_CODE_NONE) ? PCIEPHY_E_NONE : PCIEPHY_E_FAIL;
}

static int
merlin16_state(pciephy_access_t *sa, int array_size)
{
    int i, rv = ERR_CODE_NONE;
    uint32_t lane, lidx;

    if (sa == NULL || array_size == 0) {
        return PCIEPHY_E_PARAM;
    }

    for (i = 0 ; i < array_size ; i++) {
        lane = sa[i].lane_mask;
        merlin16_pcieg3_init_merlin16_pcieg3_info(&sa[i]);
        rv = merlin16_pcieg3_display_core_state_hdr();
        if (rv != ERR_CODE_NONE) {
           PCIEPHY_DIAG_OUT("Failed to get core state header: 0x%x\n", rv);
           return PCIEPHY_E_FAIL;
        }
        rv = merlin16_pcieg3_display_core_state_line(&sa[i]);
        if (rv != ERR_CODE_NONE) {
           PCIEPHY_DIAG_OUT("Failed to get core state line: 0x%x\n", rv);
           return PCIEPHY_E_FAIL;
        }
        rv = merlin16_pcieg3_display_core_state(&sa[i]);
        if (rv != ERR_CODE_NONE) {
           PCIEPHY_DIAG_OUT("Failed to get core state: 0x%x\n", rv);
           return PCIEPHY_E_FAIL;
        }
        rv = merlin16_pcieg3_display_lane_state_hdr();
        if (rv != ERR_CODE_NONE) {
           PCIEPHY_DIAG_OUT("Failed to get lane state: 0x%x\n", rv);
           return PCIEPHY_E_FAIL;
        }

        for (lidx = 0 ; lidx < PCIEPHY_MAX_LANES_PER_CORE; lidx++) {
            sa[i].lane_mask = lane & (1<<lidx);
            /*if lane is not selected pass*/
            if (sa[i].lane_mask == 0) {
                continue;
            }
            /*collect info*/
            rv = merlin16_pcieg3_display_lane_state(&sa[i]);
            if (rv != ERR_CODE_NONE) {
                break;
            }
            rv = merlin16_pcieg3_read_event_log(&sa[i]);
            if (rv != ERR_CODE_NONE) {
                break;
            }
        }
        sa[i].lane_mask = lane;
        if (rv != ERR_CODE_NONE) {
            break;
        }
    }

    return (rv == ERR_CODE_NONE) ? PCIEPHY_E_NONE : PCIEPHY_E_FAIL;
}

static int
merlin16_eyescan(pciephy_access_t *sa, int array_size)
{
    int i, rv = ERR_CODE_NONE;
    uint32_t lane, lidx;

    if (sa == NULL || array_size == 0) {
        return PCIEPHY_E_PARAM;
    }

    for (i = 0 ; i < array_size ; i++) {
        lane = sa[i].lane_mask;
        PCIEPHY_DIAG_OUT("Eyescan: lane_mask: 0x%x\n", lane);
        merlin16_pcieg3_init_merlin16_pcieg3_info(&sa[i]);

        for (lidx = 0 ; lidx < PCIEPHY_MAX_LANES_PER_CORE; lidx++) {
            sa[i].lane_mask = lane & (1<<lidx);
            sa[i].core = i;
            /*if lane is not selected pass*/
            if (sa[i].lane_mask == 0) {
                continue;
            }
            /* Display eyescan */
            PCIEPHY_DIAG_OUT("\nEyescan for Core: %d, lane: %d\n", i, lidx);
            PCIEPHY_DIAG_OUT("==============================\n");
            rv = merlin16_pcieg3_display_eye_scan(&sa[i]);
            if (rv != ERR_CODE_NONE) {
                break;
            }
        }
        sa[i].lane_mask = lane;
        if(rv != ERR_CODE_NONE) {
            break;
        }
    }

    return (rv == ERR_CODE_NONE) ? PCIEPHY_E_NONE : PCIEPHY_E_FAIL;
}

pciephy_driver_t pciephy_merlin16_driver = {
    .read = merlin16_reg_read,
    .write = merlin16_reg_write,
    .mwr = merlin16_reg_mwr,
    .pram_read = merlin16_pram_read,
    .dsc = merlin16_dsc,
    .state = merlin16_state,
    .eyescan = merlin16_eyescan,
};
