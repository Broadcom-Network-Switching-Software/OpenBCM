/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */

#include <merlin16_pcieg3_internal.h>
#include <common/srds_api_err_code.h>
#include <common/srds_api_types.h>
#include <merlin16_pcieg3_diag.h>
#include <merlin16_pcieg3_debug_functions.h>
#include <merlin16_pcieg3_dependencies.h>
#include <merlin16_pcieg3_config.h>
#include <pcig3_phy_acc.h>

/******************************************************************************
 Register read/write
 */
int pcie_phy_diag_reg_read(srds_access_t *sa, uint16_t address, uint16_t *pdata)
{
    int rv;

    rv = merlin16_pcieg3_pmd_rdt_reg(sa, address, pdata);
    return (rv == ERR_CODE_NONE) ? SRDS_E_NONE : SRDS_E_FAIL;
}

int pcie_phy_diag_reg_write(srds_access_t *sa, uint16_t address, uint16_t data)
{
    int rv;

    rv = merlin16_pcieg3_pmd_wr_reg(sa, address, data);
    return (rv == ERR_CODE_NONE) ? SRDS_E_NONE : SRDS_E_FAIL;
}

int pcie_phy_diag_reg_mwr(srds_access_t *sa, uint16_t address, uint16_t mask, uint8_t lsb, uint16_t val)
{
    int rv;

    rv = merlin16_pcieg3_pmd_mwr_reg(sa, address, mask, lsb, val);
    return (rv == ERR_CODE_NONE) ? SRDS_E_NONE : SRDS_E_FAIL;
}

int pcie_phy_diag_pram_read(srds_access_t *sa, uint32_t address, uint32_t size)
{
    uint16_t addr_l, addr_h;
    uint16_t data_l;
    int i, rv = SRDS_E_NONE;

    addr_l = (uint16_t) address & 0xffff;
    addr_h = (uint16_t) ((address >> 16) & 0xffff);

    /* Enable pram if */
    cli_out("\n_pcie_serdes_pram_read: Enable pram if\n");
    rv= pcie_phy_diag_reg_mwr(sa, 0xd202, 1 << 13, 13, 1);
    if (rv != SRDS_E_NONE) goto done;

    /* Set 16-bit read size */
    rv= pcie_phy_diag_reg_mwr(sa, 0xd202, 3 << 4, 4, 1);
    if (rv != SRDS_E_NONE) goto done;

    rv = pcie_phy_diag_reg_write(sa, 0xd208, addr_l); /* LSW ADDR */
    if (rv != SRDS_E_NONE) goto done;

    rv = pcie_phy_diag_reg_write(sa, 0xd209, addr_h); /* MSW ADDR */
    if (rv != SRDS_E_NONE) goto done;

    for (i = 0; i < size; i++) {
        rv = pcie_phy_diag_reg_read(sa, 0xd20a, &data_l); /* LSW ADDR */
        if (rv != SRDS_E_NONE) goto done;
        cli_out("address: 0x%x,  data = 0x%x\n", address, data_l);
        address += 2;
    }

done:
    if (rv != SRDS_E_NONE) {
        cli_out("\npcie_diag_pram_read: phy access failed: Error 0x%x \n", rv);
    }

    return rv;
}

/******************************************************************************
DSC
 */
int pcie_phy_diag_dsc(srds_access_t *sa, int array_size)
{
    int i, rv = ERR_CODE_NONE;
    uint32_t lane, lane_index;

    if (sa == NULL || array_size == 0) {
        return SRDS_E_PARAM;
    }

    for (i = 0 ; i < array_size ; i++) {
        lane = sa[i].lane_mask;
        rv = merlin16_pcieg3_display_core_state(&sa[i]);
        if (rv != ERR_CODE_NONE) {
           cli_out("merlin16_pcieg3_display_core_state() failed: 0x%x\n", rv);
           return SRDS_E_FAIL;
        }

        rv = merlin16_pcieg3_display_lane_state_hdr();
        if (rv != ERR_CODE_NONE) {
           cli_out("merlin16_pcieg3_display_lane_state_hdr() failed: 0x%x\n", rv);
           return SRDS_E_FAIL;
        }

        for (lane_index = 0 ; lane_index < PCIE_MAX_LANES_PER_CORE; lane_index++) {
            sa[i].lane_mask = lane & (1<<lane_index);
            /*if lane is not selected pass*/
            if(sa[i].lane_mask == 0){
                continue;
            }
            /*collect info*/
            rv = merlin16_pcieg3_display_lane_state(&sa[i]);
            if (rv != ERR_CODE_NONE) {
                cli_out("merlin16_pcieg3_display_lane_state() failed: 0x%x\n", rv);
                break;
            }
        }
        sa[i].lane_mask = lane;
        if (rv != ERR_CODE_NONE) {
            break;
        }
    }

    return (rv == ERR_CODE_NONE) ? SRDS_E_NONE : SRDS_E_FAIL;
}

/******************************************************************************
State
 */
int pcie_phy_diag_state(srds_access_t *sa, int array_size)
{
    int i, rv = ERR_CODE_NONE;
    uint32_t lane, lane_index;

    if (sa == NULL || array_size == 0) {
        return SRDS_E_PARAM;
    }

    for (i = 0 ; i < array_size ; i++) {
        lane = sa[i].lane_mask;
        merlin16_pcieg3_init_merlin16_pcieg3_info(&sa[i]);
        rv = merlin16_pcieg3_display_core_state_hdr();
        if (rv != ERR_CODE_NONE) {
           cli_out("merlin16_pcieg3_display_core_state_hdr() failed: 0x%x\n", rv);
           return SRDS_E_FAIL;
        }
        rv = merlin16_pcieg3_display_core_state_line(&sa[i]);
        if (rv != ERR_CODE_NONE) {
           cli_out("erlin16_pcieg3_display_core_state_line() failed: 0x%x\n", rv);
           return SRDS_E_FAIL;
        }
        rv = merlin16_pcieg3_display_core_state(&sa[i]);
        if (rv != ERR_CODE_NONE) {
           cli_out("merlin16_pcieg3_display_core_state() failed: 0x%x\n", rv);
           return SRDS_E_FAIL;
        }
        rv = merlin16_pcieg3_display_lane_state_hdr();
        if (rv != ERR_CODE_NONE) {
           cli_out("merlin16_pcieg3_display_lane_state_hdr() failed: 0x%x\n", rv);
           return SRDS_E_FAIL;
        }

        for (lane_index = 0 ; lane_index < PCIE_MAX_LANES_PER_CORE; lane_index++) {
            sa[i].lane_mask = lane & (1<<lane_index);
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

    return (rv == ERR_CODE_NONE) ? SRDS_E_NONE : SRDS_E_FAIL;
}

/******************************************************************************
Eyescan
 */
int pcie_phy_diag_eyescan(srds_access_t *sa, int array_size)
{
    int i, rv = ERR_CODE_NONE;
    uint32_t lane, lane_index;

    if (sa == NULL || array_size == 0) {
        return SRDS_E_PARAM;
    }

    for (i = 0 ; i < array_size ; i++) {
        lane = sa[i].lane_mask;
        cli_out("pcie_phy_diag_eyescan: lane_mask: 0x%x\n", lane);
        merlin16_pcieg3_init_merlin16_pcieg3_info(&sa[i]);

        for (lane_index = 0 ; lane_index < PCIE_MAX_LANES_PER_CORE; lane_index++) {
            sa[i].lane_mask = lane & (1<<lane_index);
            sa[i].core = i;
            /*if lane is not selected pass*/
            if (sa[i].lane_mask == 0) {
                continue;
            }
            /* Display eyescan */
            cli_out("\nEyescan for Core: %d, lane: %d\n", i, lane_index);
            cli_out("==============================\n");
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

    return (rv == ERR_CODE_NONE) ? SRDS_E_NONE : SRDS_E_FAIL;
}
